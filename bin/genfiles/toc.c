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
struct _RegionPage; // abstract -- defined in runtime_memory.c

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
#define _check_null(ptr) (ptr ? : (void*)_throw_null())
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
#endif

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
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
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
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */

extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

/* FIX?  Not sure if we want to pass filename and lineno in here... */
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())

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
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 245
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 964
void*Cyc_Absyn_exn_typ();
# 973
extern void*Cyc_Absyn_bounds_one;
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 992
void*Cyc_Absyn_void_star_typ();
# 994
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1095
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1183
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 65
int Cyc_Tcutil_is_arithmetic_type(void*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
# 88
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
# 90
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 150
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 241 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 319
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 358
int Cyc_Tcutil_is_array(void*t);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_Hashtable_Table;
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
({struct _dyneither_ptr _tmp7AA=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7A9=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp7A9,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7AA,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp7AC=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7AB=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp7AB,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7AC,ap);});}
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
({struct Cyc_List_List**_tmp7B8=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7B8;}),({
struct Cyc_List_List**_tmp7B7=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7B7;}),({
struct Cyc_Dict_Dict*_tmp7B6=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7B5=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7B5;});_tmpC;});_tmp11->aggrs_so_far=_tmp7B6;}),({
struct Cyc_List_List**_tmp7B4=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7B4;}),({
struct Cyc_Set_Set**_tmp7B3=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7B2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7B2;});_tmpE;});_tmp11->datatypes_so_far=_tmp7B3;}),({
struct Cyc_Dict_Dict*_tmp7B1=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7B0=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7B0;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7B1;}),({
struct Cyc_Dict_Dict*_tmp7AF=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7AE=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7AE;});_tmp10;});_tmp11->qvar_tags=_tmp7AF;}),({
struct Cyc_Xarray_Xarray*_tmp7AD=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7AD;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7B9=({struct Cyc_Toc_TocStateWrap _tmp774;_tmp774.dyn=_tmp1C,_tmp774.state=_tmp1B;_tmp774;});*ts=_tmp7B9;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp775;_tmp775.f1=_tmp1B,_tmp775.f2=arg;_tmp775;});
({void*_tmp7BA=f(h,& _tmp1A);res=_tmp7BA;});}
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
struct _tuple17 env=({struct _tuple17 _tmp776;_tmp776.f1=q,_tmp776.f2=type_maker;_tmp776;});
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
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11U,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 301
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 303
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 305
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 309
static struct _tuple20*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_List_List*_tmp7BC=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));({void*_tmp7BB=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=1U,_tmp6D->f1=name;_tmp6D;});_tmp6E->hd=_tmp7BB;}),_tmp6E->tl=0;_tmp6E;});_tmp6F->f1=_tmp7BC;}),_tmp6F->f2=e;_tmp6F;});}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp70=e->r;void*_tmp71=_tmp70;struct Cyc_Absyn_Exp*_tmp72;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp72=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp72);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=14U,_tmp73->f1=t,_tmp73->f2=e,_tmp73->f3=0,_tmp73->f4=Cyc_Absyn_No_coercion;_tmp73;});}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=20U,_tmp74->f1=e;_tmp74;});}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=23U,_tmp75->f1=e1,_tmp75->f2=e2;_tmp75;});}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=37U,_tmp76->f1=s;_tmp76;});}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=17U,_tmp77->f1=t;_tmp77;});}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=10U,_tmp78->f1=e,_tmp78->f2=es,_tmp78->f3=0,_tmp78->f4=1;_tmp78;});}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=1U,_tmp79->f1=e;_tmp79;});}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=2U,_tmp7A->f1=s1,_tmp7A->f2=s2;_tmp7A;});}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=6U,_tmp7B->f1=e1,_tmp7B->f2=e2,_tmp7B->f3=e3;_tmp7B;});}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=21U,_tmp7C->f1=e,_tmp7C->f2=n,_tmp7C->f3=0,_tmp7C->f4=0;_tmp7C;});}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=22U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=36U,_tmp7E->f1=tdopt,_tmp7E->f2=ds;_tmp7E;});}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=8U,_tmp7F->f1=v;_tmp7F;});}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp80=e->r;void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp82;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->tag == 20U){_LL1: _tmp82=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp82,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 385
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 394
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 403
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 412
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp83=Cyc_Tcutil_compress(t);void*_tmp84=_tmp83;enum Cyc_Absyn_Size_of _tmp8B;switch(*((int*)_tmp84)){case 6U: _LL1: _tmp8B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp84)->f2;_LL2:
# 427
{enum Cyc_Absyn_Size_of _tmp85=_tmp8B;switch(_tmp85){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp86=0U;({struct _dyneither_ptr _tmp7BD=({const char*_tmp87="impossible IntType (not char, short or int)";_tag_dyneither(_tmp87,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7BD,_tag_dyneither(_tmp86,sizeof(void*),0U));});});}_LLD:;}
# 433
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp84)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp777;_tmp777.tag=0U,({struct _dyneither_ptr _tmp7BE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp777.f1=_tmp7BE;});_tmp777;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct _dyneither_ptr _tmp7BF=({const char*_tmp89="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp89,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7BF,_tag_dyneither(_tmp88,sizeof(void*),1U));});});}_LL0:;}
# 440
return function;}
# 442
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8D=_tmp8C;void*_tmp90;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 5U){_LL1: _tmp90=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp90);}else{_LL3: _LL4:
({void*_tmp8E=0U;({struct _dyneither_ptr _tmp7C0=({const char*_tmp8F="impossible type (not pointer)";_tag_dyneither(_tmp8F,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C0,_tag_dyneither(_tmp8E,sizeof(void*),0U));});});}_LL0:;}
# 451
static int Cyc_Toc_is_array_type(void*t){
void*_tmp91=Cyc_Tcutil_compress(t);void*_tmp92=_tmp91;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 459
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp93=e->r;void*_tmp94=_tmp93;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;long long _tmp9A;int _tmp99;short _tmp98;struct _dyneither_ptr _tmp97;char _tmp96;switch(*((int*)_tmp94)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Null_c).tag){case 2U: _LL1: _tmp96=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Char_c).val).f2;_LL2:
 return _tmp96 == '\000';case 3U: _LL3: _tmp97=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Wchar_c).val;_LL4: {
# 463
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)_tmp97);
int i=0;
if(_tmp95 >= 2  && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp97.curr)[1]== 'x'  && _tmp95 >= 3) && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp95;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 473
return 0;}}case 4U: _LL5: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Short_c).val).f2;_LL6:
 return _tmp98 == 0;case 5U: _LL7: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Int_c).val).f2;_LL8:
 return _tmp99 == 0;case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9A == 0;case 1U: _LLD: _LLE:
# 478
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 477
 goto _LLE;case 14U: _LLF: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL10:
# 479
 return Cyc_Toc_is_zero(_tmp9B);case 24U: _LL11: _tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9C);case 26U: _LL13: _tmp9D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL14:
 _tmp9E=_tmp9D;goto _LL16;case 29U: _LL15: _tmp9E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp94)->f3;_LL16:
 _tmp9F=_tmp9E;goto _LL18;case 25U: _LL17: _tmp9F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL18:
 _tmpA0=_tmp9F;goto _LL1A;case 36U: _LL19: _tmpA0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL1A:
# 485
 for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmpA0->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 494
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA1=x;union Cyc_Absyn_Nmspace _tmpB3;struct _dyneither_ptr*_tmpB2;_LL1: _tmpB3=_tmpA1->f1;_tmpB2=_tmpA1->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA2=_tmpB3;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;switch((_tmpA2.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAF=0;goto _LL7;case 1U: _LL6: _tmpAF=(_tmpA2.Rel_n).val;_LL7:
 _tmpB0=_tmpAF;goto _LL9;case 2U: _LL8: _tmpB0=(_tmpA2.Abs_n).val;_LL9:
 _tmpB1=_tmpB0;goto _LLB;default: _LLA: _tmpB1=(_tmpA2.C_n).val;_LLB:
# 503
 if(_tmpB1 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp779;_tmp779.tag=0U,_tmp779.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp779;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp778;_tmp778.tag=0U,_tmp778.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp778;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({struct _dyneither_ptr _tmp7C1=({const char*_tmpA4="%s_%s_struct";_tag_dyneither(_tmpA4,sizeof(char),13U);});Cyc_aprintf(_tmp7C1,_tag_dyneither(_tmpA3,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA7=_new_region("r");struct _RegionHandle*r=& _tmpA7;_push_region(r);
{struct _dyneither_ptr _tmpAE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp77C;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0U,({struct _dyneither_ptr _tmp7C4=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7C3=r;struct Cyc_List_List*_tmp7C2=_tmpB1;Cyc_rstr_sepstr(_tmp7C3,_tmp7C2,({const char*_tmpAD="_";_tag_dyneither(_tmpAD,sizeof(char),2U);}));}));_tmp77B.f1=_tmp7C4;});_tmp77B;});struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp77A;_tmp77A.tag=0U,_tmp77A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp77A;});void*_tmpA8[3U];_tmpA8[0]=& _tmpAA,_tmpA8[1]=& _tmpAB,_tmpA8[2]=& _tmpAC;({struct _dyneither_ptr _tmp7C5=({const char*_tmpA9="%s_%s_%s_struct";_tag_dyneither(_tmpA9,sizeof(char),16U);});Cyc_aprintf(_tmp7C5,_tag_dyneither(_tmpA8,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAE;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 516
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 519
struct _tuple21 _tmpB4=*env;struct _tuple21 _tmpB5=_tmpB4;struct Cyc_Dict_Dict*_tmpC8;struct _tuple15*_tmpC7;_LL1: _tmpC8=(_tmpB5.f1)->qvar_tags;_tmpC7=_tmpB5.f2;_LL2:;{
struct _tuple15 _tmpB6=*_tmpC7;struct _tuple15 _tmpB7=_tmpB6;struct _tuple1*_tmpC6;struct _tuple1*_tmpC5;_LL4: _tmpC6=_tmpB7.f1;_tmpC5=_tmpB7.f2;_LL5:;{
struct _handler_cons _tmpB8;_push_handler(& _tmpB8);{int _tmpBA=0;if(setjmp(_tmpB8.handler))_tmpBA=1;if(!_tmpBA){{struct _tuple1*_tmpBB=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpC8,Cyc_Toc_qvar_tag_cmp,_tmpC7);_npop_handler(0U);return _tmpBB;};_pop_handler();}else{void*_tmpB9=(void*)_exn_thrown;void*_tmpBC=_tmpB9;void*_tmpC4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBC)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 523
struct _tuple15*_tmpBD=({struct _tuple15*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=_tmpC6,_tmpC3->f2=_tmpC5;_tmpC3;});
struct _tuple1*_tmpBE=_tmpC6;union Cyc_Absyn_Nmspace _tmpC2;struct _dyneither_ptr*_tmpC1;_LLC: _tmpC2=_tmpBE->f1;_tmpC1=_tmpBE->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC1,_tmpC5);
struct _tuple1*res=({struct _tuple1*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=_tmpC2,({struct _dyneither_ptr*_tmp7C6=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));*_tmpBF=newvar;_tmpBF;});_tmpC0->f2=_tmp7C6;});_tmpC0;});
({struct Cyc_Dict_Dict _tmp7C7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC8,_tmpBD,res);*_tmpC8=_tmp7C7;});
return res;};}}else{_LL9: _tmpC4=_tmpBC;_LLA:(int)_rethrow(_tmpC4);}_LL6:;}};};};}
# 532
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp77D;_tmp77D.f1=fieldname,_tmp77D.f2=dtname;_tmp77D;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 539
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->kind=Cyc_Absyn_StructA,_tmpCB->sc=Cyc_Absyn_Public,_tmpCB->tvs=0,_tmpCB->attributes=0,_tmpCB->expected_mem_kind=0,({
# 542
struct _tuple1*_tmp7CA=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));({union Cyc_Absyn_Nmspace _tmp7C9=Cyc_Absyn_Rel_n(0);_tmpC9->f1=_tmp7C9;}),_tmpC9->f2=name;_tmpC9;});_tmpCB->name=_tmp7CA;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7C8=({struct Cyc_Absyn_AggrdeclImpl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->exist_vars=0,_tmpCA->rgn_po=0,_tmpCA->tagged=0,_tmpCA->fields=fs;_tmpCA;});_tmpCB->impl=_tmp7C8;});_tmpCB;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 551
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 554
struct _tuple22 _tmpCC=*env;struct _tuple22 _tmpCD=_tmpCC;struct Cyc_List_List**_tmpE4;struct Cyc_List_List*_tmpE3;_LL1: _tmpE4=(_tmpCD.f1)->tuple_types;_tmpE3=_tmpCD.f2;_LL2:;
# 556
{struct Cyc_List_List*_tmpCE=*_tmpE4;for(0;_tmpCE != 0;_tmpCE=_tmpCE->tl){
struct _tuple23*_tmpCF=(struct _tuple23*)_tmpCE->hd;struct _tuple23*_tmpD0=_tmpCF;void*_tmpD3;struct Cyc_List_List*_tmpD2;_LL4: _tmpD3=_tmpD0->f1;_tmpD2=_tmpD0->f2;_LL5:;{
struct Cyc_List_List*_tmpD1=_tmpE3;
for(0;_tmpD1 != 0  && _tmpD2 != 0;(_tmpD1=_tmpD1->tl,_tmpD2=_tmpD2->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD1->hd)).f2,(void*)_tmpD2->hd))
break;}
if(_tmpD1 == 0  && _tmpD2 == 0)
return _tmpD3;};}}{
# 567
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _dyneither_ptr _tmp7CC=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp77E;_tmp77E.tag=1U,_tmp77E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp77E;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _dyneither_ptr _tmp7CB=({const char*_tmpE0="_tuple%d";_tag_dyneither(_tmpE0,sizeof(char),9U);});Cyc_aprintf(_tmp7CB,_tag_dyneither(_tmpDF,sizeof(void*),1U));});});*_tmpE2=_tmp7CC;});_tmpE2;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE3);
struct Cyc_List_List*_tmpD4=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7CF=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Aggrfield*_tmp7CE=({struct Cyc_Absyn_Aggrfield*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct _dyneither_ptr*_tmp7CD=Cyc_Absyn_fieldname(i);_tmpD5->name=_tmp7CD;}),_tmpD5->tq=Cyc_Toc_mt_tq,_tmpD5->type=(void*)ts2->hd,_tmpD5->width=0,_tmpD5->attributes=0,_tmpD5->requires_clause=0;_tmpD5;});_tmpD6->hd=_tmp7CE;}),_tmpD6->tl=_tmpD4;_tmpD6;});_tmpD4=_tmp7CF;});}}
({struct Cyc_List_List*_tmp7D0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);_tmpD4=_tmp7D0;});{
struct Cyc_Absyn_Aggrdecl*_tmpD7=Cyc_Toc_make_c_struct_defn(xname,_tmpD4);
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7D1=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=_tmpD7;_tmpDD;}));(_tmpDE->f1).aggr_info=_tmp7D1;}),(_tmpDE->f1).targs=0;_tmpDE;});
({struct Cyc_List_List*_tmp7D3=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Decl*_tmp7D2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=5U,_tmpD9->f1=_tmpD7;_tmpD9;}),0U);_tmpDA->hd=_tmp7D2;}),_tmpDA->tl=Cyc_Toc_result_decls;_tmpDA;});Cyc_Toc_result_decls=_tmp7D3;});
({struct Cyc_List_List*_tmp7D5=({struct Cyc_List_List*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));({struct _tuple23*_tmp7D4=({struct _tuple23*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));_tmpDB->f1=(void*)_tmpD8,_tmpDB->f2=ts;_tmpDB;});_tmpDC->hd=_tmp7D4;}),_tmpDC->tl=*_tmpE4;_tmpDC;});*_tmpE4=_tmp7D5;});
return(void*)_tmpD8;};};}
# 580
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 585
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 588
struct _tuple25*_tmpE5=env;struct Cyc_List_List**_tmpFA;enum Cyc_Absyn_AggrKind _tmpF9;struct Cyc_List_List*_tmpF8;_LL1: _tmpFA=(_tmpE5->f1)->anon_aggr_types;_tmpF9=(_tmpE5->f2)->f1;_tmpF8=(_tmpE5->f2)->f2;_LL2:;
# 590
{struct Cyc_List_List*_tmpE6=*_tmpFA;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct _tuple26*_tmpE7=(struct _tuple26*)_tmpE6->hd;struct _tuple26*_tmpE8=_tmpE7;void*_tmpEB;enum Cyc_Absyn_AggrKind _tmpEA;struct Cyc_List_List*_tmpE9;_LL4: _tmpEB=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpE9=_tmpE8->f3;_LL5:;
if(_tmpF9 != _tmpEA)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE9,_tmpF8))
return _tmpEB;}}{
# 599
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp7D7=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp77F;_tmp77F.tag=1U,_tmp77F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp77F;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp7D6=({const char*_tmpF5="_tuple%d";_tag_dyneither(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp7D6,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp7D7;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpEC=Cyc_Toc_make_c_struct_defn(xname,_tmpF8);
_tmpEC->kind=_tmpF9;{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpED=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7D8=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF2=_cycalloc(sizeof(*_tmpF2));*_tmpF2=_tmpEC;_tmpF2;}));(_tmpF3->f1).aggr_info=_tmp7D8;}),(_tmpF3->f1).targs=0;_tmpF3;});
({struct Cyc_List_List*_tmp7DA=({struct Cyc_List_List*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_Absyn_Decl*_tmp7D9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=5U,_tmpEE->f1=_tmpEC;_tmpEE;}),0U);_tmpEF->hd=_tmp7D9;}),_tmpEF->tl=Cyc_Toc_result_decls;_tmpEF;});Cyc_Toc_result_decls=_tmp7DA;});
({struct Cyc_List_List*_tmp7DC=({struct Cyc_List_List*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));({struct _tuple26*_tmp7DB=({struct _tuple26*_tmpF0=_region_malloc(d,sizeof(*_tmpF0));_tmpF0->f1=(void*)_tmpED,_tmpF0->f2=_tmpF9,_tmpF0->f3=_tmpF8;_tmpF0;});_tmpF1->hd=_tmp7DB;}),_tmpF1->tl=*_tmpFA;_tmpF1;});*_tmpFA=_tmp7DC;});
return(void*)_tmpED;};};}
# 607
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=ak,_tmpFB->f2=fs;_tmpFB;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 616
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 625
struct _tuple28 _tmpFC=*env;struct _tuple28 _tmpFD=_tmpFC;struct Cyc_List_List**_tmp120;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;_LL1: _tmp120=(_tmpFD.f1)->abs_struct_types;_tmp11F=(_tmpFD.f2)->f1;_tmp11E=(_tmpFD.f2)->f2;_tmp11D=(_tmpFD.f2)->f3;_tmp11C=(_tmpFD.f2)->f4;_LL2:;
# 629
{struct Cyc_List_List*_tmpFE=*_tmp120;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple29*_tmpFF=(struct _tuple29*)_tmpFE->hd;struct _tuple29*_tmp100=_tmpFF;struct _tuple1*_tmp108;struct Cyc_List_List*_tmp107;void*_tmp106;_LL4: _tmp108=_tmp100->f1;_tmp107=_tmp100->f2;_tmp106=_tmp100->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp108,_tmp11F)== 0  && ({
int _tmp7DD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11D);_tmp7DD == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp107);})){
int okay=1;
{struct Cyc_List_List*_tmp101=_tmp11D;for(0;_tmp101 != 0;(_tmp101=_tmp101->tl,_tmp107=_tmp107->tl)){
void*_tmp102=(void*)_tmp101->hd;
void*_tmp103=(void*)((struct Cyc_List_List*)_check_null(_tmp107))->hd;
{struct Cyc_Absyn_Kind*_tmp104=Cyc_Tcutil_typ_kind(_tmp102);struct Cyc_Absyn_Kind*_tmp105=_tmp104;switch(((struct Cyc_Absyn_Kind*)_tmp105)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 640
 continue;default: _LLB: _LLC:
# 643
 if(Cyc_Tcutil_unify(_tmp102,_tmp103) || ({void*_tmp7DE=Cyc_Toc_typ_to_c(_tmp102);Cyc_Tcutil_unify(_tmp7DE,Cyc_Toc_typ_to_c(_tmp103));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 648
break;}}
# 650
if(okay)
return _tmp106;}}}{
# 657
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _dyneither_ptr _tmp7E0=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp780;_tmp780.tag=1U,_tmp780.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp780;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct _dyneither_ptr _tmp7DF=({const char*_tmp119="_tuple%d";_tag_dyneither(_tmp119,sizeof(char),9U);});Cyc_aprintf(_tmp7DF,_tag_dyneither(_tmp118,sizeof(void*),1U));});});*_tmp11B=_tmp7E0;});_tmp11B;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp109=0;
# 661
({struct Cyc_List_List*_tmp7E2=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));({struct _tuple29*_tmp7E1=({struct _tuple29*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));_tmp10A->f1=_tmp11F,_tmp10A->f2=_tmp11D,_tmp10A->f3=x;_tmp10A;});_tmp10B->hd=_tmp7E1;}),_tmp10B->tl=*_tmp120;_tmp10B;});*_tmp120=_tmp7E2;});{
# 664
struct _RegionHandle _tmp10C=_new_region("r");struct _RegionHandle*r=& _tmp10C;_push_region(r);
{struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11E,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
struct Cyc_Absyn_Aggrfield*_tmp10E=(struct Cyc_Absyn_Aggrfield*)_tmp11C->hd;
void*t=_tmp10E->type;
struct Cyc_List_List*atts=_tmp10E->attributes;
# 674
if((_tmp11C->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))) && !
Cyc_Toc_is_array_type(t))
({struct Cyc_List_List*_tmp7E4=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({void*_tmp7E3=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=0;_tmp10F;});_tmp110->hd=_tmp7E3;}),_tmp110->tl=atts;_tmp110;});atts=_tmp7E4;});
({void*_tmp7E5=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10D,t));t=_tmp7E5;});
# 681
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp7EA=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=8U,({void*_tmp7E9=Cyc_Absyn_void_star_typ();(_tmp111->f1).elt_type=_tmp7E9;}),({struct Cyc_Absyn_Tqual _tmp7E8=Cyc_Absyn_empty_tqual(0U);(_tmp111->f1).tq=_tmp7E8;}),({
struct Cyc_Absyn_Exp*_tmp7E7=Cyc_Absyn_uint_exp(0U,0U);(_tmp111->f1).num_elts=_tmp7E7;}),({union Cyc_Absyn_Constraint*_tmp7E6=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp111->f1).zero_term=_tmp7E6;}),(_tmp111->f1).zt_loc=0U;_tmp111;});
# 682
t=_tmp7EA;});
# 685
({struct Cyc_List_List*_tmp7EC=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Aggrfield*_tmp7EB=({struct Cyc_Absyn_Aggrfield*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp10E->name,_tmp112->tq=Cyc_Toc_mt_tq,_tmp112->type=t,_tmp112->width=_tmp10E->width,_tmp112->attributes=atts,_tmp112->requires_clause=0;_tmp112;});_tmp113->hd=_tmp7EB;}),_tmp113->tl=_tmp109;_tmp113;});_tmp109=_tmp7EC;});}
# 687
({struct Cyc_List_List*_tmp7ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);_tmp109=_tmp7ED;});{
struct Cyc_Absyn_Aggrdecl*_tmp114=Cyc_Toc_make_c_struct_defn(xname,_tmp109);
({struct Cyc_List_List*_tmp7EF=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Decl*_tmp7EE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=5U,_tmp115->f1=_tmp114;_tmp115;}),0U);_tmp116->hd=_tmp7EE;}),_tmp116->tl=Cyc_Toc_result_decls;_tmp116;});Cyc_Toc_result_decls=_tmp7EF;});{
void*_tmp117=x;_npop_handler(0U);return _tmp117;};};}
# 665
;_pop_region(r);};};}
# 693
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 697
struct _tuple27 env=({struct _tuple27 _tmp781;_tmp781.f1=struct_name,_tmp781.f2=type_vars,_tmp781.f3=type_args,_tmp781.f4=fields;_tmp781;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 705
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7F2=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _dyneither_ptr _tmp7F1=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp782;_tmp782.tag=1U,_tmp782.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp782;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp7F0=({const char*_tmp122="_tmp%X";_tag_dyneither(_tmp122,sizeof(char),7U);});Cyc_aprintf(_tmp7F0,_tag_dyneither(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp7F1;});_tmp124;});_tmp125->f2=_tmp7F2;});_tmp125;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 711
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp126=*env;struct _tuple30 _tmp127=_tmp126;struct Cyc_Xarray_Xarray*_tmp12F;_LL1: _tmp12F=(_tmp127.f1)->temp_labels;_LL2:;{
int _tmp128=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7F3=_tmp128;_tmp7F3 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12F);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12F,_tmp128);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _dyneither_ptr _tmp7F5=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12D=({struct Cyc_Int_pa_PrintArg_struct _tmp783;_tmp783.tag=1U,_tmp783.f1=(unsigned int)_tmp128;_tmp783;});void*_tmp12B[1U];_tmp12B[0]=& _tmp12D;({struct _dyneither_ptr _tmp7F4=({const char*_tmp12C="_LL%X";_tag_dyneither(_tmp12C,sizeof(char),6U);});Cyc_aprintf(_tmp7F4,_tag_dyneither(_tmp12B,sizeof(void*),1U));});});*_tmp12E=_tmp7F5;});_tmp12E;});
if(({int _tmp7F6=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12F,res);_tmp7F6 != _tmp128;}))
({void*_tmp129=0U;({struct _dyneither_ptr _tmp7F7=({const char*_tmp12A="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12A,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F7,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
return res;};};}
# 722
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 728
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp130))->hd)->name)!= 0){
++ ans;
_tmp130=_tmp130->tl;}
# 735
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 741
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp131=*a;struct _tuple9 _tmp132=_tmp131;struct _dyneither_ptr*_tmp136;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;_LL1: _tmp136=_tmp132.f1;_tmp135=_tmp132.f2;_tmp134=_tmp132.f3;_LL2:;
return({struct _tuple9*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=_tmp136,_tmp133->f2=_tmp135,({void*_tmp7F8=Cyc_Toc_typ_to_c(_tmp134);_tmp133->f3=_tmp7F8;});_tmp133;});}
# 764 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual _tmp13C;struct Cyc_Absyn_Exp*_tmp13B;union Cyc_Absyn_Constraint*_tmp13A;unsigned int _tmp139;switch(*((int*)_tmp138)){case 8U: _LL1: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).elt_type;_tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).tq;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).num_elts;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zero_term;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zt_loc;_LL2:
# 767
 return({void*_tmp7F9=Cyc_Toc_typ_to_c_array(_tmp13D);Cyc_Absyn_cstar_typ(_tmp7F9,_tmp13C);});case 1U: _LL3: _tmp13E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp138)->f2;if(_tmp13E != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13E);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 773
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 775
struct Cyc_Absyn_Aggrfield*_tmp13F=({struct Cyc_Absyn_Aggrfield*_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=*f;_tmp140;});
_tmp13F->type=new_type;
_tmp13F->requires_clause=0;
_tmp13F->tq=Cyc_Toc_mt_tq;
return _tmp13F;}
# 782
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 784
return;}
# 787
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp7FB=({void**_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp7FA=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);*_tmp141=_tmp7FA;});_tmp141;});cs=_tmp7FB;});
return*cs;}
# 793
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7FE=({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp7FD=({void*_tmp7FC=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7FC,Cyc_Toc_mt_tq);});*_tmp142=_tmp7FD;});_tmp142;});r=_tmp7FE;});
return*r;}
# 799
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp801=({void**_tmp143=_cycalloc(sizeof(*_tmp143));({void*_tmp800=({void*_tmp7FF=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7FF,Cyc_Toc_mt_tq);});*_tmp143=_tmp800;});_tmp143;});r=_tmp801;});
return*r;}
# 805
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp145=_tmp144;struct Cyc_Absyn_Tvar*_tmp146;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->tag == 2U){_LL1: _tmp146=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 811
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp147=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp148=_tmp147;if(((struct Cyc_Absyn_Kind*)_tmp148)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 818
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp149=t;struct Cyc_Absyn_Datatypedecl*_tmp192;void**_tmp191;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Typedefdecl*_tmp18A;void*_tmp189;struct Cyc_List_List*_tmp188;struct _tuple1*_tmp187;union Cyc_Absyn_AggrInfoU _tmp186;struct Cyc_List_List*_tmp185;enum Cyc_Absyn_AggrKind _tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Tqual _tmp181;void*_tmp180;struct Cyc_List_List*_tmp17F;int _tmp17E;struct Cyc_Absyn_VarargInfo*_tmp17D;struct Cyc_List_List*_tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp179;unsigned int _tmp178;void*_tmp177;struct Cyc_Absyn_Tqual _tmp176;union Cyc_Absyn_Constraint*_tmp175;struct Cyc_Absyn_Datatypedecl*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;struct Cyc_Absyn_Tvar*_tmp172;void**_tmp171;switch(*((int*)_tmp149)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp171=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp149)->f2;_LL4:
# 822
 if(*_tmp171 == 0){
*_tmp171=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 826
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp171));case 2U: _LL5: _tmp172=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL6:
# 828
 if((Cyc_Tcutil_tvar_kind(_tmp172,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 830
return(void*)& Cyc_Absyn_VoidType_val;else{
# 832
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 834
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp174=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f1;_tmp173=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 836
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp173->name,_tmp174->name));}else{_LLB: _LLC:
({void*_tmp14A=0U;({struct _dyneither_ptr _tmp802=({const char*_tmp14B="unresolved DatatypeFieldType";_tag_dyneither(_tmp14B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp802,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});}case 5U: _LLD: _tmp177=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_typ;_tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_tq;_tmp175=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).bounds;_LLE:
# 841
({void*_tmp803=Cyc_Toc_typ_to_c(_tmp177);_tmp177=_tmp803;});{
void*_tmp14C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp175);void*_tmp14D=_tmp14C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp14D)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp177,(void*)& Cyc_Absyn_HeapRgn_val,_tmp176,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 846
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp17B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).tq;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).num_elts;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).zt_loc;_LL14:
# 849
 return({void*_tmp807=Cyc_Toc_typ_to_c(_tmp17B);struct Cyc_Absyn_Tqual _tmp806=_tmp17A;struct Cyc_Absyn_Exp*_tmp805=_tmp179;union Cyc_Absyn_Constraint*_tmp804=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp807,_tmp806,_tmp805,_tmp804,_tmp178);});case 9U: _LL15: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_tqual;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_typ;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).args;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).c_varargs;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).cyc_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).attributes;_LL16: {
# 855
struct Cyc_List_List*_tmp14E=0;
for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
void*_tmp14F=(void*)_tmp17C->hd;void*_tmp150=_tmp14F;switch(*((int*)_tmp150)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp808=({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->hd=(void*)_tmp17C->hd,_tmp151->tl=_tmp14E;_tmp151;});_tmp14E=_tmp808;});goto _LL46;}_LL46:;}{
# 866
struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17F);
if(_tmp17D != 0){
# 869
void*_tmp153=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp17D->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp154=({struct _tuple9*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->f1=_tmp17D->name,_tmp156->f2=_tmp17D->tq,_tmp156->f3=_tmp153;_tmp156;});
({struct Cyc_List_List*_tmp80A=({struct Cyc_List_List*_tmp809=_tmp152;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp809,({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=_tmp154,_tmp155->tl=0;_tmp155;}));});_tmp152=_tmp80A;});}
# 873
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->tag=9U,(_tmp157->f1).tvars=0,(_tmp157->f1).effect=0,(_tmp157->f1).ret_tqual=_tmp181,({void*_tmp80B=Cyc_Toc_typ_to_c(_tmp180);(_tmp157->f1).ret_typ=_tmp80B;}),(_tmp157->f1).args=_tmp152,(_tmp157->f1).c_varargs=_tmp17E,(_tmp157->f1).cyc_varargs=0,(_tmp157->f1).rgn_po=0,(_tmp157->f1).attributes=_tmp14E,(_tmp157->f1).requires_clause=0,(_tmp157->f1).requires_relns=0,(_tmp157->f1).ensures_clause=0,(_tmp157->f1).ensures_relns=0;_tmp157;});};}case 10U: _LL17: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp149)->f1;_LL18: {
# 878
struct Cyc_List_List*_tmp158=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp80E=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct _tuple12*_tmp80D=({struct _tuple12*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->f1=(*((struct _tuple12*)_tmp182->hd)).f1,({void*_tmp80C=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp182->hd)).f2);_tmp159->f2=_tmp80C;});_tmp159;});_tmp15A->hd=_tmp80D;}),_tmp15A->tl=_tmp158;_tmp15A;});_tmp158=_tmp80E;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp158));}case 12U: _LL19: _tmp184=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f1;_tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f2;_LL1A: {
# 887
struct Cyc_List_List*_tmp15B=0;
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
({struct Cyc_List_List*_tmp811=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));({struct Cyc_Absyn_Aggrfield*_tmp810=({struct Cyc_Absyn_Aggrfield*_tmp80F=(struct Cyc_Absyn_Aggrfield*)_tmp183->hd;Cyc_Toc_aggrfield_to_c(_tmp80F,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp183->hd)->type));});_tmp15C->hd=_tmp810;}),_tmp15C->tl=_tmp15B;_tmp15C;});_tmp15B=_tmp811;});}
return({enum Cyc_Absyn_AggrKind _tmp812=_tmp184;Cyc_Toc_add_anon_aggr_type(_tmp812,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15B));});}case 11U: _LL1B: _tmp186=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).aggr_info;_tmp185=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).targs;_LL1C:
# 893
{union Cyc_Absyn_AggrInfoU _tmp15D=_tmp186;if((_tmp15D.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 897
struct Cyc_Absyn_Aggrdecl*_tmp15E=Cyc_Absyn_get_known_aggrdecl(_tmp186);
if(_tmp15E->expected_mem_kind){
# 900
if(_tmp15E->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp785;_tmp785.tag=0U,({
struct _dyneither_ptr _tmp813=(struct _dyneither_ptr)(_tmp15E->kind == Cyc_Absyn_UnionA?({const char*_tmp163="union";_tag_dyneither(_tmp163,sizeof(char),6U);}):({const char*_tmp164="struct";_tag_dyneither(_tmp164,sizeof(char),7U);}));_tmp785.f1=_tmp813;});_tmp785;});struct Cyc_String_pa_PrintArg_struct _tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp784;_tmp784.tag=0U,({
struct _dyneither_ptr _tmp814=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp784.f1=_tmp814;});_tmp784;});void*_tmp15F[2U];_tmp15F[0]=& _tmp161,_tmp15F[1]=& _tmp162;({struct _dyneither_ptr _tmp815=({const char*_tmp160="%s %s was never defined.";_tag_dyneither(_tmp160,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp815,_tag_dyneither(_tmp15F,sizeof(void*),2U));});});}
# 908
if(_tmp15E->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp165=_tmp15E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields;
if(_tmp165 == 0)return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp165))->tl != 0;_tmp165=_tmp165->tl){;}{
void*_tmp166=((struct Cyc_Absyn_Aggrfield*)_tmp165->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp166))){
if(_tmp15E->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp169=({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0U,({struct _dyneither_ptr _tmp816=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp786.f1=_tmp816;});_tmp786;});void*_tmp167[1U];_tmp167[0]=& _tmp169;({struct _dyneither_ptr _tmp817=({const char*_tmp168="struct %s ended up being abstract.";_tag_dyneither(_tmp168,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp817,_tag_dyneither(_tmp167,sizeof(void*),1U));});});{
# 921
struct _RegionHandle _tmp16A=_new_region("r");struct _RegionHandle*r=& _tmp16A;_push_region(r);
{struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E->tvs,_tmp185);
void*_tmp16C=Cyc_Tcutil_rsubstitute(r,_tmp16B,_tmp166);
if(Cyc_Toc_is_abstract_type(_tmp16C)){void*_tmp16D=Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16D;}{
void*_tmp16E=Cyc_Toc_add_struct_type(_tmp15E->name,_tmp15E->tvs,_tmp185,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields);_npop_handler(0U);return _tmp16E;};}
# 922
;_pop_region(r);};}
# 927
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp187=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp149)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp188=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp149)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp188);return t;case 17U: _LL21: _tmp18C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f1;_tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f2;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f3;_tmp189=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f4;_LL22:
# 933
 if(_tmp189 == 0){
if(_tmp18B != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=17U,_tmp16F->f1=_tmp18C,_tmp16F->f2=0,_tmp16F->f3=_tmp18A,_tmp16F->f4=0;_tmp16F;});else{
return t;}}else{
# 938
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=17U,_tmp170->f1=_tmp18C,_tmp170->f2=0,_tmp170->f3=_tmp18A,({void*_tmp818=Cyc_Toc_typ_to_c(_tmp189);_tmp170->f4=_tmp818;});_tmp170;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 944
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp18D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp149)->f1;_LL36:
# 954
 return t;case 27U: _LL37: _tmp18E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp149)->f1;_LL38:
# 956
 return t;case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)){case 0U: _LL3B: _tmp18F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3C:
# 959
 Cyc_Toc_aggrdecl_to_c(_tmp18F,1);
if(_tmp18F->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp190=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3E:
# 964
 Cyc_Toc_enumdecl_to_c(_tmp190);
return t;default: _LL3F: _tmp192=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_tmp191=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f2;_LL40:
# 967
 Cyc_Toc_datatypedecl_to_c(_tmp192);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp191)));}}_LL0:;}
# 972
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp193=t;void*_tmp195;struct Cyc_Absyn_Tqual _tmp194;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->tag == 8U){_LL1: _tmp195=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).tq;_LL2:
# 975
 return({void*_tmp819=Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp194,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp819,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 982
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Datatypedecl*_tmp1A2;struct Cyc_Absyn_Datatypefield*_tmp1A1;struct Cyc_List_List*_tmp1A0;union Cyc_Absyn_AggrInfoU _tmp19F;void*_tmp19E;switch(*((int*)_tmp197)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp19E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp19E);case 11U: _LL13: _tmp19F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp197)->f1).aggr_info;_LL14:
# 999
{union Cyc_Absyn_AggrInfoU _tmp198=_tmp19F;if((_tmp198.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 1003
struct Cyc_Absyn_Aggrdecl*_tmp199=Cyc_Absyn_get_known_aggrdecl(_tmp19F);
if(_tmp199->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp199->impl))->fields;for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp19A->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp1A0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp197)->f2;_LL16:
# 1010
 for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1A0->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp1A2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f1;_tmp1A1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 1014
 _tmp1A3=_tmp1A1->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp1A3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp197)->f1;_LL1A:
# 1016
 for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple12*)_tmp1A3->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 1021
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp19D=({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0U,({struct _dyneither_ptr _tmp81A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp787.f1=_tmp81A;});_tmp787;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19D;({struct _dyneither_ptr _tmp81B=({const char*_tmp19C="atomic_typ:  bad type %s";_tag_dyneither(_tmp19C,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81B,_tag_dyneither(_tmp19B,sizeof(void*),1U));});});}_LL0:;}
# 1029
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A4=Cyc_Tcutil_compress(t);void*_tmp1A5=_tmp1A4;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->tag == 5U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_LL2: {
# 1032
void*_tmp1A6=Cyc_Tcutil_compress(_tmp1A8);void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A7)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 1036
 return 0;}_LL0:;}
# 1040
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1044
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1049
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfoU _tmp1B5;switch(*((int*)_tmp1AA)){case 11U: _LL1: _tmp1B5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AA)->f1).aggr_info;_LL2: {
# 1052
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp81C=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81C,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1057
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp788;_tmp788.tag=0U,_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp788;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp81D=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81D,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp789;_tmp789.tag=0U,({struct _dyneither_ptr _tmp81E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp789.f1=_tmp81E;});_tmp789;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp81F=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81F,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1068
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1071
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1074
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 5U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_typ;_LL9:
# 1076
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0U,({struct _dyneither_ptr _tmp820=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp78A.f1=_tmp820;});_tmp78A;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp821=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp821,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1079
 return 0;}_LL0:;}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp822=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp822,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp823=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp823,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1098
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp824=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp824,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp825=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp825,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp826=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp826,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1110
return({struct Cyc_Absyn_Exp*_tmp827=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp827,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp828=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp828,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp829=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp829,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1120
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp82A=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp82A,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1124
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp82B=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp82B,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1129
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1137
int is_string=0;
{void*_tmp1CD=e->r;void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1143
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp82D=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp82C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp82C;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp82D;});
({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Absyn_var_exp(x,0U);xexp=_tmp82E;});
({struct Cyc_Absyn_Exp*_tmp82F=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp82F;});}else{
# 1153
({struct Cyc_Absyn_Exp*_tmp831=({void*_tmp830=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp830,e);});xexp=_tmp831;});
# 1155
({struct Cyc_Absyn_Exp*_tmp833=({void*_tmp832=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp832,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp833;});}
# 1157
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D1[3U];({struct _tuple20*_tmp836=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp836;}),({
struct _tuple20*_tmp835=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp835;}),({
struct _tuple20*_tmp834=({struct _tuple20*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp834;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1194 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1198
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D7=nv;int*_tmp1D8;_LL1: _tmp1D8=_tmp1D7->in_lhs;_LL2:;
return*_tmp1D8;}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=0,_tmp1DB->toplevel=1,({
# 1209
int*_tmp838=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp838;}),({
int*_tmp837=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp837;}),_tmp1DB->rgn=r;_tmp1DB;});}
# 1214
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DC=e;struct _dyneither_ptr**_tmp1E3;struct _dyneither_ptr**_tmp1E2;struct Cyc_Toc_FallthruInfo*_tmp1E1;int _tmp1E0;int*_tmp1DF;int*_tmp1DE;_LL1: _tmp1E3=_tmp1DC->break_lab;_tmp1E2=_tmp1DC->continue_lab;_tmp1E1=_tmp1DC->fallthru_info;_tmp1E0=_tmp1DC->toplevel;_tmp1DF=_tmp1DC->in_lhs;_tmp1DE=_tmp1DC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=_tmp1E3,_tmp1DD->continue_lab=_tmp1E2,_tmp1DD->fallthru_info=_tmp1E1,_tmp1DD->toplevel=_tmp1E0,_tmp1DD->in_lhs=_tmp1DF,_tmp1DD->in_sizeof=_tmp1DE,_tmp1DD->rgn=r;_tmp1DD;});}
# 1219
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E4=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;int*_tmp1E6;_LL1: _tmp1EB=_tmp1E4->break_lab;_tmp1EA=_tmp1E4->continue_lab;_tmp1E9=_tmp1E4->fallthru_info;_tmp1E8=_tmp1E4->toplevel;_tmp1E7=_tmp1E4->in_lhs;_tmp1E6=_tmp1E4->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=_tmp1EB,_tmp1E5->continue_lab=_tmp1EA,_tmp1E5->fallthru_info=_tmp1E9,_tmp1E5->toplevel=0,_tmp1E5->in_lhs=_tmp1E7,_tmp1E5->in_sizeof=_tmp1E6,_tmp1E5->rgn=r;_tmp1E5;});}
# 1223
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F3;struct _dyneither_ptr**_tmp1F2;struct Cyc_Toc_FallthruInfo*_tmp1F1;int _tmp1F0;int*_tmp1EF;int*_tmp1EE;_LL1: _tmp1F3=_tmp1EC->break_lab;_tmp1F2=_tmp1EC->continue_lab;_tmp1F1=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1EF=_tmp1EC->in_lhs;_tmp1EE=_tmp1EC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=_tmp1F3,_tmp1ED->continue_lab=_tmp1F2,_tmp1ED->fallthru_info=_tmp1F1,_tmp1ED->toplevel=1,_tmp1ED->in_lhs=_tmp1EF,_tmp1ED->in_sizeof=_tmp1EE,_tmp1ED->rgn=r;_tmp1ED;});}
# 1227
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->in_lhs;_LL2:;
*_tmp1F5=b;}
# 1231
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F6=e;int*_tmp1F8;_LL1: _tmp1F8=_tmp1F6->in_sizeof;_LL2:;{
int _tmp1F7=*_tmp1F8;
*_tmp1F8=b;
return _tmp1F7;};}
# 1237
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FA;_LL1: _tmp1FA=_tmp1F9->in_sizeof;_LL2:;
return*_tmp1FA;}
# 1244
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FB=e;struct _dyneither_ptr**_tmp202;struct _dyneither_ptr**_tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;int _tmp1FF;int*_tmp1FE;int*_tmp1FD;_LL1: _tmp202=_tmp1FB->break_lab;_tmp201=_tmp1FB->continue_lab;_tmp200=_tmp1FB->fallthru_info;_tmp1FF=_tmp1FB->toplevel;_tmp1FE=_tmp1FB->in_lhs;_tmp1FD=_tmp1FB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->break_lab=0,_tmp1FC->continue_lab=0,_tmp1FC->fallthru_info=_tmp200,_tmp1FC->toplevel=_tmp1FF,_tmp1FC->in_lhs=_tmp1FE,_tmp1FC->in_sizeof=_tmp1FD,_tmp1FC->rgn=r;_tmp1FC;});}
# 1250
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1254
struct Cyc_Toc_Env*_tmp203=e;struct _dyneither_ptr**_tmp20C;struct _dyneither_ptr**_tmp20B;struct Cyc_Toc_FallthruInfo*_tmp20A;int _tmp209;int*_tmp208;int*_tmp207;_LL1: _tmp20C=_tmp203->break_lab;_tmp20B=_tmp203->continue_lab;_tmp20A=_tmp203->fallthru_info;_tmp209=_tmp203->toplevel;_tmp208=_tmp203->in_lhs;_tmp207=_tmp203->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp206=_region_malloc(r,sizeof(*_tmp206));
_tmp206->label=fallthru_l,_tmp206->binders=fallthru_binders;_tmp206;});
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp839=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp839;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1260
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1262
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp83A=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp83A;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1269
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1272
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp83B=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp83B;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1285 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1288
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1290
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1293
void*_tmp221=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp222=_tmp221;union Cyc_Absyn_Constraint*_tmp225;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->tag == 5U){_LLC: _tmp225=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->f1).ptr_atts).nullable;_LLD:
# 1296
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp225);}else{_LLE: _LLF:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp83C=({const char*_tmp224="Toc::is_nullable";_tag_dyneither(_tmp224,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83C,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1301
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1303
({void*_tmp226=0U;({unsigned int _tmp83E=e->loc;struct _dyneither_ptr _tmp83D=({const char*_tmp227="NULL pointer check will definitely fail";_tag_dyneither(_tmp227,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp83E,_tmp83D,_tag_dyneither(_tmp226,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1309
 return 1;}else{_LL9: _LLA:
({void*_tmp228=0U;({struct _dyneither_ptr _tmp83F=({const char*_tmp229="need_null_check";_tag_dyneither(_tmp229,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83F,_tag_dyneither(_tmp228,sizeof(void*),0U));});});}}}}_LL0:;}
# 1314
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp22A=e->annot;void*_tmp22B=_tmp22A;struct Cyc_List_List*_tmp231;struct Cyc_List_List*_tmp230;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp230=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL2:
 return _tmp230;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp231=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL4:
 return _tmp231;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1319
({void*_tmp22C=0U;({unsigned int _tmp841=e->loc;struct _dyneither_ptr _tmp840=({const char*_tmp22D="dereference of NULL pointer";_tag_dyneither(_tmp22D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp841,_tmp840,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp22E=0U;({struct _dyneither_ptr _tmp842=({const char*_tmp22F="get_relns";_tag_dyneither(_tmp22F,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp842,_tag_dyneither(_tmp22E,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp235[8U]="*bogus*";
# 1329
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1339 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1342
struct Cyc_Absyn_Vardecl*x;
{void*_tmp232=a->r;void*_tmp233=_tmp232;void*_tmp236;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->tag == 1U){_LL1: _tmp236=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_LL2: {
# 1345
struct Cyc_Absyn_Vardecl*_tmp234=Cyc_Tcutil_nonesc_vardecl(_tmp236);
if(_tmp234 == 0)goto _LL4;
x=_tmp234;
goto _LL0;}}else{_LL3: _LL4: {
# 1351
static struct _dyneither_ptr bogus_string={_tmp235,_tmp235,_tmp235 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1354
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1360
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1366
void*_tmp237=a_typ;
void*_tmp238=x->type;
x->type=_tmp237;{
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1375
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp237);
if(bound != 0){
# 1378
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp843=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp843;});}
# 1385
if(valid_rop_i){
struct Cyc_List_List*_tmp239=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1393
int _tmp23A=Cyc_Relations_consistent_relations(_tmp239);
x->type=_tmp238;
if(!_tmp23A)return 1;}
# 1398
inner_loop: {
void*_tmp23B=i->r;void*_tmp23C=_tmp23B;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp241;void*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;switch(*((int*)_tmp23C)){case 14U: _LL6: _tmp240=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_tmp23F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f2;_LL7:
 i=_tmp23F;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl != 0){_LL8: _tmp242=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->hd;_tmp241=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl)->hd;_LL9: {
# 1404
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp241,& rop_z)){
# 1407
struct Cyc_List_List*_tmp23D=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1409
int _tmp23E=Cyc_Relations_consistent_relations(_tmp23D);
x->type=_tmp238;
return !_tmp23E;}
# 1413
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1416
x->type=_tmp238;
return 0;};};}
# 1420
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp243=0U;({struct _dyneither_ptr _tmp844=({const char*_tmp244="Missing type in primop ";_tag_dyneither(_tmp244,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp844,_tag_dyneither(_tmp243,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1424
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->f1=Cyc_Toc_mt_tq,({void*_tmp845=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp245->f2=_tmp845;});_tmp245;});}
# 1429
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp246=e->r;void*_tmp247=_tmp246;struct Cyc_Absyn_Exp*_tmp24F;int _tmp24E;struct Cyc_Absyn_Exp*_tmp24D;int _tmp24C;struct Cyc_List_List*_tmp24B;switch(*((int*)_tmp247)){case 26U: _LL1: _tmp24B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_LL2:
# 1432
{struct Cyc_List_List*_tmp248=_tmp24B;for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
if((*((struct _tuple20*)_tmp248->hd)).f1 != 0)
({void*_tmp249=0U;({struct _dyneither_ptr _tmp846=({const char*_tmp24A="array designators for abstract-field initialization";_tag_dyneither(_tmp24A,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp846,_tag_dyneither(_tmp249,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp847=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24B),0U);_tmp24D=_tmp847;});_tmp24C=0;goto _LL4;case 27U: _LL3: _tmp24D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f2;_tmp24C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f4;_LL4:
 _tmp24F=_tmp24D;_tmp24E=_tmp24C;goto _LL6;case 28U: _LL5: _tmp24F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_tmp24E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f3;_LL6:
# 1438
({struct Cyc_Absyn_Exp*_tmp848=Cyc_Absyn_copy_exp(_tmp24F);_tmp24F=_tmp848;});
return _tmp24E?({struct Cyc_Absyn_Exp*_tmp849=_tmp24F;Cyc_Absyn_add_exp(_tmp849,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24F;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1447
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1455
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp250=e->r;void*_tmp251=_tmp250;struct Cyc_List_List*_tmp252;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->tag == 29U){_LL1: _tmp252=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->f3;_LL2:
 dles=_tmp252;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1461
{void*_tmp253=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp254=_tmp253;struct Cyc_Absyn_Aggrdecl*_tmp255;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp255=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->f1).aggr_info).KnownAggr).val;_LL7:
# 1463
 if(_tmp255->impl == 0)
return 0;
if(_tmp255->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp255->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{_LL8: _LL9:
# 1471
 return 0;}_LL5:;}
# 1473
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp256=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp257=(struct _tuple20*)dles->hd;struct _tuple20*_tmp258=_tmp257;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_Exp*_tmp260;_LLB: _tmp261=_tmp258->f1;_tmp260=_tmp258->f2;_LLC:;{
struct _dyneither_ptr*_tmp259=Cyc_Absyn_designatorlist_to_fieldname(_tmp261);
if(!Cyc_strptrcmp(_tmp259,_tmp256->name)){
struct Cyc_Absyn_Exp*_tmp25A=Cyc_Toc_get_varsizeexp(_tmp260);
if(_tmp25A != 0)
return _tmp25A;{
void*_tmp25B=Cyc_Tcutil_compress(_tmp256->type);void*_tmp25C=_tmp25B;void*_tmp25F;struct Cyc_Absyn_Exp*_tmp25E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->tag == 8U){_LLE: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).elt_type;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).num_elts;_LLF:
# 1488
 if(_tmp25E == 0  || !Cyc_Toc_is_zero(_tmp25E))
return 0;
# 1494
return({struct Cyc_Absyn_Exp*_tmp84C=({struct Cyc_Absyn_Exp*_tmp84B=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp84B,({struct Cyc_Absyn_Exp*_tmp25D[2U];_tmp25D[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp260)),({
# 1496
struct Cyc_Absyn_Exp*_tmp84A=Cyc_Absyn_sizeoftyp_exp(_tmp25F,0U);_tmp25D[1]=_tmp84A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1494
Cyc_Absyn_add_exp(_tmp84C,
# 1497
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1502
({void*_tmp262=0U;({struct _dyneither_ptr _tmp84D=({const char*_tmp263="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp263,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84D,_tag_dyneither(_tmp262,sizeof(void*),0U));});});};}
# 1505
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp264=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
struct Cyc_Absyn_Aggrfield*_tmp265=(struct Cyc_Absyn_Aggrfield*)_tmp264->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp265->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1512
({void*_tmp266=0U;({struct _dyneither_ptr _tmp84F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0U,_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp78B;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct _dyneither_ptr _tmp84E=({const char*_tmp268="get_member_offset %s failed";_tag_dyneither(_tmp268,sizeof(char),28U);});Cyc_aprintf(_tmp84E,_tag_dyneither(_tmp267,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84F,_tag_dyneither(_tmp266,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1516
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1519
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1522
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->tag=5U,_tmp26A->f1=e1,_tmp26A->f2=incr;_tmp26A;}),0U);}
# 1526
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1535
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1537
void*_tmp26B=e1->r;void*_tmp26C=_tmp26B;struct Cyc_Absyn_Exp*_tmp274;struct _dyneither_ptr*_tmp273;int _tmp272;int _tmp271;void*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Stmt*_tmp26E;switch(*((int*)_tmp26C)){case 37U: _LL1: _tmp26E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26E,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp270=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp26F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26F,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp274=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp273=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp272=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f4;_LL6:
# 1542
 e1->r=_tmp274->r;
({struct Cyc_Absyn_Exp*_tmp852=e1;struct Cyc_List_List*_tmp851=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=_tmp273,_tmp26D->tl=fs;_tmp26D;});struct Cyc_Absyn_Exp*(*_tmp850)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp852,_tmp851,_tmp850,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1550
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1552
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp853=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp853;});}
({void*_tmp854=(f(e1_copy,f_env))->r;e1->r=_tmp854;});
goto _LL0;}}_LL0:;}
# 1558
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1560
void*_tmp275=s->r;void*_tmp276=_tmp275;struct Cyc_Absyn_Stmt*_tmp27D;struct Cyc_Absyn_Decl*_tmp27C;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Absyn_Exp*_tmp27A;switch(*((int*)_tmp276)){case 1U: _LL1: _tmp27A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp27A,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp27C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_tmp27B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL4:
# 1563
 Cyc_Toc_lvalue_assign_stmt(_tmp27B,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27D,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp279=({struct Cyc_String_pa_PrintArg_struct _tmp78C;_tmp78C.tag=0U,({struct _dyneither_ptr _tmp855=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp78C.f1=_tmp855;});_tmp78C;});void*_tmp277[1U];_tmp277[0]=& _tmp279;({struct _dyneither_ptr _tmp856=({const char*_tmp278="lvalue_assign_stmt: %s";_tag_dyneither(_tmp278,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp856,_tag_dyneither(_tmp277,sizeof(void*),1U));});});}_LL0:;}
# 1569
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1573
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27E=e->r;void*_tmp27F=_tmp27E;struct Cyc_Absyn_Stmt*_tmp286;struct Cyc_Absyn_Exp*_tmp285;void**_tmp284;struct Cyc_Absyn_Exp**_tmp283;switch(*((int*)_tmp27F)){case 14U: _LL1: _tmp284=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_tmp283=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f2;_LL2:
# 1576
({struct Cyc_Absyn_Exp*_tmp857=Cyc_Toc_push_address_exp(*_tmp283);*_tmp283=_tmp857;});
({void*_tmp858=Cyc_Absyn_cstar_typ(*_tmp284,Cyc_Toc_mt_tq);*_tmp284=_tmp858;});
return e;case 20U: _LL3: _tmp285=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL4:
# 1580
 return _tmp285;case 37U: _LL5: _tmp286=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL6:
# 1584
 Cyc_Toc_push_address_stmt(_tmp286);
return e;default: _LL7: _LL8:
# 1587
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,({struct _dyneither_ptr _tmp859=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp78D.f1=_tmp859;});_tmp78D;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp85A=({const char*_tmp281="can't take & of exp %s";_tag_dyneither(_tmp281,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85A,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}_LL0:;}
# 1592
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp287=s->r;void*_tmp288=_tmp287;struct Cyc_Absyn_Exp**_tmp28E;struct Cyc_Absyn_Stmt*_tmp28D;struct Cyc_Absyn_Stmt*_tmp28C;switch(*((int*)_tmp288)){case 2U: _LL1: _tmp28C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL2:
 _tmp28D=_tmp28C;goto _LL4;case 12U: _LL3: _tmp28D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28D);goto _LL0;case 1U: _LL5: _tmp28E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Toc_push_address_exp(*_tmp28E);*_tmp28E=_tmp85B;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp28B=({struct Cyc_String_pa_PrintArg_struct _tmp78E;_tmp78E.tag=0U,({struct _dyneither_ptr _tmp85C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp78E.f1=_tmp85C;});_tmp78E;});void*_tmp289[1U];_tmp289[0]=& _tmp28B;({struct _dyneither_ptr _tmp85D=({const char*_tmp28A="can't take & of stmt %s";_tag_dyneither(_tmp28A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85D,_tag_dyneither(_tmp289,sizeof(void*),1U));});});}_LL0:;}
# 1601
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp28F=Cyc_Tcutil_compress(t);void*_tmp290=_tmp28F;struct Cyc_Absyn_PtrInfo _tmp293;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->tag == 5U){_LL1: _tmp293=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->f1;_LL2:
 return _tmp293;}else{_LL3: _LL4:
({void*_tmp291=0U;({struct _dyneither_ptr _tmp85E=({const char*_tmp292="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp292,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85E,_tag_dyneither(_tmp291,sizeof(void*),0U));});});}_LL0:;}
# 1611
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp294=Cyc_Tcutil_compress(t);void*_tmp295=_tmp294;int _tmp2A1;enum Cyc_Absyn_Sign _tmp2A0;enum Cyc_Absyn_Sign _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;enum Cyc_Absyn_Sign _tmp29C;switch(*((int*)_tmp295)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_null_exp(0U);res=_tmp85F;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp29C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmp860=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29C,'\000'),0U);res=_tmp860;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp29D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp861=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29D,0),0U);res=_tmp861;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp29E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LLC:
# 1619
 _tmp29F=_tmp29E;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp29F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LLE:
# 1621
({struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp29F,0),0U);res=_tmp862;});goto _LL0;default: _LLF: _tmp2A0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL10:
# 1623
({struct Cyc_Absyn_Exp*_tmp863=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp2A0,(long long)0),0U);res=_tmp863;});goto _LL0;}case 13U: _LL7: _LL8:
# 1617
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp29E=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp295)->f1){case 0U: _LL11: _LL12:
# 1624
({struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp296="0.0F";_tag_dyneither(_tmp296,sizeof(char),5U);}),0),0U);res=_tmp864;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmp865=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp297="0.0";_tag_dyneither(_tmp297,sizeof(char),4U);}),1),0U);res=_tmp865;});goto _LL0;default: _LL15: _tmp2A1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp295)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp866=({const char*_tmp298="0.0L";_tag_dyneither(_tmp298,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp866,_tmp2A1);}),0U);res=_tmp867;});goto _LL0;}default: _LL17: _LL18:
# 1628
({struct Cyc_String_pa_PrintArg_struct _tmp29B=({struct Cyc_String_pa_PrintArg_struct _tmp78F;_tmp78F.tag=0U,({struct _dyneither_ptr _tmp868=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp78F.f1=_tmp868;});_tmp78F;});void*_tmp299[1U];_tmp299[0]=& _tmp29B;({struct _dyneither_ptr _tmp869=({const char*_tmp29A="found non-zero type %s in generate_zero";_tag_dyneither(_tmp29A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp869,_tag_dyneither(_tmp299,sizeof(void*),1U));});});}_LL0:;}
# 1630
res->topt=t;
return res;}
# 1637
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1692 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp2A2=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A3=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A4=Cyc_Absyn_cstar_typ(_tmp2A2,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A5=({struct Cyc_Core_Opt*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->v=_tmp2A4;_tmp2D6;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A6=e1->r;void*_tmp2A7=_tmp2A6;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;switch(*((int*)_tmp2A7)){case 20U: _LL1: _tmp2AB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A7)->f1;_LL2:
# 1700
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AB,0,Cyc_Absyn_Other_coercion,0U);_tmp2AB=_tmp86A;});
_tmp2AB->topt=fat_ptr_type;}
# 1704
Cyc_Toc_exp_to_c(nv,_tmp2AB);xinit=_tmp2AB;goto _LL0;case 23U: _LL3: _tmp2AD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A7)->f1;_tmp2AC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A7)->f2;_LL4:
# 1706
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AD,0,Cyc_Absyn_Other_coercion,0U);_tmp2AD=_tmp86B;});
_tmp2AD->topt=fat_ptr_type;}
# 1710
Cyc_Toc_exp_to_c(nv,_tmp2AD);Cyc_Toc_exp_to_c(nv,_tmp2AC);
({struct Cyc_Absyn_Exp*_tmp86E=({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp86D,({struct Cyc_Absyn_Exp*_tmp2A8[3U];_tmp2A8[0]=_tmp2AD,({
struct Cyc_Absyn_Exp*_tmp86C=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A8[1]=_tmp86C;}),_tmp2A8[2]=_tmp2AC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1711
xinit=_tmp86E;});
# 1713
goto _LL0;default: _LL5: _LL6:
({void*_tmp2A9=0U;({struct _dyneither_ptr _tmp86F=({const char*_tmp2AA="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2AA,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp86F,_tag_dyneither(_tmp2A9,sizeof(void*),0U));});});}_LL0:;}{
# 1716
struct _tuple1*_tmp2AE=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2AF=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2AF;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2B0=({struct Cyc_Absyn_Vardecl*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->sc=Cyc_Absyn_Public,_tmp2D5->name=_tmp2AE,_tmp2D5->varloc=0U,_tmp2D5->tq=Cyc_Toc_mt_tq,_tmp2D5->type=_tmp2A3,_tmp2D5->initializer=xinit,_tmp2D5->rgn=0,_tmp2D5->attributes=0,_tmp2D5->escapes=0;_tmp2D5;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B1=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->tag=4U,_tmp2D4->f1=_tmp2B0;_tmp2D4;});
struct Cyc_Absyn_Exp*_tmp2B2=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);
_tmp2B2->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_deref_exp(_tmp2B2,0U);
_tmp2B3->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B3);{
struct _tuple1*_tmp2B4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B5=({struct Cyc_Absyn_Vardecl*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->sc=Cyc_Absyn_Public,_tmp2D3->name=_tmp2B4,_tmp2D3->varloc=0U,_tmp2D3->tq=Cyc_Toc_mt_tq,_tmp2D3->type=_tmp2A2,_tmp2D3->initializer=_tmp2B3,_tmp2D3->rgn=0,_tmp2D3->attributes=0,_tmp2D3->escapes=0;_tmp2D3;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->tag=4U,_tmp2D2->f1=_tmp2B5;_tmp2D2;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);
_tmp2B7->topt=_tmp2B3->topt;
({struct Cyc_Absyn_Exp*_tmp870=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B7,e2,0U);z_init=_tmp870;});
z_init->topt=_tmp2B7->topt;}
# 1735
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B9=({struct Cyc_Absyn_Vardecl*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->sc=Cyc_Absyn_Public,_tmp2D1->name=_tmp2B8,_tmp2D1->varloc=0U,_tmp2D1->tq=Cyc_Toc_mt_tq,_tmp2D1->type=_tmp2A2,_tmp2D1->initializer=z_init,_tmp2D1->rgn=0,_tmp2D1->attributes=0,_tmp2D1->escapes=0;_tmp2D1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->tag=4U,_tmp2D0->f1=_tmp2B9;_tmp2D0;});
# 1741
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);_tmp2BB->topt=_tmp2B3->topt;{
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2BB,_tmp2BC,0U);
_tmp2BD->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BD);{
# 1747
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_varb_exp((void*)_tmp2BA,0U);_tmp2BE->topt=_tmp2B3->topt;{
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BE,_tmp2BF,0U);
_tmp2C0->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2C0);{
# 1753
struct Cyc_List_List*_tmp2C1=({struct Cyc_Absyn_Exp*_tmp2CF[2U];({struct Cyc_Absyn_Exp*_tmp872=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);_tmp2CF[0]=_tmp872;}),({
struct Cyc_Absyn_Exp*_tmp871=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CF[1]=_tmp871;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CF,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp874=({struct Cyc_Absyn_Exp*_tmp873=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2C1,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp873,_tmp2C2,0U);});xsize=_tmp874;});{
# 1760
struct Cyc_Absyn_Exp*_tmp2C3=({struct Cyc_Absyn_Exp*_tmp875=xsize;Cyc_Absyn_and_exp(_tmp875,Cyc_Absyn_and_exp(_tmp2BD,_tmp2C0,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C4=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C5=({struct Cyc_Absyn_Exp*_tmp876=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);Cyc_Toc_member_exp(_tmp876,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp877=Cyc_Toc_cast_it(_tmp2A4,_tmp2C5);_tmp2C5=_tmp877;});{
struct Cyc_Absyn_Exp*_tmp2C6=Cyc_Absyn_deref_exp(_tmp2C5,0U);
struct Cyc_Absyn_Exp*_tmp2C7=({struct Cyc_Absyn_Exp*_tmp878=_tmp2C6;Cyc_Absyn_assign_exp(_tmp878,Cyc_Absyn_var_exp(_tmp2B8,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C8=Cyc_Absyn_exp_stmt(_tmp2C7,0U);
({struct Cyc_Absyn_Stmt*_tmp87C=({struct Cyc_Absyn_Stmt*_tmp87B=({struct Cyc_Absyn_Exp*_tmp87A=_tmp2C3;struct Cyc_Absyn_Stmt*_tmp879=_tmp2C4;Cyc_Absyn_ifthenelse_stmt(_tmp87A,_tmp879,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp87B,_tmp2C8,0U);});_tmp2C8=_tmp87C;});
({struct Cyc_Absyn_Stmt*_tmp87F=({struct Cyc_Absyn_Decl*_tmp87E=({struct Cyc_Absyn_Decl*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));({void*_tmp87D=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->tag=0U,_tmp2C9->f1=_tmp2B9;_tmp2C9;});_tmp2CA->r=_tmp87D;}),_tmp2CA->loc=0U;_tmp2CA;});Cyc_Absyn_decl_stmt(_tmp87E,_tmp2C8,0U);});_tmp2C8=_tmp87F;});
({struct Cyc_Absyn_Stmt*_tmp882=({struct Cyc_Absyn_Decl*_tmp881=({struct Cyc_Absyn_Decl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));({void*_tmp880=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->tag=0U,_tmp2CB->f1=_tmp2B5;_tmp2CB;});_tmp2CC->r=_tmp880;}),_tmp2CC->loc=0U;_tmp2CC;});Cyc_Absyn_decl_stmt(_tmp881,_tmp2C8,0U);});_tmp2C8=_tmp882;});
({struct Cyc_Absyn_Stmt*_tmp885=({struct Cyc_Absyn_Decl*_tmp884=({struct Cyc_Absyn_Decl*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));({void*_tmp883=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->tag=0U,_tmp2CD->f1=_tmp2B0;_tmp2CD;});_tmp2CE->r=_tmp883;}),_tmp2CE->loc=0U;_tmp2CE;});Cyc_Absyn_decl_stmt(_tmp884,_tmp2C8,0U);});_tmp2C8=_tmp885;});
({void*_tmp886=Cyc_Toc_stmt_exp_r(_tmp2C8);e->r=_tmp886;});};};};};};};};};};}
# 1718
;_pop_region(rgn2);};}
# 1786 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1790
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D7=Cyc_Tcutil_compress(aggrtype);void*_tmp2D8=_tmp2D7;union Cyc_Absyn_AggrInfoU _tmp2DD;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D8)->tag == 11U){_LL1: _tmp2DD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D8)->f1).aggr_info;_LL2:
# 1793
({struct Cyc_Absyn_Aggrdecl*_tmp887=Cyc_Absyn_get_known_aggrdecl(_tmp2DD);ad=_tmp887;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp2D9=0U;({struct _dyneither_ptr _tmp88A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DC=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,({
struct _dyneither_ptr _tmp888=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp790.f1=_tmp888;});_tmp790;});void*_tmp2DA[1U];_tmp2DA[0]=& _tmp2DC;({struct _dyneither_ptr _tmp889=({const char*_tmp2DB="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2DB,sizeof(char),51U);});Cyc_aprintf(_tmp889,_tag_dyneither(_tmp2DA,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp88A,_tag_dyneither(_tmp2D9,sizeof(void*),0U));});});}_LL0:;}{
# 1797
struct _tuple1*_tmp2DE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_var_exp(_tmp2DE,0U);
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_aggrarrow_exp(_tmp2DF,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E2=Cyc_Absyn_neq_exp(_tmp2E1,_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2E3=Cyc_Absyn_aggrarrow_exp(_tmp2DF,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E4=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E3,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E5=({struct Cyc_Absyn_Exp*_tmp88C=_tmp2E2;struct Cyc_Absyn_Stmt*_tmp88B=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp88C,_tmp88B,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E6=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E7=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E8=({struct _tuple1*_tmp88F=_tmp2DE;void*_tmp88E=_tmp2E6;struct Cyc_Absyn_Exp*_tmp88D=_tmp2E7;Cyc_Absyn_declare_stmt(_tmp88F,_tmp88E,_tmp88D,Cyc_Absyn_seq_stmt(_tmp2E5,_tmp2E4,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E8);}else{
# 1811
struct Cyc_Absyn_Exp*_tmp2E9=({struct Cyc_Absyn_Exp*_tmp890=aggrproj(_tmp2DF,f,0U);Cyc_Toc_member_exp(_tmp890,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2EA=Cyc_Absyn_neq_exp(_tmp2E9,_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2EB=({struct Cyc_Absyn_Exp*_tmp891=aggrproj(_tmp2DF,f,0U);Cyc_Toc_member_exp(_tmp891,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2EC=Cyc_Absyn_exp_stmt(_tmp2EB,0U);
struct Cyc_Absyn_Stmt*_tmp2ED=({struct Cyc_Absyn_Exp*_tmp893=_tmp2EA;struct Cyc_Absyn_Stmt*_tmp892=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp893,_tmp892,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EE=({struct _tuple1*_tmp896=_tmp2DE;void*_tmp895=e1_c_type;struct Cyc_Absyn_Exp*_tmp894=e1;Cyc_Absyn_declare_stmt(_tmp896,_tmp895,_tmp894,Cyc_Absyn_seq_stmt(_tmp2ED,_tmp2EC,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EE);}};}
# 1821
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1824
void*_tmp2EF=Cyc_Tcutil_compress(t);void*_tmp2F0=_tmp2EF;union Cyc_Absyn_AggrInfoU _tmp2F7;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0)->tag == 11U){_LL1: _tmp2F7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0)->f1).aggr_info;_LL2: {
# 1826
struct Cyc_Absyn_Aggrdecl*_tmp2F1=Cyc_Absyn_get_known_aggrdecl(_tmp2F7);
({int _tmp897=Cyc_Toc_get_member_offset(_tmp2F1,f);*f_tag=_tmp897;});{
# 1829
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F5=({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U,_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2F1->name).f2);_tmp792;});struct Cyc_String_pa_PrintArg_struct _tmp2F6=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp791;});void*_tmp2F3[2U];_tmp2F3[0]=& _tmp2F5,_tmp2F3[1]=& _tmp2F6;({struct _dyneither_ptr _tmp898=({const char*_tmp2F4="_union_%s_%s";_tag_dyneither(_tmp2F4,sizeof(char),13U);});Cyc_aprintf(_tmp898,_tag_dyneither(_tmp2F3,sizeof(void*),2U));});});
({void*_tmp899=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));*_tmp2F2=str;_tmp2F2;}));*tagged_member_type=_tmp899;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2F1->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1840
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1843
void*_tmp2F8=e->r;void*_tmp2F9=_tmp2F8;struct Cyc_Absyn_Exp*_tmp305;struct _dyneither_ptr*_tmp304;int*_tmp303;struct Cyc_Absyn_Exp*_tmp302;struct _dyneither_ptr*_tmp301;int*_tmp300;struct Cyc_Absyn_Exp*_tmp2FF;switch(*((int*)_tmp2F9)){case 14U: _LL1: _tmp2FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_LL2:
({void*_tmp2FA=0U;({struct _dyneither_ptr _tmp89A=({const char*_tmp2FB="cast on lhs!";_tag_dyneither(_tmp2FB,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp89A,_tag_dyneither(_tmp2FA,sizeof(void*),0U));});});case 21U: _LL3: _tmp302=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f1;_tmp301=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_tmp300=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f4;_LL4:
# 1846
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp302->topt),_tmp301,f_tag,tagged_member_type,clear_read,_tmp300);case 22U: _LL5: _tmp305=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f1;_tmp304=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_tmp303=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f4;_LL6: {
# 1849
void*_tmp2FC=Cyc_Tcutil_compress((void*)_check_null(_tmp305->topt));void*_tmp2FD=_tmp2FC;void*_tmp2FE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->tag == 5U){_LLA: _tmp2FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_typ;_LLB:
# 1851
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FE,_tmp304,f_tag,tagged_member_type,clear_read,_tmp303);}else{_LLC: _LLD:
# 1853
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1855
 return 0;}_LL0:;}
# 1868 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1872
struct _tuple1*_tmp306=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp306,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp89B=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp89B;});else{
# 1882
struct Cyc_Absyn_Exp*_tmp307=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp89E=({struct Cyc_Absyn_Exp*_tmp89D=_tmp307;struct Cyc_Absyn_Stmt*_tmp89C=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp89D,_tmp89C,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp89E;});}{
# 1885
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8A1=_tmp306;void*_tmp8A0=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp89F=
Cyc_Toc_push_address_exp(e1);
# 1885
Cyc_Absyn_declare_stmt(_tmp8A1,_tmp8A0,_tmp89F,
# 1887
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1891
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp308=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp30B=({struct Cyc_String_pa_PrintArg_struct _tmp793;_tmp793.tag=0U,({struct _dyneither_ptr _tmp8A2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp793.f1=_tmp8A2;});_tmp793;});void*_tmp309[1U];_tmp309[0]=& _tmp30B;({struct _dyneither_ptr _tmp8A3=({const char*_tmp30A="exp_to_c: no type for %s";_tag_dyneither(_tmp30A,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8A3,_tag_dyneither(_tmp309,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp30C=_tmp308;struct Cyc_Absyn_Stmt*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct _dyneither_ptr*_tmp528;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp526;int _tmp525;struct Cyc_Absyn_Exp*_tmp524;void**_tmp523;struct Cyc_Absyn_Exp*_tmp522;int _tmp521;int _tmp520;struct Cyc_List_List*_tmp51F;struct Cyc_Absyn_Datatypedecl*_tmp51E;struct Cyc_Absyn_Datatypefield*_tmp51D;struct _tuple1**_tmp51C;struct Cyc_List_List*_tmp51B;struct Cyc_List_List*_tmp51A;struct Cyc_Absyn_Aggrdecl*_tmp519;void*_tmp518;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Exp*_tmp516;void*_tmp515;int _tmp514;struct Cyc_Absyn_Vardecl*_tmp513;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp511;int _tmp510;struct Cyc_List_List*_tmp50F;struct Cyc_List_List*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;struct _dyneither_ptr*_tmp50A;int _tmp509;int _tmp508;struct Cyc_Absyn_Exp*_tmp507;struct _dyneither_ptr*_tmp506;int _tmp505;int _tmp504;struct Cyc_Absyn_Exp*_tmp503;void*_tmp502;struct Cyc_List_List*_tmp501;void*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FC;void**_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;int _tmp4F9;enum Cyc_Absyn_Coercion _tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_List_List*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F4;int _tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_List_List*_tmp4F1;int _tmp4F0;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_VarargInfo*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Core_Opt*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;enum Cyc_Absyn_Incrementor _tmp4DE;enum Cyc_Absyn_Primop _tmp4DD;struct Cyc_List_List*_tmp4DC;switch(*((int*)_tmp30C)){case 2U: _LL1: _LL2:
# 1898
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1905
struct Cyc_Absyn_Exp*_tmp30D=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8A4=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30D,_tmp30D))->r;e->r=_tmp8A4;});else{
# 1910
({void*_tmp8A6=({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8A5,({struct Cyc_Absyn_Exp*_tmp30E[3U];_tmp30E[0]=_tmp30D,_tmp30E[1]=_tmp30D,_tmp30E[2]=_tmp30D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30E,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp8A6;});}}else{
# 1912
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1914
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 3U: _LL9: _tmp4DD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4DC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LLA: {
# 1919
struct Cyc_List_List*_tmp30F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DC);
# 1921
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4DC);
{enum Cyc_Absyn_Primop _tmp310=_tmp4DD;switch(_tmp310){case Cyc_Absyn_Numelts: _LL58: _LL59: {
# 1924
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC))->hd;
{void*_tmp311=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp312=_tmp311;void*_tmp324;union Cyc_Absyn_Constraint*_tmp323;union Cyc_Absyn_Constraint*_tmp322;union Cyc_Absyn_Constraint*_tmp321;struct Cyc_Absyn_Exp*_tmp320;switch(*((int*)_tmp312)){case 8U: _LL6D: _tmp320=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312)->f1).num_elts;_LL6E:
# 1928
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp320)))
({void*_tmp313=0U;({unsigned int _tmp8A8=e->loc;struct _dyneither_ptr _tmp8A7=({const char*_tmp314="can't calculate numelts";_tag_dyneither(_tmp314,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8A8,_tmp8A7,_tag_dyneither(_tmp313,sizeof(void*),0U));});});
e->r=_tmp320->r;goto _LL6C;case 5U: _LL6F: _tmp324=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).elt_typ;_tmp323=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).nullable;_tmp322=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).bounds;_tmp321=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).zero_term;_LL70:
# 1932
{void*_tmp315=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp322);void*_tmp316=_tmp315;struct Cyc_Absyn_Exp*_tmp31B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp316)->tag == 0U){_LL74: _LL75:
# 1934
({void*_tmp8AB=({struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp8AA,({struct Cyc_Absyn_Exp*_tmp317[2U];_tmp317[0]=(struct Cyc_Absyn_Exp*)_tmp4DC->hd,({
# 1936
struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp324),0U);_tmp317[1]=_tmp8A9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp317,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1934
e->r=_tmp8AB;});
# 1937
goto _LL73;}else{_LL76: _tmp31B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp316)->f1;_LL77:
# 1940
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp321)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4DC->hd);
# 1944
({void*_tmp8AD=({struct Cyc_Absyn_Exp*_tmp8AC=function_e;Cyc_Toc_fncall_exp_r(_tmp8AC,({struct Cyc_Absyn_Exp*_tmp318[2U];_tmp318[0]=(struct Cyc_Absyn_Exp*)_tmp4DC->hd,_tmp318[1]=_tmp31B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp318,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8AD;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp323)){
if(!Cyc_Evexp_c_can_eval(_tmp31B))
({void*_tmp319=0U;({unsigned int _tmp8AF=e->loc;struct _dyneither_ptr _tmp8AE=({const char*_tmp31A="can't calculate numelts";_tag_dyneither(_tmp31A,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8AF,_tmp8AE,_tag_dyneither(_tmp319,sizeof(void*),0U));});});
# 1949
({void*_tmp8B2=({struct Cyc_Absyn_Exp*_tmp8B1=arg;struct Cyc_Absyn_Exp*_tmp8B0=_tmp31B;Cyc_Toc_conditional_exp_r(_tmp8B1,_tmp8B0,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8B2;});}else{
# 1951
e->r=_tmp31B->r;goto _LL73;}}
# 1953
goto _LL73;}_LL73:;}
# 1955
goto _LL6C;default: _LL71: _LL72:
# 1957
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp795;_tmp795.tag=0U,({
struct _dyneither_ptr _tmp8B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp795.f1=_tmp8B3;});_tmp795;});struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp794;_tmp794.tag=0U,({struct _dyneither_ptr _tmp8B4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp794.f1=_tmp8B4;});_tmp794;});void*_tmp31C[2U];_tmp31C[0]=& _tmp31E,_tmp31C[1]=& _tmp31F;({struct _dyneither_ptr _tmp8B5=({const char*_tmp31D="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp31D,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B5,_tag_dyneither(_tmp31C,sizeof(void*),2U));});});}_LL6C:;}
# 1960
goto _LL57;}case Cyc_Absyn_Plus: _LL5A: _LL5B:
# 1965
{void*_tmp325=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd);void*_tmp326=_tmp325;void*_tmp32E;union Cyc_Absyn_Constraint*_tmp32D;union Cyc_Absyn_Constraint*_tmp32C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->tag == 5U){_LL79: _tmp32E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).elt_typ;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).bounds;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).zero_term;_LL7A:
# 1967
{void*_tmp327=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32D);void*_tmp328=_tmp327;struct Cyc_Absyn_Exp*_tmp32B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp328)->tag == 0U){_LL7E: _LL7F: {
# 1969
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC->tl))->hd;
({void*_tmp8B8=({struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8B7,({struct Cyc_Absyn_Exp*_tmp329[3U];_tmp329[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp32E),0U);_tmp329[1]=_tmp8B6;}),_tmp329[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp329,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1971
e->r=_tmp8B8;});
# 1973
goto _LL7D;}}else{_LL80: _tmp32B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp328)->f1;_LL81:
# 1975
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp32C)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC->tl))->hd;
({void*_tmp8BA=({struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8B9,({struct Cyc_Absyn_Exp*_tmp32A[3U];_tmp32A[0]=e1,_tmp32A[1]=_tmp32B,_tmp32A[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32A,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8BA;});}
# 1980
goto _LL7D;}_LL7D:;}
# 1982
goto _LL78;}else{_LL7B: _LL7C:
 goto _LL78;}_LL78:;}
# 1985
goto _LL57;case Cyc_Absyn_Minus: _LL5C: _LL5D: {
# 1990
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp30F->tl))->hd)){
({void*_tmp8BC=({struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8BB,Cyc_Toc_curr_sp);});e1->r=_tmp8BC;});
({void*_tmp8BE=({struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8BD,Cyc_Toc_curr_sp);});e2->r=_tmp8BE;});
({void*_tmp8C0=({void*_tmp8BF=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmp8BF;});e1->topt=_tmp8C0;});
({void*_tmp8C2=({struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8C1,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 1998
e->r=_tmp8C2;});}else{
# 2001
({void*_tmp8C6=({struct Cyc_Absyn_Exp*_tmp8C5=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8C5,({struct Cyc_Absyn_Exp*_tmp32F[3U];_tmp32F[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8C4=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp32F[1]=_tmp8C4;}),({
struct Cyc_Absyn_Exp*_tmp8C3=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp32F[2]=_tmp8C3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32F,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2001
e->r=_tmp8C6;});}}
# 2006
goto _LL57;}case Cyc_Absyn_Eq: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Neq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Gt: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gte: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lte: _LL68: _LL69: {
# 2014
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DC->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp30F->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp330=({void*_tmp8C7=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8C7,Cyc_Toc_mt_tq);});
({void*_tmp8CA=({void*_tmp8C9=_tmp330;Cyc_Toc_cast_it_r(_tmp8C9,({struct Cyc_Absyn_Exp*_tmp8C8=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8C8,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8CA;});
e1->topt=_tmp330;}
# 2024
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp331=({void*_tmp8CB=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8CB,Cyc_Toc_mt_tq);});
({void*_tmp8CE=({void*_tmp8CD=_tmp331;Cyc_Toc_cast_it_r(_tmp8CD,({struct Cyc_Absyn_Exp*_tmp8CC=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8CC,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8CE;});
e2->topt=_tmp331;}
# 2029
goto _LL57;}default: _LL6A: _LL6B:
 goto _LL57;}_LL57:;}
# 2032
goto _LL0;}case 5U: _LLB: _tmp4DF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4DE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LLC: {
# 2034
void*e2_cyc_typ=(void*)_check_null(_tmp4DF->topt);
# 2043 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp34B="increment";_tag_dyneither(_tmp34B,sizeof(char),10U);});
if(_tmp4DE == Cyc_Absyn_PreDec  || _tmp4DE == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8CF=({const char*_tmp332="decrement";_tag_dyneither(_tmp332,sizeof(char),10U);});incr_str=_tmp8CF;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4DF,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp335=({struct Cyc_String_pa_PrintArg_struct _tmp796;_tmp796.tag=0U,_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp796;});void*_tmp333[1U];_tmp333[0]=& _tmp335;({unsigned int _tmp8D1=e->loc;struct _dyneither_ptr _tmp8D0=({const char*_tmp334="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp334,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8D1,_tmp8D0,_tag_dyneither(_tmp333,sizeof(void*),1U));});});
({void*_tmp336=0U;({struct _dyneither_ptr _tmp8D2=({const char*_tmp337="in-place inc/dec on zero-term";_tag_dyneither(_tmp337,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D2,_tag_dyneither(_tmp336,sizeof(void*),0U));});});}{
# 2052
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4DF,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp338=Cyc_Absyn_signed_int_exp(1,0U);
_tmp338->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp339=_tmp4DE;switch(_tmp339){case Cyc_Absyn_PreInc: _LL83: _LL84:
# 2059
({void*_tmp8D4=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->tag=4U,_tmp33B->f1=_tmp4DF,({struct Cyc_Core_Opt*_tmp8D3=({struct Cyc_Core_Opt*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A->v=(void*)Cyc_Absyn_Plus;_tmp33A;});_tmp33B->f2=_tmp8D3;}),_tmp33B->f3=_tmp338;_tmp33B;});e->r=_tmp8D4;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL85: _LL86:
# 2063
({void*_tmp8D6=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->tag=4U,_tmp33D->f1=_tmp4DF,({struct Cyc_Core_Opt*_tmp8D5=({struct Cyc_Core_Opt*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->v=(void*)Cyc_Absyn_Minus;_tmp33C;});_tmp33D->f2=_tmp8D5;}),_tmp33D->f3=_tmp338;_tmp33D;});e->r=_tmp8D6;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL87: _LL88:
# 2067
({struct Cyc_String_pa_PrintArg_struct _tmp340=({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U,_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp797;});void*_tmp33E[1U];_tmp33E[0]=& _tmp340;({unsigned int _tmp8D8=e->loc;struct _dyneither_ptr _tmp8D7=({const char*_tmp33F="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp33F,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8D8,_tmp8D7,_tag_dyneither(_tmp33E,sizeof(void*),1U));});});
# 2069
({void*_tmp341=0U;({struct _dyneither_ptr _tmp8D9=({const char*_tmp342="in-place inc/dec on @tagged union";_tag_dyneither(_tmp342,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D9,_tag_dyneither(_tmp341,sizeof(void*),0U));});});}_LL82:;};}{
# 2072
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4DF);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4DF);
Cyc_Toc_set_lhs(nv,0);{
# 2079
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4DE == Cyc_Absyn_PostInc  || _tmp4DE == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2085
if(_tmp4DE == Cyc_Absyn_PreDec  || _tmp4DE == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8DE=({struct Cyc_Absyn_Exp*_tmp8DD=fn_e;Cyc_Toc_fncall_exp_r(_tmp8DD,({struct Cyc_Absyn_Exp*_tmp343[3U];({struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Toc_push_address_exp(_tmp4DF);_tmp343[0]=_tmp8DC;}),({
struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp343[1]=_tmp8DB;}),({
struct Cyc_Absyn_Exp*_tmp8DA=Cyc_Absyn_signed_int_exp(change,0U);_tmp343[2]=_tmp8DA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp343,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2087
e->r=_tmp8DE;});}else{
# 2090
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2096
if(_tmp4DE != Cyc_Absyn_PostInc){
struct _tuple1*_tmp344=Cyc_Toc_temp_var();
void*_tmp345=({void*_tmp8DF=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmp8DF,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp346=Cyc_Toc_push_address_exp(_tmp4DF);
struct Cyc_Absyn_Exp*_tmp347=({struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp344,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8E0,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp8E3=({struct Cyc_Absyn_Exp*_tmp8E2=({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp344,0U),0U);Cyc_Absyn_neq_exp(_tmp8E1,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2103
Cyc_Absyn_and_exp(_tmp8E2,_tmp347,0U);});_tmp347=_tmp8E3;});{
# 2105
struct Cyc_Absyn_Stmt*_tmp348=({struct Cyc_Absyn_Exp*_tmp8E5=_tmp347;struct Cyc_Absyn_Stmt*_tmp8E4=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp344,0U),0U),0U),0U);
# 2105
Cyc_Absyn_ifthenelse_stmt(_tmp8E5,_tmp8E4,
# 2107
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8E7=({struct Cyc_Absyn_Stmt*_tmp8E6=_tmp348;Cyc_Absyn_seq_stmt(_tmp8E6,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp344,0U),0U),0U),0U);});_tmp348=_tmp8E7;});
({void*_tmp8E8=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp344,_tmp345,_tmp346,_tmp348,0U),0U))->r;e->r=_tmp8E8;});};}else{
# 2112
struct Cyc_Toc_functionSet*_tmp349=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp349,_tmp4DF);
({void*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp8EB=fn_e;Cyc_Toc_fncall_exp_r(_tmp8EB,({struct Cyc_Absyn_Exp*_tmp34A[2U];({struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Toc_push_address_exp(_tmp4DF);_tmp34A[0]=_tmp8EA;}),({
struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Absyn_signed_int_exp(1,0U);_tmp34A[1]=_tmp8E9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34A,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2114
e->r=_tmp8EC;});}}else{
# 2117
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4DF)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4DF,0,Cyc_Toc_incr_lvalue,_tmp4DE);
e->r=_tmp4DF->r;}}}
# 2121
goto _LL0;};};};}case 4U: _LLD: _tmp4E2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4E0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LLE: {
# 2140 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4E2->topt);
void*e2_old_typ=(void*)_check_null(_tmp4E0->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4E2,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E0);
({void*_tmp8ED=Cyc_Toc_tagged_union_assignop(_tmp4E2,e1_old_typ,_tmp4E1,_tmp4E0,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8ED;});
# 2151
return;}{
# 2153
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E2,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4E2,_tmp4E1,_tmp4E0,ptr_type,is_dyneither,elt_type);
# 2159
return;}{
# 2163
int e1_poly=Cyc_Toc_is_poly_project(_tmp4E2);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E0);{
# 2169
int done=0;
if(_tmp4E1 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp34C=(enum Cyc_Absyn_Primop)_tmp4E1->v;enum Cyc_Absyn_Primop _tmp34D=_tmp34C;switch(_tmp34D){case Cyc_Absyn_Plus: _LL8A: _LL8B:
 change=_tmp4E0;goto _LL89;case Cyc_Absyn_Minus: _LL8C: _LL8D:
# 2177
({struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4E0,0U);change=_tmp8EE;});goto _LL89;default: _LL8E: _LL8F:
({void*_tmp34E=0U;({struct _dyneither_ptr _tmp8EF=({const char*_tmp34F="bad t ? pointer arithmetic";_tag_dyneither(_tmp34F,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8EF,_tag_dyneither(_tmp34E,sizeof(void*),0U));});});}_LL89:;}
# 2180
done=1;{
# 2182
struct Cyc_Absyn_Exp*_tmp350=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8F3=({struct Cyc_Absyn_Exp*_tmp8F2=_tmp350;Cyc_Toc_fncall_exp_r(_tmp8F2,({struct Cyc_Absyn_Exp*_tmp351[3U];({struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Toc_push_address_exp(_tmp4E2);_tmp351[0]=_tmp8F1;}),({
struct Cyc_Absyn_Exp*_tmp8F0=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp351[1]=_tmp8F0;}),_tmp351[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp351,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2183
e->r=_tmp8F3;});};}else{
# 2186
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2189
enum Cyc_Absyn_Primop _tmp352=(enum Cyc_Absyn_Primop)_tmp4E1->v;enum Cyc_Absyn_Primop _tmp353=_tmp352;if(_tmp353 == Cyc_Absyn_Plus){_LL91: _LL92:
# 2191
 done=1;
({void*_tmp8F5=({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4E2);Cyc_Toc_fncall_exp_r(_tmp8F4,({struct Cyc_Absyn_Exp*_tmp354[2U];_tmp354[0]=_tmp4E2,_tmp354[1]=_tmp4E0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp354,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8F5;});
goto _LL90;}else{_LL93: _LL94:
({void*_tmp355=0U;({struct _dyneither_ptr _tmp8F6=({const char*_tmp356="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp356,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F6,_tag_dyneither(_tmp355,sizeof(void*),0U));});});}_LL90:;}}}
# 2198
if(!done){
# 2200
if(e1_poly)
({void*_tmp8F8=({void*_tmp8F7=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp8F7,Cyc_Absyn_new_exp(_tmp4E0->r,0U));});_tmp4E0->r=_tmp8F8;});
# 2206
if(!Cyc_Absyn_is_lvalue(_tmp4E2)){
({struct Cyc_Absyn_Exp*_tmp8F9=_tmp4E2;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8F9,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp358=1;struct _tuple31*_tmp357=_cycalloc(_check_times(_tmp358,sizeof(struct _tuple31)));(_tmp357[0]).f1=_tmp4E1,(_tmp357[0]).f2=_tmp4E0;_tmp357;}));});
e->r=_tmp4E2->r;}}
# 2211
goto _LL0;};};};}case 6U: _LLF: _tmp4E5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4E3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL10:
# 2213
 Cyc_Toc_exp_to_c(nv,_tmp4E5);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_exp_to_c(nv,_tmp4E3);
goto _LL0;case 7U: _LL11: _tmp4E7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL12:
 _tmp4E9=_tmp4E7;_tmp4E8=_tmp4E6;goto _LL14;case 8U: _LL13: _tmp4E9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL14:
 _tmp4EB=_tmp4E9;_tmp4EA=_tmp4E8;goto _LL16;case 9U: _LL15: _tmp4EB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL16:
# 2220
 Cyc_Toc_exp_to_c(nv,_tmp4EB);
Cyc_Toc_exp_to_c(nv,_tmp4EA);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3 == 0){_LL17: _tmp4ED=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL18:
# 2224
 Cyc_Toc_exp_to_c(nv,_tmp4ED);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4EC);
goto _LL0;}else{_LL19: _tmp4F2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4F0=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->num_varargs;_tmp4EF=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->injectors;_tmp4EE=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->vai;_LL1A: {
# 2236 "toc.cyc"
struct _RegionHandle _tmp359=_new_region("r");struct _RegionHandle*r=& _tmp359;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4F0,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4EE->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2244
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F1);
int num_normargs=num_args - _tmp4F0;
# 2248
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4F1=_tmp4F1->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F1))->hd);
({struct Cyc_List_List*_tmp8FA=({struct Cyc_List_List*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->hd=(struct Cyc_Absyn_Exp*)_tmp4F1->hd,_tmp35A->tl=new_args;_tmp35A;});new_args=_tmp8FA;});}}
# 2253
({struct Cyc_List_List*_tmp8FE=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));({struct Cyc_Absyn_Exp*_tmp8FD=({struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8FC,({struct Cyc_Absyn_Exp*_tmp35B[3U];_tmp35B[0]=argvexp,({
# 2255
struct Cyc_Absyn_Exp*_tmp8FB=Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp35B[1]=_tmp8FB;}),_tmp35B[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2253
_tmp35C->hd=_tmp8FD;}),_tmp35C->tl=new_args;_tmp35C;});new_args=_tmp8FE;});
# 2258
({struct Cyc_List_List*_tmp8FF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8FF;});
# 2260
Cyc_Toc_exp_to_c(nv,_tmp4F2);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4F2,new_args,0U),0U);
# 2264
if(_tmp4EE->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp35D=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4EE->type));void*_tmp35E=_tmp35D;struct Cyc_Absyn_Datatypedecl*_tmp361;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35E)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35E)->f1).datatype_info).KnownDatatype).tag == 2){_LL96: _tmp361=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35E)->f1).datatype_info).KnownDatatype).val;_LL97:
 tud=_tmp361;goto _LL95;}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp35F=0U;({struct _dyneither_ptr _tmp900=({const char*_tmp360="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp360,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp900,_tag_dyneither(_tmp35F,sizeof(void*),0U));});});}_LL95:;}{
# 2270
struct _dyneither_ptr vs=({unsigned int _tmp372=(unsigned int)_tmp4F0;struct _tuple1**_tmp371=({struct _RegionHandle*_tmp901=r;_region_malloc(_tmp901,_check_times(_tmp372,sizeof(struct _tuple1*)));});({{unsigned int _tmp798=(unsigned int)_tmp4F0;unsigned int i;for(i=0;i < _tmp798;++ i){({struct _tuple1*_tmp902=Cyc_Toc_temp_var();_tmp371[i]=_tmp902;});}}0;});_tag_dyneither(_tmp371,sizeof(struct _tuple1*),_tmp372);});
# 2272
if(_tmp4F0 != 0){
# 2276
struct Cyc_List_List*_tmp362=0;
{int i=_tmp4F0 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp904=({struct Cyc_List_List*_tmp363=_cycalloc(sizeof(*_tmp363));({struct Cyc_Absyn_Exp*_tmp903=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp363->hd=_tmp903;}),_tmp363->tl=_tmp362;_tmp363;});_tmp362=_tmp904;});}}
({struct Cyc_Absyn_Stmt*_tmp908=({struct _tuple1*_tmp907=argv;void*_tmp906=arr_type;struct Cyc_Absyn_Exp*_tmp905=Cyc_Absyn_array_exp(_tmp362,0U);Cyc_Absyn_declare_stmt(_tmp907,_tmp906,_tmp905,s,0U);});s=_tmp908;});
# 2281
({struct Cyc_List_List*_tmp909=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F1);_tmp4F1=_tmp909;});
({struct Cyc_List_List*_tmp90A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4EF);_tmp4EF=_tmp90A;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F1)- 1;
for(0;_tmp4F1 != 0;(_tmp4F1=_tmp4F1->tl,_tmp4EF=_tmp4EF->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F1->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp364=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4EF))->hd;struct Cyc_Absyn_Datatypefield*_tmp365=_tmp364;struct _tuple1*_tmp36B;struct Cyc_List_List*_tmp36A;_LL9B: _tmp36B=_tmp365->name;_tmp36A=_tmp365->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp36A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp90B=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp90B;});{
# 2295
struct _tuple1*_tmp366=Cyc_Toc_collapse_qvars(_tmp36B,tud->name);
struct Cyc_List_List*_tmp367=({struct _tuple20*_tmp369[2U];({struct _tuple20*_tmp90F=({struct _dyneither_ptr*_tmp90E=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp90E,Cyc_Toc_datatype_tag(tud,_tmp36B));});_tmp369[0]=_tmp90F;}),({
struct _tuple20*_tmp90D=({struct _dyneither_ptr*_tmp90C=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp90C,arg);});_tmp369[1]=_tmp90D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp369,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp913=({struct _tuple1*_tmp912=var;void*_tmp911=Cyc_Absyn_strctq(_tmp366);struct Cyc_Absyn_Exp*_tmp910=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->tag=29U,_tmp368->f1=_tmp366,_tmp368->f2=0,_tmp368->f3=_tmp367,_tmp368->f4=0;_tmp368;}),0U);
# 2298
Cyc_Absyn_declare_stmt(_tmp912,_tmp911,_tmp910,s,0U);});s=_tmp913;});};};}};}else{
# 2306
struct Cyc_List_List*_tmp36C=({struct _tuple32*_tmp36D[3U];({struct _tuple32*_tmp91C=({struct _tuple32*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct _dyneither_ptr _tmp91B=_tag_dyneither(0,0,0);_tmp36E->f1=_tmp91B;}),({struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_uint_exp(0U,0U);_tmp36E->f2=_tmp91A;});_tmp36E;});_tmp36D[0]=_tmp91C;}),({
struct _tuple32*_tmp919=({struct _tuple32*_tmp36F=_cycalloc(sizeof(*_tmp36F));({struct _dyneither_ptr _tmp918=_tag_dyneither(0,0,0);_tmp36F->f1=_tmp918;}),({struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_uint_exp(0U,0U);_tmp36F->f2=_tmp917;});_tmp36F;});_tmp36D[1]=_tmp919;}),({
struct _tuple32*_tmp916=({struct _tuple32*_tmp370=_cycalloc(sizeof(*_tmp370));({struct _dyneither_ptr _tmp915=_tag_dyneither(0,0,0);_tmp370->f1=_tmp915;}),({struct Cyc_Absyn_Exp*_tmp914=Cyc_Absyn_uint_exp(0U,0U);_tmp370->f2=_tmp914;});_tmp370;});_tmp36D[2]=_tmp916;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36D,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp920=({struct _tuple1*_tmp91F=argv;void*_tmp91E=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp91D=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp91F,_tmp91E,_tmp91D,s,0U);});s=_tmp920;});}};}else{
# 2314
if(_tmp4F0 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4F1 != 0;_tmp4F1=_tmp4F1->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F1->hd);
({struct Cyc_List_List*_tmp921=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->hd=(struct Cyc_Absyn_Exp*)_tmp4F1->hd,_tmp373->tl=array_args;_tmp373;});array_args=_tmp921;});}{
# 2320
struct Cyc_Absyn_Exp*_tmp374=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp922=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp374,s,0U);s=_tmp922;});};}else{
# 2323
({struct Cyc_Absyn_Stmt*_tmp926=({struct _tuple1*_tmp925=argv;void*_tmp924=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp923=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp925,_tmp924,_tmp923,s,0U);});s=_tmp926;});}}
# 2326
({void*_tmp927=Cyc_Toc_stmt_exp_r(s);e->r=_tmp927;});};}
# 2328
_npop_handler(0U);goto _LL0;
# 2236
;_pop_region(r);}}case 11U: _LL1B: _tmp4F4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL1C:
# 2331
 Cyc_Toc_exp_to_c(nv,_tmp4F4);{
struct Cyc_Absyn_Exp*_tmp375=_tmp4F3?Cyc_Toc_newrethrow_exp(_tmp4F4): Cyc_Toc_newthrow_exp(_tmp4F4);
({void*_tmp929=({void*_tmp928=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp928,_tmp375,0U);})->r;e->r=_tmp929;});
goto _LL0;};case 12U: _LL1D: _tmp4F5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL1E:
 Cyc_Toc_exp_to_c(nv,_tmp4F5);goto _LL0;case 13U: _LL1F: _tmp4F7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL20:
# 2337
 Cyc_Toc_exp_to_c(nv,_tmp4F7);
# 2346 "toc.cyc"
for(0;_tmp4F6 != 0;_tmp4F6=_tmp4F6->tl){
enum Cyc_Absyn_KindQual _tmp376=(Cyc_Tcutil_typ_kind((void*)_tmp4F6->hd))->kind;
if(_tmp376 != Cyc_Absyn_EffKind  && _tmp376 != Cyc_Absyn_RgnKind){
{void*_tmp377=Cyc_Tcutil_compress((void*)_tmp4F6->hd);void*_tmp378=_tmp377;switch(*((int*)_tmp378)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 3U: _LLA0: _LLA1:
 continue;default: _LLA2: _LLA3:
# 2353
({void*_tmp92B=({void*_tmp92A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp92A,_tmp4F7,0U);})->r;e->r=_tmp92B;});
goto _LL9D;}_LL9D:;}
# 2356
break;}}
# 2359
goto _LL0;case 14U: _LL21: _tmp4FB=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4FA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4F9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp4F8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL22: {
# 2361
void*old_t2=(void*)_check_null(_tmp4FA->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4FB;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4FB=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4FA);
# 2368
{struct _tuple33 _tmp379=({struct _tuple33 _tmp79C;({void*_tmp92D=Cyc_Tcutil_compress(old_t2);_tmp79C.f1=_tmp92D;}),({void*_tmp92C=Cyc_Tcutil_compress(new_typ);_tmp79C.f2=_tmp92C;});_tmp79C;});struct _tuple33 _tmp37A=_tmp379;struct Cyc_Absyn_PtrInfo _tmp3B8;struct Cyc_Absyn_PtrInfo _tmp3B7;struct Cyc_Absyn_PtrInfo _tmp3B6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37A.f1)->tag == 5U)switch(*((int*)_tmp37A.f2)){case 5U: _LLA5: _tmp3B7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37A.f1)->f1;_tmp3B6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37A.f2)->f1;_LLA6: {
# 2370
int _tmp37B=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B7.ptr_atts).nullable);
int _tmp37C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B6.ptr_atts).nullable);
void*_tmp37D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B7.ptr_atts).bounds);
void*_tmp37E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B6.ptr_atts).bounds);
int _tmp37F=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B7.ptr_atts).zero_term);
int _tmp380=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B6.ptr_atts).zero_term);
{struct _tuple33 _tmp381=({struct _tuple33 _tmp79B;_tmp79B.f1=_tmp37D,_tmp79B.f2=_tmp37E;_tmp79B;});struct _tuple33 _tmp382=_tmp381;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B0;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f2)->tag == 1U){_LLAC: _tmp3B1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f1)->f1;_tmp3B0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f2)->f1;_LLAD:
# 2378
 if((!Cyc_Evexp_c_can_eval(_tmp3B1) || !Cyc_Evexp_c_can_eval(_tmp3B0)) && !
Cyc_Evexp_same_const_exp(_tmp3B1,_tmp3B0))
({void*_tmp383=0U;({unsigned int _tmp92F=e->loc;struct _dyneither_ptr _tmp92E=({const char*_tmp384="can't validate cast due to potential size differences";_tag_dyneither(_tmp384,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp92F,_tmp92E,_tag_dyneither(_tmp383,sizeof(void*),0U));});});
if(_tmp37B  && !_tmp37C){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp385=0U;({struct _dyneither_ptr _tmp930=({const char*_tmp386="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp386,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp930,_tag_dyneither(_tmp385,sizeof(void*),0U));});});
# 2387
if(_tmp4F8 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp389=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,({struct _dyneither_ptr _tmp931=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp799.f1=_tmp931;});_tmp799;});void*_tmp387[1U];_tmp387[0]=& _tmp389;({struct _dyneither_ptr _tmp932=({const char*_tmp388="null-check conversion mis-classified: %s";_tag_dyneither(_tmp388,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp932,_tag_dyneither(_tmp387,sizeof(void*),1U));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp4FA);
if(do_null_check){
if(!_tmp4F9)
({void*_tmp38A=0U;({unsigned int _tmp934=e->loc;struct _dyneither_ptr _tmp933=({const char*_tmp38B="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp38B,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp934,_tmp933,_tag_dyneither(_tmp38A,sizeof(void*),0U));});});
# 2394
({void*_tmp937=({void*_tmp936=*_tmp4FB;Cyc_Toc_cast_it_r(_tmp936,({struct Cyc_Absyn_Exp*_tmp935=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp935,({struct Cyc_List_List*_tmp38C=_cycalloc(sizeof(*_tmp38C));
_tmp38C->hd=_tmp4FA,_tmp38C->tl=0;_tmp38C;}),0U);}));});
# 2394
e->r=_tmp937;});}else{
# 2398
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FA->r;}};}else{
# 2403
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FA->r;}
# 2410
goto _LLAB;}else{_LLAE: _tmp3B2=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f1)->f1;_LLAF:
# 2412
 if(!Cyc_Evexp_c_can_eval(_tmp3B2))
({void*_tmp38D=0U;({unsigned int _tmp939=e->loc;struct _dyneither_ptr _tmp938=({const char*_tmp38E="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp38E,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp939,_tmp938,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});{
struct _tuple13 _tmp38F=Cyc_Evexp_eval_const_uint_exp(_tmp3B2);struct _tuple13 _tmp390=_tmp38F;unsigned int _tmp3A2;int _tmp3A1;_LLB5: _tmp3A2=_tmp390.f1;_tmp3A1=_tmp390.f2;_LLB6:;
# 2416
if(_tmp4F8 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,({struct _dyneither_ptr _tmp93A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79A.f1=_tmp93A;});_tmp79A;});void*_tmp391[1U];_tmp391[0]=& _tmp393;({struct _dyneither_ptr _tmp93B=({const char*_tmp392="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp392,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp93B,_tag_dyneither(_tmp391,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2420
if((_tmp37F  && !(_tmp3B6.elt_tq).real_const) && !_tmp380)
# 2423
({struct Cyc_Absyn_Exp*_tmp93D=({struct Cyc_Absyn_Exp*_tmp93C=_tmp3B2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp93C,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3B2=_tmp93D;});
# 2425
({void*_tmp93E=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3B2,_tmp4FA))->r;e->r=_tmp93E;});}else{
# 2427
struct Cyc_Absyn_Exp*_tmp394=Cyc_Toc__tag_dyneither_e;
# 2429
if(_tmp37F){
# 2434
struct _tuple1*_tmp395=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp396=Cyc_Absyn_var_exp(_tmp395,0U);
struct Cyc_Absyn_Exp*arg3;
# 2439
{void*_tmp397=_tmp4FA->r;void*_tmp398=_tmp397;struct Cyc_Absyn_Vardecl*_tmp39B;struct Cyc_Absyn_Vardecl*_tmp39A;switch(*((int*)_tmp398)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp398)->f1).Wstring_c).tag){case 8U: _LLB8: _LLB9:
# 2441
 arg3=_tmp3B2;goto _LLB7;case 9U: _LLBA: _LLBB:
# 2443
 arg3=_tmp3B2;goto _LLB7;default: goto _LLC0;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp398)->f1)){case 1U: _LLBC: _tmp39A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp398)->f1)->f1;_LLBD:
 _tmp39B=_tmp39A;goto _LLBF;case 4U: _LLBE: _tmp39B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp398)->f1)->f1;_LLBF:
# 2446
 if(!Cyc_Tcutil_is_array(_tmp39B->type))goto _LLC1;
arg3=_tmp3B2;
goto _LLB7;default: goto _LLC0;}default: _LLC0: _LLC1:
# 2452
 if(_tmp3A1  && _tmp3A2 != 1)
arg3=_tmp3B2;else{
# 2455
({struct Cyc_Absyn_Exp*_tmp942=({struct Cyc_Absyn_Exp*_tmp941=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4FA);Cyc_Absyn_fncall_exp(_tmp941,({struct Cyc_Absyn_Exp*_tmp399[2U];({
# 2457
struct Cyc_Absyn_Exp*_tmp940=({void*_tmp93F=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp93F,_tmp396);});_tmp399[0]=_tmp940;}),_tmp399[1]=_tmp3B2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp399,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2455
arg3=_tmp942;});}
# 2458
goto _LLB7;}_LLB7:;}
# 2460
if(!_tmp380  && !(_tmp3B6.elt_tq).real_const)
# 2463
({struct Cyc_Absyn_Exp*_tmp944=({struct Cyc_Absyn_Exp*_tmp943=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp943,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp944;});{
# 2465
struct Cyc_Absyn_Exp*_tmp39C=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B6.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp39D=({struct Cyc_Absyn_Exp*_tmp945=_tmp394;Cyc_Absyn_fncall_exp(_tmp945,({struct Cyc_Absyn_Exp*_tmp39F[3U];_tmp39F[0]=_tmp396,_tmp39F[1]=_tmp39C,_tmp39F[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp39E=Cyc_Absyn_exp_stmt(_tmp39D,0U);
({struct Cyc_Absyn_Stmt*_tmp949=({struct _tuple1*_tmp948=_tmp395;void*_tmp947=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp946=_tmp4FA;Cyc_Absyn_declare_stmt(_tmp948,_tmp947,_tmp946,_tmp39E,0U);});_tmp39E=_tmp949;});
({void*_tmp94A=Cyc_Toc_stmt_exp_r(_tmp39E);e->r=_tmp94A;});};}else{
# 2472
({void*_tmp94D=({struct Cyc_Absyn_Exp*_tmp94C=_tmp394;Cyc_Toc_fncall_exp_r(_tmp94C,({struct Cyc_Absyn_Exp*_tmp3A0[3U];_tmp3A0[0]=_tmp4FA,({
# 2474
struct Cyc_Absyn_Exp*_tmp94B=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B6.elt_typ),0U);_tmp3A0[1]=_tmp94B;}),_tmp3A0[2]=_tmp3B2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A0,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2472
e->r=_tmp94D;});}}
# 2478
goto _LLAB;};}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f2)->tag == 1U){_LLB0: _tmp3B3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp382.f2)->f1;_LLB1:
# 2480
 if(!Cyc_Evexp_c_can_eval(_tmp3B3))
({void*_tmp3A3=0U;({unsigned int _tmp94F=e->loc;struct _dyneither_ptr _tmp94E=({const char*_tmp3A4="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp3A4,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp94F,_tmp94E,_tag_dyneither(_tmp3A3,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A5=0U;({struct _dyneither_ptr _tmp950=({const char*_tmp3A6="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp3A6,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp950,_tag_dyneither(_tmp3A5,sizeof(void*),0U));});});{
# 2492 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3A7=_tmp3B3;
if(_tmp37F  && !_tmp380)
({struct Cyc_Absyn_Exp*_tmp952=({struct Cyc_Absyn_Exp*_tmp951=_tmp3B3;Cyc_Absyn_add_exp(_tmp951,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3A7=_tmp952;});{
# 2499
struct Cyc_Absyn_Exp*_tmp3A8=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp3A9=({struct Cyc_Absyn_Exp*_tmp954=_tmp3A8;Cyc_Absyn_fncall_exp(_tmp954,({struct Cyc_Absyn_Exp*_tmp3AB[3U];_tmp3AB[0]=_tmp4FA,({
# 2502
struct Cyc_Absyn_Exp*_tmp953=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B7.elt_typ),0U);_tmp3AB[1]=_tmp953;}),_tmp3AB[2]=_tmp3A7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AB,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2504
if(!_tmp37C)
({void*_tmp957=({struct Cyc_Absyn_Exp*_tmp956=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp956,({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));
({struct Cyc_Absyn_Exp*_tmp955=Cyc_Absyn_copy_exp(_tmp3A9);_tmp3AA->hd=_tmp955;}),_tmp3AA->tl=0;_tmp3AA;}));});
# 2505
_tmp3A9->r=_tmp957;});
# 2507
({void*_tmp958=Cyc_Toc_cast_it_r(*_tmp4FB,_tmp3A9);e->r=_tmp958;});
goto _LLAB;};};}else{_LLB2: _LLB3:
# 2512
 DynCast:
 if((_tmp37F  && !_tmp380) && !(_tmp3B6.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3AC=0U;({struct _dyneither_ptr _tmp959=({const char*_tmp3AD="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3AD,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp959,_tag_dyneither(_tmp3AC,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3AE=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp95D=({struct Cyc_Absyn_Exp*_tmp95C=_tmp3AE;Cyc_Toc_fncall_exp_r(_tmp95C,({struct Cyc_Absyn_Exp*_tmp3AF[3U];_tmp3AF[0]=_tmp4FA,({
struct Cyc_Absyn_Exp*_tmp95B=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B7.elt_typ),0U);_tmp3AF[1]=_tmp95B;}),({
struct Cyc_Absyn_Exp*_tmp95A=Cyc_Absyn_uint_exp(1U,0U);_tmp3AF[2]=_tmp95A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AF,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2517
e->r=_tmp95D;});};}
# 2521
goto _LLAB;}}_LLAB:;}
# 2523
goto _LLA4;}case 6U: _LLA7: _tmp3B8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37A.f1)->f1;_LLA8:
# 2525
{void*_tmp3B4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B8.ptr_atts).bounds);void*_tmp3B5=_tmp3B4;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B5)->tag == 0U){_LLC3: _LLC4:
# 2527
({void*_tmp95F=({struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_new_exp(_tmp4FA->r,_tmp4FA->loc);Cyc_Toc_aggrmember_exp_r(_tmp95E,Cyc_Toc_curr_sp);});_tmp4FA->r=_tmp95F;});
_tmp4FA->topt=new_typ_c;
goto _LLC2;}else{_LLC5: _LLC6:
 goto _LLC2;}_LLC2:;}
# 2532
goto _LLA4;default: goto _LLA9;}else{_LLA9: _LLAA:
# 2534
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FA->r;
goto _LLA4;}_LLA4:;}
# 2538
goto _LL0;}case 15U: _LL23: _tmp4FC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL24:
# 2541
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FC);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4FC)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4FC,0,Cyc_Toc_address_lvalue,1);
# 2547
({void*_tmp961=({void*_tmp960=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp960,_tmp4FC);});e->r=_tmp961;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp4FC->topt))))
# 2551
({void*_tmp963=({void*_tmp962=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp962,_tmp4FC);});e->r=_tmp963;});}
# 2553
goto _LL0;case 16U: _LL25: _tmp4FE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4FD=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL26:
# 2560
 Cyc_Toc_exp_to_c(nv,_tmp4FD);{
# 2562
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4FD->topt));
struct _tuple1*_tmp3B9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3BA=Cyc_Absyn_var_exp(_tmp3B9,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3BB=Cyc_Tcutil_compress(elt_typ);void*_tmp3BC=_tmp3BB;void*_tmp3BD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BC)->tag == 8U){_LLC8: _tmp3BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BC)->f1).elt_type;_LLC9:
# 2568
({void*_tmp964=Cyc_Toc_typ_to_c(_tmp3BD);elt_typ=_tmp964;});
({struct Cyc_Absyn_Exp*_tmp965=Cyc_Absyn_copy_exp(_tmp3BA);lhs=_tmp965;});
goto _LLC7;}else{_LLCA: _LLCB:
# 2572
({struct Cyc_Absyn_Exp*_tmp966=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3BA),0U);lhs=_tmp966;});
goto _LLC7;}_LLC7:;}{
# 2575
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp4FD);
struct _tuple1*_tmp3BE=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3BF=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3BE),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp969=({struct Cyc_Absyn_Exp*_tmp968=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp968,({struct Cyc_Absyn_Exp*_tmp3C0[2U];_tmp3C0[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3BF),({
struct Cyc_Absyn_Exp*_tmp967=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3C0[1]=_tmp967;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C0,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2580
mexp=_tmp969;});else{
# 2583
({struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3BA,0U),0U);mexp=_tmp96A;});}{
# 2586
struct Cyc_Absyn_Exp*_tmp3C1=Cyc_Toc_get_varsizeexp(_tmp4FD);
if(_tmp3C1 != 0)
({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_add_exp(mexp,_tmp3C1,0U);mexp=_tmp96B;});
# 2590
if(_tmp4FE == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp96C=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp96C;});else{
# 2593
struct Cyc_Absyn_Exp*r=_tmp4FE;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp96D;});}{
# 2597
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp972=({struct Cyc_Absyn_Exp*_tmp971=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp971,({struct Cyc_Absyn_Exp*_tmp3C2[3U];({
struct Cyc_Absyn_Exp*_tmp970=Cyc_Absyn_copy_exp(_tmp3BA);_tmp3C2[0]=_tmp970;}),({
struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3C2[1]=_tmp96F;}),
(unsigned int)_tmp3BF?_tmp3C2[2]=_tmp3BF:({struct Cyc_Absyn_Exp*_tmp96E=Cyc_Absyn_uint_exp(1U,0U);_tmp3C2[2]=_tmp96E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C2,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2599
answer=_tmp972;});else{
# 2604
({struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_copy_exp(_tmp3BA);answer=_tmp973;});}
({void*_tmp974=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->tag=Cyc_Toc_Dest,_tmp3C3->f1=_tmp3BA;_tmp3C3;});e->annot=_tmp974;});{
struct Cyc_Absyn_Stmt*_tmp3C4=({struct _tuple1*_tmp978=_tmp3B9;void*_tmp977=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp976=mexp;Cyc_Absyn_declare_stmt(_tmp978,_tmp977,_tmp976,({
struct Cyc_Absyn_Stmt*_tmp975=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp975,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2610
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp979=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3BE),Cyc_Absyn_uint_typ,array_len,_tmp3C4,0U);_tmp3C4=_tmp979;});
({void*_tmp97A=Cyc_Toc_stmt_exp_r(_tmp3C4);e->r=_tmp97A;});
if(_tmp3C1 != 0)
({void*_tmp97C=({void*_tmp97B=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp97B,Cyc_Absyn_copy_exp(e));});e->r=_tmp97C;});
goto _LL0;};};};};};case 18U: _LL27: _tmp4FF=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL28: {
# 2619
int _tmp3C5=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FF);
Cyc_Toc_set_in_sizeof(nv,_tmp3C5);
goto _LL0;}case 17U: _LL29: _tmp500=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL2A:
({void*_tmp97E=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->tag=17U,({void*_tmp97D=Cyc_Toc_typ_to_c(_tmp500);_tmp3C6->f1=_tmp97D;});_tmp3C6;});e->r=_tmp97E;});goto _LL0;case 19U: _LL2B: _tmp502=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp501=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL2C: {
# 2625
void*_tmp3C7=_tmp502;
struct Cyc_List_List*_tmp3C8=_tmp501;
for(0;_tmp3C8 != 0;_tmp3C8=_tmp3C8->tl){
void*_tmp3C9=(void*)_tmp3C8->hd;void*_tmp3CA=_tmp3C9;unsigned int _tmp3DC;struct _dyneither_ptr*_tmp3DB;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CA)->tag == 0U){_LLCD: _tmp3DB=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CA)->f1;_LLCE:
 goto _LLCC;}else{_LLCF: _tmp3DC=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3CA)->f1;_LLD0:
# 2631
{void*_tmp3CB=Cyc_Tcutil_compress(_tmp3C7);void*_tmp3CC=_tmp3CB;struct Cyc_Absyn_Datatypefield*_tmp3DA;struct Cyc_List_List*_tmp3D9;struct Cyc_List_List*_tmp3D8;union Cyc_Absyn_AggrInfoU _tmp3D7;switch(*((int*)_tmp3CC)){case 11U: _LLD2: _tmp3D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CC)->f1).aggr_info;_LLD3: {
# 2633
struct Cyc_Absyn_Aggrdecl*_tmp3CD=Cyc_Absyn_get_known_aggrdecl(_tmp3D7);
if(_tmp3CD->impl == 0)
({void*_tmp3CE=0U;({struct _dyneither_ptr _tmp97F=({const char*_tmp3CF="struct fields must be known";_tag_dyneither(_tmp3CF,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97F,_tag_dyneither(_tmp3CE,sizeof(void*),0U));});});
_tmp3D8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3CD->impl))->fields;goto _LLD5;}case 12U: _LLD4: _tmp3D8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3CC)->f2;_LLD5: {
# 2638
struct Cyc_Absyn_Aggrfield*_tmp3D0=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D8,(int)_tmp3DC);
({void*_tmp980=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->tag=0U,_tmp3D1->f1=_tmp3D0->name;_tmp3D1;}));_tmp3C8->hd=_tmp980;});
_tmp3C7=_tmp3D0->type;
goto _LLD1;}case 10U: _LLD6: _tmp3D9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3CC)->f1;_LLD7:
# 2643
({void*_tmp982=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->tag=0U,({struct _dyneither_ptr*_tmp981=Cyc_Absyn_fieldname((int)(_tmp3DC + 1));_tmp3D2->f1=_tmp981;});_tmp3D2;}));_tmp3C8->hd=_tmp982;});
({void*_tmp983=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D9,(int)_tmp3DC)).f2;_tmp3C7=_tmp983;});
goto _LLD1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CC)->f1).field_info).KnownDatatypefield).tag == 2){_LLD8: _tmp3DA=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CC)->f1).field_info).KnownDatatypefield).val).f2;_LLD9:
# 2647
 if(_tmp3DC == 0)
({void*_tmp984=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->tag=0U,_tmp3D3->f1=Cyc_Toc_tag_sp;_tmp3D3;}));_tmp3C8->hd=_tmp984;});else{
# 2650
({void*_tmp985=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DA->typs,(int)(_tmp3DC - 1))).f2;_tmp3C7=_tmp985;});
({void*_tmp987=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->tag=0U,({struct _dyneither_ptr*_tmp986=Cyc_Absyn_fieldname((int)_tmp3DC);_tmp3D4->f1=_tmp986;});_tmp3D4;}));_tmp3C8->hd=_tmp987;});}
# 2653
goto _LLD1;}else{goto _LLDA;}default: _LLDA: _LLDB:
({void*_tmp3D5=0U;({struct _dyneither_ptr _tmp988=({const char*_tmp3D6="impossible type for offsetof tuple index";_tag_dyneither(_tmp3D6,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp988,_tag_dyneither(_tmp3D5,sizeof(void*),0U));});});
goto _LLD1;}_LLD1:;}
# 2657
goto _LLCC;}_LLCC:;}
# 2660
({void*_tmp98A=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->tag=19U,({void*_tmp989=Cyc_Toc_typ_to_c(_tmp502);_tmp3DD->f1=_tmp989;}),_tmp3DD->f2=_tmp501;_tmp3DD;});e->r=_tmp98A;});
goto _LL0;}case 20U: _LL2D: _tmp503=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL2E: {
# 2663
int _tmp3DE=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3DF=Cyc_Tcutil_compress((void*)_check_null(_tmp503->topt));
{void*_tmp3E0=_tmp3DF;void*_tmp3F5;struct Cyc_Absyn_Tqual _tmp3F4;void*_tmp3F3;union Cyc_Absyn_Constraint*_tmp3F2;union Cyc_Absyn_Constraint*_tmp3F1;union Cyc_Absyn_Constraint*_tmp3F0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->tag == 5U){_LLDD: _tmp3F5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->f1).elt_typ;_tmp3F4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->f1).elt_tq;_tmp3F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->f1).ptr_atts).rgn;_tmp3F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->f1).ptr_atts).nullable;_tmp3F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->f1).ptr_atts).bounds;_tmp3F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0)->f1).ptr_atts).zero_term;_LLDE:
# 2668
{void*_tmp3E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3F1);void*_tmp3E2=_tmp3E1;struct Cyc_Absyn_Exp*_tmp3ED;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E2)->tag == 1U){_LLE2: _tmp3ED=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E2)->f1;_LLE3: {
# 2670
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp503);
Cyc_Toc_exp_to_c(nv,_tmp503);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3E3=0U;({unsigned int _tmp98C=e->loc;struct _dyneither_ptr _tmp98B=({const char*_tmp3E4="inserted null check due to dereference";_tag_dyneither(_tmp3E4,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp98C,_tmp98B,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});
# 2676
({void*_tmp990=({void*_tmp98F=Cyc_Toc_typ_to_c(_tmp3DF);Cyc_Toc_cast_it_r(_tmp98F,({
struct Cyc_Absyn_Exp*_tmp98E=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp98E,({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));
({struct Cyc_Absyn_Exp*_tmp98D=Cyc_Absyn_copy_exp(_tmp503);_tmp3E5->hd=_tmp98D;}),_tmp3E5->tl=0;_tmp3E5;}),0U);}));});
# 2676
_tmp503->r=_tmp990;});}
# 2684
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F0)){
struct _tuple13 _tmp3E6=Cyc_Evexp_eval_const_uint_exp(_tmp3ED);struct _tuple13 _tmp3E7=_tmp3E6;unsigned int _tmp3EB;int _tmp3EA;_LLE7: _tmp3EB=_tmp3E7.f1;_tmp3EA=_tmp3E7.f2;_LLE8:;
# 2690
if(!_tmp3EA  || _tmp3EB <= 0)
({void*_tmp3E8=0U;({unsigned int _tmp992=e->loc;struct _dyneither_ptr _tmp991=({const char*_tmp3E9="cannot determine dereference is in bounds";_tag_dyneither(_tmp3E9,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp992,_tmp991,_tag_dyneither(_tmp3E8,sizeof(void*),0U));});});}
# 2693
goto _LLE1;}}else{_LLE4: _LLE5: {
# 2696
struct Cyc_Absyn_Exp*_tmp3EC=Cyc_Absyn_uint_exp(0U,0U);
_tmp3EC->topt=Cyc_Absyn_uint_typ;
({void*_tmp993=Cyc_Toc_subscript_exp_r(_tmp503,_tmp3EC);e->r=_tmp993;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLE1;}}_LLE1:;}
# 2702
goto _LLDC;}else{_LLDF: _LLE0:
({void*_tmp3EE=0U;({struct _dyneither_ptr _tmp994=({const char*_tmp3EF="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3EF,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp994,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});}_LLDC:;}
# 2705
Cyc_Toc_set_lhs(nv,_tmp3DE);
goto _LL0;};}case 21U: _LL2F: _tmp507=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp506=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp505=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp504=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL30: {
# 2708
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp507->topt);
Cyc_Toc_exp_to_c(nv,_tmp507);
if(_tmp505  && _tmp504)
({void*_tmp999=({struct Cyc_Absyn_Exp*_tmp998=_tmp507;void*_tmp997=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp996=e1_cyc_type;struct _dyneither_ptr*_tmp995=_tmp506;Cyc_Toc_check_tagged_union(_tmp998,_tmp997,_tmp996,_tmp995,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2712
e->r=_tmp999;});
# 2716
if(is_poly)
({void*_tmp99B=({void*_tmp99A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99A,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp99B;});
goto _LL0;}case 22U: _LL31: _tmp50B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp50A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp509=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp508=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL32: {
# 2720
int _tmp3F6=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp50B->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp50B);
Cyc_Toc_exp_to_c(nv,_tmp50B);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3F7=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3F8=_tmp3F7;void*_tmp40E;struct Cyc_Absyn_Tqual _tmp40D;void*_tmp40C;union Cyc_Absyn_Constraint*_tmp40B;union Cyc_Absyn_Constraint*_tmp40A;union Cyc_Absyn_Constraint*_tmp409;_LLEA: _tmp40E=_tmp3F8.elt_typ;_tmp40D=_tmp3F8.elt_tq;_tmp40C=(_tmp3F8.ptr_atts).rgn;_tmp40B=(_tmp3F8.ptr_atts).nullable;_tmp40A=(_tmp3F8.ptr_atts).bounds;_tmp409=(_tmp3F8.ptr_atts).zero_term;_LLEB:;
{void*_tmp3F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp40A);void*_tmp3FA=_tmp3F9;struct Cyc_Absyn_Exp*_tmp408;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3FA)->tag == 1U){_LLED: _tmp408=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3FA)->f1;_LLEE: {
# 2731
struct _tuple13 _tmp3FB=Cyc_Evexp_eval_const_uint_exp(_tmp408);struct _tuple13 _tmp3FC=_tmp3FB;unsigned int _tmp406;int _tmp405;_LLF2: _tmp406=_tmp3FC.f1;_tmp405=_tmp3FC.f2;_LLF3:;
if(_tmp405){
if(_tmp406 < 1)
({void*_tmp3FD=0U;({struct _dyneither_ptr _tmp99C=({const char*_tmp3FE="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp3FE,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp99C,_tag_dyneither(_tmp3FD,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3FF=0U;({unsigned int _tmp99E=e->loc;struct _dyneither_ptr _tmp99D=({const char*_tmp400="inserted null check due to dereference";_tag_dyneither(_tmp400,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp99E,_tmp99D,_tag_dyneither(_tmp3FF,sizeof(void*),0U));});});
# 2739
({void*_tmp9A2=({void*_tmp9A1=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9A1,({
struct Cyc_Absyn_Exp*_tmp9A0=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9A0,({struct Cyc_Absyn_Exp*_tmp401[1U];({struct Cyc_Absyn_Exp*_tmp99F=
Cyc_Absyn_new_exp(_tmp50B->r,0U);_tmp401[0]=_tmp99F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp401,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2739
_tmp50B->r=_tmp9A2;});}}else{
# 2744
if(!Cyc_Evexp_c_can_eval(_tmp408))
({void*_tmp402=0U;({unsigned int _tmp9A4=e->loc;struct _dyneither_ptr _tmp9A3=({const char*_tmp403="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp403,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp9A4,_tmp9A3,_tag_dyneither(_tmp402,sizeof(void*),0U));});});
# 2748
({void*_tmp9AA=({void*_tmp9A9=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9A9,({
struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9A8,({struct Cyc_Absyn_Exp*_tmp404[4U];({
struct Cyc_Absyn_Exp*_tmp9A7=Cyc_Absyn_new_exp(_tmp50B->r,0U);_tmp404[0]=_tmp9A7;}),_tmp404[1]=_tmp408,({
struct Cyc_Absyn_Exp*_tmp9A6=Cyc_Absyn_sizeoftyp_exp(_tmp40E,0U);_tmp404[2]=_tmp9A6;}),({
struct Cyc_Absyn_Exp*_tmp9A5=Cyc_Absyn_uint_exp(0U,0U);_tmp404[3]=_tmp9A5;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp404,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2748
_tmp50B->r=_tmp9AA;});}
# 2754
goto _LLEC;}}else{_LLEF: _LLF0: {
# 2757
void*ta1=Cyc_Toc_typ_to_c(_tmp40E);
({void*_tmp9B0=({void*_tmp9AF=Cyc_Absyn_cstar_typ(ta1,_tmp40D);Cyc_Toc_cast_it_r(_tmp9AF,({
struct Cyc_Absyn_Exp*_tmp9AE=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9AE,({struct Cyc_Absyn_Exp*_tmp407[3U];({
struct Cyc_Absyn_Exp*_tmp9AD=Cyc_Absyn_new_exp(_tmp50B->r,0U);_tmp407[0]=_tmp9AD;}),({
struct Cyc_Absyn_Exp*_tmp9AC=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp407[1]=_tmp9AC;}),({
struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Absyn_uint_exp(0U,0U);_tmp407[2]=_tmp9AB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp407,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2758
_tmp50B->r=_tmp9B0;});
# 2765
goto _LLEC;}}_LLEC:;}
# 2767
if(_tmp509  && _tmp508)
({void*_tmp9B5=({struct Cyc_Absyn_Exp*_tmp9B4=_tmp50B;void*_tmp9B3=Cyc_Toc_typ_to_c(e1typ);void*_tmp9B2=_tmp40E;struct _dyneither_ptr*_tmp9B1=_tmp50A;Cyc_Toc_check_tagged_union(_tmp9B4,_tmp9B3,_tmp9B2,_tmp9B1,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9B5;});
# 2770
if(is_poly  && _tmp508)
({void*_tmp9B7=({void*_tmp9B6=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9B6,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9B7;});
Cyc_Toc_set_lhs(nv,_tmp3F6);
goto _LL0;};};}case 23U: _LL33: _tmp50D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp50C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL34: {
# 2775
int _tmp40F=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp410=Cyc_Tcutil_compress((void*)_check_null(_tmp50D->topt));
# 2779
{void*_tmp411=_tmp410;void*_tmp43F;struct Cyc_Absyn_Tqual _tmp43E;void*_tmp43D;union Cyc_Absyn_Constraint*_tmp43C;union Cyc_Absyn_Constraint*_tmp43B;union Cyc_Absyn_Constraint*_tmp43A;struct Cyc_List_List*_tmp439;switch(*((int*)_tmp411)){case 10U: _LLF5: _tmp439=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp411)->f1;_LLF6:
# 2782
 Cyc_Toc_exp_to_c(nv,_tmp50D);
Cyc_Toc_exp_to_c(nv,_tmp50C);{
struct _tuple13 _tmp412=Cyc_Evexp_eval_const_uint_exp(_tmp50C);struct _tuple13 _tmp413=_tmp412;unsigned int _tmp417;int _tmp416;_LLFC: _tmp417=_tmp413.f1;_tmp416=_tmp413.f2;_LLFD:;
if(!_tmp416)
({void*_tmp414=0U;({struct _dyneither_ptr _tmp9B8=({const char*_tmp415="unknown tuple subscript in translation to C";_tag_dyneither(_tmp415,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9B8,_tag_dyneither(_tmp414,sizeof(void*),0U));});});
({void*_tmp9BA=({struct Cyc_Absyn_Exp*_tmp9B9=_tmp50D;Cyc_Toc_aggrmember_exp_r(_tmp9B9,Cyc_Absyn_fieldname((int)(_tmp417 + 1)));});e->r=_tmp9BA;});
goto _LLF4;};case 5U: _LLF7: _tmp43F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp411)->f1).elt_typ;_tmp43E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp411)->f1).elt_tq;_tmp43D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp411)->f1).ptr_atts).rgn;_tmp43C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp411)->f1).ptr_atts).nullable;_tmp43B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp411)->f1).ptr_atts).bounds;_tmp43A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp411)->f1).ptr_atts).zero_term;_LLF8: {
# 2790
struct Cyc_List_List*_tmp418=Cyc_Toc_get_relns(_tmp50D);
int _tmp419=Cyc_Toc_need_null_check(_tmp50D);
int _tmp41A=Cyc_Toc_in_sizeof(nv);
# 2798
int in_bnds=_tmp41A;
{void*_tmp41B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43B);void*_tmp41C=_tmp41B;_LLFF: _LL100:
# 2801
({int _tmp9BB=in_bnds  || Cyc_Toc_check_bounds(_tmp410,_tmp418,_tmp50D,_tmp50C);in_bnds=_tmp9BB;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp41F=({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0U,({struct _dyneither_ptr _tmp9BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79D.f1=_tmp9BC;});_tmp79D;});void*_tmp41D[1U];_tmp41D[0]=& _tmp41F;({unsigned int _tmp9BE=e->loc;struct _dyneither_ptr _tmp9BD=({const char*_tmp41E="bounds check necessary for %s";_tag_dyneither(_tmp41E,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9BE,_tmp9BD,_tag_dyneither(_tmp41D,sizeof(void*),1U));});});_LLFE:;}
# 2808
Cyc_Toc_exp_to_c(nv,_tmp50D);
Cyc_Toc_exp_to_c(nv,_tmp50C);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp420=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43B);void*_tmp421=_tmp420;struct Cyc_Absyn_Exp*_tmp436;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp421)->tag == 1U){_LL102: _tmp436=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp421)->f1;_LL103: {
# 2813
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp43C) && _tmp419;
void*ta1=Cyc_Toc_typ_to_c(_tmp43F);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp43E);
struct _tuple13 _tmp422=Cyc_Evexp_eval_const_uint_exp(_tmp436);struct _tuple13 _tmp423=_tmp422;unsigned int _tmp433;int _tmp432;_LL107: _tmp433=_tmp423.f1;_tmp432=_tmp423.f2;_LL108:;
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp424=0U;({unsigned int _tmp9C0=e->loc;struct _dyneither_ptr _tmp9BF=({const char*_tmp425="inserted null check due to dereference";_tag_dyneither(_tmp425,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C0,_tmp9BF,_tag_dyneither(_tmp424,sizeof(void*),0U));});});
({void*_tmp9C4=({void*_tmp9C3=ta2;Cyc_Toc_cast_it_r(_tmp9C3,({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9C2,({struct Cyc_Absyn_Exp*_tmp426[1U];({struct Cyc_Absyn_Exp*_tmp9C1=
Cyc_Absyn_copy_exp(_tmp50D);_tmp426[0]=_tmp9C1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp426,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2823
_tmp50D->r=_tmp9C4;});}else{
# 2827
if((_tmp432  && _tmp433 == 1) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp43A)){
# 2829
if(!Cyc_Evexp_c_can_eval(_tmp436))
({void*_tmp427=0U;({unsigned int _tmp9C6=e->loc;struct _dyneither_ptr _tmp9C5=({const char*_tmp428="cannot determine subscript is in bounds";_tag_dyneither(_tmp428,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9C6,_tmp9C5,_tag_dyneither(_tmp427,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp50D);
({void*_tmp9C9=Cyc_Toc_deref_exp_r(({void*_tmp9C8=ta2;Cyc_Toc_cast_it(_tmp9C8,({
struct Cyc_Absyn_Exp*_tmp9C7=function_e;Cyc_Absyn_fncall_exp(_tmp9C7,({struct Cyc_Absyn_Exp*_tmp429[3U];_tmp429[0]=_tmp50D,_tmp429[1]=_tmp436,_tmp429[2]=_tmp50C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp429,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2832
e->r=_tmp9C9;});};}else{
# 2835
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp436))
({void*_tmp42A=0U;({unsigned int _tmp9CB=e->loc;struct _dyneither_ptr _tmp9CA=({const char*_tmp42B="cannot determine subscript is in bounds";_tag_dyneither(_tmp42B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9CB,_tmp9CA,_tag_dyneither(_tmp42A,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp42C=0U;({unsigned int _tmp9CD=e->loc;struct _dyneither_ptr _tmp9CC=({const char*_tmp42D="inserted null check due to dereference";_tag_dyneither(_tmp42D,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9CD,_tmp9CC,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
# 2841
({void*_tmp9D1=Cyc_Toc_deref_exp_r(({void*_tmp9D0=ta2;Cyc_Toc_cast_it(_tmp9D0,({
struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9CF,({struct Cyc_Absyn_Exp*_tmp42E[4U];_tmp42E[0]=_tmp50D,_tmp42E[1]=_tmp436,({
# 2844
struct Cyc_Absyn_Exp*_tmp9CE=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp42E[2]=_tmp9CE;}),_tmp42E[3]=_tmp50C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42E,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2841
e->r=_tmp9D1;});}else{
# 2847
if(!Cyc_Evexp_c_can_eval(_tmp436))
({void*_tmp42F=0U;({unsigned int _tmp9D3=e->loc;struct _dyneither_ptr _tmp9D2=({const char*_tmp430="cannot determine subscript is in bounds";_tag_dyneither(_tmp430,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9D3,_tmp9D2,_tag_dyneither(_tmp42F,sizeof(void*),0U));});});
# 2850
({void*_tmp9D6=({struct Cyc_Absyn_Exp*_tmp9D5=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9D5,({struct Cyc_Absyn_Exp*_tmp431[2U];_tmp431[0]=_tmp436,({
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Absyn_copy_exp(_tmp50C);_tmp431[1]=_tmp9D4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp431,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2850
_tmp50C->r=_tmp9D6;});}}}}
# 2853
goto _LL101;}}else{_LL104: _LL105: {
# 2855
void*ta1=Cyc_Toc_typ_to_c(_tmp43F);
if(in_bnds){
# 2859
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9D9=({struct Cyc_Absyn_Exp*_tmp9D8=({void*_tmp9D7=Cyc_Absyn_cstar_typ(ta1,_tmp43E);Cyc_Toc_cast_it(_tmp9D7,
Cyc_Toc_member_exp(_tmp50D,Cyc_Toc_curr_sp,0U));});
# 2860
Cyc_Toc_subscript_exp_r(_tmp9D8,_tmp50C);});e->r=_tmp9D9;});}else{
# 2864
struct Cyc_Absyn_Exp*_tmp434=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmp9DD=Cyc_Toc_deref_exp_r(({void*_tmp9DC=Cyc_Absyn_cstar_typ(ta1,_tmp43E);Cyc_Toc_cast_it(_tmp9DC,({
struct Cyc_Absyn_Exp*_tmp9DB=_tmp434;Cyc_Absyn_fncall_exp(_tmp9DB,({struct Cyc_Absyn_Exp*_tmp435[3U];_tmp435[0]=_tmp50D,({
struct Cyc_Absyn_Exp*_tmp9DA=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp435[1]=_tmp9DA;}),_tmp435[2]=_tmp50C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp435,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2865
e->r=_tmp9DD;});}
# 2870
goto _LL101;}}_LL101:;}
# 2872
goto _LLF4;}default: _LLF9: _LLFA:
({void*_tmp437=0U;({struct _dyneither_ptr _tmp9DE=({const char*_tmp438="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp438,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9DE,_tag_dyneither(_tmp437,sizeof(void*),0U));});});}_LLF4:;}
# 2875
Cyc_Toc_set_lhs(nv,_tmp40F);
goto _LL0;};}case 24U: _LL35: _tmp50E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL36:
# 2878
 if(!Cyc_Toc_is_toplevel(nv)){
# 2880
void*_tmp440=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp50E));
{void*_tmp441=_tmp440;union Cyc_Absyn_AggrInfoU _tmp446;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp441)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp441)->f1).targs == 0){_LL10A: _tmp446=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp441)->f1).aggr_info;_LL10B: {
# 2883
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp446);
{int i=1;for(0;_tmp50E != 0;(_tmp50E=_tmp50E->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp50E->hd);
({struct Cyc_List_List*_tmp9E1=({struct Cyc_List_List*_tmp442=_cycalloc(sizeof(*_tmp442));({struct _tuple20*_tmp9E0=({struct _dyneither_ptr*_tmp9DF=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9DF,(struct Cyc_Absyn_Exp*)_tmp50E->hd);});_tmp442->hd=_tmp9E0;}),_tmp442->tl=dles;_tmp442;});dles=_tmp9E1;});}}
# 2889
({void*_tmp9E3=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->tag=29U,_tmp443->f1=sd->name,_tmp443->f2=0,({struct Cyc_List_List*_tmp9E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp443->f3=_tmp9E2;}),_tmp443->f4=sd;_tmp443;});e->r=_tmp9E3;});
e->topt=_tmp440;
goto _LL109;}}else{goto _LL10C;}}else{_LL10C: _LL10D:
({void*_tmp444=0U;({struct _dyneither_ptr _tmp9E4=({const char*_tmp445="tuple type not an aggregate";_tag_dyneither(_tmp445,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E4,_tag_dyneither(_tmp444,sizeof(void*),0U));});});}_LL109:;}
# 2894
goto _LL0;}else{
# 2898
struct Cyc_List_List*dles=0;
for(0;_tmp50E != 0;_tmp50E=_tmp50E->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp50E->hd);
({struct Cyc_List_List*_tmp9E6=({struct Cyc_List_List*_tmp448=_cycalloc(sizeof(*_tmp448));({struct _tuple20*_tmp9E5=({struct _tuple20*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->f1=0,_tmp447->f2=(struct Cyc_Absyn_Exp*)_tmp50E->hd;_tmp447;});_tmp448->hd=_tmp9E5;}),_tmp448->tl=dles;_tmp448;});dles=_tmp9E6;});}
# 2903
({void*_tmp9E7=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9E7;});}
# 2905
goto _LL0;case 26U: _LL37: _tmp50F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL38:
# 2911
{struct Cyc_List_List*_tmp449=_tmp50F;for(0;_tmp449 != 0;_tmp449=_tmp449->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp449->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9E8=Cyc_Toc_unresolvedmem_exp_r(0,_tmp50F);e->r=_tmp9E8;});
goto _LL0;case 27U: _LL39: _tmp513=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp512=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp511=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp510=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL3A:
# 2919
 Cyc_Toc_exp_to_c(nv,_tmp512);
Cyc_Toc_exp_to_c(nv,_tmp511);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp44A=Cyc_Evexp_eval_const_uint_exp(_tmp512);struct _tuple13 _tmp44B=_tmp44A;unsigned int _tmp455;int _tmp454;_LL10F: _tmp455=_tmp44B.f1;_tmp454=_tmp44B.f2;_LL110:;{
void*_tmp44C=Cyc_Toc_typ_to_c((void*)_check_null(_tmp511->topt));
struct Cyc_List_List*es=0;
# 2926
if(!Cyc_Toc_is_zero(_tmp511)){
if(!_tmp454)
({void*_tmp44D=0U;({unsigned int _tmp9EA=_tmp512->loc;struct _dyneither_ptr _tmp9E9=({const char*_tmp44E="cannot determine value of constant";_tag_dyneither(_tmp44E,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9EA,_tmp9E9,_tag_dyneither(_tmp44D,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp455;++ i){
({struct Cyc_List_List*_tmp9EC=({struct Cyc_List_List*_tmp450=_cycalloc(sizeof(*_tmp450));({struct _tuple20*_tmp9EB=({struct _tuple20*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->f1=0,_tmp44F->f2=_tmp511;_tmp44F;});_tmp450->hd=_tmp9EB;}),_tmp450->tl=es;_tmp450;});es=_tmp9EC;});}}
# 2932
if(_tmp510){
struct Cyc_Absyn_Exp*_tmp451=({void*_tmp9ED=_tmp44C;Cyc_Toc_cast_it(_tmp9ED,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9F0=({struct Cyc_List_List*_tmp9EF=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9EF,({struct Cyc_List_List*_tmp453=_cycalloc(sizeof(*_tmp453));({struct _tuple20*_tmp9EE=({struct _tuple20*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->f1=0,_tmp452->f2=_tmp451;_tmp452;});_tmp453->hd=_tmp9EE;}),_tmp453->tl=0;_tmp453;}));});es=_tmp9F0;});}}
# 2937
({void*_tmp9F1=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9F1;});};}
# 2939
goto _LL0;case 28U: _LL3B: _tmp516=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp515=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp514=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL3C:
# 2946
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9F2=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9F2;});else{
# 2949
Cyc_Toc_exp_to_c(nv,_tmp516);}
goto _LL0;case 30U: _LL3D: _tmp518=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp517=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL3E:
# 2953
{struct Cyc_List_List*_tmp456=_tmp517;for(0;_tmp456 != 0;_tmp456=_tmp456->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp456->hd)).f2);}}{
void*_tmp457=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp457;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp458=Cyc_Tcutil_compress(_tmp457);void*_tmp459=_tmp458;union Cyc_Absyn_AggrInfoU _tmp45D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp459)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp459)->f1).targs == 0){_LL112: _tmp45D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp459)->f1).aggr_info;_LL113: {
# 2960
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp45D);
({void*_tmp9F3=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=29U,_tmp45A->f1=sd->name,_tmp45A->f2=0,_tmp45A->f3=_tmp517,_tmp45A->f4=sd;_tmp45A;});e->r=_tmp9F3;});
e->topt=_tmp457;
goto _LL111;}}else{goto _LL114;}}else{_LL114: _LL115:
({void*_tmp45B=0U;({struct _dyneither_ptr _tmp9F4=({const char*_tmp45C="anonStruct type not an aggregate";_tag_dyneither(_tmp45C,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F4,_tag_dyneither(_tmp45B,sizeof(void*),0U));});});}_LL111:;}else{
# 2967
({void*_tmp9F5=Cyc_Toc_unresolvedmem_exp_r(0,_tmp517);e->r=_tmp9F5;});}
goto _LL0;};case 29U: _LL3F: _tmp51C=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp51B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp51A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp519=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL40:
# 2973
 if(_tmp519 == 0  || _tmp519->impl == 0)
({void*_tmp45E=0U;({struct _dyneither_ptr _tmp9F6=({const char*_tmp45F="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp45F,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F6,_tag_dyneither(_tmp45E,sizeof(void*),0U));});});{
void*_tmp460=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp461=Cyc_Tcutil_compress(_tmp460);void*_tmp462=_tmp461;union Cyc_Absyn_AggrInfoU _tmp465;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp462)->tag == 11U){_LL117: _tmp465=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp462)->f1).aggr_info;_LL118:
({struct _tuple1*_tmp9F7=(Cyc_Absyn_aggr_kinded_name(_tmp465)).f2;*_tmp51C=_tmp9F7;});goto _LL116;}else{_LL119: _LL11A:
({void*_tmp463=0U;({struct _dyneither_ptr _tmp9F8=({const char*_tmp464="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp464,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F8,_tag_dyneither(_tmp463,sizeof(void*),0U));});});}_LL116:;}{
# 2984
struct Cyc_List_List*_tmp466=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->fields;
if(_tmp466 == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp466))->tl != 0;_tmp466=_tmp466->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp467=(struct Cyc_Absyn_Aggrfield*)_tmp466->hd;
struct Cyc_List_List*_tmp468=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp51A,_tmp519->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->fields);
# 2991
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp469=Cyc_Tcutil_compress(old_typ);void*_tmp46A=_tmp469;struct Cyc_List_List*_tmp497;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46A)->tag == 11U){_LL11C: _tmp497=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46A)->f1).targs;_LL11D:
# 2995
{struct Cyc_List_List*_tmp46B=_tmp468;for(0;_tmp46B != 0;_tmp46B=_tmp46B->tl){
struct _tuple34*_tmp46C=(struct _tuple34*)_tmp46B->hd;struct _tuple34*_tmp46D=_tmp46C;struct Cyc_Absyn_Aggrfield*_tmp494;struct Cyc_Absyn_Exp*_tmp493;_LL121: _tmp494=_tmp46D->f1;_tmp493=_tmp46D->f2;_LL122:;{
void*_tmp46E=_tmp493->topt;
Cyc_Toc_exp_to_c(nv,_tmp493);
# 3000
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp494->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp493->topt)))
({void*_tmp9FA=({void*_tmp9F9=Cyc_Toc_typ_to_c(_tmp494->type);Cyc_Toc_cast_it(_tmp9F9,
Cyc_Absyn_copy_exp(_tmp493));})->r;
# 3002
_tmp493->r=_tmp9FA;});
# 3005
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->tagged){
struct _dyneither_ptr*_tmp46F=_tmp494->name;
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp519,_tmp46F),0U);
struct _tuple20*_tmp471=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp470);
struct _tuple20*_tmp472=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp493);
struct _tuple1*s=({struct _tuple1*_tmp47D=_cycalloc(sizeof(*_tmp47D));({union Cyc_Absyn_Nmspace _tmp9FE=Cyc_Absyn_Abs_n(0,1);_tmp47D->f1=_tmp9FE;}),({
struct _dyneither_ptr*_tmp9FD=({struct _dyneither_ptr*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct _dyneither_ptr _tmp9FC=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp47A=({struct Cyc_String_pa_PrintArg_struct _tmp79F;_tmp79F.tag=0U,_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp519->name).f2);_tmp79F;});struct Cyc_String_pa_PrintArg_struct _tmp47B=({struct Cyc_String_pa_PrintArg_struct _tmp79E;_tmp79E.tag=0U,_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46F);_tmp79E;});void*_tmp478[2U];_tmp478[0]=& _tmp47A,_tmp478[1]=& _tmp47B;({struct _dyneither_ptr _tmp9FB=({const char*_tmp479="_union_%s_%s";_tag_dyneither(_tmp479,sizeof(char),13U);});Cyc_aprintf(_tmp9FB,_tag_dyneither(_tmp478,sizeof(void*),2U));});});*_tmp47C=_tmp9FC;});_tmp47C;});_tmp47D->f2=_tmp9FD;});_tmp47D;});
# 3013
struct _tuple20*_tmp473=({
struct _dyneither_ptr*_tmpA00=_tmp46F;Cyc_Toc_make_field(_tmpA00,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=29U,_tmp477->f1=s,_tmp477->f2=0,({struct Cyc_List_List*_tmp9FF=({struct _tuple20*_tmp476[2U];_tmp476[0]=_tmp471,_tmp476[1]=_tmp472;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp476,sizeof(struct _tuple20*),2U));});_tmp477->f3=_tmp9FF;}),_tmp477->f4=0;_tmp477;}),0U));});
# 3016
({void*_tmpA02=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=29U,_tmp475->f1=*_tmp51C,_tmp475->f2=0,({struct Cyc_List_List*_tmpA01=({struct _tuple20*_tmp474[1U];_tmp474[0]=_tmp473;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp474,sizeof(struct _tuple20*),1U));});_tmp475->f3=_tmpA01;}),_tmp475->f4=_tmp519;_tmp475;});e->r=_tmpA02;});}
# 3022
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp467 == _tmp494){
struct Cyc_List_List*_tmp47E=({struct Cyc_List_List*_tmpA03=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->exist_vars,_tmp51B);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA03,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp519->tvs,_tmp497));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp47F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->fields;for(0;_tmp47F != 0;_tmp47F=_tmp47F->tl){
struct Cyc_Absyn_Aggrfield*_tmp480=(struct Cyc_Absyn_Aggrfield*)_tmp47F->hd;
void*_tmp481=Cyc_Tcutil_substitute(_tmp47E,_tmp480->type);
struct Cyc_Absyn_Aggrfield*_tmp482=({struct Cyc_Absyn_Aggrfield*_tmpA04=_tmp480;Cyc_Toc_aggrfield_to_c(_tmpA04,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp47E,_tmp481)));});
({struct Cyc_List_List*_tmpA05=({struct Cyc_List_List*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->hd=_tmp482,_tmp483->tl=new_fields;_tmp483;});new_fields=_tmpA05;});
# 3037
if(_tmp47F->tl == 0){
{void*_tmp484=Cyc_Tcutil_compress(_tmp482->type);void*_tmp485=_tmp484;struct Cyc_Absyn_ArrayInfo _tmp48A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp485)->tag == 8U){_LL124: _tmp48A=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp485)->f1;_LL125:
# 3040
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp48A.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp486=_tmp48A;
({struct Cyc_Absyn_Exp*_tmpA06=Cyc_Absyn_uint_exp(0U,0U);_tmp486.num_elts=_tmpA06;});
({void*_tmpA07=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->tag=8U,_tmp487->f1=_tmp486;_tmp487;});_tmp482->type=_tmpA07;});}
# 3045
goto _LL123;}else{_LL126: _LL127:
# 3049
 if(_tmp493->topt == 0)
goto _LL123;
{void*_tmp488=Cyc_Tcutil_compress((void*)_check_null(_tmp493->topt));void*_tmp489=_tmp488;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp489)->tag == 11U){_LL129: _LL12A:
# 3053
 _tmp482->type=(void*)_check_null(_tmp493->topt);goto _LL128;}else{_LL12B: _LL12C:
 goto _LL128;}_LL128:;}
# 3056
goto _LL123;}_LL123:;}
# 3060
if(!Cyc_Toc_is_array_type(_tmp480->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp480->type)))
({struct Cyc_List_List*_tmpA09=({struct Cyc_List_List*_tmp48C=_cycalloc(sizeof(*_tmp48C));({void*_tmpA08=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->tag=6U,_tmp48B->f1=0;_tmp48B;});_tmp48C->hd=_tmpA08;}),_tmp48C->tl=_tmp482->attributes;_tmp48C;});_tmp482->attributes=_tmpA09;});}}}
# 3066
({struct Cyc_Absyn_Aggrdecl*_tmpA0D=({struct _dyneither_ptr*_tmpA0C=({struct _dyneither_ptr*_tmp490=_cycalloc(sizeof(*_tmp490));({struct _dyneither_ptr _tmpA0B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp48F=({struct Cyc_Int_pa_PrintArg_struct _tmp7A0;_tmp7A0.tag=1U,_tmp7A0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A0;});void*_tmp48D[1U];_tmp48D[0]=& _tmp48F;({struct _dyneither_ptr _tmpA0A=({const char*_tmp48E="_genStruct%d";_tag_dyneither(_tmp48E,sizeof(char),13U);});Cyc_aprintf(_tmpA0A,_tag_dyneither(_tmp48D,sizeof(void*),1U));});});*_tmp490=_tmpA0B;});_tmp490;});Cyc_Toc_make_c_struct_defn(_tmpA0C,
# 3068
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 3066
_tmp519=_tmpA0D;});
# 3069
*_tmp51C=_tmp519->name;
Cyc_Toc_aggrdecl_to_c(_tmp519,1);
# 3072
({void*_tmpA0F=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpA0E=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp491=_cycalloc(sizeof(*_tmp491));*_tmp491=_tmp519;_tmp491;}));(_tmp492->f1).aggr_info=_tmpA0E;}),(_tmp492->f1).targs=0;_tmp492;});e->topt=_tmpA0F;});}};}}
# 3075
goto _LL11B;}else{_LL11E: _LL11F:
({void*_tmp495=0U;({struct _dyneither_ptr _tmpA10=({const char*_tmp496="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp496,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA10,_tag_dyneither(_tmp495,sizeof(void*),0U));});});}_LL11B:;}else{
# 3084
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->tagged){
# 3086
struct _tuple34*_tmp498=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp468))->hd;struct _tuple34*_tmp499=_tmp498;struct Cyc_Absyn_Aggrfield*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;_LL12E: _tmp4A2=_tmp499->f1;_tmp4A1=_tmp499->f2;_LL12F:;{
void*_tmp49A=(void*)_check_null(_tmp4A1->topt);
void*_tmp49B=_tmp4A2->type;
Cyc_Toc_exp_to_c(nv,_tmp4A1);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49B) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49A))
({void*_tmpA12=({
void*_tmpA11=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpA11,Cyc_Absyn_new_exp(_tmp4A1->r,0U));});
# 3092
_tmp4A1->r=_tmpA12;});{
# 3095
int i=Cyc_Toc_get_member_offset(_tmp519,_tmp4A2->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp49C=({struct _tuple20*_tmp49E[2U];({struct _tuple20*_tmpA14=({struct _tuple20*_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F->f1=0,_tmp49F->f2=field_tag_exp;_tmp49F;});_tmp49E[0]=_tmpA14;}),({struct _tuple20*_tmpA13=({struct _tuple20*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->f1=0,_tmp4A0->f2=_tmp4A1;_tmp4A0;});_tmp49E[1]=_tmpA13;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49E,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp49C,0U);
({void*_tmpA16=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp49D[1U];({struct _tuple20*_tmpA15=Cyc_Toc_make_field(_tmp4A2->name,umem);_tmp49D[0]=_tmpA15;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49D,sizeof(struct _tuple20*),1U));}));e->r=_tmpA16;});};};}else{
# 3102
struct Cyc_List_List*_tmp4A3=0;
struct Cyc_List_List*_tmp4A4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp519->impl))->fields;
for(0;_tmp4A4 != 0;_tmp4A4=_tmp4A4->tl){
struct Cyc_List_List*_tmp4A5=_tmp468;for(0;_tmp4A5 != 0;_tmp4A5=_tmp4A5->tl){
if((*((struct _tuple34*)_tmp4A5->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4A4->hd){
struct _tuple34*_tmp4A6=(struct _tuple34*)_tmp4A5->hd;struct _tuple34*_tmp4A7=_tmp4A6;struct Cyc_Absyn_Aggrfield*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;_LL131: _tmp4AD=_tmp4A7->f1;_tmp4AC=_tmp4A7->f2;_LL132:;{
void*_tmp4A8=Cyc_Toc_typ_to_c(_tmp4AD->type);
void*_tmp4A9=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4AC->topt));
Cyc_Toc_exp_to_c(nv,_tmp4AC);
# 3112
if(!Cyc_Tcutil_unify(_tmp4A8,_tmp4A9)){
# 3114
if(!Cyc_Tcutil_is_arithmetic_type(_tmp4A8) || !
Cyc_Tcutil_is_arithmetic_type(_tmp4A9))
({struct Cyc_Absyn_Exp*_tmpA18=({void*_tmpA17=_tmp4A8;Cyc_Toc_cast_it(_tmpA17,Cyc_Absyn_copy_exp(_tmp4AC));});_tmp4AC=_tmpA18;});}
({struct Cyc_List_List*_tmpA1A=({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct _tuple20*_tmpA19=({struct _tuple20*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->f1=0,_tmp4AA->f2=_tmp4AC;_tmp4AA;});_tmp4AB->hd=_tmpA19;}),_tmp4AB->tl=_tmp4A3;_tmp4AB;});_tmp4A3=_tmpA1A;});
break;};}}}
# 3121
({void*_tmpA1B=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A3));e->r=_tmpA1B;});}}
# 3124
goto _LL0;};};};case 31U: _LL41: _tmp51F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp51E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp51D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL42: {
# 3128
struct Cyc_List_List*_tmp4AE=_tmp51D->typs;
{struct Cyc_List_List*_tmp4AF=_tmp51F;for(0;_tmp4AF != 0;(_tmp4AF=_tmp4AF->tl,_tmp4AE=_tmp4AE->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4AF->hd;
void*_tmp4B0=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4AE))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4B0))
({void*_tmpA1C=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA1C;});}}{
# 3139
struct Cyc_Absyn_Exp*_tmp4B1=_tmp51E->is_extensible?Cyc_Absyn_var_exp(_tmp51D->name,0U):
 Cyc_Toc_datatype_tag(_tmp51E,_tmp51D->name);
# 3142
if(!Cyc_Toc_is_toplevel(nv)){
# 3144
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp51F != 0;(_tmp51F=_tmp51F->tl,++ i)){
({struct Cyc_List_List*_tmpA1F=({struct Cyc_List_List*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({struct _tuple20*_tmpA1E=({struct _dyneither_ptr*_tmpA1D=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA1D,(struct Cyc_Absyn_Exp*)_tmp51F->hd);});_tmp4B2->hd=_tmpA1E;}),_tmp4B2->tl=dles;_tmp4B2;});dles=_tmpA1F;});}}{
# 3150
struct _tuple20*_tmp4B3=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp4B1);
({void*_tmpA23=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->tag=29U,({struct _tuple1*_tmpA22=Cyc_Toc_collapse_qvars(_tmp51D->name,_tmp51E->name);_tmp4B5->f1=_tmpA22;}),_tmp4B5->f2=0,({
struct Cyc_List_List*_tmpA21=({struct Cyc_List_List*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->hd=_tmp4B3,({struct Cyc_List_List*_tmpA20=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4B4->tl=_tmpA20;});_tmp4B4;});_tmp4B5->f3=_tmpA21;}),_tmp4B5->f4=0;_tmp4B5;});
# 3151
e->r=_tmpA23;});};}else{
# 3156
struct Cyc_List_List*_tmp4B6=0;
for(0;_tmp51F != 0;_tmp51F=_tmp51F->tl){
({struct Cyc_List_List*_tmpA25=({struct Cyc_List_List*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));({struct _tuple20*_tmpA24=({struct _tuple20*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->f1=0,_tmp4B7->f2=(struct Cyc_Absyn_Exp*)_tmp51F->hd;_tmp4B7;});_tmp4B8->hd=_tmpA24;}),_tmp4B8->tl=_tmp4B6;_tmp4B8;});_tmp4B6=_tmpA25;});}
({void*_tmpA28=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));({struct _tuple20*_tmpA27=({struct _tuple20*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->f1=0,_tmp4B9->f2=_tmp4B1;_tmp4B9;});_tmp4BA->hd=_tmpA27;}),({
struct Cyc_List_List*_tmpA26=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4B6);_tmp4BA->tl=_tmpA26;});_tmp4BA;}));
# 3159
e->r=_tmpA28;});}
# 3162
goto _LL0;};}case 32U: _LL43: _LL44:
# 3164
 goto _LL46;case 33U: _LL45: _LL46:
 goto _LL0;case 34U: _LL47: _tmp525=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).is_calloc;_tmp524=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).rgn;_tmp523=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).elt_type;_tmp522=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).num_elts;_tmp521=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).fat_result;_tmp520=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).inline_call;_LL48: {
# 3168
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp523)));
Cyc_Toc_exp_to_c(nv,_tmp522);
# 3172
if(_tmp521){
struct _tuple1*_tmp4BB=Cyc_Toc_temp_var();
struct _tuple1*_tmp4BC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp525){
xexp=_tmp522;
if(_tmp524 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp524;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA2B=({struct Cyc_Absyn_Exp*_tmpA2A=rgn;struct Cyc_Absyn_Exp*_tmpA29=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA2A,_tmpA29,Cyc_Absyn_var_exp(_tmp4BB,0U));});pexp=_tmpA2B;});}else{
# 3183
({struct Cyc_Absyn_Exp*_tmpA2E=({void*_tmpA2D=*_tmp523;struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA2D,_tmpA2C,Cyc_Absyn_var_exp(_tmp4BB,0U));});pexp=_tmpA2E;});}
# 3185
({struct Cyc_Absyn_Exp*_tmpA33=({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA32,({struct Cyc_Absyn_Exp*_tmp4BD[3U];({
struct Cyc_Absyn_Exp*_tmpA31=Cyc_Absyn_var_exp(_tmp4BC,0U);_tmp4BD[0]=_tmpA31;}),({struct Cyc_Absyn_Exp*_tmpA30=Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp4BD[1]=_tmpA30;}),({
struct Cyc_Absyn_Exp*_tmpA2F=Cyc_Absyn_var_exp(_tmp4BB,0U);_tmp4BD[2]=_tmpA2F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BD,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3185
rexp=_tmpA33;});}else{
# 3189
if(_tmp524 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp524;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp520)
({struct Cyc_Absyn_Exp*_tmpA35=({struct Cyc_Absyn_Exp*_tmpA34=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA34,Cyc_Absyn_var_exp(_tmp4BB,0U));});pexp=_tmpA35;});else{
# 3195
({struct Cyc_Absyn_Exp*_tmpA37=({struct Cyc_Absyn_Exp*_tmpA36=rgn;Cyc_Toc_rmalloc_exp(_tmpA36,Cyc_Absyn_var_exp(_tmp4BB,0U));});pexp=_tmpA37;});}}else{
# 3197
({struct Cyc_Absyn_Exp*_tmpA39=({void*_tmpA38=*_tmp523;Cyc_Toc_malloc_exp(_tmpA38,Cyc_Absyn_var_exp(_tmp4BB,0U));});pexp=_tmpA39;});}
# 3199
({struct Cyc_Absyn_Exp*_tmpA3E=({struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA3D,({struct Cyc_Absyn_Exp*_tmp4BE[3U];({struct Cyc_Absyn_Exp*_tmpA3C=Cyc_Absyn_var_exp(_tmp4BC,0U);_tmp4BE[0]=_tmpA3C;}),({struct Cyc_Absyn_Exp*_tmpA3B=Cyc_Absyn_uint_exp(1U,0U);_tmp4BE[1]=_tmpA3B;}),({
struct Cyc_Absyn_Exp*_tmpA3A=Cyc_Absyn_var_exp(_tmp4BB,0U);_tmp4BE[2]=_tmpA3A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BE,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3199
rexp=_tmpA3E;});}{
# 3202
struct Cyc_Absyn_Stmt*_tmp4BF=({struct _tuple1*_tmpA44=_tmp4BB;void*_tmpA43=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA42=_tmp522;Cyc_Absyn_declare_stmt(_tmpA44,_tmpA43,_tmpA42,({
struct _tuple1*_tmpA41=_tmp4BC;void*_tmpA40=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA3F=pexp;Cyc_Absyn_declare_stmt(_tmpA41,_tmpA40,_tmpA3F,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA45=Cyc_Toc_stmt_exp_r(_tmp4BF);e->r=_tmpA45;});};}else{
# 3207
if(_tmp525){
if(_tmp524 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp524;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA48=({struct Cyc_Absyn_Exp*_tmpA47=rgn;struct Cyc_Absyn_Exp*_tmpA46=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA47,_tmpA46,_tmp522);})->r;e->r=_tmpA48;});}else{
# 3213
({void*_tmpA4B=({void*_tmpA4A=*_tmp523;struct Cyc_Absyn_Exp*_tmpA49=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA4A,_tmpA49,_tmp522);})->r;e->r=_tmpA4B;});}}else{
# 3216
if(_tmp524 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp524;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp520)
({void*_tmpA4C=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp522))->r;e->r=_tmpA4C;});else{
# 3222
({void*_tmpA4D=(Cyc_Toc_rmalloc_exp(rgn,_tmp522))->r;e->r=_tmpA4D;});}}else{
# 3224
({void*_tmpA4E=(Cyc_Toc_malloc_exp(*_tmp523,_tmp522))->r;e->r=_tmpA4E;});}}}
# 3228
goto _LL0;}case 35U: _LL49: _tmp527=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp526=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL4A: {
# 3237
void*e1_old_typ=(void*)_check_null(_tmp527->topt);
void*e2_old_typ=(void*)_check_null(_tmp526->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4C0=0U;({struct _dyneither_ptr _tmpA4F=({const char*_tmp4C1="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4C1,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA4F,_tag_dyneither(_tmp4C0,sizeof(void*),0U));});});{
# 3245
unsigned int _tmp4C2=e->loc;
struct _tuple1*_tmp4C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_var_exp(_tmp4C3,_tmp4C2);_tmp4C4->topt=e1_old_typ;{
struct _tuple1*_tmp4C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_var_exp(_tmp4C5,_tmp4C2);_tmp4C6->topt=e2_old_typ;{
# 3251
struct Cyc_Absyn_Exp*_tmp4C7=({struct Cyc_Absyn_Exp*_tmpA51=Cyc_Tcutil_deep_copy_exp(1,_tmp527);struct Cyc_Absyn_Exp*_tmpA50=_tmp4C6;Cyc_Absyn_assign_exp(_tmpA51,_tmpA50,_tmp4C2);});_tmp4C7->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4C8=Cyc_Absyn_exp_stmt(_tmp4C7,_tmp4C2);
struct Cyc_Absyn_Exp*_tmp4C9=({struct Cyc_Absyn_Exp*_tmpA53=Cyc_Tcutil_deep_copy_exp(1,_tmp526);struct Cyc_Absyn_Exp*_tmpA52=_tmp4C4;Cyc_Absyn_assign_exp(_tmpA53,_tmpA52,_tmp4C2);});_tmp4C9->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4CA=Cyc_Absyn_exp_stmt(_tmp4C9,_tmp4C2);
# 3256
struct Cyc_Absyn_Stmt*_tmp4CB=({struct _tuple1*_tmpA5B=_tmp4C3;void*_tmpA5A=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA59=_tmp527;struct Cyc_Absyn_Stmt*_tmpA58=({
struct _tuple1*_tmpA57=_tmp4C5;void*_tmpA56=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA55=_tmp526;struct Cyc_Absyn_Stmt*_tmpA54=
Cyc_Absyn_seq_stmt(_tmp4C8,_tmp4CA,_tmp4C2);
# 3257
Cyc_Absyn_declare_stmt(_tmpA57,_tmpA56,_tmpA55,_tmpA54,_tmp4C2);});
# 3256
Cyc_Absyn_declare_stmt(_tmpA5B,_tmpA5A,_tmpA59,_tmpA58,_tmp4C2);});
# 3259
Cyc_Toc_stmt_to_c(nv,_tmp4CB);
({void*_tmpA5C=Cyc_Toc_stmt_exp_r(_tmp4CB);e->r=_tmpA5C;});
goto _LL0;};};};};};}case 38U: _LL4B: _tmp529=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp528=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL4C: {
# 3264
void*_tmp4CC=Cyc_Tcutil_compress((void*)_check_null(_tmp529->topt));
Cyc_Toc_exp_to_c(nv,_tmp529);
{void*_tmp4CD=_tmp4CC;struct Cyc_Absyn_Aggrdecl*_tmp4D3;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CD)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CD)->f1).aggr_info).KnownAggr).tag == 2){_LL134: _tmp4D3=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CD)->f1).aggr_info).KnownAggr).val;_LL135: {
# 3268
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4D3,_tmp528),0U);
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Toc_member_exp(_tmp529,_tmp528,0U);
struct Cyc_Absyn_Exp*_tmp4D0=Cyc_Toc_member_exp(_tmp4CF,Cyc_Toc_tag_sp,0U);
({void*_tmpA5D=(Cyc_Absyn_eq_exp(_tmp4D0,_tmp4CE,0U))->r;e->r=_tmpA5D;});
goto _LL133;}}else{goto _LL136;}}else{_LL136: _LL137:
({void*_tmp4D1=0U;({struct _dyneither_ptr _tmpA5E=({const char*_tmp4D2="non-aggregate type in tagcheck";_tag_dyneither(_tmp4D2,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA5E,_tag_dyneither(_tmp4D1,sizeof(void*),0U));});});}_LL133:;}
# 3275
goto _LL0;}case 37U: _LL4D: _tmp52A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL4E:
 Cyc_Toc_stmt_to_c(nv,_tmp52A);goto _LL0;case 36U: _LL4F: _LL50:
({void*_tmp4D4=0U;({struct _dyneither_ptr _tmpA5F=({const char*_tmp4D5="UnresolvedMem";_tag_dyneither(_tmp4D5,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA5F,_tag_dyneither(_tmp4D4,sizeof(void*),0U));});});case 25U: _LL51: _LL52:
({void*_tmp4D6=0U;({struct _dyneither_ptr _tmpA60=({const char*_tmp4D7="compoundlit";_tag_dyneither(_tmp4D7,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA60,_tag_dyneither(_tmp4D6,sizeof(void*),0U));});});case 39U: _LL53: _LL54:
({void*_tmp4D8=0U;({struct _dyneither_ptr _tmpA61=({const char*_tmp4D9="valueof(-)";_tag_dyneither(_tmp4D9,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA61,_tag_dyneither(_tmp4D8,sizeof(void*),0U));});});default: _LL55: _LL56:
({void*_tmp4DA=0U;({struct _dyneither_ptr _tmpA62=({const char*_tmp4DB="__asm__";_tag_dyneither(_tmp4DB,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA62,_tag_dyneither(_tmp4DA,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3308 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3310
return({struct _tuple35*_tmp52B=_region_malloc(r,sizeof(*_tmp52B));_tmp52B->f1=0,({struct _dyneither_ptr*_tmpA64=Cyc_Toc_fresh_label();_tmp52B->f2=_tmpA64;}),({struct _dyneither_ptr*_tmpA63=Cyc_Toc_fresh_label();_tmp52B->f3=_tmpA63;}),_tmp52B->f4=sc;_tmp52B;});}
# 3315
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp52C=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3321
if((int)(((_tmp52C->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp52F=_tmp52C->orig_pat;if((_tmp52F.pattern).tag != 1)_throw_match();(_tmp52F.pattern).val;})->topt);
void*_tmp52D=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp52E=_tmp52D;switch(*((int*)_tmp52E)){case 0U: _LL1: _LL2:
# 3326
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA66=({void*_tmpA65=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA65,v);});v=_tmpA66;});goto _LL0;}_LL0:;}{
# 3332
void*_tmp530=_tmp52C->access;void*_tmp531=_tmp530;struct Cyc_Absyn_Datatypedecl*_tmp53C;struct Cyc_Absyn_Datatypefield*_tmp53B;unsigned int _tmp53A;int _tmp539;struct _dyneither_ptr*_tmp538;unsigned int _tmp537;switch(*((int*)_tmp531)){case 0U: _LLA: _LLB:
# 3337
 goto _LL9;case 1U: _LLC: _LLD:
# 3342
 if(ps->tl != 0){
void*_tmp532=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp533=_tmp532;struct Cyc_Absyn_Datatypedecl*_tmp536;struct Cyc_Absyn_Datatypefield*_tmp535;unsigned int _tmp534;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->tag == 3U){_LL15: _tmp536=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->f1;_tmp535=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->f2;_tmp534=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->f3;_LL16:
# 3345
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA69=({void*_tmpA68=({void*_tmpA67=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp535->name,_tmp536->name));Cyc_Absyn_cstar_typ(_tmpA67,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA68,v);});v=_tmpA69;});
({struct Cyc_Absyn_Exp*_tmpA6B=({struct Cyc_Absyn_Exp*_tmpA6A=v;Cyc_Absyn_aggrarrow_exp(_tmpA6A,Cyc_Absyn_fieldname((int)(_tmp534 + 1)),0U);});v=_tmpA6B;});
continue;}else{_LL17: _LL18:
# 3350
 goto _LL14;}_LL14:;}
# 3353
({struct Cyc_Absyn_Exp*_tmpA6C=Cyc_Absyn_deref_exp(v,0U);v=_tmpA6C;});
goto _LL9;case 2U: _LLE: _tmp537=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp531)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA6E=({struct Cyc_Absyn_Exp*_tmpA6D=v;Cyc_Toc_member_exp(_tmpA6D,Cyc_Absyn_fieldname((int)(_tmp537 + 1)),0U);});v=_tmpA6E;});goto _LL9;case 4U: _LL10: _tmp539=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp531)->f1;_tmp538=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp531)->f2;_LL11:
# 3357
({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Toc_member_exp(v,_tmp538,0U);v=_tmpA6F;});
if(_tmp539)
({struct Cyc_Absyn_Exp*_tmpA70=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA70;});
goto _LL9;default: _LL12: _tmp53C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp531)->f1;_tmp53B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp531)->f2;_tmp53A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp531)->f3;_LL13:
# 3362
({struct Cyc_Absyn_Exp*_tmpA72=({struct Cyc_Absyn_Exp*_tmpA71=v;Cyc_Toc_member_exp(_tmpA71,Cyc_Absyn_fieldname((int)(_tmp53A + 1)),0U);});v=_tmpA72;});
goto _LL9;}_LL9:;};}
# 3366
return v;}
# 3371
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp53D=t;struct Cyc_Absyn_Datatypedecl*_tmp556;struct Cyc_Absyn_Datatypefield*_tmp555;struct _dyneither_ptr*_tmp554;int _tmp553;int _tmp552;struct Cyc_Absyn_Datatypedecl*_tmp551;struct Cyc_Absyn_Datatypefield*_tmp550;unsigned int _tmp54F;struct _dyneither_ptr _tmp54E;int _tmp54D;void*_tmp54C;struct Cyc_Absyn_Enumfield*_tmp54B;struct Cyc_Absyn_Enumdecl*_tmp54A;struct Cyc_Absyn_Enumfield*_tmp549;struct Cyc_Absyn_Exp*_tmp548;switch(*((int*)_tmp53D)){case 0U: _LL1: _tmp548=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp53D)->f1;_LL2:
# 3375
 if(_tmp548 == 0)return v;else{return _tmp548;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA73=v;Cyc_Absyn_eq_exp(_tmpA73,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA74=v;Cyc_Absyn_neq_exp(_tmpA74,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp54A=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp53D)->f1;_tmp549=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp53D)->f2;_LL8:
# 3379
 return({struct Cyc_Absyn_Exp*_tmpA75=v;Cyc_Absyn_eq_exp(_tmpA75,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E->tag=32U,_tmp53E->f1=_tmp54A,_tmp53E->f2=_tmp549;_tmp53E;}),0U),0U);});case 4U: _LL9: _tmp54C=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp53D)->f1;_tmp54B=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp53D)->f2;_LLA:
# 3381
 return({struct Cyc_Absyn_Exp*_tmpA76=v;Cyc_Absyn_eq_exp(_tmpA76,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->tag=33U,_tmp53F->f1=_tmp54C,_tmp53F->f2=_tmp54B;_tmp53F;}),0U),0U);});case 5U: _LLB: _tmp54E=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp53D)->f1;_tmp54D=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp53D)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA77=v;Cyc_Absyn_eq_exp(_tmpA77,Cyc_Absyn_float_exp(_tmp54E,_tmp54D,0U),0U);});case 6U: _LLD: _tmp54F=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp53D)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA78=v;Cyc_Absyn_eq_exp(_tmpA78,Cyc_Absyn_signed_int_exp((int)_tmp54F,0U),0U);});case 7U: _LLF: _tmp552=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp53D)->f1;_tmp551=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp53D)->f2;_tmp550=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp53D)->f3;_LL10:
# 3387
 LOOP1: {
void*_tmp540=v->r;void*_tmp541=_tmp540;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp542;switch(*((int*)_tmp541)){case 14U: _LL16: _tmp542=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LL17:
 v=_tmp542;goto LOOP1;case 20U: _LL18: _tmp543=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_LL19:
 v=_tmp543;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3394
({struct Cyc_Absyn_Exp*_tmpA7B=({void*_tmpA7A=({void*_tmpA79=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp550->name,_tmp551->name));Cyc_Absyn_cstar_typ(_tmpA79,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA7A,v);});v=_tmpA7B;});
return({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA7C,Cyc_Absyn_uint_exp((unsigned int)_tmp552,0U),0U);});case 8U: _LL11: _tmp554=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp53D)->f1;_tmp553=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp53D)->f2;_LL12:
# 3399
({struct Cyc_Absyn_Exp*_tmpA7E=({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Toc_member_exp(v,_tmp554,0U);Cyc_Toc_member_exp(_tmpA7D,Cyc_Toc_tag_sp,0U);});v=_tmpA7E;});
return({struct Cyc_Absyn_Exp*_tmpA7F=v;Cyc_Absyn_eq_exp(_tmpA7F,Cyc_Absyn_signed_int_exp(_tmp553,0U),0U);});default: _LL13: _tmp556=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp53D)->f1;_tmp555=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp53D)->f2;_LL14:
# 3403
 LOOP2: {
void*_tmp544=v->r;void*_tmp545=_tmp544;struct Cyc_Absyn_Exp*_tmp547;struct Cyc_Absyn_Exp*_tmp546;switch(*((int*)_tmp545)){case 14U: _LL1D: _tmp546=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp545)->f2;_LL1E:
 v=_tmp546;goto LOOP2;case 20U: _LL1F: _tmp547=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_LL20:
 v=_tmp547;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3410
({struct Cyc_Absyn_Exp*_tmpA82=({void*_tmpA81=({void*_tmpA80=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp555->name,_tmp556->name));Cyc_Absyn_cstar_typ(_tmpA80,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA81,v);});v=_tmpA82;});
return({struct Cyc_Absyn_Exp*_tmpA83=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA83,Cyc_Absyn_var_exp(_tmp555->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3424
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3432
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3434
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp557=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp558=_tmp557;void*_tmp55E;_LL1: _tmp55E=_tmp558.f1;_LL2:;{
void*_tmp559=_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct _dyneither_ptr*_tmp55C;switch(*((int*)_tmp559)){case 3U: _LL4: _LL5:
# 3440
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3443
 continue;case 8U: _LLA: _tmp55C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp559)->f1;_LLB:
# 3445
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA84=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->tag=3U,_tmp55A->f1=_tmp55C;_tmp55A;});k=_tmpA84;});
continue;case 7U: _LLC: _LLD:
# 3449
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp55D=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp559)->f1;if(_tmp55D != 0){_LLF:
# 3453
({void*_tmpA85=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->tag=2U,_tmp55B->f1=_tmp55D;_tmp55B;});k=_tmpA85;});
return({struct _tuple36 _tmp7A1;_tmp7A1.f1=0,_tmp7A1.f2=k;_tmp7A1;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp7A2;_tmp7A2.f1=0,_tmp7A2.f2=k;_tmp7A2;});}_LL3:;};}
# 3462
return({struct _tuple36 _tmp7A3;_tmp7A3.f1=c,_tmp7A3.f2=k;_tmp7A3;});}
# 3467
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp55F=p;int _tmp56A;int _tmp569;unsigned int _tmp568;void*_tmp567;struct Cyc_Absyn_Enumfield*_tmp566;struct Cyc_Absyn_Enumdecl*_tmp565;struct Cyc_Absyn_Enumfield*_tmp564;switch(*((int*)_tmp55F)){case 3U: _LL1: _tmp565=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp55F)->f1;_tmp564=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp55F)->f2;_LL2:
# 3471
({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->tag=32U,_tmp560->f1=_tmp565,_tmp560->f2=_tmp564;_tmp560;}),0U);e=_tmpA86;});goto _LL0;case 4U: _LL3: _tmp567=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp55F)->f1;_tmp566=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp55F)->f2;_LL4:
# 3473
({struct Cyc_Absyn_Exp*_tmpA87=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=33U,_tmp561->f1=_tmp567,_tmp561->f2=_tmp566;_tmp561;}),0U);e=_tmpA87;});goto _LL0;case 6U: _LL5: _tmp568=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp55F)->f1;_LL6:
 _tmp569=(int)_tmp568;goto _LL8;case 7U: _LL7: _tmp569=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp55F)->f1;_LL8:
 _tmp56A=_tmp569;goto _LLA;case 8U: _LL9: _tmp56A=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp55F)->f2;_LLA:
# 3477
({struct Cyc_Absyn_Exp*_tmpA88=Cyc_Absyn_uint_exp((unsigned int)_tmp56A,0U);e=_tmpA88;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp562=0U;({struct _dyneither_ptr _tmpA89=({const char*_tmp563="compile_pat_test_as_case!";_tag_dyneither(_tmp563,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA89,_tag_dyneither(_tmp562,sizeof(void*),0U));});});}_LL0:;}
# 3481
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->tag=17U,_tmp56B->f1=e;_tmp56B;}),0U);}
# 3485
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3494
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3497
void*t=(void*)_check_null(p->topt);
void*_tmp56C=p->r;void*_tmp56D=_tmp56C;union Cyc_Absyn_AggrInfoU _tmp59E;struct Cyc_List_List*_tmp59D;struct Cyc_List_List*_tmp59C;struct Cyc_List_List*_tmp59B;struct Cyc_Absyn_Pat*_tmp59A;struct Cyc_Absyn_Datatypedecl*_tmp599;struct Cyc_Absyn_Datatypefield*_tmp598;struct Cyc_List_List*_tmp597;struct Cyc_Absyn_Vardecl*_tmp596;struct Cyc_Absyn_Pat*_tmp595;struct Cyc_Absyn_Vardecl*_tmp594;struct Cyc_Absyn_Vardecl*_tmp593;struct Cyc_Absyn_Pat*_tmp592;struct Cyc_Absyn_Vardecl*_tmp591;switch(*((int*)_tmp56D)){case 2U: _LL1: _tmp591=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp56D)->f2;_LL2: {
# 3500
struct Cyc_Absyn_Pat*_tmp56E=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp56E->topt=p->topt;
_tmp593=_tmp591;_tmp592=_tmp56E;goto _LL4;}case 1U: _LL3: _tmp593=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56D)->f1;_tmp592=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56D)->f2;_LL4:
# 3505
({struct Cyc_List_List*_tmpA8A=({struct Cyc_List_List*_tmp56F=_region_malloc(rgn,sizeof(*_tmp56F));_tmp56F->hd=_tmp593,_tmp56F->tl=*decls;_tmp56F;});*decls=_tmpA8A;});{
struct Cyc_Absyn_Stmt*_tmp570=({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->tag=4U,_tmp571->f1=_tmp593;_tmp571;}),0U);Cyc_Absyn_assign_stmt(_tmpA8B,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA8C=_tmp570;Cyc_Toc_seq_stmt_opt(_tmpA8C,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp592));});};case 4U: _LL5: _tmp594=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp56D)->f2;_LL6:
# 3510
({struct Cyc_List_List*_tmpA8D=({struct Cyc_List_List*_tmp572=_region_malloc(rgn,sizeof(*_tmp572));_tmp572->hd=_tmp594,_tmp572->tl=*decls;_tmp572;});*decls=_tmpA8D;});
return({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=4U,_tmp573->f1=_tmp594;_tmp573;}),0U);Cyc_Absyn_assign_stmt(_tmpA8E,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp596=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp56D)->f1;_tmp595=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp56D)->f2;_LLA:
# 3515
({struct Cyc_List_List*_tmpA8F=({struct Cyc_List_List*_tmp574=_region_malloc(rgn,sizeof(*_tmp574));_tmp574->hd=_tmp596,_tmp574->tl=*decls;_tmp574;});*decls=_tmpA8F;});
({void*_tmpA90=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp596->type=_tmpA90;});{
# 3518
struct Cyc_Absyn_Stmt*_tmp575=({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=4U,_tmp576->f1=_tmp596;_tmp576;}),0U);Cyc_Absyn_assign_stmt(_tmpA93,({
void*_tmpA92=({void*_tmpA91=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpA91,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA92,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA94=_tmp575;Cyc_Toc_seq_stmt_opt(_tmpA94,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp595));});};case 9U: _LLB: _LLC:
# 3523
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56D)->f1)->r)->tag == 8U){_LL17: _tmp599=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56D)->f1)->r)->f1;_tmp598=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56D)->f1)->r)->f2;_tmp597=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56D)->f1)->r)->f3;_LL18:
# 3532
 if(_tmp597 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp598->name,_tmp599->name);
void*_tmp577=({void*_tmpA95=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpA95,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA96=Cyc_Toc_cast_it(_tmp577,path);path=_tmpA96;});{
int cnt=1;
struct Cyc_List_List*_tmp578=_tmp598->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp597 != 0;(_tmp597=_tmp597->tl,_tmp578=((struct Cyc_List_List*)_check_null(_tmp578))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp579=(struct Cyc_Absyn_Pat*)_tmp597->hd;
if(_tmp579->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp57A=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp578))->hd)).f2;
void*_tmp57B=(void*)_check_null(_tmp579->topt);
void*_tmp57C=Cyc_Toc_typ_to_c_array(_tmp57B);
struct Cyc_Absyn_Exp*_tmp57D=({struct Cyc_Absyn_Exp*_tmpA97=path;Cyc_Absyn_aggrarrow_exp(_tmpA97,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp57A)))
({struct Cyc_Absyn_Exp*_tmpA98=Cyc_Toc_cast_it(_tmp57C,_tmp57D);_tmp57D=_tmpA98;});
({struct Cyc_Absyn_Stmt*_tmpA9A=({struct Cyc_Absyn_Stmt*_tmpA99=s;Cyc_Toc_seq_stmt_opt(_tmpA99,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp57D,_tmp579));});s=_tmpA9A;});};}
# 3550
return s;};};}else{_LL21: _tmp59A=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56D)->f1;_LL22:
# 3595
 return({struct _RegionHandle*_tmpA9E=rgn;struct Cyc_Toc_Env**_tmpA9D=nv;struct Cyc_List_List**_tmpA9C=decls;struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA9E,_tmpA9D,_tmpA9C,_tmpA9B,_tmp59A);});}case 8U: _LL19: _tmp59B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp56D)->f3;_LL1A:
# 3552
 _tmp59C=_tmp59B;goto _LL1C;case 5U: _LL1B: _tmp59C=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp56D)->f1;_LL1C: {
# 3554
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp59C != 0;(_tmp59C=_tmp59C->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp57E=(struct Cyc_Absyn_Pat*)_tmp59C->hd;
if(_tmp57E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp57F=(void*)_check_null(_tmp57E->topt);
struct _dyneither_ptr*_tmp580=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpAA4=({struct Cyc_Absyn_Stmt*_tmpAA3=s;Cyc_Toc_seq_stmt_opt(_tmpAA3,({struct _RegionHandle*_tmpAA2=rgn;struct Cyc_Toc_Env**_tmpAA1=nv;struct Cyc_List_List**_tmpAA0=decls;struct Cyc_Absyn_Exp*_tmpA9F=Cyc_Toc_member_exp(path,_tmp580,0U);Cyc_Toc_extract_pattern_vars(_tmpAA2,_tmpAA1,_tmpAA0,_tmpA9F,_tmp57E);}));});s=_tmpAA4;});};}
# 3564
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp56D)->f1 == 0){_LL1D: _LL1E:
({void*_tmp581=0U;({struct _dyneither_ptr _tmpAA5=({const char*_tmp582="unresolved aggregate pattern!";_tag_dyneither(_tmp582,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA5,_tag_dyneither(_tmp581,sizeof(void*),0U));});});}else{_LL1F: _tmp59E=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp56D)->f1)->aggr_info;_tmp59D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp56D)->f3;_LL20: {
# 3568
struct Cyc_Absyn_Aggrdecl*_tmp583=Cyc_Absyn_get_known_aggrdecl(_tmp59E);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp59D != 0;_tmp59D=_tmp59D->tl){
struct _tuple37*_tmp584=(struct _tuple37*)_tmp59D->hd;
struct Cyc_Absyn_Pat*_tmp585=(*_tmp584).f2;
if(_tmp585->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp586=Cyc_Absyn_designatorlist_to_fieldname((*_tmp584).f1);
void*_tmp587=(void*)_check_null(_tmp585->topt);
void*_tmp588=Cyc_Toc_typ_to_c_array(_tmp587);
struct Cyc_Absyn_Exp*_tmp589=Cyc_Toc_member_exp(path,_tmp586,0U);
# 3580
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp583->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpAA6=Cyc_Toc_member_exp(_tmp589,Cyc_Toc_val_sp,0U);_tmp589=_tmpAA6;});{
void*_tmp58A=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp583->impl))->fields,_tmp586)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp58A))
({struct Cyc_Absyn_Exp*_tmpAA7=Cyc_Toc_cast_it(_tmp588,_tmp589);_tmp589=_tmpAA7;});else{
if(!Cyc_Toc_is_array_type(_tmp58A) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp58A)))
# 3587
({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Absyn_deref_exp(({void*_tmpAA8=Cyc_Absyn_cstar_typ(_tmp588,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAA8,
Cyc_Absyn_address_exp(_tmp589,0U));}),0U);
# 3587
_tmp589=_tmpAA9;});}
# 3590
({struct Cyc_Absyn_Stmt*_tmpAAB=({struct Cyc_Absyn_Stmt*_tmpAAA=s;Cyc_Toc_seq_stmt_opt(_tmpAAA,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp589,_tmp585));});s=_tmpAAB;});};};}
# 3592
return s;}}case 15U: _LL23: _LL24:
# 3597
({void*_tmp58B=0U;({struct _dyneither_ptr _tmpAAC=({const char*_tmp58C="unknownid pat";_tag_dyneither(_tmp58C,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAC,_tag_dyneither(_tmp58B,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp58D=0U;({struct _dyneither_ptr _tmpAAD=({const char*_tmp58E="unknowncall pat";_tag_dyneither(_tmp58E,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAD,_tag_dyneither(_tmp58D,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp58F=0U;({struct _dyneither_ptr _tmpAAE=({const char*_tmp590="exp pat";_tag_dyneither(_tmp590,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAE,_tag_dyneither(_tmp58F,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3608
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3616
void*_tmp59F=dopt;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DC;void*_tmp5DB;struct Cyc_Tcpat_Rhs*_tmp5DA;if(_tmp59F == 0){_LL1: _LL2:
# 3618
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp59F)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5DA=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp59F)->f1;_LL6:
# 3623
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5A0=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5A1=_tmp5A0;int*_tmp5A8;struct _dyneither_ptr*_tmp5A7;struct _dyneither_ptr*_tmp5A6;struct Cyc_Absyn_Switch_clause*_tmp5A5;_LLA: _tmp5A8=(int*)& _tmp5A1->f1;_tmp5A7=_tmp5A1->f2;_tmp5A6=_tmp5A1->f3;_tmp5A5=_tmp5A1->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp5A2=_tmp5A5->body;
if(_tmp5A2 == _tmp5DA->rhs){
# 3628
if(*_tmp5A8)
return Cyc_Absyn_goto_stmt(_tmp5A7,0U);
*_tmp5A8=1;{
# 3632
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAB1=rgn;struct Cyc_List_List**_tmpAB0=decls;struct Cyc_Absyn_Exp*_tmpAAF=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAB1,& nv,_tmpAB0,_tmpAAF,_tmp5A5->pattern);});
# 3635
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp5A6,_tmp5A5->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAB2=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAB2;});
({struct Cyc_Absyn_Stmt*_tmpAB3=Cyc_Absyn_label_stmt(_tmp5A7,res,0U);res=_tmpAB3;});
({struct Cyc_List_List*_tmpAB5=({struct Cyc_List_List*_tmp5A4=_region_malloc(rgn,sizeof(*_tmp5A4));({struct _tuple38*_tmpAB4=({struct _tuple38*_tmp5A3=_region_malloc(rgn,sizeof(*_tmp5A3));_tmp5A3->f1=nv,_tmp5A3->f2=_tmp5A6,_tmp5A3->f3=_tmp5A2;_tmp5A3;});_tmp5A4->hd=_tmpAB4;}),_tmp5A4->tl=*bodies;_tmp5A4;});*bodies=_tmpAB5;});
return res;};}};}
# 3643
({void*_tmp5A9=0U;({struct _dyneither_ptr _tmpAB6=({const char*_tmp5AA="couldn't find rhs!";_tag_dyneither(_tmp5AA,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB6,_tag_dyneither(_tmp5A9,sizeof(void*),0U));});});default: _LL7: _tmp5DD=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp59F)->f1;_tmp5DC=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp59F)->f2;_tmp5DB=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp59F)->f3;_LL8: {
# 3646
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5DB,lscs,v);
# 3648
struct Cyc_Absyn_Exp*_tmp5AB=({struct Cyc_List_List*_tmpAB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5DD);Cyc_Toc_compile_path(_tmpAB7,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp5AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5DC);
# 3651
struct _tuple36 _tmp5AD=Cyc_Toc_admits_switch(_tmp5AC);struct _tuple36 _tmp5AE=_tmp5AD;int _tmp5D9;void*_tmp5D8;_LLD: _tmp5D9=_tmp5AE.f1;_tmp5D8=_tmp5AE.f2;_LLE:;
if(_tmp5D9 > 1){
# 3655
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));
({struct Cyc_Absyn_Switch_clause*_tmpAB9=({struct Cyc_Absyn_Switch_clause*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));({struct Cyc_Absyn_Pat*_tmpAB8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5BE->pattern=_tmpAB8;}),_tmp5BE->pat_vars=0,_tmp5BE->where_clause=0,_tmp5BE->body=res,_tmp5BE->loc=0U;_tmp5BE;});_tmp5BF->hd=_tmpAB9;}),_tmp5BF->tl=0;_tmp5BF;});
# 3658
for(0;_tmp5AC != 0;_tmp5AC=_tmp5AC->tl){
struct _tuple33 _tmp5AF=*((struct _tuple33*)_tmp5AC->hd);struct _tuple33 _tmp5B0=_tmp5AF;void*_tmp5B6;void*_tmp5B5;_LL10: _tmp5B6=_tmp5B0.f1;_tmp5B5=_tmp5B0.f2;_LL11:;{
# 3661
struct Cyc_Absyn_Pat*_tmp5B1=Cyc_Toc_compile_pat_test_as_case(_tmp5B6);
# 3663
struct Cyc_Absyn_Stmt*_tmp5B2=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5B5,lscs,v);
# 3665
({struct Cyc_List_List*_tmpABB=({struct Cyc_List_List*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));({struct Cyc_Absyn_Switch_clause*_tmpABA=({struct Cyc_Absyn_Switch_clause*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->pattern=_tmp5B1,_tmp5B3->pat_vars=0,_tmp5B3->where_clause=0,_tmp5B3->body=_tmp5B2,_tmp5B3->loc=0U;_tmp5B3;});_tmp5B4->hd=_tmpABA;}),_tmp5B4->tl=new_lscs;_tmp5B4;});new_lscs=_tmpABB;});};}
# 3667
{void*_tmp5B7=_tmp5D8;struct _dyneither_ptr*_tmp5BC;switch(*((int*)_tmp5B7)){case 1U: _LL13: _LL14:
# 3670
 LOOP1: {
void*_tmp5B8=_tmp5AB->r;void*_tmp5B9=_tmp5B8;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;switch(*((int*)_tmp5B9)){case 14U: _LL1C: _tmp5BA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B9)->f2;_LL1D:
 _tmp5AB=_tmp5BA;goto LOOP1;case 20U: _LL1E: _tmp5BB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5B9)->f1;_LL1F:
 _tmp5AB=_tmp5BB;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3676
({struct Cyc_Absyn_Exp*_tmpABD=Cyc_Absyn_deref_exp(({void*_tmpABC=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpABC,_tmp5AB);}),0U);_tmp5AB=_tmpABD;});goto _LL12;case 3U: _LL15: _tmp5BC=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5B7)->f1;_LL16:
# 3679
({struct Cyc_Absyn_Exp*_tmpABF=({struct Cyc_Absyn_Exp*_tmpABE=Cyc_Toc_member_exp(_tmp5AB,_tmp5BC,0U);Cyc_Toc_member_exp(_tmpABE,Cyc_Toc_tag_sp,0U);});_tmp5AB=_tmpABF;});goto _LL12;case 2U: _LL17: _LL18:
# 3681
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3684
({struct Cyc_Absyn_Stmt*_tmpAC0=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=10U,_tmp5BD->f1=_tmp5AB,_tmp5BD->f2=new_lscs,_tmp5BD->f3=0;_tmp5BD;}),0U);res=_tmpAC0;});}else{
# 3688
void*_tmp5C0=_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D7;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C0)->tag == 2U){_LL23: _tmp5D7=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C0)->f1;_LL24: {
# 3690
struct Cyc_List_List*_tmp5C1=_tmp5AC;void*_tmp5D0;struct Cyc_Tcpat_Rhs*_tmp5CF;if(_tmp5C1 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5C1)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5C1)->tl == 0){_LL28: _tmp5D0=((struct _tuple33*)_tmp5C1->hd)->f1;_tmp5CF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5C1->hd)->f2)->f1;_LL29:
# 3696
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5C2=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5C3=_tmp5C2;int*_tmp5CA;struct _dyneither_ptr*_tmp5C9;struct _dyneither_ptr*_tmp5C8;struct Cyc_Absyn_Switch_clause*_tmp5C7;_LL2D: _tmp5CA=(int*)& _tmp5C3->f1;_tmp5C9=_tmp5C3->f2;_tmp5C8=_tmp5C3->f3;_tmp5C7=_tmp5C3->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5C4=_tmp5C7->body;
if(_tmp5C4 == _tmp5CF->rhs){
# 3701
if(*_tmp5CA)
return Cyc_Absyn_goto_stmt(_tmp5C9,0U);
*_tmp5CA=1;{
# 3705
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAC3=rgn;struct Cyc_List_List**_tmpAC2=decls;struct Cyc_Absyn_Exp*_tmpAC1=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAC3,& nv,_tmpAC2,_tmpAC1,_tmp5C7->pattern);});
# 3708
Cyc_Toc_exp_to_c(nv,_tmp5D7);{
# 3711
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5C8,_tmp5C7->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAC4=Cyc_Absyn_ifthenelse_stmt(_tmp5D7,r,res,0U);r=_tmpAC4;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAC5=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpAC5;});
({struct Cyc_Absyn_Stmt*_tmpAC6=Cyc_Absyn_label_stmt(_tmp5C9,r,0U);r=_tmpAC6;});
({struct Cyc_List_List*_tmpAC8=({struct Cyc_List_List*_tmp5C6=_region_malloc(rgn,sizeof(*_tmp5C6));({struct _tuple38*_tmpAC7=({struct _tuple38*_tmp5C5=_region_malloc(rgn,sizeof(*_tmp5C5));_tmp5C5->f1=nv,_tmp5C5->f2=_tmp5C8,_tmp5C5->f3=_tmp5C4;_tmp5C5;});_tmp5C6->hd=_tmpAC7;}),_tmp5C6->tl=*bodies;_tmp5C6;});*bodies=_tmpAC8;});
return r;};};}};}
# 3720
({void*_tmp5CB=0U;({struct _dyneither_ptr _tmpAC9=({const char*_tmp5CC="couldn't find rhs!";_tag_dyneither(_tmp5CC,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC9,_tag_dyneither(_tmp5CB,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5CD=0U;({struct _dyneither_ptr _tmpACA=({const char*_tmp5CE="bad where clause in match compiler";_tag_dyneither(_tmp5CE,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACA,_tag_dyneither(_tmp5CD,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3725
 for(0;_tmp5AC != 0;_tmp5AC=_tmp5AC->tl){
struct _tuple33 _tmp5D1=*((struct _tuple33*)_tmp5AC->hd);struct _tuple33 _tmp5D2=_tmp5D1;void*_tmp5D6;void*_tmp5D5;_LL30: _tmp5D6=_tmp5D2.f1;_tmp5D5=_tmp5D2.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Toc_compile_pat_test(_tmp5AB,_tmp5D6);
struct Cyc_Absyn_Stmt*_tmp5D4=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5D5,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpACB=Cyc_Absyn_ifthenelse_stmt(_tmp5D3,_tmp5D4,res,0U);res=_tmpACB;});};}}_LL22:;}
# 3733
return res;}}}_LL0:;}
# 3743
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3745
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp5DE=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp5DF=_tmp5DE;struct Cyc_Toc_Env**_tmp5E1;struct Cyc_Absyn_Stmt*_tmp5E0;_LL1: _tmp5E1=(struct Cyc_Toc_Env**)& _tmp5DF->f1;_tmp5E0=_tmp5DF->f3;_LL2:;
if(_tmp5E0 == s)return _tmp5E1;}
# 3751
return 0;}
# 3755
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3758
void*_tmp5E2=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3761
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5E3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5E3;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5E4=Cyc_Toc_share_env(rgn,nv);
# 3768
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3773
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5E4,& mydecls,& mybodies,dopt,lscs,v);
# 3781
{struct Cyc_List_List*_tmp5E5=lscs;for(0;_tmp5E5 != 0;_tmp5E5=_tmp5E5->tl){
struct _tuple35*_tmp5E6=(struct _tuple35*)_tmp5E5->hd;struct _tuple35*_tmp5E7=_tmp5E6;struct _dyneither_ptr*_tmp5EF;struct Cyc_Absyn_Switch_clause*_tmp5EE;_LL1: _tmp5EF=_tmp5E7->f3;_tmp5EE=_tmp5E7->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5EE->body;
# 3785
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3788
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5E8=*envp;
# 3791
if(_tmp5E5->tl != 0){
struct _tuple35*_tmp5E9=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp5E5->tl))->hd;struct _tuple35*_tmp5EA=_tmp5E9;struct _dyneither_ptr*_tmp5ED;struct Cyc_Absyn_Switch_clause*_tmp5EC;_LL4: _tmp5ED=_tmp5EA->f3;_tmp5EC=_tmp5EA->f4;_LL5:;{
# 3795
struct Cyc_Toc_Env**_tmp5EB=Cyc_Toc_find_case_env(mybodies,_tmp5EC->body);
# 3800
if(_tmp5EB == 0)
({struct Cyc_Toc_Env*_tmpACC=Cyc_Toc_last_switchclause_env(rgn,_tmp5E8,end_l);Cyc_Toc_stmt_to_c(_tmpACC,s);});else{
# 3804
struct Cyc_List_List*vs=0;
if(_tmp5EC->pat_vars != 0){
({struct Cyc_List_List*_tmpACD=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5EC->pat_vars))->v)).f1);vs=_tmpACD;});
({struct Cyc_List_List*_tmpACE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpACE;});}
# 3809
({struct Cyc_Toc_Env*_tmpACF=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5E8,end_l,_tmp5ED,vs);Cyc_Toc_stmt_to_c(_tmpACF,s);});}};}else{
# 3813
({struct Cyc_Toc_Env*_tmpAD0=Cyc_Toc_last_switchclause_env(rgn,_tmp5E8,end_l);Cyc_Toc_stmt_to_c(_tmpAD0,s);});}};};}}{
# 3817
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAD2=test_tree;Cyc_Absyn_seq_stmt(_tmpAD2,({struct _dyneither_ptr*_tmpAD1=end_l;Cyc_Absyn_label_stmt(_tmpAD1,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3819
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5F0=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAD3=Cyc_Toc_temp_var();_tmp5F0->name=_tmpAD3;});
({void*_tmpAD4=Cyc_Toc_typ_to_c_array(_tmp5F0->type);_tmp5F0->type=_tmpAD4;});
({struct Cyc_Absyn_Stmt*_tmpAD6=({struct Cyc_Absyn_Decl*_tmpAD5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->tag=0U,_tmp5F1->f1=_tmp5F0;_tmp5F1;}),0U);Cyc_Absyn_decl_stmt(_tmpAD5,res,0U);});res=_tmpAD6;});}
# 3826
({void*_tmpADA=({struct _tuple1*_tmpAD9=v;void*_tmpAD8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpAD7=e;Cyc_Absyn_declare_stmt(_tmpAD9,_tmpAD8,_tmpAD7,res,0U);})->r;whole_s->r=_tmpADA;});
_npop_handler(0U);return;};}
# 3765
;_pop_region(rgn);};}
# 3832
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3834
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3839
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3841
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpADC=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpADC,({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));
({struct Cyc_Absyn_Exp*_tmpADB=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5F2->hd=_tmpADB;}),_tmp5F2->tl=0;_tmp5F2;}),0U);}),0U);}
# 3845
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpADE=({struct Cyc_Absyn_Stmt*_tmpADD=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpADD,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpADE;});}
# 3850
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3852
while(1){
void*_tmp5F3=s->r;void*_tmp5F4=_tmp5F3;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_List_List*_tmp64A;void*_tmp649;struct Cyc_Absyn_Stmt*_tmp648;struct Cyc_Absyn_Exp*_tmp647;struct _dyneither_ptr*_tmp646;struct Cyc_Absyn_Stmt*_tmp645;struct Cyc_Absyn_Decl*_tmp644;struct Cyc_Absyn_Stmt*_tmp643;struct Cyc_List_List*_tmp642;struct Cyc_Absyn_Switch_clause**_tmp641;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_List_List*_tmp63F;void*_tmp63E;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_Absyn_Exp*_tmp634;struct Cyc_Absyn_Stmt*_tmp633;struct Cyc_Absyn_Stmt*_tmp632;struct Cyc_Absyn_Exp*_tmp631;switch(*((int*)_tmp5F4)){case 0U: _LL1: _LL2:
# 3855
 return;case 1U: _LL3: _tmp631=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_LL4:
# 3857
 Cyc_Toc_exp_to_c(nv,_tmp631);
return;case 2U: _LL5: _tmp633=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp632=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_LL6:
# 3860
 Cyc_Toc_stmt_to_c(nv,_tmp633);
s=_tmp632;
continue;case 3U: _LL7: _tmp634=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_LL8: {
# 3864
void*topt=0;
if(_tmp634 != 0){
({void*_tmpADF=Cyc_Toc_typ_to_c((void*)_check_null(_tmp634->topt));topt=_tmpADF;});
Cyc_Toc_exp_to_c(nv,_tmp634);}{
# 3870
int _tmp5F5=Cyc_Toc_get_npop(s);
if(_tmp5F5 > 0){
if(topt != 0){
struct _tuple1*_tmp5F6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5F7=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5F6,0U),0U);
({void*_tmpAE4=({struct _tuple1*_tmpAE3=_tmp5F6;void*_tmpAE2=topt;struct Cyc_Absyn_Exp*_tmpAE1=_tmp634;Cyc_Absyn_declare_stmt(_tmpAE3,_tmpAE2,_tmpAE1,({
struct Cyc_Absyn_Stmt*_tmpAE0=Cyc_Toc_make_npop_handler(_tmp5F5);Cyc_Absyn_seq_stmt(_tmpAE0,_tmp5F7,0U);}),0U);})->r;
# 3875
s->r=_tmpAE4;});}else{
# 3879
Cyc_Toc_do_npop_before(_tmp5F5,s);}}
# 3881
return;};}case 4U: _LL9: _tmp637=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp636=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_tmp635=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f3;_LLA:
# 3883
 Cyc_Toc_exp_to_c(nv,_tmp637);
Cyc_Toc_stmt_to_c(nv,_tmp636);
s=_tmp635;
continue;case 5U: _LLB: _tmp639=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1).f1;_tmp638=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_LLC:
# 3888
 Cyc_Toc_exp_to_c(nv,_tmp639);{
struct _RegionHandle _tmp5F8=_new_region("temp");struct _RegionHandle*temp=& _tmp5F8;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAE5=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAE5,_tmp638);});
# 3892
_npop_handler(0U);return;
# 3889
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3894
struct Cyc_Toc_Env*_tmp5F9=nv;struct _dyneither_ptr**_tmp5FA;_LL22: _tmp5FA=_tmp5F9->break_lab;_LL23:;
if(_tmp5FA != 0)
({void*_tmpAE6=Cyc_Toc_goto_stmt_r(*_tmp5FA);s->r=_tmpAE6;});
# 3898
({int _tmpAE7=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAE7,s);});
return;}case 7U: _LLF: _LL10: {
# 3901
struct Cyc_Toc_Env*_tmp5FB=nv;struct _dyneither_ptr**_tmp5FC;_LL25: _tmp5FC=_tmp5FB->continue_lab;_LL26:;
if(_tmp5FC != 0)
({void*_tmpAE8=Cyc_Toc_goto_stmt_r(*_tmp5FC);s->r=_tmpAE8;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3907
({int _tmpAE9=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAE9,s);});
return;case 9U: _LL13: _tmp63D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp63C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2).f1;_tmp63B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f3).f1;_tmp63A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f4;_LL14:
# 3911
 Cyc_Toc_exp_to_c(nv,_tmp63D);Cyc_Toc_exp_to_c(nv,_tmp63C);Cyc_Toc_exp_to_c(nv,_tmp63B);{
struct _RegionHandle _tmp5FD=_new_region("temp");struct _RegionHandle*temp=& _tmp5FD;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAEA=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAEA,_tmp63A);});
# 3915
_npop_handler(0U);return;
# 3912
;_pop_region(temp);};case 10U: _LL15: _tmp640=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp63F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_tmp63E=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f3;_LL16:
# 3917
 Cyc_Toc_xlate_switch(nv,s,_tmp640,_tmp63F,_tmp63E);
return;case 11U: _LL17: _tmp642=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp641=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_LL18: {
# 3920
struct Cyc_Toc_Env*_tmp5FE=nv;struct Cyc_Toc_FallthruInfo*_tmp608;_LL28: _tmp608=_tmp5FE->fallthru_info;_LL29:;
if(_tmp608 == 0)
({void*_tmp5FF=0U;({struct _dyneither_ptr _tmpAEB=({const char*_tmp600="fallthru in unexpected place";_tag_dyneither(_tmp600,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAEB,_tag_dyneither(_tmp5FF,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp601=*_tmp608;struct Cyc_Toc_FallthruInfo _tmp602=_tmp601;struct _dyneither_ptr*_tmp607;struct Cyc_List_List*_tmp606;_LL2B: _tmp607=_tmp602.label;_tmp606=_tmp602.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp607,0U);
# 3926
({int _tmpAEC=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEC,s2);});{
struct Cyc_List_List*_tmp603=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp606);
struct Cyc_List_List*_tmp604=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp642);
for(0;_tmp603 != 0;(_tmp603=_tmp603->tl,_tmp604=_tmp604->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp604))->hd);
({struct Cyc_Absyn_Stmt*_tmpAEF=({struct Cyc_Absyn_Stmt*_tmpAEE=({struct Cyc_Absyn_Exp*_tmpAED=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->tag=5U,_tmp605->f1=(struct Cyc_Absyn_Vardecl*)_tmp603->hd;_tmp605;}),0U);Cyc_Absyn_assign_stmt(_tmpAED,(struct Cyc_Absyn_Exp*)_tmp604->hd,0U);});Cyc_Absyn_seq_stmt(_tmpAEE,s2,0U);});s2=_tmpAEF;});}
# 3934
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp644=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp643=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_LL1A:
# 3940
{void*_tmp609=_tmp644->r;void*_tmp60A=_tmp609;struct Cyc_Absyn_Tvar*_tmp625;struct Cyc_Absyn_Vardecl*_tmp624;struct Cyc_Absyn_Exp*_tmp623;struct Cyc_Absyn_Fndecl*_tmp622;struct Cyc_List_List*_tmp621;struct Cyc_Absyn_Pat*_tmp620;struct Cyc_Absyn_Exp*_tmp61F;void*_tmp61E;struct Cyc_Absyn_Vardecl*_tmp61D;switch(*((int*)_tmp60A)){case 0U: _LL2E: _tmp61D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp60A)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp61D,_tmp643);goto _LL2D;case 2U: _LL30: _tmp620=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60A)->f1;_tmp61F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60A)->f3;_tmp61E=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60A)->f4;_LL31:
# 3948
{void*_tmp60B=_tmp620->r;void*_tmp60C=_tmp60B;struct Cyc_Absyn_Vardecl*_tmp60E;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60C)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60C)->f2)->r)->tag == 0U){_LL3B: _tmp60E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60C)->f1;_LL3C:
# 3950
({struct _tuple1*_tmpAF0=Cyc_Toc_temp_var();_tmp60E->name=_tmpAF0;});
_tmp60E->initializer=_tmp61F;
({void*_tmpAF1=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->tag=0U,_tmp60D->f1=_tmp60E;_tmp60D;});_tmp644->r=_tmpAF1;});
Cyc_Toc_local_decl_to_c(nv,_tmp60E,_tmp643);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3959
({void*_tmpAF2=(Cyc_Toc_letdecl_to_c(nv,_tmp620,_tmp61E,(void*)_check_null(_tmp61F->topt),_tmp61F,_tmp643))->r;s->r=_tmpAF2;});
goto _LL3A;}_LL3A:;}
# 3962
goto _LL2D;case 3U: _LL32: _tmp621=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp60A)->f1;_LL33: {
# 3966
struct Cyc_List_List*_tmp60F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp621);
if(_tmp60F == 0)
({void*_tmp610=0U;({struct _dyneither_ptr _tmpAF3=({const char*_tmp611="empty Letv_d";_tag_dyneither(_tmp611,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF3,_tag_dyneither(_tmp610,sizeof(void*),0U));});});
({void*_tmpAF4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->tag=0U,_tmp612->f1=(struct Cyc_Absyn_Vardecl*)_tmp60F->hd;_tmp612;});_tmp644->r=_tmpAF4;});
_tmp60F=_tmp60F->tl;
for(0;_tmp60F != 0;_tmp60F=_tmp60F->tl){
struct Cyc_Absyn_Decl*_tmp613=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->tag=0U,_tmp614->f1=(struct Cyc_Absyn_Vardecl*)_tmp60F->hd;_tmp614;}),0U);
({void*_tmpAF6=({struct Cyc_Absyn_Decl*_tmpAF5=_tmp613;Cyc_Absyn_decl_stmt(_tmpAF5,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAF6;});}
# 3975
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp622=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp60A)->f1;_LL35:
# 3978
 Cyc_Toc_fndecl_to_c(nv,_tmp622,0);
Cyc_Toc_stmt_to_c(nv,_tmp643);
goto _LL2D;case 4U: _LL36: _tmp625=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60A)->f1;_tmp624=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60A)->f2;_tmp623=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60A)->f3;_LL37: {
# 3982
struct Cyc_Absyn_Stmt*_tmp615=_tmp643;
# 3984
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp624->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3991
Cyc_Toc_stmt_to_c(nv,_tmp615);
if(Cyc_Absyn_no_regions)
({void*_tmpAFA=({struct _tuple1*_tmpAF9=x_var;void*_tmpAF8=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAF7=
Cyc_Absyn_uint_exp(0U,0U);
# 3993
Cyc_Absyn_declare_stmt(_tmpAF9,_tmpAF8,_tmpAF7,_tmp615,0U);})->r;s->r=_tmpAFA;});else{
# 3995
if((unsigned int)_tmp623){
# 3997
Cyc_Toc_exp_to_c(nv,_tmp623);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpAFC=_tmp623;Cyc_Absyn_aggrarrow_exp(_tmpAFC,({struct _dyneither_ptr*_tmp617=_cycalloc(sizeof(*_tmp617));({struct _dyneither_ptr _tmpAFB=({const char*_tmp616="h";_tag_dyneither(_tmp616,sizeof(char),2U);});*_tmp617=_tmpAFB;});_tmp617;}),0U);}),0U);
({void*_tmpAFD=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp615,0U))->r;s->r=_tmpAFD;});};}else{
# 4007
({void*_tmpB0A=({
struct _tuple1*_tmpB09=rh_var;void*_tmpB08=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpB07=({
struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpAFF,({struct Cyc_List_List*_tmp618=_cycalloc(sizeof(*_tmp618));
({struct Cyc_Absyn_Exp*_tmpAFE=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp618->hd=_tmpAFE;}),_tmp618->tl=0;_tmp618;}),0U);});
# 4008
Cyc_Absyn_declare_stmt(_tmpB09,_tmpB08,_tmpB07,({
# 4012
struct _tuple1*_tmpB06=x_var;void*_tmpB05=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpB04=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpB06,_tmpB05,_tmpB04,({
struct Cyc_Absyn_Stmt*_tmpB03=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB00=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpB00,({struct Cyc_Absyn_Exp*_tmp619[1U];_tmp619[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp619,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpB03,({
struct Cyc_Absyn_Stmt*_tmpB02=_tmp615;Cyc_Absyn_seq_stmt(_tmpB02,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpB01,({struct Cyc_Absyn_Exp*_tmp61A[1U];_tmp61A[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61A,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 4007
s->r=_tmpB0A;});}}
# 4018
return;}default: _LL38: _LL39:
({void*_tmp61B=0U;({struct _dyneither_ptr _tmpB0B=({const char*_tmp61C="bad nested declaration within function";_tag_dyneither(_tmp61C,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB0B,_tag_dyneither(_tmp61B,sizeof(void*),0U));});});}_LL2D:;}
# 4021
return;case 13U: _LL1B: _tmp646=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp645=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_LL1C:
# 4023
 s=_tmp645;continue;case 14U: _LL1D: _tmp648=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp647=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2).f1;_LL1E: {
# 4025
struct _RegionHandle _tmp626=_new_region("temp");struct _RegionHandle*temp=& _tmp626;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB0C=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB0C,_tmp648);});
Cyc_Toc_exp_to_c(nv,_tmp647);
# 4029
_npop_handler(0U);return;
# 4025
;_pop_region(temp);}default: _LL1F: _tmp64B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f1;_tmp64A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f2;_tmp649=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F4)->f3;_LL20: {
# 4043 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4053
e_exp->topt=e_typ;{
struct _RegionHandle _tmp627=_new_region("temp");struct _RegionHandle*temp=& _tmp627;_push_region(temp);
# 4056
Cyc_Toc_stmt_to_c(nv,_tmp64B);{
struct Cyc_Absyn_Stmt*_tmp628=({struct Cyc_Absyn_Stmt*_tmpB0D=_tmp64B;Cyc_Absyn_seq_stmt(_tmpB0D,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4061
struct Cyc_Absyn_Stmt*_tmp629=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->tag=10U,_tmp630->f1=e_exp,_tmp630->f2=_tmp64A,_tmp630->f3=_tmp649;_tmp630;}),0U);
# 4063
Cyc_Toc_stmt_to_c(nv,_tmp629);{
# 4066
struct Cyc_Absyn_Exp*_tmp62A=({
struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB0F,({struct Cyc_List_List*_tmp62F=_cycalloc(sizeof(*_tmp62F));
({struct Cyc_Absyn_Exp*_tmpB0E=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp62F->hd=_tmpB0E;}),_tmp62F->tl=0;_tmp62F;}),0U);});
# 4070
struct Cyc_Absyn_Stmt*_tmp62B=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB11,({struct Cyc_List_List*_tmp62E=_cycalloc(sizeof(*_tmp62E));
({struct Cyc_Absyn_Exp*_tmpB10=Cyc_Absyn_address_exp(h_exp,0U);_tmp62E->hd=_tmpB10;}),_tmp62E->tl=0;_tmp62E;}),0U);}),0U);
# 4074
struct Cyc_Absyn_Exp*_tmp62C=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB20=({
struct _tuple1*_tmpB1F=h_var;void*_tmpB1E=h_typ;Cyc_Absyn_declare_stmt(_tmpB1F,_tmpB1E,0,({
struct Cyc_Absyn_Stmt*_tmpB1D=_tmp62B;Cyc_Absyn_seq_stmt(_tmpB1D,({
struct _tuple1*_tmpB1C=was_thrown_var;void*_tmpB1B=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB1A=_tmp62C;Cyc_Absyn_declare_stmt(_tmpB1C,_tmpB1B,_tmpB1A,({
struct Cyc_Absyn_Stmt*_tmpB19=({struct Cyc_Absyn_Exp*_tmpB13=_tmp62A;struct Cyc_Absyn_Stmt*_tmpB12=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp62D,0U);
# 4080
Cyc_Absyn_ifthenelse_stmt(_tmpB13,_tmpB12,
# 4082
Cyc_Toc_skip_stmt_dl(),0U);});
# 4080
Cyc_Absyn_seq_stmt(_tmpB19,({
# 4083
struct Cyc_Absyn_Exp*_tmpB18=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB17=_tmp628;Cyc_Absyn_ifthenelse_stmt(_tmpB18,_tmpB17,({
# 4085
struct _tuple1*_tmpB16=e_var;void*_tmpB15=e_typ;struct Cyc_Absyn_Exp*_tmpB14=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4085
Cyc_Absyn_declare_stmt(_tmpB16,_tmpB15,_tmpB14,_tmp629,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4076
s->r=_tmpB20;});};};
# 4089
_npop_handler(0U);return;
# 4054
;_pop_region(temp);};}}_LL0:;}}
# 4098
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB21=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB21;});{
struct _RegionHandle _tmp64C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp64C;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp64D=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp64E=f->args;for(0;_tmp64E != 0;_tmp64E=_tmp64E->tl){
struct _tuple1*_tmp64F=({struct _tuple1*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->f1=Cyc_Absyn_Loc_n,_tmp650->f2=(*((struct _tuple9*)_tmp64E->hd)).f1;_tmp650;});
({void*_tmpB22=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp64E->hd)).f3);(*((struct _tuple9*)_tmp64E->hd)).f3=_tmpB22;});}}
# 4114
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4118
({struct Cyc_Hashtable_Table**_tmpB24=({struct Cyc_Hashtable_Table**_tmp651=_cycalloc(sizeof(*_tmp651));({struct Cyc_Hashtable_Table*_tmpB23=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp651=_tmpB23;});_tmp651;});Cyc_Toc_fn_pop_table=_tmpB24;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp652=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp653=_tmp652;struct _dyneither_ptr*_tmp65C;struct Cyc_Absyn_Tqual _tmp65B;void*_tmp65A;int _tmp659;_LL1: _tmp65C=_tmp653.name;_tmp65B=_tmp653.tq;_tmp65A=_tmp653.type;_tmp659=_tmp653.inject;_LL2:;{
void*_tmp654=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp65A,(void*)& Cyc_Absyn_HeapRgn_val,_tmp65B,Cyc_Absyn_false_conref));
struct _tuple1*_tmp655=({struct _tuple1*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->f1=Cyc_Absyn_Loc_n,_tmp658->f2=(struct _dyneither_ptr*)_check_null(_tmp65C);_tmp658;});
({struct Cyc_List_List*_tmpB27=({struct Cyc_List_List*_tmpB26=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB26,({struct Cyc_List_List*_tmp657=_cycalloc(sizeof(*_tmp657));({struct _tuple9*_tmpB25=({struct _tuple9*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->f1=_tmp65C,_tmp656->f2=_tmp65B,_tmp656->f3=_tmp654;_tmp656;});_tmp657->hd=_tmpB25;}),_tmp657->tl=0;_tmp657;}));});f->args=_tmpB27;});
f->cyc_varargs=0;};}
# 4127
{struct Cyc_List_List*_tmp65D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp65D != 0;_tmp65D=_tmp65D->tl){
({void*_tmpB28=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp65D->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp65D->hd)->type=_tmpB28;});}}
# 4130
({struct Cyc_Toc_Env*_tmpB29=Cyc_Toc_clear_toplevel(frgn,_tmp64D);Cyc_Toc_stmt_to_c(_tmpB29,f->body);});}
# 4107
;_pop_region(frgn);};}
# 4134
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp65E=s;switch(_tmp65E){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple40{struct Cyc_Toc_TocState*f1;struct _tuple39*f2;};
# 4151 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4154
struct _tuple40 _tmp65F=*env;struct _tuple40 _tmp660=_tmp65F;struct Cyc_Toc_TocState*_tmp695;struct Cyc_Absyn_Aggrdecl*_tmp694;int _tmp693;_LL1: _tmp695=_tmp660.f1;_tmp694=(_tmp660.f2)->f1;_tmp693=(_tmp660.f2)->f2;_LL2:;{
struct _tuple1*_tmp661=_tmp694->name;
struct Cyc_Toc_TocState _tmp662=*_tmp695;struct Cyc_Toc_TocState _tmp663=_tmp662;struct Cyc_Dict_Dict*_tmp692;_LL4: _tmp692=_tmp663.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp664=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp692,_tmp661);
if(_tmp664 == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp694->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB2B=({struct _tuple19*_tmp665=_region_malloc(d,sizeof(*_tmp665));_tmp665->f1=_tmp694,({void*_tmpB2A=Cyc_Absyn_strctq(_tmp661);_tmp665->f2=_tmpB2A;});_tmp665;});v=_tmpB2B;});else{
# 4165
({struct _tuple19*_tmpB2D=({struct _tuple19*_tmp666=_region_malloc(d,sizeof(*_tmp666));_tmp666->f1=_tmp694,({void*_tmpB2C=Cyc_Absyn_unionq_typ(_tmp661);_tmp666->f2=_tmpB2C;});_tmp666;});v=_tmpB2D;});}
({struct Cyc_Dict_Dict _tmpB2E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp692,_tmp661,v);*_tmp692=_tmpB2E;});};}else{
# 4168
struct _tuple19*_tmp667=*_tmp664;struct _tuple19*_tmp668=_tmp667;struct Cyc_Absyn_Aggrdecl*_tmp66B;void*_tmp66A;_LL7: _tmp66B=_tmp668->f1;_tmp66A=_tmp668->f2;_LL8:;
if(_tmp66B->impl == 0){
({struct Cyc_Dict_Dict _tmpB31=({struct Cyc_Dict_Dict _tmpB30=*_tmp692;struct _tuple1*_tmpB2F=_tmp661;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB30,_tmpB2F,({struct _tuple19*_tmp669=_region_malloc(d,sizeof(*_tmp669));_tmp669->f1=_tmp694,_tmp669->f2=_tmp66A;_tmp669;}));});*_tmp692=_tmpB31;});
seen_defn_before=0;}else{
# 4173
seen_defn_before=1;}}{
# 4175
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->kind=_tmp694->kind,_tmp691->sc=Cyc_Absyn_Public,_tmp691->name=_tmp694->name,_tmp691->tvs=0,_tmp691->impl=0,_tmp691->expected_mem_kind=0,_tmp691->attributes=_tmp694->attributes;_tmp691;});
# 4182
if(_tmp694->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB32=({struct Cyc_Absyn_AggrdeclImpl*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->exist_vars=0,_tmp66C->rgn_po=0,_tmp66C->fields=0,_tmp66C->tagged=0;_tmp66C;});new_ad->impl=_tmpB32;});{
# 4187
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp66D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp694->impl))->fields;for(0;_tmp66D != 0;_tmp66D=_tmp66D->tl){
# 4191
struct Cyc_Absyn_Aggrfield*_tmp66E=(struct Cyc_Absyn_Aggrfield*)_tmp66D->hd;
void*_tmp66F=_tmp66E->type;
struct Cyc_List_List*_tmp670=_tmp66E->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp66F)) && (
_tmp694->kind == Cyc_Absyn_StructA  && _tmp66D->tl == 0  || _tmp694->kind == Cyc_Absyn_UnionA)){
# 4205 "toc.cyc"
void*_tmp671=Cyc_Tcutil_compress(_tmp66F);void*_tmp672=_tmp671;void*_tmp67A;struct Cyc_Absyn_Tqual _tmp679;union Cyc_Absyn_Constraint*_tmp678;unsigned int _tmp677;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->tag == 8U){_LLA: _tmp67A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).elt_type;_tmp679=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).tq;_tmp678=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).zero_term;_tmp677=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).zt_loc;_LLB:
# 4208
({void*_tmpB34=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->tag=8U,(_tmp673->f1).elt_type=_tmp67A,(_tmp673->f1).tq=_tmp679,({struct Cyc_Absyn_Exp*_tmpB33=Cyc_Absyn_uint_exp(0U,0U);(_tmp673->f1).num_elts=_tmpB33;}),(_tmp673->f1).zero_term=_tmp678,(_tmp673->f1).zt_loc=_tmp677;_tmp673;});_tmp66F=_tmpB34;});
goto _LL9;}else{_LLC: _LLD:
# 4211
({struct Cyc_List_List*_tmpB36=({struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));({void*_tmpB35=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->tag=6U,_tmp674->f1=0;_tmp674;});_tmp675->hd=_tmpB35;}),_tmp675->tl=_tmp670;_tmp675;});_tmp670=_tmpB36;});
({void*_tmpB3B=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->tag=8U,({void*_tmpB3A=Cyc_Absyn_void_star_typ();(_tmp676->f1).elt_type=_tmpB3A;}),({
struct Cyc_Absyn_Tqual _tmpB39=Cyc_Absyn_empty_tqual(0U);(_tmp676->f1).tq=_tmpB39;}),({
struct Cyc_Absyn_Exp*_tmpB38=Cyc_Absyn_uint_exp(0U,0U);(_tmp676->f1).num_elts=_tmpB38;}),({
union Cyc_Absyn_Constraint*_tmpB37=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp676->f1).zero_term=_tmpB37;}),(_tmp676->f1).zt_loc=0U;_tmp676;});
# 4212
_tmp66F=_tmpB3B;});}_LL9:;}{
# 4218
struct Cyc_Absyn_Aggrfield*_tmp67B=({struct Cyc_Absyn_Aggrfield*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->name=_tmp66E->name,_tmp68D->tq=Cyc_Toc_mt_tq,({
# 4220
void*_tmpB3C=Cyc_Toc_typ_to_c(_tmp66F);_tmp68D->type=_tmpB3C;}),_tmp68D->width=_tmp66E->width,_tmp68D->attributes=_tmp670,_tmp68D->requires_clause=0;_tmp68D;});
# 4228
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp694->impl))->tagged){
void*_tmp67C=_tmp67B->type;
struct _dyneither_ptr*_tmp67D=_tmp67B->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68A=({struct Cyc_String_pa_PrintArg_struct _tmp7A5;_tmp7A5.tag=0U,_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp694->name).f2);_tmp7A5;});struct Cyc_String_pa_PrintArg_struct _tmp68B=({struct Cyc_String_pa_PrintArg_struct _tmp7A4;_tmp7A4.tag=0U,_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp67D);_tmp7A4;});void*_tmp688[2U];_tmp688[0]=& _tmp68A,_tmp688[1]=& _tmp68B;({struct _dyneither_ptr _tmpB3D=({const char*_tmp689="_union_%s_%s";_tag_dyneither(_tmp689,sizeof(char),13U);});Cyc_aprintf(_tmpB3D,_tag_dyneither(_tmp688,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp687=_cycalloc(sizeof(*_tmp687));*_tmp687=s;_tmp687;});
struct Cyc_Absyn_Aggrfield*_tmp67E=({struct Cyc_Absyn_Aggrfield*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->name=Cyc_Toc_val_sp,_tmp686->tq=Cyc_Toc_mt_tq,_tmp686->type=_tmp67C,_tmp686->width=0,_tmp686->attributes=0,_tmp686->requires_clause=0;_tmp686;});
struct Cyc_Absyn_Aggrfield*_tmp67F=({struct Cyc_Absyn_Aggrfield*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->name=Cyc_Toc_tag_sp,_tmp685->tq=Cyc_Toc_mt_tq,_tmp685->type=Cyc_Absyn_sint_typ,_tmp685->width=0,_tmp685->attributes=0,_tmp685->requires_clause=0;_tmp685;});
struct Cyc_List_List*_tmp680=({struct Cyc_Absyn_Aggrfield*_tmp684[2U];_tmp684[0]=_tmp67F,_tmp684[1]=_tmp67E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp684,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp681=Cyc_Toc_make_c_struct_defn(str,_tmp680);
({struct Cyc_List_List*_tmpB3F=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));({struct Cyc_Absyn_Decl*_tmpB3E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->tag=5U,_tmp682->f1=_tmp681;_tmp682;}),0U);_tmp683->hd=_tmpB3E;}),_tmp683->tl=Cyc_Toc_result_decls;_tmp683;});Cyc_Toc_result_decls=_tmpB3F;});
({void*_tmpB40=Cyc_Absyn_strct(str);_tmp67B->type=_tmpB40;});}
# 4240
({struct Cyc_List_List*_tmpB41=({struct Cyc_List_List*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->hd=_tmp67B,_tmp68C->tl=new_fields;_tmp68C;});new_fields=_tmpB41;});};}}
# 4242
({struct Cyc_List_List*_tmpB42=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB42;});};}
# 4244
if(_tmp693)
({struct Cyc_List_List*_tmpB45=({struct Cyc_List_List*_tmp690=_cycalloc(sizeof(*_tmp690));({struct Cyc_Absyn_Decl*_tmpB44=({struct Cyc_Absyn_Decl*_tmp68F=_cycalloc(sizeof(*_tmp68F));({void*_tmpB43=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->tag=5U,_tmp68E->f1=new_ad;_tmp68E;});_tmp68F->r=_tmpB43;}),_tmp68F->loc=0U;_tmp68F;});_tmp690->hd=_tmpB44;}),_tmp690->tl=Cyc_Toc_result_decls;_tmp690;});Cyc_Toc_result_decls=_tmpB45;});
return 0;};};};}
# 4249
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple39 p=({struct _tuple39 _tmp7A6;_tmp7A6.f1=ad,_tmp7A6.f2=add_to_decls;_tmp7A6;});
((int(*)(struct _tuple39*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4278 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4281
struct _tuple41 _tmp696=*env;struct _tuple41 _tmp697=_tmp696;struct Cyc_Set_Set**_tmp6A8;struct Cyc_Absyn_Datatypedecl*_tmp6A7;_LL1: _tmp6A8=(_tmp697.f1)->datatypes_so_far;_tmp6A7=_tmp697.f2;_LL2:;{
struct _tuple1*_tmp698=_tmp6A7->name;
if(_tmp6A7->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp6A8,_tmp698))
return 0;
({struct Cyc_Set_Set*_tmpB46=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp6A8,_tmp698);*_tmp6A8=_tmpB46;});
{struct Cyc_List_List*_tmp699=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6A7->fields))->v;for(0;_tmp699 != 0;_tmp699=_tmp699->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp699->hd;
# 4289
struct Cyc_List_List*_tmp69A=0;
int i=1;
{struct Cyc_List_List*_tmp69B=f->typs;for(0;_tmp69B != 0;(_tmp69B=_tmp69B->tl,i ++)){
struct _dyneither_ptr*_tmp69C=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp69D=({struct Cyc_Absyn_Aggrfield*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->name=_tmp69C,_tmp69F->tq=(*((struct _tuple12*)_tmp69B->hd)).f1,({
void*_tmpB47=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp69B->hd)).f2);_tmp69F->type=_tmpB47;}),_tmp69F->width=0,_tmp69F->attributes=0,_tmp69F->requires_clause=0;_tmp69F;});
({struct Cyc_List_List*_tmpB48=({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->hd=_tmp69D,_tmp69E->tl=_tmp69A;_tmp69E;});_tmp69A=_tmpB48;});}}
# 4297
({struct Cyc_List_List*_tmpB4B=({struct Cyc_List_List*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({struct Cyc_Absyn_Aggrfield*_tmpB4A=({struct Cyc_Absyn_Aggrfield*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->name=Cyc_Toc_tag_sp,_tmp6A0->tq=Cyc_Toc_mt_tq,_tmp6A0->type=Cyc_Absyn_sint_typ,_tmp6A0->width=0,_tmp6A0->attributes=0,_tmp6A0->requires_clause=0;_tmp6A0;});_tmp6A1->hd=_tmpB4A;}),({
struct Cyc_List_List*_tmpB49=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp69A);_tmp6A1->tl=_tmpB49;});_tmp6A1;});
# 4297
_tmp69A=_tmpB4B;});{
# 4299
struct Cyc_Absyn_Aggrdecl*_tmp6A2=({struct _dyneither_ptr*_tmpB4D=({struct _dyneither_ptr*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));({struct _dyneither_ptr _tmpB4C=({const char*_tmp6A5="";_tag_dyneither(_tmp6A5,sizeof(char),1U);});*_tmp6A6=_tmpB4C;});_tmp6A6;});Cyc_Toc_make_c_struct_defn(_tmpB4D,_tmp69A);});
({struct _tuple1*_tmpB4E=Cyc_Toc_collapse_qvars(f->name,_tmp6A7->name);_tmp6A2->name=_tmpB4E;});
({struct Cyc_List_List*_tmpB50=({struct Cyc_List_List*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));({struct Cyc_Absyn_Decl*_tmpB4F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->tag=5U,_tmp6A3->f1=_tmp6A2;_tmp6A3;}),0U);_tmp6A4->hd=_tmpB4F;}),_tmp6A4->tl=Cyc_Toc_result_decls;_tmp6A4;});Cyc_Toc_result_decls=_tmpB50;});};}}
# 4303
return 0;};}
# 4306
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4325 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4328
struct _tuple41 _tmp6A9=*env;struct _tuple41 _tmp6AA=_tmp6A9;struct Cyc_Toc_TocState*_tmp6CC;struct Cyc_Absyn_Datatypedecl*_tmp6CB;_LL1: _tmp6CC=_tmp6AA.f1;_tmp6CB=_tmp6AA.f2;_LL2:;
if(_tmp6CB->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6AB=*_tmp6CC;struct Cyc_Toc_TocState _tmp6AC=_tmp6AB;struct Cyc_Dict_Dict*_tmp6CA;_LL4: _tmp6CA=_tmp6AC.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6AD=_tmp6CB->name;
{struct Cyc_List_List*_tmp6AE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6CB->fields))->v;for(0;_tmp6AE != 0;_tmp6AE=_tmp6AE->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6AE->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6B0=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp6AF,Cyc_Absyn_false_conref,0U);
# 4339
int*_tmp6B1=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6CA,f->name);int*_tmp6B2=_tmp6B1;if(_tmp6B2 == 0){_LL7: _LL8: {
# 4341
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB51=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB51;});{
# 4345
struct Cyc_Absyn_Vardecl*_tmp6B3=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B0,initopt);
_tmp6B3->sc=f->sc;
({struct Cyc_List_List*_tmpB53=({struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));({struct Cyc_Absyn_Decl*_tmpB52=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->tag=0U,_tmp6B4->f1=_tmp6B3;_tmp6B4;}),0U);_tmp6B5->hd=_tmpB52;}),_tmp6B5->tl=Cyc_Toc_result_decls;_tmp6B5;});Cyc_Toc_result_decls=_tmpB53;});
({struct Cyc_Dict_Dict _tmpB54=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6CA,f->name,f->sc != Cyc_Absyn_Extern);
# 4348
*_tmp6CA=_tmpB54;});{
# 4350
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6B6=f->typs;for(0;_tmp6B6 != 0;(_tmp6B6=_tmp6B6->tl,i ++)){
struct _dyneither_ptr*_tmp6B7=({struct _dyneither_ptr*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));({struct _dyneither_ptr _tmpB56=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6BD=({struct Cyc_Int_pa_PrintArg_struct _tmp7A7;_tmp7A7.tag=1U,_tmp7A7.f1=(unsigned long)i;_tmp7A7;});void*_tmp6BB[1U];_tmp6BB[0]=& _tmp6BD;({struct _dyneither_ptr _tmpB55=({const char*_tmp6BC="f%d";_tag_dyneither(_tmp6BC,sizeof(char),4U);});Cyc_aprintf(_tmpB55,_tag_dyneither(_tmp6BB,sizeof(void*),1U));});});*_tmp6BE=_tmpB56;});_tmp6BE;});
struct Cyc_Absyn_Aggrfield*_tmp6B8=({struct Cyc_Absyn_Aggrfield*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->name=_tmp6B7,_tmp6BA->tq=(*((struct _tuple12*)_tmp6B6->hd)).f1,({
void*_tmpB57=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6B6->hd)).f2);_tmp6BA->type=_tmpB57;}),_tmp6BA->width=0,_tmp6BA->attributes=0,_tmp6BA->requires_clause=0;_tmp6BA;});
({struct Cyc_List_List*_tmpB58=({struct Cyc_List_List*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=_tmp6B8,_tmp6B9->tl=fields;_tmp6B9;});fields=_tmpB58;});}}
# 4358
({struct Cyc_List_List*_tmpB5C=({struct Cyc_List_List*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));({struct Cyc_Absyn_Aggrfield*_tmpB5B=({struct Cyc_Absyn_Aggrfield*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->name=Cyc_Toc_tag_sp,_tmp6BF->tq=Cyc_Toc_mt_tq,({
void*_tmpB5A=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp6BF->type=_tmpB5A;}),_tmp6BF->width=0,_tmp6BF->attributes=0,_tmp6BF->requires_clause=0;_tmp6BF;});
# 4358
_tmp6C0->hd=_tmpB5B;}),({
# 4360
struct Cyc_List_List*_tmpB59=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C0->tl=_tmpB59;});_tmp6C0;});
# 4358
fields=_tmpB5C;});{
# 4361
struct Cyc_Absyn_Aggrdecl*_tmp6C1=({struct _dyneither_ptr*_tmpB5E=({struct _dyneither_ptr*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));({struct _dyneither_ptr _tmpB5D=({const char*_tmp6C4="";_tag_dyneither(_tmp6C4,sizeof(char),1U);});*_tmp6C5=_tmpB5D;});_tmp6C5;});Cyc_Toc_make_c_struct_defn(_tmpB5E,fields);});
({struct _tuple1*_tmpB5F=Cyc_Toc_collapse_qvars(f->name,_tmp6CB->name);_tmp6C1->name=_tmpB5F;});
({struct Cyc_List_List*_tmpB61=({struct Cyc_List_List*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));({struct Cyc_Absyn_Decl*_tmpB60=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->tag=5U,_tmp6C2->f1=_tmp6C1;_tmp6C2;}),0U);_tmp6C3->hd=_tmpB60;}),_tmp6C3->tl=Cyc_Toc_result_decls;_tmp6C3;});Cyc_Toc_result_decls=_tmpB61;});
goto _LL6;};};};}}else{if(*((int*)_tmp6B2)== 0){_LL9: _LLA:
# 4366
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6C6=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6C7=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B0,_tmp6C6);
_tmp6C7->sc=f->sc;
({struct Cyc_List_List*_tmpB63=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));({struct Cyc_Absyn_Decl*_tmpB62=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->tag=0U,_tmp6C8->f1=_tmp6C7;_tmp6C8;}),0U);_tmp6C9->hd=_tmpB62;}),_tmp6C9->tl=Cyc_Toc_result_decls;_tmp6C9;});Cyc_Toc_result_decls=_tmpB63;});
({struct Cyc_Dict_Dict _tmpB64=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6CA,f->name,1);*_tmp6CA=_tmpB64;});}
# 4373
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4377
return 0;};};}
# 4380
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4384
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4390
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB65=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB65;});
# 4394
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4403
struct _RegionHandle _tmp6CD=_new_region("temp");struct _RegionHandle*temp=& _tmp6CD;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6CE=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6CE,init);}
# 4404
;_pop_region(temp);}else{
# 4408
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4411
void*_tmp6CF=Cyc_Tcutil_compress(old_typ);void*_tmp6D0=_tmp6CF;void*_tmp6D7;struct Cyc_Absyn_Exp*_tmp6D6;union Cyc_Absyn_Constraint*_tmp6D5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D0)->tag == 8U){_LL1: _tmp6D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D0)->f1).elt_type;_tmp6D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D0)->f1).num_elts;_tmp6D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D0)->f1).zero_term;_LL2:
# 4413
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D5)){
if(_tmp6D6 == 0)
({void*_tmp6D1=0U;({struct _dyneither_ptr _tmpB66=({const char*_tmp6D2="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6D2,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB66,_tag_dyneither(_tmp6D1,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6D6;
struct Cyc_Absyn_Exp*_tmp6D3=({struct Cyc_Absyn_Exp*_tmpB68=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB68,({
struct Cyc_Absyn_Exp*_tmpB67=num_elts;Cyc_Absyn_add_exp(_tmpB67,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4420
struct Cyc_Absyn_Exp*_tmp6D4=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB6A=({struct Cyc_Absyn_Stmt*_tmpB69=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6D3,_tmp6D4,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB69,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4421
s->r=_tmpB6A;});};}
# 4424
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4432
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6D8=d;struct Cyc_List_List*_tmp6DF;struct Cyc_List_List*_tmp6DE;void**_tmp6DD;struct Cyc_Tcpat_Rhs*_tmp6DC;switch(*((int*)_tmp6D8)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6DC=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6D8)->f1;_LL4:
 _tmp6DC->rhs=success;return d;default: _LL5: _tmp6DF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6D8)->f1;_tmp6DE=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6D8)->f2;_tmp6DD=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6D8)->f3;_LL6:
# 4437
({void*_tmpB6B=Cyc_Toc_rewrite_decision(*_tmp6DD,success);*_tmp6DD=_tmpB6B;});
for(0;_tmp6DE != 0;_tmp6DE=_tmp6DE->tl){
struct _tuple33*_tmp6D9=(struct _tuple33*)_tmp6DE->hd;struct _tuple33*_tmp6DA=_tmp6D9;void**_tmp6DB;_LL8: _tmp6DB=(void**)& _tmp6DA->f2;_LL9:;
({void*_tmpB6C=Cyc_Toc_rewrite_decision(*_tmp6DB,success);*_tmp6DB=_tmpB6C;});}
# 4442
return d;}_LL0:;}
# 4453 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4455
struct _RegionHandle _tmp6E0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E0;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E1=Cyc_Toc_share_env(rgn,nv);
void*_tmp6E2=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6E1,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4465
struct Cyc_Absyn_Stmt*_tmp6E3=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4469
({void*_tmpB6D=Cyc_Toc_rewrite_decision(dopt,_tmp6E3);dopt=_tmpB6D;});{
# 4471
struct Cyc_Absyn_Switch_clause*_tmp6E4=({struct Cyc_Absyn_Switch_clause*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0->pattern=p,_tmp6F0->pat_vars=0,_tmp6F0->where_clause=0,_tmp6F0->body=_tmp6E3,_tmp6F0->loc=0U;_tmp6F0;});
struct Cyc_List_List*_tmp6E5=({struct _RegionHandle*_tmpB70=rgn;struct _RegionHandle*_tmpB6F=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB70,Cyc_Toc_gen_labels,_tmpB6F,({struct Cyc_Absyn_Switch_clause*_tmp6EF[1U];_tmp6EF[0]=_tmp6E4;({struct _RegionHandle*_tmpB6E=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB6E,_tag_dyneither(_tmp6EF,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4474
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4478
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6E1,& mydecls,& mybodies,dopt,_tmp6E5,v);
# 4481
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp6E6=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp6E7=_tmp6E6;struct Cyc_Toc_Env*_tmp6E9;struct Cyc_Absyn_Stmt*_tmp6E8;_LL1: _tmp6E9=_tmp6E7->f1;_tmp6E8=_tmp6E7->f3;_LL2:;
if(_tmp6E8 == _tmp6E3){senv=_tmp6E9;goto FOUND_ENV;}}
# 4486
({void*_tmp6EA=0U;({struct _dyneither_ptr _tmpB71=({const char*_tmp6EB="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6EB,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB71,_tag_dyneither(_tmp6EA,sizeof(void*),0U));});});
FOUND_ENV:
# 4490
 Cyc_Toc_stmt_to_c(senv,s);{
# 4492
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4494
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6EC=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB72=Cyc_Toc_temp_var();_tmp6EC->name=_tmpB72;});
({void*_tmpB73=Cyc_Toc_typ_to_c_array(_tmp6EC->type);_tmp6EC->type=_tmpB73;});
({struct Cyc_Absyn_Stmt*_tmpB75=({struct Cyc_Absyn_Decl*_tmpB74=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED->tag=0U,_tmp6ED->f1=_tmp6EC;_tmp6ED;}),0U);Cyc_Absyn_decl_stmt(_tmpB74,res,0U);});res=_tmpB75;});}
# 4501
({struct Cyc_Absyn_Stmt*_tmpB79=({struct _tuple1*_tmpB78=v;void*_tmpB77=Cyc_Toc_typ_to_c(_tmp6E2);struct Cyc_Absyn_Exp*_tmpB76=e;Cyc_Absyn_declare_stmt(_tmpB78,_tmpB77,_tmpB76,res,0U);});res=_tmpB79;});{
struct Cyc_Absyn_Stmt*_tmp6EE=res;_npop_handler(0U);return _tmp6EE;};};};};}
# 4456
;_pop_region(rgn);}
# 4509
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6F1=e->r;void*_tmp6F2=_tmp6F1;struct Cyc_Absyn_MallocInfo*_tmp71C;struct Cyc_Absyn_Stmt*_tmp71B;void**_tmp71A;void**_tmp719;struct Cyc_List_List*_tmp718;struct Cyc_List_List*_tmp717;void**_tmp716;struct Cyc_List_List*_tmp715;void**_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_List_List*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_List_List*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6F9;switch(*((int*)_tmp6F2)){case 20U: _LL1: _tmp6F9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL2:
 _tmp6FA=_tmp6F9;goto _LL4;case 21U: _LL3: _tmp6FA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL4:
 _tmp6FB=_tmp6FA;goto _LL6;case 22U: _LL5: _tmp6FB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL6:
 _tmp6FC=_tmp6FB;goto _LL8;case 15U: _LL7: _tmp6FC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL8:
 _tmp6FD=_tmp6FC;goto _LLA;case 11U: _LL9: _tmp6FD=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LLA:
 _tmp6FE=_tmp6FD;goto _LLC;case 12U: _LLB: _tmp6FE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LLC:
 _tmp6FF=_tmp6FE;goto _LLE;case 18U: _LLD: _tmp6FF=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LLE:
 _tmp700=_tmp6FF;goto _LL10;case 5U: _LLF: _tmp700=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp700);goto _LL0;case 3U: _LL11: _tmp701=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp701);goto _LL0;case 7U: _LL13: _tmp703=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp702=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL14:
 _tmp705=_tmp703;_tmp704=_tmp702;goto _LL16;case 8U: _LL15: _tmp705=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp704=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL16:
 _tmp707=_tmp705;_tmp706=_tmp704;goto _LL18;case 9U: _LL17: _tmp707=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp706=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL18:
 _tmp709=_tmp707;_tmp708=_tmp706;goto _LL1A;case 23U: _LL19: _tmp709=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp708=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL1A:
 _tmp70B=_tmp709;_tmp70A=_tmp708;goto _LL1C;case 35U: _LL1B: _tmp70B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp70A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL1C:
 _tmp70D=_tmp70B;_tmp70C=_tmp70A;goto _LL1E;case 4U: _LL1D: _tmp70D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp70C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp70D);Cyc_Toc_exptypes_to_c(_tmp70C);goto _LL0;case 6U: _LL1F: _tmp710=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp70F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_tmp70E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F2)->f3;_LL20:
# 4527
 Cyc_Toc_exptypes_to_c(_tmp710);Cyc_Toc_exptypes_to_c(_tmp70F);Cyc_Toc_exptypes_to_c(_tmp70E);goto _LL0;case 10U: _LL21: _tmp712=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp711=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL22:
# 4529
 Cyc_Toc_exptypes_to_c(_tmp712);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp711);goto _LL0;case 14U: _LL23: _tmp714=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_tmp713=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL24:
({void*_tmpB7A=Cyc_Toc_typ_to_c(*_tmp714);*_tmp714=_tmpB7A;});Cyc_Toc_exptypes_to_c(_tmp713);goto _LL0;case 25U: _LL25: _tmp716=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1)->f3;_tmp715=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL26:
# 4532
({void*_tmpB7B=Cyc_Toc_typ_to_c(*_tmp716);*_tmp716=_tmpB7B;});
_tmp717=_tmp715;goto _LL28;case 36U: _LL27: _tmp717=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6F2)->f2;_LL28:
 _tmp718=_tmp717;goto _LL2A;case 26U: _LL29: _tmp718=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL2A:
# 4536
 for(0;_tmp718 != 0;_tmp718=_tmp718->tl){
struct _tuple20 _tmp6F3=*((struct _tuple20*)_tmp718->hd);struct _tuple20 _tmp6F4=_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F5;_LL54: _tmp6F5=_tmp6F4.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp6F5);}
# 4540
goto _LL0;case 19U: _LL2B: _tmp719=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL2C:
 _tmp71A=_tmp719;goto _LL2E;case 17U: _LL2D: _tmp71A=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL2E:
({void*_tmpB7C=Cyc_Toc_typ_to_c(*_tmp71A);*_tmp71A=_tmpB7C;});goto _LL0;case 37U: _LL2F: _tmp71B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp71B);goto _LL0;case 34U: _LL31: _tmp71C=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6F2)->f1;_LL32:
# 4545
 if(_tmp71C->elt_type != 0)
({void**_tmpB7E=({void**_tmp6F6=_cycalloc(sizeof(*_tmp6F6));({void*_tmpB7D=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp71C->elt_type)));*_tmp6F6=_tmpB7D;});_tmp6F6;});_tmp71C->elt_type=_tmpB7E;});
Cyc_Toc_exptypes_to_c(_tmp71C->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 4555
 goto _LL40;case 30U: _LL3F: _LL40:
 goto _LL42;case 31U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 27U: _LL45: _LL46:
 goto _LL48;case 28U: _LL47: _LL48:
 goto _LL4A;case 24U: _LL49: _LL4A:
 goto _LL4C;case 13U: _LL4B: _LL4C:
 goto _LL4E;case 16U: _LL4D: _LL4E:
 goto _LL50;case 39U: _LL4F: _LL50:
 goto _LL52;default: _LL51: _LL52:
({void*_tmp6F7=0U;({unsigned int _tmpB80=e->loc;struct _dyneither_ptr _tmpB7F=({const char*_tmp6F8="Cyclone expression within C code";_tag_dyneither(_tmp6F8,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpB80,_tmpB7F,_tag_dyneither(_tmp6F7,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4569
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp71D=d->r;void*_tmp71E=_tmp71D;struct Cyc_Absyn_Typedefdecl*_tmp728;struct Cyc_Absyn_Enumdecl*_tmp727;struct Cyc_Absyn_Aggrdecl*_tmp726;struct Cyc_Absyn_Fndecl*_tmp725;struct Cyc_Absyn_Vardecl*_tmp724;switch(*((int*)_tmp71E)){case 0U: _LL1: _tmp724=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL2:
# 4572
({void*_tmpB81=Cyc_Toc_typ_to_c(_tmp724->type);_tmp724->type=_tmpB81;});
if(_tmp724->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp724->initializer));
goto _LL0;case 1U: _LL3: _tmp725=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL4:
# 4576
({void*_tmpB82=Cyc_Toc_typ_to_c(_tmp725->ret_type);_tmp725->ret_type=_tmpB82;});
{struct Cyc_List_List*_tmp71F=_tmp725->args;for(0;_tmp71F != 0;_tmp71F=_tmp71F->tl){
({void*_tmpB83=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp71F->hd)).f3);(*((struct _tuple9*)_tmp71F->hd)).f3=_tmpB83;});}}
# 4580
goto _LL0;case 5U: _LL5: _tmp726=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp726,1);goto _LL0;case 7U: _LL7: _tmp727=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL8:
# 4583
 if(_tmp727->fields != 0){
struct Cyc_List_List*_tmp720=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp727->fields))->v;for(0;_tmp720 != 0;_tmp720=_tmp720->tl){
struct Cyc_Absyn_Enumfield*_tmp721=(struct Cyc_Absyn_Enumfield*)_tmp720->hd;
if(_tmp721->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp721->tag));}}
# 4588
goto _LL0;case 8U: _LL9: _tmp728=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LLA:
({void*_tmpB84=Cyc_Toc_typ_to_c((void*)_check_null(_tmp728->defn));_tmp728->defn=_tmpB84;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4598
({void*_tmp722=0U;({unsigned int _tmpB86=d->loc;struct _dyneither_ptr _tmpB85=({const char*_tmp723="Cyclone declaration within C code";_tag_dyneither(_tmp723,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpB86,_tmpB85,_tag_dyneither(_tmp722,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4602
 goto _LL0;}_LL0:;}
# 4606
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp729=s->r;void*_tmp72A=_tmp729;struct Cyc_Absyn_Stmt*_tmp744;struct Cyc_Absyn_Stmt*_tmp743;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Decl*_tmp741;struct Cyc_Absyn_Stmt*_tmp740;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_List_List*_tmp73E;void*_tmp73D;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Exp*_tmp73B;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Stmt*_tmp739;struct Cyc_Absyn_Exp*_tmp738;struct Cyc_Absyn_Stmt*_tmp737;struct Cyc_Absyn_Exp*_tmp736;struct Cyc_Absyn_Stmt*_tmp735;struct Cyc_Absyn_Stmt*_tmp734;struct Cyc_Absyn_Exp*_tmp733;struct Cyc_Absyn_Stmt*_tmp732;struct Cyc_Absyn_Stmt*_tmp731;struct Cyc_Absyn_Exp*_tmp730;switch(*((int*)_tmp72A)){case 1U: _LL1: _tmp730=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp730);goto _LL0;case 2U: _LL3: _tmp732=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp731=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp732);Cyc_Toc_stmttypes_to_c(_tmp731);goto _LL0;case 3U: _LL5: _tmp733=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_LL6:
 if(_tmp733 != 0)Cyc_Toc_exptypes_to_c(_tmp733);goto _LL0;case 4U: _LL7: _tmp736=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp735=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_tmp734=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72A)->f3;_LL8:
# 4612
 Cyc_Toc_exptypes_to_c(_tmp736);Cyc_Toc_stmttypes_to_c(_tmp735);Cyc_Toc_stmttypes_to_c(_tmp734);goto _LL0;case 5U: _LL9: _tmp738=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1).f1;_tmp737=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LLA:
# 4614
 Cyc_Toc_exptypes_to_c(_tmp738);Cyc_Toc_stmttypes_to_c(_tmp737);goto _LL0;case 9U: _LLB: _tmp73C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp73B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2).f1;_tmp73A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f3).f1;_tmp739=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f4;_LLC:
# 4616
 Cyc_Toc_exptypes_to_c(_tmp73C);Cyc_Toc_exptypes_to_c(_tmp73B);Cyc_Toc_exptypes_to_c(_tmp73A);
Cyc_Toc_stmttypes_to_c(_tmp739);goto _LL0;case 10U: _LLD: _tmp73F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp73E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_tmp73D=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72A)->f3;_LLE:
# 4619
 Cyc_Toc_exptypes_to_c(_tmp73F);
for(0;_tmp73E != 0;_tmp73E=_tmp73E->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp73E->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp741=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp740=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp741);Cyc_Toc_stmttypes_to_c(_tmp740);goto _LL0;case 14U: _LL11: _tmp743=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp742=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp743);Cyc_Toc_exptypes_to_c(_tmp742);goto _LL0;case 13U: _LL13: _tmp744=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp744);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4629
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4632
({void*_tmpB87=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B->tag=0U;_tmp72B;});s->r=_tmpB87;});
goto _LL0;default: _LL1F: _LL20:
# 4635
({void*_tmp72C=0U;({unsigned int _tmpB8B=s->loc;struct _dyneither_ptr _tmpB8A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp72F=({struct Cyc_String_pa_PrintArg_struct _tmp7A8;_tmp7A8.tag=0U,({struct _dyneither_ptr _tmpB88=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp7A8.f1=_tmpB88;});_tmp7A8;});void*_tmp72D[1U];_tmp72D[0]=& _tmp72F;({struct _dyneither_ptr _tmpB89=({const char*_tmp72E="Cyclone statement in C code: %s";_tag_dyneither(_tmp72E,sizeof(char),32U);});Cyc_aprintf(_tmpB89,_tag_dyneither(_tmp72D,sizeof(void*),1U));});});Cyc_Tcutil_terr(_tmpB8B,_tmpB8A,_tag_dyneither(_tmp72C,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4643
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp745=0U;({struct _dyneither_ptr _tmpB8C=({const char*_tmp746="decls_to_c: not at toplevel!";_tag_dyneither(_tmp746,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB8C,_tag_dyneither(_tmp745,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp747=d->r;void*_tmp748=_tmp747;struct Cyc_List_List*_tmp760;struct Cyc_List_List*_tmp75F;struct Cyc_List_List*_tmp75E;struct Cyc_List_List*_tmp75D;struct Cyc_Absyn_Typedefdecl*_tmp75C;struct Cyc_Absyn_Enumdecl*_tmp75B;struct Cyc_Absyn_Datatypedecl*_tmp75A;struct Cyc_Absyn_Aggrdecl*_tmp759;struct Cyc_Absyn_Fndecl*_tmp758;struct Cyc_Absyn_Vardecl*_tmp757;switch(*((int*)_tmp748)){case 0U: _LL1: _tmp757=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LL2: {
# 4651
struct _tuple1*_tmp749=_tmp757->name;
# 4653
if(_tmp757->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB8E=({struct _tuple1*_tmp74A=_cycalloc(sizeof(*_tmp74A));({union Cyc_Absyn_Nmspace _tmpB8D=Cyc_Absyn_Abs_n(0,1);_tmp74A->f1=_tmpB8D;}),_tmp74A->f2=(*_tmp749).f2;_tmp74A;});_tmp749=_tmpB8E;});
if(_tmp757->initializer != 0){
if(_tmp757->sc == Cyc_Absyn_ExternC)_tmp757->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp757->initializer));else{
# 4660
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp757->initializer));}}
# 4662
_tmp757->name=_tmp749;
({enum Cyc_Absyn_Scope _tmpB8F=Cyc_Toc_scope_to_c(_tmp757->sc);_tmp757->sc=_tmpB8F;});
({void*_tmpB90=Cyc_Toc_typ_to_c(_tmp757->type);_tmp757->type=_tmpB90;});
({struct Cyc_List_List*_tmpB91=({struct Cyc_List_List*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B->hd=d,_tmp74B->tl=Cyc_Toc_result_decls;_tmp74B;});Cyc_Toc_result_decls=_tmpB91;});
goto _LL0;}case 1U: _LL3: _tmp758=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LL4: {
# 4668
struct _tuple1*_tmp74C=_tmp758->name;
# 4670
if(_tmp758->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB93=({struct _tuple1*_tmp74D=_cycalloc(sizeof(*_tmp74D));({union Cyc_Absyn_Nmspace _tmpB92=Cyc_Absyn_Abs_n(0,1);_tmp74D->f1=_tmpB92;}),_tmp74D->f2=(*_tmp74C).f2;_tmp74D;});_tmp74C=_tmpB93;});
_tmp758->sc=Cyc_Absyn_Public;}
# 4674
_tmp758->name=_tmp74C;
Cyc_Toc_fndecl_to_c(nv,_tmp758,cinclude);
({struct Cyc_List_List*_tmpB94=({struct Cyc_List_List*_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E->hd=d,_tmp74E->tl=Cyc_Toc_result_decls;_tmp74E;});Cyc_Toc_result_decls=_tmpB94;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp74F=0U;({struct _dyneither_ptr _tmpB95=({const char*_tmp750="letdecl at toplevel";_tag_dyneither(_tmp750,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB95,_tag_dyneither(_tmp74F,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp751=0U;({struct _dyneither_ptr _tmpB96=({const char*_tmp752="region decl at toplevel";_tag_dyneither(_tmp752,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB96,_tag_dyneither(_tmp751,sizeof(void*),0U));});});case 5U: _LLB: _tmp759=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LLC:
# 4682
 Cyc_Toc_aggrdecl_to_c(_tmp759,1);
goto _LL0;case 6U: _LLD: _tmp75A=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LLE:
# 4685
 if(_tmp75A->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp75A);else{
# 4688
Cyc_Toc_datatypedecl_to_c(_tmp75A);}
goto _LL0;case 7U: _LLF: _tmp75B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LL10:
# 4691
 Cyc_Toc_enumdecl_to_c(_tmp75B);
({struct Cyc_List_List*_tmpB97=({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753->hd=d,_tmp753->tl=Cyc_Toc_result_decls;_tmp753;});Cyc_Toc_result_decls=_tmpB97;});
goto _LL0;case 8U: _LL11: _tmp75C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LL12:
# 4695
 _tmp75C->tvs=0;
if(_tmp75C->defn != 0)
({void*_tmpB98=Cyc_Toc_typ_to_c((void*)_check_null(_tmp75C->defn));_tmp75C->defn=_tmpB98;});else{
# 4699
enum Cyc_Absyn_KindQual _tmp754=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp75C->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp755=_tmp754;if(_tmp755 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB99=Cyc_Absyn_void_star_typ();_tmp75C->defn=_tmpB99;});goto _LL1F;}else{_LL22: _LL23:
 _tmp75C->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 4707
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB9A=({struct Cyc_List_List*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->hd=d,_tmp756->tl=Cyc_Toc_result_decls;_tmp756;});Cyc_Toc_result_decls=_tmpB9A;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp75D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp748)->f2;_LL18:
 _tmp75E=_tmp75D;goto _LL1A;case 10U: _LL19: _tmp75E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp748)->f2;_LL1A:
 _tmp75F=_tmp75E;goto _LL1C;case 11U: _LL1B: _tmp75F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB9B=Cyc_Toc_decls_to_c(r,nv,_tmp75F,top,cinclude);nv=_tmpB9B;});goto _LL0;default: _LL1D: _tmp760=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp748)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpB9C=Cyc_Toc_decls_to_c(r,nv,_tmp760,top,1);nv=_tmpB9C;});goto _LL0;}_LL0:;};}
# 4718
return nv;}
# 4722
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp761=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp762=_tmp761;struct Cyc_Core_DynamicRegion*_tmp767;_LL1: _tmp767=_tmp762.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp767->h;
({struct Cyc_Toc_TocState*_tmpB9D=Cyc_Toc_empty_toc_state(h);ts=_tmpB9D;});;}
({struct Cyc_Toc_TocStateWrap*_tmpB9E=({struct Cyc_Toc_TocStateWrap*_tmp764=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp764));_tmp764->dyn=_tmp767,_tmp764->state=ts;_tmp764;});Cyc_Toc_toc_state=_tmpB9E;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpB9F=_tag_dyneither(({unsigned int _tmp766=35;struct _dyneither_ptr**_tmp765=_cycalloc(_check_times(_tmp766,sizeof(struct _dyneither_ptr*)));_tmp765[0]=& Cyc_Toc__throw_str,_tmp765[1]=& Cyc_Toc_setjmp_str,_tmp765[2]=& Cyc_Toc__push_handler_str,_tmp765[3]=& Cyc_Toc__pop_handler_str,_tmp765[4]=& Cyc_Toc__exn_thrown_str,_tmp765[5]=& Cyc_Toc__npop_handler_str,_tmp765[6]=& Cyc_Toc__check_null_str,_tmp765[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp765[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp765[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp765[10]=& Cyc_Toc__dyneither_ptr_str,_tmp765[11]=& Cyc_Toc__tag_dyneither_str,_tmp765[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp765[13]=& Cyc_Toc__get_dyneither_size_str,_tmp765[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp765[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp765[16]=& Cyc_Toc__zero_arr_plus_str,_tmp765[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp765[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp765[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp765[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp765[21]=& Cyc_Toc__cycalloc_str,_tmp765[22]=& Cyc_Toc__cyccalloc_str,_tmp765[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp765[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp765[25]=& Cyc_Toc__region_malloc_str,_tmp765[26]=& Cyc_Toc__region_calloc_str,_tmp765[27]=& Cyc_Toc__check_times_str,_tmp765[28]=& Cyc_Toc__new_region_str,_tmp765[29]=& Cyc_Toc__push_region_str,_tmp765[30]=& Cyc_Toc__pop_region_str,_tmp765[31]=& Cyc_Toc__throw_arraybounds_str,_tmp765[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp765[33]=& Cyc_Toc__throw_match_str,_tmp765[34]=& Cyc_Toc__fast_region_malloc_str;_tmp765;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpB9F;});};}
# 4773
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp768=ts;struct Cyc_Toc_TocStateWrap*_tmp769=Cyc_Toc_toc_state;ts=_tmp769;Cyc_Toc_toc_state=_tmp768;});{
struct Cyc_Toc_TocStateWrap _tmp76A=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp76B=_tmp76A;struct Cyc_Core_DynamicRegion*_tmp771;struct Cyc_Toc_TocState*_tmp770;_LL1: _tmp771=_tmp76B.dyn;_tmp770=_tmp76B.state;_LL2:;
# 4778
{struct _RegionHandle*h=& _tmp771->h;
{struct Cyc_Toc_TocState _tmp76D=*_tmp770;struct Cyc_Toc_TocState _tmp76E=_tmp76D;struct Cyc_Xarray_Xarray*_tmp76F;_LL4: _tmp76F=_tmp76E.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp76F);}
# 4779
;}
# 4782
Cyc_Core_free_rckey(_tmp771);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4785
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4791
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4793
({struct Cyc_Hashtable_Table**_tmpBA0=({struct Cyc_Hashtable_Table**_tmp772=_cycalloc(sizeof(*_tmp772));*_tmp772=pop_tables;_tmp772;});Cyc_Toc_gpop_tables=_tmpBA0;});
Cyc_Toc_init();
{struct _RegionHandle _tmp773=_new_region("start");struct _RegionHandle*start=& _tmp773;_push_region(start);
({struct _RegionHandle*_tmpBA2=start;struct Cyc_Toc_Env*_tmpBA1=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpBA2,_tmpBA1,ds,1,0);});;_pop_region(start);}
# 4802
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

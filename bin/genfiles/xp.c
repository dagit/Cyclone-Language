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

# 1 "xp.cyc"
 static char Cyc_rcsid[49U]="$Id: xp.c,v 1.16 2004-08-17 16:34:46 gwprice Exp $";
# 4 "ctype.h"
int isalnum(int);
# 22
int isspace(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 282 "cycboot.h"
int isalnum(int);
# 300
int isspace(int);
# 81 "string.h"
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
# 86
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);
# 8 "xp.h"
int Cyc_XP_add(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y,int carry);
int Cyc_XP_sub(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y,int borrow);
int Cyc_XP_mul(struct _dyneither_ptr z,int n,struct _dyneither_ptr x,int m,struct _dyneither_ptr y);
int Cyc_XP_div(int n,struct _dyneither_ptr q,struct _dyneither_ptr x,int m,struct _dyneither_ptr y,struct _dyneither_ptr r,struct _dyneither_ptr tmp);
int Cyc_XP_sum(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y);
int Cyc_XP_diff(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y);
int Cyc_XP_product(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y);
int Cyc_XP_quotient(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y);
int Cyc_XP_neg(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int carry);
int Cyc_XP_cmp(int n,struct _dyneither_ptr x,struct _dyneither_ptr y);
void Cyc_XP_lshift(int n,struct _dyneither_ptr z,int m,struct _dyneither_ptr x,int s,int fill);
# 20
void Cyc_XP_rshift(int n,struct _dyneither_ptr z,int m,struct _dyneither_ptr x,int s,int fill);
# 22
void Cyc_XP_and(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y);
void Cyc_XP_or(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y);
void Cyc_XP_xor(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y);
int Cyc_XP_length(int n,struct _dyneither_ptr x);
unsigned long Cyc_XP_fromint(int n,struct _dyneither_ptr z,unsigned long u);
# 28
unsigned long Cyc_XP_toint(int n,struct _dyneither_ptr x);
int Cyc_XP_fromstr(int n,struct _dyneither_ptr z,const char*str,int base);
# 31
struct _dyneither_ptr Cyc_XP_tostr(struct _dyneither_ptr str,int size,int base,int n,struct _dyneither_ptr x);static char _tmp0[37U]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
# 8 "xp.cyc"
static struct _dyneither_ptr Cyc_digits={_tmp0,_tmp0,_tmp0 + 37U};
static char Cyc_map[75U]={'\000','\001','\002','\003','\004','\005','\006','\a','\b','\t','$','$','$','$','$','$','$','\n','\v','\f','\r','\016','\017','\020','\021','\022','\023','\024','\025','\026','\027','\030','\031','\032','\033','\034','\035','\036','\037',' ','!','"','#','$','$','$','$','$','$','\n','\v','\f','\r','\016','\017','\020','\021','\022','\023','\024','\025','\026','\027','\030','\031','\032','\033','\034','\035','\036','\037',' ','!','"','#'};
# 18
unsigned long Cyc_XP_fromint(int n,struct _dyneither_ptr z,unsigned long u){
int i=0;
do{
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i ++))=(unsigned char)(u % (unsigned long)(1 << 8));}while(
(u /=(unsigned long)(1 << 8))> (unsigned long)0  && i < n);
for(0;i < n;++ i){
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))='\000';}
return u;}
# 27
unsigned long Cyc_XP_toint(int n,struct _dyneither_ptr x){
unsigned long u=0U;
int i=(int)sizeof(u);
if(i > n)
i=n;
while(-- i >= 0){
u=(unsigned long)(1 << 8)* u + (unsigned long)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));}
return u;}
# 36
int Cyc_XP_length(int n,struct _dyneither_ptr x){
while(n > 1  && (int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),n - 1))== 0){
-- n;}
return n;}
# 41
int Cyc_XP_add(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y,int carry){
int i;
for(i=0;i < n;++ i){
({int _tmp24=({int _tmp23=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp23 + (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));});carry +=_tmp24;});
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry % (1 << 8));
carry /=1 << 8;}
# 48
return carry;}
# 50
int Cyc_XP_sub(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y,int borrow){
int i;
for(i=0;i < n;++ i){
int d=({int _tmp25=((int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i))+ (1 << 8))- borrow;_tmp25 - (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));});
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(d % (1 << 8));
borrow=1 - d / (1 << 8);}
# 57
return borrow;}
# 59
int Cyc_XP_sum(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y){
int i;
for(i=0;i < n;++ i){
y +=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(y % (1 << 8));
y /=1 << 8;}
# 66
return y;}
# 68
int Cyc_XP_diff(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y){
int i;
for(i=0;i < n;++ i){
int d=((int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i))+ (1 << 8))- y;
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(d % (1 << 8));
y=1 - d / (1 << 8);}
# 75
return y;}
# 77
int Cyc_XP_neg(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int carry){
int i;
for(i=0;i < n;++ i){
carry +=(int)~(*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i)));
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry % (1 << 8));
carry /=1 << 8;}
# 84
return carry;}
# 86
int Cyc_XP_mul(struct _dyneither_ptr z,int n,struct _dyneither_ptr x,int m,struct _dyneither_ptr y){
int i;int j;int carryout=0;
for(i=0;i < n;++ i){
unsigned int carry=0U;
for(j=0;j < m;++ j){
({unsigned int _tmp28=(unsigned int)({int _tmp27=({int _tmp26=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp26 * (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),j));});_tmp27 + (int)*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i + j));});carry +=_tmp28;});
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i + j))=(unsigned char)(carry % (unsigned int)(1 << 8));
carry /=(unsigned int)(1 << 8);}
# 95
for(0;j < (n + m)- i;++ j){
carry +=(unsigned int)*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i + j));
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i + j))=(unsigned char)(carry % (unsigned int)(1 << 8));
carry /=(unsigned int)(1 << 8);}
# 100
carryout |=carry;}
# 102
return carryout;}
# 104
int Cyc_XP_product(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y){
int i;
unsigned int carry=0U;
for(i=0;i < n;++ i){
carry +=(unsigned int)((int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i))* y);
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry % (unsigned int)(1 << 8));
carry /=(unsigned int)(1 << 8);}
# 112
return(int)carry;}
# 114
int Cyc_XP_div(int n,struct _dyneither_ptr q,struct _dyneither_ptr x,int m,struct _dyneither_ptr y,struct _dyneither_ptr r,struct _dyneither_ptr tmp){
int nx=n;int my=m;
n=Cyc_XP_length(n,x);
m=Cyc_XP_length(m,y);
if(m == 1){
if((int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),0))== 0)
return 0;
({unsigned char _tmp29=(unsigned char)Cyc_XP_quotient(nx,q,x,(int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),0)));*((unsigned char*)_check_dyneither_subscript(r,sizeof(unsigned char),0))=_tmp29;});
if(_get_dyneither_size(r,sizeof(unsigned char))> (unsigned int)1)
({struct _dyneither_ptr _tmp2A=_dyneither_ptr_plus((struct _dyneither_ptr)r,sizeof(char),1);Cyc_memset(_tmp2A,'\000',(unsigned long)(my - 1));});}else{
if(m > n){
Cyc_memset((struct _dyneither_ptr)q,'\000',(unsigned long)nx);
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int))Cyc__memcpy)(r,(struct _dyneither_ptr)x,(unsigned int)n / sizeof(*((unsigned char*)x.curr))+ (unsigned int)((unsigned int)n % sizeof(*((unsigned char*)x.curr))== (unsigned int)0?0: 1),sizeof(*((unsigned char*)x.curr)));
({struct _dyneither_ptr _tmp2B=_dyneither_ptr_plus((struct _dyneither_ptr)r,sizeof(char),n);Cyc_memset(_tmp2B,'\000',(unsigned long)(my - n));});}else{
# 129
int k;
struct _dyneither_ptr rem=tmp;struct _dyneither_ptr dq=_dyneither_ptr_plus(_dyneither_ptr_plus(tmp,sizeof(unsigned char),n),sizeof(unsigned char),1);
2 <= m  && m <= n?0:({struct _dyneither_ptr _tmp2C=({const char*_tmp1="2 <= m && m <= n";_tag_dyneither(_tmp1,sizeof(char),17U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp2C,({const char*_tmp2="xp.cyc";_tag_dyneither(_tmp2,sizeof(char),7U);}),131U);});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int))Cyc__memcpy)(rem,(struct _dyneither_ptr)x,(unsigned int)n / sizeof(*((unsigned char*)x.curr))+ (unsigned int)((unsigned int)n % sizeof(*((unsigned char*)x.curr))== (unsigned int)0?0: 1),sizeof(*((unsigned char*)x.curr)));
*((unsigned char*)_check_dyneither_subscript(rem,sizeof(unsigned char),n))='\000';
for(k=n - m;k >= 0;-- k){
int qk;
{
int i;
(2 <= m  && m <= k + m) && k + m <= n?0:({struct _dyneither_ptr _tmp2D=({const char*_tmp3="2 <= m && m <= k+m && k+m <= n";_tag_dyneither(_tmp3,sizeof(char),31U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp2D,({const char*_tmp4="xp.cyc";_tag_dyneither(_tmp4,sizeof(char),7U);}),138U);});
{
int km=k + m;
unsigned long y2=(unsigned long)({int _tmp2E=(int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),m - 1))* (1 << 8);_tmp2E + (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),m - 2));});
unsigned long r3=(unsigned long)({int _tmp30=({int _tmp2F=(int)*((unsigned char*)_check_dyneither_subscript(rem,sizeof(unsigned char),km))* ((1 << 8)* (1 << 8));_tmp2F + (int)*((unsigned char*)_check_dyneither_subscript(rem,sizeof(unsigned char),km - 1))* (1 << 8);});_tmp30 + (int)*((unsigned char*)_check_dyneither_subscript(rem,sizeof(unsigned char),km - 2));});
# 144
qk=(int)(r3 / y2);
if(qk >= 1 << 8)
qk=255U;}
# 148
({unsigned char _tmp31=(unsigned char)Cyc_XP_product(m,dq,y,qk);*((unsigned char*)_check_dyneither_subscript(dq,sizeof(unsigned char),m))=_tmp31;});
for(i=m;i > 0;-- i){
if(({int _tmp32=(int)*((unsigned char*)_check_dyneither_subscript(rem,sizeof(unsigned char),i + k));_tmp32 != (int)*((unsigned char*)_check_dyneither_subscript(dq,sizeof(unsigned char),i));}))
break;}
if(({int _tmp33=(int)*((unsigned char*)_check_dyneither_subscript(rem,sizeof(unsigned char),i + k));_tmp33 < (int)*((unsigned char*)_check_dyneither_subscript(dq,sizeof(unsigned char),i));}))
({unsigned char _tmp34=(unsigned char)Cyc_XP_product(m,dq,y,-- qk);*((unsigned char*)_check_dyneither_subscript(dq,sizeof(unsigned char),m))=_tmp34;});}
# 155
*((unsigned char*)_check_dyneither_subscript(q,sizeof(unsigned char),k))=(unsigned char)qk;{
# 157
int borrow;
0 <= k  && k <= k + m?0:({struct _dyneither_ptr _tmp35=({const char*_tmp5="0 <= k && k <= k+m";_tag_dyneither(_tmp5,sizeof(char),19U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp35,({const char*_tmp6="xp.cyc";_tag_dyneither(_tmp6,sizeof(char),7U);}),158U);});
borrow=({int _tmp38=m + 1;struct _dyneither_ptr _tmp37=_dyneither_ptr_plus(rem,sizeof(unsigned char),k);struct _dyneither_ptr _tmp36=_dyneither_ptr_plus(rem,sizeof(unsigned char),k);Cyc_XP_sub(_tmp38,_tmp37,_tmp36,dq,0);});
borrow == 0?0:({struct _dyneither_ptr _tmp39=({const char*_tmp7="borrow == 0";_tag_dyneither(_tmp7,sizeof(char),12U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp39,({const char*_tmp8="xp.cyc";_tag_dyneither(_tmp8,sizeof(char),7U);}),160U);});};}
# 163
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int))Cyc__memcpy)(r,(struct _dyneither_ptr)rem,(unsigned int)m / sizeof(*((unsigned char*)rem.curr))+ (unsigned int)((unsigned int)m % sizeof(*((unsigned char*)rem.curr))== (unsigned int)0?0: 1),sizeof(*((unsigned char*)rem.curr)));{
# 165
int i;
for(i=(n - m)+ 1;i < nx;++ i){
*((unsigned char*)_check_dyneither_subscript(q,sizeof(unsigned char),i))='\000';}
for(i=m;i < my;++ i){
*((unsigned char*)_check_dyneither_subscript(r,sizeof(unsigned char),i))='\000';}};}}
# 172
return 1;}
# 174
int Cyc_XP_quotient(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y){
int i;
unsigned int carry=0U;
for(i=n - 1;i >= 0;-- i){
carry=carry * (unsigned int)(1 << 8)+ (unsigned int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry / (unsigned int)y);
carry %=(unsigned int)y;}
# 182
return(int)carry;}
# 184
int Cyc_XP_cmp(int n,struct _dyneither_ptr x,struct _dyneither_ptr y){
int i=n - 1;
while(i > 0  && ({int _tmp3A=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp3A == (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));})){
-- i;}
return({int _tmp3B=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp3B - (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));});}
# 190
void Cyc_XP_lshift(int n,struct _dyneither_ptr z,int m,struct _dyneither_ptr x,int s,int fill){
fill=fill?255: 0;
{
int i;int j=n - 1;
if(n > m)
i=m - 1;else{
# 197
i=(n - s / 8)- 1;}
for(0;j >= m + s / 8;-- j){
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),j))='\000';}
for(0;i >= 0;(i --,j --)){
({unsigned char _tmp3C=*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),j))=_tmp3C;});}
for(0;j >= 0;-- j){
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),j))=(unsigned char)fill;}}
# 205
s %=8;
if(s > 0){
# 208
Cyc_XP_product(n,z,z,1 << s);
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),0))|=fill >> 8 - s;}}
# 212
void Cyc_XP_rshift(int n,struct _dyneither_ptr z,int m,struct _dyneither_ptr x,int s,int fill){
fill=fill?255: 0;
{
int i;int j=0;
for(i=s / 8;i < m  && j < n;(i ++,j ++)){
({unsigned char _tmp3D=*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),j))=_tmp3D;});}
for(0;j < n;++ j){
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),j))=(unsigned char)fill;}}
# 221
s %=8;
if(s > 0){
# 224
Cyc_XP_quotient(n,z,z,1 << s);
*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),n - 1))|=fill << 8 - s;}}
# 228
void Cyc_XP_and(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y){
int i;
for(i=0;i < n;++ i){
({unsigned char _tmp3F=(unsigned char)({int _tmp3E=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp3E & (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));});*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=_tmp3F;});}}
# 233
void Cyc_XP_or(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y){
int i;
for(i=0;i < n;++ i){
({unsigned char _tmp41=(unsigned char)({int _tmp40=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp40 | (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));});*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=_tmp41;});}}
# 238
void Cyc_XP_xor(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y){
int i;
for(i=0;i < n;++ i){
({unsigned char _tmp43=(unsigned char)({int _tmp42=(int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i));_tmp42 ^ (int)*((unsigned char*)_check_dyneither_subscript(y,sizeof(unsigned char),i));});*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=_tmp43;});}}
# 243
void Cyc_XP_not(int n,struct _dyneither_ptr z,struct _dyneither_ptr x){
int i;
for(i=0;i < n;++ i){
({unsigned char _tmp44=~(*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),i)));*((unsigned char*)_check_dyneither_subscript(z,sizeof(unsigned char),i))=_tmp44;});}}
# 248
int Cyc_XP_fromstr(int n,struct _dyneither_ptr z,const char*str,int base){
# 250
const char*p=str;
(unsigned int)p?0:({struct _dyneither_ptr _tmp45=({const char*_tmp9="p";_tag_dyneither(_tmp9,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp45,({const char*_tmpA="xp.cyc";_tag_dyneither(_tmpA,sizeof(char),7U);}),251U);});
base >= 2  && base <= 36?0:({struct _dyneither_ptr _tmp46=({const char*_tmpB="base >= 2 && base <= 36";_tag_dyneither(_tmpB,sizeof(char),24U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp46,({const char*_tmpC="xp.cyc";_tag_dyneither(_tmpC,sizeof(char),7U);}),252U);});
while((int)*((const char*)_check_null(p)) &&  isspace((int)*p)){
({const char**_tmpD=& p;if(*(*_tmpD)!= 0)++(*_tmpD);else{_throw_arraybounds();}*_tmpD;});}
if(((int)*p  &&  isalnum((int)*p)) && (int)*((char*)_check_known_subscript_notnull(Cyc_map,75U,sizeof(char),(int)*p - (int)'0'))< base){
int carry;
for(0;((int)*((const char*)_check_null(p)) &&  isalnum((int)*p)) && (int)*((char*)_check_known_subscript_notnull(Cyc_map,75U,sizeof(char),(int)*p - (int)'0'))< base;({const char**_tmpE=& p;if(*(*_tmpE)!= 0)++(*_tmpE);else{_throw_arraybounds();}*_tmpE;})){
carry=Cyc_XP_product(n,z,z,base);
if(carry)
break;
Cyc_XP_sum(n,z,z,(int)*((char*)_check_known_subscript_notnull(Cyc_map,75U,sizeof(char),(int)*p - (int)'0')));}
# 263
return carry;}else{
# 265
return 0;}}
# 268
struct _dyneither_ptr Cyc_XP_tostr(struct _dyneither_ptr str,int size,int base,int n,struct _dyneither_ptr x){
# 270
int i=0;
(unsigned int)str.curr?0:({struct _dyneither_ptr _tmp47=({const char*_tmpF="str";_tag_dyneither(_tmpF,sizeof(char),4U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp47,({const char*_tmp10="xp.cyc";_tag_dyneither(_tmp10,sizeof(char),7U);}),271U);});
base >= 2  && base <= 36?0:({struct _dyneither_ptr _tmp48=({const char*_tmp11="base >= 2 && base <= 36";_tag_dyneither(_tmp11,sizeof(char),24U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp48,({const char*_tmp12="xp.cyc";_tag_dyneither(_tmp12,sizeof(char),7U);}),272U);});
do{
int r=Cyc_XP_quotient(n,x,x,base);
i < size?0:({struct _dyneither_ptr _tmp49=({const char*_tmp13="i < size";_tag_dyneither(_tmp13,sizeof(char),9U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp49,({const char*_tmp14="xp.cyc";_tag_dyneither(_tmp14,sizeof(char),7U);}),275U);});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(str,sizeof(char),i ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0U));char _tmp17=*((const char*)_check_dyneither_subscript(Cyc_digits,sizeof(char),r));if(_get_dyneither_size(_tmp15,sizeof(char))== 1U  && (_tmp16 == 0  && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
while(n > 1  && (int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),n - 1))== 0){
-- n;}}while(
n > 1  || (int)*((unsigned char*)_check_dyneither_subscript(x,sizeof(unsigned char),0))!= 0);
i < size?0:({struct _dyneither_ptr _tmp4A=({const char*_tmp18="i < size";_tag_dyneither(_tmp18,sizeof(char),9U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp4A,({const char*_tmp19="xp.cyc";_tag_dyneither(_tmp19,sizeof(char),7U);}),280U);});
({struct _dyneither_ptr _tmp1A=_dyneither_ptr_plus(str,sizeof(char),i);char _tmp1B=*((char*)_check_dyneither_subscript(_tmp1A,sizeof(char),0U));char _tmp1C='\000';if(_get_dyneither_size(_tmp1A,sizeof(char))== 1U  && (_tmp1B == 0  && _tmp1C != 0))_throw_arraybounds();*((char*)_tmp1A.curr)=_tmp1C;});
{
int j;
for(j=0;j < -- i;++ j){
char c=*((char*)_check_dyneither_subscript(str,sizeof(char),j));
({struct _dyneither_ptr _tmp1D=_dyneither_ptr_plus(str,sizeof(char),j);char _tmp1E=*((char*)_check_dyneither_subscript(_tmp1D,sizeof(char),0U));char _tmp1F=*((char*)_check_dyneither_subscript(str,sizeof(char),i));if(_get_dyneither_size(_tmp1D,sizeof(char))== 1U  && (_tmp1E == 0  && _tmp1F != 0))_throw_arraybounds();*((char*)_tmp1D.curr)=_tmp1F;});
({struct _dyneither_ptr _tmp20=_dyneither_ptr_plus(str,sizeof(char),i);char _tmp21=*((char*)_check_dyneither_subscript(_tmp20,sizeof(char),0U));char _tmp22=c;if(_get_dyneither_size(_tmp20,sizeof(char))== 1U  && (_tmp21 == 0  && _tmp22 != 0))_throw_arraybounds();*((char*)_tmp20.curr)=_tmp22;});}}
# 290
return str;}

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
 struct Cyc___cycFILE;
# 52 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdin;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 142 "cycboot.h"
int Cyc_getc(struct Cyc___cycFILE*);
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
# 222 "cycboot.h"
int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 247
int Cyc_vsscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 302
int isupper(int);
# 314
double atof(const char*);
long strtol(char*,char**,int);
# 317
unsigned long strtoul(char*,char**,int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 126 "scanf.cyc"
static struct _dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt);
# 139
static short*Cyc_va_arg_short_ptr(void*a){
void*_tmp0=a;unsigned short*_tmp4;short*_tmp3;switch(*((int*)_tmp0)){case 0U: _LL1: _tmp3=((struct Cyc_ShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;_LL2:
 return _tmp3;case 1U: _LL3: _tmp4=((struct Cyc_UShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;_LL4:
 return(short*)_tmp4;default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp37=({const char*_tmp1="scan expects short pointer";_tag_dyneither(_tmp1,sizeof(char),27U);});_tmp2->f1=_tmp37;});_tmp2;}));}_LL0:;}
# 147
static int*Cyc_va_arg_int_ptr(void*a){
void*_tmp5=a;unsigned int*_tmp9;int*_tmp8;switch(*((int*)_tmp5)){case 2U: _LL1: _tmp8=((struct Cyc_IntPtr_sa_ScanfArg_struct*)_tmp5)->f1;_LL2:
 return _tmp8;case 3U: _LL3: _tmp9=((struct Cyc_UIntPtr_sa_ScanfArg_struct*)_tmp5)->f1;_LL4:
 return(int*)_tmp9;default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp38=({const char*_tmp6="scan expects int pointer";_tag_dyneither(_tmp6,sizeof(char),25U);});_tmp7->f1=_tmp38;});_tmp7;}));}_LL0:;}
# 155
static struct _dyneither_ptr Cyc_va_arg_string_ptr(void*a){
void*_tmpA=a;struct _dyneither_ptr _tmpE;struct _dyneither_ptr _tmpD;switch(*((int*)_tmpA)){case 4U: _LL1: _tmpD=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmpA)->f1;_LL2:
 return _dyneither_ptr_decrease_size(_tmpD,sizeof(char),1U);case 7U: _LL3: _tmpE=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmpA)->f1;_LL4:
 return _tmpE;default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp39=({const char*_tmpB="scan expects char pointer";_tag_dyneither(_tmpB,sizeof(char),26U);});_tmpC->f1=_tmp39;});_tmpC;}));}_LL0:;}
# 163
static double*Cyc_va_arg_double_ptr(void*a){
void*_tmpF=a;double*_tmp12;if(((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmpF)->tag == 5U){_LL1: _tmp12=((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmpF)->f1;_LL2:
 return _tmp12;}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp3A=({const char*_tmp10="scan expects double pointer";_tag_dyneither(_tmp10,sizeof(char),28U);});_tmp11->f1=_tmp3A;});_tmp11;}));}_LL0:;}
# 170
static float*Cyc_va_arg_float_ptr(void*a){
void*_tmp13=a;float*_tmp16;if(((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp13)->tag == 6U){_LL1: _tmp16=((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp13)->f1;_LL2:
 return _tmp16;}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp3B=({const char*_tmp14="scan expects float pointer";_tag_dyneither(_tmp14,sizeof(char),27U);});_tmp15->f1=_tmp3B;});_tmp15;}));}_LL0:;}
# 177
static struct _dyneither_ptr Cyc_va_arg_char_ptr(void*a){
void*_tmp17=a;struct _dyneither_ptr _tmp1B;struct _dyneither_ptr _tmp1A;switch(*((int*)_tmp17)){case 7U: _LL1: _tmp1A=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmp17)->f1;_LL2:
 return _tmp1A;case 4U: _LL3: _tmp1B=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmp17)->f1;_LL4:
 return _dyneither_ptr_decrease_size(_tmp1B,sizeof(char),1U);default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp3C=({const char*_tmp18="scan expects char pointer";_tag_dyneither(_tmp18,sizeof(char),26U);});_tmp19->f1=_tmp3C;});_tmp19;}));}_LL0:;}
# 188
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp){
# 198
struct _dyneither_ptr fmt=fmt0;
int c;
long long width;
struct _dyneither_ptr p=_tag_dyneither(0,0,0);
int n;
int flags=0;
# 205
struct _dyneither_ptr p0=_tag_dyneither(0,0,0);
int nassigned;
int nread;
# 209
int base=0;
int use_strtoul=0;
# 213
char ccltab[256U];
# 215
char buf[351U];({{unsigned int _tmp36=350U;unsigned int i;for(i=0;i < _tmp36;++ i){buf[i]='0';}buf[_tmp36]=0;}0;});{
# 217
int seen_eof=0;
# 220
static short basefix[17U]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
# 223
nassigned=0;
nread=0;
for(0;1;0){
c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == 0)
goto done;
if( isspace(c)){
for(0;1;0){
({int _tmp3D=_IO_getc(fp);c=_tmp3D;});
if(c == - 1){
++ seen_eof;
break;}
# 236
if(! isspace(c)){
_IO_ungetc(c,fp);
break;}
# 240
++ nread;}
# 242
continue;}
# 244
if(c != '%')
goto literal;
width=(long long)0;
flags=0;
# 252
again: c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
{int _tmp1C=c;switch(_tmp1C){case 37U: _LL1: _LL2:
# 255
 literal:
({int _tmp3E=_IO_getc(fp);n=_tmp3E;});
if(n == - 1)
goto eof_failure;
if(n != c){
_IO_ungetc(n,fp);
goto match_failure;}
# 263
++ nread;
continue;case 42U: _LL3: _LL4:
# 267
 if(flags)goto control_failure;
flags=8;
goto again;case 108U: _LL5: _LL6:
# 271
 if(flags & ~(8 | 64))goto control_failure;
flags |=1;
goto again;case 76U: _LL7: _LL8:
# 275
 if(flags & ~(8 | 64))goto control_failure;
flags |=2;
goto again;case 104U: _LL9: _LLA:
# 279
 if(flags & ~(8 | 64))goto control_failure;
flags |=4;
goto again;case 48U: _LLB: _LLC:
# 283
 goto _LLE;case 49U: _LLD: _LLE: goto _LL10;case 50U: _LLF: _LL10: goto _LL12;case 51U: _LL11: _LL12: goto _LL14;case 52U: _LL13: _LL14: goto _LL16;case 53U: _LL15: _LL16:
 goto _LL18;case 54U: _LL17: _LL18: goto _LL1A;case 55U: _LL19: _LL1A: goto _LL1C;case 56U: _LL1B: _LL1C: goto _LL1E;case 57U: _LL1D: _LL1E:
 if(flags & ~(8 | 64))goto control_failure;
flags |=64;
width=(width * 10 + c)- '0';
goto again;case 68U: _LL1F: _LL20:
# 298 "scanf.cyc"
 flags |=1;
goto _LL22;case 100U: _LL21: _LL22:
# 301
 c=3;
use_strtoul=0;
base=10;
goto _LL0;case 105U: _LL23: _LL24:
# 307
 c=3;
use_strtoul=0;
base=0;
goto _LL0;case 79U: _LL25: _LL26:
# 313
 flags |=1;
goto _LL28;case 111U: _LL27: _LL28:
# 316
 c=3;
use_strtoul=1;
base=8;
goto _LL0;case 117U: _LL29: _LL2A:
# 322
 c=3;
use_strtoul=1;
base=10;
goto _LL0;case 88U: _LL2B: _LL2C:
# 327
 goto _LL2E;case 120U: _LL2D: _LL2E:
 flags |=256;
c=3;
use_strtoul=1;
base=16;
goto _LL0;case 69U: _LL2F: _LL30:
# 334
 goto _LL32;case 70U: _LL31: _LL32: goto _LL34;case 101U: _LL33: _LL34: goto _LL36;case 102U: _LL35: _LL36: goto _LL38;case 103U: _LL37: _LL38:
 c=4;
goto _LL0;case 115U: _LL39: _LL3A:
# 339
 c=2;
goto _LL0;case 91U: _LL3B: _LL3C:
# 343
({struct _dyneither_ptr _tmp3F=Cyc___sccl(ccltab,fmt);fmt=_tmp3F;});
flags |=32;
c=1;
goto _LL0;case 99U: _LL3D: _LL3E:
# 349
 flags |=32;
c=0;
goto _LL0;case 112U: _LL3F: _LL40:
# 354
 flags |=16 | 256;
c=3;
use_strtoul=1;
base=16;
goto _LL0;case 110U: _LL41: _LL42:
# 361
 if(flags & 8)
continue;
if(flags & 4)
({short _tmp40=(short)nread;*Cyc_va_arg_short_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp40;});else{
if(flags & 1)
({long _tmp41=nread;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp41;});else{
# 368
({int _tmp42=nread;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp42;});}}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 0U: _LL43: _LL44:
# 376
 nassigned=-1;
goto done;default: _LL45: _LL46:
# 380
 if( isupper(c))
flags |=1;
c=3;
use_strtoul=0;
base=10;
goto _LL0;}_LL0:;}
# 391
if(_IO_peekc(fp)== - 1)
goto eof_failure;
# 398
if((flags & 32)== 0){
({int _tmp43=_IO_peekc(fp);n=_tmp43;});
while( isspace(n)){
({int _tmp44=_IO_getc(fp);n=_tmp44;});
++ nread;
({int _tmp45=_IO_peekc(fp);n=_tmp45;});
if(n == - 1)
goto eof_failure;}}{
# 415
int _tmp1D=c;switch(_tmp1D){case 0U: _LL48: _LL49:
# 421
 if(width == 0)
width=(long long)1;
if(flags & 8){
long long sum=(long long)0;
for(0;width > 0;0){
({int _tmp46=_IO_getc(fp);n=_tmp46;});
if(n == - 1  && width != 0)
goto eof_failure;else{
if(n == - 1){
++ seen_eof;
break;}}
# 433
++ sum;
-- width;}
# 436
nread +=sum;}else{
# 438
long long sum=(long long)0;
struct _dyneither_ptr _tmp1E=Cyc_va_arg_char_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
for(0;width > 0;0){
({int _tmp47=_IO_getc(fp);n=_tmp47;});
if(n == - 1  && width != 0)
goto eof_failure;else{
if(n == - 1){
++ seen_eof;
break;}}
# 448
*((char*)_check_dyneither_subscript(_tmp1E,sizeof(char),0U))=(char)n;
_dyneither_ptr_inplace_plus(& _tmp1E,sizeof(char),1);
++ sum;
-- width;}
# 453
nread +=sum;
++ nassigned;}
# 456
goto _LL47;case 1U: _LL4A: _LL4B:
# 460
 if(width == 0)
width=(long long)4294967295U;
# 463
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while((int)ccltab[_check_known_subscript_notnull(256U,(int)((char)c))]){
++ n;
_IO_getc(fp);
if(-- width == 0)
break;
if(({int _tmp48=_IO_peekc(fp);c=_tmp48;})== - 1){
if(n == 0)
goto eof_failure;
++ seen_eof;
break;}}
# 478
if(n == 0)
goto match_failure;};}else{
# 481
struct _dyneither_ptr p4=(struct _dyneither_ptr)Cyc_va_arg_string_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _dyneither_ptr p5=p4;
int c=_IO_peekc(fp);
while((int)ccltab[_check_known_subscript_notnull(256U,(int)((char)c))]){
if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(p5,sizeof(char),0U))=(char)c;
_dyneither_ptr_inplace_plus(& p5,sizeof(char),1);
_IO_getc(fp);
if(-- width == 0)
break;
if(({int _tmp49=_IO_peekc(fp);c=_tmp49;})== - 1){
if((char*)p5.curr == (char*)p0.curr)
goto eof_failure;
++ seen_eof;
break;}}
# 498
n=(p5.curr - p4.curr)/ sizeof(char);
if(n == 0)
goto match_failure;
if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(p5,sizeof(char),0U))='\000';
++ nassigned;};}
# 505
nread +=n;
goto _LL47;case 2U: _LL4C: _LL4D:
# 510
 if(width == 0)
width=(long long)4294967295U;
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while(! isspace((int)((unsigned char)c))){
++ n;
_IO_getc(fp);
if(-- width == 0)
break;
if(({int _tmp4A=_IO_peekc(fp);c=_tmp4A;})== - 1){
++ seen_eof;
break;}}
# 525
nread +=n;};}else{
# 527
struct _dyneither_ptr _tmp1F=Cyc_va_arg_string_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _dyneither_ptr _tmp20=_tmp1F;
int c=_IO_peekc(fp);
while(! isspace((int)((unsigned char)c))){
({int _tmp4B=_IO_getc(fp);c=_tmp4B;});
if(_get_dyneither_size(_tmp20,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(_tmp20,sizeof(char),0U))=(char)c;
_dyneither_ptr_inplace_plus(& _tmp20,sizeof(char),1);
if(-- width == 0)
break;
if(({int _tmp4C=_IO_peekc(fp);c=_tmp4C;})== - 1){
++ seen_eof;
break;}}
# 542
if(_get_dyneither_size(_tmp20,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(_tmp20,sizeof(char),0U))='\000';
nread +=(_tmp20.curr - _tmp1F.curr)/ sizeof(char);
++ nassigned;};}
# 547
continue;case 3U: _LL4E: _LL4F:
# 551
 if(width == 0  || width > sizeof(buf)- 1)
width=(long long)(sizeof(buf)- 1);
flags |=(64 | 128)| 512;
for(({struct _dyneither_ptr _tmp4D=({char*_tmp21=buf;_tag_dyneither(_tmp21,sizeof(char),351U);});p=_tmp4D;});width != 0;-- width){
({int _tmp4E=(int)((unsigned char)_IO_peekc(fp));c=_tmp4E;});
# 560
{int _tmp22=c;switch(_tmp22){case 48U: _LL55: _LL56:
# 575 "scanf.cyc"
 if(base == 0){
base=8;
flags |=256;}
# 579
if(flags & 512)
flags &=~((64 | 512)| 128);else{
# 582
flags &=~((64 | 256)| 128);}
goto ok;case 49U: _LL57: _LL58:
# 586
 goto _LL5A;case 50U: _LL59: _LL5A: goto _LL5C;case 51U: _LL5B: _LL5C: goto _LL5E;case 52U: _LL5D: _LL5E: goto _LL60;case 53U: _LL5F: _LL60:
 goto _LL62;case 54U: _LL61: _LL62: goto _LL64;case 55U: _LL63: _LL64:
 base=(int)basefix[_check_known_subscript_notnull(17U,base)];
flags &=~((64 | 256)| 128);
goto ok;case 56U: _LL65: _LL66:
# 593
 goto _LL68;case 57U: _LL67: _LL68:
 base=(int)basefix[_check_known_subscript_notnull(17U,base)];
if(base <= 8)
goto _LL54;
flags &=~((64 | 256)| 128);
goto ok;case 65U: _LL69: _LL6A:
# 601
 goto _LL6C;case 66U: _LL6B: _LL6C: goto _LL6E;case 67U: _LL6D: _LL6E: goto _LL70;case 68U: _LL6F: _LL70: goto _LL72;case 69U: _LL71: _LL72:
 goto _LL74;case 70U: _LL73: _LL74: goto _LL76;case 97U: _LL75: _LL76: goto _LL78;case 98U: _LL77: _LL78: goto _LL7A;case 99U: _LL79: _LL7A:
 goto _LL7C;case 100U: _LL7B: _LL7C: goto _LL7E;case 101U: _LL7D: _LL7E: goto _LL80;case 102U: _LL7F: _LL80:
# 605
 if(base <= 10)
goto _LL54;
flags &=~((64 | 256)| 128);
goto ok;case 43U: _LL81: _LL82:
# 611
 goto _LL84;case 45U: _LL83: _LL84:
 if(flags & 64){
flags &=~ 64;
goto ok;}
# 616
goto _LL54;case 120U: _LL85: _LL86:
# 619
 goto _LL88;case 88U: _LL87: _LL88:
 if(flags & 256  && ({char*_tmp4F=(char*)(_dyneither_ptr_plus(p,sizeof(char),- 1)).curr;_tmp4F == buf;})){
base=16;
flags &=~ 256;
goto ok;}
# 625
goto _LL54;default: _LL89: _LL8A:
# 627
 goto _LL54;}_LL54:;}
# 635
break;
ok:
# 640
({struct _dyneither_ptr _tmp23=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp24=*((char*)_check_dyneither_subscript(_tmp23,sizeof(char),0U));char _tmp25=(char)c;if(_get_dyneither_size(_tmp23,sizeof(char))== 1U  && (_tmp24 == '\000'  && _tmp25 != '\000'))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});
_IO_getc(fp);
if(_IO_peekc(fp)== - 1){
++ seen_eof;
break;}}
# 653
if(flags & 128){
if((char*)p.curr > buf){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U)),fp);}
# 658
goto match_failure;}
# 660
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),-1));
if(c == 'x'  || c == 'X'){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc(c,fp);}
# 665
if((flags & 8)== 0){
unsigned long res;
# 668
({struct _dyneither_ptr _tmp26=p;char _tmp27=*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0U));char _tmp28='\000';if(_get_dyneither_size(_tmp26,sizeof(char))== 1U  && (_tmp27 == '\000'  && _tmp28 != '\000'))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});
if(use_strtoul)
({unsigned long _tmp50= strtoul(buf,0,base);res=_tmp50;});else{
# 672
({unsigned long _tmp51=(unsigned long) strtol(buf,0,base);res=_tmp51;});}
if(flags & 16)
({int _tmp52=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp52;});else{
if(flags & 4)
({short _tmp53=(short)res;*Cyc_va_arg_short_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp53;});else{
if(flags & 1)
({int _tmp54=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp54;});else{
# 680
({int _tmp55=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp55;});}}}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 684
({int _tmp57=({unsigned char*_tmp56=p.curr;_tmp56 - ({char*_tmp29=buf;_tag_dyneither(_tmp29,sizeof(char),351U);}).curr;})/ sizeof(char);nread +=_tmp57;});
goto _LL47;case 4U: _LL50: _LL51:
# 689
 if(width == 0  || width > sizeof(buf)- 1)
width=(long long)(sizeof(buf)- 1);
flags |=((64 | 128)| 256)| 512;
for(({struct _dyneither_ptr _tmp58=({char*_tmp2A=buf;_tag_dyneither(_tmp2A,sizeof(char),351U);});p=_tmp58;});width != 0;-- width){
({int _tmp59=_IO_peekc(fp);c=_tmp59;});
# 698
{int _tmp2B=c;switch(_tmp2B){case 48U: _LL8C: _LL8D:
# 700
 goto _LL8F;case 49U: _LL8E: _LL8F: goto _LL91;case 50U: _LL90: _LL91: goto _LL93;case 51U: _LL92: _LL93: goto _LL95;case 52U: _LL94: _LL95:
 goto _LL97;case 53U: _LL96: _LL97: goto _LL99;case 54U: _LL98: _LL99: goto _LL9B;case 55U: _LL9A: _LL9B: goto _LL9D;case 56U: _LL9C: _LL9D:
 goto _LL9F;case 57U: _LL9E: _LL9F:
 flags &=~(64 | 128);
goto fok;case 43U: _LLA0: _LLA1:
# 706
 goto _LLA3;case 45U: _LLA2: _LLA3:
 if(flags & 64){
flags &=~ 64;
goto fok;}
# 711
goto _LL8B;case 46U: _LLA4: _LLA5:
# 713
 if(flags & 256){
flags &=~(64 | 256);
goto fok;}
# 717
goto _LL8B;case 101U: _LLA6: _LLA7:
 goto _LLA9;case 69U: _LLA8: _LLA9:
# 720
 if((flags & (128 | 512))== 512){
flags=(flags & ~(512 | 256)| 64)| 128;
# 724
goto fok;}
# 726
goto _LL8B;default: _LLAA: _LLAB:
# 728
 goto _LL8B;}_LL8B:;}
# 730
break;
fok:
({struct _dyneither_ptr _tmp2C=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0U));char _tmp2E=(char)c;if(_get_dyneither_size(_tmp2C,sizeof(char))== 1U  && (_tmp2D == '\000'  && _tmp2E != '\000'))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});
_IO_getc(fp);
if(_IO_peekc(fp)== - 1){
++ seen_eof;
break;}}
# 744
if(flags & 128){
if(flags & 512){
# 747
while((char*)p.curr > buf){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U)),fp);}
# 751
goto match_failure;}
# 754
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U));
if(c != 'e'  && c != 'E'){
_IO_ungetc(c,fp);
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U));}
# 761
_IO_ungetc(c,fp);}
# 763
if((flags & 8)== 0){
double res;
({struct _dyneither_ptr _tmp2F=p;char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0U));char _tmp31='\000';if(_get_dyneither_size(_tmp2F,sizeof(char))== 1U  && (_tmp30 == '\000'  && _tmp31 != '\000'))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});
({double _tmp5A= atof((const char*)buf);res=_tmp5A;});
if(flags & 1)
({double _tmp5B=res;*Cyc_va_arg_double_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp5B;});else{
# 770
({float _tmp5C=(float)res;*Cyc_va_arg_float_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp5C;});}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 774
({int _tmp5E=({unsigned char*_tmp5D=p.curr;_tmp5D - ({char*_tmp32=buf;_tag_dyneither(_tmp32,sizeof(char),351U);}).curr;})/ sizeof(char);nread +=_tmp5E;});
goto _LL47;default: _LL52: _LL53:
# 777
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp5F=({const char*_tmp33="scanf3";_tag_dyneither(_tmp33,sizeof(char),7U);});_tmp34->f1=_tmp5F;});_tmp34;}));}_LL47:;};}
# 781
eof_failure:
 ++ seen_eof;
input_failure:
 if(nassigned == 0)
nassigned=-1;
control_failure:
 match_failure:
 if((unsigned int)errp)
*errp |=2;
done:
 if((unsigned int)errp  && seen_eof)
*errp |=1;
return nassigned;};}
# 802
static struct _dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt){
# 807
int c;int n;int v;
# 810
c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == '^'){
v=1;
c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}else{
# 815
v=0;}
# 817
for(n=0;n < 256;++ n){
tab[_check_known_subscript_notnull(256U,n)]=(char)v;}
if(c == 0)
return _dyneither_ptr_plus(fmt,sizeof(char),- 1);
# 829 "scanf.cyc"
v=1 - v;
for(0;1;0){
tab[_check_known_subscript_notnull(256U,c)]=(char)v;
doswitch:
 n=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));{
int _tmp35=n;switch(_tmp35){case 0U: _LL1: _LL2:
# 837
 return _dyneither_ptr_plus(fmt,sizeof(char),- 1);case 45U: _LL3: _LL4:
# 858 "scanf.cyc"
 n=(int)*((const char*)_check_dyneither_subscript(fmt,sizeof(char),0U));
if(n == ']'  || n < c){
c=(int)'-';
goto _LL0;}
# 863
_dyneither_ptr_inplace_plus(& fmt,sizeof(char),1);
do{
tab[_check_known_subscript_notnull(256U,++ c)]=(char)v;}while(c < n);
# 873
goto doswitch;
# 881
goto _LL0;case 93U: _LL5: _LL6:
# 884
 return fmt;default: _LL7: _LL8:
# 887
 c=n;
goto _LL0;}_LL0:;};}}
# 896
static int Cyc_string_getc(struct _dyneither_ptr*sptr){
char c;
struct _dyneither_ptr s=*sptr;
if((({char*_tmp60=(char*)s.curr;_tmp60 == (char*)(_tag_dyneither(0,0,0)).curr;}) || _get_dyneither_size(s,sizeof(char))== 0) || (c=*((const char*)_check_dyneither_subscript(s,sizeof(char),0)))== '\000')return - 1;
({struct _dyneither_ptr _tmp61=_dyneither_ptr_plus(s,sizeof(char),1);*sptr=_tmp61;});
return(int)c;}
# 904
static int Cyc_string_ungetc(int ignore,struct _dyneither_ptr*sptr){
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(*sptr,sizeof(char),- 1);*sptr=_tmp62;});
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _dyneither_ptr*sptr){
char c;
struct _dyneither_ptr s=*sptr;
if((({char*_tmp63=(char*)s.curr;_tmp63 == (char*)(_tag_dyneither(0,0,0)).curr;}) || _get_dyneither_size(s,sizeof(char))== 0) || (c=*((const char*)_check_dyneither_subscript(s,sizeof(char),0)))== '\000')return - 1;
return(int)c;}
# 917
int Cyc_vsscanf(struct _dyneither_ptr src1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 920
struct _dyneither_ptr src=(struct _dyneither_ptr)src1;
int err=0;
return((int(*)(int(*_IO_getc)(struct _dyneither_ptr*),int(*_IO_ungetc)(int,struct _dyneither_ptr*),int(*_IO_peekc)(struct _dyneither_ptr*),struct _dyneither_ptr*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,& err);}
# 926
int Cyc_sscanf(struct _dyneither_ptr src,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 929
return Cyc_vsscanf(src,fmt,ap);}
# 933
int Cyc_peekc(struct Cyc___cycFILE*stream){
int c=Cyc_fgetc(stream);
Cyc_ungetc(c,stream);
return c;}
# 939
int Cyc_vfscanf(struct Cyc___cycFILE*stream,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 942
int err=0;
return((int(*)(int(*_IO_getc)(struct Cyc___cycFILE*),int(*_IO_ungetc)(int,struct Cyc___cycFILE*),int(*_IO_peekc)(struct Cyc___cycFILE*),struct Cyc___cycFILE*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_getc,Cyc_ungetc,Cyc_peekc,stream,fmt,ap,& err);}
# 946
int Cyc_fscanf(struct Cyc___cycFILE*stream,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 949
return Cyc_vfscanf(stream,fmt,ap);}
# 952
int Cyc_scanf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 955
return Cyc_vfscanf(Cyc_stdin,fmt,ap);}

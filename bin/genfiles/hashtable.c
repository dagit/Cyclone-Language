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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 59
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data);
# 64
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t);
# 69
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key);
# 74
int Cyc_Hashtable_hash_string(struct _dyneither_ptr s);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 81
void Cyc_Hashtable_iter(void(*f)(void*,void*),struct Cyc_Hashtable_Table*t);
# 84
void Cyc_Hashtable_iter_c(void(*f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env);
# 88
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*));struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _dyneither_ptr tab;};
# 39 "hashtable.cyc"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 41
return({struct Cyc_Hashtable_Table*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->r=r,_tmp2->cmp=cmp,_tmp2->hash=hash,_tmp2->max_len=3,({struct _dyneither_ptr _tmp27=({unsigned int _tmp0=sz;struct Cyc_List_List**_tmp1=_region_calloc(r,sizeof(struct Cyc_List_List*),_tmp0);_tag_dyneither(_tmp1,sizeof(struct Cyc_List_List*),_tmp0);});_tmp2->tab=_tmp27;});_tmp2;});}
# 44
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 46
return Cyc_Hashtable_rcreate(Cyc_Core_heap_region,sz,cmp,hash);}struct _tuple0{void*f1;void*f2;};
# 49
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val){
struct _dyneither_ptr tab=t->tab;
int bucket=(int)({int _tmp28=(t->hash)(key);_tmp28 % _get_dyneither_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp2A=({struct Cyc_List_List*_tmp4=_region_malloc(t->r,sizeof(*_tmp4));({struct _tuple0*_tmp29=({struct _tuple0*_tmp3=_region_malloc(t->r,sizeof(*_tmp3));_tmp3->f1=key,_tmp3->f2=val;_tmp3;});_tmp4->hd=_tmp29;}),_tmp4->tl=*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),bucket));_tmp4;});((struct Cyc_List_List**)tab.curr)[bucket]=_tmp2A;});
if(({int _tmp2B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_List_List**)tab.curr)[bucket]);_tmp2B > t->max_len;}))
Cyc_Hashtable_resize(t);}
# 57
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key){
struct _dyneither_ptr tab=t->tab;
struct Cyc_List_List*l=({int _tmp2D=(int)({int _tmp2C=(t->hash)(key);_tmp2C % _get_dyneither_size(tab,sizeof(struct Cyc_List_List*));});((struct Cyc_List_List**)tab.curr)[_tmp2D];});
return Cyc_List_assoc_cmp(t->cmp,l,key);}
# 63
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key){
struct _dyneither_ptr _tmp5=t->tab;
struct Cyc_List_List*_tmp6=({int _tmp2F=(int)({int _tmp2E=(t->hash)(key);_tmp2E % _get_dyneither_size(_tmp5,sizeof(struct Cyc_List_List*));});((struct Cyc_List_List**)_tmp5.curr)[_tmp2F];});
int(*_tmp7)(void*,void*)=t->cmp;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct _tuple0*_tmp8=(struct _tuple0*)_tmp6->hd;struct _tuple0*_tmp9=_tmp8;void*_tmpB;void**_tmpA;_LL1: _tmpB=_tmp9->f1;_tmpA=(void**)& _tmp9->f2;_LL2:;
if(_tmp7(key,_tmpB)== 0)return _tmpA;}
# 71
return 0;}
# 74
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data){
struct _dyneither_ptr _tmpC=t->tab;
struct Cyc_List_List*_tmpD=({int _tmp31=(int)({int _tmp30=(t->hash)(key);_tmp30 % _get_dyneither_size(_tmpC,sizeof(struct Cyc_List_List*));});((struct Cyc_List_List**)_tmpC.curr)[_tmp31];});
int(*_tmpE)(void*,void*)=t->cmp;
for(0;_tmpD != 0;_tmpD=_tmpD->tl){
struct _tuple0 _tmpF=*((struct _tuple0*)_tmpD->hd);struct _tuple0 _tmp10=_tmpF;void*_tmp12;void*_tmp11;_LL1: _tmp12=_tmp10.f1;_tmp11=_tmp10.f2;_LL2:;
if(_tmpE(key,_tmp12)== 0){
*data=_tmp11;
return 1;}}
# 85
return 0;}
# 88
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key){
# 91
struct _dyneither_ptr _tmp13=t->tab;
int(*_tmp14)(void*,void*)=t->cmp;
int bucket=(int)({int _tmp32=(t->hash)(key);_tmp32 % _get_dyneither_size(_tmp13,sizeof(struct Cyc_List_List*));});
struct Cyc_List_List*_tmp15=*((struct Cyc_List_List**)_check_dyneither_subscript(_tmp13,sizeof(struct Cyc_List_List*),bucket));
if(_tmp15 == 0)return;
if(_tmp14(key,(((struct _tuple0*)_tmp15->hd)[0]).f1)== 0){
((struct Cyc_List_List**)_tmp13.curr)[bucket]=_tmp15->tl;
return;}{
# 100
struct Cyc_List_List*_tmp16=_tmp15->tl;for(0;((struct Cyc_List_List*)_check_null(_tmp15))->tl != 0;(_tmp15=_tmp15->tl,_tmp16=_tmp16->tl)){
# 102
if(_tmp14(key,(((struct _tuple0*)((struct Cyc_List_List*)_check_null(_tmp16))->hd)[0]).f1)== 0){
_tmp15->tl=_tmp16->tl;
return;}}};}
# 108
int Cyc_Hashtable_hash_string(struct _dyneither_ptr s){
# 110
int ans=0;
int sz=(int)_get_dyneither_size(s,sizeof(char));
int shift=0;
{int i=0;for(0;i < sz;++ i){
ans=ans ^ *((const char*)_check_dyneither_subscript(s,sizeof(char),i))<< shift;
shift +=8;
if(shift == 32)
shift=0;}}
# 119
return ans;}
# 122
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*s){
return Cyc_Hashtable_hash_string(*s);}
# 127
void Cyc_Hashtable_insert_bucket(struct _RegionHandle*r,struct _dyneither_ptr tab,int(*hash)(void*),struct Cyc_List_List*elems){
# 130
if(elems == 0)return;
Cyc_Hashtable_insert_bucket(r,tab,hash,elems->tl);{
void*key=(((struct _tuple0*)elems->hd)[0]).f1;
void*val=(((struct _tuple0*)elems->hd)[0]).f2;
int nidx=(int)({int _tmp33=hash(key);_tmp33 % _get_dyneither_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp35=({struct Cyc_List_List*_tmp18=_region_malloc(r,sizeof(*_tmp18));({struct _tuple0*_tmp34=({struct _tuple0*_tmp17=_region_malloc(r,sizeof(*_tmp17));_tmp17->f1=key,_tmp17->f2=val;_tmp17;});_tmp18->hd=_tmp34;}),_tmp18->tl=*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),nidx));_tmp18;});((struct Cyc_List_List**)tab.curr)[nidx]=_tmp35;});};}
# 138
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){
struct _dyneither_ptr odata=t->tab;
int osize=(int)_get_dyneither_size(odata,sizeof(struct Cyc_List_List*));
int nsize=2 * osize + 1;
struct _dyneither_ptr ndata=({unsigned int _tmp19=nsize;struct Cyc_List_List**_tmp1A=_region_calloc(t->r,sizeof(struct Cyc_List_List*),_tmp19);_tag_dyneither(_tmp1A,sizeof(struct Cyc_List_List*),_tmp19);});
{int i=0;for(0;i < osize;++ i){
Cyc_Hashtable_insert_bucket(t->r,ndata,t->hash,*((struct Cyc_List_List**)_check_dyneither_subscript(odata,sizeof(struct Cyc_List_List*),i)));}}
t->tab=ndata;
t->max_len=2 * t->max_len;}
# 150
void Cyc_Hashtable_iter(void(*f)(void*,void*),struct Cyc_Hashtable_Table*t){
struct _dyneither_ptr odata=t->tab;
int osize=(int)_get_dyneither_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_dyneither_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
f((((struct _tuple0*)iter->hd)[0]).f1,(((struct _tuple0*)iter->hd)[0]).f2);}}}
# 158
void Cyc_Hashtable_iter_c(void(*f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env){
struct _dyneither_ptr odata=t->tab;
int osize=(int)_get_dyneither_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_dyneither_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
f((((struct _tuple0*)iter->hd)[0]).f1,(((struct _tuple0*)iter->hd)[0]).f2,env);}}}
# 167
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*)){
# 169
struct _dyneither_ptr odata=t->tab;
int osize=(int)_get_dyneither_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
({struct Cyc_Int_pa_PrintArg_struct _tmp1D=({struct Cyc_Int_pa_PrintArg_struct _tmp26;_tmp26.tag=1U,_tmp26.f1=(unsigned long)i;_tmp26;});void*_tmp1B[1U];_tmp1B[0]=& _tmp1D;({struct _dyneither_ptr _tmp36=({const char*_tmp1C="%d: ";_tag_dyneither(_tmp1C,sizeof(char),5U);});Cyc_printf(_tmp36,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});
{struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_dyneither_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
({void*_tmp1E=0U;({struct _dyneither_ptr _tmp37=({const char*_tmp1F="(";_tag_dyneither(_tmp1F,sizeof(char),2U);});Cyc_printf(_tmp37,_tag_dyneither(_tmp1E,sizeof(void*),0U));});});
prn_key((((struct _tuple0*)iter->hd)[0]).f1);
({void*_tmp20=0U;({struct _dyneither_ptr _tmp38=({const char*_tmp21=",";_tag_dyneither(_tmp21,sizeof(char),2U);});Cyc_printf(_tmp38,_tag_dyneither(_tmp20,sizeof(void*),0U));});});
prn_val((((struct _tuple0*)iter->hd)[0]).f2);
({void*_tmp22=0U;({struct _dyneither_ptr _tmp39=({const char*_tmp23=") ";_tag_dyneither(_tmp23,sizeof(char),3U);});Cyc_printf(_tmp39,_tag_dyneither(_tmp22,sizeof(void*),0U));});});}}
# 180
({void*_tmp24=0U;({struct _dyneither_ptr _tmp3A=({const char*_tmp25="\n";_tag_dyneither(_tmp25,sizeof(char),2U);});Cyc_printf(_tmp3A,_tag_dyneither(_tmp24,sizeof(void*),0U));});});}}

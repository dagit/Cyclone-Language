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
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*));struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _dyneither_ptr tab;};
# 39 "hashtable.cyc"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 41
return({struct Cyc_Hashtable_Table*_tmp1=_region_malloc(r,sizeof(*_tmp1));_tmp1->r=r,_tmp1->cmp=cmp,_tmp1->hash=hash,_tmp1->max_len=3,({struct _dyneither_ptr _tmp25=({unsigned int _tmp0=sz;_tag_dyneither(_region_calloc(r,sizeof(struct Cyc_List_List*),_tmp0),sizeof(struct Cyc_List_List*),_tmp0);});_tmp1->tab=_tmp25;});_tmp1;});}
# 44
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 46
return Cyc_Hashtable_rcreate(Cyc_Core_heap_region,sz,cmp,hash);}struct _tuple0{void*f1;void*f2;};
# 49
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val){
struct _dyneither_ptr tab=t->tab;
int bucket=(int)({unsigned int _tmp26=(unsigned int)(t->hash)(key);_tmp26 % _get_dyneither_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp28=({struct Cyc_List_List*_tmp3=_region_malloc(t->r,sizeof(*_tmp3));({struct _tuple0*_tmp27=({struct _tuple0*_tmp2=_region_malloc(t->r,sizeof(*_tmp2));_tmp2->f1=key,_tmp2->f2=val;_tmp2;});_tmp3->hd=_tmp27;}),_tmp3->tl=*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),bucket));_tmp3;});*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),bucket))=_tmp28;});
if(({int _tmp29=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),bucket)));_tmp29 > t->max_len;}))
Cyc_Hashtable_resize(t);}
# 57
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key){
struct _dyneither_ptr tab=t->tab;
struct Cyc_List_List*l=*((struct Cyc_List_List**)({typeof(tab )_tmp2B=tab;_check_dyneither_subscript(_tmp2B,sizeof(struct Cyc_List_List*),(int)({unsigned int _tmp2A=(unsigned int)(t->hash)(key);_tmp2A % _get_dyneither_size(tab,sizeof(struct Cyc_List_List*));}));}));
return Cyc_List_assoc_cmp(t->cmp,l,key);}
# 63
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key){
struct _dyneither_ptr _tmp4=t->tab;
struct Cyc_List_List*_tmp5=*((struct Cyc_List_List**)({typeof(_tmp4 )_tmp2D=_tmp4;_check_dyneither_subscript(_tmp2D,sizeof(struct Cyc_List_List*),(int)({unsigned int _tmp2C=(unsigned int)(t->hash)(key);_tmp2C % _get_dyneither_size(_tmp4,sizeof(struct Cyc_List_List*));}));}));
int(*_tmp6)(void*,void*)=t->cmp;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple0*_tmp7=(struct _tuple0*)_tmp5->hd;struct _tuple0*_tmp8=_tmp7;void*_tmpA;void**_tmp9;_LL1: _tmpA=_tmp8->f1;_tmp9=(void**)& _tmp8->f2;_LL2:;
if(_tmp6(key,_tmpA)== 0)return _tmp9;}
# 71
return 0;}
# 74
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data){
struct _dyneither_ptr _tmpB=t->tab;
struct Cyc_List_List*_tmpC=*((struct Cyc_List_List**)({typeof(_tmpB )_tmp2F=_tmpB;_check_dyneither_subscript(_tmp2F,sizeof(struct Cyc_List_List*),(int)({unsigned int _tmp2E=(unsigned int)(t->hash)(key);_tmp2E % _get_dyneither_size(_tmpB,sizeof(struct Cyc_List_List*));}));}));
int(*_tmpD)(void*,void*)=t->cmp;
for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct _tuple0 _tmpE=*((struct _tuple0*)_tmpC->hd);struct _tuple0 _tmpF=_tmpE;void*_tmp11;void*_tmp10;_LL1: _tmp11=_tmpF.f1;_tmp10=_tmpF.f2;_LL2:;
if(_tmpD(key,_tmp11)== 0){
*data=_tmp10;
return 1;}}
# 85
return 0;}
# 88
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key){
# 91
struct _dyneither_ptr _tmp12=t->tab;
int(*_tmp13)(void*,void*)=t->cmp;
int bucket=(int)({unsigned int _tmp30=(unsigned int)(t->hash)(key);_tmp30 % _get_dyneither_size(_tmp12,sizeof(struct Cyc_List_List*));});
struct Cyc_List_List*_tmp14=*((struct Cyc_List_List**)_check_dyneither_subscript(_tmp12,sizeof(struct Cyc_List_List*),bucket));
if(_tmp14 == 0)return;
if(_tmp13(key,(((struct _tuple0*)_tmp14->hd)[0]).f1)== 0){
*((struct Cyc_List_List**)_check_dyneither_subscript(_tmp12,sizeof(struct Cyc_List_List*),bucket))=_tmp14->tl;
return;}{
# 100
struct Cyc_List_List*_tmp15=_tmp14->tl;for(0;((struct Cyc_List_List*)_check_null(_tmp14))->tl != 0;(_tmp14=_tmp14->tl,_tmp15=_tmp15->tl)){
# 102
if(_tmp13(key,(((struct _tuple0*)((struct Cyc_List_List*)_check_null(_tmp15))->hd)[0]).f1)== 0){
_tmp14->tl=_tmp15->tl;
return;}}};}
# 108
int Cyc_Hashtable_hash_string(struct _dyneither_ptr s){
# 110
int ans=0;
int sz=(int)_get_dyneither_size(s,sizeof(char));
int shift=0;
{int i=0;for(0;i < sz;++ i){
ans=ans ^ (int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i))<< shift;
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
int nidx=(int)({unsigned int _tmp31=(unsigned int)hash(key);_tmp31 % _get_dyneither_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp33=({struct Cyc_List_List*_tmp17=_region_malloc(r,sizeof(*_tmp17));({struct _tuple0*_tmp32=({struct _tuple0*_tmp16=_region_malloc(r,sizeof(*_tmp16));_tmp16->f1=key,_tmp16->f2=val;_tmp16;});_tmp17->hd=_tmp32;}),_tmp17->tl=*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),nidx));_tmp17;});*((struct Cyc_List_List**)_check_dyneither_subscript(tab,sizeof(struct Cyc_List_List*),nidx))=_tmp33;});};}
# 138
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){
struct _dyneither_ptr odata=t->tab;
int osize=(int)_get_dyneither_size(odata,sizeof(struct Cyc_List_List*));
int nsize=2 * osize + 1;
struct _dyneither_ptr ndata=({unsigned int _tmp18=nsize;_tag_dyneither(_region_calloc(t->r,sizeof(struct Cyc_List_List*),_tmp18),sizeof(struct Cyc_List_List*),_tmp18);});
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
({struct Cyc_Int_pa_PrintArg_struct _tmp1B=({struct Cyc_Int_pa_PrintArg_struct _tmp24;_tmp24.tag=1U,_tmp24.f1=(unsigned long)i;_tmp24;});void*_tmp19[1U];_tmp19[0]=& _tmp1B;({struct _dyneither_ptr _tmp34=({const char*_tmp1A="%d: ";_tag_dyneither(_tmp1A,sizeof(char),5U);});Cyc_printf(_tmp34,_tag_dyneither(_tmp19,sizeof(void*),1U));});});
{struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_dyneither_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
({void*_tmp1C=0U;({struct _dyneither_ptr _tmp35=({const char*_tmp1D="(";_tag_dyneither(_tmp1D,sizeof(char),2U);});Cyc_printf(_tmp35,_tag_dyneither(_tmp1C,sizeof(void*),0U));});});
prn_key((((struct _tuple0*)iter->hd)[0]).f1);
({void*_tmp1E=0U;({struct _dyneither_ptr _tmp36=({const char*_tmp1F=",";_tag_dyneither(_tmp1F,sizeof(char),2U);});Cyc_printf(_tmp36,_tag_dyneither(_tmp1E,sizeof(void*),0U));});});
prn_val((((struct _tuple0*)iter->hd)[0]).f2);
({void*_tmp20=0U;({struct _dyneither_ptr _tmp37=({const char*_tmp21=") ";_tag_dyneither(_tmp21,sizeof(char),3U);});Cyc_printf(_tmp37,_tag_dyneither(_tmp20,sizeof(void*),0U));});});}}
# 180
({void*_tmp22=0U;({struct _dyneither_ptr _tmp38=({const char*_tmp23="\n";_tag_dyneither(_tmp23,sizeof(char),2U);});Cyc_printf(_tmp38,_tag_dyneither(_tmp22,sizeof(void*),0U));});});}}

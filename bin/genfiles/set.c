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
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char *)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  if (!_cks_ptr) _throw_null(); \
  (_cks_ptr) + (elt_sz)*(index); })
#endif

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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 117
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 100 "set.h"
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 106
int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Set_Set{int(*cmp)(void*,void*);int cardinality;struct Cyc_List_List*nodes;};
# 39 "set.cyc"
struct Cyc_Set_Set*Cyc_Set_empty(int(*comp)(void*,void*)){
return({struct Cyc_Set_Set*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->cmp=comp,_tmp0->cardinality=0,_tmp0->nodes=0;_tmp0;});}
# 42
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*rgn,int(*comp)(void*,void*)){
return({struct Cyc_Set_Set*_tmp1=_region_malloc(rgn,sizeof(*_tmp1));_tmp1->cmp=comp,_tmp1->cardinality=0,_tmp1->nodes=0;_tmp1;});}
# 46
struct Cyc_Set_Set*Cyc_Set_singleton(int(*comp)(void*,void*),void*x){
return({struct Cyc_Set_Set*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->cmp=comp,_tmp3->cardinality=1,({struct Cyc_List_List*_tmp1D=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=x,_tmp2->tl=0;_tmp2;});_tmp3->nodes=_tmp1D;});_tmp3;});}
# 50
int Cyc_Set_cardinality(struct Cyc_Set_Set*s){
return s->cardinality;}
# 55
int Cyc_Set_is_empty(struct Cyc_Set_Set*s){
return s->cardinality == 0;}
# 60
static int Cyc_Set_member_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
while(n != 0){
int i=cmp(elt,n->hd);
if(i == 0)return 1;else{
if(i < 0)return 0;else{
n=n->tl;}}}
# 67
return 0;}
# 70
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt){
return Cyc_Set_member_b(s->cmp,s->nodes,elt);}
# 78
static struct Cyc_List_List*Cyc_Set_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
# 81
if(n == 0)
return({struct Cyc_List_List*_tmp4=_region_malloc(rgn,sizeof(*_tmp4));_tmp4->hd=elt,_tmp4->tl=0;_tmp4;});else{
# 84
int i=cmp(elt,n->hd);
if(i < 0)
return({struct Cyc_List_List*_tmp5=_region_malloc(rgn,sizeof(*_tmp5));_tmp5->hd=elt,_tmp5->tl=n;_tmp5;});else{
# 88
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp8=_region_malloc(rgn,sizeof(*_tmp8));_tmp8->hd=n->hd,_tmp8->tl=0;_tmp8;});
struct Cyc_List_List*prev=result;
n=n->tl;
while(n != 0 &&(i=cmp(n->hd,elt))< 0){
({struct Cyc_List_List*_tmp1E=({struct Cyc_List_List*_tmp6=_region_malloc(rgn,sizeof(*_tmp6));_tmp6->hd=n->hd,_tmp6->tl=0;_tmp6;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp1E;});
prev=prev->tl;
n=n->tl;}
# 96
({struct Cyc_List_List*_tmp1F=({struct Cyc_List_List*_tmp7=_region_malloc(rgn,sizeof(*_tmp7));_tmp7->hd=elt,_tmp7->tl=n;_tmp7;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp1F;});
return result;}}}
# 103
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return s;else{
return({struct Cyc_Set_Set*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->cmp=s->cmp,_tmp9->cardinality=s->cardinality + 1,({
struct Cyc_List_List*_tmp20=Cyc_Set_insert_b(Cyc_Core_heap_region,s->cmp,s->nodes,elt);_tmp9->nodes=_tmp20;});_tmp9;});}}
# 108
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return s;else{
return({struct Cyc_Set_Set*_tmpA=_region_malloc(rgn,sizeof(*_tmpA));_tmpA->cmp=s->cmp,_tmpA->cardinality=s->cardinality + 1,({
struct Cyc_List_List*_tmp21=Cyc_Set_insert_b(rgn,s->cmp,s->nodes,elt);_tmpA->nodes=_tmp21;});_tmpA;});}}
# 117
static struct Cyc_List_List*Cyc_Set_imp_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
# 120
if(n == 0)
return({struct Cyc_List_List*_tmpB=_region_malloc(rgn,sizeof(*_tmpB));_tmpB->hd=elt,_tmpB->tl=0;_tmpB;});else{
# 123
int i=cmp(elt,n->hd);
if(i < 0)
return({struct Cyc_List_List*_tmpC=_region_malloc(rgn,sizeof(*_tmpC));_tmpC->hd=elt,_tmpC->tl=n;_tmpC;});else{
# 127
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;
n=n->tl;
while(n != 0 &&(i=cmp(n->hd,elt))< 0){
prev=((struct Cyc_List_List*)_check_null(prev))->tl;
n=n->tl;}
# 133
({struct Cyc_List_List*_tmp22=({struct Cyc_List_List*_tmpD=_region_malloc(rgn,sizeof(*_tmpD));_tmpD->hd=elt,_tmpD->tl=n;_tmpD;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp22;});
return res;}}}
# 140
void Cyc_Set_imp_insert(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return;
({struct Cyc_List_List*_tmp23=Cyc_Set_imp_insert_b(Cyc_Core_heap_region,s->cmp,s->nodes,elt);s->nodes=_tmp23;});
return;}
# 145
void Cyc_Set_imp_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return;
({struct Cyc_List_List*_tmp24=Cyc_Set_imp_insert_b(rgn,s->cmp,s->nodes,elt);s->nodes=_tmp24;});
return;}
# 152
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if(s1 == s2)
return s1;
if(s1->cardinality == 0)
return s2;
if(s2->cardinality == 0)
return s1;{
# 160
struct Cyc_List_List*nodes=0;
int cardinality=0;
int(*comp)(void*,void*)=s1->cmp;
# 164
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
struct Cyc_List_List*curr=0;
# 168
while(x1 != 0 && x2 != 0){
int i=comp(x1->hd,x2->hd);
if(i == 0)
# 172
x2=x2->tl;else{
if(i < 0){
# 175
if(curr == 0){
nodes=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=x1->hd,_tmpE->tl=0;_tmpE;});
curr=nodes;}else{
# 179
({struct Cyc_List_List*_tmp25=({struct Cyc_List_List*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->hd=x1->hd,_tmpF->tl=0;_tmpF;});curr->tl=_tmp25;});
curr=curr->tl;}
# 182
x1=x1->tl;
++ cardinality;}else{
# 186
if(curr == 0){
nodes=({struct Cyc_List_List*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->hd=x2->hd,_tmp10->tl=0;_tmp10;});
curr=nodes;}else{
# 190
({struct Cyc_List_List*_tmp26=({struct Cyc_List_List*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->hd=x2->hd,_tmp11->tl=0;_tmp11;});curr->tl=_tmp26;});
curr=curr->tl;}
# 193
x2=x2->tl;
++ cardinality;}}}
# 197
if(x1 != 0){
# 199
if(curr == 0)
nodes=x1;else{
# 202
curr->tl=x1;}
({int _tmp27=Cyc_List_length(x1);cardinality +=_tmp27;});}else{
if(x2 != 0){
# 206
if(curr == 0)
nodes=x2;else{
# 209
curr->tl=x2;}
({int _tmp28=Cyc_List_length(x2);cardinality +=_tmp28;});}}
# 212
return({struct Cyc_Set_Set*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->cmp=comp,_tmp12->cardinality=cardinality,_tmp12->nodes=nodes;_tmp12;});};}
# 218
static struct Cyc_List_List*Cyc_Set_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
if(cmp(((struct Cyc_List_List*)_check_null(n))->hd,elt)== 0)return n->tl;{
# 221
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=n->hd,_tmp14->tl=0;_tmp14;});
struct Cyc_List_List*prev=result;
n=n->tl;
while(n != 0 && cmp(n->hd,elt)!= 0){
({struct Cyc_List_List*_tmp29=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=n->hd,_tmp13->tl=0;_tmp13;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp29;});
prev=prev->tl;
n=n->tl;}
# 229
({struct Cyc_List_List*_tmp2A=((struct Cyc_List_List*)_check_null(n))->tl;((struct Cyc_List_List*)_check_null(prev))->tl=_tmp2A;});
return result;};}
# 234
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))
return({struct Cyc_Set_Set*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->cmp=s->cmp,_tmp15->cardinality=s->cardinality - 1,({
struct Cyc_List_List*_tmp2B=Cyc_Set_delete_b(s->cmp,s->nodes,elt);_tmp15->nodes=_tmp2B;});_tmp15;});else{
return s;}}
# 245
static struct Cyc_List_List*Cyc_Set_imp_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt,void**ret){
if(cmp(((struct Cyc_List_List*)_check_null(n))->hd,elt)== 0)return n->tl;{
# 248
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;
n=n->tl;
while(n != 0 && cmp(n->hd,elt)!= 0){
prev=((struct Cyc_List_List*)_check_null(prev))->tl;
n=n->tl;}
# 254
({struct Cyc_List_List*_tmp2C=((struct Cyc_List_List*)_check_null(n))->tl;((struct Cyc_List_List*)_check_null(prev))->tl=_tmp2C;});
*ret=n->hd;
return res;};}
# 260
void*Cyc_Set_imp_delete(struct Cyc_Set_Set*s,void*elt){
void*ret=elt;
if(Cyc_Set_member(s,elt))
({struct Cyc_List_List*_tmp2D=Cyc_Set_imp_delete_b(s->cmp,s->nodes,elt,& ret);s->nodes=_tmp2D;});
return ret;}
# 268
void*Cyc_Set_fold(void*(*f)(void*,void*),struct Cyc_Set_Set*s,void*accum){
struct Cyc_List_List*n=s->nodes;
# 271
while(n != 0){
accum=f(n->hd,accum);
n=n->tl;}
# 275
return accum;}
# 277
void*Cyc_Set_fold_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Set_Set*s,void*accum){
struct Cyc_List_List*n=s->nodes;
# 280
while(n != 0){
accum=f(env,n->hd,accum);
n=n->tl;}
# 284
return accum;}
# 289
void Cyc_Set_app(void*(*f)(void*),struct Cyc_Set_Set*s){
Cyc_List_app(f,s->nodes);}
# 292
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s){
Cyc_List_iter(f,s->nodes);}
# 295
void Cyc_Set_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Set_Set*s){
Cyc_List_iter_c(f,env,s->nodes);}
# 302
struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if(s1 == s2)
return s1;{
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
struct Cyc_List_List*result=0;struct Cyc_List_List*prev=0;
int card=0;
if(x1 == 0)
return s1;
if(x2 == 0)
return s2;
# 315
while(x1 != 0 && x2 != 0){
int i=comp(x1->hd,x2->hd);
if(i == 0){
if(result == 0){
result=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->hd=x1->hd,_tmp16->tl=0;_tmp16;});
prev=result;}else{
# 322
({struct Cyc_List_List*_tmp2E=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=x1->hd,_tmp17->tl=0;_tmp17;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp2E;});
prev=prev->tl;}
# 325
++ card;
x1=x1->tl;
x2=x2->tl;}else{
if(i < 0)
x1=x1->tl;else{
# 331
x2=x2->tl;}}}
# 334
return({struct Cyc_Set_Set*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->cmp=comp,_tmp18->cardinality=card,_tmp18->nodes=result;_tmp18;});};}
# 337
struct Cyc_Set_Set*Cyc_Set_from_list(int(*comp)(void*,void*),struct Cyc_List_List*x){
struct Cyc_List_List*z=Cyc_List_merge_sort(comp,x);
# 340
{struct Cyc_List_List*y=z;for(0;y != 0;y=y->tl){
if(y->tl != 0 && comp(y->hd,((struct Cyc_List_List*)_check_null(y->tl))->hd)== 0)
y->tl=((struct Cyc_List_List*)_check_null(y->tl))->tl;}}
# 344
return({struct Cyc_Set_Set*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->cmp=comp,({int _tmp2F=Cyc_List_length(z);_tmp19->cardinality=_tmp2F;}),_tmp19->nodes=z;_tmp19;});}
# 347
int Cyc_Set_subset(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
# 352
while(1){
if(x1 == 0)return 1;
if(x2 == 0)return 0;{
int i=comp(x1->hd,x2->hd);
if(i == 0){
x1=x1->tl;
x2=x2->tl;}else{
if(i > 0)
x2=x2->tl;else{
return 0;}}};}
# 363
return 1;}
# 366
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
int card=s1->cardinality;
# 372
if(x2 == 0)return s1;
# 374
while(x2 != 0){
void*elt=x2->hd;
# 377
if(Cyc_Set_member_b(comp,x1,elt)){
-- card;
x1=Cyc_Set_delete_b(comp,x1,elt);}
# 381
x2=x2->tl;}
# 383
return({struct Cyc_Set_Set*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->cmp=comp,_tmp1A->cardinality=card,_tmp1A->nodes=x1;_tmp1A;});}
# 386
int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
# 389
if(s1->cardinality != s2->cardinality)return s1->cardinality - s2->cardinality;{
# 391
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
int(*cmp)(void*,void*)=s1->cmp;
while(x1 != 0){
int diff=cmp(x1->hd,((struct Cyc_List_List*)_check_null(x2))->hd);
if(diff != 0)return diff;
x1=x1->tl;
x2=x2->tl;}
# 400
return 0;};}
# 403
int Cyc_Set_equals(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
return Cyc_Set_setcmp(s1,s2)== 0;}char Cyc_Set_Absent[7U]="Absent";
# 408
struct Cyc_Set_Absent_exn_struct Cyc_Set_Absent_val={Cyc_Set_Absent};
# 413
void*Cyc_Set_choose(struct Cyc_Set_Set*s){
if(s->nodes == 0)(int)_throw((void*)& Cyc_Set_Absent_val);
return((struct Cyc_List_List*)_check_null(s->nodes))->hd;}
# 418
int Cyc_Set_iter_f(struct Cyc_List_List**elts_left,void**dest){
if(!((unsigned)*elts_left))
return 0;
*dest=((struct Cyc_List_List*)_check_null(*elts_left))->hd;
*elts_left=((struct Cyc_List_List*)_check_null(*elts_left))->tl;
return 1;}
# 425
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s){
# 428
return({struct Cyc_Iter_Iter _tmp1C;({struct Cyc_List_List**_tmp30=({struct Cyc_List_List**_tmp1B=_region_malloc(rgn,sizeof(*_tmp1B));*_tmp1B=s->nodes;_tmp1B;});_tmp1C.env=_tmp30;}),_tmp1C.next=Cyc_Set_iter_f;_tmp1C;});}

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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1145 "absyn.h"
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1151
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*name);
# 1153
struct _dyneither_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1159
int Cyc_Absyn_equal_att(void*a1,void*a2);
# 32 "cifc.h"
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl);
# 36
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_List_List*ds,struct Cyc_List_List*ovrs);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 33 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 47
int Cyc_Tcutil_is_function_type(void*t);
# 99
void*Cyc_Tcutil_copy_type(void*t);
# 194
int Cyc_Tcutil_unify(void*,void*);struct Cyc_Hashtable_Table;
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();
# 35 "cifc.cyc"
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _dyneither_ptr s){
# 37
if(is_buildlib){
struct _dyneither_ptr preamble=warn?({const char*_tmp5="Warning: user-defined";_tag_dyneither(_tmp5,sizeof(char),22U);}):({const char*_tmp6="User-defined";_tag_dyneither(_tmp6,sizeof(char),13U);});
({struct Cyc_String_pa_PrintArg_struct _tmp2=({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0U,_tmp65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)preamble);_tmp65;});struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0U,({
# 41
struct _dyneither_ptr _tmp7A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));_tmp64.f1=_tmp7A;});_tmp64;});struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0U,_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp63;});void*_tmp0[3U];_tmp0[0]=& _tmp2,_tmp0[1]=& _tmp3,_tmp0[2]=& _tmp4;({struct Cyc___cycFILE*_tmp7C=Cyc_stderr;struct _dyneither_ptr _tmp7B=({const char*_tmp1="%s type for %s incompatible with system version %s\n";_tag_dyneither(_tmp1,sizeof(char),52U);});Cyc_fprintf(_tmp7C,_tmp7B,_tag_dyneither(_tmp0,sizeof(void*),3U));});});}else{
# 43
if(warn)
({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0U,({
struct _dyneither_ptr _tmp7D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));_tmp67.f1=_tmp7D;});_tmp67;});struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0U,_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp66;});void*_tmp7[2U];_tmp7[0]=& _tmp9,_tmp7[1]=& _tmpA;({unsigned _tmp7F=loc;struct _dyneither_ptr _tmp7E=({const char*_tmp8="User-defined type for %s incompatible with system version %s";_tag_dyneither(_tmp8,sizeof(char),61U);});Cyc_Tcutil_warn(_tmp7F,_tmp7E,_tag_dyneither(_tmp7,sizeof(void*),2U));});});else{
# 47
({struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0U,({
struct _dyneither_ptr _tmp80=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));_tmp69.f1=_tmp80;});_tmp69;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0U,_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp68;});void*_tmpB[2U];_tmpB[0]=& _tmpD,_tmpB[1]=& _tmpE;({unsigned _tmp82=loc;struct _dyneither_ptr _tmp81=({const char*_tmpC="User-defined type for %s incompatible with system version %s";_tag_dyneither(_tmpC,sizeof(char),61U);});Cyc_Tcutil_terr(_tmp82,_tmp81,_tag_dyneither(_tmpB,sizeof(void*),2U));});});}}}
# 57
static int Cyc_Cifc_c_types_ok(void*ctyp,void*cyctyp){
void*_tmpF=Cyc_Toc_typ_to_c(Cyc_Tcutil_copy_type(ctyp));
void*_tmp10=Cyc_Toc_typ_to_c(Cyc_Tcutil_copy_type(cyctyp));
# 63
return Cyc_Tcutil_unify(_tmpF,_tmp10);}
# 67
static struct Cyc_List_List*Cyc_Cifc_merge_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
struct Cyc_List_List*x=0;
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))x=({struct Cyc_List_List*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->hd=(void*)a->hd,_tmp11->tl=x;_tmp11;});}}
return({struct Cyc_List_List*_tmp83=x;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp83,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(a2));});}struct _tuple10{void*f1;void*f2;};struct _tuple11{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 77
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 79
struct _tuple10 _tmp12=({struct _tuple10 _tmp78;_tmp78.f1=c_decl->r,_tmp78.f2=user_decl->r;_tmp78;});struct _tuple10 _tmp13=_tmp12;struct Cyc_Absyn_Typedefdecl*_tmp59;struct Cyc_Absyn_Typedefdecl*_tmp58;struct Cyc_Absyn_Enumdecl*_tmp57;struct Cyc_Absyn_Enumdecl*_tmp56;struct Cyc_Absyn_Aggrdecl*_tmp55;struct Cyc_Absyn_Aggrdecl*_tmp54;struct Cyc_Absyn_Fndecl*_tmp53;struct Cyc_Absyn_Vardecl*_tmp52;struct Cyc_Absyn_Vardecl*_tmp51;struct Cyc_Absyn_Vardecl*_tmp50;switch(*((int*)_tmp13.f1)){case 0U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13.f2)->tag == 0U){_LL1: _tmp51=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13.f1)->f1;_tmp50=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13.f2)->f1;_LL2:
# 81
 if(!Cyc_Cifc_c_types_ok(_tmp51->type,_tmp50->type)){
({unsigned _tmp89=loc;int _tmp88=is_buildlib;struct _tuple0*_tmp87=_tmp51->name;Cyc_Cifc_fail_merge(0,_tmp89,_tmp88,_tmp87,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0U,({struct _dyneither_ptr _tmp84=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp50->type));_tmp6B.f1=_tmp84;});_tmp6B;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0U,({struct _dyneither_ptr _tmp85=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp51->type));_tmp6A.f1=_tmp85;});_tmp6A;});void*_tmp14[2U];_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17;({struct _dyneither_ptr _tmp86=({const char*_tmp15=": type %s != %s";_tag_dyneither(_tmp15,sizeof(char),16U);});Cyc_aprintf(_tmp86,_tag_dyneither(_tmp14,sizeof(void*),2U));});}));});if(!0)return;}else{
# 89
if(_tmp50->attributes != 0)
({struct Cyc_List_List*_tmp8A=Cyc_Cifc_merge_attributes(_tmp51->attributes,_tmp50->attributes);_tmp51->attributes=_tmp8A;});
_tmp51->type=_tmp50->type;
_tmp51->tq=_tmp50->tq;}
# 94
goto _LL0;}else{goto _LLB;}case 1U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13.f2)->tag == 0U){_LL3: _tmp53=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp13.f1)->f1;_tmp52=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13.f2)->f1;_LL4:
# 97
 if(!Cyc_Tcutil_is_function_type(_tmp52->type)){
({unsigned _tmp8D=loc;int _tmp8C=is_buildlib;struct _tuple0*_tmp8B=_tmp52->name;Cyc_Cifc_fail_merge(0,_tmp8D,_tmp8C,_tmp8B,({const char*_tmp18=": type must be a function type to match decl\n";_tag_dyneither(_tmp18,sizeof(char),46U);}));});if(!0)return;}{
# 100
void*cdtype;
if(_tmp53->cached_type != 0)
cdtype=(void*)_check_null(_tmp53->cached_type);else{
# 104
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=5U,_tmp19->f1=_tmp53->i;_tmp19;});}
if(!Cyc_Cifc_c_types_ok(_tmp52->type,cdtype)){
({unsigned _tmp93=loc;int _tmp92=is_buildlib;struct _tuple0*_tmp91=_tmp52->name;Cyc_Cifc_fail_merge(0,_tmp93,_tmp92,_tmp91,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0U,({struct _dyneither_ptr _tmp8E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(cdtype));_tmp6D.f1=_tmp8E;});_tmp6D;});struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0U,({struct _dyneither_ptr _tmp8F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp52->type));_tmp6C.f1=_tmp8F;});_tmp6C;});void*_tmp1A[2U];_tmp1A[0]=& _tmp1C,_tmp1A[1]=& _tmp1D;({struct _dyneither_ptr _tmp90=({const char*_tmp1B=": type %s != %s";_tag_dyneither(_tmp1B,sizeof(char),16U);});Cyc_aprintf(_tmp90,_tag_dyneither(_tmp1A,sizeof(void*),2U));});}));});if(!0)return;}else{
# 113
void*_tmp1E=_tmp52->type;void*_tmp1F=_tmp1E;struct Cyc_Absyn_FnInfo _tmp22;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F)->tag == 5U){_LLE: _tmp22=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F)->f1;_LLF:
# 115
 if((_tmp53->i).attributes != 0)
({struct Cyc_List_List*_tmp94=Cyc_Cifc_merge_attributes(_tmp22.attributes,(_tmp53->i).attributes);_tmp22.attributes=_tmp94;});
_tmp53->i=_tmp22;
goto _LLD;}else{_LL10: _LL11:
({void*_tmp20=0U;({struct _dyneither_ptr _tmp95=({const char*_tmp21="oops!\n";_tag_dyneither(_tmp21,sizeof(char),7U);});Cyc_Tcutil_terr(0U,_tmp95,_tag_dyneither(_tmp20,sizeof(void*),0U));});});}_LLD:;}
# 122
goto _LL0;};}else{goto _LLB;}case 5U: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp13.f2)->tag == 5U){_LL5: _tmp55=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp13.f1)->f1;_tmp54=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp13.f2)->f1;_LL6:
# 130
 if((int)_tmp54->sc != (int)_tmp55->sc){
({unsigned _tmp98=loc;int _tmp97=is_buildlib;struct _tuple0*_tmp96=_tmp55->name;Cyc_Cifc_fail_merge(1,_tmp98,_tmp97,_tmp96,({const char*_tmp23=": scopes don't match (ignoring)";_tag_dyneither(_tmp23,sizeof(char),32U);}));});if(!1)return;}
# 133
if(_tmp54->impl == 0){
({unsigned _tmp9B=loc;int _tmp9A=is_buildlib;struct _tuple0*_tmp99=_tmp55->name;Cyc_Cifc_fail_merge(0,_tmp9B,_tmp9A,_tmp99,({const char*_tmp24=": no user definition";_tag_dyneither(_tmp24,sizeof(char),21U);}));});if(!0)return;}
if(_tmp55->impl == 0){
({unsigned _tmp9E=loc;int _tmp9D=is_buildlib;struct _tuple0*_tmp9C=_tmp55->name;Cyc_Cifc_fail_merge(1,_tmp9E,_tmp9D,_tmp9C,({const char*_tmp25=": no definition for system version";_tag_dyneither(_tmp25,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 140
{struct _tuple11 _tmp26=({struct _tuple11 _tmp73;_tmp73.f1=_tmp55->impl,_tmp73.f2=_tmp54->impl;_tmp73;});struct _tuple11 _tmp27=_tmp26;struct Cyc_List_List**_tmp40;struct Cyc_List_List**_tmp3F;struct Cyc_List_List*_tmp3E;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3B;int _tmp3A;if(_tmp27.f1 != 0){if(_tmp27.f2 != 0){_LL13: _tmp40=(struct Cyc_List_List**)&(_tmp27.f1)->exist_vars;_tmp3F=(struct Cyc_List_List**)&(_tmp27.f1)->rgn_po;_tmp3E=(_tmp27.f1)->fields;_tmp3D=(_tmp27.f2)->exist_vars;_tmp3C=(_tmp27.f2)->rgn_po;_tmp3B=(_tmp27.f2)->fields;_tmp3A=(_tmp27.f2)->tagged;_LL14:
# 143
 if(_tmp3A){
({unsigned _tmpA1=loc;int _tmpA0=is_buildlib;struct _tuple0*_tmp9F=_tmp55->name;Cyc_Cifc_fail_merge(0,_tmpA1,_tmpA0,_tmp9F,({const char*_tmp28=": user @tagged annotation not allowed (ignoring)";_tag_dyneither(_tmp28,sizeof(char),49U);}));});if(!0)return;}{
# 146
struct Cyc_List_List*_tmp29=_tmp3E;
# 148
while(_tmp29 != 0){
struct Cyc_Absyn_Aggrfield*_tmp2A=(struct Cyc_Absyn_Aggrfield*)_tmp29->hd;
struct Cyc_Absyn_Aggrfield*_tmp2B=Cyc_Absyn_lookup_field(_tmp3B,_tmp2A->name);
# 152
if(_tmp2B != 0){
# 155
if(!Cyc_Cifc_c_types_ok(_tmp2A->type,_tmp2B->type)){
({unsigned _tmpA7=loc;int _tmpA6=is_buildlib;struct _tuple0*_tmpA5=_tmp55->name;Cyc_Cifc_fail_merge(0,_tmpA7,_tmpA6,_tmpA5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0U,({struct _dyneither_ptr _tmpA2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2B->type));_tmp70.f1=_tmpA2;});_tmp70;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0U,_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2A->name);_tmp6F;});struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp6E;_tmp6E.tag=0U,({struct _dyneither_ptr _tmpA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2A->type));_tmp6E.f1=_tmpA3;});_tmp6E;});void*_tmp2C[3U];_tmp2C[0]=& _tmp2E,_tmp2C[1]=& _tmp2F,_tmp2C[2]=& _tmp30;({struct _dyneither_ptr _tmpA4=({const char*_tmp2D=": type %s of user definition of field %s != %s";_tag_dyneither(_tmp2D,sizeof(char),47U);});Cyc_aprintf(_tmpA4,_tag_dyneither(_tmp2C,sizeof(void*),3U));});}));});if(!0)return;}else{
# 164
if(_tmp2B->width != 0){
({unsigned _tmpAC=loc;int _tmpAB=is_buildlib;struct _tuple0*_tmpAA=_tmp55->name;Cyc_Cifc_fail_merge(1,_tmpAC,_tmpAB,_tmpAA,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0U,({struct _dyneither_ptr _tmpA8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2B->type));_tmp71.f1=_tmpA8;});_tmp71;});void*_tmp31[1U];_tmp31[0]=& _tmp33;({struct _dyneither_ptr _tmpA9=({const char*_tmp32=": ignoring width of user definition of field %s";_tag_dyneither(_tmp32,sizeof(char),48U);});Cyc_aprintf(_tmpA9,_tag_dyneither(_tmp31,sizeof(void*),1U));});}));});if(!1)return;}
# 169
if(_tmp2B->attributes != 0)
({struct Cyc_List_List*_tmpAD=Cyc_Cifc_merge_attributes(_tmp2A->attributes,_tmp2B->attributes);_tmp2A->attributes=_tmpAD;});
# 173
_tmp2A->type=_tmp2B->type;
_tmp2A->tq=_tmp2B->tq;
_tmp2A->requires_clause=_tmp2B->requires_clause;}}
# 178
_tmp29=_tmp29->tl;}
# 182
if(_tmp54->tvs != 0)_tmp55->tvs=_tmp54->tvs;
if((unsigned)_tmp3D)*_tmp40=_tmp3D;
if((unsigned)_tmp3C)*_tmp3F=_tmp3C;
# 187
_tmp29=_tmp3B;{
int missing_fields=0;
while(_tmp29 != 0){
struct Cyc_Absyn_Aggrfield*_tmp34=Cyc_Absyn_lookup_field(_tmp3E,((struct Cyc_Absyn_Aggrfield*)_tmp29->hd)->name);
if(_tmp34 == 0){
missing_fields=1;
({unsigned _tmpB1=loc;int _tmpB0=is_buildlib;struct _tuple0*_tmpAF=_tmp55->name;Cyc_Cifc_fail_merge(1,_tmpB1,_tmpB0,_tmpAF,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0U,_tmp72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp29->hd)->name);_tmp72;});void*_tmp35[1U];_tmp35[0]=& _tmp37;({struct _dyneither_ptr _tmpAE=({const char*_tmp36=": no definition of field %s in system version";_tag_dyneither(_tmp36,sizeof(char),46U);});Cyc_aprintf(_tmpAE,_tag_dyneither(_tmp35,sizeof(void*),1U));});}));});if(!1)return;}
# 197
_tmp29=_tmp29->tl;}
# 199
goto _LL12;};};}else{goto _LL15;}}else{_LL15: _LL16:
# 201
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmpB2=({const char*_tmp38="Internal Error: NULL cases not possible";_tag_dyneither(_tmp38,sizeof(char),40U);});_tmp39->f1=_tmpB2;});_tmp39;}));}_LL12:;}
# 203
goto _LL0;}else{goto _LLB;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp13.f2)->tag == 7U){_LL7: _tmp57=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp13.f1)->f1;_tmp56=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp13.f2)->f1;_LL8:
# 206
({unsigned _tmpB5=loc;int _tmpB4=is_buildlib;struct _tuple0*_tmpB3=_tmp57->name;Cyc_Cifc_fail_merge(0,_tmpB5,_tmpB4,_tmpB3,({const char*_tmp41=": enum merging not currently supported";_tag_dyneither(_tmp41,sizeof(char),39U);}));});if(!0)return;}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp13.f2)->tag == 8U){_LL9: _tmp59=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp13.f1)->f1;_tmp58=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp13.f2)->f1;_LLA:
# 209
 if(_tmp58->defn == 0){
({unsigned _tmpB8=loc;int _tmpB7=is_buildlib;struct _tuple0*_tmpB6=_tmp59->name;Cyc_Cifc_fail_merge(0,_tmpB8,_tmpB7,_tmpB6,({const char*_tmp42=": no user definition";_tag_dyneither(_tmp42,sizeof(char),21U);}));});if(!0)return;}
if(_tmp59->defn == 0){
({unsigned _tmpBB=loc;int _tmpBA=is_buildlib;struct _tuple0*_tmpB9=_tmp59->name;Cyc_Cifc_fail_merge(1,_tmpBB,_tmpBA,_tmpB9,({const char*_tmp43=": no definition for system version";_tag_dyneither(_tmp43,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 217
if(!({void*_tmpBC=(void*)_check_null(_tmp59->defn);Cyc_Cifc_c_types_ok(_tmpBC,(void*)_check_null(_tmp58->defn));})){
({unsigned _tmpC2=loc;int _tmpC1=is_buildlib;struct _tuple0*_tmpC0=_tmp59->name;Cyc_Cifc_fail_merge(0,_tmpC2,_tmpC1,_tmpC0,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp46=({struct Cyc_String_pa_PrintArg_struct _tmp75;_tmp75.tag=0U,({struct _dyneither_ptr _tmpBD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp58->defn)));_tmp75.f1=_tmpBD;});_tmp75;});struct Cyc_String_pa_PrintArg_struct _tmp47=({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0U,({struct _dyneither_ptr _tmpBE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp59->defn)));_tmp74.f1=_tmpBE;});_tmp74;});void*_tmp44[2U];_tmp44[0]=& _tmp46,_tmp44[1]=& _tmp47;({struct _dyneither_ptr _tmpBF=({const char*_tmp45=": type definition %s of user definition != %s";_tag_dyneither(_tmp45,sizeof(char),46U);});Cyc_aprintf(_tmpBF,_tag_dyneither(_tmp44,sizeof(void*),2U));});}));});if(!0)return;}else{
# 224
_tmp59->tvs=_tmp58->tvs;
_tmp59->defn=_tmp58->defn;
_tmp59->tq=_tmp58->tq;
if(_tmp58->atts != 0)
({struct Cyc_List_List*_tmpC3=Cyc_Cifc_merge_attributes(_tmp59->atts,_tmp58->atts);_tmp59->atts=_tmpC3;});}
# 231
goto _LL0;}else{goto _LLB;}default: _LLB: _LLC:
# 234
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _tmp4A=({struct Cyc_String_pa_PrintArg_struct _tmp76;_tmp76.tag=0U,({
struct _dyneither_ptr _tmpC4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp4B[1U];_tmp4B[0]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp76.f1=_tmpC4;});_tmp76;});void*_tmp48[1U];_tmp48[0]=& _tmp4A;({struct Cyc___cycFILE*_tmpC6=Cyc_stderr;struct _dyneither_ptr _tmpC5=({const char*_tmp49="Error in .cys file: bad (or unsupported) user-defined type %s\n";_tag_dyneither(_tmp49,sizeof(char),63U);});Cyc_fprintf(_tmpC6,_tmpC5,_tag_dyneither(_tmp48,sizeof(void*),1U));});});else{
# 238
({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp77;_tmp77.tag=0U,({
struct _dyneither_ptr _tmpC7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp4F[1U];_tmp4F[0]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4F,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp77.f1=_tmpC7;});_tmp77;});void*_tmp4C[1U];_tmp4C[0]=& _tmp4E;({unsigned _tmpC9=loc;struct _dyneither_ptr _tmpC8=({const char*_tmp4D="bad (or unsupported) user-defined type %s";_tag_dyneither(_tmp4D,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC9,_tmpC8,_tag_dyneither(_tmp4C,sizeof(void*),1U));});});}
return;}_LL0:;}
# 244
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_List_List*ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*_tmp5A=ovrs;for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){
struct Cyc_Absyn_Decl*_tmp5B=(struct Cyc_Absyn_Decl*)_tmp5A->hd;
struct _dyneither_ptr*_tmp5C=Cyc_Absyn_decl_name(_tmp5B);
# 249
if(!((unsigned)_tmp5C))({void*_tmp5D=0U;({unsigned _tmpCB=_tmp5B->loc;struct _dyneither_ptr _tmpCA=({const char*_tmp5E="Overriding decl without a name\n";_tag_dyneither(_tmp5E,sizeof(char),32U);});Cyc_Tcutil_warn(_tmpCB,_tmpCA,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});else{
# 251
struct Cyc_Absyn_Decl*_tmp5F=Cyc_Absyn_lookup_decl(ds,_tmp5C);
if(!((unsigned)_tmp5F))({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp79;_tmp79.tag=0U,_tmp79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5C);_tmp79;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({unsigned _tmpCD=_tmp5B->loc;struct _dyneither_ptr _tmpCC=({const char*_tmp61="%s is overridden but not defined";_tag_dyneither(_tmp61,sizeof(char),33U);});Cyc_Tcutil_warn(_tmpCD,_tmpCC,_tag_dyneither(_tmp60,sizeof(void*),1U));});});else{
Cyc_Cifc_merge_sys_user_decl(loc,0,_tmp5B,_tmp5F);}}}}

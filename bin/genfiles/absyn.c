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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133 "list.h"
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 34 "position.h"
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 315
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 463
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
extern struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val;
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr);
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 675 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 879
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 896
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 903
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 908
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 911
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 916
void*Cyc_Absyn_compress_kb(void*);
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 922
int Cyc_Absyn_type2bool(int def,void*);
# 924
void*Cyc_Absyn_app_type(void*,struct _dyneither_ptr);
# 927
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 929
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 932
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 934
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 936
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned int i);
# 939
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 941
extern void*Cyc_Absyn_empty_effect;
# 943
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 945
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 960
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_exn_type();
# 964
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
# 967
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 970
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned int);
void*Cyc_Absyn_bounds_one();
# 976
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 978
void*Cyc_Absyn_starb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*bounds,void*zero_term);
# 981
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 984
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 987
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 990
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq);
# 992
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 994
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_type(struct _tuple0*name);
# 1000
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1003
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1078
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1098
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1105
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1109
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1120
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1123
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1126
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1129
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1132
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1137
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1143
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1155
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1157
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1162
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1164
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1166
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1168
int Cyc_Absyn_fntype_att(void*);
# 1170
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1172
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1174
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1176
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1178
int Cyc_Absyn_is_require_union_type(void*);
# 1180
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1183
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1186
extern int Cyc_Absyn_porting_c_code;
# 1188
extern int Cyc_Absyn_no_regions;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 115
void*Cyc_Tcutil_compress(void*t);
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 173
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 181
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 29 "absyn.cyc"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);
# 36
int Cyc_Cyclone_tovc_r=0;
# 38
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 46
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;};}
# 53
if(ss2 != 0)return - 1;
return 0;}
# 56
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 60
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;union Cyc_Absyn_Nmspace _tmpD;struct _dyneither_ptr*_tmpC;_LL1: _tmpD=_tmp0->f1;_tmpC=_tmp0->f2;_LL2:;{
struct _tuple0*_tmp1=q2;union Cyc_Absyn_Nmspace _tmpB;struct _dyneither_ptr*_tmpA;_LL4: _tmpB=_tmp1->f1;_tmpA=_tmp1->f2;_LL5:;{
int i=Cyc_strptrcmp(_tmpC,_tmpA);
if(i != 0)return i;{
struct _tuple12 _tmp2=({struct _tuple12 _tmp128;_tmp128.f1=_tmpD,_tmp128.f2=_tmpB;_tmp128;});struct _tuple12 _tmp3=_tmp2;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp4;switch(((_tmp3.f1).Abs_n).tag){case 4U: if(((_tmp3.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 72
 return - 1;}case 1U: switch(((_tmp3.f2).Loc_n).tag){case 1U: _LL9: _tmp5=((_tmp3.f1).Rel_n).val;_tmp4=((_tmp3.f2).Rel_n).val;_LLA:
# 68
 return Cyc_Absyn_strlist_cmp(_tmp5,_tmp4);case 4U: goto _LL11;default: _LL13: _LL14:
# 74
 return - 1;}case 2U: switch(((_tmp3.f2).Rel_n).tag){case 2U: _LLB: _tmp7=((_tmp3.f1).Abs_n).val;_tmp6=((_tmp3.f2).Abs_n).val;_LLC:
# 69
 return Cyc_Absyn_strlist_cmp(_tmp7,_tmp6);case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 76
 return - 1;}default: switch(((_tmp3.f2).Rel_n).tag){case 3U: _LLD: _tmp9=((_tmp3.f1).C_n).val;_tmp8=((_tmp3.f2).C_n).val;_LLE:
# 70
 return Cyc_Absyn_strlist_cmp(_tmp9,_tmp8);case 4U: _LL11: _LL12:
# 73
 return 1;case 1U: _LL15: _LL16:
# 75
 return 1;default: _LL19: _LL1A:
# 77
 return 1;}}_LL6:;};};};};}
# 81
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 87
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
if(C_scope)
return({union Cyc_Absyn_Nmspace _tmp129;(_tmp129.C_n).tag=3U,(_tmp129.C_n).val=x;_tmp129;});else{
# 92
return({union Cyc_Absyn_Nmspace _tmp12A;(_tmp12A.Abs_n).tag=2U,(_tmp12A.Abs_n).val=x;_tmp12A;});}}
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp12B;(_tmp12B.Rel_n).tag=1U,(_tmp12B.Rel_n).val=x;_tmp12B;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 97
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _tmpF=_tmpE;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 106
static int Cyc_Absyn_new_type_counter=0;
# 108
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=Cyc_Absyn_new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 111
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 115
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp12C;_tmp12C.print_const=x.print_const  || y.print_const,_tmp12C.q_volatile=
x.q_volatile  || y.q_volatile,_tmp12C.q_restrict=
x.q_restrict  || y.q_restrict,_tmp12C.real_const=
x.real_const  || y.real_const,({
unsigned int _tmp157=Cyc_Position_segment_join(x.loc,y.loc);_tmp12C.loc=_tmp157;});_tmp12C;});}
# 123
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp12D;_tmp12D.print_const=0,_tmp12D.q_volatile=0,_tmp12D.q_restrict=0,_tmp12D.real_const=0,_tmp12D.loc=loc;_tmp12D;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp12E;_tmp12E.print_const=1,_tmp12E.q_volatile=0,_tmp12E.q_restrict=0,_tmp12E.real_const=1,_tmp12E.loc=loc;_tmp12E;});}
# 126
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 128
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp12F;(_tmp12F.UnknownDatatype).tag=1U,(_tmp12F.UnknownDatatype).val=udi;_tmp12F;});}
# 131
extern union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp130;(_tmp130.KnownDatatype).tag=2U,(_tmp130.KnownDatatype).val=d;_tmp130;});}
# 134
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp131;(_tmp131.UnknownDatatypefield).tag=1U,(_tmp131.UnknownDatatypefield).val=s;_tmp131;});}
# 137
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp132;(_tmp132.KnownDatatypefield).tag=2U,((_tmp132.KnownDatatypefield).val).f1=dd,((_tmp132.KnownDatatypefield).val).f2=df;_tmp132;});}
# 140
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp133;(_tmp133.UnknownAggr).tag=1U,((_tmp133.UnknownAggr).val).f1=ak,((_tmp133.UnknownAggr).val).f2=n,((_tmp133.UnknownAggr).val).f3=tagged;_tmp133;});}
# 143
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp134;(_tmp134.KnownAggr).tag=2U,(_tmp134.KnownAggr).val=ad;_tmp134;});}
# 146
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp13;void**_tmp12;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp12=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8:
# 151
 _tmp13=_tmp12;goto _LLA;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 150
 return k;}else{_LL9: _tmp13=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA:
# 153
({void*_tmp158=Cyc_Absyn_compress_kb(*_tmp13);*_tmp13=_tmp158;});
return*_tmp13;}}_LL0:;}
# 157
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_tmp15=_tmp14;struct Cyc_Core_Opt**_tmp1A;struct Cyc_Absyn_Kind*_tmp19;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp17;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp17=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2:
 return _tmp17;case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4:
 _tmp1A=_tmp18;_tmp19=& Cyc_Tcutil_bk;goto _LL6;default: _LL5: _tmp1A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp19=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6:
# 162
({struct Cyc_Core_Opt*_tmp15A=({struct Cyc_Core_Opt*_tmp16=_cycalloc(sizeof(*_tmp16));({void*_tmp159=Cyc_Tcutil_kind_to_bound(_tmp19);_tmp16->v=_tmp159;});_tmp16;});*_tmp1A=_tmp15A;});
return _tmp19;}_LL0:;}
# 168
void*Cyc_Absyn_app_type(void*c,struct _dyneither_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp15B;});_tmp1B;});}
# 171
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 174
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_Absyn_VoidCon_val={0U};
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={3U};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={4U};
static struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct Cyc_Absyn_HeapCon_val={5U};
static struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct Cyc_Absyn_UniqueCon_val={6U};
static struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct Cyc_Absyn_RefCntCon_val={7U};
static struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct Cyc_Absyn_AccessCon_val={8U};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9U};
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10U};
static struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct Cyc_Absyn_TrueCon_val={11U};
static struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct Cyc_Absyn_FalseCon_val={12U};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13U};
static struct Cyc_Absyn_FatCon_Absyn_TyCon_struct Cyc_Absyn_FatCon_val={14U};
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SCharCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_UCharCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NCharCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SShortCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_UShortCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NShortCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SIntCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_UIntCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NIntCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SLongCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ULongCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NLongCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SLongLongCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ULongLongCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NLongLongCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
# 203
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_FloatCon_val={2U,0};static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_DoubleCon_val={2U,1};static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_LongDoubleCon_val={2U,2};
# 208
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0U,(void*)& Cyc_Absyn_VoidCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={0U,(void*)& Cyc_Absyn_HeapCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={0U,(void*)& Cyc_Absyn_UniqueCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={0U,(void*)& Cyc_Absyn_RefCntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_TrueType_val={0U,(void*)& Cyc_Absyn_TrueCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_FalseType_val={0U,(void*)& Cyc_Absyn_FalseCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_FatType_val={0U,(void*)& Cyc_Absyn_FatCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NCharType_val={0U,(void*)& Cyc_Absyn_NCharCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SCharType_val={0U,(void*)& Cyc_Absyn_SCharCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UCharType_val={0U,(void*)& Cyc_Absyn_UCharCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NShortType_val={0U,(void*)& Cyc_Absyn_NShortCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SShortType_val={0U,(void*)& Cyc_Absyn_SShortCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UShortType_val={0U,(void*)& Cyc_Absyn_UShortCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NIntType_val={0U,(void*)& Cyc_Absyn_NIntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SIntType_val={0U,(void*)& Cyc_Absyn_SIntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UIntType_val={0U,(void*)& Cyc_Absyn_UIntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NLongType_val={0U,(void*)& Cyc_Absyn_NLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SLongType_val={0U,(void*)& Cyc_Absyn_SLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ULongType_val={0U,(void*)& Cyc_Absyn_ULongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NLongLongType_val={0U,(void*)& Cyc_Absyn_NLongLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SLongLongType_val={0U,(void*)& Cyc_Absyn_SLongLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ULongLongType_val={0U,(void*)& Cyc_Absyn_ULongLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_FloatType_val={0U,(void*)& Cyc_Absyn_FloatCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_DoubleType_val={0U,(void*)& Cyc_Absyn_DoubleCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_LongDoubleType_val={0U,(void*)& Cyc_Absyn_LongDoubleCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_EmptyEffType_val={0U,(void*)& Cyc_Absyn_JoinCon_val,0};
# 236
void*Cyc_Absyn_void_type=(void*)& Cyc_Absyn_VoidType_val;
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->tag=2U,_tmp1D->f1=x;_tmp1D;});}
# 239
void*Cyc_Absyn_uchar_type=(void*)& Cyc_Absyn_UCharType_val;
void*Cyc_Absyn_ushort_type=(void*)& Cyc_Absyn_UShortType_val;
void*Cyc_Absyn_uint_type=(void*)& Cyc_Absyn_UIntType_val;
void*Cyc_Absyn_ulong_type=(void*)& Cyc_Absyn_ULongType_val;
void*Cyc_Absyn_ulonglong_type=(void*)& Cyc_Absyn_ULongLongType_val;
# 245
void*Cyc_Absyn_schar_type=(void*)& Cyc_Absyn_SCharType_val;
void*Cyc_Absyn_sshort_type=(void*)& Cyc_Absyn_SShortType_val;
void*Cyc_Absyn_sint_type=(void*)& Cyc_Absyn_SIntType_val;
void*Cyc_Absyn_slong_type=(void*)& Cyc_Absyn_SLongType_val;
void*Cyc_Absyn_slonglong_type=(void*)& Cyc_Absyn_SLongLongType_val;
# 251
void*Cyc_Absyn_char_type=(void*)& Cyc_Absyn_NCharType_val;
void*Cyc_Absyn_nchar_type=(void*)& Cyc_Absyn_NCharType_val;
void*Cyc_Absyn_nshort_type=(void*)& Cyc_Absyn_NShortType_val;
void*Cyc_Absyn_nint_type=(void*)& Cyc_Absyn_NIntType_val;
void*Cyc_Absyn_nlong_type=(void*)& Cyc_Absyn_NLongType_val;
void*Cyc_Absyn_nlonglong_type=(void*)& Cyc_Absyn_NLongLongType_val;
# 258
void*Cyc_Absyn_float_type=(void*)& Cyc_Absyn_FloatType_val;
void*Cyc_Absyn_double_type=(void*)& Cyc_Absyn_DoubleType_val;
void*Cyc_Absyn_long_double_type=(void*)& Cyc_Absyn_LongDoubleType_val;
void*Cyc_Absyn_gen_float_type(unsigned int i){
unsigned int _tmp1E=i;switch(_tmp1E){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Int_pa_PrintArg_struct _tmp21=({struct Cyc_Int_pa_PrintArg_struct _tmp135;_tmp135.tag=1U,_tmp135.f1=(unsigned long)((int)i);_tmp135;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({struct _dyneither_ptr _tmp15C=({const char*_tmp20="gen_float_type(%d)";_tag_dyneither(_tmp20,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp15C,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});}_LL0:;}
# 270
void*Cyc_Absyn_heap_rgn_type=(void*)& Cyc_Absyn_HeapRgn_val;
void*Cyc_Absyn_unique_rgn_type=(void*)& Cyc_Absyn_UniqueRgn_val;
void*Cyc_Absyn_refcnt_rgn_type=(void*)& Cyc_Absyn_RefCntRgn_val;
# 274
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_EmptyEffType_val;
# 276
void*Cyc_Absyn_true_type=(void*)& Cyc_Absyn_TrueType_val;
void*Cyc_Absyn_false_type=(void*)& Cyc_Absyn_FalseType_val;
# 279
void*Cyc_Absyn_fat_bound_type=(void*)& Cyc_Absyn_FatType_val;
void*Cyc_Absyn_thin_bounds_type(void*t){
return({void*_tmp22[1U];_tmp22[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_dyneither(_tmp22,sizeof(void*),1U));});}
# 283
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 286
void*Cyc_Absyn_thin_bounds_int(unsigned int i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 291
void*Cyc_Absyn_bounds_one(){
static void*bone=0;
void*_tmp23=bone;
if(_tmp23 == 0){
_tmp23=Cyc_Absyn_thin_bounds_int(1U);
bone=_tmp23;}
# 298
return _tmp23;}
# 300
void*Cyc_Absyn_tag_type(void*t){
return({void*_tmp24[1U];_tmp24[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_dyneither(_tmp24,sizeof(void*),1U));});}
# 303
void*Cyc_Absyn_rgn_handle_type(void*r){
return({void*_tmp25[1U];_tmp25[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_dyneither(_tmp25,sizeof(void*),1U));});}
# 306
void*Cyc_Absyn_access_eff(void*r){
return({void*_tmp26[1U];_tmp26[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_AccessCon_val,_tag_dyneither(_tmp26,sizeof(void*),1U));});}
# 309
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=0U,_tmp27->f1=(void*)& Cyc_Absyn_JoinCon_val,_tmp27->f2=ts;_tmp27;});}
# 312
void*Cyc_Absyn_regionsof_eff(void*t){
return({void*_tmp28[1U];_tmp28[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_dyneither(_tmp28,sizeof(void*),1U));});}
# 315
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp15D=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=15U,_tmp29->f1=n,_tmp29->f2=d;_tmp29;});_tmp2A->f1=_tmp15D;}),_tmp2A->f2=0;_tmp2A;});}
# 318
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp15E=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=16U,_tmp2B->f1=fs;_tmp2B;});_tmp2C->f1=_tmp15E;}),_tmp2C->f2=0;_tmp2C;});}
# 321
void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp15F=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=17U,_tmp2D->f1=s,_tmp2D->f2=k;_tmp2D;});_tmp2E->f1=_tmp15F;}),_tmp2E->f2=0;_tmp2E;});}
# 324
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=0U,({void*_tmp160=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=18U,_tmp2F->f1=di;_tmp2F;});_tmp30->f1=_tmp160;}),_tmp30->f2=args;_tmp30;});}
# 327
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=0U,({void*_tmp161=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=19U,_tmp31->f1=di;_tmp31;});_tmp32->f1=_tmp161;}),_tmp32->f2=args;_tmp32;});}
# 330
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=0U,({void*_tmp162=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=20U,_tmp33->f1=ai;_tmp33;});_tmp34->f1=_tmp162;}),_tmp34->f2=args;_tmp34;});}
# 335
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp35=sn;switch(_tmp35){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 338
enum Cyc_Absyn_Size_of _tmp36=sz;switch(_tmp36){case Cyc_Absyn_Char_sz: _LL8: _LL9:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLA: _LLB:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLC: _LLD:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LLE: _LLF:
 return Cyc_Absyn_slong_type;default: _LL10: _LL11:
 return Cyc_Absyn_slonglong_type;}_LL7:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 346
enum Cyc_Absyn_Size_of _tmp37=sz;switch(_tmp37){case Cyc_Absyn_Char_sz: _LL13: _LL14:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL15: _LL16:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL17: _LL18:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
 return Cyc_Absyn_ulong_type;default: _LL1B: _LL1C:
 return Cyc_Absyn_ulonglong_type;}_LL12:;}default: _LL5: _LL6: {
# 354
enum Cyc_Absyn_Size_of _tmp38=sz;switch(_tmp38){case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL20: _LL21:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL22: _LL23:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL24: _LL25:
 return Cyc_Absyn_nlong_type;default: _LL26: _LL27:
 return Cyc_Absyn_nlonglong_type;}_LL1D:;}}_LL0:;}
# 366
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 369
void*Cyc_Absyn_wchar_type(){
int _tmp39= Sizeof_wchar_t;switch(_tmp39){case 1U: _LL1: _LL2:
# 380 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 384
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3A[4U]="exn";
# 388
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp3A,_tmp3A,_tmp3A + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp3B[15U]="Null_Exception";
# 400 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp3B,_tmp3B,_tmp3B + 15U};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp3C[13U]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp3C,_tmp3C,_tmp3C + 13U};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp3D[16U]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp3D,_tmp3D,_tmp3D + 16U};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp3E[10U]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp3E,_tmp3E,_tmp3E + 10U};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
# 406
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,& Cyc_Absyn_exn_l2};
# 411
static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)& Cyc_Absyn_exn_l3};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,& Cyc_Absyn_exn_ol,1};
# 414
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
# 416
static void*Cyc_Absyn_exn_type_tt(){
static void*eopt=0;
if(eopt == 0)
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp136;(_tmp136.KnownDatatype).tag=2U,(_tmp136.KnownDatatype).val=& Cyc_Absyn_exn_tud;_tmp136;}),0);
# 421
return eopt;}
# 424
void*Cyc_Absyn_exn_type(){
static void*exn_type_val=0;
if(exn_type_val == 0)
exn_type_val=({void*_tmp165=Cyc_Absyn_exn_type_tt();void*_tmp164=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp163=
Cyc_Absyn_empty_tqual(0U);
# 427
Cyc_Absyn_at_type(_tmp165,_tmp164,_tmp163,Cyc_Absyn_false_type);});
# 430
return exn_type_val;}static char _tmp3F[9U]="PrintArg";
# 434
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp3F,_tmp3F,_tmp3F + 9U};static char _tmp40[9U]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp40,_tmp40,_tmp40 + 9U};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 438
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 440
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp41[14U]="unique_region";
# 448
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp41,_tmp41,_tmp41 + 14U};static char _tmp42[5U]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp42,_tmp42,_tmp42 + 5U};
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)& Cyc_Absyn_Core_str,0};
# 452
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 454
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 456
static struct Cyc_List_List Cyc_Absyn_unique_list={(void*)& Cyc_Absyn_UniqueRgn_val,0};static struct Cyc_List_List Cyc_Absyn_heap_list={(void*)& Cyc_Absyn_UniqueRgn_val,0};static struct Cyc_List_List Cyc_Absyn_refcnt_list={(void*)& Cyc_Absyn_RefCntRgn_val,0};
# 461
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uniquergnhandle_type={0U,(void*)& Cyc_Absyn_RgnHandleCon_val,& Cyc_Absyn_unique_list};
# 464
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_uniquergnhandle_type,0,0,0,0};
# 467
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1U,& Cyc_Absyn_uniquergn_var_d};
# 469
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1U,(void*)& Cyc_Absyn_uniquergn_bind_p};
# 471
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergnhandle_type,(void*)& Cyc_Absyn_uniquergn_exp_r,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 474
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 477
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp169=Cyc_Absyn_char_type;void*_tmp168=rgn;struct Cyc_Absyn_Tqual _tmp167=Cyc_Absyn_empty_tqual(0U);void*_tmp166=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp169,_tmp168,_tmp167,_tmp166,Cyc_Absyn_true_type);});}
# 480
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp16D=Cyc_Absyn_char_type;void*_tmp16C=rgn;struct Cyc_Absyn_Tqual _tmp16B=Cyc_Absyn_const_tqual(0U);void*_tmp16A=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp16D,_tmp16C,_tmp16B,_tmp16A,Cyc_Absyn_true_type);});}
# 484
extern void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->tag=3U,_tmp43->f1=s;_tmp43;});}
# 489
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zeroterm){
# 491
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp137;_tmp137.elt_type=t,_tmp137.elt_tq=tq,
(_tmp137.ptr_atts).rgn=r,(_tmp137.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp137.ptr_atts).bounds=b,(_tmp137.ptr_atts).zero_term=zeroterm,(_tmp137.ptr_atts).ptrloc=0;_tmp137;}));}
# 497
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zeroterm){
# 499
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp138;_tmp138.elt_type=t,_tmp138.elt_tq=tq,
(_tmp138.ptr_atts).rgn=r,(_tmp138.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp138.ptr_atts).bounds=b,(_tmp138.ptr_atts).zero_term=zeroterm,(_tmp138.ptr_atts).ptrloc=0;_tmp138;}));}
# 506
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp139;_tmp139.elt_type=t,_tmp139.elt_tq=tq,
(_tmp139.ptr_atts).rgn=r,(_tmp139.ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp16E=Cyc_Absyn_bounds_one();(_tmp139.ptr_atts).bounds=_tmp16E;}),(_tmp139.ptr_atts).zero_term=zeroterm,(_tmp139.ptr_atts).ptrloc=0;_tmp139;}));}
# 513
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp13A;_tmp13A.elt_type=t,_tmp13A.elt_tq=tq,
(_tmp13A.ptr_atts).rgn=Cyc_Absyn_heap_rgn_type,(_tmp13A.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 517
void*_tmp16F=Cyc_Absyn_bounds_one();(_tmp13A.ptr_atts).bounds=_tmp16F;}),(_tmp13A.ptr_atts).zero_term=Cyc_Absyn_false_type,(_tmp13A.ptr_atts).ptrloc=0;_tmp13A;}));}
# 521
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp13B;_tmp13B.elt_type=t,_tmp13B.elt_tq=tq,
(_tmp13B.ptr_atts).rgn=r,(_tmp13B.ptr_atts).nullable=Cyc_Absyn_false_type,({
void*_tmp170=Cyc_Absyn_bounds_one();(_tmp13B.ptr_atts).bounds=_tmp170;}),(_tmp13B.ptr_atts).zero_term=zeroterm,(_tmp13B.ptr_atts).ptrloc=0;_tmp13B;}));}
# 528
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp13C;_tmp13C.elt_type=t,_tmp13C.elt_tq=tq,
(_tmp13C.ptr_atts).rgn=r,(_tmp13C.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp13C.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp13C.ptr_atts).zero_term=zeroterm,(_tmp13C.ptr_atts).ptrloc=0;_tmp13C;}));}
# 536
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc){
# 538
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=4U,(_tmp44->f1).elt_type=elt_type,(_tmp44->f1).tq=tq,(_tmp44->f1).num_elts=num_elts,(_tmp44->f1).zero_term=zero_term,(_tmp44->f1).zt_loc=ztloc;_tmp44;});}
# 541
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=11U,_tmp45->f1=e;_tmp45;});}
# 546
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 548
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->tag=8U,_tmp46->f1=n,_tmp46->f2=args,_tmp46->f3=d,_tmp46->f4=defn;_tmp46;});}
# 554
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=0U,({void*_tmp173=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=20U,({union Cyc_Absyn_AggrInfo _tmp172=({enum Cyc_Absyn_AggrKind _tmp171=k;Cyc_Absyn_UnknownAggr(_tmp171,({struct _tuple0*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=Cyc_Absyn_rel_ns_null,_tmp47->f2=name;_tmp47;}),0);});_tmp48->f1=_tmp172;});_tmp48;});_tmp49->f1=_tmp173;}),_tmp49->f2=0;_tmp49;});}
# 557
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 560
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 563
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 567
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp13D;(_tmp13D.Char_c).tag=2U,((_tmp13D.Char_c).val).f1=sn,((_tmp13D.Char_c).val).f2=c;_tmp13D;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp13E;(_tmp13E.Wchar_c).tag=3U,(_tmp13E.Wchar_c).val=s;_tmp13E;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp13F;(_tmp13F.Short_c).tag=4U,((_tmp13F.Short_c).val).f1=sn,((_tmp13F.Short_c).val).f2=s;_tmp13F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp140;(_tmp140.Int_c).tag=5U,((_tmp140.Int_c).val).f1=sn,((_tmp140.Int_c).val).f2=i;_tmp140;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp141;(_tmp141.LongLong_c).tag=6U,((_tmp141.LongLong_c).val).f1=sn,((_tmp141.LongLong_c).val).f2=l;_tmp141;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp142;(_tmp142.Float_c).tag=7U,((_tmp142.Float_c).val).f1=s,((_tmp142.Float_c).val).f2=i;_tmp142;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp143;(_tmp143.String_c).tag=8U,(_tmp143.String_c).val=s;_tmp143;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp144;(_tmp144.Wstring_c).tag=9U,(_tmp144.Wstring_c).val=s;_tmp144;});}
# 578
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->topt=0,_tmp4A->r=r,_tmp4A->loc=loc,_tmp4A->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp4A;});}
# 581
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp174=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=16U,_tmp4B->f1=rgn_handle,_tmp4B->f2=e;_tmp4B;});Cyc_Absyn_new_exp(_tmp174,loc);});}
# 584
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp4C=_cycalloc(sizeof(*_tmp4C));*_tmp4C=*e;_tmp4C;});}
# 587
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
return({void*_tmp175=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=0U,_tmp4D->f1=c;_tmp4D;});Cyc_Absyn_new_exp(_tmp175,loc);});}
# 590
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 594
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return({union Cyc_Absyn_Cnst _tmp176=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp176,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 602
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned int)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned int)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 609
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp177=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp177,loc);});}
# 615
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp178=Cyc_Absyn_Wchar_c(s);Cyc_Absyn_const_exp(_tmp178,loc);});}
# 618
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp179=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp179,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp17A=Cyc_Absyn_String_c(s);Cyc_Absyn_const_exp(_tmp17A,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp17B=Cyc_Absyn_Wstring_c(s);Cyc_Absyn_const_exp(_tmp17B,loc);});}
# 622
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
return({void*_tmp17D=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=1U,({void*_tmp17C=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=0U,_tmp4E->f1=q;_tmp4E;});_tmp4F->f1=_tmp17C;});_tmp4F;});Cyc_Absyn_new_exp(_tmp17D,loc);});}
# 625
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned int loc){
return({void*_tmp17E=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=1U,_tmp50->f1=b;_tmp50;});Cyc_Absyn_new_exp(_tmp17E,loc);});}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
return Cyc_Absyn_var_exp(q,loc);}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc){
return({void*_tmp17F=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=2U,_tmp51->f1=s;_tmp51;});Cyc_Absyn_new_exp(_tmp17F,loc);});}
# 635
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp180=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=3U,_tmp52->f1=p,_tmp52->f2=es;_tmp52;});Cyc_Absyn_new_exp(_tmp180,loc);});}
# 638
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp182=p;struct Cyc_List_List*_tmp181=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=e,_tmp53->tl=0;_tmp53;});Cyc_Absyn_primop_exp(_tmp182,_tmp181,loc);});}
# 641
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp185=p;struct Cyc_List_List*_tmp184=({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=e1,({struct Cyc_List_List*_tmp183=({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=e2,_tmp54->tl=0;_tmp54;});_tmp55->tl=_tmp183;});_tmp55;});Cyc_Absyn_primop_exp(_tmp185,_tmp184,loc);});}
# 644
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp186=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=35U,_tmp56->f1=e1,_tmp56->f2=e2;_tmp56;});Cyc_Absyn_new_exp(_tmp186,loc);});}
# 647
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 657
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp187=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=4U,_tmp57->f1=e1,_tmp57->f2=popt,_tmp57->f3=e2;_tmp57;});Cyc_Absyn_new_exp(_tmp187,loc);});}
# 660
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 663
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
return({void*_tmp188=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=5U,_tmp58->f1=e,_tmp58->f2=i;_tmp58;});Cyc_Absyn_new_exp(_tmp188,loc);});}
# 666
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
return({void*_tmp189=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=6U,_tmp59->f1=e1,_tmp59->f2=e2,_tmp59->f3=e3;_tmp59;});Cyc_Absyn_new_exp(_tmp189,loc);});}
# 673
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp18A=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=7U,_tmp5A->f1=e1,_tmp5A->f2=e2;_tmp5A;});Cyc_Absyn_new_exp(_tmp18A,loc);});}
# 676
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp18B=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=8U,_tmp5B->f1=e1,_tmp5B->f2=e2;_tmp5B;});Cyc_Absyn_new_exp(_tmp18B,loc);});}
# 679
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp18C=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=9U,_tmp5C->f1=e1,_tmp5C->f2=e2;_tmp5C;});Cyc_Absyn_new_exp(_tmp18C,loc);});}
# 682
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp18D=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=10U,_tmp5D->f1=e,_tmp5D->f2=es,_tmp5D->f3=0,_tmp5D->f4=0;_tmp5D;});Cyc_Absyn_new_exp(_tmp18D,loc);});}
# 685
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp18E=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=10U,_tmp5E->f1=e,_tmp5E->f2=es,_tmp5E->f3=0,_tmp5E->f4=1;_tmp5E;});Cyc_Absyn_new_exp(_tmp18E,loc);});}
# 688
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp18F=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=12U,_tmp5F->f1=e;_tmp5F;});Cyc_Absyn_new_exp(_tmp18F,loc);});}
# 691
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
return({void*_tmp190=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=13U,_tmp60->f1=e,_tmp60->f2=ts;_tmp60;});Cyc_Absyn_new_exp(_tmp190,loc);});}
# 694
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
return({void*_tmp191=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=14U,_tmp61->f1=t,_tmp61->f2=e,_tmp61->f3=user_cast,_tmp61->f4=c;_tmp61;});Cyc_Absyn_new_exp(_tmp191,loc);});}
# 697
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp192=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=11U,_tmp62->f1=e,_tmp62->f2=0;_tmp62;});Cyc_Absyn_new_exp(_tmp192,loc);});}
# 700
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp193=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=11U,_tmp63->f1=e,_tmp63->f2=1;_tmp63;});Cyc_Absyn_new_exp(_tmp193,loc);});}
# 703
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp194=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=15U,_tmp64->f1=e;_tmp64;});Cyc_Absyn_new_exp(_tmp194,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int loc){
return({void*_tmp195=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=17U,_tmp65->f1=t;_tmp65;});Cyc_Absyn_new_exp(_tmp195,loc);});}
# 707
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp196=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=18U,_tmp66->f1=e;_tmp66;});Cyc_Absyn_new_exp(_tmp196,loc);});}
# 710
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
return({void*_tmp197=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=19U,_tmp67->f1=t,_tmp67->f2=ofs;_tmp67;});Cyc_Absyn_new_exp(_tmp197,loc);});}
# 713
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp198=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=20U,_tmp68->f1=e;_tmp68;});Cyc_Absyn_new_exp(_tmp198,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp199=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=21U,_tmp69->f1=e,_tmp69->f2=n,_tmp69->f3=0,_tmp69->f4=0;_tmp69;});Cyc_Absyn_new_exp(_tmp199,loc);});}
# 717
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp19A=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=22U,_tmp6A->f1=e,_tmp6A->f2=n,_tmp6A->f3=0,_tmp6A->f4=0;_tmp6A;});Cyc_Absyn_new_exp(_tmp19A,loc);});}
# 720
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp19B=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=23U,_tmp6B->f1=e1,_tmp6B->f2=e2;_tmp6B;});Cyc_Absyn_new_exp(_tmp19B,loc);});}
# 723
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=24U,_tmp6C->f1=es;_tmp6C;});Cyc_Absyn_new_exp(_tmp19C,loc);});}
# 726
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){return({void*_tmp19D=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=37U,_tmp6D->f1=s;_tmp6D;});Cyc_Absyn_new_exp(_tmp19D,loc);});}
# 728
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 732
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
return({void*_tmp19E=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=39U,_tmp6E->f1=t;_tmp6E;});Cyc_Absyn_new_exp(_tmp19E,loc);});}
# 735
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
return({void*_tmp19F=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=40U,_tmp6F->f1=volatile_kw,_tmp6F->f2=body;_tmp6F;});Cyc_Absyn_new_exp(_tmp19F,loc);});}
# 738
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1A0=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=41U,_tmp70->f1=e;_tmp70;});Cyc_Absyn_new_exp(_tmp1A0,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 742
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 744
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));({struct _tuple13*_tmp1A1=({struct _tuple13*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=0,_tmp71->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp71;});_tmp72->hd=_tmp1A1;}),_tmp72->tl=dles;_tmp72;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return({void*_tmp1A2=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=26U,_tmp73->f1=dles;_tmp73;});Cyc_Absyn_new_exp(_tmp1A2,loc);});}
# 750
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 753
return({void*_tmp1A3=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=36U,_tmp74->f1=n,_tmp74->f2=dles;_tmp74;});Cyc_Absyn_new_exp(_tmp1A3,loc);});}
# 756
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
return({struct Cyc_Absyn_Stmt*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->r=s,_tmp75->loc=loc,_tmp75->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp75;});}
# 759
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp1A4=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=1U,_tmp76->f1=e;_tmp76;});Cyc_Absyn_new_stmt(_tmp1A4,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1A6=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1A5=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1A6,_tmp1A5,loc);});}}}
# 769
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=3U,_tmp77->f1=e;_tmp77;});Cyc_Absyn_new_stmt(_tmp1A7,loc);});}
# 772
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=4U,_tmp78->f1=e,_tmp78->f2=s1,_tmp78->f3=s2;_tmp78;});Cyc_Absyn_new_stmt(_tmp1A8,loc);});}
# 775
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=5U,(_tmp79->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1A9=Cyc_Absyn_skip_stmt(e->loc);(_tmp79->f1).f2=_tmp1A9;}),_tmp79->f2=s;_tmp79;});Cyc_Absyn_new_stmt(_tmp1AA,loc);});}
# 778
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1AD=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=9U,_tmp7A->f1=e1,(_tmp7A->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1AC=Cyc_Absyn_skip_stmt(e3->loc);(_tmp7A->f2).f2=_tmp1AC;}),
(_tmp7A->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1AB=Cyc_Absyn_skip_stmt(e3->loc);(_tmp7A->f3).f2=_tmp1AB;}),_tmp7A->f4=s;_tmp7A;});
# 781
Cyc_Absyn_new_stmt(_tmp1AD,loc);});}
# 785
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp1AE=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=10U,_tmp7B->f1=e,_tmp7B->f2=scs,_tmp7B->f3=0;_tmp7B;});Cyc_Absyn_new_stmt(_tmp1AE,loc);});}struct _tuple14{void*f1;void*f2;};
# 788
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmp7C=({struct _tuple14 _tmp145;_tmp145.f1=s1->r,_tmp145.f2=s2->r;_tmp145;});struct _tuple14 _tmp7D=_tmp7C;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7D.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7D.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1AF=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=2U,_tmp7E->f1=s1,_tmp7E->f2=s2;_tmp7E;});Cyc_Absyn_new_stmt(_tmp1AF,loc);});}}_LL0:;}
# 795
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
return({void*_tmp1B0=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=11U,_tmp7F->f1=el,_tmp7F->f2=0;_tmp7F;});Cyc_Absyn_new_stmt(_tmp1B0,loc);});}
# 798
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1B1=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=12U,_tmp80->f1=d,_tmp80->f2=s;_tmp80;});Cyc_Absyn_new_stmt(_tmp1B1,loc);});}
# 801
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1B3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp1B2=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp82->f1=_tmp1B2;});_tmp82;});Cyc_Absyn_new_decl(_tmp1B3,loc);});
return({void*_tmp1B4=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=12U,_tmp81->f1=d,_tmp81->f2=s;_tmp81;});Cyc_Absyn_new_stmt(_tmp1B4,loc);});}
# 805
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1B5=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=13U,_tmp83->f1=v,_tmp83->f2=s;_tmp83;});Cyc_Absyn_new_stmt(_tmp1B5,loc);});}
# 808
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1B7=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=14U,_tmp84->f1=s,(_tmp84->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1B6=Cyc_Absyn_skip_stmt(e->loc);(_tmp84->f2).f2=_tmp1B6;});_tmp84;});Cyc_Absyn_new_stmt(_tmp1B7,loc);});}
# 811
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp1B8=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=15U,_tmp85->f1=s,_tmp85->f2=scs,_tmp85->f3=0;_tmp85;});Cyc_Absyn_new_stmt(_tmp1B8,loc);});}
# 814
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
return({void*_tmp1B9=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=8U,_tmp86->f1=lab;_tmp86;});Cyc_Absyn_new_stmt(_tmp1B9,loc);});}
# 817
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp1BA=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1BA,loc);});}
# 821
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){return({struct Cyc_Absyn_Pat*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->r=p,_tmp87->topt=0,_tmp87->loc=s;_tmp87;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1BB=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=17U,_tmp88->f1=e;_tmp88;});Cyc_Absyn_new_pat(_tmp1BB,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 827
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){return({struct Cyc_Absyn_Decl*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->r=r,_tmp89->loc=loc;_tmp89;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1BC=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=2U,_tmp8A->f1=p,_tmp8A->f2=0,_tmp8A->f3=e,_tmp8A->f4=0;_tmp8A;});Cyc_Absyn_new_decl(_tmp1BC,loc);});}
# 831
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
return({void*_tmp1BD=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=3U,_tmp8B->f1=vds;_tmp8B;});Cyc_Absyn_new_decl(_tmp1BD,loc);});}
# 834
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned int loc){
return({void*_tmp1BE=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=4U,_tmp8C->f1=tv,_tmp8C->f2=vd,_tmp8C->f3=open_exp;_tmp8C;});Cyc_Absyn_new_decl(_tmp1BE,loc);});}
# 837
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 839
return({void*_tmp1C1=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=2U,({struct Cyc_Absyn_Pat*_tmp1C0=({void*_tmp1BF=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=2U,_tmp8D->f1=tv,_tmp8D->f2=vd;_tmp8D;});Cyc_Absyn_new_pat(_tmp1BF,loc);});_tmp8E->f1=_tmp1C0;}),_tmp8E->f2=0,_tmp8E->f3=e,_tmp8E->f4=0;_tmp8E;});Cyc_Absyn_new_decl(_tmp1C1,loc);});}
# 842
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->sc=Cyc_Absyn_Public,_tmp8F->name=x,_tmp8F->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1C2=Cyc_Absyn_empty_tqual(0U);_tmp8F->tq=_tmp1C2;}),_tmp8F->type=t,_tmp8F->initializer=init,_tmp8F->rgn=0,_tmp8F->attributes=0,_tmp8F->escapes=0;_tmp8F;});}
# 848
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->sc=Cyc_Absyn_Static,_tmp90->name=x,_tmp90->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1C3=Cyc_Absyn_empty_tqual(0U);_tmp90->tq=_tmp1C3;}),_tmp90->type=t,_tmp90->initializer=init,_tmp90->rgn=0,_tmp90->attributes=0,_tmp90->escapes=0;_tmp90;});}
# 853
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 857
return({struct Cyc_Absyn_AggrdeclImpl*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->exist_vars=exists,_tmp91->rgn_po=po,_tmp91->fields=fs,_tmp91->tagged=tagged;_tmp91;});}
# 860
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 863
return({void*_tmp1C5=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp1C4=({struct Cyc_Absyn_Aggrdecl*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->kind=k,_tmp92->sc=s,_tmp92->name=n,_tmp92->tvs=ts,_tmp92->impl=i,_tmp92->attributes=atts,_tmp92->expected_mem_kind=0;_tmp92;});_tmp93->f1=_tmp1C4;});_tmp93;});Cyc_Absyn_new_decl(_tmp1C5,loc);});}
# 868
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 871
return({struct Cyc_Absyn_TypeDecl*_tmp96=_cycalloc(sizeof(*_tmp96));({void*_tmp1C7=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp1C6=({struct Cyc_Absyn_Aggrdecl*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->kind=k,_tmp94->sc=s,_tmp94->name=n,_tmp94->tvs=ts,_tmp94->impl=i,_tmp94->attributes=atts,_tmp94->expected_mem_kind=0;_tmp94;});_tmp95->f1=_tmp1C6;});_tmp95;});_tmp96->r=_tmp1C7;}),_tmp96->loc=loc;_tmp96;});}
# 877
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 880
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 882
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 885
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 887
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 890
return({void*_tmp1C9=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp1C8=({struct Cyc_Absyn_Datatypedecl*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->sc=s,_tmp97->name=n,_tmp97->tvs=ts,_tmp97->fields=fs,_tmp97->is_extensible=is_extensible;_tmp97;});_tmp98->f1=_tmp1C8;});_tmp98;});Cyc_Absyn_new_decl(_tmp1C9,loc);});}
# 893
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 896
return({struct Cyc_Absyn_TypeDecl*_tmp9B=_cycalloc(sizeof(*_tmp9B));({void*_tmp1CB=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp1CA=({struct Cyc_Absyn_Datatypedecl*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->sc=s,_tmp99->name=n,_tmp99->tvs=ts,_tmp99->fields=fs,_tmp99->is_extensible=is_extensible;_tmp99;});_tmp9A->f1=_tmp1CA;});_tmp9A;});_tmp9B->r=_tmp1CB;}),_tmp9B->loc=loc;_tmp9B;});}
# 908 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp9C=(*a).f3;
void*_tmp9D=Cyc_Absyn_pointer_expand(_tmp9C,1);
if(_tmp9C != _tmp9D)
(*a).f3=_tmp9D;}
# 915
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 922
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=5U,(_tmp9E->f1).tvars=tvs,(_tmp9E->f1).ret_tqual=ret_tqual,({
# 925
void*_tmp1CC=Cyc_Absyn_pointer_expand(ret_type,0);(_tmp9E->f1).ret_type=_tmp1CC;}),(_tmp9E->f1).effect=eff_type,(_tmp9E->f1).args=args,(_tmp9E->f1).c_varargs=c_varargs,(_tmp9E->f1).cyc_varargs=cyc_varargs,(_tmp9E->f1).rgn_po=rgn_po,(_tmp9E->f1).attributes=atts,(_tmp9E->f1).requires_clause=req,(_tmp9E->f1).requires_relns=0,(_tmp9E->f1).ensures_clause=ens,(_tmp9E->f1).ensures_relns=0;_tmp9E;});}
# 939
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmp9F=Cyc_Tcutil_compress(t);void*_tmpA0=_tmp9F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->tag == 5U){_LL1: _LL2:
 return({void*_tmp1CF=t;void*_tmp1CE=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->v=& Cyc_Tcutil_rk;_tmpA1;}),0): Cyc_Absyn_heap_rgn_type;
# 941
struct Cyc_Absyn_Tqual _tmp1CD=
# 945
Cyc_Absyn_empty_tqual(0U);
# 941
Cyc_Absyn_at_type(_tmp1CF,_tmp1CE,_tmp1CD,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 946
 return t;}_LL0:;}
# 961 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpA2=e->r;void*_tmpA3=_tmpA2;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Vardecl*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA6;switch(*((int*)_tmpA3)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA3)->f1)){case 2U: _LL1: _LL2:
# 964
 return 0;case 1U: _LL3: _tmpA6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA3)->f1)->f1;_LL4:
 _tmpA7=_tmpA6;goto _LL6;case 4U: _LL5: _tmpA7=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA3)->f1)->f1;_LL6: {
# 967
void*_tmpA4=Cyc_Tcutil_compress(_tmpA7->type);void*_tmpA5=_tmpA4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA5)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 971
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpA8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA3)->f1;_LL10:
 return Cyc_Absyn_is_lvalue(_tmpA8);case 13U: _LL11: _tmpA9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA3)->f1;_LL12:
 return Cyc_Absyn_is_lvalue(_tmpA9);case 12U: _LL13: _tmpAA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA3)->f1;_LL14:
 return Cyc_Absyn_is_lvalue(_tmpAA);default: _LL15: _LL16:
 return 0;}_LL0:;}
# 982
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmpAB=fields;for(0;_tmpAB != 0;_tmpAB=_tmpAB->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmpAB->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmpAB->hd;}}
return 0;}
# 988
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 992
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 998
if(ts == 0)
return 0;
return(struct _tuple10*)ts->hd;}
# 1003
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={1U};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2U};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3U};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4U};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5U};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7U};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9U};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10U};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11U};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12U};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={13U};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={14U};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={15U};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={16U};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={17U};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={18U};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={23U};
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val={26U};
# 1022
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmpAC=a;struct _dyneither_ptr _tmpEF;struct _dyneither_ptr _tmpEE;int _tmpED;int _tmpEC;int _tmpEB;int _tmpEA;int _tmpE9;int _tmpE8;int _tmpE7;struct _dyneither_ptr _tmpE6;struct Cyc_Absyn_Exp*_tmpE5;int _tmpE4;switch(*((int*)_tmpAC)){case 0U: _LL1: _tmpE4=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpAF=({struct Cyc_Int_pa_PrintArg_struct _tmp146;_tmp146.tag=1U,_tmp146.f1=(unsigned long)_tmpE4;_tmp146;});void*_tmpAD[1U];_tmpAD[0]=& _tmpAF;({struct _dyneither_ptr _tmp1D0=({const char*_tmpAE="regparm(%d)";_tag_dyneither(_tmpAE,sizeof(char),12U);});Cyc_aprintf(_tmp1D0,_tag_dyneither(_tmpAD,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return({const char*_tmpB0="stdcall";_tag_dyneither(_tmpB0,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpB1="cdecl";_tag_dyneither(_tmpB1,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpB2="fastcall";_tag_dyneither(_tmpB2,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpB3="noreturn";_tag_dyneither(_tmpB3,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpB4="const";_tag_dyneither(_tmpB4,sizeof(char),6U);});case 6U: _LLD: _tmpE5=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LLE:
# 1031
 if(_tmpE5 == 0)return({const char*_tmpB5="aligned";_tag_dyneither(_tmpB5,sizeof(char),8U);});else{
# 1033
enum Cyc_Cyclone_C_Compilers _tmpB6=Cyc_Cyclone_c_compiler;if(_tmpB6 == Cyc_Cyclone_Gcc_c){_LL3A: _LL3B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB9=({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0U,({struct _dyneither_ptr _tmp1D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmpE5));_tmp147.f1=_tmp1D1;});_tmp147;});void*_tmpB7[1U];_tmpB7[0]=& _tmpB9;({struct _dyneither_ptr _tmp1D2=({const char*_tmpB8="aligned(%s)";_tag_dyneither(_tmpB8,sizeof(char),12U);});Cyc_aprintf(_tmp1D2,_tag_dyneither(_tmpB7,sizeof(void*),1U));});});}else{_LL3C: _LL3D:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpBC=({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0U,({struct _dyneither_ptr _tmp1D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmpE5));_tmp148.f1=_tmp1D3;});_tmp148;});void*_tmpBA[1U];_tmpBA[0]=& _tmpBC;({struct _dyneither_ptr _tmp1D4=({const char*_tmpBB="align(%s)";_tag_dyneither(_tmpBB,sizeof(char),10U);});Cyc_aprintf(_tmp1D4,_tag_dyneither(_tmpBA,sizeof(void*),1U));});});}_LL39:;}case 7U: _LLF: _LL10:
# 1037
 return({const char*_tmpBD="packed";_tag_dyneither(_tmpBD,sizeof(char),7U);});case 8U: _LL11: _tmpE6=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC0=({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0U,_tmp149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE6);_tmp149;});void*_tmpBE[1U];_tmpBE[0]=& _tmpC0;({struct _dyneither_ptr _tmp1D5=({const char*_tmpBF="section(\"%s\")";_tag_dyneither(_tmpBF,sizeof(char),14U);});Cyc_aprintf(_tmp1D5,_tag_dyneither(_tmpBE,sizeof(void*),1U));});});case 9U: _LL13: _LL14:
 return({const char*_tmpC1="nocommon";_tag_dyneither(_tmpC1,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpC2="shared";_tag_dyneither(_tmpC2,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpC3="unused";_tag_dyneither(_tmpC3,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpC4="weak";_tag_dyneither(_tmpC4,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpC5="dllimport";_tag_dyneither(_tmpC5,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpC6="dllexport";_tag_dyneither(_tmpC6,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpC7="no_instrument_function";_tag_dyneither(_tmpC7,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpC8="constructor";_tag_dyneither(_tmpC8,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpC9="destructor";_tag_dyneither(_tmpC9,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpCA="no_check_memory_usage";_tag_dyneither(_tmpCA,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpE8=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f2;_tmpE7=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f3;_LL28:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpCD=({struct Cyc_Int_pa_PrintArg_struct _tmp14B;_tmp14B.tag=1U,_tmp14B.f1=(unsigned int)_tmpE8;_tmp14B;});struct Cyc_Int_pa_PrintArg_struct _tmpCE=({struct Cyc_Int_pa_PrintArg_struct _tmp14A;_tmp14A.tag=1U,_tmp14A.f1=(unsigned int)_tmpE7;_tmp14A;});void*_tmpCB[2U];_tmpCB[0]=& _tmpCD,_tmpCB[1]=& _tmpCE;({struct _dyneither_ptr _tmp1D6=({const char*_tmpCC="format(printf,%u,%u)";_tag_dyneither(_tmpCC,sizeof(char),21U);});Cyc_aprintf(_tmp1D6,_tag_dyneither(_tmpCB,sizeof(void*),2U));});});}else{_LL29: _tmpEA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f2;_tmpE9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f3;_LL2A:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD1=({struct Cyc_Int_pa_PrintArg_struct _tmp14D;_tmp14D.tag=1U,_tmp14D.f1=(unsigned int)_tmpEA;_tmp14D;});struct Cyc_Int_pa_PrintArg_struct _tmpD2=({struct Cyc_Int_pa_PrintArg_struct _tmp14C;_tmp14C.tag=1U,_tmp14C.f1=(unsigned int)_tmpE9;_tmp14C;});void*_tmpCF[2U];_tmpCF[0]=& _tmpD1,_tmpCF[1]=& _tmpD2;({struct _dyneither_ptr _tmp1D7=({const char*_tmpD0="format(scanf,%u,%u)";_tag_dyneither(_tmpD0,sizeof(char),20U);});Cyc_aprintf(_tmp1D7,_tag_dyneither(_tmpCF,sizeof(void*),2U));});});}case 20U: _LL2B: _tmpEB=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL2C:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD5=({struct Cyc_Int_pa_PrintArg_struct _tmp14E;_tmp14E.tag=1U,_tmp14E.f1=(unsigned long)_tmpEB;_tmp14E;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({struct _dyneither_ptr _tmp1D8=({const char*_tmpD4="initializes(%d)";_tag_dyneither(_tmpD4,sizeof(char),16U);});Cyc_aprintf(_tmp1D8,_tag_dyneither(_tmpD3,sizeof(void*),1U));});});case 21U: _LL2D: _tmpEC=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL2E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD8=({struct Cyc_Int_pa_PrintArg_struct _tmp14F;_tmp14F.tag=1U,_tmp14F.f1=(unsigned long)_tmpEC;_tmp14F;});void*_tmpD6[1U];_tmpD6[0]=& _tmpD8;({struct _dyneither_ptr _tmp1D9=({const char*_tmpD7="noliveunique(%d)";_tag_dyneither(_tmpD7,sizeof(char),17U);});Cyc_aprintf(_tmp1D9,_tag_dyneither(_tmpD6,sizeof(void*),1U));});});case 22U: _LL2F: _tmpED=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL30:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDB=({struct Cyc_Int_pa_PrintArg_struct _tmp150;_tmp150.tag=1U,_tmp150.f1=(unsigned long)_tmpED;_tmp150;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({struct _dyneither_ptr _tmp1DA=({const char*_tmpDA="noconsume(%d)";_tag_dyneither(_tmpDA,sizeof(char),14U);});Cyc_aprintf(_tmp1DA,_tag_dyneither(_tmpD9,sizeof(void*),1U));});});case 23U: _LL31: _LL32:
 return({const char*_tmpDC="pure";_tag_dyneither(_tmpDC,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmpDD="always_inline";_tag_dyneither(_tmpDD,sizeof(char),14U);});case 24U: _LL35: _tmpEE=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL36:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEE);_tmp151;});void*_tmpDE[1U];_tmpDE[0]=& _tmpE0;({struct _dyneither_ptr _tmp1DB=({const char*_tmpDF="__mode__(\"%s\")";_tag_dyneither(_tmpDF,sizeof(char),15U);});Cyc_aprintf(_tmp1DB,_tag_dyneither(_tmpDE,sizeof(void*),1U));});});default: _LL37: _tmpEF=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL38:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE3=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0U,_tmp152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEF);_tmp152;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE3;({struct _dyneither_ptr _tmp1DC=({const char*_tmpE2="alias(\"%s\")";_tag_dyneither(_tmpE2,sizeof(char),12U);});Cyc_aprintf(_tmp1DC,_tag_dyneither(_tmpE1,sizeof(void*),1U));});});}_LL0:;}
# 1062
int Cyc_Absyn_fntype_att(void*a){
void*_tmpF0=a;int _tmpF1;switch(*((int*)_tmpF0)){case 0U: _LL1: _tmpF1=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpF0)->f1;_LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 1U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 23U: _LLB: _LLC:
 goto _LLE;case 19U: _LLD: _LLE:
 goto _LL10;case 5U: _LLF: _LL10:
 goto _LL12;case 21U: _LL11: _LL12:
 goto _LL14;case 20U: _LL13: _LL14:
 goto _LL16;case 22U: _LL15: _LL16:
 return 1;default: _LL17: _LL18:
 return 0;}_LL0:;}static char _tmpF2[3U]="f0";
# 1080
static struct _dyneither_ptr Cyc_Absyn_f0={_tmpF2,_tmpF2,_tmpF2 + 3U};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1U]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1U)};
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if((unsigned int)i >= fsz)
Cyc_Absyn_field_names=({unsigned int _tmpF8=(unsigned int)(i + 1);struct _dyneither_ptr**_tmpF7=_cycalloc(_check_times(_tmpF8,sizeof(struct _dyneither_ptr*)));({{unsigned int _tmp153=(unsigned int)(i + 1);unsigned int j;for(j=0;j < _tmp153;++ j){
# 1088
j < fsz?_tmpF7[j]=*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp1DF=({struct _dyneither_ptr*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct _dyneither_ptr _tmp1DE=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF5=({struct Cyc_Int_pa_PrintArg_struct _tmp154;_tmp154.tag=1U,_tmp154.f1=(unsigned long)((int)j);_tmp154;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({struct _dyneither_ptr _tmp1DD=({const char*_tmpF4="f%d";_tag_dyneither(_tmpF4,sizeof(char),4U);});Cyc_aprintf(_tmp1DD,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});*_tmpF6=_tmp1DE;});_tmpF6;});_tmpF7[j]=_tmp1DF;});}}0;});_tag_dyneither(_tmpF7,sizeof(struct _dyneither_ptr*),_tmpF8);});
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1092
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpF9=info;enum Cyc_Absyn_AggrKind _tmpFD;struct _tuple0*_tmpFC;enum Cyc_Absyn_AggrKind _tmpFB;struct _tuple0*_tmpFA;if((_tmpF9.UnknownAggr).tag == 1){_LL1: _tmpFB=((_tmpF9.UnknownAggr).val).f1;_tmpFA=((_tmpF9.UnknownAggr).val).f2;_LL2:
 return({struct _tuple11 _tmp155;_tmp155.f1=_tmpFB,_tmp155.f2=_tmpFA;_tmp155;});}else{_LL3: _tmpFD=(*(_tmpF9.KnownAggr).val)->kind;_tmpFC=(*(_tmpF9.KnownAggr).val)->name;_LL4:
 return({struct _tuple11 _tmp156;_tmp156.f1=_tmpFD,_tmp156.f2=_tmpFC;_tmp156;});}_LL0:;}
# 1098
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpFE=info;struct Cyc_Absyn_Aggrdecl*_tmp101;if((_tmpFE.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmpFF=0U;({struct _dyneither_ptr _tmp1E0=({const char*_tmp100="unchecked aggrdecl";_tag_dyneither(_tmp100,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp1E0,_tag_dyneither(_tmpFF,sizeof(void*),0U));});});}else{_LL3: _tmp101=*(_tmpFE.KnownAggr).val;_LL4:
 return _tmp101;}_LL0:;}
# 1104
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp102=Cyc_Tcutil_compress(t);void*_tmp103=_tmp102;union Cyc_Absyn_AggrInfo _tmp10C;struct Cyc_List_List*_tmp10B;switch(*((int*)_tmp103)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp103)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp10B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp103)->f2;_LL2:
# 1107
 if(_tmp10B == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp10B->hd)->requires_clause == 0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp103)->f1)->tag == 20U){_LL3: _tmp10C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp103)->f1)->f1;_LL4: {
# 1110
union Cyc_Absyn_AggrInfo _tmp104=_tmp10C;enum Cyc_Absyn_AggrKind _tmp10A;int _tmp109;enum Cyc_Absyn_AggrKind _tmp108;struct Cyc_Absyn_Aggrdecl*_tmp107;if((_tmp104.KnownAggr).tag == 2){_LL8: _tmp107=*(_tmp104.KnownAggr).val;_LL9:
# 1112
 if((int)_tmp107->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp105=_tmp107->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp105))->tagged)return 0;{
struct Cyc_List_List*_tmp106=_tmp105->fields;
if(_tmp106 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp106->hd)->requires_clause == 0;};};}else{if(((_tmp104.UnknownAggr).val).f3 == 0){_LLA: _tmp108=((_tmp104.UnknownAggr).val).f1;_LLB:
 return(int)_tmp108 == (int)Cyc_Absyn_UnionA;}else{_LLC: _tmp10A=((_tmp104.UnknownAggr).val).f1;_tmp109=(int)(((_tmp104.UnknownAggr).val).f3)->v;_LLD:
 return(int)_tmp10A == (int)Cyc_Absyn_UnionA  && !_tmp109;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1121
 return 0;}_LL0:;}
# 1124
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp10D=Cyc_Tcutil_compress(t);void*_tmp10E=_tmp10D;union Cyc_Absyn_AggrInfo _tmp117;struct Cyc_List_List*_tmp116;switch(*((int*)_tmp10E)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10E)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp116=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10E)->f2;_LL2:
# 1127
 if(_tmp116 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp116->hd)->requires_clause != 0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->tag == 20U){_LL3: _tmp117=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1;_LL4: {
# 1130
union Cyc_Absyn_AggrInfo _tmp10F=_tmp117;enum Cyc_Absyn_AggrKind _tmp115;int _tmp114;enum Cyc_Absyn_AggrKind _tmp113;struct Cyc_Absyn_Aggrdecl*_tmp112;if((_tmp10F.KnownAggr).tag == 2){_LL8: _tmp112=*(_tmp10F.KnownAggr).val;_LL9:
# 1132
 if((int)_tmp112->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp110=_tmp112->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp110))->tagged)return 0;{
struct Cyc_List_List*_tmp111=_tmp110->fields;
if(_tmp111 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp111->hd)->requires_clause != 0;};};}else{if(((_tmp10F.UnknownAggr).val).f3 == 0){_LLA: _tmp113=((_tmp10F.UnknownAggr).val).f1;_LLB:
 return 0;}else{_LLC: _tmp115=((_tmp10F.UnknownAggr).val).f1;_tmp114=(int)(((_tmp10F.UnknownAggr).val).f3)->v;_LLD:
 return 0;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1141
 return 0;}_LL0:;}
# 1145
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp118=b;struct Cyc_Absyn_Fndecl*_tmp11E;struct Cyc_Absyn_Vardecl*_tmp11D;struct Cyc_Absyn_Vardecl*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11B;struct Cyc_Absyn_Vardecl*_tmp11A;struct _tuple0*_tmp119;switch(*((int*)_tmp118)){case 0U: _LL1: _tmp119=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp118)->f1;_LL2:
 return _tmp119;case 1U: _LL3: _tmp11A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp118)->f1;_LL4:
 _tmp11B=_tmp11A;goto _LL6;case 3U: _LL5: _tmp11B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp118)->f1;_LL6:
 _tmp11C=_tmp11B;goto _LL8;case 4U: _LL7: _tmp11C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp118)->f1;_LL8:
 _tmp11D=_tmp11C;goto _LLA;case 5U: _LL9: _tmp11D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp118)->f1;_LLA:
 return _tmp11D->name;default: _LLB: _tmp11E=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp118)->f1;_LLC:
 return _tmp11E->name;}_LL0:;}
# 1156
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0  || ds->tl != 0)
({void*_tmp11F=0U;({struct _dyneither_ptr _tmp1E1=({const char*_tmp120="designator list not of length 1";_tag_dyneither(_tmp120,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp1E1,_tag_dyneither(_tmp11F,sizeof(void*),0U));});});{
void*_tmp121=(void*)ds->hd;void*_tmp122=_tmp121;struct _dyneither_ptr*_tmp125;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp122)->tag == 1U){_LL1: _tmp125=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp122)->f1;_LL2:
 return _tmp125;}else{_LL3: _LL4:
({void*_tmp123=0U;({struct _dyneither_ptr _tmp1E2=({const char*_tmp124="array designator in struct";_tag_dyneither(_tmp124,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp1E2,_tag_dyneither(_tmp123,sizeof(void*),0U));});});}_LL0:;};}
# 1165
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp126=Cyc_Tcutil_compress(t);void*_tmp127=_tmp126;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp127)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp127)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1173
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
# 1178
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;

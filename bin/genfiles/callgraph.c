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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 888 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100 "cycboot.h"
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 46 "graph.h"
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*));
# 50
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*));
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*s);
# 59
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
void Cyc_Callgraph_print_callgraph(struct Cyc___cycFILE*,struct Cyc_Dict_Dict);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 74
int Cyc_Hashtable_hash_string(struct _dyneither_ptr s);
# 47 "callgraph.cyc"
static struct Cyc_Set_Set*Cyc_Callgraph_cg_stmt(struct Cyc_Hashtable_Table*fds,struct Cyc_Absyn_Stmt*,struct Cyc_Set_Set*s);
static struct Cyc_Set_Set*Cyc_Callgraph_cg_exp(struct Cyc_Hashtable_Table*fds,struct Cyc_Absyn_Exp*,struct Cyc_Set_Set*s);
# 50
static struct Cyc_Set_Set*Cyc_Callgraph_cg_exps(struct Cyc_Hashtable_Table*fds,struct Cyc_List_List*es,struct Cyc_Set_Set*s){
for(0;es != 0;es=es->tl){
s=Cyc_Callgraph_cg_exp(fds,(struct Cyc_Absyn_Exp*)es->hd,s);}
return s;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 56
static struct Cyc_Set_Set*Cyc_Callgraph_cg_exp(struct Cyc_Hashtable_Table*fds,struct Cyc_Absyn_Exp*e,struct Cyc_Set_Set*s){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_List_List*_tmp3B;struct Cyc_List_List*_tmp3A;struct Cyc_List_List*_tmp39;struct Cyc_List_List*_tmp38;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp35;struct Cyc_Absyn_Exp*_tmp34;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_Absyn_MallocInfo _tmp32;struct Cyc_Absyn_Exp*_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_List_List*_tmp13;struct Cyc_Absyn_Vardecl*_tmp12;struct Cyc_Absyn_Fndecl*_tmp11;switch(*((int*)_tmp1)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1)){case 2U: _LL1: _tmp11=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1)->f1;_LL2: {
# 59
struct _handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))_tmp4=1;if(!_tmp4){
# 62
{struct Cyc_Set_Set*_tmp5=({struct Cyc_Set_Set*_tmp70=s;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Absyn_Fndecl*elt))Cyc_Set_insert)(_tmp70,((struct Cyc_Absyn_Fndecl*(*)(struct Cyc_Hashtable_Table*t,struct _tuple0*key))Cyc_Hashtable_lookup)(fds,_tmp11->name));});_npop_handler(0U);return _tmp5;};_pop_handler();}else{void*_tmp3=(void*)Cyc_Core_get_exn_thrown();void*_tmp6=_tmp3;void*_tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp6)->tag == Cyc_Core_Not_found){_LL5A: _LL5B:
# 64
 return s;}else{_LL5C: _tmp7=_tmp6;_LL5D:(int)_rethrow(_tmp7);}_LL59:;}};}case 1U: _LL3: _tmp12=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1)->f1;_LL4: {
# 67
struct _handler_cons _tmp8;_push_handler(& _tmp8);{int _tmpA=0;if(setjmp(_tmp8.handler))_tmpA=1;if(!_tmpA){
{struct Cyc_Set_Set*_tmpB=({struct Cyc_Set_Set*_tmp71=s;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Absyn_Fndecl*elt))Cyc_Set_insert)(_tmp71,((struct Cyc_Absyn_Fndecl*(*)(struct Cyc_Hashtable_Table*t,struct _tuple0*key))Cyc_Hashtable_lookup)(fds,_tmp12->name));});_npop_handler(0U);return _tmpB;};_pop_handler();}else{void*_tmp9=(void*)Cyc_Core_get_exn_thrown();void*_tmpC=_tmp9;void*_tmpD;if(((struct Cyc_Core_Not_found_exn_struct*)_tmpC)->tag == Cyc_Core_Not_found){_LL5F: _LL60:
# 70
 return s;}else{_LL61: _tmpD=_tmpC;_LL62:(int)_rethrow(_tmpD);}_LL5E:;}};}default: _LL5: _LL6:
# 72
 goto _LL8;}case 0U: _LL7: _LL8: goto _LLA;case 2U: _LL9: _LLA: goto _LLC;case 17U: _LLB: _LLC:
 goto _LLE;case 18U: _LLD: _LLE: goto _LL10;case 19U: _LLF: _LL10: goto _LL12;case 32U: _LL11: _LL12:
 goto _LL14;case 33U: _LL13: _LL14: goto _LL16;case 39U: _LL15: _LL16: goto _LL18;case 40U: _LL17: _LL18:
 goto _LL1A;case 41U: _LL19: _LL1A: return s;case 3U: _LL1B: _tmp13=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1C:
 return Cyc_Callgraph_cg_exps(fds,_tmp13,s);case 7U: _LL1D: _tmp15=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp14=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1E:
 _tmp17=_tmp15;_tmp16=_tmp14;goto _LL20;case 8U: _LL1F: _tmp17=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp16=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL20:
 _tmp19=_tmp17;_tmp18=_tmp16;goto _LL22;case 9U: _LL21: _tmp19=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp18=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL22:
 _tmp1B=_tmp19;_tmp1A=_tmp18;goto _LL24;case 35U: _LL23: _tmp1B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp1A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL24:
 _tmp1D=_tmp1B;_tmp1C=_tmp1A;goto _LL26;case 23U: _LL25: _tmp1D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp1C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL26:
 _tmp1F=_tmp1D;_tmp1E=_tmp1C;goto _LL28;case 27U: _LL27: _tmp1F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp1E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL28:
 _tmp21=_tmp1F;_tmp20=_tmp1E;goto _LL2A;case 4U: _LL29: _tmp21=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp20=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL2A:
 return({struct Cyc_Hashtable_Table*_tmp73=fds;struct Cyc_Absyn_Exp*_tmp72=_tmp20;Cyc_Callgraph_cg_exp(_tmp73,_tmp72,Cyc_Callgraph_cg_exp(fds,_tmp21,s));});case 11U: _LL2B: _tmp22=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2C:
 _tmp23=_tmp22;goto _LL2E;case 12U: _LL2D: _tmp23=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2E:
 _tmp24=_tmp23;goto _LL30;case 13U: _LL2F: _tmp24=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL30:
 _tmp25=_tmp24;goto _LL32;case 14U: _LL31: _tmp25=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL32:
 _tmp26=_tmp25;goto _LL34;case 15U: _LL33: _tmp26=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL34:
 _tmp27=_tmp26;goto _LL36;case 20U: _LL35: _tmp27=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL36:
 _tmp28=_tmp27;goto _LL38;case 21U: _LL37: _tmp28=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL38:
 _tmp29=_tmp28;goto _LL3A;case 22U: _LL39: _tmp29=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL3A:
 _tmp2A=_tmp29;goto _LL3C;case 38U: _LL3B: _tmp2A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL3C:
 _tmp2B=_tmp2A;goto _LL3E;case 28U: _LL3D: _tmp2B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL3E:
 _tmp2C=_tmp2B;goto _LL40;case 5U: _LL3F: _tmp2C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL40:
 return Cyc_Callgraph_cg_exp(fds,_tmp2C,s);case 6U: _LL41: _tmp2F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp2E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp2D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL42:
# 96
 return({struct Cyc_Hashtable_Table*_tmp77=fds;struct Cyc_Absyn_Exp*_tmp76=_tmp2D;Cyc_Callgraph_cg_exp(_tmp77,_tmp76,({struct Cyc_Hashtable_Table*_tmp75=fds;struct Cyc_Absyn_Exp*_tmp74=_tmp2E;Cyc_Callgraph_cg_exp(_tmp75,_tmp74,Cyc_Callgraph_cg_exp(fds,_tmp2F,s));}));});case 10U: _LL43: _tmp31=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp30=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL44:
 return({struct Cyc_Hashtable_Table*_tmp79=fds;struct Cyc_List_List*_tmp78=_tmp30;Cyc_Callgraph_cg_exps(_tmp79,_tmp78,Cyc_Callgraph_cg_exp(fds,_tmp31,s));});case 34U: _LL45: _tmp32=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL46:
 _tmp34=_tmp32.rgn;_tmp33=_tmp32.num_elts;goto _LL48;case 16U: _LL47: _tmp34=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp33=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL48:
# 100
 return({struct Cyc_Hashtable_Table*_tmp7B=fds;struct Cyc_Absyn_Exp*_tmp7A=_tmp33;Cyc_Callgraph_cg_exp(_tmp7B,_tmp7A,_tmp34 == 0?s: Cyc_Callgraph_cg_exp(fds,_tmp34,s));});case 31U: _LL49: _tmp35=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4A:
 _tmp36=_tmp35;goto _LL4C;case 24U: _LL4B: _tmp36=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4C:
 return Cyc_Callgraph_cg_exps(fds,_tmp36,s);case 36U: _LL4D: _tmp37=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL4E:
 _tmp38=_tmp37;goto _LL50;case 25U: _LL4F: _tmp38=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL50:
 _tmp39=_tmp38;goto _LL52;case 29U: _LL51: _tmp39=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL52:
 _tmp3A=_tmp39;goto _LL54;case 30U: _LL53: _tmp3A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL54:
 _tmp3B=_tmp3A;goto _LL56;case 26U: _LL55: _tmp3B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL56:
# 108
 for(0;_tmp3B != 0;_tmp3B=_tmp3B->tl){
struct _tuple10*_tmpE=(struct _tuple10*)_tmp3B->hd;struct _tuple10*_tmpF=_tmpE;struct Cyc_Absyn_Exp*_tmp10;_LL64: _tmp10=_tmpF->f2;_LL65:;
s=Cyc_Callgraph_cg_exp(fds,_tmp10,s);}
# 112
return s;default: _LL57: _tmp3C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL58:
 return Cyc_Callgraph_cg_stmt(fds,_tmp3C,s);}_LL0:;}
# 117
static struct Cyc_Set_Set*Cyc_Callgraph_cg_decl(struct Cyc_Hashtable_Table*fds,struct Cyc_Absyn_Decl*d,struct Cyc_Set_Set*s){
void*_tmp3D=d->r;void*_tmp3E=_tmp3D;struct Cyc_Absyn_Vardecl*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Vardecl*_tmp41;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp41=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3E)->f1;_LL2: {
# 120
struct Cyc_Absyn_Exp*_tmp3F=_tmp41->initializer;
return _tmp3F == 0?s: Cyc_Callgraph_cg_exp(fds,_tmp3F,s);}case 2U: _LL3: _tmp42=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp3E)->f3;_LL4:
 return Cyc_Callgraph_cg_exp(fds,_tmp42,s);case 3U: _LL5: _tmp43=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp3E)->f1;_LL6:
# 124
 for(0;_tmp43 != 0;_tmp43=_tmp43->tl){
struct Cyc_Absyn_Exp*_tmp40=((struct Cyc_Absyn_Vardecl*)_tmp43->hd)->initializer;
s=_tmp40 == 0?s: Cyc_Callgraph_cg_exp(fds,_tmp40,s);}
# 128
return s;case 4U: _LL7: _tmp45=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp3E)->f2;_tmp44=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp3E)->f3;_LL8:
# 130
 s=_tmp45->initializer == 0?s: Cyc_Callgraph_cg_exp(fds,(struct Cyc_Absyn_Exp*)_check_null(_tmp45->initializer),s);
return _tmp44 == 0?s: Cyc_Callgraph_cg_exp(fds,_tmp44,s);default: _LL9: _LLA:
 return s;}_LL0:;}
# 136
static struct Cyc_Set_Set*Cyc_Callgraph_cg_stmt(struct Cyc_Hashtable_Table*fds,struct Cyc_Absyn_Stmt*stmt,struct Cyc_Set_Set*s){
void*_tmp46=stmt->r;void*_tmp47=_tmp46;struct Cyc_Absyn_Stmt*_tmp5E;struct Cyc_Absyn_Stmt*_tmp5D;struct Cyc_List_List*_tmp5C;struct Cyc_Absyn_Stmt*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Decl*_tmp59;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_List_List*_tmp57;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_List_List*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Stmt*_tmp4F;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Stmt*_tmp4D;struct Cyc_Absyn_Stmt*_tmp4C;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Stmt*_tmp49;struct Cyc_Absyn_Exp*_tmp48;switch(*((int*)_tmp47)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 8U: _LL5: _LL6:
 goto _LL8;case 0U: _LL7: _LL8:
 return s;case 1U: _LL9: _tmp48=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LLA:
 return Cyc_Callgraph_cg_exp(fds,_tmp48,s);case 2U: _LLB: _tmp4A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp49=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LLC:
 return({struct Cyc_Hashtable_Table*_tmp7D=fds;struct Cyc_Absyn_Stmt*_tmp7C=_tmp49;Cyc_Callgraph_cg_stmt(_tmp7D,_tmp7C,Cyc_Callgraph_cg_stmt(fds,_tmp4A,s));});case 3U: _LLD: _tmp4B=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LLE:
 return _tmp4B != 0?Cyc_Callgraph_cg_exp(fds,_tmp4B,s): s;case 4U: _LLF: _tmp4E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp4D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_tmp4C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f3;_LL10:
# 146
 return({struct Cyc_Hashtable_Table*_tmp81=fds;struct Cyc_Absyn_Stmt*_tmp80=_tmp4C;Cyc_Callgraph_cg_stmt(_tmp81,_tmp80,({struct Cyc_Hashtable_Table*_tmp7F=fds;struct Cyc_Absyn_Stmt*_tmp7E=_tmp4D;Cyc_Callgraph_cg_stmt(_tmp7F,_tmp7E,Cyc_Callgraph_cg_exp(fds,_tmp4E,s));}));});case 5U: _LL11: _tmp50=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f1).f1;_tmp4F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL12:
 return({struct Cyc_Hashtable_Table*_tmp83=fds;struct Cyc_Absyn_Stmt*_tmp82=_tmp4F;Cyc_Callgraph_cg_stmt(_tmp83,_tmp82,Cyc_Callgraph_cg_exp(fds,_tmp50,s));});case 9U: _LL13: _tmp54=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp53=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f1;_tmp52=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f3).f1;_tmp51=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f4;_LL14:
# 149
 return({struct Cyc_Hashtable_Table*_tmp89=fds;struct Cyc_Absyn_Stmt*_tmp88=_tmp51;Cyc_Callgraph_cg_stmt(_tmp89,_tmp88,({struct Cyc_Hashtable_Table*_tmp87=fds;struct Cyc_Absyn_Exp*_tmp86=_tmp52;Cyc_Callgraph_cg_exp(_tmp87,_tmp86,({struct Cyc_Hashtable_Table*_tmp85=fds;struct Cyc_Absyn_Exp*_tmp84=_tmp53;Cyc_Callgraph_cg_exp(_tmp85,_tmp84,Cyc_Callgraph_cg_exp(fds,_tmp54,s));}));}));});case 10U: _LL15: _tmp56=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp55=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL16:
# 151
 s=Cyc_Callgraph_cg_exp(fds,_tmp56,s);
for(0;_tmp55 != 0;_tmp55=_tmp55->tl){
# 154
s=Cyc_Callgraph_cg_stmt(fds,((struct Cyc_Absyn_Switch_clause*)_tmp55->hd)->body,s);}
return s;case 11U: _LL17: _tmp57=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LL18:
 return Cyc_Callgraph_cg_exps(fds,_tmp57,s);case 12U: _LL19: _tmp59=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp58=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL1A:
 return({struct Cyc_Hashtable_Table*_tmp8B=fds;struct Cyc_Absyn_Stmt*_tmp8A=_tmp58;Cyc_Callgraph_cg_stmt(_tmp8B,_tmp8A,Cyc_Callgraph_cg_decl(fds,_tmp59,s));});case 14U: _LL1B: _tmp5B=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp5A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f1;_LL1C:
 return({struct Cyc_Hashtable_Table*_tmp8D=fds;struct Cyc_Absyn_Exp*_tmp8C=_tmp5A;Cyc_Callgraph_cg_exp(_tmp8D,_tmp8C,Cyc_Callgraph_cg_stmt(fds,_tmp5B,s));});case 15U: _LL1D: _tmp5D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp5C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL1E:
# 160
 s=Cyc_Callgraph_cg_stmt(fds,_tmp5D,s);
for(0;_tmp5C != 0;_tmp5C=_tmp5C->tl){
# 163
s=Cyc_Callgraph_cg_stmt(fds,((struct Cyc_Absyn_Switch_clause*)_tmp5C->hd)->body,s);}
return s;default: _LL1F: _tmp5E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL20:
 return Cyc_Callgraph_cg_stmt(fds,_tmp5E,s);}_LL0:;}
# 169
static int Cyc_Callgraph_fndecl_cmp(struct Cyc_Absyn_Fndecl*fd1,struct Cyc_Absyn_Fndecl*fd2){return(int)fd1 - (int)fd2;}
# 171
static struct Cyc_Dict_Dict Cyc_Callgraph_cg_topdecls(struct Cyc_Hashtable_Table*fds,struct Cyc_Dict_Dict cg,struct Cyc_List_List*ds){
struct Cyc_Set_Set*_tmp5F=((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*)))Cyc_Set_empty)(Cyc_Callgraph_fndecl_cmp);
{struct Cyc_List_List*_tmp60=ds;for(0;_tmp60 != 0;_tmp60=_tmp60->tl){
void*_tmp61=((struct Cyc_Absyn_Decl*)_tmp60->hd)->r;void*_tmp62=_tmp61;struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Fndecl*_tmp63;switch(*((int*)_tmp62)){case 1U: _LL1: _tmp63=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp62)->f1;_LL2:
# 176
 cg=({struct Cyc_Dict_Dict _tmp8F=cg;struct Cyc_Absyn_Fndecl*_tmp8E=_tmp63;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g,struct Cyc_Absyn_Fndecl*s,struct Cyc_Set_Set*T))Cyc_Graph_add_edges)(_tmp8F,_tmp8E,Cyc_Callgraph_cg_stmt(fds,_tmp63->body,_tmp5F));});
goto _LL0;case 10U: _LL3: _tmp64=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp62)->f2;_LL4:
 _tmp65=_tmp64;goto _LL6;case 9U: _LL5: _tmp65=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp62)->f2;_LL6:
 cg=Cyc_Callgraph_cg_topdecls(fds,cg,_tmp65);goto _LL0;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 183
return cg;}
# 187
static struct Cyc_Dict_Dict Cyc_Callgraph_enter_fndecls(struct Cyc_Hashtable_Table*fds,struct Cyc_Dict_Dict cg,struct Cyc_List_List*ds){
# 189
{struct Cyc_List_List*_tmp66=ds;for(0;_tmp66 != 0;_tmp66=_tmp66->tl){
void*_tmp67=((struct Cyc_Absyn_Decl*)_tmp66->hd)->r;void*_tmp68=_tmp67;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6A;struct Cyc_Absyn_Fndecl*_tmp69;switch(*((int*)_tmp68)){case 1U: _LL1: _tmp69=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp68)->f1;_LL2:
# 192
((void(*)(struct Cyc_Hashtable_Table*t,struct _tuple0*key,struct Cyc_Absyn_Fndecl*val))Cyc_Hashtable_insert)(fds,_tmp69->name,_tmp69);
cg=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g,struct Cyc_Absyn_Fndecl*s))Cyc_Graph_add_node)(cg,_tmp69);
goto _LL0;case 10U: _LL3: _tmp6A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp68)->f2;_LL4:
 _tmp6B=_tmp6A;goto _LL6;case 9U: _LL5: _tmp6B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp68)->f2;_LL6:
# 197
 cg=Cyc_Callgraph_enter_fndecls(fds,cg,_tmp6B);goto _LL0;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 201
return cg;}
# 204
static int Cyc_Callgraph_hash_qvar(struct _tuple0*q){return Cyc_Hashtable_hash_string(*(*q).f2);}
static int Cyc_Callgraph_hash_fndecl(struct Cyc_Absyn_Fndecl*fd){return(int)fd;}
# 207
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*ds){
# 209
struct Cyc_Hashtable_Table*fd=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tuple0*,struct _tuple0*),int(*hash)(struct _tuple0*)))Cyc_Hashtable_create)(51,Cyc_Absyn_qvar_cmp,Cyc_Callgraph_hash_qvar);
struct Cyc_Dict_Dict cg=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*)))Cyc_Graph_empty)(Cyc_Callgraph_fndecl_cmp);
cg=Cyc_Callgraph_enter_fndecls(fd,cg,ds);
# 213
cg=Cyc_Callgraph_cg_topdecls(fd,cg,ds);
return cg;}
# 217
static void Cyc_Callgraph_print_fndecl(struct Cyc___cycFILE*f,struct Cyc_Absyn_Fndecl*fd){
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0U,({struct _dyneither_ptr _tmp90=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(fd->name));_tmp6F.f1=_tmp90;});_tmp6F;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmp92=f;struct _dyneither_ptr _tmp91=({const char*_tmp6D="%s ";_tag_dyneither(_tmp6D,sizeof(char),4U);});Cyc_fprintf(_tmp92,_tmp91,_tag_dyneither(_tmp6C,sizeof(void*),1U));});});}
# 221
void Cyc_Callgraph_print_callgraph(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict cg){
((void(*)(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,struct Cyc_Absyn_Fndecl*)))Cyc_Graph_print)(f,cg,Cyc_Callgraph_print_fndecl);}

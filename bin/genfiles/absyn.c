#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
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

// pushes a frame on the stack
void _push_frame(struct _RuntimeStack *frame);

// pop N+1 frames from the stack (error if stack_size < N+1)
void _npop_frame(unsigned int n);

// returns top frame on the stack (NULL if stack is empty)
struct _RuntimeStack * _top_frame();

// pops off frames until a frame with the given tag is reached.  This
// frame is returned, or else NULL if none found.
struct _RuntimeStack * _pop_frame_until(int tag);

/***********************************************************************/
/* Low-level representations etc.                                      */
/***********************************************************************/

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

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
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

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
extern void   _reset_region(struct _RegionHandle *);
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
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
extern int _rethrow(void* e);
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
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
_check_null_fn(const void *ptr, const char *filename, unsigned lineno) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null_fn(filename,lineno);
  return _check_null_temp;
}
#define _check_null(p) (_check_null_fn((p),__FILE__,__LINE__))
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE char *
_check_known_subscript_null_fn(void *ptr, unsigned bound, unsigned elt_sz, unsigned index, const char *filename, unsigned lineno) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null_fn(filename,lineno);
  if (_cks_index >= _cks_bound) _throw_arraybounds_fn(filename,lineno);
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#define _check_known_subscript_null(p,b,e) (_check_known_subscript_null_fn(p,b,e,__FILE__,__LINE__))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_check_known_subscript_notnull_fn(unsigned bound,unsigned index,const char *filename,unsigned lineno) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds_fn(filename,lineno); 
  return _cksnn_index;
}
#define _check_known_subscript_notnull(b,i) (_check_known_subscript_notnull_fn(b,i,__FILE__,__LINE__))
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
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
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus_<type>_fn. */
static _INLINE char *
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript_fn(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index,const char *filename, unsigned lineno) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _cus_ans;
}
#define _check_dyneither_subscript(a,s,i) \
  _check_dyneither_subscript_fn(a,s,i,__FILE__,__LINE__)
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
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr_fn(struct _dyneither_ptr arr, 
                        unsigned elt_sz,unsigned num_elts,
                        const char *filename, unsigned lineno) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&
      _curr != (unsigned char *)0)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

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

/* FIX?  Not sure if we want to pass filename and lineno in here... */
static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
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
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133 "list.h"
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 34 "position.h"
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 482
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
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 525
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 692 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 894
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 921
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 924
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 944
void*Cyc_Absyn_compress_kb(void*);
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 956
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 958
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 960
extern void*Cyc_Absyn_empty_effect;
# 962
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_exn_typ();
# 966
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
# 969
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 972
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_bounds_one;
# 975
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 978
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 992
void*Cyc_Absyn_void_star_typ();
# 994
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 998
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
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
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1073
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1076
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
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1115
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1118
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1121
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1124
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1127
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1132
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1138
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1150
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1159
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1161
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1163
int Cyc_Absyn_fntype_att(void*a);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1173
int Cyc_Absyn_is_require_union_type(void*);
# 1175
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1178
extern int Cyc_Absyn_porting_c_code;
# 1180
extern int Cyc_Absyn_no_regions;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 98
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 131
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 28 "absyn.cyc"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);
# 35
int Cyc_Cyclone_tovc_r=0;
# 37
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 45
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;};}
# 52
if(ss2 != 0)return - 1;
return 0;}
# 55
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 59
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;union Cyc_Absyn_Nmspace _tmpE;struct _dyneither_ptr*_tmpD;_LL1: _tmpE=_tmp0->f1;_tmpD=_tmp0->f2;_LL2:;{
struct _tuple0*_tmp1=q2;union Cyc_Absyn_Nmspace _tmpC;struct _dyneither_ptr*_tmpB;_LL4: _tmpC=_tmp1->f1;_tmpB=_tmp1->f2;_LL5:;{
int i=Cyc_strptrcmp(_tmpD,_tmpB);
if(i != 0)return i;{
struct _tuple12 _tmp2=({struct _tuple12 _tmpA;_tmpA.f1=_tmpE;_tmpA.f2=_tmpC;_tmpA;});struct _tuple12 _tmp3=_tmp2;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp4;switch(((_tmp3.f1).Abs_n).tag){case 4U: if(((_tmp3.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 71
 return - 1;}case 1U: switch(((_tmp3.f2).Loc_n).tag){case 1U: _LL9: _tmp5=((_tmp3.f1).Rel_n).val;_tmp4=((_tmp3.f2).Rel_n).val;_LLA:
# 67
 return Cyc_Absyn_strlist_cmp(_tmp5,_tmp4);case 4U: goto _LL11;default: _LL13: _LL14:
# 73
 return - 1;}case 2U: switch(((_tmp3.f2).Rel_n).tag){case 2U: _LLB: _tmp7=((_tmp3.f1).Abs_n).val;_tmp6=((_tmp3.f2).Abs_n).val;_LLC:
# 68
 return Cyc_Absyn_strlist_cmp(_tmp7,_tmp6);case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 75
 return - 1;}default: switch(((_tmp3.f2).Rel_n).tag){case 3U: _LLD: _tmp9=((_tmp3.f1).C_n).val;_tmp8=((_tmp3.f2).C_n).val;_LLE:
# 69
 return Cyc_Absyn_strlist_cmp(_tmp9,_tmp8);case 4U: _LL11: _LL12:
# 72
 return 1;case 1U: _LL15: _LL16:
# 74
 return 1;default: _LL19: _LL1A:
# 76
 return 1;}}_LL6:;};};};};}
# 80
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 86
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
if(C_scope)
return({union Cyc_Absyn_Nmspace _tmpF;(_tmpF.C_n).val=x;(_tmpF.C_n).tag=3;_tmpF;});else{
# 91
return({union Cyc_Absyn_Nmspace _tmp10;(_tmp10.Abs_n).val=x;(_tmp10.Abs_n).tag=2;_tmp10;});}}
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp11;(_tmp11.Rel_n).val=x;(_tmp11.Rel_n).tag=1;_tmp11;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 96
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmp12=(*qv).f1;union Cyc_Absyn_Nmspace _tmp13=_tmp12;switch((_tmp13.Loc_n).tag){case 1U: if((_tmp13.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmp13.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 105
static int Cyc_Absyn_new_type_counter=0;
# 107
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14=_cycalloc(sizeof(*_tmp14));({struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp1D5=({struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp15;_tmp15.tag=1U;_tmp15.f1=k;_tmp15.f2=0;_tmp15.f3=Cyc_Absyn_new_type_counter ++;_tmp15.f4=env;_tmp15;});_tmp14[0]=_tmp1D5;});_tmp14;});}
# 110
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 114
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp16;_tmp16.print_const=x.print_const  || y.print_const;_tmp16.q_volatile=
x.q_volatile  || y.q_volatile;_tmp16.q_restrict=
x.q_restrict  || y.q_restrict;_tmp16.real_const=
x.real_const  || y.real_const;({unsigned int _tmp1D6=
Cyc_Position_segment_join(x.loc,y.loc);_tmp16.loc=_tmp1D6;});_tmp16;});}
# 122
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp17;_tmp17.print_const=0;_tmp17.q_volatile=0;_tmp17.q_restrict=0;_tmp17.real_const=0;_tmp17.loc=loc;_tmp17;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp18;_tmp18.print_const=1;_tmp18.q_volatile=0;_tmp18.q_restrict=0;_tmp18.real_const=1;_tmp18.loc=loc;_tmp18;});}
# 125
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 129
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Absyn_one_b_raw={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 131
struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct Cyc_Absyn_one_bt={1U,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
# 136
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfoU _tmp1C;(_tmp1C.UnknownDatatype).val=udi;(_tmp1C.UnknownDatatype).tag=1;_tmp1C;});}
# 139
extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfoU _tmp1D;(_tmp1D.KnownDatatype).val=d;(_tmp1D.KnownDatatype).tag=2;_tmp1D;});}
# 142
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfoU _tmp1E;(_tmp1E.UnknownDatatypefield).val=s;(_tmp1E.UnknownDatatypefield).tag=1;_tmp1E;});}
# 145
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfoU _tmp1F;({struct _tuple1 _tmp1D7=({struct _tuple1 _tmp20;_tmp20.f1=dd;_tmp20.f2=df;_tmp20;});(_tmp1F.KnownDatatypefield).val=_tmp1D7;});(_tmp1F.KnownDatatypefield).tag=2;_tmp1F;});}
# 148
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfoU _tmp21;({struct _tuple2 _tmp1D8=({struct _tuple2 _tmp22;_tmp22.f1=ak;_tmp22.f2=n;_tmp22.f3=tagged;_tmp22;});(_tmp21.UnknownAggr).val=_tmp1D8;});(_tmp21.UnknownAggr).tag=1;_tmp21;});}
# 151
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfoU _tmp23;(_tmp23.KnownAggr).val=ad;(_tmp23.KnownAggr).tag=2;_tmp23;});}
# 155
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){return({union Cyc_Absyn_Constraint*_tmp24=_cycalloc(sizeof(*_tmp24));(_tmp24->Eq_constr).val=x;(_tmp24->Eq_constr).tag=1;_tmp24;});}
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){return({union Cyc_Absyn_Constraint*_tmp25=_cycalloc_atomic(sizeof(*_tmp25));(_tmp25->No_constr).val=0;(_tmp25->No_constr).tag=3;_tmp25;});}
# 162
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0U};
# 164
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
# 171
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp27=x;union Cyc_Absyn_Constraint*_tmp2A;switch((((union Cyc_Absyn_Constraint*)_tmp27)->Eq_constr).tag){case 3U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return x;default: _LL5: _tmp2A=(_tmp27->Forward_constr).val;_LL6: {
# 176
union Cyc_Absyn_Constraint*_tmp28=Cyc_Absyn_compress_conref(_tmp2A);
({struct _union_Constraint_Forward_constr*_tmp29=& x->Forward_constr;_tmp29->tag=2;_tmp29->val=_tmp28;});
return _tmp28;}}_LL0:;}
# 182
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp2B=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint*_tmp2C=_tmp2B;void*_tmp2F;if((((union Cyc_Absyn_Constraint*)_tmp2C)->Eq_constr).tag == 1){_LL1: _tmp2F=(void*)(_tmp2C->Eq_constr).val;_LL2:
 return _tmp2F;}else{_LL3: _LL4:
({void*_tmp2D=0U;({struct _dyneither_ptr _tmp1D9=({const char*_tmp2E="conref_val";_tag_dyneither(_tmp2E,sizeof(char),11U);});Cyc_Tcutil_impos(_tmp1D9,_tag_dyneither(_tmp2D,sizeof(void*),0U));});});}_LL0:;}
# 189
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp30=x;void*_tmp31;switch((((union Cyc_Absyn_Constraint*)_tmp30)->No_constr).tag){case 1U: _LL1: _tmp31=(void*)(_tmp30->Eq_constr).val;_LL2:
 return _tmp31;case 3U: _LL3: _LL4:
 return y;default: _LL5: _LL6:
 return({void*_tmp1DA=y;Cyc_Absyn_conref_def(_tmp1DA,Cyc_Absyn_compress_conref(x));});}_LL0:;}
# 197
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){
({union Cyc_Absyn_Constraint*_tmp1DB=Cyc_Absyn_compress_conref(x);x=_tmp1DB;});{
union Cyc_Absyn_Constraint*_tmp32=x;void*_tmp34;if((((union Cyc_Absyn_Constraint*)_tmp32)->Eq_constr).tag == 1){_LL1: _tmp34=(void*)(_tmp32->Eq_constr).val;_LL2:
 return _tmp34;}else{_LL3: _LL4:
({struct _union_Constraint_Eq_constr*_tmp33=& x->Eq_constr;_tmp33->tag=1;_tmp33->val=y;});return y;}_LL0:;};}
# 205
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp35=k;void**_tmp37;void**_tmp36;switch(*((int*)_tmp35)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp36=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35)->f1)->v;_LL8:
# 210
 _tmp37=_tmp36;goto _LLA;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35)->f1 == 0){_LL5: _LL6:
# 209
 return k;}else{_LL9: _tmp37=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35)->f1)->v;_LLA:
# 212
({void*_tmp1DC=Cyc_Absyn_compress_kb(*_tmp37);*_tmp37=_tmp1DC;});
return*_tmp37;}}_LL0:;}
# 216
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp38=Cyc_Absyn_compress_kb(kb);void*_tmp39=_tmp38;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Absyn_Kind*_tmp3D;struct Cyc_Core_Opt**_tmp3C;struct Cyc_Absyn_Kind*_tmp3B;switch(*((int*)_tmp39)){case 0U: _LL1: _tmp3B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp39)->f1;_LL2:
 return _tmp3B;case 1U: _LL3: _tmp3C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp39)->f1;_LL4:
 _tmp3E=_tmp3C;_tmp3D=& Cyc_Tcutil_bk;goto _LL6;default: _LL5: _tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp39)->f1;_tmp3D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp39)->f2;_LL6:
# 221
({struct Cyc_Core_Opt*_tmp1DE=({struct Cyc_Core_Opt*_tmp3A=_cycalloc(sizeof(*_tmp3A));({void*_tmp1DD=Cyc_Tcutil_kind_to_bound(_tmp3D);_tmp3A->v=_tmp1DD;});_tmp3A;});*_tmp3E=_tmp1DE;});
return _tmp3D;}_LL0:;}
# 227
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={20U};
struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={21U};
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={22U};
struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0U};
# 233
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_char_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uchar_tt={6U,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ushort_tt={6U,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uint_tt={6U,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulong_tt={6U,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulonglong_tt={6U,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;
void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;
# 246
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_schar_tt={6U,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sshort_tt={6U,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sint_tt={6U,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slong_tt={6U,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slonglong_tt={6U,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;
void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
# 257
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nshort_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nint_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlong_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlonglong_tt={6U,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_nshort_typ=(void*)& Cyc_Absyn_nshort_tt;
void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;
void*Cyc_Absyn_nlonglong_typ=(void*)& Cyc_Absyn_nlonglong_tt;
# 266
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp52=sn;switch(_tmp52){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 269
enum Cyc_Absyn_Size_of _tmp53=sz;switch(_tmp53){case Cyc_Absyn_Char_sz: _LL8: _LL9:
 return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LLA: _LLB:
 return Cyc_Absyn_sshort_typ;case Cyc_Absyn_Int_sz: _LLC: _LLD:
 return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LLE: _LLF:
 return Cyc_Absyn_slong_typ;default: _LL10: _LL11:
 return Cyc_Absyn_slonglong_typ;}_LL7:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 277
enum Cyc_Absyn_Size_of _tmp54=sz;switch(_tmp54){case Cyc_Absyn_Char_sz: _LL13: _LL14:
 return Cyc_Absyn_uchar_typ;case Cyc_Absyn_Short_sz: _LL15: _LL16:
 return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz: _LL17: _LL18:
 return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
 return Cyc_Absyn_ulong_typ;default: _LL1B: _LL1C:
 return Cyc_Absyn_ulonglong_typ;}_LL12:;}default: _LL5: _LL6: {
# 285
enum Cyc_Absyn_Size_of _tmp55=sz;switch(_tmp55){case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
 return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz: _LL20: _LL21:
 return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL22: _LL23:
 return Cyc_Absyn_nint_typ;case Cyc_Absyn_Long_sz: _LL24: _LL25:
 return Cyc_Absyn_nlong_typ;default: _LL26: _LL27:
 return Cyc_Absyn_nlonglong_typ;}_LL1D:;}}_LL0:;}
# 295
void*Cyc_Absyn_float_typ(int i){
static struct Cyc_Absyn_FloatType_Absyn_Type_struct fl={7U,0};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct db={7U,1};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct ldb={7U,2};
if(i == 0)return(void*)& fl;
if(i == 1)return(void*)& db;
return(void*)& ldb;}
# 306
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 309
void*Cyc_Absyn_wchar_typ(){
int _tmp59= Sizeof_wchar_t;switch(_tmp59){case 1U: _LL1: _LL2:
# 320 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;default: _LL5: _LL6:
# 324
 return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}_LL0:;}static char _tmp5A[4U]="exn";
# 328
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp5A,_tmp5A,_tmp5A + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5B[15U]="Null_Exception";
# 340 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5B,_tmp5B,_tmp5B + 15U};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp5C[13U]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp5C,_tmp5C,_tmp5C + 13U};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp5D[16U]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp5D,_tmp5D,_tmp5D + 16U};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp5E[10U]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp5E,_tmp5E,_tmp5E + 10U};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
# 346
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,& Cyc_Absyn_exn_l2};
# 351
static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)& Cyc_Absyn_exn_l3};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,& Cyc_Absyn_exn_ol,1};
# 354
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
# 357
static struct Cyc_Absyn_DatatypeType_Absyn_Type_struct Cyc_Absyn_exn_typ_tt={3U,{{.KnownDatatype={2,& Cyc_Absyn_exn_tud}},0}};
# 359
void*Cyc_Absyn_exn_typ(){
static struct Cyc_Core_Opt*exn_type_val=0;
if(exn_type_val == 0)
({struct Cyc_Core_Opt*_tmp1E1=({struct Cyc_Core_Opt*_tmp60=_cycalloc(sizeof(*_tmp60));({void*_tmp1E0=({struct Cyc_Absyn_Tqual _tmp1DF=
Cyc_Absyn_empty_tqual(0U);
# 362
Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1DF,Cyc_Absyn_false_conref);});_tmp60->v=_tmp1E0;});_tmp60;});exn_type_val=_tmp1E1;});
# 365
return(void*)exn_type_val->v;}static char _tmp61[9U]="PrintArg";
# 369
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp61,_tmp61,_tmp61 + 9U};static char _tmp62[9U]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp62,_tmp62,_tmp62 + 9U};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 373
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 375
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp63[14U]="unique_region";
# 383
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp63,_tmp63,_tmp63 + 14U};static char _tmp64[5U]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp64,_tmp64,_tmp64 + 5U};
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)& Cyc_Absyn_Core_str,0};
# 387
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 389
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 391
static struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct Cyc_Absyn_uniquergn_typ={15U,(void*)& Cyc_Absyn_UniqueRgn_val};
# 394
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_uniquergn_typ,0,0,0,0};
# 397
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1U,& Cyc_Absyn_uniquergn_var_d};
# 399
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1U,(void*)& Cyc_Absyn_uniquergn_bind_p};
# 401
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergn_typ,(void*)& Cyc_Absyn_uniquergn_exp_r,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 404
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 407
static void**Cyc_Absyn_string_t_opt=0;
void*Cyc_Absyn_string_typ(void*rgn){
void*_tmp68=Cyc_Tcutil_compress(rgn);void*_tmp69=_tmp68;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp69)->tag == 20U){_LL1: _LL2:
# 411
 if(Cyc_Absyn_string_t_opt == 0){
void*t=({void*_tmp1E3=Cyc_Absyn_char_typ;struct Cyc_Absyn_Tqual _tmp1E2=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_starb_typ(_tmp1E3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1E2,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 414
({void**_tmp1E4=({void**_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=t;_tmp6A;});Cyc_Absyn_string_t_opt=_tmp1E4;});}
# 416
return*((void**)_check_null(Cyc_Absyn_string_t_opt));}else{_LL3: _LL4:
# 418
 return({void*_tmp1E7=Cyc_Absyn_char_typ;void*_tmp1E6=rgn;struct Cyc_Absyn_Tqual _tmp1E5=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_starb_typ(_tmp1E7,_tmp1E6,_tmp1E5,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});}_LL0:;}
# 421
static void**Cyc_Absyn_const_string_t_opt=0;
void*Cyc_Absyn_const_string_typ(void*rgn){
void*_tmp6B=Cyc_Tcutil_compress(rgn);void*_tmp6C=_tmp6B;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6C)->tag == 20U){_LL1: _LL2:
# 425
 if(Cyc_Absyn_const_string_t_opt == 0){
void*t=({void*_tmp1E9=Cyc_Absyn_char_typ;struct Cyc_Absyn_Tqual _tmp1E8=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_starb_typ(_tmp1E9,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1E8,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 428
({void**_tmp1EA=({void**_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=t;_tmp6D;});Cyc_Absyn_const_string_t_opt=_tmp1EA;});}
# 430
return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}else{_LL3: _LL4:
# 432
 return({void*_tmp1ED=Cyc_Absyn_char_typ;void*_tmp1EC=rgn;struct Cyc_Absyn_Tqual _tmp1EB=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_starb_typ(_tmp1ED,_tmp1EC,_tmp1EB,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});}_LL0:;}
# 437
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 439
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F1=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6F;_tmp6F.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp1F0=({struct Cyc_Absyn_PtrInfo _tmp70;_tmp70.elt_typ=t;_tmp70.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1EF=({(_tmp70.ptr_atts).rgn=r;(_tmp70.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp1EE=
# 441
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);(_tmp70.ptr_atts).bounds=_tmp1EE;});(_tmp70.ptr_atts).zero_term=zeroterm;(_tmp70.ptr_atts).ptrloc=0;_tmp70.ptr_atts;});_tmp70.ptr_atts=_tmp1EF;});_tmp70;});_tmp6F.f1=_tmp1F0;});_tmp6F;});_tmp6E[0]=_tmp1F1;});_tmp6E;});}
# 445
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 447
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F5=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp72;_tmp72.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp1F4=({struct Cyc_Absyn_PtrInfo _tmp73;_tmp73.elt_typ=t;_tmp73.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1F3=({(_tmp73.ptr_atts).rgn=r;(_tmp73.ptr_atts).nullable=Cyc_Absyn_false_conref;({union Cyc_Absyn_Constraint*_tmp1F2=
# 449
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);(_tmp73.ptr_atts).bounds=_tmp1F2;});(_tmp73.ptr_atts).zero_term=zeroterm;(_tmp73.ptr_atts).ptrloc=0;_tmp73.ptr_atts;});_tmp73.ptr_atts=_tmp1F3;});_tmp73;});_tmp72.f1=_tmp1F4;});_tmp72;});_tmp71[0]=_tmp1F5;});_tmp71;});}
# 454
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F8=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp75;_tmp75.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp1F7=({struct Cyc_Absyn_PtrInfo _tmp76;_tmp76.elt_typ=t;_tmp76.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1F6=({(_tmp76.ptr_atts).rgn=r;(_tmp76.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp76.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp76.ptr_atts).zero_term=zeroterm;(_tmp76.ptr_atts).ptrloc=0;_tmp76.ptr_atts;});_tmp76.ptr_atts=_tmp1F6;});_tmp76;});_tmp75.f1=_tmp1F7;});_tmp75;});_tmp74[0]=_tmp1F8;});_tmp74;});}
# 461
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FB=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp78;_tmp78.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp1FA=({struct Cyc_Absyn_PtrInfo _tmp79;_tmp79.elt_typ=t;_tmp79.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1F9=({(_tmp79.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val;(_tmp79.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp79.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp79.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp79.ptr_atts).ptrloc=0;_tmp79.ptr_atts;});_tmp79.ptr_atts=_tmp1F9;});_tmp79;});_tmp78.f1=_tmp1FA;});_tmp78;});_tmp77[0]=_tmp1FB;});_tmp77;});}
# 469
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FE=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7B;_tmp7B.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp1FD=({struct Cyc_Absyn_PtrInfo _tmp7C;_tmp7C.elt_typ=t;_tmp7C.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1FC=({(_tmp7C.ptr_atts).rgn=r;(_tmp7C.ptr_atts).nullable=Cyc_Absyn_false_conref;(_tmp7C.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp7C.ptr_atts).zero_term=zeroterm;(_tmp7C.ptr_atts).ptrloc=0;_tmp7C.ptr_atts;});_tmp7C.ptr_atts=_tmp1FC;});_tmp7C;});_tmp7B.f1=_tmp1FD;});_tmp7B;});_tmp7A[0]=_tmp1FE;});_tmp7A;});}
# 476
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp201=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7E;_tmp7E.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp200=({struct Cyc_Absyn_PtrInfo _tmp7F;_tmp7F.elt_typ=t;_tmp7F.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1FF=({(_tmp7F.ptr_atts).rgn=r;(_tmp7F.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp7F.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmp7F.ptr_atts).zero_term=zeroterm;(_tmp7F.ptr_atts).ptrloc=0;_tmp7F.ptr_atts;});_tmp7F.ptr_atts=_tmp1FF;});_tmp7F;});_tmp7E.f1=_tmp200;});_tmp7E;});_tmp7D[0]=_tmp201;});_tmp7D;});}
# 484
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc){
# 486
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80=_cycalloc(sizeof(*_tmp80));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp203=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp81;_tmp81.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmp202=({struct Cyc_Absyn_ArrayInfo _tmp82;_tmp82.elt_type=elt_type;_tmp82.tq=tq;_tmp82.num_elts=num_elts;_tmp82.zero_term=zero_term;_tmp82.zt_loc=ztloc;_tmp82;});_tmp81.f1=_tmp202;});_tmp81;});_tmp80[0]=_tmp203;});_tmp80;});}
# 489
void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;
# 492
if(void_star_t_opt == 0)
({void**_tmp206=({void**_tmp83=_cycalloc(sizeof(*_tmp83));({void*_tmp205=({struct Cyc_Absyn_Tqual _tmp204=
Cyc_Absyn_empty_tqual(0U);
# 493
Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,_tmp204,Cyc_Absyn_false_conref);});_tmp83[0]=_tmp205;});_tmp83;});void_star_t_opt=_tmp206;});
# 495
return*void_star_t_opt;}
# 497
static struct Cyc_Absyn_JoinEff_Absyn_Type_struct Cyc_Absyn_empty_eff={24U,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
# 504
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp20A=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp86;_tmp86.tag=11U;({struct Cyc_Absyn_AggrInfo _tmp209=({struct Cyc_Absyn_AggrInfo _tmp87;({union Cyc_Absyn_AggrInfoU _tmp208=({enum Cyc_Absyn_AggrKind _tmp207=k;Cyc_Absyn_UnknownAggr(_tmp207,({struct _tuple0*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=Cyc_Absyn_rel_ns_null;_tmp88->f2=name;_tmp88;}),0);});_tmp87.aggr_info=_tmp208;});_tmp87.targs=0;_tmp87;});_tmp86.f1=_tmp209;});_tmp86;});_tmp85[0]=_tmp20A;});_tmp85;});}
# 508
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}
# 511
void*Cyc_Absyn_strctq(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp20D=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8A;_tmp8A.tag=11U;({struct Cyc_Absyn_AggrInfo _tmp20C=({struct Cyc_Absyn_AggrInfo _tmp8B;({union Cyc_Absyn_AggrInfoU _tmp20B=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0);_tmp8B.aggr_info=_tmp20B;});_tmp8B.targs=0;_tmp8B;});_tmp8A.f1=_tmp20C;});_tmp8A;});_tmp89[0]=_tmp20D;});_tmp89;});}
# 514
void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp210=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8D;_tmp8D.tag=11U;({struct Cyc_Absyn_AggrInfo _tmp20F=({struct Cyc_Absyn_AggrInfo _tmp8E;({union Cyc_Absyn_AggrInfoU _tmp20E=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0);_tmp8E.aggr_info=_tmp20E;});_tmp8E.targs=0;_tmp8E;});_tmp8D.f1=_tmp20F;});_tmp8D;});_tmp8C[0]=_tmp210;});_tmp8C;});}
# 518
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp8F;({struct _tuple3 _tmp211=({struct _tuple3 _tmp90;_tmp90.f1=sn;_tmp90.f2=c;_tmp90;});(_tmp8F.Char_c).val=_tmp211;});(_tmp8F.Char_c).tag=2;_tmp8F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp91;(_tmp91.Wchar_c).val=s;(_tmp91.Wchar_c).tag=3;_tmp91;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp92;({struct _tuple4 _tmp212=({struct _tuple4 _tmp93;_tmp93.f1=sn;_tmp93.f2=s;_tmp93;});(_tmp92.Short_c).val=_tmp212;});(_tmp92.Short_c).tag=4;_tmp92;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp94;({struct _tuple5 _tmp213=({struct _tuple5 _tmp95;_tmp95.f1=sn;_tmp95.f2=i;_tmp95;});(_tmp94.Int_c).val=_tmp213;});(_tmp94.Int_c).tag=5;_tmp94;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp96;({struct _tuple6 _tmp214=({struct _tuple6 _tmp97;_tmp97.f1=sn;_tmp97.f2=l;_tmp97;});(_tmp96.LongLong_c).val=_tmp214;});(_tmp96.LongLong_c).tag=6;_tmp96;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp98;({struct _tuple7 _tmp215=({struct _tuple7 _tmp99;_tmp99.f1=s;_tmp99.f2=i;_tmp99;});(_tmp98.Float_c).val=_tmp215;});(_tmp98.Float_c).tag=7;_tmp98;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp9A;(_tmp9A.String_c).val=s;(_tmp9A.String_c).tag=8;_tmp9A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp9B;(_tmp9B.Wstring_c).val=s;(_tmp9B.Wstring_c).tag=9;_tmp9B;});}
# 529
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->topt=0;_tmp9C->r=r;_tmp9C->loc=loc;_tmp9C->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp9C;});}
# 532
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp217=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp216=({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp9E;_tmp9E.tag=16U;_tmp9E.f1=rgn_handle;_tmp9E.f2=e;_tmp9E;});_tmp9D[0]=_tmp216;});_tmp9D;});Cyc_Absyn_new_exp(_tmp217,loc);});}
# 535
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=*e;_tmp9F;});}
# 538
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
return({void*_tmp219=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp218=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpA1;_tmpA1.tag=0U;_tmpA1.f1=c;_tmpA1;});_tmpA0[0]=_tmp218;});_tmpA0;});Cyc_Absyn_new_exp(_tmp219,loc);});}
# 541
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 545
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return({union Cyc_Absyn_Cnst _tmp21A=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp21A,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 553
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 560
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp21B=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp21B,loc);});}
# 566
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp21C=Cyc_Absyn_Wchar_c(s);Cyc_Absyn_const_exp(_tmp21C,loc);});}
# 569
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp21D=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp21D,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp21E=Cyc_Absyn_String_c(s);Cyc_Absyn_const_exp(_tmp21E,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp21F=Cyc_Absyn_Wstring_c(s);Cyc_Absyn_const_exp(_tmp21F,loc);});}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
return({void*_tmp223=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp222=({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmpA8;_tmpA8.tag=1U;({void*_tmp221=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp220=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmpAA;_tmpAA.tag=0U;_tmpAA.f1=q;_tmpAA;});_tmpA9[0]=_tmp220;});_tmpA9;});_tmpA8.f1=_tmp221;});_tmpA8;});_tmpA7[0]=_tmp222;});_tmpA7;});Cyc_Absyn_new_exp(_tmp223,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned int loc){
return({void*_tmp225=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp224=({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmpAC;_tmpAC.tag=1U;_tmpAC.f1=b;_tmpAC;});_tmpAB[0]=_tmp224;});_tmpAB;});Cyc_Absyn_new_exp(_tmp225,loc);});}
# 580
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
return Cyc_Absyn_var_exp(q,loc);}
# 583
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc){
return({void*_tmp227=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct _tmp226=({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct _tmpAE;_tmpAE.tag=2U;_tmpAE.f1=s;_tmpAE;});_tmpAD[0]=_tmp226;});_tmpAD;});Cyc_Absyn_new_exp(_tmp227,loc);});}
# 586
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp229=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp228=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpB0;_tmpB0.tag=3U;_tmpB0.f1=p;_tmpB0.f2=es;_tmpB0;});_tmpAF[0]=_tmp228;});_tmpAF;});Cyc_Absyn_new_exp(_tmp229,loc);});}
# 589
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp22B=p;struct Cyc_List_List*_tmp22A=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=e;_tmpB1->tl=0;_tmpB1;});Cyc_Absyn_primop_exp(_tmp22B,_tmp22A,loc);});}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp22E=p;struct Cyc_List_List*_tmp22D=({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->hd=e1;({struct Cyc_List_List*_tmp22C=({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->hd=e2;_tmpB3->tl=0;_tmpB3;});_tmpB2->tl=_tmp22C;});_tmpB2;});Cyc_Absyn_primop_exp(_tmp22E,_tmp22D,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp230=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmp22F=({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmpB5;_tmpB5.tag=35U;_tmpB5.f1=e1;_tmpB5.f2=e2;_tmpB5;});_tmpB4[0]=_tmp22F;});_tmpB4;});Cyc_Absyn_new_exp(_tmp230,loc);});}
# 598
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp232=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp231=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB7;_tmpB7.tag=4U;_tmpB7.f1=e1;_tmpB7.f2=popt;_tmpB7.f3=e2;_tmpB7;});_tmpB6[0]=_tmp231;});_tmpB6;});Cyc_Absyn_new_exp(_tmp232,loc);});}
# 611
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 614
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
return({void*_tmp234=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp233=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB9;_tmpB9.tag=5U;_tmpB9.f1=e;_tmpB9.f2=i;_tmpB9;});_tmpB8[0]=_tmp233;});_tmpB8;});Cyc_Absyn_new_exp(_tmp234,loc);});}
# 617
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
return({void*_tmp236=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp235=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpBB;_tmpBB.tag=6U;_tmpBB.f1=e1;_tmpBB.f2=e2;_tmpBB.f3=e3;_tmpBB;});_tmpBA[0]=_tmp235;});_tmpBA;});Cyc_Absyn_new_exp(_tmp236,loc);});}
# 624
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp238=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp237=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmpBD;_tmpBD.tag=7U;_tmpBD.f1=e1;_tmpBD.f2=e2;_tmpBD;});_tmpBC[0]=_tmp237;});_tmpBC;});Cyc_Absyn_new_exp(_tmp238,loc);});}
# 627
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp23A=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp239=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmpBF;_tmpBF.tag=8U;_tmpBF.f1=e1;_tmpBF.f2=e2;_tmpBF;});_tmpBE[0]=_tmp239;});_tmpBE;});Cyc_Absyn_new_exp(_tmp23A,loc);});}
# 630
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp23C=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp23B=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmpC1;_tmpC1.tag=9U;_tmpC1.f1=e1;_tmpC1.f2=e2;_tmpC1;});_tmpC0[0]=_tmp23B;});_tmpC0;});Cyc_Absyn_new_exp(_tmp23C,loc);});}
# 633
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp23E=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp23D=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpC3;_tmpC3.tag=10U;_tmpC3.f1=e;_tmpC3.f2=es;_tmpC3.f3=0;_tmpC3.f4=0;_tmpC3;});_tmpC2[0]=_tmp23D;});_tmpC2;});Cyc_Absyn_new_exp(_tmp23E,loc);});}
# 636
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp240=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp23F=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpC5;_tmpC5.tag=10U;_tmpC5.f1=e;_tmpC5.f2=es;_tmpC5.f3=0;_tmpC5.f4=1;_tmpC5;});_tmpC4[0]=_tmp23F;});_tmpC4;});Cyc_Absyn_new_exp(_tmp240,loc);});}
# 639
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp242=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmp241=({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmpC7;_tmpC7.tag=12U;_tmpC7.f1=e;_tmpC7;});_tmpC6[0]=_tmp241;});_tmpC6;});Cyc_Absyn_new_exp(_tmp242,loc);});}
# 642
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
return({void*_tmp244=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp243=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpC9;_tmpC9.tag=13U;_tmpC9.f1=e;_tmpC9.f2=ts;_tmpC9;});_tmpC8[0]=_tmp243;});_tmpC8;});Cyc_Absyn_new_exp(_tmp244,loc);});}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
return({void*_tmp246=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp245=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpCB;_tmpCB.tag=14U;_tmpCB.f1=t;_tmpCB.f2=e;_tmpCB.f3=user_cast;_tmpCB.f4=c;_tmpCB;});_tmpCA[0]=_tmp245;});_tmpCA;});Cyc_Absyn_new_exp(_tmp246,loc);});}
# 648
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp248=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp247=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmpCD;_tmpCD.tag=11U;_tmpCD.f1=e;_tmpCD.f2=0;_tmpCD;});_tmpCC[0]=_tmp247;});_tmpCC;});Cyc_Absyn_new_exp(_tmp248,loc);});}
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp24A=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp249=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmpCF;_tmpCF.tag=11U;_tmpCF.f1=e;_tmpCF.f2=1;_tmpCF;});_tmpCE[0]=_tmp249;});_tmpCE;});Cyc_Absyn_new_exp(_tmp24A,loc);});}
# 654
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp24C=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmp24B=({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmpD1;_tmpD1.tag=15U;_tmpD1.f1=e;_tmpD1;});_tmpD0[0]=_tmp24B;});_tmpD0;});Cyc_Absyn_new_exp(_tmp24C,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int loc){
return({void*_tmp24E=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp24D=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpD3;_tmpD3.tag=17U;_tmpD3.f1=t;_tmpD3;});_tmpD2[0]=_tmp24D;});_tmpD2;});Cyc_Absyn_new_exp(_tmp24E,loc);});}
# 658
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp250=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp24F=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmpD5;_tmpD5.tag=18U;_tmpD5.f1=e;_tmpD5;});_tmpD4[0]=_tmp24F;});_tmpD4;});Cyc_Absyn_new_exp(_tmp250,loc);});}
# 661
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
return({void*_tmp252=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp251=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD7;_tmpD7.tag=19U;_tmpD7.f1=t;_tmpD7.f2=ofs;_tmpD7;});_tmpD6[0]=_tmp251;});_tmpD6;});Cyc_Absyn_new_exp(_tmp252,loc);});}
# 664
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp254=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp253=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpD9;_tmpD9.tag=20U;_tmpD9.f1=e;_tmpD9;});_tmpD8[0]=_tmp253;});_tmpD8;});Cyc_Absyn_new_exp(_tmp254,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp256=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp255=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpDB;_tmpDB.tag=21U;_tmpDB.f1=e;_tmpDB.f2=n;_tmpDB.f3=0;_tmpDB.f4=0;_tmpDB;});_tmpDA[0]=_tmp255;});_tmpDA;});Cyc_Absyn_new_exp(_tmp256,loc);});}
# 668
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp258=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp257=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpDD;_tmpDD.tag=22U;_tmpDD.f1=e;_tmpDD.f2=n;_tmpDD.f3=0;_tmpDD.f4=0;_tmpDD;});_tmpDC[0]=_tmp257;});_tmpDC;});Cyc_Absyn_new_exp(_tmp258,loc);});}
# 671
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp25A=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp259=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpDF;_tmpDF.tag=23U;_tmpDF.f1=e1;_tmpDF.f2=e2;_tmpDF;});_tmpDE[0]=_tmp259;});_tmpDE;});Cyc_Absyn_new_exp(_tmp25A,loc);});}
# 674
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp25C=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmp25B=({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmpE1;_tmpE1.tag=24U;_tmpE1.f1=es;_tmpE1;});_tmpE0[0]=_tmp25B;});_tmpE0;});Cyc_Absyn_new_exp(_tmp25C,loc);});}
# 677
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){return({void*_tmp25E=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp25D=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpE3;_tmpE3.tag=37U;_tmpE3.f1=s;_tmpE3;});_tmpE2[0]=_tmp25D;});_tmpE2;});Cyc_Absyn_new_exp(_tmp25E,loc);});}
# 679
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 683
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
return({void*_tmp260=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp25F=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmpE5;_tmpE5.tag=39U;_tmpE5.f1=t;_tmpE5;});_tmpE4[0]=_tmp25F;});_tmpE4;});Cyc_Absyn_new_exp(_tmp260,loc);});}
# 686
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
return({void*_tmp262=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmp261=({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmpE7;_tmpE7.tag=40U;_tmpE7.f1=volatile_kw;_tmpE7.f2=body;_tmpE7;});_tmpE6[0]=_tmp261;});_tmpE6;});Cyc_Absyn_new_exp(_tmp262,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 690
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 692
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
({struct Cyc_List_List*_tmp264=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));({struct _tuple13*_tmp263=({struct _tuple13*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->f1=0;_tmpE9->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE9;});_tmpE8->hd=_tmp263;});_tmpE8->tl=dles;_tmpE8;});dles=_tmp264;});}
({struct Cyc_List_List*_tmp265=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);dles=_tmp265;});
return({void*_tmp267=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp266=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpEB;_tmpEB.tag=26U;_tmpEB.f1=dles;_tmpEB;});_tmpEA[0]=_tmp266;});_tmpEA;});Cyc_Absyn_new_exp(_tmp267,loc);});}
# 698
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 701
return({void*_tmp269=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp268=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpED;_tmpED.tag=36U;_tmpED.f1=n;_tmpED.f2=dles;_tmpED;});_tmpEC[0]=_tmp268;});_tmpEC;});Cyc_Absyn_new_exp(_tmp269,loc);});}
# 704
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
return({struct Cyc_Absyn_Stmt*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->r=s;_tmpEE->loc=loc;_tmpEE->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmpEE;});}
# 707
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp26B=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp26A=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpF3;_tmpF3.tag=1U;_tmpF3.f1=e;_tmpF3;});_tmpF2[0]=_tmp26A;});_tmpF2;});Cyc_Absyn_new_stmt(_tmp26B,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp26D=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp26C=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp26D,_tmp26C,loc);});}}}
# 717
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp26F=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmp26E=({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmpF5;_tmpF5.tag=3U;_tmpF5.f1=e;_tmpF5;});_tmpF4[0]=_tmp26E;});_tmpF4;});Cyc_Absyn_new_stmt(_tmp26F,loc);});}
# 720
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
return({void*_tmp271=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmp270=({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmpF7;_tmpF7.tag=4U;_tmpF7.f1=e;_tmpF7.f2=s1;_tmpF7.f3=s2;_tmpF7;});_tmpF6[0]=_tmp270;});_tmpF6;});Cyc_Absyn_new_stmt(_tmp271,loc);});}
# 723
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp275=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmp274=({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmpF9;_tmpF9.tag=5U;({struct _tuple9 _tmp273=({struct _tuple9 _tmpFA;_tmpFA.f1=e;({struct Cyc_Absyn_Stmt*_tmp272=Cyc_Absyn_skip_stmt(e->loc);_tmpFA.f2=_tmp272;});_tmpFA;});_tmpF9.f1=_tmp273;});_tmpF9.f2=s;_tmpF9;});_tmpF8[0]=_tmp274;});_tmpF8;});Cyc_Absyn_new_stmt(_tmp275,loc);});}
# 726
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp27B=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmp27A=({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmpFC;_tmpFC.tag=9U;_tmpFC.f1=e1;({struct _tuple9 _tmp277=({struct _tuple9 _tmpFD;_tmpFD.f1=e2;({struct Cyc_Absyn_Stmt*_tmp276=Cyc_Absyn_skip_stmt(e3->loc);_tmpFD.f2=_tmp276;});_tmpFD;});_tmpFC.f2=_tmp277;});({struct _tuple9 _tmp279=({struct _tuple9 _tmpFE;_tmpFE.f1=e3;({struct Cyc_Absyn_Stmt*_tmp278=
Cyc_Absyn_skip_stmt(e3->loc);_tmpFE.f2=_tmp278;});_tmpFE;});_tmpFC.f3=_tmp279;});_tmpFC.f4=s;_tmpFC;});_tmpFB[0]=_tmp27A;});_tmpFB;});
# 729
Cyc_Absyn_new_stmt(_tmp27B,loc);});}
# 733
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp27D=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp27C=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp100;_tmp100.tag=10U;_tmp100.f1=e;_tmp100.f2=scs;_tmp100.f3=0;_tmp100;});_tmpFF[0]=_tmp27C;});_tmpFF;});Cyc_Absyn_new_stmt(_tmp27D,loc);});}struct _tuple14{void*f1;void*f2;};
# 736
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmp101=({struct _tuple14 _tmp105;_tmp105.f1=s1->r;_tmp105.f2=s2->r;_tmp105;});struct _tuple14 _tmp102=_tmp101;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp102.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp102.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp27F=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp103=_cycalloc(sizeof(*_tmp103));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp27E=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp104;_tmp104.tag=2U;_tmp104.f1=s1;_tmp104.f2=s2;_tmp104;});_tmp103[0]=_tmp27E;});_tmp103;});Cyc_Absyn_new_stmt(_tmp27F,loc);});}}_LL0:;}
# 743
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
return({void*_tmp281=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp280=({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp107;_tmp107.tag=11U;_tmp107.f1=el;_tmp107.f2=0;_tmp107;});_tmp106[0]=_tmp280;});_tmp106;});Cyc_Absyn_new_stmt(_tmp281,loc);});}
# 746
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp283=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp108=_cycalloc(sizeof(*_tmp108));({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp282=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp109;_tmp109.tag=12U;_tmp109.f1=d;_tmp109.f2=s;_tmp109;});_tmp108[0]=_tmp282;});_tmp108;});Cyc_Absyn_new_stmt(_tmp283,loc);});}
# 749
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl*d=({void*_tmp286=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp285=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp10D;_tmp10D.tag=0U;({struct Cyc_Absyn_Vardecl*_tmp284=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp10D.f1=_tmp284;});_tmp10D;});_tmp10C[0]=_tmp285;});_tmp10C;});Cyc_Absyn_new_decl(_tmp286,loc);});
return({void*_tmp288=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp287=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp10B;_tmp10B.tag=12U;_tmp10B.f1=d;_tmp10B.f2=s;_tmp10B;});_tmp10A[0]=_tmp287;});_tmp10A;});Cyc_Absyn_new_stmt(_tmp288,loc);});}
# 753
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp28A=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp289=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp10F;_tmp10F.tag=13U;_tmp10F.f1=v;_tmp10F.f2=s;_tmp10F;});_tmp10E[0]=_tmp289;});_tmp10E;});Cyc_Absyn_new_stmt(_tmp28A,loc);});}
# 756
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp28E=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp28D=({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp111;_tmp111.tag=14U;_tmp111.f1=s;({struct _tuple9 _tmp28C=({struct _tuple9 _tmp112;_tmp112.f1=e;({struct Cyc_Absyn_Stmt*_tmp28B=Cyc_Absyn_skip_stmt(e->loc);_tmp112.f2=_tmp28B;});_tmp112;});_tmp111.f2=_tmp28C;});_tmp111;});_tmp110[0]=_tmp28D;});_tmp110;});Cyc_Absyn_new_stmt(_tmp28E,loc);});}
# 759
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp290=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp28F=({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp114;_tmp114.tag=15U;_tmp114.f1=s;_tmp114.f2=scs;_tmp114.f3=0;_tmp114;});_tmp113[0]=_tmp28F;});_tmp113;});Cyc_Absyn_new_stmt(_tmp290,loc);});}
# 762
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
return({void*_tmp292=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp291=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp116;_tmp116.tag=8U;_tmp116.f1=lab;_tmp116;});_tmp115[0]=_tmp291;});_tmp115;});Cyc_Absyn_new_stmt(_tmp292,loc);});}
# 765
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp293=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp293,loc);});}
# 769
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){return({struct Cyc_Absyn_Pat*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->r=p;_tmp117->topt=0;_tmp117->loc=s;_tmp117;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp295=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp118=_cycalloc(sizeof(*_tmp118));({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp294=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp119;_tmp119.tag=17U;_tmp119.f1=e;_tmp119;});_tmp118[0]=_tmp294;});_tmp118;});Cyc_Absyn_new_pat(_tmp295,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 775
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){return({struct Cyc_Absyn_Decl*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->r=r;_tmp11C->loc=loc;_tmp11C;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp297=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp296=({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp11E;_tmp11E.tag=2U;_tmp11E.f1=p;_tmp11E.f2=0;_tmp11E.f3=e;_tmp11E.f4=0;_tmp11E;});_tmp11D[0]=_tmp296;});_tmp11D;});Cyc_Absyn_new_decl(_tmp297,loc);});}
# 779
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
return({void*_tmp299=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp298=({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp120;_tmp120.tag=3U;_tmp120.f1=vds;_tmp120;});_tmp11F[0]=_tmp298;});_tmp11F;});Cyc_Absyn_new_decl(_tmp299,loc);});}
# 782
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned int loc){
return({void*_tmp29B=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp29A=({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp122;_tmp122.tag=4U;_tmp122.f1=tv;_tmp122.f2=vd;_tmp122.f3=open_exp;_tmp122;});_tmp121[0]=_tmp29A;});_tmp121;});Cyc_Absyn_new_decl(_tmp29B,loc);});}
# 785
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 787
return({void*_tmp2A0=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp123=_cycalloc(sizeof(*_tmp123));({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp29F=({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp124;_tmp124.tag=2U;({struct Cyc_Absyn_Pat*_tmp29E=({void*_tmp29D=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp125=_cycalloc(sizeof(*_tmp125));({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp29C=({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp126;_tmp126.tag=2U;_tmp126.f1=tv;_tmp126.f2=vd;_tmp126;});_tmp125[0]=_tmp29C;});_tmp125;});Cyc_Absyn_new_pat(_tmp29D,loc);});_tmp124.f1=_tmp29E;});_tmp124.f2=0;_tmp124.f3=e;_tmp124.f4=0;_tmp124;});_tmp123[0]=_tmp29F;});_tmp123;});Cyc_Absyn_new_decl(_tmp2A0,loc);});}
# 790
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->sc=Cyc_Absyn_Public;_tmp127->name=x;_tmp127->varloc=varloc;({struct Cyc_Absyn_Tqual _tmp2A1=
Cyc_Absyn_empty_tqual(0U);_tmp127->tq=_tmp2A1;});_tmp127->type=t;_tmp127->initializer=init;_tmp127->rgn=0;_tmp127->attributes=0;_tmp127->escapes=0;_tmp127;});}
# 796
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->sc=Cyc_Absyn_Static;_tmp128->name=x;_tmp128->varloc=0U;({struct Cyc_Absyn_Tqual _tmp2A2=Cyc_Absyn_empty_tqual(0U);_tmp128->tq=_tmp2A2;});_tmp128->type=t;_tmp128->initializer=init;_tmp128->rgn=0;_tmp128->attributes=0;_tmp128->escapes=0;_tmp128;});}
# 801
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 805
return({struct Cyc_Absyn_AggrdeclImpl*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->exist_vars=exists;_tmp129->rgn_po=po;_tmp129->fields=fs;_tmp129->tagged=tagged;_tmp129;});}
# 808
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 811
return({void*_tmp2A5=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp2A4=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp12B;_tmp12B.tag=5U;({struct Cyc_Absyn_Aggrdecl*_tmp2A3=({struct Cyc_Absyn_Aggrdecl*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->kind=k;_tmp12C->sc=s;_tmp12C->name=n;_tmp12C->tvs=ts;_tmp12C->impl=i;_tmp12C->attributes=atts;_tmp12C->expected_mem_kind=0;_tmp12C;});_tmp12B.f1=_tmp2A3;});_tmp12B;});_tmp12A[0]=_tmp2A4;});_tmp12A;});Cyc_Absyn_new_decl(_tmp2A5,loc);});}
# 816
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 819
return({struct Cyc_Absyn_TypeDecl*_tmp12D=_cycalloc(sizeof(*_tmp12D));({void*_tmp2A8=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp2A7=({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp12F;_tmp12F.tag=0U;({struct Cyc_Absyn_Aggrdecl*_tmp2A6=({struct Cyc_Absyn_Aggrdecl*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->kind=k;_tmp130->sc=s;_tmp130->name=n;_tmp130->tvs=ts;_tmp130->impl=i;_tmp130->attributes=atts;_tmp130->expected_mem_kind=0;_tmp130;});_tmp12F.f1=_tmp2A6;});_tmp12F;});_tmp12E[0]=_tmp2A7;});_tmp12E;});_tmp12D->r=_tmp2A8;});_tmp12D->loc=loc;_tmp12D;});}
# 825
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 828
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 830
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 833
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 835
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 838
return({void*_tmp2AB=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2AA=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp132;_tmp132.tag=6U;({struct Cyc_Absyn_Datatypedecl*_tmp2A9=({struct Cyc_Absyn_Datatypedecl*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->sc=s;_tmp133->name=n;_tmp133->tvs=ts;_tmp133->fields=fs;_tmp133->is_extensible=is_extensible;_tmp133;});_tmp132.f1=_tmp2A9;});_tmp132;});_tmp131[0]=_tmp2AA;});_tmp131;});Cyc_Absyn_new_decl(_tmp2AB,loc);});}
# 841
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 844
return({struct Cyc_Absyn_TypeDecl*_tmp134=_cycalloc(sizeof(*_tmp134));({void*_tmp2AE=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp135=_cycalloc(sizeof(*_tmp135));({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp2AD=({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp136;_tmp136.tag=2U;({struct Cyc_Absyn_Datatypedecl*_tmp2AC=({struct Cyc_Absyn_Datatypedecl*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->sc=s;_tmp137->name=n;_tmp137->tvs=ts;_tmp137->fields=fs;_tmp137->is_extensible=is_extensible;_tmp137;});_tmp136.f1=_tmp2AC;});_tmp136;});_tmp135[0]=_tmp2AD;});_tmp135;});_tmp134->r=_tmp2AE;});_tmp134->loc=loc;_tmp134;});}
# 856 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp138=(*a).f3;
void*_tmp139=Cyc_Absyn_pointer_expand(_tmp138,1);
if(_tmp138 != _tmp139)
(*a).f3=_tmp139;}
# 863
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 870
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp2B1=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp13B;_tmp13B.tag=9U;({struct Cyc_Absyn_FnInfo _tmp2B0=({struct Cyc_Absyn_FnInfo _tmp13C;_tmp13C.tvars=tvs;_tmp13C.ret_tqual=ret_tqual;({void*_tmp2AF=
# 873
Cyc_Absyn_pointer_expand(ret_typ,0);_tmp13C.ret_typ=_tmp2AF;});_tmp13C.effect=eff_typ;_tmp13C.args=args;_tmp13C.c_varargs=c_varargs;_tmp13C.cyc_varargs=cyc_varargs;_tmp13C.rgn_po=rgn_po;_tmp13C.attributes=atts;_tmp13C.requires_clause=req;_tmp13C.requires_relns=0;_tmp13C.ensures_clause=ens;_tmp13C.ensures_relns=0;_tmp13C;});_tmp13B.f1=_tmp2B0;});_tmp13B;});_tmp13A[0]=_tmp2B1;});_tmp13A;});}
# 887
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmp13D=Cyc_Tcutil_compress(t);void*_tmp13E=_tmp13D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->tag == 9U){_LL1: _LL2:
 return({void*_tmp2B4=t;void*_tmp2B3=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->v=& Cyc_Tcutil_rk;_tmp13F;}),0):(void*)& Cyc_Absyn_HeapRgn_val;
# 889
struct Cyc_Absyn_Tqual _tmp2B2=
# 893
Cyc_Absyn_empty_tqual(0U);
# 889
Cyc_Absyn_at_typ(_tmp2B4,_tmp2B3,_tmp2B2,Cyc_Absyn_false_conref);});}else{_LL3: _LL4:
# 894
 return t;}_LL0:;}
# 909 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmp140=e->r;void*_tmp141=_tmp140;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Vardecl*_tmp145;struct Cyc_Absyn_Vardecl*_tmp144;switch(*((int*)_tmp141)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp141)->f1)){case 2U: _LL1: _LL2:
# 912
 return 0;case 1U: _LL3: _tmp144=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp141)->f1)->f1;_LL4:
 _tmp145=_tmp144;goto _LL6;case 4U: _LL5: _tmp145=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp141)->f1)->f1;_LL6: {
# 915
void*_tmp142=Cyc_Tcutil_compress(_tmp145->type);void*_tmp143=_tmp142;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp143)->tag == 8U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 919
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmp146=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL10:
 return Cyc_Absyn_is_lvalue(_tmp146);case 13U: _LL11: _tmp147=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL12:
 return Cyc_Absyn_is_lvalue(_tmp147);case 12U: _LL13: _tmp148=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL14:
 return Cyc_Absyn_is_lvalue(_tmp148);default: _LL15: _LL16:
 return 0;}_LL0:;}
# 930
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmp149=fields;for(0;_tmp149 != 0;_tmp149=_tmp149->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp149->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmp149->hd;}}
return 0;}
# 936
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 940
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 946
if(ts == 0)
return 0;
return(struct _tuple10*)ts->hd;}
# 951
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
# 970
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmp15C=a;struct _dyneither_ptr _tmp19F;struct _dyneither_ptr _tmp19E;int _tmp19D;int _tmp19C;int _tmp19B;int _tmp19A;int _tmp199;int _tmp198;int _tmp197;struct _dyneither_ptr _tmp196;struct Cyc_Absyn_Exp*_tmp195;int _tmp194;switch(*((int*)_tmp15C)){case 0U: _LL1: _tmp194=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp15F;_tmp15F.tag=1U;_tmp15F.f1=(unsigned long)_tmp194;({void*_tmp15D[1U]={& _tmp15F};({struct _dyneither_ptr _tmp2B5=({const char*_tmp15E="regparm(%d)";_tag_dyneither(_tmp15E,sizeof(char),12U);});Cyc_aprintf(_tmp2B5,_tag_dyneither(_tmp15D,sizeof(void*),1U));});});});case 1U: _LL3: _LL4:
 return({const char*_tmp160="stdcall";_tag_dyneither(_tmp160,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmp161="cdecl";_tag_dyneither(_tmp161,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmp162="fastcall";_tag_dyneither(_tmp162,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmp163="noreturn";_tag_dyneither(_tmp163,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmp164="const";_tag_dyneither(_tmp164,sizeof(char),6U);});case 6U: _LLD: _tmp195=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LLE:
# 979
 if(_tmp195 == 0)return({const char*_tmp165="aligned";_tag_dyneither(_tmp165,sizeof(char),8U);});else{
# 981
enum Cyc_Cyclone_C_Compilers _tmp166=Cyc_Cyclone_c_compiler;if(_tmp166 == Cyc_Cyclone_Gcc_c){_LL3A: _LL3B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp169;_tmp169.tag=0U;({struct _dyneither_ptr _tmp2B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp195));_tmp169.f1=_tmp2B6;});({void*_tmp167[1U]={& _tmp169};({struct _dyneither_ptr _tmp2B7=({const char*_tmp168="aligned(%s)";_tag_dyneither(_tmp168,sizeof(char),12U);});Cyc_aprintf(_tmp2B7,_tag_dyneither(_tmp167,sizeof(void*),1U));});});});}else{_LL3C: _LL3D:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0U;({struct _dyneither_ptr _tmp2B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp195));_tmp16C.f1=_tmp2B8;});({void*_tmp16A[1U]={& _tmp16C};({struct _dyneither_ptr _tmp2B9=({const char*_tmp16B="align(%s)";_tag_dyneither(_tmp16B,sizeof(char),10U);});Cyc_aprintf(_tmp2B9,_tag_dyneither(_tmp16A,sizeof(void*),1U));});});});}_LL39:;}case 7U: _LLF: _LL10:
# 985
 return({const char*_tmp16D="packed";_tag_dyneither(_tmp16D,sizeof(char),7U);});case 8U: _LL11: _tmp196=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0U;_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp196);({void*_tmp16E[1U]={& _tmp170};({struct _dyneither_ptr _tmp2BA=({const char*_tmp16F="section(\"%s\")";_tag_dyneither(_tmp16F,sizeof(char),14U);});Cyc_aprintf(_tmp2BA,_tag_dyneither(_tmp16E,sizeof(void*),1U));});});});case 9U: _LL13: _LL14:
 return({const char*_tmp171="nocommon";_tag_dyneither(_tmp171,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmp172="shared";_tag_dyneither(_tmp172,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmp173="unused";_tag_dyneither(_tmp173,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmp174="weak";_tag_dyneither(_tmp174,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmp175="dllimport";_tag_dyneither(_tmp175,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmp176="dllexport";_tag_dyneither(_tmp176,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmp177="no_instrument_function";_tag_dyneither(_tmp177,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmp178="constructor";_tag_dyneither(_tmp178,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmp179="destructor";_tag_dyneither(_tmp179,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmp17A="no_check_memory_usage";_tag_dyneither(_tmp17A,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15C)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmp198=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15C)->f2;_tmp197=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15C)->f3;_LL28:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp17E;_tmp17E.tag=1U;_tmp17E.f1=(unsigned int)_tmp197;({struct Cyc_Int_pa_PrintArg_struct _tmp17D;_tmp17D.tag=1U;_tmp17D.f1=(unsigned int)_tmp198;({void*_tmp17B[2U]={& _tmp17D,& _tmp17E};({struct _dyneither_ptr _tmp2BB=({const char*_tmp17C="format(printf,%u,%u)";_tag_dyneither(_tmp17C,sizeof(char),21U);});Cyc_aprintf(_tmp2BB,_tag_dyneither(_tmp17B,sizeof(void*),2U));});});});});}else{_LL29: _tmp19A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15C)->f2;_tmp199=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15C)->f3;_LL2A:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp182;_tmp182.tag=1U;_tmp182.f1=(unsigned int)_tmp199;({struct Cyc_Int_pa_PrintArg_struct _tmp181;_tmp181.tag=1U;_tmp181.f1=(unsigned int)_tmp19A;({void*_tmp17F[2U]={& _tmp181,& _tmp182};({struct _dyneither_ptr _tmp2BC=({const char*_tmp180="format(scanf,%u,%u)";_tag_dyneither(_tmp180,sizeof(char),20U);});Cyc_aprintf(_tmp2BC,_tag_dyneither(_tmp17F,sizeof(void*),2U));});});});});}case 20U: _LL2B: _tmp19B=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL2C:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp185;_tmp185.tag=1U;_tmp185.f1=(unsigned long)_tmp19B;({void*_tmp183[1U]={& _tmp185};({struct _dyneither_ptr _tmp2BD=({const char*_tmp184="initializes(%d)";_tag_dyneither(_tmp184,sizeof(char),16U);});Cyc_aprintf(_tmp2BD,_tag_dyneither(_tmp183,sizeof(void*),1U));});});});case 21U: _LL2D: _tmp19C=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL2E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp188;_tmp188.tag=1U;_tmp188.f1=(unsigned long)_tmp19C;({void*_tmp186[1U]={& _tmp188};({struct _dyneither_ptr _tmp2BE=({const char*_tmp187="noliveunique(%d)";_tag_dyneither(_tmp187,sizeof(char),17U);});Cyc_aprintf(_tmp2BE,_tag_dyneither(_tmp186,sizeof(void*),1U));});});});case 22U: _LL2F: _tmp19D=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL30:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp18B;_tmp18B.tag=1U;_tmp18B.f1=(unsigned long)_tmp19D;({void*_tmp189[1U]={& _tmp18B};({struct _dyneither_ptr _tmp2BF=({const char*_tmp18A="noconsume(%d)";_tag_dyneither(_tmp18A,sizeof(char),14U);});Cyc_aprintf(_tmp2BF,_tag_dyneither(_tmp189,sizeof(void*),1U));});});});case 23U: _LL31: _LL32:
 return({const char*_tmp18C="pure";_tag_dyneither(_tmp18C,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmp18D="always_inline";_tag_dyneither(_tmp18D,sizeof(char),14U);});case 24U: _LL35: _tmp19E=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL36:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp190;_tmp190.tag=0U;_tmp190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp19E);({void*_tmp18E[1U]={& _tmp190};({struct _dyneither_ptr _tmp2C0=({const char*_tmp18F="__mode__(\"%s\")";_tag_dyneither(_tmp18F,sizeof(char),15U);});Cyc_aprintf(_tmp2C0,_tag_dyneither(_tmp18E,sizeof(void*),1U));});});});default: _LL37: _tmp19F=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmp15C)->f1;_LL38:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0U;_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp19F);({void*_tmp191[1U]={& _tmp193};({struct _dyneither_ptr _tmp2C1=({const char*_tmp192="alias(\"%s\")";_tag_dyneither(_tmp192,sizeof(char),12U);});Cyc_aprintf(_tmp2C1,_tag_dyneither(_tmp191,sizeof(void*),1U));});});});}_LL0:;}
# 1010
int Cyc_Absyn_fntype_att(void*a){
void*_tmp1A0=a;int _tmp1A1;switch(*((int*)_tmp1A0)){case 0U: _LL1: _tmp1A1=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1A0)->f1;_LL2:
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
 return 0;}_LL0:;}static char _tmp1A2[3U]="f0";
# 1028
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1A2,_tmp1A2,_tmp1A2 + 3U};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1U]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1U)};
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if(i >= fsz)
({struct _dyneither_ptr _tmp2C5=({unsigned int _tmp1A3=(unsigned int)(i + 1);struct _dyneither_ptr**_tmp1A4=(struct _dyneither_ptr**)_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp1A3));struct _dyneither_ptr _tmp1AA=_tag_dyneither(_tmp1A4,sizeof(struct _dyneither_ptr*),_tmp1A3);{unsigned int _tmp1A5=_tmp1A3;unsigned int j;for(j=0;j < _tmp1A5;j ++){({typeof((struct _dyneither_ptr*)(
# 1036
j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));({struct _dyneither_ptr _tmp2C3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A9;_tmp1A9.tag=1U;_tmp1A9.f1=(unsigned long)((int)j);({void*_tmp1A7[1U]={& _tmp1A9};({struct _dyneither_ptr _tmp2C2=({const char*_tmp1A8="f%d";_tag_dyneither(_tmp1A8,sizeof(char),4U);});Cyc_aprintf(_tmp2C2,_tag_dyneither(_tmp1A7,sizeof(void*),1U));});});});_tmp1A6[0]=_tmp2C3;});_tmp1A6;})))_tmp2C4=(struct _dyneither_ptr*)(j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));({struct _dyneither_ptr _tmp2C3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A9;_tmp1A9.tag=1U;_tmp1A9.f1=(unsigned long)((int)j);({void*_tmp1A7[1U]={& _tmp1A9};({struct _dyneither_ptr _tmp2C2=({const char*_tmp1A8="f%d";_tag_dyneither(_tmp1A8,sizeof(char),4U);});Cyc_aprintf(_tmp2C2,_tag_dyneither(_tmp1A7,sizeof(void*),1U));});});});_tmp1A6[0]=_tmp2C3;});_tmp1A6;}));_tmp1A4[j]=_tmp2C4;});}}_tmp1AA;});
# 1034
Cyc_Absyn_field_names=_tmp2C5;});
# 1037
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1040
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1AB=info;enum Cyc_Absyn_AggrKind _tmp1B1;struct _tuple0*_tmp1B0;enum Cyc_Absyn_AggrKind _tmp1AF;struct _tuple0*_tmp1AE;if((_tmp1AB.UnknownAggr).tag == 1){_LL1: _tmp1AF=((_tmp1AB.UnknownAggr).val).f1;_tmp1AE=((_tmp1AB.UnknownAggr).val).f2;_LL2:
 return({struct _tuple11 _tmp1AC;_tmp1AC.f1=_tmp1AF;_tmp1AC.f2=_tmp1AE;_tmp1AC;});}else{_LL3: _tmp1B1=(*(_tmp1AB.KnownAggr).val)->kind;_tmp1B0=(*(_tmp1AB.KnownAggr).val)->name;_LL4:
 return({struct _tuple11 _tmp1AD;_tmp1AD.f1=_tmp1B1;_tmp1AD.f2=_tmp1B0;_tmp1AD;});}_LL0:;}
# 1046
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1B2=info;struct Cyc_Absyn_Aggrdecl*_tmp1B5;if((_tmp1B2.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmp1B3=0U;({struct _dyneither_ptr _tmp2C6=({const char*_tmp1B4="unchecked aggrdecl";_tag_dyneither(_tmp1B4,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp2C6,_tag_dyneither(_tmp1B3,sizeof(void*),0U));});});}else{_LL3: _tmp1B5=*(_tmp1B2.KnownAggr).val;_LL4:
 return _tmp1B5;}_LL0:;}
# 1052
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp1B6=Cyc_Tcutil_compress(t);void*_tmp1B7=_tmp1B6;union Cyc_Absyn_AggrInfoU _tmp1C0;struct Cyc_List_List*_tmp1BF;switch(*((int*)_tmp1B7)){case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B7)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp1BF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B7)->f2;_LL2:
# 1055
 if(_tmp1BF == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1BF->hd)->requires_clause == 0;}else{goto _LL5;}case 11U: _LL3: _tmp1C0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info;_LL4: {
# 1058
union Cyc_Absyn_AggrInfoU _tmp1B8=_tmp1C0;enum Cyc_Absyn_AggrKind _tmp1BE;int _tmp1BD;enum Cyc_Absyn_AggrKind _tmp1BC;struct Cyc_Absyn_Aggrdecl*_tmp1BB;if((_tmp1B8.KnownAggr).tag == 2){_LL8: _tmp1BB=*(_tmp1B8.KnownAggr).val;_LL9:
# 1060
 if(_tmp1BB->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1B9=_tmp1BB->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1B9))->tagged)return 0;{
struct Cyc_List_List*_tmp1BA=_tmp1B9->fields;
if(_tmp1BA == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1BA->hd)->requires_clause == 0;};};}else{if(((_tmp1B8.UnknownAggr).val).f3 == 0){_LLA: _tmp1BC=((_tmp1B8.UnknownAggr).val).f1;_LLB:
 return _tmp1BC == Cyc_Absyn_UnionA;}else{_LLC: _tmp1BE=((_tmp1B8.UnknownAggr).val).f1;_tmp1BD=(int)(((_tmp1B8.UnknownAggr).val).f3)->v;_LLD:
 return _tmp1BE == Cyc_Absyn_UnionA  && !_tmp1BD;}}_LL7:;}default: _LL5: _LL6:
# 1069
 return 0;}_LL0:;}
# 1072
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp1C1=Cyc_Tcutil_compress(t);void*_tmp1C2=_tmp1C1;union Cyc_Absyn_AggrInfoU _tmp1CB;struct Cyc_List_List*_tmp1CA;switch(*((int*)_tmp1C2)){case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C2)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp1CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C2)->f2;_LL2:
# 1075
 if(_tmp1CA == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp1CA->hd)->requires_clause != 0;}else{goto _LL5;}case 11U: _LL3: _tmp1CB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C2)->f1).aggr_info;_LL4: {
# 1078
union Cyc_Absyn_AggrInfoU _tmp1C3=_tmp1CB;enum Cyc_Absyn_AggrKind _tmp1C9;int _tmp1C8;enum Cyc_Absyn_AggrKind _tmp1C7;struct Cyc_Absyn_Aggrdecl*_tmp1C6;if((_tmp1C3.KnownAggr).tag == 2){_LL8: _tmp1C6=*(_tmp1C3.KnownAggr).val;_LL9:
# 1080
 if(_tmp1C6->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1C4=_tmp1C6->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C4))->tagged)return 0;{
struct Cyc_List_List*_tmp1C5=_tmp1C4->fields;
if(_tmp1C5 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp1C5->hd)->requires_clause != 0;};};}else{if(((_tmp1C3.UnknownAggr).val).f3 == 0){_LLA: _tmp1C7=((_tmp1C3.UnknownAggr).val).f1;_LLB:
 return 0;}else{_LLC: _tmp1C9=((_tmp1C3.UnknownAggr).val).f1;_tmp1C8=(int)(((_tmp1C3.UnknownAggr).val).f3)->v;_LLD:
 return 0;}}_LL7:;}default: _LL5: _LL6:
# 1089
 return 0;}_LL0:;}
# 1094
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp1CC=b;struct Cyc_Absyn_Fndecl*_tmp1D2;struct Cyc_Absyn_Vardecl*_tmp1D1;struct Cyc_Absyn_Vardecl*_tmp1D0;struct Cyc_Absyn_Vardecl*_tmp1CF;struct Cyc_Absyn_Vardecl*_tmp1CE;struct _tuple0*_tmp1CD;switch(*((int*)_tmp1CC)){case 0U: _LL1: _tmp1CD=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp1CC)->f1;_LL2:
 return _tmp1CD;case 1U: _LL3: _tmp1CE=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1CC)->f1;_LL4:
 _tmp1CF=_tmp1CE;goto _LL6;case 3U: _LL5: _tmp1CF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp1CC)->f1;_LL6:
 _tmp1D0=_tmp1CF;goto _LL8;case 4U: _LL7: _tmp1D0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1CC)->f1;_LL8:
 _tmp1D1=_tmp1D0;goto _LLA;case 5U: _LL9: _tmp1D1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1CC)->f1;_LLA:
 return _tmp1D1->name;default: _LLB: _tmp1D2=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1CC)->f1;_LLC:
 return _tmp1D2->name;}_LL0:;}
# 1105
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
# 1108
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;

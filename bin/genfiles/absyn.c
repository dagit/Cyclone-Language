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
int Cyc_Absyn_fntype_att(void*);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1173
int Cyc_Absyn_is_require_union_type(void*);
# 1175
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1181
extern int Cyc_Absyn_porting_c_code;
# 1183
extern int Cyc_Absyn_no_regions;
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
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
struct _tuple12 _tmp2=({struct _tuple12 _tmp127;(_tmp127.f1=_tmpD,_tmp127.f2=_tmpB);_tmp127;});struct _tuple12 _tmp3=_tmp2;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp4;switch(((_tmp3.f1).Abs_n).tag){case 4U: if(((_tmp3.f2).Loc_n).tag == 4){_LL7: _LL8:
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
return({union Cyc_Absyn_Nmspace _tmp128;((_tmp128.C_n).tag=3U,(_tmp128.C_n).val=x);_tmp128;});else{
# 92
return({union Cyc_Absyn_Nmspace _tmp129;((_tmp129.Abs_n).tag=2U,(_tmp129.Abs_n).val=x);_tmp129;});}}
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp12A;((_tmp12A.Rel_n).tag=1U,(_tmp12A.Rel_n).val=x);_tmp12A;});}
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
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));((((((((*_tmp10).tag=1U,(*_tmp10).f1=k)),(*_tmp10).f2=0)),(*_tmp10).f3=Cyc_Absyn_new_type_counter ++)),(*_tmp10).f4=env);_tmp10;});}
# 111
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 115
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp12B;(((((((_tmp12B.print_const=x.print_const  || y.print_const,_tmp12B.q_volatile=
x.q_volatile  || y.q_volatile)),_tmp12B.q_restrict=
x.q_restrict  || y.q_restrict)),_tmp12B.real_const=
x.real_const  || y.real_const)),({
unsigned int _tmp140=Cyc_Position_segment_join(x.loc,y.loc);_tmp12B.loc=_tmp140;}));_tmp12B;});}
# 123
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp12C;(((((((_tmp12C.print_const=0,_tmp12C.q_volatile=0)),_tmp12C.q_restrict=0)),_tmp12C.real_const=0)),_tmp12C.loc=loc);_tmp12C;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp12D;(((((((_tmp12D.print_const=1,_tmp12D.q_volatile=0)),_tmp12D.q_restrict=0)),_tmp12D.real_const=1)),_tmp12D.loc=loc);_tmp12D;});}
# 126
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 130
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Absyn_one_b_raw={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 132
struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct Cyc_Absyn_one_bt={1U,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
# 137
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfoU _tmp12E;((_tmp12E.UnknownDatatype).tag=1U,(_tmp12E.UnknownDatatype).val=udi);_tmp12E;});}
# 140
extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfoU _tmp12F;((_tmp12F.KnownDatatype).tag=2U,(_tmp12F.KnownDatatype).val=d);_tmp12F;});}
# 143
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfoU _tmp130;((_tmp130.UnknownDatatypefield).tag=1U,(_tmp130.UnknownDatatypefield).val=s);_tmp130;});}
# 146
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfoU _tmp131;((_tmp131.KnownDatatypefield).tag=2U,((((_tmp131.KnownDatatypefield).val).f1=dd,((_tmp131.KnownDatatypefield).val).f2=df)));_tmp131;});}
# 149
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfoU _tmp132;((_tmp132.UnknownAggr).tag=1U,((((((_tmp132.UnknownAggr).val).f1=ak,((_tmp132.UnknownAggr).val).f2=n)),((_tmp132.UnknownAggr).val).f3=tagged)));_tmp132;});}
# 152
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfoU _tmp133;((_tmp133.KnownAggr).tag=2U,(_tmp133.KnownAggr).val=ad);_tmp133;});}
# 156
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){return({union Cyc_Absyn_Constraint*_tmp11=_cycalloc(sizeof(*_tmp11));(((*_tmp11).Eq_constr).tag=1U,((*_tmp11).Eq_constr).val=x);_tmp11;});}
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){return({union Cyc_Absyn_Constraint*_tmp12=_cycalloc(sizeof(*_tmp12));(((*_tmp12).No_constr).tag=3U,((*_tmp12).No_constr).val=0);_tmp12;});}
# 163
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0U};
# 165
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
# 172
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp13=x;union Cyc_Absyn_Constraint*_tmp16;switch((((union Cyc_Absyn_Constraint*)_tmp13)->Eq_constr).tag){case 3U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return x;default: _LL5: _tmp16=(_tmp13->Forward_constr).val;_LL6: {
# 177
union Cyc_Absyn_Constraint*_tmp14=Cyc_Absyn_compress_conref(_tmp16);
({struct _union_Constraint_Forward_constr*_tmp15=& x->Forward_constr;_tmp15->tag=2;_tmp15->val=_tmp14;});
return _tmp14;}}_LL0:;}
# 183
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp17=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint*_tmp18=_tmp17;void*_tmp1B;if((((union Cyc_Absyn_Constraint*)_tmp18)->Eq_constr).tag == 1){_LL1: _tmp1B=(void*)(_tmp18->Eq_constr).val;_LL2:
 return _tmp1B;}else{_LL3: _LL4:
({void*_tmp19=0U;({struct _dyneither_ptr _tmp141=({const char*_tmp1A="conref_val";_tag_dyneither(_tmp1A,sizeof(char),11U);});Cyc_Tcutil_impos(_tmp141,_tag_dyneither(_tmp19,sizeof(void*),0U));});});}_LL0:;}
# 190
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp1C=x;void*_tmp1D;switch((((union Cyc_Absyn_Constraint*)_tmp1C)->No_constr).tag){case 1U: _LL1: _tmp1D=(void*)(_tmp1C->Eq_constr).val;_LL2:
 return _tmp1D;case 3U: _LL3: _LL4:
 return y;default: _LL5: _LL6:
 return({void*_tmp142=y;Cyc_Absyn_conref_def(_tmp142,Cyc_Absyn_compress_conref(x));});}_LL0:;}
# 198
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){
({union Cyc_Absyn_Constraint*_tmp143=Cyc_Absyn_compress_conref(x);x=_tmp143;});{
union Cyc_Absyn_Constraint*_tmp1E=x;void*_tmp20;if((((union Cyc_Absyn_Constraint*)_tmp1E)->Eq_constr).tag == 1){_LL1: _tmp20=(void*)(_tmp1E->Eq_constr).val;_LL2:
 return _tmp20;}else{_LL3: _LL4:
({struct _union_Constraint_Eq_constr*_tmp1F=& x->Eq_constr;_tmp1F->tag=1;_tmp1F->val=y;});return y;}_LL0:;};}
# 206
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp21=k;void**_tmp23;void**_tmp22;switch(*((int*)_tmp21)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp21)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp22=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp21)->f1)->v;_LL8:
# 211
 _tmp23=_tmp22;goto _LLA;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21)->f1 == 0){_LL5: _LL6:
# 210
 return k;}else{_LL9: _tmp23=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21)->f1)->v;_LLA:
# 213
({void*_tmp144=Cyc_Absyn_compress_kb(*_tmp23);*_tmp23=_tmp144;});
return*_tmp23;}}_LL0:;}
# 217
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp24=Cyc_Absyn_compress_kb(kb);void*_tmp25=_tmp24;struct Cyc_Core_Opt**_tmp2A;struct Cyc_Absyn_Kind*_tmp29;struct Cyc_Core_Opt**_tmp28;struct Cyc_Absyn_Kind*_tmp27;switch(*((int*)_tmp25)){case 0U: _LL1: _tmp27=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25)->f1;_LL2:
 return _tmp27;case 1U: _LL3: _tmp28=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25)->f1;_LL4:
 _tmp2A=_tmp28;_tmp29=& Cyc_Tcutil_bk;goto _LL6;default: _LL5: _tmp2A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25)->f1;_tmp29=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25)->f2;_LL6:
# 222
({struct Cyc_Core_Opt*_tmp146=({struct Cyc_Core_Opt*_tmp26=_cycalloc(sizeof(*_tmp26));({void*_tmp145=Cyc_Tcutil_kind_to_bound(_tmp29);(*_tmp26).v=_tmp145;});_tmp26;});*_tmp2A=_tmp146;});
return _tmp29;}_LL0:;}
# 228
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={20U};
struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={21U};
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={22U};
struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0U};
# 234
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
# 247
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
# 258
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nshort_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nint_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlong_tt={6U,Cyc_Absyn_None,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlonglong_tt={6U,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_nshort_typ=(void*)& Cyc_Absyn_nshort_tt;
void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;
void*Cyc_Absyn_nlonglong_typ=(void*)& Cyc_Absyn_nlonglong_tt;
# 267
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp2B=sn;switch(_tmp2B){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 270
enum Cyc_Absyn_Size_of _tmp2C=sz;switch(_tmp2C){case Cyc_Absyn_Char_sz: _LL8: _LL9:
 return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LLA: _LLB:
 return Cyc_Absyn_sshort_typ;case Cyc_Absyn_Int_sz: _LLC: _LLD:
 return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LLE: _LLF:
 return Cyc_Absyn_slong_typ;default: _LL10: _LL11:
 return Cyc_Absyn_slonglong_typ;}_LL7:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 278
enum Cyc_Absyn_Size_of _tmp2D=sz;switch(_tmp2D){case Cyc_Absyn_Char_sz: _LL13: _LL14:
 return Cyc_Absyn_uchar_typ;case Cyc_Absyn_Short_sz: _LL15: _LL16:
 return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz: _LL17: _LL18:
 return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
 return Cyc_Absyn_ulong_typ;default: _LL1B: _LL1C:
 return Cyc_Absyn_ulonglong_typ;}_LL12:;}default: _LL5: _LL6: {
# 286
enum Cyc_Absyn_Size_of _tmp2E=sz;switch(_tmp2E){case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
 return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz: _LL20: _LL21:
 return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL22: _LL23:
 return Cyc_Absyn_nint_typ;case Cyc_Absyn_Long_sz: _LL24: _LL25:
 return Cyc_Absyn_nlong_typ;default: _LL26: _LL27:
 return Cyc_Absyn_nlonglong_typ;}_LL1D:;}}_LL0:;}
# 296
void*Cyc_Absyn_float_typ(int i){
static struct Cyc_Absyn_FloatType_Absyn_Type_struct fl={7U,0};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct db={7U,1};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct ldb={7U,2};
if(i == 0)return(void*)& fl;
if(i == 1)return(void*)& db;
return(void*)& ldb;}
# 307
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 310
void*Cyc_Absyn_wchar_typ(){
int _tmp2F= Sizeof_wchar_t;switch(_tmp2F){case 1U: _LL1: _LL2:
# 321 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;default: _LL5: _LL6:
# 325
 return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}_LL0:;}static char _tmp30[4U]="exn";
# 329
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp30,_tmp30,_tmp30 + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp31[15U]="Null_Exception";
# 341 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp31,_tmp31,_tmp31 + 15U};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp32[13U]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp32,_tmp32,_tmp32 + 13U};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp33[16U]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp33,_tmp33,_tmp33 + 16U};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp34[10U]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp34,_tmp34,_tmp34 + 10U};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
# 347
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,& Cyc_Absyn_exn_l2};
# 352
static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)& Cyc_Absyn_exn_l3};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,& Cyc_Absyn_exn_ol,1};
# 355
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
# 358
static struct Cyc_Absyn_DatatypeType_Absyn_Type_struct Cyc_Absyn_exn_typ_tt={3U,{{.KnownDatatype={2,& Cyc_Absyn_exn_tud}},0}};
# 360
void*Cyc_Absyn_exn_typ(){
static struct Cyc_Core_Opt*exn_type_val=0;
if(exn_type_val == 0)
({struct Cyc_Core_Opt*_tmp149=({struct Cyc_Core_Opt*_tmp35=_cycalloc(sizeof(*_tmp35));({void*_tmp148=({struct Cyc_Absyn_Tqual _tmp147=
Cyc_Absyn_empty_tqual(0U);
# 363
Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,_tmp147,Cyc_Absyn_false_conref);});(*_tmp35).v=_tmp148;});_tmp35;});exn_type_val=_tmp149;});
# 366
return(void*)exn_type_val->v;}static char _tmp36[9U]="PrintArg";
# 370
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp36,_tmp36,_tmp36 + 9U};static char _tmp37[9U]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp37,_tmp37,_tmp37 + 9U};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 374
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 376
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp38[14U]="unique_region";
# 384
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp38,_tmp38,_tmp38 + 14U};static char _tmp39[5U]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp39,_tmp39,_tmp39 + 5U};
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)& Cyc_Absyn_Core_str,0};
# 388
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 390
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 392
static struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct Cyc_Absyn_uniquergn_typ={15U,(void*)& Cyc_Absyn_UniqueRgn_val};
# 395
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_uniquergn_typ,0,0,0,0};
# 398
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1U,& Cyc_Absyn_uniquergn_var_d};
# 400
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1U,(void*)& Cyc_Absyn_uniquergn_bind_p};
# 402
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergn_typ,(void*)& Cyc_Absyn_uniquergn_exp_r,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 405
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 408
static void**Cyc_Absyn_string_t_opt=0;
void*Cyc_Absyn_string_typ(void*rgn){
void*_tmp3A=Cyc_Tcutil_compress(rgn);void*_tmp3B=_tmp3A;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3B)->tag == 20U){_LL1: _LL2:
# 412
 if(Cyc_Absyn_string_t_opt == 0){
void*t=({void*_tmp14B=Cyc_Absyn_char_typ;struct Cyc_Absyn_Tqual _tmp14A=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_starb_typ(_tmp14B,(void*)& Cyc_Absyn_HeapRgn_val,_tmp14A,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 415
({void**_tmp14C=({void**_tmp3C=_cycalloc(sizeof(*_tmp3C));*_tmp3C=t;_tmp3C;});Cyc_Absyn_string_t_opt=_tmp14C;});}
# 417
return*((void**)_check_null(Cyc_Absyn_string_t_opt));}else{_LL3: _LL4:
# 419
 return({void*_tmp14F=Cyc_Absyn_char_typ;void*_tmp14E=rgn;struct Cyc_Absyn_Tqual _tmp14D=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_starb_typ(_tmp14F,_tmp14E,_tmp14D,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});}_LL0:;}
# 422
static void**Cyc_Absyn_const_string_t_opt=0;
void*Cyc_Absyn_const_string_typ(void*rgn){
void*_tmp3D=Cyc_Tcutil_compress(rgn);void*_tmp3E=_tmp3D;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E)->tag == 20U){_LL1: _LL2:
# 426
 if(Cyc_Absyn_const_string_t_opt == 0){
void*t=({void*_tmp151=Cyc_Absyn_char_typ;struct Cyc_Absyn_Tqual _tmp150=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_starb_typ(_tmp151,(void*)& Cyc_Absyn_HeapRgn_val,_tmp150,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 429
({void**_tmp152=({void**_tmp3F=_cycalloc(sizeof(*_tmp3F));*_tmp3F=t;_tmp3F;});Cyc_Absyn_const_string_t_opt=_tmp152;});}
# 431
return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}else{_LL3: _LL4:
# 433
 return({void*_tmp155=Cyc_Absyn_char_typ;void*_tmp154=rgn;struct Cyc_Absyn_Tqual _tmp153=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_starb_typ(_tmp155,_tmp154,_tmp153,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});}_LL0:;}
# 438
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 440
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40=_cycalloc(sizeof(*_tmp40));((*_tmp40).tag=5U,((((((*_tmp40).f1).elt_typ=t,((*_tmp40).f1).elt_tq=tq)),(
((((((((((*_tmp40).f1).ptr_atts).rgn=r,(((*_tmp40).f1).ptr_atts).nullable=Cyc_Absyn_true_conref)),({
union Cyc_Absyn_Constraint*_tmp156=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);(((*_tmp40).f1).ptr_atts).bounds=_tmp156;}))),(((*_tmp40).f1).ptr_atts).zero_term=zeroterm)),(((*_tmp40).f1).ptr_atts).ptrloc=0)))));_tmp40;});}
# 446
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 448
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp41=_cycalloc(sizeof(*_tmp41));((*_tmp41).tag=5U,((((((*_tmp41).f1).elt_typ=t,((*_tmp41).f1).elt_tq=tq)),(
((((((((((*_tmp41).f1).ptr_atts).rgn=r,(((*_tmp41).f1).ptr_atts).nullable=Cyc_Absyn_false_conref)),({
union Cyc_Absyn_Constraint*_tmp157=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);(((*_tmp41).f1).ptr_atts).bounds=_tmp157;}))),(((*_tmp41).f1).ptr_atts).zero_term=zeroterm)),(((*_tmp41).f1).ptr_atts).ptrloc=0)))));_tmp41;});}
# 455
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42=_cycalloc(sizeof(*_tmp42));((*_tmp42).tag=5U,((((((*_tmp42).f1).elt_typ=t,((*_tmp42).f1).elt_tq=tq)),(
((((((((((*_tmp42).f1).ptr_atts).rgn=r,(((*_tmp42).f1).ptr_atts).nullable=Cyc_Absyn_true_conref)),(((*_tmp42).f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref)),(((*_tmp42).f1).ptr_atts).zero_term=zeroterm)),(((*_tmp42).f1).ptr_atts).ptrloc=0)))));_tmp42;});}
# 462
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43=_cycalloc(sizeof(*_tmp43));((*_tmp43).tag=5U,((((((*_tmp43).f1).elt_typ=t,((*_tmp43).f1).elt_tq=tq)),(
((((((((((*_tmp43).f1).ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((*_tmp43).f1).ptr_atts).nullable=Cyc_Absyn_true_conref)),(((*_tmp43).f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref)),(((*_tmp43).f1).ptr_atts).zero_term=Cyc_Absyn_false_conref)),(((*_tmp43).f1).ptr_atts).ptrloc=0)))));_tmp43;});}
# 470
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44=_cycalloc(sizeof(*_tmp44));((*_tmp44).tag=5U,((((((*_tmp44).f1).elt_typ=t,((*_tmp44).f1).elt_tq=tq)),(
((((((((((*_tmp44).f1).ptr_atts).rgn=r,(((*_tmp44).f1).ptr_atts).nullable=Cyc_Absyn_false_conref)),(((*_tmp44).f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref)),(((*_tmp44).f1).ptr_atts).zero_term=zeroterm)),(((*_tmp44).f1).ptr_atts).ptrloc=0)))));_tmp44;});}
# 477
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));((*_tmp45).tag=5U,((((((*_tmp45).f1).elt_typ=t,((*_tmp45).f1).elt_tq=tq)),(
((((((((((*_tmp45).f1).ptr_atts).rgn=r,(((*_tmp45).f1).ptr_atts).nullable=Cyc_Absyn_true_conref)),(((*_tmp45).f1).ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref)),(((*_tmp45).f1).ptr_atts).zero_term=zeroterm)),(((*_tmp45).f1).ptr_atts).ptrloc=0)))));_tmp45;});}
# 485
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc){
# 487
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46=_cycalloc(sizeof(*_tmp46));((*_tmp46).tag=8U,((((((((((*_tmp46).f1).elt_type=elt_type,((*_tmp46).f1).tq=tq)),((*_tmp46).f1).num_elts=num_elts)),((*_tmp46).f1).zero_term=zero_term)),((*_tmp46).f1).zt_loc=ztloc)));_tmp46;});}
# 490
void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;
# 493
if(void_star_t_opt == 0)
({void**_tmp15A=({void**_tmp47=_cycalloc(sizeof(*_tmp47));({void*_tmp159=({struct Cyc_Absyn_Tqual _tmp158=
Cyc_Absyn_empty_tqual(0U);
# 494
Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,_tmp158,Cyc_Absyn_false_conref);});*_tmp47=_tmp159;});_tmp47;});void_star_t_opt=_tmp15A;});
# 496
return*void_star_t_opt;}
# 498
static struct Cyc_Absyn_JoinEff_Absyn_Type_struct Cyc_Absyn_empty_eff={24U,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
# 505
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));((*_tmp49).tag=11U,((({union Cyc_Absyn_AggrInfoU _tmp15C=({enum Cyc_Absyn_AggrKind _tmp15B=k;Cyc_Absyn_UnknownAggr(_tmp15B,({struct _tuple0*_tmp48=_cycalloc(sizeof(*_tmp48));((*_tmp48).f1=Cyc_Absyn_rel_ns_null,(*_tmp48).f2=name);_tmp48;}),0);});((*_tmp49).f1).aggr_info=_tmp15C;}),((*_tmp49).f1).targs=0)));_tmp49;});}
# 509
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}
# 512
void*Cyc_Absyn_strctq(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));((*_tmp4A).tag=11U,((({union Cyc_Absyn_AggrInfoU _tmp15D=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0);((*_tmp4A).f1).aggr_info=_tmp15D;}),((*_tmp4A).f1).targs=0)));_tmp4A;});}
# 515
void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));((*_tmp4B).tag=11U,((({union Cyc_Absyn_AggrInfoU _tmp15E=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0);((*_tmp4B).f1).aggr_info=_tmp15E;}),((*_tmp4B).f1).targs=0)));_tmp4B;});}
# 519
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp134;((_tmp134.Char_c).tag=2U,((((_tmp134.Char_c).val).f1=sn,((_tmp134.Char_c).val).f2=c)));_tmp134;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp135;((_tmp135.Wchar_c).tag=3U,(_tmp135.Wchar_c).val=s);_tmp135;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp136;((_tmp136.Short_c).tag=4U,((((_tmp136.Short_c).val).f1=sn,((_tmp136.Short_c).val).f2=s)));_tmp136;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp137;((_tmp137.Int_c).tag=5U,((((_tmp137.Int_c).val).f1=sn,((_tmp137.Int_c).val).f2=i)));_tmp137;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp138;((_tmp138.LongLong_c).tag=6U,((((_tmp138.LongLong_c).val).f1=sn,((_tmp138.LongLong_c).val).f2=l)));_tmp138;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp139;((_tmp139.Float_c).tag=7U,((((_tmp139.Float_c).val).f1=s,((_tmp139.Float_c).val).f2=i)));_tmp139;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp13A;((_tmp13A.String_c).tag=8U,(_tmp13A.String_c).val=s);_tmp13A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp13B;((_tmp13B.Wstring_c).tag=9U,(_tmp13B.Wstring_c).val=s);_tmp13B;});}
# 530
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp4C=_cycalloc(sizeof(*_tmp4C));((((((*_tmp4C).topt=0,(*_tmp4C).r=r)),(*_tmp4C).loc=loc)),(*_tmp4C).annot=(void*)& Cyc_Absyn_EmptyAnnot_val);_tmp4C;});}
# 533
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp15F=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));((((*_tmp4D).tag=16U,(*_tmp4D).f1=rgn_handle)),(*_tmp4D).f2=e);_tmp4D;});Cyc_Absyn_new_exp(_tmp15F,loc);});}
# 536
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp4E=_cycalloc(sizeof(*_tmp4E));*_tmp4E=*e;_tmp4E;});}
# 539
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
return({void*_tmp160=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));((*_tmp4F).tag=0U,(*_tmp4F).f1=c);_tmp4F;});Cyc_Absyn_new_exp(_tmp160,loc);});}
# 542
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 546
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return({union Cyc_Absyn_Cnst _tmp161=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp161,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 554
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 561
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp162=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp162,loc);});}
# 567
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp163=Cyc_Absyn_Wchar_c(s);Cyc_Absyn_const_exp(_tmp163,loc);});}
# 570
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp164=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp164,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp165=Cyc_Absyn_String_c(s);Cyc_Absyn_const_exp(_tmp165,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp166=Cyc_Absyn_Wstring_c(s);Cyc_Absyn_const_exp(_tmp166,loc);});}
# 574
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
return({void*_tmp168=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp51=_cycalloc(sizeof(*_tmp51));((*_tmp51).tag=1U,({void*_tmp167=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp50=_cycalloc(sizeof(*_tmp50));((*_tmp50).tag=0U,(*_tmp50).f1=q);_tmp50;});(*_tmp51).f1=_tmp167;}));_tmp51;});Cyc_Absyn_new_exp(_tmp168,loc);});}
# 577
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned int loc){
return({void*_tmp169=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp52=_cycalloc(sizeof(*_tmp52));((*_tmp52).tag=1U,(*_tmp52).f1=b);_tmp52;});Cyc_Absyn_new_exp(_tmp169,loc);});}
# 581
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
return Cyc_Absyn_var_exp(q,loc);}
# 584
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc){
return({void*_tmp16A=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp53=_cycalloc(sizeof(*_tmp53));((*_tmp53).tag=2U,(*_tmp53).f1=s);_tmp53;});Cyc_Absyn_new_exp(_tmp16A,loc);});}
# 587
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp16B=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));((((*_tmp54).tag=3U,(*_tmp54).f1=p)),(*_tmp54).f2=es);_tmp54;});Cyc_Absyn_new_exp(_tmp16B,loc);});}
# 590
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp16D=p;struct Cyc_List_List*_tmp16C=({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));((*_tmp55).hd=e,(*_tmp55).tl=0);_tmp55;});Cyc_Absyn_primop_exp(_tmp16D,_tmp16C,loc);});}
# 593
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp170=p;struct Cyc_List_List*_tmp16F=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));((*_tmp57).hd=e1,({struct Cyc_List_List*_tmp16E=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));((*_tmp56).hd=e2,(*_tmp56).tl=0);_tmp56;});(*_tmp57).tl=_tmp16E;}));_tmp57;});Cyc_Absyn_primop_exp(_tmp170,_tmp16F,loc);});}
# 596
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp171=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp58=_cycalloc(sizeof(*_tmp58));((((*_tmp58).tag=35U,(*_tmp58).f1=e1)),(*_tmp58).f2=e2);_tmp58;});Cyc_Absyn_new_exp(_tmp171,loc);});}
# 599
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 609
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp172=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp59=_cycalloc(sizeof(*_tmp59));((((((*_tmp59).tag=4U,(*_tmp59).f1=e1)),(*_tmp59).f2=popt)),(*_tmp59).f3=e2);_tmp59;});Cyc_Absyn_new_exp(_tmp172,loc);});}
# 612
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 615
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
return({void*_tmp173=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));((((*_tmp5A).tag=5U,(*_tmp5A).f1=e)),(*_tmp5A).f2=i);_tmp5A;});Cyc_Absyn_new_exp(_tmp173,loc);});}
# 618
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
return({void*_tmp174=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));((((((*_tmp5B).tag=6U,(*_tmp5B).f1=e1)),(*_tmp5B).f2=e2)),(*_tmp5B).f3=e3);_tmp5B;});Cyc_Absyn_new_exp(_tmp174,loc);});}
# 625
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp175=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));((((*_tmp5C).tag=7U,(*_tmp5C).f1=e1)),(*_tmp5C).f2=e2);_tmp5C;});Cyc_Absyn_new_exp(_tmp175,loc);});}
# 628
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp176=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));((((*_tmp5D).tag=8U,(*_tmp5D).f1=e1)),(*_tmp5D).f2=e2);_tmp5D;});Cyc_Absyn_new_exp(_tmp176,loc);});}
# 631
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp177=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));((((*_tmp5E).tag=9U,(*_tmp5E).f1=e1)),(*_tmp5E).f2=e2);_tmp5E;});Cyc_Absyn_new_exp(_tmp177,loc);});}
# 634
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp178=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));((((((((*_tmp5F).tag=10U,(*_tmp5F).f1=e)),(*_tmp5F).f2=es)),(*_tmp5F).f3=0)),(*_tmp5F).f4=0);_tmp5F;});Cyc_Absyn_new_exp(_tmp178,loc);});}
# 637
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp179=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp60=_cycalloc(sizeof(*_tmp60));((((((((*_tmp60).tag=10U,(*_tmp60).f1=e)),(*_tmp60).f2=es)),(*_tmp60).f3=0)),(*_tmp60).f4=1);_tmp60;});Cyc_Absyn_new_exp(_tmp179,loc);});}
# 640
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp17A=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));((*_tmp61).tag=12U,(*_tmp61).f1=e);_tmp61;});Cyc_Absyn_new_exp(_tmp17A,loc);});}
# 643
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
return({void*_tmp17B=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp62=_cycalloc(sizeof(*_tmp62));((((*_tmp62).tag=13U,(*_tmp62).f1=e)),(*_tmp62).f2=ts);_tmp62;});Cyc_Absyn_new_exp(_tmp17B,loc);});}
# 646
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
return({void*_tmp17C=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));((((((((*_tmp63).tag=14U,(*_tmp63).f1=t)),(*_tmp63).f2=e)),(*_tmp63).f3=user_cast)),(*_tmp63).f4=c);_tmp63;});Cyc_Absyn_new_exp(_tmp17C,loc);});}
# 649
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp17D=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));((((*_tmp64).tag=11U,(*_tmp64).f1=e)),(*_tmp64).f2=0);_tmp64;});Cyc_Absyn_new_exp(_tmp17D,loc);});}
# 652
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp17E=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));((((*_tmp65).tag=11U,(*_tmp65).f1=e)),(*_tmp65).f2=1);_tmp65;});Cyc_Absyn_new_exp(_tmp17E,loc);});}
# 655
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp17F=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));((*_tmp66).tag=15U,(*_tmp66).f1=e);_tmp66;});Cyc_Absyn_new_exp(_tmp17F,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int loc){
return({void*_tmp180=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));((*_tmp67).tag=17U,(*_tmp67).f1=t);_tmp67;});Cyc_Absyn_new_exp(_tmp180,loc);});}
# 659
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp181=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp68=_cycalloc(sizeof(*_tmp68));((*_tmp68).tag=18U,(*_tmp68).f1=e);_tmp68;});Cyc_Absyn_new_exp(_tmp181,loc);});}
# 662
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
return({void*_tmp182=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp69=_cycalloc(sizeof(*_tmp69));((((*_tmp69).tag=19U,(*_tmp69).f1=t)),(*_tmp69).f2=ofs);_tmp69;});Cyc_Absyn_new_exp(_tmp182,loc);});}
# 665
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp183=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));((*_tmp6A).tag=20U,(*_tmp6A).f1=e);_tmp6A;});Cyc_Absyn_new_exp(_tmp183,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp184=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));((((((((*_tmp6B).tag=21U,(*_tmp6B).f1=e)),(*_tmp6B).f2=n)),(*_tmp6B).f3=0)),(*_tmp6B).f4=0);_tmp6B;});Cyc_Absyn_new_exp(_tmp184,loc);});}
# 669
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp185=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));((((((((*_tmp6C).tag=22U,(*_tmp6C).f1=e)),(*_tmp6C).f2=n)),(*_tmp6C).f3=0)),(*_tmp6C).f4=0);_tmp6C;});Cyc_Absyn_new_exp(_tmp185,loc);});}
# 672
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp186=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));((((*_tmp6D).tag=23U,(*_tmp6D).f1=e1)),(*_tmp6D).f2=e2);_tmp6D;});Cyc_Absyn_new_exp(_tmp186,loc);});}
# 675
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp187=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));((*_tmp6E).tag=24U,(*_tmp6E).f1=es);_tmp6E;});Cyc_Absyn_new_exp(_tmp187,loc);});}
# 678
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){return({void*_tmp188=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));((*_tmp6F).tag=37U,(*_tmp6F).f1=s);_tmp6F;});Cyc_Absyn_new_exp(_tmp188,loc);});}
# 680
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 684
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
return({void*_tmp189=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));((*_tmp70).tag=39U,(*_tmp70).f1=t);_tmp70;});Cyc_Absyn_new_exp(_tmp189,loc);});}
# 687
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
return({void*_tmp18A=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));((((*_tmp71).tag=40U,(*_tmp71).f1=volatile_kw)),(*_tmp71).f2=body);_tmp71;});Cyc_Absyn_new_exp(_tmp18A,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 691
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 693
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
({struct Cyc_List_List*_tmp18C=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));(({struct _tuple13*_tmp18B=({struct _tuple13*_tmp72=_cycalloc(sizeof(*_tmp72));((*_tmp72).f1=0,(*_tmp72).f2=(struct Cyc_Absyn_Exp*)es->hd);_tmp72;});(*_tmp73).hd=_tmp18B;}),(*_tmp73).tl=dles);_tmp73;});dles=_tmp18C;});}
({struct Cyc_List_List*_tmp18D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);dles=_tmp18D;});
return({void*_tmp18E=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));((*_tmp74).tag=26U,(*_tmp74).f1=dles);_tmp74;});Cyc_Absyn_new_exp(_tmp18E,loc);});}
# 699
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 702
return({void*_tmp18F=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));((((*_tmp75).tag=36U,(*_tmp75).f1=n)),(*_tmp75).f2=dles);_tmp75;});Cyc_Absyn_new_exp(_tmp18F,loc);});}
# 705
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
return({struct Cyc_Absyn_Stmt*_tmp76=_cycalloc(sizeof(*_tmp76));((((*_tmp76).r=s,(*_tmp76).loc=loc)),(*_tmp76).annot=(void*)& Cyc_Absyn_EmptyAnnot_val);_tmp76;});}
# 708
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp190=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp77=_cycalloc(sizeof(*_tmp77));((*_tmp77).tag=1U,(*_tmp77).f1=e);_tmp77;});Cyc_Absyn_new_stmt(_tmp190,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp192=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp191=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp192,_tmp191,loc);});}}}
# 718
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp193=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp78=_cycalloc(sizeof(*_tmp78));((*_tmp78).tag=3U,(*_tmp78).f1=e);_tmp78;});Cyc_Absyn_new_stmt(_tmp193,loc);});}
# 721
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
return({void*_tmp194=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));((((((*_tmp79).tag=4U,(*_tmp79).f1=e)),(*_tmp79).f2=s1)),(*_tmp79).f3=s2);_tmp79;});Cyc_Absyn_new_stmt(_tmp194,loc);});}
# 724
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp196=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));((((*_tmp7A).tag=5U,((((*_tmp7A).f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp195=Cyc_Absyn_skip_stmt(e->loc);((*_tmp7A).f1).f2=_tmp195;}))))),(*_tmp7A).f2=s);_tmp7A;});Cyc_Absyn_new_stmt(_tmp196,loc);});}
# 727
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp199=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));((((((((*_tmp7B).tag=9U,(*_tmp7B).f1=e1)),((((*_tmp7B).f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp198=Cyc_Absyn_skip_stmt(e3->loc);((*_tmp7B).f2).f2=_tmp198;}))))),(
(((*_tmp7B).f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp197=Cyc_Absyn_skip_stmt(e3->loc);((*_tmp7B).f3).f2=_tmp197;}))))),(*_tmp7B).f4=s);_tmp7B;});
# 730
Cyc_Absyn_new_stmt(_tmp199,loc);});}
# 734
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp19A=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));((((((*_tmp7C).tag=10U,(*_tmp7C).f1=e)),(*_tmp7C).f2=scs)),(*_tmp7C).f3=0);_tmp7C;});Cyc_Absyn_new_stmt(_tmp19A,loc);});}struct _tuple14{void*f1;void*f2;};
# 737
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmp7D=({struct _tuple14 _tmp13C;(_tmp13C.f1=s1->r,_tmp13C.f2=s2->r);_tmp13C;});struct _tuple14 _tmp7E=_tmp7D;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7E.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7E.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp19B=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));((((*_tmp7F).tag=2U,(*_tmp7F).f1=s1)),(*_tmp7F).f2=s2);_tmp7F;});Cyc_Absyn_new_stmt(_tmp19B,loc);});}}_LL0:;}
# 744
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));((((*_tmp80).tag=11U,(*_tmp80).f1=el)),(*_tmp80).f2=0);_tmp80;});Cyc_Absyn_new_stmt(_tmp19C,loc);});}
# 747
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp19D=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp81=_cycalloc(sizeof(*_tmp81));((((*_tmp81).tag=12U,(*_tmp81).f1=d)),(*_tmp81).f2=s);_tmp81;});Cyc_Absyn_new_stmt(_tmp19D,loc);});}
# 750
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl*d=({void*_tmp19F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp83=_cycalloc(sizeof(*_tmp83));((*_tmp83).tag=0U,({struct Cyc_Absyn_Vardecl*_tmp19E=Cyc_Absyn_new_vardecl(0U,x,t,init);(*_tmp83).f1=_tmp19E;}));_tmp83;});Cyc_Absyn_new_decl(_tmp19F,loc);});
return({void*_tmp1A0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp82=_cycalloc(sizeof(*_tmp82));((((*_tmp82).tag=12U,(*_tmp82).f1=d)),(*_tmp82).f2=s);_tmp82;});Cyc_Absyn_new_stmt(_tmp1A0,loc);});}
# 754
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1A1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp84=_cycalloc(sizeof(*_tmp84));((((*_tmp84).tag=13U,(*_tmp84).f1=v)),(*_tmp84).f2=s);_tmp84;});Cyc_Absyn_new_stmt(_tmp1A1,loc);});}
# 757
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1A3=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp85=_cycalloc(sizeof(*_tmp85));((((*_tmp85).tag=14U,(*_tmp85).f1=s)),((((*_tmp85).f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1A2=Cyc_Absyn_skip_stmt(e->loc);((*_tmp85).f2).f2=_tmp1A2;}))));_tmp85;});Cyc_Absyn_new_stmt(_tmp1A3,loc);});}
# 760
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp1A4=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp86=_cycalloc(sizeof(*_tmp86));((((((*_tmp86).tag=15U,(*_tmp86).f1=s)),(*_tmp86).f2=scs)),(*_tmp86).f3=0);_tmp86;});Cyc_Absyn_new_stmt(_tmp1A4,loc);});}
# 763
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
return({void*_tmp1A5=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp87=_cycalloc(sizeof(*_tmp87));((*_tmp87).tag=8U,(*_tmp87).f1=lab);_tmp87;});Cyc_Absyn_new_stmt(_tmp1A5,loc);});}
# 766
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp1A6=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1A6,loc);});}
# 770
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){return({struct Cyc_Absyn_Pat*_tmp88=_cycalloc(sizeof(*_tmp88));((((*_tmp88).r=p,(*_tmp88).topt=0)),(*_tmp88).loc=s);_tmp88;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp89=_cycalloc(sizeof(*_tmp89));((*_tmp89).tag=17U,(*_tmp89).f1=e);_tmp89;});Cyc_Absyn_new_pat(_tmp1A7,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 776
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){return({struct Cyc_Absyn_Decl*_tmp8A=_cycalloc(sizeof(*_tmp8A));((*_tmp8A).r=r,(*_tmp8A).loc=loc);_tmp8A;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));((((((((*_tmp8B).tag=2U,(*_tmp8B).f1=p)),(*_tmp8B).f2=0)),(*_tmp8B).f3=e)),(*_tmp8B).f4=0);_tmp8B;});Cyc_Absyn_new_decl(_tmp1A8,loc);});}
# 780
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
return({void*_tmp1A9=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));((*_tmp8C).tag=3U,(*_tmp8C).f1=vds);_tmp8C;});Cyc_Absyn_new_decl(_tmp1A9,loc);});}
# 783
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned int loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));((((((*_tmp8D).tag=4U,(*_tmp8D).f1=tv)),(*_tmp8D).f2=vd)),(*_tmp8D).f3=open_exp);_tmp8D;});Cyc_Absyn_new_decl(_tmp1AA,loc);});}
# 786
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 788
return({void*_tmp1AD=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));((((((((*_tmp8F).tag=2U,({struct Cyc_Absyn_Pat*_tmp1AC=({void*_tmp1AB=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));((((*_tmp8E).tag=2U,(*_tmp8E).f1=tv)),(*_tmp8E).f2=vd);_tmp8E;});Cyc_Absyn_new_pat(_tmp1AB,loc);});(*_tmp8F).f1=_tmp1AC;}))),(*_tmp8F).f2=0)),(*_tmp8F).f3=e)),(*_tmp8F).f4=0);_tmp8F;});Cyc_Absyn_new_decl(_tmp1AD,loc);});}
# 791
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp90=_cycalloc(sizeof(*_tmp90));((((((((((((((((*_tmp90).sc=Cyc_Absyn_Public,(*_tmp90).name=x)),(*_tmp90).varloc=varloc)),({
struct Cyc_Absyn_Tqual _tmp1AE=Cyc_Absyn_empty_tqual(0U);(*_tmp90).tq=_tmp1AE;}))),(*_tmp90).type=t)),(*_tmp90).initializer=init)),(*_tmp90).rgn=0)),(*_tmp90).attributes=0)),(*_tmp90).escapes=0);_tmp90;});}
# 797
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp91=_cycalloc(sizeof(*_tmp91));((((((((((((((((*_tmp91).sc=Cyc_Absyn_Static,(*_tmp91).name=x)),(*_tmp91).varloc=0U)),({struct Cyc_Absyn_Tqual _tmp1AF=Cyc_Absyn_empty_tqual(0U);(*_tmp91).tq=_tmp1AF;}))),(*_tmp91).type=t)),(*_tmp91).initializer=init)),(*_tmp91).rgn=0)),(*_tmp91).attributes=0)),(*_tmp91).escapes=0);_tmp91;});}
# 802
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 806
return({struct Cyc_Absyn_AggrdeclImpl*_tmp92=_cycalloc(sizeof(*_tmp92));((((((*_tmp92).exist_vars=exists,(*_tmp92).rgn_po=po)),(*_tmp92).fields=fs)),(*_tmp92).tagged=tagged);_tmp92;});}
# 809
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 812
return({void*_tmp1B1=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp94=_cycalloc(sizeof(*_tmp94));((*_tmp94).tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp1B0=({struct Cyc_Absyn_Aggrdecl*_tmp93=_cycalloc(sizeof(*_tmp93));((((((((((((*_tmp93).kind=k,(*_tmp93).sc=s)),(*_tmp93).name=n)),(*_tmp93).tvs=ts)),(*_tmp93).impl=i)),(*_tmp93).attributes=atts)),(*_tmp93).expected_mem_kind=0);_tmp93;});(*_tmp94).f1=_tmp1B0;}));_tmp94;});Cyc_Absyn_new_decl(_tmp1B1,loc);});}
# 817
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 820
return({struct Cyc_Absyn_TypeDecl*_tmp97=_cycalloc(sizeof(*_tmp97));(({void*_tmp1B3=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp96=_cycalloc(sizeof(*_tmp96));((*_tmp96).tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp1B2=({struct Cyc_Absyn_Aggrdecl*_tmp95=_cycalloc(sizeof(*_tmp95));((((((((((((*_tmp95).kind=k,(*_tmp95).sc=s)),(*_tmp95).name=n)),(*_tmp95).tvs=ts)),(*_tmp95).impl=i)),(*_tmp95).attributes=atts)),(*_tmp95).expected_mem_kind=0);_tmp95;});(*_tmp96).f1=_tmp1B2;}));_tmp96;});(*_tmp97).r=_tmp1B3;}),(*_tmp97).loc=loc);_tmp97;});}
# 826
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 829
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 831
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 834
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 836
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 839
return({void*_tmp1B5=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp99=_cycalloc(sizeof(*_tmp99));((*_tmp99).tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp1B4=({struct Cyc_Absyn_Datatypedecl*_tmp98=_cycalloc(sizeof(*_tmp98));((((((((*_tmp98).sc=s,(*_tmp98).name=n)),(*_tmp98).tvs=ts)),(*_tmp98).fields=fs)),(*_tmp98).is_extensible=is_extensible);_tmp98;});(*_tmp99).f1=_tmp1B4;}));_tmp99;});Cyc_Absyn_new_decl(_tmp1B5,loc);});}
# 842
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 845
return({struct Cyc_Absyn_TypeDecl*_tmp9C=_cycalloc(sizeof(*_tmp9C));(({void*_tmp1B7=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));((*_tmp9B).tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp1B6=({struct Cyc_Absyn_Datatypedecl*_tmp9A=_cycalloc(sizeof(*_tmp9A));((((((((*_tmp9A).sc=s,(*_tmp9A).name=n)),(*_tmp9A).tvs=ts)),(*_tmp9A).fields=fs)),(*_tmp9A).is_extensible=is_extensible);_tmp9A;});(*_tmp9B).f1=_tmp1B6;}));_tmp9B;});(*_tmp9C).r=_tmp1B7;}),(*_tmp9C).loc=loc);_tmp9C;});}
# 857 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp9D=(*a).f3;
void*_tmp9E=Cyc_Absyn_pointer_expand(_tmp9D,1);
if(_tmp9D != _tmp9E)
(*a).f3=_tmp9E;}
# 864
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 871
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));((*_tmp9F).tag=9U,((((((((((((((((((((((((((*_tmp9F).f1).tvars=tvs,((*_tmp9F).f1).ret_tqual=ret_tqual)),({
# 874
void*_tmp1B8=Cyc_Absyn_pointer_expand(ret_typ,0);((*_tmp9F).f1).ret_typ=_tmp1B8;}))),((*_tmp9F).f1).effect=eff_typ)),((*_tmp9F).f1).args=args)),((*_tmp9F).f1).c_varargs=c_varargs)),((*_tmp9F).f1).cyc_varargs=cyc_varargs)),((*_tmp9F).f1).rgn_po=rgn_po)),((*_tmp9F).f1).attributes=atts)),((*_tmp9F).f1).requires_clause=req)),((*_tmp9F).f1).requires_relns=0)),((*_tmp9F).f1).ensures_clause=ens)),((*_tmp9F).f1).ensures_relns=0)));_tmp9F;});}
# 888
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpA0=Cyc_Tcutil_compress(t);void*_tmpA1=_tmpA0;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA1)->tag == 9U){_LL1: _LL2:
 return({void*_tmp1BB=t;void*_tmp1BA=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*_tmpA2));(*_tmpA2).v=& Cyc_Tcutil_rk;_tmpA2;}),0):(void*)& Cyc_Absyn_HeapRgn_val;
# 890
struct Cyc_Absyn_Tqual _tmp1B9=
# 894
Cyc_Absyn_empty_tqual(0U);
# 890
Cyc_Absyn_at_typ(_tmp1BB,_tmp1BA,_tmp1B9,Cyc_Absyn_false_conref);});}else{_LL3: _LL4:
# 895
 return t;}_LL0:;}
# 910 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpA3=e->r;void*_tmpA4=_tmpA3;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Vardecl*_tmpA8;struct Cyc_Absyn_Vardecl*_tmpA7;switch(*((int*)_tmpA4)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA4)->f1)){case 2U: _LL1: _LL2:
# 913
 return 0;case 1U: _LL3: _tmpA7=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA4)->f1)->f1;_LL4:
 _tmpA8=_tmpA7;goto _LL6;case 4U: _LL5: _tmpA8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA4)->f1)->f1;_LL6: {
# 916
void*_tmpA5=Cyc_Tcutil_compress(_tmpA8->type);void*_tmpA6=_tmpA5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA6)->tag == 8U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 920
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpA9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL10:
 return Cyc_Absyn_is_lvalue(_tmpA9);case 13U: _LL11: _tmpAA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL12:
 return Cyc_Absyn_is_lvalue(_tmpAA);case 12U: _LL13: _tmpAB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL14:
 return Cyc_Absyn_is_lvalue(_tmpAB);default: _LL15: _LL16:
 return 0;}_LL0:;}
# 931
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmpAC=fields;for(0;_tmpAC != 0;_tmpAC=_tmpAC->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmpAC->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmpAC->hd;}}
return 0;}
# 937
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 941
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 947
if(ts == 0)
return 0;
return(struct _tuple10*)ts->hd;}
# 952
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
# 971
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmpAD=a;struct _dyneither_ptr _tmpF0;struct _dyneither_ptr _tmpEF;int _tmpEE;int _tmpED;int _tmpEC;int _tmpEB;int _tmpEA;int _tmpE9;int _tmpE8;struct _dyneither_ptr _tmpE7;struct Cyc_Absyn_Exp*_tmpE6;int _tmpE5;switch(*((int*)_tmpAD)){case 0U: _LL1: _tmpE5=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpB0;_tmpB0.tag=1U;_tmpB0.f1=(unsigned long)_tmpE5;({void*_tmpAE[1U];_tmpAE[0]=& _tmpB0;({struct _dyneither_ptr _tmp1BC=({const char*_tmpAF="regparm(%d)";_tag_dyneither(_tmpAF,sizeof(char),12U);});Cyc_aprintf(_tmp1BC,_tag_dyneither(_tmpAE,sizeof(void*),1U));});});});case 1U: _LL3: _LL4:
 return({const char*_tmpB1="stdcall";_tag_dyneither(_tmpB1,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpB2="cdecl";_tag_dyneither(_tmpB2,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpB3="fastcall";_tag_dyneither(_tmpB3,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpB4="noreturn";_tag_dyneither(_tmpB4,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpB5="const";_tag_dyneither(_tmpB5,sizeof(char),6U);});case 6U: _LLD: _tmpE6=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LLE:
# 980
 if(_tmpE6 == 0)return({const char*_tmpB6="aligned";_tag_dyneither(_tmpB6,sizeof(char),8U);});else{
# 982
enum Cyc_Cyclone_C_Compilers _tmpB7=Cyc_Cyclone_c_compiler;if(_tmpB7 == Cyc_Cyclone_Gcc_c){_LL3A: _LL3B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpBA;_tmpBA.tag=0U;({struct _dyneither_ptr _tmp1BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmpE6));_tmpBA.f1=_tmp1BD;});({void*_tmpB8[1U];_tmpB8[0]=& _tmpBA;({struct _dyneither_ptr _tmp1BE=({const char*_tmpB9="aligned(%s)";_tag_dyneither(_tmpB9,sizeof(char),12U);});Cyc_aprintf(_tmp1BE,_tag_dyneither(_tmpB8,sizeof(void*),1U));});});});}else{_LL3C: _LL3D:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpBD;_tmpBD.tag=0U;({struct _dyneither_ptr _tmp1BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmpE6));_tmpBD.f1=_tmp1BF;});({void*_tmpBB[1U];_tmpBB[0]=& _tmpBD;({struct _dyneither_ptr _tmp1C0=({const char*_tmpBC="align(%s)";_tag_dyneither(_tmpBC,sizeof(char),10U);});Cyc_aprintf(_tmp1C0,_tag_dyneither(_tmpBB,sizeof(void*),1U));});});});}_LL39:;}case 7U: _LLF: _LL10:
# 986
 return({const char*_tmpBE="packed";_tag_dyneither(_tmpBE,sizeof(char),7U);});case 8U: _LL11: _tmpE7=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC1;_tmpC1.tag=0U;_tmpC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE7);({void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({struct _dyneither_ptr _tmp1C1=({const char*_tmpC0="section(\"%s\")";_tag_dyneither(_tmpC0,sizeof(char),14U);});Cyc_aprintf(_tmp1C1,_tag_dyneither(_tmpBF,sizeof(void*),1U));});});});case 9U: _LL13: _LL14:
 return({const char*_tmpC2="nocommon";_tag_dyneither(_tmpC2,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpC3="shared";_tag_dyneither(_tmpC3,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpC4="unused";_tag_dyneither(_tmpC4,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpC5="weak";_tag_dyneither(_tmpC5,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpC6="dllimport";_tag_dyneither(_tmpC6,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpC7="dllexport";_tag_dyneither(_tmpC7,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpC8="no_instrument_function";_tag_dyneither(_tmpC8,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpC9="constructor";_tag_dyneither(_tmpC9,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpCA="destructor";_tag_dyneither(_tmpCA,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpCB="no_check_memory_usage";_tag_dyneither(_tmpCB,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAD)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpE9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAD)->f2;_tmpE8=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAD)->f3;_LL28:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpCF;_tmpCF.tag=1U;_tmpCF.f1=(unsigned int)_tmpE8;({struct Cyc_Int_pa_PrintArg_struct _tmpCE;_tmpCE.tag=1U;_tmpCE.f1=(unsigned int)_tmpE9;({void*_tmpCC[2U];(_tmpCC[0]=& _tmpCE,_tmpCC[1]=& _tmpCF);({struct _dyneither_ptr _tmp1C2=({const char*_tmpCD="format(printf,%u,%u)";_tag_dyneither(_tmpCD,sizeof(char),21U);});Cyc_aprintf(_tmp1C2,_tag_dyneither(_tmpCC,sizeof(void*),2U));});});});});}else{_LL29: _tmpEB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAD)->f2;_tmpEA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAD)->f3;_LL2A:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD3;_tmpD3.tag=1U;_tmpD3.f1=(unsigned int)_tmpEA;({struct Cyc_Int_pa_PrintArg_struct _tmpD2;_tmpD2.tag=1U;_tmpD2.f1=(unsigned int)_tmpEB;({void*_tmpD0[2U];(_tmpD0[0]=& _tmpD2,_tmpD0[1]=& _tmpD3);({struct _dyneither_ptr _tmp1C3=({const char*_tmpD1="format(scanf,%u,%u)";_tag_dyneither(_tmpD1,sizeof(char),20U);});Cyc_aprintf(_tmp1C3,_tag_dyneither(_tmpD0,sizeof(void*),2U));});});});});}case 20U: _LL2B: _tmpEC=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL2C:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD6;_tmpD6.tag=1U;_tmpD6.f1=(unsigned long)_tmpEC;({void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({struct _dyneither_ptr _tmp1C4=({const char*_tmpD5="initializes(%d)";_tag_dyneither(_tmpD5,sizeof(char),16U);});Cyc_aprintf(_tmp1C4,_tag_dyneither(_tmpD4,sizeof(void*),1U));});});});case 21U: _LL2D: _tmpED=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL2E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD9;_tmpD9.tag=1U;_tmpD9.f1=(unsigned long)_tmpED;({void*_tmpD7[1U];_tmpD7[0]=& _tmpD9;({struct _dyneither_ptr _tmp1C5=({const char*_tmpD8="noliveunique(%d)";_tag_dyneither(_tmpD8,sizeof(char),17U);});Cyc_aprintf(_tmp1C5,_tag_dyneither(_tmpD7,sizeof(void*),1U));});});});case 22U: _LL2F: _tmpEE=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL30:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDC;_tmpDC.tag=1U;_tmpDC.f1=(unsigned long)_tmpEE;({void*_tmpDA[1U];_tmpDA[0]=& _tmpDC;({struct _dyneither_ptr _tmp1C6=({const char*_tmpDB="noconsume(%d)";_tag_dyneither(_tmpDB,sizeof(char),14U);});Cyc_aprintf(_tmp1C6,_tag_dyneither(_tmpDA,sizeof(void*),1U));});});});case 23U: _LL31: _LL32:
 return({const char*_tmpDD="pure";_tag_dyneither(_tmpDD,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmpDE="always_inline";_tag_dyneither(_tmpDE,sizeof(char),14U);});case 24U: _LL35: _tmpEF=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL36:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0U;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEF);({void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _dyneither_ptr _tmp1C7=({const char*_tmpE0="__mode__(\"%s\")";_tag_dyneither(_tmpE0,sizeof(char),15U);});Cyc_aprintf(_tmp1C7,_tag_dyneither(_tmpDF,sizeof(void*),1U));});});});default: _LL37: _tmpF0=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpAD)->f1;_LL38:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE4;_tmpE4.tag=0U;_tmpE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF0);({void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({struct _dyneither_ptr _tmp1C8=({const char*_tmpE3="alias(\"%s\")";_tag_dyneither(_tmpE3,sizeof(char),12U);});Cyc_aprintf(_tmp1C8,_tag_dyneither(_tmpE2,sizeof(void*),1U));});});});}_LL0:;}
# 1011
int Cyc_Absyn_fntype_att(void*a){
void*_tmpF1=a;int _tmpF2;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmpF2=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpF1)->f1;_LL2:
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
 return 0;}_LL0:;}static char _tmpF3[3U]="f0";
# 1029
static struct _dyneither_ptr Cyc_Absyn_f0={_tmpF3,_tmpF3,_tmpF3 + 3U};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1U]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1U)};
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if(i >= fsz)
({struct _dyneither_ptr _tmp1CC=({unsigned int _tmpF9=(unsigned int)(i + 1);struct _dyneither_ptr**_tmpF8=_cycalloc(_check_times(_tmpF9,sizeof(struct _dyneither_ptr*)));({{unsigned int _tmp13D=(unsigned int)(i + 1);unsigned int j;for(j=0;j < _tmp13D;++ j){
# 1037
j < fsz?_tmpF8[j]=*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp1CB=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp1CA=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6;_tmpF6.tag=1U;_tmpF6.f1=(unsigned long)((int)j);({void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp1C9=({const char*_tmpF5="f%d";_tag_dyneither(_tmpF5,sizeof(char),4U);});Cyc_aprintf(_tmp1C9,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});});*_tmpF7=_tmp1CA;});_tmpF7;});_tmpF8[j]=_tmp1CB;});}}0;});_tag_dyneither(_tmpF8,sizeof(struct _dyneither_ptr*),_tmpF9);});
# 1035
Cyc_Absyn_field_names=_tmp1CC;});
# 1038
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1041
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmpFA=info;enum Cyc_Absyn_AggrKind _tmpFE;struct _tuple0*_tmpFD;enum Cyc_Absyn_AggrKind _tmpFC;struct _tuple0*_tmpFB;if((_tmpFA.UnknownAggr).tag == 1){_LL1: _tmpFC=((_tmpFA.UnknownAggr).val).f1;_tmpFB=((_tmpFA.UnknownAggr).val).f2;_LL2:
 return({struct _tuple11 _tmp13E;(_tmp13E.f1=_tmpFC,_tmp13E.f2=_tmpFB);_tmp13E;});}else{_LL3: _tmpFE=(*(_tmpFA.KnownAggr).val)->kind;_tmpFD=(*(_tmpFA.KnownAggr).val)->name;_LL4:
 return({struct _tuple11 _tmp13F;(_tmp13F.f1=_tmpFE,_tmp13F.f2=_tmpFD);_tmp13F;});}_LL0:;}
# 1047
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmpFF=info;struct Cyc_Absyn_Aggrdecl*_tmp102;if((_tmpFF.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmp100=0U;({struct _dyneither_ptr _tmp1CD=({const char*_tmp101="unchecked aggrdecl";_tag_dyneither(_tmp101,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp1CD,_tag_dyneither(_tmp100,sizeof(void*),0U));});});}else{_LL3: _tmp102=*(_tmpFF.KnownAggr).val;_LL4:
 return _tmp102;}_LL0:;}
# 1053
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp103=Cyc_Tcutil_compress(t);void*_tmp104=_tmp103;union Cyc_Absyn_AggrInfoU _tmp10D;struct Cyc_List_List*_tmp10C;switch(*((int*)_tmp104)){case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp104)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp10C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp104)->f2;_LL2:
# 1056
 if(_tmp10C == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp10C->hd)->requires_clause == 0;}else{goto _LL5;}case 11U: _LL3: _tmp10D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp104)->f1).aggr_info;_LL4: {
# 1059
union Cyc_Absyn_AggrInfoU _tmp105=_tmp10D;enum Cyc_Absyn_AggrKind _tmp10B;int _tmp10A;enum Cyc_Absyn_AggrKind _tmp109;struct Cyc_Absyn_Aggrdecl*_tmp108;if((_tmp105.KnownAggr).tag == 2){_LL8: _tmp108=*(_tmp105.KnownAggr).val;_LL9:
# 1061
 if(_tmp108->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp106=_tmp108->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp106))->tagged)return 0;{
struct Cyc_List_List*_tmp107=_tmp106->fields;
if(_tmp107 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp107->hd)->requires_clause == 0;};};}else{if(((_tmp105.UnknownAggr).val).f3 == 0){_LLA: _tmp109=((_tmp105.UnknownAggr).val).f1;_LLB:
 return _tmp109 == Cyc_Absyn_UnionA;}else{_LLC: _tmp10B=((_tmp105.UnknownAggr).val).f1;_tmp10A=(int)(((_tmp105.UnknownAggr).val).f3)->v;_LLD:
 return _tmp10B == Cyc_Absyn_UnionA  && !_tmp10A;}}_LL7:;}default: _LL5: _LL6:
# 1070
 return 0;}_LL0:;}
# 1073
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp10E=Cyc_Tcutil_compress(t);void*_tmp10F=_tmp10E;union Cyc_Absyn_AggrInfoU _tmp118;struct Cyc_List_List*_tmp117;switch(*((int*)_tmp10F)){case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10F)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp117=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10F)->f2;_LL2:
# 1076
 if(_tmp117 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp117->hd)->requires_clause != 0;}else{goto _LL5;}case 11U: _LL3: _tmp118=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp10F)->f1).aggr_info;_LL4: {
# 1079
union Cyc_Absyn_AggrInfoU _tmp110=_tmp118;enum Cyc_Absyn_AggrKind _tmp116;int _tmp115;enum Cyc_Absyn_AggrKind _tmp114;struct Cyc_Absyn_Aggrdecl*_tmp113;if((_tmp110.KnownAggr).tag == 2){_LL8: _tmp113=*(_tmp110.KnownAggr).val;_LL9:
# 1081
 if(_tmp113->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp111=_tmp113->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp111))->tagged)return 0;{
struct Cyc_List_List*_tmp112=_tmp111->fields;
if(_tmp112 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->requires_clause != 0;};};}else{if(((_tmp110.UnknownAggr).val).f3 == 0){_LLA: _tmp114=((_tmp110.UnknownAggr).val).f1;_LLB:
 return 0;}else{_LLC: _tmp116=((_tmp110.UnknownAggr).val).f1;_tmp115=(int)(((_tmp110.UnknownAggr).val).f3)->v;_LLD:
 return 0;}}_LL7:;}default: _LL5: _LL6:
# 1090
 return 0;}_LL0:;}
# 1094
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp119=b;struct Cyc_Absyn_Fndecl*_tmp11F;struct Cyc_Absyn_Vardecl*_tmp11E;struct Cyc_Absyn_Vardecl*_tmp11D;struct Cyc_Absyn_Vardecl*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11B;struct _tuple0*_tmp11A;switch(*((int*)_tmp119)){case 0U: _LL1: _tmp11A=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp119)->f1;_LL2:
 return _tmp11A;case 1U: _LL3: _tmp11B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp119)->f1;_LL4:
 _tmp11C=_tmp11B;goto _LL6;case 3U: _LL5: _tmp11C=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp119)->f1;_LL6:
 _tmp11D=_tmp11C;goto _LL8;case 4U: _LL7: _tmp11D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp119)->f1;_LL8:
 _tmp11E=_tmp11D;goto _LLA;case 5U: _LL9: _tmp11E=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp119)->f1;_LLA:
 return _tmp11E->name;default: _LLB: _tmp11F=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp119)->f1;_LLC:
 return _tmp11F->name;}_LL0:;}
# 1105
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0  || ds->tl != 0)
({void*_tmp120=0U;({struct _dyneither_ptr _tmp1CE=({const char*_tmp121="designator list not of length 1";_tag_dyneither(_tmp121,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp1CE,_tag_dyneither(_tmp120,sizeof(void*),0U));});});{
void*_tmp122=(void*)ds->hd;void*_tmp123=_tmp122;struct _dyneither_ptr*_tmp126;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp123)->tag == 1U){_LL1: _tmp126=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp123)->f1;_LL2:
 return _tmp126;}else{_LL3: _LL4:
({void*_tmp124=0U;({struct _dyneither_ptr _tmp1CF=({const char*_tmp125="array designator in struct";_tag_dyneither(_tmp125,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp1CF,_tag_dyneither(_tmp124,sizeof(void*),0U));});});}_LL0:;};}
# 1114
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
# 1117
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;

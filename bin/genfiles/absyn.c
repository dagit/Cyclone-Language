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

# 35 "core.h"
 typedef char*Cyc_Cstring;
typedef char*Cyc_CstringNN;
typedef struct _dyneither_ptr Cyc_string_t;
# 40
typedef struct _dyneither_ptr Cyc_mstring_t;
# 43
typedef struct _dyneither_ptr*Cyc_stringptr_t;
# 47
typedef struct _dyneither_ptr*Cyc_mstringptr_t;
# 50
typedef char*Cyc_Cbuffer_t;
# 52
typedef char*Cyc_CbufferNN_t;
# 54
typedef struct _dyneither_ptr Cyc_buffer_t;
# 56
typedef struct _dyneither_ptr Cyc_mbuffer_t;
# 59
typedef int Cyc_bool;
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 74
typedef void*Cyc_Tcpat_decision_opt_t;
# 81
typedef unsigned int Cyc_Absyn_seg_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 107
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
typedef void*Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
typedef void*Cyc_Absyn_designator_t;
typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 221
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
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
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
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
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
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
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 893
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 915
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 920
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 923
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 932
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 937
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 943
void*Cyc_Absyn_compress_kb(void*);
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 950
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 953
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 955
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 957
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 959
extern void*Cyc_Absyn_empty_effect;
# 961
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_exn_typ();
# 965
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
# 968
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 971
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_bounds_one;
# 974
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 977
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 980
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 983
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 986
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 988
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 991
void*Cyc_Absyn_void_star_typ();
# 993
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 997
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);
# 999
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1003
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
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1071
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1074
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
# 1091
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1098
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1102
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1116
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1119
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1122
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1125
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1130
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1136
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1148
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1150
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1153
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1157
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1159
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1161
int Cyc_Absyn_fntype_att(void*a);
# 1163
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1165
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1167
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1169
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1171
int Cyc_Absyn_is_require_union_type(void*);
# 1173
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1176
extern int Cyc_Absyn_porting_c_code;
# 1178
extern int Cyc_Absyn_no_regions;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 129
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
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
struct _tuple12 _tmp2=({struct _tuple12 _tmpA;_tmpA.f1=_tmpE;_tmpA.f2=_tmpC;_tmpA;});struct _tuple12 _tmp3=_tmp2;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp4;switch(((_tmp3.f1).Abs_n).tag){case 4: if(((_tmp3.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 71
 return - 1;}case 1: switch(((_tmp3.f2).Loc_n).tag){case 1: _LL9: _tmp5=((_tmp3.f1).Rel_n).val;_tmp4=((_tmp3.f2).Rel_n).val;_LLA:
# 67
 return Cyc_Absyn_strlist_cmp(_tmp5,_tmp4);case 4: goto _LL11;default: _LL13: _LL14:
# 73
 return - 1;}case 2: switch(((_tmp3.f2).Rel_n).tag){case 2: _LLB: _tmp7=((_tmp3.f1).Abs_n).val;_tmp6=((_tmp3.f2).Abs_n).val;_LLC:
# 68
 return Cyc_Absyn_strlist_cmp(_tmp7,_tmp6);case 4: goto _LL11;case 1: goto _LL15;default: _LL17: _LL18:
# 75
 return - 1;}default: switch(((_tmp3.f2).Rel_n).tag){case 3: _LLD: _tmp9=((_tmp3.f1).C_n).val;_tmp8=((_tmp3.f2).C_n).val;_LLE:
# 69
 return Cyc_Absyn_strlist_cmp(_tmp9,_tmp8);case 4: _LL11: _LL12:
# 72
 return 1;case 1: _LL15: _LL16:
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
union Cyc_Absyn_Nmspace _tmp12=(*qv).f1;union Cyc_Absyn_Nmspace _tmp13=_tmp12;switch((_tmp13.Loc_n).tag){case 1: if((_tmp13.Rel_n).val == 0){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL22;}case 2: if((_tmp13.Abs_n).val == 0){_LL1E: _LL1F:
 goto _LL21;}else{goto _LL22;}case 4: _LL20: _LL21:
 return 0;default: _LL22: _LL23:
 return 1;}_LL1B:;}
# 105
static int Cyc_Absyn_new_type_counter=0;
# 107
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14=_cycalloc(sizeof(*_tmp14));({struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp1D3=({struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp15;_tmp15.tag=1;_tmp15.f1=k;_tmp15.f2=0;_tmp15.f3=Cyc_Absyn_new_type_counter ++;_tmp15.f4=env;_tmp15;});_tmp14[0]=_tmp1D3;});_tmp14;});}
# 110
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 114
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp16;_tmp16.print_const=x.print_const  || y.print_const;_tmp16.q_volatile=
x.q_volatile  || y.q_volatile;_tmp16.q_restrict=
x.q_restrict  || y.q_restrict;_tmp16.real_const=
x.real_const  || y.real_const;({unsigned int _tmp1D4=
Cyc_Position_segment_join(x.loc,y.loc);_tmp16.loc=_tmp1D4;});_tmp16;});}
# 122
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp17;_tmp17.print_const=0;_tmp17.q_volatile=0;_tmp17.q_restrict=0;_tmp17.real_const=0;_tmp17.loc=loc;_tmp17;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp18;_tmp18.print_const=1;_tmp18.q_volatile=0;_tmp18.q_restrict=0;_tmp18.real_const=1;_tmp18.loc=loc;_tmp18;});}
# 125
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 129
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Absyn_one_b_raw={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 131
struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct Cyc_Absyn_one_bt={1,& Cyc_Absyn_exp_unsigned_one_v};
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
return({union Cyc_Absyn_DatatypeFieldInfoU _tmp1F;({struct _tuple1 _tmp1D5=({struct _tuple1 _tmp20;_tmp20.f1=dd;_tmp20.f2=df;_tmp20;});(_tmp1F.KnownDatatypefield).val=_tmp1D5;});(_tmp1F.KnownDatatypefield).tag=2;_tmp1F;});}
# 148
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfoU _tmp21;({struct _tuple2 _tmp1D6=({struct _tuple2 _tmp22;_tmp22.f1=ak;_tmp22.f2=n;_tmp22.f3=tagged;_tmp22;});(_tmp21.UnknownAggr).val=_tmp1D6;});(_tmp21.UnknownAggr).tag=1;_tmp21;});}
# 151
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfoU _tmp23;(_tmp23.KnownAggr).val=ad;(_tmp23.KnownAggr).tag=2;_tmp23;});}
# 155
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){return({union Cyc_Absyn_Constraint*_tmp24=_cycalloc(sizeof(*_tmp24));(_tmp24->Eq_constr).val=x;(_tmp24->Eq_constr).tag=1;_tmp24;});}
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){return({union Cyc_Absyn_Constraint*_tmp25=_cycalloc_atomic(sizeof(*_tmp25));(_tmp25->No_constr).val=0;(_tmp25->No_constr).tag=3;_tmp25;});}
# 162
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0};
# 164
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
# 171
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp27=x;union Cyc_Absyn_Constraint*_tmp2A;switch((((union Cyc_Absyn_Constraint*)_tmp27)->Eq_constr).tag){case 3: _LL25: _LL26:
 goto _LL28;case 1: _LL27: _LL28:
 return x;default: _LL29: _tmp2A=(_tmp27->Forward_constr).val;_LL2A: {
# 176
union Cyc_Absyn_Constraint*_tmp28=Cyc_Absyn_compress_conref(_tmp2A);
({struct _union_Constraint_Forward_constr*_tmp29=& x->Forward_constr;_tmp29->tag=2;_tmp29->val=_tmp28;});
return _tmp28;}}_LL24:;}
# 182
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp2B=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint*_tmp2C=_tmp2B;void*_tmp2F;if((((union Cyc_Absyn_Constraint*)_tmp2C)->Eq_constr).tag == 1){_LL2C: _tmp2F=(void*)(_tmp2C->Eq_constr).val;_LL2D:
 return _tmp2F;}else{_LL2E: _LL2F:
({void*_tmp2D=0;({struct _dyneither_ptr _tmp1D7=({const char*_tmp2E="conref_val";_tag_dyneither(_tmp2E,sizeof(char),11);});Cyc_Tcutil_impos(_tmp1D7,_tag_dyneither(_tmp2D,sizeof(void*),0));});});}_LL2B:;}
# 189
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp30=x;void*_tmp31;switch((((union Cyc_Absyn_Constraint*)_tmp30)->No_constr).tag){case 1: _LL31: _tmp31=(void*)(_tmp30->Eq_constr).val;_LL32:
 return _tmp31;case 3: _LL33: _LL34:
 return y;default: _LL35: _LL36:
 return({void*_tmp1D8=y;Cyc_Absyn_conref_def(_tmp1D8,Cyc_Absyn_compress_conref(x));});}_LL30:;}
# 197
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){
({union Cyc_Absyn_Constraint*_tmp1D9=Cyc_Absyn_compress_conref(x);x=_tmp1D9;});{
union Cyc_Absyn_Constraint*_tmp32=x;void*_tmp34;if((((union Cyc_Absyn_Constraint*)_tmp32)->Eq_constr).tag == 1){_LL38: _tmp34=(void*)(_tmp32->Eq_constr).val;_LL39:
 return _tmp34;}else{_LL3A: _LL3B:
({struct _union_Constraint_Eq_constr*_tmp33=& x->Eq_constr;_tmp33->tag=1;_tmp33->val=y;});return y;}_LL37:;};}
# 205
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp35=k;void**_tmp37;void**_tmp36;switch(*((int*)_tmp35)){case 0: _LL3D: _LL3E:
 goto _LL40;case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35)->f1 == 0){_LL3F: _LL40:
 goto _LL42;}else{_LL43: _tmp36=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35)->f1)->v;_LL44:
# 210
 _tmp37=_tmp36;goto _LL46;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35)->f1 == 0){_LL41: _LL42:
# 209
 return k;}else{_LL45: _tmp37=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35)->f1)->v;_LL46:
# 212
({void*_tmp1DA=Cyc_Absyn_compress_kb(*_tmp37);*_tmp37=_tmp1DA;});
return*_tmp37;}}_LL3C:;}
# 216
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp38=Cyc_Absyn_compress_kb(kb);void*_tmp39=_tmp38;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Absyn_Kind*_tmp3D;struct Cyc_Core_Opt**_tmp3C;struct Cyc_Absyn_Kind*_tmp3B;switch(*((int*)_tmp39)){case 0: _LL48: _tmp3B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp39)->f1;_LL49:
 return _tmp3B;case 1: _LL4A: _tmp3C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp39)->f1;_LL4B:
 _tmp3E=_tmp3C;_tmp3D=& Cyc_Tcutil_bk;goto _LL4D;default: _LL4C: _tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp39)->f1;_tmp3D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp39)->f2;_LL4D:
# 221
({struct Cyc_Core_Opt*_tmp1DC=({struct Cyc_Core_Opt*_tmp3A=_cycalloc(sizeof(*_tmp3A));({void*_tmp1DB=Cyc_Tcutil_kind_to_bound(_tmp3D);_tmp3A->v=_tmp1DB;});_tmp3A;});*_tmp3E=_tmp1DC;});
return _tmp3D;}_LL47:;}
# 227
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={20};
struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={21};
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={22};
struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0};
# 233
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_char_tt={6,Cyc_Absyn_None,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uchar_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ushort_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uint_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulonglong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;
void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;
# 246
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_schar_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sshort_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sint_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slonglong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;
void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
# 257
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nshort_tt={6,Cyc_Absyn_None,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nint_tt={6,Cyc_Absyn_None,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlong_tt={6,Cyc_Absyn_None,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlonglong_tt={6,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_nshort_typ=(void*)& Cyc_Absyn_nshort_tt;
void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;
void*Cyc_Absyn_nlonglong_typ=(void*)& Cyc_Absyn_nlonglong_tt;
# 266
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp52=sn;switch(_tmp52){case Cyc_Absyn_Signed: _LL4F: _LL50: {
# 269
enum Cyc_Absyn_Size_of _tmp53=sz;switch(_tmp53){case Cyc_Absyn_Char_sz: _LL56: _LL57:
 return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LL58: _LL59:
 return Cyc_Absyn_sshort_typ;case Cyc_Absyn_Int_sz: _LL5A: _LL5B:
 return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LL5C: _LL5D:
 return Cyc_Absyn_slong_typ;default: _LL5E: _LL5F:
 return Cyc_Absyn_slonglong_typ;}_LL55:;}case Cyc_Absyn_Unsigned: _LL51: _LL52: {
# 277
enum Cyc_Absyn_Size_of _tmp54=sz;switch(_tmp54){case Cyc_Absyn_Char_sz: _LL61: _LL62:
 return Cyc_Absyn_uchar_typ;case Cyc_Absyn_Short_sz: _LL63: _LL64:
 return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz: _LL65: _LL66:
 return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL67: _LL68:
 return Cyc_Absyn_ulong_typ;default: _LL69: _LL6A:
 return Cyc_Absyn_ulonglong_typ;}_LL60:;}default: _LL53: _LL54: {
# 285
enum Cyc_Absyn_Size_of _tmp55=sz;switch(_tmp55){case Cyc_Absyn_Char_sz: _LL6C: _LL6D:
 return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz: _LL6E: _LL6F:
 return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL70: _LL71:
 return Cyc_Absyn_nint_typ;case Cyc_Absyn_Long_sz: _LL72: _LL73:
 return Cyc_Absyn_nlong_typ;default: _LL74: _LL75:
 return Cyc_Absyn_nlonglong_typ;}_LL6B:;}}_LL4E:;}
# 295
void*Cyc_Absyn_float_typ(int i){
static struct Cyc_Absyn_FloatType_Absyn_Type_struct fl={7,0};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct db={7,1};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct ldb={7,2};
if(i == 0)return(void*)& fl;
if(i == 1)return(void*)& db;
return(void*)& ldb;}
# 306
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 309
void*Cyc_Absyn_wchar_typ(){
int _tmp59= Sizeof_wchar_t;switch(_tmp59){case 1: _LL77: _LL78:
# 320 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;case 2: _LL79: _LL7A:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;default: _LL7B: _LL7C:
# 324
 return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}_LL76:;}static char _tmp5A[4]="exn";
# 328
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp5A,_tmp5A,_tmp5A + 4};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5B[15]="Null_Exception";
# 340 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5B,_tmp5B,_tmp5B + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp5C[13]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp5C,_tmp5C,_tmp5C + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp5D[16]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp5D,_tmp5D,_tmp5D + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp5E[10]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp5E,_tmp5E,_tmp5E + 10};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
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
static struct Cyc_Absyn_DatatypeType_Absyn_Type_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={2,& Cyc_Absyn_exn_tud}},0}};
# 359
void*Cyc_Absyn_exn_typ(){
static struct Cyc_Core_Opt*exn_type_val=0;
if(exn_type_val == 0)
({struct Cyc_Core_Opt*_tmp1DF=({struct Cyc_Core_Opt*_tmp60=_cycalloc(sizeof(*_tmp60));({void*_tmp1DE=({struct Cyc_Absyn_Tqual _tmp1DD=
Cyc_Absyn_empty_tqual(0);
# 362
Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1DD,Cyc_Absyn_false_conref);});_tmp60->v=_tmp1DE;});_tmp60;});exn_type_val=_tmp1DF;});
# 365
return(void*)exn_type_val->v;}static char _tmp61[9]="PrintArg";
# 369
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp61,_tmp61,_tmp61 + 9};static char _tmp62[9]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp62,_tmp62,_tmp62 + 9};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 373
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 375
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp63[14]="unique_region";
# 383
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp63,_tmp63,_tmp63 + 14};static char _tmp64[5]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp64,_tmp64,_tmp64 + 5};
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)& Cyc_Absyn_Core_str,0};
# 387
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 389
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 391
static struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct Cyc_Absyn_uniquergn_typ={15,(void*)& Cyc_Absyn_UniqueRgn_val};
# 394
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,0,{0,0,0,0,0},(void*)& Cyc_Absyn_uniquergn_typ,0,0,0,0};
# 397
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1,& Cyc_Absyn_uniquergn_var_d};
# 399
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1,(void*)& Cyc_Absyn_uniquergn_bind_p};
# 401
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergn_typ,(void*)& Cyc_Absyn_uniquergn_exp_r,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 404
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 407
static void**Cyc_Absyn_string_t_opt=0;
void*Cyc_Absyn_string_typ(void*rgn){
void*_tmp68=Cyc_Tcutil_compress(rgn);void*_tmp69=_tmp68;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp69)->tag == 20){_LL7E: _LL7F:
# 411
 if(Cyc_Absyn_string_t_opt == 0){
void*t=({void*_tmp1E1=Cyc_Absyn_char_typ;struct Cyc_Absyn_Tqual _tmp1E0=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_starb_typ(_tmp1E1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1E0,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 414
({void**_tmp1E2=({void**_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=t;_tmp6A;});Cyc_Absyn_string_t_opt=_tmp1E2;});}
# 416
return*((void**)_check_null(Cyc_Absyn_string_t_opt));}else{_LL80: _LL81:
# 418
 return({void*_tmp1E5=Cyc_Absyn_char_typ;void*_tmp1E4=rgn;struct Cyc_Absyn_Tqual _tmp1E3=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_starb_typ(_tmp1E5,_tmp1E4,_tmp1E3,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});}_LL7D:;}
# 421
static void**Cyc_Absyn_const_string_t_opt=0;
void*Cyc_Absyn_const_string_typ(void*rgn){
void*_tmp6B=Cyc_Tcutil_compress(rgn);void*_tmp6C=_tmp6B;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6C)->tag == 20){_LL83: _LL84:
# 425
 if(Cyc_Absyn_const_string_t_opt == 0){
void*t=({void*_tmp1E7=Cyc_Absyn_char_typ;struct Cyc_Absyn_Tqual _tmp1E6=Cyc_Absyn_const_tqual(0);Cyc_Absyn_starb_typ(_tmp1E7,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1E6,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 428
({void**_tmp1E8=({void**_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=t;_tmp6D;});Cyc_Absyn_const_string_t_opt=_tmp1E8;});}
# 430
return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}else{_LL85: _LL86:
# 432
 return({void*_tmp1EB=Cyc_Absyn_char_typ;void*_tmp1EA=rgn;struct Cyc_Absyn_Tqual _tmp1E9=Cyc_Absyn_const_tqual(0);Cyc_Absyn_starb_typ(_tmp1EB,_tmp1EA,_tmp1E9,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});}_LL82:;}
# 437
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 439
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1EF=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6F;_tmp6F.tag=5;({struct Cyc_Absyn_PtrInfo _tmp1EE=({struct Cyc_Absyn_PtrInfo _tmp70;_tmp70.elt_typ=t;_tmp70.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1ED=({(_tmp70.ptr_atts).rgn=r;(_tmp70.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp1EC=
# 441
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);(_tmp70.ptr_atts).bounds=_tmp1EC;});(_tmp70.ptr_atts).zero_term=zeroterm;(_tmp70.ptr_atts).ptrloc=0;_tmp70.ptr_atts;});_tmp70.ptr_atts=_tmp1ED;});_tmp70;});_tmp6F.f1=_tmp1EE;});_tmp6F;});_tmp6E[0]=_tmp1EF;});_tmp6E;});}
# 445
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 447
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp72;_tmp72.tag=5;({struct Cyc_Absyn_PtrInfo _tmp1F2=({struct Cyc_Absyn_PtrInfo _tmp73;_tmp73.elt_typ=t;_tmp73.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1F1=({(_tmp73.ptr_atts).rgn=r;(_tmp73.ptr_atts).nullable=Cyc_Absyn_false_conref;({union Cyc_Absyn_Constraint*_tmp1F0=
# 449
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);(_tmp73.ptr_atts).bounds=_tmp1F0;});(_tmp73.ptr_atts).zero_term=zeroterm;(_tmp73.ptr_atts).ptrloc=0;_tmp73.ptr_atts;});_tmp73.ptr_atts=_tmp1F1;});_tmp73;});_tmp72.f1=_tmp1F2;});_tmp72;});_tmp71[0]=_tmp1F3;});_tmp71;});}
# 454
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F6=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp75;_tmp75.tag=5;({struct Cyc_Absyn_PtrInfo _tmp1F5=({struct Cyc_Absyn_PtrInfo _tmp76;_tmp76.elt_typ=t;_tmp76.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1F4=({(_tmp76.ptr_atts).rgn=r;(_tmp76.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp76.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp76.ptr_atts).zero_term=zeroterm;(_tmp76.ptr_atts).ptrloc=0;_tmp76.ptr_atts;});_tmp76.ptr_atts=_tmp1F4;});_tmp76;});_tmp75.f1=_tmp1F5;});_tmp75;});_tmp74[0]=_tmp1F6;});_tmp74;});}
# 461
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F9=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp78;_tmp78.tag=5;({struct Cyc_Absyn_PtrInfo _tmp1F8=({struct Cyc_Absyn_PtrInfo _tmp79;_tmp79.elt_typ=t;_tmp79.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1F7=({(_tmp79.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val;(_tmp79.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp79.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp79.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp79.ptr_atts).ptrloc=0;_tmp79.ptr_atts;});_tmp79.ptr_atts=_tmp1F7;});_tmp79;});_tmp78.f1=_tmp1F8;});_tmp78;});_tmp77[0]=_tmp1F9;});_tmp77;});}
# 469
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FC=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7B;_tmp7B.tag=5;({struct Cyc_Absyn_PtrInfo _tmp1FB=({struct Cyc_Absyn_PtrInfo _tmp7C;_tmp7C.elt_typ=t;_tmp7C.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1FA=({(_tmp7C.ptr_atts).rgn=r;(_tmp7C.ptr_atts).nullable=Cyc_Absyn_false_conref;(_tmp7C.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp7C.ptr_atts).zero_term=zeroterm;(_tmp7C.ptr_atts).ptrloc=0;_tmp7C.ptr_atts;});_tmp7C.ptr_atts=_tmp1FA;});_tmp7C;});_tmp7B.f1=_tmp1FB;});_tmp7B;});_tmp7A[0]=_tmp1FC;});_tmp7A;});}
# 476
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FF=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7E;_tmp7E.tag=5;({struct Cyc_Absyn_PtrInfo _tmp1FE=({struct Cyc_Absyn_PtrInfo _tmp7F;_tmp7F.elt_typ=t;_tmp7F.elt_tq=tq;({struct Cyc_Absyn_PtrAtts _tmp1FD=({(_tmp7F.ptr_atts).rgn=r;(_tmp7F.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp7F.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmp7F.ptr_atts).zero_term=zeroterm;(_tmp7F.ptr_atts).ptrloc=0;_tmp7F.ptr_atts;});_tmp7F.ptr_atts=_tmp1FD;});_tmp7F;});_tmp7E.f1=_tmp1FE;});_tmp7E;});_tmp7D[0]=_tmp1FF;});_tmp7D;});}
# 484
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc){
# 486
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80=_cycalloc(sizeof(*_tmp80));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp201=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp81;_tmp81.tag=8;({struct Cyc_Absyn_ArrayInfo _tmp200=({struct Cyc_Absyn_ArrayInfo _tmp82;_tmp82.elt_type=elt_type;_tmp82.tq=tq;_tmp82.num_elts=num_elts;_tmp82.zero_term=zero_term;_tmp82.zt_loc=ztloc;_tmp82;});_tmp81.f1=_tmp200;});_tmp81;});_tmp80[0]=_tmp201;});_tmp80;});}
# 489
void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;
# 492
if(void_star_t_opt == 0)
({void**_tmp204=({void**_tmp83=_cycalloc(sizeof(*_tmp83));({void*_tmp203=({struct Cyc_Absyn_Tqual _tmp202=
Cyc_Absyn_empty_tqual(0);
# 493
Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,_tmp202,Cyc_Absyn_false_conref);});_tmp83[0]=_tmp203;});_tmp83;});void_star_t_opt=_tmp204;});
# 495
return*void_star_t_opt;}
# 497
static struct Cyc_Absyn_JoinEff_Absyn_Type_struct Cyc_Absyn_empty_eff={24,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
# 504
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp208=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp86;_tmp86.tag=11;({struct Cyc_Absyn_AggrInfo _tmp207=({struct Cyc_Absyn_AggrInfo _tmp87;({union Cyc_Absyn_AggrInfoU _tmp206=({enum Cyc_Absyn_AggrKind _tmp205=k;Cyc_Absyn_UnknownAggr(_tmp205,({struct _tuple0*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=Cyc_Absyn_rel_ns_null;_tmp88->f2=name;_tmp88;}),0);});_tmp87.aggr_info=_tmp206;});_tmp87.targs=0;_tmp87;});_tmp86.f1=_tmp207;});_tmp86;});_tmp85[0]=_tmp208;});_tmp85;});}
# 508
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}
# 511
void*Cyc_Absyn_strctq(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp20B=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8A;_tmp8A.tag=11;({struct Cyc_Absyn_AggrInfo _tmp20A=({struct Cyc_Absyn_AggrInfo _tmp8B;({union Cyc_Absyn_AggrInfoU _tmp209=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0);_tmp8B.aggr_info=_tmp209;});_tmp8B.targs=0;_tmp8B;});_tmp8A.f1=_tmp20A;});_tmp8A;});_tmp89[0]=_tmp20B;});_tmp89;});}
# 514
void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp20E=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8D;_tmp8D.tag=11;({struct Cyc_Absyn_AggrInfo _tmp20D=({struct Cyc_Absyn_AggrInfo _tmp8E;({union Cyc_Absyn_AggrInfoU _tmp20C=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0);_tmp8E.aggr_info=_tmp20C;});_tmp8E.targs=0;_tmp8E;});_tmp8D.f1=_tmp20D;});_tmp8D;});_tmp8C[0]=_tmp20E;});_tmp8C;});}
# 518
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp8F;({struct _tuple3 _tmp20F=({struct _tuple3 _tmp90;_tmp90.f1=sn;_tmp90.f2=c;_tmp90;});(_tmp8F.Char_c).val=_tmp20F;});(_tmp8F.Char_c).tag=2;_tmp8F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp91;(_tmp91.Wchar_c).val=s;(_tmp91.Wchar_c).tag=3;_tmp91;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp92;({struct _tuple4 _tmp210=({struct _tuple4 _tmp93;_tmp93.f1=sn;_tmp93.f2=s;_tmp93;});(_tmp92.Short_c).val=_tmp210;});(_tmp92.Short_c).tag=4;_tmp92;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp94;({struct _tuple5 _tmp211=({struct _tuple5 _tmp95;_tmp95.f1=sn;_tmp95.f2=i;_tmp95;});(_tmp94.Int_c).val=_tmp211;});(_tmp94.Int_c).tag=5;_tmp94;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp96;({struct _tuple6 _tmp212=({struct _tuple6 _tmp97;_tmp97.f1=sn;_tmp97.f2=l;_tmp97;});(_tmp96.LongLong_c).val=_tmp212;});(_tmp96.LongLong_c).tag=6;_tmp96;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp98;({struct _tuple7 _tmp213=({struct _tuple7 _tmp99;_tmp99.f1=s;_tmp99.f2=i;_tmp99;});(_tmp98.Float_c).val=_tmp213;});(_tmp98.Float_c).tag=7;_tmp98;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp9A;(_tmp9A.String_c).val=s;(_tmp9A.String_c).tag=8;_tmp9A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp9B;(_tmp9B.Wstring_c).val=s;(_tmp9B.Wstring_c).tag=9;_tmp9B;});}
# 529
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->topt=0;_tmp9C->r=r;_tmp9C->loc=loc;_tmp9C->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp9C;});}
# 532
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp215=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp214=({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp9E;_tmp9E.tag=15;_tmp9E.f1=rgn_handle;_tmp9E.f2=e;_tmp9E;});_tmp9D[0]=_tmp214;});_tmp9D;});Cyc_Absyn_new_exp(_tmp215,loc);});}
# 535
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=*e;_tmp9F;});}
# 538
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
return({void*_tmp217=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp216=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=c;_tmpA1;});_tmpA0[0]=_tmp216;});_tmpA0;});Cyc_Absyn_new_exp(_tmp217,loc);});}
# 541
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 545
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return({union Cyc_Absyn_Cnst _tmp218=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp218,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 553
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 560
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp219=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp219,loc);});}
# 566
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp21A=Cyc_Absyn_Wchar_c(s);Cyc_Absyn_const_exp(_tmp21A,loc);});}
# 569
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp21B=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp21B,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp21C=Cyc_Absyn_String_c(s);Cyc_Absyn_const_exp(_tmp21C,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp21D=Cyc_Absyn_Wstring_c(s);Cyc_Absyn_const_exp(_tmp21D,loc);});}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
return({void*_tmp221=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp220=({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmpA8;_tmpA8.tag=1;({void*_tmp21F=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp21E=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=q;_tmpAA;});_tmpA9[0]=_tmp21E;});_tmpA9;});_tmpA8.f1=_tmp21F;});_tmpA8;});_tmpA7[0]=_tmp220;});_tmpA7;});Cyc_Absyn_new_exp(_tmp221,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned int loc){
return({void*_tmp223=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp222=({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmpAC;_tmpAC.tag=1;_tmpAC.f1=b;_tmpAC;});_tmpAB[0]=_tmp222;});_tmpAB;});Cyc_Absyn_new_exp(_tmp223,loc);});}
# 580
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
return Cyc_Absyn_var_exp(q,loc);}
# 583
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp225=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp224=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpAE;_tmpAE.tag=2;_tmpAE.f1=p;_tmpAE.f2=es;_tmpAE;});_tmpAD[0]=_tmp224;});_tmpAD;});Cyc_Absyn_new_exp(_tmp225,loc);});}
# 586
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp227=p;struct Cyc_List_List*_tmp226=({struct Cyc_List_List*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=e;_tmpAF->tl=0;_tmpAF;});Cyc_Absyn_primop_exp(_tmp227,_tmp226,loc);});}
# 589
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp22A=p;struct Cyc_List_List*_tmp229=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->hd=e1;({struct Cyc_List_List*_tmp228=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=e2;_tmpB1->tl=0;_tmpB1;});_tmpB0->tl=_tmp228;});_tmpB0;});Cyc_Absyn_primop_exp(_tmp22A,_tmp229,loc);});}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp22C=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmp22B=({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmpB3;_tmpB3.tag=34;_tmpB3.f1=e1;_tmpB3.f2=e2;_tmpB3;});_tmpB2[0]=_tmp22B;});_tmpB2;});Cyc_Absyn_new_exp(_tmp22C,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 605
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp22E=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp22D=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB5;_tmpB5.tag=3;_tmpB5.f1=e1;_tmpB5.f2=popt;_tmpB5.f3=e2;_tmpB5;});_tmpB4[0]=_tmp22D;});_tmpB4;});Cyc_Absyn_new_exp(_tmp22E,loc);});}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 611
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
return({void*_tmp230=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp22F=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB7;_tmpB7.tag=4;_tmpB7.f1=e;_tmpB7.f2=i;_tmpB7;});_tmpB6[0]=_tmp22F;});_tmpB6;});Cyc_Absyn_new_exp(_tmp230,loc);});}
# 614
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
return({void*_tmp232=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp231=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpB9;_tmpB9.tag=5;_tmpB9.f1=e1;_tmpB9.f2=e2;_tmpB9.f3=e3;_tmpB9;});_tmpB8[0]=_tmp231;});_tmpB8;});Cyc_Absyn_new_exp(_tmp232,loc);});}
# 621
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp234=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp233=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmpBB;_tmpBB.tag=6;_tmpBB.f1=e1;_tmpBB.f2=e2;_tmpBB;});_tmpBA[0]=_tmp233;});_tmpBA;});Cyc_Absyn_new_exp(_tmp234,loc);});}
# 624
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp236=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp235=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmpBD;_tmpBD.tag=7;_tmpBD.f1=e1;_tmpBD.f2=e2;_tmpBD;});_tmpBC[0]=_tmp235;});_tmpBC;});Cyc_Absyn_new_exp(_tmp236,loc);});}
# 627
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp238=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp237=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmpBF;_tmpBF.tag=8;_tmpBF.f1=e1;_tmpBF.f2=e2;_tmpBF;});_tmpBE[0]=_tmp237;});_tmpBE;});Cyc_Absyn_new_exp(_tmp238,loc);});}
# 630
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp23A=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp239=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpC1;_tmpC1.tag=9;_tmpC1.f1=e;_tmpC1.f2=es;_tmpC1.f3=0;_tmpC1.f4=0;_tmpC1;});_tmpC0[0]=_tmp239;});_tmpC0;});Cyc_Absyn_new_exp(_tmp23A,loc);});}
# 633
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp23C=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp23B=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpC3;_tmpC3.tag=9;_tmpC3.f1=e;_tmpC3.f2=es;_tmpC3.f3=0;_tmpC3.f4=1;_tmpC3;});_tmpC2[0]=_tmp23B;});_tmpC2;});Cyc_Absyn_new_exp(_tmp23C,loc);});}
# 636
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp23E=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmp23D=({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmpC5;_tmpC5.tag=11;_tmpC5.f1=e;_tmpC5;});_tmpC4[0]=_tmp23D;});_tmpC4;});Cyc_Absyn_new_exp(_tmp23E,loc);});}
# 639
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
return({void*_tmp240=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp23F=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpC7;_tmpC7.tag=12;_tmpC7.f1=e;_tmpC7.f2=ts;_tmpC7;});_tmpC6[0]=_tmp23F;});_tmpC6;});Cyc_Absyn_new_exp(_tmp240,loc);});}
# 642
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
return({void*_tmp242=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp241=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpC9;_tmpC9.tag=13;_tmpC9.f1=t;_tmpC9.f2=e;_tmpC9.f3=user_cast;_tmpC9.f4=c;_tmpC9;});_tmpC8[0]=_tmp241;});_tmpC8;});Cyc_Absyn_new_exp(_tmp242,loc);});}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp244=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp243=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmpCB;_tmpCB.tag=10;_tmpCB.f1=e;_tmpCB.f2=0;_tmpCB;});_tmpCA[0]=_tmp243;});_tmpCA;});Cyc_Absyn_new_exp(_tmp244,loc);});}
# 648
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp246=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp245=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmpCD;_tmpCD.tag=10;_tmpCD.f1=e;_tmpCD.f2=1;_tmpCD;});_tmpCC[0]=_tmp245;});_tmpCC;});Cyc_Absyn_new_exp(_tmp246,loc);});}
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp248=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmp247=({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmpCF;_tmpCF.tag=14;_tmpCF.f1=e;_tmpCF;});_tmpCE[0]=_tmp247;});_tmpCE;});Cyc_Absyn_new_exp(_tmp248,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int loc){
return({void*_tmp24A=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp249=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpD1;_tmpD1.tag=16;_tmpD1.f1=t;_tmpD1;});_tmpD0[0]=_tmp249;});_tmpD0;});Cyc_Absyn_new_exp(_tmp24A,loc);});}
# 655
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp24C=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp24B=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmpD3;_tmpD3.tag=17;_tmpD3.f1=e;_tmpD3;});_tmpD2[0]=_tmp24B;});_tmpD2;});Cyc_Absyn_new_exp(_tmp24C,loc);});}
# 658
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
return({void*_tmp24E=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp24D=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD5;_tmpD5.tag=18;_tmpD5.f1=t;_tmpD5.f2=ofs;_tmpD5;});_tmpD4[0]=_tmp24D;});_tmpD4;});Cyc_Absyn_new_exp(_tmp24E,loc);});}
# 661
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp250=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp24F=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpD7;_tmpD7.tag=19;_tmpD7.f1=e;_tmpD7;});_tmpD6[0]=_tmp24F;});_tmpD6;});Cyc_Absyn_new_exp(_tmp250,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp252=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp251=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpD9;_tmpD9.tag=20;_tmpD9.f1=e;_tmpD9.f2=n;_tmpD9.f3=0;_tmpD9.f4=0;_tmpD9;});_tmpD8[0]=_tmp251;});_tmpD8;});Cyc_Absyn_new_exp(_tmp252,loc);});}
# 665
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp254=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp253=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpDB;_tmpDB.tag=21;_tmpDB.f1=e;_tmpDB.f2=n;_tmpDB.f3=0;_tmpDB.f4=0;_tmpDB;});_tmpDA[0]=_tmp253;});_tmpDA;});Cyc_Absyn_new_exp(_tmp254,loc);});}
# 668
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp256=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp255=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpDD;_tmpDD.tag=22;_tmpDD.f1=e1;_tmpDD.f2=e2;_tmpDD;});_tmpDC[0]=_tmp255;});_tmpDC;});Cyc_Absyn_new_exp(_tmp256,loc);});}
# 671
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp258=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmp257=({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmpDF;_tmpDF.tag=23;_tmpDF.f1=es;_tmpDF;});_tmpDE[0]=_tmp257;});_tmpDE;});Cyc_Absyn_new_exp(_tmp258,loc);});}
# 674
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){return({void*_tmp25A=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp259=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpE1;_tmpE1.tag=36;_tmpE1.f1=s;_tmpE1;});_tmpE0[0]=_tmp259;});_tmpE0;});Cyc_Absyn_new_exp(_tmp25A,loc);});}
# 676
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 680
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
return({void*_tmp25C=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp25B=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmpE3;_tmpE3.tag=38;_tmpE3.f1=t;_tmpE3;});_tmpE2[0]=_tmp25B;});_tmpE2;});Cyc_Absyn_new_exp(_tmp25C,loc);});}
# 683
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
return({void*_tmp25E=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmp25D=({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmpE5;_tmpE5.tag=39;_tmpE5.f1=volatile_kw;_tmpE5.f2=body;_tmpE5;});_tmpE4[0]=_tmp25D;});_tmpE4;});Cyc_Absyn_new_exp(_tmp25E,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 687
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 689
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
({struct Cyc_List_List*_tmp260=({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));({struct _tuple13*_tmp25F=({struct _tuple13*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->f1=0;_tmpE7->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE7;});_tmpE6->hd=_tmp25F;});_tmpE6->tl=dles;_tmpE6;});dles=_tmp260;});}
({struct Cyc_List_List*_tmp261=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);dles=_tmp261;});
return({void*_tmp263=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp262=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE9;_tmpE9.tag=25;_tmpE9.f1=dles;_tmpE9;});_tmpE8[0]=_tmp262;});_tmpE8;});Cyc_Absyn_new_exp(_tmp263,loc);});}
# 695
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 698
return({void*_tmp265=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp264=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpEB;_tmpEB.tag=35;_tmpEB.f1=n;_tmpEB.f2=dles;_tmpEB;});_tmpEA[0]=_tmp264;});_tmpEA;});Cyc_Absyn_new_exp(_tmp265,loc);});}
# 701
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
return({struct Cyc_Absyn_Stmt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->r=s;_tmpEC->loc=loc;_tmpEC->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmpEC;});}
# 704
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6};
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp267=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp266=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpF1;_tmpF1.tag=1;_tmpF1.f1=e;_tmpF1;});_tmpF0[0]=_tmp266;});_tmpF0;});Cyc_Absyn_new_stmt(_tmp267,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp269=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp268=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp269,_tmp268,loc);});}}}
# 714
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp26B=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmp26A=({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmpF3;_tmpF3.tag=3;_tmpF3.f1=e;_tmpF3;});_tmpF2[0]=_tmp26A;});_tmpF2;});Cyc_Absyn_new_stmt(_tmp26B,loc);});}
# 717
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
return({void*_tmp26D=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmp26C=({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmpF5;_tmpF5.tag=4;_tmpF5.f1=e;_tmpF5.f2=s1;_tmpF5.f3=s2;_tmpF5;});_tmpF4[0]=_tmp26C;});_tmpF4;});Cyc_Absyn_new_stmt(_tmp26D,loc);});}
# 720
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp271=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmp270=({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmpF7;_tmpF7.tag=5;({struct _tuple9 _tmp26F=({struct _tuple9 _tmpF8;_tmpF8.f1=e;({struct Cyc_Absyn_Stmt*_tmp26E=Cyc_Absyn_skip_stmt(e->loc);_tmpF8.f2=_tmp26E;});_tmpF8;});_tmpF7.f1=_tmp26F;});_tmpF7.f2=s;_tmpF7;});_tmpF6[0]=_tmp270;});_tmpF6;});Cyc_Absyn_new_stmt(_tmp271,loc);});}
# 723
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp277=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmp276=({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmpFA;_tmpFA.tag=9;_tmpFA.f1=e1;({struct _tuple9 _tmp273=({struct _tuple9 _tmpFB;_tmpFB.f1=e2;({struct Cyc_Absyn_Stmt*_tmp272=Cyc_Absyn_skip_stmt(e3->loc);_tmpFB.f2=_tmp272;});_tmpFB;});_tmpFA.f2=_tmp273;});({struct _tuple9 _tmp275=({struct _tuple9 _tmpFC;_tmpFC.f1=e3;({struct Cyc_Absyn_Stmt*_tmp274=
Cyc_Absyn_skip_stmt(e3->loc);_tmpFC.f2=_tmp274;});_tmpFC;});_tmpFA.f3=_tmp275;});_tmpFA.f4=s;_tmpFA;});_tmpF9[0]=_tmp276;});_tmpF9;});
# 726
Cyc_Absyn_new_stmt(_tmp277,loc);});}
# 730
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp279=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp278=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpFE;_tmpFE.tag=10;_tmpFE.f1=e;_tmpFE.f2=scs;_tmpFE.f3=0;_tmpFE;});_tmpFD[0]=_tmp278;});_tmpFD;});Cyc_Absyn_new_stmt(_tmp279,loc);});}struct _tuple14{void*f1;void*f2;};
# 733
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmpFF=({struct _tuple14 _tmp103;_tmp103.f1=s1->r;_tmp103.f2=s2->r;_tmp103;});struct _tuple14 _tmp100=_tmpFF;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp100.f1)->tag == 0){_LL88: _LL89:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp100.f2)->tag == 0){_LL8A: _LL8B:
 return s1;}else{_LL8C: _LL8D:
 return({void*_tmp27B=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp101=_cycalloc(sizeof(*_tmp101));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp27A=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp102;_tmp102.tag=2;_tmp102.f1=s1;_tmp102.f2=s2;_tmp102;});_tmp101[0]=_tmp27A;});_tmp101;});Cyc_Absyn_new_stmt(_tmp27B,loc);});}}_LL87:;}
# 740
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
return({void*_tmp27D=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp104=_cycalloc(sizeof(*_tmp104));({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp27C=({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp105;_tmp105.tag=11;_tmp105.f1=el;_tmp105.f2=0;_tmp105;});_tmp104[0]=_tmp27C;});_tmp104;});Cyc_Absyn_new_stmt(_tmp27D,loc);});}
# 743
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp27F=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp27E=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp107;_tmp107.tag=12;_tmp107.f1=d;_tmp107.f2=s;_tmp107;});_tmp106[0]=_tmp27E;});_tmp106;});Cyc_Absyn_new_stmt(_tmp27F,loc);});}
# 746
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl*d=({void*_tmp282=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp281=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp10B;_tmp10B.tag=0;({struct Cyc_Absyn_Vardecl*_tmp280=Cyc_Absyn_new_vardecl(0,x,t,init);_tmp10B.f1=_tmp280;});_tmp10B;});_tmp10A[0]=_tmp281;});_tmp10A;});Cyc_Absyn_new_decl(_tmp282,loc);});
return({void*_tmp284=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp108=_cycalloc(sizeof(*_tmp108));({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp283=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp109;_tmp109.tag=12;_tmp109.f1=d;_tmp109.f2=s;_tmp109;});_tmp108[0]=_tmp283;});_tmp108;});Cyc_Absyn_new_stmt(_tmp284,loc);});}
# 750
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp286=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp285=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp10D;_tmp10D.tag=13;_tmp10D.f1=v;_tmp10D.f2=s;_tmp10D;});_tmp10C[0]=_tmp285;});_tmp10C;});Cyc_Absyn_new_stmt(_tmp286,loc);});}
# 753
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp28A=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp289=({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp10F;_tmp10F.tag=14;_tmp10F.f1=s;({struct _tuple9 _tmp288=({struct _tuple9 _tmp110;_tmp110.f1=e;({struct Cyc_Absyn_Stmt*_tmp287=Cyc_Absyn_skip_stmt(e->loc);_tmp110.f2=_tmp287;});_tmp110;});_tmp10F.f2=_tmp288;});_tmp10F;});_tmp10E[0]=_tmp289;});_tmp10E;});Cyc_Absyn_new_stmt(_tmp28A,loc);});}
# 756
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp28C=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp111=_cycalloc(sizeof(*_tmp111));({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp28B=({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp112;_tmp112.tag=15;_tmp112.f1=s;_tmp112.f2=scs;_tmp112.f3=0;_tmp112;});_tmp111[0]=_tmp28B;});_tmp111;});Cyc_Absyn_new_stmt(_tmp28C,loc);});}
# 759
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
return({void*_tmp28E=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp28D=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp114;_tmp114.tag=8;_tmp114.f1=lab;_tmp114;});_tmp113[0]=_tmp28D;});_tmp113;});Cyc_Absyn_new_stmt(_tmp28E,loc);});}
# 762
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp28F=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp28F,loc);});}
# 766
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){return({struct Cyc_Absyn_Pat*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->r=p;_tmp115->topt=0;_tmp115->loc=s;_tmp115;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp291=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp290=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp117;_tmp117.tag=17;_tmp117.f1=e;_tmp117;});_tmp116[0]=_tmp290;});_tmp116;});Cyc_Absyn_new_pat(_tmp291,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9};
# 772
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){return({struct Cyc_Absyn_Decl*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->r=r;_tmp11A->loc=loc;_tmp11A;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp293=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp292=({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp11C;_tmp11C.tag=2;_tmp11C.f1=p;_tmp11C.f2=0;_tmp11C.f3=e;_tmp11C.f4=0;_tmp11C;});_tmp11B[0]=_tmp292;});_tmp11B;});Cyc_Absyn_new_decl(_tmp293,loc);});}
# 776
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
return({void*_tmp295=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp294=({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp11E;_tmp11E.tag=3;_tmp11E.f1=vds;_tmp11E;});_tmp11D[0]=_tmp294;});_tmp11D;});Cyc_Absyn_new_decl(_tmp295,loc);});}
# 779
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned int loc){
return({void*_tmp297=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp296=({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp120;_tmp120.tag=4;_tmp120.f1=tv;_tmp120.f2=vd;_tmp120.f3=open_exp;_tmp120;});_tmp11F[0]=_tmp296;});_tmp11F;});Cyc_Absyn_new_decl(_tmp297,loc);});}
# 782
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 784
return({void*_tmp29C=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp29B=({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp122;_tmp122.tag=2;({struct Cyc_Absyn_Pat*_tmp29A=({void*_tmp299=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp123=_cycalloc(sizeof(*_tmp123));({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp298=({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp124;_tmp124.tag=2;_tmp124.f1=tv;_tmp124.f2=vd;_tmp124;});_tmp123[0]=_tmp298;});_tmp123;});Cyc_Absyn_new_pat(_tmp299,loc);});_tmp122.f1=_tmp29A;});_tmp122.f2=0;_tmp122.f3=e;_tmp122.f4=0;_tmp122;});_tmp121[0]=_tmp29B;});_tmp121;});Cyc_Absyn_new_decl(_tmp29C,loc);});}
# 787
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->sc=Cyc_Absyn_Public;_tmp125->name=x;_tmp125->varloc=varloc;({struct Cyc_Absyn_Tqual _tmp29D=
Cyc_Absyn_empty_tqual(0);_tmp125->tq=_tmp29D;});_tmp125->type=t;_tmp125->initializer=init;_tmp125->rgn=0;_tmp125->attributes=0;_tmp125->escapes=0;_tmp125;});}
# 793
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->sc=Cyc_Absyn_Static;_tmp126->name=x;_tmp126->varloc=0;({struct Cyc_Absyn_Tqual _tmp29E=Cyc_Absyn_empty_tqual(0);_tmp126->tq=_tmp29E;});_tmp126->type=t;_tmp126->initializer=init;_tmp126->rgn=0;_tmp126->attributes=0;_tmp126->escapes=0;_tmp126;});}
# 798
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 802
return({struct Cyc_Absyn_AggrdeclImpl*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->exist_vars=exists;_tmp127->rgn_po=po;_tmp127->fields=fs;_tmp127->tagged=tagged;_tmp127;});}
# 805
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 808
return({void*_tmp2A1=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp2A0=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp129;_tmp129.tag=5;({struct Cyc_Absyn_Aggrdecl*_tmp29F=({struct Cyc_Absyn_Aggrdecl*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->kind=k;_tmp12A->sc=s;_tmp12A->name=n;_tmp12A->tvs=ts;_tmp12A->impl=i;_tmp12A->attributes=atts;_tmp12A->expected_mem_kind=0;_tmp12A;});_tmp129.f1=_tmp29F;});_tmp129;});_tmp128[0]=_tmp2A0;});_tmp128;});Cyc_Absyn_new_decl(_tmp2A1,loc);});}
# 813
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 816
return({struct Cyc_Absyn_TypeDecl*_tmp12B=_cycalloc(sizeof(*_tmp12B));({void*_tmp2A4=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp2A3=({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp12D;_tmp12D.tag=0;({struct Cyc_Absyn_Aggrdecl*_tmp2A2=({struct Cyc_Absyn_Aggrdecl*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->kind=k;_tmp12E->sc=s;_tmp12E->name=n;_tmp12E->tvs=ts;_tmp12E->impl=i;_tmp12E->attributes=atts;_tmp12E->expected_mem_kind=0;_tmp12E;});_tmp12D.f1=_tmp2A2;});_tmp12D;});_tmp12C[0]=_tmp2A3;});_tmp12C;});_tmp12B->r=_tmp2A4;});_tmp12B->loc=loc;_tmp12B;});}
# 822
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 825
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 827
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 830
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 832
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 835
return({void*_tmp2A7=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2A6=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp130;_tmp130.tag=6;({struct Cyc_Absyn_Datatypedecl*_tmp2A5=({struct Cyc_Absyn_Datatypedecl*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->sc=s;_tmp131->name=n;_tmp131->tvs=ts;_tmp131->fields=fs;_tmp131->is_extensible=is_extensible;_tmp131;});_tmp130.f1=_tmp2A5;});_tmp130;});_tmp12F[0]=_tmp2A6;});_tmp12F;});Cyc_Absyn_new_decl(_tmp2A7,loc);});}
# 838
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 841
return({struct Cyc_Absyn_TypeDecl*_tmp132=_cycalloc(sizeof(*_tmp132));({void*_tmp2AA=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp2A9=({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp134;_tmp134.tag=2;({struct Cyc_Absyn_Datatypedecl*_tmp2A8=({struct Cyc_Absyn_Datatypedecl*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->sc=s;_tmp135->name=n;_tmp135->tvs=ts;_tmp135->fields=fs;_tmp135->is_extensible=is_extensible;_tmp135;});_tmp134.f1=_tmp2A8;});_tmp134;});_tmp133[0]=_tmp2A9;});_tmp133;});_tmp132->r=_tmp2AA;});_tmp132->loc=loc;_tmp132;});}
# 853 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp136=(*a).f3;
void*_tmp137=Cyc_Absyn_pointer_expand(_tmp136,1);
if(_tmp136 != _tmp137)
(*a).f3=_tmp137;}
# 860
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 867
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp138=_cycalloc(sizeof(*_tmp138));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp2AD=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp139;_tmp139.tag=9;({struct Cyc_Absyn_FnInfo _tmp2AC=({struct Cyc_Absyn_FnInfo _tmp13A;_tmp13A.tvars=tvs;_tmp13A.ret_tqual=ret_tqual;({void*_tmp2AB=
# 870
Cyc_Absyn_pointer_expand(ret_typ,0);_tmp13A.ret_typ=_tmp2AB;});_tmp13A.effect=eff_typ;_tmp13A.args=args;_tmp13A.c_varargs=c_varargs;_tmp13A.cyc_varargs=cyc_varargs;_tmp13A.rgn_po=rgn_po;_tmp13A.attributes=atts;_tmp13A.requires_clause=req;_tmp13A.requires_relns=0;_tmp13A.ensures_clause=ens;_tmp13A.ensures_relns=0;_tmp13A;});_tmp139.f1=_tmp2AC;});_tmp139;});_tmp138[0]=_tmp2AD;});_tmp138;});}
# 884
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmp13B=Cyc_Tcutil_compress(t);void*_tmp13C=_tmp13B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13C)->tag == 9){_LL8F: _LL90:
 return({void*_tmp2B0=t;void*_tmp2AF=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->v=& Cyc_Tcutil_rk;_tmp13D;}),0):(void*)& Cyc_Absyn_HeapRgn_val;
# 886
struct Cyc_Absyn_Tqual _tmp2AE=
# 890
Cyc_Absyn_empty_tqual(0);
# 886
Cyc_Absyn_at_typ(_tmp2B0,_tmp2AF,_tmp2AE,Cyc_Absyn_false_conref);});}else{_LL91: _LL92:
# 891
 return t;}_LL8E:;}
# 906 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmp13E=e->r;void*_tmp13F=_tmp13E;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Vardecl*_tmp143;struct Cyc_Absyn_Vardecl*_tmp142;switch(*((int*)_tmp13F)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp13F)->f1)){case 2: _LL94: _LL95:
# 909
 return 0;case 1: _LL96: _tmp142=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp13F)->f1)->f1;_LL97:
 _tmp143=_tmp142;goto _LL99;case 4: _LL98: _tmp143=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp13F)->f1)->f1;_LL99: {
# 912
void*_tmp140=Cyc_Tcutil_compress(_tmp143->type);void*_tmp141=_tmp140;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp141)->tag == 8){_LLAB: _LLAC:
 return 0;}else{_LLAD: _LLAE:
 return 1;}_LLAA:;}default: _LL9A: _LL9B:
# 916
 goto _LL9D;}case 21: _LL9C: _LL9D:
 goto _LL9F;case 19: _LL9E: _LL9F:
 goto _LLA1;case 22: _LLA0: _LLA1:
 return 1;case 20: _LLA2: _tmp144=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp13F)->f1;_LLA3:
 return Cyc_Absyn_is_lvalue(_tmp144);case 12: _LLA4: _tmp145=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp13F)->f1;_LLA5:
 return Cyc_Absyn_is_lvalue(_tmp145);case 11: _LLA6: _tmp146=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp13F)->f1;_LLA7:
 return Cyc_Absyn_is_lvalue(_tmp146);default: _LLA8: _LLA9:
 return 0;}_LL93:;}
# 927
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmp147=fields;for(0;_tmp147 != 0;_tmp147=_tmp147->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp147->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmp147->hd;}}
return 0;}
# 933
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 937
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 943
if(ts == 0)
return 0;
return(struct _tuple10*)ts->hd;}
# 948
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={1};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={13};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={14};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={15};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={16};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={17};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={18};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={23};
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val={26};
# 967
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmp15A=a;struct _dyneither_ptr _tmp19D;struct _dyneither_ptr _tmp19C;int _tmp19B;int _tmp19A;int _tmp199;int _tmp198;int _tmp197;int _tmp196;int _tmp195;struct _dyneither_ptr _tmp194;struct Cyc_Absyn_Exp*_tmp193;int _tmp192;switch(*((int*)_tmp15A)){case 0: _LLB0: _tmp192=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLB1:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp15D;_tmp15D.tag=1;_tmp15D.f1=(unsigned long)_tmp192;({void*_tmp15B[1]={& _tmp15D};({struct _dyneither_ptr _tmp2B1=({const char*_tmp15C="regparm(%d)";_tag_dyneither(_tmp15C,sizeof(char),12);});Cyc_aprintf(_tmp2B1,_tag_dyneither(_tmp15B,sizeof(void*),1));});});});case 1: _LLB2: _LLB3:
 return({const char*_tmp15E="stdcall";_tag_dyneither(_tmp15E,sizeof(char),8);});case 2: _LLB4: _LLB5:
 return({const char*_tmp15F="cdecl";_tag_dyneither(_tmp15F,sizeof(char),6);});case 3: _LLB6: _LLB7:
 return({const char*_tmp160="fastcall";_tag_dyneither(_tmp160,sizeof(char),9);});case 4: _LLB8: _LLB9:
 return({const char*_tmp161="noreturn";_tag_dyneither(_tmp161,sizeof(char),9);});case 5: _LLBA: _LLBB:
 return({const char*_tmp162="const";_tag_dyneither(_tmp162,sizeof(char),6);});case 6: _LLBC: _tmp193=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLBD:
# 976
 if(_tmp193 == 0)return({const char*_tmp163="aligned";_tag_dyneither(_tmp163,sizeof(char),8);});else{
# 978
enum Cyc_Cyclone_C_Compilers _tmp164=Cyc_Cyclone_c_compiler;if(_tmp164 == Cyc_Cyclone_Gcc_c){_LLE9: _LLEA:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp167;_tmp167.tag=0;({struct _dyneither_ptr _tmp2B2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp193));_tmp167.f1=_tmp2B2;});({void*_tmp165[1]={& _tmp167};({struct _dyneither_ptr _tmp2B3=({const char*_tmp166="aligned(%s)";_tag_dyneither(_tmp166,sizeof(char),12);});Cyc_aprintf(_tmp2B3,_tag_dyneither(_tmp165,sizeof(void*),1));});});});}else{_LLEB: _LLEC:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0;({struct _dyneither_ptr _tmp2B4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp193));_tmp16A.f1=_tmp2B4;});({void*_tmp168[1]={& _tmp16A};({struct _dyneither_ptr _tmp2B5=({const char*_tmp169="align(%s)";_tag_dyneither(_tmp169,sizeof(char),10);});Cyc_aprintf(_tmp2B5,_tag_dyneither(_tmp168,sizeof(void*),1));});});});}_LLE8:;}case 7: _LLBE: _LLBF:
# 982
 return({const char*_tmp16B="packed";_tag_dyneither(_tmp16B,sizeof(char),7);});case 8: _LLC0: _tmp194=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLC1:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp194);({void*_tmp16C[1]={& _tmp16E};({struct _dyneither_ptr _tmp2B6=({const char*_tmp16D="section(\"%s\")";_tag_dyneither(_tmp16D,sizeof(char),14);});Cyc_aprintf(_tmp2B6,_tag_dyneither(_tmp16C,sizeof(void*),1));});});});case 9: _LLC2: _LLC3:
 return({const char*_tmp16F="nocommon";_tag_dyneither(_tmp16F,sizeof(char),9);});case 10: _LLC4: _LLC5:
 return({const char*_tmp170="shared";_tag_dyneither(_tmp170,sizeof(char),7);});case 11: _LLC6: _LLC7:
 return({const char*_tmp171="unused";_tag_dyneither(_tmp171,sizeof(char),7);});case 12: _LLC8: _LLC9:
 return({const char*_tmp172="weak";_tag_dyneither(_tmp172,sizeof(char),5);});case 13: _LLCA: _LLCB:
 return({const char*_tmp173="dllimport";_tag_dyneither(_tmp173,sizeof(char),10);});case 14: _LLCC: _LLCD:
 return({const char*_tmp174="dllexport";_tag_dyneither(_tmp174,sizeof(char),10);});case 15: _LLCE: _LLCF:
 return({const char*_tmp175="no_instrument_function";_tag_dyneither(_tmp175,sizeof(char),23);});case 16: _LLD0: _LLD1:
 return({const char*_tmp176="constructor";_tag_dyneither(_tmp176,sizeof(char),12);});case 17: _LLD2: _LLD3:
 return({const char*_tmp177="destructor";_tag_dyneither(_tmp177,sizeof(char),11);});case 18: _LLD4: _LLD5:
 return({const char*_tmp178="no_check_memory_usage";_tag_dyneither(_tmp178,sizeof(char),22);});case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15A)->f1 == Cyc_Absyn_Printf_ft){_LLD6: _tmp196=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15A)->f2;_tmp195=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15A)->f3;_LLD7:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp17C;_tmp17C.tag=1;_tmp17C.f1=(unsigned int)_tmp195;({struct Cyc_Int_pa_PrintArg_struct _tmp17B;_tmp17B.tag=1;_tmp17B.f1=(unsigned int)_tmp196;({void*_tmp179[2]={& _tmp17B,& _tmp17C};({struct _dyneither_ptr _tmp2B7=({const char*_tmp17A="format(printf,%u,%u)";_tag_dyneither(_tmp17A,sizeof(char),21);});Cyc_aprintf(_tmp2B7,_tag_dyneither(_tmp179,sizeof(void*),2));});});});});}else{_LLD8: _tmp198=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15A)->f2;_tmp197=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp15A)->f3;_LLD9:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp180;_tmp180.tag=1;_tmp180.f1=(unsigned int)_tmp197;({struct Cyc_Int_pa_PrintArg_struct _tmp17F;_tmp17F.tag=1;_tmp17F.f1=(unsigned int)_tmp198;({void*_tmp17D[2]={& _tmp17F,& _tmp180};({struct _dyneither_ptr _tmp2B8=({const char*_tmp17E="format(scanf,%u,%u)";_tag_dyneither(_tmp17E,sizeof(char),20);});Cyc_aprintf(_tmp2B8,_tag_dyneither(_tmp17D,sizeof(void*),2));});});});});}case 20: _LLDA: _tmp199=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLDB:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp183;_tmp183.tag=1;_tmp183.f1=(unsigned long)_tmp199;({void*_tmp181[1]={& _tmp183};({struct _dyneither_ptr _tmp2B9=({const char*_tmp182="initializes(%d)";_tag_dyneither(_tmp182,sizeof(char),16);});Cyc_aprintf(_tmp2B9,_tag_dyneither(_tmp181,sizeof(void*),1));});});});case 21: _LLDC: _tmp19A=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLDD:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp186;_tmp186.tag=1;_tmp186.f1=(unsigned long)_tmp19A;({void*_tmp184[1]={& _tmp186};({struct _dyneither_ptr _tmp2BA=({const char*_tmp185="noliveunique(%d)";_tag_dyneither(_tmp185,sizeof(char),17);});Cyc_aprintf(_tmp2BA,_tag_dyneither(_tmp184,sizeof(void*),1));});});});case 22: _LLDE: _tmp19B=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLDF:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp189;_tmp189.tag=1;_tmp189.f1=(unsigned long)_tmp19B;({void*_tmp187[1]={& _tmp189};({struct _dyneither_ptr _tmp2BB=({const char*_tmp188="noconsume(%d)";_tag_dyneither(_tmp188,sizeof(char),14);});Cyc_aprintf(_tmp2BB,_tag_dyneither(_tmp187,sizeof(void*),1));});});});case 23: _LLE0: _LLE1:
 return({const char*_tmp18A="pure";_tag_dyneither(_tmp18A,sizeof(char),5);});case 26: _LLE2: _LLE3:
 return({const char*_tmp18B="always_inline";_tag_dyneither(_tmp18B,sizeof(char),14);});case 24: _LLE4: _tmp19C=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLE5:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18E;_tmp18E.tag=0;_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp19C);({void*_tmp18C[1]={& _tmp18E};({struct _dyneither_ptr _tmp2BC=({const char*_tmp18D="__mode__(\"%s\")";_tag_dyneither(_tmp18D,sizeof(char),15);});Cyc_aprintf(_tmp2BC,_tag_dyneither(_tmp18C,sizeof(void*),1));});});});default: _LLE6: _tmp19D=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmp15A)->f1;_LLE7:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp191;_tmp191.tag=0;_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp19D);({void*_tmp18F[1]={& _tmp191};({struct _dyneither_ptr _tmp2BD=({const char*_tmp190="alias(\"%s\")";_tag_dyneither(_tmp190,sizeof(char),12);});Cyc_aprintf(_tmp2BD,_tag_dyneither(_tmp18F,sizeof(void*),1));});});});}_LLAF:;}
# 1007
int Cyc_Absyn_fntype_att(void*a){
void*_tmp19E=a;int _tmp19F;switch(*((int*)_tmp19E)){case 0: _LLEE: _tmp19F=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp19E)->f1;_LLEF:
 goto _LLF1;case 3: _LLF0: _LLF1:
 goto _LLF3;case 1: _LLF2: _LLF3:
 goto _LLF5;case 2: _LLF4: _LLF5:
 goto _LLF7;case 4: _LLF6: _LLF7:
 goto _LLF9;case 23: _LLF8: _LLF9:
 goto _LLFB;case 19: _LLFA: _LLFB:
 goto _LLFD;case 5: _LLFC: _LLFD:
 goto _LLFF;case 21: _LLFE: _LLFF:
 goto _LL101;case 20: _LL100: _LL101:
 goto _LL103;case 22: _LL102: _LL103:
 return 1;default: _LL104: _LL105:
 return 0;}_LLED:;}static char _tmp1A0[3]="f0";
# 1025
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1A0,_tmp1A0,_tmp1A0 + 3};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if(i >= fsz)
({struct _dyneither_ptr _tmp2C1=({unsigned int _tmp1A1=(unsigned int)(i + 1);struct _dyneither_ptr**_tmp1A2=(struct _dyneither_ptr**)_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp1A1));struct _dyneither_ptr _tmp1A8=_tag_dyneither(_tmp1A2,sizeof(struct _dyneither_ptr*),_tmp1A1);{unsigned int _tmp1A3=_tmp1A1;unsigned int j;for(j=0;j < _tmp1A3;j ++){({typeof((struct _dyneither_ptr*)(
# 1033
j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));({struct _dyneither_ptr _tmp2BF=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=1;_tmp1A7.f1=(unsigned long)((int)j);({void*_tmp1A5[1]={& _tmp1A7};({struct _dyneither_ptr _tmp2BE=({const char*_tmp1A6="f%d";_tag_dyneither(_tmp1A6,sizeof(char),4);});Cyc_aprintf(_tmp2BE,_tag_dyneither(_tmp1A5,sizeof(void*),1));});});});_tmp1A4[0]=_tmp2BF;});_tmp1A4;})))_tmp2C0=(struct _dyneither_ptr*)(j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));({struct _dyneither_ptr _tmp2BF=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=1;_tmp1A7.f1=(unsigned long)((int)j);({void*_tmp1A5[1]={& _tmp1A7};({struct _dyneither_ptr _tmp2BE=({const char*_tmp1A6="f%d";_tag_dyneither(_tmp1A6,sizeof(char),4);});Cyc_aprintf(_tmp2BE,_tag_dyneither(_tmp1A5,sizeof(void*),1));});});});_tmp1A4[0]=_tmp2BF;});_tmp1A4;}));_tmp1A2[j]=_tmp2C0;});}}_tmp1A8;});
# 1031
Cyc_Absyn_field_names=_tmp2C1;});
# 1034
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1037
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1A9=info;enum Cyc_Absyn_AggrKind _tmp1AF;struct _tuple0*_tmp1AE;enum Cyc_Absyn_AggrKind _tmp1AD;struct _tuple0*_tmp1AC;if((_tmp1A9.UnknownAggr).tag == 1){_LL107: _tmp1AD=((_tmp1A9.UnknownAggr).val).f1;_tmp1AC=((_tmp1A9.UnknownAggr).val).f2;_LL108:
 return({struct _tuple11 _tmp1AA;_tmp1AA.f1=_tmp1AD;_tmp1AA.f2=_tmp1AC;_tmp1AA;});}else{_LL109: _tmp1AF=(*(_tmp1A9.KnownAggr).val)->kind;_tmp1AE=(*(_tmp1A9.KnownAggr).val)->name;_LL10A:
 return({struct _tuple11 _tmp1AB;_tmp1AB.f1=_tmp1AF;_tmp1AB.f2=_tmp1AE;_tmp1AB;});}_LL106:;}
# 1043
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1B0=info;struct Cyc_Absyn_Aggrdecl*_tmp1B3;if((_tmp1B0.UnknownAggr).tag == 1){_LL10C: _LL10D:
({void*_tmp1B1=0;({struct _dyneither_ptr _tmp2C2=({const char*_tmp1B2="unchecked aggrdecl";_tag_dyneither(_tmp1B2,sizeof(char),19);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp2C2,_tag_dyneither(_tmp1B1,sizeof(void*),0));});});}else{_LL10E: _tmp1B3=*(_tmp1B0.KnownAggr).val;_LL10F:
 return _tmp1B3;}_LL10B:;}
# 1049
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp1B4=Cyc_Tcutil_compress(t);void*_tmp1B5=_tmp1B4;union Cyc_Absyn_AggrInfoU _tmp1BE;struct Cyc_List_List*_tmp1BD;switch(*((int*)_tmp1B5)){case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B5)->f1 == Cyc_Absyn_UnionA){_LL111: _tmp1BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B5)->f2;_LL112:
# 1052
 if(_tmp1BD == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1BD->hd)->requires_clause == 0;}else{goto _LL115;}case 11: _LL113: _tmp1BE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).aggr_info;_LL114: {
# 1055
union Cyc_Absyn_AggrInfoU _tmp1B6=_tmp1BE;enum Cyc_Absyn_AggrKind _tmp1BC;int _tmp1BB;enum Cyc_Absyn_AggrKind _tmp1BA;struct Cyc_Absyn_Aggrdecl*_tmp1B9;if((_tmp1B6.KnownAggr).tag == 2){_LL118: _tmp1B9=*(_tmp1B6.KnownAggr).val;_LL119:
# 1057
 if(_tmp1B9->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1B7=_tmp1B9->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1B7))->tagged)return 0;{
struct Cyc_List_List*_tmp1B8=_tmp1B7->fields;
if(_tmp1B8 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1B8->hd)->requires_clause == 0;};};}else{if(((_tmp1B6.UnknownAggr).val).f3 == 0){_LL11A: _tmp1BA=((_tmp1B6.UnknownAggr).val).f1;_LL11B:
 return _tmp1BA == Cyc_Absyn_UnionA;}else{_LL11C: _tmp1BC=((_tmp1B6.UnknownAggr).val).f1;_tmp1BB=(int)(((_tmp1B6.UnknownAggr).val).f3)->v;_LL11D:
 return _tmp1BC == Cyc_Absyn_UnionA  && !_tmp1BB;}}_LL117:;}default: _LL115: _LL116:
# 1066
 return 0;}_LL110:;}
# 1069
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp1BF=Cyc_Tcutil_compress(t);void*_tmp1C0=_tmp1BF;union Cyc_Absyn_AggrInfoU _tmp1C9;struct Cyc_List_List*_tmp1C8;switch(*((int*)_tmp1C0)){case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C0)->f1 == Cyc_Absyn_UnionA){_LL11F: _tmp1C8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C0)->f2;_LL120:
# 1072
 if(_tmp1C8 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp1C8->hd)->requires_clause != 0;}else{goto _LL123;}case 11: _LL121: _tmp1C9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C0)->f1).aggr_info;_LL122: {
# 1075
union Cyc_Absyn_AggrInfoU _tmp1C1=_tmp1C9;enum Cyc_Absyn_AggrKind _tmp1C7;int _tmp1C6;enum Cyc_Absyn_AggrKind _tmp1C5;struct Cyc_Absyn_Aggrdecl*_tmp1C4;if((_tmp1C1.KnownAggr).tag == 2){_LL126: _tmp1C4=*(_tmp1C1.KnownAggr).val;_LL127:
# 1077
 if(_tmp1C4->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1C2=_tmp1C4->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C2))->tagged)return 0;{
struct Cyc_List_List*_tmp1C3=_tmp1C2->fields;
if(_tmp1C3 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp1C3->hd)->requires_clause != 0;};};}else{if(((_tmp1C1.UnknownAggr).val).f3 == 0){_LL128: _tmp1C5=((_tmp1C1.UnknownAggr).val).f1;_LL129:
 return 0;}else{_LL12A: _tmp1C7=((_tmp1C1.UnknownAggr).val).f1;_tmp1C6=(int)(((_tmp1C1.UnknownAggr).val).f3)->v;_LL12B:
 return 0;}}_LL125:;}default: _LL123: _LL124:
# 1086
 return 0;}_LL11E:;}
# 1091
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp1CA=b;struct Cyc_Absyn_Fndecl*_tmp1D0;struct Cyc_Absyn_Vardecl*_tmp1CF;struct Cyc_Absyn_Vardecl*_tmp1CE;struct Cyc_Absyn_Vardecl*_tmp1CD;struct Cyc_Absyn_Vardecl*_tmp1CC;struct _tuple0*_tmp1CB;switch(*((int*)_tmp1CA)){case 0: _LL12D: _tmp1CB=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp1CA)->f1;_LL12E:
 return _tmp1CB;case 1: _LL12F: _tmp1CC=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1CA)->f1;_LL130:
 _tmp1CD=_tmp1CC;goto _LL132;case 3: _LL131: _tmp1CD=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp1CA)->f1;_LL132:
 _tmp1CE=_tmp1CD;goto _LL134;case 4: _LL133: _tmp1CE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1CA)->f1;_LL134:
 _tmp1CF=_tmp1CE;goto _LL136;case 5: _LL135: _tmp1CF=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1CA)->f1;_LL136:
 return _tmp1CF->name;default: _LL137: _tmp1D0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1CA)->f1;_LL138:
 return _tmp1D0->name;}_LL12C:;}
# 1102
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14};
# 1105
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;

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
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 482
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 490
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 943
void*Cyc_Absyn_compress_kb(void*);
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 972
extern void*Cyc_Absyn_bounds_one;
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1159
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1165
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1173
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 78
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 86
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 31 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 42
static int Cyc_Absyndump_expand_typedefs;
# 46
static int Cyc_Absyndump_qvar_to_Cids;
# 49
static int Cyc_Absyndump_add_cyc_prefix;
# 52
static int Cyc_Absyndump_generate_line_directives;
# 56
static int Cyc_Absyndump_to_VC;
# 58
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 73 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 76
void Cyc_Absyndump_dumptyp(void*);
void Cyc_Absyndump_dumpntyp(void*t);
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int,int do_semi);
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 91
typedef struct _dyneither_ptr Cyc_Absyndump_dump_string_t;
void Cyc_Absyndump_dumploc(unsigned int);
# 95
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 97
void Cyc_Absyndump_ignore(void*x){return;}
# 99
static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';
# 102
int Cyc_Absyndump_need_space_before(){
char _tmp0=Cyc_Absyndump_prev_char;switch(_tmp0){case 123: _LL1: _LL2:
 goto _LL4;case 125: _LL3: _LL4:
 goto _LL6;case 40: _LL5: _LL6:
 goto _LL8;case 41: _LL7: _LL8:
 goto _LLA;case 91: _LL9: _LLA:
 goto _LLC;case 93: _LLB: _LLC:
 goto _LLE;case 59: _LLD: _LLE:
 goto _LL10;case 44: _LLF: _LL10:
 goto _LL12;case 61: _LL11: _LL12:
 goto _LL14;case 63: _LL13: _LL14:
 goto _LL16;case 33: _LL15: _LL16:
 goto _LL18;case 32: _LL17: _LL18:
 goto _LL1A;case 10: _LL19: _LL1A:
 goto _LL1C;case 42: _LL1B: _LL1C:
 return 0;default: _LL1D: _LL1E:
 return 1;}_LL0:;}
# 122
void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 129
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= 1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - 1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 137
void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 140
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 145
void Cyc_Absyndump_dump_char(int c){
# 147
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1]="";
# 151
void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpE,_tmpE,_tmpE + 1};
static unsigned int last_line=0;
if(loc == 0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 157
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmpD;unsigned int _tmpC;_LL20: _tmpD=_tmp2.f1;_tmpC=_tmp2.f2;_LL21:;
if(({char*_tmp2F6=(char*)_tmpD.curr;_tmp2F6 == (char*)(_tag_dyneither(0,0,0)).curr;}) || (char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line)return;
if((char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line + 1)
({void*_tmp3=0;({struct Cyc___cycFILE*_tmp2F8=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2F7=({const char*_tmp4="\n";_tag_dyneither(_tmp4,sizeof(char),2);});Cyc_fprintf(_tmp2F8,_tmp2F7,_tag_dyneither(_tmp3,sizeof(void*),0));});});else{
if((char*)_tmpD.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp7;_tmp7.tag=1;_tmp7.f1=(unsigned long)((int)_tmpC);({void*_tmp5[1]={& _tmp7};({struct Cyc___cycFILE*_tmp2FA=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2F9=({const char*_tmp6="\n# %d\n";_tag_dyneither(_tmp6,sizeof(char),7);});Cyc_fprintf(_tmp2FA,_tmp2F9,_tag_dyneither(_tmp5,sizeof(void*),1));});});});else{
# 164
({struct Cyc_String_pa_PrintArg_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD);({struct Cyc_Int_pa_PrintArg_struct _tmpA;_tmpA.tag=1;_tmpA.f1=(unsigned long)((int)_tmpC);({void*_tmp8[2]={& _tmpA,& _tmpB};({struct Cyc___cycFILE*_tmp2FC=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2FB=({const char*_tmp9="\n# %d %s\n";_tag_dyneither(_tmp9,sizeof(char),10);});Cyc_fprintf(_tmp2FC,_tmp2FB,_tag_dyneither(_tmp8,sizeof(void*),2));});});});});}}
# 166
last_file=_tmpD;
last_line=_tmpC;};}
# 172
void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 176
void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 180
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 212
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 218
void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpF=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _tmp11=_tmp10;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp12;switch((_tmp11.C_n).tag){case 4: _LL23: _LL24:
 _tmp12=0;goto _LL26;case 1: _LL25: _tmp12=(_tmp11.Rel_n).val;_LL26:
# 224
 _tmpF=_tmp12;
goto _LL22;case 3: _LL27: _tmp13=(_tmp11.C_n).val;_LL28:
# 230
 Cyc_Absyndump_dump_char((int)' ');
goto _LL22;default: _LL29: _tmp14=(_tmp11.Abs_n).val;_LL2A:
# 233
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpF=_tmp14;
goto _LL22;}_LL22:;}
# 238
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp15="::";_tag_dyneither(_tmp15,sizeof(char),3);}));}}
# 243
if(_tmpF != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));
for(_tmpF=_tmpF->tl;_tmpF != 0;_tmpF=_tmpF->tl){
# 248
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp16="::";_tag_dyneither(_tmp16,sizeof(char),3);}));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));}
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const char*_tmp17="_";_tag_dyneither(_tmp17,sizeof(char),2);}));else{
Cyc_Absyndump_dump_nospace(({const char*_tmp18="::";_tag_dyneither(_tmp18,sizeof(char),3);}));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 256
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 259
Cyc_Absyndump_dump_str((*v).f2);}}}
# 263
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp19="restrict";_tag_dyneither(_tmp19,sizeof(char),9);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp1A="volatile";_tag_dyneither(_tmp1A,sizeof(char),9);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp1B="const";_tag_dyneither(_tmp1B,sizeof(char),6);}));}
# 269
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1C=sc;switch(_tmp1C){case Cyc_Absyn_Static: _LL2C: _LL2D:
 Cyc_Absyndump_dump(({const char*_tmp1D="static";_tag_dyneither(_tmp1D,sizeof(char),7);}));return;case Cyc_Absyn_Public: _LL2E: _LL2F:
 return;case Cyc_Absyn_Extern: _LL30: _LL31:
 Cyc_Absyndump_dump(({const char*_tmp1E="extern";_tag_dyneither(_tmp1E,sizeof(char),7);}));return;case Cyc_Absyn_ExternC: _LL32: _LL33:
 Cyc_Absyndump_dump(({const char*_tmp1F="extern \"C\"";_tag_dyneither(_tmp1F,sizeof(char),11);}));return;case Cyc_Absyn_Abstract: _LL34: _LL35:
 Cyc_Absyndump_dump(({const char*_tmp20="abstract";_tag_dyneither(_tmp20,sizeof(char),9);}));return;default: _LL36: _LL37:
 Cyc_Absyndump_dump(({const char*_tmp21="register";_tag_dyneither(_tmp21,sizeof(char),9);}));return;}_LL2B:;}
# 280
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp22=ka;enum Cyc_Absyn_KindQual _tmp37;enum Cyc_Absyn_AliasQual _tmp36;_LL39: _tmp37=_tmp22->kind;_tmp36=_tmp22->aliasqual;_LL3A:;{
enum Cyc_Absyn_KindQual _tmp23=_tmp37;switch(_tmp23){case Cyc_Absyn_AnyKind: _LL3C: _LL3D: {
# 284
enum Cyc_Absyn_AliasQual _tmp24=_tmp36;switch(_tmp24){case Cyc_Absyn_Aliasable: _LL49: _LL4A:
 Cyc_Absyndump_dump(({const char*_tmp25="A";_tag_dyneither(_tmp25,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL4B: _LL4C:
 Cyc_Absyndump_dump(({const char*_tmp26="UA";_tag_dyneither(_tmp26,sizeof(char),3);}));return;default: _LL4D: _LL4E:
 Cyc_Absyndump_dump(({const char*_tmp27="TA";_tag_dyneither(_tmp27,sizeof(char),3);}));return;}_LL48:;}case Cyc_Absyn_MemKind: _LL3E: _LL3F: {
# 290
enum Cyc_Absyn_AliasQual _tmp28=_tmp36;switch(_tmp28){case Cyc_Absyn_Aliasable: _LL50: _LL51:
 Cyc_Absyndump_dump(({const char*_tmp29="M";_tag_dyneither(_tmp29,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL52: _LL53:
 Cyc_Absyndump_dump(({const char*_tmp2A="UM";_tag_dyneither(_tmp2A,sizeof(char),3);}));return;default: _LL54: _LL55:
 Cyc_Absyndump_dump(({const char*_tmp2B="TM";_tag_dyneither(_tmp2B,sizeof(char),3);}));return;}_LL4F:;}case Cyc_Absyn_BoxKind: _LL40: _LL41: {
# 296
enum Cyc_Absyn_AliasQual _tmp2C=_tmp36;switch(_tmp2C){case Cyc_Absyn_Aliasable: _LL57: _LL58:
 Cyc_Absyndump_dump(({const char*_tmp2D="B";_tag_dyneither(_tmp2D,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL59: _LL5A:
 Cyc_Absyndump_dump(({const char*_tmp2E="UB";_tag_dyneither(_tmp2E,sizeof(char),3);}));return;default: _LL5B: _LL5C:
 Cyc_Absyndump_dump(({const char*_tmp2F="TB";_tag_dyneither(_tmp2F,sizeof(char),3);}));return;}_LL56:;}case Cyc_Absyn_RgnKind: _LL42: _LL43: {
# 302
enum Cyc_Absyn_AliasQual _tmp30=_tmp36;switch(_tmp30){case Cyc_Absyn_Aliasable: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmp31="R";_tag_dyneither(_tmp31,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmp32="UR";_tag_dyneither(_tmp32,sizeof(char),3);}));return;default: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmp33="TR";_tag_dyneither(_tmp33,sizeof(char),3);}));return;}_LL5D:;}case Cyc_Absyn_EffKind: _LL44: _LL45:
# 307
 Cyc_Absyndump_dump(({const char*_tmp34="E";_tag_dyneither(_tmp34,sizeof(char),2);}));return;default: _LL46: _LL47:
 Cyc_Absyndump_dump(({const char*_tmp35="I";_tag_dyneither(_tmp35,sizeof(char),2);}));return;}_LL3B:;};}
# 312
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp38=k;if(_tmp38 == Cyc_Absyn_StructA){_LL65: _LL66:
 Cyc_Absyndump_dump(({const char*_tmp39="struct ";_tag_dyneither(_tmp39,sizeof(char),8);}));return;}else{_LL67: _LL68:
 Cyc_Absyndump_dump(({const char*_tmp3A="union ";_tag_dyneither(_tmp3A,sizeof(char),7);}));return;}_LL64:;}
# 319
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp2FF=ts;struct _dyneither_ptr _tmp2FE=({const char*_tmp3B="<";_tag_dyneither(_tmp3B,sizeof(char),2);});struct _dyneither_ptr _tmp2FD=({const char*_tmp3C=">";_tag_dyneither(_tmp3C,sizeof(char),2);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp2FF,_tmp2FE,_tmp2FD,({const char*_tmp3D=",";_tag_dyneither(_tmp3D,sizeof(char),2);}));});}
# 322
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
Cyc_Absyndump_dump(({const char*_tmp3E="`G";_tag_dyneither(_tmp3E,sizeof(char),3);}));
{void*_tmp3F=Cyc_Absyn_compress_kb(tv->kind);void*_tmp40=_tmp3F;struct Cyc_Absyn_Kind*_tmp43;struct Cyc_Absyn_Kind*_tmp42;switch(*((int*)_tmp40)){case 0: _LL6A: _tmp42=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp40)->f1;_LL6B:
 _tmp43=_tmp42;goto _LL6D;case 2: _LL6C: _tmp43=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp40)->f2;_LL6D:
 Cyc_Absyndump_dumpkind(_tmp43);goto _LL69;default: _LL6E: _LL6F:
 Cyc_Absyndump_dump_nospace(({const char*_tmp41="K";_tag_dyneither(_tmp41,sizeof(char),2);}));goto _LL69;}_LL69:;}
# 331
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 333
Cyc_Absyndump_dump(n);}}
# 335
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp44=Cyc_Absyn_compress_kb(tv->kind);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp48;struct Cyc_Absyn_Kind*_tmp47;switch(*((int*)_tmp45)){case 1: _LL71: _LL72:
 goto _LL70;case 2: _LL73: _tmp47=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL74:
 goto _LL70;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL75: _LL76:
 goto _LL70;}else{goto _LL77;}}else{_LL77: _tmp48=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1;_LL78:
 Cyc_Absyndump_dump(({const char*_tmp46="::";_tag_dyneither(_tmp46,sizeof(char),3);}));Cyc_Absyndump_dumpkind(_tmp48);goto _LL70;}}_LL70:;};}
# 344
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp302=tvs;struct _dyneither_ptr _tmp301=({const char*_tmp49="<";_tag_dyneither(_tmp49,sizeof(char),2);});struct _dyneither_ptr _tmp300=({const char*_tmp4A=">";_tag_dyneither(_tmp4A,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp302,_tmp301,_tmp300,({const char*_tmp4B=",";_tag_dyneither(_tmp4B,sizeof(char),2);}));});}
# 347
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp305=tvs;struct _dyneither_ptr _tmp304=({const char*_tmp4C="<";_tag_dyneither(_tmp4C,sizeof(char),2);});struct _dyneither_ptr _tmp303=({const char*_tmp4D=">";_tag_dyneither(_tmp4D,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp305,_tmp304,_tmp303,({const char*_tmp4E=",";_tag_dyneither(_tmp4E,sizeof(char),2);}));});}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 351
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 354
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp308=ts;struct _dyneither_ptr _tmp307=({const char*_tmp4F="(";_tag_dyneither(_tmp4F,sizeof(char),2);});struct _dyneither_ptr _tmp306=({const char*_tmp50=")";_tag_dyneither(_tmp50,sizeof(char),2);});((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp308,_tmp307,_tmp306,({const char*_tmp51=",";_tag_dyneither(_tmp51,sizeof(char),2);}));});}
# 358
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp52=(void*)atts->hd;void*_tmp53=_tmp52;switch(*((int*)_tmp53)){case 1: _LL7A: _LL7B:
 Cyc_Absyndump_dump(({const char*_tmp54="_stdcall";_tag_dyneither(_tmp54,sizeof(char),9);}));return;case 2: _LL7C: _LL7D:
 Cyc_Absyndump_dump(({const char*_tmp55="_cdecl";_tag_dyneither(_tmp55,sizeof(char),7);}));return;case 3: _LL7E: _LL7F:
 Cyc_Absyndump_dump(({const char*_tmp56="_fastcall";_tag_dyneither(_tmp56,sizeof(char),10);}));return;default: _LL80: _LL81:
 goto _LL79;}_LL79:;}}
# 368
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 370
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp57=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp58=_tmp57;switch(*((int*)_tmp58)){case 1: _LL83: _LL84:
 goto _LL86;case 2: _LL85: _LL86:
 goto _LL88;case 3: _LL87: _LL88:
 goto _LL82;default: _LL89: _LL8A:
 hasatt=1;goto _LL82;}_LL82:;}}
# 378
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp59="__declspec(";_tag_dyneither(_tmp59,sizeof(char),12);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp5A=(void*)atts->hd;void*_tmp5B=_tmp5A;switch(*((int*)_tmp5B)){case 1: _LL8C: _LL8D:
 goto _LL8F;case 2: _LL8E: _LL8F:
 goto _LL91;case 3: _LL90: _LL91:
 goto _LL8B;default: _LL92: _LL93:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL8B;}_LL8B:;}
# 388
Cyc_Absyndump_dump_char((int)')');}
# 391
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp5C=Cyc_Cyclone_c_compiler;if(_tmp5C == Cyc_Cyclone_Gcc_c){_LL95: _LL96:
# 395
 Cyc_Absyndump_dump(({const char*_tmp5D=" __attribute__((";_tag_dyneither(_tmp5D,sizeof(char),17);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp5E=",";_tag_dyneither(_tmp5E,sizeof(char),2);}));}
# 400
Cyc_Absyndump_dump(({const char*_tmp5F=")) ";_tag_dyneither(_tmp5F,sizeof(char),4);}));
return;}else{_LL97: _LL98:
# 403
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL94:;};}
# 408
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 412
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 416
{void*_tmp60=Cyc_Tcutil_compress(t);void*_tmp61=_tmp60;struct Cyc_List_List*_tmp69;void*_tmp68;switch(*((int*)_tmp61)){case 23: _LL9A: _tmp68=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp61)->f1;_LL9B:
({struct Cyc_List_List*_tmp309=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=_tmp68;_tmp62->tl=rgions;_tmp62;});rgions=_tmp309;});goto _LL99;case 24: _LL9C: _tmp69=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp61)->f1;_LL9D:
# 419
 for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
struct _tuple15 _tmp63=Cyc_Absyndump_effects_split((void*)_tmp69->hd);struct _tuple15 _tmp64=_tmp63;struct Cyc_List_List*_tmp66;struct Cyc_List_List*_tmp65;_LLA1: _tmp66=_tmp64.f1;_tmp65=_tmp64.f2;_LLA2:;
({struct Cyc_List_List*_tmp30A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp66,rgions);rgions=_tmp30A;});
({struct Cyc_List_List*_tmp30B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp65,effects);effects=_tmp30B;});}
# 424
goto _LL99;default: _LL9E: _LL9F:
({struct Cyc_List_List*_tmp30C=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=t;_tmp67->tl=effects;_tmp67;});effects=_tmp30C;});goto _LL99;}_LL99:;}
# 427
return({struct _tuple15 _tmp6A;_tmp6A.f1=rgions;_tmp6A.f2=effects;_tmp6A;});}
# 430
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp6B=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp6C=_tmp6B;struct Cyc_List_List*_tmp6F;struct Cyc_List_List*_tmp6E;_LLA4: _tmp6F=_tmp6C.f1;_tmp6E=_tmp6C.f2;_LLA5:;
({struct Cyc_List_List*_tmp30D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F);_tmp6F=_tmp30D;});
({struct Cyc_List_List*_tmp30E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E);_tmp6E=_tmp30E;});
if(_tmp6E != 0){
{struct Cyc_List_List*_tmp6D=_tmp6E;for(0;_tmp6D != 0;_tmp6D=_tmp6D->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp6D->hd);
if(_tmp6D->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 440
if(_tmp6F != 0)Cyc_Absyndump_dump_char((int)'+');}
# 442
if(_tmp6F != 0  || _tmp6E == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp6F != 0;_tmp6F=_tmp6F->tl){
Cyc_Absyndump_dumprgn((void*)_tmp6F->hd);
if(_tmp6F->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 448
Cyc_Absyndump_dump_char((int)'}');}}
# 452
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 457
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp70=t;struct Cyc_List_List*_tmpD4;void*_tmpD3;void*_tmpD2;struct Cyc_Absyn_Datatypedecl*_tmpD1;struct Cyc_Absyn_Enumdecl*_tmpD0;struct Cyc_Absyn_Aggrdecl*_tmpCF;void*_tmpCE;void*_tmpCD;void*_tmpCC;void*_tmpCB;struct _dyneither_ptr _tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;struct _tuple0*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_List_List*_tmpC5;struct _tuple0*_tmpC4;enum Cyc_Absyn_AggrKind _tmpC3;struct Cyc_List_List*_tmpC2;union Cyc_Absyn_AggrInfoU _tmpC1;struct Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpBF;int _tmpBE;union Cyc_Absyn_DatatypeFieldInfoU _tmpBD;struct Cyc_List_List*_tmpBC;union Cyc_Absyn_DatatypeInfoU _tmpBB;struct Cyc_List_List*_tmpBA;struct Cyc_Core_Opt*_tmpB9;void*_tmpB8;int _tmpB7;struct Cyc_Core_Opt*_tmpB6;int _tmpB5;struct Cyc_Absyn_Tvar*_tmpB4;switch(*((int*)_tmp70)){case 8: _LLA7: _LLA8:
# 460
 goto _LLAA;case 9: _LLA9: _LLAA:
 goto _LLAC;case 5: _LLAB: _LLAC:
 return;case 0: _LLAD: _LLAE:
 Cyc_Absyndump_dump(({const char*_tmp71="void";_tag_dyneither(_tmp71,sizeof(char),5);}));return;case 2: _LLAF: _tmpB4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->f1;_LLB0:
 Cyc_Absyndump_dumptvar(_tmpB4);return;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f2 == 0){_LLB1: _tmpB6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f1;_tmpB5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f3;_LLB2:
# 466
 Cyc_Absyndump_dump(({const char*_tmp72="`E";_tag_dyneither(_tmp72,sizeof(char),3);}));
if(_tmpB6 == 0)Cyc_Absyndump_dump(({const char*_tmp73="K";_tag_dyneither(_tmp73,sizeof(char),2);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmpB6->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp76;_tmp76.tag=1;_tmp76.f1=(unsigned long)_tmpB5;({void*_tmp74[1]={& _tmp76};({struct _dyneither_ptr _tmp30F=({const char*_tmp75="%d";_tag_dyneither(_tmp75,sizeof(char),3);});Cyc_aprintf(_tmp30F,_tag_dyneither(_tmp74,sizeof(void*),1));});});}));return;}else{_LLB3: _tmpB9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f1;_tmpB8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f2;_tmpB7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f3;_LLB4:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmpB8));return;}case 3: _LLB5: _tmpBB=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp70)->f1).datatype_info;_tmpBA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp70)->f1).targs;_LLB6:
# 471
{union Cyc_Absyn_DatatypeInfoU _tmp77=_tmpBB;struct _tuple0*_tmp7D;int _tmp7C;struct _tuple0*_tmp7B;int _tmp7A;if((_tmp77.UnknownDatatype).tag == 1){_LL104: _tmp7B=((_tmp77.UnknownDatatype).val).name;_tmp7A=((_tmp77.UnknownDatatype).val).is_extensible;_LL105:
 _tmp7D=_tmp7B;_tmp7C=_tmp7A;goto _LL107;}else{_LL106: _tmp7D=(*(_tmp77.KnownDatatype).val)->name;_tmp7C=(*(_tmp77.KnownDatatype).val)->is_extensible;_LL107:
# 474
 if(_tmp7C)Cyc_Absyndump_dump(({const char*_tmp78="@extensible ";_tag_dyneither(_tmp78,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp79="datatype ";_tag_dyneither(_tmp79,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp7D);
Cyc_Absyndump_dumptps(_tmpBA);
goto _LL103;}_LL103:;}
# 480
goto _LLA6;case 4: _LLB7: _tmpBD=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->f1).field_info;_tmpBC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->f1).targs;_LLB8:
# 482
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7E=_tmpBD;struct _tuple0*_tmp87;int _tmp86;struct _tuple0*_tmp85;struct _tuple0*_tmp84;struct _tuple0*_tmp83;int _tmp82;if((_tmp7E.UnknownDatatypefield).tag == 1){_LL109: _tmp84=((_tmp7E.UnknownDatatypefield).val).datatype_name;_tmp83=((_tmp7E.UnknownDatatypefield).val).field_name;_tmp82=((_tmp7E.UnknownDatatypefield).val).is_extensible;_LL10A:
# 484
 _tmp87=_tmp84;_tmp86=_tmp82;_tmp85=_tmp83;goto _LL10C;}else{_LL10B: _tmp87=(((_tmp7E.KnownDatatypefield).val).f1)->name;_tmp86=(((_tmp7E.KnownDatatypefield).val).f1)->is_extensible;_tmp85=(((_tmp7E.KnownDatatypefield).val).f2)->name;_LL10C:
# 487
 if(_tmp86)Cyc_Absyndump_dump(({const char*_tmp7F="@extensible ";_tag_dyneither(_tmp7F,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp80="datatype ";_tag_dyneither(_tmp80,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp87);
Cyc_Absyndump_dump(({const char*_tmp81=".";_tag_dyneither(_tmp81,sizeof(char),2);}));
Cyc_Absyndump_dumpqvar(_tmp85);
Cyc_Absyndump_dumptps(_tmpBC);
goto _LL108;}_LL108:;}
# 495
goto _LLA6;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70)->f2){case Cyc_Absyn_Int_sz: _LLB9: _LLBA:
 goto _LLBC;case Cyc_Absyn_Long_sz: _LLBD: _LLBE:
# 498
 goto _LLC0;case Cyc_Absyn_Char_sz: _LLC1: _LLC2:
# 500
 Cyc_Absyndump_dump(({const char*_tmp8A="char";_tag_dyneither(_tmp8A,sizeof(char),5);}));return;case Cyc_Absyn_Short_sz: _LLC7: _LLC8:
# 503
 goto _LLCA;default: _LLD1: _LLD2:
# 508
 goto _LLD4;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70)->f2){case Cyc_Absyn_Int_sz: _LLBB: _LLBC:
# 497
 Cyc_Absyndump_dump(({const char*_tmp88="int";_tag_dyneither(_tmp88,sizeof(char),4);}));return;case Cyc_Absyn_Long_sz: _LLBF: _LLC0:
# 499
 Cyc_Absyndump_dump(({const char*_tmp89="long";_tag_dyneither(_tmp89,sizeof(char),5);}));return;case Cyc_Absyn_Char_sz: _LLC3: _LLC4:
# 501
 Cyc_Absyndump_dump(({const char*_tmp8B="signed char";_tag_dyneither(_tmp8B,sizeof(char),12);}));return;case Cyc_Absyn_Short_sz: _LLC9: _LLCA:
# 504
 Cyc_Absyndump_dump(({const char*_tmp8D="short";_tag_dyneither(_tmp8D,sizeof(char),6);}));return;default: _LLD3: _LLD4: {
# 510
enum Cyc_Cyclone_C_Compilers _tmp91=Cyc_Cyclone_c_compiler;if(_tmp91 == Cyc_Cyclone_Gcc_c){_LL10E: _LL10F:
 Cyc_Absyndump_dump(({const char*_tmp92="long long";_tag_dyneither(_tmp92,sizeof(char),10);}));return;}else{_LL110: _LL111:
 Cyc_Absyndump_dump(({const char*_tmp93="__int64";_tag_dyneither(_tmp93,sizeof(char),8);}));return;}_LL10D:;}}default: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70)->f2){case Cyc_Absyn_Char_sz: _LLC5: _LLC6:
# 502
 Cyc_Absyndump_dump(({const char*_tmp8C="unsigned char";_tag_dyneither(_tmp8C,sizeof(char),14);}));return;case Cyc_Absyn_Short_sz: _LLCB: _LLCC:
# 505
 Cyc_Absyndump_dump(({const char*_tmp8E="unsigned short";_tag_dyneither(_tmp8E,sizeof(char),15);}));return;case Cyc_Absyn_Int_sz: _LLCD: _LLCE:
 Cyc_Absyndump_dump(({const char*_tmp8F="unsigned int";_tag_dyneither(_tmp8F,sizeof(char),13);}));return;case Cyc_Absyn_Long_sz: _LLCF: _LLD0:
 Cyc_Absyndump_dump(({const char*_tmp90="unsigned long";_tag_dyneither(_tmp90,sizeof(char),14);}));return;default: _LLD5: _LLD6: {
# 515
enum Cyc_Cyclone_C_Compilers _tmp94=Cyc_Cyclone_c_compiler;if(_tmp94 == Cyc_Cyclone_Vc_c){_LL113: _LL114:
 Cyc_Absyndump_dump(({const char*_tmp95="unsigned __int64";_tag_dyneither(_tmp95,sizeof(char),17);}));return;}else{_LL115: _LL116:
 Cyc_Absyndump_dump(({const char*_tmp96="unsigned long long";_tag_dyneither(_tmp96,sizeof(char),19);}));return;}_LL112:;}}}case 7: _LLD7: _tmpBE=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70)->f1;_LLD8:
# 520
 if(_tmpBE == 0)
Cyc_Absyndump_dump(({const char*_tmp97="float";_tag_dyneither(_tmp97,sizeof(char),6);}));else{
if(_tmpBE == 1)
Cyc_Absyndump_dump(({const char*_tmp98="double";_tag_dyneither(_tmp98,sizeof(char),7);}));else{
# 525
Cyc_Absyndump_dump(({const char*_tmp99="long double";_tag_dyneither(_tmp99,sizeof(char),12);}));}}
return;case 10: _LLD9: _tmpBF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp70)->f1;_LLDA:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpBF);return;case 11: _LLDB: _tmpC1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp70)->f1).aggr_info;_tmpC0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp70)->f1).targs;_LLDC: {
# 529
struct _tuple10 _tmp9A=Cyc_Absyn_aggr_kinded_name(_tmpC1);struct _tuple10 _tmp9B=_tmp9A;enum Cyc_Absyn_AggrKind _tmp9D;struct _tuple0*_tmp9C;_LL118: _tmp9D=_tmp9B.f1;_tmp9C=_tmp9B.f2;_LL119:;
Cyc_Absyndump_dumpaggr_kind(_tmp9D);Cyc_Absyndump_dumpqvar(_tmp9C);Cyc_Absyndump_dumptps(_tmpC0);
return;}case 12: _LLDD: _tmpC3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp70)->f1;_tmpC2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp70)->f2;_LLDE:
# 533
 Cyc_Absyndump_dumpaggr_kind(_tmpC3);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpC2);Cyc_Absyndump_dump_char((int)'}');return;case 13: _LLDF: _tmpC4=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp70)->f1;_LLE0:
 Cyc_Absyndump_dump(({const char*_tmp9E="enum ";_tag_dyneither(_tmp9E,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmpC4);return;case 14: _LLE1: _tmpC5=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp70)->f1;_LLE2:
 Cyc_Absyndump_dump(({const char*_tmp9F="enum {";_tag_dyneither(_tmp9F,sizeof(char),7);}));Cyc_Absyndump_dumpenumfields(_tmpC5);Cyc_Absyndump_dump(({const char*_tmpA0="}";_tag_dyneither(_tmpA0,sizeof(char),2);}));return;case 17: _LLE3: _tmpC7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp70)->f1;_tmpC6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp70)->f2;_LLE4:
(Cyc_Absyndump_dumpqvar(_tmpC7),Cyc_Absyndump_dumptps(_tmpC6));return;case 18: _LLE5: _tmpC8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp70)->f1;_LLE6:
 Cyc_Absyndump_dump(({const char*_tmpA1="valueof_t(";_tag_dyneither(_tmpA1,sizeof(char),11);}));Cyc_Absyndump_dumpexp(_tmpC8);Cyc_Absyndump_dump(({const char*_tmpA2=")";_tag_dyneither(_tmpA2,sizeof(char),2);}));return;case 27: _LLE7: _tmpC9=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp70)->f1;_LLE8:
 Cyc_Absyndump_dump(({const char*_tmpA3="typeof(";_tag_dyneither(_tmpA3,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmpC9);Cyc_Absyndump_dump(({const char*_tmpA4=")";_tag_dyneither(_tmpA4,sizeof(char),2);}));return;case 28: _LLE9: _tmpCA=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp70)->f1;_LLEA:
 Cyc_Absyndump_dump(_tmpCA);return;case 15: _LLEB: _tmpCB=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp70)->f1;_LLEC:
# 541
 Cyc_Absyndump_dump(({const char*_tmpA5="region_t<";_tag_dyneither(_tmpA5,sizeof(char),10);}));Cyc_Absyndump_dumprgn(_tmpCB);Cyc_Absyndump_dump(({const char*_tmpA6=">";_tag_dyneither(_tmpA6,sizeof(char),2);}));return;case 16: _LLED: _tmpCD=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp70)->f1;_tmpCC=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp70)->f2;_LLEE:
 Cyc_Absyndump_dump(({const char*_tmpA7="dynregion_t<";_tag_dyneither(_tmpA7,sizeof(char),13);}));Cyc_Absyndump_dumprgn(_tmpCD);
Cyc_Absyndump_dump(({const char*_tmpA8=",";_tag_dyneither(_tmpA8,sizeof(char),2);}));Cyc_Absyndump_dumprgn(_tmpCC);Cyc_Absyndump_dump(({const char*_tmpA9=">";_tag_dyneither(_tmpA9,sizeof(char),2);}));return;case 19: _LLEF: _tmpCE=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp70)->f1;_LLF0:
 Cyc_Absyndump_dump(({const char*_tmpAA="tag_t<";_tag_dyneither(_tmpAA,sizeof(char),7);}));Cyc_Absyndump_dumpntyp(_tmpCE);Cyc_Absyndump_dump(({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2);}));return;case 21: _LLF1: _LLF2:
 Cyc_Absyndump_dump(({const char*_tmpAC="`U";_tag_dyneither(_tmpAC,sizeof(char),3);}));goto _LLA6;case 22: _LLF3: _LLF4:
 Cyc_Absyndump_dump(({const char*_tmpAD="`RC";_tag_dyneither(_tmpAD,sizeof(char),4);}));goto _LLA6;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)){case 0: _LLF5: _tmpCF=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)->f1;_LLF6:
# 548
 Cyc_Absyndump_dump_aggrdecl(_tmpCF);return;case 1: _LLF7: _tmpD0=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)->f1;_LLF8:
# 550
 Cyc_Absyndump_dump_enumdecl(_tmpD0);return;default: _LLF9: _tmpD1=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)->f1;_LLFA:
# 552
 Cyc_Absyndump_dump_datatypedecl(_tmpD1);return;}case 20: _LLFB: _LLFC:
 Cyc_Absyndump_dump(({const char*_tmpAE="`H";_tag_dyneither(_tmpAE,sizeof(char),3);}));goto _LLA6;case 23: _LLFD: _tmpD2=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp70)->f1;_LLFE:
 Cyc_Absyndump_dump(({const char*_tmpAF="{";_tag_dyneither(_tmpAF,sizeof(char),2);}));Cyc_Absyndump_dumptyp(_tmpD2);Cyc_Absyndump_dump(({const char*_tmpB0="}";_tag_dyneither(_tmpB0,sizeof(char),2);}));goto _LLA6;case 25: _LLFF: _tmpD3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp70)->f1;_LL100:
 Cyc_Absyndump_dump(({const char*_tmpB1="regions(";_tag_dyneither(_tmpB1,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmpD3);Cyc_Absyndump_dump(({const char*_tmpB2=")";_tag_dyneither(_tmpB2,sizeof(char),2);}));goto _LLA6;default: _LL101: _tmpD4=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp70)->f1;_LL102:
# 557
 for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){
Cyc_Absyndump_dumptyp((void*)_tmpD4->hd);
if(_tmpD4->tl != 0)Cyc_Absyndump_dump(({const char*_tmpB3="+";_tag_dyneither(_tmpB3,sizeof(char),2);}));}}_LLA6:;}
# 564
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 567
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 570
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpD5=cmp;void*_tmpD7;void*_tmpD6;_LL11B: _tmpD7=_tmpD5->f1;_tmpD6=_tmpD5->f2;_LL11C:;
Cyc_Absyndump_dumpeff(_tmpD7);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD6);}
# 574
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp310=rgn_po;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp310,({const char*_tmpD8=",";_tag_dyneither(_tmpD8,sizeof(char),2);}));});}
# 577
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 581
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 586
if(c_varargs)
Cyc_Absyndump_dump(({const char*_tmpD9="...";_tag_dyneither(_tmpD9,sizeof(char),4);}));else{
if(cyc_varargs != 0){
struct _tuple8*_tmpDA=({struct _tuple8*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->f1=cyc_varargs->name;_tmpDD->f2=cyc_varargs->tq;_tmpDD->f3=cyc_varargs->type;_tmpDD;});
Cyc_Absyndump_dump(({const char*_tmpDB="...";_tag_dyneither(_tmpDB,sizeof(char),4);}));
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpDC=" inject ";_tag_dyneither(_tmpDC,sizeof(char),9);}));
Cyc_Absyndump_dumpfunarg(_tmpDA);}}
# 594
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 598
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 602
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpDE=" @requires(";_tag_dyneither(_tmpDE,sizeof(char),12);}));
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 608
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpDF=" @ensures(";_tag_dyneither(_tmpDF,sizeof(char),11);}));
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 615
void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp311=Cyc_Absyn_empty_tqual(0);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp311,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 619
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpE0=d;struct _dyneither_ptr*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE0)->tag == 0){_LL11E: _tmpE2=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE0)->f1;_LL11F:
 Cyc_Absyndump_dump(({const char*_tmpE1=".[";_tag_dyneither(_tmpE1,sizeof(char),3);}));Cyc_Absyndump_dumpexp(_tmpE2);Cyc_Absyndump_dump_char((int)']');goto _LL11D;}else{_LL120: _tmpE3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE0)->f1;_LL121:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE3);goto _LL11D;}_LL11D:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 626
void Cyc_Absyndump_dumpde(struct _tuple17*de){
({struct Cyc_List_List*_tmp314=(*de).f1;struct _dyneither_ptr _tmp313=({const char*_tmpE4="";_tag_dyneither(_tmpE4,sizeof(char),1);});struct _dyneither_ptr _tmp312=({const char*_tmpE5="=";_tag_dyneither(_tmpE5,sizeof(char),2);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp314,_tmp313,_tmp312,({const char*_tmpE6="=";_tag_dyneither(_tmpE6,sizeof(char),2);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 631
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp318=inprec;struct Cyc_List_List*_tmp317=es;struct _dyneither_ptr _tmp316=({const char*_tmpE7="";_tag_dyneither(_tmpE7,sizeof(char),1);});struct _dyneither_ptr _tmp315=({const char*_tmpE8="";_tag_dyneither(_tmpE8,sizeof(char),1);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp318,_tmp317,_tmp316,_tmp315,({const char*_tmpE9=",";_tag_dyneither(_tmpE9,sizeof(char),2);}));});}
# 635
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpEA="(";_tag_dyneither(_tmpEA,sizeof(char),2);}));
{void*_tmpEB=e->r;void*_tmpEC=_tmpEB;struct Cyc_Absyn_Stmt*_tmp193;struct Cyc_Core_Opt*_tmp192;struct Cyc_List_List*_tmp191;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_Absyn_Exp*_tmp18F;int _tmp18E;struct Cyc_Absyn_Exp*_tmp18D;void**_tmp18C;struct Cyc_Absyn_Exp*_tmp18B;int _tmp18A;struct Cyc_Absyn_Enumfield*_tmp189;struct Cyc_Absyn_Enumfield*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Datatypefield*_tmp186;struct Cyc_List_List*_tmp185;struct _tuple0*_tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Exp*_tmp181;void*_tmp180;struct Cyc_Absyn_Vardecl*_tmp17F;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_List_List*_tmp17C;struct _tuple8*_tmp17B;struct Cyc_List_List*_tmp17A;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*_tmp176;struct _dyneither_ptr*_tmp175;struct Cyc_Absyn_Exp*_tmp174;struct _dyneither_ptr*_tmp173;struct Cyc_Absyn_Exp*_tmp172;void*_tmp171;struct Cyc_List_List*_tmp170;struct Cyc_Absyn_Exp*_tmp16F;struct _dyneither_ptr*_tmp16E;int _tmp16D;struct _dyneither_ptr _tmp16C;void*_tmp16B;struct Cyc_Absyn_Exp*_tmp16A;void*_tmp169;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp166;void*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Core_Opt*_tmp150;struct Cyc_Absyn_Exp*_tmp14F;enum Cyc_Absyn_Primop _tmp14E;struct Cyc_List_List*_tmp14D;void*_tmp14C;struct _dyneither_ptr _tmp14B;struct _dyneither_ptr _tmp14A;struct _dyneither_ptr _tmp149;enum Cyc_Absyn_Sign _tmp148;long long _tmp147;int _tmp146;int _tmp145;int _tmp144;enum Cyc_Absyn_Sign _tmp143;short _tmp142;struct _dyneither_ptr _tmp141;enum Cyc_Absyn_Sign _tmp140;char _tmp13F;switch(*((int*)_tmpEC)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).String_c).tag){case 2: _LL123: _tmp140=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Char_c).val).f1;_tmp13F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Char_c).val).f2;_LL124:
# 641
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp13F));Cyc_Absyndump_dump_char((int)'\'');
goto _LL122;case 3: _LL125: _tmp141=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Wchar_c).val;_LL126:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp141);({void*_tmpED[1]={& _tmpEF};({struct _dyneither_ptr _tmp319=({const char*_tmpEE="L'%s'";_tag_dyneither(_tmpEE,sizeof(char),6);});Cyc_aprintf(_tmp319,_tag_dyneither(_tmpED,sizeof(void*),1));});});}));goto _LL122;case 4: _LL127: _tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Short_c).val).f1;_tmp142=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Short_c).val).f2;_LL128:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF2;_tmpF2.tag=1;_tmpF2.f1=(unsigned long)((int)_tmp142);({void*_tmpF0[1]={& _tmpF2};({struct _dyneither_ptr _tmp31A=({const char*_tmpF1="%d";_tag_dyneither(_tmpF1,sizeof(char),3);});Cyc_aprintf(_tmp31A,_tag_dyneither(_tmpF0,sizeof(void*),1));});});}));goto _LL122;case 5: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL129: _tmp144=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Int_c).val).f2;_LL12A:
 _tmp145=_tmp144;goto _LL12C;case Cyc_Absyn_Signed: _LL12B: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Int_c).val).f2;_LL12C:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF5;_tmpF5.tag=1;_tmpF5.f1=(unsigned long)_tmp145;({void*_tmpF3[1]={& _tmpF5};({struct _dyneither_ptr _tmp31B=({const char*_tmpF4="%d";_tag_dyneither(_tmpF4,sizeof(char),3);});Cyc_aprintf(_tmp31B,_tag_dyneither(_tmpF3,sizeof(void*),1));});});}));goto _LL122;default: _LL12D: _tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Int_c).val).f2;_LL12E:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF8;_tmpF8.tag=1;_tmpF8.f1=(unsigned int)_tmp146;({void*_tmpF6[1]={& _tmpF8};({struct _dyneither_ptr _tmp31C=({const char*_tmpF7="%u";_tag_dyneither(_tmpF7,sizeof(char),3);});Cyc_aprintf(_tmp31C,_tag_dyneither(_tmpF6,sizeof(void*),1));});});}));goto _LL122;}case 6: _LL12F: _tmp148=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).LongLong_c).val).f1;_tmp147=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).LongLong_c).val).f2;_LL130:
# 650
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp147));goto _LL122;case 7: _LL131: _tmp149=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Float_c).val).f1;_LL132:
 Cyc_Absyndump_dump(_tmp149);goto _LL122;case 1: _LL133: _LL134:
 Cyc_Absyndump_dump(({const char*_tmpF9="NULL";_tag_dyneither(_tmpF9,sizeof(char),5);}));goto _LL122;case 8: _LL135: _tmp14A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).String_c).val;_LL136:
# 654
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14A));Cyc_Absyndump_dump_char((int)'"');
goto _LL122;default: _LL137: _tmp14B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).Wstring_c).val;_LL138:
# 657
 Cyc_Absyndump_dump(({const char*_tmpFA="L\"";_tag_dyneither(_tmpFA,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(_tmp14B);Cyc_Absyndump_dump_char((int)'"');
goto _LL122;}case 1: _LL139: _tmp14C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL13A:
# 660
 Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(_tmp14C));goto _LL122;case 2: _LL13B: _tmp14E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp14D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL13C: {
# 663
struct _dyneither_ptr _tmpFB=Cyc_Absynpp_prim2str(_tmp14E);
{int _tmpFC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp14D);int _tmpFD=_tmpFC;switch(_tmpFD){case 1: _LL18E: _LL18F:
# 666
 if(_tmp14E == Cyc_Absyn_Numelts){
Cyc_Absyndump_dump(({const char*_tmpFE="numelts(";_tag_dyneither(_tmpFE,sizeof(char),9);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14D))->hd);
Cyc_Absyndump_dump(({const char*_tmpFF=")";_tag_dyneither(_tmpFF,sizeof(char),2);}));}else{
# 671
Cyc_Absyndump_dump(_tmpFB);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14D))->hd);}
# 674
goto _LL18D;case 2: _LL190: _LL191:
# 676
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14D))->hd);
Cyc_Absyndump_dump(_tmpFB);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14D->tl))->hd);
goto _LL18D;default: _LL192: _LL193:
# 682
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp100=_cycalloc(sizeof(*_tmp100));({struct Cyc_Core_Failure_exn_struct _tmp31E=({struct Cyc_Core_Failure_exn_struct _tmp101;_tmp101.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp31D=({const char*_tmp102="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmp102,sizeof(char),47);});_tmp101.f1=_tmp31D;});_tmp101;});_tmp100[0]=_tmp31E;});_tmp100;}));}_LL18D:;}
# 684
goto _LL122;}case 3: _LL13D: _tmp151=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp150=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_tmp14F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL13E:
# 687
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp151);
if(_tmp150 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp150->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp103="=";_tag_dyneither(_tmp103,sizeof(char),2);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp14F);
goto _LL122;case 4: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEC)->f2){case Cyc_Absyn_PreInc: _LL13F: _tmp152=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL140:
# 694
 Cyc_Absyndump_dump(({const char*_tmp104="++";_tag_dyneither(_tmp104,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp152);goto _LL122;case Cyc_Absyn_PreDec: _LL141: _tmp153=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL142:
 Cyc_Absyndump_dump(({const char*_tmp105="--";_tag_dyneither(_tmp105,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);goto _LL122;case Cyc_Absyn_PostInc: _LL143: _tmp154=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL144:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp154);Cyc_Absyndump_dump(({const char*_tmp106="++";_tag_dyneither(_tmp106,sizeof(char),3);}));goto _LL122;default: _LL145: _tmp155=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL146:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp155);Cyc_Absyndump_dump(({const char*_tmp107="--";_tag_dyneither(_tmp107,sizeof(char),3);}));goto _LL122;}case 5: _LL147: _tmp158=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp157=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_tmp156=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL148:
# 700
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp158);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp157);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp156);
goto _LL122;case 6: _LL149: _tmp15A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp159=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL14A:
# 707
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);Cyc_Absyndump_dump(({const char*_tmp108=" && ";_tag_dyneither(_tmp108,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp159);goto _LL122;case 7: _LL14B: _tmp15C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp15B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL14C:
# 711
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15C);Cyc_Absyndump_dump(({const char*_tmp109=" || ";_tag_dyneither(_tmp109,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);goto _LL122;case 8: _LL14D: _tmp15E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp15D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL14E:
# 717
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15E);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);
Cyc_Absyndump_dump_char((int)')');
goto _LL122;case 9: _LL14F: _tmp160=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp15F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL150:
# 724
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp160);
Cyc_Absyndump_dump_nospace(({const char*_tmp10A="(";_tag_dyneither(_tmp10A,sizeof(char),2);}));{
# 730
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp160->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp15F);
Cyc_Absyndump_dump_nospace(({const char*_tmp10B=")";_tag_dyneither(_tmp10B,sizeof(char),2);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL122;};case 10: _LL151: _tmp161=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL152:
# 739
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump(({const char*_tmp10C="throw";_tag_dyneither(_tmp10C,sizeof(char),6);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);goto _LL122;case 11: _LL153: _tmp162=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL154:
# 742
 _tmp163=_tmp162;goto _LL156;case 12: _LL155: _tmp163=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL156:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp163);goto _LL122;case 13: _LL157: _tmp165=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp164=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL158:
# 746
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp165);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp164);
goto _LL122;case 14: _LL159: _tmp166=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL15A:
# 750
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp166);goto _LL122;case 15: _LL15B: _tmp168=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp167=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL15C:
# 753
 Cyc_Absyndump_dump(({const char*_tmp10D="new ";_tag_dyneither(_tmp10D,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp167);goto _LL122;case 16: _LL15D: _tmp169=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL15E:
# 756
 Cyc_Absyndump_dump(({const char*_tmp10E="sizeof(";_tag_dyneither(_tmp10E,sizeof(char),8);}));Cyc_Absyndump_dumptyp(_tmp169);Cyc_Absyndump_dump_char((int)')');goto _LL122;case 17: _LL15F: _tmp16A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL160:
# 759
 Cyc_Absyndump_dump(({const char*_tmp10F="sizeof(";_tag_dyneither(_tmp10F,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp16A);Cyc_Absyndump_dump_char((int)')');goto _LL122;case 38: _LL161: _tmp16B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL162:
# 762
 Cyc_Absyndump_dump(({const char*_tmp110="valueof(";_tag_dyneither(_tmp110,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp16B);Cyc_Absyndump_dump_char((int)')');goto _LL122;case 39: _LL163: _tmp16D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp16C=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL164:
# 765
 Cyc_Absyndump_dump(({const char*_tmp111="__asm__";_tag_dyneither(_tmp111,sizeof(char),8);}));
if(_tmp16D)Cyc_Absyndump_dump(({const char*_tmp112=" volatile ";_tag_dyneither(_tmp112,sizeof(char),11);}));
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp16C);
Cyc_Absyndump_dump_char((int)')');
goto _LL122;case 37: _LL165: _tmp16F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp16E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL166:
# 773
 Cyc_Absyndump_dump(({const char*_tmp113="tagcheck(";_tag_dyneither(_tmp113,sizeof(char),10);}));Cyc_Absyndump_dumpexp(_tmp16F);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp16E);
Cyc_Absyndump_dump_char((int)')');
goto _LL122;case 18: _LL167: _tmp171=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp170=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL168:
# 778
 Cyc_Absyndump_dump(({const char*_tmp114="offsetof(";_tag_dyneither(_tmp114,sizeof(char),10);}));Cyc_Absyndump_dumptyp(_tmp171);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp170 != 0;_tmp170=_tmp170->tl){
{void*_tmp115=(void*)_tmp170->hd;void*_tmp116=_tmp115;unsigned int _tmp11B;struct _dyneither_ptr*_tmp11A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp116)->tag == 0){_LL195: _tmp11A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp116)->f1;_LL196:
# 782
 Cyc_Absyndump_dump_nospace(*_tmp11A);
goto _LL194;}else{_LL197: _tmp11B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp116)->f1;_LL198:
# 785
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp119;_tmp119.tag=1;_tmp119.f1=(unsigned long)((int)_tmp11B);({void*_tmp117[1]={& _tmp119};({struct _dyneither_ptr _tmp31F=({const char*_tmp118="%d";_tag_dyneither(_tmp118,sizeof(char),3);});Cyc_aprintf(_tmp31F,_tag_dyneither(_tmp117,sizeof(void*),1));});});}));
goto _LL194;}_LL194:;}
# 788
if(_tmp170->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 790
Cyc_Absyndump_dump_char((int)')');
goto _LL122;case 19: _LL169: _tmp172=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL16A:
# 794
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp172);goto _LL122;case 20: _LL16B: _tmp174=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp173=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL16C:
# 797
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp174);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp173);
goto _LL122;case 21: _LL16D: _tmp176=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp175=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL16E:
# 801
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp176);Cyc_Absyndump_dump_nospace(({const char*_tmp11C="->";_tag_dyneither(_tmp11C,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(*_tmp175);
goto _LL122;case 22: _LL16F: _tmp178=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp177=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL170:
# 805
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp178);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp177);Cyc_Absyndump_dump_char((int)']');goto _LL122;case 23: _LL171: _tmp179=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL172:
# 809
 Cyc_Absyndump_dump(({const char*_tmp11D="$(";_tag_dyneither(_tmp11D,sizeof(char),3);}));Cyc_Absyndump_dumpexps_prec(20,_tmp179);Cyc_Absyndump_dump_char((int)')');goto _LL122;case 24: _LL173: _tmp17B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp17A=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL174:
# 812
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp17B).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp322=_tmp17A;struct _dyneither_ptr _tmp321=({const char*_tmp11E="{";_tag_dyneither(_tmp11E,sizeof(char),2);});struct _dyneither_ptr _tmp320=({const char*_tmp11F="}";_tag_dyneither(_tmp11F,sizeof(char),2);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp322,_tmp321,_tmp320,({const char*_tmp120=",";_tag_dyneither(_tmp120,sizeof(char),2);}));});
goto _LL122;case 25: _LL175: _tmp17C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL176:
# 819
({struct Cyc_List_List*_tmp325=_tmp17C;struct _dyneither_ptr _tmp324=({const char*_tmp121="{";_tag_dyneither(_tmp121,sizeof(char),2);});struct _dyneither_ptr _tmp323=({const char*_tmp122="}";_tag_dyneither(_tmp122,sizeof(char),2);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp325,_tmp324,_tmp323,({const char*_tmp123=",";_tag_dyneither(_tmp123,sizeof(char),2);}));});
goto _LL122;case 26: _LL177: _tmp17F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp17E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_tmp17D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL178:
# 823
 Cyc_Absyndump_dump(({const char*_tmp124="{for";_tag_dyneither(_tmp124,sizeof(char),5);}));Cyc_Absyndump_dump_str((*_tmp17F->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp17E);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp17D);Cyc_Absyndump_dump_char((int)'}');
goto _LL122;case 27: _LL179: _tmp181=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp180=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL17A:
# 828
 Cyc_Absyndump_dump(({const char*_tmp125="{for x ";_tag_dyneither(_tmp125,sizeof(char),8);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp181);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp180);Cyc_Absyndump_dump_char((int)'}');
goto _LL122;case 28: _LL17B: _tmp184=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp183=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_tmp182=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL17C:
# 833
 Cyc_Absyndump_dumpqvar(_tmp184);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp183 != 0)
Cyc_Absyndump_dumptps(_tmp183);
({struct Cyc_List_List*_tmp328=_tmp182;struct _dyneither_ptr _tmp327=({const char*_tmp126="";_tag_dyneither(_tmp126,sizeof(char),1);});struct _dyneither_ptr _tmp326=({const char*_tmp127="}";_tag_dyneither(_tmp127,sizeof(char),2);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp328,_tmp327,_tmp326,({const char*_tmp128=",";_tag_dyneither(_tmp128,sizeof(char),2);}));});
goto _LL122;case 29: _LL17D: _tmp185=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL17E:
# 841
({struct Cyc_List_List*_tmp32B=_tmp185;struct _dyneither_ptr _tmp32A=({const char*_tmp129="{";_tag_dyneither(_tmp129,sizeof(char),2);});struct _dyneither_ptr _tmp329=({const char*_tmp12A="}";_tag_dyneither(_tmp12A,sizeof(char),2);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp32B,_tmp32A,_tmp329,({const char*_tmp12B=",";_tag_dyneither(_tmp12B,sizeof(char),2);}));});
goto _LL122;case 30: _LL17F: _tmp187=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp186=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL180:
# 845
 Cyc_Absyndump_dumpqvar(_tmp186->name);
if(_tmp187 != 0)({struct Cyc_List_List*_tmp32E=_tmp187;struct _dyneither_ptr _tmp32D=({const char*_tmp12C="(";_tag_dyneither(_tmp12C,sizeof(char),2);});struct _dyneither_ptr _tmp32C=({const char*_tmp12D=")";_tag_dyneither(_tmp12D,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp32E,_tmp32D,_tmp32C,({const char*_tmp12E=",";_tag_dyneither(_tmp12E,sizeof(char),2);}));});
goto _LL122;case 31: _LL181: _tmp188=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL182:
# 849
 Cyc_Absyndump_dumpqvar(_tmp188->name);goto _LL122;case 32: _LL183: _tmp189=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL184:
 Cyc_Absyndump_dumpqvar(_tmp189->name);goto _LL122;case 33: _LL185: _tmp18E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).is_calloc;_tmp18D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).rgn;_tmp18C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).elt_type;_tmp18B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).num_elts;_tmp18A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).inline_call;_LL186:
# 853
 Cyc_Absyndump_dumploc(_tmp18B->loc);
if(_tmp18E){
if(_tmp18D != 0){
Cyc_Absyndump_dump(({const char*_tmp12F="rcalloc(";_tag_dyneither(_tmp12F,sizeof(char),9);}));
Cyc_Absyndump_dumpexp(_tmp18D);Cyc_Absyndump_dump(({const char*_tmp130=",";_tag_dyneither(_tmp130,sizeof(char),2);}));}else{
# 859
Cyc_Absyndump_dump(({const char*_tmp131="calloc";_tag_dyneither(_tmp131,sizeof(char),7);}));}
# 861
Cyc_Absyndump_dumpexp(_tmp18B);
Cyc_Absyndump_dump(({const char*_tmp132=",";_tag_dyneither(_tmp132,sizeof(char),2);}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp18C)),0));
Cyc_Absyndump_dump(({const char*_tmp133=")";_tag_dyneither(_tmp133,sizeof(char),2);}));}else{
# 866
if(_tmp18D != 0){
if(_tmp18A)
Cyc_Absyndump_dump(({const char*_tmp134="rmalloc_inline(";_tag_dyneither(_tmp134,sizeof(char),16);}));else{
# 870
Cyc_Absyndump_dump(({const char*_tmp135="rmalloc(";_tag_dyneither(_tmp135,sizeof(char),9);}));}
Cyc_Absyndump_dumpexp(_tmp18D);Cyc_Absyndump_dump(({const char*_tmp136=",";_tag_dyneither(_tmp136,sizeof(char),2);}));}else{
# 873
Cyc_Absyndump_dump(({const char*_tmp137="malloc(";_tag_dyneither(_tmp137,sizeof(char),8);}));}
# 876
if(_tmp18C != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp32F=Cyc_Absyn_sizeoftyp_exp(*_tmp18C,0);Cyc_Absyn_times_exp(_tmp32F,_tmp18B,0);}));else{
# 879
Cyc_Absyndump_dumpexp(_tmp18B);}
Cyc_Absyndump_dump(({const char*_tmp138=")";_tag_dyneither(_tmp138,sizeof(char),2);}));}
# 882
goto _LL122;case 34: _LL187: _tmp190=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp18F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL188:
# 885
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp190);
Cyc_Absyndump_dump_nospace(({const char*_tmp139=":=:";_tag_dyneither(_tmp139,sizeof(char),4);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp18F);
goto _LL122;case 35: _LL189: _tmp192=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmp191=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL18A:
# 892
({struct Cyc_List_List*_tmp332=_tmp191;struct _dyneither_ptr _tmp331=({const char*_tmp13A="{";_tag_dyneither(_tmp13A,sizeof(char),2);});struct _dyneither_ptr _tmp330=({const char*_tmp13B="}";_tag_dyneither(_tmp13B,sizeof(char),2);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp332,_tmp331,_tmp330,({const char*_tmp13C=",";_tag_dyneither(_tmp13C,sizeof(char),2);}));});
goto _LL122;default: _LL18B: _tmp193=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL18C:
# 896
 Cyc_Absyndump_dump_nospace(({const char*_tmp13D="({";_tag_dyneither(_tmp13D,sizeof(char),3);}));
Cyc_Absyndump_dumpstmt(_tmp193,1);
Cyc_Absyndump_dump_nospace(({const char*_tmp13E="})";_tag_dyneither(_tmp13E,sizeof(char),3);}));
goto _LL122;}_LL122:;}
# 901
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 905
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 909
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp194=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp194->where_clause == 0  && (_tmp194->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp195="default:";_tag_dyneither(_tmp195,sizeof(char),9);}));else{
# 915
Cyc_Absyndump_dump(({const char*_tmp196="case";_tag_dyneither(_tmp196,sizeof(char),5);}));
Cyc_Absyndump_dumppat(_tmp194->pattern);
if(_tmp194->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp197="&&";_tag_dyneither(_tmp197,sizeof(char),3);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp194->where_clause));}
# 921
Cyc_Absyndump_dump_nospace(({const char*_tmp198=":";_tag_dyneither(_tmp198,sizeof(char),2);}));}
# 923
Cyc_Absyndump_dumpstmt(_tmp194->body,0);}}
# 927
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp199=s->r;void*_tmp19A=_tmp199;struct Cyc_Absyn_Stmt*_tmp1D0;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Stmt*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct _dyneither_ptr*_tmp1CB;struct Cyc_Absyn_Stmt*_tmp1CA;struct Cyc_Absyn_Decl*_tmp1C9;struct Cyc_Absyn_Stmt*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Stmt*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Stmt*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Stmt*_tmp1BD;struct Cyc_Absyn_Stmt*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Stmt*_tmp1BA;struct Cyc_Absyn_Stmt*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;switch(*((int*)_tmp19A)){case 0: _LL19A: _LL19B:
 Cyc_Absyndump_dump_semi();goto _LL199;case 1: _LL19C: _tmp1B8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_LL19D:
 Cyc_Absyndump_dumpexp(_tmp1B8);Cyc_Absyndump_dump_semi();goto _LL199;case 2: _LL19E: _tmp1BA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1B9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_LL19F:
# 936
 if(Cyc_Absynpp_is_declaration(_tmp1BA)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BA,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 939
Cyc_Absyndump_dumpstmt(_tmp1BA,0);}
if(Cyc_Absynpp_is_declaration(_tmp1B9)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1B9,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 946
Cyc_Absyndump_dumpstmt(_tmp1B9,expstmt);}
goto _LL199;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1 == 0){_LL1A0: _LL1A1:
 Cyc_Absyndump_dump(({const char*_tmp19B="return;";_tag_dyneither(_tmp19B,sizeof(char),8);}));goto _LL199;}else{_LL1A2: _tmp1BB=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_LL1A3:
 Cyc_Absyndump_dump(({const char*_tmp19C="return";_tag_dyneither(_tmp19C,sizeof(char),7);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1BB));Cyc_Absyndump_dump_semi();goto _LL199;}case 4: _LL1A4: _tmp1BE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1BD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_tmp1BC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19A)->f3;_LL1A5:
# 951
 Cyc_Absyndump_dump(({const char*_tmp19D="if(";_tag_dyneither(_tmp19D,sizeof(char),4);}));Cyc_Absyndump_dumpexp(_tmp1BE);
{void*_tmp19E=_tmp1BD->r;void*_tmp19F=_tmp19E;switch(*((int*)_tmp19F)){case 2: _LL1BF: _LL1C0:
 goto _LL1C2;case 12: _LL1C1: _LL1C2:
 goto _LL1C4;case 4: _LL1C3: _LL1C4:
 goto _LL1C6;case 13: _LL1C5: _LL1C6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp1A0="){";_tag_dyneither(_tmp1A0,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1BD,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1BE;default: _LL1C7: _LL1C8:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1BD,0);}_LL1BE:;}
# 959
{void*_tmp1A1=_tmp1BC->r;void*_tmp1A2=_tmp1A1;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1A2)->tag == 0){_LL1CA: _LL1CB:
 goto _LL1C9;}else{_LL1CC: _LL1CD:
 Cyc_Absyndump_dump(({const char*_tmp1A3="else{";_tag_dyneither(_tmp1A3,sizeof(char),6);}));Cyc_Absyndump_dumpstmt(_tmp1BC,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1C9;}_LL1C9:;}
# 963
goto _LL199;case 5: _LL1A6: _tmp1C0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1).f1;_tmp1BF=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_LL1A7:
# 965
 Cyc_Absyndump_dump(({const char*_tmp1A4="while(";_tag_dyneither(_tmp1A4,sizeof(char),7);}));Cyc_Absyndump_dumpexp(_tmp1C0);Cyc_Absyndump_dump_nospace(({const char*_tmp1A5="){";_tag_dyneither(_tmp1A5,sizeof(char),3);}));
Cyc_Absyndump_dumpstmt(_tmp1BF,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL199;case 6: _LL1A8: _LL1A9:
 Cyc_Absyndump_dump(({const char*_tmp1A6="break;";_tag_dyneither(_tmp1A6,sizeof(char),7);}));goto _LL199;case 7: _LL1AA: _LL1AB:
 Cyc_Absyndump_dump(({const char*_tmp1A7="continue;";_tag_dyneither(_tmp1A7,sizeof(char),10);}));goto _LL199;case 8: _LL1AC: _tmp1C1=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_LL1AD:
 Cyc_Absyndump_dump(({const char*_tmp1A8="goto";_tag_dyneither(_tmp1A8,sizeof(char),5);}));Cyc_Absyndump_dump_str(_tmp1C1);Cyc_Absyndump_dump_semi();goto _LL199;case 9: _LL1AE: _tmp1C5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1C4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2).f1;_tmp1C3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19A)->f3).f1;_tmp1C2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19A)->f4;_LL1AF:
# 974
 Cyc_Absyndump_dump(({const char*_tmp1A9="for(";_tag_dyneither(_tmp1A9,sizeof(char),5);}));Cyc_Absyndump_dumpexp(_tmp1C5);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C4);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C3);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AA="){";_tag_dyneither(_tmp1AA,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1C2,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL199;case 10: _LL1B0: _tmp1C7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1C6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_LL1B1:
# 978
 Cyc_Absyndump_dump(({const char*_tmp1AB="switch(";_tag_dyneither(_tmp1AB,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1C7);Cyc_Absyndump_dump_nospace(({const char*_tmp1AC="){";_tag_dyneither(_tmp1AC,sizeof(char),3);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1C6);
Cyc_Absyndump_dump_char((int)'}');
goto _LL199;case 12: _LL1B2: _tmp1C9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1C8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_LL1B3:
# 983
 Cyc_Absyndump_dumpdecl(_tmp1C9);Cyc_Absyndump_dumpstmt(_tmp1C8,expstmt);goto _LL199;case 13: _LL1B4: _tmp1CB=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1CA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_LL1B5:
# 989
 if(Cyc_Absynpp_is_declaration(_tmp1CA)){
Cyc_Absyndump_dump_str(_tmp1CB);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1AD=": ({";_tag_dyneither(_tmp1AD,sizeof(char),5);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1AE=": {";_tag_dyneither(_tmp1AE,sizeof(char),4);}));}
Cyc_Absyndump_dumpstmt(_tmp1CA,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1AF="});}";_tag_dyneither(_tmp1AF,sizeof(char),5);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 995
Cyc_Absyndump_dump_str(_tmp1CB);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1CA,expstmt);}
# 997
goto _LL199;case 14: _LL1B6: _tmp1CD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1CC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2).f1;_LL1B7:
# 1000
 Cyc_Absyndump_dump(({const char*_tmp1B0="do{";_tag_dyneither(_tmp1B0,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1CD,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B1="}while(";_tag_dyneither(_tmp1B1,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1CC);Cyc_Absyndump_dump_nospace(({const char*_tmp1B2=");";_tag_dyneither(_tmp1B2,sizeof(char),3);}));
goto _LL199;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1 == 0){_LL1B8: _LL1B9:
# 1004
 Cyc_Absyndump_dump(({const char*_tmp1B3="fallthru;";_tag_dyneither(_tmp1B3,sizeof(char),10);}));goto _LL199;}else{_LL1BA: _tmp1CE=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_LL1BB:
# 1006
 Cyc_Absyndump_dump(({const char*_tmp1B4="fallthru(";_tag_dyneither(_tmp1B4,sizeof(char),10);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1CE);Cyc_Absyndump_dump_nospace(({const char*_tmp1B5=");";_tag_dyneither(_tmp1B5,sizeof(char),3);}));goto _LL199;}default: _LL1BC: _tmp1D0=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19A)->f1;_tmp1CF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19A)->f2;_LL1BD:
# 1009
 Cyc_Absyndump_dump(({const char*_tmp1B6="try";_tag_dyneither(_tmp1B6,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1D0,0);
Cyc_Absyndump_dump(({const char*_tmp1B7="catch{";_tag_dyneither(_tmp1B7,sizeof(char),7);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1CF);Cyc_Absyndump_dump_char((int)'}');
goto _LL199;}_LL199:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1017
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
({struct Cyc_List_List*_tmp335=(*dp).f1;struct _dyneither_ptr _tmp334=({const char*_tmp1D1="";_tag_dyneither(_tmp1D1,sizeof(char),1);});struct _dyneither_ptr _tmp333=({const char*_tmp1D2="=";_tag_dyneither(_tmp1D2,sizeof(char),2);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp335,_tmp334,_tmp333,({const char*_tmp1D3="=";_tag_dyneither(_tmp1D3,sizeof(char),2);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 1022
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D4=p->r;void*_tmp1D5=_tmp1D4;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Enumfield*_tmp225;struct Cyc_Absyn_Enumfield*_tmp224;struct Cyc_Absyn_Datatypefield*_tmp223;struct Cyc_List_List*_tmp222;int _tmp221;struct Cyc_List_List*_tmp220;struct Cyc_List_List*_tmp21F;int _tmp21E;union Cyc_Absyn_AggrInfoU _tmp21D;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21B;int _tmp21A;struct _tuple0*_tmp219;struct Cyc_List_List*_tmp218;int _tmp217;struct _tuple0*_tmp216;struct Cyc_Absyn_Tvar*_tmp215;struct Cyc_Absyn_Vardecl*_tmp214;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Pat*_tmp212;struct Cyc_Absyn_Vardecl*_tmp211;struct Cyc_Absyn_Pat*_tmp210;struct Cyc_List_List*_tmp20F;int _tmp20E;struct Cyc_Absyn_Tvar*_tmp20D;struct Cyc_Absyn_Vardecl*_tmp20C;struct Cyc_Absyn_Vardecl*_tmp20B;struct Cyc_Absyn_Pat*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp209;struct _dyneither_ptr _tmp208;char _tmp207;int _tmp206;int _tmp205;int _tmp204;switch(*((int*)_tmp1D5)){case 0: _LL1CF: _LL1D0:
 Cyc_Absyndump_dump_char((int)'_');goto _LL1CE;case 9: _LL1D1: _LL1D2:
 Cyc_Absyndump_dump(({const char*_tmp1D6="NULL";_tag_dyneither(_tmp1D6,sizeof(char),5);}));goto _LL1CE;case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1){case Cyc_Absyn_None: _LL1D3: _tmp204=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1D4:
 _tmp205=_tmp204;goto _LL1D6;case Cyc_Absyn_Signed: _LL1D5: _tmp205=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1D6:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1D9;_tmp1D9.tag=1;_tmp1D9.f1=(unsigned long)_tmp205;({void*_tmp1D7[1]={& _tmp1D9};({struct _dyneither_ptr _tmp336=({const char*_tmp1D8="%d";_tag_dyneither(_tmp1D8,sizeof(char),3);});Cyc_aprintf(_tmp336,_tag_dyneither(_tmp1D7,sizeof(void*),1));});});}));goto _LL1CE;default: _LL1D7: _tmp206=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1D8:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1DC;_tmp1DC.tag=1;_tmp1DC.f1=(unsigned int)_tmp206;({void*_tmp1DA[1]={& _tmp1DC};({struct _dyneither_ptr _tmp337=({const char*_tmp1DB="%u";_tag_dyneither(_tmp1DB,sizeof(char),3);});Cyc_aprintf(_tmp337,_tag_dyneither(_tmp1DA,sizeof(void*),1));});});}));goto _LL1CE;}case 11: _LL1D9: _tmp207=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1DA:
# 1030
 Cyc_Absyndump_dump(({const char*_tmp1DD="'";_tag_dyneither(_tmp1DD,sizeof(char),2);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp207));Cyc_Absyndump_dump_nospace(({const char*_tmp1DE="'";_tag_dyneither(_tmp1DE,sizeof(char),2);}));goto _LL1CE;case 12: _LL1DB: _tmp208=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1DC:
 Cyc_Absyndump_dump(_tmp208);goto _LL1CE;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2)->r)->tag == 0){_LL1DD: _tmp209=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1DE:
 Cyc_Absyndump_dumpqvar(_tmp209->name);goto _LL1CE;}else{_LL1DF: _tmp20B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp20A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1E0:
 Cyc_Absyndump_dumpqvar(_tmp20B->name);Cyc_Absyndump_dump(({const char*_tmp1DF=" as ";_tag_dyneither(_tmp1DF,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp20A);goto _LL1CE;}case 2: _LL1E1: _tmp20D=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp20C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1E2:
# 1035
 Cyc_Absyndump_dump(({const char*_tmp1E0="alias";_tag_dyneither(_tmp1E0,sizeof(char),6);}));
Cyc_Absyndump_dump(({const char*_tmp1E1="<";_tag_dyneither(_tmp1E1,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp20D);Cyc_Absyndump_dump(({const char*_tmp1E2=">";_tag_dyneither(_tmp1E2,sizeof(char),2);}));
Cyc_Absyndump_dumpvardecl(_tmp20C,p->loc,0);
goto _LL1CE;case 5: _LL1E3: _tmp20F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp20E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1E4: {
# 1040
struct _dyneither_ptr term=_tmp20E?({const char*_tmp1E5=", ...)";_tag_dyneither(_tmp1E5,sizeof(char),7);}):({const char*_tmp1E6=")";_tag_dyneither(_tmp1E6,sizeof(char),2);});
({struct Cyc_List_List*_tmp33A=_tmp20F;struct _dyneither_ptr _tmp339=({const char*_tmp1E3="$(";_tag_dyneither(_tmp1E3,sizeof(char),3);});struct _dyneither_ptr _tmp338=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp33A,_tmp339,_tmp338,({const char*_tmp1E4=",";_tag_dyneither(_tmp1E4,sizeof(char),2);}));});goto _LL1CE;}case 6: _LL1E5: _tmp210=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1E6:
 Cyc_Absyndump_dump(({const char*_tmp1E7="&";_tag_dyneither(_tmp1E7,sizeof(char),2);}));Cyc_Absyndump_dumppat(_tmp210);goto _LL1CE;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2)->r)->tag == 0){_LL1E7: _tmp211=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1E8:
# 1044
 Cyc_Absyndump_dump(({const char*_tmp1E8="*";_tag_dyneither(_tmp1E8,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp211->name);goto _LL1CE;}else{_LL1E9: _tmp213=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp212=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1EA:
# 1046
 Cyc_Absyndump_dump(({const char*_tmp1E9="*";_tag_dyneither(_tmp1E9,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp213->name);Cyc_Absyndump_dump(({const char*_tmp1EA=" as ";_tag_dyneither(_tmp1EA,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp212);goto _LL1CE;}case 4: _LL1EB: _tmp215=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp214=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1EC:
# 1048
 Cyc_Absyndump_dumpqvar(_tmp214->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp215);Cyc_Absyndump_dump_char((int)'>');goto _LL1CE;case 15: _LL1ED: _tmp216=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1EE:
 Cyc_Absyndump_dumpqvar(_tmp216);goto _LL1CE;case 16: _LL1EF: _tmp219=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp218=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_tmp217=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL1F0: {
# 1051
struct _dyneither_ptr term=_tmp217?({const char*_tmp1ED=", ...)";_tag_dyneither(_tmp1ED,sizeof(char),7);}):({const char*_tmp1EE=")";_tag_dyneither(_tmp1EE,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp219);({struct Cyc_List_List*_tmp33D=_tmp218;struct _dyneither_ptr _tmp33C=({const char*_tmp1EB="(";_tag_dyneither(_tmp1EB,sizeof(char),2);});struct _dyneither_ptr _tmp33B=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp33D,_tmp33C,_tmp33B,({const char*_tmp1EC=",";_tag_dyneither(_tmp1EC,sizeof(char),2);}));});goto _LL1CE;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1 != 0){_LL1F1: _tmp21D=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1)->aggr_info;_tmp21C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_tmp21B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_tmp21A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f4;_LL1F2: {
# 1054
struct _tuple10 _tmp1EF=Cyc_Absyn_aggr_kinded_name(_tmp21D);struct _tuple10 _tmp1F0=_tmp1EF;struct _tuple0*_tmp1F8;_LL1FE: _tmp1F8=_tmp1F0.f2;_LL1FF:;{
struct _dyneither_ptr term=_tmp21A?({const char*_tmp1F6=", ...)";_tag_dyneither(_tmp1F6,sizeof(char),7);}):({const char*_tmp1F7=")";_tag_dyneither(_tmp1F7,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp1F8);Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp340=_tmp21C;struct _dyneither_ptr _tmp33F=({const char*_tmp1F1="[";_tag_dyneither(_tmp1F1,sizeof(char),2);});struct _dyneither_ptr _tmp33E=({const char*_tmp1F2="]";_tag_dyneither(_tmp1F2,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp340,_tmp33F,_tmp33E,({const char*_tmp1F3=",";_tag_dyneither(_tmp1F3,sizeof(char),2);}));});
({struct Cyc_List_List*_tmp343=_tmp21B;struct _dyneither_ptr _tmp342=({const char*_tmp1F4="";_tag_dyneither(_tmp1F4,sizeof(char),1);});struct _dyneither_ptr _tmp341=term;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp343,_tmp342,_tmp341,({const char*_tmp1F5=",";_tag_dyneither(_tmp1F5,sizeof(char),2);}));});
goto _LL1CE;};}}else{_LL1F3: _tmp220=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_tmp21F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_tmp21E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f4;_LL1F4: {
# 1061
struct _dyneither_ptr term=_tmp21E?({const char*_tmp1FE=", ...)";_tag_dyneither(_tmp1FE,sizeof(char),7);}):({const char*_tmp1FF=")";_tag_dyneither(_tmp1FF,sizeof(char),2);});
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp346=_tmp220;struct _dyneither_ptr _tmp345=({const char*_tmp1F9="[";_tag_dyneither(_tmp1F9,sizeof(char),2);});struct _dyneither_ptr _tmp344=({const char*_tmp1FA="]";_tag_dyneither(_tmp1FA,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp346,_tmp345,_tmp344,({const char*_tmp1FB=",";_tag_dyneither(_tmp1FB,sizeof(char),2);}));});
({struct Cyc_List_List*_tmp349=_tmp21F;struct _dyneither_ptr _tmp348=({const char*_tmp1FC="";_tag_dyneither(_tmp1FC,sizeof(char),1);});struct _dyneither_ptr _tmp347=term;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp349,_tmp348,_tmp347,({const char*_tmp1FD=",";_tag_dyneither(_tmp1FD,sizeof(char),2);}));});
goto _LL1CE;}}case 8: _LL1F5: _tmp223=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_tmp222=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_tmp221=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f4;_LL1F6: {
# 1067
struct _dyneither_ptr term=_tmp221?({const char*_tmp202=", ...)";_tag_dyneither(_tmp202,sizeof(char),7);}):({const char*_tmp203=")";_tag_dyneither(_tmp203,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp223->name);
if(_tmp222 != 0)({struct Cyc_List_List*_tmp34C=_tmp222;struct _dyneither_ptr _tmp34B=({const char*_tmp200="(";_tag_dyneither(_tmp200,sizeof(char),2);});struct _dyneither_ptr _tmp34A=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp34C,_tmp34B,_tmp34A,({const char*_tmp201=",";_tag_dyneither(_tmp201,sizeof(char),2);}));});
goto _LL1CE;}case 13: _LL1F7: _tmp224=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1F8:
 _tmp225=_tmp224;goto _LL1FA;case 14: _LL1F9: _tmp225=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL1FA:
 Cyc_Absyndump_dumpqvar(_tmp225->name);goto _LL1CE;default: _LL1FB: _tmp226=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL1FC:
 Cyc_Absyndump_dumpexp(_tmp226);goto _LL1CE;}_LL1CE:;}
# 1077
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1082
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp34D=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp34D,({const char*_tmp227=",";_tag_dyneither(_tmp227,sizeof(char),2);}));});}
# 1085
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp228=" = ";_tag_dyneither(_tmp228,sizeof(char),4);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1092
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp34E=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp34E,({const char*_tmp229=",";_tag_dyneither(_tmp229,sizeof(char),2);}));});}
# 1096
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp22A=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp22B=_tmp22A;struct _dyneither_ptr*_tmp233;struct Cyc_Absyn_Tqual _tmp232;void*_tmp231;struct Cyc_Absyn_Exp*_tmp230;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Exp*_tmp22E;_LL201: _tmp233=_tmp22B->name;_tmp232=_tmp22B->tq;_tmp231=_tmp22B->type;_tmp230=_tmp22B->width;_tmp22F=_tmp22B->attributes;_tmp22E=_tmp22B->requires_clause;_LL202:;
# 1100
{enum Cyc_Cyclone_C_Compilers _tmp22C=Cyc_Cyclone_c_compiler;if(_tmp22C == Cyc_Cyclone_Gcc_c){_LL204: _LL205:
# 1102
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp232,_tmp231,Cyc_Absyndump_dump_str,_tmp233);
Cyc_Absyndump_dumpatts(_tmp22F);
goto _LL203;}else{_LL206: _LL207:
# 1106
 Cyc_Absyndump_dumpatts(_tmp22F);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp232,_tmp231,Cyc_Absyndump_dump_str,_tmp233);
goto _LL203;}_LL203:;}
# 1110
if((unsigned int)_tmp22E){
Cyc_Absyndump_dump(({const char*_tmp22D="@requires ";_tag_dyneither(_tmp22D,sizeof(char),11);}));
Cyc_Absyndump_dumpexp(_tmp22E);}
# 1114
if(_tmp230 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp230);}
# 1118
Cyc_Absyndump_dump_semi();}}
# 1122
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1127
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1131
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp234=(*pr).f1;void*_tmp235=_tmp234;switch(*((int*)_tmp235)){case 11: _LL209: _LL20A:
 goto _LL208;case 1: _LL20B: _LL20C:
 Cyc_Absyndump_dump(({const char*_tmp236="_stdcall";_tag_dyneither(_tmp236,sizeof(char),9);}));goto _LL208;case 2: _LL20D: _LL20E:
 Cyc_Absyndump_dump(({const char*_tmp237="_cdecl";_tag_dyneither(_tmp237,sizeof(char),7);}));goto _LL208;case 3: _LL20F: _LL210:
 Cyc_Absyndump_dump(({const char*_tmp238="_fastcall";_tag_dyneither(_tmp238,sizeof(char),10);}));goto _LL208;default: _LL211: _LL212:
 goto _LL208;}_LL208:;}
# 1139
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1141
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1146
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1153
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1155
void*_tmp239=t;void*_tmp23E;void*_tmp23D;switch(*((int*)_tmp239)){case 1: _LL214: _tmp23D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp239)->f2;if(_tmp23D != 0){_LL215:
 return Cyc_Absyndump_is_char_ptr(_tmp23D);}else{goto _LL218;}case 5: _LL216: _tmp23E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp239)->f1).elt_typ;_LL217:
# 1158
 L: {
void*_tmp23A=_tmp23E;void*_tmp23C;void*_tmp23B;switch(*((int*)_tmp23A)){case 1: _LL21B: _tmp23B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23A)->f2;if(_tmp23B != 0){_LL21C:
 _tmp23E=_tmp23B;goto L;}else{goto _LL221;}case 17: _LL21D: _tmp23C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23A)->f4;if(_tmp23C != 0){_LL21E:
 _tmp23E=_tmp23C;goto L;}else{goto _LL221;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23A)->f2 == Cyc_Absyn_Char_sz){_LL21F: _LL220:
 return 1;}else{goto _LL221;}default: _LL221: _LL222:
 return 0;}_LL21A:;}default: _LL218: _LL219:
# 1165
 return 0;}_LL213:;}
# 1169
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp23F=vd;enum Cyc_Absyn_Scope _tmp256;struct _tuple0*_tmp255;unsigned int _tmp254;struct Cyc_Absyn_Tqual _tmp253;void*_tmp252;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_List_List*_tmp250;_LL224: _tmp256=_tmp23F->sc;_tmp255=_tmp23F->name;_tmp254=_tmp23F->varloc;_tmp253=_tmp23F->tq;_tmp252=_tmp23F->type;_tmp251=_tmp23F->initializer;_tmp250=_tmp23F->attributes;_LL225:;
{enum Cyc_Cyclone_C_Compilers _tmp240=Cyc_Cyclone_c_compiler;if(_tmp240 == Cyc_Cyclone_Gcc_c){_LL227: _LL228:
# 1174
 if(_tmp256 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp241=Cyc_Tcutil_compress(_tmp252);void*_tmp242=_tmp241;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->tag == 9){_LL22C: _LL22D:
 goto _LL22B;}else{_LL22E: _LL22F:
 Cyc_Absyndump_dumpscope(_tmp256);}_LL22B:;}else{
# 1180
Cyc_Absyndump_dumpscope(_tmp256);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp253,_tmp252,Cyc_Absyndump_dumpqvar,_tmp255);
Cyc_Absyndump_dumpatts(_tmp250);
goto _LL226;}else{_LL229: _LL22A:
# 1185
 Cyc_Absyndump_dumpatts(_tmp250);
Cyc_Absyndump_dumpscope(_tmp256);{
struct _RegionHandle _tmp243=_new_region("temp");struct _RegionHandle*temp=& _tmp243;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp252);
struct _tuple11 _tmp244=Cyc_Absynpp_to_tms(temp,_tmp253,_tmp252);struct _tuple11 _tmp245=_tmp244;struct Cyc_Absyn_Tqual _tmp24F;void*_tmp24E;struct Cyc_List_List*_tmp24D;_LL231: _tmp24F=_tmp245.f1;_tmp24E=_tmp245.f2;_tmp24D=_tmp245.f3;_LL232:;{
# 1191
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp24D;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp246=(void*)tms2->hd;void*_tmp247=_tmp246;struct Cyc_List_List*_tmp24A;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp247)->tag == 5){_LL234: _tmp24A=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp247)->f2;_LL235:
# 1195
 for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){
void*_tmp248=(void*)_tmp24A->hd;void*_tmp249=_tmp248;switch(*((int*)_tmp249)){case 1: _LL239: _LL23A:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL238;case 2: _LL23B: _LL23C:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL238;case 3: _LL23D: _LL23E:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL238;default: _LL23F: _LL240:
 goto _LL238;}_LL238:;}
# 1202
goto _LL233;}else{_LL236: _LL237:
 goto _LL233;}_LL233:;}}
# 1205
Cyc_Absyndump_dumptq(_tmp24F);
Cyc_Absyndump_dumpntyp(_tmp24E);{
struct _tuple19 _tmp24B=({struct _tuple19 _tmp24C;_tmp24C.f1=call_conv;_tmp24C.f2=_tmp255;_tmp24C;});
({int _tmp34F=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(_tmp34F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp24D),Cyc_Absyndump_dump_callconv_qvar,& _tmp24B);});};};}
# 1210
_npop_handler(0);goto _LL226;
# 1187
;_pop_region(temp);};}_LL226:;}
# 1213
if(_tmp251 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp251);}
# 1217
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1220
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp257="@tagged ";_tag_dyneither(_tmp257,sizeof(char),9);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1229
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_List_List*_tmp352=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _dyneither_ptr _tmp351=({const char*_tmp258="<";_tag_dyneither(_tmp258,sizeof(char),2);});struct _dyneither_ptr _tmp350=({const char*_tmp259=">";_tag_dyneither(_tmp259,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp352,_tmp351,_tmp350,({const char*_tmp25A=",";_tag_dyneither(_tmp25A,sizeof(char),2);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1236
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump(({const char*_tmp25B="}";_tag_dyneither(_tmp25B,sizeof(char),2);}));
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1241
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp25C=ed;enum Cyc_Absyn_Scope _tmp261;struct _tuple0*_tmp260;struct Cyc_Core_Opt*_tmp25F;_LL242: _tmp261=_tmp25C->sc;_tmp260=_tmp25C->name;_tmp25F=_tmp25C->fields;_LL243:;
Cyc_Absyndump_dumpscope(_tmp261);
Cyc_Absyndump_dump(({const char*_tmp25D="enum ";_tag_dyneither(_tmp25D,sizeof(char),6);}));
Cyc_Absyndump_dumpqvar(_tmp260);
if(_tmp25F != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp25F->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp25E="}";_tag_dyneither(_tmp25E,sizeof(char),2);}));}}
# 1252
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp262=dd;enum Cyc_Absyn_Scope _tmp26A;struct _tuple0*_tmp269;struct Cyc_List_List*_tmp268;struct Cyc_Core_Opt*_tmp267;int _tmp266;_LL245: _tmp26A=_tmp262->sc;_tmp269=_tmp262->name;_tmp268=_tmp262->tvs;_tmp267=_tmp262->fields;_tmp266=_tmp262->is_extensible;_LL246:;
Cyc_Absyndump_dumpscope(_tmp26A);
if(_tmp266)Cyc_Absyndump_dump(({const char*_tmp263="@extensible ";_tag_dyneither(_tmp263,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp264="datatype ";_tag_dyneither(_tmp264,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp269);
Cyc_Absyndump_dumptvars(_tmp268);
if(_tmp267 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp267->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp265="}";_tag_dyneither(_tmp265,sizeof(char),2);}));}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1266
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp26B=d->r;void*_tmp26C=_tmp26B;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp299;struct _tuple0*_tmp298;struct Cyc_List_List*_tmp297;struct _dyneither_ptr*_tmp296;struct Cyc_List_List*_tmp295;struct Cyc_Absyn_Typedefdecl*_tmp294;struct Cyc_Absyn_Tvar*_tmp293;struct Cyc_Absyn_Vardecl*_tmp292;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_List_List*_tmp290;struct Cyc_Absyn_Pat*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Enumdecl*_tmp28D;struct Cyc_Absyn_Datatypedecl*_tmp28C;struct Cyc_Absyn_Aggrdecl*_tmp28B;struct Cyc_Absyn_Fndecl*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp289;switch(*((int*)_tmp26C)){case 0: _LL248: _tmp289=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL249:
# 1270
 Cyc_Absyndump_dumpvardecl(_tmp289,d->loc,1);
goto _LL247;case 1: _LL24A: _tmp28A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL24B:
# 1274
{enum Cyc_Cyclone_C_Compilers _tmp26D=Cyc_Cyclone_c_compiler;if(_tmp26D == Cyc_Cyclone_Vc_c){_LL267: _LL268:
 Cyc_Absyndump_dumpatts(_tmp28A->attributes);goto _LL266;}else{_LL269: _LL26A:
 goto _LL266;}_LL266:;}
# 1278
if(_tmp28A->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp26E=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp271;if(_tmp26E == Cyc_Cyclone_Vc_c){_LL26C: _LL26D:
 Cyc_Absyndump_dump(({const char*_tmp26F="__inline";_tag_dyneither(_tmp26F,sizeof(char),9);}));goto _LL26B;}else{_LL26E: _tmp271=_tmp26E;_LL26F:
 Cyc_Absyndump_dump(({const char*_tmp270="inline";_tag_dyneither(_tmp270,sizeof(char),7);}));goto _LL26B;}_LL26B:;}
# 1283
Cyc_Absyndump_dumpscope(_tmp28A->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp273=_cycalloc(sizeof(*_tmp273));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp355=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp274;_tmp274.tag=9;({struct Cyc_Absyn_FnInfo _tmp354=({struct Cyc_Absyn_FnInfo _tmp275;_tmp275.tvars=_tmp28A->tvs;_tmp275.effect=_tmp28A->effect;_tmp275.ret_tqual=_tmp28A->ret_tqual;_tmp275.ret_typ=_tmp28A->ret_type;({struct Cyc_List_List*_tmp353=
# 1286
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp28A->args);_tmp275.args=_tmp353;});_tmp275.c_varargs=_tmp28A->c_varargs;_tmp275.cyc_varargs=_tmp28A->cyc_varargs;_tmp275.rgn_po=_tmp28A->rgn_po;_tmp275.attributes=0;_tmp275.requires_clause=_tmp28A->requires_clause;_tmp275.requires_relns=_tmp28A->requires_relns;_tmp275.ensures_clause=_tmp28A->ensures_clause;_tmp275.ensures_relns=_tmp28A->ensures_relns;_tmp275;});_tmp274.f1=_tmp354;});_tmp274;});_tmp273[0]=_tmp355;});_tmp273;});
# 1291
{enum Cyc_Cyclone_C_Compilers _tmp272=Cyc_Cyclone_c_compiler;if(_tmp272 == Cyc_Cyclone_Gcc_c){_LL271: _LL272:
# 1293
({struct Cyc_Absyn_Tqual _tmp357=Cyc_Absyn_empty_tqual(0);void*_tmp356=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp357,_tmp356,Cyc_Absyndump_dump_atts_qvar,_tmp28A);});
goto _LL270;}else{_LL273: _LL274:
# 1296
({struct Cyc_Absyn_Tqual _tmp359=Cyc_Absyn_empty_tqual(0);void*_tmp358=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp359,_tmp358,Cyc_Absyndump_dump_callconv_fdqvar,_tmp28A);});
goto _LL270;}_LL270:;}
# 1299
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp28A->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL247;};case 5: _LL24C: _tmp28B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL24D:
# 1304
 Cyc_Absyndump_dump_aggrdecl(_tmp28B);Cyc_Absyndump_dump_semi();
goto _LL247;case 6: _LL24E: _tmp28C=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL24F:
# 1307
 Cyc_Absyndump_dump_datatypedecl(_tmp28C);Cyc_Absyndump_dump_semi();
goto _LL247;case 7: _LL250: _tmp28D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL251:
# 1310
 Cyc_Absyndump_dump_enumdecl(_tmp28D);Cyc_Absyndump_dump_semi();
goto _LL247;case 2: _LL252: _tmp28F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_tmp28E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp26C)->f3;_LL253:
# 1313
 Cyc_Absyndump_dump(({const char*_tmp276="let";_tag_dyneither(_tmp276,sizeof(char),4);}));
Cyc_Absyndump_dumppat(_tmp28F);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp28E);
Cyc_Absyndump_dump_semi();
goto _LL247;case 3: _LL254: _tmp290=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL255:
# 1320
 Cyc_Absyndump_dump(({const char*_tmp277="let ";_tag_dyneither(_tmp277,sizeof(char),5);}));
Cyc_Absyndump_dumpids(_tmp290);
Cyc_Absyndump_dump_semi();
goto _LL247;case 4: _LL256: _tmp293=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_tmp292=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp26C)->f2;_tmp291=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp26C)->f3;_LL257:
# 1325
 Cyc_Absyndump_dump(({const char*_tmp278="region";_tag_dyneither(_tmp278,sizeof(char),7);}));
Cyc_Absyndump_dump(({const char*_tmp279="<";_tag_dyneither(_tmp279,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp293);Cyc_Absyndump_dump(({const char*_tmp27A="> ";_tag_dyneither(_tmp27A,sizeof(char),3);}));Cyc_Absyndump_dumpqvar(_tmp292->name);
if((unsigned int)_tmp291){
Cyc_Absyndump_dump(({const char*_tmp27B=" = open(";_tag_dyneither(_tmp27B,sizeof(char),9);}));
Cyc_Absyndump_dumpexp(_tmp291);
Cyc_Absyndump_dump(({const char*_tmp27C=")";_tag_dyneither(_tmp27C,sizeof(char),2);}));}
# 1332
Cyc_Absyndump_dump_semi();
goto _LL247;case 8: _LL258: _tmp294=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL259:
# 1339
 Cyc_Absyndump_dump(({const char*_tmp27D="typedef";_tag_dyneither(_tmp27D,sizeof(char),8);}));{
void*t;
if(_tmp294->defn == 0)
({void*_tmp35A=Cyc_Absyn_new_evar(_tmp294->kind,0);t=_tmp35A;});else{
# 1344
t=(void*)_check_null(_tmp294->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp294->tq,t,Cyc_Absyndump_dumptypedefname,_tmp294);
Cyc_Absyndump_dumpatts(_tmp294->atts);
Cyc_Absyndump_dump_semi();
# 1349
goto _LL247;};case 9: _LL25A: _tmp296=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_tmp295=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp26C)->f2;_LL25B:
# 1351
 Cyc_Absyndump_dump(({const char*_tmp27E="namespace ";_tag_dyneither(_tmp27E,sizeof(char),11);}));
Cyc_Absyndump_dump_str(_tmp296);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp295 != 0;_tmp295=_tmp295->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp295->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL247;case 10: _LL25C: _tmp298=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_tmp297=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp26C)->f2;_LL25D:
# 1359
 Cyc_Absyndump_dump(({const char*_tmp27F="using ";_tag_dyneither(_tmp27F,sizeof(char),7);}));
Cyc_Absyndump_dumpqvar(_tmp298);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp297 != 0;_tmp297=_tmp297->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp297->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL247;case 11: _LL25E: _tmp299=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_LL25F:
# 1367
 Cyc_Absyndump_dump(({const char*_tmp280="extern \"C\" {";_tag_dyneither(_tmp280,sizeof(char),13);}));
for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp299->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL247;case 12: _LL260: _tmp29B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp26C)->f1;_tmp29A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp26C)->f2;_LL261:
# 1373
 Cyc_Absyndump_dump(({const char*_tmp281="extern \"C include\" {";_tag_dyneither(_tmp281,sizeof(char),21);}));
for(0;_tmp29B != 0;_tmp29B=_tmp29B->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29B->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp29A != 0){
Cyc_Absyndump_dump(({const char*_tmp282=" export {";_tag_dyneither(_tmp282,sizeof(char),10);}));
for(0;_tmp29A != 0;_tmp29A=_tmp29A->tl){
struct _tuple20 _tmp283=*((struct _tuple20*)_tmp29A->hd);struct _tuple20 _tmp284=_tmp283;struct _tuple0*_tmp285;_LL276: _tmp285=_tmp284.f2;_LL277:;
Cyc_Absyndump_dumpqvar(_tmp285);
if(_tmp29A->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 1384
Cyc_Absyndump_dump(({const char*_tmp286="}";_tag_dyneither(_tmp286,sizeof(char),2);}));}
# 1386
goto _LL247;case 13: _LL262: _LL263:
# 1388
 Cyc_Absyndump_dump(({const char*_tmp287=" __cyclone_port_on__; ";_tag_dyneither(_tmp287,sizeof(char),23);}));
goto _LL247;default: _LL264: _LL265:
# 1391
 Cyc_Absyndump_dump(({const char*_tmp288=" __cyclone_port_off__; ";_tag_dyneither(_tmp288,sizeof(char),24);}));
goto _LL247;}_LL247:;};}
# 1396
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1403
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1408
void*_tmp29C=(void*)tms->hd;void*_tmp29D=_tmp29C;void*_tmp2CD;union Cyc_Absyn_Constraint*_tmp2CC;union Cyc_Absyn_Constraint*_tmp2CB;union Cyc_Absyn_Constraint*_tmp2CA;struct Cyc_Absyn_Tqual _tmp2C9;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29D)->tag == 2){_LL279: _tmp2CD=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29D)->f1).rgn;_tmp2CC=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29D)->f1).nullable;_tmp2CB=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29D)->f1).bounds;_tmp2CA=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29D)->f1).zero_term;_tmp2C9=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29D)->f2;_LL27A:
# 1412
{void*_tmp29E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2CB);void*_tmp29F=_tmp29E;struct Cyc_Absyn_Exp*_tmp2A0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29F)->tag == 0){_LL27E: _LL27F:
 Cyc_Absyndump_dump_char((int)'?');goto _LL27D;}else{_LL280: _tmp2A0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29F)->f1;_LL281:
# 1415
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp2CC)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp2A0);
goto _LL27D;}_LL27D:;}
# 1419
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CA))Cyc_Absyndump_dump(({const char*_tmp2A1="@zeroterm";_tag_dyneither(_tmp2A1,sizeof(char),10);}));
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CA))Cyc_Absyndump_dump(({const char*_tmp2A2="@nozeroterm";_tag_dyneither(_tmp2A2,sizeof(char),12);}));
{void*_tmp2A3=Cyc_Tcutil_compress(_tmp2CD);void*_tmp2A4=_tmp2A3;struct Cyc_Absyn_Tvar*_tmp2AC;switch(*((int*)_tmp2A4)){case 20: _LL283: _LL284:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2A5="`H";_tag_dyneither(_tmp2A5,sizeof(char),3);}));goto _LL282;case 21: _LL285: _LL286:
 Cyc_Absyndump_dump(({const char*_tmp2A6="`U";_tag_dyneither(_tmp2A6,sizeof(char),3);}));goto _LL282;case 22: _LL287: _LL288:
 Cyc_Absyndump_dump(({const char*_tmp2A7="`RC";_tag_dyneither(_tmp2A7,sizeof(char),4);}));goto _LL282;case 2: _LL289: _tmp2AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2A4)->f1;_LL28A:
 Cyc_Absyndump_dumptvar(_tmp2AC);goto _LL282;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f2 == 0){_LL28B: _LL28C:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2CD));goto _LL282;}else{goto _LL28F;}case 17: _LL28D: _LL28E:
 Cyc_Absyndump_dump(({const char*_tmp2A8="@region(";_tag_dyneither(_tmp2A8,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp2CD);Cyc_Absyndump_dump(({const char*_tmp2A9=")";_tag_dyneither(_tmp2A9,sizeof(char),2);}));goto _LL282;default: _LL28F: _LL290:
({void*_tmp2AA=0;({struct _dyneither_ptr _tmp35B=({const char*_tmp2AB="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2AB,sizeof(char),37);});Cyc_Tcutil_impos(_tmp35B,_tag_dyneither(_tmp2AA,sizeof(void*),0));});});}_LL282:;}
# 1430
Cyc_Absyndump_dumptq(_tmp2C9);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL27B: _LL27C: {
# 1435
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2AD=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2AE=_tmp2AD;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2AE)->tag == 2){_LL292: _LL293:
 next_is_pointer=1;goto _LL291;}else{_LL294: _LL295:
 goto _LL291;}_LL291:;}
# 1441
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2AF=(void*)tms->hd;void*_tmp2B0=_tmp2AF;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2C7;unsigned int _tmp2C6;int _tmp2C5;struct Cyc_List_List*_tmp2C4;unsigned int _tmp2C3;struct Cyc_List_List*_tmp2C2;int _tmp2C1;struct Cyc_Absyn_VarargInfo*_tmp2C0;void*_tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;union Cyc_Absyn_Constraint*_tmp2BA;union Cyc_Absyn_Constraint*_tmp2B9;switch(*((int*)_tmp2B0)){case 0: _LL297: _tmp2B9=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1;_LL298:
# 1448
 Cyc_Absyndump_dump(({const char*_tmp2B1="[]";_tag_dyneither(_tmp2B1,sizeof(char),3);}));
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2B9))Cyc_Absyndump_dump(({const char*_tmp2B2="@zeroterm";_tag_dyneither(_tmp2B2,sizeof(char),10);}));
goto _LL296;case 1: _LL299: _tmp2BB=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1;_tmp2BA=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f2;_LL29A:
# 1452
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2BB);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2BA))Cyc_Absyndump_dump(({const char*_tmp2B3="@zeroterm";_tag_dyneither(_tmp2B3,sizeof(char),10);}));
goto _LL296;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->tag == 1){_LL29B: _tmp2C2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f1;_tmp2C1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f2;_tmp2C0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f3;_tmp2BF=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f4;_tmp2BE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f5;_tmp2BD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f6;_tmp2BC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f7;_LL29C:
# 1456
 Cyc_Absyndump_dumpfunargs(_tmp2C2,_tmp2C1,_tmp2C0,_tmp2BF,_tmp2BE,_tmp2BD,_tmp2BC);goto _LL296;}else{_LL29D: _tmp2C4=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f1;_tmp2C3=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1)->f2;_LL29E:
# 1458
({struct Cyc_List_List*_tmp35E=_tmp2C4;struct _dyneither_ptr _tmp35D=({const char*_tmp2B4="(";_tag_dyneither(_tmp2B4,sizeof(char),2);});struct _dyneither_ptr _tmp35C=({const char*_tmp2B5=")";_tag_dyneither(_tmp2B5,sizeof(char),2);});((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp35E,_tmp35D,_tmp35C,({const char*_tmp2B6=",";_tag_dyneither(_tmp2B6,sizeof(char),2);}));});goto _LL296;}case 4: _LL29F: _tmp2C7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f1;_tmp2C6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f2;_tmp2C5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f3;_LL2A0:
# 1460
 if(_tmp2C5)Cyc_Absyndump_dumpkindedtvars(_tmp2C7);else{Cyc_Absyndump_dumptvars(_tmp2C7);}goto _LL296;case 5: _LL2A1: _tmp2C8=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2B0)->f2;_LL2A2:
 Cyc_Absyndump_dumpatts(_tmp2C8);goto _LL296;default: _LL2A3: _LL2A4:
({void*_tmp2B7=0;({struct _dyneither_ptr _tmp35F=({const char*_tmp2B8="dumptms";_tag_dyneither(_tmp2B8,sizeof(char),8);});Cyc_Tcutil_impos(_tmp35F,_tag_dyneither(_tmp2B7,sizeof(void*),0));});});}_LL296:;}
# 1464
return;}}_LL278:;};}
# 1468
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2CE=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2CF=_new_region("temp");struct _RegionHandle*temp=& _tmp2CF;_push_region(temp);
{struct _tuple11 _tmp2D0=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2D1=_tmp2D0;struct Cyc_Absyn_Tqual _tmp2D4;void*_tmp2D3;struct Cyc_List_List*_tmp2D2;_LL2A6: _tmp2D4=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;_tmp2D2=_tmp2D1.f3;_LL2A7:;
Cyc_Absyndump_dumptq(_tmp2D4);
Cyc_Absyndump_dumpntyp(_tmp2D3);
({int _tmp362=_tmp2CE;struct Cyc_List_List*_tmp361=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D2);void(*_tmp360)(void*)=f;Cyc_Absyndump_dumptms(_tmp362,_tmp361,_tmp360,a);});}
# 1471
;_pop_region(temp);}
# 1478
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1480
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2D5=0;({struct Cyc___cycFILE*_tmp364=f;struct _dyneither_ptr _tmp363=({const char*_tmp2D6="\n";_tag_dyneither(_tmp2D6,sizeof(char),2);});Cyc_fprintf(_tmp364,_tmp363,_tag_dyneither(_tmp2D5,sizeof(void*),0));});});}
# 1486
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2D7=d->r;void*_tmp2D8=_tmp2D7;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F2;struct _dyneither_ptr*_tmp2F1;struct Cyc_List_List*_tmp2F0;struct Cyc_Absyn_Typedefdecl*_tmp2EF;struct Cyc_Absyn_Enumdecl*_tmp2EE;struct Cyc_Absyn_Datatypedecl*_tmp2ED;struct Cyc_Absyn_Aggrdecl*_tmp2EC;struct Cyc_Absyn_Fndecl*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EA;switch(*((int*)_tmp2D8)){case 0: _LL2A9: _tmp2EA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2AA:
# 1489
 if(_tmp2EA->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2EA->initializer;
_tmp2EA->initializer=0;
if(_tmp2EA->sc == Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2D9="extern ";_tag_dyneither(_tmp2D9,sizeof(char),8);}));
Cyc_Absyndump_dumpvardecl(_tmp2EA,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2DA="\n";_tag_dyneither(_tmp2DA,sizeof(char),2);}));
_tmp2EA->initializer=init;
goto _LL2A8;};case 1: _LL2AB: _tmp2EB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2AC:
# 1499
 if(_tmp2EB->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2EB->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp367=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp2DD;_tmp2DD.tag=9;({struct Cyc_Absyn_FnInfo _tmp366=({struct Cyc_Absyn_FnInfo _tmp2DE;_tmp2DE.tvars=_tmp2EB->tvs;_tmp2DE.effect=_tmp2EB->effect;_tmp2DE.ret_tqual=_tmp2EB->ret_tqual;_tmp2DE.ret_typ=_tmp2EB->ret_type;({struct Cyc_List_List*_tmp365=
# 1503
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2EB->args);_tmp2DE.args=_tmp365;});_tmp2DE.c_varargs=_tmp2EB->c_varargs;_tmp2DE.cyc_varargs=_tmp2EB->cyc_varargs;_tmp2DE.rgn_po=_tmp2EB->rgn_po;_tmp2DE.attributes=0;_tmp2DE.requires_clause=_tmp2EB->requires_clause;_tmp2DE.requires_relns=_tmp2EB->requires_relns;_tmp2DE.ensures_clause=_tmp2EB->ensures_clause;_tmp2DE.ensures_relns=_tmp2EB->ensures_relns;_tmp2DE;});_tmp2DD.f1=_tmp366;});_tmp2DD;});_tmp2DC[0]=_tmp367;});_tmp2DC;});
# 1508
({struct Cyc_Absyn_Tqual _tmp369=Cyc_Absyn_empty_tqual(0);void*_tmp368=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp369,_tmp368,Cyc_Absyndump_dump_atts_qvar,_tmp2EB);});
Cyc_Absyndump_dump(({const char*_tmp2DB=";\n";_tag_dyneither(_tmp2DB,sizeof(char),3);}));
goto _LL2A8;};case 5: _LL2AD: _tmp2EC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2AE:
# 1512
 if(_tmp2EC->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2EC->impl;
if(_tmp2EC->sc == Cyc_Absyn_Abstract)_tmp2EC->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2EC);
_tmp2EC->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2DF=";\n";_tag_dyneither(_tmp2DF,sizeof(char),3);}));
goto _LL2A8;};case 6: _LL2AF: _tmp2ED=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2B0:
# 1520
 Cyc_Absyndump_dump_datatypedecl(_tmp2ED);
Cyc_Absyndump_dump(({const char*_tmp2E0=";\n";_tag_dyneither(_tmp2E0,sizeof(char),3);}));
goto _LL2A8;case 7: _LL2B1: _tmp2EE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2B2:
# 1524
 Cyc_Absyndump_dump_enumdecl(_tmp2EE);
Cyc_Absyndump_dump(({const char*_tmp2E1=";\n";_tag_dyneither(_tmp2E1,sizeof(char),3);}));
goto _LL2A8;case 8: _LL2B3: _tmp2EF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2B4:
# 1528
 if(_tmp2EF->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2E2="\n";_tag_dyneither(_tmp2E2,sizeof(char),2);}));}
# 1532
goto _LL2A8;case 9: _LL2B5: _tmp2F1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_tmp2F0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2D8)->f2;_LL2B6:
# 1534
 Cyc_Absyndump_dump(({const char*_tmp2E3="namespace ";_tag_dyneither(_tmp2E3,sizeof(char),11);}));
Cyc_Absyndump_dump_str(_tmp2F1);
Cyc_Absyndump_dump(({const char*_tmp2E4="{\n";_tag_dyneither(_tmp2E4,sizeof(char),3);}));
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F0->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E5="}\n";_tag_dyneither(_tmp2E5,sizeof(char),3);}));
goto _LL2A8;case 10: _LL2B7: _tmp2F2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2D8)->f2;_LL2B8:
# 1542
 for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F2->hd);}
goto _LL2A8;case 11: _LL2B9: _tmp2F3=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL2BA:
# 1546
 Cyc_Absyndump_dump(({const char*_tmp2E6="extern \"C\" {";_tag_dyneither(_tmp2E6,sizeof(char),13);}));
for(0;_tmp2F3 != 0;_tmp2F3=_tmp2F3->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F3->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E7="}\n";_tag_dyneither(_tmp2E7,sizeof(char),3);}));
goto _LL2A8;case 12: _LL2BB: _tmp2F5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_tmp2F4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D8)->f2;_LL2BC:
# 1567 "absyndump.cyc"
 goto _LL2A8;default: _LL2BD: _LL2BE:
({void*_tmp2E8=0;({struct _dyneither_ptr _tmp36A=({const char*_tmp2E9="bad top-level declaration";_tag_dyneither(_tmp2E9,sizeof(char),26);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp36A,_tag_dyneither(_tmp2E8,sizeof(void*),0));});});}_LL2A8:;}
# 1572
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}

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
# 978
extern void*Cyc_Absyn_bounds_one;
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1165
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1171
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1183
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
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 86
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
if((char*)_tmpD.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line)return;
if((char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line + 1)
({void*_tmp3=0;Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_tmp4="\n";_tag_dyneither(_tmp4,sizeof(char),2);}),_tag_dyneither(_tmp3,sizeof(void*),0));});else{
if((char*)_tmpD.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp7;_tmp7.tag=1;_tmp7.f1=(unsigned long)((int)_tmpC);({void*_tmp5[1]={& _tmp7};Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_tmp6="\n# %d\n";_tag_dyneither(_tmp6,sizeof(char),7);}),_tag_dyneither(_tmp5,sizeof(void*),1));});});else{
# 164
({struct Cyc_String_pa_PrintArg_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD);({struct Cyc_Int_pa_PrintArg_struct _tmpA;_tmpA.tag=1;_tmpA.f1=(unsigned long)((int)_tmpC);({void*_tmp8[2]={& _tmpA,& _tmpB};Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_tmp9="\n# %d %s\n";_tag_dyneither(_tmp9,sizeof(char),10);}),_tag_dyneither(_tmp8,sizeof(void*),2));});});});}}
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
((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,({const char*_tmp3B="<";_tag_dyneither(_tmp3B,sizeof(char),2);}),({const char*_tmp3C=">";_tag_dyneither(_tmp3C,sizeof(char),2);}),({const char*_tmp3D=",";_tag_dyneither(_tmp3D,sizeof(char),2);}));}
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
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,({const char*_tmp49="<";_tag_dyneither(_tmp49,sizeof(char),2);}),({const char*_tmp4A=">";_tag_dyneither(_tmp4A,sizeof(char),2);}),({const char*_tmp4B=",";_tag_dyneither(_tmp4B,sizeof(char),2);}));}
# 347
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp4C="<";_tag_dyneither(_tmp4C,sizeof(char),2);}),({const char*_tmp4D=">";_tag_dyneither(_tmp4D,sizeof(char),2);}),({const char*_tmp4E=",";_tag_dyneither(_tmp4E,sizeof(char),2);}));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 351
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 354
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,({const char*_tmp4F="(";_tag_dyneither(_tmp4F,sizeof(char),2);}),({const char*_tmp50=")";_tag_dyneither(_tmp50,sizeof(char),2);}),({const char*_tmp51=",";_tag_dyneither(_tmp51,sizeof(char),2);}));}
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
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp60=(void*)tms->hd;void*_tmp61=_tmp60;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp61)->tag == 2){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;};}
# 416
static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp63=_tmp62;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp63)->tag == 20){_LL9F: _LLA0:
 Cyc_Absyndump_dump(({const char*_tmp64="`H";_tag_dyneither(_tmp64,sizeof(char),3);}));goto _LL9E;}else{_LLA1: _LLA2:
 Cyc_Absyndump_dumpntyp(t);goto _LL9E;}_LL9E:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 423
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 427
{void*_tmp65=Cyc_Tcutil_compress(t);void*_tmp66=_tmp65;struct Cyc_List_List*_tmp6E;void*_tmp6D;switch(*((int*)_tmp66)){case 23: _LLA4: _tmp6D=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp66)->f1;_LLA5:
 rgions=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=_tmp6D;_tmp67->tl=rgions;_tmp67;});goto _LLA3;case 24: _LLA6: _tmp6E=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp66)->f1;_LLA7:
# 430
 for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){
struct _tuple15 _tmp68=Cyc_Absyndump_effects_split((void*)_tmp6E->hd);struct _tuple15 _tmp69=_tmp68;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6A;_LLAB: _tmp6B=_tmp69.f1;_tmp6A=_tmp69.f2;_LLAC:;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6B,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6A,effects);}
# 435
goto _LLA3;default: _LLA8: _LLA9:
 effects=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=t;_tmp6C->tl=effects;_tmp6C;});goto _LLA3;}_LLA3:;}
# 438
return({struct _tuple15 _tmp6F;_tmp6F.f1=rgions;_tmp6F.f2=effects;_tmp6F;});}
# 441
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp70=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp71=_tmp70;struct Cyc_List_List*_tmp74;struct Cyc_List_List*_tmp73;_LLAE: _tmp74=_tmp71.f1;_tmp73=_tmp71.f2;_LLAF:;
_tmp74=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp74);
_tmp73=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp73);
if(_tmp73 != 0){
{struct Cyc_List_List*_tmp72=_tmp73;for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp72->hd);
if(_tmp72->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 451
if(_tmp74 != 0)Cyc_Absyndump_dump_char((int)'+');}
# 453
if(_tmp74 != 0  || _tmp73 == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp74 != 0;_tmp74=_tmp74->tl){
Cyc_Absyndump_dumprgn((void*)_tmp74->hd);
if(_tmp74->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 459
Cyc_Absyndump_dump_char((int)'}');}}
# 463
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 468
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp75=t;struct Cyc_List_List*_tmpD9;void*_tmpD8;void*_tmpD7;struct Cyc_Absyn_Datatypedecl*_tmpD6;struct Cyc_Absyn_Enumdecl*_tmpD5;struct Cyc_Absyn_Aggrdecl*_tmpD4;void*_tmpD3;void*_tmpD2;void*_tmpD1;void*_tmpD0;struct _dyneither_ptr _tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;struct _tuple0*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCA;struct _tuple0*_tmpC9;enum Cyc_Absyn_AggrKind _tmpC8;struct Cyc_List_List*_tmpC7;union Cyc_Absyn_AggrInfoU _tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*_tmpC4;int _tmpC3;union Cyc_Absyn_DatatypeFieldInfoU _tmpC2;struct Cyc_List_List*_tmpC1;union Cyc_Absyn_DatatypeInfoU _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Core_Opt*_tmpBE;void*_tmpBD;int _tmpBC;struct Cyc_Core_Opt*_tmpBB;int _tmpBA;struct Cyc_Absyn_Tvar*_tmpB9;switch(*((int*)_tmp75)){case 8: _LLB1: _LLB2:
# 471
 goto _LLB4;case 9: _LLB3: _LLB4:
 goto _LLB6;case 5: _LLB5: _LLB6:
 return;case 0: _LLB7: _LLB8:
 Cyc_Absyndump_dump(({const char*_tmp76="void";_tag_dyneither(_tmp76,sizeof(char),5);}));return;case 2: _LLB9: _tmpB9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1;_LLBA:
 Cyc_Absyndump_dumptvar(_tmpB9);return;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2 == 0){_LLBB: _tmpBB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f1;_tmpBA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f3;_LLBC:
# 477
 Cyc_Absyndump_dump(({const char*_tmp77="`E";_tag_dyneither(_tmp77,sizeof(char),3);}));
if(_tmpBB == 0)Cyc_Absyndump_dump(({const char*_tmp78="K";_tag_dyneither(_tmp78,sizeof(char),2);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmpBB->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp7B;_tmp7B.tag=1;_tmp7B.f1=(unsigned long)_tmpBA;({void*_tmp79[1]={& _tmp7B};Cyc_aprintf(({const char*_tmp7A="%d";_tag_dyneither(_tmp7A,sizeof(char),3);}),_tag_dyneither(_tmp79,sizeof(void*),1));});}));return;}else{_LLBD: _tmpBE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f1;_tmpBD=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2;_tmpBC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f3;_LLBE:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmpBD));return;}case 3: _LLBF: _tmpC0=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp75)->f1).datatype_info;_tmpBF=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLC0:
# 482
{union Cyc_Absyn_DatatypeInfoU _tmp7C=_tmpC0;struct _tuple0*_tmp82;int _tmp81;struct _tuple0*_tmp80;int _tmp7F;if((_tmp7C.UnknownDatatype).tag == 1){_LL10E: _tmp80=((_tmp7C.UnknownDatatype).val).name;_tmp7F=((_tmp7C.UnknownDatatype).val).is_extensible;_LL10F:
 _tmp82=_tmp80;_tmp81=_tmp7F;goto _LL111;}else{_LL110: _tmp82=(*(_tmp7C.KnownDatatype).val)->name;_tmp81=(*(_tmp7C.KnownDatatype).val)->is_extensible;_LL111:
# 485
 if(_tmp81)Cyc_Absyndump_dump(({const char*_tmp7D="@extensible ";_tag_dyneither(_tmp7D,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp7E="datatype ";_tag_dyneither(_tmp7E,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp82);
Cyc_Absyndump_dumptps(_tmpBF);
goto _LL10D;}_LL10D:;}
# 491
goto _LLB0;case 4: _LLC1: _tmpC2=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp75)->f1).field_info;_tmpC1=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLC2:
# 493
{union Cyc_Absyn_DatatypeFieldInfoU _tmp83=_tmpC2;struct _tuple0*_tmp8C;int _tmp8B;struct _tuple0*_tmp8A;struct _tuple0*_tmp89;struct _tuple0*_tmp88;int _tmp87;if((_tmp83.UnknownDatatypefield).tag == 1){_LL113: _tmp89=((_tmp83.UnknownDatatypefield).val).datatype_name;_tmp88=((_tmp83.UnknownDatatypefield).val).field_name;_tmp87=((_tmp83.UnknownDatatypefield).val).is_extensible;_LL114:
# 495
 _tmp8C=_tmp89;_tmp8B=_tmp87;_tmp8A=_tmp88;goto _LL116;}else{_LL115: _tmp8C=(((_tmp83.KnownDatatypefield).val).f1)->name;_tmp8B=(((_tmp83.KnownDatatypefield).val).f1)->is_extensible;_tmp8A=(((_tmp83.KnownDatatypefield).val).f2)->name;_LL116:
# 498
 if(_tmp8B)Cyc_Absyndump_dump(({const char*_tmp84="@extensible ";_tag_dyneither(_tmp84,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp85="datatype ";_tag_dyneither(_tmp85,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp8C);
Cyc_Absyndump_dump(({const char*_tmp86=".";_tag_dyneither(_tmp86,sizeof(char),2);}));
Cyc_Absyndump_dumpqvar(_tmp8A);
Cyc_Absyndump_dumptps(_tmpC1);
goto _LL112;}_LL112:;}
# 506
goto _LLB0;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Int_sz: _LLC3: _LLC4:
 goto _LLC6;case Cyc_Absyn_Long_sz: _LLC7: _LLC8:
# 509
 goto _LLCA;case Cyc_Absyn_Char_sz: _LLCB: _LLCC:
# 511
 Cyc_Absyndump_dump(({const char*_tmp8F="char";_tag_dyneither(_tmp8F,sizeof(char),5);}));return;case Cyc_Absyn_Short_sz: _LLD1: _LLD2:
# 514
 goto _LLD4;default: _LLDB: _LLDC:
# 519
 goto _LLDE;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Int_sz: _LLC5: _LLC6:
# 508
 Cyc_Absyndump_dump(({const char*_tmp8D="int";_tag_dyneither(_tmp8D,sizeof(char),4);}));return;case Cyc_Absyn_Long_sz: _LLC9: _LLCA:
# 510
 Cyc_Absyndump_dump(({const char*_tmp8E="long";_tag_dyneither(_tmp8E,sizeof(char),5);}));return;case Cyc_Absyn_Char_sz: _LLCD: _LLCE:
# 512
 Cyc_Absyndump_dump(({const char*_tmp90="signed char";_tag_dyneither(_tmp90,sizeof(char),12);}));return;case Cyc_Absyn_Short_sz: _LLD3: _LLD4:
# 515
 Cyc_Absyndump_dump(({const char*_tmp92="short";_tag_dyneither(_tmp92,sizeof(char),6);}));return;default: _LLDD: _LLDE: {
# 521
enum Cyc_Cyclone_C_Compilers _tmp96=Cyc_Cyclone_c_compiler;if(_tmp96 == Cyc_Cyclone_Gcc_c){_LL118: _LL119:
 Cyc_Absyndump_dump(({const char*_tmp97="long long";_tag_dyneither(_tmp97,sizeof(char),10);}));return;}else{_LL11A: _LL11B:
 Cyc_Absyndump_dump(({const char*_tmp98="__int64";_tag_dyneither(_tmp98,sizeof(char),8);}));return;}_LL117:;}}default: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Char_sz: _LLCF: _LLD0:
# 513
 Cyc_Absyndump_dump(({const char*_tmp91="unsigned char";_tag_dyneither(_tmp91,sizeof(char),14);}));return;case Cyc_Absyn_Short_sz: _LLD5: _LLD6:
# 516
 Cyc_Absyndump_dump(({const char*_tmp93="unsigned short";_tag_dyneither(_tmp93,sizeof(char),15);}));return;case Cyc_Absyn_Int_sz: _LLD7: _LLD8:
 Cyc_Absyndump_dump(({const char*_tmp94="unsigned int";_tag_dyneither(_tmp94,sizeof(char),13);}));return;case Cyc_Absyn_Long_sz: _LLD9: _LLDA:
 Cyc_Absyndump_dump(({const char*_tmp95="unsigned long";_tag_dyneither(_tmp95,sizeof(char),14);}));return;default: _LLDF: _LLE0: {
# 526
enum Cyc_Cyclone_C_Compilers _tmp99=Cyc_Cyclone_c_compiler;if(_tmp99 == Cyc_Cyclone_Vc_c){_LL11D: _LL11E:
 Cyc_Absyndump_dump(({const char*_tmp9A="unsigned __int64";_tag_dyneither(_tmp9A,sizeof(char),17);}));return;}else{_LL11F: _LL120:
 Cyc_Absyndump_dump(({const char*_tmp9B="unsigned long long";_tag_dyneither(_tmp9B,sizeof(char),19);}));return;}_LL11C:;}}}case 7: _LLE1: _tmpC3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp75)->f1;_LLE2:
# 531
 if(_tmpC3 == 0)
Cyc_Absyndump_dump(({const char*_tmp9C="float";_tag_dyneither(_tmp9C,sizeof(char),6);}));else{
if(_tmpC3 == 1)
Cyc_Absyndump_dump(({const char*_tmp9D="double";_tag_dyneither(_tmp9D,sizeof(char),7);}));else{
# 536
Cyc_Absyndump_dump(({const char*_tmp9E="long double";_tag_dyneither(_tmp9E,sizeof(char),12);}));}}
return;case 10: _LLE3: _tmpC4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp75)->f1;_LLE4:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpC4);return;case 11: _LLE5: _tmpC6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp75)->f1).aggr_info;_tmpC5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLE6: {
# 540
struct _tuple10 _tmp9F=Cyc_Absyn_aggr_kinded_name(_tmpC6);struct _tuple10 _tmpA0=_tmp9F;enum Cyc_Absyn_AggrKind _tmpA2;struct _tuple0*_tmpA1;_LL122: _tmpA2=_tmpA0.f1;_tmpA1=_tmpA0.f2;_LL123:;
Cyc_Absyndump_dumpaggr_kind(_tmpA2);Cyc_Absyndump_dumpqvar(_tmpA1);Cyc_Absyndump_dumptps(_tmpC5);
return;}case 12: _LLE7: _tmpC8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75)->f1;_tmpC7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75)->f2;_LLE8:
# 544
 Cyc_Absyndump_dumpaggr_kind(_tmpC8);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpC7);Cyc_Absyndump_dump_char((int)'}');return;case 13: _LLE9: _tmpC9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp75)->f1;_LLEA:
 Cyc_Absyndump_dump(({const char*_tmpA3="enum ";_tag_dyneither(_tmpA3,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmpC9);return;case 14: _LLEB: _tmpCA=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp75)->f1;_LLEC:
 Cyc_Absyndump_dump(({const char*_tmpA4="enum {";_tag_dyneither(_tmpA4,sizeof(char),7);}));Cyc_Absyndump_dumpenumfields(_tmpCA);Cyc_Absyndump_dump(({const char*_tmpA5="}";_tag_dyneither(_tmpA5,sizeof(char),2);}));return;case 17: _LLED: _tmpCC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f1;_tmpCB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f2;_LLEE:
(Cyc_Absyndump_dumpqvar(_tmpCC),Cyc_Absyndump_dumptps(_tmpCB));return;case 18: _LLEF: _tmpCD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp75)->f1;_LLF0:
 Cyc_Absyndump_dump(({const char*_tmpA6="valueof_t(";_tag_dyneither(_tmpA6,sizeof(char),11);}));Cyc_Absyndump_dumpexp(_tmpCD);Cyc_Absyndump_dump(({const char*_tmpA7=")";_tag_dyneither(_tmpA7,sizeof(char),2);}));return;case 27: _LLF1: _tmpCE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp75)->f1;_LLF2:
 Cyc_Absyndump_dump(({const char*_tmpA8="typeof(";_tag_dyneither(_tmpA8,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmpCE);Cyc_Absyndump_dump(({const char*_tmpA9=")";_tag_dyneither(_tmpA9,sizeof(char),2);}));return;case 28: _LLF3: _tmpCF=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp75)->f1;_LLF4:
 Cyc_Absyndump_dump(_tmpCF);return;case 15: _LLF5: _tmpD0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp75)->f1;_LLF6:
# 552
 Cyc_Absyndump_dump(({const char*_tmpAA="region_t<";_tag_dyneither(_tmpAA,sizeof(char),10);}));Cyc_Absyndump_dumprgn(_tmpD0);Cyc_Absyndump_dump(({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2);}));return;case 16: _LLF7: _tmpD2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp75)->f1;_tmpD1=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp75)->f2;_LLF8:
 Cyc_Absyndump_dump(({const char*_tmpAC="dynregion_t<";_tag_dyneither(_tmpAC,sizeof(char),13);}));Cyc_Absyndump_dumprgn(_tmpD2);
Cyc_Absyndump_dump(({const char*_tmpAD=",";_tag_dyneither(_tmpAD,sizeof(char),2);}));Cyc_Absyndump_dumprgn(_tmpD1);Cyc_Absyndump_dump(({const char*_tmpAE=">";_tag_dyneither(_tmpAE,sizeof(char),2);}));return;case 19: _LLF9: _tmpD3=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp75)->f1;_LLFA:
 Cyc_Absyndump_dump(({const char*_tmpAF="tag_t<";_tag_dyneither(_tmpAF,sizeof(char),7);}));Cyc_Absyndump_dumpntyp(_tmpD3);Cyc_Absyndump_dump(({const char*_tmpB0=">";_tag_dyneither(_tmpB0,sizeof(char),2);}));return;case 21: _LLFB: _LLFC:
 Cyc_Absyndump_dump(({const char*_tmpB1="`U";_tag_dyneither(_tmpB1,sizeof(char),3);}));goto _LLB0;case 22: _LLFD: _LLFE:
 Cyc_Absyndump_dump(({const char*_tmpB2="`RC";_tag_dyneither(_tmpB2,sizeof(char),4);}));goto _LLB0;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)){case 0: _LLFF: _tmpD4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL100:
# 559
 Cyc_Absyndump_dump_aggrdecl(_tmpD4);return;case 1: _LL101: _tmpD5=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL102:
# 561
 Cyc_Absyndump_dump_enumdecl(_tmpD5);return;default: _LL103: _tmpD6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL104:
# 563
 Cyc_Absyndump_dump_datatypedecl(_tmpD6);return;}case 20: _LL105: _LL106:
 Cyc_Absyndump_dump(({const char*_tmpB3="`H";_tag_dyneither(_tmpB3,sizeof(char),3);}));goto _LLB0;case 23: _LL107: _tmpD7=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp75)->f1;_LL108:
 Cyc_Absyndump_dump(({const char*_tmpB4="{";_tag_dyneither(_tmpB4,sizeof(char),2);}));Cyc_Absyndump_dumptyp(_tmpD7);Cyc_Absyndump_dump(({const char*_tmpB5="}";_tag_dyneither(_tmpB5,sizeof(char),2);}));goto _LLB0;case 25: _LL109: _tmpD8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp75)->f1;_LL10A:
 Cyc_Absyndump_dump(({const char*_tmpB6="regions(";_tag_dyneither(_tmpB6,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmpD8);Cyc_Absyndump_dump(({const char*_tmpB7=")";_tag_dyneither(_tmpB7,sizeof(char),2);}));goto _LLB0;default: _LL10B: _tmpD9=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp75)->f1;_LL10C:
# 568
 for(0;_tmpD9 != 0;_tmpD9=_tmpD9->tl){
Cyc_Absyndump_dumptyp((void*)_tmpD9->hd);
if(_tmpD9->tl != 0)Cyc_Absyndump_dump(({const char*_tmpB8="+";_tag_dyneither(_tmpB8,sizeof(char),2);}));}}_LLB0:;}
# 575
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 578
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 581
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpDA=cmp;void*_tmpDC;void*_tmpDB;_LL125: _tmpDC=_tmpDA->f1;_tmpDB=_tmpDA->f2;_LL126:;
Cyc_Absyndump_dumpeff(_tmpDC);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpDB);}
# 585
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,({const char*_tmpDD=",";_tag_dyneither(_tmpDD,sizeof(char),2);}));}
# 588
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 592
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 597
if(c_varargs)
Cyc_Absyndump_dump(({const char*_tmpDE="...";_tag_dyneither(_tmpDE,sizeof(char),4);}));else{
if(cyc_varargs != 0){
struct _tuple8*_tmpDF=({struct _tuple8*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->f1=cyc_varargs->name;_tmpE2->f2=cyc_varargs->tq;_tmpE2->f3=cyc_varargs->type;_tmpE2;});
Cyc_Absyndump_dump(({const char*_tmpE0="...";_tag_dyneither(_tmpE0,sizeof(char),4);}));
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpE1=" inject ";_tag_dyneither(_tmpE1,sizeof(char),9);}));
Cyc_Absyndump_dumpfunarg(_tmpDF);}}
# 605
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 609
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 613
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpE3=" @requires(";_tag_dyneither(_tmpE3,sizeof(char),12);}));
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 619
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpE4=" @ensures(";_tag_dyneither(_tmpE4,sizeof(char),11);}));
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 626
void Cyc_Absyndump_dumptyp(void*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 630
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL128: _tmpE7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL129:
 Cyc_Absyndump_dump(({const char*_tmpE6=".[";_tag_dyneither(_tmpE6,sizeof(char),3);}));Cyc_Absyndump_dumpexp(_tmpE7);Cyc_Absyndump_dump_char((int)']');goto _LL127;}else{_LL12A: _tmpE8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL12B:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE8);goto _LL127;}_LL127:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 637
void Cyc_Absyndump_dumpde(struct _tuple17*de){
((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,({const char*_tmpE9="";_tag_dyneither(_tmpE9,sizeof(char),1);}),({const char*_tmpEA="=";_tag_dyneither(_tmpEA,sizeof(char),2);}),({const char*_tmpEB="=";_tag_dyneither(_tmpEB,sizeof(char),2);}));
Cyc_Absyndump_dumpexp((*de).f2);}
# 642
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_tmpEC="";_tag_dyneither(_tmpEC,sizeof(char),1);}),({const char*_tmpED="";_tag_dyneither(_tmpED,sizeof(char),1);}),({const char*_tmpEE=",";_tag_dyneither(_tmpEE,sizeof(char),2);}));}
# 646
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpEF="(";_tag_dyneither(_tmpEF,sizeof(char),2);}));
{void*_tmpF0=e->r;void*_tmpF1=_tmpF0;struct Cyc_Absyn_Stmt*_tmp198;struct Cyc_Core_Opt*_tmp197;struct Cyc_List_List*_tmp196;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp194;int _tmp193;struct Cyc_Absyn_Exp*_tmp192;void**_tmp191;struct Cyc_Absyn_Exp*_tmp190;int _tmp18F;struct Cyc_Absyn_Enumfield*_tmp18E;struct Cyc_Absyn_Enumfield*_tmp18D;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Datatypefield*_tmp18B;struct Cyc_List_List*_tmp18A;struct _tuple0*_tmp189;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Exp*_tmp186;void*_tmp185;struct Cyc_Absyn_Vardecl*_tmp184;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_List_List*_tmp181;struct _tuple8*_tmp180;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17B;struct _dyneither_ptr*_tmp17A;struct Cyc_Absyn_Exp*_tmp179;struct _dyneither_ptr*_tmp178;struct Cyc_Absyn_Exp*_tmp177;void*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Exp*_tmp174;struct _dyneither_ptr*_tmp173;int _tmp172;struct _dyneither_ptr _tmp171;void*_tmp170;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp16E;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16B;void*_tmp16A;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Core_Opt*_tmp155;struct Cyc_Absyn_Exp*_tmp154;enum Cyc_Absyn_Primop _tmp153;struct Cyc_List_List*_tmp152;void*_tmp151;struct _dyneither_ptr _tmp150;struct _dyneither_ptr _tmp14F;struct _dyneither_ptr _tmp14E;enum Cyc_Absyn_Sign _tmp14D;long long _tmp14C;int _tmp14B;int _tmp14A;int _tmp149;enum Cyc_Absyn_Sign _tmp148;short _tmp147;struct _dyneither_ptr _tmp146;enum Cyc_Absyn_Sign _tmp145;char _tmp144;switch(*((int*)_tmpF1)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).String_c).tag){case 2: _LL12D: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Char_c).val).f1;_tmp144=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Char_c).val).f2;_LL12E:
# 652
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp144));Cyc_Absyndump_dump_char((int)'\'');
goto _LL12C;case 3: _LL12F: _tmp146=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Wchar_c).val;_LL130:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp146);({void*_tmpF2[1]={& _tmpF4};Cyc_aprintf(({const char*_tmpF3="L'%s'";_tag_dyneither(_tmpF3,sizeof(char),6);}),_tag_dyneither(_tmpF2,sizeof(void*),1));});}));goto _LL12C;case 4: _LL131: _tmp148=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Short_c).val).f1;_tmp147=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Short_c).val).f2;_LL132:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF7;_tmpF7.tag=1;_tmpF7.f1=(unsigned long)((int)_tmp147);({void*_tmpF5[1]={& _tmpF7};Cyc_aprintf(({const char*_tmpF6="%d";_tag_dyneither(_tmpF6,sizeof(char),3);}),_tag_dyneither(_tmpF5,sizeof(void*),1));});}));goto _LL12C;case 5: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL133: _tmp149=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL134:
 _tmp14A=_tmp149;goto _LL136;case Cyc_Absyn_Signed: _LL135: _tmp14A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL136:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFA;_tmpFA.tag=1;_tmpFA.f1=(unsigned long)_tmp14A;({void*_tmpF8[1]={& _tmpFA};Cyc_aprintf(({const char*_tmpF9="%d";_tag_dyneither(_tmpF9,sizeof(char),3);}),_tag_dyneither(_tmpF8,sizeof(void*),1));});}));goto _LL12C;default: _LL137: _tmp14B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL138:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFD;_tmpFD.tag=1;_tmpFD.f1=(unsigned int)_tmp14B;({void*_tmpFB[1]={& _tmpFD};Cyc_aprintf(({const char*_tmpFC="%u";_tag_dyneither(_tmpFC,sizeof(char),3);}),_tag_dyneither(_tmpFB,sizeof(void*),1));});}));goto _LL12C;}case 6: _LL139: _tmp14D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).LongLong_c).val).f1;_tmp14C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).LongLong_c).val).f2;_LL13A:
# 661
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp14C));goto _LL12C;case 7: _LL13B: _tmp14E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Float_c).val).f1;_LL13C:
 Cyc_Absyndump_dump(_tmp14E);goto _LL12C;case 1: _LL13D: _LL13E:
 Cyc_Absyndump_dump(({const char*_tmpFE="NULL";_tag_dyneither(_tmpFE,sizeof(char),5);}));goto _LL12C;case 8: _LL13F: _tmp14F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).String_c).val;_LL140:
# 665
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14F));Cyc_Absyndump_dump_char((int)'"');
goto _LL12C;default: _LL141: _tmp150=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Wstring_c).val;_LL142:
# 668
 Cyc_Absyndump_dump(({const char*_tmpFF="L\"";_tag_dyneither(_tmpFF,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(_tmp150);Cyc_Absyndump_dump_char((int)'"');
goto _LL12C;}case 1: _LL143: _tmp151=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL144:
# 671
 Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(_tmp151));goto _LL12C;case 2: _LL145: _tmp153=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp152=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL146: {
# 674
struct _dyneither_ptr _tmp100=Cyc_Absynpp_prim2str(_tmp153);
{int _tmp101=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp152);int _tmp102=_tmp101;switch(_tmp102){case 1: _LL198: _LL199:
# 677
 if(_tmp153 == Cyc_Absyn_Numelts){
Cyc_Absyndump_dump(({const char*_tmp103="numelts(";_tag_dyneither(_tmp103,sizeof(char),9);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp152))->hd);
Cyc_Absyndump_dump(({const char*_tmp104=")";_tag_dyneither(_tmp104,sizeof(char),2);}));}else{
# 682
Cyc_Absyndump_dump(_tmp100);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp152))->hd);}
# 685
goto _LL197;case 2: _LL19A: _LL19B:
# 687
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp152))->hd);
Cyc_Absyndump_dump(_tmp100);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp152->tl))->hd);
goto _LL197;default: _LL19C: _LL19D:
# 693
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Core_Failure_exn_struct _tmp106;_tmp106.tag=Cyc_Core_Failure;_tmp106.f1=({const char*_tmp107="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmp107,sizeof(char),47);});_tmp106;});_tmp105;}));}_LL197:;}
# 695
goto _LL12C;}case 3: _LL147: _tmp156=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp155=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp154=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL148:
# 698
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp156);
if(_tmp155 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp155->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp108="=";_tag_dyneither(_tmp108,sizeof(char),2);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp154);
goto _LL12C;case 4: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f2){case Cyc_Absyn_PreInc: _LL149: _tmp157=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL14A:
# 705
 Cyc_Absyndump_dump(({const char*_tmp109="++";_tag_dyneither(_tmp109,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);goto _LL12C;case Cyc_Absyn_PreDec: _LL14B: _tmp158=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL14C:
 Cyc_Absyndump_dump(({const char*_tmp10A="--";_tag_dyneither(_tmp10A,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp158);goto _LL12C;case Cyc_Absyn_PostInc: _LL14D: _tmp159=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL14E:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp159);Cyc_Absyndump_dump(({const char*_tmp10B="++";_tag_dyneither(_tmp10B,sizeof(char),3);}));goto _LL12C;default: _LL14F: _tmp15A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL150:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);Cyc_Absyndump_dump(({const char*_tmp10C="--";_tag_dyneither(_tmp10C,sizeof(char),3);}));goto _LL12C;}case 5: _LL151: _tmp15D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp15C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp15B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL152:
# 711
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp15C);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);
goto _LL12C;case 6: _LL153: _tmp15F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp15E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL154:
# 718
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15F);Cyc_Absyndump_dump(({const char*_tmp10D=" && ";_tag_dyneither(_tmp10D,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15E);goto _LL12C;case 7: _LL155: _tmp161=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp160=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL156:
# 722
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);Cyc_Absyndump_dump(({const char*_tmp10E=" || ";_tag_dyneither(_tmp10E,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp160);goto _LL12C;case 8: _LL157: _tmp163=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp162=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL158:
# 728
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp163);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp162);
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 9: _LL159: _tmp165=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp164=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL15A:
# 735
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp165);
Cyc_Absyndump_dump_nospace(({const char*_tmp10F="(";_tag_dyneither(_tmp10F,sizeof(char),2);}));{
# 741
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp165->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp164);
Cyc_Absyndump_dump_nospace(({const char*_tmp110=")";_tag_dyneither(_tmp110,sizeof(char),2);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL12C;};case 10: _LL15B: _tmp166=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL15C:
# 750
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump(({const char*_tmp111="throw";_tag_dyneither(_tmp111,sizeof(char),6);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp166);goto _LL12C;case 11: _LL15D: _tmp167=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL15E:
# 753
 _tmp168=_tmp167;goto _LL160;case 12: _LL15F: _tmp168=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL160:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp168);goto _LL12C;case 13: _LL161: _tmp16A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp169=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL162:
# 757
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp16A);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp169);
goto _LL12C;case 14: _LL163: _tmp16B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL164:
# 761
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp16B);goto _LL12C;case 15: _LL165: _tmp16D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp16C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL166:
# 764
 Cyc_Absyndump_dump(({const char*_tmp112="new ";_tag_dyneither(_tmp112,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp16C);goto _LL12C;case 16: _LL167: _tmp16E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL168:
# 767
 Cyc_Absyndump_dump(({const char*_tmp113="sizeof(";_tag_dyneither(_tmp113,sizeof(char),8);}));Cyc_Absyndump_dumptyp(_tmp16E);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 17: _LL169: _tmp16F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL16A:
# 770
 Cyc_Absyndump_dump(({const char*_tmp114="sizeof(";_tag_dyneither(_tmp114,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp16F);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 38: _LL16B: _tmp170=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL16C:
# 773
 Cyc_Absyndump_dump(({const char*_tmp115="valueof(";_tag_dyneither(_tmp115,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp170);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 39: _LL16D: _tmp172=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp171=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL16E:
# 776
 Cyc_Absyndump_dump(({const char*_tmp116="__asm__";_tag_dyneither(_tmp116,sizeof(char),8);}));
if(_tmp172)Cyc_Absyndump_dump(({const char*_tmp117=" volatile ";_tag_dyneither(_tmp117,sizeof(char),11);}));
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp171);
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 37: _LL16F: _tmp174=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp173=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL170:
# 784
 Cyc_Absyndump_dump(({const char*_tmp118="tagcheck(";_tag_dyneither(_tmp118,sizeof(char),10);}));Cyc_Absyndump_dumpexp(_tmp174);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp173);
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 18: _LL171: _tmp176=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp175=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL172:
# 789
 Cyc_Absyndump_dump(({const char*_tmp119="offsetof(";_tag_dyneither(_tmp119,sizeof(char),10);}));Cyc_Absyndump_dumptyp(_tmp176);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
{void*_tmp11A=(void*)_tmp175->hd;void*_tmp11B=_tmp11A;unsigned int _tmp120;struct _dyneither_ptr*_tmp11F;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp11B)->tag == 0){_LL19F: _tmp11F=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp11B)->f1;_LL1A0:
# 793
 Cyc_Absyndump_dump_nospace(*_tmp11F);
goto _LL19E;}else{_LL1A1: _tmp120=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp11B)->f1;_LL1A2:
# 796
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11E;_tmp11E.tag=1;_tmp11E.f1=(unsigned long)((int)_tmp120);({void*_tmp11C[1]={& _tmp11E};Cyc_aprintf(({const char*_tmp11D="%d";_tag_dyneither(_tmp11D,sizeof(char),3);}),_tag_dyneither(_tmp11C,sizeof(void*),1));});}));
goto _LL19E;}_LL19E:;}
# 799
if(_tmp175->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 801
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 19: _LL173: _tmp177=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL174:
# 805
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp177);goto _LL12C;case 20: _LL175: _tmp179=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp178=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL176:
# 808
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp179);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp178);
goto _LL12C;case 21: _LL177: _tmp17B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp17A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL178:
# 812
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17B);Cyc_Absyndump_dump_nospace(({const char*_tmp121="->";_tag_dyneither(_tmp121,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(*_tmp17A);
goto _LL12C;case 22: _LL179: _tmp17D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp17C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17A:
# 816
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17D);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp17C);Cyc_Absyndump_dump_char((int)']');goto _LL12C;case 23: _LL17B: _tmp17E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL17C:
# 820
 Cyc_Absyndump_dump(({const char*_tmp122="$(";_tag_dyneither(_tmp122,sizeof(char),3);}));Cyc_Absyndump_dumpexps_prec(20,_tmp17E);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 24: _LL17D: _tmp180=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp17F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17E:
# 823
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp180).f3);
Cyc_Absyndump_dump_char((int)')');
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp17F,({const char*_tmp123="{";_tag_dyneither(_tmp123,sizeof(char),2);}),({const char*_tmp124="}";_tag_dyneither(_tmp124,sizeof(char),2);}),({const char*_tmp125=",";_tag_dyneither(_tmp125,sizeof(char),2);}));
goto _LL12C;case 25: _LL17F: _tmp181=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL180:
# 830
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp181,({const char*_tmp126="{";_tag_dyneither(_tmp126,sizeof(char),2);}),({const char*_tmp127="}";_tag_dyneither(_tmp127,sizeof(char),2);}),({const char*_tmp128=",";_tag_dyneither(_tmp128,sizeof(char),2);}));
goto _LL12C;case 26: _LL181: _tmp184=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp183=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp182=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL182:
# 834
 Cyc_Absyndump_dump(({const char*_tmp129="{for";_tag_dyneither(_tmp129,sizeof(char),5);}));Cyc_Absyndump_dump_str((*_tmp184->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp183);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp182);Cyc_Absyndump_dump_char((int)'}');
goto _LL12C;case 27: _LL183: _tmp186=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp185=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL184:
# 839
 Cyc_Absyndump_dump(({const char*_tmp12A="{for x ";_tag_dyneither(_tmp12A,sizeof(char),8);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp186);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp185);Cyc_Absyndump_dump_char((int)'}');
goto _LL12C;case 28: _LL185: _tmp189=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp188=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp187=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL186:
# 844
 Cyc_Absyndump_dumpqvar(_tmp189);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp188 != 0)
Cyc_Absyndump_dumptps(_tmp188);
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp187,({const char*_tmp12B="";_tag_dyneither(_tmp12B,sizeof(char),1);}),({const char*_tmp12C="}";_tag_dyneither(_tmp12C,sizeof(char),2);}),({const char*_tmp12D=",";_tag_dyneither(_tmp12D,sizeof(char),2);}));
goto _LL12C;case 29: _LL187: _tmp18A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL188:
# 852
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp18A,({const char*_tmp12E="{";_tag_dyneither(_tmp12E,sizeof(char),2);}),({const char*_tmp12F="}";_tag_dyneither(_tmp12F,sizeof(char),2);}),({const char*_tmp130=",";_tag_dyneither(_tmp130,sizeof(char),2);}));
goto _LL12C;case 30: _LL189: _tmp18C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp18B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL18A:
# 856
 Cyc_Absyndump_dumpqvar(_tmp18B->name);
if(_tmp18C != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp18C,({const char*_tmp131="(";_tag_dyneither(_tmp131,sizeof(char),2);}),({const char*_tmp132=")";_tag_dyneither(_tmp132,sizeof(char),2);}),({const char*_tmp133=",";_tag_dyneither(_tmp133,sizeof(char),2);}));
goto _LL12C;case 31: _LL18B: _tmp18D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL18C:
# 860
 Cyc_Absyndump_dumpqvar(_tmp18D->name);goto _LL12C;case 32: _LL18D: _tmp18E=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL18E:
 Cyc_Absyndump_dumpqvar(_tmp18E->name);goto _LL12C;case 33: _LL18F: _tmp193=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).is_calloc;_tmp192=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).rgn;_tmp191=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).elt_type;_tmp190=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).num_elts;_tmp18F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).inline_call;_LL190:
# 864
 Cyc_Absyndump_dumploc(_tmp190->loc);
if(_tmp193){
if(_tmp192 != 0){
Cyc_Absyndump_dump(({const char*_tmp134="rcalloc(";_tag_dyneither(_tmp134,sizeof(char),9);}));
Cyc_Absyndump_dumpexp(_tmp192);Cyc_Absyndump_dump(({const char*_tmp135=",";_tag_dyneither(_tmp135,sizeof(char),2);}));}else{
# 870
Cyc_Absyndump_dump(({const char*_tmp136="calloc";_tag_dyneither(_tmp136,sizeof(char),7);}));}
# 872
Cyc_Absyndump_dumpexp(_tmp190);
Cyc_Absyndump_dump(({const char*_tmp137=",";_tag_dyneither(_tmp137,sizeof(char),2);}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp191)),0));
Cyc_Absyndump_dump(({const char*_tmp138=")";_tag_dyneither(_tmp138,sizeof(char),2);}));}else{
# 877
if(_tmp192 != 0){
if(_tmp18F)
Cyc_Absyndump_dump(({const char*_tmp139="rmalloc_inline(";_tag_dyneither(_tmp139,sizeof(char),16);}));else{
# 881
Cyc_Absyndump_dump(({const char*_tmp13A="rmalloc(";_tag_dyneither(_tmp13A,sizeof(char),9);}));}
Cyc_Absyndump_dumpexp(_tmp192);Cyc_Absyndump_dump(({const char*_tmp13B=",";_tag_dyneither(_tmp13B,sizeof(char),2);}));}else{
# 884
Cyc_Absyndump_dump(({const char*_tmp13C="malloc(";_tag_dyneither(_tmp13C,sizeof(char),8);}));}
# 887
if(_tmp191 != 0)
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp191,0),_tmp190,0));else{
# 890
Cyc_Absyndump_dumpexp(_tmp190);}
Cyc_Absyndump_dump(({const char*_tmp13D=")";_tag_dyneither(_tmp13D,sizeof(char),2);}));}
# 893
goto _LL12C;case 34: _LL191: _tmp195=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp194=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL192:
# 896
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp195);
Cyc_Absyndump_dump_nospace(({const char*_tmp13E=":=:";_tag_dyneither(_tmp13E,sizeof(char),4);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp194);
goto _LL12C;case 35: _LL193: _tmp197=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp196=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL194:
# 903
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp196,({const char*_tmp13F="{";_tag_dyneither(_tmp13F,sizeof(char),2);}),({const char*_tmp140="}";_tag_dyneither(_tmp140,sizeof(char),2);}),({const char*_tmp141=",";_tag_dyneither(_tmp141,sizeof(char),2);}));
goto _LL12C;default: _LL195: _tmp198=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL196:
# 907
 Cyc_Absyndump_dump_nospace(({const char*_tmp142="({";_tag_dyneither(_tmp142,sizeof(char),3);}));
Cyc_Absyndump_dumpstmt(_tmp198,1);
Cyc_Absyndump_dump_nospace(({const char*_tmp143="})";_tag_dyneither(_tmp143,sizeof(char),3);}));
goto _LL12C;}_LL12C:;}
# 912
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 916
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 920
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp199=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp199->where_clause == 0  && (_tmp199->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp19A="default:";_tag_dyneither(_tmp19A,sizeof(char),9);}));else{
# 926
Cyc_Absyndump_dump(({const char*_tmp19B="case";_tag_dyneither(_tmp19B,sizeof(char),5);}));
Cyc_Absyndump_dumppat(_tmp199->pattern);
if(_tmp199->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp19C="&&";_tag_dyneither(_tmp19C,sizeof(char),3);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp199->where_clause));}
# 932
Cyc_Absyndump_dump_nospace(({const char*_tmp19D=":";_tag_dyneither(_tmp19D,sizeof(char),2);}));}
# 934
Cyc_Absyndump_dumpstmt(_tmp199->body,0);}}
# 938
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp19E=s->r;void*_tmp19F=_tmp19E;struct Cyc_Absyn_Stmt*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;struct Cyc_Absyn_Stmt*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D1;struct _dyneither_ptr*_tmp1D0;struct Cyc_Absyn_Stmt*_tmp1CF;struct Cyc_Absyn_Decl*_tmp1CE;struct Cyc_Absyn_Stmt*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Stmt*_tmp1C7;struct _dyneither_ptr*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Stmt*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Stmt*_tmp1C2;struct Cyc_Absyn_Stmt*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Stmt*_tmp1BF;struct Cyc_Absyn_Stmt*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BD;switch(*((int*)_tmp19F)){case 0: _LL1A4: _LL1A5:
 Cyc_Absyndump_dump_semi();goto _LL1A3;case 1: _LL1A6: _tmp1BD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1A7:
 Cyc_Absyndump_dumpexp(_tmp1BD);Cyc_Absyndump_dump_semi();goto _LL1A3;case 2: _LL1A8: _tmp1BF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1BE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1A9:
# 947
 if(Cyc_Absynpp_is_declaration(_tmp1BF)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BF,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 950
Cyc_Absyndump_dumpstmt(_tmp1BF,0);}
if(Cyc_Absynpp_is_declaration(_tmp1BE)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BE,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 957
Cyc_Absyndump_dumpstmt(_tmp1BE,expstmt);}
goto _LL1A3;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1AA: _LL1AB:
 Cyc_Absyndump_dump(({const char*_tmp1A0="return;";_tag_dyneither(_tmp1A0,sizeof(char),8);}));goto _LL1A3;}else{_LL1AC: _tmp1C0=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1AD:
 Cyc_Absyndump_dump(({const char*_tmp1A1="return";_tag_dyneither(_tmp1A1,sizeof(char),7);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1C0));Cyc_Absyndump_dump_semi();goto _LL1A3;}case 4: _LL1AE: _tmp1C3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1C2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_tmp1C1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3;_LL1AF:
# 962
 Cyc_Absyndump_dump(({const char*_tmp1A2="if(";_tag_dyneither(_tmp1A2,sizeof(char),4);}));Cyc_Absyndump_dumpexp(_tmp1C3);
{void*_tmp1A3=_tmp1C2->r;void*_tmp1A4=_tmp1A3;switch(*((int*)_tmp1A4)){case 2: _LL1C9: _LL1CA:
 goto _LL1CC;case 12: _LL1CB: _LL1CC:
 goto _LL1CE;case 4: _LL1CD: _LL1CE:
 goto _LL1D0;case 13: _LL1CF: _LL1D0:
 Cyc_Absyndump_dump_nospace(({const char*_tmp1A5="){";_tag_dyneither(_tmp1A5,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1C2,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1C8;default: _LL1D1: _LL1D2:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1C2,0);}_LL1C8:;}
# 970
{void*_tmp1A6=_tmp1C1->r;void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1A7)->tag == 0){_LL1D4: _LL1D5:
 goto _LL1D3;}else{_LL1D6: _LL1D7:
 Cyc_Absyndump_dump(({const char*_tmp1A8="else{";_tag_dyneither(_tmp1A8,sizeof(char),6);}));Cyc_Absyndump_dumpstmt(_tmp1C1,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1D3;}_LL1D3:;}
# 974
goto _LL1A3;case 5: _LL1B0: _tmp1C5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1).f1;_tmp1C4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1B1:
# 976
 Cyc_Absyndump_dump(({const char*_tmp1A9="while(";_tag_dyneither(_tmp1A9,sizeof(char),7);}));Cyc_Absyndump_dumpexp(_tmp1C5);Cyc_Absyndump_dump_nospace(({const char*_tmp1AA="){";_tag_dyneither(_tmp1AA,sizeof(char),3);}));
Cyc_Absyndump_dumpstmt(_tmp1C4,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;case 6: _LL1B2: _LL1B3:
 Cyc_Absyndump_dump(({const char*_tmp1AB="break;";_tag_dyneither(_tmp1AB,sizeof(char),7);}));goto _LL1A3;case 7: _LL1B4: _LL1B5:
 Cyc_Absyndump_dump(({const char*_tmp1AC="continue;";_tag_dyneither(_tmp1AC,sizeof(char),10);}));goto _LL1A3;case 8: _LL1B6: _tmp1C6=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1B7:
 Cyc_Absyndump_dump(({const char*_tmp1AD="goto";_tag_dyneither(_tmp1AD,sizeof(char),5);}));Cyc_Absyndump_dump_str(_tmp1C6);Cyc_Absyndump_dump_semi();goto _LL1A3;case 9: _LL1B8: _tmp1CA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1C9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_tmp1C8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3).f1;_tmp1C7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f4;_LL1B9:
# 985
 Cyc_Absyndump_dump(({const char*_tmp1AE="for(";_tag_dyneither(_tmp1AE,sizeof(char),5);}));Cyc_Absyndump_dumpexp(_tmp1CA);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C9);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C8);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AF="){";_tag_dyneither(_tmp1AF,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1C7,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;case 10: _LL1BA: _tmp1CC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1CB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BB:
# 989
 Cyc_Absyndump_dump(({const char*_tmp1B0="switch(";_tag_dyneither(_tmp1B0,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1CC);Cyc_Absyndump_dump_nospace(({const char*_tmp1B1="){";_tag_dyneither(_tmp1B1,sizeof(char),3);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1CB);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;case 12: _LL1BC: _tmp1CE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1CD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BD:
# 994
 Cyc_Absyndump_dumpdecl(_tmp1CE);Cyc_Absyndump_dumpstmt(_tmp1CD,expstmt);goto _LL1A3;case 13: _LL1BE: _tmp1D0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1CF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BF:
# 1000
 if(Cyc_Absynpp_is_declaration(_tmp1CF)){
Cyc_Absyndump_dump_str(_tmp1D0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B2=": ({";_tag_dyneither(_tmp1B2,sizeof(char),5);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1B3=": {";_tag_dyneither(_tmp1B3,sizeof(char),4);}));}
Cyc_Absyndump_dumpstmt(_tmp1CF,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B4="});}";_tag_dyneither(_tmp1B4,sizeof(char),5);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 1006
Cyc_Absyndump_dump_str(_tmp1D0);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1CF,expstmt);}
# 1008
goto _LL1A3;case 14: _LL1C0: _tmp1D2=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1D1=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_LL1C1:
# 1011
 Cyc_Absyndump_dump(({const char*_tmp1B5="do{";_tag_dyneither(_tmp1B5,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1D2,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B6="}while(";_tag_dyneither(_tmp1B6,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1D1);Cyc_Absyndump_dump_nospace(({const char*_tmp1B7=");";_tag_dyneither(_tmp1B7,sizeof(char),3);}));
goto _LL1A3;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1C2: _LL1C3:
# 1015
 Cyc_Absyndump_dump(({const char*_tmp1B8="fallthru;";_tag_dyneither(_tmp1B8,sizeof(char),10);}));goto _LL1A3;}else{_LL1C4: _tmp1D3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1C5:
# 1017
 Cyc_Absyndump_dump(({const char*_tmp1B9="fallthru(";_tag_dyneither(_tmp1B9,sizeof(char),10);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1D3);Cyc_Absyndump_dump_nospace(({const char*_tmp1BA=");";_tag_dyneither(_tmp1BA,sizeof(char),3);}));goto _LL1A3;}default: _LL1C6: _tmp1D5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1D4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1C7:
# 1020
 Cyc_Absyndump_dump(({const char*_tmp1BB="try";_tag_dyneither(_tmp1BB,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1D5,0);
Cyc_Absyndump_dump(({const char*_tmp1BC="catch{";_tag_dyneither(_tmp1BC,sizeof(char),7);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1D4);Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;}_LL1A3:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1028
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,({const char*_tmp1D6="";_tag_dyneither(_tmp1D6,sizeof(char),1);}),({const char*_tmp1D7="=";_tag_dyneither(_tmp1D7,sizeof(char),2);}),({const char*_tmp1D8="=";_tag_dyneither(_tmp1D8,sizeof(char),2);}));
Cyc_Absyndump_dumppat((*dp).f2);}
# 1033
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D9=p->r;void*_tmp1DA=_tmp1D9;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Enumfield*_tmp22A;struct Cyc_Absyn_Enumfield*_tmp229;struct Cyc_Absyn_Datatypefield*_tmp228;struct Cyc_List_List*_tmp227;int _tmp226;struct Cyc_List_List*_tmp225;struct Cyc_List_List*_tmp224;int _tmp223;union Cyc_Absyn_AggrInfoU _tmp222;struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp220;int _tmp21F;struct _tuple0*_tmp21E;struct Cyc_List_List*_tmp21D;int _tmp21C;struct _tuple0*_tmp21B;struct Cyc_Absyn_Tvar*_tmp21A;struct Cyc_Absyn_Vardecl*_tmp219;struct Cyc_Absyn_Vardecl*_tmp218;struct Cyc_Absyn_Pat*_tmp217;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Pat*_tmp215;struct Cyc_List_List*_tmp214;int _tmp213;struct Cyc_Absyn_Tvar*_tmp212;struct Cyc_Absyn_Vardecl*_tmp211;struct Cyc_Absyn_Vardecl*_tmp210;struct Cyc_Absyn_Pat*_tmp20F;struct Cyc_Absyn_Vardecl*_tmp20E;struct _dyneither_ptr _tmp20D;char _tmp20C;int _tmp20B;int _tmp20A;int _tmp209;switch(*((int*)_tmp1DA)){case 0: _LL1D9: _LL1DA:
 Cyc_Absyndump_dump_char((int)'_');goto _LL1D8;case 9: _LL1DB: _LL1DC:
 Cyc_Absyndump_dump(({const char*_tmp1DB="NULL";_tag_dyneither(_tmp1DB,sizeof(char),5);}));goto _LL1D8;case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1){case Cyc_Absyn_None: _LL1DD: _tmp209=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1DE:
 _tmp20A=_tmp209;goto _LL1E0;case Cyc_Absyn_Signed: _LL1DF: _tmp20A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1E0:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1DE;_tmp1DE.tag=1;_tmp1DE.f1=(unsigned long)_tmp20A;({void*_tmp1DC[1]={& _tmp1DE};Cyc_aprintf(({const char*_tmp1DD="%d";_tag_dyneither(_tmp1DD,sizeof(char),3);}),_tag_dyneither(_tmp1DC,sizeof(void*),1));});}));goto _LL1D8;default: _LL1E1: _tmp20B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1E2:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=1;_tmp1E1.f1=(unsigned int)_tmp20B;({void*_tmp1DF[1]={& _tmp1E1};Cyc_aprintf(({const char*_tmp1E0="%u";_tag_dyneither(_tmp1E0,sizeof(char),3);}),_tag_dyneither(_tmp1DF,sizeof(void*),1));});}));goto _LL1D8;}case 11: _LL1E3: _tmp20C=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1E4:
# 1041
 Cyc_Absyndump_dump(({const char*_tmp1E2="'";_tag_dyneither(_tmp1E2,sizeof(char),2);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp20C));Cyc_Absyndump_dump_nospace(({const char*_tmp1E3="'";_tag_dyneither(_tmp1E3,sizeof(char),2);}));goto _LL1D8;case 12: _LL1E5: _tmp20D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1E6:
 Cyc_Absyndump_dump(_tmp20D);goto _LL1D8;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2)->r)->tag == 0){_LL1E7: _tmp20E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1E8:
 Cyc_Absyndump_dumpqvar(_tmp20E->name);goto _LL1D8;}else{_LL1E9: _tmp210=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp20F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1EA:
 Cyc_Absyndump_dumpqvar(_tmp210->name);Cyc_Absyndump_dump(({const char*_tmp1E4=" as ";_tag_dyneither(_tmp1E4,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp20F);goto _LL1D8;}case 2: _LL1EB: _tmp212=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp211=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1EC:
# 1046
 Cyc_Absyndump_dump(({const char*_tmp1E5="alias";_tag_dyneither(_tmp1E5,sizeof(char),6);}));
Cyc_Absyndump_dump(({const char*_tmp1E6="<";_tag_dyneither(_tmp1E6,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp212);Cyc_Absyndump_dump(({const char*_tmp1E7=">";_tag_dyneither(_tmp1E7,sizeof(char),2);}));
Cyc_Absyndump_dumpvardecl(_tmp211,p->loc,0);
goto _LL1D8;case 5: _LL1ED: _tmp214=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp213=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1EE: {
# 1051
struct _dyneither_ptr term=_tmp213?({const char*_tmp1EA=", ...)";_tag_dyneither(_tmp1EA,sizeof(char),7);}):({const char*_tmp1EB=")";_tag_dyneither(_tmp1EB,sizeof(char),2);});
((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp214,({const char*_tmp1E8="$(";_tag_dyneither(_tmp1E8,sizeof(char),3);}),term,({const char*_tmp1E9=",";_tag_dyneither(_tmp1E9,sizeof(char),2);}));goto _LL1D8;}case 6: _LL1EF: _tmp215=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1F0:
 Cyc_Absyndump_dump(({const char*_tmp1EC="&";_tag_dyneither(_tmp1EC,sizeof(char),2);}));Cyc_Absyndump_dumppat(_tmp215);goto _LL1D8;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2)->r)->tag == 0){_LL1F1: _tmp216=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1F2:
# 1055
 Cyc_Absyndump_dump(({const char*_tmp1ED="*";_tag_dyneither(_tmp1ED,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp216->name);goto _LL1D8;}else{_LL1F3: _tmp218=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp217=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1F4:
# 1057
 Cyc_Absyndump_dump(({const char*_tmp1EE="*";_tag_dyneither(_tmp1EE,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp218->name);Cyc_Absyndump_dump(({const char*_tmp1EF=" as ";_tag_dyneither(_tmp1EF,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp217);goto _LL1D8;}case 4: _LL1F5: _tmp21A=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp219=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1F6:
# 1059
 Cyc_Absyndump_dumpqvar(_tmp219->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp21A);Cyc_Absyndump_dump_char((int)'>');goto _LL1D8;case 15: _LL1F7: _tmp21B=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1F8:
 Cyc_Absyndump_dumpqvar(_tmp21B);goto _LL1D8;case 16: _LL1F9: _tmp21E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp21D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp21C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_LL1FA: {
# 1062
struct _dyneither_ptr term=_tmp21C?({const char*_tmp1F2=", ...)";_tag_dyneither(_tmp1F2,sizeof(char),7);}):({const char*_tmp1F3=")";_tag_dyneither(_tmp1F3,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp21E);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp21D,({const char*_tmp1F0="(";_tag_dyneither(_tmp1F0,sizeof(char),2);}),term,({const char*_tmp1F1=",";_tag_dyneither(_tmp1F1,sizeof(char),2);}));goto _LL1D8;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1 != 0){_LL1FB: _tmp222=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1)->aggr_info;_tmp221=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp220=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp21F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL1FC: {
# 1065
struct _tuple10 _tmp1F4=Cyc_Absyn_aggr_kinded_name(_tmp222);struct _tuple10 _tmp1F5=_tmp1F4;struct _tuple0*_tmp1FD;_LL208: _tmp1FD=_tmp1F5.f2;_LL209:;{
struct _dyneither_ptr term=_tmp21F?({const char*_tmp1FB=", ...)";_tag_dyneither(_tmp1FB,sizeof(char),7);}):({const char*_tmp1FC=")";_tag_dyneither(_tmp1FC,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp1FD);Cyc_Absyndump_dump_char((int)'{');
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp221,({const char*_tmp1F6="[";_tag_dyneither(_tmp1F6,sizeof(char),2);}),({const char*_tmp1F7="]";_tag_dyneither(_tmp1F7,sizeof(char),2);}),({const char*_tmp1F8=",";_tag_dyneither(_tmp1F8,sizeof(char),2);}));
((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp220,({const char*_tmp1F9="";_tag_dyneither(_tmp1F9,sizeof(char),1);}),term,({const char*_tmp1FA=",";_tag_dyneither(_tmp1FA,sizeof(char),2);}));
goto _LL1D8;};}}else{_LL1FD: _tmp225=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp224=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp223=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL1FE: {
# 1072
struct _dyneither_ptr term=_tmp223?({const char*_tmp203=", ...)";_tag_dyneither(_tmp203,sizeof(char),7);}):({const char*_tmp204=")";_tag_dyneither(_tmp204,sizeof(char),2);});
Cyc_Absyndump_dump_char((int)'{');
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp225,({const char*_tmp1FE="[";_tag_dyneither(_tmp1FE,sizeof(char),2);}),({const char*_tmp1FF="]";_tag_dyneither(_tmp1FF,sizeof(char),2);}),({const char*_tmp200=",";_tag_dyneither(_tmp200,sizeof(char),2);}));
((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp224,({const char*_tmp201="";_tag_dyneither(_tmp201,sizeof(char),1);}),term,({const char*_tmp202=",";_tag_dyneither(_tmp202,sizeof(char),2);}));
goto _LL1D8;}}case 8: _LL1FF: _tmp228=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp227=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp226=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL200: {
# 1078
struct _dyneither_ptr term=_tmp226?({const char*_tmp207=", ...)";_tag_dyneither(_tmp207,sizeof(char),7);}):({const char*_tmp208=")";_tag_dyneither(_tmp208,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp228->name);
if(_tmp227 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp227,({const char*_tmp205="(";_tag_dyneither(_tmp205,sizeof(char),2);}),term,({const char*_tmp206=",";_tag_dyneither(_tmp206,sizeof(char),2);}));
goto _LL1D8;}case 13: _LL201: _tmp229=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL202:
 _tmp22A=_tmp229;goto _LL204;case 14: _LL203: _tmp22A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL204:
 Cyc_Absyndump_dumpqvar(_tmp22A->name);goto _LL1D8;default: _LL205: _tmp22B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL206:
 Cyc_Absyndump_dumpexp(_tmp22B);goto _LL1D8;}_LL1D8:;}
# 1088
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1093
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,({const char*_tmp22C=",";_tag_dyneither(_tmp22C,sizeof(char),2);}));}
# 1096
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp22D=" = ";_tag_dyneither(_tmp22D,sizeof(char),4);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1103
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,({const char*_tmp22E=",";_tag_dyneither(_tmp22E,sizeof(char),2);}));}
# 1107
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp22F=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp230=_tmp22F;struct _dyneither_ptr*_tmp238;struct Cyc_Absyn_Tqual _tmp237;void*_tmp236;struct Cyc_Absyn_Exp*_tmp235;struct Cyc_List_List*_tmp234;struct Cyc_Absyn_Exp*_tmp233;_LL20B: _tmp238=_tmp230->name;_tmp237=_tmp230->tq;_tmp236=_tmp230->type;_tmp235=_tmp230->width;_tmp234=_tmp230->attributes;_tmp233=_tmp230->requires_clause;_LL20C:;
# 1111
{enum Cyc_Cyclone_C_Compilers _tmp231=Cyc_Cyclone_c_compiler;if(_tmp231 == Cyc_Cyclone_Gcc_c){_LL20E: _LL20F:
# 1113
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp237,_tmp236,Cyc_Absyndump_dump_str,_tmp238);
Cyc_Absyndump_dumpatts(_tmp234);
goto _LL20D;}else{_LL210: _LL211:
# 1117
 Cyc_Absyndump_dumpatts(_tmp234);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp237,_tmp236,Cyc_Absyndump_dump_str,_tmp238);
goto _LL20D;}_LL20D:;}
# 1121
if((unsigned int)_tmp233){
Cyc_Absyndump_dump(({const char*_tmp232="@requires ";_tag_dyneither(_tmp232,sizeof(char),11);}));
Cyc_Absyndump_dumpexp(_tmp233);}
# 1125
if(_tmp235 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp235);}
# 1129
Cyc_Absyndump_dump_semi();}}
# 1133
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1138
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1142
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp239=(*pr).f1;void*_tmp23A=_tmp239;switch(*((int*)_tmp23A)){case 11: _LL213: _LL214:
 goto _LL212;case 1: _LL215: _LL216:
 Cyc_Absyndump_dump(({const char*_tmp23B="_stdcall";_tag_dyneither(_tmp23B,sizeof(char),9);}));goto _LL212;case 2: _LL217: _LL218:
 Cyc_Absyndump_dump(({const char*_tmp23C="_cdecl";_tag_dyneither(_tmp23C,sizeof(char),7);}));goto _LL212;case 3: _LL219: _LL21A:
 Cyc_Absyndump_dump(({const char*_tmp23D="_fastcall";_tag_dyneither(_tmp23D,sizeof(char),10);}));goto _LL212;default: _LL21B: _LL21C:
 goto _LL212;}_LL212:;}
# 1150
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1152
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1157
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1164
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1166
void*_tmp23E=t;void*_tmp243;void*_tmp242;switch(*((int*)_tmp23E)){case 1: _LL21E: _tmp242=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23E)->f2;if(_tmp242 != 0){_LL21F:
 return Cyc_Absyndump_is_char_ptr(_tmp242);}else{goto _LL222;}case 5: _LL220: _tmp243=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E)->f1).elt_typ;_LL221:
# 1169
 L: {
void*_tmp23F=_tmp243;void*_tmp241;void*_tmp240;switch(*((int*)_tmp23F)){case 1: _LL225: _tmp240=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23F)->f2;if(_tmp240 != 0){_LL226:
 _tmp243=_tmp240;goto L;}else{goto _LL22B;}case 17: _LL227: _tmp241=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23F)->f4;if(_tmp241 != 0){_LL228:
 _tmp243=_tmp241;goto L;}else{goto _LL22B;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23F)->f2 == Cyc_Absyn_Char_sz){_LL229: _LL22A:
 return 1;}else{goto _LL22B;}default: _LL22B: _LL22C:
 return 0;}_LL224:;}default: _LL222: _LL223:
# 1176
 return 0;}_LL21D:;}
# 1180
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp244=vd;enum Cyc_Absyn_Scope _tmp25B;struct _tuple0*_tmp25A;unsigned int _tmp259;struct Cyc_Absyn_Tqual _tmp258;void*_tmp257;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_List_List*_tmp255;_LL22E: _tmp25B=_tmp244->sc;_tmp25A=_tmp244->name;_tmp259=_tmp244->varloc;_tmp258=_tmp244->tq;_tmp257=_tmp244->type;_tmp256=_tmp244->initializer;_tmp255=_tmp244->attributes;_LL22F:;
{enum Cyc_Cyclone_C_Compilers _tmp245=Cyc_Cyclone_c_compiler;if(_tmp245 == Cyc_Cyclone_Gcc_c){_LL231: _LL232:
# 1185
 if(_tmp25B == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp246=Cyc_Tcutil_compress(_tmp257);void*_tmp247=_tmp246;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp247)->tag == 9){_LL236: _LL237:
 goto _LL235;}else{_LL238: _LL239:
 Cyc_Absyndump_dumpscope(_tmp25B);}_LL235:;}else{
# 1191
Cyc_Absyndump_dumpscope(_tmp25B);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp258,_tmp257,Cyc_Absyndump_dumpqvar,_tmp25A);
Cyc_Absyndump_dumpatts(_tmp255);
goto _LL230;}else{_LL233: _LL234:
# 1196
 Cyc_Absyndump_dumpatts(_tmp255);
Cyc_Absyndump_dumpscope(_tmp25B);{
struct _RegionHandle _tmp248=_new_region("temp");struct _RegionHandle*temp=& _tmp248;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp257);
struct _tuple11 _tmp249=Cyc_Absynpp_to_tms(temp,_tmp258,_tmp257);struct _tuple11 _tmp24A=_tmp249;struct Cyc_Absyn_Tqual _tmp254;void*_tmp253;struct Cyc_List_List*_tmp252;_LL23B: _tmp254=_tmp24A.f1;_tmp253=_tmp24A.f2;_tmp252=_tmp24A.f3;_LL23C:;{
# 1202
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp252;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp24B=(void*)tms2->hd;void*_tmp24C=_tmp24B;struct Cyc_List_List*_tmp24F;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp24C)->tag == 5){_LL23E: _tmp24F=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp24C)->f2;_LL23F:
# 1206
 for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
void*_tmp24D=(void*)_tmp24F->hd;void*_tmp24E=_tmp24D;switch(*((int*)_tmp24E)){case 1: _LL243: _LL244:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL242;case 2: _LL245: _LL246:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL242;case 3: _LL247: _LL248:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL242;default: _LL249: _LL24A:
 goto _LL242;}_LL242:;}
# 1213
goto _LL23D;}else{_LL240: _LL241:
 goto _LL23D;}_LL23D:;}}
# 1216
Cyc_Absyndump_dumptq(_tmp254);
Cyc_Absyndump_dumpntyp(_tmp253);{
struct _tuple19 _tmp250=({struct _tuple19 _tmp251;_tmp251.f1=call_conv;_tmp251.f2=_tmp25A;_tmp251;});
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp252),Cyc_Absyndump_dump_callconv_qvar,& _tmp250);};};}
# 1221
_npop_handler(0);goto _LL230;
# 1198
;_pop_region(temp);};}_LL230:;}
# 1224
if(_tmp256 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp256);}
# 1228
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1231
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp25C="@tagged ";_tag_dyneither(_tmp25C,sizeof(char),9);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1240
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,({const char*_tmp25D="<";_tag_dyneither(_tmp25D,sizeof(char),2);}),({const char*_tmp25E=">";_tag_dyneither(_tmp25E,sizeof(char),2);}),({const char*_tmp25F=",";_tag_dyneither(_tmp25F,sizeof(char),2);}));
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1247
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump(({const char*_tmp260="}";_tag_dyneither(_tmp260,sizeof(char),2);}));
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1252
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp261=ed;enum Cyc_Absyn_Scope _tmp266;struct _tuple0*_tmp265;struct Cyc_Core_Opt*_tmp264;_LL24C: _tmp266=_tmp261->sc;_tmp265=_tmp261->name;_tmp264=_tmp261->fields;_LL24D:;
Cyc_Absyndump_dumpscope(_tmp266);
Cyc_Absyndump_dump(({const char*_tmp262="enum ";_tag_dyneither(_tmp262,sizeof(char),6);}));
Cyc_Absyndump_dumpqvar(_tmp265);
if(_tmp264 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp264->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp263="}";_tag_dyneither(_tmp263,sizeof(char),2);}));}}
# 1263
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp267=dd;enum Cyc_Absyn_Scope _tmp26F;struct _tuple0*_tmp26E;struct Cyc_List_List*_tmp26D;struct Cyc_Core_Opt*_tmp26C;int _tmp26B;_LL24F: _tmp26F=_tmp267->sc;_tmp26E=_tmp267->name;_tmp26D=_tmp267->tvs;_tmp26C=_tmp267->fields;_tmp26B=_tmp267->is_extensible;_LL250:;
Cyc_Absyndump_dumpscope(_tmp26F);
if(_tmp26B)Cyc_Absyndump_dump(({const char*_tmp268="@extensible ";_tag_dyneither(_tmp268,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp269="datatype ";_tag_dyneither(_tmp269,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp26E);
Cyc_Absyndump_dumptvars(_tmp26D);
if(_tmp26C != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp26C->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp26A="}";_tag_dyneither(_tmp26A,sizeof(char),2);}));}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1277
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp270=d->r;void*_tmp271=_tmp270;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp29E;struct _tuple0*_tmp29D;struct Cyc_List_List*_tmp29C;struct _dyneither_ptr*_tmp29B;struct Cyc_List_List*_tmp29A;struct Cyc_Absyn_Typedefdecl*_tmp299;struct Cyc_Absyn_Tvar*_tmp298;struct Cyc_Absyn_Vardecl*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_List_List*_tmp295;struct Cyc_Absyn_Pat*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Enumdecl*_tmp292;struct Cyc_Absyn_Datatypedecl*_tmp291;struct Cyc_Absyn_Aggrdecl*_tmp290;struct Cyc_Absyn_Fndecl*_tmp28F;struct Cyc_Absyn_Vardecl*_tmp28E;switch(*((int*)_tmp271)){case 0: _LL252: _tmp28E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL253:
# 1281
 Cyc_Absyndump_dumpvardecl(_tmp28E,d->loc,1);
goto _LL251;case 1: _LL254: _tmp28F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL255:
# 1285
{enum Cyc_Cyclone_C_Compilers _tmp272=Cyc_Cyclone_c_compiler;if(_tmp272 == Cyc_Cyclone_Vc_c){_LL271: _LL272:
 Cyc_Absyndump_dumpatts(_tmp28F->attributes);goto _LL270;}else{_LL273: _LL274:
 goto _LL270;}_LL270:;}
# 1289
if(_tmp28F->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp273=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp276;if(_tmp273 == Cyc_Cyclone_Vc_c){_LL276: _LL277:
 Cyc_Absyndump_dump(({const char*_tmp274="__inline";_tag_dyneither(_tmp274,sizeof(char),9);}));goto _LL275;}else{_LL278: _tmp276=_tmp273;_LL279:
 Cyc_Absyndump_dump(({const char*_tmp275="inline";_tag_dyneither(_tmp275,sizeof(char),7);}));goto _LL275;}_LL275:;}
# 1294
Cyc_Absyndump_dumpscope(_tmp28F->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp279;_tmp279.tag=9;_tmp279.f1=({struct Cyc_Absyn_FnInfo _tmp27A;_tmp27A.tvars=_tmp28F->tvs;_tmp27A.effect=_tmp28F->effect;_tmp27A.ret_tqual=_tmp28F->ret_tqual;_tmp27A.ret_typ=_tmp28F->ret_type;_tmp27A.args=
# 1297
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp28F->args);_tmp27A.c_varargs=_tmp28F->c_varargs;_tmp27A.cyc_varargs=_tmp28F->cyc_varargs;_tmp27A.rgn_po=_tmp28F->rgn_po;_tmp27A.attributes=0;_tmp27A.requires_clause=_tmp28F->requires_clause;_tmp27A.requires_relns=_tmp28F->requires_relns;_tmp27A.ensures_clause=_tmp28F->ensures_clause;_tmp27A.ensures_relns=_tmp28F->ensures_relns;_tmp27A;});_tmp279;});_tmp278;});
# 1302
{enum Cyc_Cyclone_C_Compilers _tmp277=Cyc_Cyclone_c_compiler;if(_tmp277 == Cyc_Cyclone_Gcc_c){_LL27B: _LL27C:
# 1304
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp28F);
goto _LL27A;}else{_LL27D: _LL27E:
# 1307
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp28F);
goto _LL27A;}_LL27A:;}
# 1310
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp28F->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL251;};case 5: _LL256: _tmp290=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL257:
# 1315
 Cyc_Absyndump_dump_aggrdecl(_tmp290);Cyc_Absyndump_dump_semi();
goto _LL251;case 6: _LL258: _tmp291=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL259:
# 1318
 Cyc_Absyndump_dump_datatypedecl(_tmp291);Cyc_Absyndump_dump_semi();
goto _LL251;case 7: _LL25A: _tmp292=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL25B:
# 1321
 Cyc_Absyndump_dump_enumdecl(_tmp292);Cyc_Absyndump_dump_semi();
goto _LL251;case 2: _LL25C: _tmp294=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_tmp293=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp271)->f3;_LL25D:
# 1324
 Cyc_Absyndump_dump(({const char*_tmp27B="let";_tag_dyneither(_tmp27B,sizeof(char),4);}));
Cyc_Absyndump_dumppat(_tmp294);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp293);
Cyc_Absyndump_dump_semi();
goto _LL251;case 3: _LL25E: _tmp295=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL25F:
# 1331
 Cyc_Absyndump_dump(({const char*_tmp27C="let ";_tag_dyneither(_tmp27C,sizeof(char),5);}));
Cyc_Absyndump_dumpids(_tmp295);
Cyc_Absyndump_dump_semi();
goto _LL251;case 4: _LL260: _tmp298=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_tmp297=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp271)->f2;_tmp296=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp271)->f3;_LL261:
# 1336
 Cyc_Absyndump_dump(({const char*_tmp27D="region";_tag_dyneither(_tmp27D,sizeof(char),7);}));
Cyc_Absyndump_dump(({const char*_tmp27E="<";_tag_dyneither(_tmp27E,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp298);Cyc_Absyndump_dump(({const char*_tmp27F="> ";_tag_dyneither(_tmp27F,sizeof(char),3);}));Cyc_Absyndump_dumpqvar(_tmp297->name);
if((unsigned int)_tmp296){
Cyc_Absyndump_dump(({const char*_tmp280=" = open(";_tag_dyneither(_tmp280,sizeof(char),9);}));
Cyc_Absyndump_dumpexp(_tmp296);
Cyc_Absyndump_dump(({const char*_tmp281=")";_tag_dyneither(_tmp281,sizeof(char),2);}));}
# 1343
Cyc_Absyndump_dump_semi();
goto _LL251;case 8: _LL262: _tmp299=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL263:
# 1350
 Cyc_Absyndump_dump(({const char*_tmp282="typedef";_tag_dyneither(_tmp282,sizeof(char),8);}));{
void*t;
if(_tmp299->defn == 0)
t=Cyc_Absyn_new_evar(_tmp299->kind,0);else{
# 1355
t=(void*)_check_null(_tmp299->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp299->tq,t,Cyc_Absyndump_dumptypedefname,_tmp299);
Cyc_Absyndump_dumpatts(_tmp299->atts);
Cyc_Absyndump_dump_semi();
# 1360
goto _LL251;};case 9: _LL264: _tmp29B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_tmp29A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp271)->f2;_LL265:
# 1362
 Cyc_Absyndump_dump(({const char*_tmp283="namespace ";_tag_dyneither(_tmp283,sizeof(char),11);}));
Cyc_Absyndump_dump_str(_tmp29B);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp29A != 0;_tmp29A=_tmp29A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29A->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL251;case 10: _LL266: _tmp29D=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_tmp29C=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp271)->f2;_LL267:
# 1370
 Cyc_Absyndump_dump(({const char*_tmp284="using ";_tag_dyneither(_tmp284,sizeof(char),7);}));
Cyc_Absyndump_dumpqvar(_tmp29D);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp29C != 0;_tmp29C=_tmp29C->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29C->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL251;case 11: _LL268: _tmp29E=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_LL269:
# 1378
 Cyc_Absyndump_dump(({const char*_tmp285="extern \"C\" {";_tag_dyneither(_tmp285,sizeof(char),13);}));
for(0;_tmp29E != 0;_tmp29E=_tmp29E->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29E->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL251;case 12: _LL26A: _tmp2A0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp271)->f1;_tmp29F=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp271)->f2;_LL26B:
# 1384
 Cyc_Absyndump_dump(({const char*_tmp286="extern \"C include\" {";_tag_dyneither(_tmp286,sizeof(char),21);}));
for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp2A0->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp29F != 0){
Cyc_Absyndump_dump(({const char*_tmp287=" export {";_tag_dyneither(_tmp287,sizeof(char),10);}));
for(0;_tmp29F != 0;_tmp29F=_tmp29F->tl){
struct _tuple20 _tmp288=*((struct _tuple20*)_tmp29F->hd);struct _tuple20 _tmp289=_tmp288;struct _tuple0*_tmp28A;_LL280: _tmp28A=_tmp289.f2;_LL281:;
Cyc_Absyndump_dumpqvar(_tmp28A);
if(_tmp29F->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 1395
Cyc_Absyndump_dump(({const char*_tmp28B="}";_tag_dyneither(_tmp28B,sizeof(char),2);}));}
# 1397
goto _LL251;case 13: _LL26C: _LL26D:
# 1399
 Cyc_Absyndump_dump(({const char*_tmp28C=" __cyclone_port_on__; ";_tag_dyneither(_tmp28C,sizeof(char),23);}));
goto _LL251;default: _LL26E: _LL26F:
# 1402
 Cyc_Absyndump_dump(({const char*_tmp28D=" __cyclone_port_off__; ";_tag_dyneither(_tmp28D,sizeof(char),24);}));
goto _LL251;}_LL251:;};}
# 1407
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1414
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1419
void*_tmp2A1=(void*)tms->hd;void*_tmp2A2=_tmp2A1;void*_tmp2D2;union Cyc_Absyn_Constraint*_tmp2D1;union Cyc_Absyn_Constraint*_tmp2D0;union Cyc_Absyn_Constraint*_tmp2CF;struct Cyc_Absyn_Tqual _tmp2CE;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A2)->tag == 2){_LL283: _tmp2D2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A2)->f1).rgn;_tmp2D1=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A2)->f1).nullable;_tmp2D0=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A2)->f1).bounds;_tmp2CF=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A2)->f1).zero_term;_tmp2CE=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A2)->f2;_LL284:
# 1423
{void*_tmp2A3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2D0);void*_tmp2A4=_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A4)->tag == 0){_LL288: _LL289:
 Cyc_Absyndump_dump_char((int)'?');goto _LL287;}else{_LL28A: _tmp2A5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A4)->f1;_LL28B:
# 1426
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp2D1)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp2A5);
goto _LL287;}_LL287:;}
# 1430
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CF))Cyc_Absyndump_dump(({const char*_tmp2A6="@zeroterm";_tag_dyneither(_tmp2A6,sizeof(char),10);}));
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CF))Cyc_Absyndump_dump(({const char*_tmp2A7="@nozeroterm";_tag_dyneither(_tmp2A7,sizeof(char),12);}));
{void*_tmp2A8=Cyc_Tcutil_compress(_tmp2D2);void*_tmp2A9=_tmp2A8;struct Cyc_Absyn_Tvar*_tmp2B1;switch(*((int*)_tmp2A9)){case 20: _LL28D: _LL28E:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2AA="`H";_tag_dyneither(_tmp2AA,sizeof(char),3);}));goto _LL28C;case 21: _LL28F: _LL290:
 Cyc_Absyndump_dump(({const char*_tmp2AB="`U";_tag_dyneither(_tmp2AB,sizeof(char),3);}));goto _LL28C;case 22: _LL291: _LL292:
 Cyc_Absyndump_dump(({const char*_tmp2AC="`RC";_tag_dyneither(_tmp2AC,sizeof(char),4);}));goto _LL28C;case 2: _LL293: _tmp2B1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2A9)->f1;_LL294:
 Cyc_Absyndump_dumptvar(_tmp2B1);goto _LL28C;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A9)->f2 == 0){_LL295: _LL296:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2D2));goto _LL28C;}else{goto _LL299;}case 17: _LL297: _LL298:
 Cyc_Absyndump_dump(({const char*_tmp2AD="@region(";_tag_dyneither(_tmp2AD,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp2D2);Cyc_Absyndump_dump(({const char*_tmp2AE=")";_tag_dyneither(_tmp2AE,sizeof(char),2);}));goto _LL28C;default: _LL299: _LL29A:
({void*_tmp2AF=0;Cyc_Tcutil_impos(({const char*_tmp2B0="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2B0,sizeof(char),37);}),_tag_dyneither(_tmp2AF,sizeof(void*),0));});}_LL28C:;}
# 1441
Cyc_Absyndump_dumptq(_tmp2CE);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL285: _LL286: {
# 1446
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2B2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2B3=_tmp2B2;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2B3)->tag == 2){_LL29C: _LL29D:
 next_is_pointer=1;goto _LL29B;}else{_LL29E: _LL29F:
 goto _LL29B;}_LL29B:;}
# 1452
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2B4=(void*)tms->hd;void*_tmp2B5=_tmp2B4;struct Cyc_List_List*_tmp2CD;struct Cyc_List_List*_tmp2CC;unsigned int _tmp2CB;int _tmp2CA;struct Cyc_List_List*_tmp2C9;unsigned int _tmp2C8;struct Cyc_List_List*_tmp2C7;int _tmp2C6;struct Cyc_Absyn_VarargInfo*_tmp2C5;void*_tmp2C4;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;union Cyc_Absyn_Constraint*_tmp2BF;union Cyc_Absyn_Constraint*_tmp2BE;switch(*((int*)_tmp2B5)){case 0: _LL2A1: _tmp2BE=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1;_LL2A2:
# 1459
 Cyc_Absyndump_dump(({const char*_tmp2B6="[]";_tag_dyneither(_tmp2B6,sizeof(char),3);}));
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2BE))Cyc_Absyndump_dump(({const char*_tmp2B7="@zeroterm";_tag_dyneither(_tmp2B7,sizeof(char),10);}));
goto _LL2A0;case 1: _LL2A3: _tmp2C0=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1;_tmp2BF=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f2;_LL2A4:
# 1463
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2C0);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2BF))Cyc_Absyndump_dump(({const char*_tmp2B8="@zeroterm";_tag_dyneither(_tmp2B8,sizeof(char),10);}));
goto _LL2A0;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->tag == 1){_LL2A5: _tmp2C7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f1;_tmp2C6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f2;_tmp2C5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f3;_tmp2C4=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f4;_tmp2C3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f5;_tmp2C2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f6;_tmp2C1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f7;_LL2A6:
# 1467
 Cyc_Absyndump_dumpfunargs(_tmp2C7,_tmp2C6,_tmp2C5,_tmp2C4,_tmp2C3,_tmp2C2,_tmp2C1);goto _LL2A0;}else{_LL2A7: _tmp2C9=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f1;_tmp2C8=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1)->f2;_LL2A8:
# 1469
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp2C9,({const char*_tmp2B9="(";_tag_dyneither(_tmp2B9,sizeof(char),2);}),({const char*_tmp2BA=")";_tag_dyneither(_tmp2BA,sizeof(char),2);}),({const char*_tmp2BB=",";_tag_dyneither(_tmp2BB,sizeof(char),2);}));goto _LL2A0;}case 4: _LL2A9: _tmp2CC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f1;_tmp2CB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f2;_tmp2CA=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f3;_LL2AA:
# 1471
 if(_tmp2CA)Cyc_Absyndump_dumpkindedtvars(_tmp2CC);else{Cyc_Absyndump_dumptvars(_tmp2CC);}goto _LL2A0;case 5: _LL2AB: _tmp2CD=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2B5)->f2;_LL2AC:
 Cyc_Absyndump_dumpatts(_tmp2CD);goto _LL2A0;default: _LL2AD: _LL2AE:
({void*_tmp2BC=0;Cyc_Tcutil_impos(({const char*_tmp2BD="dumptms";_tag_dyneither(_tmp2BD,sizeof(char),8);}),_tag_dyneither(_tmp2BC,sizeof(void*),0));});}_LL2A0:;}
# 1475
return;}}_LL282:;};}
# 1479
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2D3=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2D4=_new_region("temp");struct _RegionHandle*temp=& _tmp2D4;_push_region(temp);
{struct _tuple11 _tmp2D5=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2D6=_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D9;void*_tmp2D8;struct Cyc_List_List*_tmp2D7;_LL2B0: _tmp2D9=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;_tmp2D7=_tmp2D6.f3;_LL2B1:;
Cyc_Absyndump_dumptq(_tmp2D9);
Cyc_Absyndump_dumpntyp(_tmp2D8);
Cyc_Absyndump_dumptms(_tmp2D3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D7),f,a);}
# 1482
;_pop_region(temp);}
# 1489
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1491
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2DA=0;Cyc_fprintf(f,({const char*_tmp2DB="\n";_tag_dyneither(_tmp2DB,sizeof(char),2);}),_tag_dyneither(_tmp2DA,sizeof(void*),0));});}
# 1497
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2DC=d->r;void*_tmp2DD=_tmp2DC;struct Cyc_List_List*_tmp2FA;struct Cyc_List_List*_tmp2F9;struct Cyc_List_List*_tmp2F8;struct Cyc_List_List*_tmp2F7;struct _dyneither_ptr*_tmp2F6;struct Cyc_List_List*_tmp2F5;struct Cyc_Absyn_Typedefdecl*_tmp2F4;struct Cyc_Absyn_Enumdecl*_tmp2F3;struct Cyc_Absyn_Datatypedecl*_tmp2F2;struct Cyc_Absyn_Aggrdecl*_tmp2F1;struct Cyc_Absyn_Fndecl*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2EF;switch(*((int*)_tmp2DD)){case 0: _LL2B3: _tmp2EF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2B4:
# 1500
 if(_tmp2EF->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2EF->initializer;
_tmp2EF->initializer=0;
if(_tmp2EF->sc == Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2DE="extern ";_tag_dyneither(_tmp2DE,sizeof(char),8);}));
Cyc_Absyndump_dumpvardecl(_tmp2EF,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2DF="\n";_tag_dyneither(_tmp2DF,sizeof(char),2);}));
_tmp2EF->initializer=init;
goto _LL2B2;};case 1: _LL2B5: _tmp2F0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2B6:
# 1510
 if(_tmp2F0->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2F0->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp2E2;_tmp2E2.tag=9;_tmp2E2.f1=({struct Cyc_Absyn_FnInfo _tmp2E3;_tmp2E3.tvars=_tmp2F0->tvs;_tmp2E3.effect=_tmp2F0->effect;_tmp2E3.ret_tqual=_tmp2F0->ret_tqual;_tmp2E3.ret_typ=_tmp2F0->ret_type;_tmp2E3.args=
# 1514
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2F0->args);_tmp2E3.c_varargs=_tmp2F0->c_varargs;_tmp2E3.cyc_varargs=_tmp2F0->cyc_varargs;_tmp2E3.rgn_po=_tmp2F0->rgn_po;_tmp2E3.attributes=0;_tmp2E3.requires_clause=_tmp2F0->requires_clause;_tmp2E3.requires_relns=_tmp2F0->requires_relns;_tmp2E3.ensures_clause=_tmp2F0->ensures_clause;_tmp2E3.ensures_relns=_tmp2F0->ensures_relns;_tmp2E3;});_tmp2E2;});_tmp2E1;});
# 1519
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp2F0);
Cyc_Absyndump_dump(({const char*_tmp2E0=";\n";_tag_dyneither(_tmp2E0,sizeof(char),3);}));
goto _LL2B2;};case 5: _LL2B7: _tmp2F1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2B8:
# 1523
 if(_tmp2F1->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2F1->impl;
if(_tmp2F1->sc == Cyc_Absyn_Abstract)_tmp2F1->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2F1);
_tmp2F1->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2E4=";\n";_tag_dyneither(_tmp2E4,sizeof(char),3);}));
goto _LL2B2;};case 6: _LL2B9: _tmp2F2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2BA:
# 1531
 Cyc_Absyndump_dump_datatypedecl(_tmp2F2);
Cyc_Absyndump_dump(({const char*_tmp2E5=";\n";_tag_dyneither(_tmp2E5,sizeof(char),3);}));
goto _LL2B2;case 7: _LL2BB: _tmp2F3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2BC:
# 1535
 Cyc_Absyndump_dump_enumdecl(_tmp2F3);
Cyc_Absyndump_dump(({const char*_tmp2E6=";\n";_tag_dyneither(_tmp2E6,sizeof(char),3);}));
goto _LL2B2;case 8: _LL2BD: _tmp2F4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2BE:
# 1539
 if(_tmp2F4->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2E7="\n";_tag_dyneither(_tmp2E7,sizeof(char),2);}));}
# 1543
goto _LL2B2;case 9: _LL2BF: _tmp2F6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_tmp2F5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2DD)->f2;_LL2C0:
# 1545
 Cyc_Absyndump_dump(({const char*_tmp2E8="namespace ";_tag_dyneither(_tmp2E8,sizeof(char),11);}));
Cyc_Absyndump_dump_str(_tmp2F6);
Cyc_Absyndump_dump(({const char*_tmp2E9="{\n";_tag_dyneither(_tmp2E9,sizeof(char),3);}));
for(0;_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F5->hd);}
Cyc_Absyndump_dump(({const char*_tmp2EA="}\n";_tag_dyneither(_tmp2EA,sizeof(char),3);}));
goto _LL2B2;case 10: _LL2C1: _tmp2F7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2DD)->f2;_LL2C2:
# 1553
 for(0;_tmp2F7 != 0;_tmp2F7=_tmp2F7->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F7->hd);}
goto _LL2B2;case 11: _LL2C3: _tmp2F8=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_LL2C4:
# 1557
 Cyc_Absyndump_dump(({const char*_tmp2EB="extern \"C\" {";_tag_dyneither(_tmp2EB,sizeof(char),13);}));
for(0;_tmp2F8 != 0;_tmp2F8=_tmp2F8->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F8->hd);}
Cyc_Absyndump_dump(({const char*_tmp2EC="}\n";_tag_dyneither(_tmp2EC,sizeof(char),3);}));
goto _LL2B2;case 12: _LL2C5: _tmp2FA=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2DD)->f1;_tmp2F9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2DD)->f2;_LL2C6:
# 1578 "absyndump.cyc"
 goto _LL2B2;default: _LL2C7: _LL2C8:
({void*_tmp2ED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2EE="bad top-level declaration";_tag_dyneither(_tmp2EE,sizeof(char),26);}),_tag_dyneither(_tmp2ED,sizeof(void*),0));});}_LL2B2:;}
# 1583
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}

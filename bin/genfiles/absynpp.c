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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Buffer_t;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 944
void*Cyc_Absyn_compress_kb(void*);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 973
extern void*Cyc_Absyn_bounds_one;
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1161
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1167
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1175
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 55 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 301 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 303
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 53
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 76
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 86
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 38 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*requires,struct Cyc_Absyn_Exp*ensures);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 66
static int Cyc_Absynpp_expand_typedefs;
# 70
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 72
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 75
static int Cyc_Absynpp_add_cyc_prefix;
# 79
static int Cyc_Absynpp_to_VC;
# 82
static int Cyc_Absynpp_decls_first;
# 86
static int Cyc_Absynpp_rewrite_temp_tvars;
# 89
static int Cyc_Absynpp_print_all_tvars;
# 92
static int Cyc_Absynpp_print_all_kinds;
# 95
static int Cyc_Absynpp_print_all_effects;
# 98
static int Cyc_Absynpp_print_using_stmts;
# 103
static int Cyc_Absynpp_print_externC_stmts;
# 107
static int Cyc_Absynpp_print_full_evars;
# 110
static int Cyc_Absynpp_generate_line_directives;
# 113
static int Cyc_Absynpp_use_curr_namespace;
# 116
static int Cyc_Absynpp_print_zeroterm;
# 119
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 122
int Cyc_Absynpp_print_for_cycdoc=0;struct Cyc_Absynpp_Params;
# 143
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 162
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 182
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 202
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 222
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 243
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
({struct Cyc_List_List*_tmp531=({struct Cyc_List_List*_tmp530=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp530,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));((*_tmp1).hd=v,(*_tmp1).tl=0);_tmp1;}));});Cyc_Absynpp_curr_namespace=_tmp531;});}
# 247
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 251
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 255
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 259
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){
char _tmp2=c;switch(_tmp2){case 7U: _LL1: _LL2:
 return({const char*_tmp3="\\a";_tag_dyneither(_tmp3,sizeof(char),3U);});case 8U: _LL3: _LL4:
 return({const char*_tmp4="\\b";_tag_dyneither(_tmp4,sizeof(char),3U);});case 12U: _LL5: _LL6:
 return({const char*_tmp5="\\f";_tag_dyneither(_tmp5,sizeof(char),3U);});case 10U: _LL7: _LL8:
 return({const char*_tmp6="\\n";_tag_dyneither(_tmp6,sizeof(char),3U);});case 13U: _LL9: _LLA:
 return({const char*_tmp7="\\r";_tag_dyneither(_tmp7,sizeof(char),3U);});case 9U: _LLB: _LLC:
 return({const char*_tmp8="\\t";_tag_dyneither(_tmp8,sizeof(char),3U);});case 11U: _LLD: _LLE:
 return({const char*_tmp9="\\v";_tag_dyneither(_tmp9,sizeof(char),3U);});case 92U: _LLF: _LL10:
 return({const char*_tmpA="\\\\";_tag_dyneither(_tmpA,sizeof(char),3U);});case 34U: _LL11: _LL12:
 return({const char*_tmpB="\"";_tag_dyneither(_tmpB,sizeof(char),2U);});case 39U: _LL13: _LL14:
 return({const char*_tmpC="\\'";_tag_dyneither(_tmpC,sizeof(char),3U);});default: _LL15: _LL16:
# 272
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2U);
({struct _dyneither_ptr _tmpE=_dyneither_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_dyneither_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_dyneither_size(_tmpE,sizeof(char))== 1U  && (_tmpF == '\000'  && _tmp10 != '\000'))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _dyneither_ptr)_tmpD;}else{
# 277
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5U);
int j=0;
({struct _dyneither_ptr _tmp12=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_dyneither_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_dyneither_size(_tmp12,sizeof(char))== 1U  && (_tmp13 == '\000'  && _tmp14 != '\000'))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)('0' + ((unsigned char)c >> 6 & 3));if(_get_dyneither_size(_tmp15,sizeof(char))== 1U  && (_tmp16 == '\000'  && _tmp17 != '\000'))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _dyneither_ptr _tmp18=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_dyneither_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)('0' + (c >> 3 & 7));if(_get_dyneither_size(_tmp18,sizeof(char))== 1U  && (_tmp19 == '\000'  && _tmp1A != '\000'))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)('0' + (c & 7));if(_get_dyneither_size(_tmp1B,sizeof(char))== 1U  && (_tmp1C == '\000'  && _tmp1D != '\000'))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _dyneither_ptr)_tmp11;}}_LL0:;}
# 288
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')
return 1;}}
# 295
return 0;}
# 298
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 301
int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);
# 303
if(n > 0  && *((const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{
# 305
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1F=_tmp1E;char _tmp20;switch(_tmp1F){case 7U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 13U: _LL9: _LLA:
 goto _LLC;case 9U: _LLB: _LLC:
 goto _LLE;case 11U: _LLD: _LLE:
 goto _LL10;case 92U: _LLF: _LL10:
 goto _LL12;case 34U: _LL11: _LL12:
 len +=2;goto _LL0;default: _LL13: _tmp20=_tmp1F;_LL14:
# 318
 if(_tmp20 >= ' '  && _tmp20 <= '~')++ len;else{
len +=4;}
goto _LL0;}_LL0:;}}{
# 323
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp68;switch(_tmp22){case 7U: _LL16: _LL17:
({struct _dyneither_ptr _tmp23=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp24=*((char*)_check_dyneither_subscript(_tmp23,sizeof(char),0U));char _tmp25='\\';if(_get_dyneither_size(_tmp23,sizeof(char))== 1U  && (_tmp24 == '\000'  && _tmp25 != '\000'))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});({struct _dyneither_ptr _tmp26=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp27=*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0U));char _tmp28='a';if(_get_dyneither_size(_tmp26,sizeof(char))== 1U  && (_tmp27 == '\000'  && _tmp28 != '\000'))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});goto _LL15;case 8U: _LL18: _LL19:
({struct _dyneither_ptr _tmp29=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2A=*((char*)_check_dyneither_subscript(_tmp29,sizeof(char),0U));char _tmp2B='\\';if(_get_dyneither_size(_tmp29,sizeof(char))== 1U  && (_tmp2A == '\000'  && _tmp2B != '\000'))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});({struct _dyneither_ptr _tmp2C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0U));char _tmp2E='b';if(_get_dyneither_size(_tmp2C,sizeof(char))== 1U  && (_tmp2D == '\000'  && _tmp2E != '\000'))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});goto _LL15;case 12U: _LL1A: _LL1B:
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0U));char _tmp31='\\';if(_get_dyneither_size(_tmp2F,sizeof(char))== 1U  && (_tmp30 == '\000'  && _tmp31 != '\000'))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34='f';if(_get_dyneither_size(_tmp32,sizeof(char))== 1U  && (_tmp33 == '\000'  && _tmp34 != '\000'))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});goto _LL15;case 10U: _LL1C: _LL1D:
({struct _dyneither_ptr _tmp35=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0U));char _tmp37='\\';if(_get_dyneither_size(_tmp35,sizeof(char))== 1U  && (_tmp36 == '\000'  && _tmp37 != '\000'))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});({struct _dyneither_ptr _tmp38=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp39=*((char*)_check_dyneither_subscript(_tmp38,sizeof(char),0U));char _tmp3A='n';if(_get_dyneither_size(_tmp38,sizeof(char))== 1U  && (_tmp39 == '\000'  && _tmp3A != '\000'))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});goto _LL15;case 13U: _LL1E: _LL1F:
({struct _dyneither_ptr _tmp3B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3C=*((char*)_check_dyneither_subscript(_tmp3B,sizeof(char),0U));char _tmp3D='\\';if(_get_dyneither_size(_tmp3B,sizeof(char))== 1U  && (_tmp3C == '\000'  && _tmp3D != '\000'))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3F=*((char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0U));char _tmp40='r';if(_get_dyneither_size(_tmp3E,sizeof(char))== 1U  && (_tmp3F == '\000'  && _tmp40 != '\000'))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});goto _LL15;case 9U: _LL20: _LL21:
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0U));char _tmp43='\\';if(_get_dyneither_size(_tmp41,sizeof(char))== 1U  && (_tmp42 == '\000'  && _tmp43 != '\000'))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});({struct _dyneither_ptr _tmp44=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp45=*((char*)_check_dyneither_subscript(_tmp44,sizeof(char),0U));char _tmp46='t';if(_get_dyneither_size(_tmp44,sizeof(char))== 1U  && (_tmp45 == '\000'  && _tmp46 != '\000'))_throw_arraybounds();*((char*)_tmp44.curr)=_tmp46;});goto _LL15;case 11U: _LL22: _LL23:
({struct _dyneither_ptr _tmp47=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp48=*((char*)_check_dyneither_subscript(_tmp47,sizeof(char),0U));char _tmp49='\\';if(_get_dyneither_size(_tmp47,sizeof(char))== 1U  && (_tmp48 == '\000'  && _tmp49 != '\000'))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});({struct _dyneither_ptr _tmp4A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4B=*((char*)_check_dyneither_subscript(_tmp4A,sizeof(char),0U));char _tmp4C='v';if(_get_dyneither_size(_tmp4A,sizeof(char))== 1U  && (_tmp4B == '\000'  && _tmp4C != '\000'))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});goto _LL15;case 92U: _LL24: _LL25:
({struct _dyneither_ptr _tmp4D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4E=*((char*)_check_dyneither_subscript(_tmp4D,sizeof(char),0U));char _tmp4F='\\';if(_get_dyneither_size(_tmp4D,sizeof(char))== 1U  && (_tmp4E == '\000'  && _tmp4F != '\000'))_throw_arraybounds();*((char*)_tmp4D.curr)=_tmp4F;});({struct _dyneither_ptr _tmp50=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp51=*((char*)_check_dyneither_subscript(_tmp50,sizeof(char),0U));char _tmp52='\\';if(_get_dyneither_size(_tmp50,sizeof(char))== 1U  && (_tmp51 == '\000'  && _tmp52 != '\000'))_throw_arraybounds();*((char*)_tmp50.curr)=_tmp52;});goto _LL15;case 34U: _LL26: _LL27:
({struct _dyneither_ptr _tmp53=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp54=*((char*)_check_dyneither_subscript(_tmp53,sizeof(char),0U));char _tmp55='\\';if(_get_dyneither_size(_tmp53,sizeof(char))== 1U  && (_tmp54 == '\000'  && _tmp55 != '\000'))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});({struct _dyneither_ptr _tmp56=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp57=*((char*)_check_dyneither_subscript(_tmp56,sizeof(char),0U));char _tmp58='"';if(_get_dyneither_size(_tmp56,sizeof(char))== 1U  && (_tmp57 == '\000'  && _tmp58 != '\000'))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});goto _LL15;default: _LL28: _tmp68=_tmp22;_LL29:
# 337
 if(_tmp68 >= ' '  && _tmp68 <= '~')({struct _dyneither_ptr _tmp59=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5A=*((char*)_check_dyneither_subscript(_tmp59,sizeof(char),0U));char _tmp5B=_tmp68;if(_get_dyneither_size(_tmp59,sizeof(char))== 1U  && (_tmp5A == '\000'  && _tmp5B != '\000'))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});else{
# 339
({struct _dyneither_ptr _tmp5C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5D=*((char*)_check_dyneither_subscript(_tmp5C,sizeof(char),0U));char _tmp5E='\\';if(_get_dyneither_size(_tmp5C,sizeof(char))== 1U  && (_tmp5D == '\000'  && _tmp5E != '\000'))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});
({struct _dyneither_ptr _tmp5F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp60=*((char*)_check_dyneither_subscript(_tmp5F,sizeof(char),0U));char _tmp61=(char)('0' + (_tmp68 >> 6 & 7));if(_get_dyneither_size(_tmp5F,sizeof(char))== 1U  && (_tmp60 == '\000'  && _tmp61 != '\000'))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp63=*((char*)_check_dyneither_subscript(_tmp62,sizeof(char),0U));char _tmp64=(char)('0' + (_tmp68 >> 3 & 7));if(_get_dyneither_size(_tmp62,sizeof(char))== 1U  && (_tmp63 == '\000'  && _tmp64 != '\000'))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
({struct _dyneither_ptr _tmp65=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp66=*((char*)_check_dyneither_subscript(_tmp65,sizeof(char),0U));char _tmp67=(char)('0' + (_tmp68 & 7));if(_get_dyneither_size(_tmp65,sizeof(char))== 1U  && (_tmp66 == '\000'  && _tmp67 != '\000'))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});}
# 344
goto _LL15;}_LL15:;}}
# 346
return(struct _dyneither_ptr)t;};};};}static char _tmp69[9U]="restrict";
# 349
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9U};static char _tmp6A[9U]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[6U]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6U};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 356
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 359
if(tq.q_restrict)({struct Cyc_List_List*_tmp532=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));((*_tmp6C).hd=Cyc_Absynpp_restrict_sp,(*_tmp6C).tl=l);_tmp6C;});l=_tmp532;});
if(tq.q_volatile)({struct Cyc_List_List*_tmp533=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));((*_tmp6D).hd=Cyc_Absynpp_volatile_sp,(*_tmp6D).tl=l);_tmp6D;});l=_tmp533;});
if(tq.print_const)({struct Cyc_List_List*_tmp534=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));((*_tmp6E).hd=Cyc_Absynpp_const_sp,(*_tmp6E).tl=l);_tmp6E;});l=_tmp534;});
return({struct _dyneither_ptr _tmp537=({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1U);});struct _dyneither_ptr _tmp536=({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2U);});struct _dyneither_ptr _tmp535=({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp537,_tmp536,_tmp535,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 365
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp87;enum Cyc_Absyn_AliasQual _tmp86;_LL1: _tmp87=_tmp72->kind;_tmp86=_tmp72->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp73=_tmp87;switch(_tmp73){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 369
enum Cyc_Absyn_AliasQual _tmp74=_tmp86;switch(_tmp74){case Cyc_Absyn_Aliasable: _LL11: _LL12:
 return({const char*_tmp75="A";_tag_dyneither(_tmp75,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL13: _LL14:
 return({const char*_tmp76="UA";_tag_dyneither(_tmp76,sizeof(char),3U);});default: _LL15: _LL16:
 return({const char*_tmp77="TA";_tag_dyneither(_tmp77,sizeof(char),3U);});}_LL10:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 375
enum Cyc_Absyn_AliasQual _tmp78=_tmp86;switch(_tmp78){case Cyc_Absyn_Aliasable: _LL18: _LL19:
 return({const char*_tmp79="M";_tag_dyneither(_tmp79,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL1A: _LL1B:
 return({const char*_tmp7A="UM";_tag_dyneither(_tmp7A,sizeof(char),3U);});default: _LL1C: _LL1D:
 return({const char*_tmp7B="TM";_tag_dyneither(_tmp7B,sizeof(char),3U);});}_LL17:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 381
enum Cyc_Absyn_AliasQual _tmp7C=_tmp86;switch(_tmp7C){case Cyc_Absyn_Aliasable: _LL1F: _LL20:
 return({const char*_tmp7D="B";_tag_dyneither(_tmp7D,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL21: _LL22:
 return({const char*_tmp7E="UB";_tag_dyneither(_tmp7E,sizeof(char),3U);});default: _LL23: _LL24:
 return({const char*_tmp7F="TB";_tag_dyneither(_tmp7F,sizeof(char),3U);});}_LL1E:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 387
enum Cyc_Absyn_AliasQual _tmp80=_tmp86;switch(_tmp80){case Cyc_Absyn_Aliasable: _LL26: _LL27:
 return({const char*_tmp81="R";_tag_dyneither(_tmp81,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL28: _LL29:
 return({const char*_tmp82="UR";_tag_dyneither(_tmp82,sizeof(char),3U);});default: _LL2A: _LL2B:
 return({const char*_tmp83="TR";_tag_dyneither(_tmp83,sizeof(char),3U);});}_LL25:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 392
 return({const char*_tmp84="E";_tag_dyneither(_tmp84,sizeof(char),2U);});default: _LLE: _LLF:
 return({const char*_tmp85="I";_tag_dyneither(_tmp85,sizeof(char),2U);});}_LL3:;};}
# 396
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 398
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp88=Cyc_Absyn_compress_kb(c);void*_tmp89=_tmp88;struct Cyc_Absyn_Kind*_tmp90;struct Cyc_Absyn_Kind*_tmp8F;switch(*((int*)_tmp89)){case 0U: _LL1: _tmp8F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89)->f1;_LL2:
 return Cyc_Absynpp_kind2string(_tmp8F);case 1U: _LL3: _LL4:
# 402
 if(Cyc_PP_tex_output)
return({const char*_tmp8A="{?}";_tag_dyneither(_tmp8A,sizeof(char),4U);});else{
return({const char*_tmp8B="?";_tag_dyneither(_tmp8B,sizeof(char),2U);});}default: _LL5: _tmp90=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89)->f2;_LL6:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0U;({struct _dyneither_ptr _tmp538=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp90));_tmp8E.f1=_tmp538;});({void*_tmp8C[1U];_tmp8C[0]=& _tmp8E;({struct _dyneither_ptr _tmp539=({const char*_tmp8D="<=%s";_tag_dyneither(_tmp8D,sizeof(char),5U);});Cyc_aprintf(_tmp539,_tag_dyneither(_tmp8C,sizeof(void*),1U));});});});}_LL0:;}
# 409
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp91=Cyc_Absyn_compress_kb(c);void*_tmp92=_tmp91;struct Cyc_Absyn_Kind*_tmp96;struct Cyc_Absyn_Kind*_tmp95;switch(*((int*)_tmp92)){case 0U: _LL1: _tmp95=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp92)->f1;_LL2:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp95));case 1U: _LL3: _LL4:
# 413
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp93="{?}";_tag_dyneither(_tmp93,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp94="?";_tag_dyneither(_tmp94,sizeof(char),2U);}));}default: _LL5: _tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92)->f2;_LL6:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp96));}_LL0:;}
# 420
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp53C=({const char*_tmp97="<";_tag_dyneither(_tmp97,sizeof(char),2U);});struct _dyneither_ptr _tmp53B=({const char*_tmp98=">";_tag_dyneither(_tmp98,sizeof(char),2U);});struct _dyneither_ptr _tmp53A=({const char*_tmp99=",";_tag_dyneither(_tmp99,sizeof(char),2U);});Cyc_PP_egroup(_tmp53C,_tmp53B,_tmp53A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 428
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 430
struct _dyneither_ptr kstring=({const char*_tmpA3="K";_tag_dyneither(_tmpA3,sizeof(char),2U);});
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmp9D;switch(*((int*)_tmp9C)){case 2U: _LL1: _tmp9D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C)->f2;_LL2:
 _tmp9E=_tmp9D;goto _LL4;case 0U: _LL3: _tmp9E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C)->f1;_LL4:
({struct _dyneither_ptr _tmp53D=Cyc_Absynpp_kind2string(_tmp9E);kstring=_tmp53D;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 436
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0U;({struct _dyneither_ptr _tmp53E=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1));_tmpA2.f1=_tmp53E;});({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0U;_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);({void*_tmp9F[2U];(_tmp9F[0]=& _tmpA1,_tmp9F[1]=& _tmpA2);({struct _dyneither_ptr _tmp53F=({const char*_tmpA0="`G%s%s";_tag_dyneither(_tmpA0,sizeof(char),7U);});Cyc_aprintf(_tmp53F,_tag_dyneither(_tmp9F,sizeof(void*),2U));});});});}));}
# 438
return Cyc_PP_text(_tmp9A);}
# 441
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA4=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA5=_tmpA4;struct Cyc_Absyn_Kind*_tmpA9;struct Cyc_Absyn_Kind*_tmpA8;switch(*((int*)_tmpA5)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpA8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1;_LL8:
# 446
 return({struct Cyc_PP_Doc*_tmpA6[3U];(((({struct Cyc_PP_Doc*_tmp542=Cyc_Absynpp_tvar2doc(tv);_tmpA6[0]=_tmp542;}),({struct Cyc_PP_Doc*_tmp541=Cyc_PP_text(({const char*_tmpA7="::";_tag_dyneither(_tmpA7,sizeof(char),3U);}));_tmpA6[1]=_tmp541;}))),({struct Cyc_PP_Doc*_tmp540=Cyc_Absynpp_kind2doc(_tmpA8);_tmpA6[2]=_tmp540;}));Cyc_PP_cat(_tag_dyneither(_tmpA6,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL5: _tmpA9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5)->f2;_LL6:
# 445
 _tmpA8=_tmpA9;goto _LL8;}_LL0:;}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _dyneither_ptr _tmp545=({const char*_tmpAA="<";_tag_dyneither(_tmpAA,sizeof(char),2U);});struct _dyneither_ptr _tmp544=({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2U);});struct _dyneither_ptr _tmp543=({const char*_tmpAC=",";_tag_dyneither(_tmpAC,sizeof(char),2U);});Cyc_PP_egroup(_tmp545,_tmp544,_tmp543,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 454
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _dyneither_ptr _tmp548=({const char*_tmpAD="<";_tag_dyneither(_tmpAD,sizeof(char),2U);});struct _dyneither_ptr _tmp547=({const char*_tmpAE=">";_tag_dyneither(_tmpAE,sizeof(char),2U);});struct _dyneither_ptr _tmp546=({const char*_tmpAF=",";_tag_dyneither(_tmpAF,sizeof(char),2U);});Cyc_PP_egroup(_tmp548,_tmp547,_tmp546,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp54B=({const char*_tmpB0="(";_tag_dyneither(_tmpB0,sizeof(char),2U);});struct _dyneither_ptr _tmp54A=({const char*_tmpB1=")";_tag_dyneither(_tmpB1,sizeof(char),2U);});struct _dyneither_ptr _tmp549=({const char*_tmpB2=",";_tag_dyneither(_tmpB2,sizeof(char),2U);});Cyc_PP_group(_tmp54B,_tmp54A,_tmp549,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 468
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB3=att;switch(*((int*)_tmpB3)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL0:;}
# 477
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB4=(void*)atts->hd;void*_tmpB5=_tmpB4;switch(*((int*)_tmpB5)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB6=" _stdcall ";_tag_dyneither(_tmpB6,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpB7=" _cdecl ";_tag_dyneither(_tmpB7,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpB8=" _fastcall ";_tag_dyneither(_tmpB8,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 485
return Cyc_PP_nil_doc();}
# 488
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB9=(void*)atts2->hd;void*_tmpBA=_tmpB9;switch(*((int*)_tmpBA)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 497
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBB[3U];(((({struct Cyc_PP_Doc*_tmp551=Cyc_PP_text(({const char*_tmpBC=" __declspec(";_tag_dyneither(_tmpBC,sizeof(char),13U);}));_tmpBB[0]=_tmp551;}),({
struct Cyc_PP_Doc*_tmp550=({struct _dyneither_ptr _tmp54F=({const char*_tmpBD="";_tag_dyneither(_tmpBD,sizeof(char),1U);});struct _dyneither_ptr _tmp54E=({const char*_tmpBE="";_tag_dyneither(_tmpBE,sizeof(char),1U);});struct _dyneither_ptr _tmp54D=({const char*_tmpBF=" ";_tag_dyneither(_tmpBF,sizeof(char),2U);});Cyc_PP_group(_tmp54F,_tmp54E,_tmp54D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBB[1]=_tmp550;}))),({
struct Cyc_PP_Doc*_tmp54C=Cyc_PP_text(({const char*_tmpC0=")";_tag_dyneither(_tmpC0,sizeof(char),2U);}));_tmpBB[2]=_tmp54C;}));Cyc_PP_cat(_tag_dyneither(_tmpBB,sizeof(struct Cyc_PP_Doc*),3U));});}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 508
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC1=Cyc_Cyclone_c_compiler;if(_tmpC1 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
 return({struct Cyc_PP_Doc*_tmpC2[2U];(({struct Cyc_PP_Doc*_tmp556=Cyc_PP_text(({const char*_tmpC3=" __attribute__";_tag_dyneither(_tmpC3,sizeof(char),15U);}));_tmpC2[0]=_tmp556;}),({
struct Cyc_PP_Doc*_tmp555=({struct _dyneither_ptr _tmp554=({const char*_tmpC4="((";_tag_dyneither(_tmpC4,sizeof(char),3U);});struct _dyneither_ptr _tmp553=({const char*_tmpC5="))";_tag_dyneither(_tmpC5,sizeof(char),3U);});struct _dyneither_ptr _tmp552=({const char*_tmpC6=",";_tag_dyneither(_tmpC6,sizeof(char),2U);});Cyc_PP_group(_tmp554,_tmp553,_tmp552,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC2[1]=_tmp555;}));Cyc_PP_cat(_tag_dyneither(_tmpC2,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;};}
# 517
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC7=(void*)tms->hd;void*_tmpC8=_tmpC7;switch(*((int*)_tmpC8)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 522
enum Cyc_Cyclone_C_Compilers _tmpC9=Cyc_Cyclone_c_compiler;if(_tmpC9 == Cyc_Cyclone_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 526
 return 0;}_LL0:;};}
# 530
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 533
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp557=Cyc_PP_text_width(({const char*_tmpCA="{?}";_tag_dyneither(_tmpCA,sizeof(char),4U);}),1);Cyc_Absynpp_cache_question=_tmp557;});else{
({struct Cyc_PP_Doc*_tmp558=Cyc_PP_text(({const char*_tmpCB="?";_tag_dyneither(_tmpCB,sizeof(char),2U);}));Cyc_Absynpp_cache_question=_tmp558;});}}
# 540
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 542
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp559=Cyc_PP_text_width(({const char*_tmpCC="{\\lb}";_tag_dyneither(_tmpCC,sizeof(char),6U);}),1);Cyc_Absynpp_cache_lb=_tmp559;});else{
({struct Cyc_PP_Doc*_tmp55A=Cyc_PP_text(({const char*_tmpCD="{";_tag_dyneither(_tmpCD,sizeof(char),2U);}));Cyc_Absynpp_cache_lb=_tmp55A;});}}
# 549
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp55B=Cyc_PP_text_width(({const char*_tmpCE="{\\rb}";_tag_dyneither(_tmpCE,sizeof(char),6U);}),1);Cyc_Absynpp_cache_rb=_tmp55B;});else{
({struct Cyc_PP_Doc*_tmp55C=Cyc_PP_text(({const char*_tmpCF="}";_tag_dyneither(_tmpCF,sizeof(char),2U);}));Cyc_Absynpp_cache_rb=_tmp55C;});}}
# 558
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp55D=Cyc_PP_text_width(({const char*_tmpD0="\\$";_tag_dyneither(_tmpD0,sizeof(char),3U);}),1);Cyc_Absynpp_cache_dollar=_tmp55D;});else{
({struct Cyc_PP_Doc*_tmp55E=Cyc_PP_text(({const char*_tmpD1="$";_tag_dyneither(_tmpD1,sizeof(char),2U);}));Cyc_Absynpp_cache_dollar=_tmp55E;});}}
# 567
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 569
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD2[3U];(((({struct Cyc_PP_Doc*_tmp561=Cyc_Absynpp_lb();_tmpD2[0]=_tmp561;}),({struct Cyc_PP_Doc*_tmp560=Cyc_PP_seq(sep,ss);_tmpD2[1]=_tmp560;}))),({struct Cyc_PP_Doc*_tmp55F=Cyc_Absynpp_rb();_tmpD2[2]=_tmp55F;}));Cyc_PP_cat(_tag_dyneither(_tmpD2,sizeof(struct Cyc_PP_Doc*),3U));});}
# 574
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD3=(void*)tms->hd;void*_tmpD4=_tmpD3;struct Cyc_List_List*_tmpEB;switch(*((int*)_tmpD4)){case 0U: _LL1: _LL2:
({void*_tmpD5=0U;({struct Cyc___cycFILE*_tmp563=Cyc_stderr;struct _dyneither_ptr _tmp562=({const char*_tmpD6="Carray_mod ";_tag_dyneither(_tmpD6,sizeof(char),12U);});Cyc_fprintf(_tmp563,_tmp562,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp565=Cyc_stderr;struct _dyneither_ptr _tmp564=({const char*_tmpD8="ConstArray_mod ";_tag_dyneither(_tmpD8,sizeof(char),16U);});Cyc_fprintf(_tmp565,_tmp564,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->tag == 1U){_LL5: _tmpEB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->f1;_LL6:
# 580
({void*_tmpD9=0U;({struct Cyc___cycFILE*_tmp567=Cyc_stderr;struct _dyneither_ptr _tmp566=({const char*_tmpDA="Function_mod(";_tag_dyneither(_tmpDA,sizeof(char),14U);});Cyc_fprintf(_tmp567,_tmp566,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});
for(0;_tmpEB != 0;_tmpEB=_tmpEB->tl){
struct _dyneither_ptr*_tmpDB=(*((struct _tuple8*)_tmpEB->hd)).f1;
if(_tmpDB == 0)({void*_tmpDC=0U;({struct Cyc___cycFILE*_tmp569=Cyc_stderr;struct _dyneither_ptr _tmp568=({const char*_tmpDD="?";_tag_dyneither(_tmpDD,sizeof(char),2U);});Cyc_fprintf(_tmp569,_tmp568,_tag_dyneither(_tmpDC,sizeof(void*),0U));});});else{
({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp56B=Cyc_stderr;struct _dyneither_ptr _tmp56A=*_tmpDB;Cyc_fprintf(_tmp56B,_tmp56A,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});}
if(_tmpEB->tl != 0)({void*_tmpDF=0U;({struct Cyc___cycFILE*_tmp56D=Cyc_stderr;struct _dyneither_ptr _tmp56C=({const char*_tmpE0=",";_tag_dyneither(_tmpE0,sizeof(char),2U);});Cyc_fprintf(_tmp56D,_tmp56C,_tag_dyneither(_tmpDF,sizeof(void*),0U));});});}
# 587
({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp56F=Cyc_stderr;struct _dyneither_ptr _tmp56E=({const char*_tmpE2=") ";_tag_dyneither(_tmpE2,sizeof(char),3U);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});
goto _LL0;}else{_LL7: _LL8:
# 590
({void*_tmpE3=0U;({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _dyneither_ptr _tmp570=({const char*_tmpE4="Function_mod()";_tag_dyneither(_tmpE4,sizeof(char),15U);});Cyc_fprintf(_tmp571,_tmp570,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE5=0U;({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _dyneither_ptr _tmp572=({const char*_tmpE6="Attributes_mod ";_tag_dyneither(_tmpE6,sizeof(char),16U);});Cyc_fprintf(_tmp573,_tmp572,_tag_dyneither(_tmpE5,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpE7=0U;({struct Cyc___cycFILE*_tmp575=Cyc_stderr;struct _dyneither_ptr _tmp574=({const char*_tmpE8="TypeParams_mod ";_tag_dyneither(_tmpE8,sizeof(char),16U);});Cyc_fprintf(_tmp575,_tmp574,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpE9=0U;({struct Cyc___cycFILE*_tmp577=Cyc_stderr;struct _dyneither_ptr _tmp576=({const char*_tmpEA="Pointer_mod ";_tag_dyneither(_tmpEA,sizeof(char),13U);});Cyc_fprintf(_tmp577,_tmp576,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 595
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp579=Cyc_stderr;struct _dyneither_ptr _tmp578=({const char*_tmpED="\n";_tag_dyneither(_tmpED,sizeof(char),2U);});Cyc_fprintf(_tmp579,_tmp578,_tag_dyneither(_tmpEC,sizeof(void*),0U));});});}
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 600
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp129[3U];(((({struct Cyc_PP_Doc*_tmp57B=Cyc_PP_text(({const char*_tmp12A="(";_tag_dyneither(_tmp12A,sizeof(char),2U);}));_tmp129[0]=_tmp57B;}),_tmp129[1]=rest)),({struct Cyc_PP_Doc*_tmp57A=Cyc_PP_text(({const char*_tmp12B=")";_tag_dyneither(_tmp12B,sizeof(char),2U);}));_tmp129[2]=_tmp57A;}));Cyc_PP_cat(_tag_dyneither(_tmp129,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpEE=(void*)tms->hd;void*_tmpEF=_tmpEE;void*_tmp128;union Cyc_Absyn_Constraint*_tmp127;union Cyc_Absyn_Constraint*_tmp126;union Cyc_Absyn_Constraint*_tmp125;struct Cyc_Absyn_Tqual _tmp124;struct Cyc_List_List*_tmp123;unsigned int _tmp122;int _tmp121;struct Cyc_List_List*_tmp120;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11C;switch(*((int*)_tmpEF)){case 0U: _LL1: _tmp11C=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LL2:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF0[2U];(_tmpF0[0]=rest,((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11C)?({
struct Cyc_PP_Doc*_tmp57D=Cyc_PP_text(({const char*_tmpF1="[]@zeroterm";_tag_dyneither(_tmpF1,sizeof(char),12U);}));_tmpF0[1]=_tmp57D;}):({struct Cyc_PP_Doc*_tmp57C=Cyc_PP_text(({const char*_tmpF2="[]";_tag_dyneither(_tmpF2,sizeof(char),3U);}));_tmpF0[1]=_tmp57C;}));Cyc_PP_cat(_tag_dyneither(_tmpF0,sizeof(struct Cyc_PP_Doc*),2U));});case 1U: _LL3: _tmp11E=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp11D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LL4:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF3[4U];(((((_tmpF3[0]=rest,({struct Cyc_PP_Doc*_tmp581=Cyc_PP_text(({const char*_tmpF4="[";_tag_dyneither(_tmpF4,sizeof(char),2U);}));_tmpF3[1]=_tmp581;}))),({struct Cyc_PP_Doc*_tmp580=Cyc_Absynpp_exp2doc(_tmp11E);_tmpF3[2]=_tmp580;}))),
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11D)?({struct Cyc_PP_Doc*_tmp57F=Cyc_PP_text(({const char*_tmpF5="]@zeroterm";_tag_dyneither(_tmpF5,sizeof(char),11U);}));_tmpF3[3]=_tmp57F;}):({struct Cyc_PP_Doc*_tmp57E=Cyc_PP_text(({const char*_tmpF6="]";_tag_dyneither(_tmpF6,sizeof(char),2U);}));_tmpF3[3]=_tmp57E;}));Cyc_PP_cat(_tag_dyneither(_tmpF3,sizeof(struct Cyc_PP_Doc*),4U));});case 3U: _LL5: _tmp11F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LL6:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF7=_tmp11F;struct Cyc_List_List*_tmp105;unsigned int _tmp104;struct Cyc_List_List*_tmp103;int _tmp102;struct Cyc_Absyn_VarargInfo*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->tag == 1U){_LLE: _tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp102=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f3;_tmp100=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f4;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f5;_tmpFE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f6;_tmpFD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f7;_LLF:
# 617
 return({struct Cyc_PP_Doc*_tmpF8[2U];(_tmpF8[0]=rest,({struct Cyc_PP_Doc*_tmp582=Cyc_Absynpp_funargs2doc(_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF,_tmpFE,_tmpFD);_tmpF8[1]=_tmp582;}));Cyc_PP_cat(_tag_dyneither(_tmpF8,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL10: _tmp105=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp104=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_LL11:
# 619
 return({struct Cyc_PP_Doc*_tmpF9[2U];(_tmpF9[0]=rest,({struct Cyc_PP_Doc*_tmp586=({struct _dyneither_ptr _tmp585=({const char*_tmpFA="(";_tag_dyneither(_tmpFA,sizeof(char),2U);});struct _dyneither_ptr _tmp584=({const char*_tmpFB=")";_tag_dyneither(_tmpFB,sizeof(char),2U);});struct _dyneither_ptr _tmp583=({const char*_tmpFC=",";_tag_dyneither(_tmpFC,sizeof(char),2U);});Cyc_PP_group(_tmp585,_tmp584,_tmp583,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp105));});_tmpF9[1]=_tmp586;}));Cyc_PP_cat(_tag_dyneither(_tmpF9,sizeof(struct Cyc_PP_Doc*),2U));});}_LLD:;};case 5U: _LL7: _tmp120=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LL8: {
# 622
enum Cyc_Cyclone_C_Compilers _tmp106=Cyc_Cyclone_c_compiler;if(_tmp106 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 624
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp107[2U];(_tmp107[0]=rest,({struct Cyc_PP_Doc*_tmp587=Cyc_Absynpp_atts2doc(_tmp120);_tmp107[1]=_tmp587;}));Cyc_PP_cat(_tag_dyneither(_tmp107,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 627
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp108[2U];(({struct Cyc_PP_Doc*_tmp588=Cyc_Absynpp_callconv2doc(_tmp120);_tmp108[0]=_tmp588;}),_tmp108[1]=rest);Cyc_PP_cat(_tag_dyneither(_tmp108,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp123=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp122=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_tmp121=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f3;_LLA:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp121)
return({struct Cyc_PP_Doc*_tmp109[2U];(_tmp109[0]=rest,({struct Cyc_PP_Doc*_tmp589=Cyc_Absynpp_ktvars2doc(_tmp123);_tmp109[1]=_tmp589;}));Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 636
return({struct Cyc_PP_Doc*_tmp10A[2U];(_tmp10A[0]=rest,({struct Cyc_PP_Doc*_tmp58A=Cyc_Absynpp_tvars2doc(_tmp123);_tmp10A[1]=_tmp58A;}));Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});}default: _LLB: _tmp128=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).rgn;_tmp127=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).nullable;_tmp126=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).bounds;_tmp125=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).zero_term;_tmp124=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLC: {
# 640
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp124);
{void*_tmp10B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp126);void*_tmp10C=_tmp10B;struct Cyc_Absyn_Exp*_tmp114;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp10C)->tag == 0U){_LL18: _LL19:
({struct Cyc_PP_Doc*_tmp58B=Cyc_Absynpp_question();ptr=_tmp58B;});goto _LL17;}else{_LL1A: _tmp114=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp10C)->f1;_LL1B:
# 648
({struct Cyc_PP_Doc*_tmp58C=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp127)?({const char*_tmp10D="*";_tag_dyneither(_tmp10D,sizeof(char),2U);}):({const char*_tmp10E="@";_tag_dyneither(_tmp10E,sizeof(char),2U);}));ptr=_tmp58C;});{
struct _tuple11 _tmp10F=Cyc_Evexp_eval_const_uint_exp(_tmp114);struct _tuple11 _tmp110=_tmp10F;unsigned int _tmp113;int _tmp112;_LL1D: _tmp113=_tmp110.f1;_tmp112=_tmp110.f2;_LL1E:;
if(!_tmp112  || _tmp113 != 1)
({struct Cyc_PP_Doc*_tmp590=({struct Cyc_PP_Doc*_tmp111[4U];(((((_tmp111[0]=ptr,({struct Cyc_PP_Doc*_tmp58F=Cyc_Absynpp_lb();_tmp111[1]=_tmp58F;}))),({struct Cyc_PP_Doc*_tmp58E=Cyc_Absynpp_exp2doc(_tmp114);_tmp111[2]=_tmp58E;}))),({struct Cyc_PP_Doc*_tmp58D=Cyc_Absynpp_rb();_tmp111[3]=_tmp58D;}));Cyc_PP_cat(_tag_dyneither(_tmp111,sizeof(struct Cyc_PP_Doc*),4U));});ptr=_tmp590;});
goto _LL17;};}_LL17:;}
# 654
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
({struct Cyc_PP_Doc*_tmp591=Cyc_PP_text(({const char*_tmp115="@zeroterm";_tag_dyneither(_tmp115,sizeof(char),10U);}));ztd=_tmp591;});else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
({struct Cyc_PP_Doc*_tmp592=Cyc_PP_text(({const char*_tmp116="@nozeroterm";_tag_dyneither(_tmp116,sizeof(char),12U);}));ztd=_tmp592;});}}
# 660
{void*_tmp117=Cyc_Tcutil_compress(_tmp128);void*_tmp118=_tmp117;switch(*((int*)_tmp118)){case 20U: _LL20: _LL21:
 goto _LL1F;case 1U: _LL22: if(Cyc_Absynpp_print_for_cycdoc){_LL23:
 goto _LL1F;}else{goto _LL24;}default: _LL24: _LL25:
({struct Cyc_PP_Doc*_tmp593=Cyc_Absynpp_rgn2doc(_tmp128);rgd=_tmp593;});}_LL1F:;}{
# 665
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp11B=" ";_tag_dyneither(_tmp11B,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp11A=" ";_tag_dyneither(_tmp11A,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp119[7U];(((((((((((_tmp119[0]=ptr,_tmp119[1]=ztd)),_tmp119[2]=rgd)),_tmp119[3]=spacer1)),_tmp119[4]=tqd)),_tmp119[5]=spacer2)),_tmp119[6]=rest);Cyc_PP_cat(_tag_dyneither(_tmp119,sizeof(struct Cyc_PP_Doc*),7U));});};}}_LL0:;};}
# 671
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;switch(*((int*)_tmp12D)){case 20U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp12E="`H";_tag_dyneither(_tmp12E,sizeof(char),3U);}));case 21U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp12F="`U";_tag_dyneither(_tmp12F,sizeof(char),3U);}));case 22U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp130="`RC";_tag_dyneither(_tmp130,sizeof(char),4U);}));default: _LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 681
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 685
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;struct Cyc_List_List*_tmp136;void*_tmp135;switch(*((int*)_tmp132)){case 23U: _LL1: _tmp135=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp132)->f1;_LL2:
({struct Cyc_List_List*_tmp595=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));(({struct Cyc_PP_Doc*_tmp594=Cyc_Absynpp_rgn2doc(_tmp135);(*_tmp133).hd=_tmp594;}),(*_tmp133).tl=*rgions);_tmp133;});*rgions=_tmp595;});goto _LL0;case 24U: _LL3: _tmp136=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132)->f1;_LL4:
# 688
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp136->hd);}
# 691
goto _LL0;default: _LL5: _LL6:
({struct Cyc_List_List*_tmp597=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));(({struct Cyc_PP_Doc*_tmp596=Cyc_Absynpp_typ2doc(t);(*_tmp134).hd=_tmp596;}),(*_tmp134).tl=*effects);_tmp134;});*effects=_tmp597;});goto _LL0;}_LL0:;}
# 696
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
({struct Cyc_List_List*_tmp598=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);rgions=_tmp598;});
({struct Cyc_List_List*_tmp599=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);effects=_tmp599;});
if(rgions == 0  && effects != 0)
return({struct _dyneither_ptr _tmp59C=({const char*_tmp137="";_tag_dyneither(_tmp137,sizeof(char),1U);});struct _dyneither_ptr _tmp59B=({const char*_tmp138="";_tag_dyneither(_tmp138,sizeof(char),1U);});struct _dyneither_ptr _tmp59A=({const char*_tmp139="+";_tag_dyneither(_tmp139,sizeof(char),2U);});Cyc_PP_group(_tmp59C,_tmp59B,_tmp59A,effects);});else{
# 704
struct Cyc_PP_Doc*_tmp13A=({struct _dyneither_ptr _tmp59D=({const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp59D,rgions);});
return({struct _dyneither_ptr _tmp5A1=({const char*_tmp13B="";_tag_dyneither(_tmp13B,sizeof(char),1U);});struct _dyneither_ptr _tmp5A0=({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1U);});struct _dyneither_ptr _tmp59F=({const char*_tmp13D="+";_tag_dyneither(_tmp13D,sizeof(char),2U);});Cyc_PP_group(_tmp5A1,_tmp5A0,_tmp59F,({struct Cyc_List_List*_tmp59E=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp59E,({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));((*_tmp13E).hd=_tmp13A,(*_tmp13E).tl=0);_tmp13E;}));}));});}}
# 709
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp140=k;if(_tmp140 == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp141="struct ";_tag_dyneither(_tmp141,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp142="union ";_tag_dyneither(_tmp142,sizeof(char),7U);}));}_LL0:;}
# 717
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp143=t;struct Cyc_Absyn_Datatypedecl*_tmp1B8;struct Cyc_Absyn_Enumdecl*_tmp1B7;struct Cyc_Absyn_Aggrdecl*_tmp1B6;void*_tmp1B5;void*_tmp1B4;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Typedefdecl*_tmp1AE;struct _dyneither_ptr _tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct _tuple0*_tmp1AA;struct Cyc_List_List*_tmp1A9;enum Cyc_Absyn_AggrKind _tmp1A8;struct Cyc_List_List*_tmp1A7;union Cyc_Absyn_AggrInfoU _tmp1A6;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;int _tmp1A3;enum Cyc_Absyn_Sign _tmp1A2;enum Cyc_Absyn_Size_of _tmp1A1;union Cyc_Absyn_DatatypeFieldInfoU _tmp1A0;struct Cyc_List_List*_tmp19F;union Cyc_Absyn_DatatypeInfoU _tmp19E;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Tvar*_tmp19C;struct Cyc_Core_Opt*_tmp19B;void*_tmp19A;int _tmp199;struct Cyc_Core_Opt*_tmp198;switch(*((int*)_tmp143)){case 8U: _LL1: _LL2:
# 721
 return Cyc_PP_text(({const char*_tmp144="[[[array]]]";_tag_dyneither(_tmp144,sizeof(char),12U);}));case 9U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 5U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: _LL7: _LL8:
# 725
({struct Cyc_PP_Doc*_tmp5A2=Cyc_PP_text(({const char*_tmp145="void";_tag_dyneither(_tmp145,sizeof(char),5U);}));s=_tmp5A2;});goto _LL0;case 1U: _LL9: _tmp19B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f1;_tmp19A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_tmp199=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f3;_tmp198=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f4;_LLA:
# 727
 if(_tmp19A != 0)
# 729
return Cyc_Absynpp_ntyp2doc(_tmp19A);else{
# 731
struct _dyneither_ptr kindstring=_tmp19B == 0?({const char*_tmp14A="K";_tag_dyneither(_tmp14A,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp19B->v);
({struct Cyc_PP_Doc*_tmp5A4=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1U;_tmp149.f1=(unsigned long)_tmp199;({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0U;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);({void*_tmp146[2U];(_tmp146[0]=& _tmp148,_tmp146[1]=& _tmp149);({struct _dyneither_ptr _tmp5A3=({const char*_tmp147="`E%s%d";_tag_dyneither(_tmp147,sizeof(char),7U);});Cyc_aprintf(_tmp5A3,_tag_dyneither(_tmp146,sizeof(void*),2U));});});});}));s=_tmp5A4;});}
# 748 "absynpp.cyc"
goto _LL0;case 2U: _LLB: _tmp19C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LLC:
# 750
({struct Cyc_PP_Doc*_tmp5A5=Cyc_Absynpp_tvar2doc(_tmp19C);s=_tmp5A5;});
if(Cyc_Absynpp_print_all_kinds)
({struct Cyc_PP_Doc*_tmp5A8=({struct Cyc_PP_Doc*_tmp14B[3U];(((_tmp14B[0]=s,({struct Cyc_PP_Doc*_tmp5A7=Cyc_PP_text(({const char*_tmp14C="::";_tag_dyneither(_tmp14C,sizeof(char),3U);}));_tmp14B[1]=_tmp5A7;}))),({struct Cyc_PP_Doc*_tmp5A6=Cyc_Absynpp_kindbound2doc(_tmp19C->kind);_tmp14B[2]=_tmp5A6;}));Cyc_PP_cat(_tag_dyneither(_tmp14B,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5A8;});
# 754
goto _LL0;case 3U: _LLD: _tmp19E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).datatype_info;_tmp19D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).targs;_LLE:
# 756
{union Cyc_Absyn_DatatypeInfoU _tmp14D=_tmp19E;struct _tuple0*_tmp156;int _tmp155;struct _tuple0*_tmp154;int _tmp153;if((_tmp14D.UnknownDatatype).tag == 1){_LL40: _tmp154=((_tmp14D.UnknownDatatype).val).name;_tmp153=((_tmp14D.UnknownDatatype).val).is_extensible;_LL41:
# 758
 _tmp156=_tmp154;_tmp155=_tmp153;goto _LL43;}else{_LL42: _tmp156=(*(_tmp14D.KnownDatatype).val)->name;_tmp155=(*(_tmp14D.KnownDatatype).val)->is_extensible;_LL43: {
# 760
struct Cyc_PP_Doc*_tmp14E=Cyc_PP_text(({const char*_tmp152="datatype ";_tag_dyneither(_tmp152,sizeof(char),10U);}));
struct Cyc_PP_Doc*_tmp14F=_tmp155?Cyc_PP_text(({const char*_tmp151="@extensible ";_tag_dyneither(_tmp151,sizeof(char),13U);})): Cyc_PP_nil_doc();
({struct Cyc_PP_Doc*_tmp5AB=({struct Cyc_PP_Doc*_tmp150[4U];(((((_tmp150[0]=_tmp14F,_tmp150[1]=_tmp14E)),({struct Cyc_PP_Doc*_tmp5AA=Cyc_Absynpp_qvar2doc(_tmp156);_tmp150[2]=_tmp5AA;}))),({struct Cyc_PP_Doc*_tmp5A9=Cyc_Absynpp_tps2doc(_tmp19D);_tmp150[3]=_tmp5A9;}));Cyc_PP_cat(_tag_dyneither(_tmp150,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5AB;});
goto _LL3F;}}_LL3F:;}
# 765
goto _LL0;case 4U: _LLF: _tmp1A0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).field_info;_tmp19F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL10:
# 767
{union Cyc_Absyn_DatatypeFieldInfoU _tmp157=_tmp1A0;struct _tuple0*_tmp162;int _tmp161;struct _tuple0*_tmp160;struct _tuple0*_tmp15F;struct _tuple0*_tmp15E;int _tmp15D;if((_tmp157.UnknownDatatypefield).tag == 1){_LL45: _tmp15F=((_tmp157.UnknownDatatypefield).val).datatype_name;_tmp15E=((_tmp157.UnknownDatatypefield).val).field_name;_tmp15D=((_tmp157.UnknownDatatypefield).val).is_extensible;_LL46:
# 769
 _tmp162=_tmp15F;_tmp161=_tmp15D;_tmp160=_tmp15E;goto _LL48;}else{_LL47: _tmp162=(((_tmp157.KnownDatatypefield).val).f1)->name;_tmp161=(((_tmp157.KnownDatatypefield).val).f1)->is_extensible;_tmp160=(((_tmp157.KnownDatatypefield).val).f2)->name;_LL48: {
# 772
struct Cyc_PP_Doc*_tmp158=Cyc_PP_text(_tmp161?({const char*_tmp15B="@extensible datatype ";_tag_dyneither(_tmp15B,sizeof(char),22U);}):({const char*_tmp15C="datatype ";_tag_dyneither(_tmp15C,sizeof(char),10U);}));
({struct Cyc_PP_Doc*_tmp5AF=({struct Cyc_PP_Doc*_tmp159[4U];(((((_tmp159[0]=_tmp158,({struct Cyc_PP_Doc*_tmp5AE=Cyc_Absynpp_qvar2doc(_tmp162);_tmp159[1]=_tmp5AE;}))),({struct Cyc_PP_Doc*_tmp5AD=Cyc_PP_text(({const char*_tmp15A=".";_tag_dyneither(_tmp15A,sizeof(char),2U);}));_tmp159[2]=_tmp5AD;}))),({struct Cyc_PP_Doc*_tmp5AC=Cyc_Absynpp_qvar2doc(_tmp160);_tmp159[3]=_tmp5AC;}));Cyc_PP_cat(_tag_dyneither(_tmp159,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5AF;});
goto _LL44;}}_LL44:;}
# 776
goto _LL0;case 6U: _LL11: _tmp1A2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f2;_LL12: {
# 778
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp163=_tmp1A2;switch(_tmp163){case Cyc_Absyn_None: _LL4A: _LL4B:
 goto _LL4D;case Cyc_Absyn_Signed: _LL4C: _LL4D:
({struct _dyneither_ptr _tmp5B0=({const char*_tmp164="";_tag_dyneither(_tmp164,sizeof(char),1U);});sns=_tmp5B0;});goto _LL49;default: _LL4E: _LL4F:
({struct _dyneither_ptr _tmp5B1=({const char*_tmp165="unsigned ";_tag_dyneither(_tmp165,sizeof(char),10U);});sns=_tmp5B1;});goto _LL49;}_LL49:;}
# 785
{enum Cyc_Absyn_Size_of _tmp166=_tmp1A1;switch(_tmp166){case Cyc_Absyn_Char_sz: _LL51: _LL52:
# 787
{enum Cyc_Absyn_Sign _tmp167=_tmp1A2;switch(_tmp167){case Cyc_Absyn_None: _LL5C: _LL5D:
({struct _dyneither_ptr _tmp5B2=({const char*_tmp168="";_tag_dyneither(_tmp168,sizeof(char),1U);});sns=_tmp5B2;});goto _LL5B;case Cyc_Absyn_Signed: _LL5E: _LL5F:
({struct _dyneither_ptr _tmp5B3=({const char*_tmp169="signed ";_tag_dyneither(_tmp169,sizeof(char),8U);});sns=_tmp5B3;});goto _LL5B;default: _LL60: _LL61:
({struct _dyneither_ptr _tmp5B4=({const char*_tmp16A="unsigned ";_tag_dyneither(_tmp16A,sizeof(char),10U);});sns=_tmp5B4;});goto _LL5B;}_LL5B:;}
# 792
({struct _dyneither_ptr _tmp5B5=({const char*_tmp16B="char";_tag_dyneither(_tmp16B,sizeof(char),5U);});ts=_tmp5B5;});
goto _LL50;case Cyc_Absyn_Short_sz: _LL53: _LL54:
({struct _dyneither_ptr _tmp5B6=({const char*_tmp16C="short";_tag_dyneither(_tmp16C,sizeof(char),6U);});ts=_tmp5B6;});goto _LL50;case Cyc_Absyn_Int_sz: _LL55: _LL56:
({struct _dyneither_ptr _tmp5B7=({const char*_tmp16D="int";_tag_dyneither(_tmp16D,sizeof(char),4U);});ts=_tmp5B7;});goto _LL50;case Cyc_Absyn_Long_sz: _LL57: _LL58:
({struct _dyneither_ptr _tmp5B8=({const char*_tmp16E="long";_tag_dyneither(_tmp16E,sizeof(char),5U);});ts=_tmp5B8;});goto _LL50;default: _LL59: _LL5A:
# 798
{enum Cyc_Cyclone_C_Compilers _tmp16F=Cyc_Cyclone_c_compiler;if(_tmp16F == Cyc_Cyclone_Gcc_c){_LL63: _LL64:
({struct _dyneither_ptr _tmp5B9=({const char*_tmp170="long long";_tag_dyneither(_tmp170,sizeof(char),10U);});ts=_tmp5B9;});goto _LL62;}else{_LL65: _LL66:
({struct _dyneither_ptr _tmp5BA=({const char*_tmp171="__int64";_tag_dyneither(_tmp171,sizeof(char),8U);});ts=_tmp5BA;});goto _LL62;}_LL62:;}
# 802
goto _LL50;}_LL50:;}
# 804
({struct Cyc_PP_Doc*_tmp5BC=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0U;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0U;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);({void*_tmp172[2U];(_tmp172[0]=& _tmp174,_tmp172[1]=& _tmp175);({struct _dyneither_ptr _tmp5BB=({const char*_tmp173="%s%s";_tag_dyneither(_tmp173,sizeof(char),5U);});Cyc_aprintf(_tmp5BB,_tag_dyneither(_tmp172,sizeof(void*),2U));});});});}));s=_tmp5BC;});
goto _LL0;}case 7U: _LL13: _tmp1A3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp143)->f1;_LL14:
# 807
 if(_tmp1A3 == 0)
({struct Cyc_PP_Doc*_tmp5BD=Cyc_PP_text(({const char*_tmp176="float";_tag_dyneither(_tmp176,sizeof(char),6U);}));s=_tmp5BD;});else{
if(_tmp1A3 == 1)
({struct Cyc_PP_Doc*_tmp5BE=Cyc_PP_text(({const char*_tmp177="double";_tag_dyneither(_tmp177,sizeof(char),7U);}));s=_tmp5BE;});else{
# 812
({struct Cyc_PP_Doc*_tmp5BF=Cyc_PP_text(({const char*_tmp178="long double";_tag_dyneither(_tmp178,sizeof(char),12U);}));s=_tmp5BF;});}}
goto _LL0;case 10U: _LL15: _tmp1A4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL16:
# 815
({struct Cyc_PP_Doc*_tmp5C2=({struct Cyc_PP_Doc*_tmp179[2U];(({struct Cyc_PP_Doc*_tmp5C1=Cyc_Absynpp_dollar();_tmp179[0]=_tmp5C1;}),({struct Cyc_PP_Doc*_tmp5C0=Cyc_Absynpp_args2doc(_tmp1A4);_tmp179[1]=_tmp5C0;}));Cyc_PP_cat(_tag_dyneither(_tmp179,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp5C2;});
goto _LL0;case 11U: _LL17: _tmp1A6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).aggr_info;_tmp1A5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL18: {
# 818
struct _tuple10 _tmp17A=Cyc_Absyn_aggr_kinded_name(_tmp1A6);struct _tuple10 _tmp17B=_tmp17A;enum Cyc_Absyn_AggrKind _tmp17E;struct _tuple0*_tmp17D;_LL68: _tmp17E=_tmp17B.f1;_tmp17D=_tmp17B.f2;_LL69:;
({struct Cyc_PP_Doc*_tmp5C6=({struct Cyc_PP_Doc*_tmp17C[3U];(((({struct Cyc_PP_Doc*_tmp5C5=Cyc_Absynpp_aggr_kind2doc(_tmp17E);_tmp17C[0]=_tmp5C5;}),({struct Cyc_PP_Doc*_tmp5C4=Cyc_Absynpp_qvar2doc(_tmp17D);_tmp17C[1]=_tmp5C4;}))),({struct Cyc_PP_Doc*_tmp5C3=Cyc_Absynpp_tps2doc(_tmp1A5);_tmp17C[2]=_tmp5C3;}));Cyc_PP_cat(_tag_dyneither(_tmp17C,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5C6;});
goto _LL0;}case 12U: _LL19: _tmp1A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL1A:
# 822
({struct Cyc_PP_Doc*_tmp5CB=({struct Cyc_PP_Doc*_tmp17F[4U];(((((({struct Cyc_PP_Doc*_tmp5CA=Cyc_Absynpp_aggr_kind2doc(_tmp1A8);_tmp17F[0]=_tmp5CA;}),({struct Cyc_PP_Doc*_tmp5C9=Cyc_Absynpp_lb();_tmp17F[1]=_tmp5C9;}))),({
struct Cyc_PP_Doc*_tmp5C8=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1A7));_tmp17F[2]=_tmp5C8;}))),({
struct Cyc_PP_Doc*_tmp5C7=Cyc_Absynpp_rb();_tmp17F[3]=_tmp5C7;}));Cyc_PP_cat(_tag_dyneither(_tmp17F,sizeof(struct Cyc_PP_Doc*),4U));});
# 822
s=_tmp5CB;});
# 825
goto _LL0;case 14U: _LL1B: _tmp1A9=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp143)->f1;_LL1C:
# 827
({struct Cyc_PP_Doc*_tmp5D0=({struct Cyc_PP_Doc*_tmp180[4U];(((((({struct Cyc_PP_Doc*_tmp5CF=Cyc_PP_text(({const char*_tmp181="enum ";_tag_dyneither(_tmp181,sizeof(char),6U);}));_tmp180[0]=_tmp5CF;}),({struct Cyc_PP_Doc*_tmp5CE=Cyc_Absynpp_lb();_tmp180[1]=_tmp5CE;}))),({struct Cyc_PP_Doc*_tmp5CD=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1A9));_tmp180[2]=_tmp5CD;}))),({struct Cyc_PP_Doc*_tmp5CC=Cyc_Absynpp_rb();_tmp180[3]=_tmp5CC;}));Cyc_PP_cat(_tag_dyneither(_tmp180,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5D0;});goto _LL0;case 13U: _LL1D: _tmp1AA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp143)->f1;_LL1E:
# 829
({struct Cyc_PP_Doc*_tmp5D3=({struct Cyc_PP_Doc*_tmp182[2U];(({struct Cyc_PP_Doc*_tmp5D2=Cyc_PP_text(({const char*_tmp183="enum ";_tag_dyneither(_tmp183,sizeof(char),6U);}));_tmp182[0]=_tmp5D2;}),({struct Cyc_PP_Doc*_tmp5D1=Cyc_Absynpp_qvar2doc(_tmp1AA);_tmp182[1]=_tmp5D1;}));Cyc_PP_cat(_tag_dyneither(_tmp182,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp5D3;});goto _LL0;case 18U: _LL1F: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL20:
# 831
({struct Cyc_PP_Doc*_tmp5D7=({struct Cyc_PP_Doc*_tmp184[3U];(((({struct Cyc_PP_Doc*_tmp5D6=Cyc_PP_text(({const char*_tmp185="valueof_t(";_tag_dyneither(_tmp185,sizeof(char),11U);}));_tmp184[0]=_tmp5D6;}),({struct Cyc_PP_Doc*_tmp5D5=Cyc_Absynpp_exp2doc(_tmp1AB);_tmp184[1]=_tmp5D5;}))),({struct Cyc_PP_Doc*_tmp5D4=Cyc_PP_text(({const char*_tmp186=")";_tag_dyneither(_tmp186,sizeof(char),2U);}));_tmp184[2]=_tmp5D4;}));Cyc_PP_cat(_tag_dyneither(_tmp184,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5D7;});goto _LL0;case 27U: _LL21: _tmp1AC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL22:
# 833
({struct Cyc_PP_Doc*_tmp5DB=({struct Cyc_PP_Doc*_tmp187[3U];(((({struct Cyc_PP_Doc*_tmp5DA=Cyc_PP_text(({const char*_tmp188="typeof(";_tag_dyneither(_tmp188,sizeof(char),8U);}));_tmp187[0]=_tmp5DA;}),({struct Cyc_PP_Doc*_tmp5D9=Cyc_Absynpp_exp2doc(_tmp1AC);_tmp187[1]=_tmp5D9;}))),({struct Cyc_PP_Doc*_tmp5D8=Cyc_PP_text(({const char*_tmp189=")";_tag_dyneither(_tmp189,sizeof(char),2U);}));_tmp187[2]=_tmp5D8;}));Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5DB;});goto _LL0;case 28U: _LL23: _tmp1AD=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp143)->f1;_LL24:
# 835
({struct Cyc_PP_Doc*_tmp5DC=Cyc_PP_text(_tmp1AD);s=_tmp5DC;});goto _LL0;case 17U: _LL25: _tmp1B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_LL26:
# 842
({struct Cyc_PP_Doc*_tmp5DF=({struct Cyc_PP_Doc*_tmp18A[2U];(({struct Cyc_PP_Doc*_tmp5DE=Cyc_Absynpp_qvar2doc(_tmp1B0);_tmp18A[0]=_tmp5DE;}),({struct Cyc_PP_Doc*_tmp5DD=Cyc_Absynpp_tps2doc(_tmp1AF);_tmp18A[1]=_tmp5DD;}));Cyc_PP_cat(_tag_dyneither(_tmp18A,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp5DF;});
goto _LL0;case 15U: _LL27: _tmp1B1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp143)->f1;_LL28:
# 845
({struct Cyc_PP_Doc*_tmp5E3=({struct Cyc_PP_Doc*_tmp18B[3U];(((({struct Cyc_PP_Doc*_tmp5E2=Cyc_PP_text(({const char*_tmp18C="region_t<";_tag_dyneither(_tmp18C,sizeof(char),10U);}));_tmp18B[0]=_tmp5E2;}),({struct Cyc_PP_Doc*_tmp5E1=Cyc_Absynpp_rgn2doc(_tmp1B1);_tmp18B[1]=_tmp5E1;}))),({struct Cyc_PP_Doc*_tmp5E0=Cyc_PP_text(({const char*_tmp18D=">";_tag_dyneither(_tmp18D,sizeof(char),2U);}));_tmp18B[2]=_tmp5E0;}));Cyc_PP_cat(_tag_dyneither(_tmp18B,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5E3;});goto _LL0;case 16U: _LL29: _tmp1B3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f1;_tmp1B2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f2;_LL2A:
# 847
({struct Cyc_PP_Doc*_tmp5E9=({struct Cyc_PP_Doc*_tmp18E[5U];(((((((({struct Cyc_PP_Doc*_tmp5E8=Cyc_PP_text(({const char*_tmp18F="dynregion_t<";_tag_dyneither(_tmp18F,sizeof(char),13U);}));_tmp18E[0]=_tmp5E8;}),({struct Cyc_PP_Doc*_tmp5E7=Cyc_Absynpp_rgn2doc(_tmp1B3);_tmp18E[1]=_tmp5E7;}))),({struct Cyc_PP_Doc*_tmp5E6=Cyc_PP_text(({const char*_tmp190=",";_tag_dyneither(_tmp190,sizeof(char),2U);}));_tmp18E[2]=_tmp5E6;}))),({struct Cyc_PP_Doc*_tmp5E5=Cyc_Absynpp_rgn2doc(_tmp1B2);_tmp18E[3]=_tmp5E5;}))),({
struct Cyc_PP_Doc*_tmp5E4=Cyc_PP_text(({const char*_tmp191=">";_tag_dyneither(_tmp191,sizeof(char),2U);}));_tmp18E[4]=_tmp5E4;}));Cyc_PP_cat(_tag_dyneither(_tmp18E,sizeof(struct Cyc_PP_Doc*),5U));});
# 847
s=_tmp5E9;});
goto _LL0;case 19U: _LL2B: _tmp1B4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp143)->f1;_LL2C:
# 850
({struct Cyc_PP_Doc*_tmp5ED=({struct Cyc_PP_Doc*_tmp192[3U];(((({struct Cyc_PP_Doc*_tmp5EC=Cyc_PP_text(({const char*_tmp193="tag_t<";_tag_dyneither(_tmp193,sizeof(char),7U);}));_tmp192[0]=_tmp5EC;}),({struct Cyc_PP_Doc*_tmp5EB=Cyc_Absynpp_typ2doc(_tmp1B4);_tmp192[1]=_tmp5EB;}))),({struct Cyc_PP_Doc*_tmp5EA=Cyc_PP_text(({const char*_tmp194=">";_tag_dyneither(_tmp194,sizeof(char),2U);}));_tmp192[2]=_tmp5EA;}));Cyc_PP_cat(_tag_dyneither(_tmp192,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5ED;});goto _LL0;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 22U: _LL31: _LL32:
# 854
({struct Cyc_PP_Doc*_tmp5EE=Cyc_Absynpp_rgn2doc(t);s=_tmp5EE;});goto _LL0;case 25U: _LL33: _tmp1B5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp143)->f1;_LL34:
# 856
({struct Cyc_PP_Doc*_tmp5F2=({struct Cyc_PP_Doc*_tmp195[3U];(((({struct Cyc_PP_Doc*_tmp5F1=Cyc_PP_text(({const char*_tmp196="regions(";_tag_dyneither(_tmp196,sizeof(char),9U);}));_tmp195[0]=_tmp5F1;}),({struct Cyc_PP_Doc*_tmp5F0=Cyc_Absynpp_typ2doc(_tmp1B5);_tmp195[1]=_tmp5F0;}))),({struct Cyc_PP_Doc*_tmp5EF=Cyc_PP_text(({const char*_tmp197=")";_tag_dyneither(_tmp197,sizeof(char),2U);}));_tmp195[2]=_tmp5EF;}));Cyc_PP_cat(_tag_dyneither(_tmp195,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5F2;});goto _LL0;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
# 859
({struct Cyc_PP_Doc*_tmp5F3=Cyc_Absynpp_eff2doc(t);s=_tmp5F3;});goto _LL0;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0U: _LL39: _tmp1B6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL3A:
# 861
({struct Cyc_PP_Doc*_tmp5F4=Cyc_Absynpp_aggrdecl2doc(_tmp1B6);s=_tmp5F4;});goto _LL0;case 1U: _LL3B: _tmp1B7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL3C:
# 863
({struct Cyc_PP_Doc*_tmp5F5=Cyc_Absynpp_enumdecl2doc(_tmp1B7);s=_tmp5F5;});goto _LL0;default: _LL3D: _tmp1B8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL3E:
# 865
({struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_datatypedecl2doc(_tmp1B8);s=_tmp5F6;});goto _LL0;}}_LL0:;}
# 867
return s;}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1B9=cmp;void*_tmp1BD;void*_tmp1BC;_LL1: _tmp1BD=_tmp1B9->f1;_tmp1BC=_tmp1B9->f2;_LL2:;
return({struct Cyc_PP_Doc*_tmp1BA[3U];(((({struct Cyc_PP_Doc*_tmp5F9=Cyc_Absynpp_rgn2doc(_tmp1BD);_tmp1BA[0]=_tmp5F9;}),({struct Cyc_PP_Doc*_tmp5F8=Cyc_PP_text(({const char*_tmp1BB=" > ";_tag_dyneither(_tmp1BB,sizeof(char),4U);}));_tmp1BA[1]=_tmp5F8;}))),({struct Cyc_PP_Doc*_tmp5F7=Cyc_Absynpp_rgn2doc(_tmp1BC);_tmp1BA[2]=_tmp5F7;}));Cyc_PP_cat(_tag_dyneither(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3U));});}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _dyneither_ptr _tmp5FC=({const char*_tmp1BE="";_tag_dyneither(_tmp1BE,sizeof(char),1U);});struct _dyneither_ptr _tmp5FB=({const char*_tmp1BF="";_tag_dyneither(_tmp1BF,sizeof(char),1U);});struct _dyneither_ptr _tmp5FA=({const char*_tmp1C0=",";_tag_dyneither(_tmp1C0,sizeof(char),2U);});Cyc_PP_group(_tmp5FC,_tmp5FB,_tmp5FA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C1=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C1 == 0?0:({struct Cyc_Core_Opt*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));({struct Cyc_PP_Doc*_tmp5FD=Cyc_PP_text(*_tmp1C1);(*_tmp1C2).v=_tmp5FD;});_tmp1C2;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 889
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 893
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
({struct Cyc_List_List*_tmp600=({struct Cyc_List_List*_tmp5FF=_tmp1C3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5FF,({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));(({struct Cyc_PP_Doc*_tmp5FE=Cyc_PP_text(({const char*_tmp1C4="...";_tag_dyneither(_tmp1C4,sizeof(char),4U);}));(*_tmp1C5).hd=_tmp5FE;}),(*_tmp1C5).tl=0);_tmp1C5;}));});_tmp1C3=_tmp600;});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1C6=({struct Cyc_PP_Doc*_tmp1C8[3U];(((({struct Cyc_PP_Doc*_tmp604=Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4U);}));_tmp1C8[0]=_tmp604;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp603=Cyc_PP_text(({const char*_tmp1CA=" inject ";_tag_dyneither(_tmp1CA,sizeof(char),9U);}));_tmp1C8[1]=_tmp603;}):({struct Cyc_PP_Doc*_tmp602=Cyc_PP_text(({const char*_tmp1CB=" ";_tag_dyneither(_tmp1CB,sizeof(char),2U);}));_tmp1C8[1]=_tmp602;}))),({
struct Cyc_PP_Doc*_tmp601=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));((((*_tmp1CC).f1=cyc_varargs->name,(*_tmp1CC).f2=cyc_varargs->tq)),(*_tmp1CC).f3=cyc_varargs->type);_tmp1CC;}));_tmp1C8[2]=_tmp601;}));Cyc_PP_cat(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3U));});
# 902
({struct Cyc_List_List*_tmp606=({struct Cyc_List_List*_tmp605=_tmp1C3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp605,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));((*_tmp1C7).hd=_tmp1C6,(*_tmp1C7).tl=0);_tmp1C7;}));});_tmp1C3=_tmp606;});}}{
# 904
struct Cyc_PP_Doc*_tmp1CD=({struct _dyneither_ptr _tmp609=({const char*_tmp1DC="";_tag_dyneither(_tmp1DC,sizeof(char),1U);});struct _dyneither_ptr _tmp608=({const char*_tmp1DD="";_tag_dyneither(_tmp1DD,sizeof(char),1U);});struct _dyneither_ptr _tmp607=({const char*_tmp1DE=",";_tag_dyneither(_tmp1DE,sizeof(char),2U);});Cyc_PP_group(_tmp609,_tmp608,_tmp607,_tmp1C3);});
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
({struct Cyc_PP_Doc*_tmp60C=({struct Cyc_PP_Doc*_tmp1CE[3U];(((_tmp1CE[0]=_tmp1CD,({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(({const char*_tmp1CF=";";_tag_dyneither(_tmp1CF,sizeof(char),2U);}));_tmp1CE[1]=_tmp60B;}))),({struct Cyc_PP_Doc*_tmp60A=Cyc_Absynpp_eff2doc(effopt);_tmp1CE[2]=_tmp60A;}));Cyc_PP_cat(_tag_dyneither(_tmp1CE,sizeof(struct Cyc_PP_Doc*),3U));});_tmp1CD=_tmp60C;});
if(rgn_po != 0)
({struct Cyc_PP_Doc*_tmp60F=({struct Cyc_PP_Doc*_tmp1D0[3U];(((_tmp1D0[0]=_tmp1CD,({struct Cyc_PP_Doc*_tmp60E=Cyc_PP_text(({const char*_tmp1D1=":";_tag_dyneither(_tmp1D1,sizeof(char),2U);}));_tmp1D0[1]=_tmp60E;}))),({struct Cyc_PP_Doc*_tmp60D=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1D0[2]=_tmp60D;}));Cyc_PP_cat(_tag_dyneither(_tmp1D0,sizeof(struct Cyc_PP_Doc*),3U));});_tmp1CD=_tmp60F;});{
struct Cyc_PP_Doc*_tmp1D2=({struct Cyc_PP_Doc*_tmp1D9[3U];(((({struct Cyc_PP_Doc*_tmp611=Cyc_PP_text(({const char*_tmp1DA="(";_tag_dyneither(_tmp1DA,sizeof(char),2U);}));_tmp1D9[0]=_tmp611;}),_tmp1D9[1]=_tmp1CD)),({struct Cyc_PP_Doc*_tmp610=Cyc_PP_text(({const char*_tmp1DB=")";_tag_dyneither(_tmp1DB,sizeof(char),2U);}));_tmp1D9[2]=_tmp610;}));Cyc_PP_cat(_tag_dyneither(_tmp1D9,sizeof(struct Cyc_PP_Doc*),3U));});
if(req != 0)
({struct Cyc_PP_Doc*_tmp615=({struct Cyc_PP_Doc*_tmp1D3[4U];(((((_tmp1D3[0]=_tmp1D2,({struct Cyc_PP_Doc*_tmp614=Cyc_PP_text(({const char*_tmp1D4=" @requires(";_tag_dyneither(_tmp1D4,sizeof(char),12U);}));_tmp1D3[1]=_tmp614;}))),({struct Cyc_PP_Doc*_tmp613=Cyc_Absynpp_exp2doc(req);_tmp1D3[2]=_tmp613;}))),({struct Cyc_PP_Doc*_tmp612=Cyc_PP_text(({const char*_tmp1D5=")";_tag_dyneither(_tmp1D5,sizeof(char),2U);}));_tmp1D3[3]=_tmp612;}));Cyc_PP_cat(_tag_dyneither(_tmp1D3,sizeof(struct Cyc_PP_Doc*),4U));});_tmp1D2=_tmp615;});
if(ens != 0)
({struct Cyc_PP_Doc*_tmp619=({struct Cyc_PP_Doc*_tmp1D6[4U];(((((_tmp1D6[0]=_tmp1D2,({struct Cyc_PP_Doc*_tmp618=Cyc_PP_text(({const char*_tmp1D7=" @ensures(";_tag_dyneither(_tmp1D7,sizeof(char),11U);}));_tmp1D6[1]=_tmp618;}))),({struct Cyc_PP_Doc*_tmp617=Cyc_Absynpp_exp2doc(ens);_tmp1D6[2]=_tmp617;}))),({struct Cyc_PP_Doc*_tmp616=Cyc_PP_text(({const char*_tmp1D8=")";_tag_dyneither(_tmp1D8,sizeof(char),2U);}));_tmp1D6[3]=_tmp616;}));Cyc_PP_cat(_tag_dyneither(_tmp1D6,sizeof(struct Cyc_PP_Doc*),4U));});_tmp1D2=_tmp619;});
return _tmp1D2;};};}
# 917
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1DF=arg;struct _dyneither_ptr*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;_LL1: _tmp1E3=_tmp1DF->f1;_tmp1E2=_tmp1DF->f2;_tmp1E1=_tmp1DF->f3;_LL2:;
return({struct _tuple8*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));((((*_tmp1E0).f1=_tmp1E3,(*_tmp1E0).f2=_tmp1E2)),(*_tmp1E0).f3=_tmp1E1);_tmp1E0;});}
# 922
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 924
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E4=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1E5=(*q).f1;union Cyc_Absyn_Nmspace _tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;switch((_tmp1E6.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1E8=0;goto _LL4;case 1U: _LL3: _tmp1E8=(_tmp1E6.Rel_n).val;_LL4:
# 930
 match=0;
_tmp1E4=_tmp1E8;
goto _LL0;case 3U: _LL5: _tmp1E9=(_tmp1E6.C_n).val;_LL6:
# 934
({int _tmp61A=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E9,Cyc_Absynpp_curr_namespace);match=_tmp61A;});
# 936
goto _LL0;default: _LL7: _tmp1EA=(_tmp1E6.Abs_n).val;_LL8:
# 938
({int _tmp61B=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EA,Cyc_Absynpp_curr_namespace);match=_tmp61B;});
({struct Cyc_List_List*_tmp61C=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));((*_tmp1E7).hd=Cyc_Absynpp_cyc_stringptr,(*_tmp1E7).tl=_tmp1EA);_tmp1E7;}): _tmp1EA;_tmp1E4=_tmp61C;});
goto _LL0;}_LL0:;}
# 942
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp61E=({struct Cyc_List_List*_tmp61D=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp61D,({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));
((*_tmp1EB).hd=(*q).f2,(*_tmp1EB).tl=0);_tmp1EB;}));});
# 943
Cyc_str_sepstr(_tmp61E,({const char*_tmp1EC="_";_tag_dyneither(_tmp1EC,sizeof(char),2U);}));});else{
# 947
if(match)
return*(*q).f2;else{
# 950
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp620=({struct Cyc_List_List*_tmp61F=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp61F,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));((*_tmp1ED).hd=(*q).f2,(*_tmp1ED).tl=0);_tmp1ED;}));});Cyc_str_sepstr(_tmp620,({const char*_tmp1EE="::";_tag_dyneither(_tmp1EE,sizeof(char),3U);}));});}}}
# 954
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 958
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 962
return({struct _dyneither_ptr _tmp623=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp622=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp621=({const char*_tmp1F0="\\textbf{";_tag_dyneither(_tmp1F0,sizeof(char),9U);});Cyc_strconcat(_tmp621,(struct _dyneither_ptr)_tmp1EF);});Cyc_strconcat(_tmp622,({const char*_tmp1F1="}";_tag_dyneither(_tmp1F1,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp623,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1EF));});else{
return Cyc_PP_text(_tmp1EF);}}
# 967
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 969
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 972
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F2=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F3=_tmp1F2;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F5;switch((_tmp1F3.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1F3.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 983
 return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp624=({const char*_tmp1F4="/* bad namespace : */ ";_tag_dyneither(_tmp1F4,sizeof(char),23U);});Cyc_strconcat(_tmp624,(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1F5=(_tmp1F3.C_n).val;_LL6:
# 976
 _tmp1F6=_tmp1F5;goto _LL8;default: _LL7: _tmp1F6=(_tmp1F3.Abs_n).val;_LL8:
# 978
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1F6,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 981
goto _LLA;}}_LL0:;}else{
# 986
return*(*v).f2;}}
# 989
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 992
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1F7=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 996
return({struct _dyneither_ptr _tmp627=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp626=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp625=({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp625,(struct _dyneither_ptr)_tmp1F7);});Cyc_strconcat(_tmp626,({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp627,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));});else{
return Cyc_PP_text(_tmp1F7);}}
# 1001
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp628=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp628,t,0);});}
# 1012 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->r;void*_tmp1FB=_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp1FE;enum Cyc_Absyn_Primop _tmp1FD;switch(*((int*)_tmp1FB)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp1FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL6: {
# 1017
enum Cyc_Absyn_Primop _tmp1FC=_tmp1FD;switch(_tmp1FC){case Cyc_Absyn_Plus: _LL54: _LL55:
 return 100;case Cyc_Absyn_Times: _LL56: _LL57:
 return 110;case Cyc_Absyn_Minus: _LL58: _LL59:
 return 100;case Cyc_Absyn_Div: _LL5A: _LL5B:
 goto _LL5D;case Cyc_Absyn_Mod: _LL5C: _LL5D:
 return 110;case Cyc_Absyn_Eq: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Neq: _LL60: _LL61:
 return 70;case Cyc_Absyn_Gt: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Lt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Gte: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lte: _LL68: _LL69:
 return 80;case Cyc_Absyn_Not: _LL6A: _LL6B:
 goto _LL6D;case Cyc_Absyn_Bitnot: _LL6C: _LL6D:
 return 130;case Cyc_Absyn_Bitand: _LL6E: _LL6F:
 return 60;case Cyc_Absyn_Bitor: _LL70: _LL71:
 return 40;case Cyc_Absyn_Bitxor: _LL72: _LL73:
 return 50;case Cyc_Absyn_Bitlshift: _LL74: _LL75:
 return 90;case Cyc_Absyn_Bitlrshift: _LL76: _LL77:
 return 80;case Cyc_Absyn_Bitarshift: _LL78: _LL79:
 return 80;default: _LL7A: _LL7B:
 return 140;}_LL53:;}case 4U: _LL7: _LL8:
# 1039
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp1FE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1A:
 return Cyc_Absynpp_exp_prec(_tmp1FE);case 13U: _LL1B: _tmp1FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1C:
 return Cyc_Absynpp_exp_prec(_tmp1FF);case 14U: _LL1D: _LL1E:
 return 120;case 16U: _LL1F: _LL20:
 return 15;case 15U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 40U: _LL29: _LL2A:
 goto _LL2C;case 38U: _LL2B: _LL2C:
 goto _LL2E;case 19U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 return 130;case 21U: _LL31: _LL32:
 goto _LL34;case 22U: _LL33: _LL34:
 goto _LL36;case 23U: _LL35: _LL36:
 return 140;case 24U: _LL37: _LL38:
 return 150;case 25U: _LL39: _LL3A:
 goto _LL3C;case 26U: _LL3B: _LL3C:
 goto _LL3E;case 27U: _LL3D: _LL3E:
 goto _LL40;case 28U: _LL3F: _LL40:
 goto _LL42;case 29U: _LL41: _LL42:
 goto _LL44;case 30U: _LL43: _LL44:
 goto _LL46;case 31U: _LL45: _LL46:
 goto _LL48;case 32U: _LL47: _LL48:
 goto _LL4A;case 33U: _LL49: _LL4A:
 goto _LL4C;case 34U: _LL4B: _LL4C:
 goto _LL4E;case 35U: _LL4D: _LL4E:
 goto _LL50;case 36U: _LL4F: _LL50:
 return 140;default: _LL51: _LL52:
 return 10000;}_LL0:;}
# 1080
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1084
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp200=e->r;void*_tmp201=_tmp200;struct Cyc_Absyn_Stmt*_tmp2D4;struct Cyc_Core_Opt*_tmp2D3;struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;int _tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;void**_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;int _tmp2CB;struct Cyc_Absyn_Enumfield*_tmp2CA;struct Cyc_Absyn_Enumfield*_tmp2C9;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_Datatypefield*_tmp2C7;struct Cyc_List_List*_tmp2C6;struct _tuple0*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_List_List*_tmp2BD;struct _tuple8*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;struct _dyneither_ptr*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B5;struct _dyneither_ptr*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;void*_tmp2B2;struct Cyc_List_List*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;struct _dyneither_ptr*_tmp2AF;int _tmp2AE;struct _dyneither_ptr _tmp2AD;void*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;void*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;void*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;enum Cyc_Absyn_Incrementor _tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Core_Opt*_tmp293;struct Cyc_Absyn_Exp*_tmp292;enum Cyc_Absyn_Primop _tmp291;struct Cyc_List_List*_tmp290;struct _dyneither_ptr _tmp28F;void*_tmp28E;union Cyc_Absyn_Cnst _tmp28D;switch(*((int*)_tmp201)){case 0U: _LL1: _tmp28D=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL2:
({struct Cyc_PP_Doc*_tmp629=Cyc_Absynpp_cnst2doc(_tmp28D);s=_tmp629;});goto _LL0;case 1U: _LL3: _tmp28E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL4:
({struct Cyc_PP_Doc*_tmp62A=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp28E));s=_tmp62A;});goto _LL0;case 2U: _LL5: _tmp28F=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL6:
# 1091
({struct Cyc_PP_Doc*_tmp62F=({struct Cyc_PP_Doc*_tmp202[4U];(((((({struct Cyc_PP_Doc*_tmp62E=Cyc_PP_text(({const char*_tmp203="__cyclone_pragma__";_tag_dyneither(_tmp203,sizeof(char),19U);}));_tmp202[0]=_tmp62E;}),({struct Cyc_PP_Doc*_tmp62D=Cyc_PP_text(({const char*_tmp204="(";_tag_dyneither(_tmp204,sizeof(char),2U);}));_tmp202[1]=_tmp62D;}))),({struct Cyc_PP_Doc*_tmp62C=Cyc_PP_text(_tmp28F);_tmp202[2]=_tmp62C;}))),({struct Cyc_PP_Doc*_tmp62B=Cyc_PP_text(({const char*_tmp205=")";_tag_dyneither(_tmp205,sizeof(char),2U);}));_tmp202[3]=_tmp62B;}));Cyc_PP_cat(_tag_dyneither(_tmp202,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp62F;});goto _LL0;case 3U: _LL7: _tmp291=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp290=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL8:
({struct Cyc_PP_Doc*_tmp630=Cyc_Absynpp_primapp2doc(myprec,_tmp291,_tmp290);s=_tmp630;});goto _LL0;case 4U: _LL9: _tmp294=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp293=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp292=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LLA:
# 1094
({struct Cyc_PP_Doc*_tmp637=({struct Cyc_PP_Doc*_tmp206[5U];(((((((({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_exp2doc_prec(myprec,_tmp294);_tmp206[0]=_tmp636;}),({
struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp207=" ";_tag_dyneither(_tmp207,sizeof(char),2U);}));_tmp206[1]=_tmp635;}))),
_tmp293 == 0?({struct Cyc_PP_Doc*_tmp634=Cyc_PP_nil_doc();_tmp206[2]=_tmp634;}):({struct Cyc_PP_Doc*_tmp633=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp293->v);_tmp206[2]=_tmp633;}))),({
struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp208="= ";_tag_dyneither(_tmp208,sizeof(char),3U);}));_tmp206[3]=_tmp632;}))),({
struct Cyc_PP_Doc*_tmp631=Cyc_Absynpp_exp2doc_prec(myprec,_tmp292);_tmp206[4]=_tmp631;}));Cyc_PP_cat(_tag_dyneither(_tmp206,sizeof(struct Cyc_PP_Doc*),5U));});
# 1094
s=_tmp637;});
# 1099
goto _LL0;case 5U: _LLB: _tmp296=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp295=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LLC: {
# 1101
struct Cyc_PP_Doc*_tmp209=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296);
{enum Cyc_Absyn_Incrementor _tmp20A=_tmp295;switch(_tmp20A){case Cyc_Absyn_PreInc: _LL54: _LL55:
({struct Cyc_PP_Doc*_tmp639=({struct Cyc_PP_Doc*_tmp20B[2U];(({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp20C="++";_tag_dyneither(_tmp20C,sizeof(char),3U);}));_tmp20B[0]=_tmp638;}),_tmp20B[1]=_tmp209);Cyc_PP_cat(_tag_dyneither(_tmp20B,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp639;});goto _LL53;case Cyc_Absyn_PreDec: _LL56: _LL57:
({struct Cyc_PP_Doc*_tmp63B=({struct Cyc_PP_Doc*_tmp20D[2U];(({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp20E="--";_tag_dyneither(_tmp20E,sizeof(char),3U);}));_tmp20D[0]=_tmp63A;}),_tmp20D[1]=_tmp209);Cyc_PP_cat(_tag_dyneither(_tmp20D,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp63B;});goto _LL53;case Cyc_Absyn_PostInc: _LL58: _LL59:
({struct Cyc_PP_Doc*_tmp63D=({struct Cyc_PP_Doc*_tmp20F[2U];(_tmp20F[0]=_tmp209,({struct Cyc_PP_Doc*_tmp63C=Cyc_PP_text(({const char*_tmp210="++";_tag_dyneither(_tmp210,sizeof(char),3U);}));_tmp20F[1]=_tmp63C;}));Cyc_PP_cat(_tag_dyneither(_tmp20F,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp63D;});goto _LL53;default: _LL5A: _LL5B:
({struct Cyc_PP_Doc*_tmp63F=({struct Cyc_PP_Doc*_tmp211[2U];(_tmp211[0]=_tmp209,({struct Cyc_PP_Doc*_tmp63E=Cyc_PP_text(({const char*_tmp212="--";_tag_dyneither(_tmp212,sizeof(char),3U);}));_tmp211[1]=_tmp63E;}));Cyc_PP_cat(_tag_dyneither(_tmp211,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp63F;});goto _LL53;}_LL53:;}
# 1108
goto _LL0;}case 6U: _LLD: _tmp299=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp298=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp297=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LLE:
# 1110
({struct Cyc_PP_Doc*_tmp645=({struct Cyc_PP_Doc*_tmp213[5U];(((((((({struct Cyc_PP_Doc*_tmp644=Cyc_Absynpp_exp2doc_prec(myprec,_tmp299);_tmp213[0]=_tmp644;}),({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp214=" ? ";_tag_dyneither(_tmp214,sizeof(char),4U);}));_tmp213[1]=_tmp643;}))),({struct Cyc_PP_Doc*_tmp642=Cyc_Absynpp_exp2doc_prec(0,_tmp298);_tmp213[2]=_tmp642;}))),({
struct Cyc_PP_Doc*_tmp641=Cyc_PP_text(({const char*_tmp215=" : ";_tag_dyneither(_tmp215,sizeof(char),4U);}));_tmp213[3]=_tmp641;}))),({struct Cyc_PP_Doc*_tmp640=Cyc_Absynpp_exp2doc_prec(myprec,_tmp297);_tmp213[4]=_tmp640;}));Cyc_PP_cat(_tag_dyneither(_tmp213,sizeof(struct Cyc_PP_Doc*),5U));});
# 1110
s=_tmp645;});
# 1112
goto _LL0;case 7U: _LLF: _tmp29B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL10:
# 1114
({struct Cyc_PP_Doc*_tmp649=({struct Cyc_PP_Doc*_tmp216[3U];(((({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B);_tmp216[0]=_tmp648;}),({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp217=" && ";_tag_dyneither(_tmp217,sizeof(char),5U);}));_tmp216[1]=_tmp647;}))),({struct Cyc_PP_Doc*_tmp646=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29A);_tmp216[2]=_tmp646;}));Cyc_PP_cat(_tag_dyneither(_tmp216,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp649;});
goto _LL0;case 8U: _LL11: _tmp29D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL12:
# 1117
({struct Cyc_PP_Doc*_tmp64D=({struct Cyc_PP_Doc*_tmp218[3U];(((({struct Cyc_PP_Doc*_tmp64C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp218[0]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_PP_text(({const char*_tmp219=" || ";_tag_dyneither(_tmp219,sizeof(char),5U);}));_tmp218[1]=_tmp64B;}))),({struct Cyc_PP_Doc*_tmp64A=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29C);_tmp218[2]=_tmp64A;}));Cyc_PP_cat(_tag_dyneither(_tmp218,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp64D;});
goto _LL0;case 9U: _LL13: _tmp29F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL14:
# 1122
({struct Cyc_PP_Doc*_tmp653=({struct Cyc_PP_Doc*_tmp21A[5U];(((((((({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp21B="(";_tag_dyneither(_tmp21B,sizeof(char),2U);}));_tmp21A[0]=_tmp652;}),({struct Cyc_PP_Doc*_tmp651=Cyc_Absynpp_exp2doc(_tmp29F);_tmp21A[1]=_tmp651;}))),({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(({const char*_tmp21C=", ";_tag_dyneither(_tmp21C,sizeof(char),3U);}));_tmp21A[2]=_tmp650;}))),({struct Cyc_PP_Doc*_tmp64F=Cyc_Absynpp_exp2doc(_tmp29E);_tmp21A[3]=_tmp64F;}))),({struct Cyc_PP_Doc*_tmp64E=Cyc_PP_text(({const char*_tmp21D=")";_tag_dyneither(_tmp21D,sizeof(char),2U);}));_tmp21A[4]=_tmp64E;}));Cyc_PP_cat(_tag_dyneither(_tmp21A,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp653;});
goto _LL0;case 10U: _LL15: _tmp2A1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL16:
# 1125
({struct Cyc_PP_Doc*_tmp658=({struct Cyc_PP_Doc*_tmp21E[4U];(((((({struct Cyc_PP_Doc*_tmp657=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A1);_tmp21E[0]=_tmp657;}),({
struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp21F="(";_tag_dyneither(_tmp21F,sizeof(char),2U);}));_tmp21E[1]=_tmp656;}))),({
struct Cyc_PP_Doc*_tmp655=Cyc_Absynpp_exps2doc_prec(20,_tmp2A0);_tmp21E[2]=_tmp655;}))),({
struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(({const char*_tmp220=")";_tag_dyneither(_tmp220,sizeof(char),2U);}));_tmp21E[3]=_tmp654;}));Cyc_PP_cat(_tag_dyneither(_tmp21E,sizeof(struct Cyc_PP_Doc*),4U));});
# 1125
s=_tmp658;});
# 1129
goto _LL0;case 11U: _LL17: _tmp2A2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL18:
# 1131
({struct Cyc_PP_Doc*_tmp65B=({struct Cyc_PP_Doc*_tmp221[2U];(({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_text(({const char*_tmp222="throw ";_tag_dyneither(_tmp222,sizeof(char),7U);}));_tmp221[0]=_tmp65A;}),({struct Cyc_PP_Doc*_tmp659=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp221[1]=_tmp659;}));Cyc_PP_cat(_tag_dyneither(_tmp221,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp65B;});
goto _LL0;case 12U: _LL19: _tmp2A3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL1A:
# 1134
({struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A3);s=_tmp65C;});
goto _LL0;case 13U: _LL1B: _tmp2A4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL1C:
# 1137
({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A4);s=_tmp65D;});
goto _LL0;case 14U: _LL1D: _tmp2A6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL1E:
# 1140
({struct Cyc_PP_Doc*_tmp662=({struct Cyc_PP_Doc*_tmp223[4U];(((((({struct Cyc_PP_Doc*_tmp661=Cyc_PP_text(({const char*_tmp224="(";_tag_dyneither(_tmp224,sizeof(char),2U);}));_tmp223[0]=_tmp661;}),({
struct Cyc_PP_Doc*_tmp660=Cyc_Absynpp_typ2doc(_tmp2A6);_tmp223[1]=_tmp660;}))),({
struct Cyc_PP_Doc*_tmp65F=Cyc_PP_text(({const char*_tmp225=")";_tag_dyneither(_tmp225,sizeof(char),2U);}));_tmp223[2]=_tmp65F;}))),({
struct Cyc_PP_Doc*_tmp65E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A5);_tmp223[3]=_tmp65E;}));Cyc_PP_cat(_tag_dyneither(_tmp223,sizeof(struct Cyc_PP_Doc*),4U));});
# 1140
s=_tmp662;});
# 1144
goto _LL0;case 15U: _LL1F: _tmp2A7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL20:
# 1146
({struct Cyc_PP_Doc*_tmp665=({struct Cyc_PP_Doc*_tmp226[2U];(({struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp227="&";_tag_dyneither(_tmp227,sizeof(char),2U);}));_tmp226[0]=_tmp664;}),({
struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A7);_tmp226[1]=_tmp663;}));Cyc_PP_cat(_tag_dyneither(_tmp226,sizeof(struct Cyc_PP_Doc*),2U));});
# 1146
s=_tmp665;});
# 1148
goto _LL0;case 16U: _LL21: _tmp2A9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22:
# 1150
 if(_tmp2A9 == 0)
({struct Cyc_PP_Doc*_tmp668=({struct Cyc_PP_Doc*_tmp228[2U];(({struct Cyc_PP_Doc*_tmp667=Cyc_PP_text(({const char*_tmp229="new ";_tag_dyneither(_tmp229,sizeof(char),5U);}));_tmp228[0]=_tmp667;}),({struct Cyc_PP_Doc*_tmp666=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp228[1]=_tmp666;}));Cyc_PP_cat(_tag_dyneither(_tmp228,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp668;});else{
# 1153
({struct Cyc_PP_Doc*_tmp66D=({struct Cyc_PP_Doc*_tmp22A[4U];(((((({struct Cyc_PP_Doc*_tmp66C=Cyc_PP_text(({const char*_tmp22B="rnew(";_tag_dyneither(_tmp22B,sizeof(char),6U);}));_tmp22A[0]=_tmp66C;}),({struct Cyc_PP_Doc*_tmp66B=Cyc_Absynpp_exp2doc(_tmp2A9);_tmp22A[1]=_tmp66B;}))),({struct Cyc_PP_Doc*_tmp66A=Cyc_PP_text(({const char*_tmp22C=") ";_tag_dyneither(_tmp22C,sizeof(char),3U);}));_tmp22A[2]=_tmp66A;}))),({
struct Cyc_PP_Doc*_tmp669=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp22A[3]=_tmp669;}));Cyc_PP_cat(_tag_dyneither(_tmp22A,sizeof(struct Cyc_PP_Doc*),4U));});
# 1153
s=_tmp66D;});}
# 1155
goto _LL0;case 17U: _LL23: _tmp2AA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL24:
({struct Cyc_PP_Doc*_tmp671=({struct Cyc_PP_Doc*_tmp22D[3U];(((({struct Cyc_PP_Doc*_tmp670=Cyc_PP_text(({const char*_tmp22E="sizeof(";_tag_dyneither(_tmp22E,sizeof(char),8U);}));_tmp22D[0]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_Absynpp_typ2doc(_tmp2AA);_tmp22D[1]=_tmp66F;}))),({struct Cyc_PP_Doc*_tmp66E=Cyc_PP_text(({const char*_tmp22F=")";_tag_dyneither(_tmp22F,sizeof(char),2U);}));_tmp22D[2]=_tmp66E;}));Cyc_PP_cat(_tag_dyneither(_tmp22D,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp671;});goto _LL0;case 18U: _LL25: _tmp2AB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL26:
({struct Cyc_PP_Doc*_tmp675=({struct Cyc_PP_Doc*_tmp230[3U];(((({struct Cyc_PP_Doc*_tmp674=Cyc_PP_text(({const char*_tmp231="sizeof(";_tag_dyneither(_tmp231,sizeof(char),8U);}));_tmp230[0]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_Absynpp_exp2doc(_tmp2AB);_tmp230[1]=_tmp673;}))),({struct Cyc_PP_Doc*_tmp672=Cyc_PP_text(({const char*_tmp232=")";_tag_dyneither(_tmp232,sizeof(char),2U);}));_tmp230[2]=_tmp672;}));Cyc_PP_cat(_tag_dyneither(_tmp230,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp675;});goto _LL0;case 39U: _LL27: _tmp2AC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL28:
({struct Cyc_PP_Doc*_tmp679=({struct Cyc_PP_Doc*_tmp233[3U];(((({struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp234="valueof(";_tag_dyneither(_tmp234,sizeof(char),9U);}));_tmp233[0]=_tmp678;}),({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_typ2doc(_tmp2AC);_tmp233[1]=_tmp677;}))),({struct Cyc_PP_Doc*_tmp676=Cyc_PP_text(({const char*_tmp235=")";_tag_dyneither(_tmp235,sizeof(char),2U);}));_tmp233[2]=_tmp676;}));Cyc_PP_cat(_tag_dyneither(_tmp233,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp679;});goto _LL0;case 40U: _LL29: _tmp2AE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2A:
# 1160
 if(_tmp2AE)
({struct Cyc_PP_Doc*_tmp67E=({struct Cyc_PP_Doc*_tmp236[4U];(((((({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp237="__asm__";_tag_dyneither(_tmp237,sizeof(char),8U);}));_tmp236[0]=_tmp67D;}),({struct Cyc_PP_Doc*_tmp67C=Cyc_PP_text(({const char*_tmp238=" volatile (";_tag_dyneither(_tmp238,sizeof(char),12U);}));_tmp236[1]=_tmp67C;}))),({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(_tmp2AD);_tmp236[2]=_tmp67B;}))),({struct Cyc_PP_Doc*_tmp67A=Cyc_PP_text(({const char*_tmp239=")";_tag_dyneither(_tmp239,sizeof(char),2U);}));_tmp236[3]=_tmp67A;}));Cyc_PP_cat(_tag_dyneither(_tmp236,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp67E;});else{
# 1163
({struct Cyc_PP_Doc*_tmp682=({struct Cyc_PP_Doc*_tmp23A[3U];(((({struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp23B="__asm__(";_tag_dyneither(_tmp23B,sizeof(char),9U);}));_tmp23A[0]=_tmp681;}),({struct Cyc_PP_Doc*_tmp680=Cyc_PP_text(_tmp2AD);_tmp23A[1]=_tmp680;}))),({struct Cyc_PP_Doc*_tmp67F=Cyc_PP_text(({const char*_tmp23C=")";_tag_dyneither(_tmp23C,sizeof(char),2U);}));_tmp23A[2]=_tmp67F;}));Cyc_PP_cat(_tag_dyneither(_tmp23A,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp682;});}
goto _LL0;case 38U: _LL2B: _tmp2B0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2C:
# 1166
({struct Cyc_PP_Doc*_tmp688=({struct Cyc_PP_Doc*_tmp23D[5U];(((((((({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp23E="tagcheck(";_tag_dyneither(_tmp23E,sizeof(char),10U);}));_tmp23D[0]=_tmp687;}),({struct Cyc_PP_Doc*_tmp686=Cyc_Absynpp_exp2doc(_tmp2B0);_tmp23D[1]=_tmp686;}))),({struct Cyc_PP_Doc*_tmp685=Cyc_PP_text(({const char*_tmp23F=".";_tag_dyneither(_tmp23F,sizeof(char),2U);}));_tmp23D[2]=_tmp685;}))),({struct Cyc_PP_Doc*_tmp684=Cyc_PP_textptr(_tmp2AF);_tmp23D[3]=_tmp684;}))),({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp240=")";_tag_dyneither(_tmp240,sizeof(char),2U);}));_tmp23D[4]=_tmp683;}));Cyc_PP_cat(_tag_dyneither(_tmp23D,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp688;});
goto _LL0;case 19U: _LL2D: _tmp2B2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B1=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2E:
# 1169
({struct Cyc_PP_Doc*_tmp68C=({struct Cyc_PP_Doc*_tmp241[3U];(((({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp242="offsetof(";_tag_dyneither(_tmp242,sizeof(char),10U);}));_tmp241[0]=_tmp68B;}),({struct Cyc_PP_Doc*_tmp68A=Cyc_Absynpp_typ2doc(_tmp2B2);_tmp241[1]=_tmp68A;}))),({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp243=",";_tag_dyneither(_tmp243,sizeof(char),2U);}));_tmp241[2]=_tmp689;}));Cyc_PP_cat(_tag_dyneither(_tmp241,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp68C;});
for(0;_tmp2B1 != 0;_tmp2B1=_tmp2B1->tl){
void*_tmp244=(void*)_tmp2B1->hd;void*_tmp245=_tmp244;unsigned int _tmp24E;struct _dyneither_ptr*_tmp24D;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp245)->tag == 0U){_LL5D: _tmp24D=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp245)->f1;_LL5E:
({struct Cyc_PP_Doc*_tmp690=({struct Cyc_PP_Doc*_tmp246[3U];(((_tmp246[0]=s,({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_textptr(_tmp24D);_tmp246[1]=_tmp68F;}))),
_tmp2B1->tl != 0?({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_text(({const char*_tmp247=".";_tag_dyneither(_tmp247,sizeof(char),2U);}));_tmp246[2]=_tmp68E;}):({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_nil_doc();_tmp246[2]=_tmp68D;}));Cyc_PP_cat(_tag_dyneither(_tmp246,sizeof(struct Cyc_PP_Doc*),3U));});
# 1172
s=_tmp690;});
# 1174
goto _LL5C;}else{_LL5F: _tmp24E=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp245)->f1;_LL60:
({struct Cyc_PP_Doc*_tmp695=({struct Cyc_PP_Doc*_tmp248[3U];(((_tmp248[0]=s,({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp24B;_tmp24B.tag=1U;_tmp24B.f1=(unsigned long)((int)_tmp24E);({void*_tmp249[1U];_tmp249[0]=& _tmp24B;({struct _dyneither_ptr _tmp693=({const char*_tmp24A="%d";_tag_dyneither(_tmp24A,sizeof(char),3U);});Cyc_aprintf(_tmp693,_tag_dyneither(_tmp249,sizeof(void*),1U));});});}));_tmp248[1]=_tmp694;}))),
_tmp2B1->tl != 0?({struct Cyc_PP_Doc*_tmp692=Cyc_PP_text(({const char*_tmp24C=".";_tag_dyneither(_tmp24C,sizeof(char),2U);}));_tmp248[2]=_tmp692;}):({struct Cyc_PP_Doc*_tmp691=Cyc_PP_nil_doc();_tmp248[2]=_tmp691;}));Cyc_PP_cat(_tag_dyneither(_tmp248,sizeof(struct Cyc_PP_Doc*),3U));});
# 1175
s=_tmp695;});
# 1177
goto _LL5C;}_LL5C:;}
# 1179
({struct Cyc_PP_Doc*_tmp697=({struct Cyc_PP_Doc*_tmp24F[2U];(_tmp24F[0]=s,({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp250=")";_tag_dyneither(_tmp250,sizeof(char),2U);}));_tmp24F[1]=_tmp696;}));Cyc_PP_cat(_tag_dyneither(_tmp24F,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp697;});
goto _LL0;case 20U: _LL2F: _tmp2B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL30:
# 1182
({struct Cyc_PP_Doc*_tmp69A=({struct Cyc_PP_Doc*_tmp251[2U];(({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp252="*";_tag_dyneither(_tmp252,sizeof(char),2U);}));_tmp251[0]=_tmp699;}),({struct Cyc_PP_Doc*_tmp698=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3);_tmp251[1]=_tmp698;}));Cyc_PP_cat(_tag_dyneither(_tmp251,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp69A;});
goto _LL0;case 21U: _LL31: _tmp2B5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL32:
# 1185
({struct Cyc_PP_Doc*_tmp69E=({struct Cyc_PP_Doc*_tmp253[3U];(((({struct Cyc_PP_Doc*_tmp69D=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5);_tmp253[0]=_tmp69D;}),({struct Cyc_PP_Doc*_tmp69C=Cyc_PP_text(({const char*_tmp254=".";_tag_dyneither(_tmp254,sizeof(char),2U);}));_tmp253[1]=_tmp69C;}))),({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_textptr(_tmp2B4);_tmp253[2]=_tmp69B;}));Cyc_PP_cat(_tag_dyneither(_tmp253,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp69E;});
goto _LL0;case 22U: _LL33: _tmp2B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL34:
# 1188
({struct Cyc_PP_Doc*_tmp6A2=({struct Cyc_PP_Doc*_tmp255[3U];(((({struct Cyc_PP_Doc*_tmp6A1=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B7);_tmp255[0]=_tmp6A1;}),({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp256="->";_tag_dyneither(_tmp256,sizeof(char),3U);}));_tmp255[1]=_tmp6A0;}))),({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_textptr(_tmp2B6);_tmp255[2]=_tmp69F;}));Cyc_PP_cat(_tag_dyneither(_tmp255,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp6A2;});
goto _LL0;case 23U: _LL35: _tmp2B9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL36:
# 1191
({struct Cyc_PP_Doc*_tmp6A7=({struct Cyc_PP_Doc*_tmp257[4U];(((((({struct Cyc_PP_Doc*_tmp6A6=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B9);_tmp257[0]=_tmp6A6;}),({
struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_text(({const char*_tmp258="[";_tag_dyneither(_tmp258,sizeof(char),2U);}));_tmp257[1]=_tmp6A5;}))),({
struct Cyc_PP_Doc*_tmp6A4=Cyc_Absynpp_exp2doc(_tmp2B8);_tmp257[2]=_tmp6A4;}))),({
struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp259="]";_tag_dyneither(_tmp259,sizeof(char),2U);}));_tmp257[3]=_tmp6A3;}));Cyc_PP_cat(_tag_dyneither(_tmp257,sizeof(struct Cyc_PP_Doc*),4U));});
# 1191
s=_tmp6A7;});
# 1195
goto _LL0;case 24U: _LL37: _tmp2BA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL38:
# 1197
({struct Cyc_PP_Doc*_tmp6AC=({struct Cyc_PP_Doc*_tmp25A[4U];(((((({struct Cyc_PP_Doc*_tmp6AB=Cyc_Absynpp_dollar();_tmp25A[0]=_tmp6AB;}),({
struct Cyc_PP_Doc*_tmp6AA=Cyc_PP_text(({const char*_tmp25B="(";_tag_dyneither(_tmp25B,sizeof(char),2U);}));_tmp25A[1]=_tmp6AA;}))),({
struct Cyc_PP_Doc*_tmp6A9=Cyc_Absynpp_exps2doc_prec(20,_tmp2BA);_tmp25A[2]=_tmp6A9;}))),({
struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp25C=")";_tag_dyneither(_tmp25C,sizeof(char),2U);}));_tmp25A[3]=_tmp6A8;}));Cyc_PP_cat(_tag_dyneither(_tmp25A,sizeof(struct Cyc_PP_Doc*),4U));});
# 1197
s=_tmp6AC;});
# 1201
goto _LL0;case 25U: _LL39: _tmp2BC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL3A:
# 1203
({struct Cyc_PP_Doc*_tmp6B2=({struct Cyc_PP_Doc*_tmp25D[4U];(((((({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp25E="(";_tag_dyneither(_tmp25E,sizeof(char),2U);}));_tmp25D[0]=_tmp6B1;}),({
struct Cyc_PP_Doc*_tmp6B0=Cyc_Absynpp_typ2doc((*_tmp2BC).f3);_tmp25D[1]=_tmp6B0;}))),({
struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp25F=")";_tag_dyneither(_tmp25F,sizeof(char),2U);}));_tmp25D[2]=_tmp6AF;}))),({
struct Cyc_PP_Doc*_tmp6AE=({struct _dyneither_ptr _tmp6AD=({const char*_tmp260=",";_tag_dyneither(_tmp260,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6AD,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BB));});_tmp25D[3]=_tmp6AE;}));Cyc_PP_cat(_tag_dyneither(_tmp25D,sizeof(struct Cyc_PP_Doc*),4U));});
# 1203
s=_tmp6B2;});
# 1207
goto _LL0;case 26U: _LL3B: _tmp2BD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL3C:
# 1209
({struct Cyc_PP_Doc*_tmp6B4=({struct _dyneither_ptr _tmp6B3=({const char*_tmp261=",";_tag_dyneither(_tmp261,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6B3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BD));});s=_tmp6B4;});
goto _LL0;case 27U: _LL3D: _tmp2C0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2BE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL3E:
# 1212
({struct Cyc_PP_Doc*_tmp6BD=({struct Cyc_PP_Doc*_tmp262[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp6BC=Cyc_Absynpp_lb();_tmp262[0]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp263="for ";_tag_dyneither(_tmp263,sizeof(char),5U);}));_tmp262[1]=_tmp6BB;}))),({
struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(*(*_tmp2C0->name).f2);_tmp262[2]=_tmp6BA;}))),({
struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_text(({const char*_tmp264=" < ";_tag_dyneither(_tmp264,sizeof(char),4U);}));_tmp262[3]=_tmp6B9;}))),({
struct Cyc_PP_Doc*_tmp6B8=Cyc_Absynpp_exp2doc(_tmp2BF);_tmp262[4]=_tmp6B8;}))),({
struct Cyc_PP_Doc*_tmp6B7=Cyc_PP_text(({const char*_tmp265=" : ";_tag_dyneither(_tmp265,sizeof(char),4U);}));_tmp262[5]=_tmp6B7;}))),({
struct Cyc_PP_Doc*_tmp6B6=Cyc_Absynpp_exp2doc(_tmp2BE);_tmp262[6]=_tmp6B6;}))),({
struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_rb();_tmp262[7]=_tmp6B5;}));Cyc_PP_cat(_tag_dyneither(_tmp262,sizeof(struct Cyc_PP_Doc*),8U));});
# 1212
s=_tmp6BD;});
# 1219
goto _LL0;case 28U: _LL3F: _tmp2C2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C1=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL40:
# 1221
({struct Cyc_PP_Doc*_tmp6C5=({struct Cyc_PP_Doc*_tmp266[7U];(((((((((((({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_lb();_tmp266[0]=_tmp6C4;}),({struct Cyc_PP_Doc*_tmp6C3=Cyc_PP_text(({const char*_tmp267="for x ";_tag_dyneither(_tmp267,sizeof(char),7U);}));_tmp266[1]=_tmp6C3;}))),({
struct Cyc_PP_Doc*_tmp6C2=Cyc_PP_text(({const char*_tmp268=" < ";_tag_dyneither(_tmp268,sizeof(char),4U);}));_tmp266[2]=_tmp6C2;}))),({
struct Cyc_PP_Doc*_tmp6C1=Cyc_Absynpp_exp2doc(_tmp2C2);_tmp266[3]=_tmp6C1;}))),({
struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_text(({const char*_tmp269=" : ";_tag_dyneither(_tmp269,sizeof(char),4U);}));_tmp266[4]=_tmp6C0;}))),({
struct Cyc_PP_Doc*_tmp6BF=Cyc_Absynpp_typ2doc(_tmp2C1);_tmp266[5]=_tmp6BF;}))),({
struct Cyc_PP_Doc*_tmp6BE=Cyc_Absynpp_rb();_tmp266[6]=_tmp6BE;}));Cyc_PP_cat(_tag_dyneither(_tmp266,sizeof(struct Cyc_PP_Doc*),7U));});
# 1221
s=_tmp6C5;});
# 1227
goto _LL0;case 29U: _LL41: _tmp2C5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2C3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL42: {
# 1229
struct Cyc_List_List*_tmp26A=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C3);
({struct Cyc_PP_Doc*_tmp6CA=({struct Cyc_PP_Doc*_tmp26B[2U];(({struct Cyc_PP_Doc*_tmp6C9=Cyc_Absynpp_qvar2doc(_tmp2C5);_tmp26B[0]=_tmp6C9;}),({
struct Cyc_PP_Doc*_tmp6C8=({struct _dyneither_ptr _tmp6C7=({const char*_tmp26C=",";_tag_dyneither(_tmp26C,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6C7,
_tmp2C4 != 0?({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));(({struct Cyc_PP_Doc*_tmp6C6=Cyc_Absynpp_tps2doc(_tmp2C4);(*_tmp26D).hd=_tmp6C6;}),(*_tmp26D).tl=_tmp26A);_tmp26D;}): _tmp26A);});
# 1231
_tmp26B[1]=_tmp6C8;}));Cyc_PP_cat(_tag_dyneither(_tmp26B,sizeof(struct Cyc_PP_Doc*),2U));});
# 1230
s=_tmp6CA;});
# 1233
goto _LL0;}case 30U: _LL43: _tmp2C6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL44:
# 1235
({struct Cyc_PP_Doc*_tmp6CC=({struct _dyneither_ptr _tmp6CB=({const char*_tmp26E=",";_tag_dyneither(_tmp26E,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6CB,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C6));});s=_tmp6CC;});
goto _LL0;case 31U: _LL45: _tmp2C8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL46:
# 1238
 if(_tmp2C8 == 0)
# 1240
({struct Cyc_PP_Doc*_tmp6CD=Cyc_Absynpp_qvar2doc(_tmp2C7->name);s=_tmp6CD;});else{
# 1242
({struct Cyc_PP_Doc*_tmp6D3=({struct Cyc_PP_Doc*_tmp26F[2U];(({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_qvar2doc(_tmp2C7->name);_tmp26F[0]=_tmp6D2;}),({
struct Cyc_PP_Doc*_tmp6D1=({struct _dyneither_ptr _tmp6D0=({const char*_tmp270="(";_tag_dyneither(_tmp270,sizeof(char),2U);});struct _dyneither_ptr _tmp6CF=({const char*_tmp271=")";_tag_dyneither(_tmp271,sizeof(char),2U);});struct _dyneither_ptr _tmp6CE=({const char*_tmp272=",";_tag_dyneither(_tmp272,sizeof(char),2U);});Cyc_PP_egroup(_tmp6D0,_tmp6CF,_tmp6CE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C8));});_tmp26F[1]=_tmp6D1;}));Cyc_PP_cat(_tag_dyneither(_tmp26F,sizeof(struct Cyc_PP_Doc*),2U));});
# 1242
s=_tmp6D3;});}
# 1244
goto _LL0;case 32U: _LL47: _tmp2C9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL48:
# 1246
({struct Cyc_PP_Doc*_tmp6D4=Cyc_Absynpp_qvar2doc(_tmp2C9->name);s=_tmp6D4;});
goto _LL0;case 33U: _LL49: _tmp2CA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL4A:
# 1249
({struct Cyc_PP_Doc*_tmp6D5=Cyc_Absynpp_qvar2doc(_tmp2CA->name);s=_tmp6D5;});
goto _LL0;case 34U: _LL4B: _tmp2CF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp2CE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp2CD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp2CC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp2CB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL4C:
# 1254
 if(_tmp2CF){
# 1256
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2CD)),0U);
if(_tmp2CE == 0)
({struct Cyc_PP_Doc*_tmp6DB=({struct Cyc_PP_Doc*_tmp273[5U];(((((((({struct Cyc_PP_Doc*_tmp6DA=Cyc_PP_text(({const char*_tmp274="calloc(";_tag_dyneither(_tmp274,sizeof(char),8U);}));_tmp273[0]=_tmp6DA;}),({struct Cyc_PP_Doc*_tmp6D9=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp273[1]=_tmp6D9;}))),({struct Cyc_PP_Doc*_tmp6D8=Cyc_PP_text(({const char*_tmp275=",";_tag_dyneither(_tmp275,sizeof(char),2U);}));_tmp273[2]=_tmp6D8;}))),({struct Cyc_PP_Doc*_tmp6D7=Cyc_Absynpp_exp2doc(st);_tmp273[3]=_tmp6D7;}))),({struct Cyc_PP_Doc*_tmp6D6=Cyc_PP_text(({const char*_tmp276=")";_tag_dyneither(_tmp276,sizeof(char),2U);}));_tmp273[4]=_tmp6D6;}));Cyc_PP_cat(_tag_dyneither(_tmp273,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp6DB;});else{
# 1260
({struct Cyc_PP_Doc*_tmp6E3=({struct Cyc_PP_Doc*_tmp277[7U];(((((((((((({struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_text(({const char*_tmp278="rcalloc(";_tag_dyneither(_tmp278,sizeof(char),9U);}));_tmp277[0]=_tmp6E2;}),({struct Cyc_PP_Doc*_tmp6E1=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp277[1]=_tmp6E1;}))),({struct Cyc_PP_Doc*_tmp6E0=Cyc_PP_text(({const char*_tmp279=",";_tag_dyneither(_tmp279,sizeof(char),2U);}));_tmp277[2]=_tmp6E0;}))),({
struct Cyc_PP_Doc*_tmp6DF=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp277[3]=_tmp6DF;}))),({struct Cyc_PP_Doc*_tmp6DE=Cyc_PP_text(({const char*_tmp27A=",";_tag_dyneither(_tmp27A,sizeof(char),2U);}));_tmp277[4]=_tmp6DE;}))),({struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_exp2doc(st);_tmp277[5]=_tmp6DD;}))),({struct Cyc_PP_Doc*_tmp6DC=Cyc_PP_text(({const char*_tmp27B=")";_tag_dyneither(_tmp27B,sizeof(char),2U);}));_tmp277[6]=_tmp6DC;}));Cyc_PP_cat(_tag_dyneither(_tmp277,sizeof(struct Cyc_PP_Doc*),7U));});
# 1260
s=_tmp6E3;});}}else{
# 1263
struct Cyc_Absyn_Exp*new_e;
# 1265
if(_tmp2CD == 0)
new_e=_tmp2CC;else{
# 1268
({struct Cyc_Absyn_Exp*_tmp6E5=({struct Cyc_Absyn_Exp*_tmp6E4=Cyc_Absyn_sizeoftyp_exp(*_tmp2CD,0U);Cyc_Absyn_times_exp(_tmp6E4,_tmp2CC,0U);});new_e=_tmp6E5;});}
# 1270
if(_tmp2CE == 0)
({struct Cyc_PP_Doc*_tmp6E9=({struct Cyc_PP_Doc*_tmp27C[3U];(((({struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_text(({const char*_tmp27D="malloc(";_tag_dyneither(_tmp27D,sizeof(char),8U);}));_tmp27C[0]=_tmp6E8;}),({struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_exp2doc(new_e);_tmp27C[1]=_tmp6E7;}))),({struct Cyc_PP_Doc*_tmp6E6=Cyc_PP_text(({const char*_tmp27E=")";_tag_dyneither(_tmp27E,sizeof(char),2U);}));_tmp27C[2]=_tmp6E6;}));Cyc_PP_cat(_tag_dyneither(_tmp27C,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp6E9;});else{
# 1273
if(_tmp2CB)
({struct Cyc_PP_Doc*_tmp6EF=({struct Cyc_PP_Doc*_tmp27F[5U];(((((((({struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_text(({const char*_tmp280="rmalloc_inline(";_tag_dyneither(_tmp280,sizeof(char),16U);}));_tmp27F[0]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp27F[1]=_tmp6ED;}))),({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp281=",";_tag_dyneither(_tmp281,sizeof(char),2U);}));_tmp27F[2]=_tmp6EC;}))),({
struct Cyc_PP_Doc*_tmp6EB=Cyc_Absynpp_exp2doc(new_e);_tmp27F[3]=_tmp6EB;}))),({struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_text(({const char*_tmp282=")";_tag_dyneither(_tmp282,sizeof(char),2U);}));_tmp27F[4]=_tmp6EA;}));Cyc_PP_cat(_tag_dyneither(_tmp27F,sizeof(struct Cyc_PP_Doc*),5U));});
# 1274
s=_tmp6EF;});else{
# 1277
({struct Cyc_PP_Doc*_tmp6F5=({struct Cyc_PP_Doc*_tmp283[5U];(((((((({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp284="rmalloc(";_tag_dyneither(_tmp284,sizeof(char),9U);}));_tmp283[0]=_tmp6F4;}),({struct Cyc_PP_Doc*_tmp6F3=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp283[1]=_tmp6F3;}))),({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp285=",";_tag_dyneither(_tmp285,sizeof(char),2U);}));_tmp283[2]=_tmp6F2;}))),({
struct Cyc_PP_Doc*_tmp6F1=Cyc_Absynpp_exp2doc(new_e);_tmp283[3]=_tmp6F1;}))),({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_text(({const char*_tmp286=")";_tag_dyneither(_tmp286,sizeof(char),2U);}));_tmp283[4]=_tmp6F0;}));Cyc_PP_cat(_tag_dyneither(_tmp283,sizeof(struct Cyc_PP_Doc*),5U));});
# 1277
s=_tmp6F5;});}}}
# 1281
goto _LL0;case 35U: _LL4D: _tmp2D1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL4E:
# 1283
({struct Cyc_PP_Doc*_tmp6F9=({struct Cyc_PP_Doc*_tmp287[3U];(((({struct Cyc_PP_Doc*_tmp6F8=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D1);_tmp287[0]=_tmp6F8;}),({
struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp288=" :=: ";_tag_dyneither(_tmp288,sizeof(char),6U);}));_tmp287[1]=_tmp6F7;}))),({
struct Cyc_PP_Doc*_tmp6F6=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D0);_tmp287[2]=_tmp6F6;}));Cyc_PP_cat(_tag_dyneither(_tmp287,sizeof(struct Cyc_PP_Doc*),3U));});
# 1283
s=_tmp6F9;});
# 1286
goto _LL0;case 36U: _LL4F: _tmp2D3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL50:
# 1289
({struct Cyc_PP_Doc*_tmp6FB=({struct _dyneither_ptr _tmp6FA=({const char*_tmp289=",";_tag_dyneither(_tmp289,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6FA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D2));});s=_tmp6FB;});
goto _LL0;default: _LL51: _tmp2D4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL52:
# 1292
({struct Cyc_PP_Doc*_tmp703=({struct Cyc_PP_Doc*_tmp28A[7U];(((((((((((({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp28B="(";_tag_dyneither(_tmp28B,sizeof(char),2U);}));_tmp28A[0]=_tmp702;}),({struct Cyc_PP_Doc*_tmp701=Cyc_Absynpp_lb();_tmp28A[1]=_tmp701;}))),({struct Cyc_PP_Doc*_tmp700=Cyc_PP_blank_doc();_tmp28A[2]=_tmp700;}))),({
struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D4,1));_tmp28A[3]=_tmp6FF;}))),({
struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_blank_doc();_tmp28A[4]=_tmp6FE;}))),({struct Cyc_PP_Doc*_tmp6FD=Cyc_Absynpp_rb();_tmp28A[5]=_tmp6FD;}))),({struct Cyc_PP_Doc*_tmp6FC=Cyc_PP_text(({const char*_tmp28C=")";_tag_dyneither(_tmp28C,sizeof(char),2U);}));_tmp28A[6]=_tmp6FC;}));Cyc_PP_cat(_tag_dyneither(_tmp28A,sizeof(struct Cyc_PP_Doc*),7U));});
# 1292
s=_tmp703;});
# 1295
goto _LL0;}_LL0:;}
# 1297
if(inprec >= myprec)
({struct Cyc_PP_Doc*_tmp706=({struct Cyc_PP_Doc*_tmp2D5[3U];(((({struct Cyc_PP_Doc*_tmp705=Cyc_PP_text(({const char*_tmp2D6="(";_tag_dyneither(_tmp2D6,sizeof(char),2U);}));_tmp2D5[0]=_tmp705;}),_tmp2D5[1]=s)),({struct Cyc_PP_Doc*_tmp704=Cyc_PP_text(({const char*_tmp2D7=")";_tag_dyneither(_tmp2D7,sizeof(char),2U);}));_tmp2D5[2]=_tmp704;}));Cyc_PP_cat(_tag_dyneither(_tmp2D5,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp706;});
return s;}
# 1302
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2D8=d;struct _dyneither_ptr*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->tag == 0U){_LL1: _tmp2DE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->f1;_LL2:
 return({struct Cyc_PP_Doc*_tmp2D9[3U];(((({struct Cyc_PP_Doc*_tmp709=Cyc_PP_text(({const char*_tmp2DA=".[";_tag_dyneither(_tmp2DA,sizeof(char),3U);}));_tmp2D9[0]=_tmp709;}),({struct Cyc_PP_Doc*_tmp708=Cyc_Absynpp_exp2doc(_tmp2DE);_tmp2D9[1]=_tmp708;}))),({struct Cyc_PP_Doc*_tmp707=Cyc_PP_text(({const char*_tmp2DB="]";_tag_dyneither(_tmp2DB,sizeof(char),2U);}));_tmp2D9[2]=_tmp707;}));Cyc_PP_cat(_tag_dyneither(_tmp2D9,sizeof(struct Cyc_PP_Doc*),3U));});}else{_LL3: _tmp2DF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D8)->f1;_LL4:
 return({struct Cyc_PP_Doc*_tmp2DC[2U];(({struct Cyc_PP_Doc*_tmp70B=Cyc_PP_text(({const char*_tmp2DD=".";_tag_dyneither(_tmp2DD,sizeof(char),2U);}));_tmp2DC[0]=_tmp70B;}),({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_textptr(_tmp2DF);_tmp2DC[1]=_tmp70A;}));Cyc_PP_cat(_tag_dyneither(_tmp2DC,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}
# 1309
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2E0[2U];(({struct Cyc_PP_Doc*_tmp710=({struct _dyneither_ptr _tmp70F=({const char*_tmp2E1="";_tag_dyneither(_tmp2E1,sizeof(char),1U);});struct _dyneither_ptr _tmp70E=({const char*_tmp2E2="=";_tag_dyneither(_tmp2E2,sizeof(char),2U);});struct _dyneither_ptr _tmp70D=({const char*_tmp2E3="=";_tag_dyneither(_tmp2E3,sizeof(char),2U);});Cyc_PP_egroup(_tmp70F,_tmp70E,_tmp70D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2E0[0]=_tmp710;}),({
struct Cyc_PP_Doc*_tmp70C=Cyc_Absynpp_exp2doc((*de).f2);_tmp2E0[1]=_tmp70C;}));Cyc_PP_cat(_tag_dyneither(_tmp2E0,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1315
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _dyneither_ptr _tmp713=({const char*_tmp2E4="";_tag_dyneither(_tmp2E4,sizeof(char),1U);});struct _dyneither_ptr _tmp712=({const char*_tmp2E5="";_tag_dyneither(_tmp2E5,sizeof(char),1U);});struct _dyneither_ptr _tmp711=({const char*_tmp2E6=",";_tag_dyneither(_tmp2E6,sizeof(char),2U);});Cyc_PP_group(_tmp713,_tmp712,_tmp711,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1319
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp2FB=({unsigned int _tmp2FA=(unsigned int)28 + 1U;char*_tmp2F9=_cycalloc_atomic(_check_times(_tmp2FA,sizeof(char)));({{unsigned int _tmp528=(unsigned int)28;unsigned int i;for(i=0;i < _tmp528;++ i){_tmp2F9[i]='z';}_tmp2F9[_tmp528]=0;}0;});_tmp2F9;});_tag_dyneither(_tmp2FB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FB,(unsigned int)28 + 1U));});
({struct _dyneither_ptr _tmp2E7=_dyneither_ptr_plus(x,sizeof(char),27);char _tmp2E8=*((char*)_check_dyneither_subscript(_tmp2E7,sizeof(char),0U));char _tmp2E9='\000';if(_get_dyneither_size(_tmp2E7,sizeof(char))== 1U  && (_tmp2E8 == '\000'  && _tmp2E9 != '\000'))_throw_arraybounds();*((char*)_tmp2E7.curr)=_tmp2E9;});
({struct _dyneither_ptr _tmp2EA=_dyneither_ptr_plus(x,sizeof(char),26);char _tmp2EB=*((char*)_check_dyneither_subscript(_tmp2EA,sizeof(char),0U));char _tmp2EC='L';if(_get_dyneither_size(_tmp2EA,sizeof(char))== 1U  && (_tmp2EB == '\000'  && _tmp2EC != '\000'))_throw_arraybounds();*((char*)_tmp2EA.curr)=_tmp2EC;});
({struct _dyneither_ptr _tmp2ED=_dyneither_ptr_plus(x,sizeof(char),25);char _tmp2EE=*((char*)_check_dyneither_subscript(_tmp2ED,sizeof(char),0U));char _tmp2EF='L';if(_get_dyneither_size(_tmp2ED,sizeof(char))== 1U  && (_tmp2EE == '\000'  && _tmp2EF != '\000'))_throw_arraybounds();*((char*)_tmp2ED.curr)=_tmp2EF;});
({struct _dyneither_ptr _tmp2F0=_dyneither_ptr_plus(x,sizeof(char),24);char _tmp2F1=*((char*)_check_dyneither_subscript(_tmp2F0,sizeof(char),0U));char _tmp2F2='U';if(_get_dyneither_size(_tmp2F0,sizeof(char))== 1U  && (_tmp2F1 == '\000'  && _tmp2F2 != '\000'))_throw_arraybounds();*((char*)_tmp2F0.curr)=_tmp2F2;});
({struct _dyneither_ptr _tmp2F3=_dyneither_ptr_plus(x,sizeof(char),23);char _tmp2F4=*((char*)_check_dyneither_subscript(_tmp2F3,sizeof(char),0U));char _tmp2F5='0';if(_get_dyneither_size(_tmp2F3,sizeof(char))== 1U  && (_tmp2F4 == '\000'  && _tmp2F5 != '\000'))_throw_arraybounds();*((char*)_tmp2F3.curr)=_tmp2F5;});{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
({struct _dyneither_ptr _tmp2F6=_dyneither_ptr_plus(x,sizeof(char),index);char _tmp2F7=*((char*)_check_dyneither_subscript(_tmp2F6,sizeof(char),0U));char _tmp2F8=c;if(_get_dyneither_size(_tmp2F6,sizeof(char))== 1U  && (_tmp2F7 == '\000'  && _tmp2F8 != '\000'))_throw_arraybounds();*((char*)_tmp2F6.curr)=_tmp2F8;});
i=i / 10;
-- index;}
# 1333
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1337
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2FC=c;struct _dyneither_ptr _tmp31E;struct _dyneither_ptr _tmp31D;struct _dyneither_ptr _tmp31C;enum Cyc_Absyn_Sign _tmp31B;long long _tmp31A;enum Cyc_Absyn_Sign _tmp319;int _tmp318;enum Cyc_Absyn_Sign _tmp317;short _tmp316;struct _dyneither_ptr _tmp315;enum Cyc_Absyn_Sign _tmp314;char _tmp313;switch((_tmp2FC.String_c).tag){case 2U: _LL1: _tmp314=((_tmp2FC.Char_c).val).f1;_tmp313=((_tmp2FC.Char_c).val).f2;_LL2:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FF;_tmp2FF.tag=0U;({struct _dyneither_ptr _tmp714=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp313));_tmp2FF.f1=_tmp714;});({void*_tmp2FD[1U];_tmp2FD[0]=& _tmp2FF;({struct _dyneither_ptr _tmp715=({const char*_tmp2FE="'%s'";_tag_dyneither(_tmp2FE,sizeof(char),5U);});Cyc_aprintf(_tmp715,_tag_dyneither(_tmp2FD,sizeof(void*),1U));});});}));case 3U: _LL3: _tmp315=(_tmp2FC.Wchar_c).val;_LL4:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp302;_tmp302.tag=0U;_tmp302.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp315);({void*_tmp300[1U];_tmp300[0]=& _tmp302;({struct _dyneither_ptr _tmp716=({const char*_tmp301="L'%s'";_tag_dyneither(_tmp301,sizeof(char),6U);});Cyc_aprintf(_tmp716,_tag_dyneither(_tmp300,sizeof(void*),1U));});});}));case 4U: _LL5: _tmp317=((_tmp2FC.Short_c).val).f1;_tmp316=((_tmp2FC.Short_c).val).f2;_LL6:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp305;_tmp305.tag=1U;_tmp305.f1=(unsigned long)((int)_tmp316);({void*_tmp303[1U];_tmp303[0]=& _tmp305;({struct _dyneither_ptr _tmp717=({const char*_tmp304="%d";_tag_dyneither(_tmp304,sizeof(char),3U);});Cyc_aprintf(_tmp717,_tag_dyneither(_tmp303,sizeof(void*),1U));});});}));case 5U: _LL7: _tmp319=((_tmp2FC.Int_c).val).f1;_tmp318=((_tmp2FC.Int_c).val).f2;_LL8:
# 1343
 if(_tmp319 == Cyc_Absyn_Unsigned)
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp308;_tmp308.tag=1U;_tmp308.f1=(unsigned int)_tmp318;({void*_tmp306[1U];_tmp306[0]=& _tmp308;({struct _dyneither_ptr _tmp718=({const char*_tmp307="%uU";_tag_dyneither(_tmp307,sizeof(char),4U);});Cyc_aprintf(_tmp718,_tag_dyneither(_tmp306,sizeof(void*),1U));});});}));else{
# 1346
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30B;_tmp30B.tag=1U;_tmp30B.f1=(unsigned long)_tmp318;({void*_tmp309[1U];_tmp309[0]=& _tmp30B;({struct _dyneither_ptr _tmp719=({const char*_tmp30A="%d";_tag_dyneither(_tmp30A,sizeof(char),3U);});Cyc_aprintf(_tmp719,_tag_dyneither(_tmp309,sizeof(void*),1U));});});}));}case 6U: _LL9: _tmp31B=((_tmp2FC.LongLong_c).val).f1;_tmp31A=((_tmp2FC.LongLong_c).val).f2;_LLA:
# 1349
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp31A));case 7U: _LLB: _tmp31C=((_tmp2FC.Float_c).val).f1;_LLC:
 return Cyc_PP_text(_tmp31C);case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp30C="NULL";_tag_dyneither(_tmp30C,sizeof(char),5U);}));case 8U: _LLF: _tmp31D=(_tmp2FC.String_c).val;_LL10:
# 1353
 return({struct Cyc_PP_Doc*_tmp30D[3U];(((({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp30E="\"";_tag_dyneither(_tmp30E,sizeof(char),2U);}));_tmp30D[0]=_tmp71C;}),({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp31D));_tmp30D[1]=_tmp71B;}))),({struct Cyc_PP_Doc*_tmp71A=Cyc_PP_text(({const char*_tmp30F="\"";_tag_dyneither(_tmp30F,sizeof(char),2U);}));_tmp30D[2]=_tmp71A;}));Cyc_PP_cat(_tag_dyneither(_tmp30D,sizeof(struct Cyc_PP_Doc*),3U));});default: _LL11: _tmp31E=(_tmp2FC.Wstring_c).val;_LL12:
# 1355
 return({struct Cyc_PP_Doc*_tmp310[3U];(((({struct Cyc_PP_Doc*_tmp71F=Cyc_PP_text(({const char*_tmp311="L\"";_tag_dyneither(_tmp311,sizeof(char),3U);}));_tmp310[0]=_tmp71F;}),({struct Cyc_PP_Doc*_tmp71E=Cyc_PP_text(_tmp31E);_tmp310[1]=_tmp71E;}))),({struct Cyc_PP_Doc*_tmp71D=Cyc_PP_text(({const char*_tmp312="\"";_tag_dyneither(_tmp312,sizeof(char),2U);}));_tmp310[2]=_tmp71D;}));Cyc_PP_cat(_tag_dyneither(_tmp310,sizeof(struct Cyc_PP_Doc*),3U));});}_LL0:;}
# 1359
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp322=_cycalloc(sizeof(*_tmp322));((*_tmp322).tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp722=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp321;_tmp321.tag=0U;({struct _dyneither_ptr _tmp720=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp321.f1=_tmp720;});({void*_tmp31F[1U];_tmp31F[0]=& _tmp321;({struct _dyneither_ptr _tmp721=({const char*_tmp320="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp320,sizeof(char),47U);});Cyc_aprintf(_tmp721,_tag_dyneither(_tmp31F,sizeof(void*),1U));});});});(*_tmp322).f1=_tmp722;}));_tmp322;}));
# 1365
return({struct Cyc_PP_Doc*_tmp323[3U];(((({struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp324="numelts(";_tag_dyneither(_tmp324,sizeof(char),9U);}));_tmp323[0]=_tmp725;}),({struct Cyc_PP_Doc*_tmp724=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp323[1]=_tmp724;}))),({struct Cyc_PP_Doc*_tmp723=Cyc_PP_text(({const char*_tmp325=")";_tag_dyneither(_tmp325,sizeof(char),2U);}));_tmp323[2]=_tmp723;}));Cyc_PP_cat(_tag_dyneither(_tmp323,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1367
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp329=_cycalloc(sizeof(*_tmp329));((*_tmp329).tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp728=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp328;_tmp328.tag=0U;({struct _dyneither_ptr _tmp726=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp328.f1=_tmp726;});({void*_tmp326[1U];_tmp326[0]=& _tmp328;({struct _dyneither_ptr _tmp727=({const char*_tmp327="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp327,sizeof(char),38U);});Cyc_aprintf(_tmp727,_tag_dyneither(_tmp326,sizeof(void*),1U));});});});(*_tmp329).f1=_tmp728;}));_tmp329;}));else{
# 1371
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp32A[3U];(((_tmp32A[0]=ps,({struct Cyc_PP_Doc*_tmp729=Cyc_PP_text(({const char*_tmp32B=" ";_tag_dyneither(_tmp32B,sizeof(char),2U);}));_tmp32A[1]=_tmp729;}))),_tmp32A[2]=(struct Cyc_PP_Doc*)ds->hd);Cyc_PP_cat(_tag_dyneither(_tmp32A,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32F=_cycalloc(sizeof(*_tmp32F));((*_tmp32F).tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp72C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32E;_tmp32E.tag=0U;({struct _dyneither_ptr _tmp72A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp32E.f1=_tmp72A;});({void*_tmp32C[1U];_tmp32C[0]=& _tmp32E;({struct _dyneither_ptr _tmp72B=({const char*_tmp32D="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp32D,sizeof(char),47U);});Cyc_aprintf(_tmp72B,_tag_dyneither(_tmp32C,sizeof(void*),1U));});});});(*_tmp32F).f1=_tmp72C;}));_tmp32F;}));else{
# 1377
return({struct Cyc_PP_Doc*_tmp330[5U];(((((((_tmp330[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp72E=Cyc_PP_text(({const char*_tmp331=" ";_tag_dyneither(_tmp331,sizeof(char),2U);}));_tmp330[1]=_tmp72E;}))),_tmp330[2]=ps)),({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp332=" ";_tag_dyneither(_tmp332,sizeof(char),2U);}));_tmp330[3]=_tmp72D;}))),_tmp330[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd);Cyc_PP_cat(_tag_dyneither(_tmp330,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1381
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp333=p;switch(_tmp333){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp334="+";_tag_dyneither(_tmp334,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp335="*";_tag_dyneither(_tmp335,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp336="-";_tag_dyneither(_tmp336,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp337="/";_tag_dyneither(_tmp337,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp338="\\%";_tag_dyneither(_tmp338,sizeof(char),3U);}):({const char*_tmp339="%";_tag_dyneither(_tmp339,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp33A="==";_tag_dyneither(_tmp33A,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp33B="!=";_tag_dyneither(_tmp33B,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp33C=">";_tag_dyneither(_tmp33C,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp33D="<";_tag_dyneither(_tmp33D,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp33E=">=";_tag_dyneither(_tmp33E,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp33F="<=";_tag_dyneither(_tmp33F,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp340="!";_tag_dyneither(_tmp340,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp341="~";_tag_dyneither(_tmp341,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp342="&";_tag_dyneither(_tmp342,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp343="|";_tag_dyneither(_tmp343,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp344="^";_tag_dyneither(_tmp344,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp345="<<";_tag_dyneither(_tmp345,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp346=">>";_tag_dyneither(_tmp346,sizeof(char),3U);});case Cyc_Absyn_Bitarshift: _LL25: _LL26:
 return({const char*_tmp347=">>>";_tag_dyneither(_tmp347,sizeof(char),4U);});default: _LL27: _LL28:
 return({const char*_tmp348="numelts";_tag_dyneither(_tmp348,sizeof(char),8U);});}_LL0:;}
# 1406
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1410
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp349=s->r;void*_tmp34A=_tmp349;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34A)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1417
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp34B=st->r;void*_tmp34C=_tmp34B;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_List_List*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A3;struct _dyneither_ptr*_tmp3A2;struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Decl*_tmp3A0;struct Cyc_Absyn_Stmt*_tmp39F;struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Stmt*_tmp398;struct _dyneither_ptr*_tmp397;struct Cyc_Absyn_Exp*_tmp396;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Stmt*_tmp392;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Stmt*_tmp390;struct Cyc_Absyn_Stmt*_tmp38F;struct Cyc_Absyn_Exp*_tmp38E;switch(*((int*)_tmp34C)){case 0U: _LL1: _LL2:
({struct Cyc_PP_Doc*_tmp72F=Cyc_PP_text(({const char*_tmp34D=";";_tag_dyneither(_tmp34D,sizeof(char),2U);}));s=_tmp72F;});goto _LL0;case 1U: _LL3: _tmp38E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL4:
({struct Cyc_PP_Doc*_tmp732=({struct Cyc_PP_Doc*_tmp34E[2U];(({struct Cyc_PP_Doc*_tmp731=Cyc_Absynpp_exp2doc(_tmp38E);_tmp34E[0]=_tmp731;}),({struct Cyc_PP_Doc*_tmp730=Cyc_PP_text(({const char*_tmp34F=";";_tag_dyneither(_tmp34F,sizeof(char),2U);}));_tmp34E[1]=_tmp730;}));Cyc_PP_cat(_tag_dyneither(_tmp34E,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp732;});goto _LL0;case 2U: _LL5: _tmp390=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp38F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL6:
# 1423
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp390))
({struct Cyc_PP_Doc*_tmp748=({struct Cyc_PP_Doc*_tmp350[7U];(((((((((((({struct Cyc_PP_Doc*_tmp747=Cyc_Absynpp_lb();_tmp350[0]=_tmp747;}),({
struct Cyc_PP_Doc*_tmp746=Cyc_PP_blank_doc();_tmp350[1]=_tmp746;}))),({
struct Cyc_PP_Doc*_tmp745=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp390,0));_tmp350[2]=_tmp745;}))),({
struct Cyc_PP_Doc*_tmp744=Cyc_PP_line_doc();_tmp350[3]=_tmp744;}))),({
struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_rb();_tmp350[4]=_tmp743;}))),({
struct Cyc_PP_Doc*_tmp742=Cyc_PP_line_doc();_tmp350[5]=_tmp742;}))),
Cyc_Absynpp_is_declaration(_tmp38F)?
stmtexp?({
struct Cyc_PP_Doc*_tmp741=({struct Cyc_PP_Doc*_tmp351[7U];(((((((((((({struct Cyc_PP_Doc*_tmp740=Cyc_PP_text(({const char*_tmp352="(";_tag_dyneither(_tmp352,sizeof(char),2U);}));_tmp351[0]=_tmp740;}),({struct Cyc_PP_Doc*_tmp73F=Cyc_Absynpp_lb();_tmp351[1]=_tmp73F;}))),({struct Cyc_PP_Doc*_tmp73E=Cyc_PP_blank_doc();_tmp351[2]=_tmp73E;}))),({
struct Cyc_PP_Doc*_tmp73D=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp));_tmp351[3]=_tmp73D;}))),({
struct Cyc_PP_Doc*_tmp73C=Cyc_Absynpp_rb();_tmp351[4]=_tmp73C;}))),({struct Cyc_PP_Doc*_tmp73B=Cyc_PP_text(({const char*_tmp353=");";_tag_dyneither(_tmp353,sizeof(char),3U);}));_tmp351[5]=_tmp73B;}))),({
struct Cyc_PP_Doc*_tmp73A=Cyc_PP_line_doc();_tmp351[6]=_tmp73A;}));Cyc_PP_cat(_tag_dyneither(_tmp351,sizeof(struct Cyc_PP_Doc*),7U));});
# 1433
_tmp350[6]=_tmp741;}):({
# 1437
struct Cyc_PP_Doc*_tmp739=({struct Cyc_PP_Doc*_tmp354[5U];(((((((({struct Cyc_PP_Doc*_tmp738=Cyc_Absynpp_lb();_tmp354[0]=_tmp738;}),({struct Cyc_PP_Doc*_tmp737=Cyc_PP_blank_doc();_tmp354[1]=_tmp737;}))),({
struct Cyc_PP_Doc*_tmp736=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp));_tmp354[2]=_tmp736;}))),({
struct Cyc_PP_Doc*_tmp735=Cyc_Absynpp_rb();_tmp354[3]=_tmp735;}))),({
struct Cyc_PP_Doc*_tmp734=Cyc_PP_line_doc();_tmp354[4]=_tmp734;}));Cyc_PP_cat(_tag_dyneither(_tmp354,sizeof(struct Cyc_PP_Doc*),5U));});
# 1437
_tmp350[6]=_tmp739;}):({
# 1441
struct Cyc_PP_Doc*_tmp733=Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp);_tmp350[6]=_tmp733;}));Cyc_PP_cat(_tag_dyneither(_tmp350,sizeof(struct Cyc_PP_Doc*),7U));});
# 1425
s=_tmp748;});else{
# 1442
if(Cyc_Absynpp_is_declaration(_tmp38F))
({struct Cyc_PP_Doc*_tmp758=({struct Cyc_PP_Doc*_tmp355[4U];(((((({struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_stmt2doc(_tmp390,0);_tmp355[0]=_tmp757;}),({
struct Cyc_PP_Doc*_tmp756=Cyc_PP_line_doc();_tmp355[1]=_tmp756;}))),
stmtexp?({
struct Cyc_PP_Doc*_tmp755=({struct Cyc_PP_Doc*_tmp356[6U];(((((((((({struct Cyc_PP_Doc*_tmp754=Cyc_PP_text(({const char*_tmp357="(";_tag_dyneither(_tmp357,sizeof(char),2U);}));_tmp356[0]=_tmp754;}),({struct Cyc_PP_Doc*_tmp753=Cyc_Absynpp_lb();_tmp356[1]=_tmp753;}))),({struct Cyc_PP_Doc*_tmp752=Cyc_PP_blank_doc();_tmp356[2]=_tmp752;}))),({
struct Cyc_PP_Doc*_tmp751=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp));_tmp356[3]=_tmp751;}))),({
struct Cyc_PP_Doc*_tmp750=Cyc_Absynpp_rb();_tmp356[4]=_tmp750;}))),({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_text(({const char*_tmp358=");";_tag_dyneither(_tmp358,sizeof(char),3U);}));_tmp356[5]=_tmp74F;}));Cyc_PP_cat(_tag_dyneither(_tmp356,sizeof(struct Cyc_PP_Doc*),6U));});
# 1446
_tmp355[2]=_tmp755;}):({
# 1450
struct Cyc_PP_Doc*_tmp74E=({struct Cyc_PP_Doc*_tmp359[4U];(((((({struct Cyc_PP_Doc*_tmp74D=Cyc_Absynpp_lb();_tmp359[0]=_tmp74D;}),({struct Cyc_PP_Doc*_tmp74C=Cyc_PP_blank_doc();_tmp359[1]=_tmp74C;}))),({
struct Cyc_PP_Doc*_tmp74B=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp));_tmp359[2]=_tmp74B;}))),({
struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_rb();_tmp359[3]=_tmp74A;}));Cyc_PP_cat(_tag_dyneither(_tmp359,sizeof(struct Cyc_PP_Doc*),4U));});
# 1450
_tmp355[2]=_tmp74E;}))),({
# 1453
struct Cyc_PP_Doc*_tmp749=Cyc_PP_line_doc();_tmp355[3]=_tmp749;}));Cyc_PP_cat(_tag_dyneither(_tmp355,sizeof(struct Cyc_PP_Doc*),4U));});
# 1443
s=_tmp758;});else{
# 1455
({struct Cyc_PP_Doc*_tmp75C=({struct Cyc_PP_Doc*_tmp35A[3U];(((({struct Cyc_PP_Doc*_tmp75B=Cyc_Absynpp_stmt2doc(_tmp390,0);_tmp35A[0]=_tmp75B;}),({struct Cyc_PP_Doc*_tmp75A=Cyc_PP_line_doc();_tmp35A[1]=_tmp75A;}))),({struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp);_tmp35A[2]=_tmp759;}));Cyc_PP_cat(_tag_dyneither(_tmp35A,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp75C;});}}}else{
# 1458
({struct Cyc_PP_Doc*_tmp760=({struct Cyc_PP_Doc*_tmp35B[3U];(((({struct Cyc_PP_Doc*_tmp75F=Cyc_Absynpp_stmt2doc(_tmp390,0);_tmp35B[0]=_tmp75F;}),({struct Cyc_PP_Doc*_tmp75E=Cyc_PP_line_doc();_tmp35B[1]=_tmp75E;}))),({struct Cyc_PP_Doc*_tmp75D=Cyc_Absynpp_stmt2doc(_tmp38F,stmtexp);_tmp35B[2]=_tmp75D;}));Cyc_PP_cat(_tag_dyneither(_tmp35B,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp760;});}
goto _LL0;case 3U: _LL7: _tmp391=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL8:
# 1461
 if(_tmp391 == 0)
({struct Cyc_PP_Doc*_tmp761=Cyc_PP_text(({const char*_tmp35C="return;";_tag_dyneither(_tmp35C,sizeof(char),8U);}));s=_tmp761;});else{
# 1464
({struct Cyc_PP_Doc*_tmp766=({struct Cyc_PP_Doc*_tmp35D[3U];(((({struct Cyc_PP_Doc*_tmp765=Cyc_PP_text(({const char*_tmp35E="return ";_tag_dyneither(_tmp35E,sizeof(char),8U);}));_tmp35D[0]=_tmp765;}),
_tmp391 == 0?({struct Cyc_PP_Doc*_tmp764=Cyc_PP_nil_doc();_tmp35D[1]=_tmp764;}):({struct Cyc_PP_Doc*_tmp763=Cyc_Absynpp_exp2doc(_tmp391);_tmp35D[1]=_tmp763;}))),({
struct Cyc_PP_Doc*_tmp762=Cyc_PP_text(({const char*_tmp35F=";";_tag_dyneither(_tmp35F,sizeof(char),2U);}));_tmp35D[2]=_tmp762;}));Cyc_PP_cat(_tag_dyneither(_tmp35D,sizeof(struct Cyc_PP_Doc*),3U));});
# 1464
s=_tmp766;});}
# 1467
goto _LL0;case 4U: _LL9: _tmp394=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp393=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_tmp392=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3;_LLA: {
# 1469
int print_else;
{void*_tmp360=_tmp392->r;void*_tmp361=_tmp360;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp361)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1474
({struct Cyc_PP_Doc*_tmp77A=({struct Cyc_PP_Doc*_tmp362[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp779=Cyc_PP_text(({const char*_tmp363="if (";_tag_dyneither(_tmp363,sizeof(char),5U);}));_tmp362[0]=_tmp779;}),({
struct Cyc_PP_Doc*_tmp778=Cyc_Absynpp_exp2doc(_tmp394);_tmp362[1]=_tmp778;}))),({
struct Cyc_PP_Doc*_tmp777=Cyc_PP_text(({const char*_tmp364=") ";_tag_dyneither(_tmp364,sizeof(char),3U);}));_tmp362[2]=_tmp777;}))),({
struct Cyc_PP_Doc*_tmp776=Cyc_Absynpp_lb();_tmp362[3]=_tmp776;}))),({
struct Cyc_PP_Doc*_tmp775=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp365[2U];(({struct Cyc_PP_Doc*_tmp774=Cyc_PP_line_doc();_tmp365[0]=_tmp774;}),({struct Cyc_PP_Doc*_tmp773=Cyc_Absynpp_stmt2doc(_tmp393,0);_tmp365[1]=_tmp773;}));Cyc_PP_cat(_tag_dyneither(_tmp365,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp362[4]=_tmp775;}))),({
struct Cyc_PP_Doc*_tmp772=Cyc_PP_line_doc();_tmp362[5]=_tmp772;}))),({
struct Cyc_PP_Doc*_tmp771=Cyc_Absynpp_rb();_tmp362[6]=_tmp771;}))),
print_else?({
struct Cyc_PP_Doc*_tmp770=({struct Cyc_PP_Doc*_tmp366[6U];(((((((((({struct Cyc_PP_Doc*_tmp76F=Cyc_PP_line_doc();_tmp366[0]=_tmp76F;}),({
struct Cyc_PP_Doc*_tmp76E=Cyc_PP_text(({const char*_tmp367="else ";_tag_dyneither(_tmp367,sizeof(char),6U);}));_tmp366[1]=_tmp76E;}))),({
struct Cyc_PP_Doc*_tmp76D=Cyc_Absynpp_lb();_tmp366[2]=_tmp76D;}))),({
struct Cyc_PP_Doc*_tmp76C=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp368[2U];(({struct Cyc_PP_Doc*_tmp76B=Cyc_PP_line_doc();_tmp368[0]=_tmp76B;}),({struct Cyc_PP_Doc*_tmp76A=Cyc_Absynpp_stmt2doc(_tmp392,0);_tmp368[1]=_tmp76A;}));Cyc_PP_cat(_tag_dyneither(_tmp368,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp366[3]=_tmp76C;}))),({
struct Cyc_PP_Doc*_tmp769=Cyc_PP_line_doc();_tmp366[4]=_tmp769;}))),({
struct Cyc_PP_Doc*_tmp768=Cyc_Absynpp_rb();_tmp366[5]=_tmp768;}));Cyc_PP_cat(_tag_dyneither(_tmp366,sizeof(struct Cyc_PP_Doc*),6U));});
# 1482
_tmp362[7]=_tmp770;}):({
# 1488
struct Cyc_PP_Doc*_tmp767=Cyc_PP_nil_doc();_tmp362[7]=_tmp767;}));Cyc_PP_cat(_tag_dyneither(_tmp362,sizeof(struct Cyc_PP_Doc*),8U));});
# 1474
s=_tmp77A;});
# 1489
goto _LL0;}case 5U: _LLB: _tmp396=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1).f1;_tmp395=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LLC:
# 1491
({struct Cyc_PP_Doc*_tmp784=({struct Cyc_PP_Doc*_tmp369[7U];(((((((((((({struct Cyc_PP_Doc*_tmp783=Cyc_PP_text(({const char*_tmp36A="while (";_tag_dyneither(_tmp36A,sizeof(char),8U);}));_tmp369[0]=_tmp783;}),({
struct Cyc_PP_Doc*_tmp782=Cyc_Absynpp_exp2doc(_tmp396);_tmp369[1]=_tmp782;}))),({
struct Cyc_PP_Doc*_tmp781=Cyc_PP_text(({const char*_tmp36B=") ";_tag_dyneither(_tmp36B,sizeof(char),3U);}));_tmp369[2]=_tmp781;}))),({
struct Cyc_PP_Doc*_tmp780=Cyc_Absynpp_lb();_tmp369[3]=_tmp780;}))),({
struct Cyc_PP_Doc*_tmp77F=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36C[2U];(({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_line_doc();_tmp36C[0]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_Absynpp_stmt2doc(_tmp395,0);_tmp36C[1]=_tmp77D;}));Cyc_PP_cat(_tag_dyneither(_tmp36C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp369[4]=_tmp77F;}))),({
struct Cyc_PP_Doc*_tmp77C=Cyc_PP_line_doc();_tmp369[5]=_tmp77C;}))),({
struct Cyc_PP_Doc*_tmp77B=Cyc_Absynpp_rb();_tmp369[6]=_tmp77B;}));Cyc_PP_cat(_tag_dyneither(_tmp369,sizeof(struct Cyc_PP_Doc*),7U));});
# 1491
s=_tmp784;});
# 1498
goto _LL0;case 6U: _LLD: _LLE:
({struct Cyc_PP_Doc*_tmp785=Cyc_PP_text(({const char*_tmp36D="break;";_tag_dyneither(_tmp36D,sizeof(char),7U);}));s=_tmp785;});goto _LL0;case 7U: _LLF: _LL10:
({struct Cyc_PP_Doc*_tmp786=Cyc_PP_text(({const char*_tmp36E="continue;";_tag_dyneither(_tmp36E,sizeof(char),10U);}));s=_tmp786;});goto _LL0;case 8U: _LL11: _tmp397=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL12:
({struct Cyc_PP_Doc*_tmp788=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp371;_tmp371.tag=0U;_tmp371.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp397);({void*_tmp36F[1U];_tmp36F[0]=& _tmp371;({struct _dyneither_ptr _tmp787=({const char*_tmp370="goto %s;";_tag_dyneither(_tmp370,sizeof(char),9U);});Cyc_aprintf(_tmp787,_tag_dyneither(_tmp36F,sizeof(void*),1U));});});}));s=_tmp788;});goto _LL0;case 9U: _LL13: _tmp39B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp39A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_tmp399=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3).f1;_tmp398=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f4;_LL14:
# 1503
({struct Cyc_PP_Doc*_tmp796=({struct Cyc_PP_Doc*_tmp372[11U];(((((((((((((((((((({struct Cyc_PP_Doc*_tmp795=Cyc_PP_text(({const char*_tmp373="for(";_tag_dyneither(_tmp373,sizeof(char),5U);}));_tmp372[0]=_tmp795;}),({
struct Cyc_PP_Doc*_tmp794=Cyc_Absynpp_exp2doc(_tmp39B);_tmp372[1]=_tmp794;}))),({
struct Cyc_PP_Doc*_tmp793=Cyc_PP_text(({const char*_tmp374="; ";_tag_dyneither(_tmp374,sizeof(char),3U);}));_tmp372[2]=_tmp793;}))),({
struct Cyc_PP_Doc*_tmp792=Cyc_Absynpp_exp2doc(_tmp39A);_tmp372[3]=_tmp792;}))),({
struct Cyc_PP_Doc*_tmp791=Cyc_PP_text(({const char*_tmp375="; ";_tag_dyneither(_tmp375,sizeof(char),3U);}));_tmp372[4]=_tmp791;}))),({
struct Cyc_PP_Doc*_tmp790=Cyc_Absynpp_exp2doc(_tmp399);_tmp372[5]=_tmp790;}))),({
struct Cyc_PP_Doc*_tmp78F=Cyc_PP_text(({const char*_tmp376=") ";_tag_dyneither(_tmp376,sizeof(char),3U);}));_tmp372[6]=_tmp78F;}))),({
struct Cyc_PP_Doc*_tmp78E=Cyc_Absynpp_lb();_tmp372[7]=_tmp78E;}))),({
struct Cyc_PP_Doc*_tmp78D=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp377[2U];(({struct Cyc_PP_Doc*_tmp78C=Cyc_PP_line_doc();_tmp377[0]=_tmp78C;}),({struct Cyc_PP_Doc*_tmp78B=Cyc_Absynpp_stmt2doc(_tmp398,0);_tmp377[1]=_tmp78B;}));Cyc_PP_cat(_tag_dyneither(_tmp377,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp372[8]=_tmp78D;}))),({
struct Cyc_PP_Doc*_tmp78A=Cyc_PP_line_doc();_tmp372[9]=_tmp78A;}))),({
struct Cyc_PP_Doc*_tmp789=Cyc_Absynpp_rb();_tmp372[10]=_tmp789;}));Cyc_PP_cat(_tag_dyneither(_tmp372,sizeof(struct Cyc_PP_Doc*),11U));});
# 1503
s=_tmp796;});
# 1514
goto _LL0;case 10U: _LL15: _tmp39D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp39C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL16:
# 1516
({struct Cyc_PP_Doc*_tmp79F=({struct Cyc_PP_Doc*_tmp378[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp79E=Cyc_PP_text(({const char*_tmp379="switch (";_tag_dyneither(_tmp379,sizeof(char),9U);}));_tmp378[0]=_tmp79E;}),({
struct Cyc_PP_Doc*_tmp79D=Cyc_Absynpp_exp2doc(_tmp39D);_tmp378[1]=_tmp79D;}))),({
struct Cyc_PP_Doc*_tmp79C=Cyc_PP_text(({const char*_tmp37A=") ";_tag_dyneither(_tmp37A,sizeof(char),3U);}));_tmp378[2]=_tmp79C;}))),({
struct Cyc_PP_Doc*_tmp79B=Cyc_Absynpp_lb();_tmp378[3]=_tmp79B;}))),({
struct Cyc_PP_Doc*_tmp79A=Cyc_PP_line_doc();_tmp378[4]=_tmp79A;}))),({
struct Cyc_PP_Doc*_tmp799=Cyc_Absynpp_switchclauses2doc(_tmp39C);_tmp378[5]=_tmp799;}))),({
struct Cyc_PP_Doc*_tmp798=Cyc_PP_line_doc();_tmp378[6]=_tmp798;}))),({
struct Cyc_PP_Doc*_tmp797=Cyc_Absynpp_rb();_tmp378[7]=_tmp797;}));Cyc_PP_cat(_tag_dyneither(_tmp378,sizeof(struct Cyc_PP_Doc*),8U));});
# 1516
s=_tmp79F;});
# 1524
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1 == 0){_LL17: _LL18:
({struct Cyc_PP_Doc*_tmp7A0=Cyc_PP_text(({const char*_tmp37B="fallthru;";_tag_dyneither(_tmp37B,sizeof(char),10U);}));s=_tmp7A0;});goto _LL0;}else{_LL19: _tmp39E=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL1A:
# 1527
({struct Cyc_PP_Doc*_tmp7A4=({struct Cyc_PP_Doc*_tmp37C[3U];(((({struct Cyc_PP_Doc*_tmp7A3=Cyc_PP_text(({const char*_tmp37D="fallthru(";_tag_dyneither(_tmp37D,sizeof(char),10U);}));_tmp37C[0]=_tmp7A3;}),({struct Cyc_PP_Doc*_tmp7A2=Cyc_Absynpp_exps2doc_prec(20,_tmp39E);_tmp37C[1]=_tmp7A2;}))),({struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_text(({const char*_tmp37E=");";_tag_dyneither(_tmp37E,sizeof(char),3U);}));_tmp37C[2]=_tmp7A1;}));Cyc_PP_cat(_tag_dyneither(_tmp37C,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7A4;});goto _LL0;}case 12U: _LL1B: _tmp3A0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp39F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL1C:
# 1529
({struct Cyc_PP_Doc*_tmp7A8=({struct Cyc_PP_Doc*_tmp37F[3U];(((({struct Cyc_PP_Doc*_tmp7A7=Cyc_Absynpp_decl2doc(_tmp3A0);_tmp37F[0]=_tmp7A7;}),({
struct Cyc_PP_Doc*_tmp7A6=Cyc_PP_line_doc();_tmp37F[1]=_tmp7A6;}))),({
struct Cyc_PP_Doc*_tmp7A5=Cyc_Absynpp_stmt2doc(_tmp39F,stmtexp);_tmp37F[2]=_tmp7A5;}));Cyc_PP_cat(_tag_dyneither(_tmp37F,sizeof(struct Cyc_PP_Doc*),3U));});
# 1529
s=_tmp7A8;});
# 1532
goto _LL0;case 13U: _LL1D: _tmp3A2=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A1=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL1E:
# 1534
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3A1)){
if(stmtexp)
({struct Cyc_PP_Doc*_tmp7B1=({struct Cyc_PP_Doc*_tmp380[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp7B0=Cyc_PP_textptr(_tmp3A2);_tmp380[0]=_tmp7B0;}),({
struct Cyc_PP_Doc*_tmp7AF=Cyc_PP_text(({const char*_tmp381=": (";_tag_dyneither(_tmp381,sizeof(char),4U);}));_tmp380[1]=_tmp7AF;}))),({
struct Cyc_PP_Doc*_tmp7AE=Cyc_Absynpp_lb();_tmp380[2]=_tmp7AE;}))),({
struct Cyc_PP_Doc*_tmp7AD=Cyc_PP_line_doc();_tmp380[3]=_tmp7AD;}))),({
struct Cyc_PP_Doc*_tmp7AC=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A1,1));_tmp380[4]=_tmp7AC;}))),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_PP_line_doc();_tmp380[5]=_tmp7AB;}))),({
struct Cyc_PP_Doc*_tmp7AA=Cyc_Absynpp_rb();_tmp380[6]=_tmp7AA;}))),({struct Cyc_PP_Doc*_tmp7A9=Cyc_PP_text(({const char*_tmp382=");";_tag_dyneither(_tmp382,sizeof(char),3U);}));_tmp380[7]=_tmp7A9;}));Cyc_PP_cat(_tag_dyneither(_tmp380,sizeof(struct Cyc_PP_Doc*),8U));});
# 1536
s=_tmp7B1;});else{
# 1544
({struct Cyc_PP_Doc*_tmp7B9=({struct Cyc_PP_Doc*_tmp383[7U];(((((((((((({struct Cyc_PP_Doc*_tmp7B8=Cyc_PP_textptr(_tmp3A2);_tmp383[0]=_tmp7B8;}),({
struct Cyc_PP_Doc*_tmp7B7=Cyc_PP_text(({const char*_tmp384=": ";_tag_dyneither(_tmp384,sizeof(char),3U);}));_tmp383[1]=_tmp7B7;}))),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_Absynpp_lb();_tmp383[2]=_tmp7B6;}))),({
struct Cyc_PP_Doc*_tmp7B5=Cyc_PP_line_doc();_tmp383[3]=_tmp7B5;}))),({
struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A1,0));_tmp383[4]=_tmp7B4;}))),({
struct Cyc_PP_Doc*_tmp7B3=Cyc_PP_line_doc();_tmp383[5]=_tmp7B3;}))),({
struct Cyc_PP_Doc*_tmp7B2=Cyc_Absynpp_rb();_tmp383[6]=_tmp7B2;}));Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),7U));});
# 1544
s=_tmp7B9;});}}else{
# 1552
({struct Cyc_PP_Doc*_tmp7BD=({struct Cyc_PP_Doc*_tmp385[3U];(((({struct Cyc_PP_Doc*_tmp7BC=Cyc_PP_textptr(_tmp3A2);_tmp385[0]=_tmp7BC;}),({struct Cyc_PP_Doc*_tmp7BB=Cyc_PP_text(({const char*_tmp386=": ";_tag_dyneither(_tmp386,sizeof(char),3U);}));_tmp385[1]=_tmp7BB;}))),({struct Cyc_PP_Doc*_tmp7BA=Cyc_Absynpp_stmt2doc(_tmp3A1,stmtexp);_tmp385[2]=_tmp7BA;}));Cyc_PP_cat(_tag_dyneither(_tmp385,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7BD;});}
goto _LL0;case 14U: _LL1F: _tmp3A4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_LL20:
# 1555
({struct Cyc_PP_Doc*_tmp7C7=({struct Cyc_PP_Doc*_tmp387[9U];(((((((((((((((({struct Cyc_PP_Doc*_tmp7C6=Cyc_PP_text(({const char*_tmp388="do ";_tag_dyneither(_tmp388,sizeof(char),4U);}));_tmp387[0]=_tmp7C6;}),({
struct Cyc_PP_Doc*_tmp7C5=Cyc_Absynpp_lb();_tmp387[1]=_tmp7C5;}))),({
struct Cyc_PP_Doc*_tmp7C4=Cyc_PP_line_doc();_tmp387[2]=_tmp7C4;}))),({
struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A4,0));_tmp387[3]=_tmp7C3;}))),({
struct Cyc_PP_Doc*_tmp7C2=Cyc_PP_line_doc();_tmp387[4]=_tmp7C2;}))),({
struct Cyc_PP_Doc*_tmp7C1=Cyc_Absynpp_rb();_tmp387[5]=_tmp7C1;}))),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_PP_text(({const char*_tmp389=" while (";_tag_dyneither(_tmp389,sizeof(char),9U);}));_tmp387[6]=_tmp7C0;}))),({
struct Cyc_PP_Doc*_tmp7BF=Cyc_Absynpp_exp2doc(_tmp3A3);_tmp387[7]=_tmp7BF;}))),({
struct Cyc_PP_Doc*_tmp7BE=Cyc_PP_text(({const char*_tmp38A=");";_tag_dyneither(_tmp38A,sizeof(char),3U);}));_tmp387[8]=_tmp7BE;}));Cyc_PP_cat(_tag_dyneither(_tmp387,sizeof(struct Cyc_PP_Doc*),9U));});
# 1555
s=_tmp7C7;});
# 1564
goto _LL0;default: _LL21: _tmp3A6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL22:
# 1566
({struct Cyc_PP_Doc*_tmp7D4=({struct Cyc_PP_Doc*_tmp38B[12U];(((((((((((((((((((((({struct Cyc_PP_Doc*_tmp7D3=Cyc_PP_text(({const char*_tmp38C="try ";_tag_dyneither(_tmp38C,sizeof(char),5U);}));_tmp38B[0]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D2=Cyc_Absynpp_lb();_tmp38B[1]=_tmp7D2;}))),({
struct Cyc_PP_Doc*_tmp7D1=Cyc_PP_line_doc();_tmp38B[2]=_tmp7D1;}))),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,0));_tmp38B[3]=_tmp7D0;}))),({
struct Cyc_PP_Doc*_tmp7CF=Cyc_PP_line_doc();_tmp38B[4]=_tmp7CF;}))),({
struct Cyc_PP_Doc*_tmp7CE=Cyc_Absynpp_rb();_tmp38B[5]=_tmp7CE;}))),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_PP_text(({const char*_tmp38D=" catch ";_tag_dyneither(_tmp38D,sizeof(char),8U);}));_tmp38B[6]=_tmp7CD;}))),({
struct Cyc_PP_Doc*_tmp7CC=Cyc_Absynpp_lb();_tmp38B[7]=_tmp7CC;}))),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_line_doc();_tmp38B[8]=_tmp7CB;}))),({
struct Cyc_PP_Doc*_tmp7CA=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3A5));_tmp38B[9]=_tmp7CA;}))),({
struct Cyc_PP_Doc*_tmp7C9=Cyc_PP_line_doc();_tmp38B[10]=_tmp7C9;}))),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_Absynpp_rb();_tmp38B[11]=_tmp7C8;}));Cyc_PP_cat(_tag_dyneither(_tmp38B,sizeof(struct Cyc_PP_Doc*),12U));});
# 1566
s=_tmp7D4;});
# 1578
goto _LL0;}_LL0:;}
# 1580
return s;}
# 1583
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3A7=p->r;void*_tmp3A8=_tmp3A7;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Datatypefield*_tmp407;struct Cyc_List_List*_tmp406;int _tmp405;struct Cyc_Absyn_Datatypefield*_tmp404;struct Cyc_Absyn_Enumfield*_tmp403;struct Cyc_Absyn_Enumfield*_tmp402;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp400;int _tmp3FF;union Cyc_Absyn_AggrInfoU _tmp3FE;struct Cyc_List_List*_tmp3FD;struct Cyc_List_List*_tmp3FC;int _tmp3FB;struct _tuple0*_tmp3FA;struct Cyc_List_List*_tmp3F9;int _tmp3F8;struct _tuple0*_tmp3F7;struct Cyc_Absyn_Vardecl*_tmp3F6;struct Cyc_Absyn_Pat*_tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F4;struct Cyc_Absyn_Pat*_tmp3F3;struct Cyc_List_List*_tmp3F2;int _tmp3F1;struct Cyc_Absyn_Tvar*_tmp3F0;struct Cyc_Absyn_Vardecl*_tmp3EF;struct Cyc_Absyn_Tvar*_tmp3EE;struct Cyc_Absyn_Vardecl*_tmp3ED;struct Cyc_Absyn_Vardecl*_tmp3EC;struct Cyc_Absyn_Pat*_tmp3EB;struct Cyc_Absyn_Vardecl*_tmp3EA;struct _dyneither_ptr _tmp3E9;char _tmp3E8;enum Cyc_Absyn_Sign _tmp3E7;int _tmp3E6;switch(*((int*)_tmp3A8)){case 0U: _LL1: _LL2:
({struct Cyc_PP_Doc*_tmp7D5=Cyc_PP_text(({const char*_tmp3A9="_";_tag_dyneither(_tmp3A9,sizeof(char),2U);}));s=_tmp7D5;});goto _LL0;case 9U: _LL3: _LL4:
({struct Cyc_PP_Doc*_tmp7D6=Cyc_PP_text(({const char*_tmp3AA="NULL";_tag_dyneither(_tmp3AA,sizeof(char),5U);}));s=_tmp7D6;});goto _LL0;case 10U: _LL5: _tmp3E7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3E6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL6:
# 1589
 if(_tmp3E7 != Cyc_Absyn_Unsigned)
({struct Cyc_PP_Doc*_tmp7D8=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=1U;_tmp3AD.f1=(unsigned long)_tmp3E6;({void*_tmp3AB[1U];_tmp3AB[0]=& _tmp3AD;({struct _dyneither_ptr _tmp7D7=({const char*_tmp3AC="%d";_tag_dyneither(_tmp3AC,sizeof(char),3U);});Cyc_aprintf(_tmp7D7,_tag_dyneither(_tmp3AB,sizeof(void*),1U));});});}));s=_tmp7D8;});else{
({struct Cyc_PP_Doc*_tmp7DA=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=1U;_tmp3B0.f1=(unsigned int)_tmp3E6;({void*_tmp3AE[1U];_tmp3AE[0]=& _tmp3B0;({struct _dyneither_ptr _tmp7D9=({const char*_tmp3AF="%u";_tag_dyneither(_tmp3AF,sizeof(char),3U);});Cyc_aprintf(_tmp7D9,_tag_dyneither(_tmp3AE,sizeof(void*),1U));});});}));s=_tmp7DA;});}
goto _LL0;case 11U: _LL7: _tmp3E8=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LL8:
({struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U;({struct _dyneither_ptr _tmp7DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3E8));_tmp3B3.f1=_tmp7DB;});({void*_tmp3B1[1U];_tmp3B1[0]=& _tmp3B3;({struct _dyneither_ptr _tmp7DC=({const char*_tmp3B2="'%s'";_tag_dyneither(_tmp3B2,sizeof(char),5U);});Cyc_aprintf(_tmp7DC,_tag_dyneither(_tmp3B1,sizeof(void*),1U));});});}));s=_tmp7DD;});goto _LL0;case 12U: _LL9: _tmp3E9=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LLA:
({struct Cyc_PP_Doc*_tmp7DE=Cyc_PP_text(_tmp3E9);s=_tmp7DE;});goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2)->r)->tag == 0U){_LLB: _tmp3EA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LLC:
# 1596
({struct Cyc_PP_Doc*_tmp7DF=Cyc_Absynpp_qvar2doc(_tmp3EA->name);s=_tmp7DF;});goto _LL0;}else{_LLD: _tmp3EC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3EB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LLE:
# 1598
({struct Cyc_PP_Doc*_tmp7E3=({struct Cyc_PP_Doc*_tmp3B4[3U];(((({struct Cyc_PP_Doc*_tmp7E2=Cyc_Absynpp_qvar2doc(_tmp3EC->name);_tmp3B4[0]=_tmp7E2;}),({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_text(({const char*_tmp3B5=" as ";_tag_dyneither(_tmp3B5,sizeof(char),5U);}));_tmp3B4[1]=_tmp7E1;}))),({struct Cyc_PP_Doc*_tmp7E0=Cyc_Absynpp_pat2doc(_tmp3EB);_tmp3B4[2]=_tmp7E0;}));Cyc_PP_cat(_tag_dyneither(_tmp3B4,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7E3;});goto _LL0;}case 2U: _LLF: _tmp3EE=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3ED=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL10:
# 1600
({struct Cyc_PP_Doc*_tmp7E9=({struct Cyc_PP_Doc*_tmp3B6[5U];(((((((({struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp3B7="alias";_tag_dyneither(_tmp3B7,sizeof(char),6U);}));_tmp3B6[0]=_tmp7E8;}),({struct Cyc_PP_Doc*_tmp7E7=Cyc_PP_text(({const char*_tmp3B8=" <";_tag_dyneither(_tmp3B8,sizeof(char),3U);}));_tmp3B6[1]=_tmp7E7;}))),({struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_tvar2doc(_tmp3EE);_tmp3B6[2]=_tmp7E6;}))),({struct Cyc_PP_Doc*_tmp7E5=Cyc_PP_text(({const char*_tmp3B9="> ";_tag_dyneither(_tmp3B9,sizeof(char),3U);}));_tmp3B6[3]=_tmp7E5;}))),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_Absynpp_vardecl2doc(_tmp3ED);_tmp3B6[4]=_tmp7E4;}));Cyc_PP_cat(_tag_dyneither(_tmp3B6,sizeof(struct Cyc_PP_Doc*),5U));});
# 1600
s=_tmp7E9;});
# 1602
goto _LL0;case 4U: _LL11: _tmp3F0=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3EF=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL12:
# 1604
({struct Cyc_PP_Doc*_tmp7EE=({struct Cyc_PP_Doc*_tmp3BA[4U];(((((({struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_qvar2doc(_tmp3EF->name);_tmp3BA[0]=_tmp7ED;}),({struct Cyc_PP_Doc*_tmp7EC=Cyc_PP_text(({const char*_tmp3BB="<";_tag_dyneither(_tmp3BB,sizeof(char),2U);}));_tmp3BA[1]=_tmp7EC;}))),({struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_tvar2doc(_tmp3F0);_tmp3BA[2]=_tmp7EB;}))),({struct Cyc_PP_Doc*_tmp7EA=Cyc_PP_text(({const char*_tmp3BC=">";_tag_dyneither(_tmp3BC,sizeof(char),2U);}));_tmp3BA[3]=_tmp7EA;}));Cyc_PP_cat(_tag_dyneither(_tmp3BA,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp7EE;});
goto _LL0;case 5U: _LL13: _tmp3F2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3F1=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL14:
# 1607
({struct Cyc_PP_Doc*_tmp7F5=({struct Cyc_PP_Doc*_tmp3BD[4U];(((((({struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_dollar();_tmp3BD[0]=_tmp7F4;}),({struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_text(({const char*_tmp3BE="(";_tag_dyneither(_tmp3BE,sizeof(char),2U);}));_tmp3BD[1]=_tmp7F3;}))),({struct Cyc_PP_Doc*_tmp7F2=({struct _dyneither_ptr _tmp7F1=({const char*_tmp3BF=",";_tag_dyneither(_tmp3BF,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp7F1,_tmp3F2);});_tmp3BD[2]=_tmp7F2;}))),
_tmp3F1?({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp3C0=", ...)";_tag_dyneither(_tmp3C0,sizeof(char),7U);}));_tmp3BD[3]=_tmp7F0;}):({struct Cyc_PP_Doc*_tmp7EF=Cyc_PP_text(({const char*_tmp3C1=")";_tag_dyneither(_tmp3C1,sizeof(char),2U);}));_tmp3BD[3]=_tmp7EF;}));Cyc_PP_cat(_tag_dyneither(_tmp3BD,sizeof(struct Cyc_PP_Doc*),4U));});
# 1607
s=_tmp7F5;});
# 1609
goto _LL0;case 6U: _LL15: _tmp3F3=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LL16:
# 1611
({struct Cyc_PP_Doc*_tmp7F8=({struct Cyc_PP_Doc*_tmp3C2[2U];(({struct Cyc_PP_Doc*_tmp7F7=Cyc_PP_text(({const char*_tmp3C3="&";_tag_dyneither(_tmp3C3,sizeof(char),2U);}));_tmp3C2[0]=_tmp7F7;}),({struct Cyc_PP_Doc*_tmp7F6=Cyc_Absynpp_pat2doc(_tmp3F3);_tmp3C2[1]=_tmp7F6;}));Cyc_PP_cat(_tag_dyneither(_tmp3C2,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp7F8;});
goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2)->r)->tag == 0U){_LL17: _tmp3F4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LL18:
# 1614
({struct Cyc_PP_Doc*_tmp7FB=({struct Cyc_PP_Doc*_tmp3C4[2U];(({struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_text(({const char*_tmp3C5="*";_tag_dyneither(_tmp3C5,sizeof(char),2U);}));_tmp3C4[0]=_tmp7FA;}),({struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_qvar2doc(_tmp3F4->name);_tmp3C4[1]=_tmp7F9;}));Cyc_PP_cat(_tag_dyneither(_tmp3C4,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp7FB;});
goto _LL0;}else{_LL19: _tmp3F6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3F5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL1A:
# 1617
({struct Cyc_PP_Doc*_tmp800=({struct Cyc_PP_Doc*_tmp3C6[4U];(((((({struct Cyc_PP_Doc*_tmp7FF=Cyc_PP_text(({const char*_tmp3C7="*";_tag_dyneither(_tmp3C7,sizeof(char),2U);}));_tmp3C6[0]=_tmp7FF;}),({struct Cyc_PP_Doc*_tmp7FE=Cyc_Absynpp_qvar2doc(_tmp3F6->name);_tmp3C6[1]=_tmp7FE;}))),({struct Cyc_PP_Doc*_tmp7FD=Cyc_PP_text(({const char*_tmp3C8=" as ";_tag_dyneither(_tmp3C8,sizeof(char),5U);}));_tmp3C6[2]=_tmp7FD;}))),({struct Cyc_PP_Doc*_tmp7FC=Cyc_Absynpp_pat2doc(_tmp3F5);_tmp3C6[3]=_tmp7FC;}));Cyc_PP_cat(_tag_dyneither(_tmp3C6,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp800;});
goto _LL0;}case 15U: _LL1B: _tmp3F7=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LL1C:
# 1620
({struct Cyc_PP_Doc*_tmp801=Cyc_Absynpp_qvar2doc(_tmp3F7);s=_tmp801;});
goto _LL0;case 16U: _LL1D: _tmp3FA=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_tmp3F9=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_tmp3F8=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3A8)->f3;_LL1E: {
# 1623
struct _dyneither_ptr term=_tmp3F8?({const char*_tmp3CC=", ...)";_tag_dyneither(_tmp3CC,sizeof(char),7U);}):({const char*_tmp3CD=")";_tag_dyneither(_tmp3CD,sizeof(char),2U);});
({struct Cyc_PP_Doc*_tmp807=({struct Cyc_PP_Doc*_tmp3C9[2U];(({struct Cyc_PP_Doc*_tmp806=Cyc_Absynpp_qvar2doc(_tmp3FA);_tmp3C9[0]=_tmp806;}),({struct Cyc_PP_Doc*_tmp805=({struct _dyneither_ptr _tmp804=({const char*_tmp3CA="(";_tag_dyneither(_tmp3CA,sizeof(char),2U);});struct _dyneither_ptr _tmp803=term;struct _dyneither_ptr _tmp802=({const char*_tmp3CB=",";_tag_dyneither(_tmp3CB,sizeof(char),2U);});Cyc_PP_group(_tmp804,_tmp803,_tmp802,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3F9));});_tmp3C9[1]=_tmp805;}));Cyc_PP_cat(_tag_dyneither(_tmp3C9,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp807;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1 != 0){_LL1F: _tmp3FE=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1)->aggr_info;_tmp3FD=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_tmp3FC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f3;_tmp3FB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f4;_LL20: {
# 1627
struct _dyneither_ptr term=_tmp3FB?({const char*_tmp3D7=", ...}";_tag_dyneither(_tmp3D7,sizeof(char),7U);}):({const char*_tmp3D8="}";_tag_dyneither(_tmp3D8,sizeof(char),2U);});
struct _tuple10 _tmp3CE=Cyc_Absyn_aggr_kinded_name(_tmp3FE);struct _tuple10 _tmp3CF=_tmp3CE;struct _tuple0*_tmp3D6;_LL2E: _tmp3D6=_tmp3CF.f2;_LL2F:;
({struct Cyc_PP_Doc*_tmp812=({struct Cyc_PP_Doc*_tmp3D0[4U];(((((({struct Cyc_PP_Doc*_tmp811=Cyc_Absynpp_qvar2doc(_tmp3D6);_tmp3D0[0]=_tmp811;}),({struct Cyc_PP_Doc*_tmp810=Cyc_Absynpp_lb();_tmp3D0[1]=_tmp810;}))),({
struct Cyc_PP_Doc*_tmp80F=({struct _dyneither_ptr _tmp80E=({const char*_tmp3D1="[";_tag_dyneither(_tmp3D1,sizeof(char),2U);});struct _dyneither_ptr _tmp80D=({const char*_tmp3D2="]";_tag_dyneither(_tmp3D2,sizeof(char),2U);});struct _dyneither_ptr _tmp80C=({const char*_tmp3D3=",";_tag_dyneither(_tmp3D3,sizeof(char),2U);});Cyc_PP_egroup(_tmp80E,_tmp80D,_tmp80C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp3FD));});_tmp3D0[2]=_tmp80F;}))),({
struct Cyc_PP_Doc*_tmp80B=({struct _dyneither_ptr _tmp80A=({const char*_tmp3D4="";_tag_dyneither(_tmp3D4,sizeof(char),1U);});struct _dyneither_ptr _tmp809=term;struct _dyneither_ptr _tmp808=({const char*_tmp3D5=",";_tag_dyneither(_tmp3D5,sizeof(char),2U);});Cyc_PP_group(_tmp80A,_tmp809,_tmp808,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp3FC));});_tmp3D0[3]=_tmp80B;}));Cyc_PP_cat(_tag_dyneither(_tmp3D0,sizeof(struct Cyc_PP_Doc*),4U));});
# 1629
s=_tmp812;});
# 1632
goto _LL0;}}else{_LL21: _tmp401=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_tmp400=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f3;_tmp3FF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3A8)->f4;_LL22: {
# 1634
struct _dyneither_ptr term=_tmp3FF?({const char*_tmp3DF=", ...}";_tag_dyneither(_tmp3DF,sizeof(char),7U);}):({const char*_tmp3E0="}";_tag_dyneither(_tmp3E0,sizeof(char),2U);});
({struct Cyc_PP_Doc*_tmp81C=({struct Cyc_PP_Doc*_tmp3D9[3U];(((({struct Cyc_PP_Doc*_tmp81B=Cyc_Absynpp_lb();_tmp3D9[0]=_tmp81B;}),({
struct Cyc_PP_Doc*_tmp81A=({struct _dyneither_ptr _tmp819=({const char*_tmp3DA="[";_tag_dyneither(_tmp3DA,sizeof(char),2U);});struct _dyneither_ptr _tmp818=({const char*_tmp3DB="]";_tag_dyneither(_tmp3DB,sizeof(char),2U);});struct _dyneither_ptr _tmp817=({const char*_tmp3DC=",";_tag_dyneither(_tmp3DC,sizeof(char),2U);});Cyc_PP_egroup(_tmp819,_tmp818,_tmp817,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp401));});_tmp3D9[1]=_tmp81A;}))),({
struct Cyc_PP_Doc*_tmp816=({struct _dyneither_ptr _tmp815=({const char*_tmp3DD="";_tag_dyneither(_tmp3DD,sizeof(char),1U);});struct _dyneither_ptr _tmp814=term;struct _dyneither_ptr _tmp813=({const char*_tmp3DE=",";_tag_dyneither(_tmp3DE,sizeof(char),2U);});Cyc_PP_group(_tmp815,_tmp814,_tmp813,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp400));});_tmp3D9[2]=_tmp816;}));Cyc_PP_cat(_tag_dyneither(_tmp3D9,sizeof(struct Cyc_PP_Doc*),3U));});
# 1635
s=_tmp81C;});
# 1638
goto _LL0;}}case 13U: _LL23: _tmp402=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL24:
 _tmp403=_tmp402;goto _LL26;case 14U: _LL25: _tmp403=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL26:
({struct Cyc_PP_Doc*_tmp81D=Cyc_Absynpp_qvar2doc(_tmp403->name);s=_tmp81D;});goto _LL0;case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3A8)->f3 == 0){_LL27: _tmp404=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_LL28:
({struct Cyc_PP_Doc*_tmp81E=Cyc_Absynpp_qvar2doc(_tmp404->name);s=_tmp81E;});goto _LL0;}else{_LL29: _tmp407=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3A8)->f2;_tmp406=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3A8)->f3;_tmp405=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3A8)->f4;_LL2A: {
# 1643
struct _dyneither_ptr term=_tmp405?({const char*_tmp3E4=", ...)";_tag_dyneither(_tmp3E4,sizeof(char),7U);}):({const char*_tmp3E5=")";_tag_dyneither(_tmp3E5,sizeof(char),2U);});
({struct Cyc_PP_Doc*_tmp824=({struct Cyc_PP_Doc*_tmp3E1[2U];(({struct Cyc_PP_Doc*_tmp823=Cyc_Absynpp_qvar2doc(_tmp407->name);_tmp3E1[0]=_tmp823;}),({struct Cyc_PP_Doc*_tmp822=({struct _dyneither_ptr _tmp821=({const char*_tmp3E2="(";_tag_dyneither(_tmp3E2,sizeof(char),2U);});struct _dyneither_ptr _tmp820=term;struct _dyneither_ptr _tmp81F=({const char*_tmp3E3=",";_tag_dyneither(_tmp3E3,sizeof(char),2U);});Cyc_PP_egroup(_tmp821,_tmp820,_tmp81F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp406));});_tmp3E1[1]=_tmp822;}));Cyc_PP_cat(_tag_dyneither(_tmp3E1,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp824;});
goto _LL0;}}default: _LL2B: _tmp408=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3A8)->f1;_LL2C:
# 1647
({struct Cyc_PP_Doc*_tmp825=Cyc_Absynpp_exp2doc(_tmp408);s=_tmp825;});goto _LL0;}_LL0:;}
# 1649
return s;}
# 1652
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp409[2U];(({struct Cyc_PP_Doc*_tmp82A=({struct _dyneither_ptr _tmp829=({const char*_tmp40A="";_tag_dyneither(_tmp40A,sizeof(char),1U);});struct _dyneither_ptr _tmp828=({const char*_tmp40B="=";_tag_dyneither(_tmp40B,sizeof(char),2U);});struct _dyneither_ptr _tmp827=({const char*_tmp40C="=";_tag_dyneither(_tmp40C,sizeof(char),2U);});Cyc_PP_egroup(_tmp829,_tmp828,_tmp827,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp409[0]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_Absynpp_pat2doc((*dp).f2);_tmp409[1]=_tmp826;}));Cyc_PP_cat(_tag_dyneither(_tmp409,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp40D[2U];(({struct Cyc_PP_Doc*_tmp82E=Cyc_PP_text(({const char*_tmp40E="default: ";_tag_dyneither(_tmp40E,sizeof(char),10U);}));_tmp40D[0]=_tmp82E;}),({
struct Cyc_PP_Doc*_tmp82D=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp40F[2U];(({struct Cyc_PP_Doc*_tmp82C=Cyc_PP_line_doc();_tmp40F[0]=_tmp82C;}),({struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_stmt2doc(c->body,0);_tmp40F[1]=_tmp82B;}));Cyc_PP_cat(_tag_dyneither(_tmp40F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp40D[1]=_tmp82D;}));Cyc_PP_cat(_tag_dyneither(_tmp40D,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp410[4U];(((((({struct Cyc_PP_Doc*_tmp834=Cyc_PP_text(({const char*_tmp411="case ";_tag_dyneither(_tmp411,sizeof(char),6U);}));_tmp410[0]=_tmp834;}),({
struct Cyc_PP_Doc*_tmp833=Cyc_Absynpp_pat2doc(c->pattern);_tmp410[1]=_tmp833;}))),({
struct Cyc_PP_Doc*_tmp832=Cyc_PP_text(({const char*_tmp412=": ";_tag_dyneither(_tmp412,sizeof(char),3U);}));_tmp410[2]=_tmp832;}))),({
struct Cyc_PP_Doc*_tmp831=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp413[2U];(({struct Cyc_PP_Doc*_tmp830=Cyc_PP_line_doc();_tmp413[0]=_tmp830;}),({struct Cyc_PP_Doc*_tmp82F=Cyc_Absynpp_stmt2doc(c->body,0);_tmp413[1]=_tmp82F;}));Cyc_PP_cat(_tag_dyneither(_tmp413,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp410[3]=_tmp831;}));Cyc_PP_cat(_tag_dyneither(_tmp410,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1667
return({struct Cyc_PP_Doc*_tmp414[6U];(((((((((({struct Cyc_PP_Doc*_tmp83C=Cyc_PP_text(({const char*_tmp415="case ";_tag_dyneither(_tmp415,sizeof(char),6U);}));_tmp414[0]=_tmp83C;}),({
struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_pat2doc(c->pattern);_tmp414[1]=_tmp83B;}))),({
struct Cyc_PP_Doc*_tmp83A=Cyc_PP_text(({const char*_tmp416=" && ";_tag_dyneither(_tmp416,sizeof(char),5U);}));_tmp414[2]=_tmp83A;}))),({
struct Cyc_PP_Doc*_tmp839=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp414[3]=_tmp839;}))),({
struct Cyc_PP_Doc*_tmp838=Cyc_PP_text(({const char*_tmp417=": ";_tag_dyneither(_tmp417,sizeof(char),3U);}));_tmp414[4]=_tmp838;}))),({
struct Cyc_PP_Doc*_tmp837=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp418[2U];(({struct Cyc_PP_Doc*_tmp836=Cyc_PP_line_doc();_tmp418[0]=_tmp836;}),({struct Cyc_PP_Doc*_tmp835=Cyc_Absynpp_stmt2doc(c->body,0);_tmp418[1]=_tmp835;}));Cyc_PP_cat(_tag_dyneither(_tmp418,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp414[5]=_tmp837;}));Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1675
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _dyneither_ptr _tmp83D=({const char*_tmp419="";_tag_dyneither(_tmp419,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp83D,cs);});}
# 1679
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1683
return({struct Cyc_PP_Doc*_tmp41A[3U];(((({struct Cyc_PP_Doc*_tmp840=Cyc_Absynpp_qvar2doc(f->name);_tmp41A[0]=_tmp840;}),({struct Cyc_PP_Doc*_tmp83F=Cyc_PP_text(({const char*_tmp41B=" = ";_tag_dyneither(_tmp41B,sizeof(char),4U);}));_tmp41A[1]=_tmp83F;}))),({struct Cyc_PP_Doc*_tmp83E=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41A[2]=_tmp83E;}));Cyc_PP_cat(_tag_dyneither(_tmp41A,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1686
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _dyneither_ptr _tmp841=({const char*_tmp41C=",";_tag_dyneither(_tmp41C,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp841,fs);});}
# 1690
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1694
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _dyneither_ptr _tmp842=({const char*_tmp41D=",";_tag_dyneither(_tmp41D,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp842,vds);});}
# 1698
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp41E=vd;enum Cyc_Absyn_Scope _tmp430;struct _tuple0*_tmp42F;unsigned int _tmp42E;struct Cyc_Absyn_Tqual _tmp42D;void*_tmp42C;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_List_List*_tmp42A;_LL1: _tmp430=_tmp41E->sc;_tmp42F=_tmp41E->name;_tmp42E=_tmp41E->varloc;_tmp42D=_tmp41E->tq;_tmp42C=_tmp41E->type;_tmp42B=_tmp41E->initializer;_tmp42A=_tmp41E->attributes;_LL2:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp42F);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp42A);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp41F=Cyc_Cyclone_c_compiler;if(_tmp41F == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1707
{void*_tmp420=Cyc_Tcutil_compress(_tmp42C);void*_tmp421=_tmp420;struct Cyc_List_List*_tmp422;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp421)->tag == 9U){_LL9: _tmp422=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp421)->f1).attributes;_LLA:
# 1709
({struct Cyc_PP_Doc*_tmp843=Cyc_Absynpp_callconv2doc(_tmp422);beforenamedoc=_tmp843;});
goto _LL8;}else{_LLB: _LLC:
({struct Cyc_PP_Doc*_tmp844=Cyc_PP_nil_doc();beforenamedoc=_tmp844;});goto _LL8;}_LL8:;}
# 1713
goto _LL3;}_LL3:;}{
# 1716
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp423=Cyc_Cyclone_c_compiler;if(_tmp423 == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
({struct Cyc_PP_Doc*_tmp845=Cyc_PP_nil_doc();tmp_doc=_tmp845;});goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1721
({struct Cyc_PP_Doc*_tmp850=({struct Cyc_PP_Doc*_tmp424[5U];(((((((_tmp424[0]=tmp_doc,({
# 1723
struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_scope2doc(_tmp430);_tmp424[1]=_tmp84F;}))),({
struct Cyc_PP_Doc*_tmp84E=({struct Cyc_Absyn_Tqual _tmp84D=_tmp42D;void*_tmp84C=_tmp42C;Cyc_Absynpp_tqtd2doc(_tmp84D,_tmp84C,({struct Cyc_Core_Opt*_tmp426=_cycalloc(sizeof(*_tmp426));({struct Cyc_PP_Doc*_tmp84B=({struct Cyc_PP_Doc*_tmp425[2U];(_tmp425[0]=beforenamedoc,_tmp425[1]=sn);Cyc_PP_cat(_tag_dyneither(_tmp425,sizeof(struct Cyc_PP_Doc*),2U));});(*_tmp426).v=_tmp84B;});_tmp426;}));});_tmp424[2]=_tmp84E;}))),
_tmp42B == 0?({
struct Cyc_PP_Doc*_tmp84A=Cyc_PP_nil_doc();_tmp424[3]=_tmp84A;}):({
struct Cyc_PP_Doc*_tmp849=({struct Cyc_PP_Doc*_tmp427[2U];(({struct Cyc_PP_Doc*_tmp848=Cyc_PP_text(({const char*_tmp428=" = ";_tag_dyneither(_tmp428,sizeof(char),4U);}));_tmp427[0]=_tmp848;}),({struct Cyc_PP_Doc*_tmp847=Cyc_Absynpp_exp2doc(_tmp42B);_tmp427[1]=_tmp847;}));Cyc_PP_cat(_tag_dyneither(_tmp427,sizeof(struct Cyc_PP_Doc*),2U));});_tmp424[3]=_tmp849;}))),({
struct Cyc_PP_Doc*_tmp846=Cyc_PP_text(({const char*_tmp429=";";_tag_dyneither(_tmp429,sizeof(char),2U);}));_tmp424[4]=_tmp846;}));Cyc_PP_cat(_tag_dyneither(_tmp424,sizeof(struct Cyc_PP_Doc*),5U));});
# 1721
s=_tmp850;});
# 1729
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1732
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp431=*x;struct _tuple17 _tmp432=_tmp431;struct _tuple0*_tmp433;_LL1: _tmp433=_tmp432.f2;_LL2:;
return Cyc_Absynpp_qvar2doc(_tmp433);}
# 1737
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp434[4U];(((((({struct Cyc_PP_Doc*_tmp854=Cyc_Absynpp_scope2doc(ad->sc);_tmp434[0]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp434[1]=_tmp853;}))),({
struct Cyc_PP_Doc*_tmp852=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp434[2]=_tmp852;}))),({
struct Cyc_PP_Doc*_tmp851=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp434[3]=_tmp851;}));Cyc_PP_cat(_tag_dyneither(_tmp434,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1744
return({struct Cyc_PP_Doc*_tmp435[12U];(((((((((((((((((((((({struct Cyc_PP_Doc*_tmp865=Cyc_Absynpp_scope2doc(ad->sc);_tmp435[0]=_tmp865;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp435[1]=_tmp864;}))),({
struct Cyc_PP_Doc*_tmp863=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp435[2]=_tmp863;}))),({
struct Cyc_PP_Doc*_tmp862=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp435[3]=_tmp862;}))),({
struct Cyc_PP_Doc*_tmp861=Cyc_PP_blank_doc();_tmp435[4]=_tmp861;}))),({struct Cyc_PP_Doc*_tmp860=Cyc_Absynpp_lb();_tmp435[5]=_tmp860;}))),({
struct Cyc_PP_Doc*_tmp85F=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp435[6]=_tmp85F;}))),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp85E=Cyc_PP_nil_doc();_tmp435[7]=_tmp85E;}):({
struct Cyc_PP_Doc*_tmp85D=({struct Cyc_PP_Doc*_tmp436[2U];(({struct Cyc_PP_Doc*_tmp85C=Cyc_PP_text(({const char*_tmp437=":";_tag_dyneither(_tmp437,sizeof(char),2U);}));_tmp436[0]=_tmp85C;}),({struct Cyc_PP_Doc*_tmp85B=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp436[1]=_tmp85B;}));Cyc_PP_cat(_tag_dyneither(_tmp436,sizeof(struct Cyc_PP_Doc*),2U));});_tmp435[7]=_tmp85D;}))),({
struct Cyc_PP_Doc*_tmp85A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp438[2U];(({struct Cyc_PP_Doc*_tmp859=Cyc_PP_line_doc();_tmp438[0]=_tmp859;}),({struct Cyc_PP_Doc*_tmp858=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp438[1]=_tmp858;}));Cyc_PP_cat(_tag_dyneither(_tmp438,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp435[8]=_tmp85A;}))),({
struct Cyc_PP_Doc*_tmp857=Cyc_PP_line_doc();_tmp435[9]=_tmp857;}))),({
struct Cyc_PP_Doc*_tmp856=Cyc_Absynpp_rb();_tmp435[10]=_tmp856;}))),({
struct Cyc_PP_Doc*_tmp855=Cyc_Absynpp_atts2doc(ad->attributes);_tmp435[11]=_tmp855;}));Cyc_PP_cat(_tag_dyneither(_tmp435,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1758
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp439=dd;enum Cyc_Absyn_Scope _tmp445;struct _tuple0*_tmp444;struct Cyc_List_List*_tmp443;struct Cyc_Core_Opt*_tmp442;int _tmp441;_LL1: _tmp445=_tmp439->sc;_tmp444=_tmp439->name;_tmp443=_tmp439->tvs;_tmp442=_tmp439->fields;_tmp441=_tmp439->is_extensible;_LL2:;
if(_tmp442 == 0)
return({struct Cyc_PP_Doc*_tmp43A[5U];(((((((({struct Cyc_PP_Doc*_tmp86C=Cyc_Absynpp_scope2doc(_tmp445);_tmp43A[0]=_tmp86C;}),
_tmp441?({struct Cyc_PP_Doc*_tmp86B=Cyc_PP_text(({const char*_tmp43B="@extensible ";_tag_dyneither(_tmp43B,sizeof(char),13U);}));_tmp43A[1]=_tmp86B;}):({struct Cyc_PP_Doc*_tmp86A=Cyc_PP_blank_doc();_tmp43A[1]=_tmp86A;}))),({
struct Cyc_PP_Doc*_tmp869=Cyc_PP_text(({const char*_tmp43C="datatype ";_tag_dyneither(_tmp43C,sizeof(char),10U);}));_tmp43A[2]=_tmp869;}))),
_tmp441?({struct Cyc_PP_Doc*_tmp868=Cyc_Absynpp_qvar2bolddoc(_tmp444);_tmp43A[3]=_tmp868;}):({struct Cyc_PP_Doc*_tmp867=Cyc_Absynpp_typedef_name2bolddoc(_tmp444);_tmp43A[3]=_tmp867;}))),({
struct Cyc_PP_Doc*_tmp866=Cyc_Absynpp_ktvars2doc(_tmp443);_tmp43A[4]=_tmp866;}));Cyc_PP_cat(_tag_dyneither(_tmp43A,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1767
return({struct Cyc_PP_Doc*_tmp43D[10U];(((((((((((((((((({struct Cyc_PP_Doc*_tmp87A=Cyc_Absynpp_scope2doc(_tmp445);_tmp43D[0]=_tmp87A;}),
_tmp441?({struct Cyc_PP_Doc*_tmp879=Cyc_PP_text(({const char*_tmp43E="@extensible ";_tag_dyneither(_tmp43E,sizeof(char),13U);}));_tmp43D[1]=_tmp879;}):({struct Cyc_PP_Doc*_tmp878=Cyc_PP_blank_doc();_tmp43D[1]=_tmp878;}))),({
struct Cyc_PP_Doc*_tmp877=Cyc_PP_text(({const char*_tmp43F="datatype ";_tag_dyneither(_tmp43F,sizeof(char),10U);}));_tmp43D[2]=_tmp877;}))),
_tmp441?({struct Cyc_PP_Doc*_tmp876=Cyc_Absynpp_qvar2bolddoc(_tmp444);_tmp43D[3]=_tmp876;}):({struct Cyc_PP_Doc*_tmp875=Cyc_Absynpp_typedef_name2bolddoc(_tmp444);_tmp43D[3]=_tmp875;}))),({
struct Cyc_PP_Doc*_tmp874=Cyc_Absynpp_ktvars2doc(_tmp443);_tmp43D[4]=_tmp874;}))),({
struct Cyc_PP_Doc*_tmp873=Cyc_PP_blank_doc();_tmp43D[5]=_tmp873;}))),({struct Cyc_PP_Doc*_tmp872=Cyc_Absynpp_lb();_tmp43D[6]=_tmp872;}))),({
struct Cyc_PP_Doc*_tmp871=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp440[2U];(({struct Cyc_PP_Doc*_tmp870=Cyc_PP_line_doc();_tmp440[0]=_tmp870;}),({struct Cyc_PP_Doc*_tmp86F=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp442->v);_tmp440[1]=_tmp86F;}));Cyc_PP_cat(_tag_dyneither(_tmp440,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp43D[7]=_tmp871;}))),({
struct Cyc_PP_Doc*_tmp86E=Cyc_PP_line_doc();_tmp43D[8]=_tmp86E;}))),({
struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_rb();_tmp43D[9]=_tmp86D;}));Cyc_PP_cat(_tag_dyneither(_tmp43D,sizeof(struct Cyc_PP_Doc*),10U));});}}
# 1778
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp446=ed;enum Cyc_Absyn_Scope _tmp44E;struct _tuple0*_tmp44D;struct Cyc_Core_Opt*_tmp44C;_LL1: _tmp44E=_tmp446->sc;_tmp44D=_tmp446->name;_tmp44C=_tmp446->fields;_LL2:;
if(_tmp44C == 0)
return({struct Cyc_PP_Doc*_tmp447[3U];(((({struct Cyc_PP_Doc*_tmp87D=Cyc_Absynpp_scope2doc(_tmp44E);_tmp447[0]=_tmp87D;}),({
struct Cyc_PP_Doc*_tmp87C=Cyc_PP_text(({const char*_tmp448="enum ";_tag_dyneither(_tmp448,sizeof(char),6U);}));_tmp447[1]=_tmp87C;}))),({
struct Cyc_PP_Doc*_tmp87B=Cyc_Absynpp_typedef_name2bolddoc(_tmp44D);_tmp447[2]=_tmp87B;}));Cyc_PP_cat(_tag_dyneither(_tmp447,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1786
return({struct Cyc_PP_Doc*_tmp449[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp887=Cyc_Absynpp_scope2doc(_tmp44E);_tmp449[0]=_tmp887;}),({
struct Cyc_PP_Doc*_tmp886=Cyc_PP_text(({const char*_tmp44A="enum ";_tag_dyneither(_tmp44A,sizeof(char),6U);}));_tmp449[1]=_tmp886;}))),({
struct Cyc_PP_Doc*_tmp885=Cyc_Absynpp_qvar2bolddoc(_tmp44D);_tmp449[2]=_tmp885;}))),({
struct Cyc_PP_Doc*_tmp884=Cyc_PP_blank_doc();_tmp449[3]=_tmp884;}))),({struct Cyc_PP_Doc*_tmp883=Cyc_Absynpp_lb();_tmp449[4]=_tmp883;}))),({
struct Cyc_PP_Doc*_tmp882=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp44B[2U];(({struct Cyc_PP_Doc*_tmp881=Cyc_PP_line_doc();_tmp44B[0]=_tmp881;}),({struct Cyc_PP_Doc*_tmp880=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp44C->v);_tmp44B[1]=_tmp880;}));Cyc_PP_cat(_tag_dyneither(_tmp44B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp449[5]=_tmp882;}))),({
struct Cyc_PP_Doc*_tmp87F=Cyc_PP_line_doc();_tmp449[6]=_tmp87F;}))),({
struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_rb();_tmp449[7]=_tmp87E;}));Cyc_PP_cat(_tag_dyneither(_tmp449,sizeof(struct Cyc_PP_Doc*),8U));});}}
# 1795
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp44F=d->r;void*_tmp450=_tmp44F;struct Cyc_List_List*_tmp4AE;struct Cyc_List_List*_tmp4AD;struct Cyc_List_List*_tmp4AC;struct _tuple0*_tmp4AB;struct Cyc_List_List*_tmp4AA;struct _dyneither_ptr*_tmp4A9;struct Cyc_List_List*_tmp4A8;struct Cyc_Absyn_Typedefdecl*_tmp4A7;struct Cyc_Absyn_Enumdecl*_tmp4A6;struct Cyc_List_List*_tmp4A5;struct Cyc_Absyn_Pat*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Datatypedecl*_tmp4A2;struct Cyc_Absyn_Tvar*_tmp4A1;struct Cyc_Absyn_Vardecl*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Vardecl*_tmp49E;struct Cyc_Absyn_Aggrdecl*_tmp49D;struct Cyc_Absyn_Fndecl*_tmp49C;switch(*((int*)_tmp450)){case 1U: _LL1: _tmp49C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LL2: {
# 1799
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));((*_tmp45C).tag=9U,((((((((((((((((((((((((((*_tmp45C).f1).tvars=_tmp49C->tvs,((*_tmp45C).f1).effect=_tmp49C->effect)),((*_tmp45C).f1).ret_tqual=_tmp49C->ret_tqual)),((*_tmp45C).f1).ret_typ=_tmp49C->ret_type)),({
# 1803
struct Cyc_List_List*_tmp888=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp49C->args);((*_tmp45C).f1).args=_tmp888;}))),((*_tmp45C).f1).c_varargs=_tmp49C->c_varargs)),((*_tmp45C).f1).cyc_varargs=_tmp49C->cyc_varargs)),((*_tmp45C).f1).rgn_po=_tmp49C->rgn_po)),((*_tmp45C).f1).attributes=0)),((*_tmp45C).f1).requires_clause=_tmp49C->requires_clause)),((*_tmp45C).f1).requires_relns=_tmp49C->requires_relns)),((*_tmp45C).f1).ensures_clause=_tmp49C->ensures_clause)),((*_tmp45C).f1).ensures_relns=_tmp49C->ensures_relns)));_tmp45C;});
# 1809
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp49C->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp49C->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp451=Cyc_Cyclone_c_compiler;if(_tmp451 == Cyc_Cyclone_Gcc_c){_LL20: _LL21:
({struct Cyc_PP_Doc*_tmp889=Cyc_PP_text(({const char*_tmp452="inline ";_tag_dyneither(_tmp452,sizeof(char),8U);}));inlinedoc=_tmp889;});goto _LL1F;}else{_LL22: _LL23:
({struct Cyc_PP_Doc*_tmp88A=Cyc_PP_text(({const char*_tmp453="__inline ";_tag_dyneither(_tmp453,sizeof(char),10U);}));inlinedoc=_tmp88A;});goto _LL1F;}_LL1F:;}else{
# 1817
({struct Cyc_PP_Doc*_tmp88B=Cyc_PP_nil_doc();inlinedoc=_tmp88B;});}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp49C->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp454=Cyc_Cyclone_c_compiler;if(_tmp454 == Cyc_Cyclone_Gcc_c){_LL25: _LL26:
 beforenamedoc=attsdoc;goto _LL24;}else{_LL27: _LL28:
({struct Cyc_PP_Doc*_tmp88C=Cyc_Absynpp_callconv2doc(_tmp49C->attributes);beforenamedoc=_tmp88C;});goto _LL24;}_LL24:;}{
# 1824
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp49C->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp88F=Cyc_Absyn_empty_tqual(0U);void*_tmp88E=t;Cyc_Absynpp_tqtd2doc(_tmp88F,_tmp88E,({struct Cyc_Core_Opt*_tmp45B=_cycalloc(sizeof(*_tmp45B));({
struct Cyc_PP_Doc*_tmp88D=({struct Cyc_PP_Doc*_tmp45A[2U];(_tmp45A[0]=beforenamedoc,_tmp45A[1]=namedoc);Cyc_PP_cat(_tag_dyneither(_tmp45A,sizeof(struct Cyc_PP_Doc*),2U));});(*_tmp45B).v=_tmp88D;});_tmp45B;}));});
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp458[5U];(((((((({struct Cyc_PP_Doc*_tmp896=Cyc_PP_blank_doc();_tmp458[0]=_tmp896;}),({struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_lb();_tmp458[1]=_tmp895;}))),({
struct Cyc_PP_Doc*_tmp894=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp459[2U];(({struct Cyc_PP_Doc*_tmp893=Cyc_PP_line_doc();_tmp459[0]=_tmp893;}),({struct Cyc_PP_Doc*_tmp892=Cyc_Absynpp_stmt2doc(_tmp49C->body,0);_tmp459[1]=_tmp892;}));Cyc_PP_cat(_tag_dyneither(_tmp459,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp458[2]=_tmp894;}))),({
struct Cyc_PP_Doc*_tmp891=Cyc_PP_line_doc();_tmp458[3]=_tmp891;}))),({
struct Cyc_PP_Doc*_tmp890=Cyc_Absynpp_rb();_tmp458[4]=_tmp890;}));Cyc_PP_cat(_tag_dyneither(_tmp458,sizeof(struct Cyc_PP_Doc*),5U));});
({struct Cyc_PP_Doc*_tmp897=({struct Cyc_PP_Doc*_tmp455[4U];(((((_tmp455[0]=inlinedoc,_tmp455[1]=scopedoc)),_tmp455[2]=tqtddoc)),_tmp455[3]=bodydoc);Cyc_PP_cat(_tag_dyneither(_tmp455,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp897;});
# 1833
{enum Cyc_Cyclone_C_Compilers _tmp456=Cyc_Cyclone_c_compiler;if(_tmp456 == Cyc_Cyclone_Vc_c){_LL2A: _LL2B:
({struct Cyc_PP_Doc*_tmp898=({struct Cyc_PP_Doc*_tmp457[2U];(_tmp457[0]=attsdoc,_tmp457[1]=s);Cyc_PP_cat(_tag_dyneither(_tmp457,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp898;});goto _LL29;}else{_LL2C: _LL2D:
 goto _LL29;}_LL29:;}
# 1838
goto _LL0;};};}case 5U: _LL3: _tmp49D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LL4:
# 1841
({struct Cyc_PP_Doc*_tmp89B=({struct Cyc_PP_Doc*_tmp45D[2U];(({struct Cyc_PP_Doc*_tmp89A=Cyc_Absynpp_aggrdecl2doc(_tmp49D);_tmp45D[0]=_tmp89A;}),({struct Cyc_PP_Doc*_tmp899=Cyc_PP_text(({const char*_tmp45E=";";_tag_dyneither(_tmp45E,sizeof(char),2U);}));_tmp45D[1]=_tmp899;}));Cyc_PP_cat(_tag_dyneither(_tmp45D,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp89B;});
goto _LL0;case 0U: _LL5: _tmp49E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LL6:
# 1844
({struct Cyc_PP_Doc*_tmp89C=Cyc_Absynpp_vardecl2doc(_tmp49E);s=_tmp89C;});
goto _LL0;case 4U: _LL7: _tmp4A1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_tmp4A0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp450)->f2;_tmp49F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp450)->f3;_LL8:
# 1847
({struct Cyc_PP_Doc*_tmp8A8=({struct Cyc_PP_Doc*_tmp45F[7U];(((((((((((({struct Cyc_PP_Doc*_tmp8A7=Cyc_PP_text(({const char*_tmp460="region";_tag_dyneither(_tmp460,sizeof(char),7U);}));_tmp45F[0]=_tmp8A7;}),({
struct Cyc_PP_Doc*_tmp8A6=Cyc_PP_text(({const char*_tmp461="<";_tag_dyneither(_tmp461,sizeof(char),2U);}));_tmp45F[1]=_tmp8A6;}))),({
struct Cyc_PP_Doc*_tmp8A5=Cyc_Absynpp_tvar2doc(_tmp4A1);_tmp45F[2]=_tmp8A5;}))),({
struct Cyc_PP_Doc*_tmp8A4=Cyc_PP_text(({const char*_tmp462=">";_tag_dyneither(_tmp462,sizeof(char),2U);}));_tmp45F[3]=_tmp8A4;}))),({
struct Cyc_PP_Doc*_tmp8A3=Cyc_Absynpp_qvar2doc(_tmp4A0->name);_tmp45F[4]=_tmp8A3;}))),
(unsigned int)_tmp49F?({struct Cyc_PP_Doc*_tmp8A2=({struct Cyc_PP_Doc*_tmp463[3U];(((({struct Cyc_PP_Doc*_tmp8A1=Cyc_PP_text(({const char*_tmp464=" = open(";_tag_dyneither(_tmp464,sizeof(char),9U);}));_tmp463[0]=_tmp8A1;}),({struct Cyc_PP_Doc*_tmp8A0=Cyc_Absynpp_exp2doc(_tmp49F);_tmp463[1]=_tmp8A0;}))),({
struct Cyc_PP_Doc*_tmp89F=Cyc_PP_text(({const char*_tmp465=")";_tag_dyneither(_tmp465,sizeof(char),2U);}));_tmp463[2]=_tmp89F;}));Cyc_PP_cat(_tag_dyneither(_tmp463,sizeof(struct Cyc_PP_Doc*),3U));});
# 1852
_tmp45F[5]=_tmp8A2;}):({
struct Cyc_PP_Doc*_tmp89E=Cyc_PP_nil_doc();_tmp45F[5]=_tmp89E;}))),({
struct Cyc_PP_Doc*_tmp89D=Cyc_PP_text(({const char*_tmp466=";";_tag_dyneither(_tmp466,sizeof(char),2U);}));_tmp45F[6]=_tmp89D;}));Cyc_PP_cat(_tag_dyneither(_tmp45F,sizeof(struct Cyc_PP_Doc*),7U));});
# 1847
s=_tmp8A8;});
# 1855
goto _LL0;case 6U: _LL9: _tmp4A2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LLA:
# 1857
({struct Cyc_PP_Doc*_tmp8AB=({struct Cyc_PP_Doc*_tmp467[2U];(({struct Cyc_PP_Doc*_tmp8AA=Cyc_Absynpp_datatypedecl2doc(_tmp4A2);_tmp467[0]=_tmp8AA;}),({struct Cyc_PP_Doc*_tmp8A9=Cyc_PP_text(({const char*_tmp468=";";_tag_dyneither(_tmp468,sizeof(char),2U);}));_tmp467[1]=_tmp8A9;}));Cyc_PP_cat(_tag_dyneither(_tmp467,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp8AB;});
goto _LL0;case 2U: _LLB: _tmp4A4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_tmp4A3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp450)->f3;_LLC:
# 1860
({struct Cyc_PP_Doc*_tmp8B1=({struct Cyc_PP_Doc*_tmp469[5U];(((((((({struct Cyc_PP_Doc*_tmp8B0=Cyc_PP_text(({const char*_tmp46A="let ";_tag_dyneither(_tmp46A,sizeof(char),5U);}));_tmp469[0]=_tmp8B0;}),({
struct Cyc_PP_Doc*_tmp8AF=Cyc_Absynpp_pat2doc(_tmp4A4);_tmp469[1]=_tmp8AF;}))),({
struct Cyc_PP_Doc*_tmp8AE=Cyc_PP_text(({const char*_tmp46B=" = ";_tag_dyneither(_tmp46B,sizeof(char),4U);}));_tmp469[2]=_tmp8AE;}))),({
struct Cyc_PP_Doc*_tmp8AD=Cyc_Absynpp_exp2doc(_tmp4A3);_tmp469[3]=_tmp8AD;}))),({
struct Cyc_PP_Doc*_tmp8AC=Cyc_PP_text(({const char*_tmp46C=";";_tag_dyneither(_tmp46C,sizeof(char),2U);}));_tmp469[4]=_tmp8AC;}));Cyc_PP_cat(_tag_dyneither(_tmp469,sizeof(struct Cyc_PP_Doc*),5U));});
# 1860
s=_tmp8B1;});
# 1865
goto _LL0;case 3U: _LLD: _tmp4A5=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LLE:
# 1867
({struct Cyc_PP_Doc*_tmp8B5=({struct Cyc_PP_Doc*_tmp46D[3U];(((({struct Cyc_PP_Doc*_tmp8B4=Cyc_PP_text(({const char*_tmp46E="let ";_tag_dyneither(_tmp46E,sizeof(char),5U);}));_tmp46D[0]=_tmp8B4;}),({struct Cyc_PP_Doc*_tmp8B3=Cyc_Absynpp_ids2doc(_tmp4A5);_tmp46D[1]=_tmp8B3;}))),({struct Cyc_PP_Doc*_tmp8B2=Cyc_PP_text(({const char*_tmp46F=";";_tag_dyneither(_tmp46F,sizeof(char),2U);}));_tmp46D[2]=_tmp8B2;}));Cyc_PP_cat(_tag_dyneither(_tmp46D,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp8B5;});
goto _LL0;case 7U: _LLF: _tmp4A6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LL10:
# 1870
({struct Cyc_PP_Doc*_tmp8B8=({struct Cyc_PP_Doc*_tmp470[2U];(({struct Cyc_PP_Doc*_tmp8B7=Cyc_Absynpp_enumdecl2doc(_tmp4A6);_tmp470[0]=_tmp8B7;}),({struct Cyc_PP_Doc*_tmp8B6=Cyc_PP_text(({const char*_tmp471=";";_tag_dyneither(_tmp471,sizeof(char),2U);}));_tmp470[1]=_tmp8B6;}));Cyc_PP_cat(_tag_dyneither(_tmp470,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp8B8;});
goto _LL0;case 8U: _LL11: _tmp4A7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LL12: {
# 1873
void*t;
if(_tmp4A7->defn != 0)
t=(void*)_check_null(_tmp4A7->defn);else{
# 1877
({void*_tmp8B9=Cyc_Absyn_new_evar(_tmp4A7->kind,0);t=_tmp8B9;});}
({struct Cyc_PP_Doc*_tmp8C3=({struct Cyc_PP_Doc*_tmp472[4U];(((((({struct Cyc_PP_Doc*_tmp8C2=Cyc_PP_text(({const char*_tmp473="typedef ";_tag_dyneither(_tmp473,sizeof(char),9U);}));_tmp472[0]=_tmp8C2;}),({
struct Cyc_PP_Doc*_tmp8C1=({struct Cyc_Absyn_Tqual _tmp8C0=_tmp4A7->tq;void*_tmp8BF=t;Cyc_Absynpp_tqtd2doc(_tmp8C0,_tmp8BF,({struct Cyc_Core_Opt*_tmp475=_cycalloc(sizeof(*_tmp475));({
# 1881
struct Cyc_PP_Doc*_tmp8BE=({struct Cyc_PP_Doc*_tmp474[2U];(({struct Cyc_PP_Doc*_tmp8BD=Cyc_Absynpp_typedef_name2bolddoc(_tmp4A7->name);_tmp474[0]=_tmp8BD;}),({
struct Cyc_PP_Doc*_tmp8BC=Cyc_Absynpp_tvars2doc(_tmp4A7->tvs);_tmp474[1]=_tmp8BC;}));Cyc_PP_cat(_tag_dyneither(_tmp474,sizeof(struct Cyc_PP_Doc*),2U));});
# 1881
(*_tmp475).v=_tmp8BE;});_tmp475;}));});
# 1879
_tmp472[1]=_tmp8C1;}))),({
# 1884
struct Cyc_PP_Doc*_tmp8BB=Cyc_Absynpp_atts2doc(_tmp4A7->atts);_tmp472[2]=_tmp8BB;}))),({
struct Cyc_PP_Doc*_tmp8BA=Cyc_PP_text(({const char*_tmp476=";";_tag_dyneither(_tmp476,sizeof(char),2U);}));_tmp472[3]=_tmp8BA;}));Cyc_PP_cat(_tag_dyneither(_tmp472,sizeof(struct Cyc_PP_Doc*),4U));});
# 1878
s=_tmp8C3;});
# 1887
goto _LL0;}case 9U: _LL13: _tmp4A9=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_tmp4A8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp450)->f2;_LL14:
# 1889
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4A9);
({struct Cyc_PP_Doc*_tmp8CD=({struct Cyc_PP_Doc*_tmp477[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp8CC=Cyc_PP_text(({const char*_tmp478="namespace ";_tag_dyneither(_tmp478,sizeof(char),11U);}));_tmp477[0]=_tmp8CC;}),({
struct Cyc_PP_Doc*_tmp8CB=Cyc_PP_textptr(_tmp4A9);_tmp477[1]=_tmp8CB;}))),({
struct Cyc_PP_Doc*_tmp8CA=Cyc_PP_blank_doc();_tmp477[2]=_tmp8CA;}))),({struct Cyc_PP_Doc*_tmp8C9=Cyc_Absynpp_lb();_tmp477[3]=_tmp8C9;}))),({
struct Cyc_PP_Doc*_tmp8C8=Cyc_PP_line_doc();_tmp477[4]=_tmp8C8;}))),({
struct Cyc_PP_Doc*_tmp8C7=({struct _dyneither_ptr _tmp8C6=({const char*_tmp479="";_tag_dyneither(_tmp479,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8C6,_tmp4A8);});_tmp477[5]=_tmp8C7;}))),({
struct Cyc_PP_Doc*_tmp8C5=Cyc_PP_line_doc();_tmp477[6]=_tmp8C5;}))),({
struct Cyc_PP_Doc*_tmp8C4=Cyc_Absynpp_rb();_tmp477[7]=_tmp8C4;}));Cyc_PP_cat(_tag_dyneither(_tmp477,sizeof(struct Cyc_PP_Doc*),8U));});
# 1890
s=_tmp8CD;});
# 1897
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;case 10U: _LL15: _tmp4AB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_tmp4AA=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp450)->f2;_LL16:
# 1900
 if(Cyc_Absynpp_print_using_stmts)
({struct Cyc_PP_Doc*_tmp8D7=({struct Cyc_PP_Doc*_tmp47A[8U];(((((((((((((({struct Cyc_PP_Doc*_tmp8D6=Cyc_PP_text(({const char*_tmp47B="using ";_tag_dyneither(_tmp47B,sizeof(char),7U);}));_tmp47A[0]=_tmp8D6;}),({
struct Cyc_PP_Doc*_tmp8D5=Cyc_Absynpp_qvar2doc(_tmp4AB);_tmp47A[1]=_tmp8D5;}))),({
struct Cyc_PP_Doc*_tmp8D4=Cyc_PP_blank_doc();_tmp47A[2]=_tmp8D4;}))),({struct Cyc_PP_Doc*_tmp8D3=Cyc_Absynpp_lb();_tmp47A[3]=_tmp8D3;}))),({
struct Cyc_PP_Doc*_tmp8D2=Cyc_PP_line_doc();_tmp47A[4]=_tmp8D2;}))),({
struct Cyc_PP_Doc*_tmp8D1=({struct _dyneither_ptr _tmp8D0=({const char*_tmp47C="";_tag_dyneither(_tmp47C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8D0,_tmp4AA);});_tmp47A[5]=_tmp8D1;}))),({
struct Cyc_PP_Doc*_tmp8CF=Cyc_PP_line_doc();_tmp47A[6]=_tmp8CF;}))),({
struct Cyc_PP_Doc*_tmp8CE=Cyc_Absynpp_rb();_tmp47A[7]=_tmp8CE;}));Cyc_PP_cat(_tag_dyneither(_tmp47A,sizeof(struct Cyc_PP_Doc*),8U));});
# 1901
s=_tmp8D7;});else{
# 1909
({struct Cyc_PP_Doc*_tmp8E4=({struct Cyc_PP_Doc*_tmp47D[11U];(((((((((((((((((((({struct Cyc_PP_Doc*_tmp8E3=Cyc_PP_text(({const char*_tmp47E="/* using ";_tag_dyneither(_tmp47E,sizeof(char),10U);}));_tmp47D[0]=_tmp8E3;}),({
struct Cyc_PP_Doc*_tmp8E2=Cyc_Absynpp_qvar2doc(_tmp4AB);_tmp47D[1]=_tmp8E2;}))),({
struct Cyc_PP_Doc*_tmp8E1=Cyc_PP_blank_doc();_tmp47D[2]=_tmp8E1;}))),({
struct Cyc_PP_Doc*_tmp8E0=Cyc_Absynpp_lb();_tmp47D[3]=_tmp8E0;}))),({
struct Cyc_PP_Doc*_tmp8DF=Cyc_PP_text(({const char*_tmp47F=" */";_tag_dyneither(_tmp47F,sizeof(char),4U);}));_tmp47D[4]=_tmp8DF;}))),({
struct Cyc_PP_Doc*_tmp8DE=Cyc_PP_line_doc();_tmp47D[5]=_tmp8DE;}))),({
struct Cyc_PP_Doc*_tmp8DD=({struct _dyneither_ptr _tmp8DC=({const char*_tmp480="";_tag_dyneither(_tmp480,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8DC,_tmp4AA);});_tmp47D[6]=_tmp8DD;}))),({
struct Cyc_PP_Doc*_tmp8DB=Cyc_PP_line_doc();_tmp47D[7]=_tmp8DB;}))),({
struct Cyc_PP_Doc*_tmp8DA=Cyc_PP_text(({const char*_tmp481="/* ";_tag_dyneither(_tmp481,sizeof(char),4U);}));_tmp47D[8]=_tmp8DA;}))),({
struct Cyc_PP_Doc*_tmp8D9=Cyc_Absynpp_rb();_tmp47D[9]=_tmp8D9;}))),({
struct Cyc_PP_Doc*_tmp8D8=Cyc_PP_text(({const char*_tmp482=" */";_tag_dyneither(_tmp482,sizeof(char),4U);}));_tmp47D[10]=_tmp8D8;}));Cyc_PP_cat(_tag_dyneither(_tmp47D,sizeof(struct Cyc_PP_Doc*),11U));});
# 1909
s=_tmp8E4;});}
# 1920
goto _LL0;case 11U: _LL17: _tmp4AC=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_LL18:
# 1922
 if(Cyc_Absynpp_print_externC_stmts)
({struct Cyc_PP_Doc*_tmp8EC=({struct Cyc_PP_Doc*_tmp483[6U];(((((((((({struct Cyc_PP_Doc*_tmp8EB=Cyc_PP_text(({const char*_tmp484="extern \"C\" ";_tag_dyneither(_tmp484,sizeof(char),12U);}));_tmp483[0]=_tmp8EB;}),({
struct Cyc_PP_Doc*_tmp8EA=Cyc_Absynpp_lb();_tmp483[1]=_tmp8EA;}))),({
struct Cyc_PP_Doc*_tmp8E9=Cyc_PP_line_doc();_tmp483[2]=_tmp8E9;}))),({
struct Cyc_PP_Doc*_tmp8E8=({struct _dyneither_ptr _tmp8E7=({const char*_tmp485="";_tag_dyneither(_tmp485,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8E7,_tmp4AC);});_tmp483[3]=_tmp8E8;}))),({
struct Cyc_PP_Doc*_tmp8E6=Cyc_PP_line_doc();_tmp483[4]=_tmp8E6;}))),({
struct Cyc_PP_Doc*_tmp8E5=Cyc_Absynpp_rb();_tmp483[5]=_tmp8E5;}));Cyc_PP_cat(_tag_dyneither(_tmp483,sizeof(struct Cyc_PP_Doc*),6U));});
# 1923
s=_tmp8EC;});else{
# 1930
({struct Cyc_PP_Doc*_tmp8F7=({struct Cyc_PP_Doc*_tmp486[9U];(((((((((((((((({struct Cyc_PP_Doc*_tmp8F6=Cyc_PP_text(({const char*_tmp487="/* extern \"C\" ";_tag_dyneither(_tmp487,sizeof(char),15U);}));_tmp486[0]=_tmp8F6;}),({
struct Cyc_PP_Doc*_tmp8F5=Cyc_Absynpp_lb();_tmp486[1]=_tmp8F5;}))),({
struct Cyc_PP_Doc*_tmp8F4=Cyc_PP_text(({const char*_tmp488=" */";_tag_dyneither(_tmp488,sizeof(char),4U);}));_tmp486[2]=_tmp8F4;}))),({
struct Cyc_PP_Doc*_tmp8F3=Cyc_PP_line_doc();_tmp486[3]=_tmp8F3;}))),({
struct Cyc_PP_Doc*_tmp8F2=({struct _dyneither_ptr _tmp8F1=({const char*_tmp489="";_tag_dyneither(_tmp489,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8F1,_tmp4AC);});_tmp486[4]=_tmp8F2;}))),({
struct Cyc_PP_Doc*_tmp8F0=Cyc_PP_line_doc();_tmp486[5]=_tmp8F0;}))),({
struct Cyc_PP_Doc*_tmp8EF=Cyc_PP_text(({const char*_tmp48A="/* ";_tag_dyneither(_tmp48A,sizeof(char),4U);}));_tmp486[6]=_tmp8EF;}))),({
struct Cyc_PP_Doc*_tmp8EE=Cyc_Absynpp_rb();_tmp486[7]=_tmp8EE;}))),({
struct Cyc_PP_Doc*_tmp8ED=Cyc_PP_text(({const char*_tmp48B=" */";_tag_dyneither(_tmp48B,sizeof(char),4U);}));_tmp486[8]=_tmp8ED;}));Cyc_PP_cat(_tag_dyneither(_tmp486,sizeof(struct Cyc_PP_Doc*),9U));});
# 1930
s=_tmp8F7;});}
# 1939
goto _LL0;case 12U: _LL19: _tmp4AE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp450)->f1;_tmp4AD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp450)->f2;_LL1A:
# 1941
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4AD != 0)
({struct Cyc_PP_Doc*_tmp900=({struct Cyc_PP_Doc*_tmp48C[7U];(((((((((((({struct Cyc_PP_Doc*_tmp8FF=Cyc_Absynpp_rb();_tmp48C[0]=_tmp8FF;}),({struct Cyc_PP_Doc*_tmp8FE=Cyc_PP_text(({const char*_tmp48D=" export ";_tag_dyneither(_tmp48D,sizeof(char),9U);}));_tmp48C[1]=_tmp8FE;}))),({struct Cyc_PP_Doc*_tmp8FD=Cyc_Absynpp_lb();_tmp48C[2]=_tmp8FD;}))),({
struct Cyc_PP_Doc*_tmp8FC=Cyc_PP_line_doc();_tmp48C[3]=_tmp8FC;}))),({struct Cyc_PP_Doc*_tmp8FB=({struct _dyneither_ptr _tmp8FA=({const char*_tmp48E=",";_tag_dyneither(_tmp48E,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp8FA,_tmp4AD);});_tmp48C[4]=_tmp8FB;}))),({
struct Cyc_PP_Doc*_tmp8F9=Cyc_PP_line_doc();_tmp48C[5]=_tmp8F9;}))),({struct Cyc_PP_Doc*_tmp8F8=Cyc_Absynpp_rb();_tmp48C[6]=_tmp8F8;}));Cyc_PP_cat(_tag_dyneither(_tmp48C,sizeof(struct Cyc_PP_Doc*),7U));});
# 1944
exs_doc=_tmp900;});else{
# 1948
({struct Cyc_PP_Doc*_tmp901=Cyc_Absynpp_rb();exs_doc=_tmp901;});}
({struct Cyc_PP_Doc*_tmp908=({struct Cyc_PP_Doc*_tmp48F[6U];(((((((((({struct Cyc_PP_Doc*_tmp907=Cyc_PP_text(({const char*_tmp490="extern \"C include\" ";_tag_dyneither(_tmp490,sizeof(char),20U);}));_tmp48F[0]=_tmp907;}),({
struct Cyc_PP_Doc*_tmp906=Cyc_Absynpp_lb();_tmp48F[1]=_tmp906;}))),({
struct Cyc_PP_Doc*_tmp905=Cyc_PP_line_doc();_tmp48F[2]=_tmp905;}))),({
struct Cyc_PP_Doc*_tmp904=({struct _dyneither_ptr _tmp903=({const char*_tmp491="";_tag_dyneither(_tmp491,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp903,_tmp4AE);});_tmp48F[3]=_tmp904;}))),({
struct Cyc_PP_Doc*_tmp902=Cyc_PP_line_doc();_tmp48F[4]=_tmp902;}))),_tmp48F[5]=exs_doc);Cyc_PP_cat(_tag_dyneither(_tmp48F,sizeof(struct Cyc_PP_Doc*),6U));});
# 1949
s=_tmp908;});}else{
# 1956
({struct Cyc_PP_Doc*_tmp913=({struct Cyc_PP_Doc*_tmp492[9U];(((((((((((((((({struct Cyc_PP_Doc*_tmp912=Cyc_PP_text(({const char*_tmp493="/* extern \"C include\" ";_tag_dyneither(_tmp493,sizeof(char),23U);}));_tmp492[0]=_tmp912;}),({
struct Cyc_PP_Doc*_tmp911=Cyc_Absynpp_lb();_tmp492[1]=_tmp911;}))),({
struct Cyc_PP_Doc*_tmp910=Cyc_PP_text(({const char*_tmp494=" */";_tag_dyneither(_tmp494,sizeof(char),4U);}));_tmp492[2]=_tmp910;}))),({
struct Cyc_PP_Doc*_tmp90F=Cyc_PP_line_doc();_tmp492[3]=_tmp90F;}))),({
struct Cyc_PP_Doc*_tmp90E=({struct _dyneither_ptr _tmp90D=({const char*_tmp495="";_tag_dyneither(_tmp495,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp90D,_tmp4AE);});_tmp492[4]=_tmp90E;}))),({
struct Cyc_PP_Doc*_tmp90C=Cyc_PP_line_doc();_tmp492[5]=_tmp90C;}))),({
struct Cyc_PP_Doc*_tmp90B=Cyc_PP_text(({const char*_tmp496="/* ";_tag_dyneither(_tmp496,sizeof(char),4U);}));_tmp492[6]=_tmp90B;}))),({
struct Cyc_PP_Doc*_tmp90A=Cyc_Absynpp_rb();_tmp492[7]=_tmp90A;}))),({
struct Cyc_PP_Doc*_tmp909=Cyc_PP_text(({const char*_tmp497=" */";_tag_dyneither(_tmp497,sizeof(char),4U);}));_tmp492[8]=_tmp909;}));Cyc_PP_cat(_tag_dyneither(_tmp492,sizeof(struct Cyc_PP_Doc*),9U));});
# 1956
s=_tmp913;});}
# 1965
goto _LL0;case 13U: _LL1B: _LL1C:
# 1967
({struct Cyc_PP_Doc*_tmp916=({struct Cyc_PP_Doc*_tmp498[2U];(({struct Cyc_PP_Doc*_tmp915=Cyc_PP_text(({const char*_tmp499="__cyclone_port_on__;";_tag_dyneither(_tmp499,sizeof(char),21U);}));_tmp498[0]=_tmp915;}),({struct Cyc_PP_Doc*_tmp914=Cyc_Absynpp_lb();_tmp498[1]=_tmp914;}));Cyc_PP_cat(_tag_dyneither(_tmp498,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp916;});
goto _LL0;default: _LL1D: _LL1E:
# 1970
({struct Cyc_PP_Doc*_tmp919=({struct Cyc_PP_Doc*_tmp49A[2U];(({struct Cyc_PP_Doc*_tmp918=Cyc_PP_text(({const char*_tmp49B="__cyclone_port_off__;";_tag_dyneither(_tmp49B,sizeof(char),22U);}));_tmp49A[0]=_tmp918;}),({struct Cyc_PP_Doc*_tmp917=Cyc_Absynpp_lb();_tmp49A[1]=_tmp917;}));Cyc_PP_cat(_tag_dyneither(_tmp49A,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp919;});
goto _LL0;}_LL0:;}
# 1974
return s;}
# 1977
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4AF=sc;switch(_tmp4AF){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4B0="static ";_tag_dyneither(_tmp4B0,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4B1="extern ";_tag_dyneither(_tmp4B1,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4B2="extern \"C\" ";_tag_dyneither(_tmp4B2,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4B3="abstract ";_tag_dyneither(_tmp4B3,sizeof(char),10U);}));default: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4B4="register ";_tag_dyneither(_tmp4B4,sizeof(char),10U);}));}_LL0:;};}
# 1990
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4B5=t;struct Cyc_List_List*_tmp4B7;struct Cyc_Absyn_Tvar*_tmp4B6;switch(*((int*)_tmp4B5)){case 2U: _LL1: _tmp4B6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4B5)->f1;_LL2:
 return Cyc_Tcutil_is_temp_tvar(_tmp4B6);case 24U: _LL3: _tmp4B7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4B5)->f1;_LL4:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4B7);default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2002
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4B8=t;struct Cyc_Absyn_Typedefdecl*_tmp4BA;void*_tmp4B9;switch(*((int*)_tmp4B8)){case 12U: _LL1: _LL2:
 return 1;case 14U: _LL3: _LL4:
 return 1;case 17U: _LL5: _tmp4BA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B8)->f3;_tmp4B9=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B8)->f4;if(_tmp4B9 != 0){_LL6:
# 2009
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4B9);}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2016
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2019
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4BB=({struct _tuple16 _tmp529;(_tmp529.f1=(void*)tms->hd,_tmp529.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);_tmp529;});struct _tuple16 _tmp4BC=_tmp4BB;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4BC.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4BC.f2)->tag == 3U){_LL1: _LL2:
# 2022
 return({struct Cyc_List_List*_tmp4BE=_region_malloc(r,sizeof(*_tmp4BE));((*_tmp4BE).hd=(void*)tms->hd,({struct Cyc_List_List*_tmp91B=({struct Cyc_List_List*_tmp4BD=_region_malloc(r,sizeof(*_tmp4BD));((*_tmp4BD).hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp91A=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);(*_tmp4BD).tl=_tmp91A;}));_tmp4BD;});(*_tmp4BE).tl=_tmp91B;}));_tmp4BE;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF));((*_tmp4BF).hd=atts,(*_tmp4BF).tl=tms);_tmp4BF;});}_LL0:;}else{
# 2025
return({struct Cyc_List_List*_tmp4C0=_region_malloc(r,sizeof(*_tmp4C0));((*_tmp4C0).hd=atts,(*_tmp4C0).tl=tms);_tmp4C0;});}}
# 2030
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2032
void*_tmp4C1=t;struct _tuple0*_tmp4FE;struct Cyc_List_List*_tmp4FD;struct Cyc_Absyn_Typedefdecl*_tmp4FC;void*_tmp4FB;struct Cyc_Core_Opt*_tmp4FA;void*_tmp4F9;int _tmp4F8;struct Cyc_List_List*_tmp4F7;void*_tmp4F6;struct Cyc_Absyn_Tqual _tmp4F5;void*_tmp4F4;struct Cyc_List_List*_tmp4F3;int _tmp4F2;struct Cyc_Absyn_VarargInfo*_tmp4F1;struct Cyc_List_List*_tmp4F0;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;void*_tmp4EC;struct Cyc_Absyn_Tqual _tmp4EB;struct Cyc_Absyn_PtrAtts _tmp4EA;void*_tmp4E9;struct Cyc_Absyn_Tqual _tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;union Cyc_Absyn_Constraint*_tmp4E6;unsigned int _tmp4E5;switch(*((int*)_tmp4C1)){case 8U: _LL1: _tmp4E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).elt_type;_tmp4E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).tq;_tmp4E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).num_elts;_tmp4E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).zero_term;_tmp4E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).zt_loc;_LL2: {
# 2035
struct _tuple12 _tmp4C2=Cyc_Absynpp_to_tms(r,_tmp4E8,_tmp4E9);struct _tuple12 _tmp4C3=_tmp4C2;struct Cyc_Absyn_Tqual _tmp4C9;void*_tmp4C8;struct Cyc_List_List*_tmp4C7;_LLE: _tmp4C9=_tmp4C3.f1;_tmp4C8=_tmp4C3.f2;_tmp4C7=_tmp4C3.f3;_LLF:;{
void*tm;
if(_tmp4E7 == 0)
({void*_tmp91C=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4));((((*_tmp4C4).tag=0U,(*_tmp4C4).f1=_tmp4E6)),(*_tmp4C4).f2=_tmp4E5);_tmp4C4;});tm=_tmp91C;});else{
# 2040
({void*_tmp91D=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5));((((((*_tmp4C5).tag=1U,(*_tmp4C5).f1=_tmp4E7)),(*_tmp4C5).f2=_tmp4E6)),(*_tmp4C5).f3=_tmp4E5);_tmp4C5;});tm=_tmp91D;});}
return({struct _tuple12 _tmp52A;(((_tmp52A.f1=_tmp4C9,_tmp52A.f2=_tmp4C8)),({struct Cyc_List_List*_tmp91E=({struct Cyc_List_List*_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6));((*_tmp4C6).hd=tm,(*_tmp4C6).tl=_tmp4C7);_tmp4C6;});_tmp52A.f3=_tmp91E;}));_tmp52A;});};}case 5U: _LL3: _tmp4EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).elt_typ;_tmp4EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).elt_tq;_tmp4EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts;_LL4: {
# 2044
struct _tuple12 _tmp4CA=Cyc_Absynpp_to_tms(r,_tmp4EB,_tmp4EC);struct _tuple12 _tmp4CB=_tmp4CA;struct Cyc_Absyn_Tqual _tmp4D0;void*_tmp4CF;struct Cyc_List_List*_tmp4CE;_LL11: _tmp4D0=_tmp4CB.f1;_tmp4CF=_tmp4CB.f2;_tmp4CE=_tmp4CB.f3;_LL12:;
({struct Cyc_List_List*_tmp920=({struct Cyc_List_List*_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD));(({void*_tmp91F=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC));((((*_tmp4CC).tag=2U,(*_tmp4CC).f1=_tmp4EA)),(*_tmp4CC).f2=tq);_tmp4CC;});(*_tmp4CD).hd=_tmp91F;}),(*_tmp4CD).tl=_tmp4CE);_tmp4CD;});_tmp4CE=_tmp920;});
return({struct _tuple12 _tmp52B;(((_tmp52B.f1=_tmp4D0,_tmp52B.f2=_tmp4CF)),_tmp52B.f3=_tmp4CE);_tmp52B;});}case 9U: _LL5: _tmp4F7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).tvars;_tmp4F6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).effect;_tmp4F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).ret_tqual;_tmp4F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).ret_typ;_tmp4F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).args;_tmp4F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).c_varargs;_tmp4F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).cyc_varargs;_tmp4F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).rgn_po;_tmp4EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).attributes;_tmp4EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).requires_clause;_tmp4ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1)->f1).ensures_clause;_LL6:
# 2050
 if(!Cyc_Absynpp_print_all_tvars){
# 2054
if(_tmp4F6 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp4F6)){
_tmp4F6=0;
_tmp4F7=0;}}else{
# 2059
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2062
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp4F7);}{
# 2067
struct _tuple12 _tmp4D1=Cyc_Absynpp_to_tms(r,_tmp4F5,_tmp4F4);struct _tuple12 _tmp4D2=_tmp4D1;struct Cyc_Absyn_Tqual _tmp4E4;void*_tmp4E3;struct Cyc_List_List*_tmp4E2;_LL14: _tmp4E4=_tmp4D2.f1;_tmp4E3=_tmp4D2.f2;_tmp4E2=_tmp4D2.f3;_LL15:;{
struct Cyc_List_List*tms=_tmp4E2;
# 2078 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4D3=Cyc_Cyclone_c_compiler;if(_tmp4D3 == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2080
 if(_tmp4EF != 0)
# 2083
({struct Cyc_List_List*_tmp923=({struct _RegionHandle*_tmp922=r;void*_tmp921=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D4=_region_malloc(r,sizeof(*_tmp4D4));((((*_tmp4D4).tag=5U,(*_tmp4D4).f1=0U)),(*_tmp4D4).f2=_tmp4EF);_tmp4D4;});Cyc_Absynpp_bubble_attributes(_tmp922,_tmp921,tms);});tms=_tmp923;});
# 2085
({struct Cyc_List_List*_tmp926=({struct Cyc_List_List*_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7));(({void*_tmp925=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6));
((*_tmp4D6).tag=3U,({void*_tmp924=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D5=_region_malloc(r,sizeof(*_tmp4D5));((((((((((((((*_tmp4D5).tag=1U,(*_tmp4D5).f1=_tmp4F3)),(*_tmp4D5).f2=_tmp4F2)),(*_tmp4D5).f3=_tmp4F1)),(*_tmp4D5).f4=_tmp4F6)),(*_tmp4D5).f5=_tmp4F0)),(*_tmp4D5).f6=_tmp4EE)),(*_tmp4D5).f7=_tmp4ED);_tmp4D5;});(*_tmp4D6).f1=_tmp924;}));_tmp4D6;});
# 2085
(*_tmp4D7).hd=_tmp925;}),(*_tmp4D7).tl=tms);_tmp4D7;});tms=_tmp926;});
# 2089
goto _LL16;}else{_LL19: _LL1A:
# 2091
({struct Cyc_List_List*_tmp929=({struct Cyc_List_List*_tmp4DA=_region_malloc(r,sizeof(*_tmp4DA));(({void*_tmp928=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D9=_region_malloc(r,sizeof(*_tmp4D9));
((*_tmp4D9).tag=3U,({void*_tmp927=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8));((((((((((((((*_tmp4D8).tag=1U,(*_tmp4D8).f1=_tmp4F3)),(*_tmp4D8).f2=_tmp4F2)),(*_tmp4D8).f3=_tmp4F1)),(*_tmp4D8).f4=_tmp4F6)),(*_tmp4D8).f5=_tmp4F0)),(*_tmp4D8).f6=_tmp4EE)),(*_tmp4D8).f7=_tmp4ED);_tmp4D8;});(*_tmp4D9).f1=_tmp927;}));_tmp4D9;});
# 2091
(*_tmp4DA).hd=_tmp928;}),(*_tmp4DA).tl=tms);_tmp4DA;});tms=_tmp929;});
# 2095
for(0;_tmp4EF != 0;_tmp4EF=_tmp4EF->tl){
void*_tmp4DB=(void*)_tmp4EF->hd;void*_tmp4DC=_tmp4DB;switch(*((int*)_tmp4DC)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2100
({struct Cyc_List_List*_tmp92C=({struct Cyc_List_List*_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF));(({void*_tmp92B=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE));((((*_tmp4DE).tag=5U,(*_tmp4DE).f1=0U)),({struct Cyc_List_List*_tmp92A=({struct Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));((*_tmp4DD).hd=(void*)_tmp4EF->hd,(*_tmp4DD).tl=0);_tmp4DD;});(*_tmp4DE).f2=_tmp92A;}));_tmp4DE;});(*_tmp4DF).hd=_tmp92B;}),(*_tmp4DF).tl=tms);_tmp4DF;});tms=_tmp92C;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2104
goto _LL16;}_LL16:;}
# 2107
AfterAtts:
 if(_tmp4F7 != 0)
({struct Cyc_List_List*_tmp92E=({struct Cyc_List_List*_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1));(({void*_tmp92D=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0));((((((*_tmp4E0).tag=4U,(*_tmp4E0).f1=_tmp4F7)),(*_tmp4E0).f2=0U)),(*_tmp4E0).f3=1);_tmp4E0;});(*_tmp4E1).hd=_tmp92D;}),(*_tmp4E1).tl=tms);_tmp4E1;});tms=_tmp92E;});
return({struct _tuple12 _tmp52C;(((_tmp52C.f1=_tmp4E4,_tmp52C.f2=_tmp4E3)),_tmp52C.f3=tms);_tmp52C;});};};case 1U: _LL7: _tmp4FA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C1)->f1;_tmp4F9=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C1)->f2;_tmp4F8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C1)->f3;_LL8:
# 2113
 if(_tmp4F9 == 0)
return({struct _tuple12 _tmp52D;(((_tmp52D.f1=tq,_tmp52D.f2=t)),_tmp52D.f3=0);_tmp52D;});else{
# 2116
return Cyc_Absynpp_to_tms(r,tq,_tmp4F9);}case 17U: _LL9: _tmp4FE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C1)->f1;_tmp4FD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C1)->f2;_tmp4FC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C1)->f3;_tmp4FB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C1)->f4;_LLA:
# 2122
 if(_tmp4FB == 0  || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple12 _tmp52E;(((_tmp52E.f1=tq,_tmp52E.f2=t)),_tmp52E.f3=0);_tmp52E;});else{
# 2125
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp4FB);}default: _LLB: _LLC:
# 2130
 return({struct _tuple12 _tmp52F;(((_tmp52F.f1=tq,_tmp52F.f2=t)),_tmp52F.f3=0);_tmp52F;});}_LL0:;}
# 2134
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2136
void*_tmp4FF=t;void*_tmp504;void*_tmp503;switch(*((int*)_tmp4FF)){case 1U: _LL1: _tmp503=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FF)->f2;if(_tmp503 != 0){_LL2:
 return Cyc_Absynpp_is_char_ptr(_tmp503);}else{goto _LL5;}case 5U: _LL3: _tmp504=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FF)->f1).elt_typ;_LL4:
# 2139
 L: {
void*_tmp500=_tmp504;void*_tmp502;void*_tmp501;switch(*((int*)_tmp500)){case 1U: _LL8: _tmp501=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp500)->f2;if(_tmp501 != 0){_LL9:
 _tmp504=_tmp501;goto L;}else{goto _LLE;}case 17U: _LLA: _tmp502=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp500)->f4;if(_tmp502 != 0){_LLB:
 _tmp504=_tmp502;goto L;}else{goto _LLE;}case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp500)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 2146
 return 0;}_LL0:;}
# 2150
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp505=_new_region("temp");struct _RegionHandle*temp=& _tmp505;_push_region(temp);
{struct _tuple12 _tmp506=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp507=_tmp506;struct Cyc_Absyn_Tqual _tmp50F;void*_tmp50E;struct Cyc_List_List*_tmp50D;_LL1: _tmp50F=_tmp507.f1;_tmp50E=_tmp507.f2;_tmp50D=_tmp507.f3;_LL2:;
({struct Cyc_List_List*_tmp92F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp50D);_tmp50D=_tmp92F;});
if(_tmp50D == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp509=({struct Cyc_PP_Doc*_tmp508[2U];(({struct Cyc_PP_Doc*_tmp931=Cyc_Absynpp_tqual2doc(_tmp50F);_tmp508[0]=_tmp931;}),({struct Cyc_PP_Doc*_tmp930=Cyc_Absynpp_ntyp2doc(_tmp50E);_tmp508[1]=_tmp930;}));Cyc_PP_cat(_tag_dyneither(_tmp508,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp509;}else{
# 2157
struct Cyc_PP_Doc*_tmp50C=({struct Cyc_PP_Doc*_tmp50A[4U];(((((({
struct Cyc_PP_Doc*_tmp937=Cyc_Absynpp_tqual2doc(_tmp50F);_tmp50A[0]=_tmp937;}),({
struct Cyc_PP_Doc*_tmp936=Cyc_Absynpp_ntyp2doc(_tmp50E);_tmp50A[1]=_tmp936;}))),({
struct Cyc_PP_Doc*_tmp935=Cyc_PP_text(({const char*_tmp50B=" ";_tag_dyneither(_tmp50B,sizeof(char),2U);}));_tmp50A[2]=_tmp935;}))),({
struct Cyc_PP_Doc*_tmp934=({int _tmp933=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp932=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp933,_tmp932,_tmp50D);});_tmp50A[3]=_tmp934;}));Cyc_PP_cat(_tag_dyneither(_tmp50A,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp50C;}}
# 2152
;_pop_region(temp);}
# 2165
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp510=f->requires_clause;
if((unsigned int)_tmp510)
({struct Cyc_PP_Doc*_tmp93A=({struct Cyc_PP_Doc*_tmp511[2U];(({struct Cyc_PP_Doc*_tmp939=Cyc_PP_text(({const char*_tmp512="@requires ";_tag_dyneither(_tmp512,sizeof(char),11U);}));_tmp511[0]=_tmp939;}),({struct Cyc_PP_Doc*_tmp938=Cyc_Absynpp_exp2doc(_tmp510);_tmp511[1]=_tmp938;}));Cyc_PP_cat(_tag_dyneither(_tmp511,sizeof(struct Cyc_PP_Doc*),2U));});requires_doc=_tmp93A;});else{
# 2171
({struct Cyc_PP_Doc*_tmp93B=Cyc_PP_nil_doc();requires_doc=_tmp93B;});}{
# 2173
enum Cyc_Cyclone_C_Compilers _tmp513=Cyc_Cyclone_c_compiler;if(_tmp513 == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2176
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp514[5U];(((((((({struct Cyc_PP_Doc*_tmp943=({struct Cyc_Absyn_Tqual _tmp942=f->tq;void*_tmp941=f->type;Cyc_Absynpp_tqtd2doc(_tmp942,_tmp941,({struct Cyc_Core_Opt*_tmp515=_cycalloc(sizeof(*_tmp515));({struct Cyc_PP_Doc*_tmp940=Cyc_PP_textptr(f->name);(*_tmp515).v=_tmp940;});_tmp515;}));});_tmp514[0]=_tmp943;}),({
struct Cyc_PP_Doc*_tmp93F=Cyc_PP_text(({const char*_tmp516=":";_tag_dyneither(_tmp516,sizeof(char),2U);}));_tmp514[1]=_tmp93F;}))),({struct Cyc_PP_Doc*_tmp93E=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp514[2]=_tmp93E;}))),({
struct Cyc_PP_Doc*_tmp93D=Cyc_Absynpp_atts2doc(f->attributes);_tmp514[3]=_tmp93D;}))),({struct Cyc_PP_Doc*_tmp93C=Cyc_PP_text(({const char*_tmp517=";";_tag_dyneither(_tmp517,sizeof(char),2U);}));_tmp514[4]=_tmp93C;}));Cyc_PP_cat(_tag_dyneither(_tmp514,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2181
return({struct Cyc_PP_Doc*_tmp518[4U];(((((({struct Cyc_PP_Doc*_tmp949=({struct Cyc_Absyn_Tqual _tmp948=f->tq;void*_tmp947=f->type;Cyc_Absynpp_tqtd2doc(_tmp948,_tmp947,({struct Cyc_Core_Opt*_tmp519=_cycalloc(sizeof(*_tmp519));({struct Cyc_PP_Doc*_tmp946=Cyc_PP_textptr(f->name);(*_tmp519).v=_tmp946;});_tmp519;}));});_tmp518[0]=_tmp949;}),({
struct Cyc_PP_Doc*_tmp945=Cyc_Absynpp_atts2doc(f->attributes);_tmp518[1]=_tmp945;}))),_tmp518[2]=requires_doc)),({struct Cyc_PP_Doc*_tmp944=Cyc_PP_text(({const char*_tmp51A=";";_tag_dyneither(_tmp51A,sizeof(char),2U);}));_tmp518[3]=_tmp944;}));Cyc_PP_cat(_tag_dyneither(_tmp518,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2184
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp51B[5U];(((((((({struct Cyc_PP_Doc*_tmp951=Cyc_Absynpp_atts2doc(f->attributes);_tmp51B[0]=_tmp951;}),({
struct Cyc_PP_Doc*_tmp950=({struct Cyc_Absyn_Tqual _tmp94F=f->tq;void*_tmp94E=f->type;Cyc_Absynpp_tqtd2doc(_tmp94F,_tmp94E,({struct Cyc_Core_Opt*_tmp51C=_cycalloc(sizeof(*_tmp51C));({struct Cyc_PP_Doc*_tmp94D=Cyc_PP_textptr(f->name);(*_tmp51C).v=_tmp94D;});_tmp51C;}));});_tmp51B[1]=_tmp950;}))),({
struct Cyc_PP_Doc*_tmp94C=Cyc_PP_text(({const char*_tmp51D=":";_tag_dyneither(_tmp51D,sizeof(char),2U);}));_tmp51B[2]=_tmp94C;}))),({struct Cyc_PP_Doc*_tmp94B=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp51B[3]=_tmp94B;}))),({struct Cyc_PP_Doc*_tmp94A=Cyc_PP_text(({const char*_tmp51E=";";_tag_dyneither(_tmp51E,sizeof(char),2U);}));_tmp51B[4]=_tmp94A;}));Cyc_PP_cat(_tag_dyneither(_tmp51B,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2189
return({struct Cyc_PP_Doc*_tmp51F[4U];(((((({struct Cyc_PP_Doc*_tmp957=Cyc_Absynpp_atts2doc(f->attributes);_tmp51F[0]=_tmp957;}),({
struct Cyc_PP_Doc*_tmp956=({struct Cyc_Absyn_Tqual _tmp955=f->tq;void*_tmp954=f->type;Cyc_Absynpp_tqtd2doc(_tmp955,_tmp954,({struct Cyc_Core_Opt*_tmp520=_cycalloc(sizeof(*_tmp520));({struct Cyc_PP_Doc*_tmp953=Cyc_PP_textptr(f->name);(*_tmp520).v=_tmp953;});_tmp520;}));});_tmp51F[1]=_tmp956;}))),_tmp51F[2]=requires_doc)),({
struct Cyc_PP_Doc*_tmp952=Cyc_PP_text(({const char*_tmp521=";";_tag_dyneither(_tmp521,sizeof(char),2U);}));_tmp51F[3]=_tmp952;}));Cyc_PP_cat(_tag_dyneither(_tmp51F,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;};}
# 2196
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp958=({const char*_tmp522="";_tag_dyneither(_tmp522,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp958,fields);});}
# 2200
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp523[3U];(((({struct Cyc_PP_Doc*_tmp95C=Cyc_Absynpp_scope2doc(f->sc);_tmp523[0]=_tmp95C;}),({struct Cyc_PP_Doc*_tmp95B=Cyc_Absynpp_typedef_name2doc(f->name);_tmp523[1]=_tmp95B;}))),
f->typs == 0?({struct Cyc_PP_Doc*_tmp95A=Cyc_PP_nil_doc();_tmp523[2]=_tmp95A;}):({struct Cyc_PP_Doc*_tmp959=Cyc_Absynpp_args2doc(f->typs);_tmp523[2]=_tmp959;}));Cyc_PP_cat(_tag_dyneither(_tmp523,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2205
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp95D=({const char*_tmp524=",";_tag_dyneither(_tmp524,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp95D,fields);});}
# 2214
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp95E=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp95E,72,f);});
({void*_tmp525=0U;({struct Cyc___cycFILE*_tmp960=f;struct _dyneither_ptr _tmp95F=({const char*_tmp526="\n";_tag_dyneither(_tmp526,sizeof(char),2U);});Cyc_fprintf(_tmp960,_tmp95F,_tag_dyneither(_tmp525,sizeof(void*),0U));});});}}
# 2221
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _dyneither_ptr _tmp961=({const char*_tmp527="";_tag_dyneither(_tmp527,sizeof(char),1U);});Cyc_PP_seql(_tmp961,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2224
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2228
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2239
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

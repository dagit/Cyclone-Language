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
# 297 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
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
({struct Cyc_List_List*_tmp540=({struct Cyc_List_List*_tmp53F=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp53F,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;_tmp1;}));});Cyc_Absynpp_curr_namespace=_tmp540;});}
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
if(tq.q_restrict)({struct Cyc_List_List*_tmp541=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp;_tmp6C->tl=l;_tmp6C;});l=_tmp541;});
if(tq.q_volatile)({struct Cyc_List_List*_tmp542=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp;_tmp6D->tl=l;_tmp6D;});l=_tmp542;});
if(tq.print_const)({struct Cyc_List_List*_tmp543=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp;_tmp6E->tl=l;_tmp6E;});l=_tmp543;});
return({struct _dyneither_ptr _tmp546=({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1U);});struct _dyneither_ptr _tmp545=({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2U);});struct _dyneither_ptr _tmp544=({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp546,_tmp545,_tmp544,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
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
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0U;({struct _dyneither_ptr _tmp547=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp90));_tmp8E.f1=_tmp547;});({void*_tmp8C[1U]={& _tmp8E};({struct _dyneither_ptr _tmp548=({const char*_tmp8D="<=%s";_tag_dyneither(_tmp8D,sizeof(char),5U);});Cyc_aprintf(_tmp548,_tag_dyneither(_tmp8C,sizeof(void*),1U));});});});}_LL0:;}
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
return({struct _dyneither_ptr _tmp54B=({const char*_tmp97="<";_tag_dyneither(_tmp97,sizeof(char),2U);});struct _dyneither_ptr _tmp54A=({const char*_tmp98=">";_tag_dyneither(_tmp98,sizeof(char),2U);});struct _dyneither_ptr _tmp549=({const char*_tmp99=",";_tag_dyneither(_tmp99,sizeof(char),2U);});Cyc_PP_egroup(_tmp54B,_tmp54A,_tmp549,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 428
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 430
struct _dyneither_ptr kstring=({const char*_tmpA3="K";_tag_dyneither(_tmpA3,sizeof(char),2U);});
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmp9D;switch(*((int*)_tmp9C)){case 2U: _LL1: _tmp9D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C)->f2;_LL2:
 _tmp9E=_tmp9D;goto _LL4;case 0U: _LL3: _tmp9E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C)->f1;_LL4:
({struct _dyneither_ptr _tmp54C=Cyc_Absynpp_kind2string(_tmp9E);kstring=_tmp54C;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 436
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0U;({struct _dyneither_ptr _tmp54D=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1));_tmpA2.f1=_tmp54D;});({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0U;_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);({void*_tmp9F[2U]={& _tmpA1,& _tmpA2};({struct _dyneither_ptr _tmp54E=({const char*_tmpA0="`G%s%s";_tag_dyneither(_tmpA0,sizeof(char),7U);});Cyc_aprintf(_tmp54E,_tag_dyneither(_tmp9F,sizeof(void*),2U));});});});}));}
# 438
return Cyc_PP_text(_tmp9A);}
# 441
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA4=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA5=_tmpA4;struct Cyc_Absyn_Kind*_tmpA9;struct Cyc_Absyn_Kind*_tmpA8;switch(*((int*)_tmpA5)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpA8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1;_LL8:
# 446
 return({struct Cyc_PP_Doc*_tmpA6[3U];({struct Cyc_PP_Doc*_tmp54F=Cyc_Absynpp_kind2doc(_tmpA8);_tmpA6[2U]=_tmp54F;});({struct Cyc_PP_Doc*_tmp550=Cyc_PP_text(({const char*_tmpA7="::";_tag_dyneither(_tmpA7,sizeof(char),3U);}));_tmpA6[1U]=_tmp550;});({struct Cyc_PP_Doc*_tmp551=Cyc_Absynpp_tvar2doc(tv);_tmpA6[0U]=_tmp551;});Cyc_PP_cat(_tag_dyneither(_tmpA6,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL5: _tmpA9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5)->f2;_LL6:
# 445
 _tmpA8=_tmpA9;goto _LL8;}_LL0:;}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _dyneither_ptr _tmp554=({const char*_tmpAA="<";_tag_dyneither(_tmpAA,sizeof(char),2U);});struct _dyneither_ptr _tmp553=({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2U);});struct _dyneither_ptr _tmp552=({const char*_tmpAC=",";_tag_dyneither(_tmpAC,sizeof(char),2U);});Cyc_PP_egroup(_tmp554,_tmp553,_tmp552,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 454
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _dyneither_ptr _tmp557=({const char*_tmpAD="<";_tag_dyneither(_tmpAD,sizeof(char),2U);});struct _dyneither_ptr _tmp556=({const char*_tmpAE=">";_tag_dyneither(_tmpAE,sizeof(char),2U);});struct _dyneither_ptr _tmp555=({const char*_tmpAF=",";_tag_dyneither(_tmpAF,sizeof(char),2U);});Cyc_PP_egroup(_tmp557,_tmp556,_tmp555,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp55A=({const char*_tmpB0="(";_tag_dyneither(_tmpB0,sizeof(char),2U);});struct _dyneither_ptr _tmp559=({const char*_tmpB1=")";_tag_dyneither(_tmpB1,sizeof(char),2U);});struct _dyneither_ptr _tmp558=({const char*_tmpB2=",";_tag_dyneither(_tmpB2,sizeof(char),2U);});Cyc_PP_group(_tmp55A,_tmp559,_tmp558,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
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
return({struct Cyc_PP_Doc*_tmpBB[3U];({struct Cyc_PP_Doc*_tmp55B=
# 501
Cyc_PP_text(({const char*_tmpC0=")";_tag_dyneither(_tmpC0,sizeof(char),2U);}));_tmpBB[2U]=_tmp55B;});({struct Cyc_PP_Doc*_tmp55F=({
# 500
struct _dyneither_ptr _tmp55E=({const char*_tmpBD="";_tag_dyneither(_tmpBD,sizeof(char),1U);});struct _dyneither_ptr _tmp55D=({const char*_tmpBE="";_tag_dyneither(_tmpBE,sizeof(char),1U);});struct _dyneither_ptr _tmp55C=({const char*_tmpBF=" ";_tag_dyneither(_tmpBF,sizeof(char),2U);});Cyc_PP_group(_tmp55E,_tmp55D,_tmp55C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBB[1U]=_tmp55F;});({struct Cyc_PP_Doc*_tmp560=
# 499
Cyc_PP_text(({const char*_tmpBC=" __declspec(";_tag_dyneither(_tmpBC,sizeof(char),13U);}));_tmpBB[0U]=_tmp560;});Cyc_PP_cat(_tag_dyneither(_tmpBB,sizeof(struct Cyc_PP_Doc*),3U));});}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 508
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC1=Cyc_Cyclone_c_compiler;if(_tmpC1 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
 return({struct Cyc_PP_Doc*_tmpC2[2U];({struct Cyc_PP_Doc*_tmp564=({
struct _dyneither_ptr _tmp563=({const char*_tmpC4="((";_tag_dyneither(_tmpC4,sizeof(char),3U);});struct _dyneither_ptr _tmp562=({const char*_tmpC5="))";_tag_dyneither(_tmpC5,sizeof(char),3U);});struct _dyneither_ptr _tmp561=({const char*_tmpC6=",";_tag_dyneither(_tmpC6,sizeof(char),2U);});Cyc_PP_group(_tmp563,_tmp562,_tmp561,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC2[1U]=_tmp564;});({struct Cyc_PP_Doc*_tmp565=
# 512
Cyc_PP_text(({const char*_tmpC3=" __attribute__";_tag_dyneither(_tmpC3,sizeof(char),15U);}));_tmpC2[0U]=_tmp565;});Cyc_PP_cat(_tag_dyneither(_tmpC2,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;};}
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
({struct Cyc_PP_Doc*_tmp566=Cyc_PP_text_width(({const char*_tmpCA="{?}";_tag_dyneither(_tmpCA,sizeof(char),4U);}),1);Cyc_Absynpp_cache_question=_tmp566;});else{
({struct Cyc_PP_Doc*_tmp567=Cyc_PP_text(({const char*_tmpCB="?";_tag_dyneither(_tmpCB,sizeof(char),2U);}));Cyc_Absynpp_cache_question=_tmp567;});}}
# 540
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 542
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp568=Cyc_PP_text_width(({const char*_tmpCC="{\\lb}";_tag_dyneither(_tmpCC,sizeof(char),6U);}),1);Cyc_Absynpp_cache_lb=_tmp568;});else{
({struct Cyc_PP_Doc*_tmp569=Cyc_PP_text(({const char*_tmpCD="{";_tag_dyneither(_tmpCD,sizeof(char),2U);}));Cyc_Absynpp_cache_lb=_tmp569;});}}
# 549
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp56A=Cyc_PP_text_width(({const char*_tmpCE="{\\rb}";_tag_dyneither(_tmpCE,sizeof(char),6U);}),1);Cyc_Absynpp_cache_rb=_tmp56A;});else{
({struct Cyc_PP_Doc*_tmp56B=Cyc_PP_text(({const char*_tmpCF="}";_tag_dyneither(_tmpCF,sizeof(char),2U);}));Cyc_Absynpp_cache_rb=_tmp56B;});}}
# 558
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp56C=Cyc_PP_text_width(({const char*_tmpD0="\\$";_tag_dyneither(_tmpD0,sizeof(char),3U);}),1);Cyc_Absynpp_cache_dollar=_tmp56C;});else{
({struct Cyc_PP_Doc*_tmp56D=Cyc_PP_text(({const char*_tmpD1="$";_tag_dyneither(_tmpD1,sizeof(char),2U);}));Cyc_Absynpp_cache_dollar=_tmp56D;});}}
# 567
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 569
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD2[3U];({struct Cyc_PP_Doc*_tmp56E=Cyc_Absynpp_rb();_tmpD2[2U]=_tmp56E;});({struct Cyc_PP_Doc*_tmp56F=Cyc_PP_seq(sep,ss);_tmpD2[1U]=_tmp56F;});({struct Cyc_PP_Doc*_tmp570=Cyc_Absynpp_lb();_tmpD2[0U]=_tmp570;});Cyc_PP_cat(_tag_dyneither(_tmpD2,sizeof(struct Cyc_PP_Doc*),3U));});}
# 574
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD3=(void*)tms->hd;void*_tmpD4=_tmpD3;struct Cyc_List_List*_tmpEB;switch(*((int*)_tmpD4)){case 0U: _LL1: _LL2:
({void*_tmpD5=0U;({struct Cyc___cycFILE*_tmp572=Cyc_stderr;struct _dyneither_ptr _tmp571=({const char*_tmpD6="Carray_mod ";_tag_dyneither(_tmpD6,sizeof(char),12U);});Cyc_fprintf(_tmp572,_tmp571,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp574=Cyc_stderr;struct _dyneither_ptr _tmp573=({const char*_tmpD8="ConstArray_mod ";_tag_dyneither(_tmpD8,sizeof(char),16U);});Cyc_fprintf(_tmp574,_tmp573,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->tag == 1U){_LL5: _tmpEB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->f1;_LL6:
# 580
({void*_tmpD9=0U;({struct Cyc___cycFILE*_tmp576=Cyc_stderr;struct _dyneither_ptr _tmp575=({const char*_tmpDA="Function_mod(";_tag_dyneither(_tmpDA,sizeof(char),14U);});Cyc_fprintf(_tmp576,_tmp575,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});
for(0;_tmpEB != 0;_tmpEB=_tmpEB->tl){
struct _dyneither_ptr*_tmpDB=(*((struct _tuple8*)_tmpEB->hd)).f1;
if(_tmpDB == 0)({void*_tmpDC=0U;({struct Cyc___cycFILE*_tmp578=Cyc_stderr;struct _dyneither_ptr _tmp577=({const char*_tmpDD="?";_tag_dyneither(_tmpDD,sizeof(char),2U);});Cyc_fprintf(_tmp578,_tmp577,_tag_dyneither(_tmpDC,sizeof(void*),0U));});});else{
({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp57A=Cyc_stderr;struct _dyneither_ptr _tmp579=*_tmpDB;Cyc_fprintf(_tmp57A,_tmp579,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});}
if(_tmpEB->tl != 0)({void*_tmpDF=0U;({struct Cyc___cycFILE*_tmp57C=Cyc_stderr;struct _dyneither_ptr _tmp57B=({const char*_tmpE0=",";_tag_dyneither(_tmpE0,sizeof(char),2U);});Cyc_fprintf(_tmp57C,_tmp57B,_tag_dyneither(_tmpDF,sizeof(void*),0U));});});}
# 587
({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp57E=Cyc_stderr;struct _dyneither_ptr _tmp57D=({const char*_tmpE2=") ";_tag_dyneither(_tmpE2,sizeof(char),3U);});Cyc_fprintf(_tmp57E,_tmp57D,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});
goto _LL0;}else{_LL7: _LL8:
# 590
({void*_tmpE3=0U;({struct Cyc___cycFILE*_tmp580=Cyc_stderr;struct _dyneither_ptr _tmp57F=({const char*_tmpE4="Function_mod()";_tag_dyneither(_tmpE4,sizeof(char),15U);});Cyc_fprintf(_tmp580,_tmp57F,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE5=0U;({struct Cyc___cycFILE*_tmp582=Cyc_stderr;struct _dyneither_ptr _tmp581=({const char*_tmpE6="Attributes_mod ";_tag_dyneither(_tmpE6,sizeof(char),16U);});Cyc_fprintf(_tmp582,_tmp581,_tag_dyneither(_tmpE5,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpE7=0U;({struct Cyc___cycFILE*_tmp584=Cyc_stderr;struct _dyneither_ptr _tmp583=({const char*_tmpE8="TypeParams_mod ";_tag_dyneither(_tmpE8,sizeof(char),16U);});Cyc_fprintf(_tmp584,_tmp583,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpE9=0U;({struct Cyc___cycFILE*_tmp586=Cyc_stderr;struct _dyneither_ptr _tmp585=({const char*_tmpEA="Pointer_mod ";_tag_dyneither(_tmpEA,sizeof(char),13U);});Cyc_fprintf(_tmp586,_tmp585,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 595
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp588=Cyc_stderr;struct _dyneither_ptr _tmp587=({const char*_tmpED="\n";_tag_dyneither(_tmpED,sizeof(char),2U);});Cyc_fprintf(_tmp588,_tmp587,_tag_dyneither(_tmpEC,sizeof(void*),0U));});});}
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 600
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp129[3U];({struct Cyc_PP_Doc*_tmp589=Cyc_PP_text(({const char*_tmp12B=")";_tag_dyneither(_tmp12B,sizeof(char),2U);}));_tmp129[2U]=_tmp589;});_tmp129[1U]=rest;({struct Cyc_PP_Doc*_tmp58A=Cyc_PP_text(({const char*_tmp12A="(";_tag_dyneither(_tmp12A,sizeof(char),2U);}));_tmp129[0U]=_tmp58A;});Cyc_PP_cat(_tag_dyneither(_tmp129,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpEE=(void*)tms->hd;void*_tmpEF=_tmpEE;void*_tmp128;union Cyc_Absyn_Constraint*_tmp127;union Cyc_Absyn_Constraint*_tmp126;union Cyc_Absyn_Constraint*_tmp125;struct Cyc_Absyn_Tqual _tmp124;struct Cyc_List_List*_tmp123;unsigned int _tmp122;int _tmp121;struct Cyc_List_List*_tmp120;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11C;switch(*((int*)_tmpEF)){case 0U: _LL1: _tmp11C=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LL2:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF0[2U];({struct Cyc_PP_Doc*_tmp58B=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11C)?
Cyc_PP_text(({const char*_tmpF1="[]@zeroterm";_tag_dyneither(_tmpF1,sizeof(char),12U);})): Cyc_PP_text(({const char*_tmpF2="[]";_tag_dyneither(_tmpF2,sizeof(char),3U);}));_tmpF0[1U]=_tmp58B;});_tmpF0[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF0,sizeof(struct Cyc_PP_Doc*),2U));});case 1U: _LL3: _tmp11E=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp11D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LL4:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF3[4U];({struct Cyc_PP_Doc*_tmp58C=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11D)?Cyc_PP_text(({const char*_tmpF5="]@zeroterm";_tag_dyneither(_tmpF5,sizeof(char),11U);})): Cyc_PP_text(({const char*_tmpF6="]";_tag_dyneither(_tmpF6,sizeof(char),2U);}));_tmpF3[3U]=_tmp58C;});({struct Cyc_PP_Doc*_tmp58D=
# 611
Cyc_Absynpp_exp2doc(_tmp11E);_tmpF3[2U]=_tmp58D;});({struct Cyc_PP_Doc*_tmp58E=Cyc_PP_text(({const char*_tmpF4="[";_tag_dyneither(_tmpF4,sizeof(char),2U);}));_tmpF3[1U]=_tmp58E;});_tmpF3[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF3,sizeof(struct Cyc_PP_Doc*),4U));});case 3U: _LL5: _tmp11F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LL6:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF7=_tmp11F;struct Cyc_List_List*_tmp105;unsigned int _tmp104;struct Cyc_List_List*_tmp103;int _tmp102;struct Cyc_Absyn_VarargInfo*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->tag == 1U){_LLE: _tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp102=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f3;_tmp100=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f4;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f5;_tmpFE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f6;_tmpFD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f7;_LLF:
# 617
 return({struct Cyc_PP_Doc*_tmpF8[2U];({struct Cyc_PP_Doc*_tmp58F=Cyc_Absynpp_funargs2doc(_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF,_tmpFE,_tmpFD);_tmpF8[1U]=_tmp58F;});_tmpF8[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF8,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL10: _tmp105=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp104=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_LL11:
# 619
 return({struct Cyc_PP_Doc*_tmpF9[2U];({struct Cyc_PP_Doc*_tmp593=({struct _dyneither_ptr _tmp592=({const char*_tmpFA="(";_tag_dyneither(_tmpFA,sizeof(char),2U);});struct _dyneither_ptr _tmp591=({const char*_tmpFB=")";_tag_dyneither(_tmpFB,sizeof(char),2U);});struct _dyneither_ptr _tmp590=({const char*_tmpFC=",";_tag_dyneither(_tmpFC,sizeof(char),2U);});Cyc_PP_group(_tmp592,_tmp591,_tmp590,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp105));});_tmpF9[1U]=_tmp593;});_tmpF9[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF9,sizeof(struct Cyc_PP_Doc*),2U));});}_LLD:;};case 5U: _LL7: _tmp120=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LL8: {
# 622
enum Cyc_Cyclone_C_Compilers _tmp106=Cyc_Cyclone_c_compiler;if(_tmp106 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 624
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp107[2U];({struct Cyc_PP_Doc*_tmp594=Cyc_Absynpp_atts2doc(_tmp120);_tmp107[1U]=_tmp594;});_tmp107[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmp107,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 627
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp108[2U];_tmp108[1U]=rest;({struct Cyc_PP_Doc*_tmp595=Cyc_Absynpp_callconv2doc(_tmp120);_tmp108[0U]=_tmp595;});Cyc_PP_cat(_tag_dyneither(_tmp108,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp123=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp122=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_tmp121=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f3;_LLA:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp121)
return({struct Cyc_PP_Doc*_tmp109[2U];({struct Cyc_PP_Doc*_tmp596=Cyc_Absynpp_ktvars2doc(_tmp123);_tmp109[1U]=_tmp596;});_tmp109[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 636
return({struct Cyc_PP_Doc*_tmp10A[2U];({struct Cyc_PP_Doc*_tmp597=Cyc_Absynpp_tvars2doc(_tmp123);_tmp10A[1U]=_tmp597;});_tmp10A[0U]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});}default: _LLB: _tmp128=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).rgn;_tmp127=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).nullable;_tmp126=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).bounds;_tmp125=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).zero_term;_tmp124=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLC: {
# 640
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp124);
{void*_tmp10B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp126);void*_tmp10C=_tmp10B;struct Cyc_Absyn_Exp*_tmp114;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp10C)->tag == 0U){_LL18: _LL19:
({struct Cyc_PP_Doc*_tmp598=Cyc_Absynpp_question();ptr=_tmp598;});goto _LL17;}else{_LL1A: _tmp114=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp10C)->f1;_LL1B:
# 648
({struct Cyc_PP_Doc*_tmp599=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp127)?({const char*_tmp10D="*";_tag_dyneither(_tmp10D,sizeof(char),2U);}):({const char*_tmp10E="@";_tag_dyneither(_tmp10E,sizeof(char),2U);}));ptr=_tmp599;});{
struct _tuple11 _tmp10F=Cyc_Evexp_eval_const_uint_exp(_tmp114);struct _tuple11 _tmp110=_tmp10F;unsigned int _tmp113;int _tmp112;_LL1D: _tmp113=_tmp110.f1;_tmp112=_tmp110.f2;_LL1E:;
if(!_tmp112  || _tmp113 != 1)
({struct Cyc_PP_Doc*_tmp59D=({struct Cyc_PP_Doc*_tmp111[4U];({struct Cyc_PP_Doc*_tmp59A=Cyc_Absynpp_rb();_tmp111[3U]=_tmp59A;});({struct Cyc_PP_Doc*_tmp59B=Cyc_Absynpp_exp2doc(_tmp114);_tmp111[2U]=_tmp59B;});({struct Cyc_PP_Doc*_tmp59C=Cyc_Absynpp_lb();_tmp111[1U]=_tmp59C;});_tmp111[0U]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp111,sizeof(struct Cyc_PP_Doc*),4U));});ptr=_tmp59D;});
goto _LL17;};}_LL17:;}
# 654
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
({struct Cyc_PP_Doc*_tmp59E=Cyc_PP_text(({const char*_tmp115="@zeroterm";_tag_dyneither(_tmp115,sizeof(char),10U);}));ztd=_tmp59E;});else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
({struct Cyc_PP_Doc*_tmp59F=Cyc_PP_text(({const char*_tmp116="@nozeroterm";_tag_dyneither(_tmp116,sizeof(char),12U);}));ztd=_tmp59F;});}}
# 660
{void*_tmp117=Cyc_Tcutil_compress(_tmp128);void*_tmp118=_tmp117;switch(*((int*)_tmp118)){case 20U: _LL20: _LL21:
 goto _LL1F;case 1U: _LL22: if(Cyc_Absynpp_print_for_cycdoc){_LL23:
 goto _LL1F;}else{goto _LL24;}default: _LL24: _LL25:
({struct Cyc_PP_Doc*_tmp5A0=Cyc_Absynpp_rgn2doc(_tmp128);rgd=_tmp5A0;});}_LL1F:;}{
# 665
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp11B=" ";_tag_dyneither(_tmp11B,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp11A=" ";_tag_dyneither(_tmp11A,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp119[7U];_tmp119[6U]=rest;_tmp119[5U]=spacer2;_tmp119[4U]=tqd;_tmp119[3U]=spacer1;_tmp119[2U]=rgd;_tmp119[1U]=ztd;_tmp119[0U]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp119,sizeof(struct Cyc_PP_Doc*),7U));});};}}_LL0:;};}
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
({struct Cyc_List_List*_tmp5A2=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_PP_Doc*_tmp5A1=Cyc_Absynpp_rgn2doc(_tmp135);_tmp133->hd=_tmp5A1;});_tmp133->tl=*rgions;_tmp133;});*rgions=_tmp5A2;});goto _LL0;case 24U: _LL3: _tmp136=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132)->f1;_LL4:
# 688
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp136->hd);}
# 691
goto _LL0;default: _LL5: _LL6:
({struct Cyc_List_List*_tmp5A4=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));({struct Cyc_PP_Doc*_tmp5A3=Cyc_Absynpp_typ2doc(t);_tmp134->hd=_tmp5A3;});_tmp134->tl=*effects;_tmp134;});*effects=_tmp5A4;});goto _LL0;}_LL0:;}
# 696
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
({struct Cyc_List_List*_tmp5A5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);rgions=_tmp5A5;});
({struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);effects=_tmp5A6;});
if(rgions == 0  && effects != 0)
return({struct _dyneither_ptr _tmp5A9=({const char*_tmp137="";_tag_dyneither(_tmp137,sizeof(char),1U);});struct _dyneither_ptr _tmp5A8=({const char*_tmp138="";_tag_dyneither(_tmp138,sizeof(char),1U);});struct _dyneither_ptr _tmp5A7=({const char*_tmp139="+";_tag_dyneither(_tmp139,sizeof(char),2U);});Cyc_PP_group(_tmp5A9,_tmp5A8,_tmp5A7,effects);});else{
# 704
struct Cyc_PP_Doc*_tmp13A=({struct _dyneither_ptr _tmp5AA=({const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5AA,rgions);});
return({struct _dyneither_ptr _tmp5AE=({const char*_tmp13B="";_tag_dyneither(_tmp13B,sizeof(char),1U);});struct _dyneither_ptr _tmp5AD=({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1U);});struct _dyneither_ptr _tmp5AC=({const char*_tmp13D="+";_tag_dyneither(_tmp13D,sizeof(char),2U);});Cyc_PP_group(_tmp5AE,_tmp5AD,_tmp5AC,({struct Cyc_List_List*_tmp5AB=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5AB,({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=_tmp13A;_tmp13E->tl=0;_tmp13E;}));}));});}}
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
({struct Cyc_PP_Doc*_tmp5AF=Cyc_PP_text(({const char*_tmp145="void";_tag_dyneither(_tmp145,sizeof(char),5U);}));s=_tmp5AF;});goto _LL0;case 1U: _LL9: _tmp19B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f1;_tmp19A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_tmp199=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f3;_tmp198=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f4;_LLA:
# 727
 if(_tmp19A != 0)
# 729
return Cyc_Absynpp_ntyp2doc(_tmp19A);else{
# 731
struct _dyneither_ptr kindstring=_tmp19B == 0?({const char*_tmp14A="K";_tag_dyneither(_tmp14A,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp19B->v);
({struct Cyc_PP_Doc*_tmp5B1=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1U;_tmp149.f1=(unsigned long)_tmp199;({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0U;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);({void*_tmp146[2U]={& _tmp148,& _tmp149};({struct _dyneither_ptr _tmp5B0=({const char*_tmp147="`E%s%d";_tag_dyneither(_tmp147,sizeof(char),7U);});Cyc_aprintf(_tmp5B0,_tag_dyneither(_tmp146,sizeof(void*),2U));});});});}));s=_tmp5B1;});}
# 748 "absynpp.cyc"
goto _LL0;case 2U: _LLB: _tmp19C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LLC:
# 750
({struct Cyc_PP_Doc*_tmp5B2=Cyc_Absynpp_tvar2doc(_tmp19C);s=_tmp5B2;});
if(Cyc_Absynpp_print_all_kinds)
({struct Cyc_PP_Doc*_tmp5B5=({struct Cyc_PP_Doc*_tmp14B[3U];({struct Cyc_PP_Doc*_tmp5B3=Cyc_Absynpp_kindbound2doc(_tmp19C->kind);_tmp14B[2U]=_tmp5B3;});({struct Cyc_PP_Doc*_tmp5B4=Cyc_PP_text(({const char*_tmp14C="::";_tag_dyneither(_tmp14C,sizeof(char),3U);}));_tmp14B[1U]=_tmp5B4;});_tmp14B[0U]=s;Cyc_PP_cat(_tag_dyneither(_tmp14B,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5B5;});
# 754
goto _LL0;case 3U: _LLD: _tmp19E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).datatype_info;_tmp19D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).targs;_LLE:
# 756
{union Cyc_Absyn_DatatypeInfoU _tmp14D=_tmp19E;struct _tuple0*_tmp156;int _tmp155;struct _tuple0*_tmp154;int _tmp153;if((_tmp14D.UnknownDatatype).tag == 1){_LL40: _tmp154=((_tmp14D.UnknownDatatype).val).name;_tmp153=((_tmp14D.UnknownDatatype).val).is_extensible;_LL41:
# 758
 _tmp156=_tmp154;_tmp155=_tmp153;goto _LL43;}else{_LL42: _tmp156=(*(_tmp14D.KnownDatatype).val)->name;_tmp155=(*(_tmp14D.KnownDatatype).val)->is_extensible;_LL43: {
# 760
struct Cyc_PP_Doc*_tmp14E=Cyc_PP_text(({const char*_tmp152="datatype ";_tag_dyneither(_tmp152,sizeof(char),10U);}));
struct Cyc_PP_Doc*_tmp14F=_tmp155?Cyc_PP_text(({const char*_tmp151="@extensible ";_tag_dyneither(_tmp151,sizeof(char),13U);})): Cyc_PP_nil_doc();
({struct Cyc_PP_Doc*_tmp5B8=({struct Cyc_PP_Doc*_tmp150[4U];({struct Cyc_PP_Doc*_tmp5B6=Cyc_Absynpp_tps2doc(_tmp19D);_tmp150[3U]=_tmp5B6;});({struct Cyc_PP_Doc*_tmp5B7=Cyc_Absynpp_qvar2doc(_tmp156);_tmp150[2U]=_tmp5B7;});_tmp150[1U]=_tmp14E;_tmp150[0U]=_tmp14F;Cyc_PP_cat(_tag_dyneither(_tmp150,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5B8;});
goto _LL3F;}}_LL3F:;}
# 765
goto _LL0;case 4U: _LLF: _tmp1A0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).field_info;_tmp19F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL10:
# 767
{union Cyc_Absyn_DatatypeFieldInfoU _tmp157=_tmp1A0;struct _tuple0*_tmp162;int _tmp161;struct _tuple0*_tmp160;struct _tuple0*_tmp15F;struct _tuple0*_tmp15E;int _tmp15D;if((_tmp157.UnknownDatatypefield).tag == 1){_LL45: _tmp15F=((_tmp157.UnknownDatatypefield).val).datatype_name;_tmp15E=((_tmp157.UnknownDatatypefield).val).field_name;_tmp15D=((_tmp157.UnknownDatatypefield).val).is_extensible;_LL46:
# 769
 _tmp162=_tmp15F;_tmp161=_tmp15D;_tmp160=_tmp15E;goto _LL48;}else{_LL47: _tmp162=(((_tmp157.KnownDatatypefield).val).f1)->name;_tmp161=(((_tmp157.KnownDatatypefield).val).f1)->is_extensible;_tmp160=(((_tmp157.KnownDatatypefield).val).f2)->name;_LL48: {
# 772
struct Cyc_PP_Doc*_tmp158=Cyc_PP_text(_tmp161?({const char*_tmp15B="@extensible datatype ";_tag_dyneither(_tmp15B,sizeof(char),22U);}):({const char*_tmp15C="datatype ";_tag_dyneither(_tmp15C,sizeof(char),10U);}));
({struct Cyc_PP_Doc*_tmp5BC=({struct Cyc_PP_Doc*_tmp159[4U];({struct Cyc_PP_Doc*_tmp5B9=Cyc_Absynpp_qvar2doc(_tmp160);_tmp159[3U]=_tmp5B9;});({struct Cyc_PP_Doc*_tmp5BA=Cyc_PP_text(({const char*_tmp15A=".";_tag_dyneither(_tmp15A,sizeof(char),2U);}));_tmp159[2U]=_tmp5BA;});({struct Cyc_PP_Doc*_tmp5BB=Cyc_Absynpp_qvar2doc(_tmp162);_tmp159[1U]=_tmp5BB;});_tmp159[0U]=_tmp158;Cyc_PP_cat(_tag_dyneither(_tmp159,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5BC;});
goto _LL44;}}_LL44:;}
# 776
goto _LL0;case 6U: _LL11: _tmp1A2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f2;_LL12: {
# 778
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp163=_tmp1A2;switch(_tmp163){case Cyc_Absyn_None: _LL4A: _LL4B:
 goto _LL4D;case Cyc_Absyn_Signed: _LL4C: _LL4D:
({struct _dyneither_ptr _tmp5BD=({const char*_tmp164="";_tag_dyneither(_tmp164,sizeof(char),1U);});sns=_tmp5BD;});goto _LL49;default: _LL4E: _LL4F:
({struct _dyneither_ptr _tmp5BE=({const char*_tmp165="unsigned ";_tag_dyneither(_tmp165,sizeof(char),10U);});sns=_tmp5BE;});goto _LL49;}_LL49:;}
# 785
{enum Cyc_Absyn_Size_of _tmp166=_tmp1A1;switch(_tmp166){case Cyc_Absyn_Char_sz: _LL51: _LL52:
# 787
{enum Cyc_Absyn_Sign _tmp167=_tmp1A2;switch(_tmp167){case Cyc_Absyn_None: _LL5C: _LL5D:
({struct _dyneither_ptr _tmp5BF=({const char*_tmp168="";_tag_dyneither(_tmp168,sizeof(char),1U);});sns=_tmp5BF;});goto _LL5B;case Cyc_Absyn_Signed: _LL5E: _LL5F:
({struct _dyneither_ptr _tmp5C0=({const char*_tmp169="signed ";_tag_dyneither(_tmp169,sizeof(char),8U);});sns=_tmp5C0;});goto _LL5B;default: _LL60: _LL61:
({struct _dyneither_ptr _tmp5C1=({const char*_tmp16A="unsigned ";_tag_dyneither(_tmp16A,sizeof(char),10U);});sns=_tmp5C1;});goto _LL5B;}_LL5B:;}
# 792
({struct _dyneither_ptr _tmp5C2=({const char*_tmp16B="char";_tag_dyneither(_tmp16B,sizeof(char),5U);});ts=_tmp5C2;});
goto _LL50;case Cyc_Absyn_Short_sz: _LL53: _LL54:
({struct _dyneither_ptr _tmp5C3=({const char*_tmp16C="short";_tag_dyneither(_tmp16C,sizeof(char),6U);});ts=_tmp5C3;});goto _LL50;case Cyc_Absyn_Int_sz: _LL55: _LL56:
({struct _dyneither_ptr _tmp5C4=({const char*_tmp16D="int";_tag_dyneither(_tmp16D,sizeof(char),4U);});ts=_tmp5C4;});goto _LL50;case Cyc_Absyn_Long_sz: _LL57: _LL58:
({struct _dyneither_ptr _tmp5C5=({const char*_tmp16E="long";_tag_dyneither(_tmp16E,sizeof(char),5U);});ts=_tmp5C5;});goto _LL50;default: _LL59: _LL5A:
# 798
{enum Cyc_Cyclone_C_Compilers _tmp16F=Cyc_Cyclone_c_compiler;if(_tmp16F == Cyc_Cyclone_Gcc_c){_LL63: _LL64:
({struct _dyneither_ptr _tmp5C6=({const char*_tmp170="long long";_tag_dyneither(_tmp170,sizeof(char),10U);});ts=_tmp5C6;});goto _LL62;}else{_LL65: _LL66:
({struct _dyneither_ptr _tmp5C7=({const char*_tmp171="__int64";_tag_dyneither(_tmp171,sizeof(char),8U);});ts=_tmp5C7;});goto _LL62;}_LL62:;}
# 802
goto _LL50;}_LL50:;}
# 804
({struct Cyc_PP_Doc*_tmp5C9=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0U;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0U;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);({void*_tmp172[2U]={& _tmp174,& _tmp175};({struct _dyneither_ptr _tmp5C8=({const char*_tmp173="%s%s";_tag_dyneither(_tmp173,sizeof(char),5U);});Cyc_aprintf(_tmp5C8,_tag_dyneither(_tmp172,sizeof(void*),2U));});});});}));s=_tmp5C9;});
goto _LL0;}case 7U: _LL13: _tmp1A3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp143)->f1;_LL14:
# 807
 if(_tmp1A3 == 0)
({struct Cyc_PP_Doc*_tmp5CA=Cyc_PP_text(({const char*_tmp176="float";_tag_dyneither(_tmp176,sizeof(char),6U);}));s=_tmp5CA;});else{
if(_tmp1A3 == 1)
({struct Cyc_PP_Doc*_tmp5CB=Cyc_PP_text(({const char*_tmp177="double";_tag_dyneither(_tmp177,sizeof(char),7U);}));s=_tmp5CB;});else{
# 812
({struct Cyc_PP_Doc*_tmp5CC=Cyc_PP_text(({const char*_tmp178="long double";_tag_dyneither(_tmp178,sizeof(char),12U);}));s=_tmp5CC;});}}
goto _LL0;case 10U: _LL15: _tmp1A4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL16:
# 815
({struct Cyc_PP_Doc*_tmp5CF=({struct Cyc_PP_Doc*_tmp179[2U];({struct Cyc_PP_Doc*_tmp5CD=Cyc_Absynpp_args2doc(_tmp1A4);_tmp179[1U]=_tmp5CD;});({struct Cyc_PP_Doc*_tmp5CE=Cyc_Absynpp_dollar();_tmp179[0U]=_tmp5CE;});Cyc_PP_cat(_tag_dyneither(_tmp179,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp5CF;});
goto _LL0;case 11U: _LL17: _tmp1A6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).aggr_info;_tmp1A5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL18: {
# 818
struct _tuple10 _tmp17A=Cyc_Absyn_aggr_kinded_name(_tmp1A6);struct _tuple10 _tmp17B=_tmp17A;enum Cyc_Absyn_AggrKind _tmp17E;struct _tuple0*_tmp17D;_LL68: _tmp17E=_tmp17B.f1;_tmp17D=_tmp17B.f2;_LL69:;
({struct Cyc_PP_Doc*_tmp5D3=({struct Cyc_PP_Doc*_tmp17C[3U];({struct Cyc_PP_Doc*_tmp5D0=Cyc_Absynpp_tps2doc(_tmp1A5);_tmp17C[2U]=_tmp5D0;});({struct Cyc_PP_Doc*_tmp5D1=Cyc_Absynpp_qvar2doc(_tmp17D);_tmp17C[1U]=_tmp5D1;});({struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_aggr_kind2doc(_tmp17E);_tmp17C[0U]=_tmp5D2;});Cyc_PP_cat(_tag_dyneither(_tmp17C,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5D3;});
goto _LL0;}case 12U: _LL19: _tmp1A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL1A:
# 822
({struct Cyc_PP_Doc*_tmp5D8=({struct Cyc_PP_Doc*_tmp17F[4U];({struct Cyc_PP_Doc*_tmp5D4=
# 824
Cyc_Absynpp_rb();_tmp17F[3U]=_tmp5D4;});({struct Cyc_PP_Doc*_tmp5D5=
# 823
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1A7));_tmp17F[2U]=_tmp5D5;});({struct Cyc_PP_Doc*_tmp5D6=
# 822
Cyc_Absynpp_lb();_tmp17F[1U]=_tmp5D6;});({struct Cyc_PP_Doc*_tmp5D7=Cyc_Absynpp_aggr_kind2doc(_tmp1A8);_tmp17F[0U]=_tmp5D7;});Cyc_PP_cat(_tag_dyneither(_tmp17F,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5D8;});
# 825
goto _LL0;case 14U: _LL1B: _tmp1A9=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp143)->f1;_LL1C:
# 827
({struct Cyc_PP_Doc*_tmp5DD=({struct Cyc_PP_Doc*_tmp180[4U];({struct Cyc_PP_Doc*_tmp5D9=Cyc_Absynpp_rb();_tmp180[3U]=_tmp5D9;});({struct Cyc_PP_Doc*_tmp5DA=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1A9));_tmp180[2U]=_tmp5DA;});({struct Cyc_PP_Doc*_tmp5DB=Cyc_Absynpp_lb();_tmp180[1U]=_tmp5DB;});({struct Cyc_PP_Doc*_tmp5DC=Cyc_PP_text(({const char*_tmp181="enum ";_tag_dyneither(_tmp181,sizeof(char),6U);}));_tmp180[0U]=_tmp5DC;});Cyc_PP_cat(_tag_dyneither(_tmp180,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp5DD;});goto _LL0;case 13U: _LL1D: _tmp1AA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp143)->f1;_LL1E:
# 829
({struct Cyc_PP_Doc*_tmp5E0=({struct Cyc_PP_Doc*_tmp182[2U];({struct Cyc_PP_Doc*_tmp5DE=Cyc_Absynpp_qvar2doc(_tmp1AA);_tmp182[1U]=_tmp5DE;});({struct Cyc_PP_Doc*_tmp5DF=Cyc_PP_text(({const char*_tmp183="enum ";_tag_dyneither(_tmp183,sizeof(char),6U);}));_tmp182[0U]=_tmp5DF;});Cyc_PP_cat(_tag_dyneither(_tmp182,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp5E0;});goto _LL0;case 18U: _LL1F: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL20:
# 831
({struct Cyc_PP_Doc*_tmp5E4=({struct Cyc_PP_Doc*_tmp184[3U];({struct Cyc_PP_Doc*_tmp5E1=Cyc_PP_text(({const char*_tmp186=")";_tag_dyneither(_tmp186,sizeof(char),2U);}));_tmp184[2U]=_tmp5E1;});({struct Cyc_PP_Doc*_tmp5E2=Cyc_Absynpp_exp2doc(_tmp1AB);_tmp184[1U]=_tmp5E2;});({struct Cyc_PP_Doc*_tmp5E3=Cyc_PP_text(({const char*_tmp185="valueof_t(";_tag_dyneither(_tmp185,sizeof(char),11U);}));_tmp184[0U]=_tmp5E3;});Cyc_PP_cat(_tag_dyneither(_tmp184,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5E4;});goto _LL0;case 27U: _LL21: _tmp1AC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL22:
# 833
({struct Cyc_PP_Doc*_tmp5E8=({struct Cyc_PP_Doc*_tmp187[3U];({struct Cyc_PP_Doc*_tmp5E5=Cyc_PP_text(({const char*_tmp189=")";_tag_dyneither(_tmp189,sizeof(char),2U);}));_tmp187[2U]=_tmp5E5;});({struct Cyc_PP_Doc*_tmp5E6=Cyc_Absynpp_exp2doc(_tmp1AC);_tmp187[1U]=_tmp5E6;});({struct Cyc_PP_Doc*_tmp5E7=Cyc_PP_text(({const char*_tmp188="typeof(";_tag_dyneither(_tmp188,sizeof(char),8U);}));_tmp187[0U]=_tmp5E7;});Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5E8;});goto _LL0;case 28U: _LL23: _tmp1AD=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp143)->f1;_LL24:
# 835
({struct Cyc_PP_Doc*_tmp5E9=Cyc_PP_text(_tmp1AD);s=_tmp5E9;});goto _LL0;case 17U: _LL25: _tmp1B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_LL26:
# 842
({struct Cyc_PP_Doc*_tmp5EC=({struct Cyc_PP_Doc*_tmp18A[2U];({struct Cyc_PP_Doc*_tmp5EA=Cyc_Absynpp_tps2doc(_tmp1AF);_tmp18A[1U]=_tmp5EA;});({struct Cyc_PP_Doc*_tmp5EB=Cyc_Absynpp_qvar2doc(_tmp1B0);_tmp18A[0U]=_tmp5EB;});Cyc_PP_cat(_tag_dyneither(_tmp18A,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp5EC;});
goto _LL0;case 15U: _LL27: _tmp1B1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp143)->f1;_LL28:
# 845
({struct Cyc_PP_Doc*_tmp5F0=({struct Cyc_PP_Doc*_tmp18B[3U];({struct Cyc_PP_Doc*_tmp5ED=Cyc_PP_text(({const char*_tmp18D=">";_tag_dyneither(_tmp18D,sizeof(char),2U);}));_tmp18B[2U]=_tmp5ED;});({struct Cyc_PP_Doc*_tmp5EE=Cyc_Absynpp_rgn2doc(_tmp1B1);_tmp18B[1U]=_tmp5EE;});({struct Cyc_PP_Doc*_tmp5EF=Cyc_PP_text(({const char*_tmp18C="region_t<";_tag_dyneither(_tmp18C,sizeof(char),10U);}));_tmp18B[0U]=_tmp5EF;});Cyc_PP_cat(_tag_dyneither(_tmp18B,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5F0;});goto _LL0;case 16U: _LL29: _tmp1B3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f1;_tmp1B2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f2;_LL2A:
# 847
({struct Cyc_PP_Doc*_tmp5F6=({struct Cyc_PP_Doc*_tmp18E[5U];({struct Cyc_PP_Doc*_tmp5F1=
Cyc_PP_text(({const char*_tmp191=">";_tag_dyneither(_tmp191,sizeof(char),2U);}));_tmp18E[4U]=_tmp5F1;});({struct Cyc_PP_Doc*_tmp5F2=
# 847
Cyc_Absynpp_rgn2doc(_tmp1B2);_tmp18E[3U]=_tmp5F2;});({struct Cyc_PP_Doc*_tmp5F3=Cyc_PP_text(({const char*_tmp190=",";_tag_dyneither(_tmp190,sizeof(char),2U);}));_tmp18E[2U]=_tmp5F3;});({struct Cyc_PP_Doc*_tmp5F4=Cyc_Absynpp_rgn2doc(_tmp1B3);_tmp18E[1U]=_tmp5F4;});({struct Cyc_PP_Doc*_tmp5F5=Cyc_PP_text(({const char*_tmp18F="dynregion_t<";_tag_dyneither(_tmp18F,sizeof(char),13U);}));_tmp18E[0U]=_tmp5F5;});Cyc_PP_cat(_tag_dyneither(_tmp18E,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp5F6;});
goto _LL0;case 19U: _LL2B: _tmp1B4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp143)->f1;_LL2C:
# 850
({struct Cyc_PP_Doc*_tmp5FA=({struct Cyc_PP_Doc*_tmp192[3U];({struct Cyc_PP_Doc*_tmp5F7=Cyc_PP_text(({const char*_tmp194=">";_tag_dyneither(_tmp194,sizeof(char),2U);}));_tmp192[2U]=_tmp5F7;});({struct Cyc_PP_Doc*_tmp5F8=Cyc_Absynpp_typ2doc(_tmp1B4);_tmp192[1U]=_tmp5F8;});({struct Cyc_PP_Doc*_tmp5F9=Cyc_PP_text(({const char*_tmp193="tag_t<";_tag_dyneither(_tmp193,sizeof(char),7U);}));_tmp192[0U]=_tmp5F9;});Cyc_PP_cat(_tag_dyneither(_tmp192,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5FA;});goto _LL0;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 22U: _LL31: _LL32:
# 854
({struct Cyc_PP_Doc*_tmp5FB=Cyc_Absynpp_rgn2doc(t);s=_tmp5FB;});goto _LL0;case 25U: _LL33: _tmp1B5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp143)->f1;_LL34:
# 856
({struct Cyc_PP_Doc*_tmp5FF=({struct Cyc_PP_Doc*_tmp195[3U];({struct Cyc_PP_Doc*_tmp5FC=Cyc_PP_text(({const char*_tmp197=")";_tag_dyneither(_tmp197,sizeof(char),2U);}));_tmp195[2U]=_tmp5FC;});({struct Cyc_PP_Doc*_tmp5FD=Cyc_Absynpp_typ2doc(_tmp1B5);_tmp195[1U]=_tmp5FD;});({struct Cyc_PP_Doc*_tmp5FE=Cyc_PP_text(({const char*_tmp196="regions(";_tag_dyneither(_tmp196,sizeof(char),9U);}));_tmp195[0U]=_tmp5FE;});Cyc_PP_cat(_tag_dyneither(_tmp195,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp5FF;});goto _LL0;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
# 859
({struct Cyc_PP_Doc*_tmp600=Cyc_Absynpp_eff2doc(t);s=_tmp600;});goto _LL0;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0U: _LL39: _tmp1B6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL3A:
# 861
({struct Cyc_PP_Doc*_tmp601=Cyc_Absynpp_aggrdecl2doc(_tmp1B6);s=_tmp601;});goto _LL0;case 1U: _LL3B: _tmp1B7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL3C:
# 863
({struct Cyc_PP_Doc*_tmp602=Cyc_Absynpp_enumdecl2doc(_tmp1B7);s=_tmp602;});goto _LL0;default: _LL3D: _tmp1B8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL3E:
# 865
({struct Cyc_PP_Doc*_tmp603=Cyc_Absynpp_datatypedecl2doc(_tmp1B8);s=_tmp603;});goto _LL0;}}_LL0:;}
# 867
return s;}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1B9=cmp;void*_tmp1BD;void*_tmp1BC;_LL1: _tmp1BD=_tmp1B9->f1;_tmp1BC=_tmp1B9->f2;_LL2:;
return({struct Cyc_PP_Doc*_tmp1BA[3U];({struct Cyc_PP_Doc*_tmp604=Cyc_Absynpp_rgn2doc(_tmp1BC);_tmp1BA[2U]=_tmp604;});({struct Cyc_PP_Doc*_tmp605=Cyc_PP_text(({const char*_tmp1BB=" > ";_tag_dyneither(_tmp1BB,sizeof(char),4U);}));_tmp1BA[1U]=_tmp605;});({struct Cyc_PP_Doc*_tmp606=Cyc_Absynpp_rgn2doc(_tmp1BD);_tmp1BA[0U]=_tmp606;});Cyc_PP_cat(_tag_dyneither(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3U));});}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _dyneither_ptr _tmp609=({const char*_tmp1BE="";_tag_dyneither(_tmp1BE,sizeof(char),1U);});struct _dyneither_ptr _tmp608=({const char*_tmp1BF="";_tag_dyneither(_tmp1BF,sizeof(char),1U);});struct _dyneither_ptr _tmp607=({const char*_tmp1C0=",";_tag_dyneither(_tmp1C0,sizeof(char),2U);});Cyc_PP_group(_tmp609,_tmp608,_tmp607,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C1=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C1 == 0?0:({struct Cyc_Core_Opt*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));({struct Cyc_PP_Doc*_tmp60A=Cyc_PP_text(*_tmp1C1);_tmp1C2->v=_tmp60A;});_tmp1C2;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 889
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 893
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
({struct Cyc_List_List*_tmp60D=({struct Cyc_List_List*_tmp60C=_tmp1C3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60C,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(({const char*_tmp1C5="...";_tag_dyneither(_tmp1C5,sizeof(char),4U);}));_tmp1C4->hd=_tmp60B;});_tmp1C4->tl=0;_tmp1C4;}));});_tmp1C3=_tmp60D;});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1C6=({struct Cyc_PP_Doc*_tmp1C8[3U];({struct Cyc_PP_Doc*_tmp60E=
# 900
Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=cyc_varargs->name;_tmp1CC->f2=cyc_varargs->tq;_tmp1CC->f3=cyc_varargs->type;_tmp1CC;}));_tmp1C8[2U]=_tmp60E;});({struct Cyc_PP_Doc*_tmp60F=
# 899
cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1CA=" inject ";_tag_dyneither(_tmp1CA,sizeof(char),9U);})): Cyc_PP_text(({const char*_tmp1CB=" ";_tag_dyneither(_tmp1CB,sizeof(char),2U);}));_tmp1C8[1U]=_tmp60F;});({struct Cyc_PP_Doc*_tmp610=
# 898
Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4U);}));_tmp1C8[0U]=_tmp610;});Cyc_PP_cat(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3U));});
# 902
({struct Cyc_List_List*_tmp612=({struct Cyc_List_List*_tmp611=_tmp1C3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp611,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=_tmp1C6;_tmp1C7->tl=0;_tmp1C7;}));});_tmp1C3=_tmp612;});}}{
# 904
struct Cyc_PP_Doc*_tmp1CD=({struct _dyneither_ptr _tmp615=({const char*_tmp1DC="";_tag_dyneither(_tmp1DC,sizeof(char),1U);});struct _dyneither_ptr _tmp614=({const char*_tmp1DD="";_tag_dyneither(_tmp1DD,sizeof(char),1U);});struct _dyneither_ptr _tmp613=({const char*_tmp1DE=",";_tag_dyneither(_tmp1DE,sizeof(char),2U);});Cyc_PP_group(_tmp615,_tmp614,_tmp613,_tmp1C3);});
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
({struct Cyc_PP_Doc*_tmp618=({struct Cyc_PP_Doc*_tmp1CE[3U];({struct Cyc_PP_Doc*_tmp616=Cyc_Absynpp_eff2doc(effopt);_tmp1CE[2U]=_tmp616;});({struct Cyc_PP_Doc*_tmp617=Cyc_PP_text(({const char*_tmp1CF=";";_tag_dyneither(_tmp1CF,sizeof(char),2U);}));_tmp1CE[1U]=_tmp617;});_tmp1CE[0U]=_tmp1CD;Cyc_PP_cat(_tag_dyneither(_tmp1CE,sizeof(struct Cyc_PP_Doc*),3U));});_tmp1CD=_tmp618;});
if(rgn_po != 0)
({struct Cyc_PP_Doc*_tmp61B=({struct Cyc_PP_Doc*_tmp1D0[3U];({struct Cyc_PP_Doc*_tmp619=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1D0[2U]=_tmp619;});({struct Cyc_PP_Doc*_tmp61A=Cyc_PP_text(({const char*_tmp1D1=":";_tag_dyneither(_tmp1D1,sizeof(char),2U);}));_tmp1D0[1U]=_tmp61A;});_tmp1D0[0U]=_tmp1CD;Cyc_PP_cat(_tag_dyneither(_tmp1D0,sizeof(struct Cyc_PP_Doc*),3U));});_tmp1CD=_tmp61B;});{
struct Cyc_PP_Doc*_tmp1D2=({struct Cyc_PP_Doc*_tmp1D9[3U];({struct Cyc_PP_Doc*_tmp61C=Cyc_PP_text(({const char*_tmp1DB=")";_tag_dyneither(_tmp1DB,sizeof(char),2U);}));_tmp1D9[2U]=_tmp61C;});_tmp1D9[1U]=_tmp1CD;({struct Cyc_PP_Doc*_tmp61D=Cyc_PP_text(({const char*_tmp1DA="(";_tag_dyneither(_tmp1DA,sizeof(char),2U);}));_tmp1D9[0U]=_tmp61D;});Cyc_PP_cat(_tag_dyneither(_tmp1D9,sizeof(struct Cyc_PP_Doc*),3U));});
if(req != 0)
({struct Cyc_PP_Doc*_tmp621=({struct Cyc_PP_Doc*_tmp1D3[4U];({struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp1D5=")";_tag_dyneither(_tmp1D5,sizeof(char),2U);}));_tmp1D3[3U]=_tmp61E;});({struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_exp2doc(req);_tmp1D3[2U]=_tmp61F;});({struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(({const char*_tmp1D4=" @requires(";_tag_dyneither(_tmp1D4,sizeof(char),12U);}));_tmp1D3[1U]=_tmp620;});_tmp1D3[0U]=_tmp1D2;Cyc_PP_cat(_tag_dyneither(_tmp1D3,sizeof(struct Cyc_PP_Doc*),4U));});_tmp1D2=_tmp621;});
if(ens != 0)
({struct Cyc_PP_Doc*_tmp625=({struct Cyc_PP_Doc*_tmp1D6[4U];({struct Cyc_PP_Doc*_tmp622=Cyc_PP_text(({const char*_tmp1D8=")";_tag_dyneither(_tmp1D8,sizeof(char),2U);}));_tmp1D6[3U]=_tmp622;});({struct Cyc_PP_Doc*_tmp623=Cyc_Absynpp_exp2doc(ens);_tmp1D6[2U]=_tmp623;});({struct Cyc_PP_Doc*_tmp624=Cyc_PP_text(({const char*_tmp1D7=" @ensures(";_tag_dyneither(_tmp1D7,sizeof(char),11U);}));_tmp1D6[1U]=_tmp624;});_tmp1D6[0U]=_tmp1D2;Cyc_PP_cat(_tag_dyneither(_tmp1D6,sizeof(struct Cyc_PP_Doc*),4U));});_tmp1D2=_tmp625;});
return _tmp1D2;};};}
# 917
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1DF=arg;struct _dyneither_ptr*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;_LL1: _tmp1E3=_tmp1DF->f1;_tmp1E2=_tmp1DF->f2;_tmp1E1=_tmp1DF->f3;_LL2:;
return({struct _tuple8*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->f1=_tmp1E3;_tmp1E0->f2=_tmp1E2;_tmp1E0->f3=_tmp1E1;_tmp1E0;});}
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
({int _tmp626=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E9,Cyc_Absynpp_curr_namespace);match=_tmp626;});
# 936
goto _LL0;default: _LL7: _tmp1EA=(_tmp1E6.Abs_n).val;_LL8:
# 938
({int _tmp627=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EA,Cyc_Absynpp_curr_namespace);match=_tmp627;});
({struct Cyc_List_List*_tmp628=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=Cyc_Absynpp_cyc_stringptr;_tmp1E7->tl=_tmp1EA;_tmp1E7;}): _tmp1EA;_tmp1E4=_tmp628;});
goto _LL0;}_LL0:;}
# 942
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp62A=({struct Cyc_List_List*_tmp629=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp629,({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(*q).f2;_tmp1EB->tl=0;_tmp1EB;}));});Cyc_str_sepstr(_tmp62A,({const char*_tmp1EC="_";_tag_dyneither(_tmp1EC,sizeof(char),2U);}));});else{
# 947
if(match)
return*(*q).f2;else{
# 950
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp62C=({struct Cyc_List_List*_tmp62B=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62B,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(*q).f2;_tmp1ED->tl=0;_tmp1ED;}));});Cyc_str_sepstr(_tmp62C,({const char*_tmp1EE="::";_tag_dyneither(_tmp1EE,sizeof(char),3U);}));});}}}
# 954
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 958
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 962
return({struct _dyneither_ptr _tmp62F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp62E=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp62D=({const char*_tmp1F0="\\textbf{";_tag_dyneither(_tmp1F0,sizeof(char),9U);});Cyc_strconcat(_tmp62D,(struct _dyneither_ptr)_tmp1EF);});Cyc_strconcat(_tmp62E,({const char*_tmp1F1="}";_tag_dyneither(_tmp1F1,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp62F,(int)
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
 return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp630=({const char*_tmp1F4="/* bad namespace : */ ";_tag_dyneither(_tmp1F4,sizeof(char),23U);});Cyc_strconcat(_tmp630,(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1F5=(_tmp1F3.C_n).val;_LL6:
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
return({struct _dyneither_ptr _tmp633=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp632=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp631=({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp631,(struct _dyneither_ptr)_tmp1F7);});Cyc_strconcat(_tmp632,({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp633,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));});else{
return Cyc_PP_text(_tmp1F7);}}
# 1001
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp634=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp634,t,0);});}
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
({struct Cyc_PP_Doc*_tmp635=Cyc_Absynpp_cnst2doc(_tmp28D);s=_tmp635;});goto _LL0;case 1U: _LL3: _tmp28E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL4:
({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp28E));s=_tmp636;});goto _LL0;case 2U: _LL5: _tmp28F=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL6:
# 1091
({struct Cyc_PP_Doc*_tmp63B=({struct Cyc_PP_Doc*_tmp202[4U];({struct Cyc_PP_Doc*_tmp637=Cyc_PP_text(({const char*_tmp205=")";_tag_dyneither(_tmp205,sizeof(char),2U);}));_tmp202[3U]=_tmp637;});({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(_tmp28F);_tmp202[2U]=_tmp638;});({struct Cyc_PP_Doc*_tmp639=Cyc_PP_text(({const char*_tmp204="(";_tag_dyneither(_tmp204,sizeof(char),2U);}));_tmp202[1U]=_tmp639;});({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp203="__cyclone_pragma__";_tag_dyneither(_tmp203,sizeof(char),19U);}));_tmp202[0U]=_tmp63A;});Cyc_PP_cat(_tag_dyneither(_tmp202,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp63B;});goto _LL0;case 3U: _LL7: _tmp291=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp290=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL8:
({struct Cyc_PP_Doc*_tmp63C=Cyc_Absynpp_primapp2doc(myprec,_tmp291,_tmp290);s=_tmp63C;});goto _LL0;case 4U: _LL9: _tmp294=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp293=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp292=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LLA:
# 1094
({struct Cyc_PP_Doc*_tmp642=({struct Cyc_PP_Doc*_tmp206[5U];({struct Cyc_PP_Doc*_tmp63D=
# 1098
Cyc_Absynpp_exp2doc_prec(myprec,_tmp292);_tmp206[4U]=_tmp63D;});({struct Cyc_PP_Doc*_tmp63E=
# 1097
Cyc_PP_text(({const char*_tmp208="= ";_tag_dyneither(_tmp208,sizeof(char),3U);}));_tmp206[3U]=_tmp63E;});({struct Cyc_PP_Doc*_tmp63F=
# 1096
_tmp293 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp293->v);_tmp206[2U]=_tmp63F;});({struct Cyc_PP_Doc*_tmp640=
# 1095
Cyc_PP_text(({const char*_tmp207=" ";_tag_dyneither(_tmp207,sizeof(char),2U);}));_tmp206[1U]=_tmp640;});({struct Cyc_PP_Doc*_tmp641=
# 1094
Cyc_Absynpp_exp2doc_prec(myprec,_tmp294);_tmp206[0U]=_tmp641;});Cyc_PP_cat(_tag_dyneither(_tmp206,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp642;});
# 1099
goto _LL0;case 5U: _LLB: _tmp296=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp295=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LLC: {
# 1101
struct Cyc_PP_Doc*_tmp209=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296);
{enum Cyc_Absyn_Incrementor _tmp20A=_tmp295;switch(_tmp20A){case Cyc_Absyn_PreInc: _LL54: _LL55:
({struct Cyc_PP_Doc*_tmp644=({struct Cyc_PP_Doc*_tmp20B[2U];_tmp20B[1U]=_tmp209;({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp20C="++";_tag_dyneither(_tmp20C,sizeof(char),3U);}));_tmp20B[0U]=_tmp643;});Cyc_PP_cat(_tag_dyneither(_tmp20B,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp644;});goto _LL53;case Cyc_Absyn_PreDec: _LL56: _LL57:
({struct Cyc_PP_Doc*_tmp646=({struct Cyc_PP_Doc*_tmp20D[2U];_tmp20D[1U]=_tmp209;({struct Cyc_PP_Doc*_tmp645=Cyc_PP_text(({const char*_tmp20E="--";_tag_dyneither(_tmp20E,sizeof(char),3U);}));_tmp20D[0U]=_tmp645;});Cyc_PP_cat(_tag_dyneither(_tmp20D,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp646;});goto _LL53;case Cyc_Absyn_PostInc: _LL58: _LL59:
({struct Cyc_PP_Doc*_tmp648=({struct Cyc_PP_Doc*_tmp20F[2U];({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp210="++";_tag_dyneither(_tmp210,sizeof(char),3U);}));_tmp20F[1U]=_tmp647;});_tmp20F[0U]=_tmp209;Cyc_PP_cat(_tag_dyneither(_tmp20F,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp648;});goto _LL53;default: _LL5A: _LL5B:
({struct Cyc_PP_Doc*_tmp64A=({struct Cyc_PP_Doc*_tmp211[2U];({struct Cyc_PP_Doc*_tmp649=Cyc_PP_text(({const char*_tmp212="--";_tag_dyneither(_tmp212,sizeof(char),3U);}));_tmp211[1U]=_tmp649;});_tmp211[0U]=_tmp209;Cyc_PP_cat(_tag_dyneither(_tmp211,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp64A;});goto _LL53;}_LL53:;}
# 1108
goto _LL0;}case 6U: _LLD: _tmp299=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp298=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp297=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LLE:
# 1110
({struct Cyc_PP_Doc*_tmp650=({struct Cyc_PP_Doc*_tmp213[5U];({struct Cyc_PP_Doc*_tmp64B=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp297);_tmp213[4U]=_tmp64B;});({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(({const char*_tmp215=" : ";_tag_dyneither(_tmp215,sizeof(char),4U);}));_tmp213[3U]=_tmp64C;});({struct Cyc_PP_Doc*_tmp64D=
# 1110
Cyc_Absynpp_exp2doc_prec(0,_tmp298);_tmp213[2U]=_tmp64D;});({struct Cyc_PP_Doc*_tmp64E=Cyc_PP_text(({const char*_tmp214=" ? ";_tag_dyneither(_tmp214,sizeof(char),4U);}));_tmp213[1U]=_tmp64E;});({struct Cyc_PP_Doc*_tmp64F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp299);_tmp213[0U]=_tmp64F;});Cyc_PP_cat(_tag_dyneither(_tmp213,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp650;});
# 1112
goto _LL0;case 7U: _LLF: _tmp29B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL10:
# 1114
({struct Cyc_PP_Doc*_tmp654=({struct Cyc_PP_Doc*_tmp216[3U];({struct Cyc_PP_Doc*_tmp651=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29A);_tmp216[2U]=_tmp651;});({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp217=" && ";_tag_dyneither(_tmp217,sizeof(char),5U);}));_tmp216[1U]=_tmp652;});({struct Cyc_PP_Doc*_tmp653=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B);_tmp216[0U]=_tmp653;});Cyc_PP_cat(_tag_dyneither(_tmp216,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp654;});
goto _LL0;case 8U: _LL11: _tmp29D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL12:
# 1117
({struct Cyc_PP_Doc*_tmp658=({struct Cyc_PP_Doc*_tmp218[3U];({struct Cyc_PP_Doc*_tmp655=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29C);_tmp218[2U]=_tmp655;});({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp219=" || ";_tag_dyneither(_tmp219,sizeof(char),5U);}));_tmp218[1U]=_tmp656;});({struct Cyc_PP_Doc*_tmp657=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp218[0U]=_tmp657;});Cyc_PP_cat(_tag_dyneither(_tmp218,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp658;});
goto _LL0;case 9U: _LL13: _tmp29F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL14:
# 1122
({struct Cyc_PP_Doc*_tmp65E=({struct Cyc_PP_Doc*_tmp21A[5U];({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp21D=")";_tag_dyneither(_tmp21D,sizeof(char),2U);}));_tmp21A[4U]=_tmp659;});({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_exp2doc(_tmp29E);_tmp21A[3U]=_tmp65A;});({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp21C=", ";_tag_dyneither(_tmp21C,sizeof(char),3U);}));_tmp21A[2U]=_tmp65B;});({struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_exp2doc(_tmp29F);_tmp21A[1U]=_tmp65C;});({struct Cyc_PP_Doc*_tmp65D=Cyc_PP_text(({const char*_tmp21B="(";_tag_dyneither(_tmp21B,sizeof(char),2U);}));_tmp21A[0U]=_tmp65D;});Cyc_PP_cat(_tag_dyneither(_tmp21A,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp65E;});
goto _LL0;case 10U: _LL15: _tmp2A1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL16:
# 1125
({struct Cyc_PP_Doc*_tmp663=({struct Cyc_PP_Doc*_tmp21E[4U];({struct Cyc_PP_Doc*_tmp65F=
# 1128
Cyc_PP_text(({const char*_tmp220=")";_tag_dyneither(_tmp220,sizeof(char),2U);}));_tmp21E[3U]=_tmp65F;});({struct Cyc_PP_Doc*_tmp660=
# 1127
Cyc_Absynpp_exps2doc_prec(20,_tmp2A0);_tmp21E[2U]=_tmp660;});({struct Cyc_PP_Doc*_tmp661=
# 1126
Cyc_PP_text(({const char*_tmp21F="(";_tag_dyneither(_tmp21F,sizeof(char),2U);}));_tmp21E[1U]=_tmp661;});({struct Cyc_PP_Doc*_tmp662=
# 1125
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A1);_tmp21E[0U]=_tmp662;});Cyc_PP_cat(_tag_dyneither(_tmp21E,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp663;});
# 1129
goto _LL0;case 11U: _LL17: _tmp2A2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL18:
# 1131
({struct Cyc_PP_Doc*_tmp666=({struct Cyc_PP_Doc*_tmp221[2U];({struct Cyc_PP_Doc*_tmp664=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp221[1U]=_tmp664;});({struct Cyc_PP_Doc*_tmp665=Cyc_PP_text(({const char*_tmp222="throw ";_tag_dyneither(_tmp222,sizeof(char),7U);}));_tmp221[0U]=_tmp665;});Cyc_PP_cat(_tag_dyneither(_tmp221,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp666;});
goto _LL0;case 12U: _LL19: _tmp2A3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL1A:
# 1134
({struct Cyc_PP_Doc*_tmp667=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A3);s=_tmp667;});
goto _LL0;case 13U: _LL1B: _tmp2A4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL1C:
# 1137
({struct Cyc_PP_Doc*_tmp668=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A4);s=_tmp668;});
goto _LL0;case 14U: _LL1D: _tmp2A6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL1E:
# 1140
({struct Cyc_PP_Doc*_tmp66D=({struct Cyc_PP_Doc*_tmp223[4U];({struct Cyc_PP_Doc*_tmp669=
# 1143
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A5);_tmp223[3U]=_tmp669;});({struct Cyc_PP_Doc*_tmp66A=
# 1142
Cyc_PP_text(({const char*_tmp225=")";_tag_dyneither(_tmp225,sizeof(char),2U);}));_tmp223[2U]=_tmp66A;});({struct Cyc_PP_Doc*_tmp66B=
# 1141
Cyc_Absynpp_typ2doc(_tmp2A6);_tmp223[1U]=_tmp66B;});({struct Cyc_PP_Doc*_tmp66C=
# 1140
Cyc_PP_text(({const char*_tmp224="(";_tag_dyneither(_tmp224,sizeof(char),2U);}));_tmp223[0U]=_tmp66C;});Cyc_PP_cat(_tag_dyneither(_tmp223,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp66D;});
# 1144
goto _LL0;case 15U: _LL1F: _tmp2A7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL20:
# 1146
({struct Cyc_PP_Doc*_tmp670=({struct Cyc_PP_Doc*_tmp226[2U];({struct Cyc_PP_Doc*_tmp66E=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A7);_tmp226[1U]=_tmp66E;});({struct Cyc_PP_Doc*_tmp66F=
# 1146
Cyc_PP_text(({const char*_tmp227="&";_tag_dyneither(_tmp227,sizeof(char),2U);}));_tmp226[0U]=_tmp66F;});Cyc_PP_cat(_tag_dyneither(_tmp226,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp670;});
# 1148
goto _LL0;case 16U: _LL21: _tmp2A9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22:
# 1150
 if(_tmp2A9 == 0)
({struct Cyc_PP_Doc*_tmp673=({struct Cyc_PP_Doc*_tmp228[2U];({struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp228[1U]=_tmp671;});({struct Cyc_PP_Doc*_tmp672=Cyc_PP_text(({const char*_tmp229="new ";_tag_dyneither(_tmp229,sizeof(char),5U);}));_tmp228[0U]=_tmp672;});Cyc_PP_cat(_tag_dyneither(_tmp228,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp673;});else{
# 1153
({struct Cyc_PP_Doc*_tmp678=({struct Cyc_PP_Doc*_tmp22A[4U];({struct Cyc_PP_Doc*_tmp674=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp22A[3U]=_tmp674;});({struct Cyc_PP_Doc*_tmp675=
# 1153
Cyc_PP_text(({const char*_tmp22C=") ";_tag_dyneither(_tmp22C,sizeof(char),3U);}));_tmp22A[2U]=_tmp675;});({struct Cyc_PP_Doc*_tmp676=Cyc_Absynpp_exp2doc(_tmp2A9);_tmp22A[1U]=_tmp676;});({struct Cyc_PP_Doc*_tmp677=Cyc_PP_text(({const char*_tmp22B="rnew(";_tag_dyneither(_tmp22B,sizeof(char),6U);}));_tmp22A[0U]=_tmp677;});Cyc_PP_cat(_tag_dyneither(_tmp22A,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp678;});}
# 1155
goto _LL0;case 17U: _LL23: _tmp2AA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL24:
({struct Cyc_PP_Doc*_tmp67C=({struct Cyc_PP_Doc*_tmp22D[3U];({struct Cyc_PP_Doc*_tmp679=Cyc_PP_text(({const char*_tmp22F=")";_tag_dyneither(_tmp22F,sizeof(char),2U);}));_tmp22D[2U]=_tmp679;});({struct Cyc_PP_Doc*_tmp67A=Cyc_Absynpp_typ2doc(_tmp2AA);_tmp22D[1U]=_tmp67A;});({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp22E="sizeof(";_tag_dyneither(_tmp22E,sizeof(char),8U);}));_tmp22D[0U]=_tmp67B;});Cyc_PP_cat(_tag_dyneither(_tmp22D,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp67C;});goto _LL0;case 18U: _LL25: _tmp2AB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL26:
({struct Cyc_PP_Doc*_tmp680=({struct Cyc_PP_Doc*_tmp230[3U];({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp232=")";_tag_dyneither(_tmp232,sizeof(char),2U);}));_tmp230[2U]=_tmp67D;});({struct Cyc_PP_Doc*_tmp67E=Cyc_Absynpp_exp2doc(_tmp2AB);_tmp230[1U]=_tmp67E;});({struct Cyc_PP_Doc*_tmp67F=Cyc_PP_text(({const char*_tmp231="sizeof(";_tag_dyneither(_tmp231,sizeof(char),8U);}));_tmp230[0U]=_tmp67F;});Cyc_PP_cat(_tag_dyneither(_tmp230,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp680;});goto _LL0;case 39U: _LL27: _tmp2AC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL28:
({struct Cyc_PP_Doc*_tmp684=({struct Cyc_PP_Doc*_tmp233[3U];({struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp235=")";_tag_dyneither(_tmp235,sizeof(char),2U);}));_tmp233[2U]=_tmp681;});({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_typ2doc(_tmp2AC);_tmp233[1U]=_tmp682;});({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp234="valueof(";_tag_dyneither(_tmp234,sizeof(char),9U);}));_tmp233[0U]=_tmp683;});Cyc_PP_cat(_tag_dyneither(_tmp233,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp684;});goto _LL0;case 40U: _LL29: _tmp2AE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2A:
# 1160
 if(_tmp2AE)
({struct Cyc_PP_Doc*_tmp689=({struct Cyc_PP_Doc*_tmp236[4U];({struct Cyc_PP_Doc*_tmp685=Cyc_PP_text(({const char*_tmp239=")";_tag_dyneither(_tmp239,sizeof(char),2U);}));_tmp236[3U]=_tmp685;});({struct Cyc_PP_Doc*_tmp686=Cyc_PP_text(_tmp2AD);_tmp236[2U]=_tmp686;});({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp238=" volatile (";_tag_dyneither(_tmp238,sizeof(char),12U);}));_tmp236[1U]=_tmp687;});({struct Cyc_PP_Doc*_tmp688=Cyc_PP_text(({const char*_tmp237="__asm__";_tag_dyneither(_tmp237,sizeof(char),8U);}));_tmp236[0U]=_tmp688;});Cyc_PP_cat(_tag_dyneither(_tmp236,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp689;});else{
# 1163
({struct Cyc_PP_Doc*_tmp68D=({struct Cyc_PP_Doc*_tmp23A[3U];({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(({const char*_tmp23C=")";_tag_dyneither(_tmp23C,sizeof(char),2U);}));_tmp23A[2U]=_tmp68A;});({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(_tmp2AD);_tmp23A[1U]=_tmp68B;});({struct Cyc_PP_Doc*_tmp68C=Cyc_PP_text(({const char*_tmp23B="__asm__(";_tag_dyneither(_tmp23B,sizeof(char),9U);}));_tmp23A[0U]=_tmp68C;});Cyc_PP_cat(_tag_dyneither(_tmp23A,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp68D;});}
goto _LL0;case 38U: _LL2B: _tmp2B0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2C:
# 1166
({struct Cyc_PP_Doc*_tmp693=({struct Cyc_PP_Doc*_tmp23D[5U];({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_text(({const char*_tmp240=")";_tag_dyneither(_tmp240,sizeof(char),2U);}));_tmp23D[4U]=_tmp68E;});({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_textptr(_tmp2AF);_tmp23D[3U]=_tmp68F;});({struct Cyc_PP_Doc*_tmp690=Cyc_PP_text(({const char*_tmp23F=".";_tag_dyneither(_tmp23F,sizeof(char),2U);}));_tmp23D[2U]=_tmp690;});({struct Cyc_PP_Doc*_tmp691=Cyc_Absynpp_exp2doc(_tmp2B0);_tmp23D[1U]=_tmp691;});({struct Cyc_PP_Doc*_tmp692=Cyc_PP_text(({const char*_tmp23E="tagcheck(";_tag_dyneither(_tmp23E,sizeof(char),10U);}));_tmp23D[0U]=_tmp692;});Cyc_PP_cat(_tag_dyneither(_tmp23D,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp693;});
goto _LL0;case 19U: _LL2D: _tmp2B2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B1=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2E:
# 1169
({struct Cyc_PP_Doc*_tmp697=({struct Cyc_PP_Doc*_tmp241[3U];({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text(({const char*_tmp243=",";_tag_dyneither(_tmp243,sizeof(char),2U);}));_tmp241[2U]=_tmp694;});({struct Cyc_PP_Doc*_tmp695=Cyc_Absynpp_typ2doc(_tmp2B2);_tmp241[1U]=_tmp695;});({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp242="offsetof(";_tag_dyneither(_tmp242,sizeof(char),10U);}));_tmp241[0U]=_tmp696;});Cyc_PP_cat(_tag_dyneither(_tmp241,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp697;});
for(0;_tmp2B1 != 0;_tmp2B1=_tmp2B1->tl){
void*_tmp244=(void*)_tmp2B1->hd;void*_tmp245=_tmp244;unsigned int _tmp24E;struct _dyneither_ptr*_tmp24D;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp245)->tag == 0U){_LL5D: _tmp24D=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp245)->f1;_LL5E:
({struct Cyc_PP_Doc*_tmp69A=({struct Cyc_PP_Doc*_tmp246[3U];({struct Cyc_PP_Doc*_tmp698=
_tmp2B1->tl != 0?Cyc_PP_text(({const char*_tmp247=".";_tag_dyneither(_tmp247,sizeof(char),2U);})): Cyc_PP_nil_doc();_tmp246[2U]=_tmp698;});({struct Cyc_PP_Doc*_tmp699=
# 1172
Cyc_PP_textptr(_tmp24D);_tmp246[1U]=_tmp699;});_tmp246[0U]=s;Cyc_PP_cat(_tag_dyneither(_tmp246,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp69A;});
# 1174
goto _LL5C;}else{_LL5F: _tmp24E=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp245)->f1;_LL60:
({struct Cyc_PP_Doc*_tmp69E=({struct Cyc_PP_Doc*_tmp248[3U];({struct Cyc_PP_Doc*_tmp69B=
_tmp2B1->tl != 0?Cyc_PP_text(({const char*_tmp24C=".";_tag_dyneither(_tmp24C,sizeof(char),2U);})): Cyc_PP_nil_doc();_tmp248[2U]=_tmp69B;});({struct Cyc_PP_Doc*_tmp69D=
# 1175
Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp24B;_tmp24B.tag=1U;_tmp24B.f1=(unsigned long)((int)_tmp24E);({void*_tmp249[1U]={& _tmp24B};({struct _dyneither_ptr _tmp69C=({const char*_tmp24A="%d";_tag_dyneither(_tmp24A,sizeof(char),3U);});Cyc_aprintf(_tmp69C,_tag_dyneither(_tmp249,sizeof(void*),1U));});});}));_tmp248[1U]=_tmp69D;});_tmp248[0U]=s;Cyc_PP_cat(_tag_dyneither(_tmp248,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp69E;});
# 1177
goto _LL5C;}_LL5C:;}
# 1179
({struct Cyc_PP_Doc*_tmp6A0=({struct Cyc_PP_Doc*_tmp24F[2U];({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp250=")";_tag_dyneither(_tmp250,sizeof(char),2U);}));_tmp24F[1U]=_tmp69F;});_tmp24F[0U]=s;Cyc_PP_cat(_tag_dyneither(_tmp24F,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp6A0;});
goto _LL0;case 20U: _LL2F: _tmp2B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL30:
# 1182
({struct Cyc_PP_Doc*_tmp6A3=({struct Cyc_PP_Doc*_tmp251[2U];({struct Cyc_PP_Doc*_tmp6A1=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3);_tmp251[1U]=_tmp6A1;});({struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(({const char*_tmp252="*";_tag_dyneither(_tmp252,sizeof(char),2U);}));_tmp251[0U]=_tmp6A2;});Cyc_PP_cat(_tag_dyneither(_tmp251,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp6A3;});
goto _LL0;case 21U: _LL31: _tmp2B5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL32:
# 1185
({struct Cyc_PP_Doc*_tmp6A7=({struct Cyc_PP_Doc*_tmp253[3U];({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_textptr(_tmp2B4);_tmp253[2U]=_tmp6A4;});({struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_text(({const char*_tmp254=".";_tag_dyneither(_tmp254,sizeof(char),2U);}));_tmp253[1U]=_tmp6A5;});({struct Cyc_PP_Doc*_tmp6A6=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5);_tmp253[0U]=_tmp6A6;});Cyc_PP_cat(_tag_dyneither(_tmp253,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp6A7;});
goto _LL0;case 22U: _LL33: _tmp2B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL34:
# 1188
({struct Cyc_PP_Doc*_tmp6AB=({struct Cyc_PP_Doc*_tmp255[3U];({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_textptr(_tmp2B6);_tmp255[2U]=_tmp6A8;});({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp256="->";_tag_dyneither(_tmp256,sizeof(char),3U);}));_tmp255[1U]=_tmp6A9;});({struct Cyc_PP_Doc*_tmp6AA=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B7);_tmp255[0U]=_tmp6AA;});Cyc_PP_cat(_tag_dyneither(_tmp255,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp6AB;});
goto _LL0;case 23U: _LL35: _tmp2B9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL36:
# 1191
({struct Cyc_PP_Doc*_tmp6B0=({struct Cyc_PP_Doc*_tmp257[4U];({struct Cyc_PP_Doc*_tmp6AC=
# 1194
Cyc_PP_text(({const char*_tmp259="]";_tag_dyneither(_tmp259,sizeof(char),2U);}));_tmp257[3U]=_tmp6AC;});({struct Cyc_PP_Doc*_tmp6AD=
# 1193
Cyc_Absynpp_exp2doc(_tmp2B8);_tmp257[2U]=_tmp6AD;});({struct Cyc_PP_Doc*_tmp6AE=
# 1192
Cyc_PP_text(({const char*_tmp258="[";_tag_dyneither(_tmp258,sizeof(char),2U);}));_tmp257[1U]=_tmp6AE;});({struct Cyc_PP_Doc*_tmp6AF=
# 1191
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B9);_tmp257[0U]=_tmp6AF;});Cyc_PP_cat(_tag_dyneither(_tmp257,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp6B0;});
# 1195
goto _LL0;case 24U: _LL37: _tmp2BA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL38:
# 1197
({struct Cyc_PP_Doc*_tmp6B5=({struct Cyc_PP_Doc*_tmp25A[4U];({struct Cyc_PP_Doc*_tmp6B1=
# 1200
Cyc_PP_text(({const char*_tmp25C=")";_tag_dyneither(_tmp25C,sizeof(char),2U);}));_tmp25A[3U]=_tmp6B1;});({struct Cyc_PP_Doc*_tmp6B2=
# 1199
Cyc_Absynpp_exps2doc_prec(20,_tmp2BA);_tmp25A[2U]=_tmp6B2;});({struct Cyc_PP_Doc*_tmp6B3=
# 1198
Cyc_PP_text(({const char*_tmp25B="(";_tag_dyneither(_tmp25B,sizeof(char),2U);}));_tmp25A[1U]=_tmp6B3;});({struct Cyc_PP_Doc*_tmp6B4=
# 1197
Cyc_Absynpp_dollar();_tmp25A[0U]=_tmp6B4;});Cyc_PP_cat(_tag_dyneither(_tmp25A,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp6B5;});
# 1201
goto _LL0;case 25U: _LL39: _tmp2BC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL3A:
# 1203
({struct Cyc_PP_Doc*_tmp6BB=({struct Cyc_PP_Doc*_tmp25D[4U];({struct Cyc_PP_Doc*_tmp6B7=({
# 1206
struct _dyneither_ptr _tmp6B6=({const char*_tmp260=",";_tag_dyneither(_tmp260,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6B6,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BB));});_tmp25D[3U]=_tmp6B7;});({struct Cyc_PP_Doc*_tmp6B8=
# 1205
Cyc_PP_text(({const char*_tmp25F=")";_tag_dyneither(_tmp25F,sizeof(char),2U);}));_tmp25D[2U]=_tmp6B8;});({struct Cyc_PP_Doc*_tmp6B9=
# 1204
Cyc_Absynpp_typ2doc((*_tmp2BC).f3);_tmp25D[1U]=_tmp6B9;});({struct Cyc_PP_Doc*_tmp6BA=
# 1203
Cyc_PP_text(({const char*_tmp25E="(";_tag_dyneither(_tmp25E,sizeof(char),2U);}));_tmp25D[0U]=_tmp6BA;});Cyc_PP_cat(_tag_dyneither(_tmp25D,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp6BB;});
# 1207
goto _LL0;case 26U: _LL3B: _tmp2BD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL3C:
# 1209
({struct Cyc_PP_Doc*_tmp6BD=({struct _dyneither_ptr _tmp6BC=({const char*_tmp261=",";_tag_dyneither(_tmp261,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6BC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BD));});s=_tmp6BD;});
goto _LL0;case 27U: _LL3D: _tmp2C0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2BE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL3E:
# 1212
({struct Cyc_PP_Doc*_tmp6C6=({struct Cyc_PP_Doc*_tmp262[8U];({struct Cyc_PP_Doc*_tmp6BE=
# 1218
Cyc_Absynpp_rb();_tmp262[7U]=_tmp6BE;});({struct Cyc_PP_Doc*_tmp6BF=
# 1217
Cyc_Absynpp_exp2doc(_tmp2BE);_tmp262[6U]=_tmp6BF;});({struct Cyc_PP_Doc*_tmp6C0=
# 1216
Cyc_PP_text(({const char*_tmp265=" : ";_tag_dyneither(_tmp265,sizeof(char),4U);}));_tmp262[5U]=_tmp6C0;});({struct Cyc_PP_Doc*_tmp6C1=
# 1215
Cyc_Absynpp_exp2doc(_tmp2BF);_tmp262[4U]=_tmp6C1;});({struct Cyc_PP_Doc*_tmp6C2=
# 1214
Cyc_PP_text(({const char*_tmp264=" < ";_tag_dyneither(_tmp264,sizeof(char),4U);}));_tmp262[3U]=_tmp6C2;});({struct Cyc_PP_Doc*_tmp6C3=
# 1213
Cyc_PP_text(*(*_tmp2C0->name).f2);_tmp262[2U]=_tmp6C3;});({struct Cyc_PP_Doc*_tmp6C4=
# 1212
Cyc_PP_text(({const char*_tmp263="for ";_tag_dyneither(_tmp263,sizeof(char),5U);}));_tmp262[1U]=_tmp6C4;});({struct Cyc_PP_Doc*_tmp6C5=Cyc_Absynpp_lb();_tmp262[0U]=_tmp6C5;});Cyc_PP_cat(_tag_dyneither(_tmp262,sizeof(struct Cyc_PP_Doc*),8U));});s=_tmp6C6;});
# 1219
goto _LL0;case 28U: _LL3F: _tmp2C2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C1=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL40:
# 1221
({struct Cyc_PP_Doc*_tmp6CE=({struct Cyc_PP_Doc*_tmp266[7U];({struct Cyc_PP_Doc*_tmp6C7=
# 1226
Cyc_Absynpp_rb();_tmp266[6U]=_tmp6C7;});({struct Cyc_PP_Doc*_tmp6C8=
# 1225
Cyc_Absynpp_typ2doc(_tmp2C1);_tmp266[5U]=_tmp6C8;});({struct Cyc_PP_Doc*_tmp6C9=
# 1224
Cyc_PP_text(({const char*_tmp269=" : ";_tag_dyneither(_tmp269,sizeof(char),4U);}));_tmp266[4U]=_tmp6C9;});({struct Cyc_PP_Doc*_tmp6CA=
# 1223
Cyc_Absynpp_exp2doc(_tmp2C2);_tmp266[3U]=_tmp6CA;});({struct Cyc_PP_Doc*_tmp6CB=
# 1222
Cyc_PP_text(({const char*_tmp268=" < ";_tag_dyneither(_tmp268,sizeof(char),4U);}));_tmp266[2U]=_tmp6CB;});({struct Cyc_PP_Doc*_tmp6CC=
# 1221
Cyc_PP_text(({const char*_tmp267="for x ";_tag_dyneither(_tmp267,sizeof(char),7U);}));_tmp266[1U]=_tmp6CC;});({struct Cyc_PP_Doc*_tmp6CD=Cyc_Absynpp_lb();_tmp266[0U]=_tmp6CD;});Cyc_PP_cat(_tag_dyneither(_tmp266,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp6CE;});
# 1227
goto _LL0;case 29U: _LL41: _tmp2C5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2C3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL42: {
# 1229
struct Cyc_List_List*_tmp26A=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C3);
({struct Cyc_PP_Doc*_tmp6D3=({struct Cyc_PP_Doc*_tmp26B[2U];({struct Cyc_PP_Doc*_tmp6D1=({
struct _dyneither_ptr _tmp6D0=({const char*_tmp26C=",";_tag_dyneither(_tmp26C,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6D0,
_tmp2C4 != 0?({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));({struct Cyc_PP_Doc*_tmp6CF=Cyc_Absynpp_tps2doc(_tmp2C4);_tmp26D->hd=_tmp6CF;});_tmp26D->tl=_tmp26A;_tmp26D;}): _tmp26A);});_tmp26B[1U]=_tmp6D1;});({struct Cyc_PP_Doc*_tmp6D2=
# 1230
Cyc_Absynpp_qvar2doc(_tmp2C5);_tmp26B[0U]=_tmp6D2;});Cyc_PP_cat(_tag_dyneither(_tmp26B,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp6D3;});
# 1233
goto _LL0;}case 30U: _LL43: _tmp2C6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL44:
# 1235
({struct Cyc_PP_Doc*_tmp6D5=({struct _dyneither_ptr _tmp6D4=({const char*_tmp26E=",";_tag_dyneither(_tmp26E,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6D4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C6));});s=_tmp6D5;});
goto _LL0;case 31U: _LL45: _tmp2C8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL46:
# 1238
 if(_tmp2C8 == 0)
# 1240
({struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_qvar2doc(_tmp2C7->name);s=_tmp6D6;});else{
# 1242
({struct Cyc_PP_Doc*_tmp6DC=({struct Cyc_PP_Doc*_tmp26F[2U];({struct Cyc_PP_Doc*_tmp6DA=({
struct _dyneither_ptr _tmp6D9=({const char*_tmp270="(";_tag_dyneither(_tmp270,sizeof(char),2U);});struct _dyneither_ptr _tmp6D8=({const char*_tmp271=")";_tag_dyneither(_tmp271,sizeof(char),2U);});struct _dyneither_ptr _tmp6D7=({const char*_tmp272=",";_tag_dyneither(_tmp272,sizeof(char),2U);});Cyc_PP_egroup(_tmp6D9,_tmp6D8,_tmp6D7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C8));});_tmp26F[1U]=_tmp6DA;});({struct Cyc_PP_Doc*_tmp6DB=
# 1242
Cyc_Absynpp_qvar2doc(_tmp2C7->name);_tmp26F[0U]=_tmp6DB;});Cyc_PP_cat(_tag_dyneither(_tmp26F,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp6DC;});}
# 1244
goto _LL0;case 32U: _LL47: _tmp2C9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL48:
# 1246
({struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_qvar2doc(_tmp2C9->name);s=_tmp6DD;});
goto _LL0;case 33U: _LL49: _tmp2CA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL4A:
# 1249
({struct Cyc_PP_Doc*_tmp6DE=Cyc_Absynpp_qvar2doc(_tmp2CA->name);s=_tmp6DE;});
goto _LL0;case 34U: _LL4B: _tmp2CF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp2CE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp2CD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp2CC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp2CB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL4C:
# 1254
 if(_tmp2CF){
# 1256
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2CD)),0U);
if(_tmp2CE == 0)
({struct Cyc_PP_Doc*_tmp6E4=({struct Cyc_PP_Doc*_tmp273[5U];({struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_text(({const char*_tmp276=")";_tag_dyneither(_tmp276,sizeof(char),2U);}));_tmp273[4U]=_tmp6DF;});({struct Cyc_PP_Doc*_tmp6E0=Cyc_Absynpp_exp2doc(st);_tmp273[3U]=_tmp6E0;});({struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp275=",";_tag_dyneither(_tmp275,sizeof(char),2U);}));_tmp273[2U]=_tmp6E1;});({struct Cyc_PP_Doc*_tmp6E2=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp273[1U]=_tmp6E2;});({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_text(({const char*_tmp274="calloc(";_tag_dyneither(_tmp274,sizeof(char),8U);}));_tmp273[0U]=_tmp6E3;});Cyc_PP_cat(_tag_dyneither(_tmp273,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp6E4;});else{
# 1260
({struct Cyc_PP_Doc*_tmp6EC=({struct Cyc_PP_Doc*_tmp277[7U];({struct Cyc_PP_Doc*_tmp6E5=
Cyc_PP_text(({const char*_tmp27B=")";_tag_dyneither(_tmp27B,sizeof(char),2U);}));_tmp277[6U]=_tmp6E5;});({struct Cyc_PP_Doc*_tmp6E6=Cyc_Absynpp_exp2doc(st);_tmp277[5U]=_tmp6E6;});({struct Cyc_PP_Doc*_tmp6E7=Cyc_PP_text(({const char*_tmp27A=",";_tag_dyneither(_tmp27A,sizeof(char),2U);}));_tmp277[4U]=_tmp6E7;});({struct Cyc_PP_Doc*_tmp6E8=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp277[3U]=_tmp6E8;});({struct Cyc_PP_Doc*_tmp6E9=
# 1260
Cyc_PP_text(({const char*_tmp279=",";_tag_dyneither(_tmp279,sizeof(char),2U);}));_tmp277[2U]=_tmp6E9;});({struct Cyc_PP_Doc*_tmp6EA=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp277[1U]=_tmp6EA;});({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_text(({const char*_tmp278="rcalloc(";_tag_dyneither(_tmp278,sizeof(char),9U);}));_tmp277[0U]=_tmp6EB;});Cyc_PP_cat(_tag_dyneither(_tmp277,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp6EC;});}}else{
# 1263
struct Cyc_Absyn_Exp*new_e;
# 1265
if(_tmp2CD == 0)
new_e=_tmp2CC;else{
# 1268
({struct Cyc_Absyn_Exp*_tmp6EE=({struct Cyc_Absyn_Exp*_tmp6ED=Cyc_Absyn_sizeoftyp_exp(*_tmp2CD,0U);Cyc_Absyn_times_exp(_tmp6ED,_tmp2CC,0U);});new_e=_tmp6EE;});}
# 1270
if(_tmp2CE == 0)
({struct Cyc_PP_Doc*_tmp6F2=({struct Cyc_PP_Doc*_tmp27C[3U];({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_text(({const char*_tmp27E=")";_tag_dyneither(_tmp27E,sizeof(char),2U);}));_tmp27C[2U]=_tmp6EF;});({struct Cyc_PP_Doc*_tmp6F0=Cyc_Absynpp_exp2doc(new_e);_tmp27C[1U]=_tmp6F0;});({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_text(({const char*_tmp27D="malloc(";_tag_dyneither(_tmp27D,sizeof(char),8U);}));_tmp27C[0U]=_tmp6F1;});Cyc_PP_cat(_tag_dyneither(_tmp27C,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp6F2;});else{
# 1273
if(_tmp2CB)
({struct Cyc_PP_Doc*_tmp6F8=({struct Cyc_PP_Doc*_tmp27F[5U];({struct Cyc_PP_Doc*_tmp6F3=
Cyc_PP_text(({const char*_tmp282=")";_tag_dyneither(_tmp282,sizeof(char),2U);}));_tmp27F[4U]=_tmp6F3;});({struct Cyc_PP_Doc*_tmp6F4=Cyc_Absynpp_exp2doc(new_e);_tmp27F[3U]=_tmp6F4;});({struct Cyc_PP_Doc*_tmp6F5=
# 1274
Cyc_PP_text(({const char*_tmp281=",";_tag_dyneither(_tmp281,sizeof(char),2U);}));_tmp27F[2U]=_tmp6F5;});({struct Cyc_PP_Doc*_tmp6F6=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp27F[1U]=_tmp6F6;});({struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp280="rmalloc_inline(";_tag_dyneither(_tmp280,sizeof(char),16U);}));_tmp27F[0U]=_tmp6F7;});Cyc_PP_cat(_tag_dyneither(_tmp27F,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp6F8;});else{
# 1277
({struct Cyc_PP_Doc*_tmp6FE=({struct Cyc_PP_Doc*_tmp283[5U];({struct Cyc_PP_Doc*_tmp6F9=
Cyc_PP_text(({const char*_tmp286=")";_tag_dyneither(_tmp286,sizeof(char),2U);}));_tmp283[4U]=_tmp6F9;});({struct Cyc_PP_Doc*_tmp6FA=Cyc_Absynpp_exp2doc(new_e);_tmp283[3U]=_tmp6FA;});({struct Cyc_PP_Doc*_tmp6FB=
# 1277
Cyc_PP_text(({const char*_tmp285=",";_tag_dyneither(_tmp285,sizeof(char),2U);}));_tmp283[2U]=_tmp6FB;});({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp283[1U]=_tmp6FC;});({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp284="rmalloc(";_tag_dyneither(_tmp284,sizeof(char),9U);}));_tmp283[0U]=_tmp6FD;});Cyc_PP_cat(_tag_dyneither(_tmp283,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp6FE;});}}}
# 1281
goto _LL0;case 35U: _LL4D: _tmp2D1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL4E:
# 1283
({struct Cyc_PP_Doc*_tmp702=({struct Cyc_PP_Doc*_tmp287[3U];({struct Cyc_PP_Doc*_tmp6FF=
# 1285
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D0);_tmp287[2U]=_tmp6FF;});({struct Cyc_PP_Doc*_tmp700=
# 1284
Cyc_PP_text(({const char*_tmp288=" :=: ";_tag_dyneither(_tmp288,sizeof(char),6U);}));_tmp287[1U]=_tmp700;});({struct Cyc_PP_Doc*_tmp701=
# 1283
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D1);_tmp287[0U]=_tmp701;});Cyc_PP_cat(_tag_dyneither(_tmp287,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp702;});
# 1286
goto _LL0;case 36U: _LL4F: _tmp2D3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL50:
# 1289
({struct Cyc_PP_Doc*_tmp704=({struct _dyneither_ptr _tmp703=({const char*_tmp289=",";_tag_dyneither(_tmp289,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp703,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D2));});s=_tmp704;});
goto _LL0;default: _LL51: _tmp2D4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL52:
# 1292
({struct Cyc_PP_Doc*_tmp70C=({struct Cyc_PP_Doc*_tmp28A[7U];({struct Cyc_PP_Doc*_tmp705=
# 1294
Cyc_PP_text(({const char*_tmp28C=")";_tag_dyneither(_tmp28C,sizeof(char),2U);}));_tmp28A[6U]=_tmp705;});({struct Cyc_PP_Doc*_tmp706=Cyc_Absynpp_rb();_tmp28A[5U]=_tmp706;});({struct Cyc_PP_Doc*_tmp707=Cyc_PP_blank_doc();_tmp28A[4U]=_tmp707;});({struct Cyc_PP_Doc*_tmp708=
# 1293
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D4,1));_tmp28A[3U]=_tmp708;});({struct Cyc_PP_Doc*_tmp709=
# 1292
Cyc_PP_blank_doc();_tmp28A[2U]=_tmp709;});({struct Cyc_PP_Doc*_tmp70A=Cyc_Absynpp_lb();_tmp28A[1U]=_tmp70A;});({struct Cyc_PP_Doc*_tmp70B=Cyc_PP_text(({const char*_tmp28B="(";_tag_dyneither(_tmp28B,sizeof(char),2U);}));_tmp28A[0U]=_tmp70B;});Cyc_PP_cat(_tag_dyneither(_tmp28A,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp70C;});
# 1295
goto _LL0;}_LL0:;}
# 1297
if(inprec >= myprec)
({struct Cyc_PP_Doc*_tmp70F=({struct Cyc_PP_Doc*_tmp2D5[3U];({struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp2D7=")";_tag_dyneither(_tmp2D7,sizeof(char),2U);}));_tmp2D5[2U]=_tmp70D;});_tmp2D5[1U]=s;({struct Cyc_PP_Doc*_tmp70E=Cyc_PP_text(({const char*_tmp2D6="(";_tag_dyneither(_tmp2D6,sizeof(char),2U);}));_tmp2D5[0U]=_tmp70E;});Cyc_PP_cat(_tag_dyneither(_tmp2D5,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp70F;});
return s;}
# 1302
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2D8=d;struct _dyneither_ptr*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->tag == 0U){_LL1: _tmp2DE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->f1;_LL2:
 return({struct Cyc_PP_Doc*_tmp2D9[3U];({struct Cyc_PP_Doc*_tmp710=Cyc_PP_text(({const char*_tmp2DB="]";_tag_dyneither(_tmp2DB,sizeof(char),2U);}));_tmp2D9[2U]=_tmp710;});({struct Cyc_PP_Doc*_tmp711=Cyc_Absynpp_exp2doc(_tmp2DE);_tmp2D9[1U]=_tmp711;});({struct Cyc_PP_Doc*_tmp712=Cyc_PP_text(({const char*_tmp2DA=".[";_tag_dyneither(_tmp2DA,sizeof(char),3U);}));_tmp2D9[0U]=_tmp712;});Cyc_PP_cat(_tag_dyneither(_tmp2D9,sizeof(struct Cyc_PP_Doc*),3U));});}else{_LL3: _tmp2DF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D8)->f1;_LL4:
 return({struct Cyc_PP_Doc*_tmp2DC[2U];({struct Cyc_PP_Doc*_tmp713=Cyc_PP_textptr(_tmp2DF);_tmp2DC[1U]=_tmp713;});({struct Cyc_PP_Doc*_tmp714=Cyc_PP_text(({const char*_tmp2DD=".";_tag_dyneither(_tmp2DD,sizeof(char),2U);}));_tmp2DC[0U]=_tmp714;});Cyc_PP_cat(_tag_dyneither(_tmp2DC,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}
# 1309
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2E0[2U];({struct Cyc_PP_Doc*_tmp715=
Cyc_Absynpp_exp2doc((*de).f2);_tmp2E0[1U]=_tmp715;});({struct Cyc_PP_Doc*_tmp719=({
# 1311
struct _dyneither_ptr _tmp718=({const char*_tmp2E1="";_tag_dyneither(_tmp2E1,sizeof(char),1U);});struct _dyneither_ptr _tmp717=({const char*_tmp2E2="=";_tag_dyneither(_tmp2E2,sizeof(char),2U);});struct _dyneither_ptr _tmp716=({const char*_tmp2E3="=";_tag_dyneither(_tmp2E3,sizeof(char),2U);});Cyc_PP_egroup(_tmp718,_tmp717,_tmp716,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2E0[0U]=_tmp719;});Cyc_PP_cat(_tag_dyneither(_tmp2E0,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1315
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _dyneither_ptr _tmp71C=({const char*_tmp2E4="";_tag_dyneither(_tmp2E4,sizeof(char),1U);});struct _dyneither_ptr _tmp71B=({const char*_tmp2E5="";_tag_dyneither(_tmp2E5,sizeof(char),1U);});struct _dyneither_ptr _tmp71A=({const char*_tmp2E6=",";_tag_dyneither(_tmp2E6,sizeof(char),2U);});Cyc_PP_group(_tmp71C,_tmp71B,_tmp71A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1319
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp2FC=({unsigned int _tmp2F9=(unsigned int)28;char*_tmp2FA=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp2F9 + 1U));{unsigned int _tmp2FB=_tmp2F9;unsigned int i;for(i=0;i < _tmp2FB;i ++){_tmp2FA[i]=(char)'z';}_tmp2FA[_tmp2FB]=(char)0U;}_tmp2FA;});_tag_dyneither(_tmp2FC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FC,(unsigned int)28 + 1U));});
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
union Cyc_Absyn_Cnst _tmp2FD=c;struct _dyneither_ptr _tmp31F;struct _dyneither_ptr _tmp31E;struct _dyneither_ptr _tmp31D;enum Cyc_Absyn_Sign _tmp31C;long long _tmp31B;enum Cyc_Absyn_Sign _tmp31A;int _tmp319;enum Cyc_Absyn_Sign _tmp318;short _tmp317;struct _dyneither_ptr _tmp316;enum Cyc_Absyn_Sign _tmp315;char _tmp314;switch((_tmp2FD.String_c).tag){case 2U: _LL1: _tmp315=((_tmp2FD.Char_c).val).f1;_tmp314=((_tmp2FD.Char_c).val).f2;_LL2:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp300;_tmp300.tag=0U;({struct _dyneither_ptr _tmp71D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp314));_tmp300.f1=_tmp71D;});({void*_tmp2FE[1U]={& _tmp300};({struct _dyneither_ptr _tmp71E=({const char*_tmp2FF="'%s'";_tag_dyneither(_tmp2FF,sizeof(char),5U);});Cyc_aprintf(_tmp71E,_tag_dyneither(_tmp2FE,sizeof(void*),1U));});});}));case 3U: _LL3: _tmp316=(_tmp2FD.Wchar_c).val;_LL4:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp303;_tmp303.tag=0U;_tmp303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp316);({void*_tmp301[1U]={& _tmp303};({struct _dyneither_ptr _tmp71F=({const char*_tmp302="L'%s'";_tag_dyneither(_tmp302,sizeof(char),6U);});Cyc_aprintf(_tmp71F,_tag_dyneither(_tmp301,sizeof(void*),1U));});});}));case 4U: _LL5: _tmp318=((_tmp2FD.Short_c).val).f1;_tmp317=((_tmp2FD.Short_c).val).f2;_LL6:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp306;_tmp306.tag=1U;_tmp306.f1=(unsigned long)((int)_tmp317);({void*_tmp304[1U]={& _tmp306};({struct _dyneither_ptr _tmp720=({const char*_tmp305="%d";_tag_dyneither(_tmp305,sizeof(char),3U);});Cyc_aprintf(_tmp720,_tag_dyneither(_tmp304,sizeof(void*),1U));});});}));case 5U: _LL7: _tmp31A=((_tmp2FD.Int_c).val).f1;_tmp319=((_tmp2FD.Int_c).val).f2;_LL8:
# 1343
 if(_tmp31A == Cyc_Absyn_Unsigned)
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp309;_tmp309.tag=1U;_tmp309.f1=(unsigned int)_tmp319;({void*_tmp307[1U]={& _tmp309};({struct _dyneither_ptr _tmp721=({const char*_tmp308="%uU";_tag_dyneither(_tmp308,sizeof(char),4U);});Cyc_aprintf(_tmp721,_tag_dyneither(_tmp307,sizeof(void*),1U));});});}));else{
# 1346
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30C;_tmp30C.tag=1U;_tmp30C.f1=(unsigned long)_tmp319;({void*_tmp30A[1U]={& _tmp30C};({struct _dyneither_ptr _tmp722=({const char*_tmp30B="%d";_tag_dyneither(_tmp30B,sizeof(char),3U);});Cyc_aprintf(_tmp722,_tag_dyneither(_tmp30A,sizeof(void*),1U));});});}));}case 6U: _LL9: _tmp31C=((_tmp2FD.LongLong_c).val).f1;_tmp31B=((_tmp2FD.LongLong_c).val).f2;_LLA:
# 1349
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp31B));case 7U: _LLB: _tmp31D=((_tmp2FD.Float_c).val).f1;_LLC:
 return Cyc_PP_text(_tmp31D);case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp30D="NULL";_tag_dyneither(_tmp30D,sizeof(char),5U);}));case 8U: _LLF: _tmp31E=(_tmp2FD.String_c).val;_LL10:
# 1353
 return({struct Cyc_PP_Doc*_tmp30E[3U];({struct Cyc_PP_Doc*_tmp723=Cyc_PP_text(({const char*_tmp310="\"";_tag_dyneither(_tmp310,sizeof(char),2U);}));_tmp30E[2U]=_tmp723;});({struct Cyc_PP_Doc*_tmp724=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp31E));_tmp30E[1U]=_tmp724;});({struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp30F="\"";_tag_dyneither(_tmp30F,sizeof(char),2U);}));_tmp30E[0U]=_tmp725;});Cyc_PP_cat(_tag_dyneither(_tmp30E,sizeof(struct Cyc_PP_Doc*),3U));});default: _LL11: _tmp31F=(_tmp2FD.Wstring_c).val;_LL12:
# 1355
 return({struct Cyc_PP_Doc*_tmp311[3U];({struct Cyc_PP_Doc*_tmp726=Cyc_PP_text(({const char*_tmp313="\"";_tag_dyneither(_tmp313,sizeof(char),2U);}));_tmp311[2U]=_tmp726;});({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(_tmp31F);_tmp311[1U]=_tmp727;});({struct Cyc_PP_Doc*_tmp728=Cyc_PP_text(({const char*_tmp312="L\"";_tag_dyneither(_tmp312,sizeof(char),3U);}));_tmp311[0U]=_tmp728;});Cyc_PP_cat(_tag_dyneither(_tmp311,sizeof(struct Cyc_PP_Doc*),3U));});}_LL0:;}
# 1359
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp320=_cycalloc(sizeof(*_tmp320));({struct Cyc_Core_Failure_exn_struct _tmp72C=({struct Cyc_Core_Failure_exn_struct _tmp321;_tmp321.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp72B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp324;_tmp324.tag=0U;({struct _dyneither_ptr _tmp729=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp324.f1=_tmp729;});({void*_tmp322[1U]={& _tmp324};({struct _dyneither_ptr _tmp72A=({const char*_tmp323="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp323,sizeof(char),47U);});Cyc_aprintf(_tmp72A,_tag_dyneither(_tmp322,sizeof(void*),1U));});});});_tmp321.f1=_tmp72B;});_tmp321;});_tmp320[0]=_tmp72C;});_tmp320;}));
# 1365
return({struct Cyc_PP_Doc*_tmp325[3U];({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp327=")";_tag_dyneither(_tmp327,sizeof(char),2U);}));_tmp325[2U]=_tmp72D;});({struct Cyc_PP_Doc*_tmp72E=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp325[1U]=_tmp72E;});({struct Cyc_PP_Doc*_tmp72F=Cyc_PP_text(({const char*_tmp326="numelts(";_tag_dyneither(_tmp326,sizeof(char),9U);}));_tmp325[0U]=_tmp72F;});Cyc_PP_cat(_tag_dyneither(_tmp325,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1367
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp328=_cycalloc(sizeof(*_tmp328));({struct Cyc_Core_Failure_exn_struct _tmp733=({struct Cyc_Core_Failure_exn_struct _tmp329;_tmp329.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp732=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32C;_tmp32C.tag=0U;({struct _dyneither_ptr _tmp730=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp32C.f1=_tmp730;});({void*_tmp32A[1U]={& _tmp32C};({struct _dyneither_ptr _tmp731=({const char*_tmp32B="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp32B,sizeof(char),38U);});Cyc_aprintf(_tmp731,_tag_dyneither(_tmp32A,sizeof(void*),1U));});});});_tmp329.f1=_tmp732;});_tmp329;});_tmp328[0]=_tmp733;});_tmp328;}));else{
# 1371
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp32D[3U];_tmp32D[2U]=(struct Cyc_PP_Doc*)ds->hd;({struct Cyc_PP_Doc*_tmp734=Cyc_PP_text(({const char*_tmp32E=" ";_tag_dyneither(_tmp32E,sizeof(char),2U);}));_tmp32D[1U]=_tmp734;});_tmp32D[0U]=ps;Cyc_PP_cat(_tag_dyneither(_tmp32D,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32F=_cycalloc(sizeof(*_tmp32F));({struct Cyc_Core_Failure_exn_struct _tmp738=({struct Cyc_Core_Failure_exn_struct _tmp330;_tmp330.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp737=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp333;_tmp333.tag=0U;({struct _dyneither_ptr _tmp735=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp333.f1=_tmp735;});({void*_tmp331[1U]={& _tmp333};({struct _dyneither_ptr _tmp736=({const char*_tmp332="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp332,sizeof(char),47U);});Cyc_aprintf(_tmp736,_tag_dyneither(_tmp331,sizeof(void*),1U));});});});_tmp330.f1=_tmp737;});_tmp330;});_tmp32F[0]=_tmp738;});_tmp32F;}));else{
# 1377
return({struct Cyc_PP_Doc*_tmp334[5U];_tmp334[4U]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;({struct Cyc_PP_Doc*_tmp739=Cyc_PP_text(({const char*_tmp336=" ";_tag_dyneither(_tmp336,sizeof(char),2U);}));_tmp334[3U]=_tmp739;});_tmp334[2U]=ps;({struct Cyc_PP_Doc*_tmp73A=Cyc_PP_text(({const char*_tmp335=" ";_tag_dyneither(_tmp335,sizeof(char),2U);}));_tmp334[1U]=_tmp73A;});_tmp334[0U]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp334,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1381
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp337=p;switch(_tmp337){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp338="+";_tag_dyneither(_tmp338,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp339="*";_tag_dyneither(_tmp339,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp33A="-";_tag_dyneither(_tmp33A,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp33B="/";_tag_dyneither(_tmp33B,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp33C="\\%";_tag_dyneither(_tmp33C,sizeof(char),3U);}):({const char*_tmp33D="%";_tag_dyneither(_tmp33D,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp33E="==";_tag_dyneither(_tmp33E,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp33F="!=";_tag_dyneither(_tmp33F,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp340=">";_tag_dyneither(_tmp340,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp341="<";_tag_dyneither(_tmp341,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp342=">=";_tag_dyneither(_tmp342,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp343="<=";_tag_dyneither(_tmp343,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp344="!";_tag_dyneither(_tmp344,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp345="~";_tag_dyneither(_tmp345,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp346="&";_tag_dyneither(_tmp346,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp347="|";_tag_dyneither(_tmp347,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp348="^";_tag_dyneither(_tmp348,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp349="<<";_tag_dyneither(_tmp349,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp34A=">>";_tag_dyneither(_tmp34A,sizeof(char),3U);});case Cyc_Absyn_Bitarshift: _LL25: _LL26:
 return({const char*_tmp34B=">>>";_tag_dyneither(_tmp34B,sizeof(char),4U);});default: _LL27: _LL28:
 return({const char*_tmp34C="numelts";_tag_dyneither(_tmp34C,sizeof(char),8U);});}_LL0:;}
# 1406
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1410
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp34D=s->r;void*_tmp34E=_tmp34D;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34E)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1417
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp34F=st->r;void*_tmp350=_tmp34F;struct Cyc_Absyn_Stmt*_tmp3AA;struct Cyc_List_List*_tmp3A9;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A7;struct _dyneither_ptr*_tmp3A6;struct Cyc_Absyn_Stmt*_tmp3A5;struct Cyc_Absyn_Decl*_tmp3A4;struct Cyc_Absyn_Stmt*_tmp3A3;struct Cyc_List_List*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_List_List*_tmp3A0;struct Cyc_Absyn_Exp*_tmp39F;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_Absyn_Stmt*_tmp39C;struct _dyneither_ptr*_tmp39B;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_Absyn_Stmt*_tmp399;struct Cyc_Absyn_Exp*_tmp398;struct Cyc_Absyn_Stmt*_tmp397;struct Cyc_Absyn_Stmt*_tmp396;struct Cyc_Absyn_Exp*_tmp395;struct Cyc_Absyn_Stmt*_tmp394;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Exp*_tmp392;switch(*((int*)_tmp350)){case 0U: _LL1: _LL2:
({struct Cyc_PP_Doc*_tmp73B=Cyc_PP_text(({const char*_tmp351=";";_tag_dyneither(_tmp351,sizeof(char),2U);}));s=_tmp73B;});goto _LL0;case 1U: _LL3: _tmp392=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_LL4:
({struct Cyc_PP_Doc*_tmp73E=({struct Cyc_PP_Doc*_tmp352[2U];({struct Cyc_PP_Doc*_tmp73C=Cyc_PP_text(({const char*_tmp353=";";_tag_dyneither(_tmp353,sizeof(char),2U);}));_tmp352[1U]=_tmp73C;});({struct Cyc_PP_Doc*_tmp73D=Cyc_Absynpp_exp2doc(_tmp392);_tmp352[0U]=_tmp73D;});Cyc_PP_cat(_tag_dyneither(_tmp352,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp73E;});goto _LL0;case 2U: _LL5: _tmp394=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp393=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_LL6:
# 1423
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp394))
({struct Cyc_PP_Doc*_tmp752=({struct Cyc_PP_Doc*_tmp354[7U];({struct Cyc_PP_Doc*_tmp74B=
# 1431
Cyc_Absynpp_is_declaration(_tmp393)?
stmtexp?({struct Cyc_PP_Doc*_tmp355[7U];({struct Cyc_PP_Doc*_tmp744=
# 1436
Cyc_PP_line_doc();_tmp355[6U]=_tmp744;});({struct Cyc_PP_Doc*_tmp745=
# 1435
Cyc_PP_text(({const char*_tmp357=");";_tag_dyneither(_tmp357,sizeof(char),3U);}));_tmp355[5U]=_tmp745;});({struct Cyc_PP_Doc*_tmp746=Cyc_Absynpp_rb();_tmp355[4U]=_tmp746;});({struct Cyc_PP_Doc*_tmp747=
# 1434
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp393,stmtexp));_tmp355[3U]=_tmp747;});({struct Cyc_PP_Doc*_tmp748=
# 1433
Cyc_PP_blank_doc();_tmp355[2U]=_tmp748;});({struct Cyc_PP_Doc*_tmp749=Cyc_Absynpp_lb();_tmp355[1U]=_tmp749;});({struct Cyc_PP_Doc*_tmp74A=Cyc_PP_text(({const char*_tmp356="(";_tag_dyneither(_tmp356,sizeof(char),2U);}));_tmp355[0U]=_tmp74A;});Cyc_PP_cat(_tag_dyneither(_tmp355,sizeof(struct Cyc_PP_Doc*),7U));}):({struct Cyc_PP_Doc*_tmp358[5U];({struct Cyc_PP_Doc*_tmp73F=
# 1440
Cyc_PP_line_doc();_tmp358[4U]=_tmp73F;});({struct Cyc_PP_Doc*_tmp740=
# 1439
Cyc_Absynpp_rb();_tmp358[3U]=_tmp740;});({struct Cyc_PP_Doc*_tmp741=
# 1438
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp393,stmtexp));_tmp358[2U]=_tmp741;});({struct Cyc_PP_Doc*_tmp742=
# 1437
Cyc_PP_blank_doc();_tmp358[1U]=_tmp742;});({struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_lb();_tmp358[0U]=_tmp743;});Cyc_PP_cat(_tag_dyneither(_tmp358,sizeof(struct Cyc_PP_Doc*),5U));}):
# 1441
 Cyc_Absynpp_stmt2doc(_tmp393,stmtexp);_tmp354[6U]=_tmp74B;});({struct Cyc_PP_Doc*_tmp74C=
# 1430
Cyc_PP_line_doc();_tmp354[5U]=_tmp74C;});({struct Cyc_PP_Doc*_tmp74D=
# 1429
Cyc_Absynpp_rb();_tmp354[4U]=_tmp74D;});({struct Cyc_PP_Doc*_tmp74E=
# 1428
Cyc_PP_line_doc();_tmp354[3U]=_tmp74E;});({struct Cyc_PP_Doc*_tmp74F=
# 1427
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp394,0));_tmp354[2U]=_tmp74F;});({struct Cyc_PP_Doc*_tmp750=
# 1426
Cyc_PP_blank_doc();_tmp354[1U]=_tmp750;});({struct Cyc_PP_Doc*_tmp751=
# 1425
Cyc_Absynpp_lb();_tmp354[0U]=_tmp751;});Cyc_PP_cat(_tag_dyneither(_tmp354,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp752;});else{
# 1442
if(Cyc_Absynpp_is_declaration(_tmp393))
({struct Cyc_PP_Doc*_tmp761=({struct Cyc_PP_Doc*_tmp359[4U];({struct Cyc_PP_Doc*_tmp753=
# 1453
Cyc_PP_line_doc();_tmp359[3U]=_tmp753;});({struct Cyc_PP_Doc*_tmp75E=
# 1445
stmtexp?({struct Cyc_PP_Doc*_tmp35A[6U];({struct Cyc_PP_Doc*_tmp758=
# 1448
Cyc_PP_text(({const char*_tmp35C=");";_tag_dyneither(_tmp35C,sizeof(char),3U);}));_tmp35A[5U]=_tmp758;});({struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_rb();_tmp35A[4U]=_tmp759;});({struct Cyc_PP_Doc*_tmp75A=
# 1447
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp393,stmtexp));_tmp35A[3U]=_tmp75A;});({struct Cyc_PP_Doc*_tmp75B=
# 1446
Cyc_PP_blank_doc();_tmp35A[2U]=_tmp75B;});({struct Cyc_PP_Doc*_tmp75C=Cyc_Absynpp_lb();_tmp35A[1U]=_tmp75C;});({struct Cyc_PP_Doc*_tmp75D=Cyc_PP_text(({const char*_tmp35B="(";_tag_dyneither(_tmp35B,sizeof(char),2U);}));_tmp35A[0U]=_tmp75D;});Cyc_PP_cat(_tag_dyneither(_tmp35A,sizeof(struct Cyc_PP_Doc*),6U));}):({struct Cyc_PP_Doc*_tmp35D[4U];({struct Cyc_PP_Doc*_tmp754=
# 1452
Cyc_Absynpp_rb();_tmp35D[3U]=_tmp754;});({struct Cyc_PP_Doc*_tmp755=
# 1451
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp393,stmtexp));_tmp35D[2U]=_tmp755;});({struct Cyc_PP_Doc*_tmp756=
# 1450
Cyc_PP_blank_doc();_tmp35D[1U]=_tmp756;});({struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_lb();_tmp35D[0U]=_tmp757;});Cyc_PP_cat(_tag_dyneither(_tmp35D,sizeof(struct Cyc_PP_Doc*),4U));});_tmp359[2U]=_tmp75E;});({struct Cyc_PP_Doc*_tmp75F=
# 1444
Cyc_PP_line_doc();_tmp359[1U]=_tmp75F;});({struct Cyc_PP_Doc*_tmp760=
# 1443
Cyc_Absynpp_stmt2doc(_tmp394,0);_tmp359[0U]=_tmp760;});Cyc_PP_cat(_tag_dyneither(_tmp359,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp761;});else{
# 1455
({struct Cyc_PP_Doc*_tmp765=({struct Cyc_PP_Doc*_tmp35E[3U];({struct Cyc_PP_Doc*_tmp762=Cyc_Absynpp_stmt2doc(_tmp393,stmtexp);_tmp35E[2U]=_tmp762;});({struct Cyc_PP_Doc*_tmp763=Cyc_PP_line_doc();_tmp35E[1U]=_tmp763;});({struct Cyc_PP_Doc*_tmp764=Cyc_Absynpp_stmt2doc(_tmp394,0);_tmp35E[0U]=_tmp764;});Cyc_PP_cat(_tag_dyneither(_tmp35E,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp765;});}}}else{
# 1458
({struct Cyc_PP_Doc*_tmp769=({struct Cyc_PP_Doc*_tmp35F[3U];({struct Cyc_PP_Doc*_tmp766=Cyc_Absynpp_stmt2doc(_tmp393,stmtexp);_tmp35F[2U]=_tmp766;});({struct Cyc_PP_Doc*_tmp767=Cyc_PP_line_doc();_tmp35F[1U]=_tmp767;});({struct Cyc_PP_Doc*_tmp768=Cyc_Absynpp_stmt2doc(_tmp394,0);_tmp35F[0U]=_tmp768;});Cyc_PP_cat(_tag_dyneither(_tmp35F,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp769;});}
goto _LL0;case 3U: _LL7: _tmp395=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_LL8:
# 1461
 if(_tmp395 == 0)
({struct Cyc_PP_Doc*_tmp76A=Cyc_PP_text(({const char*_tmp360="return;";_tag_dyneither(_tmp360,sizeof(char),8U);}));s=_tmp76A;});else{
# 1464
({struct Cyc_PP_Doc*_tmp76E=({struct Cyc_PP_Doc*_tmp361[3U];({struct Cyc_PP_Doc*_tmp76B=
# 1466
Cyc_PP_text(({const char*_tmp363=";";_tag_dyneither(_tmp363,sizeof(char),2U);}));_tmp361[2U]=_tmp76B;});({struct Cyc_PP_Doc*_tmp76C=
# 1465
_tmp395 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp395);_tmp361[1U]=_tmp76C;});({struct Cyc_PP_Doc*_tmp76D=
# 1464
Cyc_PP_text(({const char*_tmp362="return ";_tag_dyneither(_tmp362,sizeof(char),8U);}));_tmp361[0U]=_tmp76D;});Cyc_PP_cat(_tag_dyneither(_tmp361,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp76E;});}
# 1467
goto _LL0;case 4U: _LL9: _tmp398=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp397=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_tmp396=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp350)->f3;_LLA: {
# 1469
int print_else;
{void*_tmp364=_tmp396->r;void*_tmp365=_tmp364;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp365)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1474
({struct Cyc_PP_Doc*_tmp781=({struct Cyc_PP_Doc*_tmp366[8U];({struct Cyc_PP_Doc*_tmp777=
# 1481
print_else?({struct Cyc_PP_Doc*_tmp36A[6U];({struct Cyc_PP_Doc*_tmp76F=
# 1487
Cyc_Absynpp_rb();_tmp36A[5U]=_tmp76F;});({struct Cyc_PP_Doc*_tmp770=
# 1486
Cyc_PP_line_doc();_tmp36A[4U]=_tmp770;});({struct Cyc_PP_Doc*_tmp773=
# 1485
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36C[2U];({struct Cyc_PP_Doc*_tmp771=Cyc_Absynpp_stmt2doc(_tmp396,0);_tmp36C[1U]=_tmp771;});({struct Cyc_PP_Doc*_tmp772=Cyc_PP_line_doc();_tmp36C[0U]=_tmp772;});Cyc_PP_cat(_tag_dyneither(_tmp36C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp36A[3U]=_tmp773;});({struct Cyc_PP_Doc*_tmp774=
# 1484
Cyc_Absynpp_lb();_tmp36A[2U]=_tmp774;});({struct Cyc_PP_Doc*_tmp775=
# 1483
Cyc_PP_text(({const char*_tmp36B="else ";_tag_dyneither(_tmp36B,sizeof(char),6U);}));_tmp36A[1U]=_tmp775;});({struct Cyc_PP_Doc*_tmp776=
# 1482
Cyc_PP_line_doc();_tmp36A[0U]=_tmp776;});Cyc_PP_cat(_tag_dyneither(_tmp36A,sizeof(struct Cyc_PP_Doc*),6U));}):
# 1488
 Cyc_PP_nil_doc();_tmp366[7U]=_tmp777;});({struct Cyc_PP_Doc*_tmp778=
# 1480
Cyc_Absynpp_rb();_tmp366[6U]=_tmp778;});({struct Cyc_PP_Doc*_tmp779=
# 1479
Cyc_PP_line_doc();_tmp366[5U]=_tmp779;});({struct Cyc_PP_Doc*_tmp77C=
# 1478
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp369[2U];({struct Cyc_PP_Doc*_tmp77A=Cyc_Absynpp_stmt2doc(_tmp397,0);_tmp369[1U]=_tmp77A;});({struct Cyc_PP_Doc*_tmp77B=Cyc_PP_line_doc();_tmp369[0U]=_tmp77B;});Cyc_PP_cat(_tag_dyneither(_tmp369,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp366[4U]=_tmp77C;});({struct Cyc_PP_Doc*_tmp77D=
# 1477
Cyc_Absynpp_lb();_tmp366[3U]=_tmp77D;});({struct Cyc_PP_Doc*_tmp77E=
# 1476
Cyc_PP_text(({const char*_tmp368=") ";_tag_dyneither(_tmp368,sizeof(char),3U);}));_tmp366[2U]=_tmp77E;});({struct Cyc_PP_Doc*_tmp77F=
# 1475
Cyc_Absynpp_exp2doc(_tmp398);_tmp366[1U]=_tmp77F;});({struct Cyc_PP_Doc*_tmp780=
# 1474
Cyc_PP_text(({const char*_tmp367="if (";_tag_dyneither(_tmp367,sizeof(char),5U);}));_tmp366[0U]=_tmp780;});Cyc_PP_cat(_tag_dyneither(_tmp366,sizeof(struct Cyc_PP_Doc*),8U));});s=_tmp781;});
# 1489
goto _LL0;}case 5U: _LLB: _tmp39A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp350)->f1).f1;_tmp399=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_LLC:
# 1491
({struct Cyc_PP_Doc*_tmp78B=({struct Cyc_PP_Doc*_tmp36D[7U];({struct Cyc_PP_Doc*_tmp782=
# 1497
Cyc_Absynpp_rb();_tmp36D[6U]=_tmp782;});({struct Cyc_PP_Doc*_tmp783=
# 1496
Cyc_PP_line_doc();_tmp36D[5U]=_tmp783;});({struct Cyc_PP_Doc*_tmp786=
# 1495
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp370[2U];({struct Cyc_PP_Doc*_tmp784=Cyc_Absynpp_stmt2doc(_tmp399,0);_tmp370[1U]=_tmp784;});({struct Cyc_PP_Doc*_tmp785=Cyc_PP_line_doc();_tmp370[0U]=_tmp785;});Cyc_PP_cat(_tag_dyneither(_tmp370,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp36D[4U]=_tmp786;});({struct Cyc_PP_Doc*_tmp787=
# 1494
Cyc_Absynpp_lb();_tmp36D[3U]=_tmp787;});({struct Cyc_PP_Doc*_tmp788=
# 1493
Cyc_PP_text(({const char*_tmp36F=") ";_tag_dyneither(_tmp36F,sizeof(char),3U);}));_tmp36D[2U]=_tmp788;});({struct Cyc_PP_Doc*_tmp789=
# 1492
Cyc_Absynpp_exp2doc(_tmp39A);_tmp36D[1U]=_tmp789;});({struct Cyc_PP_Doc*_tmp78A=
# 1491
Cyc_PP_text(({const char*_tmp36E="while (";_tag_dyneither(_tmp36E,sizeof(char),8U);}));_tmp36D[0U]=_tmp78A;});Cyc_PP_cat(_tag_dyneither(_tmp36D,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp78B;});
# 1498
goto _LL0;case 6U: _LLD: _LLE:
({struct Cyc_PP_Doc*_tmp78C=Cyc_PP_text(({const char*_tmp371="break;";_tag_dyneither(_tmp371,sizeof(char),7U);}));s=_tmp78C;});goto _LL0;case 7U: _LLF: _LL10:
({struct Cyc_PP_Doc*_tmp78D=Cyc_PP_text(({const char*_tmp372="continue;";_tag_dyneither(_tmp372,sizeof(char),10U);}));s=_tmp78D;});goto _LL0;case 8U: _LL11: _tmp39B=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_LL12:
({struct Cyc_PP_Doc*_tmp78F=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp375;_tmp375.tag=0U;_tmp375.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39B);({void*_tmp373[1U]={& _tmp375};({struct _dyneither_ptr _tmp78E=({const char*_tmp374="goto %s;";_tag_dyneither(_tmp374,sizeof(char),9U);});Cyc_aprintf(_tmp78E,_tag_dyneither(_tmp373,sizeof(void*),1U));});});}));s=_tmp78F;});goto _LL0;case 9U: _LL13: _tmp39F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp39E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp350)->f2).f1;_tmp39D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp350)->f3).f1;_tmp39C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp350)->f4;_LL14:
# 1503
({struct Cyc_PP_Doc*_tmp79D=({struct Cyc_PP_Doc*_tmp376[11U];({struct Cyc_PP_Doc*_tmp790=
# 1513
Cyc_Absynpp_rb();_tmp376[10U]=_tmp790;});({struct Cyc_PP_Doc*_tmp791=
# 1512
Cyc_PP_line_doc();_tmp376[9U]=_tmp791;});({struct Cyc_PP_Doc*_tmp794=
# 1511
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp37B[2U];({struct Cyc_PP_Doc*_tmp792=Cyc_Absynpp_stmt2doc(_tmp39C,0);_tmp37B[1U]=_tmp792;});({struct Cyc_PP_Doc*_tmp793=Cyc_PP_line_doc();_tmp37B[0U]=_tmp793;});Cyc_PP_cat(_tag_dyneither(_tmp37B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp376[8U]=_tmp794;});({struct Cyc_PP_Doc*_tmp795=
# 1510
Cyc_Absynpp_lb();_tmp376[7U]=_tmp795;});({struct Cyc_PP_Doc*_tmp796=
# 1509
Cyc_PP_text(({const char*_tmp37A=") ";_tag_dyneither(_tmp37A,sizeof(char),3U);}));_tmp376[6U]=_tmp796;});({struct Cyc_PP_Doc*_tmp797=
# 1508
Cyc_Absynpp_exp2doc(_tmp39D);_tmp376[5U]=_tmp797;});({struct Cyc_PP_Doc*_tmp798=
# 1507
Cyc_PP_text(({const char*_tmp379="; ";_tag_dyneither(_tmp379,sizeof(char),3U);}));_tmp376[4U]=_tmp798;});({struct Cyc_PP_Doc*_tmp799=
# 1506
Cyc_Absynpp_exp2doc(_tmp39E);_tmp376[3U]=_tmp799;});({struct Cyc_PP_Doc*_tmp79A=
# 1505
Cyc_PP_text(({const char*_tmp378="; ";_tag_dyneither(_tmp378,sizeof(char),3U);}));_tmp376[2U]=_tmp79A;});({struct Cyc_PP_Doc*_tmp79B=
# 1504
Cyc_Absynpp_exp2doc(_tmp39F);_tmp376[1U]=_tmp79B;});({struct Cyc_PP_Doc*_tmp79C=
# 1503
Cyc_PP_text(({const char*_tmp377="for(";_tag_dyneither(_tmp377,sizeof(char),5U);}));_tmp376[0U]=_tmp79C;});Cyc_PP_cat(_tag_dyneither(_tmp376,sizeof(struct Cyc_PP_Doc*),11U));});s=_tmp79D;});
# 1514
goto _LL0;case 10U: _LL15: _tmp3A1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp3A0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_LL16:
# 1516
({struct Cyc_PP_Doc*_tmp7A6=({struct Cyc_PP_Doc*_tmp37C[8U];({struct Cyc_PP_Doc*_tmp79E=
# 1523
Cyc_Absynpp_rb();_tmp37C[7U]=_tmp79E;});({struct Cyc_PP_Doc*_tmp79F=
# 1522
Cyc_PP_line_doc();_tmp37C[6U]=_tmp79F;});({struct Cyc_PP_Doc*_tmp7A0=
# 1521
Cyc_Absynpp_switchclauses2doc(_tmp3A0);_tmp37C[5U]=_tmp7A0;});({struct Cyc_PP_Doc*_tmp7A1=
# 1520
Cyc_PP_line_doc();_tmp37C[4U]=_tmp7A1;});({struct Cyc_PP_Doc*_tmp7A2=
# 1519
Cyc_Absynpp_lb();_tmp37C[3U]=_tmp7A2;});({struct Cyc_PP_Doc*_tmp7A3=
# 1518
Cyc_PP_text(({const char*_tmp37E=") ";_tag_dyneither(_tmp37E,sizeof(char),3U);}));_tmp37C[2U]=_tmp7A3;});({struct Cyc_PP_Doc*_tmp7A4=
# 1517
Cyc_Absynpp_exp2doc(_tmp3A1);_tmp37C[1U]=_tmp7A4;});({struct Cyc_PP_Doc*_tmp7A5=
# 1516
Cyc_PP_text(({const char*_tmp37D="switch (";_tag_dyneither(_tmp37D,sizeof(char),9U);}));_tmp37C[0U]=_tmp7A5;});Cyc_PP_cat(_tag_dyneither(_tmp37C,sizeof(struct Cyc_PP_Doc*),8U));});s=_tmp7A6;});
# 1524
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp350)->f1 == 0){_LL17: _LL18:
({struct Cyc_PP_Doc*_tmp7A7=Cyc_PP_text(({const char*_tmp37F="fallthru;";_tag_dyneither(_tmp37F,sizeof(char),10U);}));s=_tmp7A7;});goto _LL0;}else{_LL19: _tmp3A2=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_LL1A:
# 1527
({struct Cyc_PP_Doc*_tmp7AB=({struct Cyc_PP_Doc*_tmp380[3U];({struct Cyc_PP_Doc*_tmp7A8=Cyc_PP_text(({const char*_tmp382=");";_tag_dyneither(_tmp382,sizeof(char),3U);}));_tmp380[2U]=_tmp7A8;});({struct Cyc_PP_Doc*_tmp7A9=Cyc_Absynpp_exps2doc_prec(20,_tmp3A2);_tmp380[1U]=_tmp7A9;});({struct Cyc_PP_Doc*_tmp7AA=Cyc_PP_text(({const char*_tmp381="fallthru(";_tag_dyneither(_tmp381,sizeof(char),10U);}));_tmp380[0U]=_tmp7AA;});Cyc_PP_cat(_tag_dyneither(_tmp380,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7AB;});goto _LL0;}case 12U: _LL1B: _tmp3A4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp3A3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_LL1C:
# 1529
({struct Cyc_PP_Doc*_tmp7AF=({struct Cyc_PP_Doc*_tmp383[3U];({struct Cyc_PP_Doc*_tmp7AC=
# 1531
Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp);_tmp383[2U]=_tmp7AC;});({struct Cyc_PP_Doc*_tmp7AD=
# 1530
Cyc_PP_line_doc();_tmp383[1U]=_tmp7AD;});({struct Cyc_PP_Doc*_tmp7AE=
# 1529
Cyc_Absynpp_decl2doc(_tmp3A4);_tmp383[0U]=_tmp7AE;});Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7AF;});
# 1532
goto _LL0;case 13U: _LL1D: _tmp3A6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp3A5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_LL1E:
# 1534
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3A5)){
if(stmtexp)
({struct Cyc_PP_Doc*_tmp7B8=({struct Cyc_PP_Doc*_tmp384[8U];({struct Cyc_PP_Doc*_tmp7B0=
# 1542
Cyc_PP_text(({const char*_tmp386=");";_tag_dyneither(_tmp386,sizeof(char),3U);}));_tmp384[7U]=_tmp7B0;});({struct Cyc_PP_Doc*_tmp7B1=Cyc_Absynpp_rb();_tmp384[6U]=_tmp7B1;});({struct Cyc_PP_Doc*_tmp7B2=
# 1541
Cyc_PP_line_doc();_tmp384[5U]=_tmp7B2;});({struct Cyc_PP_Doc*_tmp7B3=
# 1540
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A5,1));_tmp384[4U]=_tmp7B3;});({struct Cyc_PP_Doc*_tmp7B4=
# 1539
Cyc_PP_line_doc();_tmp384[3U]=_tmp7B4;});({struct Cyc_PP_Doc*_tmp7B5=
# 1538
Cyc_Absynpp_lb();_tmp384[2U]=_tmp7B5;});({struct Cyc_PP_Doc*_tmp7B6=
# 1537
Cyc_PP_text(({const char*_tmp385=": (";_tag_dyneither(_tmp385,sizeof(char),4U);}));_tmp384[1U]=_tmp7B6;});({struct Cyc_PP_Doc*_tmp7B7=
# 1536
Cyc_PP_textptr(_tmp3A6);_tmp384[0U]=_tmp7B7;});Cyc_PP_cat(_tag_dyneither(_tmp384,sizeof(struct Cyc_PP_Doc*),8U));});s=_tmp7B8;});else{
# 1544
({struct Cyc_PP_Doc*_tmp7C0=({struct Cyc_PP_Doc*_tmp387[7U];({struct Cyc_PP_Doc*_tmp7B9=
# 1550
Cyc_Absynpp_rb();_tmp387[6U]=_tmp7B9;});({struct Cyc_PP_Doc*_tmp7BA=
# 1549
Cyc_PP_line_doc();_tmp387[5U]=_tmp7BA;});({struct Cyc_PP_Doc*_tmp7BB=
# 1548
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A5,0));_tmp387[4U]=_tmp7BB;});({struct Cyc_PP_Doc*_tmp7BC=
# 1547
Cyc_PP_line_doc();_tmp387[3U]=_tmp7BC;});({struct Cyc_PP_Doc*_tmp7BD=
# 1546
Cyc_Absynpp_lb();_tmp387[2U]=_tmp7BD;});({struct Cyc_PP_Doc*_tmp7BE=
# 1545
Cyc_PP_text(({const char*_tmp388=": ";_tag_dyneither(_tmp388,sizeof(char),3U);}));_tmp387[1U]=_tmp7BE;});({struct Cyc_PP_Doc*_tmp7BF=
# 1544
Cyc_PP_textptr(_tmp3A6);_tmp387[0U]=_tmp7BF;});Cyc_PP_cat(_tag_dyneither(_tmp387,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp7C0;});}}else{
# 1552
({struct Cyc_PP_Doc*_tmp7C4=({struct Cyc_PP_Doc*_tmp389[3U];({struct Cyc_PP_Doc*_tmp7C1=Cyc_Absynpp_stmt2doc(_tmp3A5,stmtexp);_tmp389[2U]=_tmp7C1;});({struct Cyc_PP_Doc*_tmp7C2=Cyc_PP_text(({const char*_tmp38A=": ";_tag_dyneither(_tmp38A,sizeof(char),3U);}));_tmp389[1U]=_tmp7C2;});({struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_textptr(_tmp3A6);_tmp389[0U]=_tmp7C3;});Cyc_PP_cat(_tag_dyneither(_tmp389,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7C4;});}
goto _LL0;case 14U: _LL1F: _tmp3A8=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp3A7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp350)->f2).f1;_LL20:
# 1555
({struct Cyc_PP_Doc*_tmp7CE=({struct Cyc_PP_Doc*_tmp38B[9U];({struct Cyc_PP_Doc*_tmp7C5=
# 1563
Cyc_PP_text(({const char*_tmp38E=");";_tag_dyneither(_tmp38E,sizeof(char),3U);}));_tmp38B[8U]=_tmp7C5;});({struct Cyc_PP_Doc*_tmp7C6=
# 1562
Cyc_Absynpp_exp2doc(_tmp3A7);_tmp38B[7U]=_tmp7C6;});({struct Cyc_PP_Doc*_tmp7C7=
# 1561
Cyc_PP_text(({const char*_tmp38D=" while (";_tag_dyneither(_tmp38D,sizeof(char),9U);}));_tmp38B[6U]=_tmp7C7;});({struct Cyc_PP_Doc*_tmp7C8=
# 1560
Cyc_Absynpp_rb();_tmp38B[5U]=_tmp7C8;});({struct Cyc_PP_Doc*_tmp7C9=
# 1559
Cyc_PP_line_doc();_tmp38B[4U]=_tmp7C9;});({struct Cyc_PP_Doc*_tmp7CA=
# 1558
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,0));_tmp38B[3U]=_tmp7CA;});({struct Cyc_PP_Doc*_tmp7CB=
# 1557
Cyc_PP_line_doc();_tmp38B[2U]=_tmp7CB;});({struct Cyc_PP_Doc*_tmp7CC=
# 1556
Cyc_Absynpp_lb();_tmp38B[1U]=_tmp7CC;});({struct Cyc_PP_Doc*_tmp7CD=
# 1555
Cyc_PP_text(({const char*_tmp38C="do ";_tag_dyneither(_tmp38C,sizeof(char),4U);}));_tmp38B[0U]=_tmp7CD;});Cyc_PP_cat(_tag_dyneither(_tmp38B,sizeof(struct Cyc_PP_Doc*),9U));});s=_tmp7CE;});
# 1564
goto _LL0;default: _LL21: _tmp3AA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp350)->f1;_tmp3A9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp350)->f2;_LL22:
# 1566
({struct Cyc_PP_Doc*_tmp7DB=({struct Cyc_PP_Doc*_tmp38F[12U];({struct Cyc_PP_Doc*_tmp7CF=
# 1577
Cyc_Absynpp_rb();_tmp38F[11U]=_tmp7CF;});({struct Cyc_PP_Doc*_tmp7D0=
# 1576
Cyc_PP_line_doc();_tmp38F[10U]=_tmp7D0;});({struct Cyc_PP_Doc*_tmp7D1=
# 1575
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3A9));_tmp38F[9U]=_tmp7D1;});({struct Cyc_PP_Doc*_tmp7D2=
# 1574
Cyc_PP_line_doc();_tmp38F[8U]=_tmp7D2;});({struct Cyc_PP_Doc*_tmp7D3=
# 1573
Cyc_Absynpp_lb();_tmp38F[7U]=_tmp7D3;});({struct Cyc_PP_Doc*_tmp7D4=
# 1572
Cyc_PP_text(({const char*_tmp391=" catch ";_tag_dyneither(_tmp391,sizeof(char),8U);}));_tmp38F[6U]=_tmp7D4;});({struct Cyc_PP_Doc*_tmp7D5=
# 1571
Cyc_Absynpp_rb();_tmp38F[5U]=_tmp7D5;});({struct Cyc_PP_Doc*_tmp7D6=
# 1570
Cyc_PP_line_doc();_tmp38F[4U]=_tmp7D6;});({struct Cyc_PP_Doc*_tmp7D7=
# 1569
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AA,0));_tmp38F[3U]=_tmp7D7;});({struct Cyc_PP_Doc*_tmp7D8=
# 1568
Cyc_PP_line_doc();_tmp38F[2U]=_tmp7D8;});({struct Cyc_PP_Doc*_tmp7D9=
# 1567
Cyc_Absynpp_lb();_tmp38F[1U]=_tmp7D9;});({struct Cyc_PP_Doc*_tmp7DA=
# 1566
Cyc_PP_text(({const char*_tmp390="try ";_tag_dyneither(_tmp390,sizeof(char),5U);}));_tmp38F[0U]=_tmp7DA;});Cyc_PP_cat(_tag_dyneither(_tmp38F,sizeof(struct Cyc_PP_Doc*),12U));});s=_tmp7DB;});
# 1578
goto _LL0;}_LL0:;}
# 1580
return s;}
# 1583
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AB=p->r;void*_tmp3AC=_tmp3AB;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Datatypefield*_tmp40B;struct Cyc_List_List*_tmp40A;int _tmp409;struct Cyc_Absyn_Datatypefield*_tmp408;struct Cyc_Absyn_Enumfield*_tmp407;struct Cyc_Absyn_Enumfield*_tmp406;struct Cyc_List_List*_tmp405;struct Cyc_List_List*_tmp404;int _tmp403;union Cyc_Absyn_AggrInfoU _tmp402;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp400;int _tmp3FF;struct _tuple0*_tmp3FE;struct Cyc_List_List*_tmp3FD;int _tmp3FC;struct _tuple0*_tmp3FB;struct Cyc_Absyn_Vardecl*_tmp3FA;struct Cyc_Absyn_Pat*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3F8;struct Cyc_Absyn_Pat*_tmp3F7;struct Cyc_List_List*_tmp3F6;int _tmp3F5;struct Cyc_Absyn_Tvar*_tmp3F4;struct Cyc_Absyn_Vardecl*_tmp3F3;struct Cyc_Absyn_Tvar*_tmp3F2;struct Cyc_Absyn_Vardecl*_tmp3F1;struct Cyc_Absyn_Vardecl*_tmp3F0;struct Cyc_Absyn_Pat*_tmp3EF;struct Cyc_Absyn_Vardecl*_tmp3EE;struct _dyneither_ptr _tmp3ED;char _tmp3EC;enum Cyc_Absyn_Sign _tmp3EB;int _tmp3EA;switch(*((int*)_tmp3AC)){case 0U: _LL1: _LL2:
({struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_text(({const char*_tmp3AD="_";_tag_dyneither(_tmp3AD,sizeof(char),2U);}));s=_tmp7DC;});goto _LL0;case 9U: _LL3: _LL4:
({struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_text(({const char*_tmp3AE="NULL";_tag_dyneither(_tmp3AE,sizeof(char),5U);}));s=_tmp7DD;});goto _LL0;case 10U: _LL5: _tmp3EB=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3EA=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL6:
# 1589
 if(_tmp3EB != Cyc_Absyn_Unsigned)
({struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=1U;_tmp3B1.f1=(unsigned long)_tmp3EA;({void*_tmp3AF[1U]={& _tmp3B1};({struct _dyneither_ptr _tmp7DE=({const char*_tmp3B0="%d";_tag_dyneither(_tmp3B0,sizeof(char),3U);});Cyc_aprintf(_tmp7DE,_tag_dyneither(_tmp3AF,sizeof(void*),1U));});});}));s=_tmp7DF;});else{
({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=1U;_tmp3B4.f1=(unsigned int)_tmp3EA;({void*_tmp3B2[1U]={& _tmp3B4};({struct _dyneither_ptr _tmp7E0=({const char*_tmp3B3="%u";_tag_dyneither(_tmp3B3,sizeof(char),3U);});Cyc_aprintf(_tmp7E0,_tag_dyneither(_tmp3B2,sizeof(void*),1U));});});}));s=_tmp7E1;});}
goto _LL0;case 11U: _LL7: _tmp3EC=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL8:
({struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0U;({struct _dyneither_ptr _tmp7E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3EC));_tmp3B7.f1=_tmp7E2;});({void*_tmp3B5[1U]={& _tmp3B7};({struct _dyneither_ptr _tmp7E3=({const char*_tmp3B6="'%s'";_tag_dyneither(_tmp3B6,sizeof(char),5U);});Cyc_aprintf(_tmp7E3,_tag_dyneither(_tmp3B5,sizeof(void*),1U));});});}));s=_tmp7E4;});goto _LL0;case 12U: _LL9: _tmp3ED=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LLA:
({struct Cyc_PP_Doc*_tmp7E5=Cyc_PP_text(_tmp3ED);s=_tmp7E5;});goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0U){_LLB: _tmp3EE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LLC:
# 1596
({struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_qvar2doc(_tmp3EE->name);s=_tmp7E6;});goto _LL0;}else{_LLD: _tmp3F0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3EF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LLE:
# 1598
({struct Cyc_PP_Doc*_tmp7EA=({struct Cyc_PP_Doc*_tmp3B8[3U];({struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_pat2doc(_tmp3EF);_tmp3B8[2U]=_tmp7E7;});({struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp3B9=" as ";_tag_dyneither(_tmp3B9,sizeof(char),5U);}));_tmp3B8[1U]=_tmp7E8;});({struct Cyc_PP_Doc*_tmp7E9=Cyc_Absynpp_qvar2doc(_tmp3F0->name);_tmp3B8[0U]=_tmp7E9;});Cyc_PP_cat(_tag_dyneither(_tmp3B8,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp7EA;});goto _LL0;}case 2U: _LLF: _tmp3F2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL10:
# 1600
({struct Cyc_PP_Doc*_tmp7F0=({struct Cyc_PP_Doc*_tmp3BA[5U];({struct Cyc_PP_Doc*_tmp7EB=
Cyc_Absynpp_vardecl2doc(_tmp3F1);_tmp3BA[4U]=_tmp7EB;});({struct Cyc_PP_Doc*_tmp7EC=
# 1600
Cyc_PP_text(({const char*_tmp3BD="> ";_tag_dyneither(_tmp3BD,sizeof(char),3U);}));_tmp3BA[3U]=_tmp7EC;});({struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_tvar2doc(_tmp3F2);_tmp3BA[2U]=_tmp7ED;});({struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_text(({const char*_tmp3BC=" <";_tag_dyneither(_tmp3BC,sizeof(char),3U);}));_tmp3BA[1U]=_tmp7EE;});({struct Cyc_PP_Doc*_tmp7EF=Cyc_PP_text(({const char*_tmp3BB="alias";_tag_dyneither(_tmp3BB,sizeof(char),6U);}));_tmp3BA[0U]=_tmp7EF;});Cyc_PP_cat(_tag_dyneither(_tmp3BA,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp7F0;});
# 1602
goto _LL0;case 4U: _LL11: _tmp3F4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL12:
# 1604
({struct Cyc_PP_Doc*_tmp7F5=({struct Cyc_PP_Doc*_tmp3BE[4U];({struct Cyc_PP_Doc*_tmp7F1=Cyc_PP_text(({const char*_tmp3C0=">";_tag_dyneither(_tmp3C0,sizeof(char),2U);}));_tmp3BE[3U]=_tmp7F1;});({struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_tvar2doc(_tmp3F4);_tmp3BE[2U]=_tmp7F2;});({struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_text(({const char*_tmp3BF="<";_tag_dyneither(_tmp3BF,sizeof(char),2U);}));_tmp3BE[1U]=_tmp7F3;});({struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_qvar2doc(_tmp3F3->name);_tmp3BE[0U]=_tmp7F4;});Cyc_PP_cat(_tag_dyneither(_tmp3BE,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp7F5;});
goto _LL0;case 5U: _LL13: _tmp3F6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F5=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL14:
# 1607
({struct Cyc_PP_Doc*_tmp7FB=({struct Cyc_PP_Doc*_tmp3C1[4U];({struct Cyc_PP_Doc*_tmp7F6=
_tmp3F5?Cyc_PP_text(({const char*_tmp3C4=", ...)";_tag_dyneither(_tmp3C4,sizeof(char),7U);})): Cyc_PP_text(({const char*_tmp3C5=")";_tag_dyneither(_tmp3C5,sizeof(char),2U);}));_tmp3C1[3U]=_tmp7F6;});({struct Cyc_PP_Doc*_tmp7F8=({
# 1607
struct _dyneither_ptr _tmp7F7=({const char*_tmp3C3=",";_tag_dyneither(_tmp3C3,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp7F7,_tmp3F6);});_tmp3C1[2U]=_tmp7F8;});({struct Cyc_PP_Doc*_tmp7F9=Cyc_PP_text(({const char*_tmp3C2="(";_tag_dyneither(_tmp3C2,sizeof(char),2U);}));_tmp3C1[1U]=_tmp7F9;});({struct Cyc_PP_Doc*_tmp7FA=Cyc_Absynpp_dollar();_tmp3C1[0U]=_tmp7FA;});Cyc_PP_cat(_tag_dyneither(_tmp3C1,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp7FB;});
# 1609
goto _LL0;case 6U: _LL15: _tmp3F7=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL16:
# 1611
({struct Cyc_PP_Doc*_tmp7FE=({struct Cyc_PP_Doc*_tmp3C6[2U];({struct Cyc_PP_Doc*_tmp7FC=Cyc_Absynpp_pat2doc(_tmp3F7);_tmp3C6[1U]=_tmp7FC;});({struct Cyc_PP_Doc*_tmp7FD=Cyc_PP_text(({const char*_tmp3C7="&";_tag_dyneither(_tmp3C7,sizeof(char),2U);}));_tmp3C6[0U]=_tmp7FD;});Cyc_PP_cat(_tag_dyneither(_tmp3C6,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp7FE;});
goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0U){_LL17: _tmp3F8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL18:
# 1614
({struct Cyc_PP_Doc*_tmp801=({struct Cyc_PP_Doc*_tmp3C8[2U];({struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_qvar2doc(_tmp3F8->name);_tmp3C8[1U]=_tmp7FF;});({struct Cyc_PP_Doc*_tmp800=Cyc_PP_text(({const char*_tmp3C9="*";_tag_dyneither(_tmp3C9,sizeof(char),2U);}));_tmp3C8[0U]=_tmp800;});Cyc_PP_cat(_tag_dyneither(_tmp3C8,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp801;});
goto _LL0;}else{_LL19: _tmp3FA=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL1A:
# 1617
({struct Cyc_PP_Doc*_tmp806=({struct Cyc_PP_Doc*_tmp3CA[4U];({struct Cyc_PP_Doc*_tmp802=Cyc_Absynpp_pat2doc(_tmp3F9);_tmp3CA[3U]=_tmp802;});({struct Cyc_PP_Doc*_tmp803=Cyc_PP_text(({const char*_tmp3CC=" as ";_tag_dyneither(_tmp3CC,sizeof(char),5U);}));_tmp3CA[2U]=_tmp803;});({struct Cyc_PP_Doc*_tmp804=Cyc_Absynpp_qvar2doc(_tmp3FA->name);_tmp3CA[1U]=_tmp804;});({struct Cyc_PP_Doc*_tmp805=Cyc_PP_text(({const char*_tmp3CB="*";_tag_dyneither(_tmp3CB,sizeof(char),2U);}));_tmp3CA[0U]=_tmp805;});Cyc_PP_cat(_tag_dyneither(_tmp3CA,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp806;});
goto _LL0;}case 15U: _LL1B: _tmp3FB=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL1C:
# 1620
({struct Cyc_PP_Doc*_tmp807=Cyc_Absynpp_qvar2doc(_tmp3FB);s=_tmp807;});
goto _LL0;case 16U: _LL1D: _tmp3FE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3FD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3FC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_LL1E: {
# 1623
struct _dyneither_ptr term=_tmp3FC?({const char*_tmp3D0=", ...)";_tag_dyneither(_tmp3D0,sizeof(char),7U);}):({const char*_tmp3D1=")";_tag_dyneither(_tmp3D1,sizeof(char),2U);});
({struct Cyc_PP_Doc*_tmp80D=({struct Cyc_PP_Doc*_tmp3CD[2U];({struct Cyc_PP_Doc*_tmp80B=({struct _dyneither_ptr _tmp80A=({const char*_tmp3CE="(";_tag_dyneither(_tmp3CE,sizeof(char),2U);});struct _dyneither_ptr _tmp809=term;struct _dyneither_ptr _tmp808=({const char*_tmp3CF=",";_tag_dyneither(_tmp3CF,sizeof(char),2U);});Cyc_PP_group(_tmp80A,_tmp809,_tmp808,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3FD));});_tmp3CD[1U]=_tmp80B;});({struct Cyc_PP_Doc*_tmp80C=Cyc_Absynpp_qvar2doc(_tmp3FE);_tmp3CD[0U]=_tmp80C;});Cyc_PP_cat(_tag_dyneither(_tmp3CD,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp80D;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1 != 0){_LL1F: _tmp402=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1)->aggr_info;_tmp401=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp400=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3FF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL20: {
# 1627
struct _dyneither_ptr term=_tmp3FF?({const char*_tmp3DB=", ...}";_tag_dyneither(_tmp3DB,sizeof(char),7U);}):({const char*_tmp3DC="}";_tag_dyneither(_tmp3DC,sizeof(char),2U);});
struct _tuple10 _tmp3D2=Cyc_Absyn_aggr_kinded_name(_tmp402);struct _tuple10 _tmp3D3=_tmp3D2;struct _tuple0*_tmp3DA;_LL2E: _tmp3DA=_tmp3D3.f2;_LL2F:;
({struct Cyc_PP_Doc*_tmp818=({struct Cyc_PP_Doc*_tmp3D4[4U];({struct Cyc_PP_Doc*_tmp811=({
# 1631
struct _dyneither_ptr _tmp810=({const char*_tmp3D8="";_tag_dyneither(_tmp3D8,sizeof(char),1U);});struct _dyneither_ptr _tmp80F=term;struct _dyneither_ptr _tmp80E=({const char*_tmp3D9=",";_tag_dyneither(_tmp3D9,sizeof(char),2U);});Cyc_PP_group(_tmp810,_tmp80F,_tmp80E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp400));});_tmp3D4[3U]=_tmp811;});({struct Cyc_PP_Doc*_tmp815=({
# 1630
struct _dyneither_ptr _tmp814=({const char*_tmp3D5="[";_tag_dyneither(_tmp3D5,sizeof(char),2U);});struct _dyneither_ptr _tmp813=({const char*_tmp3D6="]";_tag_dyneither(_tmp3D6,sizeof(char),2U);});struct _dyneither_ptr _tmp812=({const char*_tmp3D7=",";_tag_dyneither(_tmp3D7,sizeof(char),2U);});Cyc_PP_egroup(_tmp814,_tmp813,_tmp812,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp401));});_tmp3D4[2U]=_tmp815;});({struct Cyc_PP_Doc*_tmp816=
# 1629
Cyc_Absynpp_lb();_tmp3D4[1U]=_tmp816;});({struct Cyc_PP_Doc*_tmp817=Cyc_Absynpp_qvar2doc(_tmp3DA);_tmp3D4[0U]=_tmp817;});Cyc_PP_cat(_tag_dyneither(_tmp3D4,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp818;});
# 1632
goto _LL0;}}else{_LL21: _tmp405=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp404=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp403=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL22: {
# 1634
struct _dyneither_ptr term=_tmp403?({const char*_tmp3E3=", ...}";_tag_dyneither(_tmp3E3,sizeof(char),7U);}):({const char*_tmp3E4="}";_tag_dyneither(_tmp3E4,sizeof(char),2U);});
({struct Cyc_PP_Doc*_tmp822=({struct Cyc_PP_Doc*_tmp3DD[3U];({struct Cyc_PP_Doc*_tmp81C=({
# 1637
struct _dyneither_ptr _tmp81B=({const char*_tmp3E1="";_tag_dyneither(_tmp3E1,sizeof(char),1U);});struct _dyneither_ptr _tmp81A=term;struct _dyneither_ptr _tmp819=({const char*_tmp3E2=",";_tag_dyneither(_tmp3E2,sizeof(char),2U);});Cyc_PP_group(_tmp81B,_tmp81A,_tmp819,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp404));});_tmp3DD[2U]=_tmp81C;});({struct Cyc_PP_Doc*_tmp820=({
# 1636
struct _dyneither_ptr _tmp81F=({const char*_tmp3DE="[";_tag_dyneither(_tmp3DE,sizeof(char),2U);});struct _dyneither_ptr _tmp81E=({const char*_tmp3DF="]";_tag_dyneither(_tmp3DF,sizeof(char),2U);});struct _dyneither_ptr _tmp81D=({const char*_tmp3E0=",";_tag_dyneither(_tmp3E0,sizeof(char),2U);});Cyc_PP_egroup(_tmp81F,_tmp81E,_tmp81D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp405));});_tmp3DD[1U]=_tmp820;});({struct Cyc_PP_Doc*_tmp821=
# 1635
Cyc_Absynpp_lb();_tmp3DD[0U]=_tmp821;});Cyc_PP_cat(_tag_dyneither(_tmp3DD,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp822;});
# 1638
goto _LL0;}}case 13U: _LL23: _tmp406=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL24:
 _tmp407=_tmp406;goto _LL26;case 14U: _LL25: _tmp407=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL26:
({struct Cyc_PP_Doc*_tmp823=Cyc_Absynpp_qvar2doc(_tmp407->name);s=_tmp823;});goto _LL0;case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3 == 0){_LL27: _tmp408=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL28:
({struct Cyc_PP_Doc*_tmp824=Cyc_Absynpp_qvar2doc(_tmp408->name);s=_tmp824;});goto _LL0;}else{_LL29: _tmp40B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp40A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp409=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2A: {
# 1643
struct _dyneither_ptr term=_tmp409?({const char*_tmp3E8=", ...)";_tag_dyneither(_tmp3E8,sizeof(char),7U);}):({const char*_tmp3E9=")";_tag_dyneither(_tmp3E9,sizeof(char),2U);});
({struct Cyc_PP_Doc*_tmp82A=({struct Cyc_PP_Doc*_tmp3E5[2U];({struct Cyc_PP_Doc*_tmp828=({struct _dyneither_ptr _tmp827=({const char*_tmp3E6="(";_tag_dyneither(_tmp3E6,sizeof(char),2U);});struct _dyneither_ptr _tmp826=term;struct _dyneither_ptr _tmp825=({const char*_tmp3E7=",";_tag_dyneither(_tmp3E7,sizeof(char),2U);});Cyc_PP_egroup(_tmp827,_tmp826,_tmp825,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp40A));});_tmp3E5[1U]=_tmp828;});({struct Cyc_PP_Doc*_tmp829=Cyc_Absynpp_qvar2doc(_tmp40B->name);_tmp3E5[0U]=_tmp829;});Cyc_PP_cat(_tag_dyneither(_tmp3E5,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp82A;});
goto _LL0;}}default: _LL2B: _tmp40C=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2C:
# 1647
({struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_exp2doc(_tmp40C);s=_tmp82B;});goto _LL0;}_LL0:;}
# 1649
return s;}
# 1652
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp40D[2U];({struct Cyc_PP_Doc*_tmp82C=
Cyc_Absynpp_pat2doc((*dp).f2);_tmp40D[1U]=_tmp82C;});({struct Cyc_PP_Doc*_tmp830=({
# 1653
struct _dyneither_ptr _tmp82F=({const char*_tmp40E="";_tag_dyneither(_tmp40E,sizeof(char),1U);});struct _dyneither_ptr _tmp82E=({const char*_tmp40F="=";_tag_dyneither(_tmp40F,sizeof(char),2U);});struct _dyneither_ptr _tmp82D=({const char*_tmp410="=";_tag_dyneither(_tmp410,sizeof(char),2U);});Cyc_PP_egroup(_tmp82F,_tmp82E,_tmp82D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp40D[0U]=_tmp830;});Cyc_PP_cat(_tag_dyneither(_tmp40D,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp411[2U];({struct Cyc_PP_Doc*_tmp833=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp413[2U];({struct Cyc_PP_Doc*_tmp831=Cyc_Absynpp_stmt2doc(c->body,0);_tmp413[1U]=_tmp831;});({struct Cyc_PP_Doc*_tmp832=Cyc_PP_line_doc();_tmp413[0U]=_tmp832;});Cyc_PP_cat(_tag_dyneither(_tmp413,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp411[1U]=_tmp833;});({struct Cyc_PP_Doc*_tmp834=
# 1659
Cyc_PP_text(({const char*_tmp412="default: ";_tag_dyneither(_tmp412,sizeof(char),10U);}));_tmp411[0U]=_tmp834;});Cyc_PP_cat(_tag_dyneither(_tmp411,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1661
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp414[4U];({struct Cyc_PP_Doc*_tmp837=
# 1665
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp417[2U];({struct Cyc_PP_Doc*_tmp835=Cyc_Absynpp_stmt2doc(c->body,0);_tmp417[1U]=_tmp835;});({struct Cyc_PP_Doc*_tmp836=Cyc_PP_line_doc();_tmp417[0U]=_tmp836;});Cyc_PP_cat(_tag_dyneither(_tmp417,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp414[3U]=_tmp837;});({struct Cyc_PP_Doc*_tmp838=
# 1664
Cyc_PP_text(({const char*_tmp416=": ";_tag_dyneither(_tmp416,sizeof(char),3U);}));_tmp414[2U]=_tmp838;});({struct Cyc_PP_Doc*_tmp839=
# 1663
Cyc_Absynpp_pat2doc(c->pattern);_tmp414[1U]=_tmp839;});({struct Cyc_PP_Doc*_tmp83A=
# 1662
Cyc_PP_text(({const char*_tmp415="case ";_tag_dyneither(_tmp415,sizeof(char),6U);}));_tmp414[0U]=_tmp83A;});Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1667
return({struct Cyc_PP_Doc*_tmp418[6U];({struct Cyc_PP_Doc*_tmp83D=
# 1672
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41C[2U];({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41C[1U]=_tmp83B;});({struct Cyc_PP_Doc*_tmp83C=Cyc_PP_line_doc();_tmp41C[0U]=_tmp83C;});Cyc_PP_cat(_tag_dyneither(_tmp41C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp418[5U]=_tmp83D;});({struct Cyc_PP_Doc*_tmp83E=
# 1671
Cyc_PP_text(({const char*_tmp41B=": ";_tag_dyneither(_tmp41B,sizeof(char),3U);}));_tmp418[4U]=_tmp83E;});({struct Cyc_PP_Doc*_tmp83F=
# 1670
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp418[3U]=_tmp83F;});({struct Cyc_PP_Doc*_tmp840=
# 1669
Cyc_PP_text(({const char*_tmp41A=" && ";_tag_dyneither(_tmp41A,sizeof(char),5U);}));_tmp418[2U]=_tmp840;});({struct Cyc_PP_Doc*_tmp841=
# 1668
Cyc_Absynpp_pat2doc(c->pattern);_tmp418[1U]=_tmp841;});({struct Cyc_PP_Doc*_tmp842=
# 1667
Cyc_PP_text(({const char*_tmp419="case ";_tag_dyneither(_tmp419,sizeof(char),6U);}));_tmp418[0U]=_tmp842;});Cyc_PP_cat(_tag_dyneither(_tmp418,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1675
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _dyneither_ptr _tmp843=({const char*_tmp41D="";_tag_dyneither(_tmp41D,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp843,cs);});}
# 1679
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1683
return({struct Cyc_PP_Doc*_tmp41E[3U];({struct Cyc_PP_Doc*_tmp844=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41E[2U]=_tmp844;});({struct Cyc_PP_Doc*_tmp845=Cyc_PP_text(({const char*_tmp41F=" = ";_tag_dyneither(_tmp41F,sizeof(char),4U);}));_tmp41E[1U]=_tmp845;});({struct Cyc_PP_Doc*_tmp846=Cyc_Absynpp_qvar2doc(f->name);_tmp41E[0U]=_tmp846;});Cyc_PP_cat(_tag_dyneither(_tmp41E,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1686
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _dyneither_ptr _tmp847=({const char*_tmp420=",";_tag_dyneither(_tmp420,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp847,fs);});}
# 1690
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1694
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _dyneither_ptr _tmp848=({const char*_tmp421=",";_tag_dyneither(_tmp421,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp848,vds);});}
# 1698
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp422=vd;enum Cyc_Absyn_Scope _tmp434;struct _tuple0*_tmp433;unsigned int _tmp432;struct Cyc_Absyn_Tqual _tmp431;void*_tmp430;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_List_List*_tmp42E;_LL1: _tmp434=_tmp422->sc;_tmp433=_tmp422->name;_tmp432=_tmp422->varloc;_tmp431=_tmp422->tq;_tmp430=_tmp422->type;_tmp42F=_tmp422->initializer;_tmp42E=_tmp422->attributes;_LL2:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp433);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp42E);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp423=Cyc_Cyclone_c_compiler;if(_tmp423 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1707
{void*_tmp424=Cyc_Tcutil_compress(_tmp430);void*_tmp425=_tmp424;struct Cyc_List_List*_tmp426;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp425)->tag == 9U){_LL9: _tmp426=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp425)->f1).attributes;_LLA:
# 1709
({struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_callconv2doc(_tmp426);beforenamedoc=_tmp849;});
goto _LL8;}else{_LLB: _LLC:
({struct Cyc_PP_Doc*_tmp84A=Cyc_PP_nil_doc();beforenamedoc=_tmp84A;});goto _LL8;}_LL8:;}
# 1713
goto _LL3;}_LL3:;}{
# 1716
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp427=Cyc_Cyclone_c_compiler;if(_tmp427 == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
({struct Cyc_PP_Doc*_tmp84B=Cyc_PP_nil_doc();tmp_doc=_tmp84B;});goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1721
({struct Cyc_PP_Doc*_tmp855=({struct Cyc_PP_Doc*_tmp428[5U];({struct Cyc_PP_Doc*_tmp84C=
# 1728
Cyc_PP_text(({const char*_tmp42D=";";_tag_dyneither(_tmp42D,sizeof(char),2U);}));_tmp428[4U]=_tmp84C;});({struct Cyc_PP_Doc*_tmp84F=
# 1725
_tmp42F == 0?
Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp42B[2U];({struct Cyc_PP_Doc*_tmp84D=
Cyc_Absynpp_exp2doc(_tmp42F);_tmp42B[1U]=_tmp84D;});({struct Cyc_PP_Doc*_tmp84E=Cyc_PP_text(({const char*_tmp42C=" = ";_tag_dyneither(_tmp42C,sizeof(char),4U);}));_tmp42B[0U]=_tmp84E;});Cyc_PP_cat(_tag_dyneither(_tmp42B,sizeof(struct Cyc_PP_Doc*),2U));});_tmp428[3U]=_tmp84F;});({struct Cyc_PP_Doc*_tmp853=({
# 1724
struct Cyc_Absyn_Tqual _tmp852=_tmp431;void*_tmp851=_tmp430;Cyc_Absynpp_tqtd2doc(_tmp852,_tmp851,({struct Cyc_Core_Opt*_tmp429=_cycalloc(sizeof(*_tmp429));({struct Cyc_PP_Doc*_tmp850=({struct Cyc_PP_Doc*_tmp42A[2U];_tmp42A[1U]=sn;_tmp42A[0U]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp42A,sizeof(struct Cyc_PP_Doc*),2U));});_tmp429->v=_tmp850;});_tmp429;}));});_tmp428[2U]=_tmp853;});({struct Cyc_PP_Doc*_tmp854=
# 1723
Cyc_Absynpp_scope2doc(_tmp434);_tmp428[1U]=_tmp854;});_tmp428[0U]=tmp_doc;Cyc_PP_cat(_tag_dyneither(_tmp428,sizeof(struct Cyc_PP_Doc*),5U));});
# 1721
s=_tmp855;});
# 1729
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1732
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp435=*x;struct _tuple17 _tmp436=_tmp435;struct _tuple0*_tmp437;_LL1: _tmp437=_tmp436.f2;_LL2:;
return Cyc_Absynpp_qvar2doc(_tmp437);}
# 1737
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp438[4U];({struct Cyc_PP_Doc*_tmp856=
# 1742
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp438[3U]=_tmp856;});({struct Cyc_PP_Doc*_tmp857=
# 1741
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp438[2U]=_tmp857;});({struct Cyc_PP_Doc*_tmp858=
# 1740
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp438[1U]=_tmp858;});({struct Cyc_PP_Doc*_tmp859=
# 1739
Cyc_Absynpp_scope2doc(ad->sc);_tmp438[0U]=_tmp859;});Cyc_PP_cat(_tag_dyneither(_tmp438,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1744
return({struct Cyc_PP_Doc*_tmp439[12U];({struct Cyc_PP_Doc*_tmp85A=
# 1755
Cyc_Absynpp_atts2doc(ad->attributes);_tmp439[11U]=_tmp85A;});({struct Cyc_PP_Doc*_tmp85B=
# 1754
Cyc_Absynpp_rb();_tmp439[10U]=_tmp85B;});({struct Cyc_PP_Doc*_tmp85C=
# 1753
Cyc_PP_line_doc();_tmp439[9U]=_tmp85C;});({struct Cyc_PP_Doc*_tmp85F=
# 1752
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43C[2U];({struct Cyc_PP_Doc*_tmp85D=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43C[1U]=_tmp85D;});({struct Cyc_PP_Doc*_tmp85E=Cyc_PP_line_doc();_tmp43C[0U]=_tmp85E;});Cyc_PP_cat(_tag_dyneither(_tmp43C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp439[8U]=_tmp85F;});({struct Cyc_PP_Doc*_tmp862=
# 1750
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp43A[2U];({struct Cyc_PP_Doc*_tmp860=
Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp43A[1U]=_tmp860;});({struct Cyc_PP_Doc*_tmp861=Cyc_PP_text(({const char*_tmp43B=":";_tag_dyneither(_tmp43B,sizeof(char),2U);}));_tmp43A[0U]=_tmp861;});Cyc_PP_cat(_tag_dyneither(_tmp43A,sizeof(struct Cyc_PP_Doc*),2U));});_tmp439[7U]=_tmp862;});({struct Cyc_PP_Doc*_tmp863=
# 1749
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp439[6U]=_tmp863;});({struct Cyc_PP_Doc*_tmp864=
# 1748
Cyc_Absynpp_lb();_tmp439[5U]=_tmp864;});({struct Cyc_PP_Doc*_tmp865=Cyc_PP_blank_doc();_tmp439[4U]=_tmp865;});({struct Cyc_PP_Doc*_tmp866=
# 1747
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp439[3U]=_tmp866;});({struct Cyc_PP_Doc*_tmp867=
# 1746
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp439[2U]=_tmp867;});({struct Cyc_PP_Doc*_tmp868=
# 1745
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp439[1U]=_tmp868;});({struct Cyc_PP_Doc*_tmp869=
# 1744
Cyc_Absynpp_scope2doc(ad->sc);_tmp439[0U]=_tmp869;});Cyc_PP_cat(_tag_dyneither(_tmp439,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1758
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43D=dd;enum Cyc_Absyn_Scope _tmp449;struct _tuple0*_tmp448;struct Cyc_List_List*_tmp447;struct Cyc_Core_Opt*_tmp446;int _tmp445;_LL1: _tmp449=_tmp43D->sc;_tmp448=_tmp43D->name;_tmp447=_tmp43D->tvs;_tmp446=_tmp43D->fields;_tmp445=_tmp43D->is_extensible;_LL2:;
if(_tmp446 == 0)
return({struct Cyc_PP_Doc*_tmp43E[5U];({struct Cyc_PP_Doc*_tmp86A=
# 1765
Cyc_Absynpp_ktvars2doc(_tmp447);_tmp43E[4U]=_tmp86A;});({struct Cyc_PP_Doc*_tmp86B=
# 1764
_tmp445?Cyc_Absynpp_qvar2bolddoc(_tmp448): Cyc_Absynpp_typedef_name2bolddoc(_tmp448);_tmp43E[3U]=_tmp86B;});({struct Cyc_PP_Doc*_tmp86C=
# 1763
Cyc_PP_text(({const char*_tmp440="datatype ";_tag_dyneither(_tmp440,sizeof(char),10U);}));_tmp43E[2U]=_tmp86C;});({struct Cyc_PP_Doc*_tmp86D=
# 1762
_tmp445?Cyc_PP_text(({const char*_tmp43F="@extensible ";_tag_dyneither(_tmp43F,sizeof(char),13U);})): Cyc_PP_blank_doc();_tmp43E[1U]=_tmp86D;});({struct Cyc_PP_Doc*_tmp86E=
# 1761
Cyc_Absynpp_scope2doc(_tmp449);_tmp43E[0U]=_tmp86E;});Cyc_PP_cat(_tag_dyneither(_tmp43E,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1767
return({struct Cyc_PP_Doc*_tmp441[10U];({struct Cyc_PP_Doc*_tmp86F=
# 1775
Cyc_Absynpp_rb();_tmp441[9U]=_tmp86F;});({struct Cyc_PP_Doc*_tmp870=
# 1774
Cyc_PP_line_doc();_tmp441[8U]=_tmp870;});({struct Cyc_PP_Doc*_tmp873=
# 1773
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp444[2U];({struct Cyc_PP_Doc*_tmp871=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp446->v);_tmp444[1U]=_tmp871;});({struct Cyc_PP_Doc*_tmp872=Cyc_PP_line_doc();_tmp444[0U]=_tmp872;});Cyc_PP_cat(_tag_dyneither(_tmp444,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp441[7U]=_tmp873;});({struct Cyc_PP_Doc*_tmp874=
# 1772
Cyc_Absynpp_lb();_tmp441[6U]=_tmp874;});({struct Cyc_PP_Doc*_tmp875=Cyc_PP_blank_doc();_tmp441[5U]=_tmp875;});({struct Cyc_PP_Doc*_tmp876=
# 1771
Cyc_Absynpp_ktvars2doc(_tmp447);_tmp441[4U]=_tmp876;});({struct Cyc_PP_Doc*_tmp877=
# 1770
_tmp445?Cyc_Absynpp_qvar2bolddoc(_tmp448): Cyc_Absynpp_typedef_name2bolddoc(_tmp448);_tmp441[3U]=_tmp877;});({struct Cyc_PP_Doc*_tmp878=
# 1769
Cyc_PP_text(({const char*_tmp443="datatype ";_tag_dyneither(_tmp443,sizeof(char),10U);}));_tmp441[2U]=_tmp878;});({struct Cyc_PP_Doc*_tmp879=
# 1768
_tmp445?Cyc_PP_text(({const char*_tmp442="@extensible ";_tag_dyneither(_tmp442,sizeof(char),13U);})): Cyc_PP_blank_doc();_tmp441[1U]=_tmp879;});({struct Cyc_PP_Doc*_tmp87A=
# 1767
Cyc_Absynpp_scope2doc(_tmp449);_tmp441[0U]=_tmp87A;});Cyc_PP_cat(_tag_dyneither(_tmp441,sizeof(struct Cyc_PP_Doc*),10U));});}}
# 1778
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp44A=ed;enum Cyc_Absyn_Scope _tmp452;struct _tuple0*_tmp451;struct Cyc_Core_Opt*_tmp450;_LL1: _tmp452=_tmp44A->sc;_tmp451=_tmp44A->name;_tmp450=_tmp44A->fields;_LL2:;
if(_tmp450 == 0)
return({struct Cyc_PP_Doc*_tmp44B[3U];({struct Cyc_PP_Doc*_tmp87B=
# 1783
Cyc_Absynpp_typedef_name2bolddoc(_tmp451);_tmp44B[2U]=_tmp87B;});({struct Cyc_PP_Doc*_tmp87C=
# 1782
Cyc_PP_text(({const char*_tmp44C="enum ";_tag_dyneither(_tmp44C,sizeof(char),6U);}));_tmp44B[1U]=_tmp87C;});({struct Cyc_PP_Doc*_tmp87D=
# 1781
Cyc_Absynpp_scope2doc(_tmp452);_tmp44B[0U]=_tmp87D;});Cyc_PP_cat(_tag_dyneither(_tmp44B,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1786
return({struct Cyc_PP_Doc*_tmp44D[8U];({struct Cyc_PP_Doc*_tmp87E=
# 1792
Cyc_Absynpp_rb();_tmp44D[7U]=_tmp87E;});({struct Cyc_PP_Doc*_tmp87F=
# 1791
Cyc_PP_line_doc();_tmp44D[6U]=_tmp87F;});({struct Cyc_PP_Doc*_tmp882=
# 1790
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp44F[2U];({struct Cyc_PP_Doc*_tmp880=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp450->v);_tmp44F[1U]=_tmp880;});({struct Cyc_PP_Doc*_tmp881=Cyc_PP_line_doc();_tmp44F[0U]=_tmp881;});Cyc_PP_cat(_tag_dyneither(_tmp44F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp44D[5U]=_tmp882;});({struct Cyc_PP_Doc*_tmp883=
# 1789
Cyc_Absynpp_lb();_tmp44D[4U]=_tmp883;});({struct Cyc_PP_Doc*_tmp884=Cyc_PP_blank_doc();_tmp44D[3U]=_tmp884;});({struct Cyc_PP_Doc*_tmp885=
# 1788
Cyc_Absynpp_qvar2bolddoc(_tmp451);_tmp44D[2U]=_tmp885;});({struct Cyc_PP_Doc*_tmp886=
# 1787
Cyc_PP_text(({const char*_tmp44E="enum ";_tag_dyneither(_tmp44E,sizeof(char),6U);}));_tmp44D[1U]=_tmp886;});({struct Cyc_PP_Doc*_tmp887=
# 1786
Cyc_Absynpp_scope2doc(_tmp452);_tmp44D[0U]=_tmp887;});Cyc_PP_cat(_tag_dyneither(_tmp44D,sizeof(struct Cyc_PP_Doc*),8U));});}}
# 1795
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp453=d->r;void*_tmp454=_tmp453;struct Cyc_List_List*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct Cyc_List_List*_tmp4B2;struct _tuple0*_tmp4B1;struct Cyc_List_List*_tmp4B0;struct _dyneither_ptr*_tmp4AF;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_Typedefdecl*_tmp4AD;struct Cyc_Absyn_Enumdecl*_tmp4AC;struct Cyc_List_List*_tmp4AB;struct Cyc_Absyn_Pat*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Datatypedecl*_tmp4A8;struct Cyc_Absyn_Tvar*_tmp4A7;struct Cyc_Absyn_Vardecl*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Vardecl*_tmp4A4;struct Cyc_Absyn_Aggrdecl*_tmp4A3;struct Cyc_Absyn_Fndecl*_tmp4A2;switch(*((int*)_tmp454)){case 1U: _LL1: _tmp4A2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LL2: {
# 1799
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp460=_cycalloc(sizeof(*_tmp460));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp88A=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp461;_tmp461.tag=9U;({struct Cyc_Absyn_FnInfo _tmp889=({struct Cyc_Absyn_FnInfo _tmp462;_tmp462.tvars=_tmp4A2->tvs;_tmp462.effect=_tmp4A2->effect;_tmp462.ret_tqual=_tmp4A2->ret_tqual;_tmp462.ret_typ=_tmp4A2->ret_type;({struct Cyc_List_List*_tmp888=
# 1803
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4A2->args);_tmp462.args=_tmp888;});_tmp462.c_varargs=_tmp4A2->c_varargs;_tmp462.cyc_varargs=_tmp4A2->cyc_varargs;_tmp462.rgn_po=_tmp4A2->rgn_po;_tmp462.attributes=0;_tmp462.requires_clause=_tmp4A2->requires_clause;_tmp462.requires_relns=_tmp4A2->requires_relns;_tmp462.ensures_clause=_tmp4A2->ensures_clause;_tmp462.ensures_relns=_tmp4A2->ensures_relns;_tmp462;});_tmp461.f1=_tmp889;});_tmp461;});_tmp460[0]=_tmp88A;});_tmp460;});
# 1809
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A2->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4A2->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp455=Cyc_Cyclone_c_compiler;if(_tmp455 == Cyc_Cyclone_Gcc_c){_LL20: _LL21:
({struct Cyc_PP_Doc*_tmp88B=Cyc_PP_text(({const char*_tmp456="inline ";_tag_dyneither(_tmp456,sizeof(char),8U);}));inlinedoc=_tmp88B;});goto _LL1F;}else{_LL22: _LL23:
({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_text(({const char*_tmp457="__inline ";_tag_dyneither(_tmp457,sizeof(char),10U);}));inlinedoc=_tmp88C;});goto _LL1F;}_LL1F:;}else{
# 1817
({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_nil_doc();inlinedoc=_tmp88D;});}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4A2->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp458=Cyc_Cyclone_c_compiler;if(_tmp458 == Cyc_Cyclone_Gcc_c){_LL25: _LL26:
 beforenamedoc=attsdoc;goto _LL24;}else{_LL27: _LL28:
({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_callconv2doc(_tmp4A2->attributes);beforenamedoc=_tmp88E;});goto _LL24;}_LL24:;}{
# 1824
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4A2->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp891=Cyc_Absyn_empty_tqual(0U);void*_tmp890=t;Cyc_Absynpp_tqtd2doc(_tmp891,_tmp890,({struct Cyc_Core_Opt*_tmp45E=_cycalloc(sizeof(*_tmp45E));({struct Cyc_PP_Doc*_tmp88F=({struct Cyc_PP_Doc*_tmp45F[2U];_tmp45F[1U]=namedoc;_tmp45F[0U]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp45F,sizeof(struct Cyc_PP_Doc*),2U));});_tmp45E->v=_tmp88F;});_tmp45E;}));});
# 1827
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp45C[5U];({struct Cyc_PP_Doc*_tmp892=
# 1830
Cyc_Absynpp_rb();_tmp45C[4U]=_tmp892;});({struct Cyc_PP_Doc*_tmp893=
# 1829
Cyc_PP_line_doc();_tmp45C[3U]=_tmp893;});({struct Cyc_PP_Doc*_tmp896=
# 1828
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp45D[2U];({struct Cyc_PP_Doc*_tmp894=Cyc_Absynpp_stmt2doc(_tmp4A2->body,0);_tmp45D[1U]=_tmp894;});({struct Cyc_PP_Doc*_tmp895=Cyc_PP_line_doc();_tmp45D[0U]=_tmp895;});Cyc_PP_cat(_tag_dyneither(_tmp45D,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp45C[2U]=_tmp896;});({struct Cyc_PP_Doc*_tmp897=
# 1827
Cyc_Absynpp_lb();_tmp45C[1U]=_tmp897;});({struct Cyc_PP_Doc*_tmp898=Cyc_PP_blank_doc();_tmp45C[0U]=_tmp898;});Cyc_PP_cat(_tag_dyneither(_tmp45C,sizeof(struct Cyc_PP_Doc*),5U));});
# 1831
({struct Cyc_PP_Doc*_tmp899=({struct Cyc_PP_Doc*_tmp459[4U];_tmp459[3U]=bodydoc;_tmp459[2U]=tqtddoc;_tmp459[1U]=scopedoc;_tmp459[0U]=inlinedoc;Cyc_PP_cat(_tag_dyneither(_tmp459,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp899;});
# 1833
{enum Cyc_Cyclone_C_Compilers _tmp45A=Cyc_Cyclone_c_compiler;if(_tmp45A == Cyc_Cyclone_Vc_c){_LL2A: _LL2B:
({struct Cyc_PP_Doc*_tmp89A=({struct Cyc_PP_Doc*_tmp45B[2U];_tmp45B[1U]=s;_tmp45B[0U]=attsdoc;Cyc_PP_cat(_tag_dyneither(_tmp45B,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp89A;});goto _LL29;}else{_LL2C: _LL2D:
 goto _LL29;}_LL29:;}
# 1838
goto _LL0;};};}case 5U: _LL3: _tmp4A3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LL4:
# 1841
({struct Cyc_PP_Doc*_tmp89D=({struct Cyc_PP_Doc*_tmp463[2U];({struct Cyc_PP_Doc*_tmp89B=Cyc_PP_text(({const char*_tmp464=";";_tag_dyneither(_tmp464,sizeof(char),2U);}));_tmp463[1U]=_tmp89B;});({struct Cyc_PP_Doc*_tmp89C=Cyc_Absynpp_aggrdecl2doc(_tmp4A3);_tmp463[0U]=_tmp89C;});Cyc_PP_cat(_tag_dyneither(_tmp463,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp89D;});
goto _LL0;case 0U: _LL5: _tmp4A4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LL6:
# 1844
({struct Cyc_PP_Doc*_tmp89E=Cyc_Absynpp_vardecl2doc(_tmp4A4);s=_tmp89E;});
goto _LL0;case 4U: _LL7: _tmp4A7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_tmp4A6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp454)->f2;_tmp4A5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp454)->f3;_LL8:
# 1847
({struct Cyc_PP_Doc*_tmp8A9=({struct Cyc_PP_Doc*_tmp465[7U];({struct Cyc_PP_Doc*_tmp89F=
# 1854
Cyc_PP_text(({const char*_tmp46C=";";_tag_dyneither(_tmp46C,sizeof(char),2U);}));_tmp465[6U]=_tmp89F;});({struct Cyc_PP_Doc*_tmp8A3=
# 1852
(unsigned int)_tmp4A5?({struct Cyc_PP_Doc*_tmp469[3U];({struct Cyc_PP_Doc*_tmp8A0=
Cyc_PP_text(({const char*_tmp46B=")";_tag_dyneither(_tmp46B,sizeof(char),2U);}));_tmp469[2U]=_tmp8A0;});({struct Cyc_PP_Doc*_tmp8A1=
# 1852
Cyc_Absynpp_exp2doc(_tmp4A5);_tmp469[1U]=_tmp8A1;});({struct Cyc_PP_Doc*_tmp8A2=Cyc_PP_text(({const char*_tmp46A=" = open(";_tag_dyneither(_tmp46A,sizeof(char),9U);}));_tmp469[0U]=_tmp8A2;});Cyc_PP_cat(_tag_dyneither(_tmp469,sizeof(struct Cyc_PP_Doc*),3U));}):
 Cyc_PP_nil_doc();_tmp465[5U]=_tmp8A3;});({struct Cyc_PP_Doc*_tmp8A4=
# 1851
Cyc_Absynpp_qvar2doc(_tmp4A6->name);_tmp465[4U]=_tmp8A4;});({struct Cyc_PP_Doc*_tmp8A5=
# 1850
Cyc_PP_text(({const char*_tmp468=">";_tag_dyneither(_tmp468,sizeof(char),2U);}));_tmp465[3U]=_tmp8A5;});({struct Cyc_PP_Doc*_tmp8A6=
# 1849
Cyc_Absynpp_tvar2doc(_tmp4A7);_tmp465[2U]=_tmp8A6;});({struct Cyc_PP_Doc*_tmp8A7=
# 1848
Cyc_PP_text(({const char*_tmp467="<";_tag_dyneither(_tmp467,sizeof(char),2U);}));_tmp465[1U]=_tmp8A7;});({struct Cyc_PP_Doc*_tmp8A8=
# 1847
Cyc_PP_text(({const char*_tmp466="region";_tag_dyneither(_tmp466,sizeof(char),7U);}));_tmp465[0U]=_tmp8A8;});Cyc_PP_cat(_tag_dyneither(_tmp465,sizeof(struct Cyc_PP_Doc*),7U));});s=_tmp8A9;});
# 1855
goto _LL0;case 6U: _LL9: _tmp4A8=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LLA:
# 1857
({struct Cyc_PP_Doc*_tmp8AC=({struct Cyc_PP_Doc*_tmp46D[2U];({struct Cyc_PP_Doc*_tmp8AA=Cyc_PP_text(({const char*_tmp46E=";";_tag_dyneither(_tmp46E,sizeof(char),2U);}));_tmp46D[1U]=_tmp8AA;});({struct Cyc_PP_Doc*_tmp8AB=Cyc_Absynpp_datatypedecl2doc(_tmp4A8);_tmp46D[0U]=_tmp8AB;});Cyc_PP_cat(_tag_dyneither(_tmp46D,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp8AC;});
goto _LL0;case 2U: _LLB: _tmp4AA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_tmp4A9=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp454)->f3;_LLC:
# 1860
({struct Cyc_PP_Doc*_tmp8B2=({struct Cyc_PP_Doc*_tmp46F[5U];({struct Cyc_PP_Doc*_tmp8AD=
# 1864
Cyc_PP_text(({const char*_tmp472=";";_tag_dyneither(_tmp472,sizeof(char),2U);}));_tmp46F[4U]=_tmp8AD;});({struct Cyc_PP_Doc*_tmp8AE=
# 1863
Cyc_Absynpp_exp2doc(_tmp4A9);_tmp46F[3U]=_tmp8AE;});({struct Cyc_PP_Doc*_tmp8AF=
# 1862
Cyc_PP_text(({const char*_tmp471=" = ";_tag_dyneither(_tmp471,sizeof(char),4U);}));_tmp46F[2U]=_tmp8AF;});({struct Cyc_PP_Doc*_tmp8B0=
# 1861
Cyc_Absynpp_pat2doc(_tmp4AA);_tmp46F[1U]=_tmp8B0;});({struct Cyc_PP_Doc*_tmp8B1=
# 1860
Cyc_PP_text(({const char*_tmp470="let ";_tag_dyneither(_tmp470,sizeof(char),5U);}));_tmp46F[0U]=_tmp8B1;});Cyc_PP_cat(_tag_dyneither(_tmp46F,sizeof(struct Cyc_PP_Doc*),5U));});s=_tmp8B2;});
# 1865
goto _LL0;case 3U: _LLD: _tmp4AB=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LLE:
# 1867
({struct Cyc_PP_Doc*_tmp8B6=({struct Cyc_PP_Doc*_tmp473[3U];({struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_text(({const char*_tmp475=";";_tag_dyneither(_tmp475,sizeof(char),2U);}));_tmp473[2U]=_tmp8B3;});({struct Cyc_PP_Doc*_tmp8B4=Cyc_Absynpp_ids2doc(_tmp4AB);_tmp473[1U]=_tmp8B4;});({struct Cyc_PP_Doc*_tmp8B5=Cyc_PP_text(({const char*_tmp474="let ";_tag_dyneither(_tmp474,sizeof(char),5U);}));_tmp473[0U]=_tmp8B5;});Cyc_PP_cat(_tag_dyneither(_tmp473,sizeof(struct Cyc_PP_Doc*),3U));});s=_tmp8B6;});
goto _LL0;case 7U: _LLF: _tmp4AC=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LL10:
# 1870
({struct Cyc_PP_Doc*_tmp8B9=({struct Cyc_PP_Doc*_tmp476[2U];({struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_text(({const char*_tmp477=";";_tag_dyneither(_tmp477,sizeof(char),2U);}));_tmp476[1U]=_tmp8B7;});({struct Cyc_PP_Doc*_tmp8B8=Cyc_Absynpp_enumdecl2doc(_tmp4AC);_tmp476[0U]=_tmp8B8;});Cyc_PP_cat(_tag_dyneither(_tmp476,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp8B9;});
goto _LL0;case 8U: _LL11: _tmp4AD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LL12: {
# 1873
void*t;
if(_tmp4AD->defn != 0)
t=(void*)_check_null(_tmp4AD->defn);else{
# 1877
({void*_tmp8BA=Cyc_Absyn_new_evar(_tmp4AD->kind,0);t=_tmp8BA;});}
({struct Cyc_PP_Doc*_tmp8C4=({struct Cyc_PP_Doc*_tmp478[4U];({struct Cyc_PP_Doc*_tmp8BB=
# 1885
Cyc_PP_text(({const char*_tmp47C=";";_tag_dyneither(_tmp47C,sizeof(char),2U);}));_tmp478[3U]=_tmp8BB;});({struct Cyc_PP_Doc*_tmp8BC=
# 1884
Cyc_Absynpp_atts2doc(_tmp4AD->atts);_tmp478[2U]=_tmp8BC;});({struct Cyc_PP_Doc*_tmp8C2=({
# 1879
struct Cyc_Absyn_Tqual _tmp8C1=_tmp4AD->tq;void*_tmp8C0=t;Cyc_Absynpp_tqtd2doc(_tmp8C1,_tmp8C0,({struct Cyc_Core_Opt*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct Cyc_PP_Doc*_tmp8BF=({struct Cyc_PP_Doc*_tmp47B[2U];({struct Cyc_PP_Doc*_tmp8BD=
# 1882
Cyc_Absynpp_tvars2doc(_tmp4AD->tvs);_tmp47B[1U]=_tmp8BD;});({struct Cyc_PP_Doc*_tmp8BE=
# 1881
Cyc_Absynpp_typedef_name2bolddoc(_tmp4AD->name);_tmp47B[0U]=_tmp8BE;});Cyc_PP_cat(_tag_dyneither(_tmp47B,sizeof(struct Cyc_PP_Doc*),2U));});_tmp47A->v=_tmp8BF;});_tmp47A;}));});_tmp478[1U]=_tmp8C2;});({struct Cyc_PP_Doc*_tmp8C3=
# 1878
Cyc_PP_text(({const char*_tmp479="typedef ";_tag_dyneither(_tmp479,sizeof(char),9U);}));_tmp478[0U]=_tmp8C3;});Cyc_PP_cat(_tag_dyneither(_tmp478,sizeof(struct Cyc_PP_Doc*),4U));});s=_tmp8C4;});
# 1887
goto _LL0;}case 9U: _LL13: _tmp4AF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_tmp4AE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp454)->f2;_LL14:
# 1889
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4AF);
({struct Cyc_PP_Doc*_tmp8CE=({struct Cyc_PP_Doc*_tmp47D[8U];({struct Cyc_PP_Doc*_tmp8C5=
# 1896
Cyc_Absynpp_rb();_tmp47D[7U]=_tmp8C5;});({struct Cyc_PP_Doc*_tmp8C6=
# 1895
Cyc_PP_line_doc();_tmp47D[6U]=_tmp8C6;});({struct Cyc_PP_Doc*_tmp8C8=({
# 1894
struct _dyneither_ptr _tmp8C7=({const char*_tmp47F="";_tag_dyneither(_tmp47F,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8C7,_tmp4AE);});_tmp47D[5U]=_tmp8C8;});({struct Cyc_PP_Doc*_tmp8C9=
# 1893
Cyc_PP_line_doc();_tmp47D[4U]=_tmp8C9;});({struct Cyc_PP_Doc*_tmp8CA=
# 1892
Cyc_Absynpp_lb();_tmp47D[3U]=_tmp8CA;});({struct Cyc_PP_Doc*_tmp8CB=Cyc_PP_blank_doc();_tmp47D[2U]=_tmp8CB;});({struct Cyc_PP_Doc*_tmp8CC=
# 1891
Cyc_PP_textptr(_tmp4AF);_tmp47D[1U]=_tmp8CC;});({struct Cyc_PP_Doc*_tmp8CD=
# 1890
Cyc_PP_text(({const char*_tmp47E="namespace ";_tag_dyneither(_tmp47E,sizeof(char),11U);}));_tmp47D[0U]=_tmp8CD;});Cyc_PP_cat(_tag_dyneither(_tmp47D,sizeof(struct Cyc_PP_Doc*),8U));});s=_tmp8CE;});
# 1897
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;case 10U: _LL15: _tmp4B1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_tmp4B0=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp454)->f2;_LL16:
# 1900
 if(Cyc_Absynpp_print_using_stmts)
({struct Cyc_PP_Doc*_tmp8D8=({struct Cyc_PP_Doc*_tmp480[8U];({struct Cyc_PP_Doc*_tmp8CF=
# 1907
Cyc_Absynpp_rb();_tmp480[7U]=_tmp8CF;});({struct Cyc_PP_Doc*_tmp8D0=
# 1906
Cyc_PP_line_doc();_tmp480[6U]=_tmp8D0;});({struct Cyc_PP_Doc*_tmp8D2=({
# 1905
struct _dyneither_ptr _tmp8D1=({const char*_tmp482="";_tag_dyneither(_tmp482,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8D1,_tmp4B0);});_tmp480[5U]=_tmp8D2;});({struct Cyc_PP_Doc*_tmp8D3=
# 1904
Cyc_PP_line_doc();_tmp480[4U]=_tmp8D3;});({struct Cyc_PP_Doc*_tmp8D4=
# 1903
Cyc_Absynpp_lb();_tmp480[3U]=_tmp8D4;});({struct Cyc_PP_Doc*_tmp8D5=Cyc_PP_blank_doc();_tmp480[2U]=_tmp8D5;});({struct Cyc_PP_Doc*_tmp8D6=
# 1902
Cyc_Absynpp_qvar2doc(_tmp4B1);_tmp480[1U]=_tmp8D6;});({struct Cyc_PP_Doc*_tmp8D7=
# 1901
Cyc_PP_text(({const char*_tmp481="using ";_tag_dyneither(_tmp481,sizeof(char),7U);}));_tmp480[0U]=_tmp8D7;});Cyc_PP_cat(_tag_dyneither(_tmp480,sizeof(struct Cyc_PP_Doc*),8U));});s=_tmp8D8;});else{
# 1909
({struct Cyc_PP_Doc*_tmp8E5=({struct Cyc_PP_Doc*_tmp483[11U];({struct Cyc_PP_Doc*_tmp8D9=
# 1919
Cyc_PP_text(({const char*_tmp488=" */";_tag_dyneither(_tmp488,sizeof(char),4U);}));_tmp483[10U]=_tmp8D9;});({struct Cyc_PP_Doc*_tmp8DA=
# 1918
Cyc_Absynpp_rb();_tmp483[9U]=_tmp8DA;});({struct Cyc_PP_Doc*_tmp8DB=
# 1917
Cyc_PP_text(({const char*_tmp487="/* ";_tag_dyneither(_tmp487,sizeof(char),4U);}));_tmp483[8U]=_tmp8DB;});({struct Cyc_PP_Doc*_tmp8DC=
# 1916
Cyc_PP_line_doc();_tmp483[7U]=_tmp8DC;});({struct Cyc_PP_Doc*_tmp8DE=({
# 1915
struct _dyneither_ptr _tmp8DD=({const char*_tmp486="";_tag_dyneither(_tmp486,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8DD,_tmp4B0);});_tmp483[6U]=_tmp8DE;});({struct Cyc_PP_Doc*_tmp8DF=
# 1914
Cyc_PP_line_doc();_tmp483[5U]=_tmp8DF;});({struct Cyc_PP_Doc*_tmp8E0=
# 1913
Cyc_PP_text(({const char*_tmp485=" */";_tag_dyneither(_tmp485,sizeof(char),4U);}));_tmp483[4U]=_tmp8E0;});({struct Cyc_PP_Doc*_tmp8E1=
# 1912
Cyc_Absynpp_lb();_tmp483[3U]=_tmp8E1;});({struct Cyc_PP_Doc*_tmp8E2=
# 1911
Cyc_PP_blank_doc();_tmp483[2U]=_tmp8E2;});({struct Cyc_PP_Doc*_tmp8E3=
# 1910
Cyc_Absynpp_qvar2doc(_tmp4B1);_tmp483[1U]=_tmp8E3;});({struct Cyc_PP_Doc*_tmp8E4=
# 1909
Cyc_PP_text(({const char*_tmp484="/* using ";_tag_dyneither(_tmp484,sizeof(char),10U);}));_tmp483[0U]=_tmp8E4;});Cyc_PP_cat(_tag_dyneither(_tmp483,sizeof(struct Cyc_PP_Doc*),11U));});s=_tmp8E5;});}
# 1920
goto _LL0;case 11U: _LL17: _tmp4B2=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_LL18:
# 1922
 if(Cyc_Absynpp_print_externC_stmts)
({struct Cyc_PP_Doc*_tmp8ED=({struct Cyc_PP_Doc*_tmp489[6U];({struct Cyc_PP_Doc*_tmp8E6=
# 1928
Cyc_Absynpp_rb();_tmp489[5U]=_tmp8E6;});({struct Cyc_PP_Doc*_tmp8E7=
# 1927
Cyc_PP_line_doc();_tmp489[4U]=_tmp8E7;});({struct Cyc_PP_Doc*_tmp8E9=({
# 1926
struct _dyneither_ptr _tmp8E8=({const char*_tmp48B="";_tag_dyneither(_tmp48B,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8E8,_tmp4B2);});_tmp489[3U]=_tmp8E9;});({struct Cyc_PP_Doc*_tmp8EA=
# 1925
Cyc_PP_line_doc();_tmp489[2U]=_tmp8EA;});({struct Cyc_PP_Doc*_tmp8EB=
# 1924
Cyc_Absynpp_lb();_tmp489[1U]=_tmp8EB;});({struct Cyc_PP_Doc*_tmp8EC=
# 1923
Cyc_PP_text(({const char*_tmp48A="extern \"C\" ";_tag_dyneither(_tmp48A,sizeof(char),12U);}));_tmp489[0U]=_tmp8EC;});Cyc_PP_cat(_tag_dyneither(_tmp489,sizeof(struct Cyc_PP_Doc*),6U));});s=_tmp8ED;});else{
# 1930
({struct Cyc_PP_Doc*_tmp8F8=({struct Cyc_PP_Doc*_tmp48C[9U];({struct Cyc_PP_Doc*_tmp8EE=
# 1938
Cyc_PP_text(({const char*_tmp491=" */";_tag_dyneither(_tmp491,sizeof(char),4U);}));_tmp48C[8U]=_tmp8EE;});({struct Cyc_PP_Doc*_tmp8EF=
# 1937
Cyc_Absynpp_rb();_tmp48C[7U]=_tmp8EF;});({struct Cyc_PP_Doc*_tmp8F0=
# 1936
Cyc_PP_text(({const char*_tmp490="/* ";_tag_dyneither(_tmp490,sizeof(char),4U);}));_tmp48C[6U]=_tmp8F0;});({struct Cyc_PP_Doc*_tmp8F1=
# 1935
Cyc_PP_line_doc();_tmp48C[5U]=_tmp8F1;});({struct Cyc_PP_Doc*_tmp8F3=({
# 1934
struct _dyneither_ptr _tmp8F2=({const char*_tmp48F="";_tag_dyneither(_tmp48F,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8F2,_tmp4B2);});_tmp48C[4U]=_tmp8F3;});({struct Cyc_PP_Doc*_tmp8F4=
# 1933
Cyc_PP_line_doc();_tmp48C[3U]=_tmp8F4;});({struct Cyc_PP_Doc*_tmp8F5=
# 1932
Cyc_PP_text(({const char*_tmp48E=" */";_tag_dyneither(_tmp48E,sizeof(char),4U);}));_tmp48C[2U]=_tmp8F5;});({struct Cyc_PP_Doc*_tmp8F6=
# 1931
Cyc_Absynpp_lb();_tmp48C[1U]=_tmp8F6;});({struct Cyc_PP_Doc*_tmp8F7=
# 1930
Cyc_PP_text(({const char*_tmp48D="/* extern \"C\" ";_tag_dyneither(_tmp48D,sizeof(char),15U);}));_tmp48C[0U]=_tmp8F7;});Cyc_PP_cat(_tag_dyneither(_tmp48C,sizeof(struct Cyc_PP_Doc*),9U));});s=_tmp8F8;});}
# 1939
goto _LL0;case 12U: _LL19: _tmp4B4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp454)->f1;_tmp4B3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp454)->f2;_LL1A:
# 1941
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4B3 != 0)
({struct Cyc_PP_Doc*_tmp901=({struct Cyc_PP_Doc*_tmp492[7U];({struct Cyc_PP_Doc*_tmp8F9=
# 1946
Cyc_Absynpp_rb();_tmp492[6U]=_tmp8F9;});({struct Cyc_PP_Doc*_tmp8FA=Cyc_PP_line_doc();_tmp492[5U]=_tmp8FA;});({struct Cyc_PP_Doc*_tmp8FC=({
# 1945
struct _dyneither_ptr _tmp8FB=({const char*_tmp494=",";_tag_dyneither(_tmp494,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp8FB,_tmp4B3);});_tmp492[4U]=_tmp8FC;});({struct Cyc_PP_Doc*_tmp8FD=Cyc_PP_line_doc();_tmp492[3U]=_tmp8FD;});({struct Cyc_PP_Doc*_tmp8FE=
# 1944
Cyc_Absynpp_lb();_tmp492[2U]=_tmp8FE;});({struct Cyc_PP_Doc*_tmp8FF=Cyc_PP_text(({const char*_tmp493=" export ";_tag_dyneither(_tmp493,sizeof(char),9U);}));_tmp492[1U]=_tmp8FF;});({struct Cyc_PP_Doc*_tmp900=Cyc_Absynpp_rb();_tmp492[0U]=_tmp900;});Cyc_PP_cat(_tag_dyneither(_tmp492,sizeof(struct Cyc_PP_Doc*),7U));});exs_doc=_tmp901;});else{
# 1948
({struct Cyc_PP_Doc*_tmp902=Cyc_Absynpp_rb();exs_doc=_tmp902;});}
({struct Cyc_PP_Doc*_tmp909=({struct Cyc_PP_Doc*_tmp495[6U];_tmp495[5U]=exs_doc;({struct Cyc_PP_Doc*_tmp903=
# 1953
Cyc_PP_line_doc();_tmp495[4U]=_tmp903;});({struct Cyc_PP_Doc*_tmp905=({
# 1952
struct _dyneither_ptr _tmp904=({const char*_tmp497="";_tag_dyneither(_tmp497,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp904,_tmp4B4);});_tmp495[3U]=_tmp905;});({struct Cyc_PP_Doc*_tmp906=
# 1951
Cyc_PP_line_doc();_tmp495[2U]=_tmp906;});({struct Cyc_PP_Doc*_tmp907=
# 1950
Cyc_Absynpp_lb();_tmp495[1U]=_tmp907;});({struct Cyc_PP_Doc*_tmp908=
# 1949
Cyc_PP_text(({const char*_tmp496="extern \"C include\" ";_tag_dyneither(_tmp496,sizeof(char),20U);}));_tmp495[0U]=_tmp908;});Cyc_PP_cat(_tag_dyneither(_tmp495,sizeof(struct Cyc_PP_Doc*),6U));});s=_tmp909;});}else{
# 1956
({struct Cyc_PP_Doc*_tmp914=({struct Cyc_PP_Doc*_tmp498[9U];({struct Cyc_PP_Doc*_tmp90A=
# 1964
Cyc_PP_text(({const char*_tmp49D=" */";_tag_dyneither(_tmp49D,sizeof(char),4U);}));_tmp498[8U]=_tmp90A;});({struct Cyc_PP_Doc*_tmp90B=
# 1963
Cyc_Absynpp_rb();_tmp498[7U]=_tmp90B;});({struct Cyc_PP_Doc*_tmp90C=
# 1962
Cyc_PP_text(({const char*_tmp49C="/* ";_tag_dyneither(_tmp49C,sizeof(char),4U);}));_tmp498[6U]=_tmp90C;});({struct Cyc_PP_Doc*_tmp90D=
# 1961
Cyc_PP_line_doc();_tmp498[5U]=_tmp90D;});({struct Cyc_PP_Doc*_tmp90F=({
# 1960
struct _dyneither_ptr _tmp90E=({const char*_tmp49B="";_tag_dyneither(_tmp49B,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp90E,_tmp4B4);});_tmp498[4U]=_tmp90F;});({struct Cyc_PP_Doc*_tmp910=
# 1959
Cyc_PP_line_doc();_tmp498[3U]=_tmp910;});({struct Cyc_PP_Doc*_tmp911=
# 1958
Cyc_PP_text(({const char*_tmp49A=" */";_tag_dyneither(_tmp49A,sizeof(char),4U);}));_tmp498[2U]=_tmp911;});({struct Cyc_PP_Doc*_tmp912=
# 1957
Cyc_Absynpp_lb();_tmp498[1U]=_tmp912;});({struct Cyc_PP_Doc*_tmp913=
# 1956
Cyc_PP_text(({const char*_tmp499="/* extern \"C include\" ";_tag_dyneither(_tmp499,sizeof(char),23U);}));_tmp498[0U]=_tmp913;});Cyc_PP_cat(_tag_dyneither(_tmp498,sizeof(struct Cyc_PP_Doc*),9U));});s=_tmp914;});}
# 1965
goto _LL0;case 13U: _LL1B: _LL1C:
# 1967
({struct Cyc_PP_Doc*_tmp917=({struct Cyc_PP_Doc*_tmp49E[2U];({struct Cyc_PP_Doc*_tmp915=Cyc_Absynpp_lb();_tmp49E[1U]=_tmp915;});({struct Cyc_PP_Doc*_tmp916=Cyc_PP_text(({const char*_tmp49F="__cyclone_port_on__;";_tag_dyneither(_tmp49F,sizeof(char),21U);}));_tmp49E[0U]=_tmp916;});Cyc_PP_cat(_tag_dyneither(_tmp49E,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp917;});
goto _LL0;default: _LL1D: _LL1E:
# 1970
({struct Cyc_PP_Doc*_tmp91A=({struct Cyc_PP_Doc*_tmp4A0[2U];({struct Cyc_PP_Doc*_tmp918=Cyc_Absynpp_lb();_tmp4A0[1U]=_tmp918;});({struct Cyc_PP_Doc*_tmp919=Cyc_PP_text(({const char*_tmp4A1="__cyclone_port_off__;";_tag_dyneither(_tmp4A1,sizeof(char),22U);}));_tmp4A0[0U]=_tmp919;});Cyc_PP_cat(_tag_dyneither(_tmp4A0,sizeof(struct Cyc_PP_Doc*),2U));});s=_tmp91A;});
goto _LL0;}_LL0:;}
# 1974
return s;}
# 1977
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4B5=sc;switch(_tmp4B5){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4B6="static ";_tag_dyneither(_tmp4B6,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4B7="extern ";_tag_dyneither(_tmp4B7,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4B8="extern \"C\" ";_tag_dyneither(_tmp4B8,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4B9="abstract ";_tag_dyneither(_tmp4B9,sizeof(char),10U);}));default: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4BA="register ";_tag_dyneither(_tmp4BA,sizeof(char),10U);}));}_LL0:;};}
# 1990
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4BB=t;struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Tvar*_tmp4BC;switch(*((int*)_tmp4BB)){case 2U: _LL1: _tmp4BC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4BB)->f1;_LL2:
 return Cyc_Tcutil_is_temp_tvar(_tmp4BC);case 24U: _LL3: _tmp4BD=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4BB)->f1;_LL4:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4BD);default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2002
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4BE=t;struct Cyc_Absyn_Typedefdecl*_tmp4C0;void*_tmp4BF;switch(*((int*)_tmp4BE)){case 12U: _LL1: _LL2:
 return 1;case 14U: _LL3: _LL4:
 return 1;case 17U: _LL5: _tmp4C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BE)->f3;_tmp4BF=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BE)->f4;if(_tmp4BF != 0){_LL6:
# 2009
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4BF);}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2016
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2019
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4C1=({struct _tuple16 _tmp4C6;_tmp4C6.f1=(void*)tms->hd;_tmp4C6.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp4C6;});struct _tuple16 _tmp4C2=_tmp4C1;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4C2.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4C2.f2)->tag == 3U){_LL1: _LL2:
# 2022
 return({struct Cyc_List_List*_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3));_tmp4C3->hd=(void*)tms->hd;({struct Cyc_List_List*_tmp91C=({struct Cyc_List_List*_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4));_tmp4C4->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;({struct Cyc_List_List*_tmp91B=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4C4->tl=_tmp91B;});_tmp4C4;});_tmp4C3->tl=_tmp91C;});_tmp4C3;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5));_tmp4C5->hd=atts;_tmp4C5->tl=tms;_tmp4C5;});}_LL0:;}else{
# 2025
return({struct Cyc_List_List*_tmp4C7=_region_malloc(r,sizeof(*_tmp4C7));_tmp4C7->hd=atts;_tmp4C7->tl=tms;_tmp4C7;});}}
# 2030
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2032
void*_tmp4C8=t;struct _tuple0*_tmp515;struct Cyc_List_List*_tmp514;struct Cyc_Absyn_Typedefdecl*_tmp513;void*_tmp512;struct Cyc_Core_Opt*_tmp511;void*_tmp510;int _tmp50F;struct Cyc_List_List*_tmp50E;void*_tmp50D;struct Cyc_Absyn_Tqual _tmp50C;void*_tmp50B;struct Cyc_List_List*_tmp50A;int _tmp509;struct Cyc_Absyn_VarargInfo*_tmp508;struct Cyc_List_List*_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp504;void*_tmp503;struct Cyc_Absyn_Tqual _tmp502;struct Cyc_Absyn_PtrAtts _tmp501;void*_tmp500;struct Cyc_Absyn_Tqual _tmp4FF;struct Cyc_Absyn_Exp*_tmp4FE;union Cyc_Absyn_Constraint*_tmp4FD;unsigned int _tmp4FC;switch(*((int*)_tmp4C8)){case 8U: _LL1: _tmp500=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C8)->f1).elt_type;_tmp4FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C8)->f1).tq;_tmp4FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C8)->f1).num_elts;_tmp4FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C8)->f1).zero_term;_tmp4FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C8)->f1).zt_loc;_LL2: {
# 2035
struct _tuple12 _tmp4C9=Cyc_Absynpp_to_tms(r,_tmp4FF,_tmp500);struct _tuple12 _tmp4CA=_tmp4C9;struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D2;struct Cyc_List_List*_tmp4D1;_LLE: _tmp4D3=_tmp4CA.f1;_tmp4D2=_tmp4CA.f2;_tmp4D1=_tmp4CA.f3;_LLF:;{
void*tm;
if(_tmp4FE == 0)
({void*_tmp91E=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CB=_region_malloc(r,sizeof(*_tmp4CB));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp91D=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp4CC;_tmp4CC.tag=0U;_tmp4CC.f1=_tmp4FD;_tmp4CC.f2=_tmp4FC;_tmp4CC;});_tmp4CB[0]=_tmp91D;});_tmp4CB;});tm=_tmp91E;});else{
# 2040
({void*_tmp920=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp91F=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp4CE;_tmp4CE.tag=1U;_tmp4CE.f1=_tmp4FE;_tmp4CE.f2=_tmp4FD;_tmp4CE.f3=_tmp4FC;_tmp4CE;});_tmp4CD[0]=_tmp91F;});_tmp4CD;});tm=_tmp920;});}
return({struct _tuple12 _tmp4CF;_tmp4CF.f1=_tmp4D3;_tmp4CF.f2=_tmp4D2;({struct Cyc_List_List*_tmp921=({struct Cyc_List_List*_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0));_tmp4D0->hd=tm;_tmp4D0->tl=_tmp4D1;_tmp4D0;});_tmp4CF.f3=_tmp921;});_tmp4CF;});};}case 5U: _LL3: _tmp503=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).elt_typ;_tmp502=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).elt_tq;_tmp501=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).ptr_atts;_LL4: {
# 2044
struct _tuple12 _tmp4D4=Cyc_Absynpp_to_tms(r,_tmp502,_tmp503);struct _tuple12 _tmp4D5=_tmp4D4;struct Cyc_Absyn_Tqual _tmp4DC;void*_tmp4DB;struct Cyc_List_List*_tmp4DA;_LL11: _tmp4DC=_tmp4D5.f1;_tmp4DB=_tmp4D5.f2;_tmp4DA=_tmp4D5.f3;_LL12:;
({struct Cyc_List_List*_tmp924=({struct Cyc_List_List*_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6));({void*_tmp923=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7));({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp922=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp4D8;_tmp4D8.tag=2U;_tmp4D8.f1=_tmp501;_tmp4D8.f2=tq;_tmp4D8;});_tmp4D7[0]=_tmp922;});_tmp4D7;});_tmp4D6->hd=_tmp923;});_tmp4D6->tl=_tmp4DA;_tmp4D6;});_tmp4DA=_tmp924;});
return({struct _tuple12 _tmp4D9;_tmp4D9.f1=_tmp4DC;_tmp4D9.f2=_tmp4DB;_tmp4D9.f3=_tmp4DA;_tmp4D9;});}case 9U: _LL5: _tmp50E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).tvars;_tmp50D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).effect;_tmp50C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ret_tqual;_tmp50B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ret_typ;_tmp50A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).args;_tmp509=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).c_varargs;_tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).cyc_varargs;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).rgn_po;_tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).attributes;_tmp505=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).requires_clause;_tmp504=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ensures_clause;_LL6:
# 2050
 if(!Cyc_Absynpp_print_all_tvars){
# 2054
if(_tmp50D == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp50D)){
_tmp50D=0;
_tmp50E=0;}}else{
# 2059
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2062
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp50E);}{
# 2067
struct _tuple12 _tmp4DD=Cyc_Absynpp_to_tms(r,_tmp50C,_tmp50B);struct _tuple12 _tmp4DE=_tmp4DD;struct Cyc_Absyn_Tqual _tmp4F8;void*_tmp4F7;struct Cyc_List_List*_tmp4F6;_LL14: _tmp4F8=_tmp4DE.f1;_tmp4F7=_tmp4DE.f2;_tmp4F6=_tmp4DE.f3;_LL15:;{
struct Cyc_List_List*tms=_tmp4F6;
# 2078 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4DF=Cyc_Cyclone_c_compiler;if(_tmp4DF == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2080
 if(_tmp506 != 0)
# 2083
({struct Cyc_List_List*_tmp928=({struct _RegionHandle*_tmp927=r;void*_tmp926=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp925=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4E1;_tmp4E1.tag=5U;_tmp4E1.f1=0U;_tmp4E1.f2=_tmp506;_tmp4E1;});_tmp4E0[0]=_tmp925;});_tmp4E0;});Cyc_Absynpp_bubble_attributes(_tmp927,_tmp926,tms);});tms=_tmp928;});
# 2085
({struct Cyc_List_List*_tmp92D=({struct Cyc_List_List*_tmp4E2=_region_malloc(r,sizeof(*_tmp4E2));({void*_tmp92C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp92B=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp4E4;_tmp4E4.tag=3U;({void*_tmp92A=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp929=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp4E6;_tmp4E6.tag=1U;_tmp4E6.f1=_tmp50A;_tmp4E6.f2=_tmp509;_tmp4E6.f3=_tmp508;_tmp4E6.f4=_tmp50D;_tmp4E6.f5=_tmp507;_tmp4E6.f6=_tmp505;_tmp4E6.f7=_tmp504;_tmp4E6;});_tmp4E5[0]=_tmp929;});_tmp4E5;});_tmp4E4.f1=_tmp92A;});_tmp4E4;});_tmp4E3[0]=_tmp92B;});_tmp4E3;});_tmp4E2->hd=_tmp92C;});_tmp4E2->tl=tms;_tmp4E2;});tms=_tmp92D;});
# 2089
goto _LL16;}else{_LL19: _LL1A:
# 2091
({struct Cyc_List_List*_tmp932=({struct Cyc_List_List*_tmp4E7=_region_malloc(r,sizeof(*_tmp4E7));({void*_tmp931=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp930=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp4E9;_tmp4E9.tag=3U;({void*_tmp92F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4EA=_region_malloc(r,sizeof(*_tmp4EA));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp92E=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp4EB;_tmp4EB.tag=1U;_tmp4EB.f1=_tmp50A;_tmp4EB.f2=_tmp509;_tmp4EB.f3=_tmp508;_tmp4EB.f4=_tmp50D;_tmp4EB.f5=_tmp507;_tmp4EB.f6=_tmp505;_tmp4EB.f7=_tmp504;_tmp4EB;});_tmp4EA[0]=_tmp92E;});_tmp4EA;});_tmp4E9.f1=_tmp92F;});_tmp4E9;});_tmp4E8[0]=_tmp930;});_tmp4E8;});_tmp4E7->hd=_tmp931;});_tmp4E7->tl=tms;_tmp4E7;});tms=_tmp932;});
# 2095
for(0;_tmp506 != 0;_tmp506=_tmp506->tl){
void*_tmp4EC=(void*)_tmp506->hd;void*_tmp4ED=_tmp4EC;switch(*((int*)_tmp4ED)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2100
({struct Cyc_List_List*_tmp936=({struct Cyc_List_List*_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE));({void*_tmp935=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp934=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4F0;_tmp4F0.tag=5U;_tmp4F0.f1=0U;({struct Cyc_List_List*_tmp933=({struct Cyc_List_List*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->hd=(void*)_tmp506->hd;_tmp4F1->tl=0;_tmp4F1;});_tmp4F0.f2=_tmp933;});_tmp4F0;});_tmp4EF[0]=_tmp934;});_tmp4EF;});_tmp4EE->hd=_tmp935;});_tmp4EE->tl=tms;_tmp4EE;});tms=_tmp936;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2104
goto _LL16;}_LL16:;}
# 2107
AfterAtts:
 if(_tmp50E != 0)
({struct Cyc_List_List*_tmp939=({struct Cyc_List_List*_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2));({void*_tmp938=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp937=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp4F4;_tmp4F4.tag=4U;_tmp4F4.f1=_tmp50E;_tmp4F4.f2=0U;_tmp4F4.f3=1;_tmp4F4;});_tmp4F3[0]=_tmp937;});_tmp4F3;});_tmp4F2->hd=_tmp938;});_tmp4F2->tl=tms;_tmp4F2;});tms=_tmp939;});
return({struct _tuple12 _tmp4F5;_tmp4F5.f1=_tmp4F8;_tmp4F5.f2=_tmp4F7;_tmp4F5.f3=tms;_tmp4F5;});};};case 1U: _LL7: _tmp511=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C8)->f1;_tmp510=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C8)->f2;_tmp50F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C8)->f3;_LL8:
# 2113
 if(_tmp510 == 0)
return({struct _tuple12 _tmp4F9;_tmp4F9.f1=tq;_tmp4F9.f2=t;_tmp4F9.f3=0;_tmp4F9;});else{
# 2116
return Cyc_Absynpp_to_tms(r,tq,_tmp510);}case 17U: _LL9: _tmp515=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C8)->f1;_tmp514=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C8)->f2;_tmp513=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C8)->f3;_tmp512=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C8)->f4;_LLA:
# 2121
 if((_tmp512 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp512))
return({struct _tuple12 _tmp4FA;_tmp4FA.f1=tq;_tmp4FA.f2=t;_tmp4FA.f3=0;_tmp4FA;});else{
# 2124
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp512);}default: _LLB: _LLC:
# 2129
 return({struct _tuple12 _tmp4FB;_tmp4FB.f1=tq;_tmp4FB.f2=t;_tmp4FB.f3=0;_tmp4FB;});}_LL0:;}
# 2133
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2135
void*_tmp516=t;void*_tmp51B;void*_tmp51A;switch(*((int*)_tmp516)){case 1U: _LL1: _tmp51A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp516)->f2;if(_tmp51A != 0){_LL2:
 return Cyc_Absynpp_is_char_ptr(_tmp51A);}else{goto _LL5;}case 5U: _LL3: _tmp51B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp516)->f1).elt_typ;_LL4:
# 2138
 L: {
void*_tmp517=_tmp51B;void*_tmp519;void*_tmp518;switch(*((int*)_tmp517)){case 1U: _LL8: _tmp518=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp517)->f2;if(_tmp518 != 0){_LL9:
 _tmp51B=_tmp518;goto L;}else{goto _LLE;}case 17U: _LLA: _tmp519=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp517)->f4;if(_tmp519 != 0){_LLB:
 _tmp51B=_tmp519;goto L;}else{goto _LLE;}case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp517)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 2145
 return 0;}_LL0:;}
# 2149
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51C=_new_region("temp");struct _RegionHandle*temp=& _tmp51C;_push_region(temp);
{struct _tuple12 _tmp51D=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp51E=_tmp51D;struct Cyc_Absyn_Tqual _tmp526;void*_tmp525;struct Cyc_List_List*_tmp524;_LL1: _tmp526=_tmp51E.f1;_tmp525=_tmp51E.f2;_tmp524=_tmp51E.f3;_LL2:;
({struct Cyc_List_List*_tmp93A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp524);_tmp524=_tmp93A;});
if(_tmp524 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp520=({struct Cyc_PP_Doc*_tmp51F[2U];({struct Cyc_PP_Doc*_tmp93B=Cyc_Absynpp_ntyp2doc(_tmp525);_tmp51F[1U]=_tmp93B;});({struct Cyc_PP_Doc*_tmp93C=Cyc_Absynpp_tqual2doc(_tmp526);_tmp51F[0U]=_tmp93C;});Cyc_PP_cat(_tag_dyneither(_tmp51F,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp520;}else{
# 2156
struct Cyc_PP_Doc*_tmp523=({struct Cyc_PP_Doc*_tmp521[4U];({struct Cyc_PP_Doc*_tmp93F=({
# 2160
int _tmp93E=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp93D=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp93E,_tmp93D,_tmp524);});_tmp521[3U]=_tmp93F;});({struct Cyc_PP_Doc*_tmp940=
# 2159
Cyc_PP_text(({const char*_tmp522=" ";_tag_dyneither(_tmp522,sizeof(char),2U);}));_tmp521[2U]=_tmp940;});({struct Cyc_PP_Doc*_tmp941=
# 2158
Cyc_Absynpp_ntyp2doc(_tmp525);_tmp521[1U]=_tmp941;});({struct Cyc_PP_Doc*_tmp942=
# 2157
Cyc_Absynpp_tqual2doc(_tmp526);_tmp521[0U]=_tmp942;});Cyc_PP_cat(_tag_dyneither(_tmp521,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp523;}}
# 2151
;_pop_region(temp);}
# 2164
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp527=f->requires_clause;
if((unsigned int)_tmp527)
({struct Cyc_PP_Doc*_tmp945=({struct Cyc_PP_Doc*_tmp528[2U];({struct Cyc_PP_Doc*_tmp943=Cyc_Absynpp_exp2doc(_tmp527);_tmp528[1U]=_tmp943;});({struct Cyc_PP_Doc*_tmp944=Cyc_PP_text(({const char*_tmp529="@requires ";_tag_dyneither(_tmp529,sizeof(char),11U);}));_tmp528[0U]=_tmp944;});Cyc_PP_cat(_tag_dyneither(_tmp528,sizeof(struct Cyc_PP_Doc*),2U));});requires_doc=_tmp945;});else{
# 2170
({struct Cyc_PP_Doc*_tmp946=Cyc_PP_nil_doc();requires_doc=_tmp946;});}{
# 2172
enum Cyc_Cyclone_C_Compilers _tmp52A=Cyc_Cyclone_c_compiler;if(_tmp52A == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2175
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52B[5U];({struct Cyc_PP_Doc*_tmp947=
# 2178
Cyc_PP_text(({const char*_tmp52E=";";_tag_dyneither(_tmp52E,sizeof(char),2U);}));_tmp52B[4U]=_tmp947;});({struct Cyc_PP_Doc*_tmp948=Cyc_Absynpp_atts2doc(f->attributes);_tmp52B[3U]=_tmp948;});({struct Cyc_PP_Doc*_tmp949=
# 2177
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52B[2U]=_tmp949;});({struct Cyc_PP_Doc*_tmp94A=Cyc_PP_text(({const char*_tmp52D=":";_tag_dyneither(_tmp52D,sizeof(char),2U);}));_tmp52B[1U]=_tmp94A;});({struct Cyc_PP_Doc*_tmp94E=({
# 2176
struct Cyc_Absyn_Tqual _tmp94D=f->tq;void*_tmp94C=f->type;Cyc_Absynpp_tqtd2doc(_tmp94D,_tmp94C,({struct Cyc_Core_Opt*_tmp52C=_cycalloc(sizeof(*_tmp52C));({struct Cyc_PP_Doc*_tmp94B=Cyc_PP_textptr(f->name);_tmp52C->v=_tmp94B;});_tmp52C;}));});_tmp52B[0U]=_tmp94E;});Cyc_PP_cat(_tag_dyneither(_tmp52B,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2180
return({struct Cyc_PP_Doc*_tmp52F[4U];({struct Cyc_PP_Doc*_tmp94F=
Cyc_PP_text(({const char*_tmp531=";";_tag_dyneither(_tmp531,sizeof(char),2U);}));_tmp52F[3U]=_tmp94F;});_tmp52F[2U]=requires_doc;({struct Cyc_PP_Doc*_tmp950=Cyc_Absynpp_atts2doc(f->attributes);_tmp52F[1U]=_tmp950;});({struct Cyc_PP_Doc*_tmp954=({
# 2180
struct Cyc_Absyn_Tqual _tmp953=f->tq;void*_tmp952=f->type;Cyc_Absynpp_tqtd2doc(_tmp953,_tmp952,({struct Cyc_Core_Opt*_tmp530=_cycalloc(sizeof(*_tmp530));({struct Cyc_PP_Doc*_tmp951=Cyc_PP_textptr(f->name);_tmp530->v=_tmp951;});_tmp530;}));});_tmp52F[0U]=_tmp954;});Cyc_PP_cat(_tag_dyneither(_tmp52F,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2183
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp532[5U];({struct Cyc_PP_Doc*_tmp955=
# 2186
Cyc_PP_text(({const char*_tmp535=";";_tag_dyneither(_tmp535,sizeof(char),2U);}));_tmp532[4U]=_tmp955;});({struct Cyc_PP_Doc*_tmp956=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp532[3U]=_tmp956;});({struct Cyc_PP_Doc*_tmp957=Cyc_PP_text(({const char*_tmp534=":";_tag_dyneither(_tmp534,sizeof(char),2U);}));_tmp532[2U]=_tmp957;});({struct Cyc_PP_Doc*_tmp95B=({
# 2185
struct Cyc_Absyn_Tqual _tmp95A=f->tq;void*_tmp959=f->type;Cyc_Absynpp_tqtd2doc(_tmp95A,_tmp959,({struct Cyc_Core_Opt*_tmp533=_cycalloc(sizeof(*_tmp533));({struct Cyc_PP_Doc*_tmp958=Cyc_PP_textptr(f->name);_tmp533->v=_tmp958;});_tmp533;}));});_tmp532[1U]=_tmp95B;});({struct Cyc_PP_Doc*_tmp95C=
# 2184
Cyc_Absynpp_atts2doc(f->attributes);_tmp532[0U]=_tmp95C;});Cyc_PP_cat(_tag_dyneither(_tmp532,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2188
return({struct Cyc_PP_Doc*_tmp536[4U];({struct Cyc_PP_Doc*_tmp95D=
# 2190
Cyc_PP_text(({const char*_tmp538=";";_tag_dyneither(_tmp538,sizeof(char),2U);}));_tmp536[3U]=_tmp95D;});_tmp536[2U]=requires_doc;({struct Cyc_PP_Doc*_tmp961=({
# 2189
struct Cyc_Absyn_Tqual _tmp960=f->tq;void*_tmp95F=f->type;Cyc_Absynpp_tqtd2doc(_tmp960,_tmp95F,({struct Cyc_Core_Opt*_tmp537=_cycalloc(sizeof(*_tmp537));({struct Cyc_PP_Doc*_tmp95E=Cyc_PP_textptr(f->name);_tmp537->v=_tmp95E;});_tmp537;}));});_tmp536[1U]=_tmp961;});({struct Cyc_PP_Doc*_tmp962=
# 2188
Cyc_Absynpp_atts2doc(f->attributes);_tmp536[0U]=_tmp962;});Cyc_PP_cat(_tag_dyneither(_tmp536,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;};}
# 2195
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp963=({const char*_tmp539="";_tag_dyneither(_tmp539,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp963,fields);});}
# 2199
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp53A[3U];({struct Cyc_PP_Doc*_tmp964=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp53A[2U]=_tmp964;});({struct Cyc_PP_Doc*_tmp965=
# 2200
Cyc_Absynpp_typedef_name2doc(f->name);_tmp53A[1U]=_tmp965;});({struct Cyc_PP_Doc*_tmp966=Cyc_Absynpp_scope2doc(f->sc);_tmp53A[0U]=_tmp966;});Cyc_PP_cat(_tag_dyneither(_tmp53A,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2204
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp967=({const char*_tmp53B=",";_tag_dyneither(_tmp53B,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp967,fields);});}
# 2213
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp968=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp968,72,f);});
({void*_tmp53C=0U;({struct Cyc___cycFILE*_tmp96A=f;struct _dyneither_ptr _tmp969=({const char*_tmp53D="\n";_tag_dyneither(_tmp53D,sizeof(char),2U);});Cyc_fprintf(_tmp96A,_tmp969,_tag_dyneither(_tmp53C,sizeof(void*),0U));});});}}
# 2220
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _dyneither_ptr _tmp96B=({const char*_tmp53E="";_tag_dyneither(_tmp53E,sizeof(char),1U);});Cyc_PP_seql(_tmp96B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2223
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2227
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
# 2238
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

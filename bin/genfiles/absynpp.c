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
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
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
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 55 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 297 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
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
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4]="Cyc";
# 72
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4};
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
({struct Cyc_List_List*_tmp541=({struct Cyc_List_List*_tmp540=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp540,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;_tmp1;}));});Cyc_Absynpp_curr_namespace=_tmp541;});}
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
char _tmp2=c;switch(_tmp2){case 7: _LL1: _LL2:
 return({const char*_tmp3="\\a";_tag_dyneither(_tmp3,sizeof(char),3);});case 8: _LL3: _LL4:
 return({const char*_tmp4="\\b";_tag_dyneither(_tmp4,sizeof(char),3);});case 12: _LL5: _LL6:
 return({const char*_tmp5="\\f";_tag_dyneither(_tmp5,sizeof(char),3);});case 10: _LL7: _LL8:
 return({const char*_tmp6="\\n";_tag_dyneither(_tmp6,sizeof(char),3);});case 13: _LL9: _LLA:
 return({const char*_tmp7="\\r";_tag_dyneither(_tmp7,sizeof(char),3);});case 9: _LLB: _LLC:
 return({const char*_tmp8="\\t";_tag_dyneither(_tmp8,sizeof(char),3);});case 11: _LLD: _LLE:
 return({const char*_tmp9="\\v";_tag_dyneither(_tmp9,sizeof(char),3);});case 92: _LLF: _LL10:
 return({const char*_tmpA="\\\\";_tag_dyneither(_tmpA,sizeof(char),3);});case 34: _LL11: _LL12:
 return({const char*_tmpB="\"";_tag_dyneither(_tmpB,sizeof(char),2);});case 39: _LL13: _LL14:
 return({const char*_tmpC="\\'";_tag_dyneither(_tmpC,sizeof(char),3);});default: _LL15: _LL16:
# 272
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2);
({struct _dyneither_ptr _tmpE=_dyneither_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_dyneither_subscript(_tmpE,sizeof(char),0));char _tmp10=c;if(_get_dyneither_size(_tmpE,sizeof(char))== 1  && (_tmpF == '\000'  && _tmp10 != '\000'))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _dyneither_ptr)_tmpD;}else{
# 277
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5);
int j=0;
({struct _dyneither_ptr _tmp12=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_dyneither_subscript(_tmp12,sizeof(char),0));char _tmp14='\\';if(_get_dyneither_size(_tmp12,sizeof(char))== 1  && (_tmp13 == '\000'  && _tmp14 != '\000'))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0));char _tmp17=(char)('0' + ((unsigned char)c >> 6 & 3));if(_get_dyneither_size(_tmp15,sizeof(char))== 1  && (_tmp16 == '\000'  && _tmp17 != '\000'))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _dyneither_ptr _tmp18=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_dyneither_subscript(_tmp18,sizeof(char),0));char _tmp1A=(char)('0' + (c >> 3 & 7));if(_get_dyneither_size(_tmp18,sizeof(char))== 1  && (_tmp19 == '\000'  && _tmp1A != '\000'))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0));char _tmp1D=(char)('0' + (c & 7));if(_get_dyneither_size(_tmp1B,sizeof(char))== 1  && (_tmp1C == '\000'  && _tmp1D != '\000'))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
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
char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1F=_tmp1E;char _tmp20;switch(_tmp1F){case 7: _LL18: _LL19:
 goto _LL1B;case 8: _LL1A: _LL1B:
 goto _LL1D;case 12: _LL1C: _LL1D:
 goto _LL1F;case 10: _LL1E: _LL1F:
 goto _LL21;case 13: _LL20: _LL21:
 goto _LL23;case 9: _LL22: _LL23:
 goto _LL25;case 11: _LL24: _LL25:
 goto _LL27;case 92: _LL26: _LL27:
 goto _LL29;case 34: _LL28: _LL29:
 len +=2;goto _LL17;default: _LL2A: _tmp20=_tmp1F;_LL2B:
# 318
 if(_tmp20 >= ' '  && _tmp20 <= '~')++ len;else{
len +=4;}
goto _LL17;}_LL17:;}}{
# 323
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp68;switch(_tmp22){case 7: _LL2D: _LL2E:
({struct _dyneither_ptr _tmp23=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp24=*((char*)_check_dyneither_subscript(_tmp23,sizeof(char),0));char _tmp25='\\';if(_get_dyneither_size(_tmp23,sizeof(char))== 1  && (_tmp24 == '\000'  && _tmp25 != '\000'))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});({struct _dyneither_ptr _tmp26=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp27=*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0));char _tmp28='a';if(_get_dyneither_size(_tmp26,sizeof(char))== 1  && (_tmp27 == '\000'  && _tmp28 != '\000'))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});goto _LL2C;case 8: _LL2F: _LL30:
({struct _dyneither_ptr _tmp29=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2A=*((char*)_check_dyneither_subscript(_tmp29,sizeof(char),0));char _tmp2B='\\';if(_get_dyneither_size(_tmp29,sizeof(char))== 1  && (_tmp2A == '\000'  && _tmp2B != '\000'))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});({struct _dyneither_ptr _tmp2C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0));char _tmp2E='b';if(_get_dyneither_size(_tmp2C,sizeof(char))== 1  && (_tmp2D == '\000'  && _tmp2E != '\000'))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});goto _LL2C;case 12: _LL31: _LL32:
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0));char _tmp31='\\';if(_get_dyneither_size(_tmp2F,sizeof(char))== 1  && (_tmp30 == '\000'  && _tmp31 != '\000'))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0));char _tmp34='f';if(_get_dyneither_size(_tmp32,sizeof(char))== 1  && (_tmp33 == '\000'  && _tmp34 != '\000'))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});goto _LL2C;case 10: _LL33: _LL34:
({struct _dyneither_ptr _tmp35=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0));char _tmp37='\\';if(_get_dyneither_size(_tmp35,sizeof(char))== 1  && (_tmp36 == '\000'  && _tmp37 != '\000'))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});({struct _dyneither_ptr _tmp38=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp39=*((char*)_check_dyneither_subscript(_tmp38,sizeof(char),0));char _tmp3A='n';if(_get_dyneither_size(_tmp38,sizeof(char))== 1  && (_tmp39 == '\000'  && _tmp3A != '\000'))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});goto _LL2C;case 13: _LL35: _LL36:
({struct _dyneither_ptr _tmp3B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3C=*((char*)_check_dyneither_subscript(_tmp3B,sizeof(char),0));char _tmp3D='\\';if(_get_dyneither_size(_tmp3B,sizeof(char))== 1  && (_tmp3C == '\000'  && _tmp3D != '\000'))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3F=*((char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0));char _tmp40='r';if(_get_dyneither_size(_tmp3E,sizeof(char))== 1  && (_tmp3F == '\000'  && _tmp40 != '\000'))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});goto _LL2C;case 9: _LL37: _LL38:
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0));char _tmp43='\\';if(_get_dyneither_size(_tmp41,sizeof(char))== 1  && (_tmp42 == '\000'  && _tmp43 != '\000'))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});({struct _dyneither_ptr _tmp44=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp45=*((char*)_check_dyneither_subscript(_tmp44,sizeof(char),0));char _tmp46='t';if(_get_dyneither_size(_tmp44,sizeof(char))== 1  && (_tmp45 == '\000'  && _tmp46 != '\000'))_throw_arraybounds();*((char*)_tmp44.curr)=_tmp46;});goto _LL2C;case 11: _LL39: _LL3A:
({struct _dyneither_ptr _tmp47=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp48=*((char*)_check_dyneither_subscript(_tmp47,sizeof(char),0));char _tmp49='\\';if(_get_dyneither_size(_tmp47,sizeof(char))== 1  && (_tmp48 == '\000'  && _tmp49 != '\000'))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});({struct _dyneither_ptr _tmp4A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4B=*((char*)_check_dyneither_subscript(_tmp4A,sizeof(char),0));char _tmp4C='v';if(_get_dyneither_size(_tmp4A,sizeof(char))== 1  && (_tmp4B == '\000'  && _tmp4C != '\000'))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});goto _LL2C;case 92: _LL3B: _LL3C:
({struct _dyneither_ptr _tmp4D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4E=*((char*)_check_dyneither_subscript(_tmp4D,sizeof(char),0));char _tmp4F='\\';if(_get_dyneither_size(_tmp4D,sizeof(char))== 1  && (_tmp4E == '\000'  && _tmp4F != '\000'))_throw_arraybounds();*((char*)_tmp4D.curr)=_tmp4F;});({struct _dyneither_ptr _tmp50=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp51=*((char*)_check_dyneither_subscript(_tmp50,sizeof(char),0));char _tmp52='\\';if(_get_dyneither_size(_tmp50,sizeof(char))== 1  && (_tmp51 == '\000'  && _tmp52 != '\000'))_throw_arraybounds();*((char*)_tmp50.curr)=_tmp52;});goto _LL2C;case 34: _LL3D: _LL3E:
({struct _dyneither_ptr _tmp53=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp54=*((char*)_check_dyneither_subscript(_tmp53,sizeof(char),0));char _tmp55='\\';if(_get_dyneither_size(_tmp53,sizeof(char))== 1  && (_tmp54 == '\000'  && _tmp55 != '\000'))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});({struct _dyneither_ptr _tmp56=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp57=*((char*)_check_dyneither_subscript(_tmp56,sizeof(char),0));char _tmp58='"';if(_get_dyneither_size(_tmp56,sizeof(char))== 1  && (_tmp57 == '\000'  && _tmp58 != '\000'))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});goto _LL2C;default: _LL3F: _tmp68=_tmp22;_LL40:
# 337
 if(_tmp68 >= ' '  && _tmp68 <= '~')({struct _dyneither_ptr _tmp59=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5A=*((char*)_check_dyneither_subscript(_tmp59,sizeof(char),0));char _tmp5B=_tmp68;if(_get_dyneither_size(_tmp59,sizeof(char))== 1  && (_tmp5A == '\000'  && _tmp5B != '\000'))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});else{
# 339
({struct _dyneither_ptr _tmp5C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5D=*((char*)_check_dyneither_subscript(_tmp5C,sizeof(char),0));char _tmp5E='\\';if(_get_dyneither_size(_tmp5C,sizeof(char))== 1  && (_tmp5D == '\000'  && _tmp5E != '\000'))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});
({struct _dyneither_ptr _tmp5F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp60=*((char*)_check_dyneither_subscript(_tmp5F,sizeof(char),0));char _tmp61=(char)('0' + (_tmp68 >> 6 & 7));if(_get_dyneither_size(_tmp5F,sizeof(char))== 1  && (_tmp60 == '\000'  && _tmp61 != '\000'))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp63=*((char*)_check_dyneither_subscript(_tmp62,sizeof(char),0));char _tmp64=(char)('0' + (_tmp68 >> 3 & 7));if(_get_dyneither_size(_tmp62,sizeof(char))== 1  && (_tmp63 == '\000'  && _tmp64 != '\000'))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
({struct _dyneither_ptr _tmp65=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp66=*((char*)_check_dyneither_subscript(_tmp65,sizeof(char),0));char _tmp67=(char)('0' + (_tmp68 & 7));if(_get_dyneither_size(_tmp65,sizeof(char))== 1  && (_tmp66 == '\000'  && _tmp67 != '\000'))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});}
# 344
goto _LL2C;}_LL2C:;}}
# 346
return(struct _dyneither_ptr)t;};};};}static char _tmp69[9]="restrict";
# 349
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9};static char _tmp6A[9]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9};static char _tmp6B[6]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 356
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 359
if(tq.q_restrict)({struct Cyc_List_List*_tmp542=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp;_tmp6C->tl=l;_tmp6C;});l=_tmp542;});
if(tq.q_volatile)({struct Cyc_List_List*_tmp543=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp;_tmp6D->tl=l;_tmp6D;});l=_tmp543;});
if(tq.print_const)({struct Cyc_List_List*_tmp544=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp;_tmp6E->tl=l;_tmp6E;});l=_tmp544;});
return({struct _dyneither_ptr _tmp547=({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1);});struct _dyneither_ptr _tmp546=({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2);});struct _dyneither_ptr _tmp545=({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2);});Cyc_PP_egroup(_tmp547,_tmp546,_tmp545,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 365
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp87;enum Cyc_Absyn_AliasQual _tmp86;_LL42: _tmp87=_tmp72->kind;_tmp86=_tmp72->aliasqual;_LL43:;{
enum Cyc_Absyn_KindQual _tmp73=_tmp87;switch(_tmp73){case Cyc_Absyn_AnyKind: _LL45: _LL46: {
# 369
enum Cyc_Absyn_AliasQual _tmp74=_tmp86;switch(_tmp74){case Cyc_Absyn_Aliasable: _LL52: _LL53:
 return({const char*_tmp75="A";_tag_dyneither(_tmp75,sizeof(char),2);});case Cyc_Absyn_Unique: _LL54: _LL55:
 return({const char*_tmp76="UA";_tag_dyneither(_tmp76,sizeof(char),3);});default: _LL56: _LL57:
 return({const char*_tmp77="TA";_tag_dyneither(_tmp77,sizeof(char),3);});}_LL51:;}case Cyc_Absyn_MemKind: _LL47: _LL48: {
# 375
enum Cyc_Absyn_AliasQual _tmp78=_tmp86;switch(_tmp78){case Cyc_Absyn_Aliasable: _LL59: _LL5A:
 return({const char*_tmp79="M";_tag_dyneither(_tmp79,sizeof(char),2);});case Cyc_Absyn_Unique: _LL5B: _LL5C:
 return({const char*_tmp7A="UM";_tag_dyneither(_tmp7A,sizeof(char),3);});default: _LL5D: _LL5E:
 return({const char*_tmp7B="TM";_tag_dyneither(_tmp7B,sizeof(char),3);});}_LL58:;}case Cyc_Absyn_BoxKind: _LL49: _LL4A: {
# 381
enum Cyc_Absyn_AliasQual _tmp7C=_tmp86;switch(_tmp7C){case Cyc_Absyn_Aliasable: _LL60: _LL61:
 return({const char*_tmp7D="B";_tag_dyneither(_tmp7D,sizeof(char),2);});case Cyc_Absyn_Unique: _LL62: _LL63:
 return({const char*_tmp7E="UB";_tag_dyneither(_tmp7E,sizeof(char),3);});default: _LL64: _LL65:
 return({const char*_tmp7F="TB";_tag_dyneither(_tmp7F,sizeof(char),3);});}_LL5F:;}case Cyc_Absyn_RgnKind: _LL4B: _LL4C: {
# 387
enum Cyc_Absyn_AliasQual _tmp80=_tmp86;switch(_tmp80){case Cyc_Absyn_Aliasable: _LL67: _LL68:
 return({const char*_tmp81="R";_tag_dyneither(_tmp81,sizeof(char),2);});case Cyc_Absyn_Unique: _LL69: _LL6A:
 return({const char*_tmp82="UR";_tag_dyneither(_tmp82,sizeof(char),3);});default: _LL6B: _LL6C:
 return({const char*_tmp83="TR";_tag_dyneither(_tmp83,sizeof(char),3);});}_LL66:;}case Cyc_Absyn_EffKind: _LL4D: _LL4E:
# 392
 return({const char*_tmp84="E";_tag_dyneither(_tmp84,sizeof(char),2);});default: _LL4F: _LL50:
 return({const char*_tmp85="I";_tag_dyneither(_tmp85,sizeof(char),2);});}_LL44:;};}
# 396
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 398
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp88=Cyc_Absyn_compress_kb(c);void*_tmp89=_tmp88;struct Cyc_Absyn_Kind*_tmp90;struct Cyc_Absyn_Kind*_tmp8F;switch(*((int*)_tmp89)){case 0: _LL6E: _tmp8F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89)->f1;_LL6F:
 return Cyc_Absynpp_kind2string(_tmp8F);case 1: _LL70: _LL71:
# 402
 if(Cyc_PP_tex_output)
return({const char*_tmp8A="{?}";_tag_dyneither(_tmp8A,sizeof(char),4);});else{
return({const char*_tmp8B="?";_tag_dyneither(_tmp8B,sizeof(char),2);});}default: _LL72: _tmp90=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89)->f2;_LL73:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0;({struct _dyneither_ptr _tmp548=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp90));_tmp8E.f1=_tmp548;});({void*_tmp8C[1]={& _tmp8E};({struct _dyneither_ptr _tmp549=({const char*_tmp8D="<=%s";_tag_dyneither(_tmp8D,sizeof(char),5);});Cyc_aprintf(_tmp549,_tag_dyneither(_tmp8C,sizeof(void*),1));});});});}_LL6D:;}
# 409
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp91=Cyc_Absyn_compress_kb(c);void*_tmp92=_tmp91;struct Cyc_Absyn_Kind*_tmp96;struct Cyc_Absyn_Kind*_tmp95;switch(*((int*)_tmp92)){case 0: _LL75: _tmp95=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp92)->f1;_LL76:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp95));case 1: _LL77: _LL78:
# 413
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp93="{?}";_tag_dyneither(_tmp93,sizeof(char),4);}),1);else{
return Cyc_PP_text(({const char*_tmp94="?";_tag_dyneither(_tmp94,sizeof(char),2);}));}default: _LL79: _tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92)->f2;_LL7A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp96));}_LL74:;}
# 420
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp54C=({const char*_tmp97="<";_tag_dyneither(_tmp97,sizeof(char),2);});struct _dyneither_ptr _tmp54B=({const char*_tmp98=">";_tag_dyneither(_tmp98,sizeof(char),2);});struct _dyneither_ptr _tmp54A=({const char*_tmp99=",";_tag_dyneither(_tmp99,sizeof(char),2);});Cyc_PP_egroup(_tmp54C,_tmp54B,_tmp54A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 428
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 430
struct _dyneither_ptr kstring=({const char*_tmpA3="K";_tag_dyneither(_tmpA3,sizeof(char),2);});
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmp9D;switch(*((int*)_tmp9C)){case 2: _LL7C: _tmp9D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C)->f2;_LL7D:
 _tmp9E=_tmp9D;goto _LL7F;case 0: _LL7E: _tmp9E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C)->f1;_LL7F:
({struct _dyneither_ptr _tmp54D=Cyc_Absynpp_kind2string(_tmp9E);kstring=_tmp54D;});goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}
# 436
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;({struct _dyneither_ptr _tmp54E=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1));_tmpA2.f1=_tmp54E;});({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);({void*_tmp9F[2]={& _tmpA1,& _tmpA2};({struct _dyneither_ptr _tmp54F=({const char*_tmpA0="`G%s%s";_tag_dyneither(_tmpA0,sizeof(char),7);});Cyc_aprintf(_tmp54F,_tag_dyneither(_tmp9F,sizeof(void*),2));});});});}));}
# 438
return Cyc_PP_text(_tmp9A);}
# 441
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA4=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA5=_tmpA4;struct Cyc_Absyn_Kind*_tmpA9;struct Cyc_Absyn_Kind*_tmpA8;switch(*((int*)_tmpA5)){case 1: _LL83: _LL84:
 goto _LL86;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL85: _LL86:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL89;}}else{_LL89: _tmpA8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1;_LL8A:
# 446
 return({struct Cyc_PP_Doc*_tmpA6[3];({struct Cyc_PP_Doc*_tmp550=Cyc_Absynpp_kind2doc(_tmpA8);_tmpA6[2]=_tmp550;});({struct Cyc_PP_Doc*_tmp551=Cyc_PP_text(({const char*_tmpA7="::";_tag_dyneither(_tmpA7,sizeof(char),3);}));_tmpA6[1]=_tmp551;});({struct Cyc_PP_Doc*_tmp552=Cyc_Absynpp_tvar2doc(tv);_tmpA6[0]=_tmp552;});Cyc_PP_cat(_tag_dyneither(_tmpA6,sizeof(struct Cyc_PP_Doc*),3));});}default: _LL87: _tmpA9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5)->f2;_LL88:
# 445
 _tmpA8=_tmpA9;goto _LL8A;}_LL82:;}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _dyneither_ptr _tmp555=({const char*_tmpAA="<";_tag_dyneither(_tmpAA,sizeof(char),2);});struct _dyneither_ptr _tmp554=({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2);});struct _dyneither_ptr _tmp553=({const char*_tmpAC=",";_tag_dyneither(_tmpAC,sizeof(char),2);});Cyc_PP_egroup(_tmp555,_tmp554,_tmp553,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 454
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _dyneither_ptr _tmp558=({const char*_tmpAD="<";_tag_dyneither(_tmpAD,sizeof(char),2);});struct _dyneither_ptr _tmp557=({const char*_tmpAE=">";_tag_dyneither(_tmpAE,sizeof(char),2);});struct _dyneither_ptr _tmp556=({const char*_tmpAF=",";_tag_dyneither(_tmpAF,sizeof(char),2);});Cyc_PP_egroup(_tmp558,_tmp557,_tmp556,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp55B=({const char*_tmpB0="(";_tag_dyneither(_tmpB0,sizeof(char),2);});struct _dyneither_ptr _tmp55A=({const char*_tmpB1=")";_tag_dyneither(_tmpB1,sizeof(char),2);});struct _dyneither_ptr _tmp559=({const char*_tmpB2=",";_tag_dyneither(_tmpB2,sizeof(char),2);});Cyc_PP_group(_tmp55B,_tmp55A,_tmp559,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 468
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB3=att;switch(*((int*)_tmpB3)){case 1: _LL8C: _LL8D:
 goto _LL8F;case 2: _LL8E: _LL8F:
 goto _LL91;case 3: _LL90: _LL91:
 return Cyc_PP_nil_doc();default: _LL92: _LL93:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL8B:;}
# 477
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB4=(void*)atts->hd;void*_tmpB5=_tmpB4;switch(*((int*)_tmpB5)){case 1: _LL95: _LL96:
 return Cyc_PP_text(({const char*_tmpB6=" _stdcall ";_tag_dyneither(_tmpB6,sizeof(char),11);}));case 2: _LL97: _LL98:
 return Cyc_PP_text(({const char*_tmpB7=" _cdecl ";_tag_dyneither(_tmpB7,sizeof(char),9);}));case 3: _LL99: _LL9A:
 return Cyc_PP_text(({const char*_tmpB8=" _fastcall ";_tag_dyneither(_tmpB8,sizeof(char),12);}));default: _LL9B: _LL9C:
 goto _LL94;}_LL94:;}
# 485
return Cyc_PP_nil_doc();}
# 488
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB9=(void*)atts2->hd;void*_tmpBA=_tmpB9;switch(*((int*)_tmpBA)){case 1: _LL9E: _LL9F:
 goto _LLA1;case 2: _LLA0: _LLA1:
 goto _LLA3;case 3: _LLA2: _LLA3:
 goto _LL9D;default: _LLA4: _LLA5:
 hasatt=1;goto _LL9D;}_LL9D:;}}
# 497
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBB[3];({struct Cyc_PP_Doc*_tmp55C=
# 501
Cyc_PP_text(({const char*_tmpC0=")";_tag_dyneither(_tmpC0,sizeof(char),2);}));_tmpBB[2]=_tmp55C;});({struct Cyc_PP_Doc*_tmp560=({
# 500
struct _dyneither_ptr _tmp55F=({const char*_tmpBD="";_tag_dyneither(_tmpBD,sizeof(char),1);});struct _dyneither_ptr _tmp55E=({const char*_tmpBE="";_tag_dyneither(_tmpBE,sizeof(char),1);});struct _dyneither_ptr _tmp55D=({const char*_tmpBF=" ";_tag_dyneither(_tmpBF,sizeof(char),2);});Cyc_PP_group(_tmp55F,_tmp55E,_tmp55D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBB[1]=_tmp560;});({struct Cyc_PP_Doc*_tmp561=
# 499
Cyc_PP_text(({const char*_tmpBC=" __declspec(";_tag_dyneither(_tmpBC,sizeof(char),13);}));_tmpBB[0]=_tmp561;});Cyc_PP_cat(_tag_dyneither(_tmpBB,sizeof(struct Cyc_PP_Doc*),3));});}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 508
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC1=Cyc_Cyclone_c_compiler;if(_tmpC1 == Cyc_Cyclone_Vc_c){_LLA7: _LLA8:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LLA9: _LLAA:
 return({struct Cyc_PP_Doc*_tmpC2[2];({struct Cyc_PP_Doc*_tmp565=({
struct _dyneither_ptr _tmp564=({const char*_tmpC4="((";_tag_dyneither(_tmpC4,sizeof(char),3);});struct _dyneither_ptr _tmp563=({const char*_tmpC5="))";_tag_dyneither(_tmpC5,sizeof(char),3);});struct _dyneither_ptr _tmp562=({const char*_tmpC6=",";_tag_dyneither(_tmpC6,sizeof(char),2);});Cyc_PP_group(_tmp564,_tmp563,_tmp562,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC2[1]=_tmp565;});({struct Cyc_PP_Doc*_tmp566=
# 512
Cyc_PP_text(({const char*_tmpC3=" __attribute__";_tag_dyneither(_tmpC3,sizeof(char),15);}));_tmpC2[0]=_tmp566;});Cyc_PP_cat(_tag_dyneither(_tmpC2,sizeof(struct Cyc_PP_Doc*),2));});}_LLA6:;};}
# 517
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC7=(void*)tms->hd;void*_tmpC8=_tmpC7;switch(*((int*)_tmpC8)){case 2: _LLAC: _LLAD:
 return 1;case 5: _LLAE: _LLAF: {
# 522
enum Cyc_Cyclone_C_Compilers _tmpC9=Cyc_Cyclone_c_compiler;if(_tmpC9 == Cyc_Cyclone_Gcc_c){_LLB3: _LLB4:
 return 0;}else{_LLB5: _LLB6:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LLB2:;}default: _LLB0: _LLB1:
# 526
 return 0;}_LLAB:;};}
# 530
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 533
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp567=Cyc_PP_text_width(({const char*_tmpCA="{?}";_tag_dyneither(_tmpCA,sizeof(char),4);}),1);Cyc_Absynpp_cache_question=_tmp567;});else{
({struct Cyc_PP_Doc*_tmp568=Cyc_PP_text(({const char*_tmpCB="?";_tag_dyneither(_tmpCB,sizeof(char),2);}));Cyc_Absynpp_cache_question=_tmp568;});}}
# 540
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 542
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp569=Cyc_PP_text_width(({const char*_tmpCC="{\\lb}";_tag_dyneither(_tmpCC,sizeof(char),6);}),1);Cyc_Absynpp_cache_lb=_tmp569;});else{
({struct Cyc_PP_Doc*_tmp56A=Cyc_PP_text(({const char*_tmpCD="{";_tag_dyneither(_tmpCD,sizeof(char),2);}));Cyc_Absynpp_cache_lb=_tmp56A;});}}
# 549
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp56B=Cyc_PP_text_width(({const char*_tmpCE="{\\rb}";_tag_dyneither(_tmpCE,sizeof(char),6);}),1);Cyc_Absynpp_cache_rb=_tmp56B;});else{
({struct Cyc_PP_Doc*_tmp56C=Cyc_PP_text(({const char*_tmpCF="}";_tag_dyneither(_tmpCF,sizeof(char),2);}));Cyc_Absynpp_cache_rb=_tmp56C;});}}
# 558
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
({struct Cyc_PP_Doc*_tmp56D=Cyc_PP_text_width(({const char*_tmpD0="\\$";_tag_dyneither(_tmpD0,sizeof(char),3);}),1);Cyc_Absynpp_cache_dollar=_tmp56D;});else{
({struct Cyc_PP_Doc*_tmp56E=Cyc_PP_text(({const char*_tmpD1="$";_tag_dyneither(_tmpD1,sizeof(char),2);}));Cyc_Absynpp_cache_dollar=_tmp56E;});}}
# 567
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 569
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD2[3];({struct Cyc_PP_Doc*_tmp56F=Cyc_Absynpp_rb();_tmpD2[2]=_tmp56F;});({struct Cyc_PP_Doc*_tmp570=Cyc_PP_seq(sep,ss);_tmpD2[1]=_tmp570;});({struct Cyc_PP_Doc*_tmp571=Cyc_Absynpp_lb();_tmpD2[0]=_tmp571;});Cyc_PP_cat(_tag_dyneither(_tmpD2,sizeof(struct Cyc_PP_Doc*),3));});}
# 574
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD3=(void*)tms->hd;void*_tmpD4=_tmpD3;struct Cyc_List_List*_tmpEB;switch(*((int*)_tmpD4)){case 0: _LLB8: _LLB9:
({void*_tmpD5=0;({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _dyneither_ptr _tmp572=({const char*_tmpD6="Carray_mod ";_tag_dyneither(_tmpD6,sizeof(char),12);});Cyc_fprintf(_tmp573,_tmp572,_tag_dyneither(_tmpD5,sizeof(void*),0));});});goto _LLB7;case 1: _LLBA: _LLBB:
({void*_tmpD7=0;({struct Cyc___cycFILE*_tmp575=Cyc_stderr;struct _dyneither_ptr _tmp574=({const char*_tmpD8="ConstArray_mod ";_tag_dyneither(_tmpD8,sizeof(char),16);});Cyc_fprintf(_tmp575,_tmp574,_tag_dyneither(_tmpD7,sizeof(void*),0));});});goto _LLB7;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->tag == 1){_LLBC: _tmpEB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->f1;_LLBD:
# 580
({void*_tmpD9=0;({struct Cyc___cycFILE*_tmp577=Cyc_stderr;struct _dyneither_ptr _tmp576=({const char*_tmpDA="Function_mod(";_tag_dyneither(_tmpDA,sizeof(char),14);});Cyc_fprintf(_tmp577,_tmp576,_tag_dyneither(_tmpD9,sizeof(void*),0));});});
for(0;_tmpEB != 0;_tmpEB=_tmpEB->tl){
struct _dyneither_ptr*_tmpDB=(*((struct _tuple8*)_tmpEB->hd)).f1;
if(_tmpDB == 0)({void*_tmpDC=0;({struct Cyc___cycFILE*_tmp579=Cyc_stderr;struct _dyneither_ptr _tmp578=({const char*_tmpDD="?";_tag_dyneither(_tmpDD,sizeof(char),2);});Cyc_fprintf(_tmp579,_tmp578,_tag_dyneither(_tmpDC,sizeof(void*),0));});});else{
({void*_tmpDE=0;({struct Cyc___cycFILE*_tmp57B=Cyc_stderr;struct _dyneither_ptr _tmp57A=*_tmpDB;Cyc_fprintf(_tmp57B,_tmp57A,_tag_dyneither(_tmpDE,sizeof(void*),0));});});}
if(_tmpEB->tl != 0)({void*_tmpDF=0;({struct Cyc___cycFILE*_tmp57D=Cyc_stderr;struct _dyneither_ptr _tmp57C=({const char*_tmpE0=",";_tag_dyneither(_tmpE0,sizeof(char),2);});Cyc_fprintf(_tmp57D,_tmp57C,_tag_dyneither(_tmpDF,sizeof(void*),0));});});}
# 587
({void*_tmpE1=0;({struct Cyc___cycFILE*_tmp57F=Cyc_stderr;struct _dyneither_ptr _tmp57E=({const char*_tmpE2=") ";_tag_dyneither(_tmpE2,sizeof(char),3);});Cyc_fprintf(_tmp57F,_tmp57E,_tag_dyneither(_tmpE1,sizeof(void*),0));});});
goto _LLB7;}else{_LLBE: _LLBF:
# 590
({void*_tmpE3=0;({struct Cyc___cycFILE*_tmp581=Cyc_stderr;struct _dyneither_ptr _tmp580=({const char*_tmpE4="Function_mod()";_tag_dyneither(_tmpE4,sizeof(char),15);});Cyc_fprintf(_tmp581,_tmp580,_tag_dyneither(_tmpE3,sizeof(void*),0));});});goto _LLB7;}case 5: _LLC0: _LLC1:
({void*_tmpE5=0;({struct Cyc___cycFILE*_tmp583=Cyc_stderr;struct _dyneither_ptr _tmp582=({const char*_tmpE6="Attributes_mod ";_tag_dyneither(_tmpE6,sizeof(char),16);});Cyc_fprintf(_tmp583,_tmp582,_tag_dyneither(_tmpE5,sizeof(void*),0));});});goto _LLB7;case 4: _LLC2: _LLC3:
({void*_tmpE7=0;({struct Cyc___cycFILE*_tmp585=Cyc_stderr;struct _dyneither_ptr _tmp584=({const char*_tmpE8="TypeParams_mod ";_tag_dyneither(_tmpE8,sizeof(char),16);});Cyc_fprintf(_tmp585,_tmp584,_tag_dyneither(_tmpE7,sizeof(void*),0));});});goto _LLB7;default: _LLC4: _LLC5:
({void*_tmpE9=0;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _dyneither_ptr _tmp586=({const char*_tmpEA="Pointer_mod ";_tag_dyneither(_tmpEA,sizeof(char),13);});Cyc_fprintf(_tmp587,_tmp586,_tag_dyneither(_tmpE9,sizeof(void*),0));});});goto _LLB7;}_LLB7:;}
# 595
({void*_tmpEC=0;({struct Cyc___cycFILE*_tmp589=Cyc_stderr;struct _dyneither_ptr _tmp588=({const char*_tmpED="\n";_tag_dyneither(_tmpED,sizeof(char),2);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmpEC,sizeof(void*),0));});});}
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 600
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp129[3];({struct Cyc_PP_Doc*_tmp58A=Cyc_PP_text(({const char*_tmp12B=")";_tag_dyneither(_tmp12B,sizeof(char),2);}));_tmp129[2]=_tmp58A;});_tmp129[1]=rest;({struct Cyc_PP_Doc*_tmp58B=Cyc_PP_text(({const char*_tmp12A="(";_tag_dyneither(_tmp12A,sizeof(char),2);}));_tmp129[0]=_tmp58B;});Cyc_PP_cat(_tag_dyneither(_tmp129,sizeof(struct Cyc_PP_Doc*),3));});
void*_tmpEE=(void*)tms->hd;void*_tmpEF=_tmpEE;void*_tmp128;union Cyc_Absyn_Constraint*_tmp127;union Cyc_Absyn_Constraint*_tmp126;union Cyc_Absyn_Constraint*_tmp125;struct Cyc_Absyn_Tqual _tmp124;struct Cyc_List_List*_tmp123;unsigned int _tmp122;int _tmp121;struct Cyc_List_List*_tmp120;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11C;switch(*((int*)_tmpEF)){case 0: _LLC7: _tmp11C=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLC8:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF0[2];({struct Cyc_PP_Doc*_tmp58C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11C)?
Cyc_PP_text(({const char*_tmpF1="[]@zeroterm";_tag_dyneither(_tmpF1,sizeof(char),12);})): Cyc_PP_text(({const char*_tmpF2="[]";_tag_dyneither(_tmpF2,sizeof(char),3);}));_tmpF0[1]=_tmp58C;});_tmpF0[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF0,sizeof(struct Cyc_PP_Doc*),2));});case 1: _LLC9: _tmp11E=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp11D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCA:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF3[4];({struct Cyc_PP_Doc*_tmp58D=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11D)?Cyc_PP_text(({const char*_tmpF5="]@zeroterm";_tag_dyneither(_tmpF5,sizeof(char),11);})): Cyc_PP_text(({const char*_tmpF6="]";_tag_dyneither(_tmpF6,sizeof(char),2);}));_tmpF3[3]=_tmp58D;});({struct Cyc_PP_Doc*_tmp58E=
# 611
Cyc_Absynpp_exp2doc(_tmp11E);_tmpF3[2]=_tmp58E;});({struct Cyc_PP_Doc*_tmp58F=Cyc_PP_text(({const char*_tmpF4="[";_tag_dyneither(_tmpF4,sizeof(char),2);}));_tmpF3[1]=_tmp58F;});_tmpF3[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF3,sizeof(struct Cyc_PP_Doc*),4));});case 3: _LLCB: _tmp11F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLCC:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF7=_tmp11F;struct Cyc_List_List*_tmp105;unsigned int _tmp104;struct Cyc_List_List*_tmp103;int _tmp102;struct Cyc_Absyn_VarargInfo*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->tag == 1){_LLD4: _tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp102=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f3;_tmp100=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f4;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f5;_tmpFE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f6;_tmpFD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f7;_LLD5:
# 617
 return({struct Cyc_PP_Doc*_tmpF8[2];({struct Cyc_PP_Doc*_tmp590=Cyc_Absynpp_funargs2doc(_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF,_tmpFE,_tmpFD);_tmpF8[1]=_tmp590;});_tmpF8[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF8,sizeof(struct Cyc_PP_Doc*),2));});}else{_LLD6: _tmp105=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp104=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_LLD7:
# 619
 return({struct Cyc_PP_Doc*_tmpF9[2];({struct Cyc_PP_Doc*_tmp594=({struct _dyneither_ptr _tmp593=({const char*_tmpFA="(";_tag_dyneither(_tmpFA,sizeof(char),2);});struct _dyneither_ptr _tmp592=({const char*_tmpFB=")";_tag_dyneither(_tmpFB,sizeof(char),2);});struct _dyneither_ptr _tmp591=({const char*_tmpFC=",";_tag_dyneither(_tmpFC,sizeof(char),2);});Cyc_PP_group(_tmp593,_tmp592,_tmp591,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp105));});_tmpF9[1]=_tmp594;});_tmpF9[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF9,sizeof(struct Cyc_PP_Doc*),2));});}_LLD3:;};case 5: _LLCD: _tmp120=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCE: {
# 622
enum Cyc_Cyclone_C_Compilers _tmp106=Cyc_Cyclone_c_compiler;if(_tmp106 == Cyc_Cyclone_Gcc_c){_LLD9: _LLDA:
# 624
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp107[2];({struct Cyc_PP_Doc*_tmp595=Cyc_Absynpp_atts2doc(_tmp120);_tmp107[1]=_tmp595;});_tmp107[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp107,sizeof(struct Cyc_PP_Doc*),2));});}else{_LLDB: _LLDC:
# 627
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp108[2];_tmp108[1]=rest;({struct Cyc_PP_Doc*_tmp596=Cyc_Absynpp_callconv2doc(_tmp120);_tmp108[0]=_tmp596;});Cyc_PP_cat(_tag_dyneither(_tmp108,sizeof(struct Cyc_PP_Doc*),2));});
return rest;}_LLD8:;}case 4: _LLCF: _tmp123=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp122=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_tmp121=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f3;_LLD0:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp121)
return({struct Cyc_PP_Doc*_tmp109[2];({struct Cyc_PP_Doc*_tmp597=Cyc_Absynpp_ktvars2doc(_tmp123);_tmp109[1]=_tmp597;});_tmp109[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2));});else{
# 636
return({struct Cyc_PP_Doc*_tmp10A[2];({struct Cyc_PP_Doc*_tmp598=Cyc_Absynpp_tvars2doc(_tmp123);_tmp10A[1]=_tmp598;});_tmp10A[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2));});}default: _LLD1: _tmp128=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).rgn;_tmp127=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).nullable;_tmp126=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).bounds;_tmp125=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).zero_term;_tmp124=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLD2: {
# 640
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp124);
{void*_tmp10B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp126);void*_tmp10C=_tmp10B;struct Cyc_Absyn_Exp*_tmp114;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp10C)->tag == 0){_LLDE: _LLDF:
({struct Cyc_PP_Doc*_tmp599=Cyc_Absynpp_question();ptr=_tmp599;});goto _LLDD;}else{_LLE0: _tmp114=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp10C)->f1;_LLE1:
# 648
({struct Cyc_PP_Doc*_tmp59A=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp127)?({const char*_tmp10D="*";_tag_dyneither(_tmp10D,sizeof(char),2);}):({const char*_tmp10E="@";_tag_dyneither(_tmp10E,sizeof(char),2);}));ptr=_tmp59A;});{
struct _tuple11 _tmp10F=Cyc_Evexp_eval_const_uint_exp(_tmp114);struct _tuple11 _tmp110=_tmp10F;unsigned int _tmp113;int _tmp112;_LLE3: _tmp113=_tmp110.f1;_tmp112=_tmp110.f2;_LLE4:;
if(!_tmp112  || _tmp113 != 1)
({struct Cyc_PP_Doc*_tmp59E=({struct Cyc_PP_Doc*_tmp111[4];({struct Cyc_PP_Doc*_tmp59B=Cyc_Absynpp_rb();_tmp111[3]=_tmp59B;});({struct Cyc_PP_Doc*_tmp59C=Cyc_Absynpp_exp2doc(_tmp114);_tmp111[2]=_tmp59C;});({struct Cyc_PP_Doc*_tmp59D=Cyc_Absynpp_lb();_tmp111[1]=_tmp59D;});_tmp111[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp111,sizeof(struct Cyc_PP_Doc*),4));});ptr=_tmp59E;});
goto _LLDD;};}_LLDD:;}
# 654
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
({struct Cyc_PP_Doc*_tmp59F=Cyc_PP_text(({const char*_tmp115="@zeroterm";_tag_dyneither(_tmp115,sizeof(char),10);}));ztd=_tmp59F;});else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
({struct Cyc_PP_Doc*_tmp5A0=Cyc_PP_text(({const char*_tmp116="@nozeroterm";_tag_dyneither(_tmp116,sizeof(char),12);}));ztd=_tmp5A0;});}}
# 660
{void*_tmp117=Cyc_Tcutil_compress(_tmp128);void*_tmp118=_tmp117;switch(*((int*)_tmp118)){case 20: _LLE6: _LLE7:
 goto _LLE5;case 1: _LLE8: if(Cyc_Absynpp_print_for_cycdoc){_LLE9:
 goto _LLE5;}else{goto _LLEA;}default: _LLEA: _LLEB:
({struct Cyc_PP_Doc*_tmp5A1=Cyc_Absynpp_rgn2doc(_tmp128);rgd=_tmp5A1;});}_LLE5:;}{
# 665
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp11B=" ";_tag_dyneither(_tmp11B,sizeof(char),2);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp11A=" ";_tag_dyneither(_tmp11A,sizeof(char),2);})): mt;
return({struct Cyc_PP_Doc*_tmp119[7];_tmp119[6]=rest;_tmp119[5]=spacer2;_tmp119[4]=tqd;_tmp119[3]=spacer1;_tmp119[2]=rgd;_tmp119[1]=ztd;_tmp119[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp119,sizeof(struct Cyc_PP_Doc*),7));});};}}_LLC6:;};}
# 671
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;switch(*((int*)_tmp12D)){case 20: _LLED: _LLEE:
 return Cyc_PP_text(({const char*_tmp12E="`H";_tag_dyneither(_tmp12E,sizeof(char),3);}));case 21: _LLEF: _LLF0:
 return Cyc_PP_text(({const char*_tmp12F="`U";_tag_dyneither(_tmp12F,sizeof(char),3);}));case 22: _LLF1: _LLF2:
 return Cyc_PP_text(({const char*_tmp130="`RC";_tag_dyneither(_tmp130,sizeof(char),4);}));default: _LLF3: _LLF4:
 return Cyc_Absynpp_ntyp2doc(t);}_LLEC:;}
# 681
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 685
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;struct Cyc_List_List*_tmp136;void*_tmp135;switch(*((int*)_tmp132)){case 23: _LLF6: _tmp135=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp132)->f1;_LLF7:
({struct Cyc_List_List*_tmp5A3=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_PP_Doc*_tmp5A2=Cyc_Absynpp_rgn2doc(_tmp135);_tmp133->hd=_tmp5A2;});_tmp133->tl=*rgions;_tmp133;});*rgions=_tmp5A3;});goto _LLF5;case 24: _LLF8: _tmp136=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132)->f1;_LLF9:
# 688
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp136->hd);}
# 691
goto _LLF5;default: _LLFA: _LLFB:
({struct Cyc_List_List*_tmp5A5=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));({struct Cyc_PP_Doc*_tmp5A4=Cyc_Absynpp_typ2doc(t);_tmp134->hd=_tmp5A4;});_tmp134->tl=*effects;_tmp134;});*effects=_tmp5A5;});goto _LLF5;}_LLF5:;}
# 696
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
({struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);rgions=_tmp5A6;});
({struct Cyc_List_List*_tmp5A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);effects=_tmp5A7;});
if(rgions == 0  && effects != 0)
return({struct _dyneither_ptr _tmp5AA=({const char*_tmp137="";_tag_dyneither(_tmp137,sizeof(char),1);});struct _dyneither_ptr _tmp5A9=({const char*_tmp138="";_tag_dyneither(_tmp138,sizeof(char),1);});struct _dyneither_ptr _tmp5A8=({const char*_tmp139="+";_tag_dyneither(_tmp139,sizeof(char),2);});Cyc_PP_group(_tmp5AA,_tmp5A9,_tmp5A8,effects);});else{
# 704
struct Cyc_PP_Doc*_tmp13A=({struct _dyneither_ptr _tmp5AB=({const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2);});Cyc_Absynpp_group_braces(_tmp5AB,rgions);});
return({struct _dyneither_ptr _tmp5AF=({const char*_tmp13B="";_tag_dyneither(_tmp13B,sizeof(char),1);});struct _dyneither_ptr _tmp5AE=({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1);});struct _dyneither_ptr _tmp5AD=({const char*_tmp13D="+";_tag_dyneither(_tmp13D,sizeof(char),2);});Cyc_PP_group(_tmp5AF,_tmp5AE,_tmp5AD,({struct Cyc_List_List*_tmp5AC=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5AC,({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=_tmp13A;_tmp13E->tl=0;_tmp13E;}));}));});}}
# 709
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp140=k;if(_tmp140 == Cyc_Absyn_StructA){_LLFD: _LLFE:
 return Cyc_PP_text(({const char*_tmp141="struct ";_tag_dyneither(_tmp141,sizeof(char),8);}));}else{_LLFF: _LL100:
 return Cyc_PP_text(({const char*_tmp142="union ";_tag_dyneither(_tmp142,sizeof(char),7);}));}_LLFC:;}
# 717
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp143=t;struct Cyc_Absyn_Datatypedecl*_tmp1B8;struct Cyc_Absyn_Enumdecl*_tmp1B7;struct Cyc_Absyn_Aggrdecl*_tmp1B6;void*_tmp1B5;void*_tmp1B4;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Typedefdecl*_tmp1AE;struct _dyneither_ptr _tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct _tuple0*_tmp1AA;struct Cyc_List_List*_tmp1A9;enum Cyc_Absyn_AggrKind _tmp1A8;struct Cyc_List_List*_tmp1A7;union Cyc_Absyn_AggrInfoU _tmp1A6;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;int _tmp1A3;enum Cyc_Absyn_Sign _tmp1A2;enum Cyc_Absyn_Size_of _tmp1A1;union Cyc_Absyn_DatatypeFieldInfoU _tmp1A0;struct Cyc_List_List*_tmp19F;union Cyc_Absyn_DatatypeInfoU _tmp19E;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Tvar*_tmp19C;struct Cyc_Core_Opt*_tmp19B;void*_tmp19A;int _tmp199;struct Cyc_Core_Opt*_tmp198;switch(*((int*)_tmp143)){case 8: _LL102: _LL103:
# 721
 return Cyc_PP_text(({const char*_tmp144="[[[array]]]";_tag_dyneither(_tmp144,sizeof(char),12);}));case 9: _LL104: _LL105:
 return Cyc_PP_nil_doc();case 5: _LL106: _LL107:
 return Cyc_PP_nil_doc();case 0: _LL108: _LL109:
# 725
({struct Cyc_PP_Doc*_tmp5B0=Cyc_PP_text(({const char*_tmp145="void";_tag_dyneither(_tmp145,sizeof(char),5);}));s=_tmp5B0;});goto _LL101;case 1: _LL10A: _tmp19B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f1;_tmp19A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_tmp199=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f3;_tmp198=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f4;_LL10B:
# 727
 if(_tmp19A != 0)
# 729
return Cyc_Absynpp_ntyp2doc(_tmp19A);else{
# 731
struct _dyneither_ptr kindstring=_tmp19B == 0?({const char*_tmp14A="K";_tag_dyneither(_tmp14A,sizeof(char),2);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp19B->v);
({struct Cyc_PP_Doc*_tmp5B2=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1;_tmp149.f1=(unsigned long)_tmp199;({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);({void*_tmp146[2]={& _tmp148,& _tmp149};({struct _dyneither_ptr _tmp5B1=({const char*_tmp147="`E%s%d";_tag_dyneither(_tmp147,sizeof(char),7);});Cyc_aprintf(_tmp5B1,_tag_dyneither(_tmp146,sizeof(void*),2));});});});}));s=_tmp5B2;});}
# 748 "absynpp.cyc"
goto _LL101;case 2: _LL10C: _tmp19C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL10D:
# 750
({struct Cyc_PP_Doc*_tmp5B3=Cyc_Absynpp_tvar2doc(_tmp19C);s=_tmp5B3;});
if(Cyc_Absynpp_print_all_kinds)
({struct Cyc_PP_Doc*_tmp5B6=({struct Cyc_PP_Doc*_tmp14B[3];({struct Cyc_PP_Doc*_tmp5B4=Cyc_Absynpp_kindbound2doc(_tmp19C->kind);_tmp14B[2]=_tmp5B4;});({struct Cyc_PP_Doc*_tmp5B5=Cyc_PP_text(({const char*_tmp14C="::";_tag_dyneither(_tmp14C,sizeof(char),3);}));_tmp14B[1]=_tmp5B5;});_tmp14B[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp14B,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp5B6;});
# 754
goto _LL101;case 3: _LL10E: _tmp19E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).datatype_info;_tmp19D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL10F:
# 756
{union Cyc_Absyn_DatatypeInfoU _tmp14D=_tmp19E;struct _tuple0*_tmp156;int _tmp155;struct _tuple0*_tmp154;int _tmp153;if((_tmp14D.UnknownDatatype).tag == 1){_LL141: _tmp154=((_tmp14D.UnknownDatatype).val).name;_tmp153=((_tmp14D.UnknownDatatype).val).is_extensible;_LL142:
# 758
 _tmp156=_tmp154;_tmp155=_tmp153;goto _LL144;}else{_LL143: _tmp156=(*(_tmp14D.KnownDatatype).val)->name;_tmp155=(*(_tmp14D.KnownDatatype).val)->is_extensible;_LL144: {
# 760
struct Cyc_PP_Doc*_tmp14E=Cyc_PP_text(({const char*_tmp152="datatype ";_tag_dyneither(_tmp152,sizeof(char),10);}));
struct Cyc_PP_Doc*_tmp14F=_tmp155?Cyc_PP_text(({const char*_tmp151="@extensible ";_tag_dyneither(_tmp151,sizeof(char),13);})): Cyc_PP_nil_doc();
({struct Cyc_PP_Doc*_tmp5B9=({struct Cyc_PP_Doc*_tmp150[4];({struct Cyc_PP_Doc*_tmp5B7=Cyc_Absynpp_tps2doc(_tmp19D);_tmp150[3]=_tmp5B7;});({struct Cyc_PP_Doc*_tmp5B8=Cyc_Absynpp_qvar2doc(_tmp156);_tmp150[2]=_tmp5B8;});_tmp150[1]=_tmp14E;_tmp150[0]=_tmp14F;Cyc_PP_cat(_tag_dyneither(_tmp150,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp5B9;});
goto _LL140;}}_LL140:;}
# 765
goto _LL101;case 4: _LL110: _tmp1A0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).field_info;_tmp19F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL111:
# 767
{union Cyc_Absyn_DatatypeFieldInfoU _tmp157=_tmp1A0;struct _tuple0*_tmp162;int _tmp161;struct _tuple0*_tmp160;struct _tuple0*_tmp15F;struct _tuple0*_tmp15E;int _tmp15D;if((_tmp157.UnknownDatatypefield).tag == 1){_LL146: _tmp15F=((_tmp157.UnknownDatatypefield).val).datatype_name;_tmp15E=((_tmp157.UnknownDatatypefield).val).field_name;_tmp15D=((_tmp157.UnknownDatatypefield).val).is_extensible;_LL147:
# 769
 _tmp162=_tmp15F;_tmp161=_tmp15D;_tmp160=_tmp15E;goto _LL149;}else{_LL148: _tmp162=(((_tmp157.KnownDatatypefield).val).f1)->name;_tmp161=(((_tmp157.KnownDatatypefield).val).f1)->is_extensible;_tmp160=(((_tmp157.KnownDatatypefield).val).f2)->name;_LL149: {
# 772
struct Cyc_PP_Doc*_tmp158=Cyc_PP_text(_tmp161?({const char*_tmp15B="@extensible datatype ";_tag_dyneither(_tmp15B,sizeof(char),22);}):({const char*_tmp15C="datatype ";_tag_dyneither(_tmp15C,sizeof(char),10);}));
({struct Cyc_PP_Doc*_tmp5BD=({struct Cyc_PP_Doc*_tmp159[4];({struct Cyc_PP_Doc*_tmp5BA=Cyc_Absynpp_qvar2doc(_tmp160);_tmp159[3]=_tmp5BA;});({struct Cyc_PP_Doc*_tmp5BB=Cyc_PP_text(({const char*_tmp15A=".";_tag_dyneither(_tmp15A,sizeof(char),2);}));_tmp159[2]=_tmp5BB;});({struct Cyc_PP_Doc*_tmp5BC=Cyc_Absynpp_qvar2doc(_tmp162);_tmp159[1]=_tmp5BC;});_tmp159[0]=_tmp158;Cyc_PP_cat(_tag_dyneither(_tmp159,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp5BD;});
goto _LL145;}}_LL145:;}
# 776
goto _LL101;case 6: _LL112: _tmp1A2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f2;_LL113: {
# 778
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp163=_tmp1A2;switch(_tmp163){case Cyc_Absyn_None: _LL14B: _LL14C:
 goto _LL14E;case Cyc_Absyn_Signed: _LL14D: _LL14E:
({struct _dyneither_ptr _tmp5BE=({const char*_tmp164="";_tag_dyneither(_tmp164,sizeof(char),1);});sns=_tmp5BE;});goto _LL14A;default: _LL14F: _LL150:
({struct _dyneither_ptr _tmp5BF=({const char*_tmp165="unsigned ";_tag_dyneither(_tmp165,sizeof(char),10);});sns=_tmp5BF;});goto _LL14A;}_LL14A:;}
# 785
{enum Cyc_Absyn_Size_of _tmp166=_tmp1A1;switch(_tmp166){case Cyc_Absyn_Char_sz: _LL152: _LL153:
# 787
{enum Cyc_Absyn_Sign _tmp167=_tmp1A2;switch(_tmp167){case Cyc_Absyn_None: _LL15D: _LL15E:
({struct _dyneither_ptr _tmp5C0=({const char*_tmp168="";_tag_dyneither(_tmp168,sizeof(char),1);});sns=_tmp5C0;});goto _LL15C;case Cyc_Absyn_Signed: _LL15F: _LL160:
({struct _dyneither_ptr _tmp5C1=({const char*_tmp169="signed ";_tag_dyneither(_tmp169,sizeof(char),8);});sns=_tmp5C1;});goto _LL15C;default: _LL161: _LL162:
({struct _dyneither_ptr _tmp5C2=({const char*_tmp16A="unsigned ";_tag_dyneither(_tmp16A,sizeof(char),10);});sns=_tmp5C2;});goto _LL15C;}_LL15C:;}
# 792
({struct _dyneither_ptr _tmp5C3=({const char*_tmp16B="char";_tag_dyneither(_tmp16B,sizeof(char),5);});ts=_tmp5C3;});
goto _LL151;case Cyc_Absyn_Short_sz: _LL154: _LL155:
({struct _dyneither_ptr _tmp5C4=({const char*_tmp16C="short";_tag_dyneither(_tmp16C,sizeof(char),6);});ts=_tmp5C4;});goto _LL151;case Cyc_Absyn_Int_sz: _LL156: _LL157:
({struct _dyneither_ptr _tmp5C5=({const char*_tmp16D="int";_tag_dyneither(_tmp16D,sizeof(char),4);});ts=_tmp5C5;});goto _LL151;case Cyc_Absyn_Long_sz: _LL158: _LL159:
({struct _dyneither_ptr _tmp5C6=({const char*_tmp16E="long";_tag_dyneither(_tmp16E,sizeof(char),5);});ts=_tmp5C6;});goto _LL151;default: _LL15A: _LL15B:
# 798
{enum Cyc_Cyclone_C_Compilers _tmp16F=Cyc_Cyclone_c_compiler;if(_tmp16F == Cyc_Cyclone_Gcc_c){_LL164: _LL165:
({struct _dyneither_ptr _tmp5C7=({const char*_tmp170="long long";_tag_dyneither(_tmp170,sizeof(char),10);});ts=_tmp5C7;});goto _LL163;}else{_LL166: _LL167:
({struct _dyneither_ptr _tmp5C8=({const char*_tmp171="__int64";_tag_dyneither(_tmp171,sizeof(char),8);});ts=_tmp5C8;});goto _LL163;}_LL163:;}
# 802
goto _LL151;}_LL151:;}
# 804
({struct Cyc_PP_Doc*_tmp5CA=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);({void*_tmp172[2]={& _tmp174,& _tmp175};({struct _dyneither_ptr _tmp5C9=({const char*_tmp173="%s%s";_tag_dyneither(_tmp173,sizeof(char),5);});Cyc_aprintf(_tmp5C9,_tag_dyneither(_tmp172,sizeof(void*),2));});});});}));s=_tmp5CA;});
goto _LL101;}case 7: _LL114: _tmp1A3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp143)->f1;_LL115:
# 807
 if(_tmp1A3 == 0)
({struct Cyc_PP_Doc*_tmp5CB=Cyc_PP_text(({const char*_tmp176="float";_tag_dyneither(_tmp176,sizeof(char),6);}));s=_tmp5CB;});else{
if(_tmp1A3 == 1)
({struct Cyc_PP_Doc*_tmp5CC=Cyc_PP_text(({const char*_tmp177="double";_tag_dyneither(_tmp177,sizeof(char),7);}));s=_tmp5CC;});else{
# 812
({struct Cyc_PP_Doc*_tmp5CD=Cyc_PP_text(({const char*_tmp178="long double";_tag_dyneither(_tmp178,sizeof(char),12);}));s=_tmp5CD;});}}
goto _LL101;case 10: _LL116: _tmp1A4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL117:
# 815
({struct Cyc_PP_Doc*_tmp5D0=({struct Cyc_PP_Doc*_tmp179[2];({struct Cyc_PP_Doc*_tmp5CE=Cyc_Absynpp_args2doc(_tmp1A4);_tmp179[1]=_tmp5CE;});({struct Cyc_PP_Doc*_tmp5CF=Cyc_Absynpp_dollar();_tmp179[0]=_tmp5CF;});Cyc_PP_cat(_tag_dyneither(_tmp179,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp5D0;});
goto _LL101;case 11: _LL118: _tmp1A6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).aggr_info;_tmp1A5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL119: {
# 818
struct _tuple10 _tmp17A=Cyc_Absyn_aggr_kinded_name(_tmp1A6);struct _tuple10 _tmp17B=_tmp17A;enum Cyc_Absyn_AggrKind _tmp17E;struct _tuple0*_tmp17D;_LL169: _tmp17E=_tmp17B.f1;_tmp17D=_tmp17B.f2;_LL16A:;
({struct Cyc_PP_Doc*_tmp5D4=({struct Cyc_PP_Doc*_tmp17C[3];({struct Cyc_PP_Doc*_tmp5D1=Cyc_Absynpp_tps2doc(_tmp1A5);_tmp17C[2]=_tmp5D1;});({struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_qvar2doc(_tmp17D);_tmp17C[1]=_tmp5D2;});({struct Cyc_PP_Doc*_tmp5D3=Cyc_Absynpp_aggr_kind2doc(_tmp17E);_tmp17C[0]=_tmp5D3;});Cyc_PP_cat(_tag_dyneither(_tmp17C,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp5D4;});
goto _LL101;}case 12: _LL11A: _tmp1A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL11B:
# 822
({struct Cyc_PP_Doc*_tmp5D9=({struct Cyc_PP_Doc*_tmp17F[4];({struct Cyc_PP_Doc*_tmp5D5=
# 824
Cyc_Absynpp_rb();_tmp17F[3]=_tmp5D5;});({struct Cyc_PP_Doc*_tmp5D6=
# 823
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1A7));_tmp17F[2]=_tmp5D6;});({struct Cyc_PP_Doc*_tmp5D7=
# 822
Cyc_Absynpp_lb();_tmp17F[1]=_tmp5D7;});({struct Cyc_PP_Doc*_tmp5D8=Cyc_Absynpp_aggr_kind2doc(_tmp1A8);_tmp17F[0]=_tmp5D8;});Cyc_PP_cat(_tag_dyneither(_tmp17F,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp5D9;});
# 825
goto _LL101;case 14: _LL11C: _tmp1A9=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp143)->f1;_LL11D:
# 827
({struct Cyc_PP_Doc*_tmp5DE=({struct Cyc_PP_Doc*_tmp180[4];({struct Cyc_PP_Doc*_tmp5DA=Cyc_Absynpp_rb();_tmp180[3]=_tmp5DA;});({struct Cyc_PP_Doc*_tmp5DB=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1A9));_tmp180[2]=_tmp5DB;});({struct Cyc_PP_Doc*_tmp5DC=Cyc_Absynpp_lb();_tmp180[1]=_tmp5DC;});({struct Cyc_PP_Doc*_tmp5DD=Cyc_PP_text(({const char*_tmp181="enum ";_tag_dyneither(_tmp181,sizeof(char),6);}));_tmp180[0]=_tmp5DD;});Cyc_PP_cat(_tag_dyneither(_tmp180,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp5DE;});goto _LL101;case 13: _LL11E: _tmp1AA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp143)->f1;_LL11F:
# 829
({struct Cyc_PP_Doc*_tmp5E1=({struct Cyc_PP_Doc*_tmp182[2];({struct Cyc_PP_Doc*_tmp5DF=Cyc_Absynpp_qvar2doc(_tmp1AA);_tmp182[1]=_tmp5DF;});({struct Cyc_PP_Doc*_tmp5E0=Cyc_PP_text(({const char*_tmp183="enum ";_tag_dyneither(_tmp183,sizeof(char),6);}));_tmp182[0]=_tmp5E0;});Cyc_PP_cat(_tag_dyneither(_tmp182,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp5E1;});goto _LL101;case 18: _LL120: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL121:
# 831
({struct Cyc_PP_Doc*_tmp5E5=({struct Cyc_PP_Doc*_tmp184[3];({struct Cyc_PP_Doc*_tmp5E2=Cyc_PP_text(({const char*_tmp186=")";_tag_dyneither(_tmp186,sizeof(char),2);}));_tmp184[2]=_tmp5E2;});({struct Cyc_PP_Doc*_tmp5E3=Cyc_Absynpp_exp2doc(_tmp1AB);_tmp184[1]=_tmp5E3;});({struct Cyc_PP_Doc*_tmp5E4=Cyc_PP_text(({const char*_tmp185="valueof_t(";_tag_dyneither(_tmp185,sizeof(char),11);}));_tmp184[0]=_tmp5E4;});Cyc_PP_cat(_tag_dyneither(_tmp184,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp5E5;});goto _LL101;case 27: _LL122: _tmp1AC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL123:
# 833
({struct Cyc_PP_Doc*_tmp5E9=({struct Cyc_PP_Doc*_tmp187[3];({struct Cyc_PP_Doc*_tmp5E6=Cyc_PP_text(({const char*_tmp189=")";_tag_dyneither(_tmp189,sizeof(char),2);}));_tmp187[2]=_tmp5E6;});({struct Cyc_PP_Doc*_tmp5E7=Cyc_Absynpp_exp2doc(_tmp1AC);_tmp187[1]=_tmp5E7;});({struct Cyc_PP_Doc*_tmp5E8=Cyc_PP_text(({const char*_tmp188="typeof(";_tag_dyneither(_tmp188,sizeof(char),8);}));_tmp187[0]=_tmp5E8;});Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp5E9;});goto _LL101;case 28: _LL124: _tmp1AD=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp143)->f1;_LL125:
# 835
({struct Cyc_PP_Doc*_tmp5EA=Cyc_PP_text(_tmp1AD);s=_tmp5EA;});goto _LL101;case 17: _LL126: _tmp1B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_LL127:
# 842
({struct Cyc_PP_Doc*_tmp5ED=({struct Cyc_PP_Doc*_tmp18A[2];({struct Cyc_PP_Doc*_tmp5EB=Cyc_Absynpp_tps2doc(_tmp1AF);_tmp18A[1]=_tmp5EB;});({struct Cyc_PP_Doc*_tmp5EC=Cyc_Absynpp_qvar2doc(_tmp1B0);_tmp18A[0]=_tmp5EC;});Cyc_PP_cat(_tag_dyneither(_tmp18A,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp5ED;});
goto _LL101;case 15: _LL128: _tmp1B1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp143)->f1;_LL129:
# 845
({struct Cyc_PP_Doc*_tmp5F1=({struct Cyc_PP_Doc*_tmp18B[3];({struct Cyc_PP_Doc*_tmp5EE=Cyc_PP_text(({const char*_tmp18D=">";_tag_dyneither(_tmp18D,sizeof(char),2);}));_tmp18B[2]=_tmp5EE;});({struct Cyc_PP_Doc*_tmp5EF=Cyc_Absynpp_rgn2doc(_tmp1B1);_tmp18B[1]=_tmp5EF;});({struct Cyc_PP_Doc*_tmp5F0=Cyc_PP_text(({const char*_tmp18C="region_t<";_tag_dyneither(_tmp18C,sizeof(char),10);}));_tmp18B[0]=_tmp5F0;});Cyc_PP_cat(_tag_dyneither(_tmp18B,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp5F1;});goto _LL101;case 16: _LL12A: _tmp1B3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f1;_tmp1B2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f2;_LL12B:
# 847
({struct Cyc_PP_Doc*_tmp5F7=({struct Cyc_PP_Doc*_tmp18E[5];({struct Cyc_PP_Doc*_tmp5F2=
Cyc_PP_text(({const char*_tmp191=">";_tag_dyneither(_tmp191,sizeof(char),2);}));_tmp18E[4]=_tmp5F2;});({struct Cyc_PP_Doc*_tmp5F3=
# 847
Cyc_Absynpp_rgn2doc(_tmp1B2);_tmp18E[3]=_tmp5F3;});({struct Cyc_PP_Doc*_tmp5F4=Cyc_PP_text(({const char*_tmp190=",";_tag_dyneither(_tmp190,sizeof(char),2);}));_tmp18E[2]=_tmp5F4;});({struct Cyc_PP_Doc*_tmp5F5=Cyc_Absynpp_rgn2doc(_tmp1B3);_tmp18E[1]=_tmp5F5;});({struct Cyc_PP_Doc*_tmp5F6=Cyc_PP_text(({const char*_tmp18F="dynregion_t<";_tag_dyneither(_tmp18F,sizeof(char),13);}));_tmp18E[0]=_tmp5F6;});Cyc_PP_cat(_tag_dyneither(_tmp18E,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp5F7;});
goto _LL101;case 19: _LL12C: _tmp1B4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp143)->f1;_LL12D:
# 850
({struct Cyc_PP_Doc*_tmp5FB=({struct Cyc_PP_Doc*_tmp192[3];({struct Cyc_PP_Doc*_tmp5F8=Cyc_PP_text(({const char*_tmp194=">";_tag_dyneither(_tmp194,sizeof(char),2);}));_tmp192[2]=_tmp5F8;});({struct Cyc_PP_Doc*_tmp5F9=Cyc_Absynpp_typ2doc(_tmp1B4);_tmp192[1]=_tmp5F9;});({struct Cyc_PP_Doc*_tmp5FA=Cyc_PP_text(({const char*_tmp193="tag_t<";_tag_dyneither(_tmp193,sizeof(char),7);}));_tmp192[0]=_tmp5FA;});Cyc_PP_cat(_tag_dyneither(_tmp192,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp5FB;});goto _LL101;case 21: _LL12E: _LL12F:
 goto _LL131;case 20: _LL130: _LL131:
 goto _LL133;case 22: _LL132: _LL133:
# 854
({struct Cyc_PP_Doc*_tmp5FC=Cyc_Absynpp_rgn2doc(t);s=_tmp5FC;});goto _LL101;case 25: _LL134: _tmp1B5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp143)->f1;_LL135:
# 856
({struct Cyc_PP_Doc*_tmp600=({struct Cyc_PP_Doc*_tmp195[3];({struct Cyc_PP_Doc*_tmp5FD=Cyc_PP_text(({const char*_tmp197=")";_tag_dyneither(_tmp197,sizeof(char),2);}));_tmp195[2]=_tmp5FD;});({struct Cyc_PP_Doc*_tmp5FE=Cyc_Absynpp_typ2doc(_tmp1B5);_tmp195[1]=_tmp5FE;});({struct Cyc_PP_Doc*_tmp5FF=Cyc_PP_text(({const char*_tmp196="regions(";_tag_dyneither(_tmp196,sizeof(char),9);}));_tmp195[0]=_tmp5FF;});Cyc_PP_cat(_tag_dyneither(_tmp195,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp600;});goto _LL101;case 23: _LL136: _LL137:
 goto _LL139;case 24: _LL138: _LL139:
# 859
({struct Cyc_PP_Doc*_tmp601=Cyc_Absynpp_eff2doc(t);s=_tmp601;});goto _LL101;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0: _LL13A: _tmp1B6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13B:
# 861
({struct Cyc_PP_Doc*_tmp602=Cyc_Absynpp_aggrdecl2doc(_tmp1B6);s=_tmp602;});goto _LL101;case 1: _LL13C: _tmp1B7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13D:
# 863
({struct Cyc_PP_Doc*_tmp603=Cyc_Absynpp_enumdecl2doc(_tmp1B7);s=_tmp603;});goto _LL101;default: _LL13E: _tmp1B8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13F:
# 865
({struct Cyc_PP_Doc*_tmp604=Cyc_Absynpp_datatypedecl2doc(_tmp1B8);s=_tmp604;});goto _LL101;}}_LL101:;}
# 867
return s;}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1B9=cmp;void*_tmp1BD;void*_tmp1BC;_LL16C: _tmp1BD=_tmp1B9->f1;_tmp1BC=_tmp1B9->f2;_LL16D:;
return({struct Cyc_PP_Doc*_tmp1BA[3];({struct Cyc_PP_Doc*_tmp605=Cyc_Absynpp_rgn2doc(_tmp1BC);_tmp1BA[2]=_tmp605;});({struct Cyc_PP_Doc*_tmp606=Cyc_PP_text(({const char*_tmp1BB=" > ";_tag_dyneither(_tmp1BB,sizeof(char),4);}));_tmp1BA[1]=_tmp606;});({struct Cyc_PP_Doc*_tmp607=Cyc_Absynpp_rgn2doc(_tmp1BD);_tmp1BA[0]=_tmp607;});Cyc_PP_cat(_tag_dyneither(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3));});}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _dyneither_ptr _tmp60A=({const char*_tmp1BE="";_tag_dyneither(_tmp1BE,sizeof(char),1);});struct _dyneither_ptr _tmp609=({const char*_tmp1BF="";_tag_dyneither(_tmp1BF,sizeof(char),1);});struct _dyneither_ptr _tmp608=({const char*_tmp1C0=",";_tag_dyneither(_tmp1C0,sizeof(char),2);});Cyc_PP_group(_tmp60A,_tmp609,_tmp608,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C1=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C1 == 0?0:({struct Cyc_Core_Opt*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(*_tmp1C1);_tmp1C2->v=_tmp60B;});_tmp1C2;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 889
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 893
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
({struct Cyc_List_List*_tmp60E=({struct Cyc_List_List*_tmp60D=_tmp1C3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60D,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));({struct Cyc_PP_Doc*_tmp60C=Cyc_PP_text(({const char*_tmp1C5="...";_tag_dyneither(_tmp1C5,sizeof(char),4);}));_tmp1C4->hd=_tmp60C;});_tmp1C4->tl=0;_tmp1C4;}));});_tmp1C3=_tmp60E;});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1C6=({struct Cyc_PP_Doc*_tmp1C8[3];({struct Cyc_PP_Doc*_tmp60F=
# 900
Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=cyc_varargs->name;_tmp1CC->f2=cyc_varargs->tq;_tmp1CC->f3=cyc_varargs->type;_tmp1CC;}));_tmp1C8[2]=_tmp60F;});({struct Cyc_PP_Doc*_tmp610=
# 899
cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1CA=" inject ";_tag_dyneither(_tmp1CA,sizeof(char),9);})): Cyc_PP_text(({const char*_tmp1CB=" ";_tag_dyneither(_tmp1CB,sizeof(char),2);}));_tmp1C8[1]=_tmp610;});({struct Cyc_PP_Doc*_tmp611=
# 898
Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4);}));_tmp1C8[0]=_tmp611;});Cyc_PP_cat(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3));});
# 902
({struct Cyc_List_List*_tmp613=({struct Cyc_List_List*_tmp612=_tmp1C3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp612,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=_tmp1C6;_tmp1C7->tl=0;_tmp1C7;}));});_tmp1C3=_tmp613;});}}{
# 904
struct Cyc_PP_Doc*_tmp1CD=({struct _dyneither_ptr _tmp616=({const char*_tmp1DC="";_tag_dyneither(_tmp1DC,sizeof(char),1);});struct _dyneither_ptr _tmp615=({const char*_tmp1DD="";_tag_dyneither(_tmp1DD,sizeof(char),1);});struct _dyneither_ptr _tmp614=({const char*_tmp1DE=",";_tag_dyneither(_tmp1DE,sizeof(char),2);});Cyc_PP_group(_tmp616,_tmp615,_tmp614,_tmp1C3);});
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
({struct Cyc_PP_Doc*_tmp619=({struct Cyc_PP_Doc*_tmp1CE[3];({struct Cyc_PP_Doc*_tmp617=Cyc_Absynpp_eff2doc(effopt);_tmp1CE[2]=_tmp617;});({struct Cyc_PP_Doc*_tmp618=Cyc_PP_text(({const char*_tmp1CF=";";_tag_dyneither(_tmp1CF,sizeof(char),2);}));_tmp1CE[1]=_tmp618;});_tmp1CE[0]=_tmp1CD;Cyc_PP_cat(_tag_dyneither(_tmp1CE,sizeof(struct Cyc_PP_Doc*),3));});_tmp1CD=_tmp619;});
if(rgn_po != 0)
({struct Cyc_PP_Doc*_tmp61C=({struct Cyc_PP_Doc*_tmp1D0[3];({struct Cyc_PP_Doc*_tmp61A=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1D0[2]=_tmp61A;});({struct Cyc_PP_Doc*_tmp61B=Cyc_PP_text(({const char*_tmp1D1=":";_tag_dyneither(_tmp1D1,sizeof(char),2);}));_tmp1D0[1]=_tmp61B;});_tmp1D0[0]=_tmp1CD;Cyc_PP_cat(_tag_dyneither(_tmp1D0,sizeof(struct Cyc_PP_Doc*),3));});_tmp1CD=_tmp61C;});{
struct Cyc_PP_Doc*_tmp1D2=({struct Cyc_PP_Doc*_tmp1D9[3];({struct Cyc_PP_Doc*_tmp61D=Cyc_PP_text(({const char*_tmp1DB=")";_tag_dyneither(_tmp1DB,sizeof(char),2);}));_tmp1D9[2]=_tmp61D;});_tmp1D9[1]=_tmp1CD;({struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp1DA="(";_tag_dyneither(_tmp1DA,sizeof(char),2);}));_tmp1D9[0]=_tmp61E;});Cyc_PP_cat(_tag_dyneither(_tmp1D9,sizeof(struct Cyc_PP_Doc*),3));});
if(req != 0)
({struct Cyc_PP_Doc*_tmp622=({struct Cyc_PP_Doc*_tmp1D3[4];({struct Cyc_PP_Doc*_tmp61F=Cyc_PP_text(({const char*_tmp1D5=")";_tag_dyneither(_tmp1D5,sizeof(char),2);}));_tmp1D3[3]=_tmp61F;});({struct Cyc_PP_Doc*_tmp620=Cyc_Absynpp_exp2doc(req);_tmp1D3[2]=_tmp620;});({struct Cyc_PP_Doc*_tmp621=Cyc_PP_text(({const char*_tmp1D4=" @requires(";_tag_dyneither(_tmp1D4,sizeof(char),12);}));_tmp1D3[1]=_tmp621;});_tmp1D3[0]=_tmp1D2;Cyc_PP_cat(_tag_dyneither(_tmp1D3,sizeof(struct Cyc_PP_Doc*),4));});_tmp1D2=_tmp622;});
if(ens != 0)
({struct Cyc_PP_Doc*_tmp626=({struct Cyc_PP_Doc*_tmp1D6[4];({struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp1D8=")";_tag_dyneither(_tmp1D8,sizeof(char),2);}));_tmp1D6[3]=_tmp623;});({struct Cyc_PP_Doc*_tmp624=Cyc_Absynpp_exp2doc(ens);_tmp1D6[2]=_tmp624;});({struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp1D7=" @ensures(";_tag_dyneither(_tmp1D7,sizeof(char),11);}));_tmp1D6[1]=_tmp625;});_tmp1D6[0]=_tmp1D2;Cyc_PP_cat(_tag_dyneither(_tmp1D6,sizeof(struct Cyc_PP_Doc*),4));});_tmp1D2=_tmp626;});
return _tmp1D2;};};}
# 917
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1DF=arg;struct _dyneither_ptr*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;_LL16F: _tmp1E3=_tmp1DF->f1;_tmp1E2=_tmp1DF->f2;_tmp1E1=_tmp1DF->f3;_LL170:;
return({struct _tuple8*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->f1=_tmp1E3;_tmp1E0->f2=_tmp1E2;_tmp1E0->f3=_tmp1E1;_tmp1E0;});}
# 922
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 924
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E4=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1E5=(*q).f1;union Cyc_Absyn_Nmspace _tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;switch((_tmp1E6.C_n).tag){case 4: _LL172: _LL173:
 _tmp1E8=0;goto _LL175;case 1: _LL174: _tmp1E8=(_tmp1E6.Rel_n).val;_LL175:
# 930
 match=0;
_tmp1E4=_tmp1E8;
goto _LL171;case 3: _LL176: _tmp1E9=(_tmp1E6.C_n).val;_LL177:
# 934
({int _tmp627=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E9,Cyc_Absynpp_curr_namespace);match=_tmp627;});
# 936
goto _LL171;default: _LL178: _tmp1EA=(_tmp1E6.Abs_n).val;_LL179:
# 938
({int _tmp628=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EA,Cyc_Absynpp_curr_namespace);match=_tmp628;});
({struct Cyc_List_List*_tmp629=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=Cyc_Absynpp_cyc_stringptr;_tmp1E7->tl=_tmp1EA;_tmp1E7;}): _tmp1EA;_tmp1E4=_tmp629;});
goto _LL171;}_LL171:;}
# 942
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp62B=({struct Cyc_List_List*_tmp62A=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62A,({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(*q).f2;_tmp1EB->tl=0;_tmp1EB;}));});Cyc_str_sepstr(_tmp62B,({const char*_tmp1EC="_";_tag_dyneither(_tmp1EC,sizeof(char),2);}));});else{
# 947
if(match)
return*(*q).f2;else{
# 950
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp62D=({struct Cyc_List_List*_tmp62C=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62C,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(*q).f2;_tmp1ED->tl=0;_tmp1ED;}));});Cyc_str_sepstr(_tmp62D,({const char*_tmp1EE="::";_tag_dyneither(_tmp1EE,sizeof(char),3);}));});}}}
# 954
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 958
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 962
return({struct _dyneither_ptr _tmp630=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp62F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp62E=({const char*_tmp1F0="\\textbf{";_tag_dyneither(_tmp1F0,sizeof(char),9);});Cyc_strconcat(_tmp62E,(struct _dyneither_ptr)_tmp1EF);});Cyc_strconcat(_tmp62F,({const char*_tmp1F1="}";_tag_dyneither(_tmp1F1,sizeof(char),2);}));});Cyc_PP_text_width(_tmp630,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1EF));});else{
return Cyc_PP_text(_tmp1EF);}}
# 967
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 969
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 972
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F2=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F3=_tmp1F2;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F5;switch((_tmp1F3.C_n).tag){case 4: _LL17B: _LL17C:
 goto _LL17E;case 1: if((_tmp1F3.Rel_n).val == 0){_LL17D: _LL17E:
 return*(*v).f2;}else{_LL183: _LL184:
# 983
 return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp631=({const char*_tmp1F4="/* bad namespace : */ ";_tag_dyneither(_tmp1F4,sizeof(char),23);});Cyc_strconcat(_tmp631,(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));});}case 3: _LL17F: _tmp1F5=(_tmp1F3.C_n).val;_LL180:
# 976
 _tmp1F6=_tmp1F5;goto _LL182;default: _LL181: _tmp1F6=(_tmp1F3.Abs_n).val;_LL182:
# 978
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1F6,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 981
goto _LL184;}}_LL17A:;}else{
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
return({struct _dyneither_ptr _tmp634=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp633=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp632=({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9);});Cyc_strconcat(_tmp632,(struct _dyneither_ptr)_tmp1F7);});Cyc_strconcat(_tmp633,({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2);}));});Cyc_PP_text_width(_tmp634,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));});else{
return Cyc_PP_text(_tmp1F7);}}
# 1001
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp635=Cyc_Absyn_empty_tqual(0);Cyc_Absynpp_tqtd2doc(_tmp635,t,0);});}
# 1012 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->r;void*_tmp1FB=_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp1FE;enum Cyc_Absyn_Primop _tmp1FD;switch(*((int*)_tmp1FB)){case 0: _LL186: _LL187:
 goto _LL189;case 1: _LL188: _LL189:
 return 10000;case 3: _LL18A: _tmp1FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL18B: {
# 1017
enum Cyc_Absyn_Primop _tmp1FC=_tmp1FD;switch(_tmp1FC){case Cyc_Absyn_Plus: _LL1D9: _LL1DA:
 return 100;case Cyc_Absyn_Times: _LL1DB: _LL1DC:
 return 110;case Cyc_Absyn_Minus: _LL1DD: _LL1DE:
 return 100;case Cyc_Absyn_Div: _LL1DF: _LL1E0:
 goto _LL1E2;case Cyc_Absyn_Mod: _LL1E1: _LL1E2:
 return 110;case Cyc_Absyn_Eq: _LL1E3: _LL1E4:
 goto _LL1E6;case Cyc_Absyn_Neq: _LL1E5: _LL1E6:
 return 70;case Cyc_Absyn_Gt: _LL1E7: _LL1E8:
 goto _LL1EA;case Cyc_Absyn_Lt: _LL1E9: _LL1EA:
 goto _LL1EC;case Cyc_Absyn_Gte: _LL1EB: _LL1EC:
 goto _LL1EE;case Cyc_Absyn_Lte: _LL1ED: _LL1EE:
 return 80;case Cyc_Absyn_Not: _LL1EF: _LL1F0:
 goto _LL1F2;case Cyc_Absyn_Bitnot: _LL1F1: _LL1F2:
 return 130;case Cyc_Absyn_Bitand: _LL1F3: _LL1F4:
 return 60;case Cyc_Absyn_Bitor: _LL1F5: _LL1F6:
 return 40;case Cyc_Absyn_Bitxor: _LL1F7: _LL1F8:
 return 50;case Cyc_Absyn_Bitlshift: _LL1F9: _LL1FA:
 return 90;case Cyc_Absyn_Bitlrshift: _LL1FB: _LL1FC:
 return 80;case Cyc_Absyn_Bitarshift: _LL1FD: _LL1FE:
 return 80;default: _LL1FF: _LL200:
 return 140;}_LL1D8:;}case 4: _LL18C: _LL18D:
# 1039
 return 20;case 5: _LL18E: _LL18F:
 return 130;case 6: _LL190: _LL191:
 return 30;case 7: _LL192: _LL193:
 return 35;case 8: _LL194: _LL195:
 return 30;case 9: _LL196: _LL197:
 return 10;case 10: _LL198: _LL199:
 return 140;case 2: _LL19A: _LL19B:
 return 140;case 11: _LL19C: _LL19D:
 return 130;case 12: _LL19E: _tmp1FE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL19F:
 return Cyc_Absynpp_exp_prec(_tmp1FE);case 13: _LL1A0: _tmp1FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1A1:
 return Cyc_Absynpp_exp_prec(_tmp1FF);case 14: _LL1A2: _LL1A3:
 return 120;case 16: _LL1A4: _LL1A5:
 return 15;case 15: _LL1A6: _LL1A7:
 goto _LL1A9;case 17: _LL1A8: _LL1A9:
 goto _LL1AB;case 18: _LL1AA: _LL1AB:
 goto _LL1AD;case 39: _LL1AC: _LL1AD:
 goto _LL1AF;case 40: _LL1AE: _LL1AF:
 goto _LL1B1;case 38: _LL1B0: _LL1B1:
 goto _LL1B3;case 19: _LL1B2: _LL1B3:
 goto _LL1B5;case 20: _LL1B4: _LL1B5:
 return 130;case 21: _LL1B6: _LL1B7:
 goto _LL1B9;case 22: _LL1B8: _LL1B9:
 goto _LL1BB;case 23: _LL1BA: _LL1BB:
 return 140;case 24: _LL1BC: _LL1BD:
 return 150;case 25: _LL1BE: _LL1BF:
 goto _LL1C1;case 26: _LL1C0: _LL1C1:
 goto _LL1C3;case 27: _LL1C2: _LL1C3:
 goto _LL1C5;case 28: _LL1C4: _LL1C5:
 goto _LL1C7;case 29: _LL1C6: _LL1C7:
 goto _LL1C9;case 30: _LL1C8: _LL1C9:
 goto _LL1CB;case 31: _LL1CA: _LL1CB:
 goto _LL1CD;case 32: _LL1CC: _LL1CD:
 goto _LL1CF;case 33: _LL1CE: _LL1CF:
 goto _LL1D1;case 34: _LL1D0: _LL1D1:
 goto _LL1D3;case 35: _LL1D2: _LL1D3:
 goto _LL1D5;case 36: _LL1D4: _LL1D5:
 return 140;default: _LL1D6: _LL1D7:
 return 10000;}_LL185:;}
# 1080
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1084
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp200=e->r;void*_tmp201=_tmp200;struct Cyc_Absyn_Stmt*_tmp2D4;struct Cyc_Core_Opt*_tmp2D3;struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;int _tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;void**_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;int _tmp2CB;struct Cyc_Absyn_Enumfield*_tmp2CA;struct Cyc_Absyn_Enumfield*_tmp2C9;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_Datatypefield*_tmp2C7;struct Cyc_List_List*_tmp2C6;struct _tuple0*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_List_List*_tmp2BD;struct _tuple8*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;struct _dyneither_ptr*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B5;struct _dyneither_ptr*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;void*_tmp2B2;struct Cyc_List_List*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;struct _dyneither_ptr*_tmp2AF;int _tmp2AE;struct _dyneither_ptr _tmp2AD;void*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;void*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;void*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;enum Cyc_Absyn_Incrementor _tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Core_Opt*_tmp293;struct Cyc_Absyn_Exp*_tmp292;enum Cyc_Absyn_Primop _tmp291;struct Cyc_List_List*_tmp290;struct _dyneither_ptr _tmp28F;void*_tmp28E;union Cyc_Absyn_Cnst _tmp28D;switch(*((int*)_tmp201)){case 0: _LL202: _tmp28D=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL203:
({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_cnst2doc(_tmp28D);s=_tmp636;});goto _LL201;case 1: _LL204: _tmp28E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL205:
({struct Cyc_PP_Doc*_tmp637=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp28E));s=_tmp637;});goto _LL201;case 2: _LL206: _tmp28F=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL207:
# 1091
({struct Cyc_PP_Doc*_tmp63C=({struct Cyc_PP_Doc*_tmp202[4];({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp205=")";_tag_dyneither(_tmp205,sizeof(char),2);}));_tmp202[3]=_tmp638;});({struct Cyc_PP_Doc*_tmp639=Cyc_PP_text(_tmp28F);_tmp202[2]=_tmp639;});({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp204="(";_tag_dyneither(_tmp204,sizeof(char),2);}));_tmp202[1]=_tmp63A;});({struct Cyc_PP_Doc*_tmp63B=Cyc_PP_text(({const char*_tmp203="__cyclone_pragma__";_tag_dyneither(_tmp203,sizeof(char),19);}));_tmp202[0]=_tmp63B;});Cyc_PP_cat(_tag_dyneither(_tmp202,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp63C;});goto _LL201;case 3: _LL208: _tmp291=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp290=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL209:
({struct Cyc_PP_Doc*_tmp63D=Cyc_Absynpp_primapp2doc(myprec,_tmp291,_tmp290);s=_tmp63D;});goto _LL201;case 4: _LL20A: _tmp294=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp293=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp292=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL20B:
# 1094
({struct Cyc_PP_Doc*_tmp643=({struct Cyc_PP_Doc*_tmp206[5];({struct Cyc_PP_Doc*_tmp63E=
# 1098
Cyc_Absynpp_exp2doc_prec(myprec,_tmp292);_tmp206[4]=_tmp63E;});({struct Cyc_PP_Doc*_tmp63F=
# 1097
Cyc_PP_text(({const char*_tmp208="= ";_tag_dyneither(_tmp208,sizeof(char),3);}));_tmp206[3]=_tmp63F;});({struct Cyc_PP_Doc*_tmp640=
# 1096
_tmp293 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp293->v);_tmp206[2]=_tmp640;});({struct Cyc_PP_Doc*_tmp641=
# 1095
Cyc_PP_text(({const char*_tmp207=" ";_tag_dyneither(_tmp207,sizeof(char),2);}));_tmp206[1]=_tmp641;});({struct Cyc_PP_Doc*_tmp642=
# 1094
Cyc_Absynpp_exp2doc_prec(myprec,_tmp294);_tmp206[0]=_tmp642;});Cyc_PP_cat(_tag_dyneither(_tmp206,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp643;});
# 1099
goto _LL201;case 5: _LL20C: _tmp296=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp295=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL20D: {
# 1101
struct Cyc_PP_Doc*_tmp209=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296);
{enum Cyc_Absyn_Incrementor _tmp20A=_tmp295;switch(_tmp20A){case Cyc_Absyn_PreInc: _LL255: _LL256:
({struct Cyc_PP_Doc*_tmp645=({struct Cyc_PP_Doc*_tmp20B[2];_tmp20B[1]=_tmp209;({struct Cyc_PP_Doc*_tmp644=Cyc_PP_text(({const char*_tmp20C="++";_tag_dyneither(_tmp20C,sizeof(char),3);}));_tmp20B[0]=_tmp644;});Cyc_PP_cat(_tag_dyneither(_tmp20B,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp645;});goto _LL254;case Cyc_Absyn_PreDec: _LL257: _LL258:
({struct Cyc_PP_Doc*_tmp647=({struct Cyc_PP_Doc*_tmp20D[2];_tmp20D[1]=_tmp209;({struct Cyc_PP_Doc*_tmp646=Cyc_PP_text(({const char*_tmp20E="--";_tag_dyneither(_tmp20E,sizeof(char),3);}));_tmp20D[0]=_tmp646;});Cyc_PP_cat(_tag_dyneither(_tmp20D,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp647;});goto _LL254;case Cyc_Absyn_PostInc: _LL259: _LL25A:
({struct Cyc_PP_Doc*_tmp649=({struct Cyc_PP_Doc*_tmp20F[2];({struct Cyc_PP_Doc*_tmp648=Cyc_PP_text(({const char*_tmp210="++";_tag_dyneither(_tmp210,sizeof(char),3);}));_tmp20F[1]=_tmp648;});_tmp20F[0]=_tmp209;Cyc_PP_cat(_tag_dyneither(_tmp20F,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp649;});goto _LL254;default: _LL25B: _LL25C:
({struct Cyc_PP_Doc*_tmp64B=({struct Cyc_PP_Doc*_tmp211[2];({struct Cyc_PP_Doc*_tmp64A=Cyc_PP_text(({const char*_tmp212="--";_tag_dyneither(_tmp212,sizeof(char),3);}));_tmp211[1]=_tmp64A;});_tmp211[0]=_tmp209;Cyc_PP_cat(_tag_dyneither(_tmp211,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp64B;});goto _LL254;}_LL254:;}
# 1108
goto _LL201;}case 6: _LL20E: _tmp299=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp298=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp297=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL20F:
# 1110
({struct Cyc_PP_Doc*_tmp651=({struct Cyc_PP_Doc*_tmp213[5];({struct Cyc_PP_Doc*_tmp64C=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp297);_tmp213[4]=_tmp64C;});({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp215=" : ";_tag_dyneither(_tmp215,sizeof(char),4);}));_tmp213[3]=_tmp64D;});({struct Cyc_PP_Doc*_tmp64E=
# 1110
Cyc_Absynpp_exp2doc_prec(0,_tmp298);_tmp213[2]=_tmp64E;});({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp214=" ? ";_tag_dyneither(_tmp214,sizeof(char),4);}));_tmp213[1]=_tmp64F;});({struct Cyc_PP_Doc*_tmp650=Cyc_Absynpp_exp2doc_prec(myprec,_tmp299);_tmp213[0]=_tmp650;});Cyc_PP_cat(_tag_dyneither(_tmp213,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp651;});
# 1112
goto _LL201;case 7: _LL210: _tmp29B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL211:
# 1114
({struct Cyc_PP_Doc*_tmp655=({struct Cyc_PP_Doc*_tmp216[3];({struct Cyc_PP_Doc*_tmp652=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29A);_tmp216[2]=_tmp652;});({struct Cyc_PP_Doc*_tmp653=Cyc_PP_text(({const char*_tmp217=" && ";_tag_dyneither(_tmp217,sizeof(char),5);}));_tmp216[1]=_tmp653;});({struct Cyc_PP_Doc*_tmp654=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B);_tmp216[0]=_tmp654;});Cyc_PP_cat(_tag_dyneither(_tmp216,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp655;});
goto _LL201;case 8: _LL212: _tmp29D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL213:
# 1117
({struct Cyc_PP_Doc*_tmp659=({struct Cyc_PP_Doc*_tmp218[3];({struct Cyc_PP_Doc*_tmp656=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29C);_tmp218[2]=_tmp656;});({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp219=" || ";_tag_dyneither(_tmp219,sizeof(char),5);}));_tmp218[1]=_tmp657;});({struct Cyc_PP_Doc*_tmp658=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp218[0]=_tmp658;});Cyc_PP_cat(_tag_dyneither(_tmp218,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp659;});
goto _LL201;case 9: _LL214: _tmp29F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL215:
# 1122
({struct Cyc_PP_Doc*_tmp65F=({struct Cyc_PP_Doc*_tmp21A[5];({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_text(({const char*_tmp21D=")";_tag_dyneither(_tmp21D,sizeof(char),2);}));_tmp21A[4]=_tmp65A;});({struct Cyc_PP_Doc*_tmp65B=Cyc_Absynpp_exp2doc(_tmp29E);_tmp21A[3]=_tmp65B;});({struct Cyc_PP_Doc*_tmp65C=Cyc_PP_text(({const char*_tmp21C=", ";_tag_dyneither(_tmp21C,sizeof(char),3);}));_tmp21A[2]=_tmp65C;});({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_exp2doc(_tmp29F);_tmp21A[1]=_tmp65D;});({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp21B="(";_tag_dyneither(_tmp21B,sizeof(char),2);}));_tmp21A[0]=_tmp65E;});Cyc_PP_cat(_tag_dyneither(_tmp21A,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp65F;});
goto _LL201;case 10: _LL216: _tmp2A1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL217:
# 1125
({struct Cyc_PP_Doc*_tmp664=({struct Cyc_PP_Doc*_tmp21E[4];({struct Cyc_PP_Doc*_tmp660=
# 1128
Cyc_PP_text(({const char*_tmp220=")";_tag_dyneither(_tmp220,sizeof(char),2);}));_tmp21E[3]=_tmp660;});({struct Cyc_PP_Doc*_tmp661=
# 1127
Cyc_Absynpp_exps2doc_prec(20,_tmp2A0);_tmp21E[2]=_tmp661;});({struct Cyc_PP_Doc*_tmp662=
# 1126
Cyc_PP_text(({const char*_tmp21F="(";_tag_dyneither(_tmp21F,sizeof(char),2);}));_tmp21E[1]=_tmp662;});({struct Cyc_PP_Doc*_tmp663=
# 1125
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A1);_tmp21E[0]=_tmp663;});Cyc_PP_cat(_tag_dyneither(_tmp21E,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp664;});
# 1129
goto _LL201;case 11: _LL218: _tmp2A2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL219:
# 1131
({struct Cyc_PP_Doc*_tmp667=({struct Cyc_PP_Doc*_tmp221[2];({struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp221[1]=_tmp665;});({struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp222="throw ";_tag_dyneither(_tmp222,sizeof(char),7);}));_tmp221[0]=_tmp666;});Cyc_PP_cat(_tag_dyneither(_tmp221,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp667;});
goto _LL201;case 12: _LL21A: _tmp2A3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL21B:
# 1134
({struct Cyc_PP_Doc*_tmp668=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A3);s=_tmp668;});
goto _LL201;case 13: _LL21C: _tmp2A4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL21D:
# 1137
({struct Cyc_PP_Doc*_tmp669=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A4);s=_tmp669;});
goto _LL201;case 14: _LL21E: _tmp2A6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21F:
# 1140
({struct Cyc_PP_Doc*_tmp66E=({struct Cyc_PP_Doc*_tmp223[4];({struct Cyc_PP_Doc*_tmp66A=
# 1143
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A5);_tmp223[3]=_tmp66A;});({struct Cyc_PP_Doc*_tmp66B=
# 1142
Cyc_PP_text(({const char*_tmp225=")";_tag_dyneither(_tmp225,sizeof(char),2);}));_tmp223[2]=_tmp66B;});({struct Cyc_PP_Doc*_tmp66C=
# 1141
Cyc_Absynpp_typ2doc(_tmp2A6);_tmp223[1]=_tmp66C;});({struct Cyc_PP_Doc*_tmp66D=
# 1140
Cyc_PP_text(({const char*_tmp224="(";_tag_dyneither(_tmp224,sizeof(char),2);}));_tmp223[0]=_tmp66D;});Cyc_PP_cat(_tag_dyneither(_tmp223,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp66E;});
# 1144
goto _LL201;case 15: _LL220: _tmp2A7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL221:
# 1146
({struct Cyc_PP_Doc*_tmp671=({struct Cyc_PP_Doc*_tmp226[2];({struct Cyc_PP_Doc*_tmp66F=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A7);_tmp226[1]=_tmp66F;});({struct Cyc_PP_Doc*_tmp670=
# 1146
Cyc_PP_text(({const char*_tmp227="&";_tag_dyneither(_tmp227,sizeof(char),2);}));_tmp226[0]=_tmp670;});Cyc_PP_cat(_tag_dyneither(_tmp226,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp671;});
# 1148
goto _LL201;case 16: _LL222: _tmp2A9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL223:
# 1150
 if(_tmp2A9 == 0)
({struct Cyc_PP_Doc*_tmp674=({struct Cyc_PP_Doc*_tmp228[2];({struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp228[1]=_tmp672;});({struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(({const char*_tmp229="new ";_tag_dyneither(_tmp229,sizeof(char),5);}));_tmp228[0]=_tmp673;});Cyc_PP_cat(_tag_dyneither(_tmp228,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp674;});else{
# 1153
({struct Cyc_PP_Doc*_tmp679=({struct Cyc_PP_Doc*_tmp22A[4];({struct Cyc_PP_Doc*_tmp675=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp22A[3]=_tmp675;});({struct Cyc_PP_Doc*_tmp676=
# 1153
Cyc_PP_text(({const char*_tmp22C=") ";_tag_dyneither(_tmp22C,sizeof(char),3);}));_tmp22A[2]=_tmp676;});({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_exp2doc(_tmp2A9);_tmp22A[1]=_tmp677;});({struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp22B="rnew(";_tag_dyneither(_tmp22B,sizeof(char),6);}));_tmp22A[0]=_tmp678;});Cyc_PP_cat(_tag_dyneither(_tmp22A,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp679;});}
# 1155
goto _LL201;case 17: _LL224: _tmp2AA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL225:
({struct Cyc_PP_Doc*_tmp67D=({struct Cyc_PP_Doc*_tmp22D[3];({struct Cyc_PP_Doc*_tmp67A=Cyc_PP_text(({const char*_tmp22F=")";_tag_dyneither(_tmp22F,sizeof(char),2);}));_tmp22D[2]=_tmp67A;});({struct Cyc_PP_Doc*_tmp67B=Cyc_Absynpp_typ2doc(_tmp2AA);_tmp22D[1]=_tmp67B;});({struct Cyc_PP_Doc*_tmp67C=Cyc_PP_text(({const char*_tmp22E="sizeof(";_tag_dyneither(_tmp22E,sizeof(char),8);}));_tmp22D[0]=_tmp67C;});Cyc_PP_cat(_tag_dyneither(_tmp22D,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp67D;});goto _LL201;case 18: _LL226: _tmp2AB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL227:
({struct Cyc_PP_Doc*_tmp681=({struct Cyc_PP_Doc*_tmp230[3];({struct Cyc_PP_Doc*_tmp67E=Cyc_PP_text(({const char*_tmp232=")";_tag_dyneither(_tmp232,sizeof(char),2);}));_tmp230[2]=_tmp67E;});({struct Cyc_PP_Doc*_tmp67F=Cyc_Absynpp_exp2doc(_tmp2AB);_tmp230[1]=_tmp67F;});({struct Cyc_PP_Doc*_tmp680=Cyc_PP_text(({const char*_tmp231="sizeof(";_tag_dyneither(_tmp231,sizeof(char),8);}));_tmp230[0]=_tmp680;});Cyc_PP_cat(_tag_dyneither(_tmp230,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp681;});goto _LL201;case 39: _LL228: _tmp2AC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL229:
({struct Cyc_PP_Doc*_tmp685=({struct Cyc_PP_Doc*_tmp233[3];({struct Cyc_PP_Doc*_tmp682=Cyc_PP_text(({const char*_tmp235=")";_tag_dyneither(_tmp235,sizeof(char),2);}));_tmp233[2]=_tmp682;});({struct Cyc_PP_Doc*_tmp683=Cyc_Absynpp_typ2doc(_tmp2AC);_tmp233[1]=_tmp683;});({struct Cyc_PP_Doc*_tmp684=Cyc_PP_text(({const char*_tmp234="valueof(";_tag_dyneither(_tmp234,sizeof(char),9);}));_tmp233[0]=_tmp684;});Cyc_PP_cat(_tag_dyneither(_tmp233,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp685;});goto _LL201;case 40: _LL22A: _tmp2AE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22B:
# 1160
 if(_tmp2AE)
({struct Cyc_PP_Doc*_tmp68A=({struct Cyc_PP_Doc*_tmp236[4];({struct Cyc_PP_Doc*_tmp686=Cyc_PP_text(({const char*_tmp239=")";_tag_dyneither(_tmp239,sizeof(char),2);}));_tmp236[3]=_tmp686;});({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(_tmp2AD);_tmp236[2]=_tmp687;});({struct Cyc_PP_Doc*_tmp688=Cyc_PP_text(({const char*_tmp238=" volatile (";_tag_dyneither(_tmp238,sizeof(char),12);}));_tmp236[1]=_tmp688;});({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp237="__asm__";_tag_dyneither(_tmp237,sizeof(char),8);}));_tmp236[0]=_tmp689;});Cyc_PP_cat(_tag_dyneither(_tmp236,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp68A;});else{
# 1163
({struct Cyc_PP_Doc*_tmp68E=({struct Cyc_PP_Doc*_tmp23A[3];({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp23C=")";_tag_dyneither(_tmp23C,sizeof(char),2);}));_tmp23A[2]=_tmp68B;});({struct Cyc_PP_Doc*_tmp68C=Cyc_PP_text(_tmp2AD);_tmp23A[1]=_tmp68C;});({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp23B="__asm__(";_tag_dyneither(_tmp23B,sizeof(char),9);}));_tmp23A[0]=_tmp68D;});Cyc_PP_cat(_tag_dyneither(_tmp23A,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp68E;});}
goto _LL201;case 38: _LL22C: _tmp2B0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22D:
# 1166
({struct Cyc_PP_Doc*_tmp694=({struct Cyc_PP_Doc*_tmp23D[5];({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_text(({const char*_tmp240=")";_tag_dyneither(_tmp240,sizeof(char),2);}));_tmp23D[4]=_tmp68F;});({struct Cyc_PP_Doc*_tmp690=Cyc_PP_textptr(_tmp2AF);_tmp23D[3]=_tmp690;});({struct Cyc_PP_Doc*_tmp691=Cyc_PP_text(({const char*_tmp23F=".";_tag_dyneither(_tmp23F,sizeof(char),2);}));_tmp23D[2]=_tmp691;});({struct Cyc_PP_Doc*_tmp692=Cyc_Absynpp_exp2doc(_tmp2B0);_tmp23D[1]=_tmp692;});({struct Cyc_PP_Doc*_tmp693=Cyc_PP_text(({const char*_tmp23E="tagcheck(";_tag_dyneither(_tmp23E,sizeof(char),10);}));_tmp23D[0]=_tmp693;});Cyc_PP_cat(_tag_dyneither(_tmp23D,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp694;});
goto _LL201;case 19: _LL22E: _tmp2B2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B1=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22F:
# 1169
({struct Cyc_PP_Doc*_tmp698=({struct Cyc_PP_Doc*_tmp241[3];({struct Cyc_PP_Doc*_tmp695=Cyc_PP_text(({const char*_tmp243=",";_tag_dyneither(_tmp243,sizeof(char),2);}));_tmp241[2]=_tmp695;});({struct Cyc_PP_Doc*_tmp696=Cyc_Absynpp_typ2doc(_tmp2B2);_tmp241[1]=_tmp696;});({struct Cyc_PP_Doc*_tmp697=Cyc_PP_text(({const char*_tmp242="offsetof(";_tag_dyneither(_tmp242,sizeof(char),10);}));_tmp241[0]=_tmp697;});Cyc_PP_cat(_tag_dyneither(_tmp241,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp698;});
for(0;_tmp2B1 != 0;_tmp2B1=_tmp2B1->tl){
void*_tmp244=(void*)_tmp2B1->hd;void*_tmp245=_tmp244;unsigned int _tmp24E;struct _dyneither_ptr*_tmp24D;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp245)->tag == 0){_LL25E: _tmp24D=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp245)->f1;_LL25F:
({struct Cyc_PP_Doc*_tmp69B=({struct Cyc_PP_Doc*_tmp246[3];({struct Cyc_PP_Doc*_tmp699=
_tmp2B1->tl != 0?Cyc_PP_text(({const char*_tmp247=".";_tag_dyneither(_tmp247,sizeof(char),2);})): Cyc_PP_nil_doc();_tmp246[2]=_tmp699;});({struct Cyc_PP_Doc*_tmp69A=
# 1172
Cyc_PP_textptr(_tmp24D);_tmp246[1]=_tmp69A;});_tmp246[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp246,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp69B;});
# 1174
goto _LL25D;}else{_LL260: _tmp24E=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp245)->f1;_LL261:
({struct Cyc_PP_Doc*_tmp69F=({struct Cyc_PP_Doc*_tmp248[3];({struct Cyc_PP_Doc*_tmp69C=
_tmp2B1->tl != 0?Cyc_PP_text(({const char*_tmp24C=".";_tag_dyneither(_tmp24C,sizeof(char),2);})): Cyc_PP_nil_doc();_tmp248[2]=_tmp69C;});({struct Cyc_PP_Doc*_tmp69E=
# 1175
Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp24B;_tmp24B.tag=1;_tmp24B.f1=(unsigned long)((int)_tmp24E);({void*_tmp249[1]={& _tmp24B};({struct _dyneither_ptr _tmp69D=({const char*_tmp24A="%d";_tag_dyneither(_tmp24A,sizeof(char),3);});Cyc_aprintf(_tmp69D,_tag_dyneither(_tmp249,sizeof(void*),1));});});}));_tmp248[1]=_tmp69E;});_tmp248[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp248,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp69F;});
# 1177
goto _LL25D;}_LL25D:;}
# 1179
({struct Cyc_PP_Doc*_tmp6A1=({struct Cyc_PP_Doc*_tmp24F[2];({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp250=")";_tag_dyneither(_tmp250,sizeof(char),2);}));_tmp24F[1]=_tmp6A0;});_tmp24F[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp24F,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp6A1;});
goto _LL201;case 20: _LL230: _tmp2B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL231:
# 1182
({struct Cyc_PP_Doc*_tmp6A4=({struct Cyc_PP_Doc*_tmp251[2];({struct Cyc_PP_Doc*_tmp6A2=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3);_tmp251[1]=_tmp6A2;});({struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp252="*";_tag_dyneither(_tmp252,sizeof(char),2);}));_tmp251[0]=_tmp6A3;});Cyc_PP_cat(_tag_dyneither(_tmp251,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp6A4;});
goto _LL201;case 21: _LL232: _tmp2B5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL233:
# 1185
({struct Cyc_PP_Doc*_tmp6A8=({struct Cyc_PP_Doc*_tmp253[3];({struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_textptr(_tmp2B4);_tmp253[2]=_tmp6A5;});({struct Cyc_PP_Doc*_tmp6A6=Cyc_PP_text(({const char*_tmp254=".";_tag_dyneither(_tmp254,sizeof(char),2);}));_tmp253[1]=_tmp6A6;});({struct Cyc_PP_Doc*_tmp6A7=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5);_tmp253[0]=_tmp6A7;});Cyc_PP_cat(_tag_dyneither(_tmp253,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp6A8;});
goto _LL201;case 22: _LL234: _tmp2B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL235:
# 1188
({struct Cyc_PP_Doc*_tmp6AC=({struct Cyc_PP_Doc*_tmp255[3];({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_textptr(_tmp2B6);_tmp255[2]=_tmp6A9;});({struct Cyc_PP_Doc*_tmp6AA=Cyc_PP_text(({const char*_tmp256="->";_tag_dyneither(_tmp256,sizeof(char),3);}));_tmp255[1]=_tmp6AA;});({struct Cyc_PP_Doc*_tmp6AB=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B7);_tmp255[0]=_tmp6AB;});Cyc_PP_cat(_tag_dyneither(_tmp255,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp6AC;});
goto _LL201;case 23: _LL236: _tmp2B9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL237:
# 1191
({struct Cyc_PP_Doc*_tmp6B1=({struct Cyc_PP_Doc*_tmp257[4];({struct Cyc_PP_Doc*_tmp6AD=
# 1194
Cyc_PP_text(({const char*_tmp259="]";_tag_dyneither(_tmp259,sizeof(char),2);}));_tmp257[3]=_tmp6AD;});({struct Cyc_PP_Doc*_tmp6AE=
# 1193
Cyc_Absynpp_exp2doc(_tmp2B8);_tmp257[2]=_tmp6AE;});({struct Cyc_PP_Doc*_tmp6AF=
# 1192
Cyc_PP_text(({const char*_tmp258="[";_tag_dyneither(_tmp258,sizeof(char),2);}));_tmp257[1]=_tmp6AF;});({struct Cyc_PP_Doc*_tmp6B0=
# 1191
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B9);_tmp257[0]=_tmp6B0;});Cyc_PP_cat(_tag_dyneither(_tmp257,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp6B1;});
# 1195
goto _LL201;case 24: _LL238: _tmp2BA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL239:
# 1197
({struct Cyc_PP_Doc*_tmp6B6=({struct Cyc_PP_Doc*_tmp25A[4];({struct Cyc_PP_Doc*_tmp6B2=
# 1200
Cyc_PP_text(({const char*_tmp25C=")";_tag_dyneither(_tmp25C,sizeof(char),2);}));_tmp25A[3]=_tmp6B2;});({struct Cyc_PP_Doc*_tmp6B3=
# 1199
Cyc_Absynpp_exps2doc_prec(20,_tmp2BA);_tmp25A[2]=_tmp6B3;});({struct Cyc_PP_Doc*_tmp6B4=
# 1198
Cyc_PP_text(({const char*_tmp25B="(";_tag_dyneither(_tmp25B,sizeof(char),2);}));_tmp25A[1]=_tmp6B4;});({struct Cyc_PP_Doc*_tmp6B5=
# 1197
Cyc_Absynpp_dollar();_tmp25A[0]=_tmp6B5;});Cyc_PP_cat(_tag_dyneither(_tmp25A,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp6B6;});
# 1201
goto _LL201;case 25: _LL23A: _tmp2BC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL23B:
# 1203
({struct Cyc_PP_Doc*_tmp6BC=({struct Cyc_PP_Doc*_tmp25D[4];({struct Cyc_PP_Doc*_tmp6B8=({
# 1206
struct _dyneither_ptr _tmp6B7=({const char*_tmp260=",";_tag_dyneither(_tmp260,sizeof(char),2);});Cyc_Absynpp_group_braces(_tmp6B7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BB));});_tmp25D[3]=_tmp6B8;});({struct Cyc_PP_Doc*_tmp6B9=
# 1205
Cyc_PP_text(({const char*_tmp25F=")";_tag_dyneither(_tmp25F,sizeof(char),2);}));_tmp25D[2]=_tmp6B9;});({struct Cyc_PP_Doc*_tmp6BA=
# 1204
Cyc_Absynpp_typ2doc((*_tmp2BC).f3);_tmp25D[1]=_tmp6BA;});({struct Cyc_PP_Doc*_tmp6BB=
# 1203
Cyc_PP_text(({const char*_tmp25E="(";_tag_dyneither(_tmp25E,sizeof(char),2);}));_tmp25D[0]=_tmp6BB;});Cyc_PP_cat(_tag_dyneither(_tmp25D,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp6BC;});
# 1207
goto _LL201;case 26: _LL23C: _tmp2BD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL23D:
# 1209
({struct Cyc_PP_Doc*_tmp6BE=({struct _dyneither_ptr _tmp6BD=({const char*_tmp261=",";_tag_dyneither(_tmp261,sizeof(char),2);});Cyc_Absynpp_group_braces(_tmp6BD,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BD));});s=_tmp6BE;});
goto _LL201;case 27: _LL23E: _tmp2C0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2BE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL23F:
# 1212
({struct Cyc_PP_Doc*_tmp6C7=({struct Cyc_PP_Doc*_tmp262[8];({struct Cyc_PP_Doc*_tmp6BF=
# 1218
Cyc_Absynpp_rb();_tmp262[7]=_tmp6BF;});({struct Cyc_PP_Doc*_tmp6C0=
# 1217
Cyc_Absynpp_exp2doc(_tmp2BE);_tmp262[6]=_tmp6C0;});({struct Cyc_PP_Doc*_tmp6C1=
# 1216
Cyc_PP_text(({const char*_tmp265=" : ";_tag_dyneither(_tmp265,sizeof(char),4);}));_tmp262[5]=_tmp6C1;});({struct Cyc_PP_Doc*_tmp6C2=
# 1215
Cyc_Absynpp_exp2doc(_tmp2BF);_tmp262[4]=_tmp6C2;});({struct Cyc_PP_Doc*_tmp6C3=
# 1214
Cyc_PP_text(({const char*_tmp264=" < ";_tag_dyneither(_tmp264,sizeof(char),4);}));_tmp262[3]=_tmp6C3;});({struct Cyc_PP_Doc*_tmp6C4=
# 1213
Cyc_PP_text(*(*_tmp2C0->name).f2);_tmp262[2]=_tmp6C4;});({struct Cyc_PP_Doc*_tmp6C5=
# 1212
Cyc_PP_text(({const char*_tmp263="for ";_tag_dyneither(_tmp263,sizeof(char),5);}));_tmp262[1]=_tmp6C5;});({struct Cyc_PP_Doc*_tmp6C6=Cyc_Absynpp_lb();_tmp262[0]=_tmp6C6;});Cyc_PP_cat(_tag_dyneither(_tmp262,sizeof(struct Cyc_PP_Doc*),8));});s=_tmp6C7;});
# 1219
goto _LL201;case 28: _LL240: _tmp2C2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C1=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL241:
# 1221
({struct Cyc_PP_Doc*_tmp6CF=({struct Cyc_PP_Doc*_tmp266[7];({struct Cyc_PP_Doc*_tmp6C8=
# 1226
Cyc_Absynpp_rb();_tmp266[6]=_tmp6C8;});({struct Cyc_PP_Doc*_tmp6C9=
# 1225
Cyc_Absynpp_typ2doc(_tmp2C1);_tmp266[5]=_tmp6C9;});({struct Cyc_PP_Doc*_tmp6CA=
# 1224
Cyc_PP_text(({const char*_tmp269=" : ";_tag_dyneither(_tmp269,sizeof(char),4);}));_tmp266[4]=_tmp6CA;});({struct Cyc_PP_Doc*_tmp6CB=
# 1223
Cyc_Absynpp_exp2doc(_tmp2C2);_tmp266[3]=_tmp6CB;});({struct Cyc_PP_Doc*_tmp6CC=
# 1222
Cyc_PP_text(({const char*_tmp268=" < ";_tag_dyneither(_tmp268,sizeof(char),4);}));_tmp266[2]=_tmp6CC;});({struct Cyc_PP_Doc*_tmp6CD=
# 1221
Cyc_PP_text(({const char*_tmp267="for x ";_tag_dyneither(_tmp267,sizeof(char),7);}));_tmp266[1]=_tmp6CD;});({struct Cyc_PP_Doc*_tmp6CE=Cyc_Absynpp_lb();_tmp266[0]=_tmp6CE;});Cyc_PP_cat(_tag_dyneither(_tmp266,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp6CF;});
# 1227
goto _LL201;case 29: _LL242: _tmp2C5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2C3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL243: {
# 1229
struct Cyc_List_List*_tmp26A=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C3);
({struct Cyc_PP_Doc*_tmp6D4=({struct Cyc_PP_Doc*_tmp26B[2];({struct Cyc_PP_Doc*_tmp6D2=({
struct _dyneither_ptr _tmp6D1=({const char*_tmp26C=",";_tag_dyneither(_tmp26C,sizeof(char),2);});Cyc_Absynpp_group_braces(_tmp6D1,
_tmp2C4 != 0?({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));({struct Cyc_PP_Doc*_tmp6D0=Cyc_Absynpp_tps2doc(_tmp2C4);_tmp26D->hd=_tmp6D0;});_tmp26D->tl=_tmp26A;_tmp26D;}): _tmp26A);});_tmp26B[1]=_tmp6D2;});({struct Cyc_PP_Doc*_tmp6D3=
# 1230
Cyc_Absynpp_qvar2doc(_tmp2C5);_tmp26B[0]=_tmp6D3;});Cyc_PP_cat(_tag_dyneither(_tmp26B,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp6D4;});
# 1233
goto _LL201;}case 30: _LL244: _tmp2C6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL245:
# 1235
({struct Cyc_PP_Doc*_tmp6D6=({struct _dyneither_ptr _tmp6D5=({const char*_tmp26E=",";_tag_dyneither(_tmp26E,sizeof(char),2);});Cyc_Absynpp_group_braces(_tmp6D5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C6));});s=_tmp6D6;});
goto _LL201;case 31: _LL246: _tmp2C8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL247:
# 1238
 if(_tmp2C8 == 0)
# 1240
({struct Cyc_PP_Doc*_tmp6D7=Cyc_Absynpp_qvar2doc(_tmp2C7->name);s=_tmp6D7;});else{
# 1242
({struct Cyc_PP_Doc*_tmp6DD=({struct Cyc_PP_Doc*_tmp26F[2];({struct Cyc_PP_Doc*_tmp6DB=({
struct _dyneither_ptr _tmp6DA=({const char*_tmp270="(";_tag_dyneither(_tmp270,sizeof(char),2);});struct _dyneither_ptr _tmp6D9=({const char*_tmp271=")";_tag_dyneither(_tmp271,sizeof(char),2);});struct _dyneither_ptr _tmp6D8=({const char*_tmp272=",";_tag_dyneither(_tmp272,sizeof(char),2);});Cyc_PP_egroup(_tmp6DA,_tmp6D9,_tmp6D8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C8));});_tmp26F[1]=_tmp6DB;});({struct Cyc_PP_Doc*_tmp6DC=
# 1242
Cyc_Absynpp_qvar2doc(_tmp2C7->name);_tmp26F[0]=_tmp6DC;});Cyc_PP_cat(_tag_dyneither(_tmp26F,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp6DD;});}
# 1244
goto _LL201;case 32: _LL248: _tmp2C9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL249:
# 1246
({struct Cyc_PP_Doc*_tmp6DE=Cyc_Absynpp_qvar2doc(_tmp2C9->name);s=_tmp6DE;});
goto _LL201;case 33: _LL24A: _tmp2CA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24B:
# 1249
({struct Cyc_PP_Doc*_tmp6DF=Cyc_Absynpp_qvar2doc(_tmp2CA->name);s=_tmp6DF;});
goto _LL201;case 34: _LL24C: _tmp2CF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp2CE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp2CD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp2CC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp2CB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL24D:
# 1254
 if(_tmp2CF){
# 1256
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2CD)),0);
if(_tmp2CE == 0)
({struct Cyc_PP_Doc*_tmp6E5=({struct Cyc_PP_Doc*_tmp273[5];({struct Cyc_PP_Doc*_tmp6E0=Cyc_PP_text(({const char*_tmp276=")";_tag_dyneither(_tmp276,sizeof(char),2);}));_tmp273[4]=_tmp6E0;});({struct Cyc_PP_Doc*_tmp6E1=Cyc_Absynpp_exp2doc(st);_tmp273[3]=_tmp6E1;});({struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_text(({const char*_tmp275=",";_tag_dyneither(_tmp275,sizeof(char),2);}));_tmp273[2]=_tmp6E2;});({struct Cyc_PP_Doc*_tmp6E3=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp273[1]=_tmp6E3;});({struct Cyc_PP_Doc*_tmp6E4=Cyc_PP_text(({const char*_tmp274="calloc(";_tag_dyneither(_tmp274,sizeof(char),8);}));_tmp273[0]=_tmp6E4;});Cyc_PP_cat(_tag_dyneither(_tmp273,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp6E5;});else{
# 1260
({struct Cyc_PP_Doc*_tmp6ED=({struct Cyc_PP_Doc*_tmp277[7];({struct Cyc_PP_Doc*_tmp6E6=
Cyc_PP_text(({const char*_tmp27B=")";_tag_dyneither(_tmp27B,sizeof(char),2);}));_tmp277[6]=_tmp6E6;});({struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_exp2doc(st);_tmp277[5]=_tmp6E7;});({struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_text(({const char*_tmp27A=",";_tag_dyneither(_tmp27A,sizeof(char),2);}));_tmp277[4]=_tmp6E8;});({struct Cyc_PP_Doc*_tmp6E9=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp277[3]=_tmp6E9;});({struct Cyc_PP_Doc*_tmp6EA=
# 1260
Cyc_PP_text(({const char*_tmp279=",";_tag_dyneither(_tmp279,sizeof(char),2);}));_tmp277[2]=_tmp6EA;});({struct Cyc_PP_Doc*_tmp6EB=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp277[1]=_tmp6EB;});({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp278="rcalloc(";_tag_dyneither(_tmp278,sizeof(char),9);}));_tmp277[0]=_tmp6EC;});Cyc_PP_cat(_tag_dyneither(_tmp277,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp6ED;});}}else{
# 1263
struct Cyc_Absyn_Exp*new_e;
# 1265
if(_tmp2CD == 0)
new_e=_tmp2CC;else{
# 1268
({struct Cyc_Absyn_Exp*_tmp6EF=({struct Cyc_Absyn_Exp*_tmp6EE=Cyc_Absyn_sizeoftyp_exp(*_tmp2CD,0);Cyc_Absyn_times_exp(_tmp6EE,_tmp2CC,0);});new_e=_tmp6EF;});}
# 1270
if(_tmp2CE == 0)
({struct Cyc_PP_Doc*_tmp6F3=({struct Cyc_PP_Doc*_tmp27C[3];({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_text(({const char*_tmp27E=")";_tag_dyneither(_tmp27E,sizeof(char),2);}));_tmp27C[2]=_tmp6F0;});({struct Cyc_PP_Doc*_tmp6F1=Cyc_Absynpp_exp2doc(new_e);_tmp27C[1]=_tmp6F1;});({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp27D="malloc(";_tag_dyneither(_tmp27D,sizeof(char),8);}));_tmp27C[0]=_tmp6F2;});Cyc_PP_cat(_tag_dyneither(_tmp27C,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp6F3;});else{
# 1273
if(_tmp2CB)
({struct Cyc_PP_Doc*_tmp6F9=({struct Cyc_PP_Doc*_tmp27F[5];({struct Cyc_PP_Doc*_tmp6F4=
Cyc_PP_text(({const char*_tmp282=")";_tag_dyneither(_tmp282,sizeof(char),2);}));_tmp27F[4]=_tmp6F4;});({struct Cyc_PP_Doc*_tmp6F5=Cyc_Absynpp_exp2doc(new_e);_tmp27F[3]=_tmp6F5;});({struct Cyc_PP_Doc*_tmp6F6=
# 1274
Cyc_PP_text(({const char*_tmp281=",";_tag_dyneither(_tmp281,sizeof(char),2);}));_tmp27F[2]=_tmp6F6;});({struct Cyc_PP_Doc*_tmp6F7=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp27F[1]=_tmp6F7;});({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_text(({const char*_tmp280="rmalloc_inline(";_tag_dyneither(_tmp280,sizeof(char),16);}));_tmp27F[0]=_tmp6F8;});Cyc_PP_cat(_tag_dyneither(_tmp27F,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp6F9;});else{
# 1277
({struct Cyc_PP_Doc*_tmp6FF=({struct Cyc_PP_Doc*_tmp283[5];({struct Cyc_PP_Doc*_tmp6FA=
Cyc_PP_text(({const char*_tmp286=")";_tag_dyneither(_tmp286,sizeof(char),2);}));_tmp283[4]=_tmp6FA;});({struct Cyc_PP_Doc*_tmp6FB=Cyc_Absynpp_exp2doc(new_e);_tmp283[3]=_tmp6FB;});({struct Cyc_PP_Doc*_tmp6FC=
# 1277
Cyc_PP_text(({const char*_tmp285=",";_tag_dyneither(_tmp285,sizeof(char),2);}));_tmp283[2]=_tmp6FC;});({struct Cyc_PP_Doc*_tmp6FD=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp283[1]=_tmp6FD;});({struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_text(({const char*_tmp284="rmalloc(";_tag_dyneither(_tmp284,sizeof(char),9);}));_tmp283[0]=_tmp6FE;});Cyc_PP_cat(_tag_dyneither(_tmp283,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp6FF;});}}}
# 1281
goto _LL201;case 35: _LL24E: _tmp2D1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24F:
# 1283
({struct Cyc_PP_Doc*_tmp703=({struct Cyc_PP_Doc*_tmp287[3];({struct Cyc_PP_Doc*_tmp700=
# 1285
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D0);_tmp287[2]=_tmp700;});({struct Cyc_PP_Doc*_tmp701=
# 1284
Cyc_PP_text(({const char*_tmp288=" :=: ";_tag_dyneither(_tmp288,sizeof(char),6);}));_tmp287[1]=_tmp701;});({struct Cyc_PP_Doc*_tmp702=
# 1283
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D1);_tmp287[0]=_tmp702;});Cyc_PP_cat(_tag_dyneither(_tmp287,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp703;});
# 1286
goto _LL201;case 36: _LL250: _tmp2D3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL251:
# 1289
({struct Cyc_PP_Doc*_tmp705=({struct _dyneither_ptr _tmp704=({const char*_tmp289=",";_tag_dyneither(_tmp289,sizeof(char),2);});Cyc_Absynpp_group_braces(_tmp704,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D2));});s=_tmp705;});
goto _LL201;default: _LL252: _tmp2D4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL253:
# 1292
({struct Cyc_PP_Doc*_tmp70D=({struct Cyc_PP_Doc*_tmp28A[7];({struct Cyc_PP_Doc*_tmp706=
# 1294
Cyc_PP_text(({const char*_tmp28C=")";_tag_dyneither(_tmp28C,sizeof(char),2);}));_tmp28A[6]=_tmp706;});({struct Cyc_PP_Doc*_tmp707=Cyc_Absynpp_rb();_tmp28A[5]=_tmp707;});({struct Cyc_PP_Doc*_tmp708=Cyc_PP_blank_doc();_tmp28A[4]=_tmp708;});({struct Cyc_PP_Doc*_tmp709=
# 1293
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D4,1));_tmp28A[3]=_tmp709;});({struct Cyc_PP_Doc*_tmp70A=
# 1292
Cyc_PP_blank_doc();_tmp28A[2]=_tmp70A;});({struct Cyc_PP_Doc*_tmp70B=Cyc_Absynpp_lb();_tmp28A[1]=_tmp70B;});({struct Cyc_PP_Doc*_tmp70C=Cyc_PP_text(({const char*_tmp28B="(";_tag_dyneither(_tmp28B,sizeof(char),2);}));_tmp28A[0]=_tmp70C;});Cyc_PP_cat(_tag_dyneither(_tmp28A,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp70D;});
# 1295
goto _LL201;}_LL201:;}
# 1297
if(inprec >= myprec)
({struct Cyc_PP_Doc*_tmp710=({struct Cyc_PP_Doc*_tmp2D5[3];({struct Cyc_PP_Doc*_tmp70E=Cyc_PP_text(({const char*_tmp2D7=")";_tag_dyneither(_tmp2D7,sizeof(char),2);}));_tmp2D5[2]=_tmp70E;});_tmp2D5[1]=s;({struct Cyc_PP_Doc*_tmp70F=Cyc_PP_text(({const char*_tmp2D6="(";_tag_dyneither(_tmp2D6,sizeof(char),2);}));_tmp2D5[0]=_tmp70F;});Cyc_PP_cat(_tag_dyneither(_tmp2D5,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp710;});
return s;}
# 1302
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2D8=d;struct _dyneither_ptr*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->tag == 0){_LL263: _tmp2DE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->f1;_LL264:
 return({struct Cyc_PP_Doc*_tmp2D9[3];({struct Cyc_PP_Doc*_tmp711=Cyc_PP_text(({const char*_tmp2DB="]";_tag_dyneither(_tmp2DB,sizeof(char),2);}));_tmp2D9[2]=_tmp711;});({struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_exp2doc(_tmp2DE);_tmp2D9[1]=_tmp712;});({struct Cyc_PP_Doc*_tmp713=Cyc_PP_text(({const char*_tmp2DA=".[";_tag_dyneither(_tmp2DA,sizeof(char),3);}));_tmp2D9[0]=_tmp713;});Cyc_PP_cat(_tag_dyneither(_tmp2D9,sizeof(struct Cyc_PP_Doc*),3));});}else{_LL265: _tmp2DF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D8)->f1;_LL266:
 return({struct Cyc_PP_Doc*_tmp2DC[2];({struct Cyc_PP_Doc*_tmp714=Cyc_PP_textptr(_tmp2DF);_tmp2DC[1]=_tmp714;});({struct Cyc_PP_Doc*_tmp715=Cyc_PP_text(({const char*_tmp2DD=".";_tag_dyneither(_tmp2DD,sizeof(char),2);}));_tmp2DC[0]=_tmp715;});Cyc_PP_cat(_tag_dyneither(_tmp2DC,sizeof(struct Cyc_PP_Doc*),2));});}_LL262:;}
# 1309
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2E0[2];({struct Cyc_PP_Doc*_tmp716=
Cyc_Absynpp_exp2doc((*de).f2);_tmp2E0[1]=_tmp716;});({struct Cyc_PP_Doc*_tmp71A=({
# 1311
struct _dyneither_ptr _tmp719=({const char*_tmp2E1="";_tag_dyneither(_tmp2E1,sizeof(char),1);});struct _dyneither_ptr _tmp718=({const char*_tmp2E2="=";_tag_dyneither(_tmp2E2,sizeof(char),2);});struct _dyneither_ptr _tmp717=({const char*_tmp2E3="=";_tag_dyneither(_tmp2E3,sizeof(char),2);});Cyc_PP_egroup(_tmp719,_tmp718,_tmp717,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2E0[0]=_tmp71A;});Cyc_PP_cat(_tag_dyneither(_tmp2E0,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1315
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _dyneither_ptr _tmp71D=({const char*_tmp2E4="";_tag_dyneither(_tmp2E4,sizeof(char),1);});struct _dyneither_ptr _tmp71C=({const char*_tmp2E5="";_tag_dyneither(_tmp2E5,sizeof(char),1);});struct _dyneither_ptr _tmp71B=({const char*_tmp2E6=",";_tag_dyneither(_tmp2E6,sizeof(char),2);});Cyc_PP_group(_tmp71D,_tmp71C,_tmp71B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1319
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp2FC=({unsigned int _tmp2F9=(unsigned int)28;char*_tmp2FA=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp2F9 + 1));{unsigned int _tmp2FB=_tmp2F9;unsigned int i;for(i=0;i < _tmp2FB;i ++){_tmp2FA[i]=(char)'z';}_tmp2FA[_tmp2FB]=(char)0;}_tmp2FA;});_tag_dyneither(_tmp2FC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FC,(unsigned int)28 + 1));});
({struct _dyneither_ptr _tmp2E7=_dyneither_ptr_plus(x,sizeof(char),27);char _tmp2E8=*((char*)_check_dyneither_subscript(_tmp2E7,sizeof(char),0));char _tmp2E9='\000';if(_get_dyneither_size(_tmp2E7,sizeof(char))== 1  && (_tmp2E8 == '\000'  && _tmp2E9 != '\000'))_throw_arraybounds();*((char*)_tmp2E7.curr)=_tmp2E9;});
({struct _dyneither_ptr _tmp2EA=_dyneither_ptr_plus(x,sizeof(char),26);char _tmp2EB=*((char*)_check_dyneither_subscript(_tmp2EA,sizeof(char),0));char _tmp2EC='L';if(_get_dyneither_size(_tmp2EA,sizeof(char))== 1  && (_tmp2EB == '\000'  && _tmp2EC != '\000'))_throw_arraybounds();*((char*)_tmp2EA.curr)=_tmp2EC;});
({struct _dyneither_ptr _tmp2ED=_dyneither_ptr_plus(x,sizeof(char),25);char _tmp2EE=*((char*)_check_dyneither_subscript(_tmp2ED,sizeof(char),0));char _tmp2EF='L';if(_get_dyneither_size(_tmp2ED,sizeof(char))== 1  && (_tmp2EE == '\000'  && _tmp2EF != '\000'))_throw_arraybounds();*((char*)_tmp2ED.curr)=_tmp2EF;});
({struct _dyneither_ptr _tmp2F0=_dyneither_ptr_plus(x,sizeof(char),24);char _tmp2F1=*((char*)_check_dyneither_subscript(_tmp2F0,sizeof(char),0));char _tmp2F2='U';if(_get_dyneither_size(_tmp2F0,sizeof(char))== 1  && (_tmp2F1 == '\000'  && _tmp2F2 != '\000'))_throw_arraybounds();*((char*)_tmp2F0.curr)=_tmp2F2;});
({struct _dyneither_ptr _tmp2F3=_dyneither_ptr_plus(x,sizeof(char),23);char _tmp2F4=*((char*)_check_dyneither_subscript(_tmp2F3,sizeof(char),0));char _tmp2F5='0';if(_get_dyneither_size(_tmp2F3,sizeof(char))== 1  && (_tmp2F4 == '\000'  && _tmp2F5 != '\000'))_throw_arraybounds();*((char*)_tmp2F3.curr)=_tmp2F5;});{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
({struct _dyneither_ptr _tmp2F6=_dyneither_ptr_plus(x,sizeof(char),index);char _tmp2F7=*((char*)_check_dyneither_subscript(_tmp2F6,sizeof(char),0));char _tmp2F8=c;if(_get_dyneither_size(_tmp2F6,sizeof(char))== 1  && (_tmp2F7 == '\000'  && _tmp2F8 != '\000'))_throw_arraybounds();*((char*)_tmp2F6.curr)=_tmp2F8;});
i=i / 10;
-- index;}
# 1333
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1337
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2FD=c;struct _dyneither_ptr _tmp320;struct _dyneither_ptr _tmp31F;struct _dyneither_ptr _tmp31E;enum Cyc_Absyn_Sign _tmp31D;long long _tmp31C;int _tmp31B;int _tmp31A;int _tmp319;enum Cyc_Absyn_Sign _tmp318;short _tmp317;struct _dyneither_ptr _tmp316;enum Cyc_Absyn_Sign _tmp315;char _tmp314;switch((_tmp2FD.String_c).tag){case 2: _LL268: _tmp315=((_tmp2FD.Char_c).val).f1;_tmp314=((_tmp2FD.Char_c).val).f2;_LL269:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp300;_tmp300.tag=0;({struct _dyneither_ptr _tmp71E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp314));_tmp300.f1=_tmp71E;});({void*_tmp2FE[1]={& _tmp300};({struct _dyneither_ptr _tmp71F=({const char*_tmp2FF="'%s'";_tag_dyneither(_tmp2FF,sizeof(char),5);});Cyc_aprintf(_tmp71F,_tag_dyneither(_tmp2FE,sizeof(void*),1));});});}));case 3: _LL26A: _tmp316=(_tmp2FD.Wchar_c).val;_LL26B:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp303;_tmp303.tag=0;_tmp303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp316);({void*_tmp301[1]={& _tmp303};({struct _dyneither_ptr _tmp720=({const char*_tmp302="L'%s'";_tag_dyneither(_tmp302,sizeof(char),6);});Cyc_aprintf(_tmp720,_tag_dyneither(_tmp301,sizeof(void*),1));});});}));case 4: _LL26C: _tmp318=((_tmp2FD.Short_c).val).f1;_tmp317=((_tmp2FD.Short_c).val).f2;_LL26D:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp306;_tmp306.tag=1;_tmp306.f1=(unsigned long)((int)_tmp317);({void*_tmp304[1]={& _tmp306};({struct _dyneither_ptr _tmp721=({const char*_tmp305="%d";_tag_dyneither(_tmp305,sizeof(char),3);});Cyc_aprintf(_tmp721,_tag_dyneither(_tmp304,sizeof(void*),1));});});}));case 5: switch(((_tmp2FD.Int_c).val).f1){case Cyc_Absyn_None: _LL26E: _tmp319=((_tmp2FD.Int_c).val).f2;_LL26F:
 _tmp31A=_tmp319;goto _LL271;case Cyc_Absyn_Signed: _LL270: _tmp31A=((_tmp2FD.Int_c).val).f2;_LL271:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp309;_tmp309.tag=1;_tmp309.f1=(unsigned long)_tmp31A;({void*_tmp307[1]={& _tmp309};({struct _dyneither_ptr _tmp722=({const char*_tmp308="%d";_tag_dyneither(_tmp308,sizeof(char),3);});Cyc_aprintf(_tmp722,_tag_dyneither(_tmp307,sizeof(void*),1));});});}));default: _LL272: _tmp31B=((_tmp2FD.Int_c).val).f2;_LL273:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30C;_tmp30C.tag=1;_tmp30C.f1=(unsigned int)_tmp31B;({void*_tmp30A[1]={& _tmp30C};({struct _dyneither_ptr _tmp723=({const char*_tmp30B="%u";_tag_dyneither(_tmp30B,sizeof(char),3);});Cyc_aprintf(_tmp723,_tag_dyneither(_tmp30A,sizeof(void*),1));});});}));}case 6: _LL274: _tmp31D=((_tmp2FD.LongLong_c).val).f1;_tmp31C=((_tmp2FD.LongLong_c).val).f2;_LL275:
# 1347
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp31C));case 7: _LL276: _tmp31E=((_tmp2FD.Float_c).val).f1;_LL277:
 return Cyc_PP_text(_tmp31E);case 1: _LL278: _LL279:
 return Cyc_PP_text(({const char*_tmp30D="NULL";_tag_dyneither(_tmp30D,sizeof(char),5);}));case 8: _LL27A: _tmp31F=(_tmp2FD.String_c).val;_LL27B:
# 1351
 return({struct Cyc_PP_Doc*_tmp30E[3];({struct Cyc_PP_Doc*_tmp724=Cyc_PP_text(({const char*_tmp310="\"";_tag_dyneither(_tmp310,sizeof(char),2);}));_tmp30E[2]=_tmp724;});({struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp31F));_tmp30E[1]=_tmp725;});({struct Cyc_PP_Doc*_tmp726=Cyc_PP_text(({const char*_tmp30F="\"";_tag_dyneither(_tmp30F,sizeof(char),2);}));_tmp30E[0]=_tmp726;});Cyc_PP_cat(_tag_dyneither(_tmp30E,sizeof(struct Cyc_PP_Doc*),3));});default: _LL27C: _tmp320=(_tmp2FD.Wstring_c).val;_LL27D:
# 1353
 return({struct Cyc_PP_Doc*_tmp311[3];({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp313="\"";_tag_dyneither(_tmp313,sizeof(char),2);}));_tmp311[2]=_tmp727;});({struct Cyc_PP_Doc*_tmp728=Cyc_PP_text(_tmp320);_tmp311[1]=_tmp728;});({struct Cyc_PP_Doc*_tmp729=Cyc_PP_text(({const char*_tmp312="L\"";_tag_dyneither(_tmp312,sizeof(char),3);}));_tmp311[0]=_tmp729;});Cyc_PP_cat(_tag_dyneither(_tmp311,sizeof(struct Cyc_PP_Doc*),3));});}_LL267:;}
# 1357
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp321=_cycalloc(sizeof(*_tmp321));({struct Cyc_Core_Failure_exn_struct _tmp72D=({struct Cyc_Core_Failure_exn_struct _tmp322;_tmp322.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp72C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp325;_tmp325.tag=0;({struct _dyneither_ptr _tmp72A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp325.f1=_tmp72A;});({void*_tmp323[1]={& _tmp325};({struct _dyneither_ptr _tmp72B=({const char*_tmp324="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp324,sizeof(char),47);});Cyc_aprintf(_tmp72B,_tag_dyneither(_tmp323,sizeof(void*),1));});});});_tmp322.f1=_tmp72C;});_tmp322;});_tmp321[0]=_tmp72D;});_tmp321;}));
# 1363
return({struct Cyc_PP_Doc*_tmp326[3];({struct Cyc_PP_Doc*_tmp72E=Cyc_PP_text(({const char*_tmp328=")";_tag_dyneither(_tmp328,sizeof(char),2);}));_tmp326[2]=_tmp72E;});({struct Cyc_PP_Doc*_tmp72F=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp326[1]=_tmp72F;});({struct Cyc_PP_Doc*_tmp730=Cyc_PP_text(({const char*_tmp327="numelts(";_tag_dyneither(_tmp327,sizeof(char),9);}));_tmp326[0]=_tmp730;});Cyc_PP_cat(_tag_dyneither(_tmp326,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1365
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp329=_cycalloc(sizeof(*_tmp329));({struct Cyc_Core_Failure_exn_struct _tmp734=({struct Cyc_Core_Failure_exn_struct _tmp32A;_tmp32A.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp733=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32D;_tmp32D.tag=0;({struct _dyneither_ptr _tmp731=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp32D.f1=_tmp731;});({void*_tmp32B[1]={& _tmp32D};({struct _dyneither_ptr _tmp732=({const char*_tmp32C="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp32C,sizeof(char),38);});Cyc_aprintf(_tmp732,_tag_dyneither(_tmp32B,sizeof(void*),1));});});});_tmp32A.f1=_tmp733;});_tmp32A;});_tmp329[0]=_tmp734;});_tmp329;}));else{
# 1369
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp32E[3];_tmp32E[2]=(struct Cyc_PP_Doc*)ds->hd;({struct Cyc_PP_Doc*_tmp735=Cyc_PP_text(({const char*_tmp32F=" ";_tag_dyneither(_tmp32F,sizeof(char),2);}));_tmp32E[1]=_tmp735;});_tmp32E[0]=ps;Cyc_PP_cat(_tag_dyneither(_tmp32E,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp330=_cycalloc(sizeof(*_tmp330));({struct Cyc_Core_Failure_exn_struct _tmp739=({struct Cyc_Core_Failure_exn_struct _tmp331;_tmp331.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmp738=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp334;_tmp334.tag=0;({struct _dyneither_ptr _tmp736=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp334.f1=_tmp736;});({void*_tmp332[1]={& _tmp334};({struct _dyneither_ptr _tmp737=({const char*_tmp333="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp333,sizeof(char),47);});Cyc_aprintf(_tmp737,_tag_dyneither(_tmp332,sizeof(void*),1));});});});_tmp331.f1=_tmp738;});_tmp331;});_tmp330[0]=_tmp739;});_tmp330;}));else{
# 1375
return({struct Cyc_PP_Doc*_tmp335[5];_tmp335[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;({struct Cyc_PP_Doc*_tmp73A=Cyc_PP_text(({const char*_tmp337=" ";_tag_dyneither(_tmp337,sizeof(char),2);}));_tmp335[3]=_tmp73A;});_tmp335[2]=ps;({struct Cyc_PP_Doc*_tmp73B=Cyc_PP_text(({const char*_tmp336=" ";_tag_dyneither(_tmp336,sizeof(char),2);}));_tmp335[1]=_tmp73B;});_tmp335[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp335,sizeof(struct Cyc_PP_Doc*),5));});}}}}}
# 1379
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp338=p;switch(_tmp338){case Cyc_Absyn_Plus: _LL27F: _LL280:
 return({const char*_tmp339="+";_tag_dyneither(_tmp339,sizeof(char),2);});case Cyc_Absyn_Times: _LL281: _LL282:
 return({const char*_tmp33A="*";_tag_dyneither(_tmp33A,sizeof(char),2);});case Cyc_Absyn_Minus: _LL283: _LL284:
 return({const char*_tmp33B="-";_tag_dyneither(_tmp33B,sizeof(char),2);});case Cyc_Absyn_Div: _LL285: _LL286:
 return({const char*_tmp33C="/";_tag_dyneither(_tmp33C,sizeof(char),2);});case Cyc_Absyn_Mod: _LL287: _LL288:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp33D="\\%";_tag_dyneither(_tmp33D,sizeof(char),3);}):({const char*_tmp33E="%";_tag_dyneither(_tmp33E,sizeof(char),2);});case Cyc_Absyn_Eq: _LL289: _LL28A:
 return({const char*_tmp33F="==";_tag_dyneither(_tmp33F,sizeof(char),3);});case Cyc_Absyn_Neq: _LL28B: _LL28C:
 return({const char*_tmp340="!=";_tag_dyneither(_tmp340,sizeof(char),3);});case Cyc_Absyn_Gt: _LL28D: _LL28E:
 return({const char*_tmp341=">";_tag_dyneither(_tmp341,sizeof(char),2);});case Cyc_Absyn_Lt: _LL28F: _LL290:
 return({const char*_tmp342="<";_tag_dyneither(_tmp342,sizeof(char),2);});case Cyc_Absyn_Gte: _LL291: _LL292:
 return({const char*_tmp343=">=";_tag_dyneither(_tmp343,sizeof(char),3);});case Cyc_Absyn_Lte: _LL293: _LL294:
 return({const char*_tmp344="<=";_tag_dyneither(_tmp344,sizeof(char),3);});case Cyc_Absyn_Not: _LL295: _LL296:
 return({const char*_tmp345="!";_tag_dyneither(_tmp345,sizeof(char),2);});case Cyc_Absyn_Bitnot: _LL297: _LL298:
 return({const char*_tmp346="~";_tag_dyneither(_tmp346,sizeof(char),2);});case Cyc_Absyn_Bitand: _LL299: _LL29A:
 return({const char*_tmp347="&";_tag_dyneither(_tmp347,sizeof(char),2);});case Cyc_Absyn_Bitor: _LL29B: _LL29C:
 return({const char*_tmp348="|";_tag_dyneither(_tmp348,sizeof(char),2);});case Cyc_Absyn_Bitxor: _LL29D: _LL29E:
 return({const char*_tmp349="^";_tag_dyneither(_tmp349,sizeof(char),2);});case Cyc_Absyn_Bitlshift: _LL29F: _LL2A0:
 return({const char*_tmp34A="<<";_tag_dyneither(_tmp34A,sizeof(char),3);});case Cyc_Absyn_Bitlrshift: _LL2A1: _LL2A2:
 return({const char*_tmp34B=">>";_tag_dyneither(_tmp34B,sizeof(char),3);});case Cyc_Absyn_Bitarshift: _LL2A3: _LL2A4:
 return({const char*_tmp34C=">>>";_tag_dyneither(_tmp34C,sizeof(char),4);});default: _LL2A5: _LL2A6:
 return({const char*_tmp34D="numelts";_tag_dyneither(_tmp34D,sizeof(char),8);});}_LL27E:;}
# 1404
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1408
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp34E=s->r;void*_tmp34F=_tmp34E;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34F)->tag == 12){_LL2A8: _LL2A9:
 return 1;}else{_LL2AA: _LL2AB:
 return 0;}_LL2A7:;}
# 1415
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp350=st->r;void*_tmp351=_tmp350;struct Cyc_Absyn_Stmt*_tmp3AB;struct Cyc_List_List*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3A8;struct _dyneither_ptr*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_Absyn_Decl*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A4;struct Cyc_List_List*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A2;struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Exp*_tmp39F;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Stmt*_tmp39D;struct _dyneither_ptr*_tmp39C;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Stmt*_tmp39A;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Stmt*_tmp398;struct Cyc_Absyn_Stmt*_tmp397;struct Cyc_Absyn_Exp*_tmp396;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Stmt*_tmp394;struct Cyc_Absyn_Exp*_tmp393;switch(*((int*)_tmp351)){case 0: _LL2AD: _LL2AE:
({struct Cyc_PP_Doc*_tmp73C=Cyc_PP_text(({const char*_tmp352=";";_tag_dyneither(_tmp352,sizeof(char),2);}));s=_tmp73C;});goto _LL2AC;case 1: _LL2AF: _tmp393=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_LL2B0:
({struct Cyc_PP_Doc*_tmp73F=({struct Cyc_PP_Doc*_tmp353[2];({struct Cyc_PP_Doc*_tmp73D=Cyc_PP_text(({const char*_tmp354=";";_tag_dyneither(_tmp354,sizeof(char),2);}));_tmp353[1]=_tmp73D;});({struct Cyc_PP_Doc*_tmp73E=Cyc_Absynpp_exp2doc(_tmp393);_tmp353[0]=_tmp73E;});Cyc_PP_cat(_tag_dyneither(_tmp353,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp73F;});goto _LL2AC;case 2: _LL2B1: _tmp395=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp394=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_LL2B2:
# 1421
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp395))
({struct Cyc_PP_Doc*_tmp753=({struct Cyc_PP_Doc*_tmp355[7];({struct Cyc_PP_Doc*_tmp74C=
# 1429
Cyc_Absynpp_is_declaration(_tmp394)?
stmtexp?({struct Cyc_PP_Doc*_tmp356[7];({struct Cyc_PP_Doc*_tmp745=
# 1434
Cyc_PP_line_doc();_tmp356[6]=_tmp745;});({struct Cyc_PP_Doc*_tmp746=
# 1433
Cyc_PP_text(({const char*_tmp358=");";_tag_dyneither(_tmp358,sizeof(char),3);}));_tmp356[5]=_tmp746;});({struct Cyc_PP_Doc*_tmp747=Cyc_Absynpp_rb();_tmp356[4]=_tmp747;});({struct Cyc_PP_Doc*_tmp748=
# 1432
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp394,stmtexp));_tmp356[3]=_tmp748;});({struct Cyc_PP_Doc*_tmp749=
# 1431
Cyc_PP_blank_doc();_tmp356[2]=_tmp749;});({struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_lb();_tmp356[1]=_tmp74A;});({struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp357="(";_tag_dyneither(_tmp357,sizeof(char),2);}));_tmp356[0]=_tmp74B;});Cyc_PP_cat(_tag_dyneither(_tmp356,sizeof(struct Cyc_PP_Doc*),7));}):({struct Cyc_PP_Doc*_tmp359[5];({struct Cyc_PP_Doc*_tmp740=
# 1438
Cyc_PP_line_doc();_tmp359[4]=_tmp740;});({struct Cyc_PP_Doc*_tmp741=
# 1437
Cyc_Absynpp_rb();_tmp359[3]=_tmp741;});({struct Cyc_PP_Doc*_tmp742=
# 1436
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp394,stmtexp));_tmp359[2]=_tmp742;});({struct Cyc_PP_Doc*_tmp743=
# 1435
Cyc_PP_blank_doc();_tmp359[1]=_tmp743;});({struct Cyc_PP_Doc*_tmp744=Cyc_Absynpp_lb();_tmp359[0]=_tmp744;});Cyc_PP_cat(_tag_dyneither(_tmp359,sizeof(struct Cyc_PP_Doc*),5));}):
# 1439
 Cyc_Absynpp_stmt2doc(_tmp394,stmtexp);_tmp355[6]=_tmp74C;});({struct Cyc_PP_Doc*_tmp74D=
# 1428
Cyc_PP_line_doc();_tmp355[5]=_tmp74D;});({struct Cyc_PP_Doc*_tmp74E=
# 1427
Cyc_Absynpp_rb();_tmp355[4]=_tmp74E;});({struct Cyc_PP_Doc*_tmp74F=
# 1426
Cyc_PP_line_doc();_tmp355[3]=_tmp74F;});({struct Cyc_PP_Doc*_tmp750=
# 1425
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp395,0));_tmp355[2]=_tmp750;});({struct Cyc_PP_Doc*_tmp751=
# 1424
Cyc_PP_blank_doc();_tmp355[1]=_tmp751;});({struct Cyc_PP_Doc*_tmp752=
# 1423
Cyc_Absynpp_lb();_tmp355[0]=_tmp752;});Cyc_PP_cat(_tag_dyneither(_tmp355,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp753;});else{
# 1440
if(Cyc_Absynpp_is_declaration(_tmp394))
({struct Cyc_PP_Doc*_tmp762=({struct Cyc_PP_Doc*_tmp35A[4];({struct Cyc_PP_Doc*_tmp754=
# 1451
Cyc_PP_line_doc();_tmp35A[3]=_tmp754;});({struct Cyc_PP_Doc*_tmp75F=
# 1443
stmtexp?({struct Cyc_PP_Doc*_tmp35B[6];({struct Cyc_PP_Doc*_tmp759=
# 1446
Cyc_PP_text(({const char*_tmp35D=");";_tag_dyneither(_tmp35D,sizeof(char),3);}));_tmp35B[5]=_tmp759;});({struct Cyc_PP_Doc*_tmp75A=Cyc_Absynpp_rb();_tmp35B[4]=_tmp75A;});({struct Cyc_PP_Doc*_tmp75B=
# 1445
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp394,stmtexp));_tmp35B[3]=_tmp75B;});({struct Cyc_PP_Doc*_tmp75C=
# 1444
Cyc_PP_blank_doc();_tmp35B[2]=_tmp75C;});({struct Cyc_PP_Doc*_tmp75D=Cyc_Absynpp_lb();_tmp35B[1]=_tmp75D;});({struct Cyc_PP_Doc*_tmp75E=Cyc_PP_text(({const char*_tmp35C="(";_tag_dyneither(_tmp35C,sizeof(char),2);}));_tmp35B[0]=_tmp75E;});Cyc_PP_cat(_tag_dyneither(_tmp35B,sizeof(struct Cyc_PP_Doc*),6));}):({struct Cyc_PP_Doc*_tmp35E[4];({struct Cyc_PP_Doc*_tmp755=
# 1450
Cyc_Absynpp_rb();_tmp35E[3]=_tmp755;});({struct Cyc_PP_Doc*_tmp756=
# 1449
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp394,stmtexp));_tmp35E[2]=_tmp756;});({struct Cyc_PP_Doc*_tmp757=
# 1448
Cyc_PP_blank_doc();_tmp35E[1]=_tmp757;});({struct Cyc_PP_Doc*_tmp758=Cyc_Absynpp_lb();_tmp35E[0]=_tmp758;});Cyc_PP_cat(_tag_dyneither(_tmp35E,sizeof(struct Cyc_PP_Doc*),4));});_tmp35A[2]=_tmp75F;});({struct Cyc_PP_Doc*_tmp760=
# 1442
Cyc_PP_line_doc();_tmp35A[1]=_tmp760;});({struct Cyc_PP_Doc*_tmp761=
# 1441
Cyc_Absynpp_stmt2doc(_tmp395,0);_tmp35A[0]=_tmp761;});Cyc_PP_cat(_tag_dyneither(_tmp35A,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp762;});else{
# 1453
({struct Cyc_PP_Doc*_tmp766=({struct Cyc_PP_Doc*_tmp35F[3];({struct Cyc_PP_Doc*_tmp763=Cyc_Absynpp_stmt2doc(_tmp394,stmtexp);_tmp35F[2]=_tmp763;});({struct Cyc_PP_Doc*_tmp764=Cyc_PP_line_doc();_tmp35F[1]=_tmp764;});({struct Cyc_PP_Doc*_tmp765=Cyc_Absynpp_stmt2doc(_tmp395,0);_tmp35F[0]=_tmp765;});Cyc_PP_cat(_tag_dyneither(_tmp35F,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp766;});}}}else{
# 1456
({struct Cyc_PP_Doc*_tmp76A=({struct Cyc_PP_Doc*_tmp360[3];({struct Cyc_PP_Doc*_tmp767=Cyc_Absynpp_stmt2doc(_tmp394,stmtexp);_tmp360[2]=_tmp767;});({struct Cyc_PP_Doc*_tmp768=Cyc_PP_line_doc();_tmp360[1]=_tmp768;});({struct Cyc_PP_Doc*_tmp769=Cyc_Absynpp_stmt2doc(_tmp395,0);_tmp360[0]=_tmp769;});Cyc_PP_cat(_tag_dyneither(_tmp360,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp76A;});}
goto _LL2AC;case 3: _LL2B3: _tmp396=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_LL2B4:
# 1459
 if(_tmp396 == 0)
({struct Cyc_PP_Doc*_tmp76B=Cyc_PP_text(({const char*_tmp361="return;";_tag_dyneither(_tmp361,sizeof(char),8);}));s=_tmp76B;});else{
# 1462
({struct Cyc_PP_Doc*_tmp76F=({struct Cyc_PP_Doc*_tmp362[3];({struct Cyc_PP_Doc*_tmp76C=
# 1464
Cyc_PP_text(({const char*_tmp364=";";_tag_dyneither(_tmp364,sizeof(char),2);}));_tmp362[2]=_tmp76C;});({struct Cyc_PP_Doc*_tmp76D=
# 1463
_tmp396 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp396);_tmp362[1]=_tmp76D;});({struct Cyc_PP_Doc*_tmp76E=
# 1462
Cyc_PP_text(({const char*_tmp363="return ";_tag_dyneither(_tmp363,sizeof(char),8);}));_tmp362[0]=_tmp76E;});Cyc_PP_cat(_tag_dyneither(_tmp362,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp76F;});}
# 1465
goto _LL2AC;case 4: _LL2B5: _tmp399=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp398=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_tmp397=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp351)->f3;_LL2B6: {
# 1467
int print_else;
{void*_tmp365=_tmp397->r;void*_tmp366=_tmp365;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp366)->tag == 0){_LL2D0: _LL2D1:
 print_else=0;goto _LL2CF;}else{_LL2D2: _LL2D3:
 print_else=1;goto _LL2CF;}_LL2CF:;}
# 1472
({struct Cyc_PP_Doc*_tmp782=({struct Cyc_PP_Doc*_tmp367[8];({struct Cyc_PP_Doc*_tmp778=
# 1479
print_else?({struct Cyc_PP_Doc*_tmp36B[6];({struct Cyc_PP_Doc*_tmp770=
# 1485
Cyc_Absynpp_rb();_tmp36B[5]=_tmp770;});({struct Cyc_PP_Doc*_tmp771=
# 1484
Cyc_PP_line_doc();_tmp36B[4]=_tmp771;});({struct Cyc_PP_Doc*_tmp774=
# 1483
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36D[2];({struct Cyc_PP_Doc*_tmp772=Cyc_Absynpp_stmt2doc(_tmp397,0);_tmp36D[1]=_tmp772;});({struct Cyc_PP_Doc*_tmp773=Cyc_PP_line_doc();_tmp36D[0]=_tmp773;});Cyc_PP_cat(_tag_dyneither(_tmp36D,sizeof(struct Cyc_PP_Doc*),2));}));_tmp36B[3]=_tmp774;});({struct Cyc_PP_Doc*_tmp775=
# 1482
Cyc_Absynpp_lb();_tmp36B[2]=_tmp775;});({struct Cyc_PP_Doc*_tmp776=
# 1481
Cyc_PP_text(({const char*_tmp36C="else ";_tag_dyneither(_tmp36C,sizeof(char),6);}));_tmp36B[1]=_tmp776;});({struct Cyc_PP_Doc*_tmp777=
# 1480
Cyc_PP_line_doc();_tmp36B[0]=_tmp777;});Cyc_PP_cat(_tag_dyneither(_tmp36B,sizeof(struct Cyc_PP_Doc*),6));}):
# 1486
 Cyc_PP_nil_doc();_tmp367[7]=_tmp778;});({struct Cyc_PP_Doc*_tmp779=
# 1478
Cyc_Absynpp_rb();_tmp367[6]=_tmp779;});({struct Cyc_PP_Doc*_tmp77A=
# 1477
Cyc_PP_line_doc();_tmp367[5]=_tmp77A;});({struct Cyc_PP_Doc*_tmp77D=
# 1476
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36A[2];({struct Cyc_PP_Doc*_tmp77B=Cyc_Absynpp_stmt2doc(_tmp398,0);_tmp36A[1]=_tmp77B;});({struct Cyc_PP_Doc*_tmp77C=Cyc_PP_line_doc();_tmp36A[0]=_tmp77C;});Cyc_PP_cat(_tag_dyneither(_tmp36A,sizeof(struct Cyc_PP_Doc*),2));}));_tmp367[4]=_tmp77D;});({struct Cyc_PP_Doc*_tmp77E=
# 1475
Cyc_Absynpp_lb();_tmp367[3]=_tmp77E;});({struct Cyc_PP_Doc*_tmp77F=
# 1474
Cyc_PP_text(({const char*_tmp369=") ";_tag_dyneither(_tmp369,sizeof(char),3);}));_tmp367[2]=_tmp77F;});({struct Cyc_PP_Doc*_tmp780=
# 1473
Cyc_Absynpp_exp2doc(_tmp399);_tmp367[1]=_tmp780;});({struct Cyc_PP_Doc*_tmp781=
# 1472
Cyc_PP_text(({const char*_tmp368="if (";_tag_dyneither(_tmp368,sizeof(char),5);}));_tmp367[0]=_tmp781;});Cyc_PP_cat(_tag_dyneither(_tmp367,sizeof(struct Cyc_PP_Doc*),8));});s=_tmp782;});
# 1487
goto _LL2AC;}case 5: _LL2B7: _tmp39B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp351)->f1).f1;_tmp39A=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_LL2B8:
# 1489
({struct Cyc_PP_Doc*_tmp78C=({struct Cyc_PP_Doc*_tmp36E[7];({struct Cyc_PP_Doc*_tmp783=
# 1495
Cyc_Absynpp_rb();_tmp36E[6]=_tmp783;});({struct Cyc_PP_Doc*_tmp784=
# 1494
Cyc_PP_line_doc();_tmp36E[5]=_tmp784;});({struct Cyc_PP_Doc*_tmp787=
# 1493
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp371[2];({struct Cyc_PP_Doc*_tmp785=Cyc_Absynpp_stmt2doc(_tmp39A,0);_tmp371[1]=_tmp785;});({struct Cyc_PP_Doc*_tmp786=Cyc_PP_line_doc();_tmp371[0]=_tmp786;});Cyc_PP_cat(_tag_dyneither(_tmp371,sizeof(struct Cyc_PP_Doc*),2));}));_tmp36E[4]=_tmp787;});({struct Cyc_PP_Doc*_tmp788=
# 1492
Cyc_Absynpp_lb();_tmp36E[3]=_tmp788;});({struct Cyc_PP_Doc*_tmp789=
# 1491
Cyc_PP_text(({const char*_tmp370=") ";_tag_dyneither(_tmp370,sizeof(char),3);}));_tmp36E[2]=_tmp789;});({struct Cyc_PP_Doc*_tmp78A=
# 1490
Cyc_Absynpp_exp2doc(_tmp39B);_tmp36E[1]=_tmp78A;});({struct Cyc_PP_Doc*_tmp78B=
# 1489
Cyc_PP_text(({const char*_tmp36F="while (";_tag_dyneither(_tmp36F,sizeof(char),8);}));_tmp36E[0]=_tmp78B;});Cyc_PP_cat(_tag_dyneither(_tmp36E,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp78C;});
# 1496
goto _LL2AC;case 6: _LL2B9: _LL2BA:
({struct Cyc_PP_Doc*_tmp78D=Cyc_PP_text(({const char*_tmp372="break;";_tag_dyneither(_tmp372,sizeof(char),7);}));s=_tmp78D;});goto _LL2AC;case 7: _LL2BB: _LL2BC:
({struct Cyc_PP_Doc*_tmp78E=Cyc_PP_text(({const char*_tmp373="continue;";_tag_dyneither(_tmp373,sizeof(char),10);}));s=_tmp78E;});goto _LL2AC;case 8: _LL2BD: _tmp39C=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_LL2BE:
({struct Cyc_PP_Doc*_tmp790=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp376;_tmp376.tag=0;_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39C);({void*_tmp374[1]={& _tmp376};({struct _dyneither_ptr _tmp78F=({const char*_tmp375="goto %s;";_tag_dyneither(_tmp375,sizeof(char),9);});Cyc_aprintf(_tmp78F,_tag_dyneither(_tmp374,sizeof(void*),1));});});}));s=_tmp790;});goto _LL2AC;case 9: _LL2BF: _tmp3A0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp39F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp351)->f2).f1;_tmp39E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp351)->f3).f1;_tmp39D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp351)->f4;_LL2C0:
# 1501
({struct Cyc_PP_Doc*_tmp79E=({struct Cyc_PP_Doc*_tmp377[11];({struct Cyc_PP_Doc*_tmp791=
# 1511
Cyc_Absynpp_rb();_tmp377[10]=_tmp791;});({struct Cyc_PP_Doc*_tmp792=
# 1510
Cyc_PP_line_doc();_tmp377[9]=_tmp792;});({struct Cyc_PP_Doc*_tmp795=
# 1509
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp37C[2];({struct Cyc_PP_Doc*_tmp793=Cyc_Absynpp_stmt2doc(_tmp39D,0);_tmp37C[1]=_tmp793;});({struct Cyc_PP_Doc*_tmp794=Cyc_PP_line_doc();_tmp37C[0]=_tmp794;});Cyc_PP_cat(_tag_dyneither(_tmp37C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp377[8]=_tmp795;});({struct Cyc_PP_Doc*_tmp796=
# 1508
Cyc_Absynpp_lb();_tmp377[7]=_tmp796;});({struct Cyc_PP_Doc*_tmp797=
# 1507
Cyc_PP_text(({const char*_tmp37B=") ";_tag_dyneither(_tmp37B,sizeof(char),3);}));_tmp377[6]=_tmp797;});({struct Cyc_PP_Doc*_tmp798=
# 1506
Cyc_Absynpp_exp2doc(_tmp39E);_tmp377[5]=_tmp798;});({struct Cyc_PP_Doc*_tmp799=
# 1505
Cyc_PP_text(({const char*_tmp37A="; ";_tag_dyneither(_tmp37A,sizeof(char),3);}));_tmp377[4]=_tmp799;});({struct Cyc_PP_Doc*_tmp79A=
# 1504
Cyc_Absynpp_exp2doc(_tmp39F);_tmp377[3]=_tmp79A;});({struct Cyc_PP_Doc*_tmp79B=
# 1503
Cyc_PP_text(({const char*_tmp379="; ";_tag_dyneither(_tmp379,sizeof(char),3);}));_tmp377[2]=_tmp79B;});({struct Cyc_PP_Doc*_tmp79C=
# 1502
Cyc_Absynpp_exp2doc(_tmp3A0);_tmp377[1]=_tmp79C;});({struct Cyc_PP_Doc*_tmp79D=
# 1501
Cyc_PP_text(({const char*_tmp378="for(";_tag_dyneither(_tmp378,sizeof(char),5);}));_tmp377[0]=_tmp79D;});Cyc_PP_cat(_tag_dyneither(_tmp377,sizeof(struct Cyc_PP_Doc*),11));});s=_tmp79E;});
# 1512
goto _LL2AC;case 10: _LL2C1: _tmp3A2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp3A1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_LL2C2:
# 1514
({struct Cyc_PP_Doc*_tmp7A7=({struct Cyc_PP_Doc*_tmp37D[8];({struct Cyc_PP_Doc*_tmp79F=
# 1521
Cyc_Absynpp_rb();_tmp37D[7]=_tmp79F;});({struct Cyc_PP_Doc*_tmp7A0=
# 1520
Cyc_PP_line_doc();_tmp37D[6]=_tmp7A0;});({struct Cyc_PP_Doc*_tmp7A1=
# 1519
Cyc_Absynpp_switchclauses2doc(_tmp3A1);_tmp37D[5]=_tmp7A1;});({struct Cyc_PP_Doc*_tmp7A2=
# 1518
Cyc_PP_line_doc();_tmp37D[4]=_tmp7A2;});({struct Cyc_PP_Doc*_tmp7A3=
# 1517
Cyc_Absynpp_lb();_tmp37D[3]=_tmp7A3;});({struct Cyc_PP_Doc*_tmp7A4=
# 1516
Cyc_PP_text(({const char*_tmp37F=") ";_tag_dyneither(_tmp37F,sizeof(char),3);}));_tmp37D[2]=_tmp7A4;});({struct Cyc_PP_Doc*_tmp7A5=
# 1515
Cyc_Absynpp_exp2doc(_tmp3A2);_tmp37D[1]=_tmp7A5;});({struct Cyc_PP_Doc*_tmp7A6=
# 1514
Cyc_PP_text(({const char*_tmp37E="switch (";_tag_dyneither(_tmp37E,sizeof(char),9);}));_tmp37D[0]=_tmp7A6;});Cyc_PP_cat(_tag_dyneither(_tmp37D,sizeof(struct Cyc_PP_Doc*),8));});s=_tmp7A7;});
# 1522
goto _LL2AC;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp351)->f1 == 0){_LL2C3: _LL2C4:
({struct Cyc_PP_Doc*_tmp7A8=Cyc_PP_text(({const char*_tmp380="fallthru;";_tag_dyneither(_tmp380,sizeof(char),10);}));s=_tmp7A8;});goto _LL2AC;}else{_LL2C5: _tmp3A3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_LL2C6:
# 1525
({struct Cyc_PP_Doc*_tmp7AC=({struct Cyc_PP_Doc*_tmp381[3];({struct Cyc_PP_Doc*_tmp7A9=Cyc_PP_text(({const char*_tmp383=");";_tag_dyneither(_tmp383,sizeof(char),3);}));_tmp381[2]=_tmp7A9;});({struct Cyc_PP_Doc*_tmp7AA=Cyc_Absynpp_exps2doc_prec(20,_tmp3A3);_tmp381[1]=_tmp7AA;});({struct Cyc_PP_Doc*_tmp7AB=Cyc_PP_text(({const char*_tmp382="fallthru(";_tag_dyneither(_tmp382,sizeof(char),10);}));_tmp381[0]=_tmp7AB;});Cyc_PP_cat(_tag_dyneither(_tmp381,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp7AC;});goto _LL2AC;}case 12: _LL2C7: _tmp3A5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp3A4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_LL2C8:
# 1527
({struct Cyc_PP_Doc*_tmp7B0=({struct Cyc_PP_Doc*_tmp384[3];({struct Cyc_PP_Doc*_tmp7AD=
# 1529
Cyc_Absynpp_stmt2doc(_tmp3A4,stmtexp);_tmp384[2]=_tmp7AD;});({struct Cyc_PP_Doc*_tmp7AE=
# 1528
Cyc_PP_line_doc();_tmp384[1]=_tmp7AE;});({struct Cyc_PP_Doc*_tmp7AF=
# 1527
Cyc_Absynpp_decl2doc(_tmp3A5);_tmp384[0]=_tmp7AF;});Cyc_PP_cat(_tag_dyneither(_tmp384,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp7B0;});
# 1530
goto _LL2AC;case 13: _LL2C9: _tmp3A7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp3A6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_LL2CA:
# 1532
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3A6)){
if(stmtexp)
({struct Cyc_PP_Doc*_tmp7B9=({struct Cyc_PP_Doc*_tmp385[8];({struct Cyc_PP_Doc*_tmp7B1=
# 1540
Cyc_PP_text(({const char*_tmp387=");";_tag_dyneither(_tmp387,sizeof(char),3);}));_tmp385[7]=_tmp7B1;});({struct Cyc_PP_Doc*_tmp7B2=Cyc_Absynpp_rb();_tmp385[6]=_tmp7B2;});({struct Cyc_PP_Doc*_tmp7B3=
# 1539
Cyc_PP_line_doc();_tmp385[5]=_tmp7B3;});({struct Cyc_PP_Doc*_tmp7B4=
# 1538
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,1));_tmp385[4]=_tmp7B4;});({struct Cyc_PP_Doc*_tmp7B5=
# 1537
Cyc_PP_line_doc();_tmp385[3]=_tmp7B5;});({struct Cyc_PP_Doc*_tmp7B6=
# 1536
Cyc_Absynpp_lb();_tmp385[2]=_tmp7B6;});({struct Cyc_PP_Doc*_tmp7B7=
# 1535
Cyc_PP_text(({const char*_tmp386=": (";_tag_dyneither(_tmp386,sizeof(char),4);}));_tmp385[1]=_tmp7B7;});({struct Cyc_PP_Doc*_tmp7B8=
# 1534
Cyc_PP_textptr(_tmp3A7);_tmp385[0]=_tmp7B8;});Cyc_PP_cat(_tag_dyneither(_tmp385,sizeof(struct Cyc_PP_Doc*),8));});s=_tmp7B9;});else{
# 1542
({struct Cyc_PP_Doc*_tmp7C1=({struct Cyc_PP_Doc*_tmp388[7];({struct Cyc_PP_Doc*_tmp7BA=
# 1548
Cyc_Absynpp_rb();_tmp388[6]=_tmp7BA;});({struct Cyc_PP_Doc*_tmp7BB=
# 1547
Cyc_PP_line_doc();_tmp388[5]=_tmp7BB;});({struct Cyc_PP_Doc*_tmp7BC=
# 1546
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,0));_tmp388[4]=_tmp7BC;});({struct Cyc_PP_Doc*_tmp7BD=
# 1545
Cyc_PP_line_doc();_tmp388[3]=_tmp7BD;});({struct Cyc_PP_Doc*_tmp7BE=
# 1544
Cyc_Absynpp_lb();_tmp388[2]=_tmp7BE;});({struct Cyc_PP_Doc*_tmp7BF=
# 1543
Cyc_PP_text(({const char*_tmp389=": ";_tag_dyneither(_tmp389,sizeof(char),3);}));_tmp388[1]=_tmp7BF;});({struct Cyc_PP_Doc*_tmp7C0=
# 1542
Cyc_PP_textptr(_tmp3A7);_tmp388[0]=_tmp7C0;});Cyc_PP_cat(_tag_dyneither(_tmp388,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp7C1;});}}else{
# 1550
({struct Cyc_PP_Doc*_tmp7C5=({struct Cyc_PP_Doc*_tmp38A[3];({struct Cyc_PP_Doc*_tmp7C2=Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp);_tmp38A[2]=_tmp7C2;});({struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_text(({const char*_tmp38B=": ";_tag_dyneither(_tmp38B,sizeof(char),3);}));_tmp38A[1]=_tmp7C3;});({struct Cyc_PP_Doc*_tmp7C4=Cyc_PP_textptr(_tmp3A7);_tmp38A[0]=_tmp7C4;});Cyc_PP_cat(_tag_dyneither(_tmp38A,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp7C5;});}
goto _LL2AC;case 14: _LL2CB: _tmp3A9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp3A8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp351)->f2).f1;_LL2CC:
# 1553
({struct Cyc_PP_Doc*_tmp7CF=({struct Cyc_PP_Doc*_tmp38C[9];({struct Cyc_PP_Doc*_tmp7C6=
# 1561
Cyc_PP_text(({const char*_tmp38F=");";_tag_dyneither(_tmp38F,sizeof(char),3);}));_tmp38C[8]=_tmp7C6;});({struct Cyc_PP_Doc*_tmp7C7=
# 1560
Cyc_Absynpp_exp2doc(_tmp3A8);_tmp38C[7]=_tmp7C7;});({struct Cyc_PP_Doc*_tmp7C8=
# 1559
Cyc_PP_text(({const char*_tmp38E=" while (";_tag_dyneither(_tmp38E,sizeof(char),9);}));_tmp38C[6]=_tmp7C8;});({struct Cyc_PP_Doc*_tmp7C9=
# 1558
Cyc_Absynpp_rb();_tmp38C[5]=_tmp7C9;});({struct Cyc_PP_Doc*_tmp7CA=
# 1557
Cyc_PP_line_doc();_tmp38C[4]=_tmp7CA;});({struct Cyc_PP_Doc*_tmp7CB=
# 1556
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A9,0));_tmp38C[3]=_tmp7CB;});({struct Cyc_PP_Doc*_tmp7CC=
# 1555
Cyc_PP_line_doc();_tmp38C[2]=_tmp7CC;});({struct Cyc_PP_Doc*_tmp7CD=
# 1554
Cyc_Absynpp_lb();_tmp38C[1]=_tmp7CD;});({struct Cyc_PP_Doc*_tmp7CE=
# 1553
Cyc_PP_text(({const char*_tmp38D="do ";_tag_dyneither(_tmp38D,sizeof(char),4);}));_tmp38C[0]=_tmp7CE;});Cyc_PP_cat(_tag_dyneither(_tmp38C,sizeof(struct Cyc_PP_Doc*),9));});s=_tmp7CF;});
# 1562
goto _LL2AC;default: _LL2CD: _tmp3AB=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp351)->f1;_tmp3AA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp351)->f2;_LL2CE:
# 1564
({struct Cyc_PP_Doc*_tmp7DC=({struct Cyc_PP_Doc*_tmp390[12];({struct Cyc_PP_Doc*_tmp7D0=
# 1575
Cyc_Absynpp_rb();_tmp390[11]=_tmp7D0;});({struct Cyc_PP_Doc*_tmp7D1=
# 1574
Cyc_PP_line_doc();_tmp390[10]=_tmp7D1;});({struct Cyc_PP_Doc*_tmp7D2=
# 1573
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3AA));_tmp390[9]=_tmp7D2;});({struct Cyc_PP_Doc*_tmp7D3=
# 1572
Cyc_PP_line_doc();_tmp390[8]=_tmp7D3;});({struct Cyc_PP_Doc*_tmp7D4=
# 1571
Cyc_Absynpp_lb();_tmp390[7]=_tmp7D4;});({struct Cyc_PP_Doc*_tmp7D5=
# 1570
Cyc_PP_text(({const char*_tmp392=" catch ";_tag_dyneither(_tmp392,sizeof(char),8);}));_tmp390[6]=_tmp7D5;});({struct Cyc_PP_Doc*_tmp7D6=
# 1569
Cyc_Absynpp_rb();_tmp390[5]=_tmp7D6;});({struct Cyc_PP_Doc*_tmp7D7=
# 1568
Cyc_PP_line_doc();_tmp390[4]=_tmp7D7;});({struct Cyc_PP_Doc*_tmp7D8=
# 1567
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AB,0));_tmp390[3]=_tmp7D8;});({struct Cyc_PP_Doc*_tmp7D9=
# 1566
Cyc_PP_line_doc();_tmp390[2]=_tmp7D9;});({struct Cyc_PP_Doc*_tmp7DA=
# 1565
Cyc_Absynpp_lb();_tmp390[1]=_tmp7DA;});({struct Cyc_PP_Doc*_tmp7DB=
# 1564
Cyc_PP_text(({const char*_tmp391="try ";_tag_dyneither(_tmp391,sizeof(char),5);}));_tmp390[0]=_tmp7DB;});Cyc_PP_cat(_tag_dyneither(_tmp390,sizeof(struct Cyc_PP_Doc*),12));});s=_tmp7DC;});
# 1576
goto _LL2AC;}_LL2AC:;}
# 1578
return s;}
# 1581
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AC=p->r;void*_tmp3AD=_tmp3AC;struct Cyc_Absyn_Exp*_tmp40D;struct Cyc_Absyn_Datatypefield*_tmp40C;struct Cyc_List_List*_tmp40B;int _tmp40A;struct Cyc_Absyn_Datatypefield*_tmp409;struct Cyc_Absyn_Enumfield*_tmp408;struct Cyc_Absyn_Enumfield*_tmp407;struct Cyc_List_List*_tmp406;struct Cyc_List_List*_tmp405;int _tmp404;union Cyc_Absyn_AggrInfoU _tmp403;struct Cyc_List_List*_tmp402;struct Cyc_List_List*_tmp401;int _tmp400;struct _tuple0*_tmp3FF;struct Cyc_List_List*_tmp3FE;int _tmp3FD;struct _tuple0*_tmp3FC;struct Cyc_Absyn_Vardecl*_tmp3FB;struct Cyc_Absyn_Pat*_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3F9;struct Cyc_Absyn_Pat*_tmp3F8;struct Cyc_List_List*_tmp3F7;int _tmp3F6;struct Cyc_Absyn_Tvar*_tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F4;struct Cyc_Absyn_Tvar*_tmp3F3;struct Cyc_Absyn_Vardecl*_tmp3F2;struct Cyc_Absyn_Vardecl*_tmp3F1;struct Cyc_Absyn_Pat*_tmp3F0;struct Cyc_Absyn_Vardecl*_tmp3EF;struct _dyneither_ptr _tmp3EE;char _tmp3ED;enum Cyc_Absyn_Sign _tmp3EC;int _tmp3EB;switch(*((int*)_tmp3AD)){case 0: _LL2D5: _LL2D6:
({struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_text(({const char*_tmp3AE="_";_tag_dyneither(_tmp3AE,sizeof(char),2);}));s=_tmp7DD;});goto _LL2D4;case 9: _LL2D7: _LL2D8:
({struct Cyc_PP_Doc*_tmp7DE=Cyc_PP_text(({const char*_tmp3AF="NULL";_tag_dyneither(_tmp3AF,sizeof(char),5);}));s=_tmp7DE;});goto _LL2D4;case 10: _LL2D9: _tmp3EC=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3EB=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2DA:
# 1587
 if(_tmp3EC != Cyc_Absyn_Unsigned)
({struct Cyc_PP_Doc*_tmp7E0=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=1;_tmp3B2.f1=(unsigned long)_tmp3EB;({void*_tmp3B0[1]={& _tmp3B2};({struct _dyneither_ptr _tmp7DF=({const char*_tmp3B1="%d";_tag_dyneither(_tmp3B1,sizeof(char),3);});Cyc_aprintf(_tmp7DF,_tag_dyneither(_tmp3B0,sizeof(void*),1));});});}));s=_tmp7E0;});else{
({struct Cyc_PP_Doc*_tmp7E2=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=1;_tmp3B5.f1=(unsigned int)_tmp3EB;({void*_tmp3B3[1]={& _tmp3B5};({struct _dyneither_ptr _tmp7E1=({const char*_tmp3B4="%u";_tag_dyneither(_tmp3B4,sizeof(char),3);});Cyc_aprintf(_tmp7E1,_tag_dyneither(_tmp3B3,sizeof(void*),1));});});}));s=_tmp7E2;});}
goto _LL2D4;case 11: _LL2DB: _tmp3ED=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL2DC:
({struct Cyc_PP_Doc*_tmp7E5=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0;({struct _dyneither_ptr _tmp7E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3ED));_tmp3B8.f1=_tmp7E3;});({void*_tmp3B6[1]={& _tmp3B8};({struct _dyneither_ptr _tmp7E4=({const char*_tmp3B7="'%s'";_tag_dyneither(_tmp3B7,sizeof(char),5);});Cyc_aprintf(_tmp7E4,_tag_dyneither(_tmp3B6,sizeof(void*),1));});});}));s=_tmp7E5;});goto _LL2D4;case 12: _LL2DD: _tmp3EE=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL2DE:
({struct Cyc_PP_Doc*_tmp7E6=Cyc_PP_text(_tmp3EE);s=_tmp7E6;});goto _LL2D4;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2)->r)->tag == 0){_LL2DF: _tmp3EF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL2E0:
# 1594
({struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_qvar2doc(_tmp3EF->name);s=_tmp7E7;});goto _LL2D4;}else{_LL2E1: _tmp3F1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3F0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2E2:
# 1596
({struct Cyc_PP_Doc*_tmp7EB=({struct Cyc_PP_Doc*_tmp3B9[3];({struct Cyc_PP_Doc*_tmp7E8=Cyc_Absynpp_pat2doc(_tmp3F0);_tmp3B9[2]=_tmp7E8;});({struct Cyc_PP_Doc*_tmp7E9=Cyc_PP_text(({const char*_tmp3BA=" as ";_tag_dyneither(_tmp3BA,sizeof(char),5);}));_tmp3B9[1]=_tmp7E9;});({struct Cyc_PP_Doc*_tmp7EA=Cyc_Absynpp_qvar2doc(_tmp3F1->name);_tmp3B9[0]=_tmp7EA;});Cyc_PP_cat(_tag_dyneither(_tmp3B9,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp7EB;});goto _LL2D4;}case 2: _LL2E3: _tmp3F3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3F2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2E4:
# 1598
({struct Cyc_PP_Doc*_tmp7F1=({struct Cyc_PP_Doc*_tmp3BB[5];({struct Cyc_PP_Doc*_tmp7EC=
Cyc_Absynpp_vardecl2doc(_tmp3F2);_tmp3BB[4]=_tmp7EC;});({struct Cyc_PP_Doc*_tmp7ED=
# 1598
Cyc_PP_text(({const char*_tmp3BE="> ";_tag_dyneither(_tmp3BE,sizeof(char),3);}));_tmp3BB[3]=_tmp7ED;});({struct Cyc_PP_Doc*_tmp7EE=Cyc_Absynpp_tvar2doc(_tmp3F3);_tmp3BB[2]=_tmp7EE;});({struct Cyc_PP_Doc*_tmp7EF=Cyc_PP_text(({const char*_tmp3BD=" <";_tag_dyneither(_tmp3BD,sizeof(char),3);}));_tmp3BB[1]=_tmp7EF;});({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp3BC="alias";_tag_dyneither(_tmp3BC,sizeof(char),6);}));_tmp3BB[0]=_tmp7F0;});Cyc_PP_cat(_tag_dyneither(_tmp3BB,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp7F1;});
# 1600
goto _LL2D4;case 4: _LL2E5: _tmp3F5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3F4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2E6:
# 1602
({struct Cyc_PP_Doc*_tmp7F6=({struct Cyc_PP_Doc*_tmp3BF[4];({struct Cyc_PP_Doc*_tmp7F2=Cyc_PP_text(({const char*_tmp3C1=">";_tag_dyneither(_tmp3C1,sizeof(char),2);}));_tmp3BF[3]=_tmp7F2;});({struct Cyc_PP_Doc*_tmp7F3=Cyc_Absynpp_tvar2doc(_tmp3F5);_tmp3BF[2]=_tmp7F3;});({struct Cyc_PP_Doc*_tmp7F4=Cyc_PP_text(({const char*_tmp3C0="<";_tag_dyneither(_tmp3C0,sizeof(char),2);}));_tmp3BF[1]=_tmp7F4;});({struct Cyc_PP_Doc*_tmp7F5=Cyc_Absynpp_qvar2doc(_tmp3F4->name);_tmp3BF[0]=_tmp7F5;});Cyc_PP_cat(_tag_dyneither(_tmp3BF,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp7F6;});
goto _LL2D4;case 5: _LL2E7: _tmp3F7=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3F6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2E8:
# 1605
({struct Cyc_PP_Doc*_tmp7FC=({struct Cyc_PP_Doc*_tmp3C2[4];({struct Cyc_PP_Doc*_tmp7F7=
_tmp3F6?Cyc_PP_text(({const char*_tmp3C5=", ...)";_tag_dyneither(_tmp3C5,sizeof(char),7);})): Cyc_PP_text(({const char*_tmp3C6=")";_tag_dyneither(_tmp3C6,sizeof(char),2);}));_tmp3C2[3]=_tmp7F7;});({struct Cyc_PP_Doc*_tmp7F9=({
# 1605
struct _dyneither_ptr _tmp7F8=({const char*_tmp3C4=",";_tag_dyneither(_tmp3C4,sizeof(char),2);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp7F8,_tmp3F7);});_tmp3C2[2]=_tmp7F9;});({struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_text(({const char*_tmp3C3="(";_tag_dyneither(_tmp3C3,sizeof(char),2);}));_tmp3C2[1]=_tmp7FA;});({struct Cyc_PP_Doc*_tmp7FB=Cyc_Absynpp_dollar();_tmp3C2[0]=_tmp7FB;});Cyc_PP_cat(_tag_dyneither(_tmp3C2,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp7FC;});
# 1607
goto _LL2D4;case 6: _LL2E9: _tmp3F8=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL2EA:
# 1609
({struct Cyc_PP_Doc*_tmp7FF=({struct Cyc_PP_Doc*_tmp3C7[2];({struct Cyc_PP_Doc*_tmp7FD=Cyc_Absynpp_pat2doc(_tmp3F8);_tmp3C7[1]=_tmp7FD;});({struct Cyc_PP_Doc*_tmp7FE=Cyc_PP_text(({const char*_tmp3C8="&";_tag_dyneither(_tmp3C8,sizeof(char),2);}));_tmp3C7[0]=_tmp7FE;});Cyc_PP_cat(_tag_dyneither(_tmp3C7,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp7FF;});
goto _LL2D4;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2)->r)->tag == 0){_LL2EB: _tmp3F9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL2EC:
# 1612
({struct Cyc_PP_Doc*_tmp802=({struct Cyc_PP_Doc*_tmp3C9[2];({struct Cyc_PP_Doc*_tmp800=Cyc_Absynpp_qvar2doc(_tmp3F9->name);_tmp3C9[1]=_tmp800;});({struct Cyc_PP_Doc*_tmp801=Cyc_PP_text(({const char*_tmp3CA="*";_tag_dyneither(_tmp3CA,sizeof(char),2);}));_tmp3C9[0]=_tmp801;});Cyc_PP_cat(_tag_dyneither(_tmp3C9,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp802;});
goto _LL2D4;}else{_LL2ED: _tmp3FB=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3FA=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2EE:
# 1615
({struct Cyc_PP_Doc*_tmp807=({struct Cyc_PP_Doc*_tmp3CB[4];({struct Cyc_PP_Doc*_tmp803=Cyc_Absynpp_pat2doc(_tmp3FA);_tmp3CB[3]=_tmp803;});({struct Cyc_PP_Doc*_tmp804=Cyc_PP_text(({const char*_tmp3CD=" as ";_tag_dyneither(_tmp3CD,sizeof(char),5);}));_tmp3CB[2]=_tmp804;});({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_qvar2doc(_tmp3FB->name);_tmp3CB[1]=_tmp805;});({struct Cyc_PP_Doc*_tmp806=Cyc_PP_text(({const char*_tmp3CC="*";_tag_dyneither(_tmp3CC,sizeof(char),2);}));_tmp3CB[0]=_tmp806;});Cyc_PP_cat(_tag_dyneither(_tmp3CB,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp807;});
goto _LL2D4;}case 15: _LL2EF: _tmp3FC=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL2F0:
# 1618
({struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_qvar2doc(_tmp3FC);s=_tmp808;});
goto _LL2D4;case 16: _LL2F1: _tmp3FF=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_tmp3FE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_tmp3FD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AD)->f3;_LL2F2: {
# 1621
struct _dyneither_ptr term=_tmp3FD?({const char*_tmp3D1=", ...)";_tag_dyneither(_tmp3D1,sizeof(char),7);}):({const char*_tmp3D2=")";_tag_dyneither(_tmp3D2,sizeof(char),2);});
({struct Cyc_PP_Doc*_tmp80E=({struct Cyc_PP_Doc*_tmp3CE[2];({struct Cyc_PP_Doc*_tmp80C=({struct _dyneither_ptr _tmp80B=({const char*_tmp3CF="(";_tag_dyneither(_tmp3CF,sizeof(char),2);});struct _dyneither_ptr _tmp80A=term;struct _dyneither_ptr _tmp809=({const char*_tmp3D0=",";_tag_dyneither(_tmp3D0,sizeof(char),2);});Cyc_PP_group(_tmp80B,_tmp80A,_tmp809,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3FE));});_tmp3CE[1]=_tmp80C;});({struct Cyc_PP_Doc*_tmp80D=Cyc_Absynpp_qvar2doc(_tmp3FF);_tmp3CE[0]=_tmp80D;});Cyc_PP_cat(_tag_dyneither(_tmp3CE,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp80E;});
goto _LL2D4;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1 != 0){_LL2F3: _tmp403=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1)->aggr_info;_tmp402=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_tmp401=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f3;_tmp400=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f4;_LL2F4: {
# 1625
struct _dyneither_ptr term=_tmp400?({const char*_tmp3DC=", ...}";_tag_dyneither(_tmp3DC,sizeof(char),7);}):({const char*_tmp3DD="}";_tag_dyneither(_tmp3DD,sizeof(char),2);});
struct _tuple10 _tmp3D3=Cyc_Absyn_aggr_kinded_name(_tmp403);struct _tuple10 _tmp3D4=_tmp3D3;struct _tuple0*_tmp3DB;_LL302: _tmp3DB=_tmp3D4.f2;_LL303:;
({struct Cyc_PP_Doc*_tmp819=({struct Cyc_PP_Doc*_tmp3D5[4];({struct Cyc_PP_Doc*_tmp812=({
# 1629
struct _dyneither_ptr _tmp811=({const char*_tmp3D9="";_tag_dyneither(_tmp3D9,sizeof(char),1);});struct _dyneither_ptr _tmp810=term;struct _dyneither_ptr _tmp80F=({const char*_tmp3DA=",";_tag_dyneither(_tmp3DA,sizeof(char),2);});Cyc_PP_group(_tmp811,_tmp810,_tmp80F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp401));});_tmp3D5[3]=_tmp812;});({struct Cyc_PP_Doc*_tmp816=({
# 1628
struct _dyneither_ptr _tmp815=({const char*_tmp3D6="[";_tag_dyneither(_tmp3D6,sizeof(char),2);});struct _dyneither_ptr _tmp814=({const char*_tmp3D7="]";_tag_dyneither(_tmp3D7,sizeof(char),2);});struct _dyneither_ptr _tmp813=({const char*_tmp3D8=",";_tag_dyneither(_tmp3D8,sizeof(char),2);});Cyc_PP_egroup(_tmp815,_tmp814,_tmp813,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp402));});_tmp3D5[2]=_tmp816;});({struct Cyc_PP_Doc*_tmp817=
# 1627
Cyc_Absynpp_lb();_tmp3D5[1]=_tmp817;});({struct Cyc_PP_Doc*_tmp818=Cyc_Absynpp_qvar2doc(_tmp3DB);_tmp3D5[0]=_tmp818;});Cyc_PP_cat(_tag_dyneither(_tmp3D5,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp819;});
# 1630
goto _LL2D4;}}else{_LL2F5: _tmp406=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_tmp405=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f3;_tmp404=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AD)->f4;_LL2F6: {
# 1632
struct _dyneither_ptr term=_tmp404?({const char*_tmp3E4=", ...}";_tag_dyneither(_tmp3E4,sizeof(char),7);}):({const char*_tmp3E5="}";_tag_dyneither(_tmp3E5,sizeof(char),2);});
({struct Cyc_PP_Doc*_tmp823=({struct Cyc_PP_Doc*_tmp3DE[3];({struct Cyc_PP_Doc*_tmp81D=({
# 1635
struct _dyneither_ptr _tmp81C=({const char*_tmp3E2="";_tag_dyneither(_tmp3E2,sizeof(char),1);});struct _dyneither_ptr _tmp81B=term;struct _dyneither_ptr _tmp81A=({const char*_tmp3E3=",";_tag_dyneither(_tmp3E3,sizeof(char),2);});Cyc_PP_group(_tmp81C,_tmp81B,_tmp81A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp405));});_tmp3DE[2]=_tmp81D;});({struct Cyc_PP_Doc*_tmp821=({
# 1634
struct _dyneither_ptr _tmp820=({const char*_tmp3DF="[";_tag_dyneither(_tmp3DF,sizeof(char),2);});struct _dyneither_ptr _tmp81F=({const char*_tmp3E0="]";_tag_dyneither(_tmp3E0,sizeof(char),2);});struct _dyneither_ptr _tmp81E=({const char*_tmp3E1=",";_tag_dyneither(_tmp3E1,sizeof(char),2);});Cyc_PP_egroup(_tmp820,_tmp81F,_tmp81E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp406));});_tmp3DE[1]=_tmp821;});({struct Cyc_PP_Doc*_tmp822=
# 1633
Cyc_Absynpp_lb();_tmp3DE[0]=_tmp822;});Cyc_PP_cat(_tag_dyneither(_tmp3DE,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp823;});
# 1636
goto _LL2D4;}}case 13: _LL2F7: _tmp407=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2F8:
 _tmp408=_tmp407;goto _LL2FA;case 14: _LL2F9: _tmp408=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2FA:
({struct Cyc_PP_Doc*_tmp824=Cyc_Absynpp_qvar2doc(_tmp408->name);s=_tmp824;});goto _LL2D4;case 8: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AD)->f3 == 0){_LL2FB: _tmp409=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_LL2FC:
({struct Cyc_PP_Doc*_tmp825=Cyc_Absynpp_qvar2doc(_tmp409->name);s=_tmp825;});goto _LL2D4;}else{_LL2FD: _tmp40C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AD)->f2;_tmp40B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AD)->f3;_tmp40A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AD)->f4;_LL2FE: {
# 1641
struct _dyneither_ptr term=_tmp40A?({const char*_tmp3E9=", ...)";_tag_dyneither(_tmp3E9,sizeof(char),7);}):({const char*_tmp3EA=")";_tag_dyneither(_tmp3EA,sizeof(char),2);});
({struct Cyc_PP_Doc*_tmp82B=({struct Cyc_PP_Doc*_tmp3E6[2];({struct Cyc_PP_Doc*_tmp829=({struct _dyneither_ptr _tmp828=({const char*_tmp3E7="(";_tag_dyneither(_tmp3E7,sizeof(char),2);});struct _dyneither_ptr _tmp827=term;struct _dyneither_ptr _tmp826=({const char*_tmp3E8=",";_tag_dyneither(_tmp3E8,sizeof(char),2);});Cyc_PP_egroup(_tmp828,_tmp827,_tmp826,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp40B));});_tmp3E6[1]=_tmp829;});({struct Cyc_PP_Doc*_tmp82A=Cyc_Absynpp_qvar2doc(_tmp40C->name);_tmp3E6[0]=_tmp82A;});Cyc_PP_cat(_tag_dyneither(_tmp3E6,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp82B;});
goto _LL2D4;}}default: _LL2FF: _tmp40D=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AD)->f1;_LL300:
# 1645
({struct Cyc_PP_Doc*_tmp82C=Cyc_Absynpp_exp2doc(_tmp40D);s=_tmp82C;});goto _LL2D4;}_LL2D4:;}
# 1647
return s;}
# 1650
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp40E[2];({struct Cyc_PP_Doc*_tmp82D=
Cyc_Absynpp_pat2doc((*dp).f2);_tmp40E[1]=_tmp82D;});({struct Cyc_PP_Doc*_tmp831=({
# 1651
struct _dyneither_ptr _tmp830=({const char*_tmp40F="";_tag_dyneither(_tmp40F,sizeof(char),1);});struct _dyneither_ptr _tmp82F=({const char*_tmp410="=";_tag_dyneither(_tmp410,sizeof(char),2);});struct _dyneither_ptr _tmp82E=({const char*_tmp411="=";_tag_dyneither(_tmp411,sizeof(char),2);});Cyc_PP_egroup(_tmp830,_tmp82F,_tmp82E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp40E[0]=_tmp831;});Cyc_PP_cat(_tag_dyneither(_tmp40E,sizeof(struct Cyc_PP_Doc*),2));});}
# 1655
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp412[2];({struct Cyc_PP_Doc*_tmp834=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp414[2];({struct Cyc_PP_Doc*_tmp832=Cyc_Absynpp_stmt2doc(c->body,0);_tmp414[1]=_tmp832;});({struct Cyc_PP_Doc*_tmp833=Cyc_PP_line_doc();_tmp414[0]=_tmp833;});Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),2));}));_tmp412[1]=_tmp834;});({struct Cyc_PP_Doc*_tmp835=
# 1657
Cyc_PP_text(({const char*_tmp413="default: ";_tag_dyneither(_tmp413,sizeof(char),10);}));_tmp412[0]=_tmp835;});Cyc_PP_cat(_tag_dyneither(_tmp412,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1659
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp415[4];({struct Cyc_PP_Doc*_tmp838=
# 1663
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp418[2];({struct Cyc_PP_Doc*_tmp836=Cyc_Absynpp_stmt2doc(c->body,0);_tmp418[1]=_tmp836;});({struct Cyc_PP_Doc*_tmp837=Cyc_PP_line_doc();_tmp418[0]=_tmp837;});Cyc_PP_cat(_tag_dyneither(_tmp418,sizeof(struct Cyc_PP_Doc*),2));}));_tmp415[3]=_tmp838;});({struct Cyc_PP_Doc*_tmp839=
# 1662
Cyc_PP_text(({const char*_tmp417=": ";_tag_dyneither(_tmp417,sizeof(char),3);}));_tmp415[2]=_tmp839;});({struct Cyc_PP_Doc*_tmp83A=
# 1661
Cyc_Absynpp_pat2doc(c->pattern);_tmp415[1]=_tmp83A;});({struct Cyc_PP_Doc*_tmp83B=
# 1660
Cyc_PP_text(({const char*_tmp416="case ";_tag_dyneither(_tmp416,sizeof(char),6);}));_tmp415[0]=_tmp83B;});Cyc_PP_cat(_tag_dyneither(_tmp415,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1665
return({struct Cyc_PP_Doc*_tmp419[6];({struct Cyc_PP_Doc*_tmp83E=
# 1670
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41D[2];({struct Cyc_PP_Doc*_tmp83C=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41D[1]=_tmp83C;});({struct Cyc_PP_Doc*_tmp83D=Cyc_PP_line_doc();_tmp41D[0]=_tmp83D;});Cyc_PP_cat(_tag_dyneither(_tmp41D,sizeof(struct Cyc_PP_Doc*),2));}));_tmp419[5]=_tmp83E;});({struct Cyc_PP_Doc*_tmp83F=
# 1669
Cyc_PP_text(({const char*_tmp41C=": ";_tag_dyneither(_tmp41C,sizeof(char),3);}));_tmp419[4]=_tmp83F;});({struct Cyc_PP_Doc*_tmp840=
# 1668
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp419[3]=_tmp840;});({struct Cyc_PP_Doc*_tmp841=
# 1667
Cyc_PP_text(({const char*_tmp41B=" && ";_tag_dyneither(_tmp41B,sizeof(char),5);}));_tmp419[2]=_tmp841;});({struct Cyc_PP_Doc*_tmp842=
# 1666
Cyc_Absynpp_pat2doc(c->pattern);_tmp419[1]=_tmp842;});({struct Cyc_PP_Doc*_tmp843=
# 1665
Cyc_PP_text(({const char*_tmp41A="case ";_tag_dyneither(_tmp41A,sizeof(char),6);}));_tmp419[0]=_tmp843;});Cyc_PP_cat(_tag_dyneither(_tmp419,sizeof(struct Cyc_PP_Doc*),6));});}}}
# 1673
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _dyneither_ptr _tmp844=({const char*_tmp41E="";_tag_dyneither(_tmp41E,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp844,cs);});}
# 1677
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1681
return({struct Cyc_PP_Doc*_tmp41F[3];({struct Cyc_PP_Doc*_tmp845=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41F[2]=_tmp845;});({struct Cyc_PP_Doc*_tmp846=Cyc_PP_text(({const char*_tmp420=" = ";_tag_dyneither(_tmp420,sizeof(char),4);}));_tmp41F[1]=_tmp846;});({struct Cyc_PP_Doc*_tmp847=Cyc_Absynpp_qvar2doc(f->name);_tmp41F[0]=_tmp847;});Cyc_PP_cat(_tag_dyneither(_tmp41F,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _dyneither_ptr _tmp848=({const char*_tmp421=",";_tag_dyneither(_tmp421,sizeof(char),2);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp848,fs);});}
# 1688
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1692
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _dyneither_ptr _tmp849=({const char*_tmp422=",";_tag_dyneither(_tmp422,sizeof(char),2);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp849,vds);});}
# 1696
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp423=vd;enum Cyc_Absyn_Scope _tmp435;struct _tuple0*_tmp434;unsigned int _tmp433;struct Cyc_Absyn_Tqual _tmp432;void*_tmp431;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_List_List*_tmp42F;_LL305: _tmp435=_tmp423->sc;_tmp434=_tmp423->name;_tmp433=_tmp423->varloc;_tmp432=_tmp423->tq;_tmp431=_tmp423->type;_tmp430=_tmp423->initializer;_tmp42F=_tmp423->attributes;_LL306:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp434);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp42F);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp424=Cyc_Cyclone_c_compiler;if(_tmp424 == Cyc_Cyclone_Gcc_c){_LL308: _LL309:
 beforenamedoc=attsdoc;goto _LL307;}else{_LL30A: _LL30B:
# 1705
{void*_tmp425=Cyc_Tcutil_compress(_tmp431);void*_tmp426=_tmp425;struct Cyc_List_List*_tmp427;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp426)->tag == 9){_LL30D: _tmp427=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp426)->f1).attributes;_LL30E:
# 1707
({struct Cyc_PP_Doc*_tmp84A=Cyc_Absynpp_callconv2doc(_tmp427);beforenamedoc=_tmp84A;});
goto _LL30C;}else{_LL30F: _LL310:
({struct Cyc_PP_Doc*_tmp84B=Cyc_PP_nil_doc();beforenamedoc=_tmp84B;});goto _LL30C;}_LL30C:;}
# 1711
goto _LL307;}_LL307:;}{
# 1714
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp428=Cyc_Cyclone_c_compiler;if(_tmp428 == Cyc_Cyclone_Gcc_c){_LL312: _LL313:
({struct Cyc_PP_Doc*_tmp84C=Cyc_PP_nil_doc();tmp_doc=_tmp84C;});goto _LL311;}else{_LL314: _LL315:
 tmp_doc=attsdoc;goto _LL311;}_LL311:;}
# 1719
({struct Cyc_PP_Doc*_tmp856=({struct Cyc_PP_Doc*_tmp429[5];({struct Cyc_PP_Doc*_tmp84D=
# 1726
Cyc_PP_text(({const char*_tmp42E=";";_tag_dyneither(_tmp42E,sizeof(char),2);}));_tmp429[4]=_tmp84D;});({struct Cyc_PP_Doc*_tmp850=
# 1723
_tmp430 == 0?
Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp42C[2];({struct Cyc_PP_Doc*_tmp84E=
Cyc_Absynpp_exp2doc(_tmp430);_tmp42C[1]=_tmp84E;});({struct Cyc_PP_Doc*_tmp84F=Cyc_PP_text(({const char*_tmp42D=" = ";_tag_dyneither(_tmp42D,sizeof(char),4);}));_tmp42C[0]=_tmp84F;});Cyc_PP_cat(_tag_dyneither(_tmp42C,sizeof(struct Cyc_PP_Doc*),2));});_tmp429[3]=_tmp850;});({struct Cyc_PP_Doc*_tmp854=({
# 1722
struct Cyc_Absyn_Tqual _tmp853=_tmp432;void*_tmp852=_tmp431;Cyc_Absynpp_tqtd2doc(_tmp853,_tmp852,({struct Cyc_Core_Opt*_tmp42A=_cycalloc(sizeof(*_tmp42A));({struct Cyc_PP_Doc*_tmp851=({struct Cyc_PP_Doc*_tmp42B[2];_tmp42B[1]=sn;_tmp42B[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp42B,sizeof(struct Cyc_PP_Doc*),2));});_tmp42A->v=_tmp851;});_tmp42A;}));});_tmp429[2]=_tmp854;});({struct Cyc_PP_Doc*_tmp855=
# 1721
Cyc_Absynpp_scope2doc(_tmp435);_tmp429[1]=_tmp855;});_tmp429[0]=tmp_doc;Cyc_PP_cat(_tag_dyneither(_tmp429,sizeof(struct Cyc_PP_Doc*),5));});
# 1719
s=_tmp856;});
# 1727
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1730
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp436=*x;struct _tuple17 _tmp437=_tmp436;struct _tuple0*_tmp438;_LL317: _tmp438=_tmp437.f2;_LL318:;
return Cyc_Absynpp_qvar2doc(_tmp438);}
# 1735
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp439[4];({struct Cyc_PP_Doc*_tmp857=
# 1740
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp439[3]=_tmp857;});({struct Cyc_PP_Doc*_tmp858=
# 1739
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp439[2]=_tmp858;});({struct Cyc_PP_Doc*_tmp859=
# 1738
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp439[1]=_tmp859;});({struct Cyc_PP_Doc*_tmp85A=
# 1737
Cyc_Absynpp_scope2doc(ad->sc);_tmp439[0]=_tmp85A;});Cyc_PP_cat(_tag_dyneither(_tmp439,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1742
return({struct Cyc_PP_Doc*_tmp43A[12];({struct Cyc_PP_Doc*_tmp85B=
# 1753
Cyc_Absynpp_atts2doc(ad->attributes);_tmp43A[11]=_tmp85B;});({struct Cyc_PP_Doc*_tmp85C=
# 1752
Cyc_Absynpp_rb();_tmp43A[10]=_tmp85C;});({struct Cyc_PP_Doc*_tmp85D=
# 1751
Cyc_PP_line_doc();_tmp43A[9]=_tmp85D;});({struct Cyc_PP_Doc*_tmp860=
# 1750
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43D[2];({struct Cyc_PP_Doc*_tmp85E=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43D[1]=_tmp85E;});({struct Cyc_PP_Doc*_tmp85F=Cyc_PP_line_doc();_tmp43D[0]=_tmp85F;});Cyc_PP_cat(_tag_dyneither(_tmp43D,sizeof(struct Cyc_PP_Doc*),2));}));_tmp43A[8]=_tmp860;});({struct Cyc_PP_Doc*_tmp863=
# 1748
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp43B[2];({struct Cyc_PP_Doc*_tmp861=
Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp43B[1]=_tmp861;});({struct Cyc_PP_Doc*_tmp862=Cyc_PP_text(({const char*_tmp43C=":";_tag_dyneither(_tmp43C,sizeof(char),2);}));_tmp43B[0]=_tmp862;});Cyc_PP_cat(_tag_dyneither(_tmp43B,sizeof(struct Cyc_PP_Doc*),2));});_tmp43A[7]=_tmp863;});({struct Cyc_PP_Doc*_tmp864=
# 1747
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp43A[6]=_tmp864;});({struct Cyc_PP_Doc*_tmp865=
# 1746
Cyc_Absynpp_lb();_tmp43A[5]=_tmp865;});({struct Cyc_PP_Doc*_tmp866=Cyc_PP_blank_doc();_tmp43A[4]=_tmp866;});({struct Cyc_PP_Doc*_tmp867=
# 1745
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp43A[3]=_tmp867;});({struct Cyc_PP_Doc*_tmp868=
# 1744
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp43A[2]=_tmp868;});({struct Cyc_PP_Doc*_tmp869=
# 1743
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp43A[1]=_tmp869;});({struct Cyc_PP_Doc*_tmp86A=
# 1742
Cyc_Absynpp_scope2doc(ad->sc);_tmp43A[0]=_tmp86A;});Cyc_PP_cat(_tag_dyneither(_tmp43A,sizeof(struct Cyc_PP_Doc*),12));});}}
# 1756
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43E=dd;enum Cyc_Absyn_Scope _tmp44A;struct _tuple0*_tmp449;struct Cyc_List_List*_tmp448;struct Cyc_Core_Opt*_tmp447;int _tmp446;_LL31A: _tmp44A=_tmp43E->sc;_tmp449=_tmp43E->name;_tmp448=_tmp43E->tvs;_tmp447=_tmp43E->fields;_tmp446=_tmp43E->is_extensible;_LL31B:;
if(_tmp447 == 0)
return({struct Cyc_PP_Doc*_tmp43F[5];({struct Cyc_PP_Doc*_tmp86B=
# 1763
Cyc_Absynpp_ktvars2doc(_tmp448);_tmp43F[4]=_tmp86B;});({struct Cyc_PP_Doc*_tmp86C=
# 1762
_tmp446?Cyc_Absynpp_qvar2bolddoc(_tmp449): Cyc_Absynpp_typedef_name2bolddoc(_tmp449);_tmp43F[3]=_tmp86C;});({struct Cyc_PP_Doc*_tmp86D=
# 1761
Cyc_PP_text(({const char*_tmp441="datatype ";_tag_dyneither(_tmp441,sizeof(char),10);}));_tmp43F[2]=_tmp86D;});({struct Cyc_PP_Doc*_tmp86E=
# 1760
_tmp446?Cyc_PP_text(({const char*_tmp440="@extensible ";_tag_dyneither(_tmp440,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp43F[1]=_tmp86E;});({struct Cyc_PP_Doc*_tmp86F=
# 1759
Cyc_Absynpp_scope2doc(_tmp44A);_tmp43F[0]=_tmp86F;});Cyc_PP_cat(_tag_dyneither(_tmp43F,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1765
return({struct Cyc_PP_Doc*_tmp442[10];({struct Cyc_PP_Doc*_tmp870=
# 1773
Cyc_Absynpp_rb();_tmp442[9]=_tmp870;});({struct Cyc_PP_Doc*_tmp871=
# 1772
Cyc_PP_line_doc();_tmp442[8]=_tmp871;});({struct Cyc_PP_Doc*_tmp874=
# 1771
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp445[2];({struct Cyc_PP_Doc*_tmp872=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp447->v);_tmp445[1]=_tmp872;});({struct Cyc_PP_Doc*_tmp873=Cyc_PP_line_doc();_tmp445[0]=_tmp873;});Cyc_PP_cat(_tag_dyneither(_tmp445,sizeof(struct Cyc_PP_Doc*),2));}));_tmp442[7]=_tmp874;});({struct Cyc_PP_Doc*_tmp875=
# 1770
Cyc_Absynpp_lb();_tmp442[6]=_tmp875;});({struct Cyc_PP_Doc*_tmp876=Cyc_PP_blank_doc();_tmp442[5]=_tmp876;});({struct Cyc_PP_Doc*_tmp877=
# 1769
Cyc_Absynpp_ktvars2doc(_tmp448);_tmp442[4]=_tmp877;});({struct Cyc_PP_Doc*_tmp878=
# 1768
_tmp446?Cyc_Absynpp_qvar2bolddoc(_tmp449): Cyc_Absynpp_typedef_name2bolddoc(_tmp449);_tmp442[3]=_tmp878;});({struct Cyc_PP_Doc*_tmp879=
# 1767
Cyc_PP_text(({const char*_tmp444="datatype ";_tag_dyneither(_tmp444,sizeof(char),10);}));_tmp442[2]=_tmp879;});({struct Cyc_PP_Doc*_tmp87A=
# 1766
_tmp446?Cyc_PP_text(({const char*_tmp443="@extensible ";_tag_dyneither(_tmp443,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp442[1]=_tmp87A;});({struct Cyc_PP_Doc*_tmp87B=
# 1765
Cyc_Absynpp_scope2doc(_tmp44A);_tmp442[0]=_tmp87B;});Cyc_PP_cat(_tag_dyneither(_tmp442,sizeof(struct Cyc_PP_Doc*),10));});}}
# 1776
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp44B=ed;enum Cyc_Absyn_Scope _tmp453;struct _tuple0*_tmp452;struct Cyc_Core_Opt*_tmp451;_LL31D: _tmp453=_tmp44B->sc;_tmp452=_tmp44B->name;_tmp451=_tmp44B->fields;_LL31E:;
if(_tmp451 == 0)
return({struct Cyc_PP_Doc*_tmp44C[3];({struct Cyc_PP_Doc*_tmp87C=
# 1781
Cyc_Absynpp_typedef_name2bolddoc(_tmp452);_tmp44C[2]=_tmp87C;});({struct Cyc_PP_Doc*_tmp87D=
# 1780
Cyc_PP_text(({const char*_tmp44D="enum ";_tag_dyneither(_tmp44D,sizeof(char),6);}));_tmp44C[1]=_tmp87D;});({struct Cyc_PP_Doc*_tmp87E=
# 1779
Cyc_Absynpp_scope2doc(_tmp453);_tmp44C[0]=_tmp87E;});Cyc_PP_cat(_tag_dyneither(_tmp44C,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1784
return({struct Cyc_PP_Doc*_tmp44E[8];({struct Cyc_PP_Doc*_tmp87F=
# 1790
Cyc_Absynpp_rb();_tmp44E[7]=_tmp87F;});({struct Cyc_PP_Doc*_tmp880=
# 1789
Cyc_PP_line_doc();_tmp44E[6]=_tmp880;});({struct Cyc_PP_Doc*_tmp883=
# 1788
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp450[2];({struct Cyc_PP_Doc*_tmp881=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp451->v);_tmp450[1]=_tmp881;});({struct Cyc_PP_Doc*_tmp882=Cyc_PP_line_doc();_tmp450[0]=_tmp882;});Cyc_PP_cat(_tag_dyneither(_tmp450,sizeof(struct Cyc_PP_Doc*),2));}));_tmp44E[5]=_tmp883;});({struct Cyc_PP_Doc*_tmp884=
# 1787
Cyc_Absynpp_lb();_tmp44E[4]=_tmp884;});({struct Cyc_PP_Doc*_tmp885=Cyc_PP_blank_doc();_tmp44E[3]=_tmp885;});({struct Cyc_PP_Doc*_tmp886=
# 1786
Cyc_Absynpp_qvar2bolddoc(_tmp452);_tmp44E[2]=_tmp886;});({struct Cyc_PP_Doc*_tmp887=
# 1785
Cyc_PP_text(({const char*_tmp44F="enum ";_tag_dyneither(_tmp44F,sizeof(char),6);}));_tmp44E[1]=_tmp887;});({struct Cyc_PP_Doc*_tmp888=
# 1784
Cyc_Absynpp_scope2doc(_tmp453);_tmp44E[0]=_tmp888;});Cyc_PP_cat(_tag_dyneither(_tmp44E,sizeof(struct Cyc_PP_Doc*),8));});}}
# 1793
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp454=d->r;void*_tmp455=_tmp454;struct Cyc_List_List*_tmp4B5;struct Cyc_List_List*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct _tuple0*_tmp4B2;struct Cyc_List_List*_tmp4B1;struct _dyneither_ptr*_tmp4B0;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Typedefdecl*_tmp4AE;struct Cyc_Absyn_Enumdecl*_tmp4AD;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_Pat*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Datatypedecl*_tmp4A9;struct Cyc_Absyn_Tvar*_tmp4A8;struct Cyc_Absyn_Vardecl*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Vardecl*_tmp4A5;struct Cyc_Absyn_Aggrdecl*_tmp4A4;struct Cyc_Absyn_Fndecl*_tmp4A3;switch(*((int*)_tmp455)){case 1: _LL320: _tmp4A3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL321: {
# 1797
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp461=_cycalloc(sizeof(*_tmp461));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp88B=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp462;_tmp462.tag=9;({struct Cyc_Absyn_FnInfo _tmp88A=({struct Cyc_Absyn_FnInfo _tmp463;_tmp463.tvars=_tmp4A3->tvs;_tmp463.effect=_tmp4A3->effect;_tmp463.ret_tqual=_tmp4A3->ret_tqual;_tmp463.ret_typ=_tmp4A3->ret_type;({struct Cyc_List_List*_tmp889=
# 1801
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4A3->args);_tmp463.args=_tmp889;});_tmp463.c_varargs=_tmp4A3->c_varargs;_tmp463.cyc_varargs=_tmp4A3->cyc_varargs;_tmp463.rgn_po=_tmp4A3->rgn_po;_tmp463.attributes=0;_tmp463.requires_clause=_tmp4A3->requires_clause;_tmp463.requires_relns=_tmp4A3->requires_relns;_tmp463.ensures_clause=_tmp4A3->ensures_clause;_tmp463.ensures_relns=_tmp4A3->ensures_relns;_tmp463;});_tmp462.f1=_tmp88A;});_tmp462;});_tmp461[0]=_tmp88B;});_tmp461;});
# 1807
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A3->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4A3->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp456=Cyc_Cyclone_c_compiler;if(_tmp456 == Cyc_Cyclone_Gcc_c){_LL33F: _LL340:
({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_text(({const char*_tmp457="inline ";_tag_dyneither(_tmp457,sizeof(char),8);}));inlinedoc=_tmp88C;});goto _LL33E;}else{_LL341: _LL342:
({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp458="__inline ";_tag_dyneither(_tmp458,sizeof(char),10);}));inlinedoc=_tmp88D;});goto _LL33E;}_LL33E:;}else{
# 1815
({struct Cyc_PP_Doc*_tmp88E=Cyc_PP_nil_doc();inlinedoc=_tmp88E;});}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4A3->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp459=Cyc_Cyclone_c_compiler;if(_tmp459 == Cyc_Cyclone_Gcc_c){_LL344: _LL345:
 beforenamedoc=attsdoc;goto _LL343;}else{_LL346: _LL347:
({struct Cyc_PP_Doc*_tmp88F=Cyc_Absynpp_callconv2doc(_tmp4A3->attributes);beforenamedoc=_tmp88F;});goto _LL343;}_LL343:;}{
# 1822
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4A3->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp892=Cyc_Absyn_empty_tqual(0);void*_tmp891=t;Cyc_Absynpp_tqtd2doc(_tmp892,_tmp891,({struct Cyc_Core_Opt*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct Cyc_PP_Doc*_tmp890=({struct Cyc_PP_Doc*_tmp460[2];_tmp460[1]=namedoc;_tmp460[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp460,sizeof(struct Cyc_PP_Doc*),2));});_tmp45F->v=_tmp890;});_tmp45F;}));});
# 1825
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp45D[5];({struct Cyc_PP_Doc*_tmp893=
# 1828
Cyc_Absynpp_rb();_tmp45D[4]=_tmp893;});({struct Cyc_PP_Doc*_tmp894=
# 1827
Cyc_PP_line_doc();_tmp45D[3]=_tmp894;});({struct Cyc_PP_Doc*_tmp897=
# 1826
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp45E[2];({struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_stmt2doc(_tmp4A3->body,0);_tmp45E[1]=_tmp895;});({struct Cyc_PP_Doc*_tmp896=Cyc_PP_line_doc();_tmp45E[0]=_tmp896;});Cyc_PP_cat(_tag_dyneither(_tmp45E,sizeof(struct Cyc_PP_Doc*),2));}));_tmp45D[2]=_tmp897;});({struct Cyc_PP_Doc*_tmp898=
# 1825
Cyc_Absynpp_lb();_tmp45D[1]=_tmp898;});({struct Cyc_PP_Doc*_tmp899=Cyc_PP_blank_doc();_tmp45D[0]=_tmp899;});Cyc_PP_cat(_tag_dyneither(_tmp45D,sizeof(struct Cyc_PP_Doc*),5));});
# 1829
({struct Cyc_PP_Doc*_tmp89A=({struct Cyc_PP_Doc*_tmp45A[4];_tmp45A[3]=bodydoc;_tmp45A[2]=tqtddoc;_tmp45A[1]=scopedoc;_tmp45A[0]=inlinedoc;Cyc_PP_cat(_tag_dyneither(_tmp45A,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp89A;});
# 1831
{enum Cyc_Cyclone_C_Compilers _tmp45B=Cyc_Cyclone_c_compiler;if(_tmp45B == Cyc_Cyclone_Vc_c){_LL349: _LL34A:
({struct Cyc_PP_Doc*_tmp89B=({struct Cyc_PP_Doc*_tmp45C[2];_tmp45C[1]=s;_tmp45C[0]=attsdoc;Cyc_PP_cat(_tag_dyneither(_tmp45C,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp89B;});goto _LL348;}else{_LL34B: _LL34C:
 goto _LL348;}_LL348:;}
# 1836
goto _LL31F;};};}case 5: _LL322: _tmp4A4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL323:
# 1839
({struct Cyc_PP_Doc*_tmp89E=({struct Cyc_PP_Doc*_tmp464[2];({struct Cyc_PP_Doc*_tmp89C=Cyc_PP_text(({const char*_tmp465=";";_tag_dyneither(_tmp465,sizeof(char),2);}));_tmp464[1]=_tmp89C;});({struct Cyc_PP_Doc*_tmp89D=Cyc_Absynpp_aggrdecl2doc(_tmp4A4);_tmp464[0]=_tmp89D;});Cyc_PP_cat(_tag_dyneither(_tmp464,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp89E;});
goto _LL31F;case 0: _LL324: _tmp4A5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL325:
# 1842
({struct Cyc_PP_Doc*_tmp89F=Cyc_Absynpp_vardecl2doc(_tmp4A5);s=_tmp89F;});
goto _LL31F;case 4: _LL326: _tmp4A8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_tmp4A7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp455)->f2;_tmp4A6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp455)->f3;_LL327:
# 1845
({struct Cyc_PP_Doc*_tmp8AA=({struct Cyc_PP_Doc*_tmp466[7];({struct Cyc_PP_Doc*_tmp8A0=
# 1852
Cyc_PP_text(({const char*_tmp46D=";";_tag_dyneither(_tmp46D,sizeof(char),2);}));_tmp466[6]=_tmp8A0;});({struct Cyc_PP_Doc*_tmp8A4=
# 1850
(unsigned int)_tmp4A6?({struct Cyc_PP_Doc*_tmp46A[3];({struct Cyc_PP_Doc*_tmp8A1=
Cyc_PP_text(({const char*_tmp46C=")";_tag_dyneither(_tmp46C,sizeof(char),2);}));_tmp46A[2]=_tmp8A1;});({struct Cyc_PP_Doc*_tmp8A2=
# 1850
Cyc_Absynpp_exp2doc(_tmp4A6);_tmp46A[1]=_tmp8A2;});({struct Cyc_PP_Doc*_tmp8A3=Cyc_PP_text(({const char*_tmp46B=" = open(";_tag_dyneither(_tmp46B,sizeof(char),9);}));_tmp46A[0]=_tmp8A3;});Cyc_PP_cat(_tag_dyneither(_tmp46A,sizeof(struct Cyc_PP_Doc*),3));}):
 Cyc_PP_nil_doc();_tmp466[5]=_tmp8A4;});({struct Cyc_PP_Doc*_tmp8A5=
# 1849
Cyc_Absynpp_qvar2doc(_tmp4A7->name);_tmp466[4]=_tmp8A5;});({struct Cyc_PP_Doc*_tmp8A6=
# 1848
Cyc_PP_text(({const char*_tmp469=">";_tag_dyneither(_tmp469,sizeof(char),2);}));_tmp466[3]=_tmp8A6;});({struct Cyc_PP_Doc*_tmp8A7=
# 1847
Cyc_Absynpp_tvar2doc(_tmp4A8);_tmp466[2]=_tmp8A7;});({struct Cyc_PP_Doc*_tmp8A8=
# 1846
Cyc_PP_text(({const char*_tmp468="<";_tag_dyneither(_tmp468,sizeof(char),2);}));_tmp466[1]=_tmp8A8;});({struct Cyc_PP_Doc*_tmp8A9=
# 1845
Cyc_PP_text(({const char*_tmp467="region";_tag_dyneither(_tmp467,sizeof(char),7);}));_tmp466[0]=_tmp8A9;});Cyc_PP_cat(_tag_dyneither(_tmp466,sizeof(struct Cyc_PP_Doc*),7));});s=_tmp8AA;});
# 1853
goto _LL31F;case 6: _LL328: _tmp4A9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL329:
# 1855
({struct Cyc_PP_Doc*_tmp8AD=({struct Cyc_PP_Doc*_tmp46E[2];({struct Cyc_PP_Doc*_tmp8AB=Cyc_PP_text(({const char*_tmp46F=";";_tag_dyneither(_tmp46F,sizeof(char),2);}));_tmp46E[1]=_tmp8AB;});({struct Cyc_PP_Doc*_tmp8AC=Cyc_Absynpp_datatypedecl2doc(_tmp4A9);_tmp46E[0]=_tmp8AC;});Cyc_PP_cat(_tag_dyneither(_tmp46E,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp8AD;});
goto _LL31F;case 2: _LL32A: _tmp4AB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_tmp4AA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp455)->f3;_LL32B:
# 1858
({struct Cyc_PP_Doc*_tmp8B3=({struct Cyc_PP_Doc*_tmp470[5];({struct Cyc_PP_Doc*_tmp8AE=
# 1862
Cyc_PP_text(({const char*_tmp473=";";_tag_dyneither(_tmp473,sizeof(char),2);}));_tmp470[4]=_tmp8AE;});({struct Cyc_PP_Doc*_tmp8AF=
# 1861
Cyc_Absynpp_exp2doc(_tmp4AA);_tmp470[3]=_tmp8AF;});({struct Cyc_PP_Doc*_tmp8B0=
# 1860
Cyc_PP_text(({const char*_tmp472=" = ";_tag_dyneither(_tmp472,sizeof(char),4);}));_tmp470[2]=_tmp8B0;});({struct Cyc_PP_Doc*_tmp8B1=
# 1859
Cyc_Absynpp_pat2doc(_tmp4AB);_tmp470[1]=_tmp8B1;});({struct Cyc_PP_Doc*_tmp8B2=
# 1858
Cyc_PP_text(({const char*_tmp471="let ";_tag_dyneither(_tmp471,sizeof(char),5);}));_tmp470[0]=_tmp8B2;});Cyc_PP_cat(_tag_dyneither(_tmp470,sizeof(struct Cyc_PP_Doc*),5));});s=_tmp8B3;});
# 1863
goto _LL31F;case 3: _LL32C: _tmp4AC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL32D:
# 1865
({struct Cyc_PP_Doc*_tmp8B7=({struct Cyc_PP_Doc*_tmp474[3];({struct Cyc_PP_Doc*_tmp8B4=Cyc_PP_text(({const char*_tmp476=";";_tag_dyneither(_tmp476,sizeof(char),2);}));_tmp474[2]=_tmp8B4;});({struct Cyc_PP_Doc*_tmp8B5=Cyc_Absynpp_ids2doc(_tmp4AC);_tmp474[1]=_tmp8B5;});({struct Cyc_PP_Doc*_tmp8B6=Cyc_PP_text(({const char*_tmp475="let ";_tag_dyneither(_tmp475,sizeof(char),5);}));_tmp474[0]=_tmp8B6;});Cyc_PP_cat(_tag_dyneither(_tmp474,sizeof(struct Cyc_PP_Doc*),3));});s=_tmp8B7;});
goto _LL31F;case 7: _LL32E: _tmp4AD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL32F:
# 1868
({struct Cyc_PP_Doc*_tmp8BA=({struct Cyc_PP_Doc*_tmp477[2];({struct Cyc_PP_Doc*_tmp8B8=Cyc_PP_text(({const char*_tmp478=";";_tag_dyneither(_tmp478,sizeof(char),2);}));_tmp477[1]=_tmp8B8;});({struct Cyc_PP_Doc*_tmp8B9=Cyc_Absynpp_enumdecl2doc(_tmp4AD);_tmp477[0]=_tmp8B9;});Cyc_PP_cat(_tag_dyneither(_tmp477,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp8BA;});
goto _LL31F;case 8: _LL330: _tmp4AE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL331: {
# 1871
void*t;
if(_tmp4AE->defn != 0)
t=(void*)_check_null(_tmp4AE->defn);else{
# 1875
({void*_tmp8BB=Cyc_Absyn_new_evar(_tmp4AE->kind,0);t=_tmp8BB;});}
({struct Cyc_PP_Doc*_tmp8C5=({struct Cyc_PP_Doc*_tmp479[4];({struct Cyc_PP_Doc*_tmp8BC=
# 1883
Cyc_PP_text(({const char*_tmp47D=";";_tag_dyneither(_tmp47D,sizeof(char),2);}));_tmp479[3]=_tmp8BC;});({struct Cyc_PP_Doc*_tmp8BD=
# 1882
Cyc_Absynpp_atts2doc(_tmp4AE->atts);_tmp479[2]=_tmp8BD;});({struct Cyc_PP_Doc*_tmp8C3=({
# 1877
struct Cyc_Absyn_Tqual _tmp8C2=_tmp4AE->tq;void*_tmp8C1=t;Cyc_Absynpp_tqtd2doc(_tmp8C2,_tmp8C1,({struct Cyc_Core_Opt*_tmp47B=_cycalloc(sizeof(*_tmp47B));({struct Cyc_PP_Doc*_tmp8C0=({struct Cyc_PP_Doc*_tmp47C[2];({struct Cyc_PP_Doc*_tmp8BE=
# 1880
Cyc_Absynpp_tvars2doc(_tmp4AE->tvs);_tmp47C[1]=_tmp8BE;});({struct Cyc_PP_Doc*_tmp8BF=
# 1879
Cyc_Absynpp_typedef_name2bolddoc(_tmp4AE->name);_tmp47C[0]=_tmp8BF;});Cyc_PP_cat(_tag_dyneither(_tmp47C,sizeof(struct Cyc_PP_Doc*),2));});_tmp47B->v=_tmp8C0;});_tmp47B;}));});_tmp479[1]=_tmp8C3;});({struct Cyc_PP_Doc*_tmp8C4=
# 1876
Cyc_PP_text(({const char*_tmp47A="typedef ";_tag_dyneither(_tmp47A,sizeof(char),9);}));_tmp479[0]=_tmp8C4;});Cyc_PP_cat(_tag_dyneither(_tmp479,sizeof(struct Cyc_PP_Doc*),4));});s=_tmp8C5;});
# 1885
goto _LL31F;}case 9: _LL332: _tmp4B0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_tmp4AF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp455)->f2;_LL333:
# 1887
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4B0);
({struct Cyc_PP_Doc*_tmp8CF=({struct Cyc_PP_Doc*_tmp47E[8];({struct Cyc_PP_Doc*_tmp8C6=
# 1894
Cyc_Absynpp_rb();_tmp47E[7]=_tmp8C6;});({struct Cyc_PP_Doc*_tmp8C7=
# 1893
Cyc_PP_line_doc();_tmp47E[6]=_tmp8C7;});({struct Cyc_PP_Doc*_tmp8C9=({
# 1892
struct _dyneither_ptr _tmp8C8=({const char*_tmp480="";_tag_dyneither(_tmp480,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8C8,_tmp4AF);});_tmp47E[5]=_tmp8C9;});({struct Cyc_PP_Doc*_tmp8CA=
# 1891
Cyc_PP_line_doc();_tmp47E[4]=_tmp8CA;});({struct Cyc_PP_Doc*_tmp8CB=
# 1890
Cyc_Absynpp_lb();_tmp47E[3]=_tmp8CB;});({struct Cyc_PP_Doc*_tmp8CC=Cyc_PP_blank_doc();_tmp47E[2]=_tmp8CC;});({struct Cyc_PP_Doc*_tmp8CD=
# 1889
Cyc_PP_textptr(_tmp4B0);_tmp47E[1]=_tmp8CD;});({struct Cyc_PP_Doc*_tmp8CE=
# 1888
Cyc_PP_text(({const char*_tmp47F="namespace ";_tag_dyneither(_tmp47F,sizeof(char),11);}));_tmp47E[0]=_tmp8CE;});Cyc_PP_cat(_tag_dyneither(_tmp47E,sizeof(struct Cyc_PP_Doc*),8));});s=_tmp8CF;});
# 1895
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL31F;case 10: _LL334: _tmp4B2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_tmp4B1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp455)->f2;_LL335:
# 1898
 if(Cyc_Absynpp_print_using_stmts)
({struct Cyc_PP_Doc*_tmp8D9=({struct Cyc_PP_Doc*_tmp481[8];({struct Cyc_PP_Doc*_tmp8D0=
# 1905
Cyc_Absynpp_rb();_tmp481[7]=_tmp8D0;});({struct Cyc_PP_Doc*_tmp8D1=
# 1904
Cyc_PP_line_doc();_tmp481[6]=_tmp8D1;});({struct Cyc_PP_Doc*_tmp8D3=({
# 1903
struct _dyneither_ptr _tmp8D2=({const char*_tmp483="";_tag_dyneither(_tmp483,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8D2,_tmp4B1);});_tmp481[5]=_tmp8D3;});({struct Cyc_PP_Doc*_tmp8D4=
# 1902
Cyc_PP_line_doc();_tmp481[4]=_tmp8D4;});({struct Cyc_PP_Doc*_tmp8D5=
# 1901
Cyc_Absynpp_lb();_tmp481[3]=_tmp8D5;});({struct Cyc_PP_Doc*_tmp8D6=Cyc_PP_blank_doc();_tmp481[2]=_tmp8D6;});({struct Cyc_PP_Doc*_tmp8D7=
# 1900
Cyc_Absynpp_qvar2doc(_tmp4B2);_tmp481[1]=_tmp8D7;});({struct Cyc_PP_Doc*_tmp8D8=
# 1899
Cyc_PP_text(({const char*_tmp482="using ";_tag_dyneither(_tmp482,sizeof(char),7);}));_tmp481[0]=_tmp8D8;});Cyc_PP_cat(_tag_dyneither(_tmp481,sizeof(struct Cyc_PP_Doc*),8));});s=_tmp8D9;});else{
# 1907
({struct Cyc_PP_Doc*_tmp8E6=({struct Cyc_PP_Doc*_tmp484[11];({struct Cyc_PP_Doc*_tmp8DA=
# 1917
Cyc_PP_text(({const char*_tmp489=" */";_tag_dyneither(_tmp489,sizeof(char),4);}));_tmp484[10]=_tmp8DA;});({struct Cyc_PP_Doc*_tmp8DB=
# 1916
Cyc_Absynpp_rb();_tmp484[9]=_tmp8DB;});({struct Cyc_PP_Doc*_tmp8DC=
# 1915
Cyc_PP_text(({const char*_tmp488="/* ";_tag_dyneither(_tmp488,sizeof(char),4);}));_tmp484[8]=_tmp8DC;});({struct Cyc_PP_Doc*_tmp8DD=
# 1914
Cyc_PP_line_doc();_tmp484[7]=_tmp8DD;});({struct Cyc_PP_Doc*_tmp8DF=({
# 1913
struct _dyneither_ptr _tmp8DE=({const char*_tmp487="";_tag_dyneither(_tmp487,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8DE,_tmp4B1);});_tmp484[6]=_tmp8DF;});({struct Cyc_PP_Doc*_tmp8E0=
# 1912
Cyc_PP_line_doc();_tmp484[5]=_tmp8E0;});({struct Cyc_PP_Doc*_tmp8E1=
# 1911
Cyc_PP_text(({const char*_tmp486=" */";_tag_dyneither(_tmp486,sizeof(char),4);}));_tmp484[4]=_tmp8E1;});({struct Cyc_PP_Doc*_tmp8E2=
# 1910
Cyc_Absynpp_lb();_tmp484[3]=_tmp8E2;});({struct Cyc_PP_Doc*_tmp8E3=
# 1909
Cyc_PP_blank_doc();_tmp484[2]=_tmp8E3;});({struct Cyc_PP_Doc*_tmp8E4=
# 1908
Cyc_Absynpp_qvar2doc(_tmp4B2);_tmp484[1]=_tmp8E4;});({struct Cyc_PP_Doc*_tmp8E5=
# 1907
Cyc_PP_text(({const char*_tmp485="/* using ";_tag_dyneither(_tmp485,sizeof(char),10);}));_tmp484[0]=_tmp8E5;});Cyc_PP_cat(_tag_dyneither(_tmp484,sizeof(struct Cyc_PP_Doc*),11));});s=_tmp8E6;});}
# 1918
goto _LL31F;case 11: _LL336: _tmp4B3=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_LL337:
# 1920
 if(Cyc_Absynpp_print_externC_stmts)
({struct Cyc_PP_Doc*_tmp8EE=({struct Cyc_PP_Doc*_tmp48A[6];({struct Cyc_PP_Doc*_tmp8E7=
# 1926
Cyc_Absynpp_rb();_tmp48A[5]=_tmp8E7;});({struct Cyc_PP_Doc*_tmp8E8=
# 1925
Cyc_PP_line_doc();_tmp48A[4]=_tmp8E8;});({struct Cyc_PP_Doc*_tmp8EA=({
# 1924
struct _dyneither_ptr _tmp8E9=({const char*_tmp48C="";_tag_dyneither(_tmp48C,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8E9,_tmp4B3);});_tmp48A[3]=_tmp8EA;});({struct Cyc_PP_Doc*_tmp8EB=
# 1923
Cyc_PP_line_doc();_tmp48A[2]=_tmp8EB;});({struct Cyc_PP_Doc*_tmp8EC=
# 1922
Cyc_Absynpp_lb();_tmp48A[1]=_tmp8EC;});({struct Cyc_PP_Doc*_tmp8ED=
# 1921
Cyc_PP_text(({const char*_tmp48B="extern \"C\" ";_tag_dyneither(_tmp48B,sizeof(char),12);}));_tmp48A[0]=_tmp8ED;});Cyc_PP_cat(_tag_dyneither(_tmp48A,sizeof(struct Cyc_PP_Doc*),6));});s=_tmp8EE;});else{
# 1928
({struct Cyc_PP_Doc*_tmp8F9=({struct Cyc_PP_Doc*_tmp48D[9];({struct Cyc_PP_Doc*_tmp8EF=
# 1936
Cyc_PP_text(({const char*_tmp492=" */";_tag_dyneither(_tmp492,sizeof(char),4);}));_tmp48D[8]=_tmp8EF;});({struct Cyc_PP_Doc*_tmp8F0=
# 1935
Cyc_Absynpp_rb();_tmp48D[7]=_tmp8F0;});({struct Cyc_PP_Doc*_tmp8F1=
# 1934
Cyc_PP_text(({const char*_tmp491="/* ";_tag_dyneither(_tmp491,sizeof(char),4);}));_tmp48D[6]=_tmp8F1;});({struct Cyc_PP_Doc*_tmp8F2=
# 1933
Cyc_PP_line_doc();_tmp48D[5]=_tmp8F2;});({struct Cyc_PP_Doc*_tmp8F4=({
# 1932
struct _dyneither_ptr _tmp8F3=({const char*_tmp490="";_tag_dyneither(_tmp490,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8F3,_tmp4B3);});_tmp48D[4]=_tmp8F4;});({struct Cyc_PP_Doc*_tmp8F5=
# 1931
Cyc_PP_line_doc();_tmp48D[3]=_tmp8F5;});({struct Cyc_PP_Doc*_tmp8F6=
# 1930
Cyc_PP_text(({const char*_tmp48F=" */";_tag_dyneither(_tmp48F,sizeof(char),4);}));_tmp48D[2]=_tmp8F6;});({struct Cyc_PP_Doc*_tmp8F7=
# 1929
Cyc_Absynpp_lb();_tmp48D[1]=_tmp8F7;});({struct Cyc_PP_Doc*_tmp8F8=
# 1928
Cyc_PP_text(({const char*_tmp48E="/* extern \"C\" ";_tag_dyneither(_tmp48E,sizeof(char),15);}));_tmp48D[0]=_tmp8F8;});Cyc_PP_cat(_tag_dyneither(_tmp48D,sizeof(struct Cyc_PP_Doc*),9));});s=_tmp8F9;});}
# 1937
goto _LL31F;case 12: _LL338: _tmp4B5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp455)->f1;_tmp4B4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp455)->f2;_LL339:
# 1939
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4B4 != 0)
({struct Cyc_PP_Doc*_tmp902=({struct Cyc_PP_Doc*_tmp493[7];({struct Cyc_PP_Doc*_tmp8FA=
# 1944
Cyc_Absynpp_rb();_tmp493[6]=_tmp8FA;});({struct Cyc_PP_Doc*_tmp8FB=Cyc_PP_line_doc();_tmp493[5]=_tmp8FB;});({struct Cyc_PP_Doc*_tmp8FD=({
# 1943
struct _dyneither_ptr _tmp8FC=({const char*_tmp495=",";_tag_dyneither(_tmp495,sizeof(char),2);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp8FC,_tmp4B4);});_tmp493[4]=_tmp8FD;});({struct Cyc_PP_Doc*_tmp8FE=Cyc_PP_line_doc();_tmp493[3]=_tmp8FE;});({struct Cyc_PP_Doc*_tmp8FF=
# 1942
Cyc_Absynpp_lb();_tmp493[2]=_tmp8FF;});({struct Cyc_PP_Doc*_tmp900=Cyc_PP_text(({const char*_tmp494=" export ";_tag_dyneither(_tmp494,sizeof(char),9);}));_tmp493[1]=_tmp900;});({struct Cyc_PP_Doc*_tmp901=Cyc_Absynpp_rb();_tmp493[0]=_tmp901;});Cyc_PP_cat(_tag_dyneither(_tmp493,sizeof(struct Cyc_PP_Doc*),7));});exs_doc=_tmp902;});else{
# 1946
({struct Cyc_PP_Doc*_tmp903=Cyc_Absynpp_rb();exs_doc=_tmp903;});}
({struct Cyc_PP_Doc*_tmp90A=({struct Cyc_PP_Doc*_tmp496[6];_tmp496[5]=exs_doc;({struct Cyc_PP_Doc*_tmp904=
# 1951
Cyc_PP_line_doc();_tmp496[4]=_tmp904;});({struct Cyc_PP_Doc*_tmp906=({
# 1950
struct _dyneither_ptr _tmp905=({const char*_tmp498="";_tag_dyneither(_tmp498,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp905,_tmp4B5);});_tmp496[3]=_tmp906;});({struct Cyc_PP_Doc*_tmp907=
# 1949
Cyc_PP_line_doc();_tmp496[2]=_tmp907;});({struct Cyc_PP_Doc*_tmp908=
# 1948
Cyc_Absynpp_lb();_tmp496[1]=_tmp908;});({struct Cyc_PP_Doc*_tmp909=
# 1947
Cyc_PP_text(({const char*_tmp497="extern \"C include\" ";_tag_dyneither(_tmp497,sizeof(char),20);}));_tmp496[0]=_tmp909;});Cyc_PP_cat(_tag_dyneither(_tmp496,sizeof(struct Cyc_PP_Doc*),6));});s=_tmp90A;});}else{
# 1954
({struct Cyc_PP_Doc*_tmp915=({struct Cyc_PP_Doc*_tmp499[9];({struct Cyc_PP_Doc*_tmp90B=
# 1962
Cyc_PP_text(({const char*_tmp49E=" */";_tag_dyneither(_tmp49E,sizeof(char),4);}));_tmp499[8]=_tmp90B;});({struct Cyc_PP_Doc*_tmp90C=
# 1961
Cyc_Absynpp_rb();_tmp499[7]=_tmp90C;});({struct Cyc_PP_Doc*_tmp90D=
# 1960
Cyc_PP_text(({const char*_tmp49D="/* ";_tag_dyneither(_tmp49D,sizeof(char),4);}));_tmp499[6]=_tmp90D;});({struct Cyc_PP_Doc*_tmp90E=
# 1959
Cyc_PP_line_doc();_tmp499[5]=_tmp90E;});({struct Cyc_PP_Doc*_tmp910=({
# 1958
struct _dyneither_ptr _tmp90F=({const char*_tmp49C="";_tag_dyneither(_tmp49C,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp90F,_tmp4B5);});_tmp499[4]=_tmp910;});({struct Cyc_PP_Doc*_tmp911=
# 1957
Cyc_PP_line_doc();_tmp499[3]=_tmp911;});({struct Cyc_PP_Doc*_tmp912=
# 1956
Cyc_PP_text(({const char*_tmp49B=" */";_tag_dyneither(_tmp49B,sizeof(char),4);}));_tmp499[2]=_tmp912;});({struct Cyc_PP_Doc*_tmp913=
# 1955
Cyc_Absynpp_lb();_tmp499[1]=_tmp913;});({struct Cyc_PP_Doc*_tmp914=
# 1954
Cyc_PP_text(({const char*_tmp49A="/* extern \"C include\" ";_tag_dyneither(_tmp49A,sizeof(char),23);}));_tmp499[0]=_tmp914;});Cyc_PP_cat(_tag_dyneither(_tmp499,sizeof(struct Cyc_PP_Doc*),9));});s=_tmp915;});}
# 1963
goto _LL31F;case 13: _LL33A: _LL33B:
# 1965
({struct Cyc_PP_Doc*_tmp918=({struct Cyc_PP_Doc*_tmp49F[2];({struct Cyc_PP_Doc*_tmp916=Cyc_Absynpp_lb();_tmp49F[1]=_tmp916;});({struct Cyc_PP_Doc*_tmp917=Cyc_PP_text(({const char*_tmp4A0="__cyclone_port_on__;";_tag_dyneither(_tmp4A0,sizeof(char),21);}));_tmp49F[0]=_tmp917;});Cyc_PP_cat(_tag_dyneither(_tmp49F,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp918;});
goto _LL31F;default: _LL33C: _LL33D:
# 1968
({struct Cyc_PP_Doc*_tmp91B=({struct Cyc_PP_Doc*_tmp4A1[2];({struct Cyc_PP_Doc*_tmp919=Cyc_Absynpp_lb();_tmp4A1[1]=_tmp919;});({struct Cyc_PP_Doc*_tmp91A=Cyc_PP_text(({const char*_tmp4A2="__cyclone_port_off__;";_tag_dyneither(_tmp4A2,sizeof(char),22);}));_tmp4A1[0]=_tmp91A;});Cyc_PP_cat(_tag_dyneither(_tmp4A1,sizeof(struct Cyc_PP_Doc*),2));});s=_tmp91B;});
goto _LL31F;}_LL31F:;}
# 1972
return s;}
# 1975
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4B6=sc;switch(_tmp4B6){case Cyc_Absyn_Static: _LL34E: _LL34F:
 return Cyc_PP_text(({const char*_tmp4B7="static ";_tag_dyneither(_tmp4B7,sizeof(char),8);}));case Cyc_Absyn_Public: _LL350: _LL351:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL352: _LL353:
 return Cyc_PP_text(({const char*_tmp4B8="extern ";_tag_dyneither(_tmp4B8,sizeof(char),8);}));case Cyc_Absyn_ExternC: _LL354: _LL355:
 return Cyc_PP_text(({const char*_tmp4B9="extern \"C\" ";_tag_dyneither(_tmp4B9,sizeof(char),12);}));case Cyc_Absyn_Abstract: _LL356: _LL357:
 return Cyc_PP_text(({const char*_tmp4BA="abstract ";_tag_dyneither(_tmp4BA,sizeof(char),10);}));default: _LL358: _LL359:
 return Cyc_PP_text(({const char*_tmp4BB="register ";_tag_dyneither(_tmp4BB,sizeof(char),10);}));}_LL34D:;};}
# 1988
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4BC=t;struct Cyc_List_List*_tmp4BE;struct Cyc_Absyn_Tvar*_tmp4BD;switch(*((int*)_tmp4BC)){case 2: _LL35B: _tmp4BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4BC)->f1;_LL35C:
 return Cyc_Tcutil_is_temp_tvar(_tmp4BD);case 24: _LL35D: _tmp4BE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4BC)->f1;_LL35E:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4BE);default: _LL35F: _LL360:
 return 0;}_LL35A:;}
# 2000
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4BF=t;struct Cyc_Absyn_Typedefdecl*_tmp4C1;void*_tmp4C0;switch(*((int*)_tmp4BF)){case 12: _LL362: _LL363:
 return 1;case 14: _LL364: _LL365:
 return 1;case 17: _LL366: _tmp4C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f3;_tmp4C0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f4;if(_tmp4C0 != 0){_LL367:
# 2007
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4C0);}else{goto _LL368;}default: _LL368: _LL369:
 return 0;}_LL361:;}
# 2014
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2017
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4C2=({struct _tuple16 _tmp4C7;_tmp4C7.f1=(void*)tms->hd;_tmp4C7.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp4C7;});struct _tuple16 _tmp4C3=_tmp4C2;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4C3.f1)->tag == 2){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4C3.f2)->tag == 3){_LL36B: _LL36C:
# 2020
 return({struct Cyc_List_List*_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4));_tmp4C4->hd=(void*)tms->hd;({struct Cyc_List_List*_tmp91D=({struct Cyc_List_List*_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5));_tmp4C5->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;({struct Cyc_List_List*_tmp91C=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4C5->tl=_tmp91C;});_tmp4C5;});_tmp4C4->tl=_tmp91D;});_tmp4C4;});}else{goto _LL36D;}}else{_LL36D: _LL36E:
 return({struct Cyc_List_List*_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6));_tmp4C6->hd=atts;_tmp4C6->tl=tms;_tmp4C6;});}_LL36A:;}else{
# 2023
return({struct Cyc_List_List*_tmp4C8=_region_malloc(r,sizeof(*_tmp4C8));_tmp4C8->hd=atts;_tmp4C8->tl=tms;_tmp4C8;});}}
# 2028
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2030
void*_tmp4C9=t;struct _tuple0*_tmp516;struct Cyc_List_List*_tmp515;struct Cyc_Absyn_Typedefdecl*_tmp514;void*_tmp513;struct Cyc_Core_Opt*_tmp512;void*_tmp511;int _tmp510;struct Cyc_List_List*_tmp50F;void*_tmp50E;struct Cyc_Absyn_Tqual _tmp50D;void*_tmp50C;struct Cyc_List_List*_tmp50B;int _tmp50A;struct Cyc_Absyn_VarargInfo*_tmp509;struct Cyc_List_List*_tmp508;struct Cyc_List_List*_tmp507;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp505;void*_tmp504;struct Cyc_Absyn_Tqual _tmp503;struct Cyc_Absyn_PtrAtts _tmp502;void*_tmp501;struct Cyc_Absyn_Tqual _tmp500;struct Cyc_Absyn_Exp*_tmp4FF;union Cyc_Absyn_Constraint*_tmp4FE;unsigned int _tmp4FD;switch(*((int*)_tmp4C9)){case 8: _LL370: _tmp501=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).elt_type;_tmp500=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).tq;_tmp4FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).num_elts;_tmp4FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zero_term;_tmp4FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zt_loc;_LL371: {
# 2033
struct _tuple12 _tmp4CA=Cyc_Absynpp_to_tms(r,_tmp500,_tmp501);struct _tuple12 _tmp4CB=_tmp4CA;struct Cyc_Absyn_Tqual _tmp4D4;void*_tmp4D3;struct Cyc_List_List*_tmp4D2;_LL37D: _tmp4D4=_tmp4CB.f1;_tmp4D3=_tmp4CB.f2;_tmp4D2=_tmp4CB.f3;_LL37E:;{
void*tm;
if(_tmp4FF == 0)
({void*_tmp91F=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp91E=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp4CD;_tmp4CD.tag=0;_tmp4CD.f1=_tmp4FE;_tmp4CD.f2=_tmp4FD;_tmp4CD;});_tmp4CC[0]=_tmp91E;});_tmp4CC;});tm=_tmp91F;});else{
# 2038
({void*_tmp921=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp920=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp4CF;_tmp4CF.tag=1;_tmp4CF.f1=_tmp4FF;_tmp4CF.f2=_tmp4FE;_tmp4CF.f3=_tmp4FD;_tmp4CF;});_tmp4CE[0]=_tmp920;});_tmp4CE;});tm=_tmp921;});}
return({struct _tuple12 _tmp4D0;_tmp4D0.f1=_tmp4D4;_tmp4D0.f2=_tmp4D3;({struct Cyc_List_List*_tmp922=({struct Cyc_List_List*_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1));_tmp4D1->hd=tm;_tmp4D1->tl=_tmp4D2;_tmp4D1;});_tmp4D0.f3=_tmp922;});_tmp4D0;});};}case 5: _LL372: _tmp504=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_typ;_tmp503=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_tq;_tmp502=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).ptr_atts;_LL373: {
# 2042
struct _tuple12 _tmp4D5=Cyc_Absynpp_to_tms(r,_tmp503,_tmp504);struct _tuple12 _tmp4D6=_tmp4D5;struct Cyc_Absyn_Tqual _tmp4DD;void*_tmp4DC;struct Cyc_List_List*_tmp4DB;_LL380: _tmp4DD=_tmp4D6.f1;_tmp4DC=_tmp4D6.f2;_tmp4DB=_tmp4D6.f3;_LL381:;
({struct Cyc_List_List*_tmp925=({struct Cyc_List_List*_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7));({void*_tmp924=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8));({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp923=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp4D9;_tmp4D9.tag=2;_tmp4D9.f1=_tmp502;_tmp4D9.f2=tq;_tmp4D9;});_tmp4D8[0]=_tmp923;});_tmp4D8;});_tmp4D7->hd=_tmp924;});_tmp4D7->tl=_tmp4DB;_tmp4D7;});_tmp4DB=_tmp925;});
return({struct _tuple12 _tmp4DA;_tmp4DA.f1=_tmp4DD;_tmp4DA.f2=_tmp4DC;_tmp4DA.f3=_tmp4DB;_tmp4DA;});}case 9: _LL374: _tmp50F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).tvars;_tmp50E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).effect;_tmp50D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_tqual;_tmp50C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_typ;_tmp50B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).args;_tmp50A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).c_varargs;_tmp509=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).cyc_varargs;_tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).rgn_po;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).attributes;_tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).requires_clause;_tmp505=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ensures_clause;_LL375:
# 2048
 if(!Cyc_Absynpp_print_all_tvars){
# 2052
if(_tmp50E == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp50E)){
_tmp50E=0;
_tmp50F=0;}}else{
# 2057
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2060
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp50F);}{
# 2065
struct _tuple12 _tmp4DE=Cyc_Absynpp_to_tms(r,_tmp50D,_tmp50C);struct _tuple12 _tmp4DF=_tmp4DE;struct Cyc_Absyn_Tqual _tmp4F9;void*_tmp4F8;struct Cyc_List_List*_tmp4F7;_LL383: _tmp4F9=_tmp4DF.f1;_tmp4F8=_tmp4DF.f2;_tmp4F7=_tmp4DF.f3;_LL384:;{
struct Cyc_List_List*tms=_tmp4F7;
# 2076 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4E0=Cyc_Cyclone_c_compiler;if(_tmp4E0 == Cyc_Cyclone_Gcc_c){_LL386: _LL387:
# 2078
 if(_tmp507 != 0)
# 2081
({struct Cyc_List_List*_tmp929=({struct _RegionHandle*_tmp928=r;void*_tmp927=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp926=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4E2;_tmp4E2.tag=5;_tmp4E2.f1=0;_tmp4E2.f2=_tmp507;_tmp4E2;});_tmp4E1[0]=_tmp926;});_tmp4E1;});Cyc_Absynpp_bubble_attributes(_tmp928,_tmp927,tms);});tms=_tmp929;});
# 2083
({struct Cyc_List_List*_tmp92E=({struct Cyc_List_List*_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3));({void*_tmp92D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp92C=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp4E5;_tmp4E5.tag=3;({void*_tmp92B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp92A=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp4E7;_tmp4E7.tag=1;_tmp4E7.f1=_tmp50B;_tmp4E7.f2=_tmp50A;_tmp4E7.f3=_tmp509;_tmp4E7.f4=_tmp50E;_tmp4E7.f5=_tmp508;_tmp4E7.f6=_tmp506;_tmp4E7.f7=_tmp505;_tmp4E7;});_tmp4E6[0]=_tmp92A;});_tmp4E6;});_tmp4E5.f1=_tmp92B;});_tmp4E5;});_tmp4E4[0]=_tmp92C;});_tmp4E4;});_tmp4E3->hd=_tmp92D;});_tmp4E3->tl=tms;_tmp4E3;});tms=_tmp92E;});
# 2087
goto _LL385;}else{_LL388: _LL389:
# 2089
({struct Cyc_List_List*_tmp933=({struct Cyc_List_List*_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8));({void*_tmp932=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp931=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp4EA;_tmp4EA.tag=3;({void*_tmp930=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp92F=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp4EC;_tmp4EC.tag=1;_tmp4EC.f1=_tmp50B;_tmp4EC.f2=_tmp50A;_tmp4EC.f3=_tmp509;_tmp4EC.f4=_tmp50E;_tmp4EC.f5=_tmp508;_tmp4EC.f6=_tmp506;_tmp4EC.f7=_tmp505;_tmp4EC;});_tmp4EB[0]=_tmp92F;});_tmp4EB;});_tmp4EA.f1=_tmp930;});_tmp4EA;});_tmp4E9[0]=_tmp931;});_tmp4E9;});_tmp4E8->hd=_tmp932;});_tmp4E8->tl=tms;_tmp4E8;});tms=_tmp933;});
# 2093
for(0;_tmp507 != 0;_tmp507=_tmp507->tl){
void*_tmp4ED=(void*)_tmp507->hd;void*_tmp4EE=_tmp4ED;switch(*((int*)_tmp4EE)){case 1: _LL38B: _LL38C:
 goto _LL38E;case 2: _LL38D: _LL38E:
 goto _LL390;case 3: _LL38F: _LL390:
# 2098
({struct Cyc_List_List*_tmp937=({struct Cyc_List_List*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));({void*_tmp936=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp935=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4F1;_tmp4F1.tag=5;_tmp4F1.f1=0;({struct Cyc_List_List*_tmp934=({struct Cyc_List_List*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->hd=(void*)_tmp507->hd;_tmp4F2->tl=0;_tmp4F2;});_tmp4F1.f2=_tmp934;});_tmp4F1;});_tmp4F0[0]=_tmp935;});_tmp4F0;});_tmp4EF->hd=_tmp936;});_tmp4EF->tl=tms;_tmp4EF;});tms=_tmp937;});
goto AfterAtts;default: _LL391: _LL392:
 goto _LL38A;}_LL38A:;}
# 2102
goto _LL385;}_LL385:;}
# 2105
AfterAtts:
 if(_tmp50F != 0)
({struct Cyc_List_List*_tmp93A=({struct Cyc_List_List*_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3));({void*_tmp939=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp938=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp4F5;_tmp4F5.tag=4;_tmp4F5.f1=_tmp50F;_tmp4F5.f2=0;_tmp4F5.f3=1;_tmp4F5;});_tmp4F4[0]=_tmp938;});_tmp4F4;});_tmp4F3->hd=_tmp939;});_tmp4F3->tl=tms;_tmp4F3;});tms=_tmp93A;});
return({struct _tuple12 _tmp4F6;_tmp4F6.f1=_tmp4F9;_tmp4F6.f2=_tmp4F8;_tmp4F6.f3=tms;_tmp4F6;});};};case 1: _LL376: _tmp512=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f1;_tmp511=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f2;_tmp510=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f3;_LL377:
# 2111
 if(_tmp511 == 0)
return({struct _tuple12 _tmp4FA;_tmp4FA.f1=tq;_tmp4FA.f2=t;_tmp4FA.f3=0;_tmp4FA;});else{
# 2114
return Cyc_Absynpp_to_tms(r,tq,_tmp511);}case 17: _LL378: _tmp516=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f1;_tmp515=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f2;_tmp514=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp513=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;_LL379:
# 2119
 if((_tmp513 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp513))
return({struct _tuple12 _tmp4FB;_tmp4FB.f1=tq;_tmp4FB.f2=t;_tmp4FB.f3=0;_tmp4FB;});else{
# 2122
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp513);}default: _LL37A: _LL37B:
# 2127
 return({struct _tuple12 _tmp4FC;_tmp4FC.f1=tq;_tmp4FC.f2=t;_tmp4FC.f3=0;_tmp4FC;});}_LL36F:;}
# 2131
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2133
void*_tmp517=t;void*_tmp51C;void*_tmp51B;switch(*((int*)_tmp517)){case 1: _LL394: _tmp51B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp517)->f2;if(_tmp51B != 0){_LL395:
 return Cyc_Absynpp_is_char_ptr(_tmp51B);}else{goto _LL398;}case 5: _LL396: _tmp51C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).elt_typ;_LL397:
# 2136
 L: {
void*_tmp518=_tmp51C;void*_tmp51A;void*_tmp519;switch(*((int*)_tmp518)){case 1: _LL39B: _tmp519=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp518)->f2;if(_tmp519 != 0){_LL39C:
 _tmp51C=_tmp519;goto L;}else{goto _LL3A1;}case 17: _LL39D: _tmp51A=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp518)->f4;if(_tmp51A != 0){_LL39E:
 _tmp51C=_tmp51A;goto L;}else{goto _LL3A1;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp518)->f2 == Cyc_Absyn_Char_sz){_LL39F: _LL3A0:
 return 1;}else{goto _LL3A1;}default: _LL3A1: _LL3A2:
 return 0;}_LL39A:;}default: _LL398: _LL399:
# 2143
 return 0;}_LL393:;}
# 2147
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51D=_new_region("temp");struct _RegionHandle*temp=& _tmp51D;_push_region(temp);
{struct _tuple12 _tmp51E=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp51F=_tmp51E;struct Cyc_Absyn_Tqual _tmp527;void*_tmp526;struct Cyc_List_List*_tmp525;_LL3A4: _tmp527=_tmp51F.f1;_tmp526=_tmp51F.f2;_tmp525=_tmp51F.f3;_LL3A5:;
({struct Cyc_List_List*_tmp93B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp525);_tmp525=_tmp93B;});
if(_tmp525 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp521=({struct Cyc_PP_Doc*_tmp520[2];({struct Cyc_PP_Doc*_tmp93C=Cyc_Absynpp_ntyp2doc(_tmp526);_tmp520[1]=_tmp93C;});({struct Cyc_PP_Doc*_tmp93D=Cyc_Absynpp_tqual2doc(_tmp527);_tmp520[0]=_tmp93D;});Cyc_PP_cat(_tag_dyneither(_tmp520,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _tmp521;}else{
# 2154
struct Cyc_PP_Doc*_tmp524=({struct Cyc_PP_Doc*_tmp522[4];({struct Cyc_PP_Doc*_tmp940=({
# 2158
int _tmp93F=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp93E=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp93F,_tmp93E,_tmp525);});_tmp522[3]=_tmp940;});({struct Cyc_PP_Doc*_tmp941=
# 2157
Cyc_PP_text(({const char*_tmp523=" ";_tag_dyneither(_tmp523,sizeof(char),2);}));_tmp522[2]=_tmp941;});({struct Cyc_PP_Doc*_tmp942=
# 2156
Cyc_Absynpp_ntyp2doc(_tmp526);_tmp522[1]=_tmp942;});({struct Cyc_PP_Doc*_tmp943=
# 2155
Cyc_Absynpp_tqual2doc(_tmp527);_tmp522[0]=_tmp943;});Cyc_PP_cat(_tag_dyneither(_tmp522,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _tmp524;}}
# 2149
;_pop_region(temp);}
# 2162
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp528=f->requires_clause;
if((unsigned int)_tmp528)
({struct Cyc_PP_Doc*_tmp946=({struct Cyc_PP_Doc*_tmp529[2];({struct Cyc_PP_Doc*_tmp944=Cyc_Absynpp_exp2doc(_tmp528);_tmp529[1]=_tmp944;});({struct Cyc_PP_Doc*_tmp945=Cyc_PP_text(({const char*_tmp52A="@requires ";_tag_dyneither(_tmp52A,sizeof(char),11);}));_tmp529[0]=_tmp945;});Cyc_PP_cat(_tag_dyneither(_tmp529,sizeof(struct Cyc_PP_Doc*),2));});requires_doc=_tmp946;});else{
# 2168
({struct Cyc_PP_Doc*_tmp947=Cyc_PP_nil_doc();requires_doc=_tmp947;});}{
# 2170
enum Cyc_Cyclone_C_Compilers _tmp52B=Cyc_Cyclone_c_compiler;if(_tmp52B == Cyc_Cyclone_Gcc_c){_LL3A7: _LL3A8:
# 2173
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52C[5];({struct Cyc_PP_Doc*_tmp948=
# 2176
Cyc_PP_text(({const char*_tmp52F=";";_tag_dyneither(_tmp52F,sizeof(char),2);}));_tmp52C[4]=_tmp948;});({struct Cyc_PP_Doc*_tmp949=Cyc_Absynpp_atts2doc(f->attributes);_tmp52C[3]=_tmp949;});({struct Cyc_PP_Doc*_tmp94A=
# 2175
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52C[2]=_tmp94A;});({struct Cyc_PP_Doc*_tmp94B=Cyc_PP_text(({const char*_tmp52E=":";_tag_dyneither(_tmp52E,sizeof(char),2);}));_tmp52C[1]=_tmp94B;});({struct Cyc_PP_Doc*_tmp94F=({
# 2174
struct Cyc_Absyn_Tqual _tmp94E=f->tq;void*_tmp94D=f->type;Cyc_Absynpp_tqtd2doc(_tmp94E,_tmp94D,({struct Cyc_Core_Opt*_tmp52D=_cycalloc(sizeof(*_tmp52D));({struct Cyc_PP_Doc*_tmp94C=Cyc_PP_textptr(f->name);_tmp52D->v=_tmp94C;});_tmp52D;}));});_tmp52C[0]=_tmp94F;});Cyc_PP_cat(_tag_dyneither(_tmp52C,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2178
return({struct Cyc_PP_Doc*_tmp530[4];({struct Cyc_PP_Doc*_tmp950=
Cyc_PP_text(({const char*_tmp532=";";_tag_dyneither(_tmp532,sizeof(char),2);}));_tmp530[3]=_tmp950;});_tmp530[2]=requires_doc;({struct Cyc_PP_Doc*_tmp951=Cyc_Absynpp_atts2doc(f->attributes);_tmp530[1]=_tmp951;});({struct Cyc_PP_Doc*_tmp955=({
# 2178
struct Cyc_Absyn_Tqual _tmp954=f->tq;void*_tmp953=f->type;Cyc_Absynpp_tqtd2doc(_tmp954,_tmp953,({struct Cyc_Core_Opt*_tmp531=_cycalloc(sizeof(*_tmp531));({struct Cyc_PP_Doc*_tmp952=Cyc_PP_textptr(f->name);_tmp531->v=_tmp952;});_tmp531;}));});_tmp530[0]=_tmp955;});Cyc_PP_cat(_tag_dyneither(_tmp530,sizeof(struct Cyc_PP_Doc*),4));});}}else{_LL3A9: _LL3AA:
# 2181
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp533[5];({struct Cyc_PP_Doc*_tmp956=
# 2184
Cyc_PP_text(({const char*_tmp536=";";_tag_dyneither(_tmp536,sizeof(char),2);}));_tmp533[4]=_tmp956;});({struct Cyc_PP_Doc*_tmp957=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp533[3]=_tmp957;});({struct Cyc_PP_Doc*_tmp958=Cyc_PP_text(({const char*_tmp535=":";_tag_dyneither(_tmp535,sizeof(char),2);}));_tmp533[2]=_tmp958;});({struct Cyc_PP_Doc*_tmp95C=({
# 2183
struct Cyc_Absyn_Tqual _tmp95B=f->tq;void*_tmp95A=f->type;Cyc_Absynpp_tqtd2doc(_tmp95B,_tmp95A,({struct Cyc_Core_Opt*_tmp534=_cycalloc(sizeof(*_tmp534));({struct Cyc_PP_Doc*_tmp959=Cyc_PP_textptr(f->name);_tmp534->v=_tmp959;});_tmp534;}));});_tmp533[1]=_tmp95C;});({struct Cyc_PP_Doc*_tmp95D=
# 2182
Cyc_Absynpp_atts2doc(f->attributes);_tmp533[0]=_tmp95D;});Cyc_PP_cat(_tag_dyneither(_tmp533,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2186
return({struct Cyc_PP_Doc*_tmp537[4];({struct Cyc_PP_Doc*_tmp95E=
# 2188
Cyc_PP_text(({const char*_tmp539=";";_tag_dyneither(_tmp539,sizeof(char),2);}));_tmp537[3]=_tmp95E;});_tmp537[2]=requires_doc;({struct Cyc_PP_Doc*_tmp962=({
# 2187
struct Cyc_Absyn_Tqual _tmp961=f->tq;void*_tmp960=f->type;Cyc_Absynpp_tqtd2doc(_tmp961,_tmp960,({struct Cyc_Core_Opt*_tmp538=_cycalloc(sizeof(*_tmp538));({struct Cyc_PP_Doc*_tmp95F=Cyc_PP_textptr(f->name);_tmp538->v=_tmp95F;});_tmp538;}));});_tmp537[1]=_tmp962;});({struct Cyc_PP_Doc*_tmp963=
# 2186
Cyc_Absynpp_atts2doc(f->attributes);_tmp537[0]=_tmp963;});Cyc_PP_cat(_tag_dyneither(_tmp537,sizeof(struct Cyc_PP_Doc*),4));});}}_LL3A6:;};}
# 2193
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp964=({const char*_tmp53A="";_tag_dyneither(_tmp53A,sizeof(char),1);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp964,fields);});}
# 2197
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp53B[3];({struct Cyc_PP_Doc*_tmp965=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp53B[2]=_tmp965;});({struct Cyc_PP_Doc*_tmp966=
# 2198
Cyc_Absynpp_typedef_name2doc(f->name);_tmp53B[1]=_tmp966;});({struct Cyc_PP_Doc*_tmp967=Cyc_Absynpp_scope2doc(f->sc);_tmp53B[0]=_tmp967;});Cyc_PP_cat(_tag_dyneither(_tmp53B,sizeof(struct Cyc_PP_Doc*),3));});}
# 2202
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp968=({const char*_tmp53C=",";_tag_dyneither(_tmp53C,sizeof(char),2);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp968,fields);});}
# 2211
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp969=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp969,72,f);});
({void*_tmp53D=0;({struct Cyc___cycFILE*_tmp96B=f;struct _dyneither_ptr _tmp96A=({const char*_tmp53E="\n";_tag_dyneither(_tmp53E,sizeof(char),2);});Cyc_fprintf(_tmp96B,_tmp96A,_tag_dyneither(_tmp53D,sizeof(void*),0));});});}}
# 2218
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _dyneither_ptr _tmp96C=({const char*_tmp53F="";_tag_dyneither(_tmp53F,sizeof(char),1);});Cyc_PP_seql(_tmp96C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2221
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2225
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
# 2236
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

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
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 49 "buffer.h"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 72
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _dyneither_ptr,int offset,int len);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);char Cyc_Arg_Bad[4U]="Bad";char Cyc_Arg_Error[6U]="Error";struct Cyc_Arg_Prefix_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Exact_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple0{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _dyneither_ptr x){
while(l != 0){
struct _dyneither_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned long _tmp1=Cyc_strlen((struct _dyneither_ptr)_tmp0);
if(_tmp1 > 0  && (*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}else{
# 76
if(Cyc_strcmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}
l=l->tl;}
# 80
(int)_throw((void*)({struct Cyc_Core_Not_found_exn_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_Core_Not_found_exn_struct _tmp63=({struct Cyc_Core_Not_found_exn_struct _tmp3;_tmp3.tag=Cyc_Core_Not_found;_tmp3;});_tmp2[0]=_tmp63;});_tmp2;}));}
# 89
static struct _dyneither_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _dyneither_ptr s){
if(({char*_tmp64=(char*)s.curr;_tmp64 == (char*)(_tag_dyneither(0,0,0)).curr;}))return _tag_dyneither(0,0,0);
if(howmuch < 0)howmuch=0;{
unsigned long _tmp4=Cyc_strlen((struct _dyneither_ptr)s);
_tmp4 <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmp65=({const char*_tmp5="len <= numelts(s)";_tag_dyneither(_tmp5,sizeof(char),18U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp65,({const char*_tmp6="arg.cyc";_tag_dyneither(_tmp6,sizeof(char),8U);}),93U);});
if(howmuch > _tmp4){
Cyc_Buffer_add_string(b,s);
return _tag_dyneither(0,0,0);}{
# 100
int i;
for(i=howmuch - 1;i >= 0  && ! isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));-- i){
;}
# 105
if(i < 0)
for(i=howmuch?howmuch - 1: 0;(i < _tmp4  && (int)((const char*)s.curr)[i]) && ! isspace((int)((const char*)s.curr)[i]);++ i){
;}
# 111
Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _dyneither_ptr whatsleft=_tag_dyneither(0,0,0);
# 116
for(0;(i < _tmp4  && (int)((const char*)s.curr)[i]) &&  isspace((int)((const char*)s.curr)[i]);++ i){
;}
if(i < _tmp4  && (int)((const char*)s.curr)[i])({struct _dyneither_ptr _tmp66=_dyneither_ptr_plus(s,sizeof(char),i);whatsleft=_tmp66;});
return whatsleft;};};};}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc){
# 129
if(({char*_tmp67=(char*)item.curr;_tmp67 != (char*)(_tag_dyneither(0,0,0)).curr;}))Cyc_Buffer_add_string(b,item);
if(({char*_tmp68=(char*)desc.curr;_tmp68 == (char*)(_tag_dyneither(0,0,0)).curr;}))return;
if(indent < 0)indent=0;
if(margin < 0)margin=0;{
# 135
struct _dyneither_ptr indentstr=({unsigned int _tmp13=(unsigned int)(indent + 2);char*_tmp14=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp13 + 1U));struct _dyneither_ptr _tmp16=_tag_dyneither(_tmp14,sizeof(char),_tmp13 + 1U);{unsigned int _tmp15=_tmp13;unsigned int i;for(i=0;i < _tmp15;i ++){_tmp14[i]=(char)'\000';}_tmp14[_tmp15]=(char)0U;}_tmp16;});
{unsigned int i=0U;for(0;i < indent + 1;++ i){
({struct _dyneither_ptr _tmp7=_dyneither_ptr_plus(indentstr,sizeof(char),(int)i);char _tmp8=*((char*)_check_dyneither_subscript(_tmp7,sizeof(char),0U));char _tmp9=i == 0?'\n':' ';if(_get_dyneither_size(_tmp7,sizeof(char))== 1U  && (_tmp8 == '\000'  && _tmp9 != '\000'))_throw_arraybounds();*((char*)_tmp7.curr)=_tmp9;});}}{
unsigned long _tmpA=Cyc_strlen((struct _dyneither_ptr)item);
struct _dyneither_ptr itemsep;
if(Cyc_strlen((struct _dyneither_ptr)desc)> 0){
if(_tmpA + 1 > indent)
itemsep=indentstr;else{
# 144
struct _dyneither_ptr temp=({unsigned int _tmpE=(indent - _tmpA)+ 1;char*_tmpF=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpE + 1U));struct _dyneither_ptr _tmp11=_tag_dyneither(_tmpF,sizeof(char),_tmpE + 1U);{unsigned int _tmp10=_tmpE;unsigned int i;for(i=0;i < _tmp10;i ++){_tmpF[i]=(char)'\000';}_tmpF[_tmp10]=(char)0U;}_tmp11;});
{unsigned int i=0U;for(0;i < indent - _tmpA;++ i){({struct _dyneither_ptr _tmpB=_dyneither_ptr_plus(temp,sizeof(char),(int)i);char _tmpC=*((char*)_check_dyneither_subscript(_tmpB,sizeof(char),0U));char _tmpD=' ';if(_get_dyneither_size(_tmpB,sizeof(char))== 1U  && (_tmpC == '\000'  && _tmpD != '\000'))_throw_arraybounds();*((char*)_tmpB.curr)=_tmpD;});}}
itemsep=temp;}}else{
# 149
return;}
# 151
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)itemsep);
# 153
while(({char*_tmp69=(char*)desc.curr;_tmp69 != (char*)(_tag_dyneither(0,0,0)).curr;})){
({struct _dyneither_ptr _tmp6A=Cyc_Arg_Justify_break_line(b,margin - indent,desc);desc=_tmp6A;});
if(({char*_tmp6B=(char*)desc.curr;_tmp6B != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)indentstr);else{
# 158
({struct Cyc_Buffer_t*_tmp6C=b;Cyc_Buffer_add_string(_tmp6C,({const char*_tmp12="\n";_tag_dyneither(_tmp12,sizeof(char),2U);}));});}}
# 160
return;};};}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 166
({struct Cyc_String_pa_PrintArg_struct _tmp19;_tmp19.tag=0U;_tmp19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)errmsg);({void*_tmp17[1U]={& _tmp19};({struct Cyc___cycFILE*_tmp6E=Cyc_stderr;struct _dyneither_ptr _tmp6D=({const char*_tmp18="%s\n";_tag_dyneither(_tmp18,sizeof(char),4U);});Cyc_fprintf(_tmp6E,_tmp6D,_tag_dyneither(_tmp17,sizeof(void*),1U));});});});{
struct Cyc_Buffer_t*_tmp1A=Cyc_Buffer_create(1024U);
while(speclist != 0){
({struct Cyc_Buffer_t*_tmp70=_tmp1A;struct _dyneither_ptr _tmp6F=(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f1,(struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f3);
# 169
Cyc_Arg_Justify_justify_b(_tmp70,12,72,_tmp6F,(*((struct _tuple0*)speclist->hd)).f5);});
# 172
speclist=speclist->tl;}
# 174
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0U;({struct _dyneither_ptr _tmp71=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Buffer_contents(_tmp1A));_tmp1D.f1=_tmp71;});({void*_tmp1B[1U]={& _tmp1D};({struct Cyc___cycFILE*_tmp73=Cyc_stderr;struct _dyneither_ptr _tmp72=({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3U);});Cyc_fprintf(_tmp73,_tmp72,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});});};}
# 177
int Cyc_Arg_current=0;
static struct _dyneither_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 183
struct _dyneither_ptr progname=prog_pos < _get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr))?*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),prog_pos)):({const char*_tmp38="(?)";_tag_dyneither(_tmp38,sizeof(char),4U);});
{void*_tmp1E=e;struct _dyneither_ptr _tmp37;struct _dyneither_ptr _tmp36;struct _dyneither_ptr _tmp35;struct _dyneither_ptr _tmp34;struct _dyneither_ptr _tmp33;struct _dyneither_ptr _tmp32;switch(*((int*)_tmp1E)){case 0U: _LL1: _tmp32=((struct Cyc_Arg_Unknown_Arg_error_struct*)_tmp1E)->f1;_LL2:
# 186
 if(({struct _dyneither_ptr _tmp74=(struct _dyneither_ptr)_tmp32;Cyc_strcmp(_tmp74,({const char*_tmp1F="-help";_tag_dyneither(_tmp1F,sizeof(char),6U);}));})!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp23;_tmp23.tag=0U;_tmp23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32);({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0U;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp20[2U]={& _tmp22,& _tmp23};({struct Cyc___cycFILE*_tmp76=Cyc_stderr;struct _dyneither_ptr _tmp75=({const char*_tmp21="%s: unknown option `%s'.\n";_tag_dyneither(_tmp21,sizeof(char),26U);});Cyc_fprintf(_tmp76,_tmp75,_tag_dyneither(_tmp20,sizeof(void*),2U));});});});});
goto _LL0;case 1U: _LL3: _tmp33=((struct Cyc_Arg_Missing_Arg_error_struct*)_tmp1E)->f1;_LL4:
# 190
({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0U;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33);({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0U;_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp24[2U]={& _tmp26,& _tmp27};({struct Cyc___cycFILE*_tmp78=Cyc_stderr;struct _dyneither_ptr _tmp77=({const char*_tmp25="%s: option `%s' needs an argument.\n";_tag_dyneither(_tmp25,sizeof(char),36U);});Cyc_fprintf(_tmp78,_tmp77,_tag_dyneither(_tmp24,sizeof(void*),2U));});});});});
goto _LL0;case 3U: _LL5: _tmp36=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E)->f1;_tmp35=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E)->f2;_tmp34=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E)->f3;_LL6:
# 193
({struct Cyc_String_pa_PrintArg_struct _tmp2D;_tmp2D.tag=0U;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp2C;_tmp2C.tag=0U;_tmp2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36);({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0U;_tmp2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35);({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0U;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp28[4U]={& _tmp2A,& _tmp2B,& _tmp2C,& _tmp2D};({struct Cyc___cycFILE*_tmp7A=Cyc_stderr;struct _dyneither_ptr _tmp79=({const char*_tmp29="%s: wrong argument `%s'; option `%s' expects %s.\n";_tag_dyneither(_tmp29,sizeof(char),50U);});Cyc_fprintf(_tmp7A,_tmp79,_tag_dyneither(_tmp28,sizeof(void*),4U));});});});});});});
# 195
goto _LL0;default: _LL7: _tmp37=((struct Cyc_Arg_Message_Arg_error_struct*)_tmp1E)->f1;_LL8:
# 197
({struct Cyc_String_pa_PrintArg_struct _tmp31;_tmp31.tag=0U;_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37);({struct Cyc_String_pa_PrintArg_struct _tmp30;_tmp30.tag=0U;_tmp30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp2E[2U]={& _tmp30,& _tmp31};({struct Cyc___cycFILE*_tmp7C=Cyc_stderr;struct _dyneither_ptr _tmp7B=({const char*_tmp2F="%s: %s.\n";_tag_dyneither(_tmp2F,sizeof(char),9U);});Cyc_fprintf(_tmp7C,_tmp7B,_tag_dyneither(_tmp2E,sizeof(void*),2U));});});});});
goto _LL0;}_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);
Cyc_Arg_current=(int)_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr orig_args){
# 209
Cyc_Arg_args=orig_args;{
# 211
int initpos=Cyc_Arg_current;
unsigned int l=_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));
# 214
if(({char*_tmp7D=(char*)((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),(int)(l - 1)))->curr;_tmp7D == (char*)(_tag_dyneither(0,0,0)).curr;}))l=l - 1;
++ Cyc_Arg_current;
while(Cyc_Arg_current < l){
struct _dyneither_ptr s=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current));
if((({char*_tmp7E=(char*)s.curr;_tmp7E != (char*)(_tag_dyneither(0,0,0)).curr;}) && _get_dyneither_size(s,sizeof(char))>= 1) && ((const char*)s.curr)[0]== '-'){
void*action;
{struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(_tmp39.handler))_tmp3B=1;if(!_tmp3B){({void*_tmp7F=Cyc_Arg_lookup(speclist,s);action=_tmp7F;});;_pop_handler();}else{void*_tmp3A=(void*)_exn_thrown;void*_tmp3C=_tmp3A;void*_tmp3F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp3C)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 224
 if(!anonflagfun(s)){
({int _tmp83=initpos;void*_tmp82=(void*)({struct Cyc_Arg_Unknown_Arg_error_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_Arg_Unknown_Arg_error_struct _tmp80=({struct Cyc_Arg_Unknown_Arg_error_struct _tmp3E;_tmp3E.tag=0U;_tmp3E.f1=s;_tmp3E;});_tmp3D[0]=_tmp80;});_tmp3D;});struct Cyc_List_List*_tmp81=speclist;Cyc_Arg_stop(_tmp83,_tmp82,_tmp81,errmsg);});
return;}else{
# 229
++ Cyc_Arg_current;
continue;}}else{_LL3: _tmp3F=_tmp3C;_LL4:(int)_rethrow(_tmp3F);}_LL0:;}};}
# 233
{struct _handler_cons _tmp40;_push_handler(& _tmp40);{int _tmp42=0;if(setjmp(_tmp40.handler))_tmp42=1;if(!_tmp42){
{void*_tmp43=action;void(*_tmp55)(struct _dyneither_ptr);void(*_tmp54)(int);void(*_tmp53)(struct _dyneither_ptr);void(*_tmp52)(struct _dyneither_ptr,struct _dyneither_ptr);int*_tmp51;int*_tmp50;void(*_tmp4F)(struct _dyneither_ptr);void(*_tmp4E)();switch(*((int*)_tmp43)){case 0U: _LL6: _tmp4E=((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_tmp43)->f1;_LL7:
 _tmp4E();goto _LL5;case 1U: _LL8: _tmp4F=((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_tmp43)->f1;_LL9:
 _tmp4F(s);goto _LL5;case 3U: _LLA: _tmp50=((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_tmp43)->f1;_LLB:
*_tmp50=1;goto _LL5;case 4U: _LLC: _tmp51=((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_tmp43)->f1;_LLD:
*_tmp51=0;goto _LL5;case 2U: _LLE: _tmp52=((struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_tmp43)->f1;_LLF:
# 240
 if(Cyc_Arg_current + 1 < l){
_tmp52(s,*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 244
({int _tmp87=initpos;void*_tmp86=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp44=_cycalloc(sizeof(*_tmp44));({struct Cyc_Arg_Missing_Arg_error_struct _tmp84=({struct Cyc_Arg_Missing_Arg_error_struct _tmp45;_tmp45.tag=1U;_tmp45.f1=s;_tmp45;});_tmp44[0]=_tmp84;});_tmp44;});struct Cyc_List_List*_tmp85=speclist;Cyc_Arg_stop(_tmp87,_tmp86,_tmp85,errmsg);});}
goto _LL5;case 5U: _LL10: _tmp53=((struct Cyc_Arg_String_spec_Arg_Spec_struct*)_tmp43)->f1;_LL11:
# 247
 if(Cyc_Arg_current + 1 < l){
_tmp53(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 251
({int _tmp8B=initpos;void*_tmp8A=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp46=_cycalloc(sizeof(*_tmp46));({struct Cyc_Arg_Missing_Arg_error_struct _tmp88=({struct Cyc_Arg_Missing_Arg_error_struct _tmp47;_tmp47.tag=1U;_tmp47.f1=s;_tmp47;});_tmp46[0]=_tmp88;});_tmp46;});struct Cyc_List_List*_tmp89=speclist;Cyc_Arg_stop(_tmp8B,_tmp8A,_tmp89,errmsg);});}
goto _LL5;case 6U: _LL12: _tmp54=((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_tmp43)->f1;_LL13: {
# 254
struct _dyneither_ptr arg=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1));
int n=0;
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp4A;_tmp4A.tag=2U;_tmp4A.f1=& n;({void*_tmp48[1U]={& _tmp4A};({struct _dyneither_ptr _tmp8D=arg;struct _dyneither_ptr _tmp8C=({const char*_tmp49="%d";_tag_dyneither(_tmp49,sizeof(char),3U);});Cyc_sscanf(_tmp8D,_tmp8C,_tag_dyneither(_tmp48,sizeof(void*),1U));});});})!= 1){
({int _tmp92=initpos;void*_tmp91=(void*)({struct Cyc_Arg_Wrong_Arg_error_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));({struct Cyc_Arg_Wrong_Arg_error_struct _tmp8F=({struct Cyc_Arg_Wrong_Arg_error_struct _tmp4C;_tmp4C.tag=3U;_tmp4C.f1=s;_tmp4C.f2=arg;({struct _dyneither_ptr _tmp8E=({const char*_tmp4D="an integer";_tag_dyneither(_tmp4D,sizeof(char),11U);});_tmp4C.f3=_tmp8E;});_tmp4C;});_tmp4B[0]=_tmp8F;});_tmp4B;});struct Cyc_List_List*_tmp90=speclist;Cyc_Arg_stop(_tmp92,_tmp91,_tmp90,errmsg);});
_npop_handler(0U);return;}
# 260
_tmp54(n);
++ Cyc_Arg_current;
goto _LL5;}default: _LL14: _tmp55=((struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_tmp43)->f1;_LL15:
# 264
 while(Cyc_Arg_current < l - 1){
_tmp55(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}
# 268
goto _LL5;}_LL5:;}
# 234
;_pop_handler();}else{void*_tmp41=(void*)_exn_thrown;void*_tmp56=_tmp41;void*_tmp5A;struct _dyneither_ptr _tmp59;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp56)->tag == Cyc_Arg_Bad){_LL17: _tmp59=((struct Cyc_Arg_Bad_exn_struct*)_tmp56)->f1;_LL18:
# 271
({int _tmp96=initpos;void*_tmp95=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp57=_cycalloc(sizeof(*_tmp57));({struct Cyc_Arg_Message_Arg_error_struct _tmp93=({struct Cyc_Arg_Message_Arg_error_struct _tmp58;_tmp58.tag=2U;_tmp58.f1=_tmp59;_tmp58;});_tmp57[0]=_tmp93;});_tmp57;});struct Cyc_List_List*_tmp94=speclist;Cyc_Arg_stop(_tmp96,_tmp95,_tmp94,errmsg);});goto _LL16;}else{_LL19: _tmp5A=_tmp56;_LL1A:(int)_rethrow(_tmp5A);}_LL16:;}};}
# 273
++ Cyc_Arg_current;}else{
# 276
{struct _handler_cons _tmp5B;_push_handler(& _tmp5B);{int _tmp5D=0;if(setjmp(_tmp5B.handler))_tmp5D=1;if(!_tmp5D){anonfun(s);;_pop_handler();}else{void*_tmp5C=(void*)_exn_thrown;void*_tmp5E=_tmp5C;void*_tmp62;struct _dyneither_ptr _tmp61;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp5E)->tag == Cyc_Arg_Bad){_LL1C: _tmp61=((struct Cyc_Arg_Bad_exn_struct*)_tmp5E)->f1;_LL1D:
# 278
({int _tmp9A=initpos;void*_tmp99=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct Cyc_Arg_Message_Arg_error_struct _tmp97=({struct Cyc_Arg_Message_Arg_error_struct _tmp60;_tmp60.tag=2U;_tmp60.f1=_tmp61;_tmp60;});_tmp5F[0]=_tmp97;});_tmp5F;});struct Cyc_List_List*_tmp98=speclist;Cyc_Arg_stop(_tmp9A,_tmp99,_tmp98,errmsg);});goto _LL1B;}else{_LL1E: _tmp62=_tmp5E;_LL1F:(int)_rethrow(_tmp62);}_LL1B:;}};}
# 280
++ Cyc_Arg_current;}}};}

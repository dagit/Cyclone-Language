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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
# 295 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 299
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 304
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 311
return 0;
return dyn.curr;};}
# 317
static unsigned int arr_prevsize(struct _dyneither_ptr arr,unsigned int elt_sz){
unsigned char*_get_arr_size_curr=arr.curr;
unsigned char*_get_arr_size_base=arr.base;
return
(_get_arr_size_curr < _get_arr_size_base  || _get_arr_size_curr >= arr.last_plus_one)?0:(_get_arr_size_curr - _get_arr_size_base)/ elt_sz;}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_PP_Ppstate;
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
# 75
typedef void*Cyc_Tcpat_decision_opt_t;
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
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 211
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 213
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 222
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 316
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 333
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 368
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 373
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 428 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 431
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 511
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 537
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 544
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 562
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 725 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 942
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 965
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 997
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1107
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1116
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);
# 34
typedef struct Cyc_List_List*Cyc_Binding_namespace_name_t;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};
# 39
typedef void*Cyc_Binding_nsdirective_t;struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 45
typedef struct Cyc_Binding_NSCtxt*Cyc_Binding_nsctxt_t;
# 47
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 40 "binding.cyc"
typedef struct Cyc_Dict_Dict Cyc_Binding_dict_t;
typedef unsigned int Cyc_Binding_seg_t;char Cyc_Binding_BindingError[13]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 45
int Cyc_Binding_warn_override=0;struct Cyc_Binding_NSCtxt;
# 59
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->curr_ns=0;_tmp1->availables=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp4;_tmp4.tag=0;_tmp4.f1=0;_tmp4;});_tmp3;});_tmp2->tl=0;_tmp2;});_tmp1->ns_data=
# 63
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp1;});}
# 65
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp5=abs_ns;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;switch((_tmp5.Abs_n).tag){case 3: _LL1: _tmp8=(_tmp5.C_n).val;_LL2:
 _tmp9=_tmp8;goto _LL4;case 2: _LL3: _tmp9=(_tmp5.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp9);default: _LL5: _LL6:
({void*_tmp6=0;Cyc_Warn_impos(({const char*_tmp7="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp7,sizeof(char),33);}),_tag_dyneither(_tmp6,sizeof(void*),0));});}_LL0:;}
# 83 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 86
struct Cyc_List_List*_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpA))
return _tmpA;
{struct Cyc_List_List*_tmpB=ctxt->availables;for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmpC=(void*)_tmpB->hd;void*_tmpD=_tmpC;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmpF;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpD)->tag == 1){_LL8: _tmpF=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpD)->f1;_LL9:
 _tmp10=_tmpF;goto _LLB;}else{_LLA: _tmp10=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpD)->f1;_LLB: {
# 93
struct Cyc_List_List*_tmpE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp10,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpE))
return _tmpE;
goto _LL7;}}_LL7:;}}
# 98
({struct Cyc_String_pa_PrintArg_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(rel_ns,({const char*_tmp14="::";_tag_dyneither(_tmp14,sizeof(char),3);})));({void*_tmp11[1]={& _tmp13};Cyc_Warn_err(loc,({const char*_tmp12="namespace %s not found";_tag_dyneither(_tmp12,sizeof(char),23);}),_tag_dyneither(_tmp11,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp15=_cycalloc_atomic(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp16;_tmp16.tag=Cyc_Binding_BindingError;_tmp16;});_tmp15;}));}
# 103
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp17=qv;union Cyc_Absyn_Nmspace _tmp43;struct _dyneither_ptr*_tmp42;_LLD: _tmp43=_tmp17->f1;_tmp42=_tmp17->f2;_LLE:;{
union Cyc_Absyn_Nmspace _tmp18=_tmp43;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp3F;switch((_tmp18.Abs_n).tag){case 1: if((_tmp18.Rel_n).val == 0){_LL10: _LL11:
# 107
{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){{void*_tmp1C=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns),_tmp42);_npop_handler(0);return _tmp1C;};_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1D=_tmp1A;void*_tmp1E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1D)->tag == Cyc_Dict_Absent){_LL1B: _LL1C:
 goto _LL1A;}else{_LL1D: _tmp1E=_tmp1D;_LL1E:(int)_rethrow(_tmp1E);}_LL1A:;}};}
{struct Cyc_List_List*_tmp1F=ctxt->availables;for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
void*_tmp20=(void*)_tmp1F->hd;void*_tmp21=_tmp20;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp28;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp21)->tag == 1){_LL20: _tmp28=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp21)->f1;_LL21:
 _tmp29=_tmp28;goto _LL23;}else{_LL22: _tmp29=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp21)->f1;_LL23:
# 113
{struct _handler_cons _tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){{void*_tmp25=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp29),_tmp42);_npop_handler(0);return _tmp25;};_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp26=_tmp23;void*_tmp27;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp26)->tag == Cyc_Dict_Absent){_LL25: _LL26:
 goto _LL24;}else{_LL27: _tmp27=_tmp26;_LL28:(int)_rethrow(_tmp27);}_LL24:;}};}
goto _LL1F;}_LL1F:;}}
# 117
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2A=_cycalloc_atomic(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp2B;_tmp2B.tag=Cyc_Binding_BindingError;_tmp2B;});_tmp2A;}));}else{_LL12: _tmp3F=(_tmp18.Rel_n).val;_LL13: {
# 119
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct Cyc_List_List*_tmp2F=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3F);
void*_tmp30=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2F),_tmp42);_npop_handler(0);return _tmp30;}
# 120
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp31=_tmp2D;void*_tmp34;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent){_LL2A: _LL2B:
# 122
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp32=_cycalloc_atomic(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp33;_tmp33.tag=Cyc_Binding_BindingError;_tmp33;});_tmp32;}));}else{_LL2C: _tmp34=_tmp31;_LL2D:(int)_rethrow(_tmp34);}_LL29:;}};}}case 3: _LL14: _tmp40=(_tmp18.C_n).val;_LL15:
 _tmp41=_tmp40;goto _LL17;case 2: _LL16: _tmp41=(_tmp18.Abs_n).val;_LL17: {
# 125
struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){{void*_tmp38=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp41),_tmp42);_npop_handler(0);return _tmp38;};_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp39=_tmp36;void*_tmp3C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp39)->tag == Cyc_Dict_Absent){_LL2F: _LL30:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3A=_cycalloc_atomic(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp3B;_tmp3B.tag=Cyc_Binding_BindingError;_tmp3B;});_tmp3A;}));}else{_LL31: _tmp3C=_tmp39;_LL32:(int)_rethrow(_tmp3C);}_LL2E:;}};}default: _LL18: _LL19:
({void*_tmp3D=0;Cyc_Warn_impos(({const char*_tmp3E="lookup local in global";_tag_dyneither(_tmp3E,sizeof(char),23);}),_tag_dyneither(_tmp3D,sizeof(void*),0));});}_LLF:;};}
# 131
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp44=ctxt->curr_ns;
struct Cyc_List_List*_tmp45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp44,({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=subname;_tmp49->tl=0;_tmp49;}));
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp45))
ctxt->ns_data=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(ctxt->ns_data,_tmp45,mkdata(env));
ctxt->curr_ns=_tmp45;
ctxt->availables=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp48;_tmp48.tag=0;_tmp48.f1=_tmp45;_tmp48;});_tmp47;});_tmp46->tl=ctxt->availables;_tmp46;});}
# 139
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp4A=0;Cyc_Warn_impos(({const char*_tmp4B="leaving topmost namespace";_tag_dyneither(_tmp4B,sizeof(char),26);}),_tag_dyneither(_tmp4A,sizeof(void*),0));});{
void*_tmp4C=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp4D=_tmp4C;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp4D)->tag == 0){_LL34: _LL35:
# 144
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
ctxt->curr_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);
goto _LL33;}else{_LL36: _LL37:
# 148
({void*_tmp4E=0;Cyc_Warn_impos(({const char*_tmp4F="leaving using as namespace";_tag_dyneither(_tmp4F,sizeof(char),27);}),_tag_dyneither(_tmp4E,sizeof(void*),0));});}_LL33:;};}
# 151
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp50=usename;union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp5E;_LL39: _tmp5F=_tmp50->f1;_tmp5E=_tmp50->f2;_LL3A:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp51=_tmp5F;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp59;switch((_tmp51.Loc_n).tag){case 1: _LL3C: _tmp59=(_tmp51.Rel_n).val;_LL3D:
# 156
 ns=Cyc_Binding_resolve_rel_ns(loc,ctxt,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp59,({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=_tmp5E;_tmp52->tl=0;_tmp52;})));{
struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
(*usename).f1=Cyc_Absyn_Abs_n(_tmp53,0);
goto _LL3B;};case 2: _LL3E: _tmp5A=(_tmp51.Abs_n).val;_LL3F:
# 161
 ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5A,({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=_tmp5E;_tmp54->tl=0;_tmp54;}));
goto _LL3B;case 4: _LL40: _LL41:
({void*_tmp55=0;Cyc_Warn_impos(({const char*_tmp56="enter_using, Loc_n";_tag_dyneither(_tmp56,sizeof(char),19);}),_tag_dyneither(_tmp55,sizeof(void*),0));});default: _LL42: _LL43:
({void*_tmp57=0;Cyc_Warn_impos(({const char*_tmp58="enter_using, C_n";_tag_dyneither(_tmp58,sizeof(char),17);}),_tag_dyneither(_tmp57,sizeof(void*),0));});}_LL3B:;}
# 166
ctxt->availables=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->hd=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Binding_Using_Binding_NSDirective_struct _tmp5D;_tmp5D.tag=1;_tmp5D.f1=ns;_tmp5D;});_tmp5C;});_tmp5B->tl=ctxt->availables;_tmp5B;});
return ns;};}
# 169
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp60=0;Cyc_Warn_impos(({const char*_tmp61="leaving topmost namespace as a using";_tag_dyneither(_tmp61,sizeof(char),37);}),_tag_dyneither(_tmp60,sizeof(void*),0));});{
void*_tmp62=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp63=_tmp62;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp63)->tag == 1){_LL45: _LL46:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL44;}else{_LL47: _LL48:
({void*_tmp64=0;Cyc_Warn_impos(({const char*_tmp65="leaving namespace as using";_tag_dyneither(_tmp65,sizeof(char),27);}),_tag_dyneither(_tmp64,sizeof(void*),0));});}_LL44:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 194 "binding.cyc"
typedef void*Cyc_Binding_resolved_t;struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 203
typedef struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_renv_t;struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 211
typedef struct Cyc_Binding_Env*Cyc_Binding_env_t;
# 213
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 216
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 219
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->aggrdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->typedefs=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->ordinaries=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66;});}
# 227
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 230
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 233
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 236
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 239
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 242
static void*Cyc_Binding_lookup_ordinary(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp67=qv;union Cyc_Absyn_Nmspace _tmp6C;struct _dyneither_ptr*_tmp6B;_LL4A: _tmp6C=_tmp67->f1;_tmp6B=_tmp67->f2;_LL4B:;{
union Cyc_Absyn_Nmspace _tmp68=_tmp6C;switch((_tmp68.Rel_n).tag){case 4: _LL4D: _LL4E:
# 246
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp69=_cycalloc_atomic(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp6A;_tmp6A.tag=Cyc_Binding_BindingError;_tmp6A;});_tmp69;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B);case 1: if((_tmp68.Rel_n).val == 0){_LL4F: _LL50:
# 250
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B);
goto _LL52;}else{goto _LL51;}default: _LL51: _LL52:
# 254
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL4C:;};}
# 258
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned int,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_aggrfields(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_function_stuff(unsigned int,struct Cyc_Binding_Env*,void*,void*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 270
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int,unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 275
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp6D=(*qv).f1;union Cyc_Absyn_Nmspace _tmp6E=_tmp6D;switch((_tmp6E.Abs_n).tag){case 1: if((_tmp6E.Rel_n).val == 0){_LL54: _LL55:
# 278
 if(Cyc_Binding_at_toplevel(env))
(*qv).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);else{
# 281
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL53;}else{goto _LL5A;}case 4: _LL56: _LL57:
 goto _LL53;case 2: _LL58: _LL59:
# 285
 if(!Cyc_Binding_at_toplevel(env))
goto _LL5B;
goto _LL53;default: _LL5A: _LL5B:
# 289
({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(qv));({void*_tmp6F[1]={& _tmp71};Cyc_Warn_err(loc,({const char*_tmp70="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp70,sizeof(char),51);}),_tag_dyneither(_tmp6F,sizeof(void*),1));});});}_LL53:;}
# 296
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp72=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp72;}
# 303
void Cyc_Binding_check_warn_override(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _dyneither_ptr hides_what;
struct _handler_cons _tmp73;_push_handler(& _tmp73);{int _tmp75=0;if(setjmp(_tmp73.handler))_tmp75=1;if(!_tmp75){
{void*_tmp76=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp77=_tmp76;void*_tmp83;switch(*((int*)_tmp77)){case 0: _LL5D: _tmp83=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp77)->f1;_LL5E:
# 308
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0);return;}
{void*_tmp78=_tmp83;switch(*((int*)_tmp78)){case 1: _LL68: _LL69:
 hides_what=({const char*_tmp79="global variable";_tag_dyneither(_tmp79,sizeof(char),16);});goto _LL67;case 4: _LL6A: _LL6B:
 hides_what=({const char*_tmp7A="local variable";_tag_dyneither(_tmp7A,sizeof(char),15);});goto _LL67;case 3: _LL6C: _LL6D:
 hides_what=({const char*_tmp7B="parameter";_tag_dyneither(_tmp7B,sizeof(char),10);});goto _LL67;case 5: _LL6E: _LL6F:
 hides_what=({const char*_tmp7C="pattern variable";_tag_dyneither(_tmp7C,sizeof(char),17);});goto _LL67;case 2: _LL70: _LL71:
 hides_what=({const char*_tmp7D="function";_tag_dyneither(_tmp7D,sizeof(char),9);});goto _LL67;default: _LL72: _LL73:
({void*_tmp7E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(({const char*_tmp7F="shadowing free variable!";_tag_dyneither(_tmp7F,sizeof(char),25);}),_tag_dyneither(_tmp7E,sizeof(void*),0));});}_LL67:;}
# 318
goto _LL5C;case 1: _LL5F: _LL60:
 hides_what=({const char*_tmp80="struct constructor";_tag_dyneither(_tmp80,sizeof(char),19);});goto _LL5C;case 2: _LL61: _LL62:
 hides_what=({const char*_tmp81="datatype constructor";_tag_dyneither(_tmp81,sizeof(char),21);});goto _LL5C;case 3: _LL63: _LL64:
 goto _LL66;default: _LL65: _LL66:
 hides_what=({const char*_tmp82="enum tag";_tag_dyneither(_tmp82,sizeof(char),9);});goto _LL5C;}_LL5C:;}
# 324
({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)hides_what);({void*_tmp84[1]={& _tmp86};Cyc_Warn_warn(loc,({const char*_tmp85="declaration hides %s";_tag_dyneither(_tmp85,sizeof(char),21);}),_tag_dyneither(_tmp84,sizeof(void*),1));});});
_npop_handler(0);return;
# 306
;_pop_handler();}else{void*_tmp74=(void*)_exn_thrown;void*_tmp87=_tmp74;void*_tmp88;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp87)->tag == Cyc_Binding_BindingError){_LL75: _LL76:
# 326
 return;}else{_LL77: _tmp88=_tmp87;_LL78:(int)_rethrow(_tmp88);}_LL74:;}};}
# 329
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp89=vd->name;struct _tuple0*_tmp8A=_tmp89;union Cyc_Absyn_Nmspace _tmp95;struct _dyneither_ptr*_tmp94;_LL7A: _tmp95=_tmp8A->f1;_tmp94=_tmp8A->f2;_LL7B:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp94,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp8E;_tmp8E.tag=4;_tmp8E.f1=vd;_tmp8E;});_tmp8D;});_tmp8C;});_tmp8B;}));else{
# 339
struct Cyc_Binding_ResolveNSEnv*_tmp8F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp95);
_tmp8F->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp8F->ordinaries,_tmp94,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp93;_tmp93.tag=1;_tmp93.f1=vd;_tmp93;});_tmp92;});_tmp91;});_tmp90;}));}};}
# 346
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp96=s->r;void*_tmp97=_tmp96;struct Cyc_Absyn_Stmt*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Decl*_tmpAC;struct Cyc_Absyn_Stmt*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Stmt*_tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Stmt*_tmp9B;struct Cyc_Absyn_Stmt*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;switch(*((int*)_tmp97)){case 0: _LL7D: _LL7E:
 return;case 1: _LL7F: _tmp99=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL80:
 Cyc_Binding_resolve_exp(env,_tmp99);return;case 2: _LL81: _tmp9B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL82:
 Cyc_Binding_resolve_stmt(env,_tmp9B);Cyc_Binding_resolve_stmt(env,_tmp9A);return;case 3: _LL83: _tmp9C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL84:
 if((unsigned int)_tmp9C)Cyc_Binding_resolve_exp(env,_tmp9C);return;case 4: _LL85: _tmp9F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmp9E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_tmp9D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f3;_LL86:
# 353
 Cyc_Binding_resolve_exp(env,_tmp9F);Cyc_Binding_resolve_stmt(env,_tmp9E);Cyc_Binding_resolve_stmt(env,_tmp9D);return;case 14: _LL87: _tmpA1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpA0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp97)->f2).f1;_LL88:
 _tmpA3=_tmpA0;_tmpA2=_tmpA1;goto _LL8A;case 5: _LL89: _tmpA3=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp97)->f1).f1;_tmpA2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL8A:
 Cyc_Binding_resolve_exp(env,_tmpA3);Cyc_Binding_resolve_stmt(env,_tmpA2);return;case 9: _LL8B: _tmpA7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpA6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f2).f1;_tmpA5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f3).f1;_tmpA4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f4;_LL8C:
# 357
 Cyc_Binding_resolve_exp(env,_tmpA7);Cyc_Binding_resolve_exp(env,_tmpA6);Cyc_Binding_resolve_exp(env,_tmpA5);
Cyc_Binding_resolve_stmt(env,_tmpA4);
return;case 6: _LL8D: _LL8E:
 goto _LL90;case 7: _LL8F: _LL90:
 goto _LL92;case 8: _LL91: _LL92:
 return;case 11: _LL93: _tmpA8=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL94:
# 364
 for(0;_tmpA8 != 0;_tmpA8=_tmpA8->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpA8->hd);}
return;case 13: _LL95: _tmpA9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL96:
 Cyc_Binding_resolve_stmt(env,_tmpA9);return;case 16: _LL97: _tmpAA=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL98:
 Cyc_Binding_resolve_exp(env,_tmpAA);return;case 12: _LL99: _tmpAC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpAB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL9A: {
# 370
struct Cyc_Dict_Dict _tmp98=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmpAC);
Cyc_Binding_resolve_stmt(env,_tmpAB);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp98;
return;}case 10: _LL9B: _tmpAE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpAD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL9C:
 Cyc_Binding_resolve_exp(env,_tmpAE);Cyc_Binding_resolve_scs(env,_tmpAD);return;default: _LL9D: _tmpB0=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpAF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL9E:
 Cyc_Binding_resolve_stmt(env,_tmpB0);Cyc_Binding_resolve_scs(env,_tmpAF);return;}_LL7C:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 379
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpB1=e->r;void*_tmpB2=_tmpB1;void*_tmp13F;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Datatypedecl*_tmp13C;struct Cyc_Absyn_Datatypefield*_tmp13B;struct Cyc_Absyn_Enumdecl*_tmp13A;struct Cyc_Absyn_Enumfield*_tmp139;void*_tmp138;struct Cyc_Absyn_Enumfield*_tmp137;struct Cyc_List_List*_tmp136;void*_tmp135;void*_tmp134;void*_tmp133;struct Cyc_Absyn_Stmt*_tmp132;void*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;void**_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_List_List*_tmp110;struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;void*_tmp10D;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct _tuple0**_tmp108;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Aggrdecl**_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_List_List*_tmp103;int*_tmp102;void**_tmp101;switch(*((int*)_tmpB2)){case 1: _LLA0: _tmp101=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLA1: {
# 382
void*_tmpB3=*_tmp101;void*_tmpB4=_tmpB3;struct _tuple0*_tmpD0;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpB4)->tag == 0){_LLF1: _tmpD0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpB4)->f1;_LLF2: {
# 384
struct _handler_cons _tmpB5;_push_handler(& _tmpB5);{int _tmpB7=0;if(setjmp(_tmpB5.handler))_tmpB7=1;if(!_tmpB7){{void*_tmpB8=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpD0);void*_tmpB9=_tmpB8;struct Cyc_Absyn_Aggrdecl*_tmpCA;void*_tmpC9;struct Cyc_Absyn_Enumfield*_tmpC8;struct Cyc_Absyn_Enumdecl*_tmpC7;struct Cyc_Absyn_Enumfield*_tmpC6;struct Cyc_Absyn_Datatypedecl*_tmpC5;struct Cyc_Absyn_Datatypefield*_tmpC4;void*_tmpC3;switch(*((int*)_tmpB9)){case 0: _LLF6: _tmpC3=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpB9)->f1;_LLF7:
*_tmp101=_tmpC3;_npop_handler(0);return;case 2: _LLF8: _tmpC5=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpB9)->f1;_tmpC4=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpB9)->f2;_LLF9:
 e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpBB;_tmpBB.tag=30;_tmpBB.f1=0;_tmpBB.f2=_tmpC5;_tmpBB.f3=_tmpC4;_tmpBB;});_tmpBA;});_npop_handler(0);return;case 3: _LLFA: _tmpC7=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpB9)->f1;_tmpC6=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpB9)->f2;_LLFB:
 e->r=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpBD;_tmpBD.tag=31;_tmpBD.f1=_tmpC7;_tmpBD.f2=_tmpC6;_tmpBD;});_tmpBC;});_npop_handler(0);return;case 4: _LLFC: _tmpC9=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpB9)->f1;_tmpC8=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpB9)->f2;_LLFD:
 e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpBF;_tmpBF.tag=32;_tmpBF.f1=_tmpC9;_tmpBF.f2=_tmpC8;_tmpBF;});_tmpBE;});_npop_handler(0);return;default: _LLFE: _tmpCA=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpB9)->f1;_LLFF:
# 390
({struct Cyc_String_pa_PrintArg_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD0));({void*_tmpC0[1]={& _tmpC2};Cyc_Warn_err(e->loc,({const char*_tmpC1="bad occurrence of type name %s";_tag_dyneither(_tmpC1,sizeof(char),31);}),_tag_dyneither(_tmpC0,sizeof(void*),1));});});
_npop_handler(0);return;}_LLF5:;}
# 384
;_pop_handler();}else{void*_tmpB6=(void*)_exn_thrown;void*_tmpCB=_tmpB6;void*_tmpCF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpCB)->tag == Cyc_Binding_BindingError){_LL101: _LL102:
# 394
({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD0));({void*_tmpCC[1]={& _tmpCE};Cyc_Warn_err(e->loc,({const char*_tmpCD="undeclared identifier %s";_tag_dyneither(_tmpCD,sizeof(char),25);}),_tag_dyneither(_tmpCC,sizeof(void*),1));});});
return;}else{_LL103: _tmpCF=_tmpCB;_LL104:(int)_rethrow(_tmpCF);}_LL100:;}};}}else{_LLF3: _LLF4:
# 397
 return;}_LLF0:;}case 9: _LLA2: _tmp104=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_tmp102=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB2)->f4;_LLA3:
# 400
*_tmp102=1;
# 402
{struct Cyc_List_List*_tmpD1=_tmp103;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpD1->hd);}}{
void*_tmpD2=_tmp104->r;void*_tmpD3=_tmpD2;void**_tmpF2;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD3)->tag == 1){_LL106: _tmpF2=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD3)->f1;_LL107: {
# 406
void*_tmpD4=*_tmpF2;void*_tmpD5=_tmpD4;struct _tuple0*_tmpF1;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD5)->tag == 0){_LL10B: _tmpF1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD5)->f1;_LL10C: {
# 408
struct _handler_cons _tmpD6;_push_handler(& _tmpD6);{int _tmpD8=0;if(setjmp(_tmpD6.handler))_tmpD8=1;if(!_tmpD8){{void*_tmpD9=Cyc_Binding_lookup_ordinary(_tmp104->loc,env,_tmpF1);void*_tmpDA=_tmpD9;struct Cyc_Absyn_Datatypedecl*_tmpEB;struct Cyc_Absyn_Datatypefield*_tmpEA;struct Cyc_Absyn_Aggrdecl*_tmpE9;void*_tmpE8;switch(*((int*)_tmpDA)){case 0: _LL110: _tmpE8=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpDA)->f1;_LL111:
# 410
*_tmpF2=_tmpE8;_npop_handler(0);return;case 1: _LL112: _tmpE9=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpDA)->f1;_LL113: {
# 412
struct Cyc_List_List*_tmpDB=0;
for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
_tmpDB=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=({struct _tuple10*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->f1=0;_tmpDD->f2=(struct Cyc_Absyn_Exp*)_tmp103->hd;_tmpDD;});_tmpDC->tl=_tmpDB;_tmpDC;});}
e->r=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpDF;_tmpDF.tag=28;_tmpDF.f1=_tmpE9->name;_tmpDF.f2=0;_tmpDF.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDB);_tmpDF.f4=_tmpE9;_tmpDF;});_tmpDE;});
_npop_handler(0);return;}case 2: _LL114: _tmpEB=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpDA)->f1;_tmpEA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpDA)->f2;_LL115:
# 418
 if(_tmpEA->typs == 0)
# 420
({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpEA->name));({void*_tmpE0[1]={& _tmpE2};Cyc_Warn_err(e->loc,({const char*_tmpE1="%s is a constant, not a function";_tag_dyneither(_tmpE1,sizeof(char),33);}),_tag_dyneither(_tmpE0,sizeof(void*),1));});});
e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE4;_tmpE4.tag=30;_tmpE4.f1=_tmp103;_tmpE4.f2=_tmpEB;_tmpE4.f3=_tmpEA;_tmpE4;});_tmpE3;});
_npop_handler(0);return;case 4: _LL116: _LL117:
 goto _LL119;default: _LL118: _LL119:
# 426
({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpF1));({void*_tmpE5[1]={& _tmpE7};Cyc_Warn_err(e->loc,({const char*_tmpE6="%s is an enum constructor, not a function";_tag_dyneither(_tmpE6,sizeof(char),42);}),_tag_dyneither(_tmpE5,sizeof(void*),1));});});
_npop_handler(0);return;}_LL10F:;}
# 408
;_pop_handler();}else{void*_tmpD7=(void*)_exn_thrown;void*_tmpEC=_tmpD7;void*_tmpF0;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpEC)->tag == Cyc_Binding_BindingError){_LL11B: _LL11C:
# 430
({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF1));({void*_tmpED[1]={& _tmpEF};Cyc_Warn_err(e->loc,({const char*_tmpEE="undeclared identifier %s";_tag_dyneither(_tmpEE,sizeof(char),25);}),_tag_dyneither(_tmpED,sizeof(void*),1));});});
return;}else{_LL11D: _tmpF0=_tmpEC;_LL11E:(int)_rethrow(_tmpF0);}_LL11A:;}};}}else{_LL10D: _LL10E:
# 433
 return;}_LL10A:;}}else{_LL108: _LL109:
# 435
 Cyc_Binding_resolve_exp(env,_tmp104);return;}_LL105:;};case 28: _LLA4: _tmp108=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp107=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_tmp106=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB2)->f3;_tmp105=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB2)->f4;_LLA5:
# 439
 for(0;_tmp106 != 0;_tmp106=_tmp106->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp106->hd)).f2);}{
struct _handler_cons _tmpF3;_push_handler(& _tmpF3);{int _tmpF5=0;if(setjmp(_tmpF3.handler))_tmpF5=1;if(!_tmpF5){
*_tmp105=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmp108,Cyc_Binding_lookup_aggrdecl);
*_tmp108=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmp105))->name;
_npop_handler(0);return;
# 442
;_pop_handler();}else{void*_tmpF4=(void*)_exn_thrown;void*_tmpF6=_tmpF4;void*_tmpFA;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpF6)->tag == Cyc_Binding_BindingError){_LL120: _LL121:
# 446
({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp108));({void*_tmpF7[1]={& _tmpF9};Cyc_Warn_err(e->loc,({const char*_tmpF8="unbound struct/union name %s";_tag_dyneither(_tmpF8,sizeof(char),29);}),_tag_dyneither(_tmpF7,sizeof(void*),1));});});
return;}else{_LL122: _tmpFA=_tmpF6;_LL123:(int)_rethrow(_tmpFA);}_LL11F:;}};};case 26: _LLA6: _tmp10B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp10A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_tmp109=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB2)->f3;_LLA7:
# 451
 Cyc_Binding_resolve_exp(env,_tmp10A);
if(Cyc_Binding_at_toplevel(env)){
env->local_vars=({struct Cyc_Dict_Dict*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmpFB;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp10B);
Cyc_Binding_resolve_exp(env,_tmp109);
env->local_vars=0;
return;}{
# 459
struct Cyc_Dict_Dict _tmpFC=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp10B);
Cyc_Binding_resolve_exp(env,_tmp109);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpFC;
return;};case 35: _LLA8: _tmp10C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLA9:
# 467
 for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp10C->hd)).f2);}
return;case 27: _LLAA: _tmp10E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp10D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLAB:
# 473
 Cyc_Binding_resolve_exp(env,_tmp10E);
Cyc_Binding_resolve_type(e->loc,env,_tmp10D);
return;case 0: _LLAC: _LLAD:
# 477
 return;case 23: _LLAE: _tmp10F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLAF:
# 479
 _tmp110=_tmp10F;goto _LLB1;case 2: _LLB0: _tmp110=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLB1:
# 481
 for(0;_tmp110 != 0;_tmp110=_tmp110->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp110->hd);}
return;case 37: _LLB2: _tmp111=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLB3:
# 485
 _tmp112=_tmp111;goto _LLB5;case 11: _LLB4: _tmp112=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLB5:
 _tmp113=_tmp112;goto _LLB7;case 17: _LLB6: _tmp113=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLB7:
 _tmp114=_tmp113;goto _LLB9;case 10: _LLB8: _tmp114=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLB9:
 _tmp115=_tmp114;goto _LLBB;case 4: _LLBA: _tmp115=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLBB:
 _tmp116=_tmp115;goto _LLBD;case 21: _LLBC: _tmp116=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLBD:
 _tmp117=_tmp116;goto _LLBF;case 20: _LLBE: _tmp117=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLBF:
 _tmp118=_tmp117;goto _LLC1;case 14: _LLC0: _tmp118=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLC1:
 _tmp119=_tmp118;goto _LLC3;case 19: _LLC2: _tmp119=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLC3:
 Cyc_Binding_resolve_exp(env,_tmp119);return;case 34: _LLC4: _tmp11B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp11A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLC5:
# 495
 _tmp11D=_tmp11B;_tmp11C=_tmp11A;goto _LLC7;case 8: _LLC6: _tmp11D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp11C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLC7:
 _tmp11F=_tmp11D;_tmp11E=_tmp11C;goto _LLC9;case 3: _LLC8: _tmp11F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp11E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB2)->f3;_LLC9:
 _tmp121=_tmp11F;_tmp120=_tmp11E;goto _LLCB;case 22: _LLCA: _tmp121=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp120=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLCB:
 _tmp123=_tmp121;_tmp122=_tmp120;goto _LLCD;case 6: _LLCC: _tmp123=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp122=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLCD:
 _tmp125=_tmp123;_tmp124=_tmp122;goto _LLCF;case 7: _LLCE: _tmp125=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp124=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLCF:
 Cyc_Binding_resolve_exp(env,_tmp125);Cyc_Binding_resolve_exp(env,_tmp124);return;case 33: _LLD0: _tmp128=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB2)->f1).rgn;_tmp127=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB2)->f1).elt_type;_tmp126=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB2)->f1).num_elts;_LLD1:
# 503
 if(_tmp128 != 0)Cyc_Binding_resolve_exp(env,_tmp128);
if(_tmp127 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp127);
Cyc_Binding_resolve_exp(env,_tmp126);
return;case 15: _LLD2: _tmp12A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp129=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLD3:
# 509
 if(_tmp12A != 0)Cyc_Binding_resolve_exp(env,_tmp12A);
Cyc_Binding_resolve_exp(env,_tmp129);
return;case 5: _LLD4: _tmp12D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp12C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_tmp12B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB2)->f3;_LLD5:
# 514
 Cyc_Binding_resolve_exp(env,_tmp12D);Cyc_Binding_resolve_exp(env,_tmp12C);Cyc_Binding_resolve_exp(env,_tmp12B);return;case 12: _LLD6: _tmp12F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp12E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLD7:
# 517
 Cyc_Binding_resolve_exp(env,_tmp12F);
for(0;_tmp12E != 0;_tmp12E=_tmp12E->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp12E->hd);}
return;case 13: _LLD8: _tmp131=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp130=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLD9:
# 523
 Cyc_Binding_resolve_exp(env,_tmp130);Cyc_Binding_resolve_type(e->loc,env,_tmp131);return;case 36: _LLDA: _tmp132=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLDB:
# 525
 Cyc_Binding_resolve_stmt(env,_tmp132);return;case 38: _LLDC: _tmp133=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLDD:
# 527
 _tmp134=_tmp133;goto _LLDF;case 18: _LLDE: _tmp134=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLDF:
 _tmp135=_tmp134;goto _LLE1;case 16: _LLE0: _tmp135=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLE1:
 Cyc_Binding_resolve_type(e->loc,env,_tmp135);return;case 25: _LLE2: _tmp136=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LLE3:
# 532
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp136->hd)).f2);}
return;case 24: _LLE4: _LLE5:
# 537
({void*_tmpFD=0;Cyc_Warn_err(e->loc,({const char*_tmpFE="binding: compound literals unimplemented";_tag_dyneither(_tmpFE,sizeof(char),41);}),_tag_dyneither(_tmpFD,sizeof(void*),0));});return;case 32: _LLE6: _tmp138=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp137=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLE7:
# 540
 Cyc_Binding_resolve_type(e->loc,env,_tmp138);return;case 31: _LLE8: _tmp13A=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp139=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLE9:
 return;case 30: _LLEA: _tmp13D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp13C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_tmp13B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB2)->f3;_LLEB:
# 543
 for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp13D->hd);}
return;case 29: _LLEC: _tmp13F=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmp13E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LLED:
# 547
 Cyc_Binding_resolve_type(e->loc,env,_tmp13F);
for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp13E->hd)).f2);}
return;default: _LLEE: _LLEF:
# 553
({void*_tmpFF=0;Cyc_Warn_err(e->loc,({const char*_tmp100="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmp100,sizeof(char),49);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});}_LL9F:;}
# 557
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp140=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp141=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp142=_tmp141;struct Cyc_Absyn_Pat*_tmp145;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Stmt*_tmp143;_LL125: _tmp145=_tmp142->pattern;_tmp144=_tmp142->where_clause;_tmp143=_tmp142->body;_LL126:;
Cyc_Binding_resolve_pat(env,_tmp145);
if(_tmp144 != 0)
Cyc_Binding_resolve_exp(env,_tmp144);
Cyc_Binding_resolve_stmt(env,_tmp143);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp140;}
# 567
return;}
# 569
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp146=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp147=_tmp146;struct _dyneither_ptr*_tmp14D;struct Cyc_Absyn_Tqual _tmp14C;void*_tmp14B;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_Exp*_tmp148;_LL128: _tmp14D=_tmp147->name;_tmp14C=_tmp147->tq;_tmp14B=_tmp147->type;_tmp14A=_tmp147->width;_tmp149=_tmp147->attributes;_tmp148=_tmp147->requires_clause;_LL129:;
Cyc_Binding_resolve_type(loc,env,_tmp14B);
if(_tmp14A != 0)
Cyc_Binding_resolve_exp(env,_tmp14A);
if(_tmp148 != 0)
Cyc_Binding_resolve_exp(env,_tmp148);}
# 578
return;}struct _tuple11{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 581
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 587
struct Cyc_List_List*_tmp14E=0;
for(0;args != 0;args=args->tl){
struct _tuple11*_tmp14F=(struct _tuple11*)args->hd;struct _tuple11*_tmp150=_tmp14F;void*_tmp157;struct Cyc_Absyn_Tqual _tmp156;void*_tmp155;_LL12B: _tmp157=_tmp150->f1;_tmp156=_tmp150->f2;_tmp155=_tmp150->f3;_LL12C:;{
struct _dyneither_ptr*_tmp151=f(_tmp157);
if(_tmp151 == 0)continue;{
struct Cyc_Absyn_Vardecl*_tmp152=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=Cyc_Absyn_Loc_n;_tmp154->f2=_tmp151;_tmp154;}),_tmp155,0);
_tmp152->tq=_tmp156;
_tmp14E=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=_tmp152;_tmp153->tl=_tmp14E;_tmp153;});};};}
# 596
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp158=*vai;struct Cyc_Absyn_VarargInfo _tmp159=_tmp158;struct _dyneither_ptr*_tmp163;struct Cyc_Absyn_Tqual _tmp162;void*_tmp161;int _tmp160;_LL12E: _tmp163=_tmp159.name;_tmp162=_tmp159.tq;_tmp161=_tmp159.type;_tmp160=_tmp159.inject;_LL12F:;
if(_tmp163 == 0){
if(need_va_name)
({void*_tmp15A=0;Cyc_Warn_err(loc,({const char*_tmp15B="missing name for vararg";_tag_dyneither(_tmp15B,sizeof(char),24);}),_tag_dyneither(_tmp15A,sizeof(void*),0));});}else{
# 603
void*_tmp15C=Cyc_Absyn_dyneither_typ(_tmp161,(void*)& Cyc_Absyn_HeapRgn_val,_tmp162,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp15D=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->f1=Cyc_Absyn_Loc_n;_tmp15F->f2=_tmp163;_tmp15F;}),_tmp15C,0);
_tmp14E=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->hd=_tmp15D;_tmp15E->tl=_tmp14E;_tmp15E;});}}
# 608
_tmp14E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14E);
return _tmp14E;}
# 611
struct _dyneither_ptr*Cyc_Binding_get_vds_fndecl(struct _dyneither_ptr*v){return v;}
struct _dyneither_ptr*Cyc_Binding_get_vds_fntype(struct _dyneither_ptr*v){return v;}
# 614
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause){
# 618
if(eff != 0)
Cyc_Binding_resolve_type(loc,env,eff);
Cyc_Binding_resolve_type(loc,env,rettyp);
for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)args->hd)).f3);}
if(vai != 0)
Cyc_Binding_resolve_type(loc,env,vai->type);
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
if(req_clause != 0)
Cyc_Binding_resolve_exp(env,req_clause);
if(ens_clause != 0){
# 630
struct Cyc_Dict_Dict _tmp164=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp165=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp166=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->f1=Cyc_Absyn_Loc_n;_tmp16B->f2=_tmp165;_tmp16B;}),rettyp,0);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp165,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp168;_tmp168.tag=0;_tmp168.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp16A;_tmp16A.tag=4;_tmp16A.f1=_tmp166;_tmp16A;});_tmp169;});_tmp168;});_tmp167;}));
# 635
Cyc_Binding_resolve_exp(env,ens_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp164;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 645
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp16D=t;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;void*_tmp1E8;void*_tmp1E7;void*_tmp1E6;void*_tmp1E5;void*_tmp1E4;void*_tmp1E3;struct Cyc_List_List*_tmp1E2;struct Cyc_Absyn_FnInfo _tmp1E1;void*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_List_List*_tmp1DE;void*_tmp1DD;void*_tmp1DC;union Cyc_Absyn_Constraint*_tmp1DB;void*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;struct Cyc_Absyn_TypeDecl*_tmp1D7;void***_tmp1D6;struct _tuple0**_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Typedefdecl**_tmp1D3;struct _tuple0**_tmp1D2;struct Cyc_Absyn_Enumdecl*_tmp1D1;union Cyc_Absyn_AggrInfoU*_tmp1D0;struct Cyc_List_List*_tmp1CF;union Cyc_Absyn_DatatypeFieldInfoU*_tmp1CE;struct Cyc_List_List*_tmp1CD;union Cyc_Absyn_DatatypeInfoU*_tmp1CC;struct Cyc_List_List*_tmp1CB;switch(*((int*)_tmp16D)){case 3: _LL131: _tmp1CC=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16D)->f1).datatype_info;_tmp1CB=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16D)->f1).targs;_LL132:
# 648
 for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CB->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp16E=*_tmp1CC;union Cyc_Absyn_DatatypeInfoU _tmp16F=_tmp16E;struct _tuple0*_tmp178;int _tmp177;if((_tmp16F.UnknownDatatype).tag == 1){_LL16C: _tmp178=((_tmp16F.UnknownDatatype).val).name;_tmp177=((_tmp16F.UnknownDatatype).val).is_extensible;_LL16D: {
# 652
struct _handler_cons _tmp170;_push_handler(& _tmp170);{int _tmp172=0;if(setjmp(_tmp170.handler))_tmp172=1;if(!_tmp172){
{struct Cyc_Absyn_Datatypedecl*_tmp173=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp178,Cyc_Binding_lookup_datatypedecl))->hd;
*_tmp1CC=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp174;_tmp174.name=_tmp173->name;_tmp174.is_extensible=_tmp177;_tmp174;}));
_npop_handler(0);return;}
# 653
;_pop_handler();}else{void*_tmp171=(void*)_exn_thrown;void*_tmp175=_tmp171;void*_tmp176;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp175)->tag == Cyc_Binding_BindingError){_LL171: _LL172:
# 657
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp178,Cyc_Absyn_Public);return;}else{_LL173: _tmp176=_tmp175;_LL174:(int)_rethrow(_tmp176);}_LL170:;}};}}else{_LL16E: _LL16F:
 return;}_LL16B:;};case 4: _LL133: _tmp1CE=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp16D)->f1).field_info;_tmp1CD=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp16D)->f1).targs;_LL134:
# 661
 for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CD->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp179=*_tmp1CE;union Cyc_Absyn_DatatypeFieldInfoU _tmp17A=_tmp179;struct _tuple0*_tmp18E;struct _tuple0*_tmp18D;int _tmp18C;if((_tmp17A.UnknownDatatypefield).tag == 1){_LL176: _tmp18E=((_tmp17A.UnknownDatatypefield).val).datatype_name;_tmp18D=((_tmp17A.UnknownDatatypefield).val).field_name;_tmp18C=((_tmp17A.UnknownDatatypefield).val).is_extensible;_LL177:
# 667
{struct _handler_cons _tmp17B;_push_handler(& _tmp17B);{int _tmp17D=0;if(setjmp(_tmp17B.handler))_tmp17D=1;if(!_tmp17D){
{void*_tmp17E=Cyc_Binding_lookup_ordinary(loc,env,_tmp18D);void*_tmp17F=_tmp17E;struct Cyc_Absyn_Datatypedecl*_tmp186;struct Cyc_Absyn_Datatypefield*_tmp185;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17F)->tag == 2){_LL17B: _tmp186=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17F)->f1;_tmp185=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17F)->f2;_LL17C: {
# 670
struct Cyc_Absyn_Datatypedecl*_tmp180=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp18E,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp186->name,_tmp180->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp185->name));({void*_tmp181[1]={& _tmp183};Cyc_Warn_err(loc,({const char*_tmp182="shadowed datatype field %s not yet implemented properly";_tag_dyneither(_tmp182,sizeof(char),56);}),_tag_dyneither(_tmp181,sizeof(void*),1));});});
_npop_handler(0);return;}
# 676
*_tmp1CE=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp184;_tmp184.datatype_name=_tmp186->name;_tmp184.field_name=_tmp185->name;_tmp184.is_extensible=_tmp18C;_tmp184;}));
# 678
_npop_handler(0);return;}}else{_LL17D: _LL17E:
 goto _LL17A;}_LL17A:;}
# 668
;_pop_handler();}else{void*_tmp17C=(void*)_exn_thrown;void*_tmp187=_tmp17C;void*_tmp188;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp187)->tag == Cyc_Binding_BindingError){_LL180: _LL181:
# 681
 goto _LL17F;}else{_LL182: _tmp188=_tmp187;_LL183:(int)_rethrow(_tmp188);}_LL17F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp18D));({void*_tmp189[1]={& _tmp18B};Cyc_Warn_err(loc,({const char*_tmp18A="%s is not a datatype field";_tag_dyneither(_tmp18A,sizeof(char),27);}),_tag_dyneither(_tmp189,sizeof(void*),1));});});
return;}else{_LL178: _LL179:
 return;}_LL175:;};case 11: _LL135: _tmp1D0=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16D)->f1).aggr_info;_tmp1CF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16D)->f1).targs;_LL136:
# 688
 for(0;_tmp1CF != 0;_tmp1CF=_tmp1CF->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CF->hd);}{
union Cyc_Absyn_AggrInfoU _tmp18F=*_tmp1D0;union Cyc_Absyn_AggrInfoU _tmp190=_tmp18F;enum Cyc_Absyn_AggrKind _tmp19D;struct _tuple0*_tmp19C;struct Cyc_Core_Opt*_tmp19B;if((_tmp190.UnknownAggr).tag == 1){_LL185: _tmp19D=((_tmp190.UnknownAggr).val).f1;_tmp19C=((_tmp190.UnknownAggr).val).f2;_tmp19B=((_tmp190.UnknownAggr).val).f3;_LL186: {
# 692
struct _handler_cons _tmp191;_push_handler(& _tmp191);{int _tmp193=0;if(setjmp(_tmp191.handler))_tmp193=1;if(!_tmp193){
{struct Cyc_Absyn_Aggrdecl*_tmp194=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp19C,Cyc_Binding_lookup_aggrdecl);
if(_tmp194->kind != _tmp19D)
({void*_tmp195=0;Cyc_Warn_err(loc,({const char*_tmp196="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp196,sizeof(char),51);}),_tag_dyneither(_tmp195,sizeof(void*),0));});
if(((_tmp19D == Cyc_Absyn_UnionA  && _tmp19B != 0) && _tmp194->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp194->impl))->tagged != (int)_tmp19B->v)
# 698
({void*_tmp197=0;Cyc_Warn_err(loc,({const char*_tmp198="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp198,sizeof(char),42);}),_tag_dyneither(_tmp197,sizeof(void*),0));});
*_tmp1D0=Cyc_Absyn_UnknownAggr(_tmp19D,_tmp194->name,_tmp19B);
_npop_handler(0);return;}
# 693
;_pop_handler();}else{void*_tmp192=(void*)_exn_thrown;void*_tmp199=_tmp192;void*_tmp19A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp199)->tag == Cyc_Binding_BindingError){_LL18A: _LL18B:
# 702
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp19C,Cyc_Absyn_Public);return;}else{_LL18C: _tmp19A=_tmp199;_LL18D:(int)_rethrow(_tmp19A);}_LL189:;}};}}else{_LL187: _LL188:
 return;}_LL184:;};case 13: _LL137: _tmp1D2=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16D)->f1;_tmp1D1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16D)->f2;_LL138:
# 706
 if(_tmp1D1 != 0)
return;{
struct _handler_cons _tmp19E;_push_handler(& _tmp19E);{int _tmp1A0=0;if(setjmp(_tmp19E.handler))_tmp1A0=1;if(!_tmp1A0){
{struct Cyc_Absyn_Enumdecl*_tmp1A1=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1D2,Cyc_Binding_lookup_enumdecl);
*_tmp1D2=_tmp1A1->name;
_npop_handler(0);return;}
# 709
;_pop_handler();}else{void*_tmp19F=(void*)_exn_thrown;void*_tmp1A2=_tmp19F;void*_tmp1A3;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A2)->tag == Cyc_Binding_BindingError){_LL18F: _LL190:
# 713
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1D2,Cyc_Absyn_Public);return;}else{_LL191: _tmp1A3=_tmp1A2;_LL192:(int)_rethrow(_tmp1A3);}_LL18E:;}};};case 17: _LL139: _tmp1D5=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16D)->f1;_tmp1D4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16D)->f2;_tmp1D3=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16D)->f3;_LL13A:
# 716
 for(0;_tmp1D4 != 0;_tmp1D4=_tmp1D4->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D4->hd);}
{struct _handler_cons _tmp1A4;_push_handler(& _tmp1A4);{int _tmp1A6=0;if(setjmp(_tmp1A4.handler))_tmp1A6=1;if(!_tmp1A6){
{struct Cyc_Absyn_Typedefdecl*_tmp1A7=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1D5,Cyc_Binding_lookup_typedefdecl);
# 721
*_tmp1D5=_tmp1A7->name;
_npop_handler(0);return;}
# 719
;_pop_handler();}else{void*_tmp1A5=(void*)_exn_thrown;void*_tmp1A8=_tmp1A5;void*_tmp1A9;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A8)->tag == Cyc_Binding_BindingError){_LL194: _LL195:
# 723
 goto _LL193;}else{_LL196: _tmp1A9=_tmp1A8;_LL197:(int)_rethrow(_tmp1A9);}_LL193:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1D5));({void*_tmp1AA[1]={& _tmp1AC};Cyc_Warn_err(loc,({const char*_tmp1AB="unbound typdef name %s";_tag_dyneither(_tmp1AB,sizeof(char),23);}),_tag_dyneither(_tmp1AA,sizeof(void*),1));});});
return;case 26: _LL13B: _tmp1D7=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D)->f1;_tmp1D6=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D)->f2;_LL13C: {
# 729
struct Cyc_Dict_Dict*_tmp1AD=env->local_vars;
env->local_vars=0;
{void*_tmp1AE=_tmp1D7->r;void*_tmp1AF=_tmp1AE;struct Cyc_Absyn_Datatypedecl*_tmp1BB;struct Cyc_Absyn_Enumdecl*_tmp1BA;struct Cyc_Absyn_Aggrdecl*_tmp1B9;switch(*((int*)_tmp1AF)){case 0: _LL199: _tmp1B9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1AF)->f1;_LL19A:
# 733
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1B2;_tmp1B2.tag=5;_tmp1B2.f1=_tmp1B9;_tmp1B2;});_tmp1B1;});_tmp1B0->loc=_tmp1D7->loc;_tmp1B0;}));goto _LL198;case 1: _LL19B: _tmp1BA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1AF)->f1;_LL19C:
# 735
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1B5;_tmp1B5.tag=7;_tmp1B5.f1=_tmp1BA;_tmp1B5;});_tmp1B4;});_tmp1B3->loc=_tmp1D7->loc;_tmp1B3;}));goto _LL198;default: _LL19D: _tmp1BB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1AF)->f1;_LL19E:
# 737
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1B8;_tmp1B8.tag=6;_tmp1B8.f1=_tmp1BB;_tmp1B8;});_tmp1B7;});_tmp1B6->loc=_tmp1D7->loc;_tmp1B6;}));goto _LL198;}_LL198:;}
# 739
env->local_vars=_tmp1AD;
return;}case 12: _LL13D: _tmp1D8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16D)->f2;_LL13E:
# 746
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1D8);
return;case 14: _LL13F: _tmp1D9=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16D)->f1;_LL140:
# 751
 for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
struct Cyc_Absyn_Enumfield*_tmp1BC=(struct Cyc_Absyn_Enumfield*)_tmp1D9->hd;
if(_tmp1BC->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1BC->tag));}
# 756
return;case 1: _LL141: _tmp1DA=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16D)->f2;_LL142:
 if(_tmp1DA != 0)Cyc_Binding_resolve_type(loc,env,_tmp1DA);return;case 28: _LL143: _LL144:
# 759
 goto _LL146;case 0: _LL145: _LL146:
 goto _LL148;case 6: _LL147: _LL148:
 goto _LL14A;case 7: _LL149: _LL14A:
 goto _LL14C;case 20: _LL14B: _LL14C:
 goto _LL14E;case 21: _LL14D: _LL14E:
 goto _LL150;case 22: _LL14F: _LL150:
 goto _LL152;case 2: _LL151: _LL152:
 return;case 5: _LL153: _tmp1DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16D)->f1).elt_typ;_tmp1DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16D)->f1).ptr_atts).rgn;_tmp1DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16D)->f1).ptr_atts).bounds;_LL154:
# 769
 Cyc_Binding_resolve_type(loc,env,_tmp1DD);
Cyc_Binding_resolve_type(loc,env,_tmp1DC);
while(1){
union Cyc_Absyn_Constraint*_tmp1BD=_tmp1DB;void*_tmp1C1;union Cyc_Absyn_Constraint*_tmp1C0;switch((((union Cyc_Absyn_Constraint*)_tmp1BD)->Forward_constr).tag){case 3: _LL1A0: _LL1A1:
 return;case 2: _LL1A2: _tmp1C0=(_tmp1BD->Forward_constr).val;_LL1A3:
 _tmp1DB=_tmp1C0;continue;default: _LL1A4: _tmp1C1=(void*)(_tmp1BD->Eq_constr).val;_LL1A5: {
# 776
void*_tmp1BE=_tmp1C1;struct Cyc_Absyn_Exp*_tmp1BF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1BE)->tag == 0){_LL1A7: _LL1A8:
 return;}else{_LL1A9: _tmp1BF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1BE)->f1;_LL1AA:
 Cyc_Binding_resolve_exp(env,_tmp1BF);return;}_LL1A6:;}}_LL19F:;}case 10: _LL155: _tmp1DE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16D)->f1;_LL156:
# 782
 for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp1DE->hd)).f2);}
return;case 8: _LL157: _tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16D)->f1).elt_type;_tmp1DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16D)->f1).num_elts;_LL158:
# 786
 Cyc_Binding_resolve_type(loc,env,_tmp1E0);
if(_tmp1DF != 0)Cyc_Binding_resolve_exp(env,_tmp1DF);
return;case 9: _LL159: _tmp1E1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1;_LL15A: {
# 793
struct Cyc_List_List*_tmp1C2=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fntype,0,loc,env,_tmp1E1.args,_tmp1E1.cyc_varargs);
# 796
struct Cyc_Dict_Dict*_tmp1C3=env->local_vars;
if(_tmp1C3 != 0)
env->local_vars=({struct Cyc_Dict_Dict*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1C4;});else{
# 800
env->local_vars=({struct Cyc_Dict_Dict*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp1C5;});}
{struct Cyc_List_List*_tmp1C6=_tmp1C2;for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*((struct Cyc_Absyn_Vardecl*)_tmp1C6->hd)->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp1C8;_tmp1C8.tag=0;_tmp1C8.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp1CA;_tmp1CA.tag=3;_tmp1CA.f1=(struct Cyc_Absyn_Vardecl*)_tmp1C6->hd;_tmp1CA;});_tmp1C9;});_tmp1C8;});_tmp1C7;}));}}
# 805
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp1E1.effect,_tmp1E1.ret_typ,_tmp1E1.args,_tmp1E1.cyc_varargs,_tmp1E1.rgn_po,_tmp1E1.requires_clause,_tmp1E1.ensures_clause);
# 809
env->local_vars=_tmp1C3;
return;}case 24: _LL15B: _tmp1E2=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp16D)->f1;_LL15C:
# 813
 for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1E2->hd);}
return;case 19: _LL15D: _tmp1E3=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp16D)->f1;_LL15E:
# 817
 _tmp1E4=_tmp1E3;goto _LL160;case 23: _LL15F: _tmp1E4=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp16D)->f1;_LL160:
 _tmp1E5=_tmp1E4;goto _LL162;case 25: _LL161: _tmp1E5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp16D)->f1;_LL162:
 _tmp1E6=_tmp1E5;goto _LL164;case 15: _LL163: _tmp1E6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp16D)->f1;_LL164:
 Cyc_Binding_resolve_type(loc,env,_tmp1E6);return;case 16: _LL165: _tmp1E8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp16D)->f1;_tmp1E7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp16D)->f2;_LL166:
# 823
 Cyc_Binding_resolve_type(loc,env,_tmp1E8);Cyc_Binding_resolve_type(loc,env,_tmp1E7);return;case 18: _LL167: _tmp1E9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp16D)->f1;_LL168:
# 825
 _tmp1EA=_tmp1E9;goto _LL16A;default: _LL169: _tmp1EA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp16D)->f1;_LL16A:
 Cyc_Binding_resolve_exp(env,_tmp1EA);return;}_LL130:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 830
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1EB=p;void*_tmp245;unsigned int _tmp244;_LL1AC: _tmp245=_tmp1EB->r;_tmp244=_tmp1EB->loc;_LL1AD:;{
void*_tmp1EC=_tmp245;struct Cyc_List_List*_tmp243;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_List_List*_tmp241;struct Cyc_Absyn_Pat*_tmp240;struct Cyc_Absyn_Vardecl*_tmp23F;struct Cyc_Absyn_Vardecl*_tmp23E;struct Cyc_Absyn_Vardecl*_tmp23D;struct Cyc_Absyn_Pat*_tmp23C;struct Cyc_Absyn_Vardecl*_tmp23B;struct Cyc_Absyn_Pat*_tmp23A;struct Cyc_List_List*_tmp239;struct Cyc_List_List*_tmp238;struct _tuple0*_tmp237;struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp234;int _tmp233;struct _tuple0*_tmp232;struct Cyc_List_List*_tmp231;int _tmp230;struct _tuple0*_tmp22F;switch(*((int*)_tmp1EC)){case 15: _LL1AF: _tmp22F=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL1B0:
# 834
{struct _handler_cons _tmp1ED;_push_handler(& _tmp1ED);{int _tmp1EF=0;if(setjmp(_tmp1ED.handler))_tmp1EF=1;if(!_tmp1EF){{void*_tmp1F0=Cyc_Binding_lookup_ordinary(_tmp244,env,_tmp22F);void*_tmp1F1=_tmp1F0;void*_tmp1FF;struct Cyc_Absyn_Enumfield*_tmp1FE;struct Cyc_Absyn_Enumdecl*_tmp1FD;struct Cyc_Absyn_Enumfield*_tmp1FC;struct Cyc_Absyn_Datatypedecl*_tmp1FB;struct Cyc_Absyn_Datatypefield*_tmp1FA;switch(*((int*)_tmp1F1)){case 0: _LL1D8: _LL1D9:
 goto _LL1D7;case 1: _LL1DA: _LL1DB:
# 837
({void*_tmp1F2=0;Cyc_Warn_err(_tmp244,({const char*_tmp1F3="struct tag used without arguments in pattern";_tag_dyneither(_tmp1F3,sizeof(char),45);}),_tag_dyneither(_tmp1F2,sizeof(void*),0));});
_npop_handler(0);return;case 2: _LL1DC: _tmp1FB=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1F1)->f1;_tmp1FA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1F1)->f2;_LL1DD:
# 840
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1F5;_tmp1F5.tag=8;_tmp1F5.f1=_tmp1FB;_tmp1F5.f2=_tmp1FA;_tmp1F5.f3=0;_tmp1F5.f4=0;_tmp1F5;});_tmp1F4;});_npop_handler(0);return;case 3: _LL1DE: _tmp1FD=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1F1)->f1;_tmp1FC=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1F1)->f2;_LL1DF:
# 842
 p->r=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp1F7;_tmp1F7.tag=13;_tmp1F7.f1=_tmp1FD;_tmp1F7.f2=_tmp1FC;_tmp1F7;});_tmp1F6;});_npop_handler(0);return;default: _LL1E0: _tmp1FF=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1F1)->f1;_tmp1FE=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1F1)->f2;_LL1E1:
# 844
 p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8[0]=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp1F9;_tmp1F9.tag=14;_tmp1F9.f1=_tmp1FF;_tmp1F9.f2=_tmp1FE;_tmp1F9;});_tmp1F8;});_npop_handler(0);return;}_LL1D7:;}
# 834
;_pop_handler();}else{void*_tmp1EE=(void*)_exn_thrown;void*_tmp200=_tmp1EE;void*_tmp201;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp200)->tag == Cyc_Binding_BindingError){_LL1E3: _LL1E4:
# 845
 goto _LL1E2;}else{_LL1E5: _tmp201=_tmp200;_LL1E6:(int)_rethrow(_tmp201);}_LL1E2:;}};}
Cyc_Binding_absolutize_decl(_tmp244,env,_tmp22F,Cyc_Absyn_Public);{
struct Cyc_Absyn_Vardecl*_tmp202=Cyc_Absyn_new_vardecl(_tmp22F,(void*)& Cyc_Absyn_VoidType_val,0);
p->r=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp204;_tmp204.tag=1;_tmp204.f1=_tmp202;_tmp204.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp204;});_tmp203;});
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*_tmp202->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp206;_tmp206.tag=0;_tmp206.f1=(void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp208;_tmp208.tag=5;_tmp208.f1=_tmp202;_tmp208;});_tmp207;});_tmp206;});_tmp205;}));
# 851
return;};case 16: _LL1B1: _tmp232=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_tmp231=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_tmp230=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL1B2:
# 854
{struct Cyc_List_List*_tmp209=_tmp231;for(0;_tmp209 != 0;_tmp209=_tmp209->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp209->hd);}}
{struct _handler_cons _tmp20A;_push_handler(& _tmp20A);{int _tmp20C=0;if(setjmp(_tmp20A.handler))_tmp20C=1;if(!_tmp20C){{void*_tmp20D=Cyc_Binding_lookup_ordinary(_tmp244,env,_tmp232);void*_tmp20E=_tmp20D;struct Cyc_Absyn_Datatypedecl*_tmp21C;struct Cyc_Absyn_Datatypefield*_tmp21B;struct Cyc_Absyn_Aggrdecl*_tmp21A;switch(*((int*)_tmp20E)){case 0: _LL1E8: _LL1E9:
 goto _LL1E7;case 1: _LL1EA: _tmp21A=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp20E)->f1;_LL1EB: {
# 859
struct Cyc_List_List*_tmp20F=0;
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
_tmp20F=({struct Cyc_List_List*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->hd=({struct _tuple13*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->f1=0;_tmp211->f2=(struct Cyc_Absyn_Pat*)_tmp231->hd;_tmp211;});_tmp210->tl=_tmp20F;_tmp210;});}
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp213;_tmp213.tag=7;_tmp213.f1=({struct Cyc_Absyn_AggrInfo*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=_tmp21A;_tmp215;}));_tmp214->targs=0;_tmp214;});_tmp213.f2=0;_tmp213.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp20F);_tmp213.f4=_tmp230;_tmp213;});_tmp212;});
_npop_handler(0);return;}case 2: _LL1EC: _tmp21C=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp20E)->f1;_tmp21B=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp20E)->f2;_LL1ED:
# 866
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp217;_tmp217.tag=8;_tmp217.f1=_tmp21C;_tmp217.f2=_tmp21B;_tmp217.f3=_tmp231;_tmp217.f4=_tmp230;_tmp217;});_tmp216;});_npop_handler(0);return;case 3: _LL1EE: _LL1EF:
 goto _LL1F1;default: _LL1F0: _LL1F1:
# 869
({void*_tmp218=0;Cyc_Warn_err(_tmp244,({const char*_tmp219="enum tag used with arguments in pattern";_tag_dyneither(_tmp219,sizeof(char),40);}),_tag_dyneither(_tmp218,sizeof(void*),0));});_npop_handler(0);return;}_LL1E7:;}
# 856
;_pop_handler();}else{void*_tmp20B=(void*)_exn_thrown;void*_tmp21D=_tmp20B;void*_tmp21E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp21D)->tag == Cyc_Binding_BindingError){_LL1F3: _LL1F4:
# 870
 goto _LL1F2;}else{_LL1F5: _tmp21E=_tmp21D;_LL1F6:(int)_rethrow(_tmp21E);}_LL1F2:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp232));({void*_tmp21F[1]={& _tmp221};Cyc_Warn_err(_tmp244,({const char*_tmp220="%s is not a constructor in pattern";_tag_dyneither(_tmp220,sizeof(char),35);}),_tag_dyneither(_tmp21F,sizeof(void*),1));});});
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1)->aggr_info).UnknownAggr).tag == 1){_LL1B3: _tmp237=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1)->aggr_info).UnknownAggr).val).f2;_tmp236=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1)->targs;_tmp235=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_tmp234=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_tmp233=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f4;_LL1B4:
# 875
{struct Cyc_List_List*_tmp222=_tmp234;for(0;_tmp222 != 0;_tmp222=_tmp222->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp222->hd)).f2);}}
{struct _handler_cons _tmp223;_push_handler(& _tmp223);{int _tmp225=0;if(setjmp(_tmp223.handler))_tmp225=1;if(!_tmp225){
{struct Cyc_Absyn_Aggrdecl*_tmp226=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp244,env->ns,_tmp237,Cyc_Binding_lookup_aggrdecl);
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp228;_tmp228.tag=7;_tmp228.f1=({struct Cyc_Absyn_AggrInfo*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=_tmp226;_tmp22A;}));_tmp229->targs=_tmp236;_tmp229;});_tmp228.f2=_tmp235;_tmp228.f3=_tmp234;_tmp228.f4=_tmp233;_tmp228;});_tmp227;});}
# 878
;_pop_handler();}else{void*_tmp224=(void*)_exn_thrown;void*_tmp22B=_tmp224;void*_tmp22E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp22B)->tag == Cyc_Binding_BindingError){_LL1F8: _LL1F9:
# 882
({void*_tmp22C=0;Cyc_Warn_err(_tmp244,({const char*_tmp22D="non-aggregate name has designator patterns";_tag_dyneither(_tmp22D,sizeof(char),43);}),_tag_dyneither(_tmp22C,sizeof(void*),0));});
return;}else{_LL1FA: _tmp22E=_tmp22B;_LL1FB:(int)_rethrow(_tmp22E);}_LL1F7:;}};}
# 885
return;}else{_LL1CF: _tmp238=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL1D0:
# 914
 for(0;_tmp238 != 0;_tmp238=_tmp238->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp238->hd)).f2);}
return;}}else{_LL1CD: _tmp239=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL1CE:
# 912
 _tmp238=_tmp239;goto _LL1D0;}case 0: _LL1B5: _LL1B6:
# 887
 return;case 3: _LL1B7: _tmp23B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_tmp23A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LL1B8:
# 889
 _tmp23D=_tmp23B;_tmp23C=_tmp23A;goto _LL1BA;case 1: _LL1B9: _tmp23D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_tmp23C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LL1BA:
# 891
 Cyc_Binding_resolve_pat(env,_tmp23C);
_tmp23E=_tmp23D;goto _LL1BC;case 4: _LL1BB: _tmp23E=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LL1BC:
 _tmp23F=_tmp23E;goto _LL1BE;case 2: _LL1BD: _tmp23F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LL1BE:
# 895
 Cyc_Binding_resolve_and_add_vardecl(_tmp244,env,_tmp23F);goto _LL1AE;case 6: _LL1BF: _tmp240=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL1C0:
# 897
 Cyc_Binding_resolve_pat(env,_tmp240);return;case 5: _LL1C1: _tmp241=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL1C2:
# 899
 for(0;_tmp241 != 0;_tmp241=_tmp241->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp241->hd);}
goto _LL1AE;case 17: _LL1C3: _tmp242=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL1C4:
# 903
 Cyc_Binding_resolve_exp(env,_tmp242);return;case 9: _LL1C5: _LL1C6:
# 905
 goto _LL1C8;case 10: _LL1C7: _LL1C8:
 goto _LL1CA;case 11: _LL1C9: _LL1CA:
 goto _LL1CC;case 12: _LL1CB: _LL1CC:
 return;case 8: _LL1D1: _tmp243=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL1D2:
# 918
 for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp243->hd);}
return;case 13: _LL1D3: _LL1D4:
 goto _LL1D6;default: _LL1D5: _LL1D6:
 return;}_LL1AE:;};}struct _tuple14{void*f1;void*f2;};
# 926
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}struct _tuple15{unsigned int f1;struct _tuple0*f2;int f3;};
# 940 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 943
unsigned int loc=d->loc;
void*_tmp246=d->r;void*_tmp247=_tmp246;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List*_tmp2C9;struct Cyc_List_List*_tmp2C8;struct _tuple0*_tmp2C7;struct Cyc_List_List*_tmp2C6;struct _dyneither_ptr*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct Cyc_Absyn_Datatypedecl*_tmp2C3;struct Cyc_Absyn_Enumdecl*_tmp2C2;struct Cyc_Absyn_Aggrdecl*_tmp2C1;struct Cyc_Absyn_Typedefdecl*_tmp2C0;struct Cyc_Absyn_Tvar*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2BE;int _tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_Absyn_Pat*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Fndecl*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B7;switch(*((int*)_tmp247)){case 0: _LL1FD: _tmp2B7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL1FE:
# 947
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2B7);
# 952
if(_tmp2B7->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp2B7->initializer));
goto _LL1FC;case 1: _LL1FF: _tmp2B8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL200:
# 957
 Cyc_Binding_absolutize_decl(loc,env,_tmp2B8->name,_tmp2B8->sc);{
struct _tuple0*_tmp248=_tmp2B8->name;struct _tuple0*_tmp249=_tmp248;union Cyc_Absyn_Nmspace _tmp262;struct _dyneither_ptr*_tmp261;_LL21C: _tmp262=_tmp249->f1;_tmp261=_tmp249->f2;_LL21D:;{
# 960
struct Cyc_List_List*_tmp24A=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fndecl,1,loc,env,_tmp2B8->args,_tmp2B8->cyc_varargs);
# 962
_tmp2B8->param_vardecls=({struct Cyc_Core_Opt*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B->v=_tmp24A;_tmp24B;});{
# 964
struct Cyc_Dict_Dict*_tmp24C=env->local_vars;
if(_tmp24C != 0)
env->local_vars=({struct Cyc_Dict_Dict*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp24D;});else{
# 968
env->local_vars=({struct Cyc_Dict_Dict*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp24E;});}
{struct Cyc_List_List*_tmp24F=_tmp24A;for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*((struct Cyc_Absyn_Vardecl*)_tmp24F->hd)->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp251;_tmp251.tag=0;_tmp251.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp253;_tmp253.tag=3;_tmp253.f1=(struct Cyc_Absyn_Vardecl*)_tmp24F->hd;_tmp253;});_tmp252;});_tmp251;});_tmp250;}));}}
# 973
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp2B8->effect,_tmp2B8->ret_type,_tmp2B8->args,_tmp2B8->cyc_varargs,_tmp2B8->rgn_po,_tmp2B8->requires_clause,_tmp2B8->ensures_clause);
# 977
if(_tmp24C != 0){
# 979
*_tmp24C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp24C,_tmp261,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp257;_tmp257.tag=2;_tmp257.f1=_tmp2B8;_tmp257;});_tmp256;});_tmp255;});_tmp254;}));
# 981
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp261,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp259;_tmp259.tag=0;_tmp259.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp25B;_tmp25B.tag=2;_tmp25B.f1=_tmp2B8;_tmp25B;});_tmp25A;});_tmp259;});_tmp258;}));}else{
# 984
struct Cyc_Binding_ResolveNSEnv*_tmp25C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp262);
_tmp25C->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp25C->ordinaries,_tmp261,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp260;_tmp260.tag=2;_tmp260.f1=_tmp2B8;_tmp260;});_tmp25F;});_tmp25E;});_tmp25D;}));}
# 990
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp2B8->body);
# 993
env->local_vars=_tmp24C;
goto _LL1FC;};};};case 2: _LL201: _tmp2BA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_tmp2B9=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp247)->f3;_LL202:
# 997
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp263=0;Cyc_Warn_err(loc,({const char*_tmp264="let not allowed at top-level";_tag_dyneither(_tmp264,sizeof(char),29);}),_tag_dyneither(_tmp263,sizeof(void*),0));});
goto _LL1FC;}
# 1001
Cyc_Binding_resolve_exp(env,_tmp2B9);
Cyc_Binding_resolve_pat(env,_tmp2BA);
goto _LL1FC;case 3: _LL203: _tmp2BB=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL204:
# 1006
 for(0;_tmp2BB != 0;_tmp2BB=_tmp2BB->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp2BB->hd);}
goto _LL1FC;case 4: _LL205: _tmp2BF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_tmp2BE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp247)->f2;_tmp2BD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp247)->f3;_tmp2BC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp247)->f4;_LL206:
# 1011
 if(_tmp2BC != 0)
Cyc_Binding_resolve_exp(env,_tmp2BC);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2BE);
goto _LL1FC;case 8: _LL207: _tmp2C0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL208:
# 1017
 Cyc_Binding_absolutize_decl(loc,env,_tmp2C0->name,_tmp2C0->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp265=_tmp2C0->name;struct _tuple0*_tmp266=_tmp265;union Cyc_Absyn_Nmspace _tmp269;struct _dyneither_ptr*_tmp268;_LL21F: _tmp269=_tmp266->f1;_tmp268=_tmp266->f2;_LL220:;{
# 1020
struct Cyc_Binding_ResolveNSEnv*_tmp267=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp269);
if(_tmp2C0->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp2C0->defn));
# 1024
_tmp267->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp267->typedefs,_tmp268,_tmp2C0);
# 1026
goto _LL1FC;};};case 5: _LL209: _tmp2C1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL20A:
# 1029
 Cyc_Binding_absolutize_decl(loc,env,_tmp2C1->name,_tmp2C1->sc);{
struct _tuple0*_tmp26A=_tmp2C1->name;struct _tuple0*_tmp26B=_tmp26A;union Cyc_Absyn_Nmspace _tmp274;struct _dyneither_ptr*_tmp273;_LL222: _tmp274=_tmp26B->f1;_tmp273=_tmp26B->f2;_LL223:;{
# 1032
struct Cyc_Binding_ResolveNSEnv*_tmp26C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp274);
# 1034
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp26C->aggrdecls,_tmp273) && _tmp2C1->impl == 0)
goto _LL1FC;
# 1037
_tmp26C->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp26C->aggrdecls,_tmp273,_tmp2C1);
# 1039
_tmp26C->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp26C->ordinaries,_tmp273,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Binding_AggrRes_Binding_Resolved_struct _tmp26E;_tmp26E.tag=1;_tmp26E.f1=_tmp2C1;_tmp26E;});_tmp26D;}));
# 1041
if(_tmp2C1->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp26F=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C1->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp270=_tmp26F;struct Cyc_List_List*_tmp272;struct Cyc_List_List*_tmp271;_LL225: _tmp272=_tmp270->rgn_po;_tmp271=_tmp270->fields;_LL226:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp272);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp271);}
# 1046
goto _LL1FC;};};case 7: _LL20B: _tmp2C2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL20C:
# 1049
 Cyc_Binding_absolutize_decl(loc,env,_tmp2C2->name,_tmp2C2->sc);{
struct _tuple0*_tmp275=_tmp2C2->name;struct _tuple0*_tmp276=_tmp275;union Cyc_Absyn_Nmspace _tmp27C;struct _dyneither_ptr*_tmp27B;_LL228: _tmp27C=_tmp276->f1;_tmp27B=_tmp276->f2;_LL229:;{
# 1052
struct Cyc_Binding_ResolveNSEnv*_tmp277=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp27C);
# 1054
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp277->enumdecls,_tmp27B) && _tmp2C2->fields == 0)
goto _LL1FC;
# 1058
_tmp277->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp277->enumdecls,_tmp27B,_tmp2C2);
# 1060
if(_tmp2C2->fields != 0){
# 1062
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2C2->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp278=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp278->loc,env,_tmp278->name,_tmp2C2->sc);
if(_tmp278->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp278->tag));
_tmp277->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp277->ordinaries,(*_tmp278->name).f2,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Binding_EnumRes_Binding_Resolved_struct _tmp27A;_tmp27A.tag=3;_tmp27A.f1=_tmp2C2;_tmp27A.f2=_tmp278;_tmp27A;});_tmp279;}));}}
# 1071
goto _LL1FC;};};case 6: _LL20D: _tmp2C3=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL20E:
# 1075
{struct _handler_cons _tmp27D;_push_handler(& _tmp27D);{int _tmp27F=0;if(setjmp(_tmp27D.handler))_tmp27F=1;if(!_tmp27F){
{struct Cyc_List_List*_tmp280=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp2C3->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp281=(struct Cyc_Absyn_Datatypedecl*)_tmp280->hd;
if(!_tmp281->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp282=_cycalloc_atomic(sizeof(*_tmp282));_tmp282[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp283;_tmp283.tag=Cyc_Binding_BindingError;_tmp283;});_tmp282;}));
_tmp2C3->name=_tmp281->name;
_tmp2C3->is_extensible=1;{
struct _tuple0*_tmp284=_tmp2C3->name;struct _tuple0*_tmp285=_tmp284;union Cyc_Absyn_Nmspace _tmp289;struct _dyneither_ptr*_tmp288;_LL22B: _tmp289=_tmp285->f1;_tmp288=_tmp285->f2;_LL22C:;{
struct Cyc_Binding_ResolveNSEnv*_tmp286=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp289);
_tmp286->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp286->datatypedecls,_tmp288,({struct Cyc_List_List*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287->hd=_tmp2C3;_tmp287->tl=_tmp280;_tmp287;}));};};}
# 1076
;_pop_handler();}else{void*_tmp27E=(void*)_exn_thrown;void*_tmp28A=_tmp27E;void*_tmp291;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp28A)->tag == Cyc_Binding_BindingError){_LL22E: _LL22F:
# 1088
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp2C3->name,_tmp2C3->sc);{
struct _tuple0*_tmp28B=_tmp2C3->name;struct _tuple0*_tmp28C=_tmp28B;union Cyc_Absyn_Nmspace _tmp290;struct _dyneither_ptr*_tmp28F;_LL233: _tmp290=_tmp28C->f1;_tmp28F=_tmp28C->f2;_LL234:;{
struct Cyc_Binding_ResolveNSEnv*_tmp28D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp290);
# 1092
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp28D->datatypedecls,_tmp28F) && _tmp2C3->fields == 0)
# 1094
goto _LL22D;
_tmp28D->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp28D->datatypedecls,_tmp28F,({struct Cyc_List_List*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E->hd=_tmp2C3;_tmp28E->tl=0;_tmp28E;}));
# 1097
goto _LL22D;};};}else{_LL230: _tmp291=_tmp28A;_LL231:(int)_rethrow(_tmp291);}_LL22D:;}};}{
# 1100
struct _tuple0*_tmp292=_tmp2C3->name;struct _tuple0*_tmp293=_tmp292;union Cyc_Absyn_Nmspace _tmp2A1;struct _dyneither_ptr*_tmp2A0;_LL236: _tmp2A1=_tmp293->f1;_tmp2A0=_tmp293->f2;_LL237:;
if(_tmp2C3->fields != 0){
struct Cyc_List_List*_tmp294=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2C3->fields))->v;for(0;_tmp294 != 0;_tmp294=_tmp294->tl){
struct Cyc_Absyn_Datatypefield*_tmp295=(struct Cyc_Absyn_Datatypefield*)_tmp294->hd;
{struct Cyc_List_List*_tmp296=_tmp295->typs;for(0;_tmp296 != 0;_tmp296=_tmp296->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp296->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp297=(*_tmp295->name).f1;union Cyc_Absyn_Nmspace _tmp298=_tmp297;switch((_tmp298.Abs_n).tag){case 1: if((_tmp298.Rel_n).val == 0){_LL239: _LL23A:
# 1108
 if(_tmp2C3->is_extensible)
(*_tmp295->name).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);else{
# 1111
(*_tmp295->name).f1=(*_tmp2C3->name).f1;}
goto _LL238;}else{_LL23B: _LL23C:
# 1114
({void*_tmp299=0;Cyc_Warn_err(_tmp295->loc,({const char*_tmp29A="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp29A,sizeof(char),56);}),_tag_dyneither(_tmp299,sizeof(void*),0));});
return;}case 2: _LL23D: _LL23E:
 goto _LL238;default: _LL23F: _LL240:
({void*_tmp29B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(({const char*_tmp29C="datatype field Loc_n or C_n";_tag_dyneither(_tmp29C,sizeof(char),28);}),_tag_dyneither(_tmp29B,sizeof(void*),0));});}_LL238:;}{
# 1119
struct Cyc_Binding_ResolveNSEnv*_tmp29D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp295->name).f1);
_tmp29D->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp29D->ordinaries,(*_tmp295->name).f2,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E[0]=({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct _tmp29F;_tmp29F.tag=2;_tmp29F.f1=_tmp2C3;_tmp29F.f2=_tmp295;_tmp29F;});_tmp29E;}));};}}
# 1124
goto _LL1FC;};case 9: _LL20F: _tmp2C5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_tmp2C4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp247)->f2;_LL210:
# 1127
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp2C5,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp2C4);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL1FC;case 10: _LL211: _tmp2C7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_tmp2C6=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp247)->f2;_LL212:
# 1132
((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp2C7);
Cyc_Binding_resolve_decls(env,_tmp2C6);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL1FC;case 11: _LL213: _tmp2C8=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_LL214: {
# 1138
int _tmp2A2=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2C8);
env->in_cinclude=_tmp2A2;
goto _LL1FC;}case 12: _LL215: _tmp2CA=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp247)->f1;_tmp2C9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp247)->f2;_LL216:
# 1149
{struct Cyc_List_List*_tmp2A3=_tmp2C9;for(0;_tmp2A3 != 0;_tmp2A3=_tmp2A3->tl){
struct _tuple15*_tmp2A4=(struct _tuple15*)_tmp2A3->hd;struct _tuple15*_tmp2A5=_tmp2A4;unsigned int _tmp2A7;struct _tuple0*_tmp2A6;_LL242: _tmp2A7=_tmp2A5->f1;_tmp2A6=_tmp2A5->f2;_LL243:;
Cyc_Binding_absolutize_decl(_tmp2A7,env,_tmp2A6,Cyc_Absyn_ExternC);}}
# 1157
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp2A8=0;Cyc_Warn_err(loc,({const char*_tmp2A9="extern \"C include\" not at toplevel";_tag_dyneither(_tmp2A9,sizeof(char),35);}),_tag_dyneither(_tmp2A8,sizeof(void*),0));});
goto _LL1FC;}{
# 1162
struct Cyc_Binding_ResolveNSEnv*_tmp2AA=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp2AB=_tmp2AA->ordinaries;
int _tmp2AC=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2CA);{
struct Cyc_Dict_Dict _tmp2AD=_tmp2AA->ordinaries;
struct Cyc_Dict_Dict _tmp2AE=_tmp2AB;
{struct Cyc_List_List*_tmp2AF=_tmp2C9;for(0;_tmp2AF != 0;_tmp2AF=_tmp2AF->tl){
struct _tuple15*_tmp2B0=(struct _tuple15*)_tmp2AF->hd;struct _tuple15*_tmp2B1=_tmp2B0;unsigned int _tmp2B6;struct _dyneither_ptr*_tmp2B5;_LL245: _tmp2B6=_tmp2B1->f1;_tmp2B5=(_tmp2B1->f2)->f2;_LL246:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp2AD,_tmp2B5) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp2AB,_tmp2B5) && 
((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2AB,_tmp2B5)== ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2AD,_tmp2B5))
({struct Cyc_String_pa_PrintArg_struct _tmp2B4;_tmp2B4.tag=0;_tmp2B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B5);({void*_tmp2B2[1]={& _tmp2B4};Cyc_Warn_err(_tmp2B6,({const char*_tmp2B3="%s is exported but not defined";_tag_dyneither(_tmp2B3,sizeof(char),31);}),_tag_dyneither(_tmp2B2,sizeof(void*),1));});});
_tmp2AE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2AE,_tmp2B5,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2AD,_tmp2B5));}}
# 1177
_tmp2AA->ordinaries=_tmp2AE;
env->in_cinclude=_tmp2AC;
goto _LL1FC;};};case 13: _LL217: _LL218:
# 1181
 goto _LL21A;default: _LL219: _LL21A:
 goto _LL1FC;}_LL1FC:;}
# 1186
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1191
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2CB=({struct Cyc_Binding_Env*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->in_cinclude=0;_tmp2CF->ns=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2CF->local_vars=0;_tmp2CF;});
Cyc_Binding_resolve_decl(_tmp2CB,({struct Cyc_Absyn_Decl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2CE;_tmp2CE.tag=6;_tmp2CE.f1=Cyc_Absyn_exn_tud;_tmp2CE;});_tmp2CD;});_tmp2CC->loc=0;_tmp2CC;}));
Cyc_Binding_resolve_decls(_tmp2CB,tds);}

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
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
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
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 940
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 963
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
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
# 29 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);
# 33
typedef struct Cyc_List_List*Cyc_Binding_namespace_name_t;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};
# 38
typedef void*Cyc_Binding_nsdirective_t;struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 44
typedef struct Cyc_Binding_NSCtxt*Cyc_Binding_nsctxt_t;
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 40 "binding.cyc"
typedef struct Cyc_Dict_Dict Cyc_Binding_dict_t;char Cyc_Binding_BindingError[13]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 44
int Cyc_Binding_warn_override=0;struct Cyc_Binding_NSCtxt;
# 58
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->curr_ns=0;({struct Cyc_List_List*_tmp2CE=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp2CD=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp3=_cycalloc(sizeof(*_tmp3));({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp2CC=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp4;_tmp4.tag=0;_tmp4.f1=0;_tmp4;});_tmp3[0]=_tmp2CC;});_tmp3;});_tmp2->hd=_tmp2CD;});_tmp2->tl=0;_tmp2;});_tmp1->availables=_tmp2CE;});({struct Cyc_Dict_Dict _tmp2CF=
# 62
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp1->ns_data=_tmp2CF;});_tmp1;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp5=abs_ns;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;switch((_tmp5.Abs_n).tag){case 3: _LL1: _tmp8=(_tmp5.C_n).val;_LL2:
 _tmp9=_tmp8;goto _LL4;case 2: _LL3: _tmp9=(_tmp5.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp9);default: _LL5: _LL6:
({void*_tmp6=0;({struct _dyneither_ptr _tmp2D0=({const char*_tmp7="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp7,sizeof(char),33);});Cyc_Warn_impos(_tmp2D0,_tag_dyneither(_tmp6,sizeof(void*),0));});});}_LL0:;}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpA))
return _tmpA;
{struct Cyc_List_List*_tmpB=ctxt->availables;for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmpC=(void*)_tmpB->hd;void*_tmpD=_tmpC;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmpF;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpD)->tag == 1){_LL8: _tmpF=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpD)->f1;_LL9:
 _tmp10=_tmpF;goto _LLB;}else{_LLA: _tmp10=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpD)->f1;_LLB: {
# 92
struct Cyc_List_List*_tmpE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp10,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpE))
return _tmpE;
goto _LL7;}}_LL7:;}}
# 97
({struct Cyc_String_pa_PrintArg_struct _tmp13;_tmp13.tag=0;({struct _dyneither_ptr _tmp2D2=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_List_List*_tmp2D1=rel_ns;Cyc_str_sepstr(_tmp2D1,({const char*_tmp14="::";_tag_dyneither(_tmp14,sizeof(char),3);}));}));_tmp13.f1=_tmp2D2;});({void*_tmp11[1]={& _tmp13};({unsigned int _tmp2D4=loc;struct _dyneither_ptr _tmp2D3=({const char*_tmp12="namespace %s not found";_tag_dyneither(_tmp12,sizeof(char),23);});Cyc_Warn_err(_tmp2D4,_tmp2D3,_tag_dyneither(_tmp11,sizeof(void*),1));});});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp15=_cycalloc_atomic(sizeof(*_tmp15));({struct Cyc_Binding_BindingError_exn_struct _tmp2D5=({struct Cyc_Binding_BindingError_exn_struct _tmp16;_tmp16.tag=Cyc_Binding_BindingError;_tmp16;});_tmp15[0]=_tmp2D5;});_tmp15;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp17=qv;union Cyc_Absyn_Nmspace _tmp43;struct _dyneither_ptr*_tmp42;_LLD: _tmp43=_tmp17->f1;_tmp42=_tmp17->f2;_LLE:;{
union Cyc_Absyn_Nmspace _tmp18=_tmp43;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp3F;switch((_tmp18.Abs_n).tag){case 1: if((_tmp18.Rel_n).val == 0){_LL10: _LL11:
# 106
{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){{void*_tmp1C=({void*(*_tmp2D7)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2D6=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp2D7(_tmp2D6,_tmp42);});_npop_handler(0);return _tmp1C;};_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1D=_tmp1A;void*_tmp1E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1D)->tag == Cyc_Dict_Absent){_LL1B: _LL1C:
 goto _LL1A;}else{_LL1D: _tmp1E=_tmp1D;_LL1E:(int)_rethrow(_tmp1E);}_LL1A:;}};}
{struct Cyc_List_List*_tmp1F=ctxt->availables;for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
void*_tmp20=(void*)_tmp1F->hd;void*_tmp21=_tmp20;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp28;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp21)->tag == 1){_LL20: _tmp28=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp21)->f1;_LL21:
 _tmp29=_tmp28;goto _LL23;}else{_LL22: _tmp29=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp21)->f1;_LL23:
# 112
{struct _handler_cons _tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){{void*_tmp25=({void*(*_tmp2D9)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2D8=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp29);_tmp2D9(_tmp2D8,_tmp42);});_npop_handler(0);return _tmp25;};_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp26=_tmp23;void*_tmp27;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp26)->tag == Cyc_Dict_Absent){_LL25: _LL26:
 goto _LL24;}else{_LL27: _tmp27=_tmp26;_LL28:(int)_rethrow(_tmp27);}_LL24:;}};}
goto _LL1F;}_LL1F:;}}
# 116
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2A=_cycalloc_atomic(sizeof(*_tmp2A));({struct Cyc_Binding_BindingError_exn_struct _tmp2DA=({struct Cyc_Binding_BindingError_exn_struct _tmp2B;_tmp2B.tag=Cyc_Binding_BindingError;_tmp2B;});_tmp2A[0]=_tmp2DA;});_tmp2A;}));}else{_LL12: _tmp3F=(_tmp18.Rel_n).val;_LL13: {
# 118
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct Cyc_List_List*_tmp2F=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3F);
void*_tmp30=({void*(*_tmp2DC)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2DB=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2F);_tmp2DC(_tmp2DB,_tmp42);});_npop_handler(0);return _tmp30;}
# 119
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp31=_tmp2D;void*_tmp34;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent){_LL2A: _LL2B:
# 121
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp32=_cycalloc_atomic(sizeof(*_tmp32));({struct Cyc_Binding_BindingError_exn_struct _tmp2DD=({struct Cyc_Binding_BindingError_exn_struct _tmp33;_tmp33.tag=Cyc_Binding_BindingError;_tmp33;});_tmp32[0]=_tmp2DD;});_tmp32;}));}else{_LL2C: _tmp34=_tmp31;_LL2D:(int)_rethrow(_tmp34);}_LL29:;}};}}case 3: _LL14: _tmp40=(_tmp18.C_n).val;_LL15:
 _tmp41=_tmp40;goto _LL17;case 2: _LL16: _tmp41=(_tmp18.Abs_n).val;_LL17: {
# 124
struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){{void*_tmp38=({void*(*_tmp2DF)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2DE=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp41);_tmp2DF(_tmp2DE,_tmp42);});_npop_handler(0);return _tmp38;};_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp39=_tmp36;void*_tmp3C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp39)->tag == Cyc_Dict_Absent){_LL2F: _LL30:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3A=_cycalloc_atomic(sizeof(*_tmp3A));({struct Cyc_Binding_BindingError_exn_struct _tmp2E0=({struct Cyc_Binding_BindingError_exn_struct _tmp3B;_tmp3B.tag=Cyc_Binding_BindingError;_tmp3B;});_tmp3A[0]=_tmp2E0;});_tmp3A;}));}else{_LL31: _tmp3C=_tmp39;_LL32:(int)_rethrow(_tmp3C);}_LL2E:;}};}default: _LL18: _LL19:
({void*_tmp3D=0;({struct _dyneither_ptr _tmp2E1=({const char*_tmp3E="lookup local in global";_tag_dyneither(_tmp3E,sizeof(char),23);});Cyc_Warn_impos(_tmp2E1,_tag_dyneither(_tmp3D,sizeof(void*),0));});});}_LLF:;};}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp44=ctxt->curr_ns;
struct Cyc_List_List*_tmp45=({struct Cyc_List_List*_tmp2E2=_tmp44;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2E2,({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=subname;_tmp49->tl=0;_tmp49;}));});
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp45))
({struct Cyc_Dict_Dict _tmp2E5=({struct Cyc_Dict_Dict _tmp2E4=ctxt->ns_data;struct Cyc_List_List*_tmp2E3=_tmp45;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp2E4,_tmp2E3,mkdata(env));});ctxt->ns_data=_tmp2E5;});
ctxt->curr_ns=_tmp45;
({struct Cyc_List_List*_tmp2E8=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));({void*_tmp2E7=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp47=_cycalloc(sizeof(*_tmp47));({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp2E6=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp48;_tmp48.tag=0;_tmp48.f1=_tmp45;_tmp48;});_tmp47[0]=_tmp2E6;});_tmp47;});_tmp46->hd=_tmp2E7;});_tmp46->tl=ctxt->availables;_tmp46;});ctxt->availables=_tmp2E8;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp4A=0;({struct _dyneither_ptr _tmp2E9=({const char*_tmp4B="leaving topmost namespace";_tag_dyneither(_tmp4B,sizeof(char),26);});Cyc_Warn_impos(_tmp2E9,_tag_dyneither(_tmp4A,sizeof(void*),0));});});{
void*_tmp4C=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp4D=_tmp4C;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp4D)->tag == 0){_LL34: _LL35:
# 143
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp2EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp2EA;});
goto _LL33;}else{_LL36: _LL37:
# 147
({void*_tmp4E=0;({struct _dyneither_ptr _tmp2EB=({const char*_tmp4F="leaving using as namespace";_tag_dyneither(_tmp4F,sizeof(char),27);});Cyc_Warn_impos(_tmp2EB,_tag_dyneither(_tmp4E,sizeof(void*),0));});});}_LL33:;};}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp50=usename;union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp5E;_LL39: _tmp5F=_tmp50->f1;_tmp5E=_tmp50->f2;_LL3A:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp51=_tmp5F;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp59;switch((_tmp51.Loc_n).tag){case 1: _LL3C: _tmp59=(_tmp51.Rel_n).val;_LL3D:
# 155
({struct Cyc_List_List*_tmp2EF=({unsigned int _tmp2EE=loc;struct Cyc_Binding_NSCtxt*_tmp2ED=ctxt;Cyc_Binding_resolve_rel_ns(_tmp2EE,_tmp2ED,({struct Cyc_List_List*_tmp2EC=_tmp59;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2EC,({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=_tmp5E;_tmp52->tl=0;_tmp52;}));}));});ns=_tmp2EF;});{
struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
({union Cyc_Absyn_Nmspace _tmp2F0=Cyc_Absyn_Abs_n(_tmp53,0);(*usename).f1=_tmp2F0;});
goto _LL3B;};case 2: _LL3E: _tmp5A=(_tmp51.Abs_n).val;_LL3F:
# 160
({struct Cyc_List_List*_tmp2F2=({struct Cyc_List_List*_tmp2F1=_tmp5A;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2F1,({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=_tmp5E;_tmp54->tl=0;_tmp54;}));});ns=_tmp2F2;});
goto _LL3B;case 4: _LL40: _LL41:
({void*_tmp55=0;({struct _dyneither_ptr _tmp2F3=({const char*_tmp56="enter_using, Loc_n";_tag_dyneither(_tmp56,sizeof(char),19);});Cyc_Warn_impos(_tmp2F3,_tag_dyneither(_tmp55,sizeof(void*),0));});});default: _LL42: _LL43:
({void*_tmp57=0;({struct _dyneither_ptr _tmp2F4=({const char*_tmp58="enter_using, C_n";_tag_dyneither(_tmp58,sizeof(char),17);});Cyc_Warn_impos(_tmp2F4,_tag_dyneither(_tmp57,sizeof(void*),0));});});}_LL3B:;}
# 165
({struct Cyc_List_List*_tmp2F7=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));({void*_tmp2F6=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct Cyc_Binding_Using_Binding_NSDirective_struct _tmp2F5=({struct Cyc_Binding_Using_Binding_NSDirective_struct _tmp5D;_tmp5D.tag=1;_tmp5D.f1=ns;_tmp5D;});_tmp5C[0]=_tmp2F5;});_tmp5C;});_tmp5B->hd=_tmp2F6;});_tmp5B->tl=ctxt->availables;_tmp5B;});ctxt->availables=_tmp2F7;});
return ns;};}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp60=0;({struct _dyneither_ptr _tmp2F8=({const char*_tmp61="leaving topmost namespace as a using";_tag_dyneither(_tmp61,sizeof(char),37);});Cyc_Warn_impos(_tmp2F8,_tag_dyneither(_tmp60,sizeof(void*),0));});});{
void*_tmp62=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp63=_tmp62;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp63)->tag == 1){_LL45: _LL46:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL44;}else{_LL47: _LL48:
({void*_tmp64=0;({struct _dyneither_ptr _tmp2F9=({const char*_tmp65="leaving namespace as using";_tag_dyneither(_tmp65,sizeof(char),27);});Cyc_Warn_impos(_tmp2F9,_tag_dyneither(_tmp64,sizeof(void*),0));});});}_LL44:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 193 "binding.cyc"
typedef void*Cyc_Binding_resolved_t;struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 202
typedef struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_renv_t;struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 210
typedef struct Cyc_Binding_Env*Cyc_Binding_env_t;
# 212
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp66=_cycalloc(sizeof(*_tmp66));({struct Cyc_Dict_Dict _tmp2FA=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->aggrdecls=_tmp2FA;});({struct Cyc_Dict_Dict _tmp2FB=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->datatypedecls=_tmp2FB;});({struct Cyc_Dict_Dict _tmp2FC=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->enumdecls=_tmp2FC;});({struct Cyc_Dict_Dict _tmp2FD=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->typedefs=_tmp2FD;});({struct Cyc_Dict_Dict _tmp2FE=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp66->ordinaries=_tmp2FE;});_tmp66;});}
# 226
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 229
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 232
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 235
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 238
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 241
static void*Cyc_Binding_lookup_ordinary(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp67=qv;union Cyc_Absyn_Nmspace _tmp6C;struct _dyneither_ptr*_tmp6B;_LL4A: _tmp6C=_tmp67->f1;_tmp6B=_tmp67->f2;_LL4B:;{
union Cyc_Absyn_Nmspace _tmp68=_tmp6C;switch((_tmp68.Rel_n).tag){case 4: _LL4D: _LL4E:
# 245
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp69=_cycalloc_atomic(sizeof(*_tmp69));({struct Cyc_Binding_BindingError_exn_struct _tmp2FF=({struct Cyc_Binding_BindingError_exn_struct _tmp6A;_tmp6A.tag=Cyc_Binding_BindingError;_tmp6A;});_tmp69[0]=_tmp2FF;});_tmp69;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B);case 1: if((_tmp68.Rel_n).val == 0){_LL4F: _LL50:
# 249
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6B);
goto _LL52;}else{goto _LL51;}default: _LL51: _LL52:
# 253
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL4C:;};}
# 257
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
# 269
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int,unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 274
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp6D=(*qv).f1;union Cyc_Absyn_Nmspace _tmp6E=_tmp6D;switch((_tmp6E.Abs_n).tag){case 1: if((_tmp6E.Rel_n).val == 0){_LL54: _LL55:
# 277
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp301=({struct Cyc_List_List*_tmp300=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp300,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);});(*qv).f1=_tmp301;});else{
# 280
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL53;}else{goto _LL5A;}case 4: _LL56: _LL57:
 goto _LL53;case 2: _LL58: _LL59:
# 284
 if(!Cyc_Binding_at_toplevel(env))
goto _LL5B;
goto _LL53;default: _LL5A: _LL5B:
# 288
({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;({struct _dyneither_ptr _tmp302=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(qv));_tmp71.f1=_tmp302;});({void*_tmp6F[1]={& _tmp71};({unsigned int _tmp304=loc;struct _dyneither_ptr _tmp303=({const char*_tmp70="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp70,sizeof(char),51);});Cyc_Warn_err(_tmp304,_tmp303,_tag_dyneither(_tmp6F,sizeof(void*),1));});});});}_LL53:;}
# 295
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp72=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp72;}
# 302
void Cyc_Binding_check_warn_override(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _dyneither_ptr hides_what;
struct _handler_cons _tmp73;_push_handler(& _tmp73);{int _tmp75=0;if(setjmp(_tmp73.handler))_tmp75=1;if(!_tmp75){
{void*_tmp76=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp77=_tmp76;void*_tmp83;switch(*((int*)_tmp77)){case 0: _LL5D: _tmp83=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp77)->f1;_LL5E:
# 307
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0);return;}
{void*_tmp78=_tmp83;switch(*((int*)_tmp78)){case 1: _LL68: _LL69:
({struct _dyneither_ptr _tmp305=({const char*_tmp79="global variable";_tag_dyneither(_tmp79,sizeof(char),16);});hides_what=_tmp305;});goto _LL67;case 4: _LL6A: _LL6B:
({struct _dyneither_ptr _tmp306=({const char*_tmp7A="local variable";_tag_dyneither(_tmp7A,sizeof(char),15);});hides_what=_tmp306;});goto _LL67;case 3: _LL6C: _LL6D:
({struct _dyneither_ptr _tmp307=({const char*_tmp7B="parameter";_tag_dyneither(_tmp7B,sizeof(char),10);});hides_what=_tmp307;});goto _LL67;case 5: _LL6E: _LL6F:
({struct _dyneither_ptr _tmp308=({const char*_tmp7C="pattern variable";_tag_dyneither(_tmp7C,sizeof(char),17);});hides_what=_tmp308;});goto _LL67;case 2: _LL70: _LL71:
({struct _dyneither_ptr _tmp309=({const char*_tmp7D="function";_tag_dyneither(_tmp7D,sizeof(char),9);});hides_what=_tmp309;});goto _LL67;default: _LL72: _LL73:
({void*_tmp7E=0;({struct _dyneither_ptr _tmp30A=({const char*_tmp7F="shadowing free variable!";_tag_dyneither(_tmp7F,sizeof(char),25);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp30A,_tag_dyneither(_tmp7E,sizeof(void*),0));});});}_LL67:;}
# 317
goto _LL5C;case 1: _LL5F: _LL60:
({struct _dyneither_ptr _tmp30B=({const char*_tmp80="struct constructor";_tag_dyneither(_tmp80,sizeof(char),19);});hides_what=_tmp30B;});goto _LL5C;case 2: _LL61: _LL62:
({struct _dyneither_ptr _tmp30C=({const char*_tmp81="datatype constructor";_tag_dyneither(_tmp81,sizeof(char),21);});hides_what=_tmp30C;});goto _LL5C;case 3: _LL63: _LL64:
 goto _LL66;default: _LL65: _LL66:
({struct _dyneither_ptr _tmp30D=({const char*_tmp82="enum tag";_tag_dyneither(_tmp82,sizeof(char),9);});hides_what=_tmp30D;});goto _LL5C;}_LL5C:;}
# 323
({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)hides_what);({void*_tmp84[1]={& _tmp86};({unsigned int _tmp30F=loc;struct _dyneither_ptr _tmp30E=({const char*_tmp85="declaration hides %s";_tag_dyneither(_tmp85,sizeof(char),21);});Cyc_Warn_warn(_tmp30F,_tmp30E,_tag_dyneither(_tmp84,sizeof(void*),1));});});});
_npop_handler(0);return;
# 305
;_pop_handler();}else{void*_tmp74=(void*)_exn_thrown;void*_tmp87=_tmp74;void*_tmp88;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp87)->tag == Cyc_Binding_BindingError){_LL75: _LL76:
# 325
 return;}else{_LL77: _tmp88=_tmp87;_LL78:(int)_rethrow(_tmp88);}_LL74:;}};}
# 328
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp89=vd->name;struct _tuple0*_tmp8A=_tmp89;union Cyc_Absyn_Nmspace _tmp95;struct _dyneither_ptr*_tmp94;_LL7A: _tmp95=_tmp8A->f1;_tmp94=_tmp8A->f2;_LL7B:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp315=({struct Cyc_Dict_Dict _tmp314=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp313=_tmp94;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp314,_tmp313,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp312=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp8C;_tmp8C.tag=0;({void*_tmp311=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp310=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp8E;_tmp8E.tag=4;_tmp8E.f1=vd;_tmp8E;});_tmp8D[0]=_tmp310;});_tmp8D;});_tmp8C.f1=_tmp311;});_tmp8C;});_tmp8B[0]=_tmp312;});_tmp8B;}));});*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp315;});else{
# 338
struct Cyc_Binding_ResolveNSEnv*_tmp8F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp95);
({struct Cyc_Dict_Dict _tmp31B=({struct Cyc_Dict_Dict _tmp31A=_tmp8F->ordinaries;struct _dyneither_ptr*_tmp319=_tmp94;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp31A,_tmp319,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp90=_cycalloc(sizeof(*_tmp90));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp318=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp91;_tmp91.tag=0;({void*_tmp317=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp92=_cycalloc(sizeof(*_tmp92));({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp316=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp93;_tmp93.tag=1;_tmp93.f1=vd;_tmp93;});_tmp92[0]=_tmp316;});_tmp92;});_tmp91.f1=_tmp317;});_tmp91;});_tmp90[0]=_tmp318;});_tmp90;}));});_tmp8F->ordinaries=_tmp31B;});}};}
# 345
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp96=s->r;void*_tmp97=_tmp96;struct Cyc_Absyn_Stmt*_tmpAF;struct Cyc_List_List*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_List_List*_tmpAC;struct Cyc_Absyn_Decl*_tmpAB;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_Absyn_Stmt*_tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Stmt*_tmp9B;struct Cyc_Absyn_Stmt*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;switch(*((int*)_tmp97)){case 0: _LL7D: _LL7E:
 return;case 1: _LL7F: _tmp99=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL80:
 Cyc_Binding_resolve_exp(env,_tmp99);return;case 2: _LL81: _tmp9B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL82:
 Cyc_Binding_resolve_stmt(env,_tmp9B);Cyc_Binding_resolve_stmt(env,_tmp9A);return;case 3: _LL83: _tmp9C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL84:
 if((unsigned int)_tmp9C)Cyc_Binding_resolve_exp(env,_tmp9C);return;case 4: _LL85: _tmp9F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmp9E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_tmp9D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f3;_LL86:
# 352
 Cyc_Binding_resolve_exp(env,_tmp9F);Cyc_Binding_resolve_stmt(env,_tmp9E);Cyc_Binding_resolve_stmt(env,_tmp9D);return;case 14: _LL87: _tmpA1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpA0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp97)->f2).f1;_LL88:
 _tmpA3=_tmpA0;_tmpA2=_tmpA1;goto _LL8A;case 5: _LL89: _tmpA3=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp97)->f1).f1;_tmpA2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL8A:
 Cyc_Binding_resolve_exp(env,_tmpA3);Cyc_Binding_resolve_stmt(env,_tmpA2);return;case 9: _LL8B: _tmpA7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpA6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f2).f1;_tmpA5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f3).f1;_tmpA4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f4;_LL8C:
# 356
 Cyc_Binding_resolve_exp(env,_tmpA7);Cyc_Binding_resolve_exp(env,_tmpA6);Cyc_Binding_resolve_exp(env,_tmpA5);
Cyc_Binding_resolve_stmt(env,_tmpA4);
return;case 6: _LL8D: _LL8E:
 goto _LL90;case 7: _LL8F: _LL90:
 goto _LL92;case 8: _LL91: _LL92:
 return;case 11: _LL93: _tmpA8=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LL94:
# 363
 for(0;_tmpA8 != 0;_tmpA8=_tmpA8->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpA8->hd);}
return;case 13: _LL95: _tmpA9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL96:
 Cyc_Binding_resolve_stmt(env,_tmpA9);return;case 12: _LL97: _tmpAB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpAA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL98: {
# 368
struct Cyc_Dict_Dict _tmp98=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmpAB);
Cyc_Binding_resolve_stmt(env,_tmpAA);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp98;
return;}case 10: _LL99: _tmpAD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpAC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL9A:
 Cyc_Binding_resolve_exp(env,_tmpAD);Cyc_Binding_resolve_scs(env,_tmpAC);return;default: _LL9B: _tmpAF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpAE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL9C:
 Cyc_Binding_resolve_stmt(env,_tmpAF);Cyc_Binding_resolve_scs(env,_tmpAE);return;}_LL7C:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 377
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpB0=e->r;void*_tmpB1=_tmpB0;void*_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Datatypedecl*_tmp13B;struct Cyc_Absyn_Datatypefield*_tmp13A;struct Cyc_Absyn_Enumdecl*_tmp139;struct Cyc_Absyn_Enumfield*_tmp138;void*_tmp137;struct Cyc_Absyn_Enumfield*_tmp136;struct Cyc_List_List*_tmp135;void*_tmp134;void*_tmp133;void*_tmp132;struct Cyc_Absyn_Stmt*_tmp131;void*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;void**_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;void*_tmp10C;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Vardecl*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct _tuple0**_tmp107;struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp105;struct Cyc_Absyn_Aggrdecl**_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_List_List*_tmp102;int*_tmp101;void**_tmp100;switch(*((int*)_tmpB1)){case 1: _LL9E: _tmp100=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LL9F: {
# 380
void*_tmpB2=*_tmp100;void*_tmpB3=_tmpB2;struct _tuple0*_tmpCF;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpB3)->tag == 0){_LLF1: _tmpCF=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpB3)->f1;_LLF2: {
# 382
struct _handler_cons _tmpB4;_push_handler(& _tmpB4);{int _tmpB6=0;if(setjmp(_tmpB4.handler))_tmpB6=1;if(!_tmpB6){{void*_tmpB7=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpCF);void*_tmpB8=_tmpB7;struct Cyc_Absyn_Aggrdecl*_tmpC9;void*_tmpC8;struct Cyc_Absyn_Enumfield*_tmpC7;struct Cyc_Absyn_Enumdecl*_tmpC6;struct Cyc_Absyn_Enumfield*_tmpC5;struct Cyc_Absyn_Datatypedecl*_tmpC4;struct Cyc_Absyn_Datatypefield*_tmpC3;void*_tmpC2;switch(*((int*)_tmpB8)){case 0: _LLF6: _tmpC2=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpB8)->f1;_LLF7:
*_tmp100=_tmpC2;_npop_handler(0);return;case 2: _LLF8: _tmpC4=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpB8)->f1;_tmpC3=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpB8)->f2;_LLF9:
({void*_tmp31D=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp31C=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpBA;_tmpBA.tag=31;_tmpBA.f1=0;_tmpBA.f2=_tmpC4;_tmpBA.f3=_tmpC3;_tmpBA;});_tmpB9[0]=_tmp31C;});_tmpB9;});e->r=_tmp31D;});_npop_handler(0);return;case 3: _LLFA: _tmpC6=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpB8)->f1;_tmpC5=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpB8)->f2;_LLFB:
({void*_tmp31F=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp31E=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpBC;_tmpBC.tag=32;_tmpBC.f1=_tmpC6;_tmpBC.f2=_tmpC5;_tmpBC;});_tmpBB[0]=_tmp31E;});_tmpBB;});e->r=_tmp31F;});_npop_handler(0);return;case 4: _LLFC: _tmpC8=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpB8)->f1;_tmpC7=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpB8)->f2;_LLFD:
({void*_tmp321=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp320=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpBE;_tmpBE.tag=33;_tmpBE.f1=_tmpC8;_tmpBE.f2=_tmpC7;_tmpBE;});_tmpBD[0]=_tmp320;});_tmpBD;});e->r=_tmp321;});_npop_handler(0);return;default: _LLFE: _tmpC9=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpB8)->f1;_LLFF:
# 388
({struct Cyc_String_pa_PrintArg_struct _tmpC1;_tmpC1.tag=0;({struct _dyneither_ptr _tmp322=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCF));_tmpC1.f1=_tmp322;});({void*_tmpBF[1]={& _tmpC1};({unsigned int _tmp324=e->loc;struct _dyneither_ptr _tmp323=({const char*_tmpC0="bad occurrence of type name %s";_tag_dyneither(_tmpC0,sizeof(char),31);});Cyc_Warn_err(_tmp324,_tmp323,_tag_dyneither(_tmpBF,sizeof(void*),1));});});});
_npop_handler(0);return;}_LLF5:;}
# 382
;_pop_handler();}else{void*_tmpB5=(void*)_exn_thrown;void*_tmpCA=_tmpB5;void*_tmpCE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpCA)->tag == Cyc_Binding_BindingError){_LL101: _LL102:
# 392
({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0;({struct _dyneither_ptr _tmp325=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCF));_tmpCD.f1=_tmp325;});({void*_tmpCB[1]={& _tmpCD};({unsigned int _tmp327=e->loc;struct _dyneither_ptr _tmp326=({const char*_tmpCC="undeclared identifier %s";_tag_dyneither(_tmpCC,sizeof(char),25);});Cyc_Warn_err(_tmp327,_tmp326,_tag_dyneither(_tmpCB,sizeof(void*),1));});});});
return;}else{_LL103: _tmpCE=_tmpCA;_LL104:(int)_rethrow(_tmpCE);}_LL100:;}};}}else{_LLF3: _LLF4:
# 395
 return;}_LLF0:;}case 10: _LLA0: _tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp102=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_tmp101=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB1)->f4;_LLA1:
# 398
*_tmp101=1;
# 400
{struct Cyc_List_List*_tmpD0=_tmp102;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpD0->hd);}}{
void*_tmpD1=_tmp103->r;void*_tmpD2=_tmpD1;void**_tmpF1;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD2)->tag == 1){_LL106: _tmpF1=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD2)->f1;_LL107: {
# 404
void*_tmpD3=*_tmpF1;void*_tmpD4=_tmpD3;struct _tuple0*_tmpF0;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD4)->tag == 0){_LL10B: _tmpF0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD4)->f1;_LL10C: {
# 406
struct _handler_cons _tmpD5;_push_handler(& _tmpD5);{int _tmpD7=0;if(setjmp(_tmpD5.handler))_tmpD7=1;if(!_tmpD7){{void*_tmpD8=Cyc_Binding_lookup_ordinary(_tmp103->loc,env,_tmpF0);void*_tmpD9=_tmpD8;struct Cyc_Absyn_Datatypedecl*_tmpEA;struct Cyc_Absyn_Datatypefield*_tmpE9;struct Cyc_Absyn_Aggrdecl*_tmpE8;void*_tmpE7;switch(*((int*)_tmpD9)){case 0: _LL110: _tmpE7=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpD9)->f1;_LL111:
# 408
*_tmpF1=_tmpE7;_npop_handler(0);return;case 1: _LL112: _tmpE8=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpD9)->f1;_LL113: {
# 410
struct Cyc_List_List*_tmpDA=0;
for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
({struct Cyc_List_List*_tmp329=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct _tuple10*_tmp328=({struct _tuple10*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=0;_tmpDC->f2=(struct Cyc_Absyn_Exp*)_tmp102->hd;_tmpDC;});_tmpDB->hd=_tmp328;});_tmpDB->tl=_tmpDA;_tmpDB;});_tmpDA=_tmp329;});}
({void*_tmp32C=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp32B=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpDE;_tmpDE.tag=29;_tmpDE.f1=_tmpE8->name;_tmpDE.f2=0;({struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDA);_tmpDE.f3=_tmp32A;});_tmpDE.f4=_tmpE8;_tmpDE;});_tmpDD[0]=_tmp32B;});_tmpDD;});e->r=_tmp32C;});
_npop_handler(0);return;}case 2: _LL114: _tmpEA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpD9)->f1;_tmpE9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpD9)->f2;_LL115:
# 416
 if(_tmpE9->typs == 0)
# 418
({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;({struct _dyneither_ptr _tmp32D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpE9->name));_tmpE1.f1=_tmp32D;});({void*_tmpDF[1]={& _tmpE1};({unsigned int _tmp32F=e->loc;struct _dyneither_ptr _tmp32E=({const char*_tmpE0="%s is a constant, not a function";_tag_dyneither(_tmpE0,sizeof(char),33);});Cyc_Warn_err(_tmp32F,_tmp32E,_tag_dyneither(_tmpDF,sizeof(void*),1));});});});
({void*_tmp331=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp330=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE3;_tmpE3.tag=31;_tmpE3.f1=_tmp102;_tmpE3.f2=_tmpEA;_tmpE3.f3=_tmpE9;_tmpE3;});_tmpE2[0]=_tmp330;});_tmpE2;});e->r=_tmp331;});
_npop_handler(0);return;case 4: _LL116: _LL117:
 goto _LL119;default: _LL118: _LL119:
# 424
({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0;({struct _dyneither_ptr _tmp332=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpF0));_tmpE6.f1=_tmp332;});({void*_tmpE4[1]={& _tmpE6};({unsigned int _tmp334=e->loc;struct _dyneither_ptr _tmp333=({const char*_tmpE5="%s is an enum constructor, not a function";_tag_dyneither(_tmpE5,sizeof(char),42);});Cyc_Warn_err(_tmp334,_tmp333,_tag_dyneither(_tmpE4,sizeof(void*),1));});});});
_npop_handler(0);return;}_LL10F:;}
# 406
;_pop_handler();}else{void*_tmpD6=(void*)_exn_thrown;void*_tmpEB=_tmpD6;void*_tmpEF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpEB)->tag == Cyc_Binding_BindingError){_LL11B: _LL11C:
# 428
({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0;({struct _dyneither_ptr _tmp335=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF0));_tmpEE.f1=_tmp335;});({void*_tmpEC[1]={& _tmpEE};({unsigned int _tmp337=e->loc;struct _dyneither_ptr _tmp336=({const char*_tmpED="undeclared identifier %s";_tag_dyneither(_tmpED,sizeof(char),25);});Cyc_Warn_err(_tmp337,_tmp336,_tag_dyneither(_tmpEC,sizeof(void*),1));});});});
return;}else{_LL11D: _tmpEF=_tmpEB;_LL11E:(int)_rethrow(_tmpEF);}_LL11A:;}};}}else{_LL10D: _LL10E:
# 431
 return;}_LL10A:;}}else{_LL108: _LL109:
# 433
 Cyc_Binding_resolve_exp(env,_tmp103);return;}_LL105:;};case 29: _LLA2: _tmp107=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp106=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_tmp105=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB1)->f3;_tmp104=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB1)->f4;_LLA3:
# 437
 for(0;_tmp105 != 0;_tmp105=_tmp105->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp105->hd)).f2);}{
struct _handler_cons _tmpF2;_push_handler(& _tmpF2);{int _tmpF4=0;if(setjmp(_tmpF2.handler))_tmpF4=1;if(!_tmpF4){
({struct Cyc_Absyn_Aggrdecl*_tmp338=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmp107,Cyc_Binding_lookup_aggrdecl);*_tmp104=_tmp338;});
*_tmp107=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmp104))->name;
_npop_handler(0);return;
# 440
;_pop_handler();}else{void*_tmpF3=(void*)_exn_thrown;void*_tmpF5=_tmpF3;void*_tmpF9;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpF5)->tag == Cyc_Binding_BindingError){_LL120: _LL121:
# 444
({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;({struct _dyneither_ptr _tmp339=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp107));_tmpF8.f1=_tmp339;});({void*_tmpF6[1]={& _tmpF8};({unsigned int _tmp33B=e->loc;struct _dyneither_ptr _tmp33A=({const char*_tmpF7="unbound struct/union name %s";_tag_dyneither(_tmpF7,sizeof(char),29);});Cyc_Warn_err(_tmp33B,_tmp33A,_tag_dyneither(_tmpF6,sizeof(void*),1));});});});
return;}else{_LL122: _tmpF9=_tmpF5;_LL123:(int)_rethrow(_tmpF9);}_LL11F:;}};};case 27: _LLA4: _tmp10A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp109=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_tmp108=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB1)->f3;_LLA5:
# 449
 Cyc_Binding_resolve_exp(env,_tmp109);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp33D=({struct Cyc_Dict_Dict*_tmpFA=_cycalloc(sizeof(*_tmpFA));({struct Cyc_Dict_Dict _tmp33C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmpFA[0]=_tmp33C;});_tmpFA;});env->local_vars=_tmp33D;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp10A);
Cyc_Binding_resolve_exp(env,_tmp108);
env->local_vars=0;
return;}{
# 457
struct Cyc_Dict_Dict _tmpFB=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp10A);
Cyc_Binding_resolve_exp(env,_tmp108);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpFB;
return;};case 36: _LLA6: _tmp10B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLA7:
# 465
 for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp10B->hd)).f2);}
return;case 28: _LLA8: _tmp10D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp10C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLA9:
# 471
 Cyc_Binding_resolve_exp(env,_tmp10D);
Cyc_Binding_resolve_type(e->loc,env,_tmp10C);
return;case 2: _LLAA: _LLAB:
# 475
 goto _LLAD;case 0: _LLAC: _LLAD:
 return;case 24: _LLAE: _tmp10E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLAF:
# 478
 _tmp10F=_tmp10E;goto _LLB1;case 3: _LLB0: _tmp10F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLB1:
# 480
 for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp10F->hd);}
return;case 38: _LLB2: _tmp110=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLB3:
# 484
 _tmp111=_tmp110;goto _LLB5;case 12: _LLB4: _tmp111=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLB5:
 _tmp112=_tmp111;goto _LLB7;case 18: _LLB6: _tmp112=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLB7:
 _tmp113=_tmp112;goto _LLB9;case 11: _LLB8: _tmp113=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLB9:
 _tmp114=_tmp113;goto _LLBB;case 5: _LLBA: _tmp114=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLBB:
 _tmp115=_tmp114;goto _LLBD;case 22: _LLBC: _tmp115=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLBD:
 _tmp116=_tmp115;goto _LLBF;case 21: _LLBE: _tmp116=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLBF:
 _tmp117=_tmp116;goto _LLC1;case 15: _LLC0: _tmp117=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLC1:
 _tmp118=_tmp117;goto _LLC3;case 20: _LLC2: _tmp118=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLC3:
 Cyc_Binding_resolve_exp(env,_tmp118);return;case 35: _LLC4: _tmp11A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp119=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLC5:
# 494
 _tmp11C=_tmp11A;_tmp11B=_tmp119;goto _LLC7;case 9: _LLC6: _tmp11C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp11B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLC7:
 _tmp11E=_tmp11C;_tmp11D=_tmp11B;goto _LLC9;case 4: _LLC8: _tmp11E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp11D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB1)->f3;_LLC9:
 _tmp120=_tmp11E;_tmp11F=_tmp11D;goto _LLCB;case 23: _LLCA: _tmp120=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp11F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLCB:
 _tmp122=_tmp120;_tmp121=_tmp11F;goto _LLCD;case 7: _LLCC: _tmp122=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp121=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLCD:
 _tmp124=_tmp122;_tmp123=_tmp121;goto _LLCF;case 8: _LLCE: _tmp124=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp123=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLCF:
 Cyc_Binding_resolve_exp(env,_tmp124);Cyc_Binding_resolve_exp(env,_tmp123);return;case 34: _LLD0: _tmp127=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB1)->f1).rgn;_tmp126=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB1)->f1).elt_type;_tmp125=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB1)->f1).num_elts;_LLD1:
# 502
 if(_tmp127 != 0)Cyc_Binding_resolve_exp(env,_tmp127);
if(_tmp126 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp126);
Cyc_Binding_resolve_exp(env,_tmp125);
return;case 16: _LLD2: _tmp129=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp128=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLD3:
# 508
 if(_tmp129 != 0)Cyc_Binding_resolve_exp(env,_tmp129);
Cyc_Binding_resolve_exp(env,_tmp128);
return;case 6: _LLD4: _tmp12C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp12B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_tmp12A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB1)->f3;_LLD5:
# 513
 Cyc_Binding_resolve_exp(env,_tmp12C);Cyc_Binding_resolve_exp(env,_tmp12B);Cyc_Binding_resolve_exp(env,_tmp12A);return;case 13: _LLD6: _tmp12E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp12D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLD7:
# 516
 Cyc_Binding_resolve_exp(env,_tmp12E);
for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp12D->hd);}
return;case 14: _LLD8: _tmp130=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp12F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLD9:
# 522
 Cyc_Binding_resolve_exp(env,_tmp12F);Cyc_Binding_resolve_type(e->loc,env,_tmp130);return;case 37: _LLDA: _tmp131=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLDB:
# 524
 Cyc_Binding_resolve_stmt(env,_tmp131);return;case 39: _LLDC: _tmp132=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLDD:
# 526
 _tmp133=_tmp132;goto _LLDF;case 19: _LLDE: _tmp133=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLDF:
 _tmp134=_tmp133;goto _LLE1;case 17: _LLE0: _tmp134=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLE1:
 Cyc_Binding_resolve_type(e->loc,env,_tmp134);return;case 26: _LLE2: _tmp135=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_LLE3:
# 531
 for(0;_tmp135 != 0;_tmp135=_tmp135->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp135->hd)).f2);}
return;case 25: _LLE4: _LLE5:
# 536
({void*_tmpFC=0;({unsigned int _tmp33F=e->loc;struct _dyneither_ptr _tmp33E=({const char*_tmpFD="binding: compound literals unimplemented";_tag_dyneither(_tmpFD,sizeof(char),41);});Cyc_Warn_err(_tmp33F,_tmp33E,_tag_dyneither(_tmpFC,sizeof(void*),0));});});return;case 33: _LLE6: _tmp137=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp136=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLE7:
# 539
 Cyc_Binding_resolve_type(e->loc,env,_tmp137);return;case 32: _LLE8: _tmp139=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp138=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLE9:
 return;case 31: _LLEA: _tmp13C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp13B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_tmp13A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB1)->f3;_LLEB:
# 542
 for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp13C->hd);}
return;case 30: _LLEC: _tmp13E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB1)->f1;_tmp13D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB1)->f2;_LLED:
# 546
 Cyc_Binding_resolve_type(e->loc,env,_tmp13E);
for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp13D->hd)).f2);}
return;default: _LLEE: _LLEF:
# 552
({void*_tmpFE=0;({unsigned int _tmp341=e->loc;struct _dyneither_ptr _tmp340=({const char*_tmpFF="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmpFF,sizeof(char),49);});Cyc_Warn_err(_tmp341,_tmp340,_tag_dyneither(_tmpFE,sizeof(void*),0));});});}_LL9D:;}
# 556
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp13F=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp140=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp141=_tmp140;struct Cyc_Absyn_Pat*_tmp144;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Stmt*_tmp142;_LL125: _tmp144=_tmp141->pattern;_tmp143=_tmp141->where_clause;_tmp142=_tmp141->body;_LL126:;
Cyc_Binding_resolve_pat(env,_tmp144);
if(_tmp143 != 0)
Cyc_Binding_resolve_exp(env,_tmp143);
Cyc_Binding_resolve_stmt(env,_tmp142);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp13F;}
# 566
return;}
# 568
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp145=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp146=_tmp145;struct _dyneither_ptr*_tmp14C;struct Cyc_Absyn_Tqual _tmp14B;void*_tmp14A;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Exp*_tmp147;_LL128: _tmp14C=_tmp146->name;_tmp14B=_tmp146->tq;_tmp14A=_tmp146->type;_tmp149=_tmp146->width;_tmp148=_tmp146->attributes;_tmp147=_tmp146->requires_clause;_LL129:;
Cyc_Binding_resolve_type(loc,env,_tmp14A);
if(_tmp149 != 0)
Cyc_Binding_resolve_exp(env,_tmp149);
if(_tmp147 != 0)
Cyc_Binding_resolve_exp(env,_tmp147);}
# 577
return;}struct _tuple11{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 580
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 586
struct Cyc_List_List*_tmp14D=0;
for(0;args != 0;args=args->tl){
struct _tuple11*_tmp14E=(struct _tuple11*)args->hd;struct _tuple11*_tmp14F=_tmp14E;void*_tmp156;struct Cyc_Absyn_Tqual _tmp155;void*_tmp154;_LL12B: _tmp156=_tmp14F->f1;_tmp155=_tmp14F->f2;_tmp154=_tmp14F->f3;_LL12C:;{
struct _dyneither_ptr*_tmp150=f(_tmp156);
if(_tmp150 == 0)continue;{
struct Cyc_Absyn_Vardecl*_tmp151=({struct _tuple0*_tmp342=({struct _tuple0*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->f1=Cyc_Absyn_Loc_n;_tmp153->f2=_tmp150;_tmp153;});Cyc_Absyn_new_vardecl(0,_tmp342,_tmp154,0);});
_tmp151->tq=_tmp155;
({struct Cyc_List_List*_tmp343=({struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->hd=_tmp151;_tmp152->tl=_tmp14D;_tmp152;});_tmp14D=_tmp343;});};};}
# 595
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp157=*vai;struct Cyc_Absyn_VarargInfo _tmp158=_tmp157;struct _dyneither_ptr*_tmp162;struct Cyc_Absyn_Tqual _tmp161;void*_tmp160;int _tmp15F;_LL12E: _tmp162=_tmp158.name;_tmp161=_tmp158.tq;_tmp160=_tmp158.type;_tmp15F=_tmp158.inject;_LL12F:;
if(_tmp162 == 0){
if(need_va_name)
({void*_tmp159=0;({unsigned int _tmp345=loc;struct _dyneither_ptr _tmp344=({const char*_tmp15A="missing name for vararg";_tag_dyneither(_tmp15A,sizeof(char),24);});Cyc_Warn_err(_tmp345,_tmp344,_tag_dyneither(_tmp159,sizeof(void*),0));});});}else{
# 602
void*_tmp15B=Cyc_Absyn_dyneither_typ(_tmp160,(void*)& Cyc_Absyn_HeapRgn_val,_tmp161,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp15C=({struct _tuple0*_tmp346=({struct _tuple0*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->f1=Cyc_Absyn_Loc_n;_tmp15E->f2=_tmp162;_tmp15E;});Cyc_Absyn_new_vardecl(0,_tmp346,_tmp15B,0);});
({struct Cyc_List_List*_tmp347=({struct Cyc_List_List*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->hd=_tmp15C;_tmp15D->tl=_tmp14D;_tmp15D;});_tmp14D=_tmp347;});}}
# 607
({struct Cyc_List_List*_tmp348=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14D);_tmp14D=_tmp348;});
return _tmp14D;}
# 610
struct _dyneither_ptr*Cyc_Binding_get_vds_fndecl(struct _dyneither_ptr*v){return v;}
struct _dyneither_ptr*Cyc_Binding_get_vds_fntype(struct _dyneither_ptr*v){return v;}
# 613
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause){
# 617
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
# 629
struct Cyc_Dict_Dict _tmp163=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp164=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp165=({struct _tuple0*_tmp349=({struct _tuple0*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->f1=Cyc_Absyn_Loc_n;_tmp16A->f2=_tmp164;_tmp16A;});Cyc_Absyn_new_vardecl(0,_tmp349,rettyp,0);});
({struct Cyc_Dict_Dict _tmp34F=({struct Cyc_Dict_Dict _tmp34E=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp34D=_tmp164;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp34E,_tmp34D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp166=_cycalloc(sizeof(*_tmp166));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp34C=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp167;_tmp167.tag=0;({void*_tmp34B=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp168=_cycalloc(sizeof(*_tmp168));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp34A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp169;_tmp169.tag=4;_tmp169.f1=_tmp165;_tmp169;});_tmp168[0]=_tmp34A;});_tmp168;});_tmp167.f1=_tmp34B;});_tmp167;});_tmp166[0]=_tmp34C;});_tmp166;}));});*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp34F;});
# 634
Cyc_Binding_resolve_exp(env,ens_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp163;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 644
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp16C=t;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EA;void*_tmp1E9;void*_tmp1E8;void*_tmp1E7;void*_tmp1E6;void*_tmp1E5;void*_tmp1E4;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_FnInfo _tmp1E2;void*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_List_List*_tmp1DF;void*_tmp1DE;void*_tmp1DD;union Cyc_Absyn_Constraint*_tmp1DC;void*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_TypeDecl*_tmp1D8;void***_tmp1D7;struct _tuple0**_tmp1D6;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Typedefdecl**_tmp1D4;struct _tuple0**_tmp1D3;struct Cyc_Absyn_Enumdecl*_tmp1D2;union Cyc_Absyn_AggrInfoU*_tmp1D1;struct Cyc_List_List*_tmp1D0;union Cyc_Absyn_DatatypeFieldInfoU*_tmp1CF;struct Cyc_List_List*_tmp1CE;union Cyc_Absyn_DatatypeInfoU*_tmp1CD;struct Cyc_List_List*_tmp1CC;switch(*((int*)_tmp16C)){case 3: _LL131: _tmp1CD=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C)->f1).datatype_info;_tmp1CC=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C)->f1).targs;_LL132:
# 647
 for(0;_tmp1CC != 0;_tmp1CC=_tmp1CC->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CC->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp16D=*_tmp1CD;union Cyc_Absyn_DatatypeInfoU _tmp16E=_tmp16D;struct _tuple0*_tmp177;int _tmp176;if((_tmp16E.UnknownDatatype).tag == 1){_LL16C: _tmp177=((_tmp16E.UnknownDatatype).val).name;_tmp176=((_tmp16E.UnknownDatatype).val).is_extensible;_LL16D: {
# 651
struct _handler_cons _tmp16F;_push_handler(& _tmp16F);{int _tmp171=0;if(setjmp(_tmp16F.handler))_tmp171=1;if(!_tmp171){
{struct Cyc_Absyn_Datatypedecl*_tmp172=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp177,Cyc_Binding_lookup_datatypedecl))->hd;
({union Cyc_Absyn_DatatypeInfoU _tmp350=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp173;_tmp173.name=_tmp172->name;_tmp173.is_extensible=_tmp176;_tmp173;}));*_tmp1CD=_tmp350;});
_npop_handler(0);return;}
# 652
;_pop_handler();}else{void*_tmp170=(void*)_exn_thrown;void*_tmp174=_tmp170;void*_tmp175;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp174)->tag == Cyc_Binding_BindingError){_LL171: _LL172:
# 656
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp177,Cyc_Absyn_Public);return;}else{_LL173: _tmp175=_tmp174;_LL174:(int)_rethrow(_tmp175);}_LL170:;}};}}else{_LL16E: _LL16F:
 return;}_LL16B:;};case 4: _LL133: _tmp1CF=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp16C)->f1).field_info;_tmp1CE=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp16C)->f1).targs;_LL134:
# 660
 for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CE->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp178=*_tmp1CF;union Cyc_Absyn_DatatypeFieldInfoU _tmp179=_tmp178;struct _tuple0*_tmp18F;struct _tuple0*_tmp18E;int _tmp18D;if((_tmp179.UnknownDatatypefield).tag == 1){_LL176: _tmp18F=((_tmp179.UnknownDatatypefield).val).datatype_name;_tmp18E=((_tmp179.UnknownDatatypefield).val).field_name;_tmp18D=((_tmp179.UnknownDatatypefield).val).is_extensible;_LL177:
# 666
{struct _handler_cons _tmp17A;_push_handler(& _tmp17A);{int _tmp17C=0;if(setjmp(_tmp17A.handler))_tmp17C=1;if(!_tmp17C){
{void*_tmp17D=Cyc_Binding_lookup_ordinary(loc,env,_tmp18E);void*_tmp17E=_tmp17D;struct Cyc_Absyn_Datatypedecl*_tmp187;struct Cyc_Absyn_Datatypefield*_tmp186;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17E)->tag == 2){_LL17B: _tmp187=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17E)->f1;_tmp186=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17E)->f2;_LL17C: {
# 669
struct Cyc_Absyn_Datatypedecl*_tmp17F=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp18F,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp187->name,_tmp17F->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0;({struct _dyneither_ptr _tmp351=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 674
Cyc_Absynpp_qvar2string(_tmp187->name));_tmp184.f1=_tmp351;});({struct Cyc_String_pa_PrintArg_struct _tmp183;_tmp183.tag=0;({struct _dyneither_ptr _tmp352=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 673
Cyc_Absynpp_qvar2string(_tmp17F->name));_tmp183.f1=_tmp352;});({struct Cyc_String_pa_PrintArg_struct _tmp182;_tmp182.tag=0;({struct _dyneither_ptr _tmp353=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 672
Cyc_Absynpp_qvar2string(_tmp186->name));_tmp182.f1=_tmp353;});({void*_tmp180[3]={& _tmp182,& _tmp183,& _tmp184};({unsigned int _tmp355=loc;struct _dyneither_ptr _tmp354=({const char*_tmp181="%s is a variant of %s, not %s (shadowing not yet implemented properly)";_tag_dyneither(_tmp181,sizeof(char),71);});Cyc_Warn_err(_tmp355,_tmp354,_tag_dyneither(_tmp180,sizeof(void*),3));});});});});});
# 675
_npop_handler(0);return;}
# 677
({union Cyc_Absyn_DatatypeFieldInfoU _tmp356=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp185;_tmp185.datatype_name=_tmp187->name;_tmp185.field_name=_tmp186->name;_tmp185.is_extensible=_tmp18D;_tmp185;}));*_tmp1CF=_tmp356;});
# 679
_npop_handler(0);return;}}else{_LL17D: _LL17E:
 goto _LL17A;}_LL17A:;}
# 667
;_pop_handler();}else{void*_tmp17B=(void*)_exn_thrown;void*_tmp188=_tmp17B;void*_tmp189;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp188)->tag == Cyc_Binding_BindingError){_LL180: _LL181:
# 682
 goto _LL17F;}else{_LL182: _tmp189=_tmp188;_LL183:(int)_rethrow(_tmp189);}_LL17F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp18C;_tmp18C.tag=0;({struct _dyneither_ptr _tmp357=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp18E));_tmp18C.f1=_tmp357;});({void*_tmp18A[1]={& _tmp18C};({unsigned int _tmp359=loc;struct _dyneither_ptr _tmp358=({const char*_tmp18B="%s is not a datatype field";_tag_dyneither(_tmp18B,sizeof(char),27);});Cyc_Warn_err(_tmp359,_tmp358,_tag_dyneither(_tmp18A,sizeof(void*),1));});});});
return;}else{_LL178: _LL179:
 return;}_LL175:;};case 11: _LL135: _tmp1D1=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16C)->f1).aggr_info;_tmp1D0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16C)->f1).targs;_LL136:
# 689
 for(0;_tmp1D0 != 0;_tmp1D0=_tmp1D0->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D0->hd);}{
union Cyc_Absyn_AggrInfoU _tmp190=*_tmp1D1;union Cyc_Absyn_AggrInfoU _tmp191=_tmp190;enum Cyc_Absyn_AggrKind _tmp19E;struct _tuple0*_tmp19D;struct Cyc_Core_Opt*_tmp19C;if((_tmp191.UnknownAggr).tag == 1){_LL185: _tmp19E=((_tmp191.UnknownAggr).val).f1;_tmp19D=((_tmp191.UnknownAggr).val).f2;_tmp19C=((_tmp191.UnknownAggr).val).f3;_LL186: {
# 693
struct _handler_cons _tmp192;_push_handler(& _tmp192);{int _tmp194=0;if(setjmp(_tmp192.handler))_tmp194=1;if(!_tmp194){
{struct Cyc_Absyn_Aggrdecl*_tmp195=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp19D,Cyc_Binding_lookup_aggrdecl);
if(_tmp195->kind != _tmp19E)
({void*_tmp196=0;({unsigned int _tmp35B=loc;struct _dyneither_ptr _tmp35A=({const char*_tmp197="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp197,sizeof(char),51);});Cyc_Warn_err(_tmp35B,_tmp35A,_tag_dyneither(_tmp196,sizeof(void*),0));});});
if(((_tmp19E == Cyc_Absyn_UnionA  && _tmp19C != 0) && _tmp195->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp195->impl))->tagged != (int)_tmp19C->v)
# 699
({void*_tmp198=0;({unsigned int _tmp35D=loc;struct _dyneither_ptr _tmp35C=({const char*_tmp199="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp199,sizeof(char),42);});Cyc_Warn_err(_tmp35D,_tmp35C,_tag_dyneither(_tmp198,sizeof(void*),0));});});
({union Cyc_Absyn_AggrInfoU _tmp35E=Cyc_Absyn_UnknownAggr(_tmp19E,_tmp195->name,_tmp19C);*_tmp1D1=_tmp35E;});
_npop_handler(0);return;}
# 694
;_pop_handler();}else{void*_tmp193=(void*)_exn_thrown;void*_tmp19A=_tmp193;void*_tmp19B;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp19A)->tag == Cyc_Binding_BindingError){_LL18A: _LL18B:
# 703
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp19D,Cyc_Absyn_Public);return;}else{_LL18C: _tmp19B=_tmp19A;_LL18D:(int)_rethrow(_tmp19B);}_LL189:;}};}}else{_LL187: _LL188:
 return;}_LL184:;};case 13: _LL137: _tmp1D3=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16C)->f1;_tmp1D2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16C)->f2;_LL138:
# 707
 if(_tmp1D2 != 0)
return;{
struct _handler_cons _tmp19F;_push_handler(& _tmp19F);{int _tmp1A1=0;if(setjmp(_tmp19F.handler))_tmp1A1=1;if(!_tmp1A1){
{struct Cyc_Absyn_Enumdecl*_tmp1A2=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1D3,Cyc_Binding_lookup_enumdecl);
*_tmp1D3=_tmp1A2->name;
_npop_handler(0);return;}
# 710
;_pop_handler();}else{void*_tmp1A0=(void*)_exn_thrown;void*_tmp1A3=_tmp1A0;void*_tmp1A4;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A3)->tag == Cyc_Binding_BindingError){_LL18F: _LL190:
# 714
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1D3,Cyc_Absyn_Public);return;}else{_LL191: _tmp1A4=_tmp1A3;_LL192:(int)_rethrow(_tmp1A4);}_LL18E:;}};};case 17: _LL139: _tmp1D6=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16C)->f1;_tmp1D5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16C)->f2;_tmp1D4=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16C)->f3;_LL13A:
# 717
 for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D5->hd);}
{struct _handler_cons _tmp1A5;_push_handler(& _tmp1A5);{int _tmp1A7=0;if(setjmp(_tmp1A5.handler))_tmp1A7=1;if(!_tmp1A7){
{struct Cyc_Absyn_Typedefdecl*_tmp1A8=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1D6,Cyc_Binding_lookup_typedefdecl);
# 722
*_tmp1D6=_tmp1A8->name;
_npop_handler(0);return;}
# 720
;_pop_handler();}else{void*_tmp1A6=(void*)_exn_thrown;void*_tmp1A9=_tmp1A6;void*_tmp1AA;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A9)->tag == Cyc_Binding_BindingError){_LL194: _LL195:
# 724
 goto _LL193;}else{_LL196: _tmp1AA=_tmp1A9;_LL197:(int)_rethrow(_tmp1AA);}_LL193:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0;({struct _dyneither_ptr _tmp35F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1D6));_tmp1AD.f1=_tmp35F;});({void*_tmp1AB[1]={& _tmp1AD};({unsigned int _tmp361=loc;struct _dyneither_ptr _tmp360=({const char*_tmp1AC="unbound typdef name %s";_tag_dyneither(_tmp1AC,sizeof(char),23);});Cyc_Warn_err(_tmp361,_tmp360,_tag_dyneither(_tmp1AB,sizeof(void*),1));});});});
return;case 26: _LL13B: _tmp1D8=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C)->f1;_tmp1D7=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C)->f2;_LL13C: {
# 730
struct Cyc_Dict_Dict*_tmp1AE=env->local_vars;
env->local_vars=0;
{void*_tmp1AF=_tmp1D8->r;void*_tmp1B0=_tmp1AF;struct Cyc_Absyn_Datatypedecl*_tmp1BC;struct Cyc_Absyn_Enumdecl*_tmp1BB;struct Cyc_Absyn_Aggrdecl*_tmp1BA;switch(*((int*)_tmp1B0)){case 0: _LL199: _tmp1BA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1B0)->f1;_LL19A:
# 734
({struct Cyc_Binding_Env*_tmp364=env;Cyc_Binding_resolve_decl(_tmp364,({struct Cyc_Absyn_Decl*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));({void*_tmp363=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp362=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1B3;_tmp1B3.tag=5;_tmp1B3.f1=_tmp1BA;_tmp1B3;});_tmp1B2[0]=_tmp362;});_tmp1B2;});_tmp1B1->r=_tmp363;});_tmp1B1->loc=_tmp1D8->loc;_tmp1B1;}));});goto _LL198;case 1: _LL19B: _tmp1BB=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1B0)->f1;_LL19C:
# 736
({struct Cyc_Binding_Env*_tmp367=env;Cyc_Binding_resolve_decl(_tmp367,({struct Cyc_Absyn_Decl*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));({void*_tmp366=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp365=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1B6;_tmp1B6.tag=7;_tmp1B6.f1=_tmp1BB;_tmp1B6;});_tmp1B5[0]=_tmp365;});_tmp1B5;});_tmp1B4->r=_tmp366;});_tmp1B4->loc=_tmp1D8->loc;_tmp1B4;}));});goto _LL198;default: _LL19D: _tmp1BC=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1B0)->f1;_LL19E:
# 738
({struct Cyc_Binding_Env*_tmp36A=env;Cyc_Binding_resolve_decl(_tmp36A,({struct Cyc_Absyn_Decl*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({void*_tmp369=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp368=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1B9;_tmp1B9.tag=6;_tmp1B9.f1=_tmp1BC;_tmp1B9;});_tmp1B8[0]=_tmp368;});_tmp1B8;});_tmp1B7->r=_tmp369;});_tmp1B7->loc=_tmp1D8->loc;_tmp1B7;}));});goto _LL198;}_LL198:;}
# 740
env->local_vars=_tmp1AE;
return;}case 12: _LL13D: _tmp1D9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16C)->f2;_LL13E:
# 747
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1D9);
return;case 14: _LL13F: _tmp1DA=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16C)->f1;_LL140:
# 752
 for(0;_tmp1DA != 0;_tmp1DA=_tmp1DA->tl){
struct Cyc_Absyn_Enumfield*_tmp1BD=(struct Cyc_Absyn_Enumfield*)_tmp1DA->hd;
if(_tmp1BD->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1BD->tag));}
# 757
return;case 1: _LL141: _tmp1DB=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16C)->f2;_LL142:
 if(_tmp1DB != 0)Cyc_Binding_resolve_type(loc,env,_tmp1DB);return;case 28: _LL143: _LL144:
# 760
 goto _LL146;case 0: _LL145: _LL146:
 goto _LL148;case 6: _LL147: _LL148:
 goto _LL14A;case 7: _LL149: _LL14A:
 goto _LL14C;case 20: _LL14B: _LL14C:
 goto _LL14E;case 21: _LL14D: _LL14E:
 goto _LL150;case 22: _LL14F: _LL150:
 goto _LL152;case 2: _LL151: _LL152:
 return;case 5: _LL153: _tmp1DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16C)->f1).elt_typ;_tmp1DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16C)->f1).ptr_atts).rgn;_tmp1DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16C)->f1).ptr_atts).bounds;_LL154:
# 770
 Cyc_Binding_resolve_type(loc,env,_tmp1DE);
Cyc_Binding_resolve_type(loc,env,_tmp1DD);
while(1){
union Cyc_Absyn_Constraint*_tmp1BE=_tmp1DC;void*_tmp1C2;union Cyc_Absyn_Constraint*_tmp1C1;switch((((union Cyc_Absyn_Constraint*)_tmp1BE)->Forward_constr).tag){case 3: _LL1A0: _LL1A1:
 return;case 2: _LL1A2: _tmp1C1=(_tmp1BE->Forward_constr).val;_LL1A3:
 _tmp1DC=_tmp1C1;continue;default: _LL1A4: _tmp1C2=(void*)(_tmp1BE->Eq_constr).val;_LL1A5: {
# 777
void*_tmp1BF=_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1BF)->tag == 0){_LL1A7: _LL1A8:
 return;}else{_LL1A9: _tmp1C0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1BF)->f1;_LL1AA:
 Cyc_Binding_resolve_exp(env,_tmp1C0);return;}_LL1A6:;}}_LL19F:;}case 10: _LL155: _tmp1DF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16C)->f1;_LL156:
# 783
 for(0;_tmp1DF != 0;_tmp1DF=_tmp1DF->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp1DF->hd)).f2);}
return;case 8: _LL157: _tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16C)->f1).elt_type;_tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16C)->f1).num_elts;_LL158:
# 787
 Cyc_Binding_resolve_type(loc,env,_tmp1E1);
if(_tmp1E0 != 0)Cyc_Binding_resolve_exp(env,_tmp1E0);
return;case 9: _LL159: _tmp1E2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1;_LL15A: {
# 794
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fntype,0,loc,env,_tmp1E2.args,_tmp1E2.cyc_varargs);
# 797
struct Cyc_Dict_Dict*_tmp1C4=env->local_vars;
if(_tmp1C4 != 0)
({struct Cyc_Dict_Dict*_tmp36B=({struct Cyc_Dict_Dict*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1C5;});env->local_vars=_tmp36B;});else{
# 801
({struct Cyc_Dict_Dict*_tmp36D=({struct Cyc_Dict_Dict*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));({struct Cyc_Dict_Dict _tmp36C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp1C6[0]=_tmp36C;});_tmp1C6;});env->local_vars=_tmp36D;});}
{struct Cyc_List_List*_tmp1C7=_tmp1C3;for(0;_tmp1C7 != 0;_tmp1C7=_tmp1C7->tl){
({struct Cyc_Dict_Dict _tmp373=({struct Cyc_Dict_Dict _tmp372=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp371=(*((struct Cyc_Absyn_Vardecl*)_tmp1C7->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp372,_tmp371,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp370=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp1C9;_tmp1C9.tag=0;({void*_tmp36F=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp36E=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp1CB;_tmp1CB.tag=3;_tmp1CB.f1=(struct Cyc_Absyn_Vardecl*)_tmp1C7->hd;_tmp1CB;});_tmp1CA[0]=_tmp36E;});_tmp1CA;});_tmp1C9.f1=_tmp36F;});_tmp1C9;});_tmp1C8[0]=_tmp370;});_tmp1C8;}));});*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp373;});}}
# 806
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp1E2.effect,_tmp1E2.ret_typ,_tmp1E2.args,_tmp1E2.cyc_varargs,_tmp1E2.rgn_po,_tmp1E2.requires_clause,_tmp1E2.ensures_clause);
# 810
env->local_vars=_tmp1C4;
return;}case 24: _LL15B: _tmp1E3=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp16C)->f1;_LL15C:
# 814
 for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1E3->hd);}
return;case 19: _LL15D: _tmp1E4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp16C)->f1;_LL15E:
# 818
 _tmp1E5=_tmp1E4;goto _LL160;case 23: _LL15F: _tmp1E5=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp16C)->f1;_LL160:
 _tmp1E6=_tmp1E5;goto _LL162;case 25: _LL161: _tmp1E6=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp16C)->f1;_LL162:
 _tmp1E7=_tmp1E6;goto _LL164;case 15: _LL163: _tmp1E7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp16C)->f1;_LL164:
 Cyc_Binding_resolve_type(loc,env,_tmp1E7);return;case 16: _LL165: _tmp1E9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp16C)->f1;_tmp1E8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp16C)->f2;_LL166:
# 824
 Cyc_Binding_resolve_type(loc,env,_tmp1E9);Cyc_Binding_resolve_type(loc,env,_tmp1E8);return;case 18: _LL167: _tmp1EA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp16C)->f1;_LL168:
# 826
 _tmp1EB=_tmp1EA;goto _LL16A;default: _LL169: _tmp1EB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp16C)->f1;_LL16A:
 Cyc_Binding_resolve_exp(env,_tmp1EB);return;}_LL130:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 831
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1EC=p;void*_tmp242;unsigned int _tmp241;_LL1AC: _tmp242=_tmp1EC->r;_tmp241=_tmp1EC->loc;_LL1AD:;{
void*_tmp1ED=_tmp242;struct Cyc_List_List*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_List_List*_tmp23E;struct Cyc_Absyn_Pat*_tmp23D;struct Cyc_Absyn_Vardecl*_tmp23C;struct Cyc_Absyn_Vardecl*_tmp23B;struct Cyc_Absyn_Vardecl*_tmp23A;struct Cyc_Absyn_Pat*_tmp239;struct Cyc_Absyn_Vardecl*_tmp238;struct Cyc_Absyn_Pat*_tmp237;struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp235;struct _tuple0*_tmp234;struct Cyc_List_List*_tmp233;struct Cyc_List_List*_tmp232;struct Cyc_List_List*_tmp231;int _tmp230;struct _tuple0*_tmp22F;struct Cyc_List_List*_tmp22E;int _tmp22D;struct _tuple0*_tmp22C;switch(*((int*)_tmp1ED)){case 15: _LL1AF: _tmp22C=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL1B0:
# 835
{struct _handler_cons _tmp1EE;_push_handler(& _tmp1EE);{int _tmp1F0=0;if(setjmp(_tmp1EE.handler))_tmp1F0=1;if(!_tmp1F0){{void*_tmp1F1=Cyc_Binding_lookup_ordinary(_tmp241,env,_tmp22C);void*_tmp1F2=_tmp1F1;void*_tmp200;struct Cyc_Absyn_Enumfield*_tmp1FF;struct Cyc_Absyn_Enumdecl*_tmp1FE;struct Cyc_Absyn_Enumfield*_tmp1FD;struct Cyc_Absyn_Datatypedecl*_tmp1FC;struct Cyc_Absyn_Datatypefield*_tmp1FB;switch(*((int*)_tmp1F2)){case 0: _LL1D8: _LL1D9:
 goto _LL1D7;case 1: _LL1DA: _LL1DB:
# 838
({void*_tmp1F3=0;({unsigned int _tmp375=_tmp241;struct _dyneither_ptr _tmp374=({const char*_tmp1F4="struct tag used without arguments in pattern";_tag_dyneither(_tmp1F4,sizeof(char),45);});Cyc_Warn_err(_tmp375,_tmp374,_tag_dyneither(_tmp1F3,sizeof(void*),0));});});
_npop_handler(0);return;case 2: _LL1DC: _tmp1FC=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1F2)->f1;_tmp1FB=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1F2)->f2;_LL1DD:
# 841
({void*_tmp377=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp376=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1F6;_tmp1F6.tag=8;_tmp1F6.f1=_tmp1FC;_tmp1F6.f2=_tmp1FB;_tmp1F6.f3=0;_tmp1F6.f4=0;_tmp1F6;});_tmp1F5[0]=_tmp376;});_tmp1F5;});p->r=_tmp377;});_npop_handler(0);return;case 3: _LL1DE: _tmp1FE=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1F2)->f1;_tmp1FD=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1F2)->f2;_LL1DF:
# 843
({void*_tmp379=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp378=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp1F8;_tmp1F8.tag=13;_tmp1F8.f1=_tmp1FE;_tmp1F8.f2=_tmp1FD;_tmp1F8;});_tmp1F7[0]=_tmp378;});_tmp1F7;});p->r=_tmp379;});_npop_handler(0);return;default: _LL1E0: _tmp200=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1F2)->f1;_tmp1FF=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1F2)->f2;_LL1E1:
# 845
({void*_tmp37B=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp37A=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp1FA;_tmp1FA.tag=14;_tmp1FA.f1=_tmp200;_tmp1FA.f2=_tmp1FF;_tmp1FA;});_tmp1F9[0]=_tmp37A;});_tmp1F9;});p->r=_tmp37B;});_npop_handler(0);return;}_LL1D7:;}
# 835
;_pop_handler();}else{void*_tmp1EF=(void*)_exn_thrown;void*_tmp201=_tmp1EF;void*_tmp202;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp201)->tag == Cyc_Binding_BindingError){_LL1E3: _LL1E4:
# 846
 goto _LL1E2;}else{_LL1E5: _tmp202=_tmp201;_LL1E6:(int)_rethrow(_tmp202);}_LL1E2:;}};}{
struct Cyc_Absyn_Vardecl*_tmp203=Cyc_Absyn_new_vardecl(0,_tmp22C,(void*)& Cyc_Absyn_VoidType_val,0);
Cyc_Binding_resolve_and_add_vardecl(_tmp241,env,_tmp203);
({void*_tmp37E=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp204=_cycalloc(sizeof(*_tmp204));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp37D=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp205;_tmp205.tag=1;_tmp205.f1=_tmp203;({struct Cyc_Absyn_Pat*_tmp37C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp205.f2=_tmp37C;});_tmp205;});_tmp204[0]=_tmp37D;});_tmp204;});p->r=_tmp37E;});
return;};case 16: _LL1B1: _tmp22F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp22E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_tmp22D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL1B2:
# 853
{struct Cyc_List_List*_tmp206=_tmp22E;for(0;_tmp206 != 0;_tmp206=_tmp206->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp206->hd);}}
{struct _handler_cons _tmp207;_push_handler(& _tmp207);{int _tmp209=0;if(setjmp(_tmp207.handler))_tmp209=1;if(!_tmp209){{void*_tmp20A=Cyc_Binding_lookup_ordinary(_tmp241,env,_tmp22F);void*_tmp20B=_tmp20A;struct Cyc_Absyn_Datatypedecl*_tmp219;struct Cyc_Absyn_Datatypefield*_tmp218;struct Cyc_Absyn_Aggrdecl*_tmp217;switch(*((int*)_tmp20B)){case 0: _LL1E8: _LL1E9:
 goto _LL1E7;case 1: _LL1EA: _tmp217=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp20B)->f1;_LL1EB: {
# 858
struct Cyc_List_List*_tmp20C=0;
for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
({struct Cyc_List_List*_tmp380=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));({struct _tuple13*_tmp37F=({struct _tuple13*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->f1=0;_tmp20E->f2=(struct Cyc_Absyn_Pat*)_tmp22E->hd;_tmp20E;});_tmp20D->hd=_tmp37F;});_tmp20D->tl=_tmp20C;_tmp20D;});_tmp20C=_tmp380;});}
({void*_tmp385=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp20F=_cycalloc(sizeof(*_tmp20F));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp384=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp210;_tmp210.tag=7;({struct Cyc_Absyn_AggrInfo*_tmp382=({struct Cyc_Absyn_AggrInfo*_tmp211=_cycalloc(sizeof(*_tmp211));({union Cyc_Absyn_AggrInfoU _tmp381=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212[0]=_tmp217;_tmp212;}));_tmp211->aggr_info=_tmp381;});_tmp211->targs=0;_tmp211;});_tmp210.f1=_tmp382;});_tmp210.f2=0;({struct Cyc_List_List*_tmp383=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp20C);_tmp210.f3=_tmp383;});_tmp210.f4=_tmp22D;_tmp210;});_tmp20F[0]=_tmp384;});_tmp20F;});
# 861
p->r=_tmp385;});
# 863
_npop_handler(0);return;}case 2: _LL1EC: _tmp219=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp20B)->f1;_tmp218=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp20B)->f2;_LL1ED:
# 865
({void*_tmp387=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp213=_cycalloc(sizeof(*_tmp213));({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp386=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp214;_tmp214.tag=8;_tmp214.f1=_tmp219;_tmp214.f2=_tmp218;_tmp214.f3=_tmp22E;_tmp214.f4=_tmp22D;_tmp214;});_tmp213[0]=_tmp386;});_tmp213;});p->r=_tmp387;});_npop_handler(0);return;case 3: _LL1EE: _LL1EF:
 goto _LL1F1;default: _LL1F0: _LL1F1:
# 868
({void*_tmp215=0;({unsigned int _tmp389=_tmp241;struct _dyneither_ptr _tmp388=({const char*_tmp216="enum tag used with arguments in pattern";_tag_dyneither(_tmp216,sizeof(char),40);});Cyc_Warn_err(_tmp389,_tmp388,_tag_dyneither(_tmp215,sizeof(void*),0));});});_npop_handler(0);return;}_LL1E7:;}
# 855
;_pop_handler();}else{void*_tmp208=(void*)_exn_thrown;void*_tmp21A=_tmp208;void*_tmp21B;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp21A)->tag == Cyc_Binding_BindingError){_LL1F3: _LL1F4:
# 869
 goto _LL1F2;}else{_LL1F5: _tmp21B=_tmp21A;_LL1F6:(int)_rethrow(_tmp21B);}_LL1F2:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp21E;_tmp21E.tag=0;({struct _dyneither_ptr _tmp38A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp22F));_tmp21E.f1=_tmp38A;});({void*_tmp21C[1]={& _tmp21E};({unsigned int _tmp38C=_tmp241;struct _dyneither_ptr _tmp38B=({const char*_tmp21D="%s is not a constructor in pattern";_tag_dyneither(_tmp21D,sizeof(char),35);});Cyc_Warn_err(_tmp38C,_tmp38B,_tag_dyneither(_tmp21C,sizeof(void*),1));});});});
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->aggr_info).UnknownAggr).tag == 1){_LL1B3: _tmp234=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->aggr_info).UnknownAggr).val).f2;_tmp233=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->targs;_tmp232=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_tmp231=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_tmp230=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f4;_LL1B4:
# 874
{struct Cyc_List_List*_tmp21F=_tmp231;for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp21F->hd)).f2);}}
{struct _handler_cons _tmp220;_push_handler(& _tmp220);{int _tmp222=0;if(setjmp(_tmp220.handler))_tmp222=1;if(!_tmp222){
{struct Cyc_Absyn_Aggrdecl*_tmp223=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp241,env->ns,_tmp234,Cyc_Binding_lookup_aggrdecl);
({void*_tmp390=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp224=_cycalloc(sizeof(*_tmp224));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp38F=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp225;_tmp225.tag=7;({struct Cyc_Absyn_AggrInfo*_tmp38E=({struct Cyc_Absyn_AggrInfo*_tmp226=_cycalloc(sizeof(*_tmp226));({union Cyc_Absyn_AggrInfoU _tmp38D=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227[0]=_tmp223;_tmp227;}));_tmp226->aggr_info=_tmp38D;});_tmp226->targs=_tmp233;_tmp226;});_tmp225.f1=_tmp38E;});_tmp225.f2=_tmp232;_tmp225.f3=_tmp231;_tmp225.f4=_tmp230;_tmp225;});_tmp224[0]=_tmp38F;});_tmp224;});p->r=_tmp390;});}
# 877
;_pop_handler();}else{void*_tmp221=(void*)_exn_thrown;void*_tmp228=_tmp221;void*_tmp22B;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp228)->tag == Cyc_Binding_BindingError){_LL1F8: _LL1F9:
# 881
({void*_tmp229=0;({unsigned int _tmp392=_tmp241;struct _dyneither_ptr _tmp391=({const char*_tmp22A="non-aggregate name has designator patterns";_tag_dyneither(_tmp22A,sizeof(char),43);});Cyc_Warn_err(_tmp392,_tmp391,_tag_dyneither(_tmp229,sizeof(void*),0));});});
return;}else{_LL1FA: _tmp22B=_tmp228;_LL1FB:(int)_rethrow(_tmp22B);}_LL1F7:;}};}
# 884
return;}else{_LL1CF: _tmp235=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL1D0:
# 913
 for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp235->hd)).f2);}
return;}}else{_LL1CD: _tmp236=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL1CE:
# 911
 _tmp235=_tmp236;goto _LL1D0;}case 0: _LL1B5: _LL1B6:
# 886
 return;case 3: _LL1B7: _tmp238=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp237=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL1B8:
# 888
 _tmp23A=_tmp238;_tmp239=_tmp237;goto _LL1BA;case 1: _LL1B9: _tmp23A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp239=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL1BA:
# 890
 Cyc_Binding_resolve_pat(env,_tmp239);
_tmp23B=_tmp23A;goto _LL1BC;case 4: _LL1BB: _tmp23B=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL1BC:
 _tmp23C=_tmp23B;goto _LL1BE;case 2: _LL1BD: _tmp23C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL1BE:
# 894
 Cyc_Binding_resolve_and_add_vardecl(_tmp241,env,_tmp23C);goto _LL1AE;case 6: _LL1BF: _tmp23D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL1C0:
# 896
 Cyc_Binding_resolve_pat(env,_tmp23D);return;case 5: _LL1C1: _tmp23E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL1C2:
# 898
 for(0;_tmp23E != 0;_tmp23E=_tmp23E->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp23E->hd);}
goto _LL1AE;case 17: _LL1C3: _tmp23F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL1C4:
# 902
 Cyc_Binding_resolve_exp(env,_tmp23F);return;case 9: _LL1C5: _LL1C6:
# 904
 goto _LL1C8;case 10: _LL1C7: _LL1C8:
 goto _LL1CA;case 11: _LL1C9: _LL1CA:
 goto _LL1CC;case 12: _LL1CB: _LL1CC:
 return;case 8: _LL1D1: _tmp240=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL1D2:
# 917
 for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp240->hd);}
return;case 13: _LL1D3: _LL1D4:
 goto _LL1D6;default: _LL1D5: _LL1D6:
 return;}_LL1AE:;};}struct _tuple14{void*f1;void*f2;};
# 925
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}struct _tuple15{unsigned int f1;struct _tuple0*f2;int f3;};
# 939 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 942
unsigned int loc=d->loc;
void*_tmp243=d->r;void*_tmp244=_tmp243;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct _tuple0*_tmp2C3;struct Cyc_List_List*_tmp2C2;struct _dyneither_ptr*_tmp2C1;struct Cyc_List_List*_tmp2C0;struct Cyc_Absyn_Datatypedecl*_tmp2BF;struct Cyc_Absyn_Enumdecl*_tmp2BE;struct Cyc_Absyn_Aggrdecl*_tmp2BD;struct Cyc_Absyn_Typedefdecl*_tmp2BC;struct Cyc_Absyn_Tvar*_tmp2BB;struct Cyc_Absyn_Vardecl*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_Pat*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Fndecl*_tmp2B5;struct Cyc_Absyn_Vardecl*_tmp2B4;switch(*((int*)_tmp244)){case 0: _LL1FD: _tmp2B4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL1FE:
# 946
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2B4);
# 951
if(_tmp2B4->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp2B4->initializer));
goto _LL1FC;case 1: _LL1FF: _tmp2B5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL200:
# 956
 Cyc_Binding_absolutize_decl(loc,env,_tmp2B5->name,_tmp2B5->sc);{
struct _tuple0*_tmp245=_tmp2B5->name;struct _tuple0*_tmp246=_tmp245;union Cyc_Absyn_Nmspace _tmp25F;struct _dyneither_ptr*_tmp25E;_LL21C: _tmp25F=_tmp246->f1;_tmp25E=_tmp246->f2;_LL21D:;{
# 959
struct Cyc_List_List*_tmp247=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fndecl,1,loc,env,_tmp2B5->args,_tmp2B5->cyc_varargs);
# 961
({struct Cyc_Core_Opt*_tmp393=({struct Cyc_Core_Opt*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->v=_tmp247;_tmp248;});_tmp2B5->param_vardecls=_tmp393;});
# 963
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,_tmp2B5->name);{
# 966
struct Cyc_Dict_Dict*_tmp249=env->local_vars;
if(_tmp249 != 0)
({struct Cyc_Dict_Dict*_tmp394=({struct Cyc_Dict_Dict*_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp24A;});env->local_vars=_tmp394;});else{
# 970
({struct Cyc_Dict_Dict*_tmp396=({struct Cyc_Dict_Dict*_tmp24B=_cycalloc(sizeof(*_tmp24B));({struct Cyc_Dict_Dict _tmp395=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp24B[0]=_tmp395;});_tmp24B;});env->local_vars=_tmp396;});}
{struct Cyc_List_List*_tmp24C=_tmp247;for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
({struct Cyc_Dict_Dict _tmp39C=({struct Cyc_Dict_Dict _tmp39B=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp39A=(*((struct Cyc_Absyn_Vardecl*)_tmp24C->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp39B,_tmp39A,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp399=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp24E;_tmp24E.tag=0;({void*_tmp398=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp397=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp250;_tmp250.tag=3;_tmp250.f1=(struct Cyc_Absyn_Vardecl*)_tmp24C->hd;_tmp250;});_tmp24F[0]=_tmp397;});_tmp24F;});_tmp24E.f1=_tmp398;});_tmp24E;});_tmp24D[0]=_tmp399;});_tmp24D;}));});*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp39C;});}}
# 975
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp2B5->effect,_tmp2B5->ret_type,_tmp2B5->args,_tmp2B5->cyc_varargs,_tmp2B5->rgn_po,_tmp2B5->requires_clause,_tmp2B5->ensures_clause);
# 979
if(_tmp249 != 0){
# 981
({struct Cyc_Dict_Dict _tmp3A2=({struct Cyc_Dict_Dict _tmp3A1=*_tmp249;struct _dyneither_ptr*_tmp3A0=_tmp25E;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3A1,_tmp3A0,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp251=_cycalloc(sizeof(*_tmp251));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp39F=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp252;_tmp252.tag=0;({void*_tmp39E=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp253=_cycalloc(sizeof(*_tmp253));({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp39D=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp254;_tmp254.tag=2;_tmp254.f1=_tmp2B5;_tmp254;});_tmp253[0]=_tmp39D;});_tmp253;});_tmp252.f1=_tmp39E;});_tmp252;});_tmp251[0]=_tmp39F;});_tmp251;}));});*_tmp249=_tmp3A2;});
# 983
({struct Cyc_Dict_Dict _tmp3A8=({struct Cyc_Dict_Dict _tmp3A7=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp3A6=_tmp25E;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3A7,_tmp3A6,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp255=_cycalloc(sizeof(*_tmp255));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp3A5=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp256;_tmp256.tag=0;({void*_tmp3A4=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp257=_cycalloc(sizeof(*_tmp257));({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp3A3=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp258;_tmp258.tag=2;_tmp258.f1=_tmp2B5;_tmp258;});_tmp257[0]=_tmp3A3;});_tmp257;});_tmp256.f1=_tmp3A4;});_tmp256;});_tmp255[0]=_tmp3A5;});_tmp255;}));});*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp3A8;});}else{
# 986
struct Cyc_Binding_ResolveNSEnv*_tmp259=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp25F);
({struct Cyc_Dict_Dict _tmp3AE=({struct Cyc_Dict_Dict _tmp3AD=_tmp259->ordinaries;struct _dyneither_ptr*_tmp3AC=_tmp25E;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3AD,_tmp3AC,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp3AB=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp25B;_tmp25B.tag=0;({void*_tmp3AA=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp25C=_cycalloc(sizeof(*_tmp25C));({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp3A9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp25D;_tmp25D.tag=2;_tmp25D.f1=_tmp2B5;_tmp25D;});_tmp25C[0]=_tmp3A9;});_tmp25C;});_tmp25B.f1=_tmp3AA;});_tmp25B;});_tmp25A[0]=_tmp3AB;});_tmp25A;}));});_tmp259->ordinaries=_tmp3AE;});}
# 992
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp2B5->body);
# 995
env->local_vars=_tmp249;
goto _LL1FC;};};};case 2: _LL201: _tmp2B7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_tmp2B6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp244)->f3;_LL202:
# 999
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp260=0;({unsigned int _tmp3B0=loc;struct _dyneither_ptr _tmp3AF=({const char*_tmp261="let not allowed at top-level";_tag_dyneither(_tmp261,sizeof(char),29);});Cyc_Warn_err(_tmp3B0,_tmp3AF,_tag_dyneither(_tmp260,sizeof(void*),0));});});
goto _LL1FC;}
# 1003
Cyc_Binding_resolve_exp(env,_tmp2B6);
Cyc_Binding_resolve_pat(env,_tmp2B7);
goto _LL1FC;case 3: _LL203: _tmp2B8=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL204:
# 1008
 for(0;_tmp2B8 != 0;_tmp2B8=_tmp2B8->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp2B8->hd);}
goto _LL1FC;case 4: _LL205: _tmp2BB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_tmp2BA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp244)->f2;_tmp2B9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp244)->f3;_LL206:
# 1013
 if(_tmp2B9 != 0)
Cyc_Binding_resolve_exp(env,_tmp2B9);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2BA);
goto _LL1FC;case 8: _LL207: _tmp2BC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL208:
# 1019
 Cyc_Binding_absolutize_decl(loc,env,_tmp2BC->name,_tmp2BC->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp262=_tmp2BC->name;struct _tuple0*_tmp263=_tmp262;union Cyc_Absyn_Nmspace _tmp266;struct _dyneither_ptr*_tmp265;_LL21F: _tmp266=_tmp263->f1;_tmp265=_tmp263->f2;_LL220:;{
# 1022
struct Cyc_Binding_ResolveNSEnv*_tmp264=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp266);
if(_tmp2BC->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp2BC->defn));
# 1026
({struct Cyc_Dict_Dict _tmp3B1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp264->typedefs,_tmp265,_tmp2BC);_tmp264->typedefs=_tmp3B1;});
# 1028
goto _LL1FC;};};case 5: _LL209: _tmp2BD=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL20A:
# 1031
 Cyc_Binding_absolutize_decl(loc,env,_tmp2BD->name,_tmp2BD->sc);{
struct _tuple0*_tmp267=_tmp2BD->name;struct _tuple0*_tmp268=_tmp267;union Cyc_Absyn_Nmspace _tmp271;struct _dyneither_ptr*_tmp270;_LL222: _tmp271=_tmp268->f1;_tmp270=_tmp268->f2;_LL223:;{
# 1034
struct Cyc_Binding_ResolveNSEnv*_tmp269=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp271);
# 1036
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp269->aggrdecls,_tmp270) && _tmp2BD->impl == 0)
goto _LL1FC;
# 1039
({struct Cyc_Dict_Dict _tmp3B2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp269->aggrdecls,_tmp270,_tmp2BD);_tmp269->aggrdecls=_tmp3B2;});
# 1041
({struct Cyc_Dict_Dict _tmp3B6=({struct Cyc_Dict_Dict _tmp3B5=_tmp269->ordinaries;struct _dyneither_ptr*_tmp3B4=_tmp270;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3B5,_tmp3B4,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));({struct Cyc_Binding_AggrRes_Binding_Resolved_struct _tmp3B3=({struct Cyc_Binding_AggrRes_Binding_Resolved_struct _tmp26B;_tmp26B.tag=1;_tmp26B.f1=_tmp2BD;_tmp26B;});_tmp26A[0]=_tmp3B3;});_tmp26A;}));});_tmp269->ordinaries=_tmp3B6;});
# 1043
if(_tmp2BD->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp26C=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp26D=_tmp26C;struct Cyc_List_List*_tmp26F;struct Cyc_List_List*_tmp26E;_LL225: _tmp26F=_tmp26D->rgn_po;_tmp26E=_tmp26D->fields;_LL226:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp26F);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp26E);}
# 1048
goto _LL1FC;};};case 7: _LL20B: _tmp2BE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL20C:
# 1051
 Cyc_Binding_absolutize_decl(loc,env,_tmp2BE->name,_tmp2BE->sc);{
struct _tuple0*_tmp272=_tmp2BE->name;struct _tuple0*_tmp273=_tmp272;union Cyc_Absyn_Nmspace _tmp279;struct _dyneither_ptr*_tmp278;_LL228: _tmp279=_tmp273->f1;_tmp278=_tmp273->f2;_LL229:;{
# 1054
struct Cyc_Binding_ResolveNSEnv*_tmp274=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp279);
# 1056
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp274->enumdecls,_tmp278) && _tmp2BE->fields == 0)
goto _LL1FC;
# 1060
({struct Cyc_Dict_Dict _tmp3B7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp274->enumdecls,_tmp278,_tmp2BE);_tmp274->enumdecls=_tmp3B7;});
# 1062
if(_tmp2BE->fields != 0){
# 1064
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BE->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp275=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp275->loc,env,_tmp275->name,_tmp2BE->sc);
if(_tmp275->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp275->tag));
({struct Cyc_Dict_Dict _tmp3BB=({struct Cyc_Dict_Dict _tmp3BA=_tmp274->ordinaries;struct _dyneither_ptr*_tmp3B9=(*_tmp275->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3BA,_tmp3B9,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp276=_cycalloc(sizeof(*_tmp276));({struct Cyc_Binding_EnumRes_Binding_Resolved_struct _tmp3B8=({struct Cyc_Binding_EnumRes_Binding_Resolved_struct _tmp277;_tmp277.tag=3;_tmp277.f1=_tmp2BE;_tmp277.f2=_tmp275;_tmp277;});_tmp276[0]=_tmp3B8;});_tmp276;}));});_tmp274->ordinaries=_tmp3BB;});}}
# 1073
goto _LL1FC;};};case 6: _LL20D: _tmp2BF=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL20E:
# 1077
{struct _handler_cons _tmp27A;_push_handler(& _tmp27A);{int _tmp27C=0;if(setjmp(_tmp27A.handler))_tmp27C=1;if(!_tmp27C){
{struct Cyc_List_List*_tmp27D=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp2BF->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp27E=(struct Cyc_Absyn_Datatypedecl*)_tmp27D->hd;
if(!_tmp27E->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp27F=_cycalloc_atomic(sizeof(*_tmp27F));({struct Cyc_Binding_BindingError_exn_struct _tmp3BC=({struct Cyc_Binding_BindingError_exn_struct _tmp280;_tmp280.tag=Cyc_Binding_BindingError;_tmp280;});_tmp27F[0]=_tmp3BC;});_tmp27F;}));
_tmp2BF->name=_tmp27E->name;
_tmp2BF->is_extensible=1;{
struct _tuple0*_tmp281=_tmp2BF->name;struct _tuple0*_tmp282=_tmp281;union Cyc_Absyn_Nmspace _tmp286;struct _dyneither_ptr*_tmp285;_LL22B: _tmp286=_tmp282->f1;_tmp285=_tmp282->f2;_LL22C:;{
struct Cyc_Binding_ResolveNSEnv*_tmp283=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp286);
({struct Cyc_Dict_Dict _tmp3BF=({struct Cyc_Dict_Dict _tmp3BE=_tmp283->datatypedecls;struct _dyneither_ptr*_tmp3BD=_tmp285;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3BE,_tmp3BD,({struct Cyc_List_List*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->hd=_tmp2BF;_tmp284->tl=_tmp27D;_tmp284;}));});_tmp283->datatypedecls=_tmp3BF;});};};}
# 1078
;_pop_handler();}else{void*_tmp27B=(void*)_exn_thrown;void*_tmp287=_tmp27B;void*_tmp28E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp287)->tag == Cyc_Binding_BindingError){_LL22E: _LL22F:
# 1090
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp2BF->name,_tmp2BF->sc);{
struct _tuple0*_tmp288=_tmp2BF->name;struct _tuple0*_tmp289=_tmp288;union Cyc_Absyn_Nmspace _tmp28D;struct _dyneither_ptr*_tmp28C;_LL233: _tmp28D=_tmp289->f1;_tmp28C=_tmp289->f2;_LL234:;{
struct Cyc_Binding_ResolveNSEnv*_tmp28A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp28D);
# 1094
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp28A->datatypedecls,_tmp28C) && _tmp2BF->fields == 0)
# 1096
goto _LL22D;
({struct Cyc_Dict_Dict _tmp3C2=({struct Cyc_Dict_Dict _tmp3C1=_tmp28A->datatypedecls;struct _dyneither_ptr*_tmp3C0=_tmp28C;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3C1,_tmp3C0,({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->hd=_tmp2BF;_tmp28B->tl=0;_tmp28B;}));});_tmp28A->datatypedecls=_tmp3C2;});
# 1099
goto _LL22D;};};}else{_LL230: _tmp28E=_tmp287;_LL231:(int)_rethrow(_tmp28E);}_LL22D:;}};}{
# 1102
struct _tuple0*_tmp28F=_tmp2BF->name;struct _tuple0*_tmp290=_tmp28F;union Cyc_Absyn_Nmspace _tmp29E;struct _dyneither_ptr*_tmp29D;_LL236: _tmp29E=_tmp290->f1;_tmp29D=_tmp290->f2;_LL237:;
if(_tmp2BF->fields != 0){
struct Cyc_List_List*_tmp291=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BF->fields))->v;for(0;_tmp291 != 0;_tmp291=_tmp291->tl){
struct Cyc_Absyn_Datatypefield*_tmp292=(struct Cyc_Absyn_Datatypefield*)_tmp291->hd;
{struct Cyc_List_List*_tmp293=_tmp292->typs;for(0;_tmp293 != 0;_tmp293=_tmp293->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp293->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp294=(*_tmp292->name).f1;union Cyc_Absyn_Nmspace _tmp295=_tmp294;switch((_tmp295.Abs_n).tag){case 1: if((_tmp295.Rel_n).val == 0){_LL239: _LL23A:
# 1110
 if(_tmp2BF->is_extensible)
({union Cyc_Absyn_Nmspace _tmp3C3=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*_tmp292->name).f1=_tmp3C3;});else{
# 1113
(*_tmp292->name).f1=(*_tmp2BF->name).f1;}
goto _LL238;}else{_LL23B: _LL23C:
# 1116
({void*_tmp296=0;({unsigned int _tmp3C5=_tmp292->loc;struct _dyneither_ptr _tmp3C4=({const char*_tmp297="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp297,sizeof(char),56);});Cyc_Warn_err(_tmp3C5,_tmp3C4,_tag_dyneither(_tmp296,sizeof(void*),0));});});
return;}case 2: _LL23D: _LL23E:
 goto _LL238;default: _LL23F: _LL240:
({void*_tmp298=0;({struct _dyneither_ptr _tmp3C6=({const char*_tmp299="datatype field Loc_n or C_n";_tag_dyneither(_tmp299,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp3C6,_tag_dyneither(_tmp298,sizeof(void*),0));});});}_LL238:;}{
# 1121
struct Cyc_Binding_ResolveNSEnv*_tmp29A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp292->name).f1);
({struct Cyc_Dict_Dict _tmp3CA=({struct Cyc_Dict_Dict _tmp3C9=_tmp29A->ordinaries;struct _dyneither_ptr*_tmp3C8=(*_tmp292->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3C9,_tmp3C8,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp29B=_cycalloc(sizeof(*_tmp29B));({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct _tmp3C7=({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct _tmp29C;_tmp29C.tag=2;_tmp29C.f1=_tmp2BF;_tmp29C.f2=_tmp292;_tmp29C;});_tmp29B[0]=_tmp3C7;});_tmp29B;}));});_tmp29A->ordinaries=_tmp3CA;});};}}
# 1126
goto _LL1FC;};case 9: _LL20F: _tmp2C1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_tmp2C0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp244)->f2;_LL210:
# 1129
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp2C1,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp2C0);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL1FC;case 10: _LL211: _tmp2C3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_tmp2C2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp244)->f2;_LL212:
# 1134
((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp2C3);
Cyc_Binding_resolve_decls(env,_tmp2C2);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL1FC;case 11: _LL213: _tmp2C4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_LL214: {
# 1140
int _tmp29F=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2C4);
env->in_cinclude=_tmp29F;
goto _LL1FC;}case 12: _LL215: _tmp2C6=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp244)->f1;_tmp2C5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp244)->f2;_LL216:
# 1151
{struct Cyc_List_List*_tmp2A0=_tmp2C5;for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
struct _tuple15*_tmp2A1=(struct _tuple15*)_tmp2A0->hd;struct _tuple15*_tmp2A2=_tmp2A1;unsigned int _tmp2A4;struct _tuple0*_tmp2A3;_LL242: _tmp2A4=_tmp2A2->f1;_tmp2A3=_tmp2A2->f2;_LL243:;
Cyc_Binding_absolutize_decl(_tmp2A4,env,_tmp2A3,Cyc_Absyn_ExternC);}}
# 1159
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp2A5=0;({unsigned int _tmp3CC=loc;struct _dyneither_ptr _tmp3CB=({const char*_tmp2A6="extern \"C include\" not at toplevel";_tag_dyneither(_tmp2A6,sizeof(char),35);});Cyc_Warn_err(_tmp3CC,_tmp3CB,_tag_dyneither(_tmp2A5,sizeof(void*),0));});});
goto _LL1FC;}{
# 1164
struct Cyc_Binding_ResolveNSEnv*_tmp2A7=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp2A8=_tmp2A7->ordinaries;
int _tmp2A9=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2C6);{
struct Cyc_Dict_Dict _tmp2AA=_tmp2A7->ordinaries;
struct Cyc_Dict_Dict _tmp2AB=_tmp2A8;
{struct Cyc_List_List*_tmp2AC=_tmp2C5;for(0;_tmp2AC != 0;_tmp2AC=_tmp2AC->tl){
struct _tuple15*_tmp2AD=(struct _tuple15*)_tmp2AC->hd;struct _tuple15*_tmp2AE=_tmp2AD;unsigned int _tmp2B3;struct _dyneither_ptr*_tmp2B2;_LL245: _tmp2B3=_tmp2AE->f1;_tmp2B2=(_tmp2AE->f2)->f2;_LL246:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp2AA,_tmp2B2) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp2A8,_tmp2B2) && ({
void*_tmp3CD=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2A8,_tmp2B2);_tmp3CD == ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2AA,_tmp2B2);}))
({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B2);({void*_tmp2AF[1]={& _tmp2B1};({unsigned int _tmp3CF=_tmp2B3;struct _dyneither_ptr _tmp3CE=({const char*_tmp2B0="%s is exported but not defined";_tag_dyneither(_tmp2B0,sizeof(char),31);});Cyc_Warn_err(_tmp3CF,_tmp3CE,_tag_dyneither(_tmp2AF,sizeof(void*),1));});});});
({struct Cyc_Dict_Dict _tmp3D2=({struct Cyc_Dict_Dict _tmp3D1=_tmp2AB;struct _dyneither_ptr*_tmp3D0=_tmp2B2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3D1,_tmp3D0,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2AA,_tmp2B2));});_tmp2AB=_tmp3D2;});}}
# 1179
_tmp2A7->ordinaries=_tmp2AB;
env->in_cinclude=_tmp2A9;
goto _LL1FC;};};case 13: _LL217: _LL218:
# 1183
 goto _LL21A;default: _LL219: _LL21A:
 goto _LL1FC;}_LL1FC:;}
# 1188
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1193
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2C7=({struct Cyc_Binding_Env*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->in_cinclude=0;({struct Cyc_Binding_NSCtxt*_tmp3D3=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2CB->ns=_tmp3D3;});_tmp2CB->local_vars=0;_tmp2CB;});
({struct Cyc_Binding_Env*_tmp3D6=_tmp2C7;Cyc_Binding_resolve_decl(_tmp3D6,({struct Cyc_Absyn_Decl*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));({void*_tmp3D5=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp3D4=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2CA;_tmp2CA.tag=6;_tmp2CA.f1=Cyc_Absyn_exn_tud;_tmp2CA;});_tmp2C9[0]=_tmp3D4;});_tmp2C9;});_tmp2C8->r=_tmp3D5;});_tmp2C8->loc=0;_tmp2C8;}));});
Cyc_Binding_resolve_decls(_tmp2C7,tds);}

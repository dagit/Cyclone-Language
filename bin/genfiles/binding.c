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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172 "list.h"
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 44 "binding.cyc"
int Cyc_Binding_warn_override=0;struct Cyc_Binding_NSCtxt;
# 58
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));((((*_tmp3).curr_ns=0,({
struct Cyc_List_List*_tmp2A3=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));(({void*_tmp2A2=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));((*_tmp1).tag=0U,(*_tmp1).f1=0);_tmp1;});(*_tmp2).hd=_tmp2A2;}),(*_tmp2).tl=0);_tmp2;});(*_tmp3).availables=_tmp2A3;}))),({
struct Cyc_Dict_Dict _tmp2A1=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);(*_tmp3).ns_data=_tmp2A1;}));_tmp3;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch((_tmp4.Abs_n).tag){case 3U: _LL1: _tmp7=(_tmp4.C_n).val;_LL2:
 _tmp8=_tmp7;goto _LL4;case 2U: _LL3: _tmp8=(_tmp4.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp8);default: _LL5: _LL6:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp2A4=({const char*_tmp6="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp6,sizeof(char),33U);});Cyc_Warn_impos(_tmp2A4,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}_LL0:;}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp9))
return _tmp9;
{struct Cyc_List_List*_tmpA=ctxt->availables;for(0;_tmpA != 0;_tmpA=_tmpA->tl){
void*_tmpB=(void*)_tmpA->hd;void*_tmpC=_tmpB;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmpE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->tag == 1U){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2:
 _tmpF=_tmpE;goto _LL4;}else{_LL3: _tmpF=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {
# 92
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpF,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpD))
return _tmpD;
goto _LL0;}}_LL0:;}}
# 97
({struct Cyc_String_pa_PrintArg_struct _tmp12;_tmp12.tag=0U;({struct _dyneither_ptr _tmp2A6=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_List_List*_tmp2A5=rel_ns;Cyc_str_sepstr(_tmp2A5,({const char*_tmp13="::";_tag_dyneither(_tmp13,sizeof(char),3U);}));}));_tmp12.f1=_tmp2A6;});({void*_tmp10[1U];_tmp10[0]=& _tmp12;({unsigned int _tmp2A8=loc;struct _dyneither_ptr _tmp2A7=({const char*_tmp11="namespace %s not found";_tag_dyneither(_tmp11,sizeof(char),23U);});Cyc_Warn_err(_tmp2A8,_tmp2A7,_tag_dyneither(_tmp10,sizeof(void*),1U));});});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));(*_tmp14).tag=Cyc_Binding_BindingError;_tmp14;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp15=qv;union Cyc_Absyn_Nmspace _tmp3E;struct _dyneither_ptr*_tmp3D;_LL1: _tmp3E=_tmp15->f1;_tmp3D=_tmp15->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp16=_tmp3E;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3B;struct Cyc_List_List*_tmp3A;switch((_tmp16.Abs_n).tag){case 1U: if((_tmp16.Rel_n).val == 0){_LL4: _LL5:
# 106
{struct _handler_cons _tmp17;_push_handler(& _tmp17);{int _tmp19=0;if(setjmp(_tmp17.handler))_tmp19=1;if(!_tmp19){{void*_tmp1A=({void*(*_tmp2AA)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2A9=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp2AA(_tmp2A9,_tmp3D);});_npop_handler(0U);return _tmp1A;};_pop_handler();}else{void*_tmp18=(void*)_exn_thrown;void*_tmp1B=_tmp18;void*_tmp1C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp1C=_tmp1B;_LL12:(int)_rethrow(_tmp1C);}_LLE:;}};}
{struct Cyc_List_List*_tmp1D=ctxt->availables;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
void*_tmp1E=(void*)_tmp1D->hd;void*_tmp1F=_tmp1E;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp26;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1F)->tag == 1U){_LL14: _tmp26=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1F)->f1;_LL15:
 _tmp27=_tmp26;goto _LL17;}else{_LL16: _tmp27=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp1F)->f1;_LL17:
# 112
{struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler))_tmp22=1;if(!_tmp22){{void*_tmp23=({void*(*_tmp2AC)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2AB=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp27);_tmp2AC(_tmp2AB,_tmp3D);});_npop_handler(0U);return _tmp23;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp24=_tmp21;void*_tmp25;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp25=_tmp24;_LL1C:(int)_rethrow(_tmp25);}_LL18:;}};}
goto _LL13;}_LL13:;}}
# 116
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp28=_cycalloc(sizeof(*_tmp28));(*_tmp28).tag=Cyc_Binding_BindingError;_tmp28;}));}else{_LL6: _tmp3A=(_tmp16.Rel_n).val;_LL7: {
# 118
struct _handler_cons _tmp29;_push_handler(& _tmp29);{int _tmp2B=0;if(setjmp(_tmp29.handler))_tmp2B=1;if(!_tmp2B){
{struct Cyc_List_List*_tmp2C=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3A);
void*_tmp2D=({void*(*_tmp2AE)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2AD=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2C);_tmp2AE(_tmp2AD,_tmp3D);});_npop_handler(0U);return _tmp2D;}
# 119
;_pop_handler();}else{void*_tmp2A=(void*)_exn_thrown;void*_tmp2E=_tmp2A;void*_tmp30;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 121
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));(*_tmp2F).tag=Cyc_Binding_BindingError;_tmp2F;}));}else{_LL20: _tmp30=_tmp2E;_LL21:(int)_rethrow(_tmp30);}_LL1D:;}};}}case 3U: _LL8: _tmp3B=(_tmp16.C_n).val;_LL9:
 _tmp3C=_tmp3B;goto _LLB;case 2U: _LLA: _tmp3C=(_tmp16.Abs_n).val;_LLB: {
# 124
struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){{void*_tmp34=({void*(*_tmp2B0)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2AF=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp3C);_tmp2B0(_tmp2AF,_tmp3D);});_npop_handler(0U);return _tmp34;};_pop_handler();}else{void*_tmp32=(void*)_exn_thrown;void*_tmp35=_tmp32;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp36=_cycalloc(sizeof(*_tmp36));(*_tmp36).tag=Cyc_Binding_BindingError;_tmp36;}));}else{_LL25: _tmp37=_tmp35;_LL26:(int)_rethrow(_tmp37);}_LL22:;}};}default: _LLC: _LLD:
({void*_tmp38=0U;({struct _dyneither_ptr _tmp2B1=({const char*_tmp39="lookup local in global";_tag_dyneither(_tmp39,sizeof(char),23U);});Cyc_Warn_impos(_tmp2B1,_tag_dyneither(_tmp38,sizeof(void*),0U));});});}_LL3:;};}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp3F=ctxt->curr_ns;
struct Cyc_List_List*_tmp40=({struct Cyc_List_List*_tmp2B2=_tmp3F;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2B2,({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));((*_tmp43).hd=subname,(*_tmp43).tl=0);_tmp43;}));});
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp40))
({struct Cyc_Dict_Dict _tmp2B5=({struct Cyc_Dict_Dict _tmp2B4=ctxt->ns_data;struct Cyc_List_List*_tmp2B3=_tmp40;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp2B4,_tmp2B3,mkdata(env));});ctxt->ns_data=_tmp2B5;});
ctxt->curr_ns=_tmp40;
({struct Cyc_List_List*_tmp2B7=({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));(({void*_tmp2B6=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp41=_cycalloc(sizeof(*_tmp41));((*_tmp41).tag=0U,(*_tmp41).f1=_tmp40);_tmp41;});(*_tmp42).hd=_tmp2B6;}),(*_tmp42).tl=ctxt->availables);_tmp42;});ctxt->availables=_tmp2B7;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp44=0U;({struct _dyneither_ptr _tmp2B8=({const char*_tmp45="leaving topmost namespace";_tag_dyneither(_tmp45,sizeof(char),26U);});Cyc_Warn_impos(_tmp2B8,_tag_dyneither(_tmp44,sizeof(void*),0U));});});{
void*_tmp46=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp47=_tmp46;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp47)->tag == 0U){_LL1: _LL2:
# 143
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp2B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp2B9;});
goto _LL0;}else{_LL3: _LL4:
# 147
({void*_tmp48=0U;({struct _dyneither_ptr _tmp2BA=({const char*_tmp49="leaving using as namespace";_tag_dyneither(_tmp49,sizeof(char),27U);});Cyc_Warn_impos(_tmp2BA,_tag_dyneither(_tmp48,sizeof(void*),0U));});});}_LL0:;};}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4A=usename;union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp57;_LL1: _tmp58=_tmp4A->f1;_tmp57=_tmp4A->f2;_LL2:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp4B=_tmp58;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp53;switch((_tmp4B.Loc_n).tag){case 1U: _LL4: _tmp53=(_tmp4B.Rel_n).val;_LL5:
# 155
({struct Cyc_List_List*_tmp2BE=({unsigned int _tmp2BD=loc;struct Cyc_Binding_NSCtxt*_tmp2BC=ctxt;Cyc_Binding_resolve_rel_ns(_tmp2BD,_tmp2BC,({struct Cyc_List_List*_tmp2BB=_tmp53;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2BB,({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));((*_tmp4C).hd=_tmp57,(*_tmp4C).tl=0);_tmp4C;}));}));});ns=_tmp2BE;});{
struct Cyc_List_List*_tmp4D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
({union Cyc_Absyn_Nmspace _tmp2BF=Cyc_Absyn_Abs_n(_tmp4D,0);(*usename).f1=_tmp2BF;});
goto _LL3;};case 2U: _LL6: _tmp54=(_tmp4B.Abs_n).val;_LL7:
# 160
({struct Cyc_List_List*_tmp2C1=({struct Cyc_List_List*_tmp2C0=_tmp54;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2C0,({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));((*_tmp4E).hd=_tmp57,(*_tmp4E).tl=0);_tmp4E;}));});ns=_tmp2C1;});
goto _LL3;case 4U: _LL8: _LL9:
({void*_tmp4F=0U;({struct _dyneither_ptr _tmp2C2=({const char*_tmp50="enter_using, Loc_n";_tag_dyneither(_tmp50,sizeof(char),19U);});Cyc_Warn_impos(_tmp2C2,_tag_dyneither(_tmp4F,sizeof(void*),0U));});});default: _LLA: _LLB:
({void*_tmp51=0U;({struct _dyneither_ptr _tmp2C3=({const char*_tmp52="enter_using, C_n";_tag_dyneither(_tmp52,sizeof(char),17U);});Cyc_Warn_impos(_tmp2C3,_tag_dyneither(_tmp51,sizeof(void*),0U));});});}_LL3:;}
# 165
({struct Cyc_List_List*_tmp2C5=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));(({void*_tmp2C4=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp55=_cycalloc(sizeof(*_tmp55));((*_tmp55).tag=1U,(*_tmp55).f1=ns);_tmp55;});(*_tmp56).hd=_tmp2C4;}),(*_tmp56).tl=ctxt->availables);_tmp56;});ctxt->availables=_tmp2C5;});
return ns;};}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp59=0U;({struct _dyneither_ptr _tmp2C6=({const char*_tmp5A="leaving topmost namespace as a using";_tag_dyneither(_tmp5A,sizeof(char),37U);});Cyc_Warn_impos(_tmp2C6,_tag_dyneither(_tmp59,sizeof(void*),0U));});});{
void*_tmp5B=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp5C=_tmp5B;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp5C)->tag == 1U){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp5D=0U;({struct _dyneither_ptr _tmp2C7=({const char*_tmp5E="leaving namespace as using";_tag_dyneither(_tmp5E,sizeof(char),27U);});Cyc_Warn_impos(_tmp2C7,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});}_LL0:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 212 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp5F=_cycalloc(sizeof(*_tmp5F));(((((((({struct Cyc_Dict_Dict _tmp2CC=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);(*_tmp5F).aggrdecls=_tmp2CC;}),({
struct Cyc_Dict_Dict _tmp2CB=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);(*_tmp5F).datatypedecls=_tmp2CB;}))),({
struct Cyc_Dict_Dict _tmp2CA=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);(*_tmp5F).enumdecls=_tmp2CA;}))),({
struct Cyc_Dict_Dict _tmp2C9=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);(*_tmp5F).typedefs=_tmp2C9;}))),({
struct Cyc_Dict_Dict _tmp2C8=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);(*_tmp5F).ordinaries=_tmp2C8;}));_tmp5F;});}
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
struct _tuple0*_tmp60=qv;union Cyc_Absyn_Nmspace _tmp64;struct _dyneither_ptr*_tmp63;_LL1: _tmp64=_tmp60->f1;_tmp63=_tmp60->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp61=_tmp64;switch((_tmp61.Rel_n).tag){case 4U: _LL4: _LL5:
# 245
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));(*_tmp62).tag=Cyc_Binding_BindingError;_tmp62;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63);case 1U: if((_tmp61.Rel_n).val == 0){_LL6: _LL7:
# 249
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;};}
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
union Cyc_Absyn_Nmspace _tmp65=(*qv).f1;union Cyc_Absyn_Nmspace _tmp66=_tmp65;switch((_tmp66.Abs_n).tag){case 1U: if((_tmp66.Rel_n).val == 0){_LL1: _LL2:
# 277
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp2CE=({struct Cyc_List_List*_tmp2CD=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp2CD,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);});(*qv).f1=_tmp2CE;});else{
# 280
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _LL4:
 goto _LL0;case 2U: _LL5: _LL6:
# 284
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
# 288
({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0U;({struct _dyneither_ptr _tmp2CF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(qv));_tmp69.f1=_tmp2CF;});({void*_tmp67[1U];_tmp67[0]=& _tmp69;({unsigned int _tmp2D1=loc;struct _dyneither_ptr _tmp2D0=({const char*_tmp68="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp68,sizeof(char),51U);});Cyc_Warn_err(_tmp2D1,_tmp2D0,_tag_dyneither(_tmp67,sizeof(void*),1U));});});});}_LL0:;}
# 295
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp6A=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp6A;}
# 302
void Cyc_Binding_check_warn_override(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _dyneither_ptr hides_what;
struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){
{void*_tmp6E=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp6F=_tmp6E;void*_tmp7B;switch(*((int*)_tmp6F)){case 0U: _LL1: _tmp7B=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp6F)->f1;_LL2:
# 307
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0U);return;}
{void*_tmp70=_tmp7B;switch(*((int*)_tmp70)){case 1U: _LLC: _LLD:
({struct _dyneither_ptr _tmp2D2=({const char*_tmp71="global variable";_tag_dyneither(_tmp71,sizeof(char),16U);});hides_what=_tmp2D2;});goto _LLB;case 4U: _LLE: _LLF:
({struct _dyneither_ptr _tmp2D3=({const char*_tmp72="local variable";_tag_dyneither(_tmp72,sizeof(char),15U);});hides_what=_tmp2D3;});goto _LLB;case 3U: _LL10: _LL11:
({struct _dyneither_ptr _tmp2D4=({const char*_tmp73="parameter";_tag_dyneither(_tmp73,sizeof(char),10U);});hides_what=_tmp2D4;});goto _LLB;case 5U: _LL12: _LL13:
({struct _dyneither_ptr _tmp2D5=({const char*_tmp74="pattern variable";_tag_dyneither(_tmp74,sizeof(char),17U);});hides_what=_tmp2D5;});goto _LLB;case 2U: _LL14: _LL15:
({struct _dyneither_ptr _tmp2D6=({const char*_tmp75="function";_tag_dyneither(_tmp75,sizeof(char),9U);});hides_what=_tmp2D6;});goto _LLB;default: _LL16: _LL17:
({void*_tmp76=0U;({struct _dyneither_ptr _tmp2D7=({const char*_tmp77="shadowing free variable!";_tag_dyneither(_tmp77,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2D7,_tag_dyneither(_tmp76,sizeof(void*),0U));});});}_LLB:;}
# 317
goto _LL0;case 1U: _LL3: _LL4:
({struct _dyneither_ptr _tmp2D8=({const char*_tmp78="struct constructor";_tag_dyneither(_tmp78,sizeof(char),19U);});hides_what=_tmp2D8;});goto _LL0;case 2U: _LL5: _LL6:
({struct _dyneither_ptr _tmp2D9=({const char*_tmp79="datatype constructor";_tag_dyneither(_tmp79,sizeof(char),21U);});hides_what=_tmp2D9;});goto _LL0;case 3U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
({struct _dyneither_ptr _tmp2DA=({const char*_tmp7A="enum tag";_tag_dyneither(_tmp7A,sizeof(char),9U);});hides_what=_tmp2DA;});goto _LL0;}_LL0:;}
# 323
({struct Cyc_String_pa_PrintArg_struct _tmp7E;_tmp7E.tag=0U;_tmp7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)hides_what);({void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp2DC=loc;struct _dyneither_ptr _tmp2DB=({const char*_tmp7D="declaration hides %s";_tag_dyneither(_tmp7D,sizeof(char),21U);});Cyc_Warn_warn(_tmp2DC,_tmp2DB,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});});
_npop_handler(0U);return;
# 305
;_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp7F=_tmp6C;void*_tmp80;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp7F)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 325
 return;}else{_LL1B: _tmp80=_tmp7F;_LL1C:(int)_rethrow(_tmp80);}_LL18:;}};}
# 328
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp81=vd->name;struct _tuple0*_tmp82=_tmp81;union Cyc_Absyn_Nmspace _tmp89;struct _dyneither_ptr*_tmp88;_LL1: _tmp89=_tmp82->f1;_tmp88=_tmp82->f2;_LL2:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp2E0=({struct Cyc_Dict_Dict _tmp2DF=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp2DE=_tmp88;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2DF,_tmp2DE,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp84=_cycalloc(sizeof(*_tmp84));
((*_tmp84).tag=0U,({void*_tmp2DD=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp83=_cycalloc(sizeof(*_tmp83));((*_tmp83).tag=4U,(*_tmp83).f1=vd);_tmp83;});(*_tmp84).f1=_tmp2DD;}));_tmp84;}));});
# 335
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp2E0;});else{
# 338
struct Cyc_Binding_ResolveNSEnv*_tmp85=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp89);
({struct Cyc_Dict_Dict _tmp2E4=({struct Cyc_Dict_Dict _tmp2E3=_tmp85->ordinaries;struct _dyneither_ptr*_tmp2E2=_tmp88;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2E3,_tmp2E2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp87=_cycalloc(sizeof(*_tmp87));
# 341
((*_tmp87).tag=0U,({void*_tmp2E1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp86=_cycalloc(sizeof(*_tmp86));((*_tmp86).tag=1U,(*_tmp86).f1=vd);_tmp86;});(*_tmp87).f1=_tmp2E1;}));_tmp87;}));});
# 339
_tmp85->ordinaries=_tmp2E4;});}};}
# 345
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp8A=s->r;void*_tmp8B=_tmp8A;struct Cyc_Absyn_Stmt*_tmpA3;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_Decl*_tmp9F;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Stmt*_tmp96;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Stmt*_tmp92;struct Cyc_Absyn_Stmt*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Stmt*_tmp8F;struct Cyc_Absyn_Stmt*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;switch(*((int*)_tmp8B)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp8D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL4:
 Cyc_Binding_resolve_exp(env,_tmp8D);return;case 2U: _LL5: _tmp8F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp8E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL6:
 Cyc_Binding_resolve_stmt(env,_tmp8F);Cyc_Binding_resolve_stmt(env,_tmp8E);return;case 3U: _LL7: _tmp90=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL8:
 if((unsigned int)_tmp90)Cyc_Binding_resolve_exp(env,_tmp90);return;case 4U: _LL9: _tmp93=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp92=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_tmp91=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f3;_LLA:
# 352
 Cyc_Binding_resolve_exp(env,_tmp93);Cyc_Binding_resolve_stmt(env,_tmp92);Cyc_Binding_resolve_stmt(env,_tmp91);return;case 14U: _LLB: _tmp95=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp94=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2).f1;_LLC:
 _tmp97=_tmp94;_tmp96=_tmp95;goto _LLE;case 5U: _LLD: _tmp97=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1).f1;_tmp96=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LLE:
 Cyc_Binding_resolve_exp(env,_tmp97);Cyc_Binding_resolve_stmt(env,_tmp96);return;case 9U: _LLF: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp9A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2).f1;_tmp99=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f3).f1;_tmp98=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f4;_LL10:
# 356
 Cyc_Binding_resolve_exp(env,_tmp9B);Cyc_Binding_resolve_exp(env,_tmp9A);Cyc_Binding_resolve_exp(env,_tmp99);
Cyc_Binding_resolve_stmt(env,_tmp98);
return;case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp9C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL18:
# 363
 for(0;_tmp9C != 0;_tmp9C=_tmp9C->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp9C->hd);}
return;case 13U: _LL19: _tmp9D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1A:
 Cyc_Binding_resolve_stmt(env,_tmp9D);return;case 12U: _LL1B: _tmp9F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp9E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1C: {
# 368
struct Cyc_Dict_Dict _tmp8C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmp9F);
Cyc_Binding_resolve_stmt(env,_tmp9E);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp8C;
return;}case 10U: _LL1D: _tmpA1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmpA0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1E:
 Cyc_Binding_resolve_exp(env,_tmpA1);Cyc_Binding_resolve_scs(env,_tmpA0);return;default: _LL1F: _tmpA3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmpA2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL20:
 Cyc_Binding_resolve_stmt(env,_tmpA3);Cyc_Binding_resolve_scs(env,_tmpA2);return;}_LL0:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 377
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpA4=e->r;void*_tmpA5=_tmpA4;void*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12B;struct Cyc_Absyn_Datatypedecl*_tmp12A;struct Cyc_Absyn_Datatypefield*_tmp129;struct Cyc_Absyn_Enumdecl*_tmp128;struct Cyc_Absyn_Enumfield*_tmp127;void*_tmp126;struct Cyc_Absyn_Enumfield*_tmp125;struct Cyc_List_List*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;struct Cyc_Absyn_Stmt*_tmp120;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;void**_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Vardecl*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct _tuple0**_tmpF6;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Aggrdecl**_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF1;int*_tmpF0;void**_tmpEF;switch(*((int*)_tmpA5)){case 1U: _LL1: _tmpEF=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL2: {
# 380
void*_tmpA6=*_tmpEF;void*_tmpA7=_tmpA6;struct _tuple0*_tmpC0;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7)->tag == 0U){_LL54: _tmpC0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7)->f1;_LL55: {
# 382
struct _handler_cons _tmpA8;_push_handler(& _tmpA8);{int _tmpAA=0;if(setjmp(_tmpA8.handler))_tmpAA=1;if(!_tmpAA){{void*_tmpAB=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpC0);void*_tmpAC=_tmpAB;struct Cyc_Absyn_Aggrdecl*_tmpBA;void*_tmpB9;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpB6;struct Cyc_Absyn_Datatypedecl*_tmpB5;struct Cyc_Absyn_Datatypefield*_tmpB4;void*_tmpB3;switch(*((int*)_tmpAC)){case 0U: _LL59: _tmpB3=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpAC)->f1;_LL5A:
*_tmpEF=_tmpB3;_npop_handler(0U);return;case 2U: _LL5B: _tmpB5=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB4=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL5C:
({void*_tmp2E5=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));((((((*_tmpAD).tag=31U,(*_tmpAD).f1=0)),(*_tmpAD).f2=_tmpB5)),(*_tmpAD).f3=_tmpB4);_tmpAD;});e->r=_tmp2E5;});_npop_handler(0U);return;case 3U: _LL5D: _tmpB7=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB6=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL5E:
({void*_tmp2E6=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));((((*_tmpAE).tag=32U,(*_tmpAE).f1=_tmpB7)),(*_tmpAE).f2=_tmpB6);_tmpAE;});e->r=_tmp2E6;});_npop_handler(0U);return;case 4U: _LL5F: _tmpB9=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB8=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL60:
({void*_tmp2E7=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));((((*_tmpAF).tag=33U,(*_tmpAF).f1=_tmpB9)),(*_tmpAF).f2=_tmpB8);_tmpAF;});e->r=_tmp2E7;});_npop_handler(0U);return;default: _LL61: _tmpBA=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpAC)->f1;_LL62:
# 388
({struct Cyc_String_pa_PrintArg_struct _tmpB2;_tmpB2.tag=0U;({struct _dyneither_ptr _tmp2E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC0));_tmpB2.f1=_tmp2E8;});({void*_tmpB0[1U];_tmpB0[0]=& _tmpB2;({unsigned int _tmp2EA=e->loc;struct _dyneither_ptr _tmp2E9=({const char*_tmpB1="bad occurrence of type name %s";_tag_dyneither(_tmpB1,sizeof(char),31U);});Cyc_Warn_err(_tmp2EA,_tmp2E9,_tag_dyneither(_tmpB0,sizeof(void*),1U));});});});
_npop_handler(0U);return;}_LL58:;}
# 382
;_pop_handler();}else{void*_tmpA9=(void*)_exn_thrown;void*_tmpBB=_tmpA9;void*_tmpBF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpBB)->tag == Cyc_Binding_BindingError){_LL64: _LL65:
# 392
({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0U;({struct _dyneither_ptr _tmp2EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC0));_tmpBE.f1=_tmp2EB;});({void*_tmpBC[1U];_tmpBC[0]=& _tmpBE;({unsigned int _tmp2ED=e->loc;struct _dyneither_ptr _tmp2EC=({const char*_tmpBD="undeclared identifier %s";_tag_dyneither(_tmpBD,sizeof(char),25U);});Cyc_Warn_err(_tmp2ED,_tmp2EC,_tag_dyneither(_tmpBC,sizeof(void*),1U));});});});
return;}else{_LL66: _tmpBF=_tmpBB;_LL67:(int)_rethrow(_tmpBF);}_LL63:;}};}}else{_LL56: _LL57:
# 395
 return;}_LL53:;}case 10U: _LL3: _tmpF2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF0=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f4;_LL4:
# 398
*_tmpF0=1;
# 400
{struct Cyc_List_List*_tmpC1=_tmpF1;for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpC1->hd);}}{
void*_tmpC2=_tmpF2->r;void*_tmpC3=_tmpC2;void**_tmpE0;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3)->tag == 1U){_LL69: _tmpE0=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3)->f1;_LL6A: {
# 404
void*_tmpC4=*_tmpE0;void*_tmpC5=_tmpC4;struct _tuple0*_tmpDF;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC5)->tag == 0U){_LL6E: _tmpDF=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC5)->f1;_LL6F: {
# 406
struct _handler_cons _tmpC6;_push_handler(& _tmpC6);{int _tmpC8=0;if(setjmp(_tmpC6.handler))_tmpC8=1;if(!_tmpC8){{void*_tmpC9=Cyc_Binding_lookup_ordinary(_tmpF2->loc,env,_tmpDF);void*_tmpCA=_tmpC9;struct Cyc_Absyn_Datatypedecl*_tmpD9;struct Cyc_Absyn_Datatypefield*_tmpD8;struct Cyc_Absyn_Aggrdecl*_tmpD7;void*_tmpD6;switch(*((int*)_tmpCA)){case 0U: _LL73: _tmpD6=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpCA)->f1;_LL74:
# 408
*_tmpE0=_tmpD6;_npop_handler(0U);return;case 1U: _LL75: _tmpD7=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpCA)->f1;_LL76: {
# 410
struct Cyc_List_List*_tmpCB=0;
for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
({struct Cyc_List_List*_tmp2EF=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));(({struct _tuple10*_tmp2EE=({struct _tuple10*_tmpCC=_cycalloc(sizeof(*_tmpCC));((*_tmpCC).f1=0,(*_tmpCC).f2=(struct Cyc_Absyn_Exp*)_tmpF1->hd);_tmpCC;});(*_tmpCD).hd=_tmp2EE;}),(*_tmpCD).tl=_tmpCB);_tmpCD;});_tmpCB=_tmp2EF;});}
({void*_tmp2F1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));((((((((*_tmpCE).tag=29U,(*_tmpCE).f1=_tmpD7->name)),(*_tmpCE).f2=0)),({struct Cyc_List_List*_tmp2F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCB);(*_tmpCE).f3=_tmp2F0;}))),(*_tmpCE).f4=_tmpD7);_tmpCE;});e->r=_tmp2F1;});
_npop_handler(0U);return;}case 2U: _LL77: _tmpD9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCA)->f1;_tmpD8=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCA)->f2;_LL78:
# 416
 if(_tmpD8->typs == 0)
# 418
({struct Cyc_String_pa_PrintArg_struct _tmpD1;_tmpD1.tag=0U;({struct _dyneither_ptr _tmp2F2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpD8->name));_tmpD1.f1=_tmp2F2;});({void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned int _tmp2F4=e->loc;struct _dyneither_ptr _tmp2F3=({const char*_tmpD0="%s is a constant, not a function";_tag_dyneither(_tmpD0,sizeof(char),33U);});Cyc_Warn_err(_tmp2F4,_tmp2F3,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});});
({void*_tmp2F5=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));((((((*_tmpD2).tag=31U,(*_tmpD2).f1=_tmpF1)),(*_tmpD2).f2=_tmpD9)),(*_tmpD2).f3=_tmpD8);_tmpD2;});e->r=_tmp2F5;});
_npop_handler(0U);return;case 4U: _LL79: _LL7A:
 goto _LL7C;default: _LL7B: _LL7C:
# 424
({struct Cyc_String_pa_PrintArg_struct _tmpD5;_tmpD5.tag=0U;({struct _dyneither_ptr _tmp2F6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpDF));_tmpD5.f1=_tmp2F6;});({void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({unsigned int _tmp2F8=e->loc;struct _dyneither_ptr _tmp2F7=({const char*_tmpD4="%s is an enum constructor, not a function";_tag_dyneither(_tmpD4,sizeof(char),42U);});Cyc_Warn_err(_tmp2F8,_tmp2F7,_tag_dyneither(_tmpD3,sizeof(void*),1U));});});});
_npop_handler(0U);return;}_LL72:;}
# 406
;_pop_handler();}else{void*_tmpC7=(void*)_exn_thrown;void*_tmpDA=_tmpC7;void*_tmpDE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpDA)->tag == Cyc_Binding_BindingError){_LL7E: _LL7F:
# 428
({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0U;({struct _dyneither_ptr _tmp2F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpDF));_tmpDD.f1=_tmp2F9;});({void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({unsigned int _tmp2FB=e->loc;struct _dyneither_ptr _tmp2FA=({const char*_tmpDC="undeclared identifier %s";_tag_dyneither(_tmpDC,sizeof(char),25U);});Cyc_Warn_err(_tmp2FB,_tmp2FA,_tag_dyneither(_tmpDB,sizeof(void*),1U));});});});
return;}else{_LL80: _tmpDE=_tmpDA;_LL81:(int)_rethrow(_tmpDE);}_LL7D:;}};}}else{_LL70: _LL71:
# 431
 return;}_LL6D:;}}else{_LL6B: _LL6C:
# 433
 Cyc_Binding_resolve_exp(env,_tmpF2);return;}_LL68:;};case 29U: _LL5: _tmpF6=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_tmpF3=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f4;_LL6:
# 437
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpF4->hd)).f2);}{
struct _handler_cons _tmpE1;_push_handler(& _tmpE1);{int _tmpE3=0;if(setjmp(_tmpE1.handler))_tmpE3=1;if(!_tmpE3){
({struct Cyc_Absyn_Aggrdecl*_tmp2FC=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmpF6,Cyc_Binding_lookup_aggrdecl);*_tmpF3=_tmp2FC;});
*_tmpF6=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmpF3))->name;
_npop_handler(0U);return;
# 440
;_pop_handler();}else{void*_tmpE2=(void*)_exn_thrown;void*_tmpE4=_tmpE2;void*_tmpE8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpE4)->tag == Cyc_Binding_BindingError){_LL83: _LL84:
# 444
({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0U;({struct _dyneither_ptr _tmp2FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmpF6));_tmpE7.f1=_tmp2FD;});({void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({unsigned int _tmp2FF=e->loc;struct _dyneither_ptr _tmp2FE=({const char*_tmpE6="unbound struct/union name %s";_tag_dyneither(_tmpE6,sizeof(char),29U);});Cyc_Warn_err(_tmp2FF,_tmp2FE,_tag_dyneither(_tmpE5,sizeof(void*),1U));});});});
return;}else{_LL85: _tmpE8=_tmpE4;_LL86:(int)_rethrow(_tmpE8);}_LL82:;}};};case 27U: _LL7: _tmpF9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL8:
# 449
 Cyc_Binding_resolve_exp(env,_tmpF8);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp301=({struct Cyc_Dict_Dict*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct Cyc_Dict_Dict _tmp300=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmpE9=_tmp300;});_tmpE9;});env->local_vars=_tmp301;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF9);
Cyc_Binding_resolve_exp(env,_tmpF7);
env->local_vars=0;
return;}{
# 457
struct Cyc_Dict_Dict _tmpEA=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF9);
Cyc_Binding_resolve_exp(env,_tmpF7);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpEA;
return;};case 36U: _LL9: _tmpFA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LLA:
# 465
 for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpFA->hd)).f2);}
return;case 28U: _LLB: _tmpFC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpFB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LLC:
# 471
 Cyc_Binding_resolve_exp(env,_tmpFC);
Cyc_Binding_resolve_type(e->loc,env,_tmpFB);
return;case 2U: _LLD: _LLE:
# 475
 goto _LL10;case 0U: _LLF: _LL10:
 return;case 24U: _LL11: _tmpFD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL12:
# 478
 _tmpFE=_tmpFD;goto _LL14;case 3U: _LL13: _tmpFE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL14:
# 480
 for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpFE->hd);}
return;case 38U: _LL15: _tmpFF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL16:
# 484
 _tmp100=_tmpFF;goto _LL18;case 12U: _LL17: _tmp100=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL18:
 _tmp101=_tmp100;goto _LL1A;case 18U: _LL19: _tmp101=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL1A:
 _tmp102=_tmp101;goto _LL1C;case 11U: _LL1B: _tmp102=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL1C:
 _tmp103=_tmp102;goto _LL1E;case 5U: _LL1D: _tmp103=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL1E:
 _tmp104=_tmp103;goto _LL20;case 22U: _LL1F: _tmp104=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL20:
 _tmp105=_tmp104;goto _LL22;case 21U: _LL21: _tmp105=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL22:
 _tmp106=_tmp105;goto _LL24;case 15U: _LL23: _tmp106=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL24:
 _tmp107=_tmp106;goto _LL26;case 20U: _LL25: _tmp107=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL26:
 Cyc_Binding_resolve_exp(env,_tmp107);return;case 35U: _LL27: _tmp109=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp108=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL28:
# 494
 _tmp10B=_tmp109;_tmp10A=_tmp108;goto _LL2A;case 9U: _LL29: _tmp10B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL2A:
 _tmp10D=_tmp10B;_tmp10C=_tmp10A;goto _LL2C;case 4U: _LL2B: _tmp10D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL2C:
 _tmp10F=_tmp10D;_tmp10E=_tmp10C;goto _LL2E;case 23U: _LL2D: _tmp10F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL2E:
 _tmp111=_tmp10F;_tmp110=_tmp10E;goto _LL30;case 7U: _LL2F: _tmp111=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp110=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL30:
 _tmp113=_tmp111;_tmp112=_tmp110;goto _LL32;case 8U: _LL31: _tmp113=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp112=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL32:
 Cyc_Binding_resolve_exp(env,_tmp113);Cyc_Binding_resolve_exp(env,_tmp112);return;case 34U: _LL33: _tmp116=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).rgn;_tmp115=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).elt_type;_tmp114=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).num_elts;_LL34:
# 502
 if(_tmp116 != 0)Cyc_Binding_resolve_exp(env,_tmp116);
if(_tmp115 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp115);
Cyc_Binding_resolve_exp(env,_tmp114);
return;case 16U: _LL35: _tmp118=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp117=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL36:
# 508
 if(_tmp118 != 0)Cyc_Binding_resolve_exp(env,_tmp118);
Cyc_Binding_resolve_exp(env,_tmp117);
return;case 6U: _LL37: _tmp11B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL38:
# 513
 Cyc_Binding_resolve_exp(env,_tmp11B);Cyc_Binding_resolve_exp(env,_tmp11A);Cyc_Binding_resolve_exp(env,_tmp119);return;case 13U: _LL39: _tmp11D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL3A:
# 516
 Cyc_Binding_resolve_exp(env,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp11C->hd);}
return;case 14U: _LL3B: _tmp11F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL3C:
# 522
 Cyc_Binding_resolve_exp(env,_tmp11E);Cyc_Binding_resolve_type(e->loc,env,_tmp11F);return;case 37U: _LL3D: _tmp120=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL3E:
# 524
 Cyc_Binding_resolve_stmt(env,_tmp120);return;case 39U: _LL3F: _tmp121=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL40:
# 526
 _tmp122=_tmp121;goto _LL42;case 19U: _LL41: _tmp122=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL42:
 _tmp123=_tmp122;goto _LL44;case 17U: _LL43: _tmp123=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL44:
 Cyc_Binding_resolve_type(e->loc,env,_tmp123);return;case 26U: _LL45: _tmp124=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL46:
# 531
 for(0;_tmp124 != 0;_tmp124=_tmp124->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp124->hd)).f2);}
return;case 25U: _LL47: _LL48:
# 536
({void*_tmpEB=0U;({unsigned int _tmp303=e->loc;struct _dyneither_ptr _tmp302=({const char*_tmpEC="binding: compound literals unimplemented";_tag_dyneither(_tmpEC,sizeof(char),41U);});Cyc_Warn_err(_tmp303,_tmp302,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});return;case 33U: _LL49: _tmp126=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp125=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL4A:
# 539
 Cyc_Binding_resolve_type(e->loc,env,_tmp126);return;case 32U: _LL4B: _tmp128=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp127=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL4C:
 return;case 31U: _LL4D: _tmp12B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp12A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmp129=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL4E:
# 542
 for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp12B->hd);}
return;case 30U: _LL4F: _tmp12D=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp12C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL50:
# 546
 Cyc_Binding_resolve_type(e->loc,env,_tmp12D);
for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp12C->hd)).f2);}
return;default: _LL51: _LL52:
# 552
({void*_tmpED=0U;({unsigned int _tmp305=e->loc;struct _dyneither_ptr _tmp304=({const char*_tmpEE="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmpEE,sizeof(char),49U);});Cyc_Warn_err(_tmp305,_tmp304,_tag_dyneither(_tmpED,sizeof(void*),0U));});});}_LL0:;}
# 556
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp12E=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp12F=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp130=_tmp12F;struct Cyc_Absyn_Pat*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Stmt*_tmp131;_LL1: _tmp133=_tmp130->pattern;_tmp132=_tmp130->where_clause;_tmp131=_tmp130->body;_LL2:;
Cyc_Binding_resolve_pat(env,_tmp133);
if(_tmp132 != 0)
Cyc_Binding_resolve_exp(env,_tmp132);
Cyc_Binding_resolve_stmt(env,_tmp131);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp12E;}
# 566
return;}
# 568
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp134=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp135=_tmp134;struct _dyneither_ptr*_tmp13B;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_List_List*_tmp137;struct Cyc_Absyn_Exp*_tmp136;_LL1: _tmp13B=_tmp135->name;_tmp13A=_tmp135->tq;_tmp139=_tmp135->type;_tmp138=_tmp135->width;_tmp137=_tmp135->attributes;_tmp136=_tmp135->requires_clause;_LL2:;
Cyc_Binding_resolve_type(loc,env,_tmp139);
if(_tmp138 != 0)
Cyc_Binding_resolve_exp(env,_tmp138);
if(_tmp136 != 0)
Cyc_Binding_resolve_exp(env,_tmp136);}
# 577
return;}struct _tuple11{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 580
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 586
struct Cyc_List_List*_tmp13C=0;
for(0;args != 0;args=args->tl){
struct _tuple11*_tmp13D=(struct _tuple11*)args->hd;struct _tuple11*_tmp13E=_tmp13D;void*_tmp145;struct Cyc_Absyn_Tqual _tmp144;void*_tmp143;_LL1: _tmp145=_tmp13E->f1;_tmp144=_tmp13E->f2;_tmp143=_tmp13E->f3;_LL2:;{
struct _dyneither_ptr*_tmp13F=f(_tmp145);
if(_tmp13F == 0)continue;{
struct Cyc_Absyn_Vardecl*_tmp140=({struct _tuple0*_tmp306=({struct _tuple0*_tmp142=_cycalloc(sizeof(*_tmp142));((*_tmp142).f1=Cyc_Absyn_Loc_n,(*_tmp142).f2=_tmp13F);_tmp142;});Cyc_Absyn_new_vardecl(0U,_tmp306,_tmp143,0);});
_tmp140->tq=_tmp144;
({struct Cyc_List_List*_tmp307=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));((*_tmp141).hd=_tmp140,(*_tmp141).tl=_tmp13C);_tmp141;});_tmp13C=_tmp307;});};};}
# 595
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp146=*vai;struct Cyc_Absyn_VarargInfo _tmp147=_tmp146;struct _dyneither_ptr*_tmp151;struct Cyc_Absyn_Tqual _tmp150;void*_tmp14F;int _tmp14E;_LL4: _tmp151=_tmp147.name;_tmp150=_tmp147.tq;_tmp14F=_tmp147.type;_tmp14E=_tmp147.inject;_LL5:;
if(_tmp151 == 0){
if(need_va_name)
({void*_tmp148=0U;({unsigned int _tmp309=loc;struct _dyneither_ptr _tmp308=({const char*_tmp149="missing name for vararg";_tag_dyneither(_tmp149,sizeof(char),24U);});Cyc_Warn_err(_tmp309,_tmp308,_tag_dyneither(_tmp148,sizeof(void*),0U));});});}else{
# 602
void*_tmp14A=Cyc_Absyn_dyneither_typ(_tmp14F,(void*)& Cyc_Absyn_HeapRgn_val,_tmp150,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp14B=({struct _tuple0*_tmp30A=({struct _tuple0*_tmp14D=_cycalloc(sizeof(*_tmp14D));((*_tmp14D).f1=Cyc_Absyn_Loc_n,(*_tmp14D).f2=_tmp151);_tmp14D;});Cyc_Absyn_new_vardecl(0U,_tmp30A,_tmp14A,0);});
({struct Cyc_List_List*_tmp30B=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));((*_tmp14C).hd=_tmp14B,(*_tmp14C).tl=_tmp13C);_tmp14C;});_tmp13C=_tmp30B;});}}
# 607
({struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13C);_tmp13C=_tmp30C;});
return _tmp13C;}
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
struct Cyc_Dict_Dict _tmp152=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp153=({struct _dyneither_ptr*_tmp159=_cycalloc(sizeof(*_tmp159));({struct _dyneither_ptr _tmp30D=({const char*_tmp158="return_value";_tag_dyneither(_tmp158,sizeof(char),13U);});*_tmp159=_tmp30D;});_tmp159;});
struct Cyc_Absyn_Vardecl*_tmp154=({struct _tuple0*_tmp30E=({struct _tuple0*_tmp157=_cycalloc(sizeof(*_tmp157));((*_tmp157).f1=Cyc_Absyn_Loc_n,(*_tmp157).f2=_tmp153);_tmp157;});Cyc_Absyn_new_vardecl(0U,_tmp30E,rettyp,0);});
({struct Cyc_Dict_Dict _tmp312=({struct Cyc_Dict_Dict _tmp311=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp310=_tmp153;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp311,_tmp310,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp156=_cycalloc(sizeof(*_tmp156));
((*_tmp156).tag=0U,({void*_tmp30F=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp155=_cycalloc(sizeof(*_tmp155));((*_tmp155).tag=4U,(*_tmp155).f1=_tmp154);_tmp155;});(*_tmp156).f1=_tmp30F;}));_tmp156;}));});
# 632
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp312;});
# 634
Cyc_Binding_resolve_exp(env,ens_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp152;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 644
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp15A=t;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D1;void*_tmp1D0;void*_tmp1CF;void*_tmp1CE;void*_tmp1CD;void*_tmp1CC;void*_tmp1CB;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_FnInfo _tmp1C9;void*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_List_List*_tmp1C6;void*_tmp1C5;void*_tmp1C4;union Cyc_Absyn_Constraint*_tmp1C3;void*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_TypeDecl*_tmp1BF;void***_tmp1BE;struct _tuple0**_tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Typedefdecl**_tmp1BB;struct _tuple0**_tmp1BA;struct Cyc_Absyn_Enumdecl*_tmp1B9;union Cyc_Absyn_AggrInfoU*_tmp1B8;struct Cyc_List_List*_tmp1B7;union Cyc_Absyn_DatatypeFieldInfoU*_tmp1B6;struct Cyc_List_List*_tmp1B5;union Cyc_Absyn_DatatypeInfoU*_tmp1B4;struct Cyc_List_List*_tmp1B3;switch(*((int*)_tmp15A)){case 3U: _LL1: _tmp1B4=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A)->f1).datatype_info;_tmp1B3=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A)->f1).targs;_LL2:
# 647
 for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B3->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp15B=*_tmp1B4;union Cyc_Absyn_DatatypeInfoU _tmp15C=_tmp15B;struct _tuple0*_tmp164;int _tmp163;if((_tmp15C.UnknownDatatype).tag == 1){_LL3C: _tmp164=((_tmp15C.UnknownDatatype).val).name;_tmp163=((_tmp15C.UnknownDatatype).val).is_extensible;_LL3D: {
# 651
struct _handler_cons _tmp15D;_push_handler(& _tmp15D);{int _tmp15F=0;if(setjmp(_tmp15D.handler))_tmp15F=1;if(!_tmp15F){
{struct Cyc_Absyn_Datatypedecl*_tmp160=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp164,Cyc_Binding_lookup_datatypedecl))->hd;
({union Cyc_Absyn_DatatypeInfoU _tmp313=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp29F;(_tmp29F.name=_tmp160->name,_tmp29F.is_extensible=_tmp163);_tmp29F;}));*_tmp1B4=_tmp313;});
_npop_handler(0U);return;}
# 652
;_pop_handler();}else{void*_tmp15E=(void*)_exn_thrown;void*_tmp161=_tmp15E;void*_tmp162;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp161)->tag == Cyc_Binding_BindingError){_LL41: _LL42:
# 656
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp164,Cyc_Absyn_Public);return;}else{_LL43: _tmp162=_tmp161;_LL44:(int)_rethrow(_tmp162);}_LL40:;}};}}else{_LL3E: _LL3F:
 return;}_LL3B:;};case 4U: _LL3: _tmp1B6=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15A)->f1).field_info;_tmp1B5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15A)->f1).targs;_LL4:
# 660
 for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B5->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp165=*_tmp1B6;union Cyc_Absyn_DatatypeFieldInfoU _tmp166=_tmp165;struct _tuple0*_tmp17B;struct _tuple0*_tmp17A;int _tmp179;if((_tmp166.UnknownDatatypefield).tag == 1){_LL46: _tmp17B=((_tmp166.UnknownDatatypefield).val).datatype_name;_tmp17A=((_tmp166.UnknownDatatypefield).val).field_name;_tmp179=((_tmp166.UnknownDatatypefield).val).is_extensible;_LL47:
# 666
{struct _handler_cons _tmp167;_push_handler(& _tmp167);{int _tmp169=0;if(setjmp(_tmp167.handler))_tmp169=1;if(!_tmp169){
{void*_tmp16A=Cyc_Binding_lookup_ordinary(loc,env,_tmp17A);void*_tmp16B=_tmp16A;struct Cyc_Absyn_Datatypedecl*_tmp173;struct Cyc_Absyn_Datatypefield*_tmp172;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16B)->tag == 2U){_LL4B: _tmp173=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16B)->f1;_tmp172=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16B)->f2;_LL4C: {
# 669
struct Cyc_Absyn_Datatypedecl*_tmp16C=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp17B,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp173->name,_tmp16C->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0U;({struct _dyneither_ptr _tmp314=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 674
Cyc_Absynpp_qvar2string(_tmp173->name));_tmp171.f1=_tmp314;});({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0U;({struct _dyneither_ptr _tmp315=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 673
Cyc_Absynpp_qvar2string(_tmp16C->name));_tmp170.f1=_tmp315;});({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0U;({struct _dyneither_ptr _tmp316=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 672
Cyc_Absynpp_qvar2string(_tmp172->name));_tmp16F.f1=_tmp316;});({void*_tmp16D[3U];(((_tmp16D[0]=& _tmp16F,_tmp16D[1]=& _tmp170)),_tmp16D[2]=& _tmp171);({unsigned int _tmp318=loc;struct _dyneither_ptr _tmp317=({const char*_tmp16E="%s is a variant of %s, not %s (shadowing not yet implemented properly)";_tag_dyneither(_tmp16E,sizeof(char),71U);});Cyc_Warn_err(_tmp318,_tmp317,_tag_dyneither(_tmp16D,sizeof(void*),3U));});});});});});
# 675
_npop_handler(0U);return;}
# 677
({union Cyc_Absyn_DatatypeFieldInfoU _tmp319=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2A0;(((_tmp2A0.datatype_name=_tmp173->name,_tmp2A0.field_name=_tmp172->name)),_tmp2A0.is_extensible=_tmp179);_tmp2A0;}));*_tmp1B6=_tmp319;});
# 679
_npop_handler(0U);return;}}else{_LL4D: _LL4E:
 goto _LL4A;}_LL4A:;}
# 667
;_pop_handler();}else{void*_tmp168=(void*)_exn_thrown;void*_tmp174=_tmp168;void*_tmp175;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp174)->tag == Cyc_Binding_BindingError){_LL50: _LL51:
# 682
 goto _LL4F;}else{_LL52: _tmp175=_tmp174;_LL53:(int)_rethrow(_tmp175);}_LL4F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0U;({struct _dyneither_ptr _tmp31A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp17A));_tmp178.f1=_tmp31A;});({void*_tmp176[1U];_tmp176[0]=& _tmp178;({unsigned int _tmp31C=loc;struct _dyneither_ptr _tmp31B=({const char*_tmp177="%s is not a datatype field";_tag_dyneither(_tmp177,sizeof(char),27U);});Cyc_Warn_err(_tmp31C,_tmp31B,_tag_dyneither(_tmp176,sizeof(void*),1U));});});});
return;}else{_LL48: _LL49:
 return;}_LL45:;};case 11U: _LL5: _tmp1B8=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15A)->f1).aggr_info;_tmp1B7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15A)->f1).targs;_LL6:
# 689
 for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B7->hd);}{
union Cyc_Absyn_AggrInfoU _tmp17C=*_tmp1B8;union Cyc_Absyn_AggrInfoU _tmp17D=_tmp17C;enum Cyc_Absyn_AggrKind _tmp18A;struct _tuple0*_tmp189;struct Cyc_Core_Opt*_tmp188;if((_tmp17D.UnknownAggr).tag == 1){_LL55: _tmp18A=((_tmp17D.UnknownAggr).val).f1;_tmp189=((_tmp17D.UnknownAggr).val).f2;_tmp188=((_tmp17D.UnknownAggr).val).f3;_LL56: {
# 693
struct _handler_cons _tmp17E;_push_handler(& _tmp17E);{int _tmp180=0;if(setjmp(_tmp17E.handler))_tmp180=1;if(!_tmp180){
{struct Cyc_Absyn_Aggrdecl*_tmp181=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp189,Cyc_Binding_lookup_aggrdecl);
if(_tmp181->kind != _tmp18A)
({void*_tmp182=0U;({unsigned int _tmp31E=loc;struct _dyneither_ptr _tmp31D=({const char*_tmp183="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp183,sizeof(char),51U);});Cyc_Warn_err(_tmp31E,_tmp31D,_tag_dyneither(_tmp182,sizeof(void*),0U));});});
if(((_tmp18A == Cyc_Absyn_UnionA  && _tmp188 != 0) && _tmp181->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp181->impl))->tagged != (int)_tmp188->v)
# 699
({void*_tmp184=0U;({unsigned int _tmp320=loc;struct _dyneither_ptr _tmp31F=({const char*_tmp185="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp185,sizeof(char),42U);});Cyc_Warn_err(_tmp320,_tmp31F,_tag_dyneither(_tmp184,sizeof(void*),0U));});});
({union Cyc_Absyn_AggrInfoU _tmp321=Cyc_Absyn_UnknownAggr(_tmp18A,_tmp181->name,_tmp188);*_tmp1B8=_tmp321;});
_npop_handler(0U);return;}
# 694
;_pop_handler();}else{void*_tmp17F=(void*)_exn_thrown;void*_tmp186=_tmp17F;void*_tmp187;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp186)->tag == Cyc_Binding_BindingError){_LL5A: _LL5B:
# 703
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp189,Cyc_Absyn_Public);return;}else{_LL5C: _tmp187=_tmp186;_LL5D:(int)_rethrow(_tmp187);}_LL59:;}};}}else{_LL57: _LL58:
 return;}_LL54:;};case 13U: _LL7: _tmp1BA=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15A)->f1;_tmp1B9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15A)->f2;_LL8:
# 707
 if(_tmp1B9 != 0)
return;{
struct _handler_cons _tmp18B;_push_handler(& _tmp18B);{int _tmp18D=0;if(setjmp(_tmp18B.handler))_tmp18D=1;if(!_tmp18D){
{struct Cyc_Absyn_Enumdecl*_tmp18E=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1BA,Cyc_Binding_lookup_enumdecl);
*_tmp1BA=_tmp18E->name;
_npop_handler(0U);return;}
# 710
;_pop_handler();}else{void*_tmp18C=(void*)_exn_thrown;void*_tmp18F=_tmp18C;void*_tmp190;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp18F)->tag == Cyc_Binding_BindingError){_LL5F: _LL60:
# 714
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1BA,Cyc_Absyn_Public);return;}else{_LL61: _tmp190=_tmp18F;_LL62:(int)_rethrow(_tmp190);}_LL5E:;}};};case 17U: _LL9: _tmp1BD=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15A)->f1;_tmp1BC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15A)->f2;_tmp1BB=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15A)->f3;_LLA:
# 717
 for(0;_tmp1BC != 0;_tmp1BC=_tmp1BC->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1BC->hd);}
{struct _handler_cons _tmp191;_push_handler(& _tmp191);{int _tmp193=0;if(setjmp(_tmp191.handler))_tmp193=1;if(!_tmp193){
{struct Cyc_Absyn_Typedefdecl*_tmp194=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1BD,Cyc_Binding_lookup_typedefdecl);
# 722
*_tmp1BD=_tmp194->name;
_npop_handler(0U);return;}
# 720
;_pop_handler();}else{void*_tmp192=(void*)_exn_thrown;void*_tmp195=_tmp192;void*_tmp196;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp195)->tag == Cyc_Binding_BindingError){_LL64: _LL65:
# 724
 goto _LL63;}else{_LL66: _tmp196=_tmp195;_LL67:(int)_rethrow(_tmp196);}_LL63:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0U;({struct _dyneither_ptr _tmp322=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1BD));_tmp199.f1=_tmp322;});({void*_tmp197[1U];_tmp197[0]=& _tmp199;({unsigned int _tmp324=loc;struct _dyneither_ptr _tmp323=({const char*_tmp198="unbound typdef name %s";_tag_dyneither(_tmp198,sizeof(char),23U);});Cyc_Warn_err(_tmp324,_tmp323,_tag_dyneither(_tmp197,sizeof(void*),1U));});});});
return;case 26U: _LLB: _tmp1BF=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A)->f1;_tmp1BE=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A)->f2;_LLC: {
# 730
struct Cyc_Dict_Dict*_tmp19A=env->local_vars;
env->local_vars=0;
{void*_tmp19B=_tmp1BF->r;void*_tmp19C=_tmp19B;struct Cyc_Absyn_Datatypedecl*_tmp1A5;struct Cyc_Absyn_Enumdecl*_tmp1A4;struct Cyc_Absyn_Aggrdecl*_tmp1A3;switch(*((int*)_tmp19C)){case 0U: _LL69: _tmp1A3=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp19C)->f1;_LL6A:
# 734
({struct Cyc_Binding_Env*_tmp326=env;Cyc_Binding_resolve_decl(_tmp326,({struct Cyc_Absyn_Decl*_tmp19E=_cycalloc(sizeof(*_tmp19E));(({void*_tmp325=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));((*_tmp19D).tag=5U,(*_tmp19D).f1=_tmp1A3);_tmp19D;});(*_tmp19E).r=_tmp325;}),(*_tmp19E).loc=_tmp1BF->loc);_tmp19E;}));});goto _LL68;case 1U: _LL6B: _tmp1A4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp19C)->f1;_LL6C:
# 736
({struct Cyc_Binding_Env*_tmp328=env;Cyc_Binding_resolve_decl(_tmp328,({struct Cyc_Absyn_Decl*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));(({void*_tmp327=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));((*_tmp19F).tag=7U,(*_tmp19F).f1=_tmp1A4);_tmp19F;});(*_tmp1A0).r=_tmp327;}),(*_tmp1A0).loc=_tmp1BF->loc);_tmp1A0;}));});goto _LL68;default: _LL6D: _tmp1A5=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp19C)->f1;_LL6E:
# 738
({struct Cyc_Binding_Env*_tmp32A=env;Cyc_Binding_resolve_decl(_tmp32A,({struct Cyc_Absyn_Decl*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));(({void*_tmp329=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));((*_tmp1A1).tag=6U,(*_tmp1A1).f1=_tmp1A5);_tmp1A1;});(*_tmp1A2).r=_tmp329;}),(*_tmp1A2).loc=_tmp1BF->loc);_tmp1A2;}));});goto _LL68;}_LL68:;}
# 740
env->local_vars=_tmp19A;
return;}case 12U: _LLD: _tmp1C0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15A)->f2;_LLE:
# 747
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1C0);
return;case 14U: _LLF: _tmp1C1=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp15A)->f1;_LL10:
# 752
 for(0;_tmp1C1 != 0;_tmp1C1=_tmp1C1->tl){
struct Cyc_Absyn_Enumfield*_tmp1A6=(struct Cyc_Absyn_Enumfield*)_tmp1C1->hd;
if(_tmp1A6->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1A6->tag));}
# 757
return;case 1U: _LL11: _tmp1C2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15A)->f2;_LL12:
 if(_tmp1C2 != 0)Cyc_Binding_resolve_type(loc,env,_tmp1C2);return;case 28U: _LL13: _LL14:
# 760
 goto _LL16;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 20U: _LL1B: _LL1C:
 goto _LL1E;case 21U: _LL1D: _LL1E:
 goto _LL20;case 22U: _LL1F: _LL20:
 goto _LL22;case 2U: _LL21: _LL22:
 return;case 5U: _LL23: _tmp1C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15A)->f1).elt_typ;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15A)->f1).ptr_atts).rgn;_tmp1C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15A)->f1).ptr_atts).bounds;_LL24:
# 770
 Cyc_Binding_resolve_type(loc,env,_tmp1C5);
Cyc_Binding_resolve_type(loc,env,_tmp1C4);
while(1){
union Cyc_Absyn_Constraint*_tmp1A7=_tmp1C3;void*_tmp1AB;union Cyc_Absyn_Constraint*_tmp1AA;switch((((union Cyc_Absyn_Constraint*)_tmp1A7)->Forward_constr).tag){case 3U: _LL70: _LL71:
 return;case 2U: _LL72: _tmp1AA=(_tmp1A7->Forward_constr).val;_LL73:
 _tmp1C3=_tmp1AA;continue;default: _LL74: _tmp1AB=(void*)(_tmp1A7->Eq_constr).val;_LL75: {
# 777
void*_tmp1A8=_tmp1AB;struct Cyc_Absyn_Exp*_tmp1A9;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1A8)->tag == 0U){_LL77: _LL78:
 return;}else{_LL79: _tmp1A9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1A8)->f1;_LL7A:
 Cyc_Binding_resolve_exp(env,_tmp1A9);return;}_LL76:;}}_LL6F:;}case 10U: _LL25: _tmp1C6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15A)->f1;_LL26:
# 783
 for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp1C6->hd)).f2);}
return;case 8U: _LL27: _tmp1C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15A)->f1).elt_type;_tmp1C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15A)->f1).num_elts;_LL28:
# 787
 Cyc_Binding_resolve_type(loc,env,_tmp1C8);
if(_tmp1C7 != 0)Cyc_Binding_resolve_exp(env,_tmp1C7);
return;case 9U: _LL29: _tmp1C9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15A)->f1;_LL2A: {
# 794
struct Cyc_List_List*_tmp1AC=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fntype,0,loc,env,_tmp1C9.args,_tmp1C9.cyc_varargs);
# 797
struct Cyc_Dict_Dict*_tmp1AD=env->local_vars;
if(_tmp1AD != 0)
({struct Cyc_Dict_Dict*_tmp32B=({struct Cyc_Dict_Dict*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));*_tmp1AE=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1AE;});env->local_vars=_tmp32B;});else{
# 801
({struct Cyc_Dict_Dict*_tmp32D=({struct Cyc_Dict_Dict*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));({struct Cyc_Dict_Dict _tmp32C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1AF=_tmp32C;});_tmp1AF;});env->local_vars=_tmp32D;});}
{struct Cyc_List_List*_tmp1B0=_tmp1AC;for(0;_tmp1B0 != 0;_tmp1B0=_tmp1B0->tl){
({struct Cyc_Dict_Dict _tmp331=({struct Cyc_Dict_Dict _tmp330=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp32F=(*((struct Cyc_Absyn_Vardecl*)_tmp1B0->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp330,_tmp32F,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));
((*_tmp1B2).tag=0U,({void*_tmp32E=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));((*_tmp1B1).tag=3U,(*_tmp1B1).f1=(struct Cyc_Absyn_Vardecl*)_tmp1B0->hd);_tmp1B1;});(*_tmp1B2).f1=_tmp32E;}));_tmp1B2;}));});
# 803
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp331;});}}
# 806
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp1C9.effect,_tmp1C9.ret_typ,_tmp1C9.args,_tmp1C9.cyc_varargs,_tmp1C9.rgn_po,_tmp1C9.requires_clause,_tmp1C9.ensures_clause);
# 810
env->local_vars=_tmp1AD;
return;}case 24U: _LL2B: _tmp1CA=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp15A)->f1;_LL2C:
# 814
 for(0;_tmp1CA != 0;_tmp1CA=_tmp1CA->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CA->hd);}
return;case 19U: _LL2D: _tmp1CB=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp15A)->f1;_LL2E:
# 818
 _tmp1CC=_tmp1CB;goto _LL30;case 23U: _LL2F: _tmp1CC=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp15A)->f1;_LL30:
 _tmp1CD=_tmp1CC;goto _LL32;case 25U: _LL31: _tmp1CD=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp15A)->f1;_LL32:
 _tmp1CE=_tmp1CD;goto _LL34;case 15U: _LL33: _tmp1CE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A)->f1;_LL34:
 Cyc_Binding_resolve_type(loc,env,_tmp1CE);return;case 16U: _LL35: _tmp1D0=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp15A)->f1;_tmp1CF=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp15A)->f2;_LL36:
# 824
 Cyc_Binding_resolve_type(loc,env,_tmp1D0);Cyc_Binding_resolve_type(loc,env,_tmp1CF);return;case 18U: _LL37: _tmp1D1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp15A)->f1;_LL38:
# 826
 _tmp1D2=_tmp1D1;goto _LL3A;default: _LL39: _tmp1D2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp15A)->f1;_LL3A:
 Cyc_Binding_resolve_exp(env,_tmp1D2);return;}_LL0:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 831
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1D3=p;void*_tmp222;unsigned int _tmp221;_LL1: _tmp222=_tmp1D3->r;_tmp221=_tmp1D3->loc;_LL2:;{
void*_tmp1D4=_tmp222;struct Cyc_List_List*_tmp220;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_List_List*_tmp21E;struct Cyc_Absyn_Pat*_tmp21D;struct Cyc_Absyn_Vardecl*_tmp21C;struct Cyc_Absyn_Vardecl*_tmp21B;struct Cyc_Absyn_Vardecl*_tmp21A;struct Cyc_Absyn_Pat*_tmp219;struct Cyc_Absyn_Vardecl*_tmp218;struct Cyc_Absyn_Pat*_tmp217;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp215;struct _tuple0*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp211;int _tmp210;struct _tuple0*_tmp20F;struct Cyc_List_List*_tmp20E;int _tmp20D;struct _tuple0*_tmp20C;switch(*((int*)_tmp1D4)){case 15U: _LL4: _tmp20C=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL5:
# 835
{struct _handler_cons _tmp1D5;_push_handler(& _tmp1D5);{int _tmp1D7=0;if(setjmp(_tmp1D5.handler))_tmp1D7=1;if(!_tmp1D7){{void*_tmp1D8=Cyc_Binding_lookup_ordinary(_tmp221,env,_tmp20C);void*_tmp1D9=_tmp1D8;void*_tmp1E4;struct Cyc_Absyn_Enumfield*_tmp1E3;struct Cyc_Absyn_Enumdecl*_tmp1E2;struct Cyc_Absyn_Enumfield*_tmp1E1;struct Cyc_Absyn_Datatypedecl*_tmp1E0;struct Cyc_Absyn_Datatypefield*_tmp1DF;switch(*((int*)_tmp1D9)){case 0U: _LL2D: _LL2E:
 goto _LL2C;case 1U: _LL2F: _LL30:
# 838
({void*_tmp1DA=0U;({unsigned int _tmp333=_tmp221;struct _dyneither_ptr _tmp332=({const char*_tmp1DB="struct tag used without arguments in pattern";_tag_dyneither(_tmp1DB,sizeof(char),45U);});Cyc_Warn_err(_tmp333,_tmp332,_tag_dyneither(_tmp1DA,sizeof(void*),0U));});});
_npop_handler(0U);return;case 2U: _LL31: _tmp1E0=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1D9)->f1;_tmp1DF=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1D9)->f2;_LL32:
# 841
({void*_tmp334=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));((((((((*_tmp1DC).tag=8U,(*_tmp1DC).f1=_tmp1E0)),(*_tmp1DC).f2=_tmp1DF)),(*_tmp1DC).f3=0)),(*_tmp1DC).f4=0);_tmp1DC;});p->r=_tmp334;});_npop_handler(0U);return;case 3U: _LL33: _tmp1E2=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1D9)->f1;_tmp1E1=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1D9)->f2;_LL34:
# 843
({void*_tmp335=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));((((*_tmp1DD).tag=13U,(*_tmp1DD).f1=_tmp1E2)),(*_tmp1DD).f2=_tmp1E1);_tmp1DD;});p->r=_tmp335;});_npop_handler(0U);return;default: _LL35: _tmp1E4=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1D9)->f1;_tmp1E3=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1D9)->f2;_LL36:
# 845
({void*_tmp336=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));((((*_tmp1DE).tag=14U,(*_tmp1DE).f1=_tmp1E4)),(*_tmp1DE).f2=_tmp1E3);_tmp1DE;});p->r=_tmp336;});_npop_handler(0U);return;}_LL2C:;}
# 835
;_pop_handler();}else{void*_tmp1D6=(void*)_exn_thrown;void*_tmp1E5=_tmp1D6;void*_tmp1E6;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1E5)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 846
 goto _LL37;}else{_LL3A: _tmp1E6=_tmp1E5;_LL3B:(int)_rethrow(_tmp1E6);}_LL37:;}};}{
struct Cyc_Absyn_Vardecl*_tmp1E7=Cyc_Absyn_new_vardecl(0U,_tmp20C,(void*)& Cyc_Absyn_VoidType_val,0);
Cyc_Binding_resolve_and_add_vardecl(_tmp221,env,_tmp1E7);
({void*_tmp338=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));((((*_tmp1E8).tag=1U,(*_tmp1E8).f1=_tmp1E7)),({struct Cyc_Absyn_Pat*_tmp337=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);(*_tmp1E8).f2=_tmp337;}));_tmp1E8;});p->r=_tmp338;});
return;};case 16U: _LL6: _tmp20F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp20E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_tmp20D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_LL7:
# 853
{struct Cyc_List_List*_tmp1E9=_tmp20E;for(0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp1E9->hd);}}
{struct _handler_cons _tmp1EA;_push_handler(& _tmp1EA);{int _tmp1EC=0;if(setjmp(_tmp1EA.handler))_tmp1EC=1;if(!_tmp1EC){{void*_tmp1ED=Cyc_Binding_lookup_ordinary(_tmp221,env,_tmp20F);void*_tmp1EE=_tmp1ED;struct Cyc_Absyn_Datatypedecl*_tmp1FA;struct Cyc_Absyn_Datatypefield*_tmp1F9;struct Cyc_Absyn_Aggrdecl*_tmp1F8;switch(*((int*)_tmp1EE)){case 0U: _LL3D: _LL3E:
 goto _LL3C;case 1U: _LL3F: _tmp1F8=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp1EE)->f1;_LL40: {
# 858
struct Cyc_List_List*_tmp1EF=0;
for(0;_tmp20E != 0;_tmp20E=_tmp20E->tl){
({struct Cyc_List_List*_tmp33A=({struct Cyc_List_List*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));(({struct _tuple13*_tmp339=({struct _tuple13*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));((*_tmp1F0).f1=0,(*_tmp1F0).f2=(struct Cyc_Absyn_Pat*)_tmp20E->hd);_tmp1F0;});(*_tmp1F1).hd=_tmp339;}),(*_tmp1F1).tl=_tmp1EF);_tmp1F1;});_tmp1EF=_tmp33A;});}
({void*_tmp33E=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));((((((((*_tmp1F4).tag=7U,({struct Cyc_Absyn_AggrInfo*_tmp33D=({struct Cyc_Absyn_AggrInfo*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));(({union Cyc_Absyn_AggrInfoU _tmp33C=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1F2=_cycalloc(sizeof(*_tmp1F2));*_tmp1F2=_tmp1F8;_tmp1F2;}));(*_tmp1F3).aggr_info=_tmp33C;}),(*_tmp1F3).targs=0);_tmp1F3;});(*_tmp1F4).f1=_tmp33D;}))),(*_tmp1F4).f2=0)),({
struct Cyc_List_List*_tmp33B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1EF);(*_tmp1F4).f3=_tmp33B;}))),(*_tmp1F4).f4=_tmp20D);_tmp1F4;});
# 861
p->r=_tmp33E;});
# 863
_npop_handler(0U);return;}case 2U: _LL41: _tmp1FA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1EE)->f1;_tmp1F9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1EE)->f2;_LL42:
# 865
({void*_tmp33F=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));((((((((*_tmp1F5).tag=8U,(*_tmp1F5).f1=_tmp1FA)),(*_tmp1F5).f2=_tmp1F9)),(*_tmp1F5).f3=_tmp20E)),(*_tmp1F5).f4=_tmp20D);_tmp1F5;});p->r=_tmp33F;});_npop_handler(0U);return;case 3U: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 868
({void*_tmp1F6=0U;({unsigned int _tmp341=_tmp221;struct _dyneither_ptr _tmp340=({const char*_tmp1F7="enum tag used with arguments in pattern";_tag_dyneither(_tmp1F7,sizeof(char),40U);});Cyc_Warn_err(_tmp341,_tmp340,_tag_dyneither(_tmp1F6,sizeof(void*),0U));});});_npop_handler(0U);return;}_LL3C:;}
# 855
;_pop_handler();}else{void*_tmp1EB=(void*)_exn_thrown;void*_tmp1FB=_tmp1EB;void*_tmp1FC;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1FB)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 869
 goto _LL47;}else{_LL4A: _tmp1FC=_tmp1FB;_LL4B:(int)_rethrow(_tmp1FC);}_LL47:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0U;({struct _dyneither_ptr _tmp342=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp20F));_tmp1FF.f1=_tmp342;});({void*_tmp1FD[1U];_tmp1FD[0]=& _tmp1FF;({unsigned int _tmp344=_tmp221;struct _dyneither_ptr _tmp343=({const char*_tmp1FE="%s is not a constructor in pattern";_tag_dyneither(_tmp1FE,sizeof(char),35U);});Cyc_Warn_err(_tmp344,_tmp343,_tag_dyneither(_tmp1FD,sizeof(void*),1U));});});});
return;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1)->aggr_info).UnknownAggr).tag == 1){_LL8: _tmp214=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1)->aggr_info).UnknownAggr).val).f2;_tmp213=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1)->targs;_tmp212=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_tmp211=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_tmp210=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f4;_LL9:
# 874
{struct Cyc_List_List*_tmp200=_tmp211;for(0;_tmp200 != 0;_tmp200=_tmp200->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp200->hd)).f2);}}
{struct _handler_cons _tmp201;_push_handler(& _tmp201);{int _tmp203=0;if(setjmp(_tmp201.handler))_tmp203=1;if(!_tmp203){
{struct Cyc_Absyn_Aggrdecl*_tmp204=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp221,env->ns,_tmp214,Cyc_Binding_lookup_aggrdecl);
({void*_tmp347=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp207=_cycalloc(sizeof(*_tmp207));((((((((*_tmp207).tag=7U,({struct Cyc_Absyn_AggrInfo*_tmp346=({struct Cyc_Absyn_AggrInfo*_tmp206=_cycalloc(sizeof(*_tmp206));(({union Cyc_Absyn_AggrInfoU _tmp345=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp205=_cycalloc(sizeof(*_tmp205));*_tmp205=_tmp204;_tmp205;}));(*_tmp206).aggr_info=_tmp345;}),(*_tmp206).targs=_tmp213);_tmp206;});(*_tmp207).f1=_tmp346;}))),(*_tmp207).f2=_tmp212)),(*_tmp207).f3=_tmp211)),(*_tmp207).f4=_tmp210);_tmp207;});p->r=_tmp347;});}
# 877
;_pop_handler();}else{void*_tmp202=(void*)_exn_thrown;void*_tmp208=_tmp202;void*_tmp20B;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp208)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 881
({void*_tmp209=0U;({unsigned int _tmp349=_tmp221;struct _dyneither_ptr _tmp348=({const char*_tmp20A="non-aggregate name has designator patterns";_tag_dyneither(_tmp20A,sizeof(char),43U);});Cyc_Warn_err(_tmp349,_tmp348,_tag_dyneither(_tmp209,sizeof(void*),0U));});});
return;}else{_LL4F: _tmp20B=_tmp208;_LL50:(int)_rethrow(_tmp20B);}_LL4C:;}};}
# 884
return;}else{_LL24: _tmp215=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_LL25:
# 913
 for(0;_tmp215 != 0;_tmp215=_tmp215->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp215->hd)).f2);}
return;}}else{_LL22: _tmp216=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_LL23:
# 911
 _tmp215=_tmp216;goto _LL25;}case 0U: _LLA: _LLB:
# 886
 return;case 3U: _LLC: _tmp218=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp217=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LLD:
# 888
 _tmp21A=_tmp218;_tmp219=_tmp217;goto _LLF;case 1U: _LLE: _tmp21A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp219=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LLF:
# 890
 Cyc_Binding_resolve_pat(env,_tmp219);
_tmp21B=_tmp21A;goto _LL11;case 4U: _LL10: _tmp21B=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL11:
 _tmp21C=_tmp21B;goto _LL13;case 2U: _LL12: _tmp21C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL13:
# 894
 Cyc_Binding_resolve_and_add_vardecl(_tmp221,env,_tmp21C);goto _LL3;case 6U: _LL14: _tmp21D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL15:
# 896
 Cyc_Binding_resolve_pat(env,_tmp21D);return;case 5U: _LL16: _tmp21E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL17:
# 898
 for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp21E->hd);}
goto _LL3;case 17U: _LL18: _tmp21F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL19:
# 902
 Cyc_Binding_resolve_exp(env,_tmp21F);return;case 9U: _LL1A: _LL1B:
# 904
 goto _LL1D;case 10U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
 return;case 8U: _LL26: _tmp220=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_LL27:
# 917
 for(0;_tmp220 != 0;_tmp220=_tmp220->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp220->hd);}
return;case 13U: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;};}struct _tuple14{void*f1;void*f2;};
# 925
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}struct _tuple15{unsigned int f1;struct _tuple0*f2;int f3;};
# 939 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 942
unsigned int loc=d->loc;
void*_tmp223=d->r;void*_tmp224=_tmp223;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp298;struct _tuple0*_tmp297;struct Cyc_List_List*_tmp296;struct _dyneither_ptr*_tmp295;struct Cyc_List_List*_tmp294;struct Cyc_Absyn_Datatypedecl*_tmp293;struct Cyc_Absyn_Enumdecl*_tmp292;struct Cyc_Absyn_Aggrdecl*_tmp291;struct Cyc_Absyn_Typedefdecl*_tmp290;struct Cyc_Absyn_Tvar*_tmp28F;struct Cyc_Absyn_Vardecl*_tmp28E;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_List_List*_tmp28C;struct Cyc_Absyn_Pat*_tmp28B;struct Cyc_Absyn_Exp*_tmp28A;struct Cyc_Absyn_Fndecl*_tmp289;struct Cyc_Absyn_Vardecl*_tmp288;switch(*((int*)_tmp224)){case 0U: _LL1: _tmp288=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LL2:
# 946
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp288);
# 951
if(_tmp288->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp288->initializer));
goto _LL0;case 1U: _LL3: _tmp289=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LL4:
# 956
 Cyc_Binding_absolutize_decl(loc,env,_tmp289->name,_tmp289->sc);{
struct _tuple0*_tmp225=_tmp289->name;struct _tuple0*_tmp226=_tmp225;union Cyc_Absyn_Nmspace _tmp237;struct _dyneither_ptr*_tmp236;_LL20: _tmp237=_tmp226->f1;_tmp236=_tmp226->f2;_LL21:;{
# 959
struct Cyc_List_List*_tmp227=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fndecl,1,loc,env,_tmp289->args,_tmp289->cyc_varargs);
# 961
({struct Cyc_Core_Opt*_tmp34A=({struct Cyc_Core_Opt*_tmp228=_cycalloc(sizeof(*_tmp228));(*_tmp228).v=_tmp227;_tmp228;});_tmp289->param_vardecls=_tmp34A;});
# 963
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,_tmp289->name);{
# 966
struct Cyc_Dict_Dict*_tmp229=env->local_vars;
if(_tmp229 != 0)
({struct Cyc_Dict_Dict*_tmp34B=({struct Cyc_Dict_Dict*_tmp22A=_cycalloc(sizeof(*_tmp22A));*_tmp22A=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp22A;});env->local_vars=_tmp34B;});else{
# 970
({struct Cyc_Dict_Dict*_tmp34D=({struct Cyc_Dict_Dict*_tmp22B=_cycalloc(sizeof(*_tmp22B));({struct Cyc_Dict_Dict _tmp34C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp22B=_tmp34C;});_tmp22B;});env->local_vars=_tmp34D;});}
{struct Cyc_List_List*_tmp22C=_tmp227;for(0;_tmp22C != 0;_tmp22C=_tmp22C->tl){
({struct Cyc_Dict_Dict _tmp351=({struct Cyc_Dict_Dict _tmp350=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp34F=(*((struct Cyc_Absyn_Vardecl*)_tmp22C->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp350,_tmp34F,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp22E=_cycalloc(sizeof(*_tmp22E));
((*_tmp22E).tag=0U,({void*_tmp34E=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));((*_tmp22D).tag=3U,(*_tmp22D).f1=(struct Cyc_Absyn_Vardecl*)_tmp22C->hd);_tmp22D;});(*_tmp22E).f1=_tmp34E;}));_tmp22E;}));});
# 972
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp351;});}}
# 975
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp289->effect,_tmp289->ret_type,_tmp289->args,_tmp289->cyc_varargs,_tmp289->rgn_po,_tmp289->requires_clause,_tmp289->ensures_clause);
# 979
if(_tmp229 != 0){
# 981
({struct Cyc_Dict_Dict _tmp355=({struct Cyc_Dict_Dict _tmp354=*_tmp229;struct _dyneither_ptr*_tmp353=_tmp236;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp354,_tmp353,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp230=_cycalloc(sizeof(*_tmp230));
((*_tmp230).tag=0U,({void*_tmp352=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));((*_tmp22F).tag=2U,(*_tmp22F).f1=_tmp289);_tmp22F;});(*_tmp230).f1=_tmp352;}));_tmp230;}));});
# 981
*_tmp229=_tmp355;});
# 983
({struct Cyc_Dict_Dict _tmp359=({struct Cyc_Dict_Dict _tmp358=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp357=_tmp236;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp358,_tmp357,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp232=_cycalloc(sizeof(*_tmp232));
((*_tmp232).tag=0U,({void*_tmp356=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp231=_cycalloc(sizeof(*_tmp231));((*_tmp231).tag=2U,(*_tmp231).f1=_tmp289);_tmp231;});(*_tmp232).f1=_tmp356;}));_tmp232;}));});
# 983
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp359;});}else{
# 986
struct Cyc_Binding_ResolveNSEnv*_tmp233=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp237);
({struct Cyc_Dict_Dict _tmp35D=({struct Cyc_Dict_Dict _tmp35C=_tmp233->ordinaries;struct _dyneither_ptr*_tmp35B=_tmp236;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp35C,_tmp35B,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp235=_cycalloc(sizeof(*_tmp235));
# 989
((*_tmp235).tag=0U,({void*_tmp35A=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp234=_cycalloc(sizeof(*_tmp234));((*_tmp234).tag=2U,(*_tmp234).f1=_tmp289);_tmp234;});(*_tmp235).f1=_tmp35A;}));_tmp235;}));});
# 987
_tmp233->ordinaries=_tmp35D;});}
# 992
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp289->body);
# 995
env->local_vars=_tmp229;
goto _LL0;};};};case 2U: _LL5: _tmp28B=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_tmp28A=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp224)->f3;_LL6:
# 999
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp238=0U;({unsigned int _tmp35F=loc;struct _dyneither_ptr _tmp35E=({const char*_tmp239="let not allowed at top-level";_tag_dyneither(_tmp239,sizeof(char),29U);});Cyc_Warn_err(_tmp35F,_tmp35E,_tag_dyneither(_tmp238,sizeof(void*),0U));});});
goto _LL0;}
# 1003
Cyc_Binding_resolve_exp(env,_tmp28A);
Cyc_Binding_resolve_pat(env,_tmp28B);
goto _LL0;case 3U: _LL7: _tmp28C=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LL8:
# 1008
 for(0;_tmp28C != 0;_tmp28C=_tmp28C->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp28C->hd);}
goto _LL0;case 4U: _LL9: _tmp28F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_tmp28E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp224)->f2;_tmp28D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp224)->f3;_LLA:
# 1013
 if(_tmp28D != 0)
Cyc_Binding_resolve_exp(env,_tmp28D);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp28E);
goto _LL0;case 8U: _LLB: _tmp290=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LLC:
# 1019
 Cyc_Binding_absolutize_decl(loc,env,_tmp290->name,_tmp290->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp23A=_tmp290->name;struct _tuple0*_tmp23B=_tmp23A;union Cyc_Absyn_Nmspace _tmp23E;struct _dyneither_ptr*_tmp23D;_LL23: _tmp23E=_tmp23B->f1;_tmp23D=_tmp23B->f2;_LL24:;{
# 1022
struct Cyc_Binding_ResolveNSEnv*_tmp23C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp23E);
if(_tmp290->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp290->defn));
# 1026
({struct Cyc_Dict_Dict _tmp360=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp23C->typedefs,_tmp23D,_tmp290);_tmp23C->typedefs=_tmp360;});
# 1028
goto _LL0;};};case 5U: _LLD: _tmp291=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LLE:
# 1031
 Cyc_Binding_absolutize_decl(loc,env,_tmp291->name,_tmp291->sc);{
struct _tuple0*_tmp23F=_tmp291->name;struct _tuple0*_tmp240=_tmp23F;union Cyc_Absyn_Nmspace _tmp248;struct _dyneither_ptr*_tmp247;_LL26: _tmp248=_tmp240->f1;_tmp247=_tmp240->f2;_LL27:;{
# 1034
struct Cyc_Binding_ResolveNSEnv*_tmp241=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp248);
# 1036
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp241->aggrdecls,_tmp247) && _tmp291->impl == 0)
goto _LL0;
# 1039
({struct Cyc_Dict_Dict _tmp361=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp241->aggrdecls,_tmp247,_tmp291);_tmp241->aggrdecls=_tmp361;});
# 1041
({struct Cyc_Dict_Dict _tmp364=({struct Cyc_Dict_Dict _tmp363=_tmp241->ordinaries;struct _dyneither_ptr*_tmp362=_tmp247;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp363,_tmp362,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp242=_cycalloc(sizeof(*_tmp242));
((*_tmp242).tag=1U,(*_tmp242).f1=_tmp291);_tmp242;}));});
# 1041
_tmp241->ordinaries=_tmp364;});
# 1043
if(_tmp291->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp243=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp291->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp244=_tmp243;struct Cyc_List_List*_tmp246;struct Cyc_List_List*_tmp245;_LL29: _tmp246=_tmp244->rgn_po;_tmp245=_tmp244->fields;_LL2A:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp246);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp245);}
# 1048
goto _LL0;};};case 7U: _LLF: _tmp292=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LL10:
# 1051
 Cyc_Binding_absolutize_decl(loc,env,_tmp292->name,_tmp292->sc);{
struct _tuple0*_tmp249=_tmp292->name;struct _tuple0*_tmp24A=_tmp249;union Cyc_Absyn_Nmspace _tmp24F;struct _dyneither_ptr*_tmp24E;_LL2C: _tmp24F=_tmp24A->f1;_tmp24E=_tmp24A->f2;_LL2D:;{
# 1054
struct Cyc_Binding_ResolveNSEnv*_tmp24B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp24F);
# 1056
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp24B->enumdecls,_tmp24E) && _tmp292->fields == 0)
goto _LL0;
# 1060
({struct Cyc_Dict_Dict _tmp365=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp24B->enumdecls,_tmp24E,_tmp292);_tmp24B->enumdecls=_tmp365;});
# 1062
if(_tmp292->fields != 0){
# 1064
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp292->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp24C=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp24C->loc,env,_tmp24C->name,_tmp292->sc);
if(_tmp24C->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp24C->tag));
({struct Cyc_Dict_Dict _tmp368=({struct Cyc_Dict_Dict _tmp367=_tmp24B->ordinaries;struct _dyneither_ptr*_tmp366=(*_tmp24C->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp367,_tmp366,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));
# 1071
((((*_tmp24D).tag=3U,(*_tmp24D).f1=_tmp292)),(*_tmp24D).f2=_tmp24C);_tmp24D;}));});
# 1069
_tmp24B->ordinaries=_tmp368;});}}
# 1073
goto _LL0;};};case 6U: _LL11: _tmp293=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LL12:
# 1077
{struct _handler_cons _tmp250;_push_handler(& _tmp250);{int _tmp252=0;if(setjmp(_tmp250.handler))_tmp252=1;if(!_tmp252){
{struct Cyc_List_List*_tmp253=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp293->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp254=(struct Cyc_Absyn_Datatypedecl*)_tmp253->hd;
if(!_tmp254->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp255=_cycalloc(sizeof(*_tmp255));(*_tmp255).tag=Cyc_Binding_BindingError;_tmp255;}));
_tmp293->name=_tmp254->name;
_tmp293->is_extensible=1;{
struct _tuple0*_tmp256=_tmp293->name;struct _tuple0*_tmp257=_tmp256;union Cyc_Absyn_Nmspace _tmp25B;struct _dyneither_ptr*_tmp25A;_LL2F: _tmp25B=_tmp257->f1;_tmp25A=_tmp257->f2;_LL30:;{
struct Cyc_Binding_ResolveNSEnv*_tmp258=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp25B);
({struct Cyc_Dict_Dict _tmp36B=({struct Cyc_Dict_Dict _tmp36A=_tmp258->datatypedecls;struct _dyneither_ptr*_tmp369=_tmp25A;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp36A,_tmp369,({struct Cyc_List_List*_tmp259=_cycalloc(sizeof(*_tmp259));
((*_tmp259).hd=_tmp293,(*_tmp259).tl=_tmp253);_tmp259;}));});
# 1086
_tmp258->datatypedecls=_tmp36B;});};};}
# 1078
;_pop_handler();}else{void*_tmp251=(void*)_exn_thrown;void*_tmp25C=_tmp251;void*_tmp263;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp25C)->tag == Cyc_Binding_BindingError){_LL32: _LL33:
# 1090
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp293->name,_tmp293->sc);{
struct _tuple0*_tmp25D=_tmp293->name;struct _tuple0*_tmp25E=_tmp25D;union Cyc_Absyn_Nmspace _tmp262;struct _dyneither_ptr*_tmp261;_LL37: _tmp262=_tmp25E->f1;_tmp261=_tmp25E->f2;_LL38:;{
struct Cyc_Binding_ResolveNSEnv*_tmp25F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp262);
# 1094
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp25F->datatypedecls,_tmp261) && _tmp293->fields == 0)
# 1096
goto _LL31;
({struct Cyc_Dict_Dict _tmp36E=({struct Cyc_Dict_Dict _tmp36D=_tmp25F->datatypedecls;struct _dyneither_ptr*_tmp36C=_tmp261;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp36D,_tmp36C,({struct Cyc_List_List*_tmp260=_cycalloc(sizeof(*_tmp260));
((*_tmp260).hd=_tmp293,(*_tmp260).tl=0);_tmp260;}));});
# 1097
_tmp25F->datatypedecls=_tmp36E;});
# 1099
goto _LL31;};};}else{_LL34: _tmp263=_tmp25C;_LL35:(int)_rethrow(_tmp263);}_LL31:;}};}{
# 1102
struct _tuple0*_tmp264=_tmp293->name;struct _tuple0*_tmp265=_tmp264;union Cyc_Absyn_Nmspace _tmp272;struct _dyneither_ptr*_tmp271;_LL3A: _tmp272=_tmp265->f1;_tmp271=_tmp265->f2;_LL3B:;
if(_tmp293->fields != 0){
struct Cyc_List_List*_tmp266=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp293->fields))->v;for(0;_tmp266 != 0;_tmp266=_tmp266->tl){
struct Cyc_Absyn_Datatypefield*_tmp267=(struct Cyc_Absyn_Datatypefield*)_tmp266->hd;
{struct Cyc_List_List*_tmp268=_tmp267->typs;for(0;_tmp268 != 0;_tmp268=_tmp268->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp268->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp269=(*_tmp267->name).f1;union Cyc_Absyn_Nmspace _tmp26A=_tmp269;switch((_tmp26A.Abs_n).tag){case 1U: if((_tmp26A.Rel_n).val == 0){_LL3D: _LL3E:
# 1110
 if(_tmp293->is_extensible)
({union Cyc_Absyn_Nmspace _tmp36F=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*_tmp267->name).f1=_tmp36F;});else{
# 1113
(*_tmp267->name).f1=(*_tmp293->name).f1;}
goto _LL3C;}else{_LL3F: _LL40:
# 1116
({void*_tmp26B=0U;({unsigned int _tmp371=_tmp267->loc;struct _dyneither_ptr _tmp370=({const char*_tmp26C="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp26C,sizeof(char),56U);});Cyc_Warn_err(_tmp371,_tmp370,_tag_dyneither(_tmp26B,sizeof(void*),0U));});});
return;}case 2U: _LL41: _LL42:
 goto _LL3C;default: _LL43: _LL44:
({void*_tmp26D=0U;({struct _dyneither_ptr _tmp372=({const char*_tmp26E="datatype field Loc_n or C_n";_tag_dyneither(_tmp26E,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp372,_tag_dyneither(_tmp26D,sizeof(void*),0U));});});}_LL3C:;}{
# 1121
struct Cyc_Binding_ResolveNSEnv*_tmp26F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp267->name).f1);
({struct Cyc_Dict_Dict _tmp375=({struct Cyc_Dict_Dict _tmp374=_tmp26F->ordinaries;struct _dyneither_ptr*_tmp373=(*_tmp267->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp374,_tmp373,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp270=_cycalloc(sizeof(*_tmp270));
# 1124
((((*_tmp270).tag=2U,(*_tmp270).f1=_tmp293)),(*_tmp270).f2=_tmp267);_tmp270;}));});
# 1122
_tmp26F->ordinaries=_tmp375;});};}}
# 1126
goto _LL0;};case 9U: _LL13: _tmp295=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_tmp294=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp224)->f2;_LL14:
# 1129
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp295,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp294);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;case 10U: _LL15: _tmp297=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_tmp296=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp224)->f2;_LL16:
# 1134
((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp297);
Cyc_Binding_resolve_decls(env,_tmp296);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL0;case 11U: _LL17: _tmp298=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_LL18: {
# 1140
int _tmp273=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp298);
env->in_cinclude=_tmp273;
goto _LL0;}case 12U: _LL19: _tmp29A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp224)->f1;_tmp299=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp224)->f2;_LL1A:
# 1151
{struct Cyc_List_List*_tmp274=_tmp299;for(0;_tmp274 != 0;_tmp274=_tmp274->tl){
struct _tuple15*_tmp275=(struct _tuple15*)_tmp274->hd;struct _tuple15*_tmp276=_tmp275;unsigned int _tmp278;struct _tuple0*_tmp277;_LL46: _tmp278=_tmp276->f1;_tmp277=_tmp276->f2;_LL47:;
Cyc_Binding_absolutize_decl(_tmp278,env,_tmp277,Cyc_Absyn_ExternC);}}
# 1159
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp279=0U;({unsigned int _tmp377=loc;struct _dyneither_ptr _tmp376=({const char*_tmp27A="extern \"C include\" not at toplevel";_tag_dyneither(_tmp27A,sizeof(char),35U);});Cyc_Warn_err(_tmp377,_tmp376,_tag_dyneither(_tmp279,sizeof(void*),0U));});});
goto _LL0;}{
# 1164
struct Cyc_Binding_ResolveNSEnv*_tmp27B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp27C=_tmp27B->ordinaries;
int _tmp27D=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp29A);{
struct Cyc_Dict_Dict _tmp27E=_tmp27B->ordinaries;
struct Cyc_Dict_Dict _tmp27F=_tmp27C;
{struct Cyc_List_List*_tmp280=_tmp299;for(0;_tmp280 != 0;_tmp280=_tmp280->tl){
struct _tuple15*_tmp281=(struct _tuple15*)_tmp280->hd;struct _tuple15*_tmp282=_tmp281;unsigned int _tmp287;struct _dyneither_ptr*_tmp286;_LL49: _tmp287=_tmp282->f1;_tmp286=(_tmp282->f2)->f2;_LL4A:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27E,_tmp286) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27C,_tmp286) && ({
void*_tmp378=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27C,_tmp286);_tmp378 == ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27E,_tmp286);}))
({struct Cyc_String_pa_PrintArg_struct _tmp285;_tmp285.tag=0U;_tmp285.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp286);({void*_tmp283[1U];_tmp283[0]=& _tmp285;({unsigned int _tmp37A=_tmp287;struct _dyneither_ptr _tmp379=({const char*_tmp284="%s is exported but not defined";_tag_dyneither(_tmp284,sizeof(char),31U);});Cyc_Warn_err(_tmp37A,_tmp379,_tag_dyneither(_tmp283,sizeof(void*),1U));});});});
({struct Cyc_Dict_Dict _tmp37D=({struct Cyc_Dict_Dict _tmp37C=_tmp27F;struct _dyneither_ptr*_tmp37B=_tmp286;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp37C,_tmp37B,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27E,_tmp286));});_tmp27F=_tmp37D;});}}
# 1179
_tmp27B->ordinaries=_tmp27F;
env->in_cinclude=_tmp27D;
goto _LL0;};};case 13U: _LL1B: _LL1C:
# 1183
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 1188
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1193
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp29B=({struct Cyc_Binding_Env*_tmp29E=_cycalloc(sizeof(*_tmp29E));((((*_tmp29E).in_cinclude=0,({struct Cyc_Binding_NSCtxt*_tmp37E=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);(*_tmp29E).ns=_tmp37E;}))),(*_tmp29E).local_vars=0);_tmp29E;});
({struct Cyc_Binding_Env*_tmp380=_tmp29B;Cyc_Binding_resolve_decl(_tmp380,({struct Cyc_Absyn_Decl*_tmp29D=_cycalloc(sizeof(*_tmp29D));(({void*_tmp37F=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));((*_tmp29C).tag=6U,(*_tmp29C).f1=Cyc_Absyn_exn_tud);_tmp29C;});(*_tmp29D).r=_tmp37F;}),(*_tmp29D).loc=0U);_tmp29D;}));});
Cyc_Binding_resolve_decls(_tmp29B,tds);}

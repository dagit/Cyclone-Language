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

# 69 "core.h"
 void exit(int);
# 38 "cycboot.h"
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 310
int toupper(int);
# 318
int system(const char*);
# 321
int mkdir(const char*pathname,unsigned short mode);
# 324
int close(int);
int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 265 "core.h"
void Cyc_Core_rethrow(void*);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 80 "lexing.h"
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 103 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 108
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
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
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1104 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1175
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};struct Cyc_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;};struct _tuple13{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple13 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0U,Cyc_Extern_sc  = 1U,Cyc_ExternC_sc  = 2U,Cyc_Static_sc  = 3U,Cyc_Auto_sc  = 4U,Cyc_Register_sc  = 5U,Cyc_Abstract_sc  = 6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple14{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple14 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple15{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple16{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple13 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple18{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple19 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple9*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 66 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 33 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*t);
void Cyc_Toc_finish();
# 147 "tcutil.h"
int Cyc_Tcutil_unify(void*,void*);
# 30 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple1*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 29 "specsfile.h"
struct _dyneither_ptr Cyc_Specsfile_target_arch;
struct _dyneither_ptr Cyc_Specsfile_btarget;
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s);
# 35
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file);
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline);
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path;
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv);
# 45
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s);
# 78 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4U]="gcc";
# 88
static struct _dyneither_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4U};static char _tmp1[1U]="";
static struct _dyneither_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1U};
# 91
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
int Cyc_noexpand_r=1;
# 95
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 99
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 102
if(Cyc_log_file == 0){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp48A=Cyc_stderr;struct _dyneither_ptr _tmp489=({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp48A,_tmp489,_tag_dyneither(_tmp2,sizeof(void*),0U));});});
 exit(1);}{
# 106
int _tmp4=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp4;};}
# 111
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
({struct Cyc_Set_Set**_tmp48C=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp48B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);_tmp5[0]=_tmp48B;});_tmp5;});Cyc_current_targets=_tmp48C;});}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 119
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 125
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 133
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple23{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct _tuple24{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct _tuple25{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};
# 148
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_user_defs=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
struct _dyneither_ptr Cyc_current_symbol={(void*)0,(void*)0,(void*)(0 + 0)};
int Cyc_rename_current_symbol=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 169
int Cyc_numdef=0;
# 171
static struct Cyc_List_List*Cyc_cppargs=0;static char _tmp6[14U]="BUILDLIB_sym_";
# 173
struct _dyneither_ptr Cyc_user_prefix={_tmp6,_tmp6,_tmp6 + 14U};
static struct _dyneither_ptr*Cyc_add_user_prefix(struct _dyneither_ptr*symbol){
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_user_prefix,(struct _dyneither_ptr)*symbol);
return({struct _dyneither_ptr*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=s;_tmp7;});}
# 178
static struct _dyneither_ptr Cyc_remove_user_prefix(struct _dyneither_ptr symbol){
unsigned int prefix_len=Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix);
if(Cyc_strncmp((struct _dyneither_ptr)symbol,(struct _dyneither_ptr)Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0U;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)symbol);({void*_tmp8[1U]={& _tmpA};({struct Cyc___cycFILE*_tmp48E=Cyc_stderr;struct _dyneither_ptr _tmp48D=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_dyneither(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp48E,_tmp48D,_tag_dyneither(_tmp8,sizeof(void*),1U));});});});
return symbol;}
# 184
return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp491=(struct _dyneither_ptr)symbol;int _tmp490=(int)prefix_len;Cyc_substring(_tmp491,_tmp490,({unsigned long _tmp48F=Cyc_strlen((struct _dyneither_ptr)symbol);_tmp48F - prefix_len;}));});}
# 187
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_tmpC=_tmpB;struct Cyc_Absyn_Typedefdecl*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp12;switch(*((int*)_tmpC)){case 5U: _LL1: _tmp12=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2:
# 190
({struct _dyneither_ptr*_tmp493=({struct _dyneither_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));({struct _dyneither_ptr _tmp492=Cyc_remove_user_prefix(*(*_tmp12->name).f2);_tmpD[0]=_tmp492;});_tmpD;});(*_tmp12->name).f2=_tmp493;});
goto _LL0;case 7U: _LL3: _tmp13=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4:
# 193
({struct _dyneither_ptr*_tmp495=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _dyneither_ptr _tmp494=Cyc_remove_user_prefix(*(*_tmp13->name).f2);_tmpE[0]=_tmp494;});_tmpE;});(*_tmp13->name).f2=_tmp495;});
goto _LL0;case 8U: _LL5: _tmp14=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6:
# 196
({struct _dyneither_ptr*_tmp497=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _dyneither_ptr _tmp496=Cyc_remove_user_prefix(*(*_tmp14->name).f2);_tmpF[0]=_tmp496;});_tmpF;});(*_tmp14->name).f2=_tmp497;});
goto _LL0;case 1U: _LL7: _LL8:
# 199
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 4U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 3U: _LL11: _LL12:
 goto _LL14;case 9U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 211
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp499=Cyc_stderr;struct _dyneither_ptr _tmp498=({const char*_tmp11="Error in .cys file: bad user-defined type definition\n";_tag_dyneither(_tmp11,sizeof(char),54U);});Cyc_fprintf(_tmp499,_tmp498,_tag_dyneither(_tmp10,sizeof(void*),0U));});});
 exit(1);}_LL0:;}
# 217
static struct Cyc_Absyn_Decl*Cyc_named_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*decl_name){
struct _dyneither_ptr*name;
for(0;decls != 0;decls=decls->tl){
{void*_tmp15=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_tmp16=_tmp15;struct Cyc_Absyn_Typedefdecl*_tmp22;struct Cyc_Absyn_Enumdecl*_tmp21;struct Cyc_Absyn_Aggrdecl*_tmp20;switch(*((int*)_tmp16)){case 5U: _LL1: _tmp20=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL2: {
# 222
struct _tuple1*_tmp17=_tmp20->name;struct _tuple1*_tmp18=_tmp17;struct _dyneither_ptr*_tmp19;_LL20: _tmp19=_tmp18->f2;_LL21:;
name=_tmp19;
goto _LL0;}case 7U: _LL3: _tmp21=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL4: {
# 226
struct _tuple1*_tmp1A=_tmp21->name;struct _tuple1*_tmp1B=_tmp1A;struct _dyneither_ptr*_tmp1C;_LL23: _tmp1C=_tmp1B->f2;_LL24:;
name=_tmp1C;
goto _LL0;}case 8U: _LL5: _tmp22=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL6: {
# 230
struct _tuple1*_tmp1D=_tmp22->name;struct _tuple1*_tmp1E=_tmp1D;struct _dyneither_ptr*_tmp1F;_LL26: _tmp1F=_tmp1E->f2;_LL27:;
name=_tmp1F;
goto _LL0;}case 0U: _LL7: _LL8:
# 234
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LLC;case 13U: _LLB: _LLC:
 goto _LLE;case 14U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 6U: _LL11: _LL12:
 goto _LL14;case 3U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 10U: _LL17: _LL18:
 goto _LL1A;case 11U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 246
 name=0;
goto _LL0;}_LL0:;}
# 249
if((unsigned int)name  && !Cyc_strptrcmp(name,decl_name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 252
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Core_Impossible_exn_struct _tmp49B=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp49A=({const char*_tmp25="Internal Error: bad user-def name";_tag_dyneither(_tmp25,sizeof(char),34U);});_tmp24.f1=_tmp49A;});_tmp24;});_tmp23[0]=_tmp49B;});_tmp23;}));}struct _tuple26{void*f1;void*f2;};struct _tuple27{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 259
static void Cyc_merge_rep_decl(struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
{struct _tuple26 _tmp26=({struct _tuple26 _tmpA2;_tmpA2.f1=c_decl->r;_tmpA2.f2=user_decl->r;_tmpA2;});struct _tuple26 _tmp27=_tmp26;struct Cyc_Absyn_Typedefdecl*_tmpA1;struct Cyc_Absyn_Typedefdecl*_tmpA0;struct Cyc_Absyn_Enumdecl*_tmp9F;struct Cyc_Absyn_Enumdecl*_tmp9E;struct Cyc_Absyn_Aggrdecl*_tmp9D;struct Cyc_Absyn_Aggrdecl*_tmp9C;switch(*((int*)_tmp27.f1)){case 5U: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 5U){_LL1: _tmp9D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmp9C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL2:
# 265
 if(_tmp9C->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0U;({struct _dyneither_ptr _tmp49C=(struct _dyneither_ptr)({const char*_tmp2C=": universal quantification in user definition not allowed";_tag_dyneither(_tmp2C,sizeof(char),58U);});_tmp2B.f1=_tmp49C;});({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0U;({struct _dyneither_ptr _tmp49D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp2A.f1=_tmp49D;});({void*_tmp28[2U]={& _tmp2A,& _tmp2B};({struct Cyc___cycFILE*_tmp49F=Cyc_stderr;struct _dyneither_ptr _tmp49E=({const char*_tmp29="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp29,sizeof(char),62U);});Cyc_fprintf(_tmp49F,_tmp49E,_tag_dyneither(_tmp28,sizeof(void*),2U));});});});});return;}
if(_tmp9C->sc != _tmp9D->sc)
({struct Cyc_String_pa_PrintArg_struct _tmp30;_tmp30.tag=0U;({struct _dyneither_ptr _tmp4A0=(struct _dyneither_ptr)({const char*_tmp31=": scopes don't match";_tag_dyneither(_tmp31,sizeof(char),21U);});_tmp30.f1=_tmp4A0;});({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0U;({struct _dyneither_ptr _tmp4A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp2F.f1=_tmp4A1;});({void*_tmp2D[2U]={& _tmp2F,& _tmp30};({struct Cyc___cycFILE*_tmp4A3=Cyc_stderr;struct _dyneither_ptr _tmp4A2=({const char*_tmp2E="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp2E,sizeof(char),71U);});Cyc_fprintf(_tmp4A3,_tmp4A2,_tag_dyneither(_tmp2D,sizeof(void*),2U));});});});});
# 270
if(_tmp9C->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0U;({struct _dyneither_ptr _tmp4A4=(struct _dyneither_ptr)({const char*_tmp36=": no user definition";_tag_dyneither(_tmp36,sizeof(char),21U);});_tmp35.f1=_tmp4A4;});({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0U;({struct _dyneither_ptr _tmp4A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp34.f1=_tmp4A5;});({void*_tmp32[2U]={& _tmp34,& _tmp35};({struct Cyc___cycFILE*_tmp4A7=Cyc_stderr;struct _dyneither_ptr _tmp4A6=({const char*_tmp33="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp33,sizeof(char),62U);});Cyc_fprintf(_tmp4A7,_tmp4A6,_tag_dyneither(_tmp32,sizeof(void*),2U));});});});});return;}
if(_tmp9D->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3A;_tmp3A.tag=0U;({struct _dyneither_ptr _tmp4A8=(struct _dyneither_ptr)({const char*_tmp3B=": no definition for system version";_tag_dyneither(_tmp3B,sizeof(char),35U);});_tmp3A.f1=_tmp4A8;});({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0U;({struct _dyneither_ptr _tmp4A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp39.f1=_tmp4A9;});({void*_tmp37[2U]={& _tmp39,& _tmp3A};({struct Cyc___cycFILE*_tmp4AB=Cyc_stderr;struct _dyneither_ptr _tmp4AA=({const char*_tmp38="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp38,sizeof(char),71U);});Cyc_fprintf(_tmp4AB,_tmp4AA,_tag_dyneither(_tmp37,sizeof(void*),2U));});});});});
c_decl->r=user_decl->r;
return;}
# 277
{struct _tuple27 _tmp3C=({struct _tuple27 _tmp79;_tmp79.f1=_tmp9D->impl;_tmp79.f2=_tmp9C->impl;_tmp79;});struct _tuple27 _tmp3D=_tmp3C;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;int _tmp74;if(_tmp3D.f1 != 0){if(_tmp3D.f2 != 0){_LLA: _tmp78=(_tmp3D.f1)->fields;_tmp77=(_tmp3D.f2)->exist_vars;_tmp76=(_tmp3D.f2)->rgn_po;_tmp75=(_tmp3D.f2)->fields;_tmp74=(_tmp3D.f2)->tagged;_LLB:
# 279
 if(_tmp74){
({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0U;({struct _dyneither_ptr _tmp4AC=(struct _dyneither_ptr)({const char*_tmp42=": user @tagged annotation not allowed";_tag_dyneither(_tmp42,sizeof(char),38U);});_tmp41.f1=_tmp4AC;});({struct Cyc_String_pa_PrintArg_struct _tmp40;_tmp40.tag=0U;({struct _dyneither_ptr _tmp4AD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp40.f1=_tmp4AD;});({void*_tmp3E[2U]={& _tmp40,& _tmp41};({struct Cyc___cycFILE*_tmp4AF=Cyc_stderr;struct _dyneither_ptr _tmp4AE=({const char*_tmp3F="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp3F,sizeof(char),62U);});Cyc_fprintf(_tmp4AF,_tmp4AE,_tag_dyneither(_tmp3E,sizeof(void*),2U));});});});});return;}
if((unsigned int)_tmp77){
({struct Cyc_String_pa_PrintArg_struct _tmp46;_tmp46.tag=0U;({struct _dyneither_ptr _tmp4B0=(struct _dyneither_ptr)({const char*_tmp47=": existential quantification in user definition not allowed";_tag_dyneither(_tmp47,sizeof(char),60U);});_tmp46.f1=_tmp4B0;});({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0U;({struct _dyneither_ptr _tmp4B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp45.f1=_tmp4B1;});({void*_tmp43[2U]={& _tmp45,& _tmp46};({struct Cyc___cycFILE*_tmp4B3=Cyc_stderr;struct _dyneither_ptr _tmp4B2=({const char*_tmp44="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp44,sizeof(char),62U);});Cyc_fprintf(_tmp4B3,_tmp4B2,_tag_dyneither(_tmp43,sizeof(void*),2U));});});});});return;}
if((unsigned int)_tmp76){
({struct Cyc_String_pa_PrintArg_struct _tmp4B;_tmp4B.tag=0U;({struct _dyneither_ptr _tmp4B4=(struct _dyneither_ptr)({const char*_tmp4C=": region partial order in user definition not allowed";_tag_dyneither(_tmp4C,sizeof(char),54U);});_tmp4B.f1=_tmp4B4;});({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0U;({struct _dyneither_ptr _tmp4B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp4A.f1=_tmp4B5;});({void*_tmp48[2U]={& _tmp4A,& _tmp4B};({struct Cyc___cycFILE*_tmp4B7=Cyc_stderr;struct _dyneither_ptr _tmp4B6=({const char*_tmp49="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp49,sizeof(char),62U);});Cyc_fprintf(_tmp4B7,_tmp4B6,_tag_dyneither(_tmp48,sizeof(void*),2U));});});});});return;}{
struct Cyc_List_List*_tmp4D=_tmp78;
# 287
while(_tmp4D != 0){
struct Cyc_Absyn_Aggrfield*_tmp4E=(struct Cyc_Absyn_Aggrfield*)_tmp4D->hd;
struct Cyc_Absyn_Aggrfield*_tmp4F=Cyc_Absyn_lookup_field(_tmp75,_tmp4E->name);
# 291
if(_tmp4F != 0){
# 294
void*_tmp50=Cyc_Toc_typ_to_c(_tmp4E->type);
void*_tmp51=Cyc_Toc_typ_to_c(_tmp4F->type);
if(!Cyc_Tcutil_unify(_tmp50,_tmp51))
({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0U;({struct _dyneither_ptr _tmp4BB=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U;({struct _dyneither_ptr _tmp4B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp5A.f1=_tmp4B8;});({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0U;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4E->name);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0U;({struct _dyneither_ptr _tmp4B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));_tmp58.f1=_tmp4B9;});({void*_tmp56[3U]={& _tmp58,& _tmp59,& _tmp5A};({struct _dyneither_ptr _tmp4BA=({const char*_tmp57=": type %s of user definition of field %s != %s";_tag_dyneither(_tmp57,sizeof(char),47U);});Cyc_aprintf(_tmp4BA,_tag_dyneither(_tmp56,sizeof(void*),3U));});});});});}));_tmp55.f1=_tmp4BB;});({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0U;({struct _dyneither_ptr _tmp4BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp54.f1=_tmp4BC;});({void*_tmp52[2U]={& _tmp54,& _tmp55};({struct Cyc___cycFILE*_tmp4BE=Cyc_stderr;struct _dyneither_ptr _tmp4BD=({const char*_tmp53="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp53,sizeof(char),71U);});Cyc_fprintf(_tmp4BE,_tmp4BD,_tag_dyneither(_tmp52,sizeof(void*),2U));});});});});else{
# 304
if(_tmp4F->width != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0U;({struct _dyneither_ptr _tmp4C1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0U;({struct _dyneither_ptr _tmp4BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));_tmp61.f1=_tmp4BF;});({void*_tmp5F[1U]={& _tmp61};({struct _dyneither_ptr _tmp4C0=({const char*_tmp60=": ignoring width of user definition of field %s";_tag_dyneither(_tmp60,sizeof(char),48U);});Cyc_aprintf(_tmp4C0,_tag_dyneither(_tmp5F,sizeof(void*),1U));});});}));_tmp5E.f1=_tmp4C1;});({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U;({struct _dyneither_ptr _tmp4C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp5D.f1=_tmp4C2;});({void*_tmp5B[2U]={& _tmp5D,& _tmp5E};({struct Cyc___cycFILE*_tmp4C4=Cyc_stderr;struct _dyneither_ptr _tmp4C3=({const char*_tmp5C="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp5C,sizeof(char),71U);});Cyc_fprintf(_tmp4C4,_tmp4C3,_tag_dyneither(_tmp5B,sizeof(void*),2U));});});});});
# 308
if(_tmp4F->attributes != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0U;({struct _dyneither_ptr _tmp4C7=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0U;({struct _dyneither_ptr _tmp4C5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));_tmp68.f1=_tmp4C5;});({void*_tmp66[1U]={& _tmp68};({struct _dyneither_ptr _tmp4C6=({const char*_tmp67=": ignoring attributes of user definition of field %s";_tag_dyneither(_tmp67,sizeof(char),53U);});Cyc_aprintf(_tmp4C6,_tag_dyneither(_tmp66,sizeof(void*),1U));});});}));_tmp65.f1=_tmp4C7;});({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0U;({struct _dyneither_ptr _tmp4C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp64.f1=_tmp4C8;});({void*_tmp62[2U]={& _tmp64,& _tmp65};({struct Cyc___cycFILE*_tmp4CA=Cyc_stderr;struct _dyneither_ptr _tmp4C9=({const char*_tmp63="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp63,sizeof(char),71U);});Cyc_fprintf(_tmp4CA,_tmp4C9,_tag_dyneither(_tmp62,sizeof(void*),2U));});});});});
# 314
_tmp4E->type=_tmp4F->type;
_tmp4E->tq=_tmp4F->tq;
_tmp4E->requires_clause=_tmp4F->requires_clause;}}
# 319
_tmp4D=_tmp4D->tl;}
# 322
_tmp4D=_tmp75;{
int missing_fields=0;
while(_tmp4D != 0){
struct Cyc_Absyn_Aggrfield*_tmp69=Cyc_Absyn_lookup_field(_tmp78,((struct Cyc_Absyn_Aggrfield*)_tmp4D->hd)->name);
if(_tmp69 == 0){
missing_fields=1;
({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0U;({struct _dyneither_ptr _tmp4CC=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0U;_tmp70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp4D->hd)->name);({void*_tmp6E[1U]={& _tmp70};({struct _dyneither_ptr _tmp4CB=({const char*_tmp6F=": no definition of field %s in system version";_tag_dyneither(_tmp6F,sizeof(char),46U);});Cyc_aprintf(_tmp4CB,_tag_dyneither(_tmp6E,sizeof(void*),1U));});});}));_tmp6D.f1=_tmp4CC;});({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0U;({struct _dyneither_ptr _tmp4CD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp6C.f1=_tmp4CD;});({void*_tmp6A[2U]={& _tmp6C,& _tmp6D};({struct Cyc___cycFILE*_tmp4CF=Cyc_stderr;struct _dyneither_ptr _tmp4CE=({const char*_tmp6B="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp6B,sizeof(char),71U);});Cyc_fprintf(_tmp4CF,_tmp4CE,_tag_dyneither(_tmp6A,sizeof(void*),2U));});});});});}
# 331
_tmp4D=_tmp4D->tl;}
# 333
goto _LL9;};};}else{goto _LLC;}}else{_LLC: _LLD:
# 335
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Core_Impossible_exn_struct _tmp4D1=({struct Cyc_Core_Impossible_exn_struct _tmp72;_tmp72.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp4D0=({const char*_tmp73="Internal Error: NULL cases not possible";_tag_dyneither(_tmp73,sizeof(char),40U);});_tmp72.f1=_tmp4D0;});_tmp72;});_tmp71[0]=_tmp4D1;});_tmp71;}));}_LL9:;}
# 337
return;}else{goto _LL7;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 7U){_LL3: _tmp9F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmp9E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL4:
# 339
({struct Cyc_String_pa_PrintArg_struct _tmp7D;_tmp7D.tag=0U;({struct _dyneither_ptr _tmp4D2=(struct _dyneither_ptr)({const char*_tmp7E=": enum merging not currently supported";_tag_dyneither(_tmp7E,sizeof(char),39U);});_tmp7D.f1=_tmp4D2;});({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0U;({struct _dyneither_ptr _tmp4D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F->name));_tmp7C.f1=_tmp4D3;});({void*_tmp7A[2U]={& _tmp7C,& _tmp7D};({struct Cyc___cycFILE*_tmp4D5=Cyc_stderr;struct _dyneither_ptr _tmp4D4=({const char*_tmp7B="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp7B,sizeof(char),62U);});Cyc_fprintf(_tmp4D5,_tmp4D4,_tag_dyneither(_tmp7A,sizeof(void*),2U));});});});});return;}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 8U){_LL5: _tmpA1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmpA0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL6:
# 341
 if(_tmpA0->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0U;({struct _dyneither_ptr _tmp4D6=(struct _dyneither_ptr)({const char*_tmp83=": universal quantification in user definition not supported";_tag_dyneither(_tmp83,sizeof(char),60U);});_tmp82.f1=_tmp4D6;});({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0U;({struct _dyneither_ptr _tmp4D7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp81.f1=_tmp4D7;});({void*_tmp7F[2U]={& _tmp81,& _tmp82};({struct Cyc___cycFILE*_tmp4D9=Cyc_stderr;struct _dyneither_ptr _tmp4D8=({const char*_tmp80="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp80,sizeof(char),62U);});Cyc_fprintf(_tmp4D9,_tmp4D8,_tag_dyneither(_tmp7F,sizeof(void*),2U));});});});});return;}
# 344
if(_tmpA0->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp87;_tmp87.tag=0U;({struct _dyneither_ptr _tmp4DA=(struct _dyneither_ptr)({const char*_tmp88=": no user definition";_tag_dyneither(_tmp88,sizeof(char),21U);});_tmp87.f1=_tmp4DA;});({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0U;({struct _dyneither_ptr _tmp4DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp86.f1=_tmp4DB;});({void*_tmp84[2U]={& _tmp86,& _tmp87};({struct Cyc___cycFILE*_tmp4DD=Cyc_stderr;struct _dyneither_ptr _tmp4DC=({const char*_tmp85="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp85,sizeof(char),62U);});Cyc_fprintf(_tmp4DD,_tmp4DC,_tag_dyneither(_tmp84,sizeof(void*),2U));});});});});return;}
if(_tmpA1->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0U;({struct _dyneither_ptr _tmp4DE=(struct _dyneither_ptr)({const char*_tmp8D=": no definition for system version";_tag_dyneither(_tmp8D,sizeof(char),35U);});_tmp8C.f1=_tmp4DE;});({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0U;({struct _dyneither_ptr _tmp4DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp8B.f1=_tmp4DF;});({void*_tmp89[2U]={& _tmp8B,& _tmp8C};({struct Cyc___cycFILE*_tmp4E1=Cyc_stderr;struct _dyneither_ptr _tmp4E0=({const char*_tmp8A="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp8A,sizeof(char),71U);});Cyc_fprintf(_tmp4E1,_tmp4E0,_tag_dyneither(_tmp89,sizeof(void*),2U));});});});});
c_decl->r=user_decl->r;
return;}{
# 352
void*_tmp8E=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA1->defn));
void*_tmp8F=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA0->defn));
if(!Cyc_Tcutil_unify(_tmp8E,_tmp8F))
({struct Cyc_String_pa_PrintArg_struct _tmp93;_tmp93.tag=0U;({struct _dyneither_ptr _tmp4E5=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0U;({struct _dyneither_ptr _tmp4E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpA1->defn)));_tmp97.f1=_tmp4E2;});({struct Cyc_String_pa_PrintArg_struct _tmp96;_tmp96.tag=0U;({struct _dyneither_ptr _tmp4E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpA0->defn)));_tmp96.f1=_tmp4E3;});({void*_tmp94[2U]={& _tmp96,& _tmp97};({struct _dyneither_ptr _tmp4E4=({const char*_tmp95=": type definition %s of user definition != %s";_tag_dyneither(_tmp95,sizeof(char),46U);});Cyc_aprintf(_tmp4E4,_tag_dyneither(_tmp94,sizeof(void*),2U));});});});}));_tmp93.f1=_tmp4E5;});({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0U;({struct _dyneither_ptr _tmp4E6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp92.f1=_tmp4E6;});({void*_tmp90[2U]={& _tmp92,& _tmp93};({struct Cyc___cycFILE*_tmp4E8=Cyc_stderr;struct _dyneither_ptr _tmp4E7=({const char*_tmp91="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp91,sizeof(char),71U);});Cyc_fprintf(_tmp4E8,_tmp4E7,_tag_dyneither(_tmp90,sizeof(void*),2U));});});});});else{
# 361
_tmpA1->defn=_tmpA0->defn;
_tmpA1->tq=_tmpA0->tq;}
# 364
goto _LL0;};}else{goto _LL7;}default: _LL7: _LL8:
# 366
({struct Cyc_String_pa_PrintArg_struct _tmp9A;_tmp9A.tag=0U;({struct _dyneither_ptr _tmp4E9=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 368
Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp9B[1U];_tmp9B[0U]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9B,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp9A.f1=_tmp4E9;});({void*_tmp98[1U]={& _tmp9A};({struct Cyc___cycFILE*_tmp4EB=Cyc_stderr;struct _dyneither_ptr _tmp4EA=({const char*_tmp99="Error in .cys file: bad user-defined type %s\n";_tag_dyneither(_tmp99,sizeof(char),46U);});Cyc_fprintf(_tmp4EB,_tmp4EA,_tag_dyneither(_tmp98,sizeof(void*),1U));});});});
return;}_LL0:;}
# 372
c_decl->r=user_decl->r;}
# 377
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 384
int state;int base;int backtrk;
int c;
state=start_state;
# 388
if(state >= 0){
({int _tmp4EC=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp4EC;});
lbuf->lex_last_action=- 1;}else{
# 392
state=(- state)- 1;}
# 394
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(431U,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 402
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 406
c=256;}}else{
# 408
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 411
if(Cyc_lex_check[_check_known_subscript_notnull(3846U,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3846U,base + c)];else{
# 414
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct Cyc_Lexing_Error_exn_struct _tmp4EE=({struct Cyc_Lexing_Error_exn_struct _tmpA4;_tmpA4.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp4ED=({const char*_tmpA5="empty token";_tag_dyneither(_tmpA5,sizeof(char),12U);});_tmpA4.f1=_tmp4ED;});_tmpA4;});_tmpA3[0]=_tmp4EE;});_tmpA3;}));else{
# 420
return lbuf->lex_last_action;}}else{
# 423
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 427
struct _tuple21*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp4EF=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp4EF;});
{int _tmpA6=lexstate;switch(_tmpA6){case 0U: _LL1: _LL2:
# 386 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
({struct Cyc_Set_Set**_tmp4F2=({struct Cyc_Set_Set**_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct Cyc_Set_Set*_tmp4F1=({struct Cyc_Set_Set*_tmp4F0=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(_tmp4F0,(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});_tmpA7[0]=_tmp4F1;});_tmpA7;});Cyc_current_targets=_tmp4F2;});}
# 391
return({struct _tuple21*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmpA8->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmpA8;});case 1U: _LL3: _LL4:
# 394 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2U: _LL5: _LL6:
# 396
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 400
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Lexing_Error_exn_struct _tmp4F4=({struct Cyc_Lexing_Error_exn_struct _tmpAA;_tmpAA.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp4F3=({const char*_tmpAB="some action didn't return!";_tag_dyneither(_tmpAB,sizeof(char),27U);});_tmpAA.f1=_tmp4F3;});_tmpAA;});_tmpA9[0]=_tmp4F4;});_tmpA9;}));}
# 402
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp4F5=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp4F5;});
{int _tmpAC=lexstate;switch(_tmpAC){case 0U: _LL1: _LL2:
# 400 "buildlib.cyl"
({struct _dyneither_ptr*_tmp4F9=({struct _dyneither_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _dyneither_ptr _tmp4F8=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp4F7=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp4F7,0,(unsigned long)(({
int _tmp4F6=Cyc_Lexing_lexeme_end(lexbuf);_tmp4F6 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});_tmpAD[0]=_tmp4F8;});_tmpAD;});
# 400
Cyc_current_source=_tmp4F9;});
# 402
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp4FB=({struct Cyc_Set_Set**_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct Cyc_Set_Set*_tmp4FA=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpAE[0]=_tmp4FA;});_tmpAE;});Cyc_current_targets=_tmp4FB;});
Cyc_token(lexbuf);
return 0;case 1U: _LL3: _LL4:
# 408
({struct _dyneither_ptr*_tmp4FF=({struct _dyneither_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));({struct _dyneither_ptr _tmp4FE=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp4FD=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp4FD,0,(unsigned long)(({
int _tmp4FC=Cyc_Lexing_lexeme_end(lexbuf);_tmp4FC - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});_tmpAF[0]=_tmp4FE;});_tmpAF;});
# 408
Cyc_current_source=_tmp4FF;});
# 410
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp501=({struct Cyc_Set_Set**_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_Set_Set*_tmp500=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpB0[0]=_tmp500;});_tmpB0;});Cyc_current_targets=_tmp501;});
Cyc_args(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 416
({struct _dyneither_ptr*_tmp503=({struct _dyneither_ptr*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct _dyneither_ptr _tmp502=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpB1[0]=_tmp502;});_tmpB1;});Cyc_current_source=_tmp503;});
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp505=({struct Cyc_Set_Set**_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct Cyc_Set_Set*_tmp504=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpB2[0]=_tmp504;});_tmpB2;});Cyc_current_targets=_tmp505;});
Cyc_token(lexbuf);
return 0;default: _LL7: _LL8:
# 422
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 425
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct Cyc_Lexing_Error_exn_struct _tmp507=({struct Cyc_Lexing_Error_exn_struct _tmpB4;_tmpB4.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp506=({const char*_tmpB5="some action didn't return!";_tag_dyneither(_tmpB5,sizeof(char),27U);});_tmpB4.f1=_tmp506;});_tmpB4;});_tmpB3[0]=_tmp507;});_tmpB3;}));}
# 427
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp508=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp508;});
{int _tmpB6=lexstate;switch(_tmpB6){case 0U: _LL1: _LL2: {
# 425 "buildlib.cyl"
struct _dyneither_ptr*_tmpB7=({struct _dyneither_ptr*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct _dyneither_ptr _tmp50B=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp50A=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp50A,0,(unsigned long)(({
int _tmp509=Cyc_Lexing_lexeme_end(lexbuf);_tmp509 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});_tmpB9[0]=_tmp50B;});_tmpB9;});
({struct Cyc_List_List*_tmp50C=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->hd=_tmpB7;_tmpB8->tl=Cyc_current_args;_tmpB8;});Cyc_current_args=_tmp50C;});
return Cyc_args(lexbuf);}case 1U: _LL3: _LL4: {
# 431
struct _dyneither_ptr*_tmpBA=({struct _dyneither_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _dyneither_ptr _tmp50F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp50E=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp50E,0,(unsigned long)(({
int _tmp50D=Cyc_Lexing_lexeme_end(lexbuf);_tmp50D - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});_tmpBC[0]=_tmp50F;});_tmpBC;});
({struct Cyc_List_List*_tmp510=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=_tmpBA;_tmpBB->tl=Cyc_current_args;_tmpBB;});Cyc_current_args=_tmp510;});
return Cyc_args(lexbuf);}case 2U: _LL5: _LL6: {
# 437
struct _dyneither_ptr*_tmpBD=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct _dyneither_ptr _tmp513=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp512=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp512,0,(unsigned long)(({
int _tmp511=Cyc_Lexing_lexeme_end(lexbuf);_tmp511 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});_tmpBF[0]=_tmp513;});_tmpBF;});
({struct Cyc_List_List*_tmp514=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=_tmpBD;_tmpBE->tl=Cyc_current_args;_tmpBE;});Cyc_current_args=_tmp514;});
return Cyc_token(lexbuf);}default: _LL7: _LL8:
# 442
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL0:;}
# 445
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));({struct Cyc_Lexing_Error_exn_struct _tmp516=({struct Cyc_Lexing_Error_exn_struct _tmpC1;_tmpC1.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp515=({const char*_tmpC2="some action didn't return!";_tag_dyneither(_tmpC2,sizeof(char),27U);});_tmpC1.f1=_tmp515;});_tmpC1;});_tmpC0[0]=_tmp516;});_tmpC0;}));}
# 447
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp517=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp517;});
{int _tmpC3=lexstate;switch(_tmpC3){case 0U: _LL1: _LL2:
# 446 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct _dyneither_ptr _tmp518=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpC4[0]=_tmp518;});_tmpC4;}));return Cyc_token(lexbuf);case 1U: _LL3: _LL4:
# 449 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 452 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3U: _LL7: _LL8:
# 455 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4U: _LL9: _LLA:
# 458 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5U: _LLB: _LLC:
# 460
 return Cyc_token(lexbuf);case 6U: _LLD: _LLE:
# 462
 return Cyc_token(lexbuf);case 7U: _LLF: _LL10:
# 464
 return Cyc_token(lexbuf);case 8U: _LL11: _LL12:
# 467 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9U: _LL13: _LL14:
# 470 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10U: _LL15: _LL16:
# 473 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11U: _LL17: _LL18:
# 475
 return Cyc_token(lexbuf);case 12U: _LL19: _LL1A:
# 477
 return Cyc_token(lexbuf);case 13U: _LL1B: _LL1C:
# 479
 return Cyc_token(lexbuf);case 14U: _LL1D: _LL1E:
# 481
 return Cyc_token(lexbuf);case 15U: _LL1F: _LL20:
# 483
 return Cyc_token(lexbuf);case 16U: _LL21: _LL22:
# 485
 return Cyc_token(lexbuf);case 17U: _LL23: _LL24:
# 487
 return Cyc_token(lexbuf);case 18U: _LL25: _LL26:
# 489
 return Cyc_token(lexbuf);case 19U: _LL27: _LL28:
# 491
 return Cyc_token(lexbuf);case 20U: _LL29: _LL2A:
# 493
 return Cyc_token(lexbuf);case 21U: _LL2B: _LL2C:
# 495
 return Cyc_token(lexbuf);case 22U: _LL2D: _LL2E:
# 497
 return Cyc_token(lexbuf);case 23U: _LL2F: _LL30:
# 499
 return Cyc_token(lexbuf);case 24U: _LL31: _LL32:
# 502 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25U: _LL33: _LL34:
# 504
 return Cyc_token(lexbuf);case 26U: _LL35: _LL36:
# 506
 return Cyc_token(lexbuf);case 27U: _LL37: _LL38:
# 508
 return Cyc_token(lexbuf);case 28U: _LL39: _LL3A:
# 510
 return Cyc_token(lexbuf);case 29U: _LL3B: _LL3C:
# 512
 return Cyc_token(lexbuf);case 30U: _LL3D: _LL3E:
# 514
 return Cyc_token(lexbuf);case 31U: _LL3F: _LL40:
# 516
 return Cyc_token(lexbuf);case 32U: _LL41: _LL42:
# 518
 return Cyc_token(lexbuf);case 33U: _LL43: _LL44:
# 520
 return Cyc_token(lexbuf);case 34U: _LL45: _LL46:
# 522
 return Cyc_token(lexbuf);case 35U: _LL47: _LL48:
# 524
 return Cyc_token(lexbuf);case 36U: _LL49: _LL4A:
# 526
 return Cyc_token(lexbuf);case 37U: _LL4B: _LL4C:
# 528
 return Cyc_token(lexbuf);case 38U: _LL4D: _LL4E:
# 530
 return Cyc_token(lexbuf);case 39U: _LL4F: _LL50:
# 532
 return Cyc_token(lexbuf);case 40U: _LL51: _LL52:
# 534
 return Cyc_token(lexbuf);case 41U: _LL53: _LL54:
# 536
 return Cyc_token(lexbuf);case 42U: _LL55: _LL56:
# 538
 return Cyc_token(lexbuf);case 43U: _LL57: _LL58:
# 540
 return Cyc_token(lexbuf);case 44U: _LL59: _LL5A:
# 542
 return Cyc_token(lexbuf);case 45U: _LL5B: _LL5C:
# 544
 return Cyc_token(lexbuf);case 46U: _LL5D: _LL5E:
# 546
 return Cyc_token(lexbuf);case 47U: _LL5F: _LL60:
# 548
 return Cyc_token(lexbuf);case 48U: _LL61: _LL62:
# 551 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL63: _LL64:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL0:;}
# 555
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));({struct Cyc_Lexing_Error_exn_struct _tmp51A=({struct Cyc_Lexing_Error_exn_struct _tmpC6;_tmpC6.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp519=({const char*_tmpC7="some action didn't return!";_tag_dyneither(_tmpC7,sizeof(char),27U);});_tmpC6.f1=_tmp519;});_tmpC6;});_tmpC5[0]=_tmp51A;});_tmpC5;}));}
# 557
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp51B=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp51B;});
{int _tmpC8=lexstate;switch(_tmpC8){case 0U: _LL1: _LL2:
# 556 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1U: _LL3: _LL4:
# 557 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 558 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3U: _LL7: _LL8:
# 559 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4U: _LL9: _LLA:
# 562 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5U: _LLB: _LLC:
# 565 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6U: _LLD: _LLE:
# 567
 return Cyc_string(lexbuf);case 7U: _LLF: _LL10:
# 568 "buildlib.cyl"
 return 0;case 8U: _LL11: _LL12:
# 569 "buildlib.cyl"
 return 0;case 9U: _LL13: _LL14:
# 570 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL15: _LL16:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL0:;}
# 574
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));({struct Cyc_Lexing_Error_exn_struct _tmp51D=({struct Cyc_Lexing_Error_exn_struct _tmpCA;_tmpCA.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp51C=({const char*_tmpCB="some action didn't return!";_tag_dyneither(_tmpCB,sizeof(char),27U);});_tmpCA.f1=_tmp51C;});_tmpCA;});_tmpC9[0]=_tmp51D;});_tmpC9;}));}
# 576
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp51E=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp51E;});
{int _tmpCC=lexstate;switch(_tmpCC){case 0U: _LL1: _LL2:
# 579 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 581
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2U: _LL5: _LL6:
# 588 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCD=0U;({struct _dyneither_ptr _tmp51F=({const char*_tmpCE="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpCE,sizeof(char),44U);});Cyc_log(_tmp51F,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});
return 1;case 3U: _LL7: _LL8:
# 594 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCF=0U;({struct _dyneither_ptr _tmp520=({const char*_tmpD0="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpD0,sizeof(char),44U);});Cyc_log(_tmp520,_tag_dyneither(_tmpCF,sizeof(void*),0U));});});
return 1;case 4U: _LL9: _LLA:
# 600 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD1=0U;({struct _dyneither_ptr _tmp521=({const char*_tmpD2="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpD2,sizeof(char),44U);});Cyc_log(_tmp521,_tag_dyneither(_tmpD1,sizeof(void*),0U));});});
return 1;case 5U: _LLB: _LLC:
# 606 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD3=0U;({struct _dyneither_ptr _tmp522=({const char*_tmpD4="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpD4,sizeof(char),44U);});Cyc_log(_tmp522,_tag_dyneither(_tmpD3,sizeof(void*),0U));});});
return 1;case 6U: _LLD: _LLE:
# 610
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD5=0U;({struct _dyneither_ptr _tmp523=({const char*_tmpD6="Warning: use of region sidestepped\n";_tag_dyneither(_tmpD6,sizeof(char),36U);});Cyc_log(_tmp523,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});
return 1;case 7U: _LLF: _LL10:
# 614
({void*_tmpD7=0U;({struct _dyneither_ptr _tmp524=({const char*_tmpD8="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmpD8,sizeof(char),39U);});Cyc_log(_tmp524,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});
return 1;case 8U: _LL11: _LL12:
# 619 "buildlib.cyl"
({void*_tmpD9=0U;({struct _dyneither_ptr _tmp525=({const char*_tmpDA="Warning: use of mode HI deleted\n";_tag_dyneither(_tmpDA,sizeof(char),33U);});Cyc_log(_tmp525,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});
return 1;case 9U: _LL13: _LL14:
# 622
({void*_tmpDB=0U;({struct _dyneither_ptr _tmp526=({const char*_tmpDC="Warning: use of mode SI deleted\n";_tag_dyneither(_tmpDC,sizeof(char),33U);});Cyc_log(_tmp526,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
return 1;case 10U: _LL15: _LL16:
# 625
({void*_tmpDD=0U;({struct _dyneither_ptr _tmp527=({const char*_tmpDE="Warning: use of mode QI deleted\n";_tag_dyneither(_tmpDE,sizeof(char),33U);});Cyc_log(_tmp527,_tag_dyneither(_tmpDD,sizeof(void*),0U));});});
return 1;case 11U: _LL17: _LL18:
# 628
({void*_tmpDF=0U;({struct _dyneither_ptr _tmp528=({const char*_tmpE0="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpE0,sizeof(char),33U);});Cyc_log(_tmp528,_tag_dyneither(_tmpDF,sizeof(void*),0U));});});
return 1;case 12U: _LL19: _LL1A:
# 631
({void*_tmpE1=0U;({struct _dyneither_ptr _tmp529=({const char*_tmpE2="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpE2,sizeof(char),33U);});Cyc_log(_tmp529,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});
return 1;case 13U: _LL1B: _LL1C:
# 634
({void*_tmpE3=0U;({struct _dyneither_ptr _tmp52A=({const char*_tmpE4="Warning: use of mode word deleted\n";_tag_dyneither(_tmpE4,sizeof(char),35U);});Cyc_log(_tmp52A,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});
return 1;case 14U: _LL1D: _LL1E:
# 637
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15U: _LL1F: _LL20:
# 639
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16U: _LL21: _LL22:
# 641
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17U: _LL23: _LL24:
# 643
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18U: _LL25: _LL26:
# 648 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19U: _LL27: _LL28:
# 650
 return 1;case 20U: _LL29: _LL2A:
# 652
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpE5=0U;({struct _dyneither_ptr _tmp52B=({const char*_tmpE6="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmpE6,sizeof(char),42U);});Cyc_log(_tmp52B,_tag_dyneither(_tmpE5,sizeof(void*),0U));});});
return 1;case 21U: _LL2B: _LL2C:
# 658
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpE7=0U;({struct _dyneither_ptr _tmp52C=({const char*_tmpE8="Warning: replacing use of __asm with 0\n";_tag_dyneither(_tmpE8,sizeof(char),40U);});Cyc_log(_tmp52C,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});
return 1;case 22U: _LL2D: _LL2E:
# 664
({int _tmp52D=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp52D,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL2F: _LL30:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL0:;}
# 668
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct Cyc_Lexing_Error_exn_struct _tmp52F=({struct Cyc_Lexing_Error_exn_struct _tmpEA;_tmpEA.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp52E=({const char*_tmpEB="some action didn't return!";_tag_dyneither(_tmpEB,sizeof(char),27U);});_tmpEA.f1=_tmp52E;});_tmpEA;});_tmpE9[0]=_tmp52F;});_tmpE9;}));}
# 670
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp530=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp530;});
{int _tmpEC=lexstate;switch(_tmpEC){case 0U: _LL1: _LL2:
# 668 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 670
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2U: _LL5: _LL6:
# 672
({void*_tmpED=0U;({struct _dyneither_ptr _tmp531=({const char*_tmpEE="Warning: unclosed string\n";_tag_dyneither(_tmpEE,sizeof(char),26U);});Cyc_log(_tmp531,_tag_dyneither(_tmpED,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0U;({struct _dyneither_ptr _tmp532=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpF1.f1=_tmp532;});({void*_tmpEF[1U]={& _tmpF1};({struct Cyc___cycFILE*_tmp534=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp533=({const char*_tmpF0="%s";_tag_dyneither(_tmpF0,sizeof(char),3U);});Cyc_fprintf(_tmp534,_tmp533,_tag_dyneither(_tmpEF,sizeof(void*),1U));});});});return 1;case 3U: _LL7: _LL8:
# 675
({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0U;({struct _dyneither_ptr _tmp535=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpF4.f1=_tmp535;});({void*_tmpF2[1U]={& _tmpF4};({struct Cyc___cycFILE*_tmp537=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp536=({const char*_tmpF3="%s";_tag_dyneither(_tmpF3,sizeof(char),3U);});Cyc_fprintf(_tmp537,_tmp536,_tag_dyneither(_tmpF2,sizeof(void*),1U));});});});return 1;case 4U: _LL9: _LLA:
# 677
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0U;({struct _dyneither_ptr _tmp538=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpF7.f1=_tmp538;});({void*_tmpF5[1U]={& _tmpF7};({struct Cyc___cycFILE*_tmp53A=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp539=({const char*_tmpF6="%s";_tag_dyneither(_tmpF6,sizeof(char),3U);});Cyc_fprintf(_tmp53A,_tmp539,_tag_dyneither(_tmpF5,sizeof(void*),1U));});});});return 1;case 5U: _LLB: _LLC:
# 679
({struct Cyc_String_pa_PrintArg_struct _tmpFA;_tmpFA.tag=0U;({struct _dyneither_ptr _tmp53B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpFA.f1=_tmp53B;});({void*_tmpF8[1U]={& _tmpFA};({struct Cyc___cycFILE*_tmp53D=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp53C=({const char*_tmpF9="%s";_tag_dyneither(_tmpF9,sizeof(char),3U);});Cyc_fprintf(_tmp53D,_tmp53C,_tag_dyneither(_tmpF8,sizeof(void*),1U));});});});return 1;case 6U: _LLD: _LLE:
# 681
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0U;({struct _dyneither_ptr _tmp53E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpFD.f1=_tmp53E;});({void*_tmpFB[1U]={& _tmpFD};({struct Cyc___cycFILE*_tmp540=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp53F=({const char*_tmpFC="%s";_tag_dyneither(_tmpFC,sizeof(char),3U);});Cyc_fprintf(_tmp540,_tmp53F,_tag_dyneither(_tmpFB,sizeof(void*),1U));});});});return 1;case 7U: _LLF: _LL10:
# 683
({struct Cyc_String_pa_PrintArg_struct _tmp100;_tmp100.tag=0U;({struct _dyneither_ptr _tmp541=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp100.f1=_tmp541;});({void*_tmpFE[1U]={& _tmp100};({struct Cyc___cycFILE*_tmp543=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp542=({const char*_tmpFF="%s";_tag_dyneither(_tmpFF,sizeof(char),3U);});Cyc_fprintf(_tmp543,_tmp542,_tag_dyneither(_tmpFE,sizeof(void*),1U));});});});return 1;case 8U: _LL11: _LL12:
# 685
({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0U;({struct _dyneither_ptr _tmp544=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp103.f1=_tmp544;});({void*_tmp101[1U]={& _tmp103};({struct Cyc___cycFILE*_tmp546=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp545=({const char*_tmp102="%s";_tag_dyneither(_tmp102,sizeof(char),3U);});Cyc_fprintf(_tmp546,_tmp545,_tag_dyneither(_tmp101,sizeof(void*),1U));});});});return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL0:;}
# 689
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp104=_cycalloc(sizeof(*_tmp104));({struct Cyc_Lexing_Error_exn_struct _tmp548=({struct Cyc_Lexing_Error_exn_struct _tmp105;_tmp105.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp547=({const char*_tmp106="some action didn't return!";_tag_dyneither(_tmp106,sizeof(char),27U);});_tmp105.f1=_tmp547;});_tmp105;});_tmp104[0]=_tmp548;});_tmp104;}));}
# 691
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp549=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp549;});
{int _tmp107=lexstate;switch(_tmp107){case 0U: _LL1: _LL2:
# 695 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 697
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2U: _LL5: _LL6:
# 701
 ++ Cyc_parens_to_match;
return 1;case 3U: _LL7: _LL8:
# 704
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 707
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 710
 return 1;case 6U: _LLD: _LLE:
# 712
 return 1;default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL0:;}
# 716
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp108=_cycalloc(sizeof(*_tmp108));({struct Cyc_Lexing_Error_exn_struct _tmp54B=({struct Cyc_Lexing_Error_exn_struct _tmp109;_tmp109.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp54A=({const char*_tmp10A="some action didn't return!";_tag_dyneither(_tmp10A,sizeof(char),27U);});_tmp109.f1=_tmp54A;});_tmp109;});_tmp108[0]=_tmp54B;});_tmp108;}));}
# 718
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp54C=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp54C;});
{int _tmp10B=lexstate;switch(_tmp10B){case 0U: _LL1: _LL2:
# 716 "buildlib.cyl"
({void*_tmp10C=0U;({struct _dyneither_ptr _tmp54D=({const char*_tmp10D="Warning: unclosed string\n";_tag_dyneither(_tmp10D,sizeof(char),26U);});Cyc_log(_tmp54D,_tag_dyneither(_tmp10C,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 718
 return 0;case 2U: _LL5: _LL6:
# 720
({void*_tmp10E=0U;({struct _dyneither_ptr _tmp54E=({const char*_tmp10F="Warning: unclosed string\n";_tag_dyneither(_tmp10F,sizeof(char),26U);});Cyc_log(_tmp54E,_tag_dyneither(_tmp10E,sizeof(void*),0U));});});return 1;case 3U: _LL7: _LL8:
# 722
 return 1;case 4U: _LL9: _LLA:
# 724
 return 1;case 5U: _LLB: _LLC:
# 726
 return 1;case 6U: _LLD: _LLE:
# 728
 return 1;case 7U: _LLF: _LL10:
# 730
 return 1;case 8U: _LL11: _LL12:
# 732
 return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL0:;}
# 736
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Lexing_Error_exn_struct _tmp550=({struct Cyc_Lexing_Error_exn_struct _tmp111;_tmp111.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp54F=({const char*_tmp112="some action didn't return!";_tag_dyneither(_tmp112,sizeof(char),27U);});_tmp111.f1=_tmp54F;});_tmp111;});_tmp110[0]=_tmp550;});_tmp110;}));}
# 738
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp551=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp551;});
{int _tmp113=lexstate;switch(_tmp113){case 0U: _LL1: _LL2:
# 736 "buildlib.cyl"
({void*_tmp114=0U;({struct _dyneither_ptr _tmp552=({const char*_tmp115="Warning: unclosed comment\n";_tag_dyneither(_tmp115,sizeof(char),27U);});Cyc_log(_tmp552,_tag_dyneither(_tmp114,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 738
 return 0;case 2U: _LL5: _LL6:
# 740
 return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL0:;}
# 744
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Lexing_Error_exn_struct _tmp554=({struct Cyc_Lexing_Error_exn_struct _tmp117;_tmp117.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp553=({const char*_tmp118="some action didn't return!";_tag_dyneither(_tmp118,sizeof(char),27U);});_tmp117.f1=_tmp553;});_tmp117;});_tmp116[0]=_tmp554;});_tmp116;}));}
# 746
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple22*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp555=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp555;});
{int _tmp119=lexstate;switch(_tmp119){case 0U: _LL1: _LL2:
# 748 "buildlib.cyl"
({struct _dyneither_ptr _tmp556=({const char*_tmp11A="#define ";_tag_dyneither(_tmp11A,sizeof(char),9U);});Cyc_current_line=_tmp556;});
Cyc_suck_macroname(lexbuf);
return({struct _tuple22*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->f1=Cyc_current_line;_tmp11B->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp11B;});case 1U: _LL3: _LL4:
# 752
 return Cyc_suck_line(lexbuf);case 2U: _LL5: _LL6:
# 754
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL0:;}
# 758
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_Lexing_Error_exn_struct _tmp558=({struct Cyc_Lexing_Error_exn_struct _tmp11D;_tmp11D.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp557=({const char*_tmp11E="some action didn't return!";_tag_dyneither(_tmp11E,sizeof(char),27U);});_tmp11D.f1=_tmp557;});_tmp11D;});_tmp11C[0]=_tmp558;});_tmp11C;}));}
# 760
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp559=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp559;});
{int _tmp11F=lexstate;if(_tmp11F == 0){_LL1: _LL2:
# 758 "buildlib.cyl"
({struct _dyneither_ptr*_tmp55B=({struct _dyneither_ptr*_tmp120=_cycalloc(sizeof(*_tmp120));({struct _dyneither_ptr _tmp55A=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp120[0]=_tmp55A;});_tmp120;});Cyc_current_source=_tmp55B;});
({struct _dyneither_ptr _tmp55C=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));Cyc_current_line=_tmp55C;});
return Cyc_suck_restofline(lexbuf);}else{_LL3: _LL4:
# 762
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 765
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Lexing_Error_exn_struct _tmp55E=({struct Cyc_Lexing_Error_exn_struct _tmp122;_tmp122.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp55D=({const char*_tmp123="some action didn't return!";_tag_dyneither(_tmp123,sizeof(char),27U);});_tmp122.f1=_tmp55D;});_tmp122;});_tmp121[0]=_tmp55E;});_tmp121;}));}
# 767
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp55F=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp55F;});
{int _tmp124=lexstate;if(_tmp124 == 0){_LL1: _LL2:
# 765 "buildlib.cyl"
({struct _dyneither_ptr _tmp561=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp560=(struct _dyneither_ptr)Cyc_current_line;Cyc_strconcat(_tmp560,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});Cyc_current_line=_tmp561;});return 0;}else{_LL3: _LL4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL0:;}
# 769
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp125=_cycalloc(sizeof(*_tmp125));({struct Cyc_Lexing_Error_exn_struct _tmp563=({struct Cyc_Lexing_Error_exn_struct _tmp126;_tmp126.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp562=({const char*_tmp127="some action didn't return!";_tag_dyneither(_tmp127,sizeof(char),27U);});_tmp126.f1=_tmp562;});_tmp126;});_tmp125[0]=_tmp563;});_tmp125;}));}
# 771
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple25*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp564=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp564;});
{int _tmp128=lexstate;switch(_tmp128){case 0U: _LL1: _LL2:
# 772 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1U: _LL3: _LL4:
# 774
({struct _dyneither_ptr _tmp567=(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp566=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp566,0,(unsigned long)(({
int _tmp565=Cyc_Lexing_lexeme_end(lexbuf);_tmp565 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 774
Cyc_current_headerfile=_tmp567;});
# 777
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
({struct Cyc_List_List*_tmp568=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_hstubs=_tmp568;});
({struct Cyc_List_List*_tmp569=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cstubs=_tmp569;});
({struct Cyc_List_List*_tmp56A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);Cyc_current_cycstubs=_tmp56A;});
return({struct _tuple25*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->f1=Cyc_current_headerfile;_tmp129->f2=Cyc_current_symbols;_tmp129->f3=Cyc_current_user_defs;_tmp129->f4=Cyc_current_omit_symbols;_tmp129->f5=Cyc_current_hstubs;_tmp129->f6=Cyc_current_cstubs;_tmp129->f7=Cyc_current_cycstubs;_tmp129;});case 2U: _LL5: _LL6:
# 796
 return Cyc_spec(lexbuf);case 3U: _LL7: _LL8:
# 798
 return 0;case 4U: _LL9: _LLA:
# 800
({struct Cyc_Int_pa_PrintArg_struct _tmp12C;_tmp12C.tag=1U;({unsigned long _tmp56B=(unsigned long)((int)
# 802
Cyc_Lexing_lexeme_char(lexbuf,0));_tmp12C.f1=_tmp56B;});({void*_tmp12A[1U]={& _tmp12C};({struct Cyc___cycFILE*_tmp56D=Cyc_stderr;struct _dyneither_ptr _tmp56C=({const char*_tmp12B="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp12B,sizeof(char),67U);});Cyc_fprintf(_tmp56D,_tmp56C,_tag_dyneither(_tmp12A,sizeof(void*),1U));});});});
return 0;default: _LLB: _LLC:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL0:;}
# 807
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct Cyc_Lexing_Error_exn_struct _tmp56F=({struct Cyc_Lexing_Error_exn_struct _tmp12E;_tmp12E.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp56E=({const char*_tmp12F="some action didn't return!";_tag_dyneither(_tmp12F,sizeof(char),27U);});_tmp12E.f1=_tmp56E;});_tmp12E;});_tmp12D[0]=_tmp56F;});_tmp12D;}));}
# 809
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp570=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp570;});
{int _tmp130=lexstate;switch(_tmp130){case 0U: _LL1: _LL2:
# 807 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 809
 return 0;case 2U: _LL5: _LL6:
# 811
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
({struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);Cyc_current_symbols=_tmp571;});
return 1;case 3U: _LL7: _LL8:
# 816
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp131=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(_tmp131 != Cyc_current_user_defs){
({void*_tmp132=0U;({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _dyneither_ptr _tmp572=({const char*_tmp133="Error in .cys file: got optional definition in omitsymbols\n";_tag_dyneither(_tmp133,sizeof(char),60U);});Cyc_fprintf(_tmp573,_tmp572,_tag_dyneither(_tmp132,sizeof(void*),0U));});});
# 822
return 0;}
# 824
({struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);Cyc_current_omit_symbols=_tmp574;});
return 1;};case 4U: _LL9: _LLA:
# 827
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp575=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp575;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp135=_cycalloc(sizeof(*_tmp135));({struct _dyneither_ptr _tmp576=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp135->f1=_tmp576;});({struct _dyneither_ptr _tmp577=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp135->f2=_tmp577;});_tmp135;});
({struct Cyc_List_List*_tmp578=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=x;_tmp134->tl=Cyc_current_hstubs;_tmp134;});Cyc_current_hstubs=_tmp578;});
return 1;};case 5U: _LLB: _LLC: {
# 835
struct _dyneither_ptr _tmp136=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp136,sizeof(char),(int)Cyc_strlen(({const char*_tmp137="hstub";_tag_dyneither(_tmp137,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp136,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp136,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp136;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp138=Cyc_substring((struct _dyneither_ptr)_tmp136,0,(unsigned long)((t.curr - _tmp136.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp579=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp579;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->f1=(struct _dyneither_ptr)_tmp138;({struct _dyneither_ptr _tmp57A=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp13A->f2=_tmp57A;});_tmp13A;});
({struct Cyc_List_List*_tmp57B=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=x;_tmp139->tl=Cyc_current_hstubs;_tmp139;});Cyc_current_hstubs=_tmp57B;});
return 1;};};};}case 6U: _LLD: _LLE:
# 849
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp57C=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp57C;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp13C=_cycalloc(sizeof(*_tmp13C));({struct _dyneither_ptr _tmp57D=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp13C->f1=_tmp57D;});({struct _dyneither_ptr _tmp57E=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp13C->f2=_tmp57E;});_tmp13C;});
({struct Cyc_List_List*_tmp57F=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=x;_tmp13B->tl=Cyc_current_cstubs;_tmp13B;});Cyc_current_cstubs=_tmp57F;});
return 1;};case 7U: _LLF: _LL10: {
# 857
struct _dyneither_ptr _tmp13D=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp13D,sizeof(char),(int)Cyc_strlen(({const char*_tmp13E="cstub";_tag_dyneither(_tmp13E,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp13D,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp13D,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp13D;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp13F=Cyc_substring((struct _dyneither_ptr)_tmp13D,0,(unsigned long)((t.curr - _tmp13D.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp580=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp580;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=(struct _dyneither_ptr)_tmp13F;({struct _dyneither_ptr _tmp581=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp141->f2=_tmp581;});_tmp141;});
({struct Cyc_List_List*_tmp582=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=x;_tmp140->tl=Cyc_current_cstubs;_tmp140;});Cyc_current_cstubs=_tmp582;});
return 1;};};};}case 8U: _LL11: _LL12:
# 871
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp583=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp583;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp143=_cycalloc(sizeof(*_tmp143));({struct _dyneither_ptr _tmp584=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp143->f1=_tmp584;});({struct _dyneither_ptr _tmp585=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp143->f2=_tmp585;});_tmp143;});
({struct Cyc_List_List*_tmp586=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=x;_tmp142->tl=Cyc_current_cycstubs;_tmp142;});Cyc_current_cycstubs=_tmp586;});
return 1;};case 9U: _LL13: _LL14: {
# 879
struct _dyneither_ptr _tmp144=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp144,sizeof(char),(int)Cyc_strlen(({const char*_tmp145="cycstub";_tag_dyneither(_tmp145,sizeof(char),8U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp144,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp144,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp144;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp146=Cyc_substring((struct _dyneither_ptr)_tmp144,0,(unsigned long)((t.curr - _tmp144.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp587=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp587;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->f1=(struct _dyneither_ptr)_tmp146;({struct _dyneither_ptr _tmp588=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp148->f2=_tmp588;});_tmp148;});
({struct Cyc_List_List*_tmp589=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->hd=x;_tmp147->tl=Cyc_current_cycstubs;_tmp147;});Cyc_current_cycstubs=_tmp589;});
return 1;};};};}case 10U: _LL15: _LL16:
# 893
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp58A=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp58A;});
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmp14A=_cycalloc(sizeof(*_tmp14A));({struct _dyneither_ptr _tmp58B=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp14A[0]=_tmp58B;});_tmp14A;});
({struct Cyc_List_List*_tmp58C=({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=x;_tmp149->tl=Cyc_current_cpp;_tmp149;});Cyc_current_cpp=_tmp58C;});
return 1;};case 11U: _LL17: _LL18:
# 900
 return 1;case 12U: _LL19: _LL1A:
# 902
 return 1;case 13U: _LL1B: _LL1C:
# 904
({struct Cyc_Int_pa_PrintArg_struct _tmp14D;_tmp14D.tag=1U;({unsigned long _tmp58D=(unsigned long)((int)
# 906
Cyc_Lexing_lexeme_char(lexbuf,0));_tmp14D.f1=_tmp58D;});({void*_tmp14B[1U]={& _tmp14D};({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _dyneither_ptr _tmp58E=({const char*_tmp14C="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmp14C,sizeof(char),58U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmp14B,sizeof(void*),1U));});});});
return 0;default: _LL1D: _LL1E:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL0:;}
# 911
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14E=_cycalloc(sizeof(*_tmp14E));({struct Cyc_Lexing_Error_exn_struct _tmp591=({struct Cyc_Lexing_Error_exn_struct _tmp14F;_tmp14F.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp590=({const char*_tmp150="some action didn't return!";_tag_dyneither(_tmp150,sizeof(char),27U);});_tmp14F.f1=_tmp590;});_tmp14F;});_tmp14E[0]=_tmp591;});_tmp14E;}));}
# 913
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp592=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp592;});
{int _tmp151=lexstate;switch(_tmp151){case 0U: _LL1: _LL2:
# 917 "buildlib.cyl"
({struct Cyc_List_List*_tmp595=({struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));({struct _dyneither_ptr*_tmp594=({struct _dyneither_ptr*_tmp153=_cycalloc(sizeof(*_tmp153));({struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp153[0]=_tmp593;});_tmp153;});_tmp152->hd=_tmp594;});_tmp152->tl=Cyc_snarfed_symbols;_tmp152;});Cyc_snarfed_symbols=_tmp595;});
return 1;case 1U: _LL3: _LL4: {
# 920
struct _dyneither_ptr _tmp154=Cyc_Lexing_lexeme(lexbuf);
struct _dyneither_ptr t=_tmp154;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
({struct _dyneither_ptr _tmp596=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp154,0,(unsigned long)((t.curr - _tmp154.curr)/ sizeof(char)));Cyc_current_symbol=_tmp596;});
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp597=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp597;});
while(Cyc_block(lexbuf)){;}
# 929
Cyc_rename_current_symbol=0;{
struct _tuple24*user_def=({struct _tuple24*_tmp158=_cycalloc(sizeof(*_tmp158));({struct _dyneither_ptr*_tmp598=({struct _dyneither_ptr*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=Cyc_current_symbol;_tmp15A;});_tmp158->f1=_tmp598;});({struct _dyneither_ptr*_tmp59A=({struct _dyneither_ptr*_tmp159=_cycalloc(sizeof(*_tmp159));({struct _dyneither_ptr _tmp599=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp159[0]=_tmp599;});_tmp159;});_tmp158->f2=_tmp59A;});_tmp158;});
({struct Cyc_List_List*_tmp59C=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _dyneither_ptr*_tmp59B=({struct _dyneither_ptr*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=(struct _dyneither_ptr)Cyc_current_symbol;_tmp156;});_tmp155->hd=_tmp59B;});_tmp155->tl=Cyc_snarfed_symbols;_tmp155;});Cyc_snarfed_symbols=_tmp59C;});
({struct Cyc_List_List*_tmp59D=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->hd=user_def;_tmp157->tl=Cyc_current_user_defs;_tmp157;});Cyc_current_user_defs=_tmp59D;});
return 1;};}case 2U: _LL5: _LL6:
# 936
 return 1;case 3U: _LL7: _LL8:
# 938
 return 0;case 4U: _LL9: _LLA:
# 940
({void*_tmp15B=0U;({struct Cyc___cycFILE*_tmp59F=Cyc_stderr;struct _dyneither_ptr _tmp59E=({const char*_tmp15C="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmp15C,sizeof(char),44U);});Cyc_fprintf(_tmp59F,_tmp59E,_tag_dyneither(_tmp15B,sizeof(void*),0U));});});
# 942
return 0;case 5U: _LLB: _LLC:
# 944
({struct Cyc_Int_pa_PrintArg_struct _tmp15F;_tmp15F.tag=1U;({unsigned long _tmp5A0=(unsigned long)((int)
# 946
Cyc_Lexing_lexeme_char(lexbuf,0));_tmp15F.f1=_tmp5A0;});({void*_tmp15D[1U]={& _tmp15F};({struct Cyc___cycFILE*_tmp5A2=Cyc_stderr;struct _dyneither_ptr _tmp5A1=({const char*_tmp15E="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmp15E,sizeof(char),57U);});Cyc_fprintf(_tmp5A2,_tmp5A1,_tag_dyneither(_tmp15D,sizeof(void*),1U));});});});
return 0;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL0:;}
# 951
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp160=_cycalloc(sizeof(*_tmp160));({struct Cyc_Lexing_Error_exn_struct _tmp5A4=({struct Cyc_Lexing_Error_exn_struct _tmp161;_tmp161.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5A3=({const char*_tmp162="some action didn't return!";_tag_dyneither(_tmp162,sizeof(char),27U);});_tmp161.f1=_tmp5A3;});_tmp161;});_tmp160[0]=_tmp5A4;});_tmp160;}));}
# 953
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5A5=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5A5;});
{int _tmp163=lexstate;switch(_tmp163){case 0U: _LL1: _LL2:
# 957 "buildlib.cyl"
({void*_tmp164=0U;({struct _dyneither_ptr _tmp5A6=({const char*_tmp165="Warning: unclosed brace\n";_tag_dyneither(_tmp165,sizeof(char),25U);});Cyc_log(_tmp5A6,_tag_dyneither(_tmp164,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 959
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2U: _LL5: _LL6:
# 964
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3U: _LL7: _LL8:
# 968
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 972
({struct Cyc_Buffer_t*_tmp5A7=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5A7,({const char*_tmp166="/*";_tag_dyneither(_tmp166,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 976
({struct Cyc_Buffer_t*_tmp5A8=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5A8,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE: {
# 979
struct _dyneither_ptr _tmp167=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol  && !Cyc_strcmp((struct _dyneither_ptr)_tmp167,(struct _dyneither_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp5A9=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5A9,*Cyc_add_user_prefix(({struct _dyneither_ptr*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=_tmp167;_tmp168;})));});else{
# 983
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tmp167);}
return 1;}case 7U: _LLF: _LL10:
# 986
({struct Cyc_Buffer_t*_tmp5AA=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp5AA,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL11: _LL12:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL0:;}
# 991
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp169=_cycalloc(sizeof(*_tmp169));({struct Cyc_Lexing_Error_exn_struct _tmp5AC=({struct Cyc_Lexing_Error_exn_struct _tmp16A;_tmp16A.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5AB=({const char*_tmp16B="some action didn't return!";_tag_dyneither(_tmp16B,sizeof(char),27U);});_tmp16A.f1=_tmp5AB;});_tmp16A;});_tmp169[0]=_tmp5AC;});_tmp169;}));}
# 993
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5AD=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5AD;});
{int _tmp16C=lexstate;switch(_tmp16C){case 0U: _LL1: _LL2:
# 991 "buildlib.cyl"
({void*_tmp16D=0U;({struct _dyneither_ptr _tmp5AE=({const char*_tmp16E="Warning: unclosed string\n";_tag_dyneither(_tmp16E,sizeof(char),26U);});Cyc_log(_tmp5AE,_tag_dyneither(_tmp16D,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 993
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2U: _LL5: _LL6:
# 995
({void*_tmp16F=0U;({struct _dyneither_ptr _tmp5AF=({const char*_tmp170="Warning: unclosed string\n";_tag_dyneither(_tmp170,sizeof(char),26U);});Cyc_log(_tmp5AF,_tag_dyneither(_tmp16F,sizeof(void*),0U));});});
({struct Cyc_Buffer_t*_tmp5B0=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B0,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3U: _LL7: _LL8:
# 999
({struct Cyc_Buffer_t*_tmp5B1=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B1,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4U: _LL9: _LLA:
# 1002
({struct Cyc_Buffer_t*_tmp5B2=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B2,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5U: _LLB: _LLC:
# 1005
({struct Cyc_Buffer_t*_tmp5B3=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B3,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE:
# 1008
({struct Cyc_Buffer_t*_tmp5B4=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B4,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7U: _LLF: _LL10:
# 1011
({struct Cyc_Buffer_t*_tmp5B5=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B5,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8U: _LL11: _LL12:
# 1014
({struct Cyc_Buffer_t*_tmp5B6=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B6,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL0:;}
# 1019
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp171=_cycalloc(sizeof(*_tmp171));({struct Cyc_Lexing_Error_exn_struct _tmp5B8=({struct Cyc_Lexing_Error_exn_struct _tmp172;_tmp172.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5B7=({const char*_tmp173="some action didn't return!";_tag_dyneither(_tmp173,sizeof(char),27U);});_tmp172.f1=_tmp5B7;});_tmp172;});_tmp171[0]=_tmp5B8;});_tmp171;}));}
# 1021
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5B9=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5B9;});
{int _tmp174=lexstate;switch(_tmp174){case 0U: _LL1: _LL2:
# 1019 "buildlib.cyl"
({void*_tmp175=0U;({struct _dyneither_ptr _tmp5BA=({const char*_tmp176="Warning: unclosed comment\n";_tag_dyneither(_tmp176,sizeof(char),27U);});Cyc_log(_tmp5BA,_tag_dyneither(_tmp175,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 1021
({struct Cyc_Buffer_t*_tmp5BB=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BB,({const char*_tmp177="*/";_tag_dyneither(_tmp177,sizeof(char),3U);}));});return 0;case 2U: _LL5: _LL6:
# 1023
({struct Cyc_Buffer_t*_tmp5BC=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BC,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL0:;}
# 1028
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp178=_cycalloc(sizeof(*_tmp178));({struct Cyc_Lexing_Error_exn_struct _tmp5BE=({struct Cyc_Lexing_Error_exn_struct _tmp179;_tmp179.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5BD=({const char*_tmp17A="some action didn't return!";_tag_dyneither(_tmp17A,sizeof(char),27U);});_tmp179.f1=_tmp5BD;});_tmp179;});_tmp178[0]=_tmp5BE;});_tmp178;}));}
# 1030
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 1036 "buildlib.cyl"
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmp17B=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmp17C=_tmp17B;struct Cyc_List_List*_tmp1CA;void*_tmp1C9;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct _dyneither_ptr*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct _dyneither_ptr*_tmp1C4;void*_tmp1C3;void*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;void**_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BD;void*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_List_List*_tmp1A7;void*_tmp1A6;switch(*((int*)_tmp17C)){case 1U: _LL1: _tmp1A6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL2: {
# 1040
struct _dyneither_ptr*_tmp17D=(*Cyc_Absyn_binding2qvar(_tmp1A6)).f2;
Cyc_add_target(_tmp17D);
return;}case 3U: _LL3: _tmp1A7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL4:
# 1044
 for(0;_tmp1A7 != 0;_tmp1A7=_tmp1A7->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1A7->hd,dep);}
# 1047
return;case 23U: _LL5: _tmp1A9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1A8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL6:
# 1049
 _tmp1AB=_tmp1A9;_tmp1AA=_tmp1A8;goto _LL8;case 9U: _LL7: _tmp1AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL8:
# 1051
 _tmp1AD=_tmp1AB;_tmp1AC=_tmp1AA;goto _LLA;case 4U: _LL9: _tmp1AD=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1AC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17C)->f3;_LLA:
# 1053
 Cyc_scan_exp(_tmp1AD,dep);
Cyc_scan_exp(_tmp1AC,dep);
return;case 20U: _LLB: _tmp1AE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LLC:
# 1057
 _tmp1AF=_tmp1AE;goto _LLE;case 18U: _LLD: _tmp1AF=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LLE:
# 1059
 _tmp1B0=_tmp1AF;goto _LL10;case 15U: _LLF: _tmp1B0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL10:
# 1061
 _tmp1B1=_tmp1B0;goto _LL12;case 5U: _LL11: _tmp1B1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL12:
# 1063
 Cyc_scan_exp(_tmp1B1,dep);
return;case 6U: _LL13: _tmp1B4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_tmp1B2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17C)->f3;_LL14:
# 1066
 Cyc_scan_exp(_tmp1B4,dep);
Cyc_scan_exp(_tmp1B3,dep);
Cyc_scan_exp(_tmp1B2,dep);
return;case 7U: _LL15: _tmp1B6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL16:
 _tmp1B8=_tmp1B6;_tmp1B7=_tmp1B5;goto _LL18;case 8U: _LL17: _tmp1B8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL18:
# 1072
 Cyc_scan_exp(_tmp1B8,dep);
Cyc_scan_exp(_tmp1B7,dep);
return;case 10U: _LL19: _tmp1BA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1A:
# 1076
 Cyc_scan_exp(_tmp1BA,dep);
for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1B9->hd,dep);}
# 1080
return;case 14U: _LL1B: _tmp1BC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1BB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1C:
# 1082
 Cyc_scan_type(_tmp1BC,dep);
Cyc_scan_exp(_tmp1BB,dep);
return;case 34U: _LL1D: _tmp1C0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).is_calloc;_tmp1BF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).rgn;_tmp1BE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).elt_type;_tmp1BD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).num_elts;_LL1E:
# 1086
 if(_tmp1BF != 0)Cyc_scan_exp(_tmp1BF,dep);
if(_tmp1BE != 0)Cyc_scan_type(*_tmp1BE,dep);
Cyc_scan_exp(_tmp1BD,dep);
return;case 38U: _LL1F: _tmp1C1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL20:
# 1091
 Cyc_scan_exp(_tmp1C1,dep);return;case 39U: _LL21: _tmp1C2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL22:
 _tmp1C3=_tmp1C2;goto _LL24;case 17U: _LL23: _tmp1C3=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL24:
# 1094
 Cyc_scan_type(_tmp1C3,dep);
return;case 21U: _LL25: _tmp1C5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1C4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL26:
# 1097
 _tmp1C7=_tmp1C5;_tmp1C6=_tmp1C4;goto _LL28;case 22U: _LL27: _tmp1C7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1C6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL28:
# 1099
 Cyc_scan_exp(_tmp1C7,dep);
Cyc_add_target(_tmp1C6);
return;case 19U: _LL29: _tmp1C9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1C8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL2A:
# 1103
 Cyc_scan_type(_tmp1C9,dep);
# 1105
{void*_tmp17E=(void*)((struct Cyc_List_List*)_check_null(_tmp1C8))->hd;void*_tmp17F=_tmp17E;struct _dyneither_ptr*_tmp180;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp17F)->tag == 0U){_LL54: _tmp180=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp17F)->f1;_LL55:
 Cyc_add_target(_tmp180);goto _LL53;}else{_LL56: _LL57:
 goto _LL53;}_LL53:;}
# 1109
return;case 0U: _LL2B: _LL2C:
# 1111
 return;case 36U: _LL2D: _tmp1CA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL2E:
# 1113
 for(0;_tmp1CA != 0;_tmp1CA=_tmp1CA->tl){
struct _tuple28*_tmp181=(struct _tuple28*)_tmp1CA->hd;struct _tuple28*_tmp182=_tmp181;struct Cyc_Absyn_Exp*_tmp183;_LL59: _tmp183=_tmp182->f2;_LL5A:;
Cyc_scan_exp(_tmp183,dep);}
# 1117
return;case 40U: _LL2F: _LL30:
 return;case 2U: _LL31: _LL32:
# 1120
({void*_tmp184=0U;({struct Cyc___cycFILE*_tmp5C0=Cyc_stderr;struct _dyneither_ptr _tmp5BF=({const char*_tmp185="Error: unexpected Pragma_e\n";_tag_dyneither(_tmp185,sizeof(char),28U);});Cyc_fprintf(_tmp5C0,_tmp5BF,_tag_dyneither(_tmp184,sizeof(void*),0U));});});
 exit(1);return;case 35U: _LL33: _LL34:
# 1123
({void*_tmp186=0U;({struct Cyc___cycFILE*_tmp5C2=Cyc_stderr;struct _dyneither_ptr _tmp5C1=({const char*_tmp187="Error: unexpected Swap_e\n";_tag_dyneither(_tmp187,sizeof(char),26U);});Cyc_fprintf(_tmp5C2,_tmp5C1,_tag_dyneither(_tmp186,sizeof(void*),0U));});});
 exit(1);return;case 37U: _LL35: _LL36:
# 1126
({void*_tmp188=0U;({struct Cyc___cycFILE*_tmp5C4=Cyc_stderr;struct _dyneither_ptr _tmp5C3=({const char*_tmp189="Error: unexpected Stmt_e\n";_tag_dyneither(_tmp189,sizeof(char),26U);});Cyc_fprintf(_tmp5C4,_tmp5C3,_tag_dyneither(_tmp188,sizeof(void*),0U));});});
 exit(1);return;case 11U: _LL37: _LL38:
# 1129
({void*_tmp18A=0U;({struct Cyc___cycFILE*_tmp5C6=Cyc_stderr;struct _dyneither_ptr _tmp5C5=({const char*_tmp18B="Error: unexpected Throw_e\n";_tag_dyneither(_tmp18B,sizeof(char),27U);});Cyc_fprintf(_tmp5C6,_tmp5C5,_tag_dyneither(_tmp18A,sizeof(void*),0U));});});
 exit(1);return;case 12U: _LL39: _LL3A:
# 1132
({void*_tmp18C=0U;({struct Cyc___cycFILE*_tmp5C8=Cyc_stderr;struct _dyneither_ptr _tmp5C7=({const char*_tmp18D="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp18D,sizeof(char),35U);});Cyc_fprintf(_tmp5C8,_tmp5C7,_tag_dyneither(_tmp18C,sizeof(void*),0U));});});
 exit(1);return;case 13U: _LL3B: _LL3C:
# 1135
({void*_tmp18E=0U;({struct Cyc___cycFILE*_tmp5CA=Cyc_stderr;struct _dyneither_ptr _tmp5C9=({const char*_tmp18F="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp18F,sizeof(char),33U);});Cyc_fprintf(_tmp5CA,_tmp5C9,_tag_dyneither(_tmp18E,sizeof(void*),0U));});});
 exit(1);return;case 16U: _LL3D: _LL3E:
# 1138
({void*_tmp190=0U;({struct Cyc___cycFILE*_tmp5CC=Cyc_stderr;struct _dyneither_ptr _tmp5CB=({const char*_tmp191="Error: unexpected New_e\n";_tag_dyneither(_tmp191,sizeof(char),25U);});Cyc_fprintf(_tmp5CC,_tmp5CB,_tag_dyneither(_tmp190,sizeof(void*),0U));});});
 exit(1);return;case 24U: _LL3F: _LL40:
# 1141
({void*_tmp192=0U;({struct Cyc___cycFILE*_tmp5CE=Cyc_stderr;struct _dyneither_ptr _tmp5CD=({const char*_tmp193="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp193,sizeof(char),27U);});Cyc_fprintf(_tmp5CE,_tmp5CD,_tag_dyneither(_tmp192,sizeof(void*),0U));});});
 exit(1);return;case 25U: _LL41: _LL42:
# 1144
({void*_tmp194=0U;({struct Cyc___cycFILE*_tmp5D0=Cyc_stderr;struct _dyneither_ptr _tmp5CF=({const char*_tmp195="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp195,sizeof(char),33U);});Cyc_fprintf(_tmp5D0,_tmp5CF,_tag_dyneither(_tmp194,sizeof(void*),0U));});});
 exit(1);return;case 26U: _LL43: _LL44:
# 1147
({void*_tmp196=0U;({struct Cyc___cycFILE*_tmp5D2=Cyc_stderr;struct _dyneither_ptr _tmp5D1=({const char*_tmp197="Error: unexpected Array_e\n";_tag_dyneither(_tmp197,sizeof(char),27U);});Cyc_fprintf(_tmp5D2,_tmp5D1,_tag_dyneither(_tmp196,sizeof(void*),0U));});});
 exit(1);return;case 27U: _LL45: _LL46:
# 1150
({void*_tmp198=0U;({struct Cyc___cycFILE*_tmp5D4=Cyc_stderr;struct _dyneither_ptr _tmp5D3=({const char*_tmp199="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp199,sizeof(char),35U);});Cyc_fprintf(_tmp5D4,_tmp5D3,_tag_dyneither(_tmp198,sizeof(void*),0U));});});
 exit(1);return;case 28U: _LL47: _LL48:
# 1153
({void*_tmp19A=0U;({struct Cyc___cycFILE*_tmp5D6=Cyc_stderr;struct _dyneither_ptr _tmp5D5=({const char*_tmp19B="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmp19B,sizeof(char),41U);});Cyc_fprintf(_tmp5D6,_tmp5D5,_tag_dyneither(_tmp19A,sizeof(void*),0U));});});
 exit(1);return;case 29U: _LL49: _LL4A:
# 1156
({void*_tmp19C=0U;({struct Cyc___cycFILE*_tmp5D8=Cyc_stderr;struct _dyneither_ptr _tmp5D7=({const char*_tmp19D="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp19D,sizeof(char),31U);});Cyc_fprintf(_tmp5D8,_tmp5D7,_tag_dyneither(_tmp19C,sizeof(void*),0U));});});
 exit(1);return;case 30U: _LL4B: _LL4C:
# 1159
({void*_tmp19E=0U;({struct Cyc___cycFILE*_tmp5DA=Cyc_stderr;struct _dyneither_ptr _tmp5D9=({const char*_tmp19F="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp19F,sizeof(char),32U);});Cyc_fprintf(_tmp5DA,_tmp5D9,_tag_dyneither(_tmp19E,sizeof(void*),0U));});});
 exit(1);return;case 31U: _LL4D: _LL4E:
# 1162
({void*_tmp1A0=0U;({struct Cyc___cycFILE*_tmp5DC=Cyc_stderr;struct _dyneither_ptr _tmp5DB=({const char*_tmp1A1="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp1A1,sizeof(char),30U);});Cyc_fprintf(_tmp5DC,_tmp5DB,_tag_dyneither(_tmp1A0,sizeof(void*),0U));});});
 exit(1);return;case 32U: _LL4F: _LL50:
# 1165
({void*_tmp1A2=0U;({struct Cyc___cycFILE*_tmp5DE=Cyc_stderr;struct _dyneither_ptr _tmp5DD=({const char*_tmp1A3="Error: unexpected Enum_e\n";_tag_dyneither(_tmp1A3,sizeof(char),26U);});Cyc_fprintf(_tmp5DE,_tmp5DD,_tag_dyneither(_tmp1A2,sizeof(void*),0U));});});
 exit(1);return;default: _LL51: _LL52:
# 1168
({void*_tmp1A4=0U;({struct Cyc___cycFILE*_tmp5E0=Cyc_stderr;struct _dyneither_ptr _tmp5DF=({const char*_tmp1A5="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp1A5,sizeof(char),30U);});Cyc_fprintf(_tmp5E0,_tmp5DF,_tag_dyneither(_tmp1A4,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1173
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 1178
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp1CB=t;struct Cyc_Absyn_Datatypedecl*_tmp209;struct Cyc_Absyn_Enumdecl*_tmp208;struct Cyc_Absyn_Aggrdecl*_tmp207;struct _dyneither_ptr*_tmp206;struct _dyneither_ptr*_tmp205;union Cyc_Absyn_AggrInfoU _tmp204;struct Cyc_List_List*_tmp203;struct Cyc_Absyn_FnInfo _tmp202;struct Cyc_Absyn_Exp*_tmp201;void*_tmp200;struct Cyc_Absyn_Exp*_tmp1FF;union Cyc_Absyn_Constraint*_tmp1FE;struct Cyc_Absyn_PtrInfo _tmp1FD;switch(*((int*)_tmp1CB)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 7U: _LL7: _LL8:
# 1185
 return;case 5U: _LL9: _tmp1FD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CB)->f1;_LLA:
# 1188
 Cyc_scan_type(_tmp1FD.elt_typ,dep);
return;case 8U: _LLB: _tmp200=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).elt_type;_tmp1FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).num_elts;_tmp1FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).zero_term;_LLC:
# 1191
 Cyc_scan_type(_tmp200,dep);
Cyc_scan_exp_opt(_tmp1FF,dep);
return;case 27U: _LLD: _tmp201=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1CB)->f1;_LLE:
# 1195
 Cyc_scan_exp(_tmp201,dep);
return;case 9U: _LLF: _tmp202=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CB)->f1;_LL10:
# 1198
 Cyc_scan_type(_tmp202.ret_typ,dep);
{struct Cyc_List_List*_tmp1CC=_tmp202.args;for(0;_tmp1CC != 0;_tmp1CC=_tmp1CC->tl){
struct _tuple9*_tmp1CD=(struct _tuple9*)_tmp1CC->hd;struct _tuple9*_tmp1CE=_tmp1CD;void*_tmp1CF;_LL40: _tmp1CF=_tmp1CE->f3;_LL41:;
Cyc_scan_type(_tmp1CF,dep);}}
# 1203
if(_tmp202.cyc_varargs != 0)
Cyc_scan_type((_tmp202.cyc_varargs)->type,dep);
return;case 12U: _LL11: _tmp203=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1CB)->f2;_LL12:
# 1207
 for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp203->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp203->hd)->width,dep);}
# 1211
return;case 14U: _LL13: _LL14:
# 1213
 return;case 11U: _LL15: _tmp204=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1CB)->f1).aggr_info;_LL16: {
# 1215
struct _tuple11 _tmp1D0=Cyc_Absyn_aggr_kinded_name(_tmp204);struct _tuple11 _tmp1D1=_tmp1D0;struct _dyneither_ptr*_tmp1D2;_LL43: _tmp1D2=(_tmp1D1.f2)->f2;_LL44:;
_tmp205=_tmp1D2;goto _LL18;}case 13U: _LL17: _tmp205=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1CB)->f1)->f2;_LL18:
# 1218
 _tmp206=_tmp205;goto _LL1A;case 17U: _LL19: _tmp206=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1CB)->f1)->f2;_LL1A:
# 1220
 Cyc_add_target(_tmp206);
return;case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1CB)->f1)->r)){case 0U: _LL1B: _tmp207=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1CB)->f1)->r)->f1;_LL1C:
# 1224
({struct Cyc_Absyn_Decl*_tmp5E2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp5E1=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D4;_tmp1D4.tag=5U;_tmp1D4.f1=_tmp207;_tmp1D4;});_tmp1D3[0]=_tmp5E1;});_tmp1D3;}),0U);Cyc_scan_decl(_tmp5E2,dep);});{
struct _tuple1*_tmp1D5=_tmp207->name;struct _tuple1*_tmp1D6=_tmp1D5;struct _dyneither_ptr*_tmp1D7;_LL46: _tmp1D7=_tmp1D6->f2;_LL47:;
Cyc_add_target(_tmp1D7);
return;};case 1U: _LL1D: _tmp208=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1CB)->f1)->r)->f1;_LL1E:
# 1230
({struct Cyc_Absyn_Decl*_tmp5E4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp5E3=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1D9;_tmp1D9.tag=7U;_tmp1D9.f1=_tmp208;_tmp1D9;});_tmp1D8[0]=_tmp5E3;});_tmp1D8;}),0U);Cyc_scan_decl(_tmp5E4,dep);});{
struct _tuple1*_tmp1DA=_tmp208->name;struct _tuple1*_tmp1DB=_tmp1DA;struct _dyneither_ptr*_tmp1DC;_LL49: _tmp1DC=_tmp1DB->f2;_LL4A:;
Cyc_add_target(_tmp1DC);
return;};default: _LL1F: _tmp209=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1CB)->f1)->r)->f1;_LL20:
# 1236
({void*_tmp1DD=0U;({struct Cyc___cycFILE*_tmp5E6=Cyc_stderr;struct _dyneither_ptr _tmp5E5=({const char*_tmp1DE="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp1DE,sizeof(char),40U);});Cyc_fprintf(_tmp5E6,_tmp5E5,_tag_dyneither(_tmp1DD,sizeof(void*),0U));});});
 exit(1);return;}case 1U: _LL21: _LL22:
# 1239
({void*_tmp1DF=0U;({struct Cyc___cycFILE*_tmp5E8=Cyc_stderr;struct _dyneither_ptr _tmp5E7=({const char*_tmp1E0="Error: unexpected Evar\n";_tag_dyneither(_tmp1E0,sizeof(char),24U);});Cyc_fprintf(_tmp5E8,_tmp5E7,_tag_dyneither(_tmp1DF,sizeof(void*),0U));});});
 exit(1);return;case 2U: _LL23: _LL24:
# 1242
({void*_tmp1E1=0U;({struct Cyc___cycFILE*_tmp5EA=Cyc_stderr;struct _dyneither_ptr _tmp5E9=({const char*_tmp1E2="Error: unexpected VarType\n";_tag_dyneither(_tmp1E2,sizeof(char),27U);});Cyc_fprintf(_tmp5EA,_tmp5E9,_tag_dyneither(_tmp1E1,sizeof(void*),0U));});});
 exit(1);return;case 3U: _LL25: _LL26:
# 1245
({void*_tmp1E3=0U;({struct Cyc___cycFILE*_tmp5EC=Cyc_stderr;struct _dyneither_ptr _tmp5EB=({const char*_tmp1E4="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp1E4,sizeof(char),32U);});Cyc_fprintf(_tmp5EC,_tmp5EB,_tag_dyneither(_tmp1E3,sizeof(void*),0U));});});
 exit(1);return;case 4U: _LL27: _LL28:
# 1248
({void*_tmp1E5=0U;({struct Cyc___cycFILE*_tmp5EE=Cyc_stderr;struct _dyneither_ptr _tmp5ED=({const char*_tmp1E6="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp1E6,sizeof(char),37U);});Cyc_fprintf(_tmp5EE,_tmp5ED,_tag_dyneither(_tmp1E5,sizeof(void*),0U));});});
 exit(1);return;case 10U: _LL29: _LL2A:
# 1251
({void*_tmp1E7=0U;({struct Cyc___cycFILE*_tmp5F0=Cyc_stderr;struct _dyneither_ptr _tmp5EF=({const char*_tmp1E8="Error: unexpected TupleType\n";_tag_dyneither(_tmp1E8,sizeof(char),29U);});Cyc_fprintf(_tmp5F0,_tmp5EF,_tag_dyneither(_tmp1E7,sizeof(void*),0U));});});
 exit(1);return;case 15U: _LL2B: _LL2C:
# 1254
({void*_tmp1E9=0U;({struct Cyc___cycFILE*_tmp5F2=Cyc_stderr;struct _dyneither_ptr _tmp5F1=({const char*_tmp1EA="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp1EA,sizeof(char),33U);});Cyc_fprintf(_tmp5F2,_tmp5F1,_tag_dyneither(_tmp1E9,sizeof(void*),0U));});});
 exit(1);return;case 16U: _LL2D: _LL2E:
# 1257
({void*_tmp1EB=0U;({struct Cyc___cycFILE*_tmp5F4=Cyc_stderr;struct _dyneither_ptr _tmp5F3=({const char*_tmp1EC="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp1EC,sizeof(char),30U);});Cyc_fprintf(_tmp5F4,_tmp5F3,_tag_dyneither(_tmp1EB,sizeof(void*),0U));});});
 exit(1);return;case 20U: _LL2F: _LL30:
# 1260
({void*_tmp1ED=0U;({struct Cyc___cycFILE*_tmp5F6=Cyc_stderr;struct _dyneither_ptr _tmp5F5=({const char*_tmp1EE="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp1EE,sizeof(char),27U);});Cyc_fprintf(_tmp5F6,_tmp5F5,_tag_dyneither(_tmp1ED,sizeof(void*),0U));});});
 exit(1);return;case 21U: _LL31: _LL32:
# 1263
({void*_tmp1EF=0U;({struct Cyc___cycFILE*_tmp5F8=Cyc_stderr;struct _dyneither_ptr _tmp5F7=({const char*_tmp1F0="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp1F0,sizeof(char),29U);});Cyc_fprintf(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp1EF,sizeof(void*),0U));});});
 exit(1);return;case 22U: _LL33: _LL34:
# 1266
({void*_tmp1F1=0U;({struct Cyc___cycFILE*_tmp5FA=Cyc_stderr;struct _dyneither_ptr _tmp5F9=({const char*_tmp1F2="Error: unexpected RefCntRgn\n";_tag_dyneither(_tmp1F2,sizeof(char),29U);});Cyc_fprintf(_tmp5FA,_tmp5F9,_tag_dyneither(_tmp1F1,sizeof(void*),0U));});});
 exit(1);return;case 23U: _LL35: _LL36:
# 1269
({void*_tmp1F3=0U;({struct Cyc___cycFILE*_tmp5FC=Cyc_stderr;struct _dyneither_ptr _tmp5FB=({const char*_tmp1F4="Error: unexpected AccessEff\n";_tag_dyneither(_tmp1F4,sizeof(char),29U);});Cyc_fprintf(_tmp5FC,_tmp5FB,_tag_dyneither(_tmp1F3,sizeof(void*),0U));});});
 exit(1);return;case 24U: _LL37: _LL38:
# 1272
({void*_tmp1F5=0U;({struct Cyc___cycFILE*_tmp5FE=Cyc_stderr;struct _dyneither_ptr _tmp5FD=({const char*_tmp1F6="Error: unexpected JoinEff\n";_tag_dyneither(_tmp1F6,sizeof(char),27U);});Cyc_fprintf(_tmp5FE,_tmp5FD,_tag_dyneither(_tmp1F5,sizeof(void*),0U));});});
 exit(1);return;case 25U: _LL39: _LL3A:
# 1275
({void*_tmp1F7=0U;({struct Cyc___cycFILE*_tmp600=Cyc_stderr;struct _dyneither_ptr _tmp5FF=({const char*_tmp1F8="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp1F8,sizeof(char),27U);});Cyc_fprintf(_tmp600,_tmp5FF,_tag_dyneither(_tmp1F7,sizeof(void*),0U));});});
 exit(1);return;case 19U: _LL3B: _LL3C:
# 1278
({void*_tmp1F9=0U;({struct Cyc___cycFILE*_tmp602=Cyc_stderr;struct _dyneither_ptr _tmp601=({const char*_tmp1FA="Error: unexpected tag_t\n";_tag_dyneither(_tmp1FA,sizeof(char),25U);});Cyc_fprintf(_tmp602,_tmp601,_tag_dyneither(_tmp1F9,sizeof(void*),0U));});});
 exit(1);return;default: _LL3D: _LL3E:
# 1281
({void*_tmp1FB=0U;({struct Cyc___cycFILE*_tmp604=Cyc_stderr;struct _dyneither_ptr _tmp603=({const char*_tmp1FC="Error: unexpected valueof_t\n";_tag_dyneither(_tmp1FC,sizeof(char),29U);});Cyc_fprintf(_tmp604,_tmp603,_tag_dyneither(_tmp1FB,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1286
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp20A=Cyc_current_targets;
struct _dyneither_ptr*_tmp20B=Cyc_current_source;
({struct Cyc_Set_Set**_tmp606=({struct Cyc_Set_Set**_tmp20C=_cycalloc(sizeof(*_tmp20C));({struct Cyc_Set_Set*_tmp605=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp20C[0]=_tmp605;});_tmp20C;});Cyc_current_targets=_tmp606;});
{void*_tmp20D=d->r;void*_tmp20E=_tmp20D;struct Cyc_Absyn_Typedefdecl*_tmp242;struct Cyc_Absyn_Enumdecl*_tmp241;struct Cyc_Absyn_Aggrdecl*_tmp240;struct Cyc_Absyn_Fndecl*_tmp23F;struct Cyc_Absyn_Vardecl*_tmp23E;switch(*((int*)_tmp20E)){case 0U: _LL1: _tmp23E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp20E)->f1;_LL2: {
# 1292
struct _tuple1*_tmp20F=_tmp23E->name;struct _tuple1*_tmp210=_tmp20F;struct _dyneither_ptr*_tmp211;_LL20: _tmp211=_tmp210->f2;_LL21:;
Cyc_current_source=_tmp211;
Cyc_scan_type(_tmp23E->type,dep);
Cyc_scan_exp_opt(_tmp23E->initializer,dep);
goto _LL0;}case 1U: _LL3: _tmp23F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp20E)->f1;_LL4: {
# 1298
struct _tuple1*_tmp212=_tmp23F->name;struct _tuple1*_tmp213=_tmp212;struct _dyneither_ptr*_tmp21A;_LL23: _tmp21A=_tmp213->f2;_LL24:;
Cyc_current_source=_tmp21A;
Cyc_scan_type(_tmp23F->ret_type,dep);
{struct Cyc_List_List*_tmp214=_tmp23F->args;for(0;_tmp214 != 0;_tmp214=_tmp214->tl){
struct _tuple9*_tmp215=(struct _tuple9*)_tmp214->hd;struct _tuple9*_tmp216=_tmp215;void*_tmp217;_LL26: _tmp217=_tmp216->f3;_LL27:;
Cyc_scan_type(_tmp217,dep);}}
# 1305
if(_tmp23F->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp23F->cyc_varargs))->type,dep);
if(_tmp23F->is_inline)
({void*_tmp218=0U;({struct Cyc___cycFILE*_tmp608=Cyc_stderr;struct _dyneither_ptr _tmp607=({const char*_tmp219="Warning: ignoring inline function\n";_tag_dyneither(_tmp219,sizeof(char),35U);});Cyc_fprintf(_tmp608,_tmp607,_tag_dyneither(_tmp218,sizeof(void*),0U));});});
# 1310
goto _LL0;}case 5U: _LL5: _tmp240=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp20E)->f1;_LL6: {
# 1312
struct _tuple1*_tmp21B=_tmp240->name;struct _tuple1*_tmp21C=_tmp21B;struct _dyneither_ptr*_tmp220;_LL29: _tmp220=_tmp21C->f2;_LL2A:;
Cyc_current_source=_tmp220;
if((unsigned int)_tmp240->impl){
{struct Cyc_List_List*_tmp21D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp240->impl))->fields;for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){
struct Cyc_Absyn_Aggrfield*_tmp21E=(struct Cyc_Absyn_Aggrfield*)_tmp21D->hd;
Cyc_scan_type(_tmp21E->type,dep);
Cyc_scan_exp_opt(_tmp21E->width,dep);}}{
# 1322
struct Cyc_List_List*_tmp21F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp240->impl))->fields;for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){;}};}
# 1326
goto _LL0;}case 7U: _LL7: _tmp241=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp20E)->f1;_LL8: {
# 1328
struct _tuple1*_tmp221=_tmp241->name;struct _tuple1*_tmp222=_tmp221;struct _dyneither_ptr*_tmp226;_LL2C: _tmp226=_tmp222->f2;_LL2D:;
Cyc_current_source=_tmp226;
if((unsigned int)_tmp241->fields){
{struct Cyc_List_List*_tmp223=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp241->fields))->v;for(0;_tmp223 != 0;_tmp223=_tmp223->tl){
struct Cyc_Absyn_Enumfield*_tmp224=(struct Cyc_Absyn_Enumfield*)_tmp223->hd;
Cyc_scan_exp_opt(_tmp224->tag,dep);}}{
# 1337
struct Cyc_List_List*_tmp225=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp241->fields))->v;for(0;_tmp225 != 0;_tmp225=_tmp225->tl){;}};}
# 1341
goto _LL0;}case 8U: _LL9: _tmp242=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp20E)->f1;_LLA: {
# 1343
struct _tuple1*_tmp227=_tmp242->name;struct _tuple1*_tmp228=_tmp227;struct _dyneither_ptr*_tmp229;_LL2F: _tmp229=_tmp228->f2;_LL30:;
Cyc_current_source=_tmp229;
if((unsigned int)_tmp242->defn)
Cyc_scan_type((void*)_check_null(_tmp242->defn),dep);
goto _LL0;}case 4U: _LLB: _LLC:
# 1349
({void*_tmp22A=0U;({struct Cyc___cycFILE*_tmp60A=Cyc_stderr;struct _dyneither_ptr _tmp609=({const char*_tmp22B="Error: unexpected region declaration";_tag_dyneither(_tmp22B,sizeof(char),37U);});Cyc_fprintf(_tmp60A,_tmp609,_tag_dyneither(_tmp22A,sizeof(void*),0U));});});
 exit(1);case 13U: _LLD: _LLE:
# 1352
({void*_tmp22C=0U;({struct Cyc___cycFILE*_tmp60C=Cyc_stderr;struct _dyneither_ptr _tmp60B=({const char*_tmp22D="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp22D,sizeof(char),38U);});Cyc_fprintf(_tmp60C,_tmp60B,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
 exit(1);case 14U: _LLF: _LL10:
# 1355
({void*_tmp22E=0U;({struct Cyc___cycFILE*_tmp60E=Cyc_stderr;struct _dyneither_ptr _tmp60D=({const char*_tmp22F="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp22F,sizeof(char),39U);});Cyc_fprintf(_tmp60E,_tmp60D,_tag_dyneither(_tmp22E,sizeof(void*),0U));});});
 exit(1);case 2U: _LL11: _LL12:
# 1358
({void*_tmp230=0U;({struct Cyc___cycFILE*_tmp610=Cyc_stderr;struct _dyneither_ptr _tmp60F=({const char*_tmp231="Error: unexpected let declaration\n";_tag_dyneither(_tmp231,sizeof(char),35U);});Cyc_fprintf(_tmp610,_tmp60F,_tag_dyneither(_tmp230,sizeof(void*),0U));});});
 exit(1);case 6U: _LL13: _LL14:
# 1361
({void*_tmp232=0U;({struct Cyc___cycFILE*_tmp612=Cyc_stderr;struct _dyneither_ptr _tmp611=({const char*_tmp233="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp233,sizeof(char),40U);});Cyc_fprintf(_tmp612,_tmp611,_tag_dyneither(_tmp232,sizeof(void*),0U));});});
 exit(1);case 3U: _LL15: _LL16:
# 1364
({void*_tmp234=0U;({struct Cyc___cycFILE*_tmp614=Cyc_stderr;struct _dyneither_ptr _tmp613=({const char*_tmp235="Error: unexpected let declaration\n";_tag_dyneither(_tmp235,sizeof(char),35U);});Cyc_fprintf(_tmp614,_tmp613,_tag_dyneither(_tmp234,sizeof(void*),0U));});});
 exit(1);case 9U: _LL17: _LL18:
# 1367
({void*_tmp236=0U;({struct Cyc___cycFILE*_tmp616=Cyc_stderr;struct _dyneither_ptr _tmp615=({const char*_tmp237="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp237,sizeof(char),41U);});Cyc_fprintf(_tmp616,_tmp615,_tag_dyneither(_tmp236,sizeof(void*),0U));});});
 exit(1);case 10U: _LL19: _LL1A:
# 1370
({void*_tmp238=0U;({struct Cyc___cycFILE*_tmp618=Cyc_stderr;struct _dyneither_ptr _tmp617=({const char*_tmp239="Error: unexpected using declaration\n";_tag_dyneither(_tmp239,sizeof(char),37U);});Cyc_fprintf(_tmp618,_tmp617,_tag_dyneither(_tmp238,sizeof(void*),0U));});});
 exit(1);case 11U: _LL1B: _LL1C:
# 1373
({void*_tmp23A=0U;({struct Cyc___cycFILE*_tmp61A=Cyc_stderr;struct _dyneither_ptr _tmp619=({const char*_tmp23B="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp23B,sizeof(char),42U);});Cyc_fprintf(_tmp61A,_tmp619,_tag_dyneither(_tmp23A,sizeof(void*),0U));});});
 exit(1);default: _LL1D: _LL1E:
# 1376
({void*_tmp23C=0U;({struct Cyc___cycFILE*_tmp61C=Cyc_stderr;struct _dyneither_ptr _tmp61B=({const char*_tmp23D="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp23D,sizeof(char),50U);});Cyc_fprintf(_tmp61C,_tmp61B,_tag_dyneither(_tmp23C,sizeof(void*),0U));});});
 exit(1);}_LL0:;}{
# 1384
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp243=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp244;_push_handler(& _tmp244);{int _tmp246=0;if(setjmp(_tmp244.handler))_tmp246=1;if(!_tmp246){
({struct Cyc_Set_Set*_tmp61D=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp243);old=_tmp61D;});;_pop_handler();}else{void*_tmp245=(void*)_exn_thrown;void*_tmp247=_tmp245;void*_tmp248;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp247)->tag == Cyc_Core_Not_found){_LL32: _LL33:
# 1389
({struct Cyc_Set_Set*_tmp61E=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);old=_tmp61E;});goto _LL31;}else{_LL34: _tmp248=_tmp247;_LL35:(int)_rethrow(_tmp248);}_LL31:;}};}{
# 1391
struct Cyc_Set_Set*_tmp249=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp243,_tmp249);
# 1394
Cyc_current_targets=_tmp20A;
Cyc_current_source=_tmp20B;};};}
# 1398
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1402
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp24A;_push_handler(& _tmp24A);{int _tmp24C=0;if(setjmp(_tmp24A.handler))_tmp24C=1;if(!_tmp24C){{struct Cyc_Set_Set*_tmp24D=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0U);return _tmp24D;};_pop_handler();}else{void*_tmp24B=(void*)_exn_thrown;void*_tmp24E=_tmp24B;void*_tmp24F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp24E)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 1405
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL3: _tmp24F=_tmp24E;_LL4:(int)_rethrow(_tmp24F);}_LL0:;}};}
# 1409
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1419 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
({struct Cyc_Set_Set*_tmp61F=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);curr=_tmp61F;});}{
# 1424
struct Cyc_Set_Set*_tmp250=curr;
# 1426
struct _dyneither_ptr*_tmp251=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1U);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp250)> 0){
struct Cyc_Set_Set*_tmp252=emptyset;
struct Cyc_Iter_Iter _tmp253=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp250);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp253,& _tmp251)){
({struct Cyc_Set_Set*_tmp621=({struct Cyc_Set_Set*_tmp620=_tmp252;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp620,Cyc_find(t,_tmp251));});_tmp252=_tmp621;});}
({struct Cyc_Set_Set*_tmp622=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp252,curr);_tmp250=_tmp622;});
({struct Cyc_Set_Set*_tmp623=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp250);curr=_tmp623;});}
# 1435
return curr;};}
# 1438
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0U,Cyc_GATHER  = 1U,Cyc_GATHERSCRIPT  = 2U,Cyc_FINISH  = 3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1444
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1448
if(Cyc_script_file == 0){
({void*_tmp255=0U;({struct Cyc___cycFILE*_tmp625=Cyc_stderr;struct _dyneither_ptr _tmp624=({const char*_tmp256="Internal error: script file is NULL\n";_tag_dyneither(_tmp256,sizeof(char),37U);});Cyc_fprintf(_tmp625,_tmp624,_tag_dyneither(_tmp255,sizeof(void*),0U));});});
 exit(1);}
# 1452
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1455
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp25A;_tmp25A.tag=0U;_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({struct Cyc_String_pa_PrintArg_struct _tmp259;_tmp259.tag=0U;_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp257[2U]={& _tmp259,& _tmp25A};({struct _dyneither_ptr _tmp626=({const char*_tmp258="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp258,sizeof(char),36U);});Cyc_prscript(_tmp626,_tag_dyneither(_tmp257,sizeof(void*),2U));});});});});else{
# 1462
int _tmp25B=({unsigned short _tmp25F[0U];({const char*_tmp627=(const char*)_untag_dyneither_ptr(d,sizeof(char),1U);Cyc_open(_tmp627,0,_tag_dyneither(_tmp25F,sizeof(unsigned short),0U));});});
if(_tmp25B == - 1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1U)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp25E;_tmp25E.tag=0U;_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp25C[1U]={& _tmp25E};({struct Cyc___cycFILE*_tmp629=Cyc_stderr;struct _dyneither_ptr _tmp628=({const char*_tmp25D="Error: could not create directory %s\n";_tag_dyneither(_tmp25D,sizeof(char),38U);});Cyc_fprintf(_tmp629,_tmp628,_tag_dyneither(_tmp25C,sizeof(void*),1U));});});});
return 1;}}else{
# 1469
 close(_tmp25B);}}
# 1471
return 0;}
# 1474
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1478
struct _dyneither_ptr _tmp260=Cyc_strdup((struct _dyneither_ptr)file);
# 1480
struct Cyc_List_List*_tmp261=0;
while(1){
({struct _dyneither_ptr _tmp62A=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp260);_tmp260=_tmp62A;});
if(Cyc_strlen((struct _dyneither_ptr)_tmp260)== 0)break;
({struct Cyc_List_List*_tmp62C=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));({struct _dyneither_ptr*_tmp62B=({struct _dyneither_ptr*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=(struct _dyneither_ptr)_tmp260;_tmp263;});_tmp262->hd=_tmp62B;});_tmp262->tl=_tmp261;_tmp262;});_tmp261=_tmp62C;});}
# 1487
for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp261->hd)))return 1;}
# 1490
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1497
static int Cyc_is_other_special(char c){
char _tmp264=c;switch(_tmp264){case 92U: _LL1: _LL2:
 goto _LL4;case 34U: _LL3: _LL4:
 goto _LL6;case 59U: _LL5: _LL6:
 goto _LL8;case 38U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 41U: _LLB: _LLC:
 goto _LLE;case 124U: _LLD: _LLE:
 goto _LL10;case 94U: _LLF: _LL10:
 goto _LL12;case 60U: _LL11: _LL12:
 goto _LL14;case 62U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
# 1512
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 1518
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp265=Cyc_strlen((struct _dyneither_ptr)s);
# 1522
int _tmp266=0;
int _tmp267=0;
{int i=0;for(0;i < _tmp265;++ i){
char _tmp268=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp268 == '\'')++ _tmp266;else{
if(Cyc_is_other_special(_tmp268))++ _tmp267;}}}
# 1531
if(_tmp266 == 0  && _tmp267 == 0)
return s;
# 1535
if(_tmp266 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp269[3U];({struct _dyneither_ptr*_tmp62D=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2U);_tmp269[2U]=_tmp62D;});({struct _dyneither_ptr*_tmp62E=({struct _dyneither_ptr*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=(struct _dyneither_ptr)s;_tmp26B;});_tmp269[1U]=_tmp62E;});({struct _dyneither_ptr*_tmp62F=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2U);_tmp269[0U]=_tmp62F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp269,sizeof(struct _dyneither_ptr*),3U));}));{
# 1539
unsigned long _tmp26D=(_tmp265 + _tmp266)+ _tmp267;
struct _dyneither_ptr s2=({unsigned int _tmp277=_tmp26D + 1;char*_tmp278=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp277 + 1U));struct _dyneither_ptr _tmp27A=_tag_dyneither(_tmp278,sizeof(char),_tmp277 + 1U);{unsigned int _tmp279=_tmp277;unsigned int i;for(i=0;i < _tmp279;i ++){_tmp278[i]=(char)'\000';}_tmp278[_tmp279]=(char)0U;}_tmp27A;});
int _tmp26E=0;
int _tmp26F=0;
for(0;_tmp26E < _tmp265;++ _tmp26E){
char _tmp270=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp26E));
if(_tmp270 == '\''  || Cyc_is_other_special(_tmp270))
({struct _dyneither_ptr _tmp271=_dyneither_ptr_plus(s2,sizeof(char),_tmp26F ++);char _tmp272=*((char*)_check_dyneither_subscript(_tmp271,sizeof(char),0U));char _tmp273='\\';if(_get_dyneither_size(_tmp271,sizeof(char))== 1U  && (_tmp272 == '\000'  && _tmp273 != '\000'))_throw_arraybounds();*((char*)_tmp271.curr)=_tmp273;});
({struct _dyneither_ptr _tmp274=_dyneither_ptr_plus(s2,sizeof(char),_tmp26F ++);char _tmp275=*((char*)_check_dyneither_subscript(_tmp274,sizeof(char),0U));char _tmp276=_tmp270;if(_get_dyneither_size(_tmp274,sizeof(char))== 1U  && (_tmp275 == '\000'  && _tmp276 != '\000'))_throw_arraybounds();*((char*)_tmp274.curr)=_tmp276;});}
# 1549
return(struct _dyneither_ptr)s2;};}
# 1551
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp27B=_cycalloc(sizeof(*_tmp27B));({struct _dyneither_ptr _tmp630=Cyc_sh_escape_string(*sp);_tmp27B[0]=_tmp630;});_tmp27B;});}
# 1556
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1563
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1568
({struct Cyc_String_pa_PrintArg_struct _tmp27E;_tmp27E.tag=0U;({struct _dyneither_ptr _tmp631=(struct _dyneither_ptr)({const char*_tmp27F=filename;_tag_dyneither(_tmp27F,sizeof(char),_get_zero_arr_size_char((void*)_tmp27F,1U));});_tmp27E.f1=_tmp631;});({void*_tmp27C[1U]={& _tmp27E};({struct Cyc___cycFILE*_tmp633=Cyc_stderr;struct _dyneither_ptr _tmp632=({const char*_tmp27D="********************************* %s...\n";_tag_dyneither(_tmp27D,sizeof(char),41U);});Cyc_fprintf(_tmp633,_tmp632,_tag_dyneither(_tmp27C,sizeof(void*),1U));});});});
# 1571
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp282;_tmp282.tag=0U;({struct _dyneither_ptr _tmp634=(struct _dyneither_ptr)({const char*_tmp283=filename;_tag_dyneither(_tmp283,sizeof(char),_get_zero_arr_size_char((void*)_tmp283,1U));});_tmp282.f1=_tmp634;});({void*_tmp280[1U]={& _tmp282};({struct _dyneither_ptr _tmp635=({const char*_tmp281="\n%s:\n";_tag_dyneither(_tmp281,sizeof(char),6U);});Cyc_log(_tmp635,_tag_dyneither(_tmp280,sizeof(void*),1U));});});});{
# 1583 "buildlib.cyl"
struct _dyneither_ptr _tmp284=Cyc_Filename_basename(({const char*_tmp3D8=filename;_tag_dyneither(_tmp3D8,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D8,1U));}));
struct _dyneither_ptr _tmp285=Cyc_Filename_dirname(({const char*_tmp3D7=filename;_tag_dyneither(_tmp3D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D7,1U));}));
struct _dyneither_ptr _tmp286=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp284);
const char*_tmp287=(const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp636=(struct _dyneither_ptr)_tmp286;Cyc_strconcat(_tmp636,({const char*_tmp3D6=".iA";_tag_dyneither(_tmp3D6,sizeof(char),4U);}));}),sizeof(char),1U));
# 1588
const char*_tmp288=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp285,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U;_tmp3D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286);({void*_tmp3D0[1U]={& _tmp3D2};({struct _dyneither_ptr _tmp639=({const char*_tmp3D1="%s.iB";_tag_dyneither(_tmp3D1,sizeof(char),6U);});Cyc_aprintf(_tmp639,_tag_dyneither(_tmp3D0,sizeof(void*),1U));});});}):({struct _dyneither_ptr _tmp638=(struct _dyneither_ptr)_tmp285;Cyc_Filename_concat(_tmp638,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0U;_tmp3D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286);({void*_tmp3D3[1U]={& _tmp3D5};({struct _dyneither_ptr _tmp637=({const char*_tmp3D4="%s.iB";_tag_dyneither(_tmp3D4,sizeof(char),6U);});Cyc_aprintf(_tmp637,_tag_dyneither(_tmp3D3,sizeof(void*),1U));});});}));}),sizeof(char),1U));
# 1593
const char*_tmp289=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp285,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=0U;_tmp3CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286);({void*_tmp3CA[1U]={& _tmp3CC};({struct _dyneither_ptr _tmp63C=({const char*_tmp3CB="%s.iC";_tag_dyneither(_tmp3CB,sizeof(char),6U);});Cyc_aprintf(_tmp63C,_tag_dyneither(_tmp3CA,sizeof(void*),1U));});});}):({struct _dyneither_ptr _tmp63B=(struct _dyneither_ptr)_tmp285;Cyc_Filename_concat(_tmp63B,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0U;_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286);({void*_tmp3CD[1U]={& _tmp3CF};({struct _dyneither_ptr _tmp63A=({const char*_tmp3CE="%s.iC";_tag_dyneither(_tmp3CE,sizeof(char),6U);});Cyc_aprintf(_tmp63A,_tag_dyneither(_tmp3CD,sizeof(void*),1U));});});}));}),sizeof(char),1U));
# 1598
const char*_tmp28A=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp285,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=0U;_tmp3C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286);({void*_tmp3C4[1U]={& _tmp3C6};({struct _dyneither_ptr _tmp63F=({const char*_tmp3C5="%s.iD";_tag_dyneither(_tmp3C5,sizeof(char),6U);});Cyc_aprintf(_tmp63F,_tag_dyneither(_tmp3C4,sizeof(void*),1U));});});}):({struct _dyneither_ptr _tmp63E=(struct _dyneither_ptr)_tmp285;Cyc_Filename_concat(_tmp63E,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0U;_tmp3C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286);({void*_tmp3C7[1U]={& _tmp3C9};({struct _dyneither_ptr _tmp63D=({const char*_tmp3C8="%s.iD";_tag_dyneither(_tmp3C8,sizeof(char),6U);});Cyc_aprintf(_tmp63D,_tag_dyneither(_tmp3C7,sizeof(void*),1U));});});}));}),sizeof(char),1U));
# 1603
{struct _handler_cons _tmp28B;_push_handler(& _tmp28B);{int _tmp28D=0;if(setjmp(_tmp28B.handler))_tmp28D=1;if(!_tmp28D){
# 1606
if(Cyc_force_directory_prefixes(({const char*_tmp28E=filename;_tag_dyneither(_tmp28E,sizeof(char),_get_zero_arr_size_char((void*)_tmp28E,1U));}))){
int _tmp28F=1;_npop_handler(0U);return _tmp28F;}
# 1611
if(Cyc_mode != Cyc_FINISH){
({struct Cyc_List_List*_tmp640=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);Cyc_current_cpp=_tmp640;});
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp292;_tmp292.tag=0U;({struct _dyneither_ptr _tmp641=(struct _dyneither_ptr)({const char*_tmp293=_tmp287;_tag_dyneither(_tmp293,sizeof(char),_get_zero_arr_size_char((void*)_tmp293,1U));});_tmp292.f1=_tmp641;});({void*_tmp290[1U]={& _tmp292};({struct _dyneither_ptr _tmp642=({const char*_tmp291="cat >%s <<XXX\n";_tag_dyneither(_tmp291,sizeof(char),15U);});Cyc_prscript(_tmp642,_tag_dyneither(_tmp290,sizeof(void*),1U));});});});
{struct Cyc_List_List*_tmp294=Cyc_current_cpp;for(0;_tmp294 != 0;_tmp294=_tmp294->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0U;_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp294->hd));({void*_tmp295[1U]={& _tmp297};({struct _dyneither_ptr _tmp643=({const char*_tmp296="%s";_tag_dyneither(_tmp296,sizeof(char),3U);});Cyc_prscript(_tmp643,_tag_dyneither(_tmp295,sizeof(void*),1U));});});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp29A;_tmp29A.tag=0U;({struct _dyneither_ptr _tmp644=(struct _dyneither_ptr)({const char*_tmp29B=filename;_tag_dyneither(_tmp29B,sizeof(char),_get_zero_arr_size_char((void*)_tmp29B,1U));});_tmp29A.f1=_tmp644;});({void*_tmp298[1U]={& _tmp29A};({struct _dyneither_ptr _tmp645=({const char*_tmp299="#include <%s>\n";_tag_dyneither(_tmp299,sizeof(char),15U);});Cyc_prscript(_tmp645,_tag_dyneither(_tmp298,sizeof(void*),1U));});});});
({void*_tmp29C=0U;({struct _dyneither_ptr _tmp646=({const char*_tmp29D="XXX\n";_tag_dyneither(_tmp29D,sizeof(char),5U);});Cyc_prscript(_tmp646,_tag_dyneither(_tmp29C,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0U;({struct _dyneither_ptr _tmp647=(struct _dyneither_ptr)({const char*_tmp2A4=_tmp287;_tag_dyneither(_tmp2A4,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A4,1U));});_tmp2A2.f1=_tmp647;});({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0U;({struct _dyneither_ptr _tmp648=(struct _dyneither_ptr)({const char*_tmp2A3=_tmp288;_tag_dyneither(_tmp2A3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A3,1U));});_tmp2A1.f1=_tmp648;});({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0U;_tmp2A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp29E[3U]={& _tmp2A0,& _tmp2A1,& _tmp2A2};({struct _dyneither_ptr _tmp649=({const char*_tmp29F="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp29F,sizeof(char),35U);});Cyc_prscript(_tmp649,_tag_dyneither(_tmp29E,sizeof(void*),3U));});});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0U;({struct _dyneither_ptr _tmp64A=(struct _dyneither_ptr)({const char*_tmp2AB=_tmp287;_tag_dyneither(_tmp2AB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AB,1U));});_tmp2A9.f1=_tmp64A;});({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0U;({struct _dyneither_ptr _tmp64B=(struct _dyneither_ptr)({const char*_tmp2AA=_tmp289;_tag_dyneither(_tmp2AA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AA,1U));});_tmp2A8.f1=_tmp64B;});({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0U;_tmp2A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp2A5[3U]={& _tmp2A7,& _tmp2A8,& _tmp2A9};({struct _dyneither_ptr _tmp64C=({const char*_tmp2A6="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp2A6,sizeof(char),31U);});Cyc_prscript(_tmp64C,_tag_dyneither(_tmp2A5,sizeof(void*),3U));});});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0U;({struct _dyneither_ptr _tmp64D=(struct _dyneither_ptr)({const char*_tmp2AF=_tmp287;_tag_dyneither(_tmp2AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AF,1U));});_tmp2AE.f1=_tmp64D;});({void*_tmp2AC[1U]={& _tmp2AE};({struct _dyneither_ptr _tmp64E=({const char*_tmp2AD="rm %s\n";_tag_dyneither(_tmp2AD,sizeof(char),7U);});Cyc_prscript(_tmp64E,_tag_dyneither(_tmp2AC,sizeof(void*),1U));});});});}else{
# 1624
({struct Cyc___cycFILE*_tmp64F=Cyc_fopen(_tmp287,"w");maybe=_tmp64F;});
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2B2;_tmp2B2.tag=0U;({struct _dyneither_ptr _tmp650=(struct _dyneither_ptr)({const char*_tmp2B3=_tmp287;_tag_dyneither(_tmp2B3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B3,1U));});_tmp2B2.f1=_tmp650;});({void*_tmp2B0[1U]={& _tmp2B2};({struct Cyc___cycFILE*_tmp652=Cyc_stderr;struct _dyneither_ptr _tmp651=({const char*_tmp2B1="Error: could not create file %s\n";_tag_dyneither(_tmp2B1,sizeof(char),33U);});Cyc_fprintf(_tmp652,_tmp651,_tag_dyneither(_tmp2B0,sizeof(void*),1U));});});});{
int _tmp2B4=1;_npop_handler(0U);return _tmp2B4;};}
# 1629
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2B7;_tmp2B7.tag=0U;({struct _dyneither_ptr _tmp653=(struct _dyneither_ptr)({const char*_tmp2B8=_tmp287;_tag_dyneither(_tmp2B8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B8,1U));});_tmp2B7.f1=_tmp653;});({void*_tmp2B5[1U]={& _tmp2B7};({struct Cyc___cycFILE*_tmp655=Cyc_stderr;struct _dyneither_ptr _tmp654=({const char*_tmp2B6="Creating %s\n";_tag_dyneither(_tmp2B6,sizeof(char),13U);});Cyc_fprintf(_tmp655,_tmp654,_tag_dyneither(_tmp2B5,sizeof(void*),1U));});});});
out_file=maybe;
{struct Cyc_List_List*_tmp2B9=Cyc_current_cpp;for(0;_tmp2B9 != 0;_tmp2B9=_tmp2B9->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp2B9->hd),sizeof(char),1U)),out_file);}}
# 1635
({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0U;({struct _dyneither_ptr _tmp656=(struct _dyneither_ptr)({const char*_tmp2BD=filename;_tag_dyneither(_tmp2BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BD,1U));});_tmp2BC.f1=_tmp656;});({void*_tmp2BA[1U]={& _tmp2BC};({struct Cyc___cycFILE*_tmp658=out_file;struct _dyneither_ptr _tmp657=({const char*_tmp2BB="#include <%s>\n";_tag_dyneither(_tmp2BB,sizeof(char),15U);});Cyc_fprintf(_tmp658,_tmp657,_tag_dyneither(_tmp2BA,sizeof(void*),1U));});});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp2BE=({
struct Cyc_List_List*_tmp65C=({struct Cyc_List_List*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));({struct _dyneither_ptr*_tmp65A=({struct _dyneither_ptr*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));({struct _dyneither_ptr _tmp659=(struct _dyneither_ptr)({const char*_tmp2DD="";_tag_dyneither(_tmp2DD,sizeof(char),1U);});_tmp2DC[0]=_tmp659;});_tmp2DC;});_tmp2DB->hd=_tmp65A;});({struct Cyc_List_List*_tmp65B=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp2DB->tl=_tmp65B;});_tmp2DB;});
# 1638
Cyc_str_sepstr(_tmp65C,({const char*_tmp2DE=" ";_tag_dyneither(_tmp2DE,sizeof(char),2U);}));});
# 1641
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2D8;_tmp2D8.tag=0U;({struct _dyneither_ptr _tmp65D=(struct _dyneither_ptr)({const char*_tmp2DA=_tmp287;_tag_dyneither(_tmp2DA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2DA,1U));});_tmp2D8.f1=_tmp65D;});({struct Cyc_String_pa_PrintArg_struct _tmp2D7;_tmp2D7.tag=0U;({struct _dyneither_ptr _tmp65E=(struct _dyneither_ptr)({const char*_tmp2D9=_tmp288;_tag_dyneither(_tmp2D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D9,1U));});_tmp2D7.f1=_tmp65E;});({struct Cyc_String_pa_PrintArg_struct _tmp2D6;_tmp2D6.tag=0U;_tmp2D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BE);({struct Cyc_String_pa_PrintArg_struct _tmp2D5;_tmp2D5.tag=0U;_tmp2D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp2D4;_tmp2D4.tag=0U;_tmp2D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=0U;_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp2D1[6U]={& _tmp2D3,& _tmp2D4,& _tmp2D5,& _tmp2D6,& _tmp2D7,& _tmp2D8};({struct _dyneither_ptr _tmp65F=({const char*_tmp2D2="%s %s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp2D2,sizeof(char),33U);});Cyc_aprintf(_tmp65F,_tag_dyneither(_tmp2D1,sizeof(void*),6U));});});});});});});});}),sizeof(char),1U));
# 1644
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2C1;_tmp2C1.tag=0U;({struct _dyneither_ptr _tmp660=(struct _dyneither_ptr)({char*_tmp2C2=cmd;_tag_dyneither(_tmp2C2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C2,1U));});_tmp2C1.f1=_tmp660;});({void*_tmp2BF[1U]={& _tmp2C1};({struct Cyc___cycFILE*_tmp662=Cyc_stderr;struct _dyneither_ptr _tmp661=({const char*_tmp2C0="%s\n";_tag_dyneither(_tmp2C0,sizeof(char),4U);});Cyc_fprintf(_tmp662,_tmp661,_tag_dyneither(_tmp2BF,sizeof(void*),1U));});});});
if(! system((const char*)cmd)){
# 1649
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2CA;_tmp2CA.tag=0U;({struct _dyneither_ptr _tmp663=(struct _dyneither_ptr)({const char*_tmp2CC=_tmp287;_tag_dyneither(_tmp2CC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2CC,1U));});_tmp2CA.f1=_tmp663;});({struct Cyc_String_pa_PrintArg_struct _tmp2C9;_tmp2C9.tag=0U;({struct _dyneither_ptr _tmp664=(struct _dyneither_ptr)({const char*_tmp2CB=_tmp289;_tag_dyneither(_tmp2CB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2CB,1U));});_tmp2C9.f1=_tmp664;});({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0U;_tmp2C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BE);({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0U;_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0U;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0U;_tmp2C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp2C3[6U]={& _tmp2C5,& _tmp2C6,& _tmp2C7,& _tmp2C8,& _tmp2C9,& _tmp2CA};({struct _dyneither_ptr _tmp665=({const char*_tmp2C4="%s %s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp2C4,sizeof(char),29U);});Cyc_aprintf(_tmp665,_tag_dyneither(_tmp2C3,sizeof(void*),6U));});});});});});});});}),sizeof(char),1U));
# 1652
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2CF;_tmp2CF.tag=0U;({struct _dyneither_ptr _tmp666=(struct _dyneither_ptr)({char*_tmp2D0=cmd;_tag_dyneither(_tmp2D0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D0,1U));});_tmp2CF.f1=_tmp666;});({void*_tmp2CD[1U]={& _tmp2CF};({struct Cyc___cycFILE*_tmp668=Cyc_stderr;struct _dyneither_ptr _tmp667=({const char*_tmp2CE="%s\n";_tag_dyneither(_tmp2CE,sizeof(char),4U);});Cyc_fprintf(_tmp668,_tmp667,_tag_dyneither(_tmp2CD,sizeof(void*),1U));});});});
 system((const char*)cmd);}};}}
# 1659
if(Cyc_gathering()){int _tmp2DF=0;_npop_handler(0U);return _tmp2DF;}{
# 1662
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
({struct Cyc___cycFILE*_tmp669=Cyc_fopen(_tmp288,"r");maybe=_tmp669;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));({struct Cyc_NO_SUPPORT_exn_struct _tmp66D=({struct Cyc_NO_SUPPORT_exn_struct _tmp2E1;_tmp2E1.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp66C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E4;_tmp2E4.tag=0U;({struct _dyneither_ptr _tmp66A=(struct _dyneither_ptr)({const char*_tmp2E5=_tmp288;_tag_dyneither(_tmp2E5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E5,1U));});_tmp2E4.f1=_tmp66A;});({void*_tmp2E2[1U]={& _tmp2E4};({struct _dyneither_ptr _tmp66B=({const char*_tmp2E3="can't open macrosfile %s";_tag_dyneither(_tmp2E3,sizeof(char),25U);});Cyc_aprintf(_tmp66B,_tag_dyneither(_tmp2E2,sizeof(void*),1U));});});});_tmp2E1.f1=_tmp66C;});_tmp2E1;});_tmp2E0[0]=_tmp66D;});_tmp2E0;}));
# 1667
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp2E6=Cyc_Lexing_from_file(in_file);
struct _tuple21*entry;
while(({struct _tuple21*_tmp66E=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp2E6);entry=_tmp66E;})!= 0){
struct _tuple21*_tmp2E7=(struct _tuple21*)_check_null(entry);struct _tuple21*_tmp2E8=_tmp2E7;struct _dyneither_ptr*_tmp2EA;struct Cyc_Set_Set*_tmp2E9;_LL1: _tmp2EA=_tmp2E8->f1;_tmp2E9=_tmp2E8->f2;_LL2:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp2EA,_tmp2E9);}
# 1676
Cyc_fclose(in_file);
# 1679
({struct Cyc___cycFILE*_tmp66F=Cyc_fopen(_tmp289,"r");maybe=_tmp66F;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));({struct Cyc_NO_SUPPORT_exn_struct _tmp673=({struct Cyc_NO_SUPPORT_exn_struct _tmp2EC;_tmp2EC.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp672=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0U;({struct _dyneither_ptr _tmp670=(struct _dyneither_ptr)({const char*_tmp2F0=_tmp289;_tag_dyneither(_tmp2F0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F0,1U));});_tmp2EF.f1=_tmp670;});({void*_tmp2ED[1U]={& _tmp2EF};({struct _dyneither_ptr _tmp671=({const char*_tmp2EE="can't open declsfile %s";_tag_dyneither(_tmp2EE,sizeof(char),24U);});Cyc_aprintf(_tmp671,_tag_dyneither(_tmp2ED,sizeof(void*),1U));});});});_tmp2EC.f1=_tmp672;});_tmp2EC;});_tmp2EB[0]=_tmp673;});_tmp2EB;}));
# 1683
in_file=maybe;
({struct Cyc_Lexing_lexbuf*_tmp674=Cyc_Lexing_from_file(in_file);_tmp2E6=_tmp674;});
({struct Cyc___cycFILE*_tmp675=Cyc_fopen(_tmp28A,"w");Cyc_slurp_out=_tmp675;});
if(!((unsigned int)Cyc_slurp_out)){int _tmp2F1=1;_npop_handler(0U);return _tmp2F1;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp2E6)){;}{
# 1689
struct Cyc_List_List*_tmp2F2=user_defs;
while(_tmp2F2 != 0){
struct _tuple24*_tmp2F3=(struct _tuple24*)_tmp2F2->hd;struct _tuple24*_tmp2F4=_tmp2F3;struct _dyneither_ptr*_tmp2F8;_LL4: _tmp2F8=_tmp2F4->f2;_LL5:;
({struct Cyc_String_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=0U;_tmp2F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F8);({void*_tmp2F5[1U]={& _tmp2F7};({struct Cyc___cycFILE*_tmp677=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp676=({const char*_tmp2F6="%s";_tag_dyneither(_tmp2F6,sizeof(char),3U);});Cyc_fprintf(_tmp677,_tmp676,_tag_dyneither(_tmp2F5,sizeof(void*),1U));});});});
_tmp2F2=_tmp2F2->tl;}
# 1695
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1701
({struct Cyc___cycFILE*_tmp678=Cyc_fopen(_tmp28A,"r");maybe=_tmp678;});
if(!((unsigned int)maybe)){int _tmp2F9=1;_npop_handler(0U);return _tmp2F9;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp2FA=_tmp28A;_tag_dyneither(_tmp2FA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FA,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp2FB=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1711
{struct Cyc_List_List*_tmp2FC=_tmp2FB;for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp2FC->hd,t);}}{
# 1715
struct Cyc_List_List*_tmp2FD=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);
struct Cyc_Set_Set*_tmp2FE=({struct Cyc_List_List*_tmp679=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,_tmp2FD);Cyc_reachable(_tmp679,t);});
# 1719
struct Cyc_List_List*_tmp2FF=0;
struct Cyc_List_List*_tmp300=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp301=_tmp2FB;for(0;_tmp301 != 0;_tmp301=_tmp301->tl){
struct Cyc_Absyn_Decl*_tmp302=(struct Cyc_Absyn_Decl*)_tmp301->hd;
struct _dyneither_ptr*name;
{void*_tmp303=_tmp302->r;void*_tmp304=_tmp303;struct Cyc_Absyn_Typedefdecl*_tmp31F;struct Cyc_Absyn_Enumdecl*_tmp31E;struct Cyc_Absyn_Aggrdecl*_tmp31D;struct Cyc_Absyn_Fndecl*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31B;switch(*((int*)_tmp304)){case 0U: _LL7: _tmp31B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp304)->f1;_LL8: {
# 1727
struct _tuple1*_tmp305=_tmp31B->name;struct _tuple1*_tmp306=_tmp305;struct _dyneither_ptr*_tmp307;_LL26: _tmp307=_tmp306->f2;_LL27:;
({struct Cyc_Set_Set*_tmp67A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp307);defined_symbols=_tmp67A;});
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp307))name=0;else{
name=_tmp307;}
goto _LL6;}case 1U: _LL9: _tmp31C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp304)->f1;_LLA: {
# 1733
struct _tuple1*_tmp308=_tmp31C->name;struct _tuple1*_tmp309=_tmp308;struct _dyneither_ptr*_tmp30A;_LL29: _tmp30A=_tmp309->f2;_LL2A:;
({struct Cyc_Set_Set*_tmp67B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp30A);defined_symbols=_tmp67B;});
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp30A))name=0;else{
name=_tmp30A;}
goto _LL6;}case 5U: _LLB: _tmp31D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp304)->f1;_LLC: {
# 1739
struct _tuple1*_tmp30B=_tmp31D->name;struct _tuple1*_tmp30C=_tmp30B;struct _dyneither_ptr*_tmp30D;_LL2C: _tmp30D=_tmp30C->f2;_LL2D:;
name=_tmp30D;
goto _LL6;}case 7U: _LLD: _tmp31E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp304)->f1;_LLE: {
# 1743
struct _tuple1*_tmp30E=_tmp31E->name;struct _tuple1*_tmp30F=_tmp30E;struct _dyneither_ptr*_tmp317;_LL2F: _tmp317=_tmp30F->f2;_LL30:;
name=_tmp317;
# 1747
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FE,name))
({struct Cyc_List_List*_tmp67C=({struct Cyc_List_List*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310->hd=_tmp302;_tmp310->tl=_tmp2FF;_tmp310;});_tmp2FF=_tmp67C;});else{
# 1750
if((unsigned int)_tmp31E->fields){
struct Cyc_List_List*_tmp311=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31E->fields))->v;for(0;_tmp311 != 0;_tmp311=_tmp311->tl){
struct Cyc_Absyn_Enumfield*_tmp312=(struct Cyc_Absyn_Enumfield*)_tmp311->hd;
struct _tuple1*_tmp313=_tmp312->name;struct _tuple1*_tmp314=_tmp313;struct _dyneither_ptr*_tmp316;_LL32: _tmp316=_tmp314->f2;_LL33:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FE,_tmp316)){
({struct Cyc_List_List*_tmp67D=({struct Cyc_List_List*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->hd=_tmp302;_tmp315->tl=_tmp2FF;_tmp315;});_tmp2FF=_tmp67D;});
break;}}}}
# 1760
name=0;
goto _LL6;}case 8U: _LLF: _tmp31F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp304)->f1;_LL10: {
# 1763
struct _tuple1*_tmp318=_tmp31F->name;struct _tuple1*_tmp319=_tmp318;struct _dyneither_ptr*_tmp31A;_LL35: _tmp31A=_tmp319->f2;_LL36:;
name=_tmp31A;
goto _LL6;}case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 2U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 3U: _LL19: _LL1A:
 goto _LL1C;case 9U: _LL1B: _LL1C:
 goto _LL1E;case 10U: _LL1D: _LL1E:
 goto _LL20;case 11U: _LL1F: _LL20:
 goto _LL22;case 12U: _LL21: _LL22:
 goto _LL24;default: _LL23: _LL24:
# 1776
 name=0;
goto _LL6;}_LL6:;}
# 1780
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FE,name)){
if(({struct _dyneither_ptr _tmp67F=(struct _dyneither_ptr)*name;struct _dyneither_ptr _tmp67E=(struct _dyneither_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp67F,_tmp67E,Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix));})!= 0)
({struct Cyc_List_List*_tmp680=({struct Cyc_List_List*_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320->hd=_tmp302;_tmp320->tl=_tmp2FF;_tmp320;});_tmp2FF=_tmp680;});else{
# 1785
Cyc_rename_decl(_tmp302);
({struct Cyc_List_List*_tmp681=({struct Cyc_List_List*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->hd=_tmp302;_tmp321->tl=_tmp300;_tmp321;});_tmp300=_tmp681;});}}}}
# 1792
if(!Cyc_do_setjmp){
({struct Cyc___cycFILE*_tmp682=Cyc_fopen(filename,"w");maybe=_tmp682;});
if(!((unsigned int)maybe)){int _tmp322=1;_npop_handler(0U);return _tmp322;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0U;({struct _dyneither_ptr _tmp683=(struct _dyneither_ptr)({const char*_tmp39D=filename;_tag_dyneither(_tmp39D,sizeof(char),_get_zero_arr_size_char((void*)_tmp39D,1U));});_tmp39C.f1=_tmp683;});({void*_tmp39A[1U]={& _tmp39C};({struct _dyneither_ptr _tmp684=({const char*_tmp39B="_%s_";_tag_dyneither(_tmp39B,sizeof(char),5U);});Cyc_aprintf(_tmp684,_tag_dyneither(_tmp39A,sizeof(void*),1U));});});});
{int _tmp323=0;for(0;_tmp323 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp323){
if(((char*)ifdefmacro.curr)[_tmp323]== '.'  || ((char*)ifdefmacro.curr)[_tmp323]== '/')
({struct _dyneither_ptr _tmp324=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp323);char _tmp325=*((char*)_check_dyneither_subscript(_tmp324,sizeof(char),0U));char _tmp326='_';if(_get_dyneither_size(_tmp324,sizeof(char))== 1U  && (_tmp325 == '\000'  && _tmp326 != '\000'))_throw_arraybounds();*((char*)_tmp324.curr)=_tmp326;});else{
if(((char*)ifdefmacro.curr)[_tmp323]!= '_'  && ((char*)ifdefmacro.curr)[_tmp323]!= '/')
({struct _dyneither_ptr _tmp327=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp323);char _tmp328=*((char*)_check_dyneither_subscript(_tmp327,sizeof(char),0U));char _tmp329=(char) toupper((int)((char*)ifdefmacro.curr)[_tmp323]);if(_get_dyneither_size(_tmp327,sizeof(char))== 1U  && (_tmp328 == '\000'  && _tmp329 != '\000'))_throw_arraybounds();*((char*)_tmp327.curr)=_tmp329;});}}}
# 1804
({struct Cyc_String_pa_PrintArg_struct _tmp32D;_tmp32D.tag=0U;_tmp32D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({struct Cyc_String_pa_PrintArg_struct _tmp32C;_tmp32C.tag=0U;_tmp32C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp32A[2U]={& _tmp32C,& _tmp32D};({struct Cyc___cycFILE*_tmp686=out_file;struct _dyneither_ptr _tmp685=({const char*_tmp32B="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp32B,sizeof(char),23U);});Cyc_fprintf(_tmp686,_tmp685,_tag_dyneither(_tmp32A,sizeof(void*),2U));});});});});{
# 1811
struct Cyc_List_List*_tmp32E=0;
struct Cyc_List_List*_tmp32F=0;
{struct Cyc_List_List*_tmp330=_tmp2FF;for(0;_tmp330 != 0;_tmp330=_tmp330->tl){
struct Cyc_Absyn_Decl*_tmp331=(struct Cyc_Absyn_Decl*)_tmp330->hd;
int _tmp332=0;
struct _dyneither_ptr*name;
{void*_tmp333=_tmp331->r;void*_tmp334=_tmp333;struct Cyc_Absyn_Typedefdecl*_tmp348;struct Cyc_Absyn_Enumdecl*_tmp347;struct Cyc_Absyn_Aggrdecl*_tmp346;struct Cyc_Absyn_Fndecl*_tmp345;struct Cyc_Absyn_Vardecl*_tmp344;switch(*((int*)_tmp334)){case 0U: _LL38: _tmp344=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp334)->f1;_LL39: {
# 1819
struct _tuple1*_tmp335=_tmp344->name;struct _tuple1*_tmp336=_tmp335;struct _dyneither_ptr*_tmp337;_LL57: _tmp337=_tmp336->f2;_LL58:;
name=_tmp337;
goto _LL37;}case 1U: _LL3A: _tmp345=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp334)->f1;_LL3B:
# 1823
 if(_tmp345->is_inline){name=0;goto _LL37;}{
struct _tuple1*_tmp338=_tmp345->name;struct _tuple1*_tmp339=_tmp338;struct _dyneither_ptr*_tmp33A;_LL5A: _tmp33A=_tmp339->f2;_LL5B:;
name=_tmp33A;
goto _LL37;};case 5U: _LL3C: _tmp346=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp334)->f1;_LL3D: {
# 1828
struct _tuple1*_tmp33B=_tmp346->name;struct _tuple1*_tmp33C=_tmp33B;struct _dyneither_ptr*_tmp33D;_LL5D: _tmp33D=_tmp33C->f2;_LL5E:;
name=_tmp33D;
goto _LL37;}case 7U: _LL3E: _tmp347=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp334)->f1;_LL3F: {
# 1832
struct _tuple1*_tmp33E=_tmp347->name;struct _tuple1*_tmp33F=_tmp33E;struct _dyneither_ptr*_tmp340;_LL60: _tmp340=_tmp33F->f2;_LL61:;
name=_tmp340;
goto _LL37;}case 8U: _LL40: _tmp348=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp334)->f1;_LL41: {
# 1836
struct _tuple1*_tmp341=_tmp348->name;struct _tuple1*_tmp342=_tmp341;struct _dyneither_ptr*_tmp343;_LL63: _tmp343=_tmp342->f2;_LL64:;
name=_tmp343;
goto _LL37;}case 4U: _LL42: _LL43:
 goto _LL45;case 13U: _LL44: _LL45:
 goto _LL47;case 14U: _LL46: _LL47:
 goto _LL49;case 2U: _LL48: _LL49:
 goto _LL4B;case 6U: _LL4A: _LL4B:
 goto _LL4D;case 3U: _LL4C: _LL4D:
 goto _LL4F;case 9U: _LL4E: _LL4F:
 goto _LL51;case 10U: _LL50: _LL51:
 goto _LL53;case 11U: _LL52: _LL53:
 goto _LL55;default: _LL54: _LL55:
# 1849
 name=0;
goto _LL37;}_LL37:;}
# 1852
if(!((unsigned int)name) && !_tmp332)continue;
# 1857
if(({struct Cyc_Set_Set*_tmp687=_tmp2FE;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp687,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp349=Cyc_named_decl(_tmp300,name);
Cyc_merge_rep_decl(_tmp349,_tmp331);
({struct Cyc_List_List*_tmp688=({struct Cyc_List_List*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=_tmp331;_tmp34A->tl=_tmp32E;_tmp34A;});_tmp32E=_tmp688;});}else{
# 1863
({struct Cyc_List_List*_tmp689=({struct Cyc_List_List*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->hd=_tmp331;_tmp34B->tl=_tmp32E;_tmp34B;});_tmp32E=_tmp689;});}
({struct Cyc_List_List*_tmp68A=({struct Cyc_List_List*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C->hd=name;_tmp34C->tl=_tmp32F;_tmp34C;});_tmp32F=_tmp68A;});}}
# 1868
{struct _handler_cons _tmp34D;_push_handler(& _tmp34D);{int _tmp34F=0;if(setjmp(_tmp34D.handler))_tmp34F=1;if(!_tmp34F){
Cyc_Binding_resolve_all(_tmp32E);
({struct Cyc_Tcenv_Tenv*_tmp68B=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp68B,1,_tmp32E);});
# 1869
;_pop_handler();}else{void*_tmp34E=(void*)_exn_thrown;void*_tmp350=_tmp34E;_LL66: _LL67:
# 1872
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp351=_cycalloc(sizeof(*_tmp351));({struct Cyc_NO_SUPPORT_exn_struct _tmp68D=({struct Cyc_NO_SUPPORT_exn_struct _tmp352;_tmp352.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp68C=({const char*_tmp353="can't typecheck acquired declarations";_tag_dyneither(_tmp353,sizeof(char),38U);});_tmp352.f1=_tmp68C;});_tmp352;});_tmp351[0]=_tmp68D;});_tmp351;}));_LL65:;}};}
# 1876
{struct _tuple0 _tmp354=({struct _tuple0 _tmp368;_tmp368.f1=_tmp32E;_tmp368.f2=_tmp32F;_tmp368;});struct _tuple0 _tmp355=_tmp354;struct Cyc_List_List*_tmp367;struct Cyc_List_List*_tmp366;_LL6B: _tmp367=_tmp355.f1;_tmp366=_tmp355.f2;_LL6C:;for(0;
_tmp367 != 0  && _tmp366 != 0;(_tmp367=_tmp367->tl,_tmp366=_tmp366->tl)){
struct Cyc_Absyn_Decl*_tmp356=(struct Cyc_Absyn_Decl*)_tmp367->hd;
struct _dyneither_ptr*_tmp357=(struct _dyneither_ptr*)_tmp366->hd;
int _tmp358=0;
if(!((unsigned int)_tmp357))
_tmp358=1;
# 1885
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp357){
({struct _dyneither_ptr _tmp68F=({struct Cyc_String_pa_PrintArg_struct _tmp35B;_tmp35B.tag=0U;_tmp35B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp357);({void*_tmp359[1U]={& _tmp35B};({struct _dyneither_ptr _tmp68E=({const char*_tmp35A="_%s_def_";_tag_dyneither(_tmp35A,sizeof(char),9U);});Cyc_aprintf(_tmp68E,_tag_dyneither(_tmp359,sizeof(void*),1U));});});});ifdefmacro=_tmp68F;});
({struct Cyc_String_pa_PrintArg_struct _tmp35E;_tmp35E.tag=0U;_tmp35E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp35C[1U]={& _tmp35E};({struct Cyc___cycFILE*_tmp691=out_file;struct _dyneither_ptr _tmp690=({const char*_tmp35D="#ifndef %s\n";_tag_dyneither(_tmp35D,sizeof(char),12U);});Cyc_fprintf(_tmp691,_tmp690,_tag_dyneither(_tmp35C,sizeof(void*),1U));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp361;_tmp361.tag=0U;_tmp361.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp35F[1U]={& _tmp361};({struct Cyc___cycFILE*_tmp693=out_file;struct _dyneither_ptr _tmp692=({const char*_tmp360="#define %s\n";_tag_dyneither(_tmp360,sizeof(char),12U);});Cyc_fprintf(_tmp693,_tmp692,_tag_dyneither(_tmp35F,sizeof(void*),1U));});});});
# 1891
({struct Cyc_List_List*_tmp694=({struct Cyc_Absyn_Decl*_tmp362[1U];_tmp362[0U]=_tmp356;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp362,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp694,out_file);});
({void*_tmp363=0U;({struct Cyc___cycFILE*_tmp696=out_file;struct _dyneither_ptr _tmp695=({const char*_tmp364="#endif\n";_tag_dyneither(_tmp364,sizeof(char),8U);});Cyc_fprintf(_tmp696,_tmp695,_tag_dyneither(_tmp363,sizeof(void*),0U));});});}else{
# 1896
({struct Cyc_List_List*_tmp697=({struct Cyc_Absyn_Decl*_tmp365[1U];_tmp365[0U]=_tmp356;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp365,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp697,out_file);});}}}
# 1901
({struct Cyc___cycFILE*_tmp698=Cyc_fopen(_tmp288,"r");maybe=_tmp698;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp369=_cycalloc(sizeof(*_tmp369));({struct Cyc_NO_SUPPORT_exn_struct _tmp69C=({struct Cyc_NO_SUPPORT_exn_struct _tmp36A;_tmp36A.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp69B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp36D;_tmp36D.tag=0U;({struct _dyneither_ptr _tmp699=(struct _dyneither_ptr)({const char*_tmp36E=_tmp288;_tag_dyneither(_tmp36E,sizeof(char),_get_zero_arr_size_char((void*)_tmp36E,1U));});_tmp36D.f1=_tmp699;});({void*_tmp36B[1U]={& _tmp36D};({struct _dyneither_ptr _tmp69A=({const char*_tmp36C="can't open macrosfile %s";_tag_dyneither(_tmp36C,sizeof(char),25U);});Cyc_aprintf(_tmp69A,_tag_dyneither(_tmp36B,sizeof(void*),1U));});});});_tmp36A.f1=_tmp69B;});_tmp36A;});_tmp369[0]=_tmp69C;});_tmp369;}));
# 1904
in_file=maybe;
({struct Cyc_Lexing_lexbuf*_tmp69D=Cyc_Lexing_from_file(in_file);_tmp2E6=_tmp69D;});{
struct _tuple22*entry2;
while(({struct _tuple22*_tmp69E=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp2E6);entry2=_tmp69E;})!= 0){
struct _tuple22*_tmp36F=(struct _tuple22*)_check_null(entry2);struct _tuple22*_tmp370=_tmp36F;struct _dyneither_ptr _tmp37A;struct _dyneither_ptr*_tmp379;_LL6E: _tmp37A=_tmp370->f1;_tmp379=_tmp370->f2;_LL6F:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FE,_tmp379)){
({struct Cyc_String_pa_PrintArg_struct _tmp373;_tmp373.tag=0U;_tmp373.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp379);({void*_tmp371[1U]={& _tmp373};({struct Cyc___cycFILE*_tmp6A0=out_file;struct _dyneither_ptr _tmp69F=({const char*_tmp372="#ifndef %s\n";_tag_dyneither(_tmp372,sizeof(char),12U);});Cyc_fprintf(_tmp6A0,_tmp69F,_tag_dyneither(_tmp371,sizeof(void*),1U));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp376;_tmp376.tag=0U;_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37A);({void*_tmp374[1U]={& _tmp376};({struct Cyc___cycFILE*_tmp6A2=out_file;struct _dyneither_ptr _tmp6A1=({const char*_tmp375="%s\n";_tag_dyneither(_tmp375,sizeof(char),4U);});Cyc_fprintf(_tmp6A2,_tmp6A1,_tag_dyneither(_tmp374,sizeof(void*),1U));});});});
({void*_tmp377=0U;({struct Cyc___cycFILE*_tmp6A4=out_file;struct _dyneither_ptr _tmp6A3=({const char*_tmp378="#endif\n";_tag_dyneither(_tmp378,sizeof(char),8U);});Cyc_fprintf(_tmp6A4,_tmp6A3,_tag_dyneither(_tmp377,sizeof(void*),0U));});});}}
# 1915
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1918
if(hstubs != 0){
struct Cyc_List_List*_tmp37B=hstubs;for(0;_tmp37B != 0;_tmp37B=_tmp37B->tl){
struct _tuple23*_tmp37C=(struct _tuple23*)_tmp37B->hd;struct _tuple23*_tmp37D=_tmp37C;struct _dyneither_ptr _tmp383;struct _dyneither_ptr _tmp382;_LL71: _tmp383=_tmp37D->f1;_tmp382=_tmp37D->f2;_LL72:;
if(({char*_tmp6A7=(char*)_tmp382.curr;_tmp6A7 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6A6=(char*)_tmp383.curr;_tmp6A6 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6A5=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6A5,({struct _dyneither_ptr*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=_tmp383;_tmp37E;}));})))
# 1924
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp382,sizeof(char),1U)),out_file);else{
# 1926
({struct Cyc_String_pa_PrintArg_struct _tmp381;_tmp381.tag=0U;_tmp381.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp383);({void*_tmp37F[1U]={& _tmp381};({struct _dyneither_ptr _tmp6A8=({const char*_tmp380="%s is not supported on this platform\n";_tag_dyneither(_tmp380,sizeof(char),38U);});Cyc_log(_tmp6A8,_tag_dyneither(_tmp37F,sizeof(void*),1U));});});});}}}
# 1929
({void*_tmp384=0U;({struct Cyc___cycFILE*_tmp6AA=out_file;struct _dyneither_ptr _tmp6A9=({const char*_tmp385="#endif\n";_tag_dyneither(_tmp385,sizeof(char),8U);});Cyc_fprintf(_tmp6AA,_tmp6A9,_tag_dyneither(_tmp384,sizeof(void*),0U));});});
if(Cyc_do_setjmp){int _tmp386=0;_npop_handler(0U);return _tmp386;}else{
Cyc_fclose(out_file);}
# 1934
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp387=cstubs;for(0;_tmp387 != 0;_tmp387=_tmp387->tl){
struct _tuple23*_tmp388=(struct _tuple23*)_tmp387->hd;struct _tuple23*_tmp389=_tmp388;struct _dyneither_ptr _tmp38C;struct _dyneither_ptr _tmp38B;_LL74: _tmp38C=_tmp389->f1;_tmp38B=_tmp389->f2;_LL75:;
if(({char*_tmp6AD=(char*)_tmp38B.curr;_tmp6AD != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6AC=(char*)_tmp38C.curr;_tmp6AC == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6AB=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6AB,({struct _dyneither_ptr*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=_tmp38C;_tmp38A;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp38B,sizeof(char),1U)),out_file);}};}
# 1945
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1947
({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U;({struct _dyneither_ptr _tmp6AE=(struct _dyneither_ptr)({const char*_tmp390=filename;_tag_dyneither(_tmp390,sizeof(char),_get_zero_arr_size_char((void*)_tmp390,1U));});_tmp38F.f1=_tmp6AE;});({void*_tmp38D[1U]={& _tmp38F};({struct Cyc___cycFILE*_tmp6B0=out_file;struct _dyneither_ptr _tmp6AF=({const char*_tmp38E="#include <%s>\n\n";_tag_dyneither(_tmp38E,sizeof(char),16U);});Cyc_fprintf(_tmp6B0,_tmp6AF,_tag_dyneither(_tmp38D,sizeof(void*),1U));});});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp391=cycstubs;for(0;_tmp391 != 0;_tmp391=_tmp391->tl){
struct _tuple23*_tmp392=(struct _tuple23*)_tmp391->hd;struct _tuple23*_tmp393=_tmp392;struct _dyneither_ptr _tmp396;struct _dyneither_ptr _tmp395;_LL77: _tmp396=_tmp393->f1;_tmp395=_tmp393->f2;_LL78:;
if(({char*_tmp6B3=(char*)_tmp395.curr;_tmp6B3 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6B2=(char*)_tmp396.curr;_tmp6B2 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6B1=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6B1,({struct _dyneither_ptr*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394[0]=_tmp396;_tmp394;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp395,sizeof(char),1U)),out_file);}}
# 1956
({void*_tmp397=0U;({struct Cyc___cycFILE*_tmp6B5=out_file;struct _dyneither_ptr _tmp6B4=({const char*_tmp398="\n";_tag_dyneither(_tmp398,sizeof(char),2U);});Cyc_fprintf(_tmp6B5,_tmp6B4,_tag_dyneither(_tmp397,sizeof(void*),0U));});});}{
# 1959
int _tmp399=0;_npop_handler(0U);return _tmp399;};};};};};};};};};
# 1606
;_pop_handler();}else{void*_tmp28C=(void*)_exn_thrown;void*_tmp39E=_tmp28C;void*_tmp3B5;struct _dyneither_ptr _tmp3B4;struct _dyneither_ptr _tmp3B3;struct _dyneither_ptr _tmp3B2;struct _dyneither_ptr _tmp3B1;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp39E)->tag == Cyc_Core_Impossible){_LL7A: _tmp3B1=((struct Cyc_Core_Impossible_exn_struct*)_tmp39E)->f1;_LL7B:
# 1963
({struct Cyc_String_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=0U;_tmp3A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B1);({void*_tmp39F[1U]={& _tmp3A1};({struct Cyc___cycFILE*_tmp6B7=Cyc_stderr;struct _dyneither_ptr _tmp6B6=({const char*_tmp3A0="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp3A0,sizeof(char),26U);});Cyc_fprintf(_tmp6B7,_tmp6B6,_tag_dyneither(_tmp39F,sizeof(void*),1U));});});});goto _LL79;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp39E)->tag == Cyc_Dict_Absent){_LL7C: _LL7D:
# 1965
({void*_tmp3A2=0U;({struct Cyc___cycFILE*_tmp6B9=Cyc_stderr;struct _dyneither_ptr _tmp6B8=({const char*_tmp3A3="Got Dict::Absent\n";_tag_dyneither(_tmp3A3,sizeof(char),18U);});Cyc_fprintf(_tmp6B9,_tmp6B8,_tag_dyneither(_tmp3A2,sizeof(void*),0U));});});goto _LL79;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp39E)->tag == Cyc_Core_Failure){_LL7E: _tmp3B2=((struct Cyc_Core_Failure_exn_struct*)_tmp39E)->f1;_LL7F:
# 1967
({struct Cyc_String_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=0U;_tmp3A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B2);({void*_tmp3A4[1U]={& _tmp3A6};({struct Cyc___cycFILE*_tmp6BB=Cyc_stderr;struct _dyneither_ptr _tmp6BA=({const char*_tmp3A5="Got Core::Failure(%s)\n";_tag_dyneither(_tmp3A5,sizeof(char),23U);});Cyc_fprintf(_tmp6BB,_tmp6BA,_tag_dyneither(_tmp3A4,sizeof(void*),1U));});});});goto _LL79;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39E)->tag == Cyc_Core_Invalid_argument){_LL80: _tmp3B3=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39E)->f1;_LL81:
# 1969
({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0U;_tmp3A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B3);({void*_tmp3A7[1U]={& _tmp3A9};({struct Cyc___cycFILE*_tmp6BD=Cyc_stderr;struct _dyneither_ptr _tmp6BC=({const char*_tmp3A8="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp3A8,sizeof(char),26U);});Cyc_fprintf(_tmp6BD,_tmp6BC,_tag_dyneither(_tmp3A7,sizeof(void*),1U));});});});goto _LL79;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp39E)->tag == Cyc_Core_Not_found){_LL82: _LL83:
# 1971
({void*_tmp3AA=0U;({struct Cyc___cycFILE*_tmp6BF=Cyc_stderr;struct _dyneither_ptr _tmp6BE=({const char*_tmp3AB="Got Not_found\n";_tag_dyneither(_tmp3AB,sizeof(char),15U);});Cyc_fprintf(_tmp6BF,_tmp6BE,_tag_dyneither(_tmp3AA,sizeof(void*),0U));});});goto _LL79;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp39E)->tag == Cyc_NO_SUPPORT){_LL84: _tmp3B4=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp39E)->f1;_LL85:
# 1973
({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U;_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B4);({void*_tmp3AC[1U]={& _tmp3AE};({struct Cyc___cycFILE*_tmp6C1=Cyc_stderr;struct _dyneither_ptr _tmp6C0=({const char*_tmp3AD="No support because %s\n";_tag_dyneither(_tmp3AD,sizeof(char),23U);});Cyc_fprintf(_tmp6C1,_tmp6C0,_tag_dyneither(_tmp3AC,sizeof(void*),1U));});});});goto _LL79;}else{_LL86: _tmp3B5=_tmp39E;_LL87:
# 1975
({void*_tmp3AF=0U;({struct Cyc___cycFILE*_tmp6C3=Cyc_stderr;struct _dyneither_ptr _tmp6C2=({const char*_tmp3B0="Got unknown exception\n";_tag_dyneither(_tmp3B0,sizeof(char),23U);});Cyc_fprintf(_tmp6C3,_tmp6C2,_tag_dyneither(_tmp3AF,sizeof(void*),0U));});});
Cyc_Core_rethrow(_tmp3B5);}}}}}}_LL79:;}};}
# 1981
({struct Cyc___cycFILE*_tmp6C4=Cyc_fopen(filename,"w");maybe=_tmp6C4;});
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0U;({struct _dyneither_ptr _tmp6C5=(struct _dyneither_ptr)({const char*_tmp3B9=filename;_tag_dyneither(_tmp3B9,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B9,1U));});_tmp3B8.f1=_tmp6C5;});({void*_tmp3B6[1U]={& _tmp3B8};({struct Cyc___cycFILE*_tmp6C7=Cyc_stderr;struct _dyneither_ptr _tmp6C6=({const char*_tmp3B7="Error: could not create file %s\n";_tag_dyneither(_tmp3B7,sizeof(char),33U);});Cyc_fprintf(_tmp6C7,_tmp6C6,_tag_dyneither(_tmp3B6,sizeof(void*),1U));});});});
return 1;}
# 1986
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0U;({struct _dyneither_ptr _tmp6C8=(struct _dyneither_ptr)({const char*_tmp3BD=filename;_tag_dyneither(_tmp3BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp3BD,1U));});_tmp3BC.f1=_tmp6C8;});({void*_tmp3BA[1U]={& _tmp3BC};({struct Cyc___cycFILE*_tmp6CA=out_file;struct _dyneither_ptr _tmp6C9=({const char*_tmp3BB="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp3BB,sizeof(char),48U);});Cyc_fprintf(_tmp6CA,_tmp6C9,_tag_dyneither(_tmp3BA,sizeof(void*),1U));});});});
# 1990
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0U;({struct _dyneither_ptr _tmp6CB=(struct _dyneither_ptr)({const char*_tmp3C1=filename;_tag_dyneither(_tmp3C1,sizeof(char),_get_zero_arr_size_char((void*)_tmp3C1,1U));});_tmp3C0.f1=_tmp6CB;});({void*_tmp3BE[1U]={& _tmp3C0};({struct Cyc___cycFILE*_tmp6CD=Cyc_stderr;struct _dyneither_ptr _tmp6CC=({const char*_tmp3BF="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp3BF,sizeof(char),52U);});Cyc_fprintf(_tmp6CD,_tmp6CC,_tag_dyneither(_tmp3BE,sizeof(void*),1U));});});});
# 1993
({void*_tmp3C2=0U;({struct _dyneither_ptr _tmp6CE=({const char*_tmp3C3="Not supported on this platform\n";_tag_dyneither(_tmp3C3,sizeof(char),32U);});Cyc_log(_tmp6CE,_tag_dyneither(_tmp3C2,sizeof(void*),0U));});});
# 2000
return 0;};}
# 2004
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0U;({struct _dyneither_ptr _tmp6CF=(struct _dyneither_ptr)({const char*_tmp3DC=file;_tag_dyneither(_tmp3DC,sizeof(char),_get_zero_arr_size_char((void*)_tmp3DC,1U));});_tmp3DB.f1=_tmp6CF;});({void*_tmp3D9[1U]={& _tmp3DB};({struct Cyc___cycFILE*_tmp6D1=Cyc_stderr;struct _dyneither_ptr _tmp6D0=({const char*_tmp3DA="Processing %s\n";_tag_dyneither(_tmp3DA,sizeof(char),15U);});Cyc_fprintf(_tmp6D1,_tmp6D0,_tag_dyneither(_tmp3D9,sizeof(void*),1U));});});});{
struct Cyc___cycFILE*_tmp3DD=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp3DD)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0U;({struct _dyneither_ptr _tmp6D2=(struct _dyneither_ptr)({const char*_tmp3E1=file;_tag_dyneither(_tmp3E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp3E1,1U));});_tmp3E0.f1=_tmp6D2;});({void*_tmp3DE[1U]={& _tmp3E0};({struct Cyc___cycFILE*_tmp6D4=Cyc_stderr;struct _dyneither_ptr _tmp6D3=({const char*_tmp3DF="Error: could not open %s\n";_tag_dyneither(_tmp3DF,sizeof(char),26U);});Cyc_fprintf(_tmp6D4,_tmp6D3,_tag_dyneither(_tmp3DE,sizeof(void*),1U));});});});
return 1;}{
# 2012
struct Cyc___cycFILE*_tmp3E2=_tmp3DD;
# 2016
struct _dyneither_ptr buf=({char*_tmp401=({unsigned int _tmp3FE=(unsigned int)1024;char*_tmp3FF=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp3FE + 1U));{unsigned int _tmp400=_tmp3FE;unsigned int i;for(i=0;i < _tmp400;i ++){_tmp3FF[i]=(char)'\000';}_tmp3FF[_tmp400]=(char)0U;}_tmp3FF;});_tag_dyneither(_tmp401,sizeof(char),_get_zero_arr_size_char((void*)_tmp401,(unsigned int)1024 + 1U));});
struct _dyneither_ptr _tmp3E3=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0U;({struct _dyneither_ptr _tmp6D5=(struct _dyneither_ptr)({const char*_tmp3E7=dir;_tag_dyneither(_tmp3E7,sizeof(char),_get_zero_arr_size_char((void*)_tmp3E7,1U));});_tmp3E6.f1=_tmp6D5;});({void*_tmp3E4[1U]={& _tmp3E6};({struct Cyc___cycFILE*_tmp6D7=Cyc_stderr;struct _dyneither_ptr _tmp6D6=({const char*_tmp3E5="Error: can't change directory to %s\n";_tag_dyneither(_tmp3E5,sizeof(char),37U);});Cyc_fprintf(_tmp6D7,_tmp6D6,_tag_dyneither(_tmp3E4,sizeof(void*),1U));});});});
return 1;}}
# 2024
if(Cyc_mode == Cyc_GATHER){
# 2026
struct _dyneither_ptr _tmp3E8=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0U;_tmp3F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0U;_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0U;_tmp3EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp3EC[3U]={& _tmp3EE,& _tmp3EF,& _tmp3F0};({struct _dyneither_ptr _tmp6D8=({const char*_tmp3ED="echo | %s %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp3ED,sizeof(char),42U);});Cyc_aprintf(_tmp6D8,_tag_dyneither(_tmp3EC,sizeof(void*),3U));});});});});});
# 2028
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0U;_tmp3EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E8);({void*_tmp3E9[1U]={& _tmp3EB};({struct Cyc___cycFILE*_tmp6DA=Cyc_stderr;struct _dyneither_ptr _tmp6D9=({const char*_tmp3EA="%s\n";_tag_dyneither(_tmp3EA,sizeof(char),4U);});Cyc_fprintf(_tmp6DA,_tmp6D9,_tag_dyneither(_tmp3E9,sizeof(void*),1U));});});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp3E8,sizeof(char),1U)));}{
# 2033
struct Cyc_Lexing_lexbuf*_tmp3F1=Cyc_Lexing_from_file(_tmp3E2);
struct _tuple25*entry;
while(({struct _tuple25*_tmp6DB=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp3F1);entry=_tmp6DB;})!= 0){
struct _tuple25*_tmp3F2=(struct _tuple25*)_check_null(entry);struct _tuple25*_tmp3F3=_tmp3F2;struct _dyneither_ptr _tmp3FA;struct Cyc_List_List*_tmp3F9;struct Cyc_List_List*_tmp3F8;struct Cyc_List_List*_tmp3F7;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F4;_LL1: _tmp3FA=_tmp3F3->f1;_tmp3F9=_tmp3F3->f2;_tmp3F8=_tmp3F3->f3;_tmp3F7=_tmp3F3->f4;_tmp3F6=_tmp3F3->f5;_tmp3F5=_tmp3F3->f6;_tmp3F4=_tmp3F3->f7;_LL2:;
# 2038
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp3FA,sizeof(char),1U)),_tmp3F9,_tmp3F8,_tmp3F7,_tmp3F6,_tmp3F5,_tmp3F4))
# 2040
return 1;}
# 2042
Cyc_fclose(_tmp3E2);
# 2044
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3E3,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3FD;_tmp3FD.tag=0U;_tmp3FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E3);({void*_tmp3FB[1U]={& _tmp3FD};({struct Cyc___cycFILE*_tmp6DD=Cyc_stderr;struct _dyneither_ptr _tmp6DC=({const char*_tmp3FC="Error: could not change directory to %s\n";_tag_dyneither(_tmp3FC,sizeof(char),41U);});Cyc_fprintf(_tmp6DD,_tmp6DC,_tag_dyneither(_tmp3FB,sizeof(void*),1U));});});});
return 1;}}
# 2050
return 0;};};};}
# 2054
int Cyc_process_setjmp(const char*dir){
# 2057
struct _dyneither_ptr buf=({char*_tmp413=({unsigned int _tmp410=(unsigned int)1024;char*_tmp411=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp410 + 1U));{unsigned int _tmp412=_tmp410;unsigned int i;for(i=0;i < _tmp412;i ++){_tmp411[i]=(char)'\000';}_tmp411[_tmp412]=(char)0U;}_tmp411;});_tag_dyneither(_tmp413,sizeof(char),_get_zero_arr_size_char((void*)_tmp413,(unsigned int)1024 + 1U));});
struct _dyneither_ptr _tmp402=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp405;_tmp405.tag=0U;({struct _dyneither_ptr _tmp6DE=(struct _dyneither_ptr)({const char*_tmp406=dir;_tag_dyneither(_tmp406,sizeof(char),_get_zero_arr_size_char((void*)_tmp406,1U));});_tmp405.f1=_tmp6DE;});({void*_tmp403[1U]={& _tmp405};({struct Cyc___cycFILE*_tmp6E0=Cyc_stderr;struct _dyneither_ptr _tmp6DF=({const char*_tmp404="Error: can't change directory to %s\n";_tag_dyneither(_tmp404,sizeof(char),37U);});Cyc_fprintf(_tmp6E0,_tmp6DF,_tag_dyneither(_tmp403,sizeof(void*),1U));});});});
return 1;}
# 2063
if(({struct Cyc_List_List*_tmp6E5=({struct _dyneither_ptr*_tmp407[1U];({struct _dyneither_ptr*_tmp6E1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8U);_tmp407[0U]=_tmp6E1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp407,sizeof(struct _dyneither_ptr*),1U));});Cyc_process_file("setjmp.h",_tmp6E5,0,0,({struct _tuple23*_tmp409[1U];({struct _tuple23*_tmp6E4=({struct _tuple23*_tmp40A=_cycalloc(sizeof(*_tmp40A));({struct _dyneither_ptr _tmp6E2=({const char*_tmp40C="setjmp";_tag_dyneither(_tmp40C,sizeof(char),7U);});_tmp40A->f1=_tmp6E2;});({struct _dyneither_ptr _tmp6E3=({const char*_tmp40B="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp40B,sizeof(char),29U);});_tmp40A->f2=_tmp6E3;});_tmp40A;});_tmp409[0U]=_tmp6E4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp409,sizeof(struct _tuple23*),1U));}),0,0);}))
# 2066
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp402,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp40F;_tmp40F.tag=0U;_tmp40F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp402);({void*_tmp40D[1U]={& _tmp40F};({struct Cyc___cycFILE*_tmp6E7=Cyc_stderr;struct _dyneither_ptr _tmp6E6=({const char*_tmp40E="Error: could not change directory to %s\n";_tag_dyneither(_tmp40E,sizeof(char),41U);});Cyc_fprintf(_tmp6E7,_tmp6E6,_tag_dyneither(_tmp40D,sizeof(void*),1U));});});});
return 1;}
# 2071
return 0;}static char _tmp414[13U]="BUILDLIB.OUT";
# 2075
static struct _dyneither_ptr Cyc_output_dir={_tmp414,_tmp414,_tmp414 + 13U};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 2079
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp6E8=({struct Cyc_List_List*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1U));_tmp415->tl=Cyc_spec_files;_tmp415;});Cyc_spec_files=_tmp6E8;});}
# 2083
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 2087
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 2090
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 2093
static void Cyc_add_cpparg(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp6EA=({struct Cyc_List_List*_tmp416=_cycalloc(sizeof(*_tmp416));({struct _dyneither_ptr*_tmp6E9=({struct _dyneither_ptr*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=s;_tmp417;});_tmp416->hd=_tmp6E9;});_tmp416->tl=Cyc_cppargs;_tmp416;});Cyc_cppargs=_tmp6EA;});}
# 2096
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp41A;_tmp41A.tag=0U;_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp418[1U]={& _tmp41A};({struct Cyc___cycFILE*_tmp6EC=Cyc_stderr;struct _dyneither_ptr _tmp6EB=({const char*_tmp419="Unsupported option %s\n";_tag_dyneither(_tmp419,sizeof(char),23U);});Cyc_fprintf(_tmp6EC,_tmp6EB,_tag_dyneither(_tmp418,sizeof(void*),1U));});});});
Cyc_badparse=1;}
# 2105
void GC_blacklist_warn_clear();struct _tuple29{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 2109
struct Cyc_List_List*options=({struct _tuple29*_tmp452[9U];({struct _tuple29*_tmp6F2=({struct _tuple29*_tmp483=_cycalloc(sizeof(*_tmp483));({struct _dyneither_ptr _tmp6ED=({const char*_tmp488="-";_tag_dyneither(_tmp488,sizeof(char),2U);});_tmp483->f1=_tmp6ED;});_tmp483->f2=1;({struct _dyneither_ptr _tmp6EE=({const char*_tmp487="";_tag_dyneither(_tmp487,sizeof(char),1U);});_tmp483->f3=_tmp6EE;});({void*_tmp6F0=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp485=_cycalloc(sizeof(*_tmp485));({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp6EF=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp486;_tmp486.tag=1U;_tmp486.f1=Cyc_add_cpparg;_tmp486;});_tmp485[0]=_tmp6EF;});_tmp485;});_tmp483->f4=_tmp6F0;});({struct _dyneither_ptr _tmp6F1=({const char*_tmp484="";_tag_dyneither(_tmp484,sizeof(char),1U);});_tmp483->f5=_tmp6F1;});_tmp483;});_tmp452[8U]=_tmp6F2;});({struct _tuple29*_tmp6F8=({struct _tuple29*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct _dyneither_ptr _tmp6F3=({const char*_tmp482="-v";_tag_dyneither(_tmp482,sizeof(char),3U);});_tmp47D->f1=_tmp6F3;});_tmp47D->f2=0;({struct _dyneither_ptr _tmp6F4=({const char*_tmp481="";_tag_dyneither(_tmp481,sizeof(char),1U);});_tmp47D->f3=_tmp6F4;});({void*_tmp6F6=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp6F5=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp480;_tmp480.tag=3U;_tmp480.f1=& Cyc_verbose;_tmp480;});_tmp47F[0]=_tmp6F5;});_tmp47F;});_tmp47D->f4=_tmp6F6;});({struct _dyneither_ptr _tmp6F7=({const char*_tmp47E="Verbose operation";_tag_dyneither(_tmp47E,sizeof(char),18U);});_tmp47D->f5=_tmp6F7;});_tmp47D;});_tmp452[7U]=_tmp6F8;});({struct _tuple29*_tmp6FE=({struct _tuple29*_tmp477=_cycalloc(sizeof(*_tmp477));({struct _dyneither_ptr _tmp6F9=({const char*_tmp47C="-B";_tag_dyneither(_tmp47C,sizeof(char),3U);});_tmp477->f1=_tmp6F9;});_tmp477->f2=1;({struct _dyneither_ptr _tmp6FA=({const char*_tmp47B="<file>";_tag_dyneither(_tmp47B,sizeof(char),7U);});_tmp477->f3=_tmp6FA;});({void*_tmp6FC=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp479=_cycalloc(sizeof(*_tmp479));({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp6FB=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp47A;_tmp47A.tag=1U;_tmp47A.f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp47A;});_tmp479[0]=_tmp6FB;});_tmp479;});_tmp477->f4=_tmp6FC;});({struct _dyneither_ptr _tmp6FD=({const char*_tmp478="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp478,sizeof(char),60U);});_tmp477->f5=_tmp6FD;});_tmp477;});_tmp452[6U]=_tmp6FE;});({struct _tuple29*_tmp704=({struct _tuple29*_tmp471=_cycalloc(sizeof(*_tmp471));({struct _dyneither_ptr _tmp6FF=({const char*_tmp476="-b";_tag_dyneither(_tmp476,sizeof(char),3U);});_tmp471->f1=_tmp6FF;});_tmp471->f2=0;({struct _dyneither_ptr _tmp700=({const char*_tmp475=" <machine>";_tag_dyneither(_tmp475,sizeof(char),11U);});_tmp471->f3=_tmp700;});({void*_tmp702=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp473=_cycalloc(sizeof(*_tmp473));({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp701=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp474;_tmp474.tag=5U;_tmp474.f1=Cyc_Specsfile_set_target_arch;_tmp474;});_tmp473[0]=_tmp701;});_tmp473;});_tmp471->f4=_tmp702;});({struct _dyneither_ptr _tmp703=({const char*_tmp472="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp472,sizeof(char),52U);});_tmp471->f5=_tmp703;});_tmp471;});_tmp452[5U]=_tmp704;});({struct _tuple29*_tmp70A=({struct _tuple29*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct _dyneither_ptr _tmp705=({const char*_tmp470="-setjmp";_tag_dyneither(_tmp470,sizeof(char),8U);});_tmp46B->f1=_tmp705;});_tmp46B->f2=0;({struct _dyneither_ptr _tmp706=({const char*_tmp46F="";_tag_dyneither(_tmp46F,sizeof(char),1U);});_tmp46B->f3=_tmp706;});({void*_tmp708=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp707=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp46E;_tmp46E.tag=3U;_tmp46E.f1=& Cyc_do_setjmp;_tmp46E;});_tmp46D[0]=_tmp707;});_tmp46D;});_tmp46B->f4=_tmp708;});({struct _dyneither_ptr _tmp709=({const char*_tmp46C="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp46C,sizeof(char),186U);});_tmp46B->f5=_tmp709;});_tmp46B;});_tmp452[4U]=_tmp70A;});({struct _tuple29*_tmp710=({struct _tuple29*_tmp465=_cycalloc(sizeof(*_tmp465));({struct _dyneither_ptr _tmp70B=({const char*_tmp46A="-finish";_tag_dyneither(_tmp46A,sizeof(char),8U);});_tmp465->f1=_tmp70B;});_tmp465->f2=0;({struct _dyneither_ptr _tmp70C=({const char*_tmp469="";_tag_dyneither(_tmp469,sizeof(char),1U);});_tmp465->f3=_tmp70C;});({void*_tmp70E=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp70D=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp468;_tmp468.tag=0U;_tmp468.f1=Cyc_set_FINISH;_tmp468;});_tmp467[0]=_tmp70D;});_tmp467;});_tmp465->f4=_tmp70E;});({struct _dyneither_ptr _tmp70F=({const char*_tmp466="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp466,sizeof(char),57U);});_tmp465->f5=_tmp70F;});_tmp465;});_tmp452[3U]=_tmp710;});({struct _tuple29*_tmp716=({struct _tuple29*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct _dyneither_ptr _tmp711=({const char*_tmp464="-gatherscript";_tag_dyneither(_tmp464,sizeof(char),14U);});_tmp45F->f1=_tmp711;});_tmp45F->f2=0;({struct _dyneither_ptr _tmp712=({const char*_tmp463="";_tag_dyneither(_tmp463,sizeof(char),1U);});_tmp45F->f3=_tmp712;});({void*_tmp714=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp461=_cycalloc(sizeof(*_tmp461));({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp713=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp462;_tmp462.tag=0U;_tmp462.f1=Cyc_set_GATHERSCRIPT;_tmp462;});_tmp461[0]=_tmp713;});_tmp461;});_tmp45F->f4=_tmp714;});({struct _dyneither_ptr _tmp715=({const char*_tmp460="Produce a script to gather C library info";_tag_dyneither(_tmp460,sizeof(char),42U);});_tmp45F->f5=_tmp715;});_tmp45F;});_tmp452[2U]=_tmp716;});({struct _tuple29*_tmp71C=({struct _tuple29*_tmp459=_cycalloc(sizeof(*_tmp459));({struct _dyneither_ptr _tmp717=({const char*_tmp45E="-gather";_tag_dyneither(_tmp45E,sizeof(char),8U);});_tmp459->f1=_tmp717;});_tmp459->f2=0;({struct _dyneither_ptr _tmp718=({const char*_tmp45D="";_tag_dyneither(_tmp45D,sizeof(char),1U);});_tmp459->f3=_tmp718;});({void*_tmp71A=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp719=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp45C;_tmp45C.tag=0U;_tmp45C.f1=Cyc_set_GATHER;_tmp45C;});_tmp45B[0]=_tmp719;});_tmp45B;});_tmp459->f4=_tmp71A;});({struct _dyneither_ptr _tmp71B=({const char*_tmp45A="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp45A,sizeof(char),56U);});_tmp459->f5=_tmp71B;});_tmp459;});_tmp452[1U]=_tmp71C;});({struct _tuple29*_tmp722=({struct _tuple29*_tmp453=_cycalloc(sizeof(*_tmp453));({struct _dyneither_ptr _tmp71D=({const char*_tmp458="-d";_tag_dyneither(_tmp458,sizeof(char),3U);});_tmp453->f1=_tmp71D;});_tmp453->f2=0;({struct _dyneither_ptr _tmp71E=({const char*_tmp457=" <file>";_tag_dyneither(_tmp457,sizeof(char),8U);});_tmp453->f3=_tmp71E;});({void*_tmp720=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp455=_cycalloc(sizeof(*_tmp455));({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp71F=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp456;_tmp456.tag=5U;_tmp456.f1=Cyc_set_output_dir;_tmp456;});_tmp455[0]=_tmp71F;});_tmp455;});_tmp453->f4=_tmp720;});({struct _dyneither_ptr _tmp721=({const char*_tmp454="Set the output directory to <file>";_tag_dyneither(_tmp454,sizeof(char),35U);});_tmp453->f5=_tmp721;});_tmp453;});_tmp452[0U]=_tmp722;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp452,sizeof(struct _tuple29*),9U));});
# 2142
struct _dyneither_ptr _tmp41B=({struct Cyc_List_List*_tmp724=options;struct _dyneither_ptr _tmp723=({const char*_tmp451="Options:";_tag_dyneither(_tmp451,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp724,Cyc_add_spec_file,Cyc_no_other,_tmp723,argv);});
# 2144
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp726=options;struct _dyneither_ptr _tmp725=({const char*_tmp41C="Options:";_tag_dyneither(_tmp41C,sizeof(char),9U);});Cyc_Arg_parse(_tmp726,Cyc_add_spec_file,Cyc_no_other,_tmp725,_tmp41B);});
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp727=options;Cyc_Arg_usage(_tmp727,({const char*_tmp41D="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp41D,sizeof(char),59U);}));});
# 2154
return 1;}{
# 2160
struct _dyneither_ptr _tmp41E=Cyc_Specsfile_find_in_arch_path(({const char*_tmp450="cycspecs";_tag_dyneither(_tmp450,sizeof(char),9U);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0U;_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41E);({void*_tmp41F[1U]={& _tmp421};({struct Cyc___cycFILE*_tmp729=Cyc_stderr;struct _dyneither_ptr _tmp728=({const char*_tmp420="Reading from specs file %s\n";_tag_dyneither(_tmp420,sizeof(char),28U);});Cyc_fprintf(_tmp729,_tmp728,_tag_dyneither(_tmp41F,sizeof(void*),1U));});});});{
struct Cyc_List_List*_tmp422=Cyc_Specsfile_read_specs(_tmp41E);
({struct _dyneither_ptr _tmp72B=({struct Cyc_List_List*_tmp72A=_tmp422;Cyc_Specsfile_get_spec(_tmp72A,({const char*_tmp423="cyclone_target_cflags";_tag_dyneither(_tmp423,sizeof(char),22U);}));});Cyc_target_cflags=_tmp72B;});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U;_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp424[1U]={& _tmp426};({struct Cyc___cycFILE*_tmp72D=Cyc_stderr;struct _dyneither_ptr _tmp72C=({const char*_tmp425="Target cflags are %s\n";_tag_dyneither(_tmp425,sizeof(char),22U);});Cyc_fprintf(_tmp72D,_tmp72C,_tag_dyneither(_tmp424,sizeof(void*),1U));});});});
({struct _dyneither_ptr _tmp72F=({struct Cyc_List_List*_tmp72E=_tmp422;Cyc_Specsfile_get_spec(_tmp72E,({const char*_tmp427="cyclone_cc";_tag_dyneither(_tmp427,sizeof(char),11U);}));});Cyc_cyclone_cc=_tmp72F;});
if(!((unsigned int)Cyc_cyclone_cc.curr))({struct _dyneither_ptr _tmp730=({const char*_tmp428="gcc";_tag_dyneither(_tmp428,sizeof(char),4U);});Cyc_cyclone_cc=_tmp730;});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0U;_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp429[1U]={& _tmp42B};({struct Cyc___cycFILE*_tmp732=Cyc_stderr;struct _dyneither_ptr _tmp731=({const char*_tmp42A="C compiler is %s\n";_tag_dyneither(_tmp42A,sizeof(char),18U);});Cyc_fprintf(_tmp732,_tmp731,_tag_dyneither(_tmp429,sizeof(void*),1U));});});});
# 2169
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp42C=0U;({struct Cyc___cycFILE*_tmp734=Cyc_stderr;struct _dyneither_ptr _tmp733=({const char*_tmp42D="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp42D,sizeof(char),22U);});Cyc_fprintf(_tmp734,_tmp733,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
({struct Cyc___cycFILE*_tmp735=Cyc_fopen("BUILDLIB.sh","w");Cyc_script_file=_tmp735;});
if(!((unsigned int)Cyc_script_file)){
({void*_tmp42E=0U;({struct Cyc___cycFILE*_tmp737=Cyc_stderr;struct _dyneither_ptr _tmp736=({const char*_tmp42F="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp42F,sizeof(char),35U);});Cyc_fprintf(_tmp737,_tmp736,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});
 exit(1);}
# 2177
({void*_tmp430=0U;({struct _dyneither_ptr _tmp738=({const char*_tmp431="#!/bin/sh\n";_tag_dyneither(_tmp431,sizeof(char),11U);});Cyc_prscript(_tmp738,_tag_dyneither(_tmp430,sizeof(void*),0U));});});
({void*_tmp432=0U;({struct _dyneither_ptr _tmp739=({const char*_tmp433="GCC=\"gcc\"\n";_tag_dyneither(_tmp433,sizeof(char),11U);});Cyc_prscript(_tmp739,_tag_dyneither(_tmp432,sizeof(void*),0U));});});}
# 2182
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp436;_tmp436.tag=0U;_tmp436.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp434[1U]={& _tmp436};({struct Cyc___cycFILE*_tmp73B=Cyc_stderr;struct _dyneither_ptr _tmp73A=({const char*_tmp435="Error: could not create directory %s\n";_tag_dyneither(_tmp435,sizeof(char),38U);});Cyc_fprintf(_tmp73B,_tmp73A,_tag_dyneither(_tmp434,sizeof(void*),1U));});});});
return 1;}
# 2186
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp439;_tmp439.tag=0U;_tmp439.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp437[1U]={& _tmp439};({struct Cyc___cycFILE*_tmp73D=Cyc_stderr;struct _dyneither_ptr _tmp73C=({const char*_tmp438="Output directory is %s\n";_tag_dyneither(_tmp438,sizeof(char),24U);});Cyc_fprintf(_tmp73D,_tmp73C,_tag_dyneither(_tmp437,sizeof(void*),1U));});});});
# 2189
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp43C;_tmp43C.tag=0U;_tmp43C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp43A[1U]={& _tmp43C};({struct _dyneither_ptr _tmp73E=({const char*_tmp43B="cd %s\n";_tag_dyneither(_tmp43B,sizeof(char),7U);});Cyc_prscript(_tmp73E,_tag_dyneither(_tmp43A,sizeof(void*),1U));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp43F;_tmp43F.tag=0U;_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp43D[1U]={& _tmp43F};({struct _dyneither_ptr _tmp73F=({const char*_tmp43E="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp43E,sizeof(char),41U);});Cyc_prscript(_tmp73F,_tag_dyneither(_tmp43D,sizeof(void*),1U));});});});}
# 2195
if(!Cyc_gathering()){
# 2198
({struct Cyc___cycFILE*_tmp741=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp740=Cyc_output_dir;Cyc_Filename_concat(_tmp740,({const char*_tmp440="BUILDLIB.LOG";_tag_dyneither(_tmp440,sizeof(char),13U);}));}),sizeof(char),1U)),"w");Cyc_log_file=_tmp741;});
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp443;_tmp443.tag=0U;_tmp443.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp441[1U]={& _tmp443};({struct Cyc___cycFILE*_tmp743=Cyc_stderr;struct _dyneither_ptr _tmp742=({const char*_tmp442="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp442,sizeof(char),50U);});Cyc_fprintf(_tmp743,_tmp742,_tag_dyneither(_tmp441,sizeof(void*),1U));});});});
return 1;}
# 2204
if(!Cyc_do_setjmp){
# 2206
({struct Cyc___cycFILE*_tmp745=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp744=Cyc_output_dir;Cyc_Filename_concat(_tmp744,({const char*_tmp444="cstubs.c";_tag_dyneither(_tmp444,sizeof(char),9U);}));}),sizeof(char),1U)),"w");Cyc_cstubs_file=_tmp745;});
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp447;_tmp447.tag=0U;_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp445[1U]={& _tmp447};({struct Cyc___cycFILE*_tmp747=Cyc_stderr;struct _dyneither_ptr _tmp746=({const char*_tmp446="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp446,sizeof(char),50U);});Cyc_fprintf(_tmp747,_tmp746,_tag_dyneither(_tmp445,sizeof(void*),1U));});});});
return 1;}
# 2213
({struct Cyc___cycFILE*_tmp749=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp748=Cyc_output_dir;Cyc_Filename_concat(_tmp748,({const char*_tmp448="cycstubs.cyc";_tag_dyneither(_tmp448,sizeof(char),13U);}));}),sizeof(char),1U)),"w");Cyc_cycstubs_file=_tmp749;});
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp44B;_tmp44B.tag=0U;_tmp44B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp449[1U]={& _tmp44B};({struct Cyc___cycFILE*_tmp74B=Cyc_stderr;struct _dyneither_ptr _tmp74A=({const char*_tmp44A="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp44A,sizeof(char),52U);});Cyc_fprintf(_tmp74B,_tmp74A,_tag_dyneither(_tmp449,sizeof(void*),1U));});});});
# 2218
return 1;}
# 2220
({void*_tmp44C=0U;({struct Cyc___cycFILE*_tmp74D=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _dyneither_ptr _tmp74C=({const char*_tmp44D="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp44D,sizeof(char),32U);});Cyc_fprintf(_tmp74D,_tmp74C,_tag_dyneither(_tmp44C,sizeof(void*),0U));});});}}{
# 2227
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 2233
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp44E=0U;({struct Cyc___cycFILE*_tmp74F=Cyc_stderr;struct _dyneither_ptr _tmp74E=({const char*_tmp44F="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp44F,sizeof(char),22U);});Cyc_fprintf(_tmp74F,_tmp74E,_tag_dyneither(_tmp44E,sizeof(void*),0U));});});
 exit(1);}}}
# 2241
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2244
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2252
return 0;};};};};}

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
# 69 "core.h"
void exit(int);
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;
# 38
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
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
# 127
typedef void*Cyc_sarg_t;
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
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
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
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
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 33
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1110 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1173
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1185
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 33
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 44
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 49
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
typedef struct _tuple11*Cyc_declarator_list_t;
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 56
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 64
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 68
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};
# 80
typedef void*Cyc_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_pointer_quals_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Dict_T;
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
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 46
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 49
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 36 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 47
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 63
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 67
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 78
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 94 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 112
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 33 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 30 "binding.h"
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
# 29 "specsfile.h"
struct _dyneither_ptr Cyc_Specsfile_target_arch;
struct _dyneither_ptr Cyc_Specsfile_btarget;
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s);
typedef struct Cyc_List_List*Cyc_Specsfile_specs_t;
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file);
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline);
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path;
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv);
# 45
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s);
# 76 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4]="gcc";
# 86
static struct _dyneither_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4};static char _tmp1[1]="";
static struct _dyneither_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1};
# 89
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
# 92
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 96
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 99
if(Cyc_log_file == 0){
({void*_tmp2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34);}),_tag_dyneither(_tmp2,sizeof(void*),0));});
 exit(1);}{
# 103
int _tmp4=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp4;};}
# 108
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);_tmp5;});}struct _tuple20{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 115
typedef struct _tuple20*Cyc_line_t;
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 122
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple21{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 129
typedef struct _tuple21*Cyc_suck_line_t;
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 137
typedef struct _tuple22*Cyc_ifdefined_t;struct _tuple23{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};
# 139
typedef struct _tuple23*Cyc_spec_t;
# 141
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 159
int Cyc_numdef=0;
# 161
static struct Cyc_List_List*Cyc_cppargs=0;
# 164
const int Cyc_lex_base[425]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,- 18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,1171,1185,1255};
const int Cyc_lex_backtrk[425]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[425]={- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3481]={0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,45,45,45,22,45,0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,415,415,415,415,415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,31,217,218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,413,413,413,413,121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,61,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,129,34,34,34,34,34,34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,104,327,328,329,330,331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,19,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,37,308,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,35,35,35,35,178,172,163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,169,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,97,97,104,104,97,176,104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,21,97,182,104,183,184,185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,112,91,197,112,198,199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,21,112,200,201,202,206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,219,220,92,222,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,225,- 1,226,- 1,227,228,99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,235,236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,243,240,50,241,244,245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,137,137,137,137,137,137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,259,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,102,100,261,262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,33,269,270,272,273,288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,282,36,284,285,286,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,291,44,292,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,44,411,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,410,408,78,- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,351,383,346,341,78,343,344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,79,79,79,79,79,79,79,79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,294,155,80,287,287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,278,80,270,223,275,275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,79,79,79,79,79,275,100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,294,419,292,316,316,277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,292,0,316,0,0,316,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,0,0,0,92,0,300,412,0,412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,412,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,378,0,0,0,0,357,0,357,0,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,358,358,358,358,358,358,358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,405,405,405,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,138,312,312,312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,136,136,136,136,136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,314,314,314,314,314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,314,314,314,314,314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,314,0,0,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,314,314,314,314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,353,353,353,353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,0,0,0,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,356,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,359,359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,352,93,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,359,359,359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,93,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,0,0,0,0,373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,354,0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,378,378,378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,368,368,368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,360,93,0,0,0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,368,368,368,368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,370,0,0,390,390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,369,0,0,0,0,0,0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,395,0,0,0,0,0,0,0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,402,0,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,413,413,413,413,413,413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,0,0,0,0,413,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3481]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,41,41,45,45,41,417,45,- 1,- 1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,166,167,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,169,88,170,171,172,173,174,96,96,104,104,96,175,104,105,105,176,178,105,179,88,180,13,13,80,80,13,73,80,16,96,181,104,182,183,184,185,105,186,187,188,189,190,192,193,13,194,80,195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,222,92,224,103,225,110,226,227,99,99,228,14,99,229,230,231,232,32,32,32,32,32,32,128,128,128,128,128,128,128,128,99,234,235,236,238,111,237,130,15,15,237,239,15,101,101,240,241,101,242,237,14,237,243,244,245,14,14,246,247,248,249,15,14,250,251,252,101,134,134,134,134,134,134,134,134,253,254,15,15,15,15,15,15,15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,276,277,278,33,33,33,33,33,33,137,137,137,137,137,137,137,137,279,280,281,282,283,284,285,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,294,295,296,15,297,298,299,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,329,273,312,312,312,312,312,312,312,312,332,326,334,337,78,335,335,323,139,139,273,344,139,140,140,324,327,140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,139,388,79,287,287,140,389,287,391,318,139,318,318,392,393,140,355,356,365,366,140,79,394,395,275,275,287,396,275,293,293,338,318,293,287,397,398,79,79,79,79,79,79,79,79,79,79,275,355,356,365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,89,292,292,293,418,292,309,309,275,419,309,420,421,293,371,422,372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,126,126,390,390,390,390,390,390,390,390,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,405,405,405,405,405,- 1,136,136,136,136,136,136,- 1,313,313,313,313,313,313,313,313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,361,361,361,361,361,361,361,- 1,- 1,314,314,314,314,314,314,314,314,314,314,310,313,313,313,313,313,313,314,314,314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,352,352,352,352,352,352,352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,- 1,353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,363,- 1,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,- 1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,- 1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,368,368,368,368,368,368,368,368,368,- 1,367,367,367,367,367,367,368,368,368,368,368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 171
int state;int base;int backtrk;
int c;
state=start_state;
# 175
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 179
state=(- state)- 1;}
# 181
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(425,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 189
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 193
c=256;}}else{
# 195
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 198
if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
# 201
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Lexing_Error_exn_struct _tmp7;_tmp7.tag=Cyc_Lexing_Error;_tmp7.f1=({const char*_tmp8="empty token";_tag_dyneither(_tmp8,sizeof(char),12);});_tmp7;});_tmp6;}));else{
# 207
return lbuf->lex_last_action;}}else{
# 210
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 214
struct _tuple20*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp9=lexstate;switch(_tmp9){case 0: _LL1: _LL2:
# 173 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);_tmpA;});}
# 178
return({struct _tuple20*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmpB->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmpB;});case 1: _LL3: _LL4:
# 181 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL5: _LL6:
# 183
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 187
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct Cyc_Lexing_Error_exn_struct _tmpD;_tmpD.tag=Cyc_Lexing_Error;_tmpD.f1=({const char*_tmpE="some action didn't return!";_tag_dyneither(_tmpE,sizeof(char),27);});_tmpD;});_tmpC;}));}
# 189
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpF=lexstate;switch(_tmpF){case 0: _LLA: _LLB:
# 187 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp10;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp11;});
Cyc_token(lexbuf);
return 0;case 1: _LLC: _LLD:
# 195
 Cyc_current_source=({struct _dyneither_ptr*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp12;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp13;});
Cyc_args(lexbuf);
return 0;case 2: _LLE: _LLF:
# 203
 Cyc_current_source=({struct _dyneither_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp14;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp15;});
Cyc_token(lexbuf);
return 0;default: _LL10: _LL11:
# 209
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL9:;}
# 212
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Lexing_Error_exn_struct _tmp17;_tmp17.tag=Cyc_Lexing_Error;_tmp17.f1=({const char*_tmp18="some action didn't return!";_tag_dyneither(_tmp18,sizeof(char),27);});_tmp17;});_tmp16;}));}
# 214
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp19=lexstate;switch(_tmp19){case 0: _LL13: _LL14: {
# 212 "buildlib.cyl"
struct _dyneither_ptr*_tmp1A=({struct _dyneither_ptr*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp1C;});
Cyc_current_args=({struct Cyc_List_List*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->hd=_tmp1A;_tmp1B->tl=Cyc_current_args;_tmp1B;});
return Cyc_args(lexbuf);}case 1: _LL15: _LL16: {
# 218
struct _dyneither_ptr*_tmp1D=({struct _dyneither_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp1F;});
Cyc_current_args=({struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=_tmp1D;_tmp1E->tl=Cyc_current_args;_tmp1E;});
return Cyc_args(lexbuf);}case 2: _LL17: _LL18: {
# 224
struct _dyneither_ptr*_tmp20=({struct _dyneither_ptr*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp22;});
Cyc_current_args=({struct Cyc_List_List*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->hd=_tmp20;_tmp21->tl=Cyc_current_args;_tmp21;});
return Cyc_token(lexbuf);}default: _LL19: _LL1A:
# 229
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL12:;}
# 232
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Lexing_Error_exn_struct _tmp24;_tmp24.tag=Cyc_Lexing_Error;_tmp24.f1=({const char*_tmp25="some action didn't return!";_tag_dyneither(_tmp25,sizeof(char),27);});_tmp24;});_tmp23;}));}
# 234
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp26=lexstate;switch(_tmp26){case 0: _LL1C: _LL1D:
# 233 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp27;}));return Cyc_token(lexbuf);case 1: _LL1E: _LL1F:
# 236 "buildlib.cyl"
 return 0;case 2: _LL20: _LL21:
# 239 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL22: _LL23:
# 242 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL24: _LL25:
# 245 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL26: _LL27:
# 247
 return Cyc_token(lexbuf);case 6: _LL28: _LL29:
# 249
 return Cyc_token(lexbuf);case 7: _LL2A: _LL2B:
# 251
 return Cyc_token(lexbuf);case 8: _LL2C: _LL2D:
# 254 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL2E: _LL2F:
# 257 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL30: _LL31:
# 260 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL32: _LL33:
# 262
 return Cyc_token(lexbuf);case 12: _LL34: _LL35:
# 264
 return Cyc_token(lexbuf);case 13: _LL36: _LL37:
# 266
 return Cyc_token(lexbuf);case 14: _LL38: _LL39:
# 268
 return Cyc_token(lexbuf);case 15: _LL3A: _LL3B:
# 270
 return Cyc_token(lexbuf);case 16: _LL3C: _LL3D:
# 272
 return Cyc_token(lexbuf);case 17: _LL3E: _LL3F:
# 274
 return Cyc_token(lexbuf);case 18: _LL40: _LL41:
# 276
 return Cyc_token(lexbuf);case 19: _LL42: _LL43:
# 278
 return Cyc_token(lexbuf);case 20: _LL44: _LL45:
# 280
 return Cyc_token(lexbuf);case 21: _LL46: _LL47:
# 282
 return Cyc_token(lexbuf);case 22: _LL48: _LL49:
# 284
 return Cyc_token(lexbuf);case 23: _LL4A: _LL4B:
# 286
 return Cyc_token(lexbuf);case 24: _LL4C: _LL4D:
# 289 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LL4E: _LL4F:
# 291
 return Cyc_token(lexbuf);case 26: _LL50: _LL51:
# 293
 return Cyc_token(lexbuf);case 27: _LL52: _LL53:
# 295
 return Cyc_token(lexbuf);case 28: _LL54: _LL55:
# 297
 return Cyc_token(lexbuf);case 29: _LL56: _LL57:
# 299
 return Cyc_token(lexbuf);case 30: _LL58: _LL59:
# 301
 return Cyc_token(lexbuf);case 31: _LL5A: _LL5B:
# 303
 return Cyc_token(lexbuf);case 32: _LL5C: _LL5D:
# 305
 return Cyc_token(lexbuf);case 33: _LL5E: _LL5F:
# 307
 return Cyc_token(lexbuf);case 34: _LL60: _LL61:
# 309
 return Cyc_token(lexbuf);case 35: _LL62: _LL63:
# 311
 return Cyc_token(lexbuf);case 36: _LL64: _LL65:
# 313
 return Cyc_token(lexbuf);case 37: _LL66: _LL67:
# 315
 return Cyc_token(lexbuf);case 38: _LL68: _LL69:
# 317
 return Cyc_token(lexbuf);case 39: _LL6A: _LL6B:
# 319
 return Cyc_token(lexbuf);case 40: _LL6C: _LL6D:
# 321
 return Cyc_token(lexbuf);case 41: _LL6E: _LL6F:
# 323
 return Cyc_token(lexbuf);case 42: _LL70: _LL71:
# 325
 return Cyc_token(lexbuf);case 43: _LL72: _LL73:
# 327
 return Cyc_token(lexbuf);case 44: _LL74: _LL75:
# 329
 return Cyc_token(lexbuf);case 45: _LL76: _LL77:
# 331
 return Cyc_token(lexbuf);case 46: _LL78: _LL79:
# 333
 return Cyc_token(lexbuf);case 47: _LL7A: _LL7B:
# 335
 return Cyc_token(lexbuf);case 48: _LL7C: _LL7D:
# 338 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL7E: _LL7F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL1B:;}
# 342
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Lexing_Error_exn_struct _tmp29;_tmp29.tag=Cyc_Lexing_Error;_tmp29.f1=({const char*_tmp2A="some action didn't return!";_tag_dyneither(_tmp2A,sizeof(char),27);});_tmp29;});_tmp28;}));}
# 344
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2B=lexstate;switch(_tmp2B){case 0: _LL81: _LL82:
# 343 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LL83: _LL84:
# 344 "buildlib.cyl"
 return 0;case 2: _LL85: _LL86:
# 345 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LL87: _LL88:
# 346 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LL89: _LL8A:
# 349 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LL8B: _LL8C:
# 352 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LL8D: _LL8E:
# 354
 return Cyc_string(lexbuf);case 7: _LL8F: _LL90:
# 355 "buildlib.cyl"
 return 0;case 8: _LL91: _LL92:
# 356 "buildlib.cyl"
 return 0;case 9: _LL93: _LL94:
# 357 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL95: _LL96:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL80:;}
# 361
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_Lexing_Error_exn_struct _tmp2D;_tmp2D.tag=Cyc_Lexing_Error;_tmp2D.f1=({const char*_tmp2E="some action didn't return!";_tag_dyneither(_tmp2E,sizeof(char),27);});_tmp2D;});_tmp2C;}));}
# 363
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2F=lexstate;switch(_tmp2F){case 0: _LL98: _LL99:
# 366 "buildlib.cyl"
 return 0;case 1: _LL9A: _LL9B:
# 368
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL9C: _LL9D:
# 375 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp30=0;Cyc_log(({const char*_tmp31="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp31,sizeof(char),44);}),_tag_dyneither(_tmp30,sizeof(void*),0));});
return 1;case 3: _LL9E: _LL9F:
# 381 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp32=0;Cyc_log(({const char*_tmp33="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp33,sizeof(char),44);}),_tag_dyneither(_tmp32,sizeof(void*),0));});
return 1;case 4: _LLA0: _LLA1:
# 387 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp34=0;Cyc_log(({const char*_tmp35="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp35,sizeof(char),44);}),_tag_dyneither(_tmp34,sizeof(void*),0));});
return 1;case 5: _LLA2: _LLA3:
# 393 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp36=0;Cyc_log(({const char*_tmp37="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp37,sizeof(char),44);}),_tag_dyneither(_tmp36,sizeof(void*),0));});
return 1;case 6: _LLA4: _LLA5:
# 397
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp38=0;Cyc_log(({const char*_tmp39="Warning: use of region sidestepped\n";_tag_dyneither(_tmp39,sizeof(char),36);}),_tag_dyneither(_tmp38,sizeof(void*),0));});
return 1;case 7: _LLA6: _LLA7:
# 401
({void*_tmp3A=0;Cyc_log(({const char*_tmp3B="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmp3B,sizeof(char),39);}),_tag_dyneither(_tmp3A,sizeof(void*),0));});
return 1;case 8: _LLA8: _LLA9:
# 406 "buildlib.cyl"
({void*_tmp3C=0;Cyc_log(({const char*_tmp3D="Warning: use of mode HI deleted\n";_tag_dyneither(_tmp3D,sizeof(char),33);}),_tag_dyneither(_tmp3C,sizeof(void*),0));});
return 1;case 9: _LLAA: _LLAB:
# 409
({void*_tmp3E=0;Cyc_log(({const char*_tmp3F="Warning: use of mode SI deleted\n";_tag_dyneither(_tmp3F,sizeof(char),33);}),_tag_dyneither(_tmp3E,sizeof(void*),0));});
return 1;case 10: _LLAC: _LLAD:
# 412
({void*_tmp40=0;Cyc_log(({const char*_tmp41="Warning: use of mode QI deleted\n";_tag_dyneither(_tmp41,sizeof(char),33);}),_tag_dyneither(_tmp40,sizeof(void*),0));});
return 1;case 11: _LLAE: _LLAF:
# 415
({void*_tmp42=0;Cyc_log(({const char*_tmp43="Warning: use of mode DI deleted\n";_tag_dyneither(_tmp43,sizeof(char),33);}),_tag_dyneither(_tmp42,sizeof(void*),0));});
return 1;case 12: _LLB0: _LLB1:
# 418
({void*_tmp44=0;Cyc_log(({const char*_tmp45="Warning: use of mode DI deleted\n";_tag_dyneither(_tmp45,sizeof(char),33);}),_tag_dyneither(_tmp44,sizeof(void*),0));});
return 1;case 13: _LLB2: _LLB3:
# 421
({void*_tmp46=0;Cyc_log(({const char*_tmp47="Warning: use of mode word deleted\n";_tag_dyneither(_tmp47,sizeof(char),35);}),_tag_dyneither(_tmp46,sizeof(void*),0));});
return 1;case 14: _LLB4: _LLB5:
# 424
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LLB6: _LLB7:
# 426
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LLB8: _LLB9:
# 428
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17: _LLBA: _LLBB:
# 430
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LLBC: _LLBD:
# 435 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19: _LLBE: _LLBF:
# 437
 return 1;case 20: _LLC0: _LLC1:
# 439
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp48=0;Cyc_log(({const char*_tmp49="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmp49,sizeof(char),42);}),_tag_dyneither(_tmp48,sizeof(void*),0));});
return 1;case 21: _LLC2: _LLC3:
# 445
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default: _LLC4: _LLC5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL97:;}
# 449
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Lexing_Error_exn_struct _tmp4B;_tmp4B.tag=Cyc_Lexing_Error;_tmp4B.f1=({const char*_tmp4C="some action didn't return!";_tag_dyneither(_tmp4C,sizeof(char),27);});_tmp4B;});_tmp4A;}));}
# 451
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp4D=lexstate;switch(_tmp4D){case 0: _LLC7: _LLC8:
# 449 "buildlib.cyl"
 return 0;case 1: _LLC9: _LLCA:
# 451
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LLCB: _LLCC:
# 453
({void*_tmp4E=0;Cyc_log(({const char*_tmp4F="Warning: unclosed string\n";_tag_dyneither(_tmp4F,sizeof(char),26);}),_tag_dyneither(_tmp4E,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp50[1]={& _tmp52};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp51="%s";_tag_dyneither(_tmp51,sizeof(char),3);}),_tag_dyneither(_tmp50,sizeof(void*),1));});});return 1;case 3: _LLCD: _LLCE:
# 456
({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp53[1]={& _tmp55};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp54="%s";_tag_dyneither(_tmp54,sizeof(char),3);}),_tag_dyneither(_tmp53,sizeof(void*),1));});});return 1;case 4: _LLCF: _LLD0:
# 458
({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp56[1]={& _tmp58};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp57="%s";_tag_dyneither(_tmp57,sizeof(char),3);}),_tag_dyneither(_tmp56,sizeof(void*),1));});});return 1;case 5: _LLD1: _LLD2:
# 460
({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp59[1]={& _tmp5B};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp5A="%s";_tag_dyneither(_tmp5A,sizeof(char),3);}),_tag_dyneither(_tmp59,sizeof(void*),1));});});return 1;case 6: _LLD3: _LLD4:
# 462
({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp5C[1]={& _tmp5E};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp5D="%s";_tag_dyneither(_tmp5D,sizeof(char),3);}),_tag_dyneither(_tmp5C,sizeof(void*),1));});});return 1;case 7: _LLD5: _LLD6:
# 464
({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp5F[1]={& _tmp61};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp60="%s";_tag_dyneither(_tmp60,sizeof(char),3);}),_tag_dyneither(_tmp5F,sizeof(void*),1));});});return 1;case 8: _LLD7: _LLD8:
# 466
({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp62[1]={& _tmp64};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp63="%s";_tag_dyneither(_tmp63,sizeof(char),3);}),_tag_dyneither(_tmp62,sizeof(void*),1));});});return 1;default: _LLD9: _LLDA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LLC6:;}
# 470
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Lexing_Error_exn_struct _tmp66;_tmp66.tag=Cyc_Lexing_Error;_tmp66.f1=({const char*_tmp67="some action didn't return!";_tag_dyneither(_tmp67,sizeof(char),27);});_tmp66;});_tmp65;}));}
# 472
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp68=lexstate;switch(_tmp68){case 0: _LLDC: _LLDD:
# 476 "buildlib.cyl"
 return 0;case 1: _LLDE: _LLDF:
# 478
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LLE0: _LLE1:
# 482
 ++ Cyc_parens_to_match;
return 1;case 3: _LLE2: _LLE3:
# 485
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LLE4: _LLE5:
# 488
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LLE6: _LLE7:
# 491
 return 1;case 6: _LLE8: _LLE9:
# 493
 return 1;default: _LLEA: _LLEB:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LLDB:;}
# 497
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Lexing_Error_exn_struct _tmp6A;_tmp6A.tag=Cyc_Lexing_Error;_tmp6A.f1=({const char*_tmp6B="some action didn't return!";_tag_dyneither(_tmp6B,sizeof(char),27);});_tmp6A;});_tmp69;}));}
# 499
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp6C=lexstate;switch(_tmp6C){case 0: _LLED: _LLEE:
# 497 "buildlib.cyl"
({void*_tmp6D=0;Cyc_log(({const char*_tmp6E="Warning: unclosed string\n";_tag_dyneither(_tmp6E,sizeof(char),26);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});return 0;case 1: _LLEF: _LLF0:
# 499
 return 0;case 2: _LLF1: _LLF2:
# 501
({void*_tmp6F=0;Cyc_log(({const char*_tmp70="Warning: unclosed string\n";_tag_dyneither(_tmp70,sizeof(char),26);}),_tag_dyneither(_tmp6F,sizeof(void*),0));});return 1;case 3: _LLF3: _LLF4:
# 503
 return 1;case 4: _LLF5: _LLF6:
# 505
 return 1;case 5: _LLF7: _LLF8:
# 507
 return 1;case 6: _LLF9: _LLFA:
# 509
 return 1;case 7: _LLFB: _LLFC:
# 511
 return 1;case 8: _LLFD: _LLFE:
# 513
 return 1;default: _LLFF: _LL100:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LLEC:;}
# 517
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Lexing_Error_exn_struct _tmp72;_tmp72.tag=Cyc_Lexing_Error;_tmp72.f1=({const char*_tmp73="some action didn't return!";_tag_dyneither(_tmp73,sizeof(char),27);});_tmp72;});_tmp71;}));}
# 519
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp74=lexstate;switch(_tmp74){case 0: _LL102: _LL103:
# 517 "buildlib.cyl"
({void*_tmp75=0;Cyc_log(({const char*_tmp76="Warning: unclosed comment\n";_tag_dyneither(_tmp76,sizeof(char),27);}),_tag_dyneither(_tmp75,sizeof(void*),0));});return 0;case 1: _LL104: _LL105:
# 519
 return 0;case 2: _LL106: _LL107:
# 521
 return 1;default: _LL108: _LL109:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL101:;}
# 525
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Lexing_Error_exn_struct _tmp78;_tmp78.tag=Cyc_Lexing_Error;_tmp78.f1=({const char*_tmp79="some action didn't return!";_tag_dyneither(_tmp79,sizeof(char),27);});_tmp78;});_tmp77;}));}
# 527
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple21*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7A=lexstate;switch(_tmp7A){case 0: _LL10B: _LL10C:
# 529 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp7B="#define ";_tag_dyneither(_tmp7B,sizeof(char),9);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple21*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=Cyc_current_line;_tmp7C->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp7C;});case 1: _LL10D: _LL10E:
# 533
 return Cyc_suck_line(lexbuf);case 2: _LL10F: _LL110:
# 535
 return 0;default: _LL111: _LL112:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL10A:;}
# 539
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Lexing_Error_exn_struct _tmp7E;_tmp7E.tag=Cyc_Lexing_Error;_tmp7E.f1=({const char*_tmp7F="some action didn't return!";_tag_dyneither(_tmp7F,sizeof(char),27);});_tmp7E;});_tmp7D;}));}
# 541
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp80=lexstate;if(_tmp80 == 0){_LL114: _LL115:
# 539 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp81;});
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL116: _LL117:
# 543
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL113:;}
# 546
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Lexing_Error_exn_struct _tmp83;_tmp83.tag=Cyc_Lexing_Error;_tmp83.f1=({const char*_tmp84="some action didn't return!";_tag_dyneither(_tmp84,sizeof(char),27);});_tmp83;});_tmp82;}));}
# 548
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp85=lexstate;if(_tmp85 == 0){_LL119: _LL11A:
# 546 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;}else{_LL11B: _LL11C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL118:;}
# 550
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Lexing_Error_exn_struct _tmp87;_tmp87.tag=Cyc_Lexing_Error;_tmp87.f1=({const char*_tmp88="some action didn't return!";_tag_dyneither(_tmp88,sizeof(char),27);});_tmp87;});_tmp86;}));}
# 552
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple23*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp89=lexstate;switch(_tmp89){case 0: _LL11E: _LL11F:
# 553 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL120: _LL121:
# 555
 Cyc_current_headerfile=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));
Cyc_current_symbols=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);
return({struct _tuple23*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=Cyc_current_headerfile;_tmp8A->f2=Cyc_current_symbols;_tmp8A->f3=Cyc_current_omit_symbols;_tmp8A->f4=Cyc_current_hstubs;_tmp8A->f5=Cyc_current_cstubs;_tmp8A->f6=Cyc_current_cycstubs;_tmp8A;});case 2: _LL122: _LL123:
# 575
 return Cyc_spec(lexbuf);case 3: _LL124: _LL125:
# 577
 return 0;case 4: _LL126: _LL127:
# 579
({struct Cyc_Int_pa_PrintArg_struct _tmp8D;_tmp8D.tag=1;_tmp8D.f1=(unsigned long)((int)
# 581
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmp8B[1]={& _tmp8D};Cyc_fprintf(Cyc_stderr,({const char*_tmp8C="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp8C,sizeof(char),67);}),_tag_dyneither(_tmp8B,sizeof(void*),1));});});
return 0;default: _LL128: _LL129:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL11D:;}
# 586
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Lexing_Error_exn_struct _tmp8F;_tmp8F.tag=Cyc_Lexing_Error;_tmp8F.f1=({const char*_tmp90="some action didn't return!";_tag_dyneither(_tmp90,sizeof(char),27);});_tmp8F;});_tmp8E;}));}
# 588
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp91=lexstate;switch(_tmp91){case 0: _LL12B: _LL12C:
# 586 "buildlib.cyl"
 return 0;case 1: _LL12D: _LL12E:
# 588
 return 0;case 2: _LL12F: _LL130:
# 590
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL131: _LL132:
# 595
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;case 4: _LL133: _LL134:
# 600
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp93->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp93;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->hd=x;_tmp92->tl=Cyc_current_hstubs;_tmp92;});
return 1;};case 5: _LL135: _LL136: {
# 608
struct _dyneither_ptr _tmp94=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp94,sizeof(char),(int)Cyc_strlen(({const char*_tmp95="hstub";_tag_dyneither(_tmp95,sizeof(char),6);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp94,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp94,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp94;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp96=Cyc_substring((struct _dyneither_ptr)_tmp94,0,(unsigned long)((t.curr - _tmp94.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->f1=(struct _dyneither_ptr)_tmp96;_tmp98->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x;_tmp97->tl=Cyc_current_hstubs;_tmp97;});
return 1;};};};}case 6: _LL137: _LL138:
# 622
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp9A->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9A;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->hd=x;_tmp99->tl=Cyc_current_cstubs;_tmp99;});
return 1;};case 7: _LL139: _LL13A: {
# 630
struct _dyneither_ptr _tmp9B=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp9B,sizeof(char),(int)Cyc_strlen(({const char*_tmp9C="cstub";_tag_dyneither(_tmp9C,sizeof(char),6);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp9B,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp9B,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp9B;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp9D=Cyc_substring((struct _dyneither_ptr)_tmp9B,0,(unsigned long)((t.curr - _tmp9B.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=(struct _dyneither_ptr)_tmp9D;_tmp9F->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9F;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=x;_tmp9E->tl=Cyc_current_cstubs;_tmp9E;});
return 1;};};};}case 8: _LL13B: _LL13C:
# 644
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmpA1->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA1;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->hd=x;_tmpA0->tl=Cyc_current_cycstubs;_tmpA0;});
return 1;};case 9: _LL13D: _LL13E: {
# 652
struct _dyneither_ptr _tmpA2=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmpA2,sizeof(char),(int)Cyc_strlen(({const char*_tmpA3="cycstub";_tag_dyneither(_tmpA3,sizeof(char),8);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmpA2,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmpA2,sizeof(char),1);}{
struct _dyneither_ptr t=_tmpA2;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmpA4=Cyc_substring((struct _dyneither_ptr)_tmpA2,0,(unsigned long)((t.curr - _tmpA2.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->f1=(struct _dyneither_ptr)_tmpA4;_tmpA6->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA6;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=x;_tmpA5->tl=Cyc_current_cycstubs;_tmpA5;});
return 1;};};};}case 10: _LL13F: _LL140:
# 666
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA8;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->hd=x;_tmpA7->tl=Cyc_current_cpp;_tmpA7;});
return 1;};case 11: _LL141: _LL142:
# 673
 return 1;case 12: _LL143: _LL144:
# 675
 return 1;case 13: _LL145: _LL146:
# 677
({struct Cyc_Int_pa_PrintArg_struct _tmpAB;_tmpAB.tag=1;_tmpAB.f1=(unsigned long)((int)
# 679
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmpA9[1]={& _tmpAB};Cyc_fprintf(Cyc_stderr,({const char*_tmpAA="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmpAA,sizeof(char),58);}),_tag_dyneither(_tmpA9,sizeof(void*),1));});});
return 0;default: _LL147: _LL148:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL12A:;}
# 684
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Lexing_Error_exn_struct _tmpAD;_tmpAD.tag=Cyc_Lexing_Error;_tmpAD.f1=({const char*_tmpAE="some action didn't return!";_tag_dyneither(_tmpAE,sizeof(char),27);});_tmpAD;});_tmpAC;}));}
# 686
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpAF=lexstate;switch(_tmpAF){case 0: _LL14A: _LL14B:
# 689 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->hd=({struct _dyneither_ptr*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpB1;});_tmpB0->tl=Cyc_snarfed_symbols;_tmpB0;});
return 1;case 1: _LL14C: _LL14D:
# 692
 return 1;case 2: _LL14E: _LL14F:
# 694
 return 0;case 3: _LL150: _LL151:
# 696
({void*_tmpB2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB3="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmpB3,sizeof(char),44);}),_tag_dyneither(_tmpB2,sizeof(void*),0));});
# 698
return 0;case 4: _LL152: _LL153:
# 700
({struct Cyc_Int_pa_PrintArg_struct _tmpB6;_tmpB6.tag=1;_tmpB6.f1=(unsigned long)((int)
# 702
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmpB4[1]={& _tmpB6};Cyc_fprintf(Cyc_stderr,({const char*_tmpB5="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmpB5,sizeof(char),57);}),_tag_dyneither(_tmpB4,sizeof(void*),1));});});
return 0;default: _LL154: _LL155:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL149:;}
# 707
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Lexing_Error_exn_struct _tmpB8;_tmpB8.tag=Cyc_Lexing_Error;_tmpB8.f1=({const char*_tmpB9="some action didn't return!";_tag_dyneither(_tmpB9,sizeof(char),27);});_tmpB8;});_tmpB7;}));}
# 709
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpBA=lexstate;switch(_tmpBA){case 0: _LL157: _LL158:
# 713 "buildlib.cyl"
({void*_tmpBB=0;Cyc_log(({const char*_tmpBC="Warning: unclosed brace\n";_tag_dyneither(_tmpBC,sizeof(char),25);}),_tag_dyneither(_tmpBB,sizeof(void*),0));});return 0;case 1: _LL159: _LL15A:
# 715
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LL15B: _LL15C:
# 720
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LL15D: _LL15E:
# 724
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LL15F: _LL160:
# 728
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmpBD="/*";_tag_dyneither(_tmpBD,sizeof(char),3);}));
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LL161: _LL162:
# 732
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL163: _LL164:
# 735
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LL165: _LL166:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL156:;}
# 740
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Lexing_Error_exn_struct _tmpBF;_tmpBF.tag=Cyc_Lexing_Error;_tmpBF.f1=({const char*_tmpC0="some action didn't return!";_tag_dyneither(_tmpC0,sizeof(char),27);});_tmpBF;});_tmpBE;}));}
# 742
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC1=lexstate;switch(_tmpC1){case 0: _LL168: _LL169:
# 740 "buildlib.cyl"
({void*_tmpC2=0;Cyc_log(({const char*_tmpC3="Warning: unclosed string\n";_tag_dyneither(_tmpC3,sizeof(char),26);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});return 0;case 1: _LL16A: _LL16B:
# 742
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LL16C: _LL16D:
# 744
({void*_tmpC4=0;Cyc_log(({const char*_tmpC5="Warning: unclosed string\n";_tag_dyneither(_tmpC5,sizeof(char),26);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LL16E: _LL16F:
# 748
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 4: _LL170: _LL171:
# 751
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 5: _LL172: _LL173:
# 754
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL174: _LL175:
# 757
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 7: _LL176: _LL177:
# 760
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 8: _LL178: _LL179:
# 763
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL17A: _LL17B:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL167:;}
# 768
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Lexing_Error_exn_struct _tmpC7;_tmpC7.tag=Cyc_Lexing_Error;_tmpC7.f1=({const char*_tmpC8="some action didn't return!";_tag_dyneither(_tmpC8,sizeof(char),27);});_tmpC7;});_tmpC6;}));}
# 770
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC9=lexstate;switch(_tmpC9){case 0: _LL17D: _LL17E:
# 768 "buildlib.cyl"
({void*_tmpCA=0;Cyc_log(({const char*_tmpCB="Warning: unclosed comment\n";_tag_dyneither(_tmpCB,sizeof(char),27);}),_tag_dyneither(_tmpCA,sizeof(void*),0));});return 0;case 1: _LL17F: _LL180:
# 770
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmpCC="*/";_tag_dyneither(_tmpCC,sizeof(char),3);}));return 0;case 2: _LL181: _LL182:
# 772
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL183: _LL184:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL17C:;}
# 777
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Lexing_Error_exn_struct _tmpCE;_tmpCE.tag=Cyc_Lexing_Error;_tmpCE.f1=({const char*_tmpCF="some action didn't return!";_tag_dyneither(_tmpCF,sizeof(char),27);});_tmpCE;});_tmpCD;}));}
# 779
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 782 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 785
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpD0=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmpD1=_tmpD0;struct Cyc_List_List*_tmp11D;void*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct _dyneither_ptr*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct _dyneither_ptr*_tmp117;void*_tmp116;void*_tmp115;struct Cyc_Absyn_Exp*_tmp114;int _tmp113;struct Cyc_Absyn_Exp*_tmp112;void**_tmp111;struct Cyc_Absyn_Exp*_tmp110;void*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_List_List*_tmpFA;void*_tmpF9;switch(*((int*)_tmpD1)){case 1: _LL186: _tmpF9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL187: {
# 789
struct _dyneither_ptr*_tmpD2=(*Cyc_Absyn_binding2qvar(_tmpF9)).f2;
Cyc_add_target(_tmpD2);
return;}case 2: _LL188: _tmpFA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL189:
# 793
 for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpFA->hd,dep);}
# 796
return;case 22: _LL18A: _tmpFC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmpFB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL18B:
# 798
 _tmpFE=_tmpFC;_tmpFD=_tmpFB;goto _LL18D;case 8: _LL18C: _tmpFE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmpFD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL18D:
# 800
 _tmp100=_tmpFE;_tmpFF=_tmpFD;goto _LL18F;case 3: _LL18E: _tmp100=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmpFF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD1)->f3;_LL18F:
# 802
 Cyc_scan_exp(_tmp100,dep);
Cyc_scan_exp(_tmpFF,dep);
return;case 19: _LL190: _tmp101=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL191:
# 806
 _tmp102=_tmp101;goto _LL193;case 17: _LL192: _tmp102=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL193:
# 808
 _tmp103=_tmp102;goto _LL195;case 14: _LL194: _tmp103=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL195:
# 810
 _tmp104=_tmp103;goto _LL197;case 4: _LL196: _tmp104=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL197:
# 812
 Cyc_scan_exp(_tmp104,dep);
return;case 5: _LL198: _tmp107=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp106=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_tmp105=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1)->f3;_LL199:
# 815
 Cyc_scan_exp(_tmp107,dep);
Cyc_scan_exp(_tmp106,dep);
Cyc_scan_exp(_tmp105,dep);
return;case 6: _LL19A: _tmp109=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp108=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL19B:
 _tmp10B=_tmp109;_tmp10A=_tmp108;goto _LL19D;case 7: _LL19C: _tmp10B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp10A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL19D:
# 821
 Cyc_scan_exp(_tmp10B,dep);
Cyc_scan_exp(_tmp10A,dep);
return;case 9: _LL19E: _tmp10D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp10C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL19F:
# 825
 Cyc_scan_exp(_tmp10D,dep);
for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp10C->hd,dep);}
# 829
return;case 13: _LL1A0: _tmp10F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp10E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL1A1:
# 831
 Cyc_scan_type(_tmp10F,dep);
Cyc_scan_exp(_tmp10E,dep);
return;case 33: _LL1A2: _tmp113=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD1)->f1).is_calloc;_tmp112=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD1)->f1).rgn;_tmp111=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD1)->f1).elt_type;_tmp110=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD1)->f1).num_elts;_LL1A3:
# 835
 if(_tmp112 != 0)Cyc_scan_exp(_tmp112,dep);
if(_tmp111 != 0)Cyc_scan_type(*_tmp111,dep);
Cyc_scan_exp(_tmp110,dep);
return;case 37: _LL1A4: _tmp114=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL1A5:
# 840
 Cyc_scan_exp(_tmp114,dep);return;case 38: _LL1A6: _tmp115=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL1A7:
 _tmp116=_tmp115;goto _LL1A9;case 16: _LL1A8: _tmp116=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_LL1A9:
# 843
 Cyc_scan_type(_tmp116,dep);
return;case 20: _LL1AA: _tmp118=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp117=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL1AB:
# 846
 _tmp11A=_tmp118;_tmp119=_tmp117;goto _LL1AD;case 21: _LL1AC: _tmp11A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp119=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL1AD:
# 848
 Cyc_scan_exp(_tmp11A,dep);
Cyc_add_target(_tmp119);
return;case 18: _LL1AE: _tmp11C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1)->f1;_tmp11B=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL1AF:
# 852
 Cyc_scan_type(_tmp11C,dep);
# 854
{void*_tmpD3=(void*)((struct Cyc_List_List*)_check_null(_tmp11B))->hd;void*_tmpD4=_tmpD3;struct _dyneither_ptr*_tmpD5;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpD4)->tag == 0){_LL1D7: _tmpD5=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpD4)->f1;_LL1D8:
 Cyc_add_target(_tmpD5);goto _LL1D6;}else{_LL1D9: _LL1DA:
 goto _LL1D6;}_LL1D6:;}
# 858
return;case 0: _LL1B0: _LL1B1:
# 860
 return;case 35: _LL1B2: _tmp11D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD1)->f2;_LL1B3:
# 862
 for(0;_tmp11D != 0;_tmp11D=_tmp11D->tl){
struct _tuple24*_tmpD6=(struct _tuple24*)_tmp11D->hd;struct _tuple24*_tmpD7=_tmpD6;struct Cyc_Absyn_Exp*_tmpD8;_LL1DC: _tmpD8=_tmpD7->f2;_LL1DD:;
Cyc_scan_exp(_tmpD8,dep);}
# 866
return;case 39: _LL1B4: _LL1B5:
 return;case 34: _LL1B6: _LL1B7:
# 869
({void*_tmpD9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDA="Error: unexpected Swap_e\n";_tag_dyneither(_tmpDA,sizeof(char),26);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});
 exit(1);return;case 36: _LL1B8: _LL1B9:
# 872
({void*_tmpDB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDC="Error: unexpected Stmt_e\n";_tag_dyneither(_tmpDC,sizeof(char),26);}),_tag_dyneither(_tmpDB,sizeof(void*),0));});
 exit(1);return;case 10: _LL1BA: _LL1BB:
# 875
({void*_tmpDD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDE="Error: unexpected Throw_e\n";_tag_dyneither(_tmpDE,sizeof(char),27);}),_tag_dyneither(_tmpDD,sizeof(void*),0));});
 exit(1);return;case 11: _LL1BC: _LL1BD:
# 878
({void*_tmpDF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE0="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmpE0,sizeof(char),35);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});
 exit(1);return;case 12: _LL1BE: _LL1BF:
# 881
({void*_tmpE1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE2="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmpE2,sizeof(char),33);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});
 exit(1);return;case 15: _LL1C0: _LL1C1:
# 884
({void*_tmpE3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE4="Error: unexpected New_e\n";_tag_dyneither(_tmpE4,sizeof(char),25);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});
 exit(1);return;case 23: _LL1C2: _LL1C3:
# 887
({void*_tmpE5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE6="Error: unexpected Tuple_e\n";_tag_dyneither(_tmpE6,sizeof(char),27);}),_tag_dyneither(_tmpE5,sizeof(void*),0));});
 exit(1);return;case 24: _LL1C4: _LL1C5:
# 890
({void*_tmpE7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE8="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmpE8,sizeof(char),33);}),_tag_dyneither(_tmpE7,sizeof(void*),0));});
 exit(1);return;case 25: _LL1C6: _LL1C7:
# 893
({void*_tmpE9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpEA="Error: unexpected Array_e\n";_tag_dyneither(_tmpEA,sizeof(char),27);}),_tag_dyneither(_tmpE9,sizeof(void*),0));});
 exit(1);return;case 26: _LL1C8: _LL1C9:
# 896
({void*_tmpEB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpEC="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmpEC,sizeof(char),35);}),_tag_dyneither(_tmpEB,sizeof(void*),0));});
 exit(1);return;case 27: _LL1CA: _LL1CB:
# 899
({void*_tmpED=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpEE="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmpEE,sizeof(char),41);}),_tag_dyneither(_tmpED,sizeof(void*),0));});
 exit(1);return;case 28: _LL1CC: _LL1CD:
# 902
({void*_tmpEF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF0="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmpF0,sizeof(char),31);}),_tag_dyneither(_tmpEF,sizeof(void*),0));});
 exit(1);return;case 29: _LL1CE: _LL1CF:
# 905
({void*_tmpF1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF2="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmpF2,sizeof(char),32);}),_tag_dyneither(_tmpF1,sizeof(void*),0));});
 exit(1);return;case 30: _LL1D0: _LL1D1:
# 908
({void*_tmpF3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF4="Error: unexpected Datatype_e\n";_tag_dyneither(_tmpF4,sizeof(char),30);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});
 exit(1);return;case 31: _LL1D2: _LL1D3:
# 911
({void*_tmpF5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF6="Error: unexpected Enum_e\n";_tag_dyneither(_tmpF6,sizeof(char),26);}),_tag_dyneither(_tmpF5,sizeof(void*),0));});
 exit(1);return;default: _LL1D4: _LL1D5:
# 914
({void*_tmpF7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF8="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmpF8,sizeof(char),30);}),_tag_dyneither(_tmpF7,sizeof(void*),0));});
 exit(1);return;}_LL185:;}
# 919
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 924
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp11E=t;struct Cyc_Absyn_Datatypedecl*_tmp15C;struct Cyc_Absyn_Enumdecl*_tmp15B;struct Cyc_Absyn_Aggrdecl*_tmp15A;struct _dyneither_ptr*_tmp159;struct _dyneither_ptr*_tmp158;union Cyc_Absyn_AggrInfoU _tmp157;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_FnInfo _tmp155;struct Cyc_Absyn_Exp*_tmp154;void*_tmp153;struct Cyc_Absyn_Exp*_tmp152;union Cyc_Absyn_Constraint*_tmp151;struct Cyc_Absyn_PtrInfo _tmp150;switch(*((int*)_tmp11E)){case 0: _LL1DF: _LL1E0:
 goto _LL1E2;case 6: _LL1E1: _LL1E2:
 goto _LL1E4;case 28: _LL1E3: _LL1E4:
 goto _LL1E6;case 7: _LL1E5: _LL1E6:
# 931
 return;case 5: _LL1E7: _tmp150=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11E)->f1;_LL1E8:
# 934
 Cyc_scan_type(_tmp150.elt_typ,dep);
return;case 8: _LL1E9: _tmp153=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11E)->f1).elt_type;_tmp152=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11E)->f1).num_elts;_tmp151=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11E)->f1).zero_term;_LL1EA:
# 937
 Cyc_scan_type(_tmp153,dep);
Cyc_scan_exp_opt(_tmp152,dep);
return;case 27: _LL1EB: _tmp154=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp11E)->f1;_LL1EC:
# 941
 Cyc_scan_exp(_tmp154,dep);
return;case 9: _LL1ED: _tmp155=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11E)->f1;_LL1EE:
# 944
 Cyc_scan_type(_tmp155.ret_typ,dep);
{struct Cyc_List_List*_tmp11F=_tmp155.args;for(0;_tmp11F != 0;_tmp11F=_tmp11F->tl){
struct _tuple8*_tmp120=(struct _tuple8*)_tmp11F->hd;struct _tuple8*_tmp121=_tmp120;void*_tmp122;_LL21E: _tmp122=_tmp121->f3;_LL21F:;
Cyc_scan_type(_tmp122,dep);}}
# 949
if(_tmp155.cyc_varargs != 0)
Cyc_scan_type((_tmp155.cyc_varargs)->type,dep);
return;case 12: _LL1EF: _tmp156=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11E)->f2;_LL1F0:
# 953
 for(0;_tmp156 != 0;_tmp156=_tmp156->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp156->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp156->hd)->width,dep);}
# 957
return;case 14: _LL1F1: _LL1F2:
# 959
 return;case 11: _LL1F3: _tmp157=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11E)->f1).aggr_info;_LL1F4: {
# 961
struct _tuple10 _tmp123=Cyc_Absyn_aggr_kinded_name(_tmp157);struct _tuple10 _tmp124=_tmp123;struct _dyneither_ptr*_tmp125;_LL221: _tmp125=(_tmp124.f2)->f2;_LL222:;
_tmp158=_tmp125;goto _LL1F6;}case 13: _LL1F5: _tmp158=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp11E)->f1)->f2;_LL1F6:
# 964
 _tmp159=_tmp158;goto _LL1F8;case 17: _LL1F7: _tmp159=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11E)->f1)->f2;_LL1F8:
# 966
 Cyc_add_target(_tmp159);
return;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11E)->f1)->r)){case 0: _LL1F9: _tmp15A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11E)->f1)->r)->f1;_LL1FA:
# 970
 Cyc_scan_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp127;_tmp127.tag=5;_tmp127.f1=_tmp15A;_tmp127;});_tmp126;}),0),dep);{
struct _tuple0*_tmp128=_tmp15A->name;struct _tuple0*_tmp129=_tmp128;struct _dyneither_ptr*_tmp12A;_LL224: _tmp12A=_tmp129->f2;_LL225:;
Cyc_add_target(_tmp12A);
return;};case 1: _LL1FB: _tmp15B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11E)->f1)->r)->f1;_LL1FC:
# 976
 Cyc_scan_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp12C;_tmp12C.tag=7;_tmp12C.f1=_tmp15B;_tmp12C;});_tmp12B;}),0),dep);{
struct _tuple0*_tmp12D=_tmp15B->name;struct _tuple0*_tmp12E=_tmp12D;struct _dyneither_ptr*_tmp12F;_LL227: _tmp12F=_tmp12E->f2;_LL228:;
Cyc_add_target(_tmp12F);
return;};default: _LL1FD: _tmp15C=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11E)->f1)->r)->f1;_LL1FE:
# 982
({void*_tmp130=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp131="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp131,sizeof(char),40);}),_tag_dyneither(_tmp130,sizeof(void*),0));});
 exit(1);return;}case 1: _LL1FF: _LL200:
# 985
({void*_tmp132=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp133="Error: unexpected Evar\n";_tag_dyneither(_tmp133,sizeof(char),24);}),_tag_dyneither(_tmp132,sizeof(void*),0));});
 exit(1);return;case 2: _LL201: _LL202:
# 988
({void*_tmp134=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp135="Error: unexpected VarType\n";_tag_dyneither(_tmp135,sizeof(char),27);}),_tag_dyneither(_tmp134,sizeof(void*),0));});
 exit(1);return;case 3: _LL203: _LL204:
# 991
({void*_tmp136=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp137="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp137,sizeof(char),32);}),_tag_dyneither(_tmp136,sizeof(void*),0));});
 exit(1);return;case 4: _LL205: _LL206:
# 994
({void*_tmp138=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp139="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp139,sizeof(char),37);}),_tag_dyneither(_tmp138,sizeof(void*),0));});
 exit(1);return;case 10: _LL207: _LL208:
# 997
({void*_tmp13A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13B="Error: unexpected TupleType\n";_tag_dyneither(_tmp13B,sizeof(char),29);}),_tag_dyneither(_tmp13A,sizeof(void*),0));});
 exit(1);return;case 15: _LL209: _LL20A:
# 1000
({void*_tmp13C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13D="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp13D,sizeof(char),33);}),_tag_dyneither(_tmp13C,sizeof(void*),0));});
 exit(1);return;case 16: _LL20B: _LL20C:
# 1003
({void*_tmp13E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13F="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp13F,sizeof(char),30);}),_tag_dyneither(_tmp13E,sizeof(void*),0));});
 exit(1);return;case 20: _LL20D: _LL20E:
# 1006
({void*_tmp140=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp141="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp141,sizeof(char),27);}),_tag_dyneither(_tmp140,sizeof(void*),0));});
 exit(1);return;case 21: _LL20F: _LL210:
# 1009
({void*_tmp142=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp143="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp143,sizeof(char),29);}),_tag_dyneither(_tmp142,sizeof(void*),0));});
 exit(1);return;case 22: _LL211: _LL212:
# 1012
({void*_tmp144=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp145="Error: unexpected RefCntRgn\n";_tag_dyneither(_tmp145,sizeof(char),29);}),_tag_dyneither(_tmp144,sizeof(void*),0));});
 exit(1);return;case 23: _LL213: _LL214:
# 1015
({void*_tmp146=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp147="Error: unexpected AccessEff\n";_tag_dyneither(_tmp147,sizeof(char),29);}),_tag_dyneither(_tmp146,sizeof(void*),0));});
 exit(1);return;case 24: _LL215: _LL216:
# 1018
({void*_tmp148=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp149="Error: unexpected JoinEff\n";_tag_dyneither(_tmp149,sizeof(char),27);}),_tag_dyneither(_tmp148,sizeof(void*),0));});
 exit(1);return;case 25: _LL217: _LL218:
# 1021
({void*_tmp14A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14B="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp14B,sizeof(char),27);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});
 exit(1);return;case 19: _LL219: _LL21A:
# 1024
({void*_tmp14C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14D="Error: unexpected tag_t\n";_tag_dyneither(_tmp14D,sizeof(char),25);}),_tag_dyneither(_tmp14C,sizeof(void*),0));});
 exit(1);return;default: _LL21B: _LL21C:
# 1027
({void*_tmp14E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14F="Error: unexpected valueof_t\n";_tag_dyneither(_tmp14F,sizeof(char),29);}),_tag_dyneither(_tmp14E,sizeof(void*),0));});
 exit(1);return;}_LL1DE:;}
# 1032
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp15D=Cyc_current_targets;
struct _dyneither_ptr*_tmp15E=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp15F;});
{void*_tmp160=d->r;void*_tmp161=_tmp160;struct Cyc_Absyn_Typedefdecl*_tmp195;struct Cyc_Absyn_Enumdecl*_tmp194;struct Cyc_Absyn_Aggrdecl*_tmp193;struct Cyc_Absyn_Fndecl*_tmp192;struct Cyc_Absyn_Vardecl*_tmp191;switch(*((int*)_tmp161)){case 0: _LL22A: _tmp191=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp161)->f1;_LL22B: {
# 1038
struct _tuple0*_tmp162=_tmp191->name;struct _tuple0*_tmp163=_tmp162;struct _dyneither_ptr*_tmp164;_LL249: _tmp164=_tmp163->f2;_LL24A:;
Cyc_current_source=_tmp164;
Cyc_scan_type(_tmp191->type,dep);
Cyc_scan_exp_opt(_tmp191->initializer,dep);
goto _LL229;}case 1: _LL22C: _tmp192=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp161)->f1;_LL22D: {
# 1044
struct _tuple0*_tmp165=_tmp192->name;struct _tuple0*_tmp166=_tmp165;struct _dyneither_ptr*_tmp16D;_LL24C: _tmp16D=_tmp166->f2;_LL24D:;
Cyc_current_source=_tmp16D;
Cyc_scan_type(_tmp192->ret_type,dep);
{struct Cyc_List_List*_tmp167=_tmp192->args;for(0;_tmp167 != 0;_tmp167=_tmp167->tl){
struct _tuple8*_tmp168=(struct _tuple8*)_tmp167->hd;struct _tuple8*_tmp169=_tmp168;void*_tmp16A;_LL24F: _tmp16A=_tmp169->f3;_LL250:;
Cyc_scan_type(_tmp16A,dep);}}
# 1051
if(_tmp192->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp192->cyc_varargs))->type,dep);
if(_tmp192->is_inline)
({void*_tmp16B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp16C="Warning: ignoring inline function\n";_tag_dyneither(_tmp16C,sizeof(char),35);}),_tag_dyneither(_tmp16B,sizeof(void*),0));});
# 1056
goto _LL229;}case 5: _LL22E: _tmp193=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp161)->f1;_LL22F: {
# 1058
struct _tuple0*_tmp16E=_tmp193->name;struct _tuple0*_tmp16F=_tmp16E;struct _dyneither_ptr*_tmp173;_LL252: _tmp173=_tmp16F->f2;_LL253:;
Cyc_current_source=_tmp173;
if((unsigned int)_tmp193->impl){
{struct Cyc_List_List*_tmp170=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp193->impl))->fields;for(0;_tmp170 != 0;_tmp170=_tmp170->tl){
struct Cyc_Absyn_Aggrfield*_tmp171=(struct Cyc_Absyn_Aggrfield*)_tmp170->hd;
Cyc_scan_type(_tmp171->type,dep);
Cyc_scan_exp_opt(_tmp171->width,dep);}}{
# 1068
struct Cyc_List_List*_tmp172=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp193->impl))->fields;for(0;_tmp172 != 0;_tmp172=_tmp172->tl){;}};}
# 1072
goto _LL229;}case 7: _LL230: _tmp194=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp161)->f1;_LL231: {
# 1074
struct _tuple0*_tmp174=_tmp194->name;struct _tuple0*_tmp175=_tmp174;struct _dyneither_ptr*_tmp179;_LL255: _tmp179=_tmp175->f2;_LL256:;
Cyc_current_source=_tmp179;
if((unsigned int)_tmp194->fields){
{struct Cyc_List_List*_tmp176=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp194->fields))->v;for(0;_tmp176 != 0;_tmp176=_tmp176->tl){
struct Cyc_Absyn_Enumfield*_tmp177=(struct Cyc_Absyn_Enumfield*)_tmp176->hd;
Cyc_scan_exp_opt(_tmp177->tag,dep);}}{
# 1083
struct Cyc_List_List*_tmp178=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp194->fields))->v;for(0;_tmp178 != 0;_tmp178=_tmp178->tl){;}};}
# 1087
goto _LL229;}case 8: _LL232: _tmp195=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp161)->f1;_LL233: {
# 1089
struct _tuple0*_tmp17A=_tmp195->name;struct _tuple0*_tmp17B=_tmp17A;struct _dyneither_ptr*_tmp17C;_LL258: _tmp17C=_tmp17B->f2;_LL259:;
Cyc_current_source=_tmp17C;
if((unsigned int)_tmp195->defn)
Cyc_scan_type((void*)_check_null(_tmp195->defn),dep);
goto _LL229;}case 4: _LL234: _LL235:
# 1095
({void*_tmp17D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp17E="Error: unexpected region declaration";_tag_dyneither(_tmp17E,sizeof(char),37);}),_tag_dyneither(_tmp17D,sizeof(void*),0));});
 exit(1);case 13: _LL236: _LL237:
# 1101
({void*_tmp17F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp180="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp180,sizeof(char),38);}),_tag_dyneither(_tmp17F,sizeof(void*),0));});
 exit(1);case 14: _LL238: _LL239:
# 1104
({void*_tmp181=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp182="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp182,sizeof(char),39);}),_tag_dyneither(_tmp181,sizeof(void*),0));});
 exit(1);case 2: _LL23A: _LL23B:
# 1107
({void*_tmp183=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp184="Error: unexpected let declaration\n";_tag_dyneither(_tmp184,sizeof(char),35);}),_tag_dyneither(_tmp183,sizeof(void*),0));});
 exit(1);case 6: _LL23C: _LL23D:
# 1110
({void*_tmp185=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp186="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp186,sizeof(char),40);}),_tag_dyneither(_tmp185,sizeof(void*),0));});
 exit(1);case 3: _LL23E: _LL23F:
# 1113
({void*_tmp187=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp188="Error: unexpected let declaration\n";_tag_dyneither(_tmp188,sizeof(char),35);}),_tag_dyneither(_tmp187,sizeof(void*),0));});
 exit(1);case 9: _LL240: _LL241:
# 1116
({void*_tmp189=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18A="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp18A,sizeof(char),41);}),_tag_dyneither(_tmp189,sizeof(void*),0));});
 exit(1);case 10: _LL242: _LL243:
# 1119
({void*_tmp18B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18C="Error: unexpected using declaration\n";_tag_dyneither(_tmp18C,sizeof(char),37);}),_tag_dyneither(_tmp18B,sizeof(void*),0));});
 exit(1);case 11: _LL244: _LL245:
# 1122
({void*_tmp18D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18E="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp18E,sizeof(char),42);}),_tag_dyneither(_tmp18D,sizeof(void*),0));});
 exit(1);default: _LL246: _LL247:
# 1125
({void*_tmp18F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp190="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp190,sizeof(char),50);}),_tag_dyneither(_tmp18F,sizeof(void*),0));});
 exit(1);}_LL229:;}{
# 1133
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp196=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp197;_push_handler(& _tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp196);;_pop_handler();}else{void*_tmp198=(void*)_exn_thrown;void*_tmp19A=_tmp198;void*_tmp19B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp19A)->tag == Cyc_Core_Not_found){_LL25B: _LL25C:
# 1138
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL25A;}else{_LL25D: _tmp19B=_tmp19A;_LL25E:(int)_rethrow(_tmp19B);}_LL25A:;}};}{
# 1140
struct Cyc_Set_Set*_tmp19C=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp196,_tmp19C);
# 1143
Cyc_current_targets=_tmp15D;
Cyc_current_source=_tmp15E;};};}
# 1147
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1151
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp19D;_push_handler(& _tmp19D);{int _tmp19F=0;if(setjmp(_tmp19D.handler))_tmp19F=1;if(!_tmp19F){{struct Cyc_Set_Set*_tmp1A0=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp1A0;};_pop_handler();}else{void*_tmp19E=(void*)_exn_thrown;void*_tmp1A1=_tmp19E;void*_tmp1A2;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp1A1)->tag == Cyc_Core_Not_found){_LL260: _LL261:
# 1154
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL262: _tmp1A2=_tmp1A1;_LL263:(int)_rethrow(_tmp1A2);}_LL25F:;}};}
# 1158
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1169 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1A3=curr;
# 1175
struct _dyneither_ptr*_tmp1A4=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1A3)> 0){
struct Cyc_Set_Set*_tmp1A5=emptyset;
struct Cyc_Iter_Iter _tmp1A6=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1A3);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1A6,& _tmp1A4)){
_tmp1A5=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1A5,Cyc_find(t,_tmp1A4));}
_tmp1A3=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1A5,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp1A3);}
# 1184
return curr;};}
# 1187
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1193
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1197
if(Cyc_script_file == 0){
({void*_tmp1A8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1A9="Internal error: script file is NULL\n";_tag_dyneither(_tmp1A9,sizeof(char),37);}),_tag_dyneither(_tmp1A8,sizeof(void*),0));});
 exit(1);}
# 1201
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1204
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0;_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp1AA[2]={& _tmp1AC,& _tmp1AD};Cyc_prscript(({const char*_tmp1AB="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp1AB,sizeof(char),36);}),_tag_dyneither(_tmp1AA,sizeof(void*),2));});});});else{
# 1211
int _tmp1AE=({unsigned short _tmp1B2[0];Cyc_open((const char*)_untag_dyneither_ptr(d,sizeof(char),1),0,_tag_dyneither(_tmp1B2,sizeof(unsigned short),0));});
if(_tmp1AE == - 1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp1B1;_tmp1B1.tag=0;_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp1AF[1]={& _tmp1B1};Cyc_fprintf(Cyc_stderr,({const char*_tmp1B0="Error: could not create directory %s\n";_tag_dyneither(_tmp1B0,sizeof(char),38);}),_tag_dyneither(_tmp1AF,sizeof(void*),1));});});
return 1;}}else{
# 1218
 close(_tmp1AE);}}
# 1220
return 0;}
# 1223
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1227
struct _dyneither_ptr _tmp1B3=Cyc_strdup((struct _dyneither_ptr)file);
# 1229
struct Cyc_List_List*_tmp1B4=0;
while(1){
_tmp1B3=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1B3);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1B3)== 0)break;
_tmp1B4=({struct Cyc_List_List*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->hd=({struct _dyneither_ptr*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6[0]=(struct _dyneither_ptr)_tmp1B3;_tmp1B6;});_tmp1B5->tl=_tmp1B4;_tmp1B5;});}
# 1236
for(0;_tmp1B4 != 0;_tmp1B4=_tmp1B4->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp1B4->hd)))return 1;}
# 1239
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1246
static int Cyc_is_other_special(char c){
char _tmp1B7=c;switch(_tmp1B7){case 92: _LL265: _LL266:
 goto _LL268;case 34: _LL267: _LL268:
 goto _LL26A;case 59: _LL269: _LL26A:
 goto _LL26C;case 38: _LL26B: _LL26C:
 goto _LL26E;case 40: _LL26D: _LL26E:
 goto _LL270;case 41: _LL26F: _LL270:
 goto _LL272;case 124: _LL271: _LL272:
 goto _LL274;case 94: _LL273: _LL274:
 goto _LL276;case 60: _LL275: _LL276:
 goto _LL278;case 62: _LL277: _LL278:
 goto _LL27A;case 10: _LL279: _LL27A:
# 1261
 goto _LL27C;case 9: _LL27B: _LL27C:
 return 1;default: _LL27D: _LL27E:
 return 0;}_LL264:;}
# 1267
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1B8=Cyc_strlen((struct _dyneither_ptr)s);
# 1271
int _tmp1B9=0;
int _tmp1BA=0;
{int i=0;for(0;i < _tmp1B8;++ i){
char _tmp1BB=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp1BB == '\'')++ _tmp1B9;else{
if(Cyc_is_other_special(_tmp1BB))++ _tmp1BA;}}}
# 1280
if(_tmp1B9 == 0  && _tmp1BA == 0)
return s;
# 1284
if(_tmp1B9 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp1BC[3];_tmp1BC[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);_tmp1BC[1]=({struct _dyneither_ptr*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE[0]=(struct _dyneither_ptr)s;_tmp1BE;});_tmp1BC[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1BC,sizeof(struct _dyneither_ptr*),3));}));{
# 1288
unsigned long _tmp1C0=(_tmp1B8 + _tmp1B9)+ _tmp1BA;
struct _dyneither_ptr s2=({unsigned int _tmp1CA=_tmp1C0 + 1;char*_tmp1CB=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp1CA + 1));struct _dyneither_ptr _tmp1CD=_tag_dyneither(_tmp1CB,sizeof(char),_tmp1CA + 1);{unsigned int _tmp1CC=_tmp1CA;unsigned int i;for(i=0;i < _tmp1CC;i ++){_tmp1CB[i]=(char)'\000';}_tmp1CB[_tmp1CC]=(char)0;}_tmp1CD;});
int _tmp1C1=0;
int _tmp1C2=0;
for(0;_tmp1C1 < _tmp1B8;++ _tmp1C1){
char _tmp1C3=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp1C1));
if(_tmp1C3 == '\''  || Cyc_is_other_special(_tmp1C3))
({struct _dyneither_ptr _tmp1C4=_dyneither_ptr_plus(s2,sizeof(char),_tmp1C2 ++);char _tmp1C5=*((char*)_check_dyneither_subscript(_tmp1C4,sizeof(char),0));char _tmp1C6='\\';if(_get_dyneither_size(_tmp1C4,sizeof(char))== 1  && (_tmp1C5 == '\000'  && _tmp1C6 != '\000'))_throw_arraybounds();*((char*)_tmp1C4.curr)=_tmp1C6;});
({struct _dyneither_ptr _tmp1C7=_dyneither_ptr_plus(s2,sizeof(char),_tmp1C2 ++);char _tmp1C8=*((char*)_check_dyneither_subscript(_tmp1C7,sizeof(char),0));char _tmp1C9=_tmp1C3;if(_get_dyneither_size(_tmp1C7,sizeof(char))== 1  && (_tmp1C8 == '\000'  && _tmp1C9 != '\000'))_throw_arraybounds();*((char*)_tmp1C7.curr)=_tmp1C9;});}
# 1298
return(struct _dyneither_ptr)s2;};}
# 1300
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=Cyc_sh_escape_string(*sp);_tmp1CE;});}struct _tuple25{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1305
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1311
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1316
({struct Cyc_String_pa_PrintArg_struct _tmp1D1;_tmp1D1.tag=0;_tmp1D1.f1=(struct _dyneither_ptr)({const char*_tmp1D2=filename;_tag_dyneither(_tmp1D2,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D2,1));});({void*_tmp1CF[1]={& _tmp1D1};Cyc_fprintf(Cyc_stderr,({const char*_tmp1D0="********************************* %s...\n";_tag_dyneither(_tmp1D0,sizeof(char),41);}),_tag_dyneither(_tmp1CF,sizeof(void*),1));});});
# 1319
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct _dyneither_ptr)({const char*_tmp1D6=filename;_tag_dyneither(_tmp1D6,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D6,1));});({void*_tmp1D3[1]={& _tmp1D5};Cyc_log(({const char*_tmp1D4="\n%s:\n";_tag_dyneither(_tmp1D4,sizeof(char),6);}),_tag_dyneither(_tmp1D3,sizeof(void*),1));});});{
# 1331 "buildlib.cyl"
struct _dyneither_ptr _tmp1D7=Cyc_Filename_basename(({const char*_tmp320=filename;_tag_dyneither(_tmp320,sizeof(char),_get_zero_arr_size_char((void*)_tmp320,1));}));
struct _dyneither_ptr _tmp1D8=Cyc_Filename_dirname(({const char*_tmp31F=filename;_tag_dyneither(_tmp31F,sizeof(char),_get_zero_arr_size_char((void*)_tmp31F,1));}));
struct _dyneither_ptr _tmp1D9=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp1D7);
const char*_tmp1DA=(const char*)_check_null(_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp1D9,({const char*_tmp31E=".iA";_tag_dyneither(_tmp31E,sizeof(char),4);})),sizeof(char),1));
const char*_tmp1DB=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp1D8,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp31A;_tmp31A.tag=0;_tmp31A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D9);({void*_tmp318[1]={& _tmp31A};Cyc_aprintf(({const char*_tmp319="%s.iB";_tag_dyneither(_tmp319,sizeof(char),6);}),_tag_dyneither(_tmp318,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D8,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31D;_tmp31D.tag=0;_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D9);({void*_tmp31B[1]={& _tmp31D};Cyc_aprintf(({const char*_tmp31C="%s.iB";_tag_dyneither(_tmp31C,sizeof(char),6);}),_tag_dyneither(_tmp31B,sizeof(void*),1));});})),sizeof(char),1));
# 1339
const char*_tmp1DC=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp1D8,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp314;_tmp314.tag=0;_tmp314.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D9);({void*_tmp312[1]={& _tmp314};Cyc_aprintf(({const char*_tmp313="%s.iC";_tag_dyneither(_tmp313,sizeof(char),6);}),_tag_dyneither(_tmp312,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D8,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp317;_tmp317.tag=0;_tmp317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D9);({void*_tmp315[1]={& _tmp317};Cyc_aprintf(({const char*_tmp316="%s.iC";_tag_dyneither(_tmp316,sizeof(char),6);}),_tag_dyneither(_tmp315,sizeof(void*),1));});})),sizeof(char),1));
# 1343
const char*_tmp1DD=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp1D8,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp30E;_tmp30E.tag=0;_tmp30E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D9);({void*_tmp30C[1]={& _tmp30E};Cyc_aprintf(({const char*_tmp30D="%s.iD";_tag_dyneither(_tmp30D,sizeof(char),6);}),_tag_dyneither(_tmp30C,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D8,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp311;_tmp311.tag=0;_tmp311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D9);({void*_tmp30F[1]={& _tmp311};Cyc_aprintf(({const char*_tmp310="%s.iD";_tag_dyneither(_tmp310,sizeof(char),6);}),_tag_dyneither(_tmp30F,sizeof(void*),1));});})),sizeof(char),1));
# 1348
{struct _handler_cons _tmp1DE;_push_handler(& _tmp1DE);{int _tmp1E0=0;if(setjmp(_tmp1DE.handler))_tmp1E0=1;if(!_tmp1E0){
# 1351
if(Cyc_force_directory_prefixes(({const char*_tmp1E1=filename;_tag_dyneither(_tmp1E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E1,1));}))){
int _tmp1E2=1;_npop_handler(0);return _tmp1E2;}
# 1356
if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _dyneither_ptr)({const char*_tmp1E6=_tmp1DA;_tag_dyneither(_tmp1E6,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E6,1));});({void*_tmp1E3[1]={& _tmp1E5};Cyc_prscript(({const char*_tmp1E4="cat >%s <<XXX\n";_tag_dyneither(_tmp1E4,sizeof(char),15);}),_tag_dyneither(_tmp1E3,sizeof(void*),1));});});
{struct Cyc_List_List*_tmp1E7=Cyc_current_cpp;for(0;_tmp1E7 != 0;_tmp1E7=_tmp1E7->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp1E7->hd));({void*_tmp1E8[1]={& _tmp1EA};Cyc_prscript(({const char*_tmp1E9="%s";_tag_dyneither(_tmp1E9,sizeof(char),3);}),_tag_dyneither(_tmp1E8,sizeof(void*),1));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _dyneither_ptr)({const char*_tmp1EE=filename;_tag_dyneither(_tmp1EE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1EE,1));});({void*_tmp1EB[1]={& _tmp1ED};Cyc_prscript(({const char*_tmp1EC="#include <%s>\n";_tag_dyneither(_tmp1EC,sizeof(char),15);}),_tag_dyneither(_tmp1EB,sizeof(void*),1));});});
({void*_tmp1EF=0;Cyc_prscript(({const char*_tmp1F0="XXX\n";_tag_dyneither(_tmp1F0,sizeof(char),5);}),_tag_dyneither(_tmp1EF,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct _dyneither_ptr)({const char*_tmp1F7=_tmp1DA;_tag_dyneither(_tmp1F7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F7,1));});({struct Cyc_String_pa_PrintArg_struct _tmp1F4;_tmp1F4.tag=0;_tmp1F4.f1=(struct _dyneither_ptr)({const char*_tmp1F6=_tmp1DB;_tag_dyneither(_tmp1F6,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F6,1));});({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0;_tmp1F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp1F1[3]={& _tmp1F3,& _tmp1F4,& _tmp1F5};Cyc_prscript(({const char*_tmp1F2="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp1F2,sizeof(char),35);}),_tag_dyneither(_tmp1F1,sizeof(void*),3));});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=(struct _dyneither_ptr)({const char*_tmp1FE=_tmp1DA;_tag_dyneither(_tmp1FE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FE,1));});({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(struct _dyneither_ptr)({const char*_tmp1FD=_tmp1DC;_tag_dyneither(_tmp1FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FD,1));});({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp1F8[3]={& _tmp1FA,& _tmp1FB,& _tmp1FC};Cyc_prscript(({const char*_tmp1F9="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp1F9,sizeof(char),31);}),_tag_dyneither(_tmp1F8,sizeof(void*),3));});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(struct _dyneither_ptr)({const char*_tmp202=_tmp1DA;_tag_dyneither(_tmp202,sizeof(char),_get_zero_arr_size_char((void*)_tmp202,1));});({void*_tmp1FF[1]={& _tmp201};Cyc_prscript(({const char*_tmp200="rm %s\n";_tag_dyneither(_tmp200,sizeof(char),7);}),_tag_dyneither(_tmp1FF,sizeof(void*),1));});});}else{
# 1369
maybe=Cyc_fopen(_tmp1DA,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0;_tmp205.f1=(struct _dyneither_ptr)({const char*_tmp206=_tmp1DA;_tag_dyneither(_tmp206,sizeof(char),_get_zero_arr_size_char((void*)_tmp206,1));});({void*_tmp203[1]={& _tmp205};Cyc_fprintf(Cyc_stderr,({const char*_tmp204="Error: could not create file %s\n";_tag_dyneither(_tmp204,sizeof(char),33);}),_tag_dyneither(_tmp203,sizeof(void*),1));});});{
int _tmp207=1;_npop_handler(0);return _tmp207;};}
# 1374
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0;_tmp20A.f1=(struct _dyneither_ptr)({const char*_tmp20B=_tmp1DA;_tag_dyneither(_tmp20B,sizeof(char),_get_zero_arr_size_char((void*)_tmp20B,1));});({void*_tmp208[1]={& _tmp20A};Cyc_fprintf(Cyc_stderr,({const char*_tmp209="Creating %s\n";_tag_dyneither(_tmp209,sizeof(char),13);}),_tag_dyneither(_tmp208,sizeof(void*),1));});});
out_file=maybe;
{struct Cyc_List_List*_tmp20C=Cyc_current_cpp;for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp20C->hd),sizeof(char),1)),out_file);}}
# 1380
({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0;_tmp20F.f1=(struct _dyneither_ptr)({const char*_tmp210=filename;_tag_dyneither(_tmp210,sizeof(char),_get_zero_arr_size_char((void*)_tmp210,1));});({void*_tmp20D[1]={& _tmp20F};Cyc_fprintf(out_file,({const char*_tmp20E="#include <%s>\n";_tag_dyneither(_tmp20E,sizeof(char),15);}),_tag_dyneither(_tmp20D,sizeof(void*),1));});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp211=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->hd=({struct _dyneither_ptr*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F[0]=(struct _dyneither_ptr)({const char*_tmp230="";_tag_dyneither(_tmp230,sizeof(char),1);});_tmp22F;});_tmp22E->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp22E;}),({const char*_tmp231=" ";_tag_dyneither(_tmp231,sizeof(char),2);}));
# 1386
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp22B;_tmp22B.tag=0;_tmp22B.f1=(struct _dyneither_ptr)({const char*_tmp22D=_tmp1DA;_tag_dyneither(_tmp22D,sizeof(char),_get_zero_arr_size_char((void*)_tmp22D,1));});({struct Cyc_String_pa_PrintArg_struct _tmp22A;_tmp22A.tag=0;_tmp22A.f1=(struct _dyneither_ptr)({const char*_tmp22C=_tmp1DB;_tag_dyneither(_tmp22C,sizeof(char),_get_zero_arr_size_char((void*)_tmp22C,1));});({struct Cyc_String_pa_PrintArg_struct _tmp229;_tmp229.tag=0;_tmp229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp211);({struct Cyc_String_pa_PrintArg_struct _tmp228;_tmp228.tag=0;_tmp228.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0;_tmp226.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp224[6]={& _tmp226,& _tmp227,& _tmp228,& _tmp229,& _tmp22A,& _tmp22B};Cyc_aprintf(({const char*_tmp225="%s %s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp225,sizeof(char),33);}),_tag_dyneither(_tmp224,sizeof(void*),6));});});});});});});}),sizeof(char),1));
# 1389
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(struct _dyneither_ptr)({char*_tmp215=cmd;_tag_dyneither(_tmp215,sizeof(char),_get_zero_arr_size_char((void*)_tmp215,1));});({void*_tmp212[1]={& _tmp214};Cyc_fprintf(Cyc_stderr,({const char*_tmp213="%s\n";_tag_dyneither(_tmp213,sizeof(char),4);}),_tag_dyneither(_tmp212,sizeof(void*),1));});});
if(! system((const char*)cmd)){
# 1394
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp21D;_tmp21D.tag=0;_tmp21D.f1=(struct _dyneither_ptr)({const char*_tmp21F=_tmp1DA;_tag_dyneither(_tmp21F,sizeof(char),_get_zero_arr_size_char((void*)_tmp21F,1));});({struct Cyc_String_pa_PrintArg_struct _tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _dyneither_ptr)({const char*_tmp21E=_tmp1DC;_tag_dyneither(_tmp21E,sizeof(char),_get_zero_arr_size_char((void*)_tmp21E,1));});({struct Cyc_String_pa_PrintArg_struct _tmp21B;_tmp21B.tag=0;_tmp21B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp211);({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0;_tmp21A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0;_tmp219.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp218;_tmp218.tag=0;_tmp218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp216[6]={& _tmp218,& _tmp219,& _tmp21A,& _tmp21B,& _tmp21C,& _tmp21D};Cyc_aprintf(({const char*_tmp217="%s %s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp217,sizeof(char),29);}),_tag_dyneither(_tmp216,sizeof(void*),6));});});});});});});}),sizeof(char),1));
# 1397
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0;_tmp222.f1=(struct _dyneither_ptr)({char*_tmp223=cmd;_tag_dyneither(_tmp223,sizeof(char),_get_zero_arr_size_char((void*)_tmp223,1));});({void*_tmp220[1]={& _tmp222};Cyc_fprintf(Cyc_stderr,({const char*_tmp221="%s\n";_tag_dyneither(_tmp221,sizeof(char),4);}),_tag_dyneither(_tmp220,sizeof(void*),1));});});
 system((const char*)cmd);}};}}
# 1404
if(Cyc_gathering()){int _tmp232=0;_npop_handler(0);return _tmp232;}{
# 1407
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1DB,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp234;_tmp234.tag=Cyc_NO_SUPPORT;_tmp234.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp237;_tmp237.tag=0;_tmp237.f1=(struct _dyneither_ptr)({const char*_tmp238=_tmp1DB;_tag_dyneither(_tmp238,sizeof(char),_get_zero_arr_size_char((void*)_tmp238,1));});({void*_tmp235[1]={& _tmp237};Cyc_aprintf(({const char*_tmp236="can't open macrosfile %s";_tag_dyneither(_tmp236,sizeof(char),25);}),_tag_dyneither(_tmp235,sizeof(void*),1));});});_tmp234;});_tmp233;}));
# 1412
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp239=Cyc_Lexing_from_file(in_file);
struct _tuple20*entry;
while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp239))!= 0){
struct _tuple20*_tmp23A=(struct _tuple20*)_check_null(entry);struct _tuple20*_tmp23B=_tmp23A;struct _dyneither_ptr*_tmp23D;struct Cyc_Set_Set*_tmp23C;_LL280: _tmp23D=_tmp23B->f1;_tmp23C=_tmp23B->f2;_LL281:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp23D,_tmp23C);}
# 1421
Cyc_fclose(in_file);
# 1424
maybe=Cyc_fopen(_tmp1DC,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp23F;_tmp23F.tag=Cyc_NO_SUPPORT;_tmp23F.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp242;_tmp242.tag=0;_tmp242.f1=(struct _dyneither_ptr)({const char*_tmp243=_tmp1DC;_tag_dyneither(_tmp243,sizeof(char),_get_zero_arr_size_char((void*)_tmp243,1));});({void*_tmp240[1]={& _tmp242};Cyc_aprintf(({const char*_tmp241="can't open declsfile %s";_tag_dyneither(_tmp241,sizeof(char),24);}),_tag_dyneither(_tmp240,sizeof(void*),1));});});_tmp23F;});_tmp23E;}));
# 1428
in_file=maybe;
_tmp239=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp1DD,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp244=1;_npop_handler(0);return _tmp244;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp239)){;}
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1438
maybe=Cyc_fopen(_tmp1DD,"r");
if(!((unsigned int)maybe)){int _tmp245=1;_npop_handler(0);return _tmp245;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp246=_tmp1DD;_tag_dyneither(_tmp246,sizeof(char),_get_zero_arr_size_char((void*)_tmp246,1));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp247=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1448
{struct Cyc_List_List*_tmp248=_tmp247;for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp248->hd,t);}}{
# 1452
struct Cyc_Set_Set*_tmp249=Cyc_reachable(start_symbols,t);
# 1455
struct Cyc_List_List*_tmp24A=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp24B=_tmp247;for(0;_tmp24B != 0;_tmp24B=_tmp24B->tl){
struct Cyc_Absyn_Decl*_tmp24C=(struct Cyc_Absyn_Decl*)_tmp24B->hd;
struct _dyneither_ptr*name;
{void*_tmp24D=_tmp24C->r;void*_tmp24E=_tmp24D;struct Cyc_Absyn_Typedefdecl*_tmp269;struct Cyc_Absyn_Enumdecl*_tmp268;struct Cyc_Absyn_Aggrdecl*_tmp267;struct Cyc_Absyn_Fndecl*_tmp266;struct Cyc_Absyn_Vardecl*_tmp265;switch(*((int*)_tmp24E)){case 0: _LL283: _tmp265=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp24E)->f1;_LL284: {
# 1464
struct _tuple0*_tmp24F=_tmp265->name;struct _tuple0*_tmp250=_tmp24F;struct _dyneither_ptr*_tmp251;_LL2A2: _tmp251=_tmp250->f2;_LL2A3:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp251);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp251))name=0;else{
name=_tmp251;}
goto _LL282;}case 1: _LL285: _tmp266=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp24E)->f1;_LL286: {
# 1470
struct _tuple0*_tmp252=_tmp266->name;struct _tuple0*_tmp253=_tmp252;struct _dyneither_ptr*_tmp254;_LL2A5: _tmp254=_tmp253->f2;_LL2A6:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp254);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp254))name=0;else{
name=_tmp254;}
goto _LL282;}case 5: _LL287: _tmp267=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp24E)->f1;_LL288: {
# 1476
struct _tuple0*_tmp255=_tmp267->name;struct _tuple0*_tmp256=_tmp255;struct _dyneither_ptr*_tmp257;_LL2A8: _tmp257=_tmp256->f2;_LL2A9:;
name=_tmp257;
goto _LL282;}case 7: _LL289: _tmp268=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp24E)->f1;_LL28A: {
# 1480
struct _tuple0*_tmp258=_tmp268->name;struct _tuple0*_tmp259=_tmp258;struct _dyneither_ptr*_tmp261;_LL2AB: _tmp261=_tmp259->f2;_LL2AC:;
name=_tmp261;
# 1484
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp249,name))
_tmp24A=({struct Cyc_List_List*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->hd=_tmp24C;_tmp25A->tl=_tmp24A;_tmp25A;});else{
# 1487
if((unsigned int)_tmp268->fields){
struct Cyc_List_List*_tmp25B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp268->fields))->v;for(0;_tmp25B != 0;_tmp25B=_tmp25B->tl){
struct Cyc_Absyn_Enumfield*_tmp25C=(struct Cyc_Absyn_Enumfield*)_tmp25B->hd;
struct _tuple0*_tmp25D=_tmp25C->name;struct _tuple0*_tmp25E=_tmp25D;struct _dyneither_ptr*_tmp260;_LL2AE: _tmp260=_tmp25E->f2;_LL2AF:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp249,_tmp260)){
_tmp24A=({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=_tmp24C;_tmp25F->tl=_tmp24A;_tmp25F;});
break;}}}}
# 1497
name=0;
goto _LL282;}case 8: _LL28B: _tmp269=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp24E)->f1;_LL28C: {
# 1500
struct _tuple0*_tmp262=_tmp269->name;struct _tuple0*_tmp263=_tmp262;struct _dyneither_ptr*_tmp264;_LL2B1: _tmp264=_tmp263->f2;_LL2B2:;
name=_tmp264;
goto _LL282;}case 13: _LL28D: _LL28E:
 goto _LL290;case 14: _LL28F: _LL290:
 goto _LL292;case 2: _LL291: _LL292:
 goto _LL294;case 6: _LL293: _LL294:
 goto _LL296;case 3: _LL295: _LL296:
 goto _LL298;case 9: _LL297: _LL298:
 goto _LL29A;case 10: _LL299: _LL29A:
 goto _LL29C;case 11: _LL29B: _LL29C:
 goto _LL29E;case 12: _LL29D: _LL29E:
 goto _LL2A0;default: _LL29F: _LL2A0:
# 1514
 name=0;
goto _LL282;}_LL282:;}
# 1517
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp249,name))
_tmp24A=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=_tmp24C;_tmp26A->tl=_tmp24A;_tmp26A;});}}
# 1522
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp26B=1;_npop_handler(0);return _tmp26B;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2E5;_tmp2E5.tag=0;_tmp2E5.f1=(struct _dyneither_ptr)({const char*_tmp2E6=filename;_tag_dyneither(_tmp2E6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E6,1));});({void*_tmp2E3[1]={& _tmp2E5};Cyc_aprintf(({const char*_tmp2E4="_%s_";_tag_dyneither(_tmp2E4,sizeof(char),5);}),_tag_dyneither(_tmp2E3,sizeof(void*),1));});});
{int _tmp26C=0;for(0;_tmp26C < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp26C){
if(((char*)ifdefmacro.curr)[_tmp26C]== '.'  || ((char*)ifdefmacro.curr)[_tmp26C]== '/')
({struct _dyneither_ptr _tmp26D=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp26C);char _tmp26E=*((char*)_check_dyneither_subscript(_tmp26D,sizeof(char),0));char _tmp26F='_';if(_get_dyneither_size(_tmp26D,sizeof(char))== 1  && (_tmp26E == '\000'  && _tmp26F != '\000'))_throw_arraybounds();*((char*)_tmp26D.curr)=_tmp26F;});else{
if(((char*)ifdefmacro.curr)[_tmp26C]!= '_'  && ((char*)ifdefmacro.curr)[_tmp26C]!= '/')
({struct _dyneither_ptr _tmp270=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp26C);char _tmp271=*((char*)_check_dyneither_subscript(_tmp270,sizeof(char),0));char _tmp272=(char) toupper((int)((char*)ifdefmacro.curr)[_tmp26C]);if(_get_dyneither_size(_tmp270,sizeof(char))== 1  && (_tmp271 == '\000'  && _tmp272 != '\000'))_throw_arraybounds();*((char*)_tmp270.curr)=_tmp272;});}}}
# 1534
({struct Cyc_String_pa_PrintArg_struct _tmp276;_tmp276.tag=0;_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({struct Cyc_String_pa_PrintArg_struct _tmp275;_tmp275.tag=0;_tmp275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp273[2]={& _tmp275,& _tmp276};Cyc_fprintf(out_file,({const char*_tmp274="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp274,sizeof(char),23);}),_tag_dyneither(_tmp273,sizeof(void*),2));});});});{
# 1541
struct Cyc_List_List*_tmp277=0;
struct Cyc_List_List*_tmp278=0;
{struct Cyc_List_List*_tmp279=_tmp24A;for(0;_tmp279 != 0;_tmp279=_tmp279->tl){
struct Cyc_Absyn_Decl*_tmp27A=(struct Cyc_Absyn_Decl*)_tmp279->hd;
int _tmp27B=0;
struct _dyneither_ptr*name;
{void*_tmp27C=_tmp27A->r;void*_tmp27D=_tmp27C;struct Cyc_Absyn_Typedefdecl*_tmp291;struct Cyc_Absyn_Enumdecl*_tmp290;struct Cyc_Absyn_Aggrdecl*_tmp28F;struct Cyc_Absyn_Fndecl*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28D;switch(*((int*)_tmp27D)){case 0: _LL2B4: _tmp28D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp27D)->f1;_LL2B5: {
# 1549
struct _tuple0*_tmp27E=_tmp28D->name;struct _tuple0*_tmp27F=_tmp27E;struct _dyneither_ptr*_tmp280;_LL2D3: _tmp280=_tmp27F->f2;_LL2D4:;
name=_tmp280;
goto _LL2B3;}case 1: _LL2B6: _tmp28E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp27D)->f1;_LL2B7:
# 1553
 if(_tmp28E->is_inline){name=0;goto _LL2B3;}{
struct _tuple0*_tmp281=_tmp28E->name;struct _tuple0*_tmp282=_tmp281;struct _dyneither_ptr*_tmp283;_LL2D6: _tmp283=_tmp282->f2;_LL2D7:;
name=_tmp283;
goto _LL2B3;};case 5: _LL2B8: _tmp28F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27D)->f1;_LL2B9: {
# 1558
struct _tuple0*_tmp284=_tmp28F->name;struct _tuple0*_tmp285=_tmp284;struct _dyneither_ptr*_tmp286;_LL2D9: _tmp286=_tmp285->f2;_LL2DA:;
name=_tmp286;
goto _LL2B3;}case 7: _LL2BA: _tmp290=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27D)->f1;_LL2BB: {
# 1562
struct _tuple0*_tmp287=_tmp290->name;struct _tuple0*_tmp288=_tmp287;struct _dyneither_ptr*_tmp289;_LL2DC: _tmp289=_tmp288->f2;_LL2DD:;
name=_tmp289;
goto _LL2B3;}case 8: _LL2BC: _tmp291=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27D)->f1;_LL2BD: {
# 1566
struct _tuple0*_tmp28A=_tmp291->name;struct _tuple0*_tmp28B=_tmp28A;struct _dyneither_ptr*_tmp28C;_LL2DF: _tmp28C=_tmp28B->f2;_LL2E0:;
name=_tmp28C;
goto _LL2B3;}case 4: _LL2BE: _LL2BF:
 goto _LL2C1;case 13: _LL2C0: _LL2C1:
# 1571
 goto _LL2C3;case 14: _LL2C2: _LL2C3:
 goto _LL2C5;case 2: _LL2C4: _LL2C5:
 goto _LL2C7;case 6: _LL2C6: _LL2C7:
 goto _LL2C9;case 3: _LL2C8: _LL2C9:
 goto _LL2CB;case 9: _LL2CA: _LL2CB:
 goto _LL2CD;case 10: _LL2CC: _LL2CD:
 goto _LL2CF;case 11: _LL2CE: _LL2CF:
 goto _LL2D1;default: _LL2D0: _LL2D1:
# 1580
 name=0;
goto _LL2B3;}_LL2B3:;}
# 1583
if(!((unsigned int)name) && !_tmp27B)continue;
_tmp277=({struct Cyc_List_List*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->hd=_tmp27A;_tmp292->tl=_tmp277;_tmp292;});
_tmp278=({struct Cyc_List_List*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->hd=name;_tmp293->tl=_tmp278;_tmp293;});}}
# 1589
{struct _handler_cons _tmp294;_push_handler(& _tmp294);{int _tmp296=0;if(setjmp(_tmp294.handler))_tmp296=1;if(!_tmp296){
{struct _RegionHandle _tmp297=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp297;_push_region(tc_rgn);
Cyc_Binding_resolve_all(_tmp277);{
struct Cyc_Tcenv_Tenv*_tmp298=Cyc_Tcenv_tc_init(tc_rgn);
Cyc_Tc_tc(tc_rgn,_tmp298,1,_tmp277);};
# 1591
;_pop_region(tc_rgn);}
# 1590
;_pop_handler();}else{void*_tmp295=(void*)_exn_thrown;void*_tmp299=_tmp295;_LL2E2: _LL2E3:
# 1597
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp29B;_tmp29B.tag=Cyc_NO_SUPPORT;_tmp29B.f1=({const char*_tmp29C="can't typecheck acquired declarations";_tag_dyneither(_tmp29C,sizeof(char),38);});_tmp29B;});_tmp29A;}));
goto _LL2E1;_LL2E1:;}};}
# 1602
{struct _tuple25 _tmp29D=({struct _tuple25 _tmp2B1;_tmp2B1.f1=_tmp277;_tmp2B1.f2=_tmp278;_tmp2B1;});struct _tuple25 _tmp29E=_tmp29D;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*_tmp2AF;_LL2E7: _tmp2B0=_tmp29E.f1;_tmp2AF=_tmp29E.f2;_LL2E8:;for(0;
_tmp2B0 != 0  && _tmp2AF != 0;(_tmp2B0=_tmp2B0->tl,_tmp2AF=_tmp2AF->tl)){
struct Cyc_Absyn_Decl*_tmp29F=(struct Cyc_Absyn_Decl*)_tmp2B0->hd;
struct _dyneither_ptr*_tmp2A0=(struct _dyneither_ptr*)_tmp2AF->hd;
int _tmp2A1=0;
if(!((unsigned int)_tmp2A0))
_tmp2A1=1;
# 1611
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp2A0){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2A0);({void*_tmp2A2[1]={& _tmp2A4};Cyc_aprintf(({const char*_tmp2A3="_%s_def_";_tag_dyneither(_tmp2A3,sizeof(char),9);}),_tag_dyneither(_tmp2A2,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0;_tmp2A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp2A5[1]={& _tmp2A7};Cyc_fprintf(out_file,({const char*_tmp2A6="#ifndef %s\n";_tag_dyneither(_tmp2A6,sizeof(char),12);}),_tag_dyneither(_tmp2A5,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp2A8[1]={& _tmp2AA};Cyc_fprintf(out_file,({const char*_tmp2A9="#define %s\n";_tag_dyneither(_tmp2A9,sizeof(char),12);}),_tag_dyneither(_tmp2A8,sizeof(void*),1));});});
# 1617
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp2AB[1];_tmp2AB[0]=_tmp29F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AB,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);
({void*_tmp2AC=0;Cyc_fprintf(out_file,({const char*_tmp2AD="#endif\n";_tag_dyneither(_tmp2AD,sizeof(char),8);}),_tag_dyneither(_tmp2AC,sizeof(void*),0));});}else{
# 1622
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp2AE[1];_tmp2AE[0]=_tmp29F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AE,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}
# 1627
maybe=Cyc_fopen(_tmp1DB,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp2B3;_tmp2B3.tag=Cyc_NO_SUPPORT;_tmp2B3.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct _dyneither_ptr)({const char*_tmp2B7=_tmp1DB;_tag_dyneither(_tmp2B7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B7,1));});({void*_tmp2B4[1]={& _tmp2B6};Cyc_aprintf(({const char*_tmp2B5="can't open macrosfile %s";_tag_dyneither(_tmp2B5,sizeof(char),25);}),_tag_dyneither(_tmp2B4,sizeof(void*),1));});});_tmp2B3;});_tmp2B2;}));
# 1630
in_file=maybe;
_tmp239=Cyc_Lexing_from_file(in_file);{
struct _tuple21*entry2;
while((entry2=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp239))!= 0){
struct _tuple21*_tmp2B8=(struct _tuple21*)_check_null(entry2);struct _tuple21*_tmp2B9=_tmp2B8;struct _dyneither_ptr _tmp2C3;struct _dyneither_ptr*_tmp2C2;_LL2EA: _tmp2C3=_tmp2B9->f1;_tmp2C2=_tmp2B9->f2;_LL2EB:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp249,_tmp2C2)){
({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2C2);({void*_tmp2BA[1]={& _tmp2BC};Cyc_fprintf(out_file,({const char*_tmp2BB="#ifndef %s\n";_tag_dyneither(_tmp2BB,sizeof(char),12);}),_tag_dyneither(_tmp2BA,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2C3);({void*_tmp2BD[1]={& _tmp2BF};Cyc_fprintf(out_file,({const char*_tmp2BE="%s\n";_tag_dyneither(_tmp2BE,sizeof(char),4);}),_tag_dyneither(_tmp2BD,sizeof(void*),1));});});
({void*_tmp2C0=0;Cyc_fprintf(out_file,({const char*_tmp2C1="#endif\n";_tag_dyneither(_tmp2C1,sizeof(char),8);}),_tag_dyneither(_tmp2C0,sizeof(void*),0));});}}
# 1641
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1644
if(hstubs != 0){
struct Cyc_List_List*_tmp2C4=hstubs;for(0;_tmp2C4 != 0;_tmp2C4=_tmp2C4->tl){
struct _tuple22*_tmp2C5=(struct _tuple22*)_tmp2C4->hd;struct _tuple22*_tmp2C6=_tmp2C5;struct _dyneither_ptr _tmp2CC;struct _dyneither_ptr _tmp2CB;_LL2ED: _tmp2CC=_tmp2C6->f1;_tmp2CB=_tmp2C6->f2;_LL2EE:;
if((char*)_tmp2CB.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2CC.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[0]=_tmp2CC;_tmp2C7;}))))
# 1650
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp2CB,sizeof(char),1)),out_file);else{
# 1652
({struct Cyc_String_pa_PrintArg_struct _tmp2CA;_tmp2CA.tag=0;_tmp2CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2CC);({void*_tmp2C8[1]={& _tmp2CA};Cyc_log(({const char*_tmp2C9="%s is not supported on this platform\n";_tag_dyneither(_tmp2C9,sizeof(char),38);}),_tag_dyneither(_tmp2C8,sizeof(void*),1));});});}}}
# 1655
({void*_tmp2CD=0;Cyc_fprintf(out_file,({const char*_tmp2CE="#endif\n";_tag_dyneither(_tmp2CE,sizeof(char),8);}),_tag_dyneither(_tmp2CD,sizeof(void*),0));});
if(Cyc_do_setjmp){int _tmp2CF=0;_npop_handler(0);return _tmp2CF;}else{
Cyc_fclose(out_file);}
# 1660
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2D0=cstubs;for(0;_tmp2D0 != 0;_tmp2D0=_tmp2D0->tl){
struct _tuple22*_tmp2D1=(struct _tuple22*)_tmp2D0->hd;struct _tuple22*_tmp2D2=_tmp2D1;struct _dyneither_ptr _tmp2D5;struct _dyneither_ptr _tmp2D4;_LL2F0: _tmp2D5=_tmp2D2->f1;_tmp2D4=_tmp2D2->f2;_LL2F1:;
if((char*)_tmp2D4.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2D5.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=_tmp2D5;_tmp2D3;}))))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp2D4,sizeof(char),1)),out_file);}};}
# 1671
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1673
({struct Cyc_String_pa_PrintArg_struct _tmp2D8;_tmp2D8.tag=0;_tmp2D8.f1=(struct _dyneither_ptr)({const char*_tmp2D9=filename;_tag_dyneither(_tmp2D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D9,1));});({void*_tmp2D6[1]={& _tmp2D8};Cyc_fprintf(out_file,({const char*_tmp2D7="#include <%s>\n\n";_tag_dyneither(_tmp2D7,sizeof(char),16);}),_tag_dyneither(_tmp2D6,sizeof(void*),1));});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2DA=cycstubs;for(0;_tmp2DA != 0;_tmp2DA=_tmp2DA->tl){
struct _tuple22*_tmp2DB=(struct _tuple22*)_tmp2DA->hd;struct _tuple22*_tmp2DC=_tmp2DB;struct _dyneither_ptr _tmp2DF;struct _dyneither_ptr _tmp2DE;_LL2F3: _tmp2DF=_tmp2DC->f1;_tmp2DE=_tmp2DC->f2;_LL2F4:;
if((char*)_tmp2DE.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2DF.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD[0]=_tmp2DF;_tmp2DD;}))))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp2DE,sizeof(char),1)),out_file);}}
# 1682
({void*_tmp2E0=0;Cyc_fprintf(out_file,({const char*_tmp2E1="\n";_tag_dyneither(_tmp2E1,sizeof(char),2);}),_tag_dyneither(_tmp2E0,sizeof(void*),0));});}{
# 1685
int _tmp2E2=0;_npop_handler(0);return _tmp2E2;};};};};};};};};
# 1351
;_pop_handler();}else{void*_tmp1DF=(void*)_exn_thrown;void*_tmp2E7=_tmp1DF;struct _dyneither_ptr _tmp2FD;struct _dyneither_ptr _tmp2FC;struct _dyneither_ptr _tmp2FB;struct _dyneither_ptr _tmp2FA;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2E7)->tag == Cyc_Core_Impossible){_LL2F6: _tmp2FA=((struct Cyc_Core_Impossible_exn_struct*)_tmp2E7)->f1;_LL2F7:
# 1689
({struct Cyc_String_pa_PrintArg_struct _tmp2EA;_tmp2EA.tag=0;_tmp2EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2FA);({void*_tmp2E8[1]={& _tmp2EA};Cyc_fprintf(Cyc_stderr,({const char*_tmp2E9="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp2E9,sizeof(char),26);}),_tag_dyneither(_tmp2E8,sizeof(void*),1));});});goto _LL2F5;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E7)->tag == Cyc_Dict_Absent){_LL2F8: _LL2F9:
# 1691
({void*_tmp2EB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EC="Got Dict::Absent\n";_tag_dyneither(_tmp2EC,sizeof(char),18);}),_tag_dyneither(_tmp2EB,sizeof(void*),0));});goto _LL2F5;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2E7)->tag == Cyc_Core_Failure){_LL2FA: _tmp2FB=((struct Cyc_Core_Failure_exn_struct*)_tmp2E7)->f1;_LL2FB:
# 1693
({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2FB);({void*_tmp2ED[1]={& _tmp2EF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2EE="Got Core::Failure(%s)\n";_tag_dyneither(_tmp2EE,sizeof(char),23);}),_tag_dyneither(_tmp2ED,sizeof(void*),1));});});goto _LL2F5;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2E7)->tag == Cyc_Core_Invalid_argument){_LL2FC: _tmp2FC=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2E7)->f1;_LL2FD:
# 1695
({struct Cyc_String_pa_PrintArg_struct _tmp2F2;_tmp2F2.tag=0;_tmp2F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2FC);({void*_tmp2F0[1]={& _tmp2F2};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F1="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp2F1,sizeof(char),26);}),_tag_dyneither(_tmp2F0,sizeof(void*),1));});});goto _LL2F5;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2E7)->tag == Cyc_Core_Not_found){_LL2FE: _LL2FF:
# 1697
({void*_tmp2F3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F4="Got Not_found\n";_tag_dyneither(_tmp2F4,sizeof(char),15);}),_tag_dyneither(_tmp2F3,sizeof(void*),0));});goto _LL2F5;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2E7)->tag == Cyc_NO_SUPPORT){_LL300: _tmp2FD=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2E7)->f1;_LL301:
# 1699
({struct Cyc_String_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=0;_tmp2F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2FD);({void*_tmp2F5[1]={& _tmp2F7};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F6="No support because %s\n";_tag_dyneither(_tmp2F6,sizeof(char),23);}),_tag_dyneither(_tmp2F5,sizeof(void*),1));});});goto _LL2F5;}else{_LL302: _LL303:
# 1701
({void*_tmp2F8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F9="Got unknown exception\n";_tag_dyneither(_tmp2F9,sizeof(char),23);}),_tag_dyneither(_tmp2F8,sizeof(void*),0));});goto _LL2F5;}}}}}}_LL2F5:;}};}
# 1706
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp300;_tmp300.tag=0;_tmp300.f1=(struct _dyneither_ptr)({const char*_tmp301=filename;_tag_dyneither(_tmp301,sizeof(char),_get_zero_arr_size_char((void*)_tmp301,1));});({void*_tmp2FE[1]={& _tmp300};Cyc_fprintf(Cyc_stderr,({const char*_tmp2FF="Error: could not create file %s\n";_tag_dyneither(_tmp2FF,sizeof(char),33);}),_tag_dyneither(_tmp2FE,sizeof(void*),1));});});
return 1;}
# 1711
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp304;_tmp304.tag=0;_tmp304.f1=(struct _dyneither_ptr)({const char*_tmp305=filename;_tag_dyneither(_tmp305,sizeof(char),_get_zero_arr_size_char((void*)_tmp305,1));});({void*_tmp302[1]={& _tmp304};Cyc_fprintf(out_file,({const char*_tmp303="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp303,sizeof(char),48);}),_tag_dyneither(_tmp302,sizeof(void*),1));});});
# 1715
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp308;_tmp308.tag=0;_tmp308.f1=(struct _dyneither_ptr)({const char*_tmp309=filename;_tag_dyneither(_tmp309,sizeof(char),_get_zero_arr_size_char((void*)_tmp309,1));});({void*_tmp306[1]={& _tmp308};Cyc_fprintf(Cyc_stderr,({const char*_tmp307="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp307,sizeof(char),52);}),_tag_dyneither(_tmp306,sizeof(void*),1));});});
# 1718
({void*_tmp30A=0;Cyc_log(({const char*_tmp30B="Not supported on this platform\n";_tag_dyneither(_tmp30B,sizeof(char),32);}),_tag_dyneither(_tmp30A,sizeof(void*),0));});
# 1725
return 0;};}
# 1729
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp323;_tmp323.tag=0;_tmp323.f1=(struct _dyneither_ptr)({const char*_tmp324=file;_tag_dyneither(_tmp324,sizeof(char),_get_zero_arr_size_char((void*)_tmp324,1));});({void*_tmp321[1]={& _tmp323};Cyc_fprintf(Cyc_stderr,({const char*_tmp322="Processing %s\n";_tag_dyneither(_tmp322,sizeof(char),15);}),_tag_dyneither(_tmp321,sizeof(void*),1));});});{
struct Cyc___cycFILE*_tmp325=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp325)){
({struct Cyc_String_pa_PrintArg_struct _tmp328;_tmp328.tag=0;_tmp328.f1=(struct _dyneither_ptr)({const char*_tmp329=file;_tag_dyneither(_tmp329,sizeof(char),_get_zero_arr_size_char((void*)_tmp329,1));});({void*_tmp326[1]={& _tmp328};Cyc_fprintf(Cyc_stderr,({const char*_tmp327="Error: could not open %s\n";_tag_dyneither(_tmp327,sizeof(char),26);}),_tag_dyneither(_tmp326,sizeof(void*),1));});});
return 1;}{
# 1737
struct Cyc___cycFILE*_tmp32A=_tmp325;
# 1741
struct _dyneither_ptr buf=({char*_tmp348=({unsigned int _tmp345=(unsigned int)1024;char*_tmp346=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp345 + 1));{unsigned int _tmp347=_tmp345;unsigned int i;for(i=0;i < _tmp347;i ++){_tmp346[i]=(char)'\000';}_tmp346[_tmp347]=(char)0;}_tmp346;});_tag_dyneither(_tmp348,sizeof(char),_get_zero_arr_size_char((void*)_tmp348,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp32B=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp32E;_tmp32E.tag=0;_tmp32E.f1=(struct _dyneither_ptr)({const char*_tmp32F=dir;_tag_dyneither(_tmp32F,sizeof(char),_get_zero_arr_size_char((void*)_tmp32F,1));});({void*_tmp32C[1]={& _tmp32E};Cyc_fprintf(Cyc_stderr,({const char*_tmp32D="Error: can't change directory to %s\n";_tag_dyneither(_tmp32D,sizeof(char),37);}),_tag_dyneither(_tmp32C,sizeof(void*),1));});});
return 1;}}
# 1749
if(Cyc_mode == Cyc_GATHER){
# 1751
struct _dyneither_ptr _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp338;_tmp338.tag=0;_tmp338.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp336;_tmp336.tag=0;_tmp336.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp334[3]={& _tmp336,& _tmp337,& _tmp338};Cyc_aprintf(({const char*_tmp335="echo | %s %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp335,sizeof(char),42);}),_tag_dyneither(_tmp334,sizeof(void*),3));});});});});
# 1753
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp333;_tmp333.tag=0;_tmp333.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp330);({void*_tmp331[1]={& _tmp333};Cyc_fprintf(Cyc_stderr,({const char*_tmp332="%s\n";_tag_dyneither(_tmp332,sizeof(char),4);}),_tag_dyneither(_tmp331,sizeof(void*),1));});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp330,sizeof(char),1)));}{
# 1758
struct Cyc_Lexing_lexbuf*_tmp339=Cyc_Lexing_from_file(_tmp32A);
struct _tuple23*entry;
while((entry=((struct _tuple23*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp339))!= 0){
struct _tuple23*_tmp33A=(struct _tuple23*)_check_null(entry);struct _tuple23*_tmp33B=_tmp33A;struct _dyneither_ptr _tmp341;struct Cyc_List_List*_tmp340;struct Cyc_List_List*_tmp33F;struct Cyc_List_List*_tmp33E;struct Cyc_List_List*_tmp33D;struct Cyc_List_List*_tmp33C;_LL307: _tmp341=_tmp33B->f1;_tmp340=_tmp33B->f2;_tmp33F=_tmp33B->f3;_tmp33E=_tmp33B->f4;_tmp33D=_tmp33B->f5;_tmp33C=_tmp33B->f6;_LL308:;
# 1763
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp341,sizeof(char),1)),_tmp340,_tmp33F,_tmp33E,_tmp33D,_tmp33C))
# 1765
return 1;}
# 1767
Cyc_fclose(_tmp32A);
# 1769
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp32B,sizeof(char),1))))){
({struct Cyc_String_pa_PrintArg_struct _tmp344;_tmp344.tag=0;_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32B);({void*_tmp342[1]={& _tmp344};Cyc_fprintf(Cyc_stderr,({const char*_tmp343="Error: could not change directory to %s\n";_tag_dyneither(_tmp343,sizeof(char),41);}),_tag_dyneither(_tmp342,sizeof(void*),1));});});
return 1;}}
# 1775
return 0;};};};}
# 1779
int Cyc_process_setjmp(const char*dir){
# 1782
struct _dyneither_ptr buf=({char*_tmp35A=({unsigned int _tmp357=(unsigned int)1024;char*_tmp358=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp357 + 1));{unsigned int _tmp359=_tmp357;unsigned int i;for(i=0;i < _tmp359;i ++){_tmp358[i]=(char)'\000';}_tmp358[_tmp359]=(char)0;}_tmp358;});_tag_dyneither(_tmp35A,sizeof(char),_get_zero_arr_size_char((void*)_tmp35A,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp349=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp34C;_tmp34C.tag=0;_tmp34C.f1=(struct _dyneither_ptr)({const char*_tmp34D=dir;_tag_dyneither(_tmp34D,sizeof(char),_get_zero_arr_size_char((void*)_tmp34D,1));});({void*_tmp34A[1]={& _tmp34C};Cyc_fprintf(Cyc_stderr,({const char*_tmp34B="Error: can't change directory to %s\n";_tag_dyneither(_tmp34B,sizeof(char),37);}),_tag_dyneither(_tmp34A,sizeof(void*),1));});});
return 1;}
# 1788
if(Cyc_process_file("setjmp.h",({struct _dyneither_ptr*_tmp34E[1];_tmp34E[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34E,sizeof(struct _dyneither_ptr*),1));}),0,({struct _tuple22*_tmp350[1];_tmp350[0]=({struct _tuple22*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->f1=({const char*_tmp353="setjmp";_tag_dyneither(_tmp353,sizeof(char),7);});_tmp351->f2=({const char*_tmp352="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp352,sizeof(char),29);});_tmp351;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp350,sizeof(struct _tuple22*),1));}),0,0))
# 1791
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp349,sizeof(char),1))))){
({struct Cyc_String_pa_PrintArg_struct _tmp356;_tmp356.tag=0;_tmp356.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp349);({void*_tmp354[1]={& _tmp356};Cyc_fprintf(Cyc_stderr,({const char*_tmp355="Error: could not change directory to %s\n";_tag_dyneither(_tmp355,sizeof(char),41);}),_tag_dyneither(_tmp354,sizeof(void*),1));});});
return 1;}
# 1796
return 0;}static char _tmp35B[13]="BUILDLIB.OUT";
# 1800
static struct _dyneither_ptr Cyc_output_dir={_tmp35B,_tmp35B,_tmp35B + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 1804
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1));_tmp35C->tl=Cyc_spec_files;_tmp35C;});}
# 1808
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 1812
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1815
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 1818
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D->hd=({struct _dyneither_ptr*_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E[0]=s;_tmp35E;});_tmp35D->tl=Cyc_cppargs;_tmp35D;});}
# 1821
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp361;_tmp361.tag=0;_tmp361.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp35F[1]={& _tmp361};Cyc_fprintf(Cyc_stderr,({const char*_tmp360="Unsupported option %s\n";_tag_dyneither(_tmp360,sizeof(char),23);}),_tag_dyneither(_tmp35F,sizeof(void*),1));});});
Cyc_badparse=1;}
# 1830
void GC_blacklist_warn_clear();struct _tuple26{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 1834
struct Cyc_List_List*options=({struct _tuple26*_tmp399[9];_tmp399[8]=({struct _tuple26*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->f1=({const char*_tmp3CF="-";_tag_dyneither(_tmp3CF,sizeof(char),2);});_tmp3CA->f2=1;_tmp3CA->f3=({const char*_tmp3CE="";_tag_dyneither(_tmp3CE,sizeof(char),1);});_tmp3CA->f4=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp3CD;_tmp3CD.tag=1;_tmp3CD.f1=Cyc_add_cpparg;_tmp3CD;});_tmp3CC;});_tmp3CA->f5=({const char*_tmp3CB="";_tag_dyneither(_tmp3CB,sizeof(char),1);});_tmp3CA;});_tmp399[7]=({struct _tuple26*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->f1=({const char*_tmp3C9="-v";_tag_dyneither(_tmp3C9,sizeof(char),3);});_tmp3C4->f2=0;_tmp3C4->f3=({const char*_tmp3C8="";_tag_dyneither(_tmp3C8,sizeof(char),1);});_tmp3C4->f4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp3C7;_tmp3C7.tag=3;_tmp3C7.f1=& Cyc_verbose;_tmp3C7;});_tmp3C6;});_tmp3C4->f5=({const char*_tmp3C5="Verbose operation";_tag_dyneither(_tmp3C5,sizeof(char),18);});_tmp3C4;});_tmp399[6]=({struct _tuple26*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=({const char*_tmp3C3="-B";_tag_dyneither(_tmp3C3,sizeof(char),3);});_tmp3BE->f2=1;_tmp3BE->f3=({const char*_tmp3C2="<file>";_tag_dyneither(_tmp3C2,sizeof(char),7);});_tmp3BE->f4=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0[0]=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp3C1;_tmp3C1.tag=1;_tmp3C1.f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp3C1;});_tmp3C0;});_tmp3BE->f5=({const char*_tmp3BF="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp3BF,sizeof(char),60);});_tmp3BE;});_tmp399[5]=({struct _tuple26*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->f1=({const char*_tmp3BD="-b";_tag_dyneither(_tmp3BD,sizeof(char),3);});_tmp3B8->f2=0;_tmp3B8->f3=({const char*_tmp3BC=" <machine>";_tag_dyneither(_tmp3BC,sizeof(char),11);});_tmp3B8->f4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA[0]=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp3BB;_tmp3BB.tag=5;_tmp3BB.f1=Cyc_Specsfile_set_target_arch;_tmp3BB;});_tmp3BA;});_tmp3B8->f5=({const char*_tmp3B9="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp3B9,sizeof(char),52);});_tmp3B8;});_tmp399[4]=({struct _tuple26*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->f1=({const char*_tmp3B7="-setjmp";_tag_dyneither(_tmp3B7,sizeof(char),8);});_tmp3B2->f2=0;_tmp3B2->f3=({const char*_tmp3B6="";_tag_dyneither(_tmp3B6,sizeof(char),1);});_tmp3B2->f4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp3B5;_tmp3B5.tag=3;_tmp3B5.f1=& Cyc_do_setjmp;_tmp3B5;});_tmp3B4;});_tmp3B2->f5=({const char*_tmp3B3="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp3B3,sizeof(char),186);});_tmp3B2;});_tmp399[3]=({struct _tuple26*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->f1=({const char*_tmp3B1="-finish";_tag_dyneither(_tmp3B1,sizeof(char),8);});_tmp3AC->f2=0;_tmp3AC->f3=({const char*_tmp3B0="";_tag_dyneither(_tmp3B0,sizeof(char),1);});_tmp3AC->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp3AF;_tmp3AF.tag=0;_tmp3AF.f1=Cyc_set_FINISH;_tmp3AF;});_tmp3AE;});_tmp3AC->f5=({const char*_tmp3AD="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp3AD,sizeof(char),57);});_tmp3AC;});_tmp399[2]=({struct _tuple26*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->f1=({const char*_tmp3AB="-gatherscript";_tag_dyneither(_tmp3AB,sizeof(char),14);});_tmp3A6->f2=0;_tmp3A6->f3=({const char*_tmp3AA="";_tag_dyneither(_tmp3AA,sizeof(char),1);});_tmp3A6->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp3A9;_tmp3A9.tag=0;_tmp3A9.f1=Cyc_set_GATHERSCRIPT;_tmp3A9;});_tmp3A8;});_tmp3A6->f5=({const char*_tmp3A7="Produce a script to gather C library info";_tag_dyneither(_tmp3A7,sizeof(char),42);});_tmp3A6;});_tmp399[1]=({struct _tuple26*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->f1=({const char*_tmp3A5="-gather";_tag_dyneither(_tmp3A5,sizeof(char),8);});_tmp3A0->f2=0;_tmp3A0->f3=({const char*_tmp3A4="";_tag_dyneither(_tmp3A4,sizeof(char),1);});_tmp3A0->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp3A3;_tmp3A3.tag=0;_tmp3A3.f1=Cyc_set_GATHER;_tmp3A3;});_tmp3A2;});_tmp3A0->f5=({const char*_tmp3A1="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp3A1,sizeof(char),56);});_tmp3A0;});_tmp399[0]=({struct _tuple26*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->f1=({const char*_tmp39F="-d";_tag_dyneither(_tmp39F,sizeof(char),3);});_tmp39A->f2=0;_tmp39A->f3=({const char*_tmp39E=" <file>";_tag_dyneither(_tmp39E,sizeof(char),8);});_tmp39A->f4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp39D;_tmp39D.tag=5;_tmp39D.f1=Cyc_set_output_dir;_tmp39D;});_tmp39C;});_tmp39A->f5=({const char*_tmp39B="Set the output directory to <file>";_tag_dyneither(_tmp39B,sizeof(char),35);});_tmp39A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp399,sizeof(struct _tuple26*),9));});
# 1867
struct _dyneither_ptr _tmp362=Cyc_Specsfile_parse_b(options,Cyc_add_spec_file,Cyc_no_other,({const char*_tmp398="Options:";_tag_dyneither(_tmp398,sizeof(char),9);}),argv);
# 1869
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_spec_file,Cyc_no_other,({const char*_tmp363="Options:";_tag_dyneither(_tmp363,sizeof(char),9);}),_tmp362);
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_Arg_usage(options,({const char*_tmp364="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp364,sizeof(char),59);}));
# 1879
return 1;}{
# 1885
struct _dyneither_ptr _tmp365=Cyc_Specsfile_find_in_arch_path(({const char*_tmp397="cycspecs";_tag_dyneither(_tmp397,sizeof(char),9);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp368;_tmp368.tag=0;_tmp368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp365);({void*_tmp366[1]={& _tmp368};Cyc_fprintf(Cyc_stderr,({const char*_tmp367="Reading from specs file %s\n";_tag_dyneither(_tmp367,sizeof(char),28);}),_tag_dyneither(_tmp366,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp369=Cyc_Specsfile_read_specs(_tmp365);
Cyc_target_cflags=Cyc_Specsfile_get_spec(_tmp369,({const char*_tmp36A="cyclone_target_cflags";_tag_dyneither(_tmp36A,sizeof(char),22);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp36D;_tmp36D.tag=0;_tmp36D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp36B[1]={& _tmp36D};Cyc_fprintf(Cyc_stderr,({const char*_tmp36C="Target cflags are %s\n";_tag_dyneither(_tmp36C,sizeof(char),22);}),_tag_dyneither(_tmp36B,sizeof(void*),1));});});
Cyc_cyclone_cc=Cyc_Specsfile_get_spec(_tmp369,({const char*_tmp36E="cyclone_cc";_tag_dyneither(_tmp36E,sizeof(char),11);}));
if(!((unsigned int)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp36F="gcc";_tag_dyneither(_tmp36F,sizeof(char),4);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp372;_tmp372.tag=0;_tmp372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp370[1]={& _tmp372};Cyc_fprintf(Cyc_stderr,({const char*_tmp371="C compiler is %s\n";_tag_dyneither(_tmp371,sizeof(char),18);}),_tag_dyneither(_tmp370,sizeof(void*),1));});});
# 1894
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp373=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp374="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp374,sizeof(char),22);}),_tag_dyneither(_tmp373,sizeof(void*),0));});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
({void*_tmp375=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp376="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp376,sizeof(char),35);}),_tag_dyneither(_tmp375,sizeof(void*),0));});
 exit(1);}
# 1902
({void*_tmp377=0;Cyc_prscript(({const char*_tmp378="#!/bin/sh\n";_tag_dyneither(_tmp378,sizeof(char),11);}),_tag_dyneither(_tmp377,sizeof(void*),0));});
({void*_tmp379=0;Cyc_prscript(({const char*_tmp37A="GCC=\"gcc\"\n";_tag_dyneither(_tmp37A,sizeof(char),11);}),_tag_dyneither(_tmp379,sizeof(void*),0));});}
# 1907
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp37D;_tmp37D.tag=0;_tmp37D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp37B[1]={& _tmp37D};Cyc_fprintf(Cyc_stderr,({const char*_tmp37C="Error: could not create directory %s\n";_tag_dyneither(_tmp37C,sizeof(char),38);}),_tag_dyneither(_tmp37B,sizeof(void*),1));});});
return 1;}
# 1911
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp380;_tmp380.tag=0;_tmp380.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp37E[1]={& _tmp380};Cyc_fprintf(Cyc_stderr,({const char*_tmp37F="Output directory is %s\n";_tag_dyneither(_tmp37F,sizeof(char),24);}),_tag_dyneither(_tmp37E,sizeof(void*),1));});});
# 1914
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp383;_tmp383.tag=0;_tmp383.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp381[1]={& _tmp383};Cyc_prscript(({const char*_tmp382="cd %s\n";_tag_dyneither(_tmp382,sizeof(char),7);}),_tag_dyneither(_tmp381,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0;_tmp386.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp384[1]={& _tmp386};Cyc_prscript(({const char*_tmp385="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp385,sizeof(char),41);}),_tag_dyneither(_tmp384,sizeof(void*),1));});});}
# 1920
if(!Cyc_gathering()){
# 1923
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp387="BUILDLIB.LOG";_tag_dyneither(_tmp387,sizeof(char),13);})),sizeof(char),1)),"w");
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0;_tmp38A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp388[1]={& _tmp38A};Cyc_fprintf(Cyc_stderr,({const char*_tmp389="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp389,sizeof(char),50);}),_tag_dyneither(_tmp388,sizeof(void*),1));});});
return 1;}
# 1929
if(!Cyc_do_setjmp){
# 1931
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp38B="cstubs.c";_tag_dyneither(_tmp38B,sizeof(char),9);})),sizeof(char),1)),"w");
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0;_tmp38E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp38C[1]={& _tmp38E};Cyc_fprintf(Cyc_stderr,({const char*_tmp38D="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp38D,sizeof(char),50);}),_tag_dyneither(_tmp38C,sizeof(void*),1));});});
return 1;}
# 1938
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp38F="cycstubs.cyc";_tag_dyneither(_tmp38F,sizeof(char),13);})),sizeof(char),1)),"w");
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0;_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp390[1]={& _tmp392};Cyc_fprintf(Cyc_stderr,({const char*_tmp391="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp391,sizeof(char),52);}),_tag_dyneither(_tmp390,sizeof(void*),1));});});
# 1943
return 1;}
# 1945
({void*_tmp393=0;Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),({const char*_tmp394="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp394,sizeof(char),32);}),_tag_dyneither(_tmp393,sizeof(void*),0));});}}{
# 1952
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 1958
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp395=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp396="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp396,sizeof(char),22);}),_tag_dyneither(_tmp395,sizeof(void*),0));});
 exit(1);}}}
# 1966
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 1969
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 1977
return 0;};};};};}

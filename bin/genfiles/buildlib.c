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
# 265 "core.h"
void Cyc_Core_rethrow(void*);
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
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1110 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1161
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1173
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1185
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
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 33
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 44
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;};
# 49
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple13{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple13 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_declarator_list_t;
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
typedef struct Cyc_List_List*Cyc_pointer_quals_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple14{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple14 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple15{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple16{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple13 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple18{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple19 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple9*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 68
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 33 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 31 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 38
void*Cyc_Toc_typ_to_c(void*t);
# 145 "tcutil.h"
int Cyc_Tcutil_unify(void*,void*);
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
# 78 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4]="gcc";
# 88
static struct _dyneither_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4};static char _tmp1[1]="";
static struct _dyneither_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1};
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
({void*_tmp2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34);}),_tag_dyneither(_tmp2,sizeof(void*),0));});
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
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);_tmp5;});}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 118
typedef struct _tuple21*Cyc_line_t;
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
# 132
typedef struct _tuple22*Cyc_suck_line_t;
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple23{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 140
typedef struct _tuple23*Cyc_ifdefined_t;struct _tuple24{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 144
typedef struct _tuple24*Cyc_userdef_t;struct _tuple25{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};
# 146
typedef struct _tuple25*Cyc_spec_t;
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
static struct Cyc_List_List*Cyc_cppargs=0;static char _tmp6[14]="BUILDLIB_sym_";
# 173
struct _dyneither_ptr Cyc_user_prefix={_tmp6,_tmp6,_tmp6 + 14};
static struct _dyneither_ptr*Cyc_add_user_prefix(struct _dyneither_ptr*symbol){
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_user_prefix,(struct _dyneither_ptr)*symbol);
return({struct _dyneither_ptr*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=s;_tmp7;});}
# 178
static struct _dyneither_ptr Cyc_remove_user_prefix(struct _dyneither_ptr symbol){
unsigned int prefix_len=Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix);
if(Cyc_strncmp((struct _dyneither_ptr)symbol,(struct _dyneither_ptr)Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)symbol);({void*_tmp8[1]={& _tmpA};Cyc_fprintf(Cyc_stderr,({const char*_tmp9="Internal error: bad user type name %s\n";_tag_dyneither(_tmp9,sizeof(char),39);}),_tag_dyneither(_tmp8,sizeof(void*),1));});});
return symbol;}
# 184
return(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)symbol,(int)prefix_len,Cyc_strlen((struct _dyneither_ptr)symbol)- prefix_len);}
# 187
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_tmpC=_tmpB;struct Cyc_Absyn_Typedefdecl*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp12;switch(*((int*)_tmpC)){case 5: _LL1: _tmp12=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2:
# 190
(*_tmp12->name).f2=({struct _dyneither_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=Cyc_remove_user_prefix(*(*_tmp12->name).f2);_tmpD;});
goto _LL0;case 7: _LL3: _tmp13=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4:
# 193
(*_tmp13->name).f2=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=Cyc_remove_user_prefix(*(*_tmp13->name).f2);_tmpE;});
goto _LL0;case 8: _LL5: _tmp14=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6:
# 196
(*_tmp14->name).f2=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=Cyc_remove_user_prefix(*(*_tmp14->name).f2);_tmpF;});
goto _LL0;case 1: _LL7: _LL8:
# 199
 goto _LLA;case 0: _LL9: _LLA:
 goto _LLC;case 4: _LLB: _LLC:
 goto _LLE;case 6: _LLD: _LLE:
 goto _LL10;case 2: _LLF: _LL10:
 goto _LL12;case 3: _LL11: _LL12:
 goto _LL14;case 9: _LL13: _LL14:
 goto _LL16;case 10: _LL15: _LL16:
 goto _LL18;case 11: _LL17: _LL18:
 goto _LL1A;case 12: _LL19: _LL1A:
 goto _LL1C;case 13: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 211
({void*_tmp10=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11="Error in .cys file: bad user-defined type definition\n";_tag_dyneither(_tmp11,sizeof(char),54);}),_tag_dyneither(_tmp10,sizeof(void*),0));});
 exit(1);}_LL0:;}
# 217
static struct Cyc_Absyn_Decl*Cyc_named_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*decl_name){
struct _dyneither_ptr*name;
for(0;decls != 0;decls=decls->tl){
{void*_tmp15=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_tmp16=_tmp15;struct Cyc_Absyn_Typedefdecl*_tmp22;struct Cyc_Absyn_Enumdecl*_tmp21;struct Cyc_Absyn_Aggrdecl*_tmp20;switch(*((int*)_tmp16)){case 5: _LL20: _tmp20=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL21: {
# 222
struct _tuple1*_tmp17=_tmp20->name;struct _tuple1*_tmp18=_tmp17;struct _dyneither_ptr*_tmp19;_LL3F: _tmp19=_tmp18->f2;_LL40:;
name=_tmp19;
goto _LL1F;}case 7: _LL22: _tmp21=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL23: {
# 226
struct _tuple1*_tmp1A=_tmp21->name;struct _tuple1*_tmp1B=_tmp1A;struct _dyneither_ptr*_tmp1C;_LL42: _tmp1C=_tmp1B->f2;_LL43:;
name=_tmp1C;
goto _LL1F;}case 8: _LL24: _tmp22=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL25: {
# 230
struct _tuple1*_tmp1D=_tmp22->name;struct _tuple1*_tmp1E=_tmp1D;struct _dyneither_ptr*_tmp1F;_LL45: _tmp1F=_tmp1E->f2;_LL46:;
name=_tmp1F;
goto _LL1F;}case 0: _LL26: _LL27:
# 234
 goto _LL29;case 1: _LL28: _LL29:
 goto _LL2B;case 13: _LL2A: _LL2B:
 goto _LL2D;case 14: _LL2C: _LL2D:
 goto _LL2F;case 2: _LL2E: _LL2F:
 goto _LL31;case 6: _LL30: _LL31:
 goto _LL33;case 3: _LL32: _LL33:
 goto _LL35;case 9: _LL34: _LL35:
 goto _LL37;case 10: _LL36: _LL37:
 goto _LL39;case 11: _LL38: _LL39:
 goto _LL3B;case 12: _LL3A: _LL3B:
 goto _LL3D;default: _LL3C: _LL3D:
# 246
 name=0;
goto _LL1F;}_LL1F:;}
# 249
if((unsigned int)name  && !Cyc_strptrcmp(name,decl_name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 252
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=({const char*_tmp25="Internal Error: bad user-def name";_tag_dyneither(_tmp25,sizeof(char),34);});_tmp24;});_tmp23;}));}struct _tuple26{void*f1;void*f2;};struct _tuple27{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 259
static void Cyc_merge_rep_decl(struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
{struct _tuple26 _tmp26=({struct _tuple26 _tmpA2;_tmpA2.f1=c_decl->r;_tmpA2.f2=user_decl->r;_tmpA2;});struct _tuple26 _tmp27=_tmp26;struct Cyc_Absyn_Typedefdecl*_tmpA1;struct Cyc_Absyn_Typedefdecl*_tmpA0;struct Cyc_Absyn_Enumdecl*_tmp9F;struct Cyc_Absyn_Enumdecl*_tmp9E;struct Cyc_Absyn_Aggrdecl*_tmp9D;struct Cyc_Absyn_Aggrdecl*_tmp9C;switch(*((int*)_tmp27.f1)){case 5: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 5){_LL48: _tmp9D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmp9C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL49:
# 265
 if(_tmp9C->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _dyneither_ptr)({const char*_tmp2C=": universal quantification in user definition not allowed";_tag_dyneither(_tmp2C,sizeof(char),58);});({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp28[2]={& _tmp2A,& _tmp2B};Cyc_fprintf(Cyc_stderr,({const char*_tmp29="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp29,sizeof(char),62);}),_tag_dyneither(_tmp28,sizeof(void*),2));});});});return;}
if(_tmp9C->sc != _tmp9D->sc)
({struct Cyc_String_pa_PrintArg_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct _dyneither_ptr)({const char*_tmp31=": scopes don't match";_tag_dyneither(_tmp31,sizeof(char),21);});({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp2D[2]={& _tmp2F,& _tmp30};Cyc_fprintf(Cyc_stderr,({const char*_tmp2E="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp2E,sizeof(char),71);}),_tag_dyneither(_tmp2D,sizeof(void*),2));});});});
# 270
if(_tmp9C->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct _dyneither_ptr)({const char*_tmp36=": no user definition";_tag_dyneither(_tmp36,sizeof(char),21);});({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp32[2]={& _tmp34,& _tmp35};Cyc_fprintf(Cyc_stderr,({const char*_tmp33="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp33,sizeof(char),62);}),_tag_dyneither(_tmp32,sizeof(void*),2));});});});return;}
if(_tmp9D->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _dyneither_ptr)({const char*_tmp3B=": no definition for system version";_tag_dyneither(_tmp3B,sizeof(char),35);});({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp37[2]={& _tmp39,& _tmp3A};Cyc_fprintf(Cyc_stderr,({const char*_tmp38="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp38,sizeof(char),71);}),_tag_dyneither(_tmp37,sizeof(void*),2));});});});
c_decl->r=user_decl->r;
return;}
# 277
{struct _tuple27 _tmp3C=({struct _tuple27 _tmp79;_tmp79.f1=_tmp9D->impl;_tmp79.f2=_tmp9C->impl;_tmp79;});struct _tuple27 _tmp3D=_tmp3C;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;int _tmp74;if(_tmp3D.f1 != 0){if(_tmp3D.f2 != 0){_LL51: _tmp78=(_tmp3D.f1)->fields;_tmp77=(_tmp3D.f2)->exist_vars;_tmp76=(_tmp3D.f2)->rgn_po;_tmp75=(_tmp3D.f2)->fields;_tmp74=(_tmp3D.f2)->tagged;_LL52:
# 279
 if(_tmp74){
({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0;_tmp41.f1=(struct _dyneither_ptr)({const char*_tmp42=": user @tagged annotation not allowed";_tag_dyneither(_tmp42,sizeof(char),38);});({struct Cyc_String_pa_PrintArg_struct _tmp40;_tmp40.tag=0;_tmp40.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp3E[2]={& _tmp40,& _tmp41};Cyc_fprintf(Cyc_stderr,({const char*_tmp3F="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp3F,sizeof(char),62);}),_tag_dyneither(_tmp3E,sizeof(void*),2));});});});return;}
if((unsigned int)_tmp77){
({struct Cyc_String_pa_PrintArg_struct _tmp46;_tmp46.tag=0;_tmp46.f1=(struct _dyneither_ptr)({const char*_tmp47=": existential quantification in user definition not allowed";_tag_dyneither(_tmp47,sizeof(char),60);});({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp43[2]={& _tmp45,& _tmp46};Cyc_fprintf(Cyc_stderr,({const char*_tmp44="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp44,sizeof(char),62);}),_tag_dyneither(_tmp43,sizeof(void*),2));});});});return;}
if((unsigned int)_tmp76){
({struct Cyc_String_pa_PrintArg_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _dyneither_ptr)({const char*_tmp4C=": region partial order in user definition not allowed";_tag_dyneither(_tmp4C,sizeof(char),54);});({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp48[2]={& _tmp4A,& _tmp4B};Cyc_fprintf(Cyc_stderr,({const char*_tmp49="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp49,sizeof(char),62);}),_tag_dyneither(_tmp48,sizeof(void*),2));});});});return;}{
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
({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4E->name);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));({void*_tmp56[3]={& _tmp58,& _tmp59,& _tmp5A};Cyc_aprintf(({const char*_tmp57=": type %s of user definition of field %s != %s";_tag_dyneither(_tmp57,sizeof(char),47);}),_tag_dyneither(_tmp56,sizeof(void*),3));});});});}));({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_fprintf(Cyc_stderr,({const char*_tmp53="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp53,sizeof(char),71);}),_tag_dyneither(_tmp52,sizeof(void*),2));});});});else{
# 304
if(_tmp4F->width != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));({void*_tmp5F[1]={& _tmp61};Cyc_aprintf(({const char*_tmp60=": ignoring width of user definition of field %s";_tag_dyneither(_tmp60,sizeof(char),48);}),_tag_dyneither(_tmp5F,sizeof(void*),1));});}));({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0;_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp5B[2]={& _tmp5D,& _tmp5E};Cyc_fprintf(Cyc_stderr,({const char*_tmp5C="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp5C,sizeof(char),71);}),_tag_dyneither(_tmp5B,sizeof(void*),2));});});});
# 308
if(_tmp4F->attributes != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));({void*_tmp66[1]={& _tmp68};Cyc_aprintf(({const char*_tmp67=": ignoring attributes of user definition of field %s";_tag_dyneither(_tmp67,sizeof(char),53);}),_tag_dyneither(_tmp66,sizeof(void*),1));});}));({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp62[2]={& _tmp64,& _tmp65};Cyc_fprintf(Cyc_stderr,({const char*_tmp63="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp63,sizeof(char),71);}),_tag_dyneither(_tmp62,sizeof(void*),2));});});});
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
({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp4D->hd)->name);({void*_tmp6E[1]={& _tmp70};Cyc_aprintf(({const char*_tmp6F=": no definition of field %s in system version";_tag_dyneither(_tmp6F,sizeof(char),46);}),_tag_dyneither(_tmp6E,sizeof(void*),1));});}));({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));({void*_tmp6A[2]={& _tmp6C,& _tmp6D};Cyc_fprintf(Cyc_stderr,({const char*_tmp6B="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp6B,sizeof(char),71);}),_tag_dyneither(_tmp6A,sizeof(void*),2));});});});}
# 331
_tmp4D=_tmp4D->tl;}
# 333
goto _LL50;};};}else{goto _LL53;}}else{_LL53: _LL54:
# 335
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Core_Impossible_exn_struct _tmp72;_tmp72.tag=Cyc_Core_Impossible;_tmp72.f1=({const char*_tmp73="Internal Error: NULL cases not possible";_tag_dyneither(_tmp73,sizeof(char),40);});_tmp72;});_tmp71;}));}_LL50:;}
# 337
return;}else{goto _LL4E;}case 7: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 7){_LL4A: _tmp9F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmp9E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL4B:
# 339
({struct Cyc_String_pa_PrintArg_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(struct _dyneither_ptr)({const char*_tmp7E=": enum merging not currently supported";_tag_dyneither(_tmp7E,sizeof(char),39);});({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F->name));({void*_tmp7A[2]={& _tmp7C,& _tmp7D};Cyc_fprintf(Cyc_stderr,({const char*_tmp7B="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp7B,sizeof(char),62);}),_tag_dyneither(_tmp7A,sizeof(void*),2));});});});return;}else{goto _LL4E;}case 8: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 8){_LL4C: _tmpA1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmpA0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL4D:
# 341
 if(_tmpA0->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct _dyneither_ptr)({const char*_tmp83=": universal quantification in user definition not supported";_tag_dyneither(_tmp83,sizeof(char),60);});({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));({void*_tmp7F[2]={& _tmp81,& _tmp82};Cyc_fprintf(Cyc_stderr,({const char*_tmp80="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp80,sizeof(char),62);}),_tag_dyneither(_tmp7F,sizeof(void*),2));});});});return;}
# 344
if(_tmpA0->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(struct _dyneither_ptr)({const char*_tmp88=": no user definition";_tag_dyneither(_tmp88,sizeof(char),21);});({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));({void*_tmp84[2]={& _tmp86,& _tmp87};Cyc_fprintf(Cyc_stderr,({const char*_tmp85="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp85,sizeof(char),62);}),_tag_dyneither(_tmp84,sizeof(void*),2));});});});return;}
if(_tmpA1->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _dyneither_ptr)({const char*_tmp8D=": no definition for system version";_tag_dyneither(_tmp8D,sizeof(char),35);});({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));({void*_tmp89[2]={& _tmp8B,& _tmp8C};Cyc_fprintf(Cyc_stderr,({const char*_tmp8A="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp8A,sizeof(char),71);}),_tag_dyneither(_tmp89,sizeof(void*),2));});});});
c_decl->r=user_decl->r;
return;}{
# 352
void*_tmp8E=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA1->defn));
void*_tmp8F=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA0->defn));
if(!Cyc_Tcutil_unify(_tmp8E,_tmp8F))
({struct Cyc_String_pa_PrintArg_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0;_tmp97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpA1->defn)));({struct Cyc_String_pa_PrintArg_struct _tmp96;_tmp96.tag=0;_tmp96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpA0->defn)));({void*_tmp94[2]={& _tmp96,& _tmp97};Cyc_aprintf(({const char*_tmp95=": type definition %s of user definition != %s";_tag_dyneither(_tmp95,sizeof(char),46);}),_tag_dyneither(_tmp94,sizeof(void*),2));});});}));({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));({void*_tmp90[2]={& _tmp92,& _tmp93};Cyc_fprintf(Cyc_stderr,({const char*_tmp91="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp91,sizeof(char),71);}),_tag_dyneither(_tmp90,sizeof(void*),2));});});});else{
# 361
_tmpA1->defn=_tmpA0->defn;
_tmpA1->tq=_tmpA0->tq;}
# 364
goto _LL47;};}else{goto _LL4E;}default: _LL4E: _LL4F:
# 366
({struct Cyc_String_pa_PrintArg_struct _tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 368
Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp9B[1];_tmp9B[0]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9B,sizeof(struct Cyc_Absyn_Decl*),1));})));({void*_tmp98[1]={& _tmp9A};Cyc_fprintf(Cyc_stderr,({const char*_tmp99="Error in .cys file: bad user-defined type %s\n";_tag_dyneither(_tmp99,sizeof(char),46);}),_tag_dyneither(_tmp98,sizeof(void*),1));});});
return;}_LL47:;}
# 372
c_decl->r=user_decl->r;}
# 377
const int Cyc_lex_base[430]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 22,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,755,756,1254,- 21,1275,755,756,753,752,751,746,748,751,752,750,763,1368,756,757,755,768,1370,1394,771,772,762,763,761,774,1485,766,767,776,789,1594,- 7,- 8,8,956,2570,9,930,2594,2632,1103,1259,- 49,1278,- 2,828,- 4,829,930,1479,830,1104,925,1560,831,2659,2702,862,861,864,975,2772,892,1053,- 36,- 42,- 37,2847,- 28,906,- 40,- 25,928,- 27,- 45,- 39,- 48,2922,2951,1676,995,1005,1337,2961,2991,1693,1787,3024,3055,3093,998,1041,3163,3201,1077,1087,1079,1123,1144,1200,- 6,- 34,946,3133,- 47,- 30,- 32,- 46,- 29,- 31,- 33,964,3241,999,1000,1806,1002,1029,1040,1043,1045,1121,1127,1129,1130,3314,3398,- 23,- 16,- 18,1908,1133,- 24,- 41,- 38,- 35,- 26,1488,3480,2,3563,1132,15,1078,1079,1079,1077,1116,1126,1196};
const int Cyc_lex_backtrk[430]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[430]={- 1,- 1,- 1,322,311,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,391,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3820]={0,0,0,0,0,0,0,0,0,0,22,19,28,422,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,423,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,420,420,420,420,420,420,420,420,420,420,31,106,22,210,117,42,219,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,31,222,223,226,420,135,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,418,418,418,418,418,418,418,418,418,418,124,20,77,22,70,57,58,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,59,60,61,62,418,63,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,64,65,132,323,324,323,323,324,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,323,325,326,75,78,327,328,329,48,48,330,331,48,332,333,334,335,336,336,336,336,336,336,336,336,336,337,79,338,339,340,48,19,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,19,- 1,- 1,342,341,80,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,312,343,104,313,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,314,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,315,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,277,278,293,288,284,285,286,287,102,289,290,291,292,37,294,295,296,297,306,301,302,303,304,305,307,308,38,39,39,39,39,39,39,39,39,39,39,309,310,416,415,390,381,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,356,352,353,354,39,351,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,346,22,33,33,33,33,33,33,33,33,33,33,321,321,350,413,321,385,31,33,33,33,33,33,33,320,320,320,320,320,320,320,320,386,387,321,355,21,414,384,41,41,41,41,41,41,41,41,41,41,412,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,348,349,160,196,41,279,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,409,41,41,41,41,41,41,41,41,41,41,105,36,36,275,35,228,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,344,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,388,46,35,244,31,31,31,31,31,31,31,31,245,49,246,31,31,102,389,167,46,408,182,298,298,143,21,298,280,280,345,424,280,425,426,427,47,47,47,47,47,47,47,47,47,47,298,31,31,102,247,280,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,428,429,21,380,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,299,380,46,278,278,281,0,278,321,321,300,0,321,0,0,282,380,0,0,0,283,46,0,0,280,280,278,417,280,417,417,321,0,21,279,0,0,47,47,47,47,47,47,47,47,47,47,280,380,0,417,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,0,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,281,0,81,292,292,297,297,292,0,297,282,363,363,363,363,363,363,363,363,363,363,81,0,0,0,0,292,0,297,298,298,0,0,298,19,0,31,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,298,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,- 1,0,83,0,299,305,305,0,417,305,417,417,0,0,300,0,0,0,0,0,0,83,0,0,0,0,0,0,305,- 1,0,417,0,0,0,0,28,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,392,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,310,310,92,382,310,383,383,383,383,383,383,383,383,383,383,0,0,0,0,0,0,92,0,310,0,0,0,0,0,0,0,35,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,362,95,362,0,0,363,363,363,363,363,363,363,363,363,363,0,- 1,366,0,366,95,0,367,367,367,367,367,367,367,367,367,367,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,367,367,367,367,367,367,367,367,367,367,143,0,0,0,0,0,0,0,100,410,410,410,410,410,410,410,410,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,143,0,103,0,0,0,0,0,0,411,411,411,411,411,411,411,411,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,316,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,0,0,0,0,0,0,0,0,317,317,317,317,317,317,317,317,0,0,0,0,0,0,0,0,0,141,141,141,141,141,141,0,319,319,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,319,319,319,319,319,319,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,319,319,319,319,319,319,319,319,319,319,318,319,319,319,319,319,319,319,319,319,319,319,319,0,0,357,0,368,368,368,368,368,368,368,368,369,369,0,0,0,0,0,0,0,0,0,0,0,359,319,319,319,319,319,319,370,0,0,0,0,0,0,0,0,371,0,0,372,357,0,358,358,358,358,358,358,358,358,358,358,359,0,0,0,0,0,0,370,0,0,0,359,0,0,0,0,371,0,360,372,0,0,0,0,0,0,0,361,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,359,0,0,0,0,0,0,360,0,0,0,0,0,0,0,0,361,347,347,347,347,347,347,347,347,347,347,0,0,0,0,0,0,0,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,0,0,0,0,347,0,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,0,0,0,0,0,0,0,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,0,0,0,0,347,0,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,0,0,0,0,0,0,365,96,0,0,0,0,357,96,358,358,358,358,358,358,358,358,358,358,363,363,363,363,363,363,363,363,363,363,0,359,0,0,365,96,0,0,360,0,0,96,94,0,0,0,0,361,94,0,364,364,364,364,364,364,364,364,364,364,0,0,0,359,0,0,0,0,0,0,360,365,96,0,94,0,0,0,96,361,94,0,0,367,367,367,367,367,367,367,367,367,367,0,0,0,0,0,0,0,0,0,0,365,96,96,0,0,0,0,96,96,357,0,368,368,368,368,368,368,368,368,369,369,0,0,0,0,0,0,0,0,0,0,0,359,0,96,0,0,0,0,378,96,0,0,0,0,0,0,357,379,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,359,0,0,0,0,0,359,378,0,0,0,0,0,376,0,0,379,0,0,0,0,0,377,0,0,383,383,383,383,383,383,383,383,383,383,0,0,0,359,0,0,0,0,0,0,376,365,96,0,0,0,0,0,96,377,373,373,373,373,373,373,373,373,373,373,0,0,0,0,0,0,0,373,373,373,373,373,373,365,96,0,0,0,0,0,96,0,0,0,0,0,0,0,373,373,373,373,373,373,373,373,373,373,0,373,373,373,373,373,373,373,373,373,373,373,373,0,0,0,393,0,374,0,0,394,0,0,0,0,0,375,0,0,395,395,395,395,395,395,395,395,0,373,373,373,373,373,373,396,0,0,0,0,374,0,0,0,0,0,0,0,0,375,0,0,0,0,0,0,0,0,0,0,0,0,0,0,397,0,0,0,0,398,399,0,0,0,400,0,0,0,0,0,0,0,401,0,0,0,402,0,403,0,404,0,405,406,406,406,406,406,406,406,406,406,406,0,0,0,0,0,0,0,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,0,0,0,0,0,0,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,407,0,0,0,0,0,0,0,0,406,406,406,406,406,406,406,406,406,406,0,0,0,0,0,0,0,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,0,0,0,0,0,0,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,19,0,0,419,0,0,0,418,418,418,418,418,418,418,418,418,418,0,0,0,0,0,0,0,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,0,0,0,0,418,0,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,421,0,0,0,0,0,0,0,420,420,420,420,420,420,420,420,420,420,0,0,0,0,0,0,0,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,0,0,0,0,420,0,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3820]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,313,316,43,43,46,46,43,422,46,- 1,- 1,- 1,- 1,- 1,123,238,419,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,276,277,281,282,283,284,285,286,287,288,289,290,291,16,293,294,295,296,299,300,301,302,303,304,306,307,16,16,16,16,16,16,16,16,16,16,16,308,309,325,327,330,334,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,337,338,338,338,16,339,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,342,16,33,33,33,33,33,33,33,33,33,33,314,314,349,328,314,332,15,33,33,33,33,33,33,317,317,317,317,317,317,317,317,332,332,314,352,314,328,382,39,39,39,39,39,39,39,39,39,39,391,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,340,340,393,394,39,396,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,397,41,41,41,41,41,41,41,41,41,41,398,360,361,399,370,400,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,360,361,343,370,41,371,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,331,44,371,242,320,320,320,320,320,320,320,320,242,401,242,374,375,376,331,402,44,403,404,144,144,411,421,144,145,145,343,423,145,424,425,426,44,44,44,44,44,44,44,44,44,44,144,374,375,376,242,145,377,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,427,428,429,378,44,377,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,378,47,278,278,145,- 1,278,321,321,144,- 1,321,- 1,- 1,145,379,- 1,- 1,- 1,145,47,- 1,- 1,280,280,278,323,280,323,323,321,- 1,321,278,- 1,- 1,47,47,47,47,47,47,47,47,47,47,280,379,- 1,323,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,- 1,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,- 1,81,292,292,297,297,292,- 1,297,280,362,362,362,362,362,362,362,362,362,362,81,- 1,- 1,- 1,- 1,292,- 1,297,298,298,- 1,- 1,298,292,- 1,297,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,298,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,329,- 1,82,- 1,298,305,305,- 1,417,305,417,417,- 1,- 1,298,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,305,329,- 1,417,- 1,- 1,- 1,- 1,305,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,329,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,310,310,92,333,310,333,333,333,333,333,333,333,333,333,333,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,310,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,359,93,359,- 1,- 1,359,359,359,359,359,359,359,359,359,359,- 1,329,365,- 1,365,93,- 1,365,365,365,365,365,365,365,365,365,365,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,366,366,366,366,366,366,366,366,366,366,395,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,395,395,395,395,395,395,395,395,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,410,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,410,410,410,410,410,410,410,410,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,315,- 1,- 1,315,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,315,315,315,315,315,315,315,315,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,- 1,318,318,318,318,318,318,318,318,318,318,- 1,- 1,- 1,- 1,- 1,- 1,- 1,318,318,318,318,318,318,- 1,- 1,315,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,319,319,319,319,319,319,319,319,319,319,315,318,318,318,318,318,318,319,319,319,319,319,319,- 1,- 1,335,- 1,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,335,319,319,319,319,319,319,335,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,335,- 1,- 1,335,336,- 1,336,336,336,336,336,336,336,336,336,336,335,- 1,- 1,- 1,- 1,- 1,- 1,335,- 1,- 1,- 1,336,- 1,- 1,- 1,- 1,335,- 1,336,335,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,- 1,- 1,- 1,- 1,- 1,- 1,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,341,341,341,341,341,341,341,341,341,341,- 1,- 1,- 1,- 1,- 1,- 1,- 1,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,- 1,- 1,- 1,- 1,341,- 1,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,347,347,347,347,347,347,347,347,347,347,- 1,- 1,- 1,- 1,- 1,- 1,- 1,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,- 1,- 1,- 1,- 1,347,- 1,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,347,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,357,357,- 1,- 1,- 1,- 1,358,357,358,358,358,358,358,358,358,358,358,358,363,363,363,363,363,363,363,363,363,363,- 1,358,- 1,- 1,357,357,- 1,- 1,358,- 1,- 1,357,363,- 1,- 1,- 1,- 1,358,363,- 1,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,358,- 1,- 1,- 1,- 1,- 1,- 1,358,364,364,- 1,363,- 1,- 1,- 1,364,358,363,- 1,- 1,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,364,364,367,- 1,- 1,- 1,- 1,364,367,368,- 1,368,368,368,368,368,368,368,368,368,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,367,- 1,- 1,- 1,- 1,368,367,- 1,- 1,- 1,- 1,- 1,- 1,369,368,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,369,368,- 1,- 1,- 1,- 1,- 1,369,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,369,- 1,- 1,383,383,383,383,383,383,383,383,383,383,- 1,- 1,- 1,369,- 1,- 1,- 1,- 1,- 1,- 1,369,383,383,- 1,- 1,- 1,- 1,- 1,383,369,372,372,372,372,372,372,372,372,372,372,- 1,- 1,- 1,- 1,- 1,- 1,- 1,372,372,372,372,372,372,383,383,- 1,- 1,- 1,- 1,- 1,383,- 1,- 1,- 1,- 1,- 1,- 1,- 1,373,373,373,373,373,373,373,373,373,373,- 1,372,372,372,372,372,372,373,373,373,373,373,373,- 1,- 1,- 1,392,- 1,373,- 1,- 1,392,- 1,- 1,- 1,- 1,- 1,373,- 1,- 1,392,392,392,392,392,392,392,392,- 1,373,373,373,373,373,373,392,- 1,- 1,- 1,- 1,373,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,373,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,392,- 1,- 1,- 1,- 1,392,392,- 1,- 1,- 1,392,- 1,- 1,- 1,- 1,- 1,- 1,- 1,392,- 1,- 1,- 1,392,- 1,392,- 1,392,- 1,392,405,405,405,405,405,405,405,405,405,405,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,- 1,- 1,- 1,- 1,- 1,- 1,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,405,406,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,406,406,406,406,406,406,406,406,406,406,- 1,- 1,- 1,- 1,- 1,- 1,- 1,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,- 1,- 1,- 1,- 1,- 1,- 1,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,418,- 1,- 1,418,- 1,- 1,- 1,418,418,418,418,418,418,418,418,418,418,- 1,- 1,- 1,- 1,- 1,- 1,- 1,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,- 1,- 1,- 1,- 1,418,- 1,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,418,420,- 1,- 1,- 1,- 1,- 1,- 1,- 1,420,420,420,420,420,420,420,420,420,420,- 1,- 1,- 1,- 1,- 1,- 1,- 1,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,- 1,- 1,- 1,- 1,420,- 1,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 384
int state;int base;int backtrk;
int c;
state=start_state;
# 388
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 392
state=(- state)- 1;}
# 394
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(430,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(430,state)];
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
if(Cyc_lex_check[_check_known_subscript_notnull(3820,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3820,base + c)];else{
# 414
state=Cyc_lex_default[_check_known_subscript_notnull(430,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Lexing_Error_exn_struct _tmpA4;_tmpA4.tag=Cyc_Lexing_Error;_tmpA4.f1=({const char*_tmpA5="empty token";_tag_dyneither(_tmpA5,sizeof(char),12);});_tmpA4;});_tmpA3;}));else{
# 420
return lbuf->lex_last_action;}}else{
# 423
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 427
struct _tuple21*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpA6=lexstate;switch(_tmpA6){case 0: _LL56: _LL57:
# 386 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);_tmpA7;});}
# 391
return({struct _tuple21*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmpA8->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmpA8;});case 1: _LL58: _LL59:
# 394 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL5A: _LL5B:
# 396
 return 0;default: _LL5C: _LL5D:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL55:;}
# 400
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Lexing_Error_exn_struct _tmpAA;_tmpAA.tag=Cyc_Lexing_Error;_tmpAA.f1=({const char*_tmpAB="some action didn't return!";_tag_dyneither(_tmpAB,sizeof(char),27);});_tmpAA;});_tmpA9;}));}
# 402
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpAC=lexstate;switch(_tmpAC){case 0: _LL5F: _LL60:
# 400 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmpAD;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpAE;});
Cyc_token(lexbuf);
return 0;case 1: _LL61: _LL62:
# 408
 Cyc_current_source=({struct _dyneither_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpAF;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpB0;});
Cyc_args(lexbuf);
return 0;case 2: _LL63: _LL64:
# 416
 Cyc_current_source=({struct _dyneither_ptr*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpB1;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpB2;});
Cyc_token(lexbuf);
return 0;default: _LL65: _LL66:
# 422
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL5E:;}
# 425
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Lexing_Error_exn_struct _tmpB4;_tmpB4.tag=Cyc_Lexing_Error;_tmpB4.f1=({const char*_tmpB5="some action didn't return!";_tag_dyneither(_tmpB5,sizeof(char),27);});_tmpB4;});_tmpB3;}));}
# 427
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB6=lexstate;switch(_tmpB6){case 0: _LL68: _LL69: {
# 425 "buildlib.cyl"
struct _dyneither_ptr*_tmpB7=({struct _dyneither_ptr*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmpB9;});
Cyc_current_args=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->hd=_tmpB7;_tmpB8->tl=Cyc_current_args;_tmpB8;});
return Cyc_args(lexbuf);}case 1: _LL6A: _LL6B: {
# 431
struct _dyneither_ptr*_tmpBA=({struct _dyneither_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpBC;});
Cyc_current_args=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=_tmpBA;_tmpBB->tl=Cyc_current_args;_tmpBB;});
return Cyc_args(lexbuf);}case 2: _LL6C: _LL6D: {
# 437
struct _dyneither_ptr*_tmpBD=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpBF;});
Cyc_current_args=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=_tmpBD;_tmpBE->tl=Cyc_current_args;_tmpBE;});
return Cyc_token(lexbuf);}default: _LL6E: _LL6F:
# 442
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL67:;}
# 445
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Lexing_Error_exn_struct _tmpC1;_tmpC1.tag=Cyc_Lexing_Error;_tmpC1.f1=({const char*_tmpC2="some action didn't return!";_tag_dyneither(_tmpC2,sizeof(char),27);});_tmpC1;});_tmpC0;}));}
# 447
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC3=lexstate;switch(_tmpC3){case 0: _LL71: _LL72:
# 446 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpC4;}));return Cyc_token(lexbuf);case 1: _LL73: _LL74:
# 449 "buildlib.cyl"
 return 0;case 2: _LL75: _LL76:
# 452 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL77: _LL78:
# 455 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL79: _LL7A:
# 458 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL7B: _LL7C:
# 460
 return Cyc_token(lexbuf);case 6: _LL7D: _LL7E:
# 462
 return Cyc_token(lexbuf);case 7: _LL7F: _LL80:
# 464
 return Cyc_token(lexbuf);case 8: _LL81: _LL82:
# 467 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL83: _LL84:
# 470 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL85: _LL86:
# 473 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL87: _LL88:
# 475
 return Cyc_token(lexbuf);case 12: _LL89: _LL8A:
# 477
 return Cyc_token(lexbuf);case 13: _LL8B: _LL8C:
# 479
 return Cyc_token(lexbuf);case 14: _LL8D: _LL8E:
# 481
 return Cyc_token(lexbuf);case 15: _LL8F: _LL90:
# 483
 return Cyc_token(lexbuf);case 16: _LL91: _LL92:
# 485
 return Cyc_token(lexbuf);case 17: _LL93: _LL94:
# 487
 return Cyc_token(lexbuf);case 18: _LL95: _LL96:
# 489
 return Cyc_token(lexbuf);case 19: _LL97: _LL98:
# 491
 return Cyc_token(lexbuf);case 20: _LL99: _LL9A:
# 493
 return Cyc_token(lexbuf);case 21: _LL9B: _LL9C:
# 495
 return Cyc_token(lexbuf);case 22: _LL9D: _LL9E:
# 497
 return Cyc_token(lexbuf);case 23: _LL9F: _LLA0:
# 499
 return Cyc_token(lexbuf);case 24: _LLA1: _LLA2:
# 502 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LLA3: _LLA4:
# 504
 return Cyc_token(lexbuf);case 26: _LLA5: _LLA6:
# 506
 return Cyc_token(lexbuf);case 27: _LLA7: _LLA8:
# 508
 return Cyc_token(lexbuf);case 28: _LLA9: _LLAA:
# 510
 return Cyc_token(lexbuf);case 29: _LLAB: _LLAC:
# 512
 return Cyc_token(lexbuf);case 30: _LLAD: _LLAE:
# 514
 return Cyc_token(lexbuf);case 31: _LLAF: _LLB0:
# 516
 return Cyc_token(lexbuf);case 32: _LLB1: _LLB2:
# 518
 return Cyc_token(lexbuf);case 33: _LLB3: _LLB4:
# 520
 return Cyc_token(lexbuf);case 34: _LLB5: _LLB6:
# 522
 return Cyc_token(lexbuf);case 35: _LLB7: _LLB8:
# 524
 return Cyc_token(lexbuf);case 36: _LLB9: _LLBA:
# 526
 return Cyc_token(lexbuf);case 37: _LLBB: _LLBC:
# 528
 return Cyc_token(lexbuf);case 38: _LLBD: _LLBE:
# 530
 return Cyc_token(lexbuf);case 39: _LLBF: _LLC0:
# 532
 return Cyc_token(lexbuf);case 40: _LLC1: _LLC2:
# 534
 return Cyc_token(lexbuf);case 41: _LLC3: _LLC4:
# 536
 return Cyc_token(lexbuf);case 42: _LLC5: _LLC6:
# 538
 return Cyc_token(lexbuf);case 43: _LLC7: _LLC8:
# 540
 return Cyc_token(lexbuf);case 44: _LLC9: _LLCA:
# 542
 return Cyc_token(lexbuf);case 45: _LLCB: _LLCC:
# 544
 return Cyc_token(lexbuf);case 46: _LLCD: _LLCE:
# 546
 return Cyc_token(lexbuf);case 47: _LLCF: _LLD0:
# 548
 return Cyc_token(lexbuf);case 48: _LLD1: _LLD2:
# 551 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LLD3: _LLD4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL70:;}
# 555
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Lexing_Error_exn_struct _tmpC6;_tmpC6.tag=Cyc_Lexing_Error;_tmpC6.f1=({const char*_tmpC7="some action didn't return!";_tag_dyneither(_tmpC7,sizeof(char),27);});_tmpC6;});_tmpC5;}));}
# 557
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC8=lexstate;switch(_tmpC8){case 0: _LLD6: _LLD7:
# 556 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LLD8: _LLD9:
# 557 "buildlib.cyl"
 return 0;case 2: _LLDA: _LLDB:
# 558 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LLDC: _LLDD:
# 559 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LLDE: _LLDF:
# 562 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LLE0: _LLE1:
# 565 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LLE2: _LLE3:
# 567
 return Cyc_string(lexbuf);case 7: _LLE4: _LLE5:
# 568 "buildlib.cyl"
 return 0;case 8: _LLE6: _LLE7:
# 569 "buildlib.cyl"
 return 0;case 9: _LLE8: _LLE9:
# 570 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LLEA: _LLEB:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LLD5:;}
# 574
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Lexing_Error_exn_struct _tmpCA;_tmpCA.tag=Cyc_Lexing_Error;_tmpCA.f1=({const char*_tmpCB="some action didn't return!";_tag_dyneither(_tmpCB,sizeof(char),27);});_tmpCA;});_tmpC9;}));}
# 576
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpCC=lexstate;switch(_tmpCC){case 0: _LLED: _LLEE:
# 579 "buildlib.cyl"
 return 0;case 1: _LLEF: _LLF0:
# 581
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LLF1: _LLF2:
# 588 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCD=0;Cyc_log(({const char*_tmpCE="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpCE,sizeof(char),44);}),_tag_dyneither(_tmpCD,sizeof(void*),0));});
return 1;case 3: _LLF3: _LLF4:
# 594 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCF=0;Cyc_log(({const char*_tmpD0="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpD0,sizeof(char),44);}),_tag_dyneither(_tmpCF,sizeof(void*),0));});
return 1;case 4: _LLF5: _LLF6:
# 600 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD1=0;Cyc_log(({const char*_tmpD2="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpD2,sizeof(char),44);}),_tag_dyneither(_tmpD1,sizeof(void*),0));});
return 1;case 5: _LLF7: _LLF8:
# 606 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD3=0;Cyc_log(({const char*_tmpD4="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpD4,sizeof(char),44);}),_tag_dyneither(_tmpD3,sizeof(void*),0));});
return 1;case 6: _LLF9: _LLFA:
# 610
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD5=0;Cyc_log(({const char*_tmpD6="Warning: use of region sidestepped\n";_tag_dyneither(_tmpD6,sizeof(char),36);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});
return 1;case 7: _LLFB: _LLFC:
# 614
({void*_tmpD7=0;Cyc_log(({const char*_tmpD8="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmpD8,sizeof(char),39);}),_tag_dyneither(_tmpD7,sizeof(void*),0));});
return 1;case 8: _LLFD: _LLFE:
# 619 "buildlib.cyl"
({void*_tmpD9=0;Cyc_log(({const char*_tmpDA="Warning: use of mode HI deleted\n";_tag_dyneither(_tmpDA,sizeof(char),33);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});
return 1;case 9: _LLFF: _LL100:
# 622
({void*_tmpDB=0;Cyc_log(({const char*_tmpDC="Warning: use of mode SI deleted\n";_tag_dyneither(_tmpDC,sizeof(char),33);}),_tag_dyneither(_tmpDB,sizeof(void*),0));});
return 1;case 10: _LL101: _LL102:
# 625
({void*_tmpDD=0;Cyc_log(({const char*_tmpDE="Warning: use of mode QI deleted\n";_tag_dyneither(_tmpDE,sizeof(char),33);}),_tag_dyneither(_tmpDD,sizeof(void*),0));});
return 1;case 11: _LL103: _LL104:
# 628
({void*_tmpDF=0;Cyc_log(({const char*_tmpE0="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpE0,sizeof(char),33);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});
return 1;case 12: _LL105: _LL106:
# 631
({void*_tmpE1=0;Cyc_log(({const char*_tmpE2="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpE2,sizeof(char),33);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});
return 1;case 13: _LL107: _LL108:
# 634
({void*_tmpE3=0;Cyc_log(({const char*_tmpE4="Warning: use of mode word deleted\n";_tag_dyneither(_tmpE4,sizeof(char),35);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});
return 1;case 14: _LL109: _LL10A:
# 637
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LL10B: _LL10C:
# 639
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL10D: _LL10E:
# 641
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17: _LL10F: _LL110:
# 643
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LL111: _LL112:
# 648 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19: _LL113: _LL114:
# 650
 return 1;case 20: _LL115: _LL116:
# 652
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpE5=0;Cyc_log(({const char*_tmpE6="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmpE6,sizeof(char),42);}),_tag_dyneither(_tmpE5,sizeof(void*),0));});
return 1;case 21: _LL117: _LL118:
# 658
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default: _LL119: _LL11A:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LLEC:;}
# 662
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Lexing_Error_exn_struct _tmpE8;_tmpE8.tag=Cyc_Lexing_Error;_tmpE8.f1=({const char*_tmpE9="some action didn't return!";_tag_dyneither(_tmpE9,sizeof(char),27);});_tmpE8;});_tmpE7;}));}
# 664
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpEA=lexstate;switch(_tmpEA){case 0: _LL11C: _LL11D:
# 662 "buildlib.cyl"
 return 0;case 1: _LL11E: _LL11F:
# 664
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL120: _LL121:
# 666
({void*_tmpEB=0;Cyc_log(({const char*_tmpEC="Warning: unclosed string\n";_tag_dyneither(_tmpEC,sizeof(char),26);}),_tag_dyneither(_tmpEB,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpED[1]={& _tmpEF};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmpEE="%s";_tag_dyneither(_tmpEE,sizeof(char),3);}),_tag_dyneither(_tmpED,sizeof(void*),1));});});return 1;case 3: _LL122: _LL123:
# 669
({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpF0[1]={& _tmpF2};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmpF1="%s";_tag_dyneither(_tmpF1,sizeof(char),3);}),_tag_dyneither(_tmpF0,sizeof(void*),1));});});return 1;case 4: _LL124: _LL125:
# 671
({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpF3[1]={& _tmpF5};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmpF4="%s";_tag_dyneither(_tmpF4,sizeof(char),3);}),_tag_dyneither(_tmpF3,sizeof(void*),1));});});return 1;case 5: _LL126: _LL127:
# 673
({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpF6[1]={& _tmpF8};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmpF7="%s";_tag_dyneither(_tmpF7,sizeof(char),3);}),_tag_dyneither(_tmpF6,sizeof(void*),1));});});return 1;case 6: _LL128: _LL129:
# 675
({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpF9[1]={& _tmpFB};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmpFA="%s";_tag_dyneither(_tmpFA,sizeof(char),3);}),_tag_dyneither(_tmpF9,sizeof(void*),1));});});return 1;case 7: _LL12A: _LL12B:
# 677
({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpFC[1]={& _tmpFE};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmpFD="%s";_tag_dyneither(_tmpFD,sizeof(char),3);}),_tag_dyneither(_tmpFC,sizeof(void*),1));});});return 1;case 8: _LL12C: _LL12D:
# 679
({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmpFF[1]={& _tmp101};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp100="%s";_tag_dyneither(_tmp100,sizeof(char),3);}),_tag_dyneither(_tmpFF,sizeof(void*),1));});});return 1;default: _LL12E: _LL12F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL11B:;}
# 683
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Lexing_Error_exn_struct _tmp103;_tmp103.tag=Cyc_Lexing_Error;_tmp103.f1=({const char*_tmp104="some action didn't return!";_tag_dyneither(_tmp104,sizeof(char),27);});_tmp103;});_tmp102;}));}
# 685
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp105=lexstate;switch(_tmp105){case 0: _LL131: _LL132:
# 689 "buildlib.cyl"
 return 0;case 1: _LL133: _LL134:
# 691
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LL135: _LL136:
# 695
 ++ Cyc_parens_to_match;
return 1;case 3: _LL137: _LL138:
# 698
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LL139: _LL13A:
# 701
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LL13B: _LL13C:
# 704
 return 1;case 6: _LL13D: _LL13E:
# 706
 return 1;default: _LL13F: _LL140:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL130:;}
# 710
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Lexing_Error_exn_struct _tmp107;_tmp107.tag=Cyc_Lexing_Error;_tmp107.f1=({const char*_tmp108="some action didn't return!";_tag_dyneither(_tmp108,sizeof(char),27);});_tmp107;});_tmp106;}));}
# 712
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp109=lexstate;switch(_tmp109){case 0: _LL142: _LL143:
# 710 "buildlib.cyl"
({void*_tmp10A=0;Cyc_log(({const char*_tmp10B="Warning: unclosed string\n";_tag_dyneither(_tmp10B,sizeof(char),26);}),_tag_dyneither(_tmp10A,sizeof(void*),0));});return 0;case 1: _LL144: _LL145:
# 712
 return 0;case 2: _LL146: _LL147:
# 714
({void*_tmp10C=0;Cyc_log(({const char*_tmp10D="Warning: unclosed string\n";_tag_dyneither(_tmp10D,sizeof(char),26);}),_tag_dyneither(_tmp10C,sizeof(void*),0));});return 1;case 3: _LL148: _LL149:
# 716
 return 1;case 4: _LL14A: _LL14B:
# 718
 return 1;case 5: _LL14C: _LL14D:
# 720
 return 1;case 6: _LL14E: _LL14F:
# 722
 return 1;case 7: _LL150: _LL151:
# 724
 return 1;case 8: _LL152: _LL153:
# 726
 return 1;default: _LL154: _LL155:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL141:;}
# 730
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Lexing_Error_exn_struct _tmp10F;_tmp10F.tag=Cyc_Lexing_Error;_tmp10F.f1=({const char*_tmp110="some action didn't return!";_tag_dyneither(_tmp110,sizeof(char),27);});_tmp10F;});_tmp10E;}));}
# 732
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp111=lexstate;switch(_tmp111){case 0: _LL157: _LL158:
# 730 "buildlib.cyl"
({void*_tmp112=0;Cyc_log(({const char*_tmp113="Warning: unclosed comment\n";_tag_dyneither(_tmp113,sizeof(char),27);}),_tag_dyneither(_tmp112,sizeof(void*),0));});return 0;case 1: _LL159: _LL15A:
# 732
 return 0;case 2: _LL15B: _LL15C:
# 734
 return 1;default: _LL15D: _LL15E:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL156:;}
# 738
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Lexing_Error_exn_struct _tmp115;_tmp115.tag=Cyc_Lexing_Error;_tmp115.f1=({const char*_tmp116="some action didn't return!";_tag_dyneither(_tmp116,sizeof(char),27);});_tmp115;});_tmp114;}));}
# 740
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple22*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp117=lexstate;switch(_tmp117){case 0: _LL160: _LL161:
# 742 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp118="#define ";_tag_dyneither(_tmp118,sizeof(char),9);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple22*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=Cyc_current_line;_tmp119->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp119;});case 1: _LL162: _LL163:
# 746
 return Cyc_suck_line(lexbuf);case 2: _LL164: _LL165:
# 748
 return 0;default: _LL166: _LL167:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL15F:;}
# 752
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Lexing_Error_exn_struct _tmp11B;_tmp11B.tag=Cyc_Lexing_Error;_tmp11B.f1=({const char*_tmp11C="some action didn't return!";_tag_dyneither(_tmp11C,sizeof(char),27);});_tmp11B;});_tmp11A;}));}
# 754
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp11D=lexstate;if(_tmp11D == 0){_LL169: _LL16A:
# 752 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp11E;});
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL16B: _LL16C:
# 756
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL168:;}
# 759
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Lexing_Error_exn_struct _tmp120;_tmp120.tag=Cyc_Lexing_Error;_tmp120.f1=({const char*_tmp121="some action didn't return!";_tag_dyneither(_tmp121,sizeof(char),27);});_tmp120;});_tmp11F;}));}
# 761
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp122=lexstate;if(_tmp122 == 0){_LL16E: _LL16F:
# 759 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;}else{_LL170: _LL171:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL16D:;}
# 763
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123[0]=({struct Cyc_Lexing_Error_exn_struct _tmp124;_tmp124.tag=Cyc_Lexing_Error;_tmp124.f1=({const char*_tmp125="some action didn't return!";_tag_dyneither(_tmp125,sizeof(char),27);});_tmp124;});_tmp123;}));}
# 765
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple25*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp126=lexstate;switch(_tmp126){case 0: _LL173: _LL174:
# 766 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL175: _LL176:
# 768
 Cyc_current_headerfile=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);
return({struct _tuple25*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->f1=Cyc_current_headerfile;_tmp127->f2=Cyc_current_symbols;_tmp127->f3=Cyc_current_user_defs;_tmp127->f4=Cyc_current_omit_symbols;_tmp127->f5=Cyc_current_hstubs;_tmp127->f6=Cyc_current_cstubs;_tmp127->f7=Cyc_current_cycstubs;_tmp127;});case 2: _LL177: _LL178:
# 790
 return Cyc_spec(lexbuf);case 3: _LL179: _LL17A:
# 792
 return 0;case 4: _LL17B: _LL17C:
# 794
({struct Cyc_Int_pa_PrintArg_struct _tmp12A;_tmp12A.tag=1;_tmp12A.f1=(unsigned long)((int)
# 796
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmp128[1]={& _tmp12A};Cyc_fprintf(Cyc_stderr,({const char*_tmp129="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp129,sizeof(char),67);}),_tag_dyneither(_tmp128,sizeof(void*),1));});});
return 0;default: _LL17D: _LL17E:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL172:;}
# 801
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Lexing_Error_exn_struct _tmp12C;_tmp12C.tag=Cyc_Lexing_Error;_tmp12C.f1=({const char*_tmp12D="some action didn't return!";_tag_dyneither(_tmp12D,sizeof(char),27);});_tmp12C;});_tmp12B;}));}
# 803
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp12E=lexstate;switch(_tmp12E){case 0: _LL180: _LL181:
# 801 "buildlib.cyl"
 return 0;case 1: _LL182: _LL183:
# 803
 return 0;case 2: _LL184: _LL185:
# 805
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL186: _LL187:
# 810
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp12F=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(_tmp12F != Cyc_current_user_defs){
({void*_tmp130=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp131="Error in .cys file: got optional definition in omitsymbols\n";_tag_dyneither(_tmp131,sizeof(char),60);}),_tag_dyneither(_tmp130,sizeof(void*),0));});
# 816
return 0;}
# 818
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;};case 4: _LL188: _LL189:
# 821
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp133->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp133;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=x;_tmp132->tl=Cyc_current_hstubs;_tmp132;});
return 1;};case 5: _LL18A: _LL18B: {
# 829
struct _dyneither_ptr _tmp134=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp134,sizeof(char),(int)Cyc_strlen(({const char*_tmp135="hstub";_tag_dyneither(_tmp135,sizeof(char),6);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp134,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp134,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp134;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp136=Cyc_substring((struct _dyneither_ptr)_tmp134,0,(unsigned long)((t.curr - _tmp134.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->f1=(struct _dyneither_ptr)_tmp136;_tmp138->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp138;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=x;_tmp137->tl=Cyc_current_hstubs;_tmp137;});
return 1;};};};}case 6: _LL18C: _LL18D:
# 843
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp13A->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp13A;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=x;_tmp139->tl=Cyc_current_cstubs;_tmp139;});
return 1;};case 7: _LL18E: _LL18F: {
# 851
struct _dyneither_ptr _tmp13B=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp13B,sizeof(char),(int)Cyc_strlen(({const char*_tmp13C="cstub";_tag_dyneither(_tmp13C,sizeof(char),6);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp13B,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp13B,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp13B;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp13D=Cyc_substring((struct _dyneither_ptr)_tmp13B,0,(unsigned long)((t.curr - _tmp13B.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->f1=(struct _dyneither_ptr)_tmp13D;_tmp13F->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp13F;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=x;_tmp13E->tl=Cyc_current_cstubs;_tmp13E;});
return 1;};};};}case 8: _LL190: _LL191:
# 865
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp141->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp141;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=x;_tmp140->tl=Cyc_current_cycstubs;_tmp140;});
return 1;};case 9: _LL192: _LL193: {
# 873
struct _dyneither_ptr _tmp142=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp142,sizeof(char),(int)Cyc_strlen(({const char*_tmp143="cycstub";_tag_dyneither(_tmp143,sizeof(char),8);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp142,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp142,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp142;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp144=Cyc_substring((struct _dyneither_ptr)_tmp142,0,(unsigned long)((t.curr - _tmp142.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->f1=(struct _dyneither_ptr)_tmp144;_tmp146->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp146;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->hd=x;_tmp145->tl=Cyc_current_cycstubs;_tmp145;});
return 1;};};};}case 10: _LL194: _LL195:
# 887
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp148;});
Cyc_current_cpp=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->hd=x;_tmp147->tl=Cyc_current_cpp;_tmp147;});
return 1;};case 11: _LL196: _LL197:
# 894
 return 1;case 12: _LL198: _LL199:
# 896
 return 1;case 13: _LL19A: _LL19B:
# 898
({struct Cyc_Int_pa_PrintArg_struct _tmp14B;_tmp14B.tag=1;_tmp14B.f1=(unsigned long)((int)
# 900
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmp149[1]={& _tmp14B};Cyc_fprintf(Cyc_stderr,({const char*_tmp14A="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmp14A,sizeof(char),58);}),_tag_dyneither(_tmp149,sizeof(void*),1));});});
return 0;default: _LL19C: _LL19D:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL17F:;}
# 905
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_Lexing_Error_exn_struct _tmp14D;_tmp14D.tag=Cyc_Lexing_Error;_tmp14D.f1=({const char*_tmp14E="some action didn't return!";_tag_dyneither(_tmp14E,sizeof(char),27);});_tmp14D;});_tmp14C;}));}
# 907
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp14F=lexstate;switch(_tmp14F){case 0: _LL19F: _LL1A0:
# 911 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=({struct _dyneither_ptr*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp151;});_tmp150->tl=Cyc_snarfed_symbols;_tmp150;});
return 1;case 1: _LL1A1: _LL1A2: {
# 914
struct _dyneither_ptr _tmp152=Cyc_Lexing_lexeme(lexbuf);
struct _dyneither_ptr t=_tmp152;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp152,0,(unsigned long)((t.curr - _tmp152.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}
# 923
Cyc_rename_current_symbol=0;{
struct _tuple24*user_def=({struct _tuple24*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->f1=({struct _dyneither_ptr*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=Cyc_current_symbol;_tmp158;});_tmp156->f2=({struct _dyneither_ptr*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157[0]=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp157;});_tmp156;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=({struct _dyneither_ptr*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=(struct _dyneither_ptr)Cyc_current_symbol;_tmp154;});_tmp153->tl=Cyc_snarfed_symbols;_tmp153;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=user_def;_tmp155->tl=Cyc_current_user_defs;_tmp155;});
return 1;};}case 2: _LL1A3: _LL1A4:
# 930
 return 1;case 3: _LL1A5: _LL1A6:
# 932
 return 0;case 4: _LL1A7: _LL1A8:
# 934
({void*_tmp159=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp15A="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmp15A,sizeof(char),44);}),_tag_dyneither(_tmp159,sizeof(void*),0));});
# 936
return 0;case 5: _LL1A9: _LL1AA:
# 938
({struct Cyc_Int_pa_PrintArg_struct _tmp15D;_tmp15D.tag=1;_tmp15D.f1=(unsigned long)((int)
# 940
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmp15B[1]={& _tmp15D};Cyc_fprintf(Cyc_stderr,({const char*_tmp15C="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmp15C,sizeof(char),57);}),_tag_dyneither(_tmp15B,sizeof(void*),1));});});
return 0;default: _LL1AB: _LL1AC:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL19E:;}
# 945
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Lexing_Error_exn_struct _tmp15F;_tmp15F.tag=Cyc_Lexing_Error;_tmp15F.f1=({const char*_tmp160="some action didn't return!";_tag_dyneither(_tmp160,sizeof(char),27);});_tmp15F;});_tmp15E;}));}
# 947
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp161=lexstate;switch(_tmp161){case 0: _LL1AE: _LL1AF:
# 951 "buildlib.cyl"
({void*_tmp162=0;Cyc_log(({const char*_tmp163="Warning: unclosed brace\n";_tag_dyneither(_tmp163,sizeof(char),25);}),_tag_dyneither(_tmp162,sizeof(void*),0));});return 0;case 1: _LL1B0: _LL1B1:
# 953
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LL1B2: _LL1B3:
# 958
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LL1B4: _LL1B5:
# 962
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LL1B6: _LL1B7:
# 966
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmp164="/*";_tag_dyneither(_tmp164,sizeof(char),3);}));
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LL1B8: _LL1B9:
# 970
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL1BA: _LL1BB: {
# 973
struct _dyneither_ptr _tmp165=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol  && !Cyc_strcmp((struct _dyneither_ptr)_tmp165,(struct _dyneither_ptr)Cyc_current_symbol))
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),*Cyc_add_user_prefix(({struct _dyneither_ptr*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=_tmp165;_tmp166;})));else{
# 977
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tmp165);}
return 1;}case 7: _LL1BC: _LL1BD:
# 980
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LL1BE: _LL1BF:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL1AD:;}
# 985
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Lexing_Error_exn_struct _tmp168;_tmp168.tag=Cyc_Lexing_Error;_tmp168.f1=({const char*_tmp169="some action didn't return!";_tag_dyneither(_tmp169,sizeof(char),27);});_tmp168;});_tmp167;}));}
# 987
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp16A=lexstate;switch(_tmp16A){case 0: _LL1C1: _LL1C2:
# 985 "buildlib.cyl"
({void*_tmp16B=0;Cyc_log(({const char*_tmp16C="Warning: unclosed string\n";_tag_dyneither(_tmp16C,sizeof(char),26);}),_tag_dyneither(_tmp16B,sizeof(void*),0));});return 0;case 1: _LL1C3: _LL1C4:
# 987
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LL1C5: _LL1C6:
# 989
({void*_tmp16D=0;Cyc_log(({const char*_tmp16E="Warning: unclosed string\n";_tag_dyneither(_tmp16E,sizeof(char),26);}),_tag_dyneither(_tmp16D,sizeof(void*),0));});
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LL1C7: _LL1C8:
# 993
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 4: _LL1C9: _LL1CA:
# 996
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 5: _LL1CB: _LL1CC:
# 999
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL1CD: _LL1CE:
# 1002
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 7: _LL1CF: _LL1D0:
# 1005
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 8: _LL1D1: _LL1D2:
# 1008
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL1D3: _LL1D4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL1C0:;}
# 1013
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F[0]=({struct Cyc_Lexing_Error_exn_struct _tmp170;_tmp170.tag=Cyc_Lexing_Error;_tmp170.f1=({const char*_tmp171="some action didn't return!";_tag_dyneither(_tmp171,sizeof(char),27);});_tmp170;});_tmp16F;}));}
# 1015
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp172=lexstate;switch(_tmp172){case 0: _LL1D6: _LL1D7:
# 1013 "buildlib.cyl"
({void*_tmp173=0;Cyc_log(({const char*_tmp174="Warning: unclosed comment\n";_tag_dyneither(_tmp174,sizeof(char),27);}),_tag_dyneither(_tmp173,sizeof(void*),0));});return 0;case 1: _LL1D8: _LL1D9:
# 1015
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmp175="*/";_tag_dyneither(_tmp175,sizeof(char),3);}));return 0;case 2: _LL1DA: _LL1DB:
# 1017
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL1DC: _LL1DD:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL1D5:;}
# 1022
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Lexing_Error_exn_struct _tmp177;_tmp177.tag=Cyc_Lexing_Error;_tmp177.f1=({const char*_tmp178="some action didn't return!";_tag_dyneither(_tmp178,sizeof(char),27);});_tmp177;});_tmp176;}));}
# 1024
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 1027 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 1030
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmp179=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmp17A=_tmp179;struct Cyc_List_List*_tmp1C6;void*_tmp1C5;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct _dyneither_ptr*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct _dyneither_ptr*_tmp1C0;void*_tmp1BF;void*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BD;int _tmp1BC;struct Cyc_Absyn_Exp*_tmp1BB;void**_tmp1BA;struct Cyc_Absyn_Exp*_tmp1B9;void*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_List_List*_tmp1A3;void*_tmp1A2;switch(*((int*)_tmp17A)){case 1: _LL1DF: _tmp1A2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL1E0: {
# 1034
struct _dyneither_ptr*_tmp17B=(*Cyc_Absyn_binding2qvar(_tmp1A2)).f2;
Cyc_add_target(_tmp17B);
return;}case 2: _LL1E1: _tmp1A3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1E2:
# 1038
 for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1A3->hd,dep);}
# 1041
return;case 22: _LL1E3: _tmp1A5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1A4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1E4:
# 1043
 _tmp1A7=_tmp1A5;_tmp1A6=_tmp1A4;goto _LL1E6;case 8: _LL1E5: _tmp1A7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1A6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1E6:
# 1045
 _tmp1A9=_tmp1A7;_tmp1A8=_tmp1A6;goto _LL1E8;case 3: _LL1E7: _tmp1A9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1A8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17A)->f3;_LL1E8:
# 1047
 Cyc_scan_exp(_tmp1A9,dep);
Cyc_scan_exp(_tmp1A8,dep);
return;case 19: _LL1E9: _tmp1AA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL1EA:
# 1051
 _tmp1AB=_tmp1AA;goto _LL1EC;case 17: _LL1EB: _tmp1AB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL1EC:
# 1053
 _tmp1AC=_tmp1AB;goto _LL1EE;case 14: _LL1ED: _tmp1AC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL1EE:
# 1055
 _tmp1AD=_tmp1AC;goto _LL1F0;case 4: _LL1EF: _tmp1AD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL1F0:
# 1057
 Cyc_scan_exp(_tmp1AD,dep);
return;case 5: _LL1F1: _tmp1B0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_tmp1AE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17A)->f3;_LL1F2:
# 1060
 Cyc_scan_exp(_tmp1B0,dep);
Cyc_scan_exp(_tmp1AF,dep);
Cyc_scan_exp(_tmp1AE,dep);
return;case 6: _LL1F3: _tmp1B2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1B1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1F4:
 _tmp1B4=_tmp1B2;_tmp1B3=_tmp1B1;goto _LL1F6;case 7: _LL1F5: _tmp1B4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1B3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1F6:
# 1066
 Cyc_scan_exp(_tmp1B4,dep);
Cyc_scan_exp(_tmp1B3,dep);
return;case 9: _LL1F7: _tmp1B6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1B5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1F8:
# 1070
 Cyc_scan_exp(_tmp1B6,dep);
for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1B5->hd,dep);}
# 1074
return;case 13: _LL1F9: _tmp1B8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1B7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL1FA:
# 1076
 Cyc_scan_type(_tmp1B8,dep);
Cyc_scan_exp(_tmp1B7,dep);
return;case 33: _LL1FB: _tmp1BC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17A)->f1).is_calloc;_tmp1BB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17A)->f1).rgn;_tmp1BA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17A)->f1).elt_type;_tmp1B9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17A)->f1).num_elts;_LL1FC:
# 1080
 if(_tmp1BB != 0)Cyc_scan_exp(_tmp1BB,dep);
if(_tmp1BA != 0)Cyc_scan_type(*_tmp1BA,dep);
Cyc_scan_exp(_tmp1B9,dep);
return;case 37: _LL1FD: _tmp1BD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL1FE:
# 1085
 Cyc_scan_exp(_tmp1BD,dep);return;case 38: _LL1FF: _tmp1BE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL200:
 _tmp1BF=_tmp1BE;goto _LL202;case 16: _LL201: _tmp1BF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_LL202:
# 1088
 Cyc_scan_type(_tmp1BF,dep);
return;case 20: _LL203: _tmp1C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL204:
# 1091
 _tmp1C3=_tmp1C1;_tmp1C2=_tmp1C0;goto _LL206;case 21: _LL205: _tmp1C3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL206:
# 1093
 Cyc_scan_exp(_tmp1C3,dep);
Cyc_add_target(_tmp1C2);
return;case 18: _LL207: _tmp1C5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17A)->f1;_tmp1C4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL208:
# 1097
 Cyc_scan_type(_tmp1C5,dep);
# 1099
{void*_tmp17C=(void*)((struct Cyc_List_List*)_check_null(_tmp1C4))->hd;void*_tmp17D=_tmp17C;struct _dyneither_ptr*_tmp17E;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp17D)->tag == 0){_LL230: _tmp17E=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp17D)->f1;_LL231:
 Cyc_add_target(_tmp17E);goto _LL22F;}else{_LL232: _LL233:
 goto _LL22F;}_LL22F:;}
# 1103
return;case 0: _LL209: _LL20A:
# 1105
 return;case 35: _LL20B: _tmp1C6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp17A)->f2;_LL20C:
# 1107
 for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
struct _tuple28*_tmp17F=(struct _tuple28*)_tmp1C6->hd;struct _tuple28*_tmp180=_tmp17F;struct Cyc_Absyn_Exp*_tmp181;_LL235: _tmp181=_tmp180->f2;_LL236:;
Cyc_scan_exp(_tmp181,dep);}
# 1111
return;case 39: _LL20D: _LL20E:
 return;case 34: _LL20F: _LL210:
# 1114
({void*_tmp182=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp183="Error: unexpected Swap_e\n";_tag_dyneither(_tmp183,sizeof(char),26);}),_tag_dyneither(_tmp182,sizeof(void*),0));});
 exit(1);return;case 36: _LL211: _LL212:
# 1117
({void*_tmp184=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp185="Error: unexpected Stmt_e\n";_tag_dyneither(_tmp185,sizeof(char),26);}),_tag_dyneither(_tmp184,sizeof(void*),0));});
 exit(1);return;case 10: _LL213: _LL214:
# 1120
({void*_tmp186=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp187="Error: unexpected Throw_e\n";_tag_dyneither(_tmp187,sizeof(char),27);}),_tag_dyneither(_tmp186,sizeof(void*),0));});
 exit(1);return;case 11: _LL215: _LL216:
# 1123
({void*_tmp188=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp189="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp189,sizeof(char),35);}),_tag_dyneither(_tmp188,sizeof(void*),0));});
 exit(1);return;case 12: _LL217: _LL218:
# 1126
({void*_tmp18A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18B="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp18B,sizeof(char),33);}),_tag_dyneither(_tmp18A,sizeof(void*),0));});
 exit(1);return;case 15: _LL219: _LL21A:
# 1129
({void*_tmp18C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18D="Error: unexpected New_e\n";_tag_dyneither(_tmp18D,sizeof(char),25);}),_tag_dyneither(_tmp18C,sizeof(void*),0));});
 exit(1);return;case 23: _LL21B: _LL21C:
# 1132
({void*_tmp18E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18F="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp18F,sizeof(char),27);}),_tag_dyneither(_tmp18E,sizeof(void*),0));});
 exit(1);return;case 24: _LL21D: _LL21E:
# 1135
({void*_tmp190=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp191="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp191,sizeof(char),33);}),_tag_dyneither(_tmp190,sizeof(void*),0));});
 exit(1);return;case 25: _LL21F: _LL220:
# 1138
({void*_tmp192=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp193="Error: unexpected Array_e\n";_tag_dyneither(_tmp193,sizeof(char),27);}),_tag_dyneither(_tmp192,sizeof(void*),0));});
 exit(1);return;case 26: _LL221: _LL222:
# 1141
({void*_tmp194=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp195="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp195,sizeof(char),35);}),_tag_dyneither(_tmp194,sizeof(void*),0));});
 exit(1);return;case 27: _LL223: _LL224:
# 1144
({void*_tmp196=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp197="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmp197,sizeof(char),41);}),_tag_dyneither(_tmp196,sizeof(void*),0));});
 exit(1);return;case 28: _LL225: _LL226:
# 1147
({void*_tmp198=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp199="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp199,sizeof(char),31);}),_tag_dyneither(_tmp198,sizeof(void*),0));});
 exit(1);return;case 29: _LL227: _LL228:
# 1150
({void*_tmp19A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp19B="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp19B,sizeof(char),32);}),_tag_dyneither(_tmp19A,sizeof(void*),0));});
 exit(1);return;case 30: _LL229: _LL22A:
# 1153
({void*_tmp19C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp19D="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp19D,sizeof(char),30);}),_tag_dyneither(_tmp19C,sizeof(void*),0));});
 exit(1);return;case 31: _LL22B: _LL22C:
# 1156
({void*_tmp19E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp19F="Error: unexpected Enum_e\n";_tag_dyneither(_tmp19F,sizeof(char),26);}),_tag_dyneither(_tmp19E,sizeof(void*),0));});
 exit(1);return;default: _LL22D: _LL22E:
# 1159
({void*_tmp1A0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1A1="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp1A1,sizeof(char),30);}),_tag_dyneither(_tmp1A0,sizeof(void*),0));});
 exit(1);return;}_LL1DE:;}
# 1164
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 1169
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp1C7=t;struct Cyc_Absyn_Datatypedecl*_tmp205;struct Cyc_Absyn_Enumdecl*_tmp204;struct Cyc_Absyn_Aggrdecl*_tmp203;struct _dyneither_ptr*_tmp202;struct _dyneither_ptr*_tmp201;union Cyc_Absyn_AggrInfoU _tmp200;struct Cyc_List_List*_tmp1FF;struct Cyc_Absyn_FnInfo _tmp1FE;struct Cyc_Absyn_Exp*_tmp1FD;void*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FB;union Cyc_Absyn_Constraint*_tmp1FA;struct Cyc_Absyn_PtrInfo _tmp1F9;switch(*((int*)_tmp1C7)){case 0: _LL238: _LL239:
 goto _LL23B;case 6: _LL23A: _LL23B:
 goto _LL23D;case 28: _LL23C: _LL23D:
 goto _LL23F;case 7: _LL23E: _LL23F:
# 1176
 return;case 5: _LL240: _tmp1F9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C7)->f1;_LL241:
# 1179
 Cyc_scan_type(_tmp1F9.elt_typ,dep);
return;case 8: _LL242: _tmp1FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C7)->f1).elt_type;_tmp1FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C7)->f1).num_elts;_tmp1FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C7)->f1).zero_term;_LL243:
# 1182
 Cyc_scan_type(_tmp1FC,dep);
Cyc_scan_exp_opt(_tmp1FB,dep);
return;case 27: _LL244: _tmp1FD=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1C7)->f1;_LL245:
# 1186
 Cyc_scan_exp(_tmp1FD,dep);
return;case 9: _LL246: _tmp1FE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C7)->f1;_LL247:
# 1189
 Cyc_scan_type(_tmp1FE.ret_typ,dep);
{struct Cyc_List_List*_tmp1C8=_tmp1FE.args;for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
struct _tuple9*_tmp1C9=(struct _tuple9*)_tmp1C8->hd;struct _tuple9*_tmp1CA=_tmp1C9;void*_tmp1CB;_LL277: _tmp1CB=_tmp1CA->f3;_LL278:;
Cyc_scan_type(_tmp1CB,dep);}}
# 1194
if(_tmp1FE.cyc_varargs != 0)
Cyc_scan_type((_tmp1FE.cyc_varargs)->type,dep);
return;case 12: _LL248: _tmp1FF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C7)->f2;_LL249:
# 1198
 for(0;_tmp1FF != 0;_tmp1FF=_tmp1FF->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp1FF->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp1FF->hd)->width,dep);}
# 1202
return;case 14: _LL24A: _LL24B:
# 1204
 return;case 11: _LL24C: _tmp200=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C7)->f1).aggr_info;_LL24D: {
# 1206
struct _tuple11 _tmp1CC=Cyc_Absyn_aggr_kinded_name(_tmp200);struct _tuple11 _tmp1CD=_tmp1CC;struct _dyneither_ptr*_tmp1CE;_LL27A: _tmp1CE=(_tmp1CD.f2)->f2;_LL27B:;
_tmp201=_tmp1CE;goto _LL24F;}case 13: _LL24E: _tmp201=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1C7)->f1)->f2;_LL24F:
# 1209
 _tmp202=_tmp201;goto _LL251;case 17: _LL250: _tmp202=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1C7)->f1)->f2;_LL251:
# 1211
 Cyc_add_target(_tmp202);
return;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C7)->f1)->r)){case 0: _LL252: _tmp203=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C7)->f1)->r)->f1;_LL253:
# 1215
 Cyc_scan_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D0;_tmp1D0.tag=5;_tmp1D0.f1=_tmp203;_tmp1D0;});_tmp1CF;}),0),dep);{
struct _tuple1*_tmp1D1=_tmp203->name;struct _tuple1*_tmp1D2=_tmp1D1;struct _dyneither_ptr*_tmp1D3;_LL27D: _tmp1D3=_tmp1D2->f2;_LL27E:;
Cyc_add_target(_tmp1D3);
return;};case 1: _LL254: _tmp204=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C7)->f1)->r)->f1;_LL255:
# 1221
 Cyc_scan_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1D5;_tmp1D5.tag=7;_tmp1D5.f1=_tmp204;_tmp1D5;});_tmp1D4;}),0),dep);{
struct _tuple1*_tmp1D6=_tmp204->name;struct _tuple1*_tmp1D7=_tmp1D6;struct _dyneither_ptr*_tmp1D8;_LL280: _tmp1D8=_tmp1D7->f2;_LL281:;
Cyc_add_target(_tmp1D8);
return;};default: _LL256: _tmp205=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C7)->f1)->r)->f1;_LL257:
# 1227
({void*_tmp1D9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1DA="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp1DA,sizeof(char),40);}),_tag_dyneither(_tmp1D9,sizeof(void*),0));});
 exit(1);return;}case 1: _LL258: _LL259:
# 1230
({void*_tmp1DB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1DC="Error: unexpected Evar\n";_tag_dyneither(_tmp1DC,sizeof(char),24);}),_tag_dyneither(_tmp1DB,sizeof(void*),0));});
 exit(1);return;case 2: _LL25A: _LL25B:
# 1233
({void*_tmp1DD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1DE="Error: unexpected VarType\n";_tag_dyneither(_tmp1DE,sizeof(char),27);}),_tag_dyneither(_tmp1DD,sizeof(void*),0));});
 exit(1);return;case 3: _LL25C: _LL25D:
# 1236
({void*_tmp1DF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1E0="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp1E0,sizeof(char),32);}),_tag_dyneither(_tmp1DF,sizeof(void*),0));});
 exit(1);return;case 4: _LL25E: _LL25F:
# 1239
({void*_tmp1E1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1E2="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp1E2,sizeof(char),37);}),_tag_dyneither(_tmp1E1,sizeof(void*),0));});
 exit(1);return;case 10: _LL260: _LL261:
# 1242
({void*_tmp1E3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1E4="Error: unexpected TupleType\n";_tag_dyneither(_tmp1E4,sizeof(char),29);}),_tag_dyneither(_tmp1E3,sizeof(void*),0));});
 exit(1);return;case 15: _LL262: _LL263:
# 1245
({void*_tmp1E5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1E6="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp1E6,sizeof(char),33);}),_tag_dyneither(_tmp1E5,sizeof(void*),0));});
 exit(1);return;case 16: _LL264: _LL265:
# 1248
({void*_tmp1E7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1E8="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp1E8,sizeof(char),30);}),_tag_dyneither(_tmp1E7,sizeof(void*),0));});
 exit(1);return;case 20: _LL266: _LL267:
# 1251
({void*_tmp1E9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1EA="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp1EA,sizeof(char),27);}),_tag_dyneither(_tmp1E9,sizeof(void*),0));});
 exit(1);return;case 21: _LL268: _LL269:
# 1254
({void*_tmp1EB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1EC="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp1EC,sizeof(char),29);}),_tag_dyneither(_tmp1EB,sizeof(void*),0));});
 exit(1);return;case 22: _LL26A: _LL26B:
# 1257
({void*_tmp1ED=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1EE="Error: unexpected RefCntRgn\n";_tag_dyneither(_tmp1EE,sizeof(char),29);}),_tag_dyneither(_tmp1ED,sizeof(void*),0));});
 exit(1);return;case 23: _LL26C: _LL26D:
# 1260
({void*_tmp1EF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F0="Error: unexpected AccessEff\n";_tag_dyneither(_tmp1F0,sizeof(char),29);}),_tag_dyneither(_tmp1EF,sizeof(void*),0));});
 exit(1);return;case 24: _LL26E: _LL26F:
# 1263
({void*_tmp1F1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F2="Error: unexpected JoinEff\n";_tag_dyneither(_tmp1F2,sizeof(char),27);}),_tag_dyneither(_tmp1F1,sizeof(void*),0));});
 exit(1);return;case 25: _LL270: _LL271:
# 1266
({void*_tmp1F3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F4="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp1F4,sizeof(char),27);}),_tag_dyneither(_tmp1F3,sizeof(void*),0));});
 exit(1);return;case 19: _LL272: _LL273:
# 1269
({void*_tmp1F5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F6="Error: unexpected tag_t\n";_tag_dyneither(_tmp1F6,sizeof(char),25);}),_tag_dyneither(_tmp1F5,sizeof(void*),0));});
 exit(1);return;default: _LL274: _LL275:
# 1272
({void*_tmp1F7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F8="Error: unexpected valueof_t\n";_tag_dyneither(_tmp1F8,sizeof(char),29);}),_tag_dyneither(_tmp1F7,sizeof(void*),0));});
 exit(1);return;}_LL237:;}
# 1277
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp206=Cyc_current_targets;
struct _dyneither_ptr*_tmp207=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp208;});
{void*_tmp209=d->r;void*_tmp20A=_tmp209;struct Cyc_Absyn_Typedefdecl*_tmp23E;struct Cyc_Absyn_Enumdecl*_tmp23D;struct Cyc_Absyn_Aggrdecl*_tmp23C;struct Cyc_Absyn_Fndecl*_tmp23B;struct Cyc_Absyn_Vardecl*_tmp23A;switch(*((int*)_tmp20A)){case 0: _LL283: _tmp23A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL284: {
# 1283
struct _tuple1*_tmp20B=_tmp23A->name;struct _tuple1*_tmp20C=_tmp20B;struct _dyneither_ptr*_tmp20D;_LL2A2: _tmp20D=_tmp20C->f2;_LL2A3:;
Cyc_current_source=_tmp20D;
Cyc_scan_type(_tmp23A->type,dep);
Cyc_scan_exp_opt(_tmp23A->initializer,dep);
goto _LL282;}case 1: _LL285: _tmp23B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL286: {
# 1289
struct _tuple1*_tmp20E=_tmp23B->name;struct _tuple1*_tmp20F=_tmp20E;struct _dyneither_ptr*_tmp216;_LL2A5: _tmp216=_tmp20F->f2;_LL2A6:;
Cyc_current_source=_tmp216;
Cyc_scan_type(_tmp23B->ret_type,dep);
{struct Cyc_List_List*_tmp210=_tmp23B->args;for(0;_tmp210 != 0;_tmp210=_tmp210->tl){
struct _tuple9*_tmp211=(struct _tuple9*)_tmp210->hd;struct _tuple9*_tmp212=_tmp211;void*_tmp213;_LL2A8: _tmp213=_tmp212->f3;_LL2A9:;
Cyc_scan_type(_tmp213,dep);}}
# 1296
if(_tmp23B->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp23B->cyc_varargs))->type,dep);
if(_tmp23B->is_inline)
({void*_tmp214=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp215="Warning: ignoring inline function\n";_tag_dyneither(_tmp215,sizeof(char),35);}),_tag_dyneither(_tmp214,sizeof(void*),0));});
# 1301
goto _LL282;}case 5: _LL287: _tmp23C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL288: {
# 1303
struct _tuple1*_tmp217=_tmp23C->name;struct _tuple1*_tmp218=_tmp217;struct _dyneither_ptr*_tmp21C;_LL2AB: _tmp21C=_tmp218->f2;_LL2AC:;
Cyc_current_source=_tmp21C;
if((unsigned int)_tmp23C->impl){
{struct Cyc_List_List*_tmp219=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23C->impl))->fields;for(0;_tmp219 != 0;_tmp219=_tmp219->tl){
struct Cyc_Absyn_Aggrfield*_tmp21A=(struct Cyc_Absyn_Aggrfield*)_tmp219->hd;
Cyc_scan_type(_tmp21A->type,dep);
Cyc_scan_exp_opt(_tmp21A->width,dep);}}{
# 1313
struct Cyc_List_List*_tmp21B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23C->impl))->fields;for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){;}};}
# 1317
goto _LL282;}case 7: _LL289: _tmp23D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL28A: {
# 1319
struct _tuple1*_tmp21D=_tmp23D->name;struct _tuple1*_tmp21E=_tmp21D;struct _dyneither_ptr*_tmp222;_LL2AE: _tmp222=_tmp21E->f2;_LL2AF:;
Cyc_current_source=_tmp222;
if((unsigned int)_tmp23D->fields){
{struct Cyc_List_List*_tmp21F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23D->fields))->v;for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
struct Cyc_Absyn_Enumfield*_tmp220=(struct Cyc_Absyn_Enumfield*)_tmp21F->hd;
Cyc_scan_exp_opt(_tmp220->tag,dep);}}{
# 1328
struct Cyc_List_List*_tmp221=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23D->fields))->v;for(0;_tmp221 != 0;_tmp221=_tmp221->tl){;}};}
# 1332
goto _LL282;}case 8: _LL28B: _tmp23E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL28C: {
# 1334
struct _tuple1*_tmp223=_tmp23E->name;struct _tuple1*_tmp224=_tmp223;struct _dyneither_ptr*_tmp225;_LL2B1: _tmp225=_tmp224->f2;_LL2B2:;
Cyc_current_source=_tmp225;
if((unsigned int)_tmp23E->defn)
Cyc_scan_type((void*)_check_null(_tmp23E->defn),dep);
goto _LL282;}case 4: _LL28D: _LL28E:
# 1340
({void*_tmp226=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp227="Error: unexpected region declaration";_tag_dyneither(_tmp227,sizeof(char),37);}),_tag_dyneither(_tmp226,sizeof(void*),0));});
 exit(1);case 13: _LL28F: _LL290:
# 1343
({void*_tmp228=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp229="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp229,sizeof(char),38);}),_tag_dyneither(_tmp228,sizeof(void*),0));});
 exit(1);case 14: _LL291: _LL292:
# 1346
({void*_tmp22A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp22B="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp22B,sizeof(char),39);}),_tag_dyneither(_tmp22A,sizeof(void*),0));});
 exit(1);case 2: _LL293: _LL294:
# 1349
({void*_tmp22C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp22D="Error: unexpected let declaration\n";_tag_dyneither(_tmp22D,sizeof(char),35);}),_tag_dyneither(_tmp22C,sizeof(void*),0));});
 exit(1);case 6: _LL295: _LL296:
# 1352
({void*_tmp22E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp22F="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp22F,sizeof(char),40);}),_tag_dyneither(_tmp22E,sizeof(void*),0));});
 exit(1);case 3: _LL297: _LL298:
# 1355
({void*_tmp230=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp231="Error: unexpected let declaration\n";_tag_dyneither(_tmp231,sizeof(char),35);}),_tag_dyneither(_tmp230,sizeof(void*),0));});
 exit(1);case 9: _LL299: _LL29A:
# 1358
({void*_tmp232=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp233="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp233,sizeof(char),41);}),_tag_dyneither(_tmp232,sizeof(void*),0));});
 exit(1);case 10: _LL29B: _LL29C:
# 1361
({void*_tmp234=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp235="Error: unexpected using declaration\n";_tag_dyneither(_tmp235,sizeof(char),37);}),_tag_dyneither(_tmp234,sizeof(void*),0));});
 exit(1);case 11: _LL29D: _LL29E:
# 1364
({void*_tmp236=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp237="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp237,sizeof(char),42);}),_tag_dyneither(_tmp236,sizeof(void*),0));});
 exit(1);default: _LL29F: _LL2A0:
# 1367
({void*_tmp238=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp239="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp239,sizeof(char),50);}),_tag_dyneither(_tmp238,sizeof(void*),0));});
 exit(1);}_LL282:;}{
# 1375
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp23F=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp240;_push_handler(& _tmp240);{int _tmp242=0;if(setjmp(_tmp240.handler))_tmp242=1;if(!_tmp242){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp23F);;_pop_handler();}else{void*_tmp241=(void*)_exn_thrown;void*_tmp243=_tmp241;void*_tmp244;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp243)->tag == Cyc_Core_Not_found){_LL2B4: _LL2B5:
# 1380
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL2B3;}else{_LL2B6: _tmp244=_tmp243;_LL2B7:(int)_rethrow(_tmp244);}_LL2B3:;}};}{
# 1382
struct Cyc_Set_Set*_tmp245=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp23F,_tmp245);
# 1385
Cyc_current_targets=_tmp206;
Cyc_current_source=_tmp207;};};}
# 1389
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1393
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp246;_push_handler(& _tmp246);{int _tmp248=0;if(setjmp(_tmp246.handler))_tmp248=1;if(!_tmp248){{struct Cyc_Set_Set*_tmp249=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp249;};_pop_handler();}else{void*_tmp247=(void*)_exn_thrown;void*_tmp24A=_tmp247;void*_tmp24B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp24A)->tag == Cyc_Core_Not_found){_LL2B9: _LL2BA:
# 1396
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL2BB: _tmp24B=_tmp24A;_LL2BC:(int)_rethrow(_tmp24B);}_LL2B8:;}};}
# 1400
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1410 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
# 1415
struct Cyc_Set_Set*_tmp24C=curr;
# 1417
struct _dyneither_ptr*_tmp24D=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp24C)> 0){
struct Cyc_Set_Set*_tmp24E=emptyset;
struct Cyc_Iter_Iter _tmp24F=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp24C);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp24F,& _tmp24D)){
_tmp24E=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp24E,Cyc_find(t,_tmp24D));}
_tmp24C=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp24E,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp24C);}
# 1426
return curr;};}
# 1429
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1435
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1439
if(Cyc_script_file == 0){
({void*_tmp251=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp252="Internal error: script file is NULL\n";_tag_dyneither(_tmp252,sizeof(char),37);}),_tag_dyneither(_tmp251,sizeof(void*),0));});
 exit(1);}
# 1443
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1446
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp256;_tmp256.tag=0;_tmp256.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({struct Cyc_String_pa_PrintArg_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp253[2]={& _tmp255,& _tmp256};Cyc_prscript(({const char*_tmp254="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp254,sizeof(char),36);}),_tag_dyneither(_tmp253,sizeof(void*),2));});});});else{
# 1453
int _tmp257=({unsigned short _tmp25B[0];Cyc_open((const char*)_untag_dyneither_ptr(d,sizeof(char),1),0,_tag_dyneither(_tmp25B,sizeof(unsigned short),0));});
if(_tmp257 == - 1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp25A;_tmp25A.tag=0;_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp258[1]={& _tmp25A};Cyc_fprintf(Cyc_stderr,({const char*_tmp259="Error: could not create directory %s\n";_tag_dyneither(_tmp259,sizeof(char),38);}),_tag_dyneither(_tmp258,sizeof(void*),1));});});
return 1;}}else{
# 1460
 close(_tmp257);}}
# 1462
return 0;}
# 1465
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1469
struct _dyneither_ptr _tmp25C=Cyc_strdup((struct _dyneither_ptr)file);
# 1471
struct Cyc_List_List*_tmp25D=0;
while(1){
_tmp25C=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp25C);
if(Cyc_strlen((struct _dyneither_ptr)_tmp25C)== 0)break;
_tmp25D=({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=({struct _dyneither_ptr*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=(struct _dyneither_ptr)_tmp25C;_tmp25F;});_tmp25E->tl=_tmp25D;_tmp25E;});}
# 1478
for(0;_tmp25D != 0;_tmp25D=_tmp25D->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp25D->hd)))return 1;}
# 1481
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1488
static int Cyc_is_other_special(char c){
char _tmp260=c;switch(_tmp260){case 92: _LL2BE: _LL2BF:
 goto _LL2C1;case 34: _LL2C0: _LL2C1:
 goto _LL2C3;case 59: _LL2C2: _LL2C3:
 goto _LL2C5;case 38: _LL2C4: _LL2C5:
 goto _LL2C7;case 40: _LL2C6: _LL2C7:
 goto _LL2C9;case 41: _LL2C8: _LL2C9:
 goto _LL2CB;case 124: _LL2CA: _LL2CB:
 goto _LL2CD;case 94: _LL2CC: _LL2CD:
 goto _LL2CF;case 60: _LL2CE: _LL2CF:
 goto _LL2D1;case 62: _LL2D0: _LL2D1:
 goto _LL2D3;case 10: _LL2D2: _LL2D3:
# 1503
 goto _LL2D5;case 9: _LL2D4: _LL2D5:
 return 1;default: _LL2D6: _LL2D7:
 return 0;}_LL2BD:;}
# 1509
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp261=Cyc_strlen((struct _dyneither_ptr)s);
# 1513
int _tmp262=0;
int _tmp263=0;
{int i=0;for(0;i < _tmp261;++ i){
char _tmp264=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp264 == '\'')++ _tmp262;else{
if(Cyc_is_other_special(_tmp264))++ _tmp263;}}}
# 1522
if(_tmp262 == 0  && _tmp263 == 0)
return s;
# 1526
if(_tmp262 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp265[3];_tmp265[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);_tmp265[1]=({struct _dyneither_ptr*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=(struct _dyneither_ptr)s;_tmp267;});_tmp265[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp265,sizeof(struct _dyneither_ptr*),3));}));{
# 1530
unsigned long _tmp269=(_tmp261 + _tmp262)+ _tmp263;
struct _dyneither_ptr s2=({unsigned int _tmp273=_tmp269 + 1;char*_tmp274=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp273 + 1));struct _dyneither_ptr _tmp276=_tag_dyneither(_tmp274,sizeof(char),_tmp273 + 1);{unsigned int _tmp275=_tmp273;unsigned int i;for(i=0;i < _tmp275;i ++){_tmp274[i]=(char)'\000';}_tmp274[_tmp275]=(char)0;}_tmp276;});
int _tmp26A=0;
int _tmp26B=0;
for(0;_tmp26A < _tmp261;++ _tmp26A){
char _tmp26C=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp26A));
if(_tmp26C == '\''  || Cyc_is_other_special(_tmp26C))
({struct _dyneither_ptr _tmp26D=_dyneither_ptr_plus(s2,sizeof(char),_tmp26B ++);char _tmp26E=*((char*)_check_dyneither_subscript(_tmp26D,sizeof(char),0));char _tmp26F='\\';if(_get_dyneither_size(_tmp26D,sizeof(char))== 1  && (_tmp26E == '\000'  && _tmp26F != '\000'))_throw_arraybounds();*((char*)_tmp26D.curr)=_tmp26F;});
({struct _dyneither_ptr _tmp270=_dyneither_ptr_plus(s2,sizeof(char),_tmp26B ++);char _tmp271=*((char*)_check_dyneither_subscript(_tmp270,sizeof(char),0));char _tmp272=_tmp26C;if(_get_dyneither_size(_tmp270,sizeof(char))== 1  && (_tmp271 == '\000'  && _tmp272 != '\000'))_throw_arraybounds();*((char*)_tmp270.curr)=_tmp272;});}
# 1540
return(struct _dyneither_ptr)s2;};}
# 1542
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277[0]=Cyc_sh_escape_string(*sp);_tmp277;});}
# 1547
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1554
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1559
({struct Cyc_String_pa_PrintArg_struct _tmp27A;_tmp27A.tag=0;_tmp27A.f1=(struct _dyneither_ptr)({const char*_tmp27B=filename;_tag_dyneither(_tmp27B,sizeof(char),_get_zero_arr_size_char((void*)_tmp27B,1));});({void*_tmp278[1]={& _tmp27A};Cyc_fprintf(Cyc_stderr,({const char*_tmp279="********************************* %s...\n";_tag_dyneither(_tmp279,sizeof(char),41);}),_tag_dyneither(_tmp278,sizeof(void*),1));});});
# 1562
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp27E;_tmp27E.tag=0;_tmp27E.f1=(struct _dyneither_ptr)({const char*_tmp27F=filename;_tag_dyneither(_tmp27F,sizeof(char),_get_zero_arr_size_char((void*)_tmp27F,1));});({void*_tmp27C[1]={& _tmp27E};Cyc_log(({const char*_tmp27D="\n%s:\n";_tag_dyneither(_tmp27D,sizeof(char),6);}),_tag_dyneither(_tmp27C,sizeof(void*),1));});});{
# 1574 "buildlib.cyl"
struct _dyneither_ptr _tmp280=Cyc_Filename_basename(({const char*_tmp3D4=filename;_tag_dyneither(_tmp3D4,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D4,1));}));
struct _dyneither_ptr _tmp281=Cyc_Filename_dirname(({const char*_tmp3D3=filename;_tag_dyneither(_tmp3D3,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D3,1));}));
struct _dyneither_ptr _tmp282=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp280);
const char*_tmp283=(const char*)_check_null(_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp282,({const char*_tmp3D2=".iA";_tag_dyneither(_tmp3D2,sizeof(char),4);})),sizeof(char),1));
# 1579
const char*_tmp284=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp281,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0;_tmp3CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp282);({void*_tmp3CC[1]={& _tmp3CE};Cyc_aprintf(({const char*_tmp3CD="%s.iB";_tag_dyneither(_tmp3CD,sizeof(char),6);}),_tag_dyneither(_tmp3CC,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp281,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0;_tmp3D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp282);({void*_tmp3CF[1]={& _tmp3D1};Cyc_aprintf(({const char*_tmp3D0="%s.iB";_tag_dyneither(_tmp3D0,sizeof(char),6);}),_tag_dyneither(_tmp3CF,sizeof(void*),1));});})),sizeof(char),1));
# 1584
const char*_tmp285=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp281,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0;_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp282);({void*_tmp3C6[1]={& _tmp3C8};Cyc_aprintf(({const char*_tmp3C7="%s.iC";_tag_dyneither(_tmp3C7,sizeof(char),6);}),_tag_dyneither(_tmp3C6,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp281,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0;_tmp3CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp282);({void*_tmp3C9[1]={& _tmp3CB};Cyc_aprintf(({const char*_tmp3CA="%s.iC";_tag_dyneither(_tmp3CA,sizeof(char),6);}),_tag_dyneither(_tmp3C9,sizeof(void*),1));});})),sizeof(char),1));
# 1589
const char*_tmp286=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp281,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0;_tmp3C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp282);({void*_tmp3C0[1]={& _tmp3C2};Cyc_aprintf(({const char*_tmp3C1="%s.iD";_tag_dyneither(_tmp3C1,sizeof(char),6);}),_tag_dyneither(_tmp3C0,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp281,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C5;_tmp3C5.tag=0;_tmp3C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp282);({void*_tmp3C3[1]={& _tmp3C5};Cyc_aprintf(({const char*_tmp3C4="%s.iD";_tag_dyneither(_tmp3C4,sizeof(char),6);}),_tag_dyneither(_tmp3C3,sizeof(void*),1));});})),sizeof(char),1));
# 1594
{struct _handler_cons _tmp287;_push_handler(& _tmp287);{int _tmp289=0;if(setjmp(_tmp287.handler))_tmp289=1;if(!_tmp289){
# 1597
if(Cyc_force_directory_prefixes(({const char*_tmp28A=filename;_tag_dyneither(_tmp28A,sizeof(char),_get_zero_arr_size_char((void*)_tmp28A,1));}))){
int _tmp28B=1;_npop_handler(0);return _tmp28B;}
# 1602
if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp28E;_tmp28E.tag=0;_tmp28E.f1=(struct _dyneither_ptr)({const char*_tmp28F=_tmp283;_tag_dyneither(_tmp28F,sizeof(char),_get_zero_arr_size_char((void*)_tmp28F,1));});({void*_tmp28C[1]={& _tmp28E};Cyc_prscript(({const char*_tmp28D="cat >%s <<XXX\n";_tag_dyneither(_tmp28D,sizeof(char),15);}),_tag_dyneither(_tmp28C,sizeof(void*),1));});});
{struct Cyc_List_List*_tmp290=Cyc_current_cpp;for(0;_tmp290 != 0;_tmp290=_tmp290->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp290->hd));({void*_tmp291[1]={& _tmp293};Cyc_prscript(({const char*_tmp292="%s";_tag_dyneither(_tmp292,sizeof(char),3);}),_tag_dyneither(_tmp291,sizeof(void*),1));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp296;_tmp296.tag=0;_tmp296.f1=(struct _dyneither_ptr)({const char*_tmp297=filename;_tag_dyneither(_tmp297,sizeof(char),_get_zero_arr_size_char((void*)_tmp297,1));});({void*_tmp294[1]={& _tmp296};Cyc_prscript(({const char*_tmp295="#include <%s>\n";_tag_dyneither(_tmp295,sizeof(char),15);}),_tag_dyneither(_tmp294,sizeof(void*),1));});});
({void*_tmp298=0;Cyc_prscript(({const char*_tmp299="XXX\n";_tag_dyneither(_tmp299,sizeof(char),5);}),_tag_dyneither(_tmp298,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct _dyneither_ptr)({const char*_tmp2A0=_tmp283;_tag_dyneither(_tmp2A0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A0,1));});({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0;_tmp29D.f1=(struct _dyneither_ptr)({const char*_tmp29F=_tmp284;_tag_dyneither(_tmp29F,sizeof(char),_get_zero_arr_size_char((void*)_tmp29F,1));});({struct Cyc_String_pa_PrintArg_struct _tmp29C;_tmp29C.tag=0;_tmp29C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp29A[3]={& _tmp29C,& _tmp29D,& _tmp29E};Cyc_prscript(({const char*_tmp29B="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp29B,sizeof(char),35);}),_tag_dyneither(_tmp29A,sizeof(void*),3));});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0;_tmp2A5.f1=(struct _dyneither_ptr)({const char*_tmp2A7=_tmp283;_tag_dyneither(_tmp2A7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A7,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _dyneither_ptr)({const char*_tmp2A6=_tmp285;_tag_dyneither(_tmp2A6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A6,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp2A1[3]={& _tmp2A3,& _tmp2A4,& _tmp2A5};Cyc_prscript(({const char*_tmp2A2="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp2A2,sizeof(char),31);}),_tag_dyneither(_tmp2A1,sizeof(void*),3));});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct _dyneither_ptr)({const char*_tmp2AB=_tmp283;_tag_dyneither(_tmp2AB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AB,1));});({void*_tmp2A8[1]={& _tmp2AA};Cyc_prscript(({const char*_tmp2A9="rm %s\n";_tag_dyneither(_tmp2A9,sizeof(char),7);}),_tag_dyneither(_tmp2A8,sizeof(void*),1));});});}else{
# 1615
maybe=Cyc_fopen(_tmp283,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _dyneither_ptr)({const char*_tmp2AF=_tmp283;_tag_dyneither(_tmp2AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AF,1));});({void*_tmp2AC[1]={& _tmp2AE};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AD="Error: could not create file %s\n";_tag_dyneither(_tmp2AD,sizeof(char),33);}),_tag_dyneither(_tmp2AC,sizeof(void*),1));});});{
int _tmp2B0=1;_npop_handler(0);return _tmp2B0;};}
# 1620
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct _dyneither_ptr)({const char*_tmp2B4=_tmp283;_tag_dyneither(_tmp2B4,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B4,1));});({void*_tmp2B1[1]={& _tmp2B3};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B2="Creating %s\n";_tag_dyneither(_tmp2B2,sizeof(char),13);}),_tag_dyneither(_tmp2B1,sizeof(void*),1));});});
out_file=maybe;
{struct Cyc_List_List*_tmp2B5=Cyc_current_cpp;for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp2B5->hd),sizeof(char),1)),out_file);}}
# 1626
({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct _dyneither_ptr)({const char*_tmp2B9=filename;_tag_dyneither(_tmp2B9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B9,1));});({void*_tmp2B6[1]={& _tmp2B8};Cyc_fprintf(out_file,({const char*_tmp2B7="#include <%s>\n";_tag_dyneither(_tmp2B7,sizeof(char),15);}),_tag_dyneither(_tmp2B6,sizeof(void*),1));});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp2BA=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->hd=({struct _dyneither_ptr*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=(struct _dyneither_ptr)({const char*_tmp2D9="";_tag_dyneither(_tmp2D9,sizeof(char),1);});_tmp2D8;});_tmp2D7->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp2D7;}),({const char*_tmp2DA=" ";_tag_dyneither(_tmp2DA,sizeof(char),2);}));
# 1632
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2D4;_tmp2D4.tag=0;_tmp2D4.f1=(struct _dyneither_ptr)({const char*_tmp2D6=_tmp283;_tag_dyneither(_tmp2D6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D6,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(struct _dyneither_ptr)({const char*_tmp2D5=_tmp284;_tag_dyneither(_tmp2D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D5,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BA);({struct Cyc_String_pa_PrintArg_struct _tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp2CF;_tmp2CF.tag=0;_tmp2CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp2CD[6]={& _tmp2CF,& _tmp2D0,& _tmp2D1,& _tmp2D2,& _tmp2D3,& _tmp2D4};Cyc_aprintf(({const char*_tmp2CE="%s %s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp2CE,sizeof(char),33);}),_tag_dyneither(_tmp2CD,sizeof(void*),6));});});});});});});}),sizeof(char),1));
# 1635
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct _dyneither_ptr)({char*_tmp2BE=cmd;_tag_dyneither(_tmp2BE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BE,1));});({void*_tmp2BB[1]={& _tmp2BD};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BC="%s\n";_tag_dyneither(_tmp2BC,sizeof(char),4);}),_tag_dyneither(_tmp2BB,sizeof(void*),1));});});
if(! system((const char*)cmd)){
# 1640
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dyneither_ptr)({const char*_tmp2C8=_tmp283;_tag_dyneither(_tmp2C8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C8,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct _dyneither_ptr)({const char*_tmp2C7=_tmp285;_tag_dyneither(_tmp2C7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C7,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BA);({struct Cyc_String_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp2C1;_tmp2C1.tag=0;_tmp2C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp2BF[6]={& _tmp2C1,& _tmp2C2,& _tmp2C3,& _tmp2C4,& _tmp2C5,& _tmp2C6};Cyc_aprintf(({const char*_tmp2C0="%s %s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp2C0,sizeof(char),29);}),_tag_dyneither(_tmp2BF,sizeof(void*),6));});});});});});});}),sizeof(char),1));
# 1643
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2CB;_tmp2CB.tag=0;_tmp2CB.f1=(struct _dyneither_ptr)({char*_tmp2CC=cmd;_tag_dyneither(_tmp2CC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2CC,1));});({void*_tmp2C9[1]={& _tmp2CB};Cyc_fprintf(Cyc_stderr,({const char*_tmp2CA="%s\n";_tag_dyneither(_tmp2CA,sizeof(char),4);}),_tag_dyneither(_tmp2C9,sizeof(void*),1));});});
 system((const char*)cmd);}};}}
# 1650
if(Cyc_gathering()){int _tmp2DB=0;_npop_handler(0);return _tmp2DB;}{
# 1653
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp284,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp2DD;_tmp2DD.tag=Cyc_NO_SUPPORT;_tmp2DD.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E0;_tmp2E0.tag=0;_tmp2E0.f1=(struct _dyneither_ptr)({const char*_tmp2E1=_tmp284;_tag_dyneither(_tmp2E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E1,1));});({void*_tmp2DE[1]={& _tmp2E0};Cyc_aprintf(({const char*_tmp2DF="can't open macrosfile %s";_tag_dyneither(_tmp2DF,sizeof(char),25);}),_tag_dyneither(_tmp2DE,sizeof(void*),1));});});_tmp2DD;});_tmp2DC;}));
# 1658
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp2E2=Cyc_Lexing_from_file(in_file);
struct _tuple21*entry;
while((entry=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp2E2))!= 0){
struct _tuple21*_tmp2E3=(struct _tuple21*)_check_null(entry);struct _tuple21*_tmp2E4=_tmp2E3;struct _dyneither_ptr*_tmp2E6;struct Cyc_Set_Set*_tmp2E5;_LL2D9: _tmp2E6=_tmp2E4->f1;_tmp2E5=_tmp2E4->f2;_LL2DA:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp2E6,_tmp2E5);}
# 1667
Cyc_fclose(in_file);
# 1670
maybe=Cyc_fopen(_tmp285,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp2E8;_tmp2E8.tag=Cyc_NO_SUPPORT;_tmp2E8.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EB;_tmp2EB.tag=0;_tmp2EB.f1=(struct _dyneither_ptr)({const char*_tmp2EC=_tmp285;_tag_dyneither(_tmp2EC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EC,1));});({void*_tmp2E9[1]={& _tmp2EB};Cyc_aprintf(({const char*_tmp2EA="can't open declsfile %s";_tag_dyneither(_tmp2EA,sizeof(char),24);}),_tag_dyneither(_tmp2E9,sizeof(void*),1));});});_tmp2E8;});_tmp2E7;}));
# 1674
in_file=maybe;
_tmp2E2=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp286,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp2ED=1;_npop_handler(0);return _tmp2ED;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp2E2)){;}{
# 1680
struct Cyc_List_List*_tmp2EE=user_defs;
while(_tmp2EE != 0){
struct _tuple24*_tmp2EF=(struct _tuple24*)_tmp2EE->hd;struct _tuple24*_tmp2F0=_tmp2EF;struct _dyneither_ptr*_tmp2F4;_LL2DC: _tmp2F4=_tmp2F0->f2;_LL2DD:;
({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0;_tmp2F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F4);({void*_tmp2F1[1]={& _tmp2F3};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp2F2="%s";_tag_dyneither(_tmp2F2,sizeof(char),3);}),_tag_dyneither(_tmp2F1,sizeof(void*),1));});});
_tmp2EE=_tmp2EE->tl;}
# 1686
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1691
maybe=Cyc_fopen(_tmp286,"r");
if(!((unsigned int)maybe)){int _tmp2F5=1;_npop_handler(0);return _tmp2F5;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp2F6=_tmp286;_tag_dyneither(_tmp2F6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F6,1));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp2F7=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1701
{struct Cyc_List_List*_tmp2F8=_tmp2F7;for(0;_tmp2F8 != 0;_tmp2F8=_tmp2F8->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp2F8->hd,t);}}{
# 1705
struct Cyc_List_List*_tmp2F9=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);
struct Cyc_Set_Set*_tmp2FA=Cyc_reachable(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,_tmp2F9),t);
# 1709
struct Cyc_List_List*_tmp2FB=0;
struct Cyc_List_List*_tmp2FC=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp2FD=_tmp2F7;for(0;_tmp2FD != 0;_tmp2FD=_tmp2FD->tl){
struct Cyc_Absyn_Decl*_tmp2FE=(struct Cyc_Absyn_Decl*)_tmp2FD->hd;
struct _dyneither_ptr*name;
{void*_tmp2FF=_tmp2FE->r;void*_tmp300=_tmp2FF;struct Cyc_Absyn_Typedefdecl*_tmp31B;struct Cyc_Absyn_Enumdecl*_tmp31A;struct Cyc_Absyn_Aggrdecl*_tmp319;struct Cyc_Absyn_Fndecl*_tmp318;struct Cyc_Absyn_Vardecl*_tmp317;switch(*((int*)_tmp300)){case 0: _LL2DF: _tmp317=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2E0: {
# 1717
struct _tuple1*_tmp301=_tmp317->name;struct _tuple1*_tmp302=_tmp301;struct _dyneither_ptr*_tmp303;_LL2FE: _tmp303=_tmp302->f2;_LL2FF:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp303);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp303))name=0;else{
name=_tmp303;}
goto _LL2DE;}case 1: _LL2E1: _tmp318=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2E2: {
# 1723
struct _tuple1*_tmp304=_tmp318->name;struct _tuple1*_tmp305=_tmp304;struct _dyneither_ptr*_tmp306;_LL301: _tmp306=_tmp305->f2;_LL302:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp306);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp306))name=0;else{
name=_tmp306;}
goto _LL2DE;}case 5: _LL2E3: _tmp319=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2E4: {
# 1729
struct _tuple1*_tmp307=_tmp319->name;struct _tuple1*_tmp308=_tmp307;struct _dyneither_ptr*_tmp309;_LL304: _tmp309=_tmp308->f2;_LL305:;
name=_tmp309;
goto _LL2DE;}case 7: _LL2E5: _tmp31A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2E6: {
# 1733
struct _tuple1*_tmp30A=_tmp31A->name;struct _tuple1*_tmp30B=_tmp30A;struct _dyneither_ptr*_tmp313;_LL307: _tmp313=_tmp30B->f2;_LL308:;
name=_tmp313;
# 1737
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FA,name))
_tmp2FB=({struct Cyc_List_List*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->hd=_tmp2FE;_tmp30C->tl=_tmp2FB;_tmp30C;});else{
# 1740
if((unsigned int)_tmp31A->fields){
struct Cyc_List_List*_tmp30D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31A->fields))->v;for(0;_tmp30D != 0;_tmp30D=_tmp30D->tl){
struct Cyc_Absyn_Enumfield*_tmp30E=(struct Cyc_Absyn_Enumfield*)_tmp30D->hd;
struct _tuple1*_tmp30F=_tmp30E->name;struct _tuple1*_tmp310=_tmp30F;struct _dyneither_ptr*_tmp312;_LL30A: _tmp312=_tmp310->f2;_LL30B:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FA,_tmp312)){
_tmp2FB=({struct Cyc_List_List*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->hd=_tmp2FE;_tmp311->tl=_tmp2FB;_tmp311;});
break;}}}}
# 1750
name=0;
goto _LL2DE;}case 8: _LL2E7: _tmp31B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2E8: {
# 1753
struct _tuple1*_tmp314=_tmp31B->name;struct _tuple1*_tmp315=_tmp314;struct _dyneither_ptr*_tmp316;_LL30D: _tmp316=_tmp315->f2;_LL30E:;
name=_tmp316;
goto _LL2DE;}case 13: _LL2E9: _LL2EA:
 goto _LL2EC;case 14: _LL2EB: _LL2EC:
 goto _LL2EE;case 2: _LL2ED: _LL2EE:
 goto _LL2F0;case 6: _LL2EF: _LL2F0:
 goto _LL2F2;case 3: _LL2F1: _LL2F2:
 goto _LL2F4;case 9: _LL2F3: _LL2F4:
 goto _LL2F6;case 10: _LL2F5: _LL2F6:
 goto _LL2F8;case 11: _LL2F7: _LL2F8:
 goto _LL2FA;case 12: _LL2F9: _LL2FA:
 goto _LL2FC;default: _LL2FB: _LL2FC:
# 1766
 name=0;
goto _LL2DE;}_LL2DE:;}
# 1770
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FA,name)){
if(Cyc_strncmp((struct _dyneither_ptr)*name,(struct _dyneither_ptr)Cyc_user_prefix,Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix))!= 0)
_tmp2FB=({struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->hd=_tmp2FE;_tmp31C->tl=_tmp2FB;_tmp31C;});else{
# 1775
Cyc_rename_decl(_tmp2FE);
_tmp2FC=({struct Cyc_List_List*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->hd=_tmp2FE;_tmp31D->tl=_tmp2FC;_tmp31D;});}}}}
# 1782
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp31E=1;_npop_handler(0);return _tmp31E;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0;_tmp398.f1=(struct _dyneither_ptr)({const char*_tmp399=filename;_tag_dyneither(_tmp399,sizeof(char),_get_zero_arr_size_char((void*)_tmp399,1));});({void*_tmp396[1]={& _tmp398};Cyc_aprintf(({const char*_tmp397="_%s_";_tag_dyneither(_tmp397,sizeof(char),5);}),_tag_dyneither(_tmp396,sizeof(void*),1));});});
{int _tmp31F=0;for(0;_tmp31F < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp31F){
if(((char*)ifdefmacro.curr)[_tmp31F]== '.'  || ((char*)ifdefmacro.curr)[_tmp31F]== '/')
({struct _dyneither_ptr _tmp320=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp31F);char _tmp321=*((char*)_check_dyneither_subscript(_tmp320,sizeof(char),0));char _tmp322='_';if(_get_dyneither_size(_tmp320,sizeof(char))== 1  && (_tmp321 == '\000'  && _tmp322 != '\000'))_throw_arraybounds();*((char*)_tmp320.curr)=_tmp322;});else{
if(((char*)ifdefmacro.curr)[_tmp31F]!= '_'  && ((char*)ifdefmacro.curr)[_tmp31F]!= '/')
({struct _dyneither_ptr _tmp323=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp31F);char _tmp324=*((char*)_check_dyneither_subscript(_tmp323,sizeof(char),0));char _tmp325=(char) toupper((int)((char*)ifdefmacro.curr)[_tmp31F]);if(_get_dyneither_size(_tmp323,sizeof(char))== 1  && (_tmp324 == '\000'  && _tmp325 != '\000'))_throw_arraybounds();*((char*)_tmp323.curr)=_tmp325;});}}}
# 1794
({struct Cyc_String_pa_PrintArg_struct _tmp329;_tmp329.tag=0;_tmp329.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({struct Cyc_String_pa_PrintArg_struct _tmp328;_tmp328.tag=0;_tmp328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp326[2]={& _tmp328,& _tmp329};Cyc_fprintf(out_file,({const char*_tmp327="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp327,sizeof(char),23);}),_tag_dyneither(_tmp326,sizeof(void*),2));});});});{
# 1801
struct Cyc_List_List*_tmp32A=0;
struct Cyc_List_List*_tmp32B=0;
{struct Cyc_List_List*_tmp32C=_tmp2FB;for(0;_tmp32C != 0;_tmp32C=_tmp32C->tl){
struct Cyc_Absyn_Decl*_tmp32D=(struct Cyc_Absyn_Decl*)_tmp32C->hd;
int _tmp32E=0;
struct _dyneither_ptr*name;
{void*_tmp32F=_tmp32D->r;void*_tmp330=_tmp32F;struct Cyc_Absyn_Typedefdecl*_tmp344;struct Cyc_Absyn_Enumdecl*_tmp343;struct Cyc_Absyn_Aggrdecl*_tmp342;struct Cyc_Absyn_Fndecl*_tmp341;struct Cyc_Absyn_Vardecl*_tmp340;switch(*((int*)_tmp330)){case 0: _LL310: _tmp340=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp330)->f1;_LL311: {
# 1809
struct _tuple1*_tmp331=_tmp340->name;struct _tuple1*_tmp332=_tmp331;struct _dyneither_ptr*_tmp333;_LL32F: _tmp333=_tmp332->f2;_LL330:;
name=_tmp333;
goto _LL30F;}case 1: _LL312: _tmp341=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp330)->f1;_LL313:
# 1813
 if(_tmp341->is_inline){name=0;goto _LL30F;}{
struct _tuple1*_tmp334=_tmp341->name;struct _tuple1*_tmp335=_tmp334;struct _dyneither_ptr*_tmp336;_LL332: _tmp336=_tmp335->f2;_LL333:;
name=_tmp336;
goto _LL30F;};case 5: _LL314: _tmp342=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp330)->f1;_LL315: {
# 1818
struct _tuple1*_tmp337=_tmp342->name;struct _tuple1*_tmp338=_tmp337;struct _dyneither_ptr*_tmp339;_LL335: _tmp339=_tmp338->f2;_LL336:;
name=_tmp339;
goto _LL30F;}case 7: _LL316: _tmp343=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp330)->f1;_LL317: {
# 1822
struct _tuple1*_tmp33A=_tmp343->name;struct _tuple1*_tmp33B=_tmp33A;struct _dyneither_ptr*_tmp33C;_LL338: _tmp33C=_tmp33B->f2;_LL339:;
name=_tmp33C;
goto _LL30F;}case 8: _LL318: _tmp344=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp330)->f1;_LL319: {
# 1826
struct _tuple1*_tmp33D=_tmp344->name;struct _tuple1*_tmp33E=_tmp33D;struct _dyneither_ptr*_tmp33F;_LL33B: _tmp33F=_tmp33E->f2;_LL33C:;
name=_tmp33F;
goto _LL30F;}case 4: _LL31A: _LL31B:
 goto _LL31D;case 13: _LL31C: _LL31D:
 goto _LL31F;case 14: _LL31E: _LL31F:
 goto _LL321;case 2: _LL320: _LL321:
 goto _LL323;case 6: _LL322: _LL323:
 goto _LL325;case 3: _LL324: _LL325:
 goto _LL327;case 9: _LL326: _LL327:
 goto _LL329;case 10: _LL328: _LL329:
 goto _LL32B;case 11: _LL32A: _LL32B:
 goto _LL32D;default: _LL32C: _LL32D:
# 1839
 name=0;
goto _LL30F;}_LL30F:;}
# 1842
if(!((unsigned int)name) && !_tmp32E)continue;
# 1847
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FA,Cyc_add_user_prefix(name))){
struct Cyc_Absyn_Decl*_tmp345=Cyc_named_decl(_tmp2FC,name);
Cyc_merge_rep_decl(_tmp345,_tmp32D);
_tmp32A=({struct Cyc_List_List*_tmp346=_cycalloc(sizeof(*_tmp346));_tmp346->hd=_tmp32D;_tmp346->tl=_tmp32A;_tmp346;});}else{
# 1853
_tmp32A=({struct Cyc_List_List*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->hd=_tmp32D;_tmp347->tl=_tmp32A;_tmp347;});}
_tmp32B=({struct Cyc_List_List*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->hd=name;_tmp348->tl=_tmp32B;_tmp348;});}}
# 1858
{struct _handler_cons _tmp349;_push_handler(& _tmp349);{int _tmp34B=0;if(setjmp(_tmp349.handler))_tmp34B=1;if(!_tmp34B){
Cyc_Binding_resolve_all(_tmp32A);
Cyc_Tc_tc(Cyc_Core_heap_region,Cyc_Tcenv_tc_init(),1,_tmp32A);
# 1859
;_pop_handler();}else{void*_tmp34A=(void*)_exn_thrown;void*_tmp34C=_tmp34A;_LL33E: _LL33F:
# 1862
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp34E;_tmp34E.tag=Cyc_NO_SUPPORT;_tmp34E.f1=({const char*_tmp34F="can't typecheck acquired declarations";_tag_dyneither(_tmp34F,sizeof(char),38);});_tmp34E;});_tmp34D;}));_LL33D:;}};}
# 1866
{struct _tuple0 _tmp350=({struct _tuple0 _tmp364;_tmp364.f1=_tmp32A;_tmp364.f2=_tmp32B;_tmp364;});struct _tuple0 _tmp351=_tmp350;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp362;_LL343: _tmp363=_tmp351.f1;_tmp362=_tmp351.f2;_LL344:;for(0;
_tmp363 != 0  && _tmp362 != 0;(_tmp363=_tmp363->tl,_tmp362=_tmp362->tl)){
struct Cyc_Absyn_Decl*_tmp352=(struct Cyc_Absyn_Decl*)_tmp363->hd;
struct _dyneither_ptr*_tmp353=(struct _dyneither_ptr*)_tmp362->hd;
int _tmp354=0;
if(!((unsigned int)_tmp353))
_tmp354=1;
# 1875
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp353){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp357;_tmp357.tag=0;_tmp357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp353);({void*_tmp355[1]={& _tmp357};Cyc_aprintf(({const char*_tmp356="_%s_def_";_tag_dyneither(_tmp356,sizeof(char),9);}),_tag_dyneither(_tmp355,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp35A;_tmp35A.tag=0;_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp358[1]={& _tmp35A};Cyc_fprintf(out_file,({const char*_tmp359="#ifndef %s\n";_tag_dyneither(_tmp359,sizeof(char),12);}),_tag_dyneither(_tmp358,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp35D;_tmp35D.tag=0;_tmp35D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp35B[1]={& _tmp35D};Cyc_fprintf(out_file,({const char*_tmp35C="#define %s\n";_tag_dyneither(_tmp35C,sizeof(char),12);}),_tag_dyneither(_tmp35B,sizeof(void*),1));});});
# 1881
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp35E[1];_tmp35E[0]=_tmp352;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35E,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);
({void*_tmp35F=0;Cyc_fprintf(out_file,({const char*_tmp360="#endif\n";_tag_dyneither(_tmp360,sizeof(char),8);}),_tag_dyneither(_tmp35F,sizeof(void*),0));});}else{
# 1886
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp361[1];_tmp361[0]=_tmp352;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp361,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}
# 1891
maybe=Cyc_fopen(_tmp284,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp366;_tmp366.tag=Cyc_NO_SUPPORT;_tmp366.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp369;_tmp369.tag=0;_tmp369.f1=(struct _dyneither_ptr)({const char*_tmp36A=_tmp284;_tag_dyneither(_tmp36A,sizeof(char),_get_zero_arr_size_char((void*)_tmp36A,1));});({void*_tmp367[1]={& _tmp369};Cyc_aprintf(({const char*_tmp368="can't open macrosfile %s";_tag_dyneither(_tmp368,sizeof(char),25);}),_tag_dyneither(_tmp367,sizeof(void*),1));});});_tmp366;});_tmp365;}));
# 1894
in_file=maybe;
_tmp2E2=Cyc_Lexing_from_file(in_file);{
struct _tuple22*entry2;
while((entry2=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp2E2))!= 0){
struct _tuple22*_tmp36B=(struct _tuple22*)_check_null(entry2);struct _tuple22*_tmp36C=_tmp36B;struct _dyneither_ptr _tmp376;struct _dyneither_ptr*_tmp375;_LL346: _tmp376=_tmp36C->f1;_tmp375=_tmp36C->f2;_LL347:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FA,_tmp375)){
({struct Cyc_String_pa_PrintArg_struct _tmp36F;_tmp36F.tag=0;_tmp36F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp375);({void*_tmp36D[1]={& _tmp36F};Cyc_fprintf(out_file,({const char*_tmp36E="#ifndef %s\n";_tag_dyneither(_tmp36E,sizeof(char),12);}),_tag_dyneither(_tmp36D,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp372;_tmp372.tag=0;_tmp372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp376);({void*_tmp370[1]={& _tmp372};Cyc_fprintf(out_file,({const char*_tmp371="%s\n";_tag_dyneither(_tmp371,sizeof(char),4);}),_tag_dyneither(_tmp370,sizeof(void*),1));});});
({void*_tmp373=0;Cyc_fprintf(out_file,({const char*_tmp374="#endif\n";_tag_dyneither(_tmp374,sizeof(char),8);}),_tag_dyneither(_tmp373,sizeof(void*),0));});}}
# 1905
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1908
if(hstubs != 0){
struct Cyc_List_List*_tmp377=hstubs;for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct _tuple23*_tmp378=(struct _tuple23*)_tmp377->hd;struct _tuple23*_tmp379=_tmp378;struct _dyneither_ptr _tmp37F;struct _dyneither_ptr _tmp37E;_LL349: _tmp37F=_tmp379->f1;_tmp37E=_tmp379->f2;_LL34A:;
if((char*)_tmp37E.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp37F.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A[0]=_tmp37F;_tmp37A;}))))
# 1914
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp37E,sizeof(char),1)),out_file);else{
# 1916
({struct Cyc_String_pa_PrintArg_struct _tmp37D;_tmp37D.tag=0;_tmp37D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37F);({void*_tmp37B[1]={& _tmp37D};Cyc_log(({const char*_tmp37C="%s is not supported on this platform\n";_tag_dyneither(_tmp37C,sizeof(char),38);}),_tag_dyneither(_tmp37B,sizeof(void*),1));});});}}}
# 1919
({void*_tmp380=0;Cyc_fprintf(out_file,({const char*_tmp381="#endif\n";_tag_dyneither(_tmp381,sizeof(char),8);}),_tag_dyneither(_tmp380,sizeof(void*),0));});
if(Cyc_do_setjmp){int _tmp382=0;_npop_handler(0);return _tmp382;}else{
Cyc_fclose(out_file);}
# 1924
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp383=cstubs;for(0;_tmp383 != 0;_tmp383=_tmp383->tl){
struct _tuple23*_tmp384=(struct _tuple23*)_tmp383->hd;struct _tuple23*_tmp385=_tmp384;struct _dyneither_ptr _tmp388;struct _dyneither_ptr _tmp387;_LL34C: _tmp388=_tmp385->f1;_tmp387=_tmp385->f2;_LL34D:;
if((char*)_tmp387.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp388.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=_tmp388;_tmp386;}))))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp387,sizeof(char),1)),out_file);}};}
# 1935
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1937
({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0;_tmp38B.f1=(struct _dyneither_ptr)({const char*_tmp38C=filename;_tag_dyneither(_tmp38C,sizeof(char),_get_zero_arr_size_char((void*)_tmp38C,1));});({void*_tmp389[1]={& _tmp38B};Cyc_fprintf(out_file,({const char*_tmp38A="#include <%s>\n\n";_tag_dyneither(_tmp38A,sizeof(char),16);}),_tag_dyneither(_tmp389,sizeof(void*),1));});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp38D=cycstubs;for(0;_tmp38D != 0;_tmp38D=_tmp38D->tl){
struct _tuple23*_tmp38E=(struct _tuple23*)_tmp38D->hd;struct _tuple23*_tmp38F=_tmp38E;struct _dyneither_ptr _tmp392;struct _dyneither_ptr _tmp391;_LL34F: _tmp392=_tmp38F->f1;_tmp391=_tmp38F->f2;_LL350:;
if((char*)_tmp391.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp392.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390[0]=_tmp392;_tmp390;}))))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp391,sizeof(char),1)),out_file);}}
# 1946
({void*_tmp393=0;Cyc_fprintf(out_file,({const char*_tmp394="\n";_tag_dyneither(_tmp394,sizeof(char),2);}),_tag_dyneither(_tmp393,sizeof(void*),0));});}{
# 1949
int _tmp395=0;_npop_handler(0);return _tmp395;};};};};};};};};};
# 1597
;_pop_handler();}else{void*_tmp288=(void*)_exn_thrown;void*_tmp39A=_tmp288;void*_tmp3B1;struct _dyneither_ptr _tmp3B0;struct _dyneither_ptr _tmp3AF;struct _dyneither_ptr _tmp3AE;struct _dyneither_ptr _tmp3AD;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp39A)->tag == Cyc_Core_Impossible){_LL352: _tmp3AD=((struct Cyc_Core_Impossible_exn_struct*)_tmp39A)->f1;_LL353:
# 1953
({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0;_tmp39D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AD);({void*_tmp39B[1]={& _tmp39D};Cyc_fprintf(Cyc_stderr,({const char*_tmp39C="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp39C,sizeof(char),26);}),_tag_dyneither(_tmp39B,sizeof(void*),1));});});goto _LL351;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp39A)->tag == Cyc_Dict_Absent){_LL354: _LL355:
# 1955
({void*_tmp39E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp39F="Got Dict::Absent\n";_tag_dyneither(_tmp39F,sizeof(char),18);}),_tag_dyneither(_tmp39E,sizeof(void*),0));});goto _LL351;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp39A)->tag == Cyc_Core_Failure){_LL356: _tmp3AE=((struct Cyc_Core_Failure_exn_struct*)_tmp39A)->f1;_LL357:
# 1957
({struct Cyc_String_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=0;_tmp3A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AE);({void*_tmp3A0[1]={& _tmp3A2};Cyc_fprintf(Cyc_stderr,({const char*_tmp3A1="Got Core::Failure(%s)\n";_tag_dyneither(_tmp3A1,sizeof(char),23);}),_tag_dyneither(_tmp3A0,sizeof(void*),1));});});goto _LL351;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39A)->tag == Cyc_Core_Invalid_argument){_LL358: _tmp3AF=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39A)->f1;_LL359:
# 1959
({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0;_tmp3A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AF);({void*_tmp3A3[1]={& _tmp3A5};Cyc_fprintf(Cyc_stderr,({const char*_tmp3A4="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp3A4,sizeof(char),26);}),_tag_dyneither(_tmp3A3,sizeof(void*),1));});});goto _LL351;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp39A)->tag == Cyc_Core_Not_found){_LL35A: _LL35B:
# 1961
({void*_tmp3A6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3A7="Got Not_found\n";_tag_dyneither(_tmp3A7,sizeof(char),15);}),_tag_dyneither(_tmp3A6,sizeof(void*),0));});goto _LL351;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp39A)->tag == Cyc_NO_SUPPORT){_LL35C: _tmp3B0=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp39A)->f1;_LL35D:
# 1963
({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B0);({void*_tmp3A8[1]={& _tmp3AA};Cyc_fprintf(Cyc_stderr,({const char*_tmp3A9="No support because %s\n";_tag_dyneither(_tmp3A9,sizeof(char),23);}),_tag_dyneither(_tmp3A8,sizeof(void*),1));});});goto _LL351;}else{_LL35E: _tmp3B1=_tmp39A;_LL35F:
# 1965
({void*_tmp3AB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3AC="Got unknown exception\n";_tag_dyneither(_tmp3AC,sizeof(char),23);}),_tag_dyneither(_tmp3AB,sizeof(void*),0));});
Cyc_Core_rethrow(_tmp3B1);}}}}}}_LL351:;}};}
# 1971
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0;_tmp3B4.f1=(struct _dyneither_ptr)({const char*_tmp3B5=filename;_tag_dyneither(_tmp3B5,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B5,1));});({void*_tmp3B2[1]={& _tmp3B4};Cyc_fprintf(Cyc_stderr,({const char*_tmp3B3="Error: could not create file %s\n";_tag_dyneither(_tmp3B3,sizeof(char),33);}),_tag_dyneither(_tmp3B2,sizeof(void*),1));});});
return 1;}
# 1976
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0;_tmp3B8.f1=(struct _dyneither_ptr)({const char*_tmp3B9=filename;_tag_dyneither(_tmp3B9,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B9,1));});({void*_tmp3B6[1]={& _tmp3B8};Cyc_fprintf(out_file,({const char*_tmp3B7="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp3B7,sizeof(char),48);}),_tag_dyneither(_tmp3B6,sizeof(void*),1));});});
# 1980
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0;_tmp3BC.f1=(struct _dyneither_ptr)({const char*_tmp3BD=filename;_tag_dyneither(_tmp3BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp3BD,1));});({void*_tmp3BA[1]={& _tmp3BC};Cyc_fprintf(Cyc_stderr,({const char*_tmp3BB="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp3BB,sizeof(char),52);}),_tag_dyneither(_tmp3BA,sizeof(void*),1));});});
# 1983
({void*_tmp3BE=0;Cyc_log(({const char*_tmp3BF="Not supported on this platform\n";_tag_dyneither(_tmp3BF,sizeof(char),32);}),_tag_dyneither(_tmp3BE,sizeof(void*),0));});
# 1990
return 0;};}
# 1994
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0;_tmp3D7.f1=(struct _dyneither_ptr)({const char*_tmp3D8=file;_tag_dyneither(_tmp3D8,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D8,1));});({void*_tmp3D5[1]={& _tmp3D7};Cyc_fprintf(Cyc_stderr,({const char*_tmp3D6="Processing %s\n";_tag_dyneither(_tmp3D6,sizeof(char),15);}),_tag_dyneither(_tmp3D5,sizeof(void*),1));});});{
struct Cyc___cycFILE*_tmp3D9=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp3D9)){
({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0;_tmp3DC.f1=(struct _dyneither_ptr)({const char*_tmp3DD=file;_tag_dyneither(_tmp3DD,sizeof(char),_get_zero_arr_size_char((void*)_tmp3DD,1));});({void*_tmp3DA[1]={& _tmp3DC};Cyc_fprintf(Cyc_stderr,({const char*_tmp3DB="Error: could not open %s\n";_tag_dyneither(_tmp3DB,sizeof(char),26);}),_tag_dyneither(_tmp3DA,sizeof(void*),1));});});
return 1;}{
# 2002
struct Cyc___cycFILE*_tmp3DE=_tmp3D9;
# 2006
struct _dyneither_ptr buf=({char*_tmp3FD=({unsigned int _tmp3FA=(unsigned int)1024;char*_tmp3FB=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp3FA + 1));{unsigned int _tmp3FC=_tmp3FA;unsigned int i;for(i=0;i < _tmp3FC;i ++){_tmp3FB[i]=(char)'\000';}_tmp3FB[_tmp3FC]=(char)0;}_tmp3FB;});_tag_dyneither(_tmp3FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp3FD,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp3DF=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0;_tmp3E2.f1=(struct _dyneither_ptr)({const char*_tmp3E3=dir;_tag_dyneither(_tmp3E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp3E3,1));});({void*_tmp3E0[1]={& _tmp3E2};Cyc_fprintf(Cyc_stderr,({const char*_tmp3E1="Error: can't change directory to %s\n";_tag_dyneither(_tmp3E1,sizeof(char),37);}),_tag_dyneither(_tmp3E0,sizeof(void*),1));});});
return 1;}}
# 2014
if(Cyc_mode == Cyc_GATHER){
# 2016
struct _dyneither_ptr _tmp3E4=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0;_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0;_tmp3EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0;_tmp3EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp3E8[3]={& _tmp3EA,& _tmp3EB,& _tmp3EC};Cyc_aprintf(({const char*_tmp3E9="echo | %s %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp3E9,sizeof(char),42);}),_tag_dyneither(_tmp3E8,sizeof(void*),3));});});});});
# 2018
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E4);({void*_tmp3E5[1]={& _tmp3E7};Cyc_fprintf(Cyc_stderr,({const char*_tmp3E6="%s\n";_tag_dyneither(_tmp3E6,sizeof(char),4);}),_tag_dyneither(_tmp3E5,sizeof(void*),1));});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp3E4,sizeof(char),1)));}{
# 2023
struct Cyc_Lexing_lexbuf*_tmp3ED=Cyc_Lexing_from_file(_tmp3DE);
struct _tuple25*entry;
while((entry=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp3ED))!= 0){
struct _tuple25*_tmp3EE=(struct _tuple25*)_check_null(entry);struct _tuple25*_tmp3EF=_tmp3EE;struct _dyneither_ptr _tmp3F6;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F4;struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*_tmp3F2;struct Cyc_List_List*_tmp3F1;struct Cyc_List_List*_tmp3F0;_LL363: _tmp3F6=_tmp3EF->f1;_tmp3F5=_tmp3EF->f2;_tmp3F4=_tmp3EF->f3;_tmp3F3=_tmp3EF->f4;_tmp3F2=_tmp3EF->f5;_tmp3F1=_tmp3EF->f6;_tmp3F0=_tmp3EF->f7;_LL364:;
# 2028
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp3F6,sizeof(char),1)),_tmp3F5,_tmp3F4,_tmp3F3,_tmp3F2,_tmp3F1,_tmp3F0))
# 2030
return 1;}
# 2032
Cyc_fclose(_tmp3DE);
# 2034
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3DF,sizeof(char),1))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3F9;_tmp3F9.tag=0;_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3DF);({void*_tmp3F7[1]={& _tmp3F9};Cyc_fprintf(Cyc_stderr,({const char*_tmp3F8="Error: could not change directory to %s\n";_tag_dyneither(_tmp3F8,sizeof(char),41);}),_tag_dyneither(_tmp3F7,sizeof(void*),1));});});
return 1;}}
# 2040
return 0;};};};}
# 2044
int Cyc_process_setjmp(const char*dir){
# 2047
struct _dyneither_ptr buf=({char*_tmp40F=({unsigned int _tmp40C=(unsigned int)1024;char*_tmp40D=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp40C + 1));{unsigned int _tmp40E=_tmp40C;unsigned int i;for(i=0;i < _tmp40E;i ++){_tmp40D[i]=(char)'\000';}_tmp40D[_tmp40E]=(char)0;}_tmp40D;});_tag_dyneither(_tmp40F,sizeof(char),_get_zero_arr_size_char((void*)_tmp40F,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp3FE=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp401;_tmp401.tag=0;_tmp401.f1=(struct _dyneither_ptr)({const char*_tmp402=dir;_tag_dyneither(_tmp402,sizeof(char),_get_zero_arr_size_char((void*)_tmp402,1));});({void*_tmp3FF[1]={& _tmp401};Cyc_fprintf(Cyc_stderr,({const char*_tmp400="Error: can't change directory to %s\n";_tag_dyneither(_tmp400,sizeof(char),37);}),_tag_dyneither(_tmp3FF,sizeof(void*),1));});});
return 1;}
# 2053
if(Cyc_process_file("setjmp.h",({struct _dyneither_ptr*_tmp403[1];_tmp403[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp403,sizeof(struct _dyneither_ptr*),1));}),0,0,({struct _tuple23*_tmp405[1];_tmp405[0]=({struct _tuple23*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->f1=({const char*_tmp408="setjmp";_tag_dyneither(_tmp408,sizeof(char),7);});_tmp406->f2=({const char*_tmp407="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp407,sizeof(char),29);});_tmp406;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp405,sizeof(struct _tuple23*),1));}),0,0))
# 2056
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3FE,sizeof(char),1))))){
({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0;_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3FE);({void*_tmp409[1]={& _tmp40B};Cyc_fprintf(Cyc_stderr,({const char*_tmp40A="Error: could not change directory to %s\n";_tag_dyneither(_tmp40A,sizeof(char),41);}),_tag_dyneither(_tmp409,sizeof(void*),1));});});
return 1;}
# 2061
return 0;}static char _tmp410[13]="BUILDLIB.OUT";
# 2065
static struct _dyneither_ptr Cyc_output_dir={_tmp410,_tmp410,_tmp410 + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 2069
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1));_tmp411->tl=Cyc_spec_files;_tmp411;});}
# 2073
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 2077
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 2080
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 2083
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->hd=({struct _dyneither_ptr*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413[0]=s;_tmp413;});_tmp412->tl=Cyc_cppargs;_tmp412;});}
# 2086
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0;_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp414[1]={& _tmp416};Cyc_fprintf(Cyc_stderr,({const char*_tmp415="Unsupported option %s\n";_tag_dyneither(_tmp415,sizeof(char),23);}),_tag_dyneither(_tmp414,sizeof(void*),1));});});
Cyc_badparse=1;}
# 2095
void GC_blacklist_warn_clear();struct _tuple29{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 2099
struct Cyc_List_List*options=({struct _tuple29*_tmp44E[9];_tmp44E[8]=({struct _tuple29*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->f1=({const char*_tmp484="-";_tag_dyneither(_tmp484,sizeof(char),2);});_tmp47F->f2=1;_tmp47F->f3=({const char*_tmp483="";_tag_dyneither(_tmp483,sizeof(char),1);});_tmp47F->f4=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp482;_tmp482.tag=1;_tmp482.f1=Cyc_add_cpparg;_tmp482;});_tmp481;});_tmp47F->f5=({const char*_tmp480="";_tag_dyneither(_tmp480,sizeof(char),1);});_tmp47F;});_tmp44E[7]=({struct _tuple29*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->f1=({const char*_tmp47E="-v";_tag_dyneither(_tmp47E,sizeof(char),3);});_tmp479->f2=0;_tmp479->f3=({const char*_tmp47D="";_tag_dyneither(_tmp47D,sizeof(char),1);});_tmp479->f4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp47C;_tmp47C.tag=3;_tmp47C.f1=& Cyc_verbose;_tmp47C;});_tmp47B;});_tmp479->f5=({const char*_tmp47A="Verbose operation";_tag_dyneither(_tmp47A,sizeof(char),18);});_tmp479;});_tmp44E[6]=({struct _tuple29*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->f1=({const char*_tmp478="-B";_tag_dyneither(_tmp478,sizeof(char),3);});_tmp473->f2=1;_tmp473->f3=({const char*_tmp477="<file>";_tag_dyneither(_tmp477,sizeof(char),7);});_tmp473->f4=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp476;_tmp476.tag=1;_tmp476.f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp476;});_tmp475;});_tmp473->f5=({const char*_tmp474="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp474,sizeof(char),60);});_tmp473;});_tmp44E[5]=({struct _tuple29*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->f1=({const char*_tmp472="-b";_tag_dyneither(_tmp472,sizeof(char),3);});_tmp46D->f2=0;_tmp46D->f3=({const char*_tmp471=" <machine>";_tag_dyneither(_tmp471,sizeof(char),11);});_tmp46D->f4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp470;_tmp470.tag=5;_tmp470.f1=Cyc_Specsfile_set_target_arch;_tmp470;});_tmp46F;});_tmp46D->f5=({const char*_tmp46E="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp46E,sizeof(char),52);});_tmp46D;});_tmp44E[4]=({struct _tuple29*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->f1=({const char*_tmp46C="-setjmp";_tag_dyneither(_tmp46C,sizeof(char),8);});_tmp467->f2=0;_tmp467->f3=({const char*_tmp46B="";_tag_dyneither(_tmp46B,sizeof(char),1);});_tmp467->f4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp46A;_tmp46A.tag=3;_tmp46A.f1=& Cyc_do_setjmp;_tmp46A;});_tmp469;});_tmp467->f5=({const char*_tmp468="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp468,sizeof(char),186);});_tmp467;});_tmp44E[3]=({struct _tuple29*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->f1=({const char*_tmp466="-finish";_tag_dyneither(_tmp466,sizeof(char),8);});_tmp461->f2=0;_tmp461->f3=({const char*_tmp465="";_tag_dyneither(_tmp465,sizeof(char),1);});_tmp461->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp464;_tmp464.tag=0;_tmp464.f1=Cyc_set_FINISH;_tmp464;});_tmp463;});_tmp461->f5=({const char*_tmp462="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp462,sizeof(char),57);});_tmp461;});_tmp44E[2]=({struct _tuple29*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->f1=({const char*_tmp460="-gatherscript";_tag_dyneither(_tmp460,sizeof(char),14);});_tmp45B->f2=0;_tmp45B->f3=({const char*_tmp45F="";_tag_dyneither(_tmp45F,sizeof(char),1);});_tmp45B->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp45E;_tmp45E.tag=0;_tmp45E.f1=Cyc_set_GATHERSCRIPT;_tmp45E;});_tmp45D;});_tmp45B->f5=({const char*_tmp45C="Produce a script to gather C library info";_tag_dyneither(_tmp45C,sizeof(char),42);});_tmp45B;});_tmp44E[1]=({struct _tuple29*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->f1=({const char*_tmp45A="-gather";_tag_dyneither(_tmp45A,sizeof(char),8);});_tmp455->f2=0;_tmp455->f3=({const char*_tmp459="";_tag_dyneither(_tmp459,sizeof(char),1);});_tmp455->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp458;_tmp458.tag=0;_tmp458.f1=Cyc_set_GATHER;_tmp458;});_tmp457;});_tmp455->f5=({const char*_tmp456="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp456,sizeof(char),56);});_tmp455;});_tmp44E[0]=({struct _tuple29*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->f1=({const char*_tmp454="-d";_tag_dyneither(_tmp454,sizeof(char),3);});_tmp44F->f2=0;_tmp44F->f3=({const char*_tmp453=" <file>";_tag_dyneither(_tmp453,sizeof(char),8);});_tmp44F->f4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451[0]=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp452;_tmp452.tag=5;_tmp452.f1=Cyc_set_output_dir;_tmp452;});_tmp451;});_tmp44F->f5=({const char*_tmp450="Set the output directory to <file>";_tag_dyneither(_tmp450,sizeof(char),35);});_tmp44F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp44E,sizeof(struct _tuple29*),9));});
# 2132
struct _dyneither_ptr _tmp417=Cyc_Specsfile_parse_b(options,Cyc_add_spec_file,Cyc_no_other,({const char*_tmp44D="Options:";_tag_dyneither(_tmp44D,sizeof(char),9);}),argv);
# 2134
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_spec_file,Cyc_no_other,({const char*_tmp418="Options:";_tag_dyneither(_tmp418,sizeof(char),9);}),_tmp417);
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_Arg_usage(options,({const char*_tmp419="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp419,sizeof(char),59);}));
# 2144
return 1;}{
# 2150
struct _dyneither_ptr _tmp41A=Cyc_Specsfile_find_in_arch_path(({const char*_tmp44C="cycspecs";_tag_dyneither(_tmp44C,sizeof(char),9);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp41D;_tmp41D.tag=0;_tmp41D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41A);({void*_tmp41B[1]={& _tmp41D};Cyc_fprintf(Cyc_stderr,({const char*_tmp41C="Reading from specs file %s\n";_tag_dyneither(_tmp41C,sizeof(char),28);}),_tag_dyneither(_tmp41B,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp41E=Cyc_Specsfile_read_specs(_tmp41A);
Cyc_target_cflags=Cyc_Specsfile_get_spec(_tmp41E,({const char*_tmp41F="cyclone_target_cflags";_tag_dyneither(_tmp41F,sizeof(char),22);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0;_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp420[1]={& _tmp422};Cyc_fprintf(Cyc_stderr,({const char*_tmp421="Target cflags are %s\n";_tag_dyneither(_tmp421,sizeof(char),22);}),_tag_dyneither(_tmp420,sizeof(void*),1));});});
Cyc_cyclone_cc=Cyc_Specsfile_get_spec(_tmp41E,({const char*_tmp423="cyclone_cc";_tag_dyneither(_tmp423,sizeof(char),11);}));
if(!((unsigned int)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp424="gcc";_tag_dyneither(_tmp424,sizeof(char),4);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0;_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp425[1]={& _tmp427};Cyc_fprintf(Cyc_stderr,({const char*_tmp426="C compiler is %s\n";_tag_dyneither(_tmp426,sizeof(char),18);}),_tag_dyneither(_tmp425,sizeof(void*),1));});});
# 2159
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp428=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp429="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp429,sizeof(char),22);}),_tag_dyneither(_tmp428,sizeof(void*),0));});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
({void*_tmp42A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp42B="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp42B,sizeof(char),35);}),_tag_dyneither(_tmp42A,sizeof(void*),0));});
 exit(1);}
# 2167
({void*_tmp42C=0;Cyc_prscript(({const char*_tmp42D="#!/bin/sh\n";_tag_dyneither(_tmp42D,sizeof(char),11);}),_tag_dyneither(_tmp42C,sizeof(void*),0));});
({void*_tmp42E=0;Cyc_prscript(({const char*_tmp42F="GCC=\"gcc\"\n";_tag_dyneither(_tmp42F,sizeof(char),11);}),_tag_dyneither(_tmp42E,sizeof(void*),0));});}
# 2172
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp432;_tmp432.tag=0;_tmp432.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp430[1]={& _tmp432};Cyc_fprintf(Cyc_stderr,({const char*_tmp431="Error: could not create directory %s\n";_tag_dyneither(_tmp431,sizeof(char),38);}),_tag_dyneither(_tmp430,sizeof(void*),1));});});
return 1;}
# 2176
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp435;_tmp435.tag=0;_tmp435.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp433[1]={& _tmp435};Cyc_fprintf(Cyc_stderr,({const char*_tmp434="Output directory is %s\n";_tag_dyneither(_tmp434,sizeof(char),24);}),_tag_dyneither(_tmp433,sizeof(void*),1));});});
# 2179
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp438;_tmp438.tag=0;_tmp438.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp436[1]={& _tmp438};Cyc_prscript(({const char*_tmp437="cd %s\n";_tag_dyneither(_tmp437,sizeof(char),7);}),_tag_dyneither(_tmp436,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp43B;_tmp43B.tag=0;_tmp43B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp439[1]={& _tmp43B};Cyc_prscript(({const char*_tmp43A="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp43A,sizeof(char),41);}),_tag_dyneither(_tmp439,sizeof(void*),1));});});}
# 2185
if(!Cyc_gathering()){
# 2188
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp43C="BUILDLIB.LOG";_tag_dyneither(_tmp43C,sizeof(char),13);})),sizeof(char),1)),"w");
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp43F;_tmp43F.tag=0;_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp43D[1]={& _tmp43F};Cyc_fprintf(Cyc_stderr,({const char*_tmp43E="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp43E,sizeof(char),50);}),_tag_dyneither(_tmp43D,sizeof(void*),1));});});
return 1;}
# 2194
if(!Cyc_do_setjmp){
# 2196
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp440="cstubs.c";_tag_dyneither(_tmp440,sizeof(char),9);})),sizeof(char),1)),"w");
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp443;_tmp443.tag=0;_tmp443.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp441[1]={& _tmp443};Cyc_fprintf(Cyc_stderr,({const char*_tmp442="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp442,sizeof(char),50);}),_tag_dyneither(_tmp441,sizeof(void*),1));});});
return 1;}
# 2203
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp444="cycstubs.cyc";_tag_dyneither(_tmp444,sizeof(char),13);})),sizeof(char),1)),"w");
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp447;_tmp447.tag=0;_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp445[1]={& _tmp447};Cyc_fprintf(Cyc_stderr,({const char*_tmp446="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp446,sizeof(char),52);}),_tag_dyneither(_tmp445,sizeof(void*),1));});});
# 2208
return 1;}
# 2210
({void*_tmp448=0;Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),({const char*_tmp449="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp449,sizeof(char),32);}),_tag_dyneither(_tmp448,sizeof(void*),0));});}}{
# 2217
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 2223
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp44A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp44B="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp44B,sizeof(char),22);}),_tag_dyneither(_tmp44A,sizeof(void*),0));});
 exit(1);}}}
# 2231
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2234
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2242
return 0;};};};};}

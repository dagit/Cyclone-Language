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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1108 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1159
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1171
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1183
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
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 66
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 33 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 31 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 38
void*Cyc_Toc_typ_to_c(void*t);
void Cyc_Toc_finish();
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
({void*_tmp2=0;({struct Cyc___cycFILE*_tmp488=Cyc_stderr;struct _dyneither_ptr _tmp487=({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34);});Cyc_fprintf(_tmp488,_tmp487,_tag_dyneither(_tmp2,sizeof(void*),0));});});
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
({struct Cyc_Set_Set**_tmp48A=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp489=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);_tmp5[0]=_tmp489;});_tmp5;});Cyc_current_targets=_tmp48A;});}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
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
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)symbol);({void*_tmp8[1]={& _tmpA};({struct Cyc___cycFILE*_tmp48C=Cyc_stderr;struct _dyneither_ptr _tmp48B=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_dyneither(_tmp9,sizeof(char),39);});Cyc_fprintf(_tmp48C,_tmp48B,_tag_dyneither(_tmp8,sizeof(void*),1));});});});
return symbol;}
# 184
return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp48F=(struct _dyneither_ptr)symbol;int _tmp48E=(int)prefix_len;Cyc_substring(_tmp48F,_tmp48E,({unsigned long _tmp48D=Cyc_strlen((struct _dyneither_ptr)symbol);_tmp48D - prefix_len;}));});}
# 187
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_tmpC=_tmpB;struct Cyc_Absyn_Typedefdecl*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp12;switch(*((int*)_tmpC)){case 5: _LL1: _tmp12=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2:
# 190
({struct _dyneither_ptr*_tmp491=({struct _dyneither_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));({struct _dyneither_ptr _tmp490=Cyc_remove_user_prefix(*(*_tmp12->name).f2);_tmpD[0]=_tmp490;});_tmpD;});(*_tmp12->name).f2=_tmp491;});
goto _LL0;case 7: _LL3: _tmp13=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4:
# 193
({struct _dyneither_ptr*_tmp493=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _dyneither_ptr _tmp492=Cyc_remove_user_prefix(*(*_tmp13->name).f2);_tmpE[0]=_tmp492;});_tmpE;});(*_tmp13->name).f2=_tmp493;});
goto _LL0;case 8: _LL5: _tmp14=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6:
# 196
({struct _dyneither_ptr*_tmp495=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _dyneither_ptr _tmp494=Cyc_remove_user_prefix(*(*_tmp14->name).f2);_tmpF[0]=_tmp494;});_tmpF;});(*_tmp14->name).f2=_tmp495;});
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
({void*_tmp10=0;({struct Cyc___cycFILE*_tmp497=Cyc_stderr;struct _dyneither_ptr _tmp496=({const char*_tmp11="Error in .cys file: bad user-defined type definition\n";_tag_dyneither(_tmp11,sizeof(char),54);});Cyc_fprintf(_tmp497,_tmp496,_tag_dyneither(_tmp10,sizeof(void*),0));});});
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
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Core_Impossible_exn_struct _tmp499=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp498=({const char*_tmp25="Internal Error: bad user-def name";_tag_dyneither(_tmp25,sizeof(char),34);});_tmp24.f1=_tmp498;});_tmp24;});_tmp23[0]=_tmp499;});_tmp23;}));}struct _tuple26{void*f1;void*f2;};struct _tuple27{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 259
static void Cyc_merge_rep_decl(struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
{struct _tuple26 _tmp26=({struct _tuple26 _tmpA2;_tmpA2.f1=c_decl->r;_tmpA2.f2=user_decl->r;_tmpA2;});struct _tuple26 _tmp27=_tmp26;struct Cyc_Absyn_Typedefdecl*_tmpA1;struct Cyc_Absyn_Typedefdecl*_tmpA0;struct Cyc_Absyn_Enumdecl*_tmp9F;struct Cyc_Absyn_Enumdecl*_tmp9E;struct Cyc_Absyn_Aggrdecl*_tmp9D;struct Cyc_Absyn_Aggrdecl*_tmp9C;switch(*((int*)_tmp27.f1)){case 5: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 5){_LL48: _tmp9D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmp9C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL49:
# 265
 if(_tmp9C->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0;({struct _dyneither_ptr _tmp49A=(struct _dyneither_ptr)({const char*_tmp2C=": universal quantification in user definition not allowed";_tag_dyneither(_tmp2C,sizeof(char),58);});_tmp2B.f1=_tmp49A;});({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;({struct _dyneither_ptr _tmp49B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp2A.f1=_tmp49B;});({void*_tmp28[2]={& _tmp2A,& _tmp2B};({struct Cyc___cycFILE*_tmp49D=Cyc_stderr;struct _dyneither_ptr _tmp49C=({const char*_tmp29="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp29,sizeof(char),62);});Cyc_fprintf(_tmp49D,_tmp49C,_tag_dyneither(_tmp28,sizeof(void*),2));});});});});return;}
if(_tmp9C->sc != _tmp9D->sc)
({struct Cyc_String_pa_PrintArg_struct _tmp30;_tmp30.tag=0;({struct _dyneither_ptr _tmp49E=(struct _dyneither_ptr)({const char*_tmp31=": scopes don't match";_tag_dyneither(_tmp31,sizeof(char),21);});_tmp30.f1=_tmp49E;});({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0;({struct _dyneither_ptr _tmp49F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp2F.f1=_tmp49F;});({void*_tmp2D[2]={& _tmp2F,& _tmp30};({struct Cyc___cycFILE*_tmp4A1=Cyc_stderr;struct _dyneither_ptr _tmp4A0=({const char*_tmp2E="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp2E,sizeof(char),71);});Cyc_fprintf(_tmp4A1,_tmp4A0,_tag_dyneither(_tmp2D,sizeof(void*),2));});});});});
# 270
if(_tmp9C->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0;({struct _dyneither_ptr _tmp4A2=(struct _dyneither_ptr)({const char*_tmp36=": no user definition";_tag_dyneither(_tmp36,sizeof(char),21);});_tmp35.f1=_tmp4A2;});({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0;({struct _dyneither_ptr _tmp4A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp34.f1=_tmp4A3;});({void*_tmp32[2]={& _tmp34,& _tmp35};({struct Cyc___cycFILE*_tmp4A5=Cyc_stderr;struct _dyneither_ptr _tmp4A4=({const char*_tmp33="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp33,sizeof(char),62);});Cyc_fprintf(_tmp4A5,_tmp4A4,_tag_dyneither(_tmp32,sizeof(void*),2));});});});});return;}
if(_tmp9D->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3A;_tmp3A.tag=0;({struct _dyneither_ptr _tmp4A6=(struct _dyneither_ptr)({const char*_tmp3B=": no definition for system version";_tag_dyneither(_tmp3B,sizeof(char),35);});_tmp3A.f1=_tmp4A6;});({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0;({struct _dyneither_ptr _tmp4A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp39.f1=_tmp4A7;});({void*_tmp37[2]={& _tmp39,& _tmp3A};({struct Cyc___cycFILE*_tmp4A9=Cyc_stderr;struct _dyneither_ptr _tmp4A8=({const char*_tmp38="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp38,sizeof(char),71);});Cyc_fprintf(_tmp4A9,_tmp4A8,_tag_dyneither(_tmp37,sizeof(void*),2));});});});});
c_decl->r=user_decl->r;
return;}
# 277
{struct _tuple27 _tmp3C=({struct _tuple27 _tmp79;_tmp79.f1=_tmp9D->impl;_tmp79.f2=_tmp9C->impl;_tmp79;});struct _tuple27 _tmp3D=_tmp3C;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;int _tmp74;if(_tmp3D.f1 != 0){if(_tmp3D.f2 != 0){_LL51: _tmp78=(_tmp3D.f1)->fields;_tmp77=(_tmp3D.f2)->exist_vars;_tmp76=(_tmp3D.f2)->rgn_po;_tmp75=(_tmp3D.f2)->fields;_tmp74=(_tmp3D.f2)->tagged;_LL52:
# 279
 if(_tmp74){
({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0;({struct _dyneither_ptr _tmp4AA=(struct _dyneither_ptr)({const char*_tmp42=": user @tagged annotation not allowed";_tag_dyneither(_tmp42,sizeof(char),38);});_tmp41.f1=_tmp4AA;});({struct Cyc_String_pa_PrintArg_struct _tmp40;_tmp40.tag=0;({struct _dyneither_ptr _tmp4AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp40.f1=_tmp4AB;});({void*_tmp3E[2]={& _tmp40,& _tmp41};({struct Cyc___cycFILE*_tmp4AD=Cyc_stderr;struct _dyneither_ptr _tmp4AC=({const char*_tmp3F="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp3F,sizeof(char),62);});Cyc_fprintf(_tmp4AD,_tmp4AC,_tag_dyneither(_tmp3E,sizeof(void*),2));});});});});return;}
if((unsigned int)_tmp77){
({struct Cyc_String_pa_PrintArg_struct _tmp46;_tmp46.tag=0;({struct _dyneither_ptr _tmp4AE=(struct _dyneither_ptr)({const char*_tmp47=": existential quantification in user definition not allowed";_tag_dyneither(_tmp47,sizeof(char),60);});_tmp46.f1=_tmp4AE;});({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0;({struct _dyneither_ptr _tmp4AF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp45.f1=_tmp4AF;});({void*_tmp43[2]={& _tmp45,& _tmp46};({struct Cyc___cycFILE*_tmp4B1=Cyc_stderr;struct _dyneither_ptr _tmp4B0=({const char*_tmp44="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp44,sizeof(char),62);});Cyc_fprintf(_tmp4B1,_tmp4B0,_tag_dyneither(_tmp43,sizeof(void*),2));});});});});return;}
if((unsigned int)_tmp76){
({struct Cyc_String_pa_PrintArg_struct _tmp4B;_tmp4B.tag=0;({struct _dyneither_ptr _tmp4B2=(struct _dyneither_ptr)({const char*_tmp4C=": region partial order in user definition not allowed";_tag_dyneither(_tmp4C,sizeof(char),54);});_tmp4B.f1=_tmp4B2;});({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0;({struct _dyneither_ptr _tmp4B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp4A.f1=_tmp4B3;});({void*_tmp48[2]={& _tmp4A,& _tmp4B};({struct Cyc___cycFILE*_tmp4B5=Cyc_stderr;struct _dyneither_ptr _tmp4B4=({const char*_tmp49="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp49,sizeof(char),62);});Cyc_fprintf(_tmp4B5,_tmp4B4,_tag_dyneither(_tmp48,sizeof(void*),2));});});});});return;}{
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
({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;({struct _dyneither_ptr _tmp4B9=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;({struct _dyneither_ptr _tmp4B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp5A.f1=_tmp4B6;});({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4E->name);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;({struct _dyneither_ptr _tmp4B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));_tmp58.f1=_tmp4B7;});({void*_tmp56[3]={& _tmp58,& _tmp59,& _tmp5A};({struct _dyneither_ptr _tmp4B8=({const char*_tmp57=": type %s of user definition of field %s != %s";_tag_dyneither(_tmp57,sizeof(char),47);});Cyc_aprintf(_tmp4B8,_tag_dyneither(_tmp56,sizeof(void*),3));});});});});}));_tmp55.f1=_tmp4B9;});({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0;({struct _dyneither_ptr _tmp4BA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp54.f1=_tmp4BA;});({void*_tmp52[2]={& _tmp54,& _tmp55};({struct Cyc___cycFILE*_tmp4BC=Cyc_stderr;struct _dyneither_ptr _tmp4BB=({const char*_tmp53="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp53,sizeof(char),71);});Cyc_fprintf(_tmp4BC,_tmp4BB,_tag_dyneither(_tmp52,sizeof(void*),2));});});});});else{
# 304
if(_tmp4F->width != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;({struct _dyneither_ptr _tmp4BF=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0;({struct _dyneither_ptr _tmp4BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));_tmp61.f1=_tmp4BD;});({void*_tmp5F[1]={& _tmp61};({struct _dyneither_ptr _tmp4BE=({const char*_tmp60=": ignoring width of user definition of field %s";_tag_dyneither(_tmp60,sizeof(char),48);});Cyc_aprintf(_tmp4BE,_tag_dyneither(_tmp5F,sizeof(void*),1));});});}));_tmp5E.f1=_tmp4BF;});({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0;({struct _dyneither_ptr _tmp4C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp5D.f1=_tmp4C0;});({void*_tmp5B[2]={& _tmp5D,& _tmp5E};({struct Cyc___cycFILE*_tmp4C2=Cyc_stderr;struct _dyneither_ptr _tmp4C1=({const char*_tmp5C="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp5C,sizeof(char),71);});Cyc_fprintf(_tmp4C2,_tmp4C1,_tag_dyneither(_tmp5B,sizeof(void*),2));});});});});
# 308
if(_tmp4F->attributes != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0;({struct _dyneither_ptr _tmp4C5=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;({struct _dyneither_ptr _tmp4C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F->type));_tmp68.f1=_tmp4C3;});({void*_tmp66[1]={& _tmp68};({struct _dyneither_ptr _tmp4C4=({const char*_tmp67=": ignoring attributes of user definition of field %s";_tag_dyneither(_tmp67,sizeof(char),53);});Cyc_aprintf(_tmp4C4,_tag_dyneither(_tmp66,sizeof(void*),1));});});}));_tmp65.f1=_tmp4C5;});({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0;({struct _dyneither_ptr _tmp4C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp64.f1=_tmp4C6;});({void*_tmp62[2]={& _tmp64,& _tmp65};({struct Cyc___cycFILE*_tmp4C8=Cyc_stderr;struct _dyneither_ptr _tmp4C7=({const char*_tmp63="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp63,sizeof(char),71);});Cyc_fprintf(_tmp4C8,_tmp4C7,_tag_dyneither(_tmp62,sizeof(void*),2));});});});});
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
({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0;({struct _dyneither_ptr _tmp4CA=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp4D->hd)->name);({void*_tmp6E[1]={& _tmp70};({struct _dyneither_ptr _tmp4C9=({const char*_tmp6F=": no definition of field %s in system version";_tag_dyneither(_tmp6F,sizeof(char),46);});Cyc_aprintf(_tmp4C9,_tag_dyneither(_tmp6E,sizeof(void*),1));});});}));_tmp6D.f1=_tmp4CA;});({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0;({struct _dyneither_ptr _tmp4CB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));_tmp6C.f1=_tmp4CB;});({void*_tmp6A[2]={& _tmp6C,& _tmp6D};({struct Cyc___cycFILE*_tmp4CD=Cyc_stderr;struct _dyneither_ptr _tmp4CC=({const char*_tmp6B="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp6B,sizeof(char),71);});Cyc_fprintf(_tmp4CD,_tmp4CC,_tag_dyneither(_tmp6A,sizeof(void*),2));});});});});}
# 331
_tmp4D=_tmp4D->tl;}
# 333
goto _LL50;};};}else{goto _LL53;}}else{_LL53: _LL54:
# 335
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Core_Impossible_exn_struct _tmp4CF=({struct Cyc_Core_Impossible_exn_struct _tmp72;_tmp72.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp4CE=({const char*_tmp73="Internal Error: NULL cases not possible";_tag_dyneither(_tmp73,sizeof(char),40);});_tmp72.f1=_tmp4CE;});_tmp72;});_tmp71[0]=_tmp4CF;});_tmp71;}));}_LL50:;}
# 337
return;}else{goto _LL4E;}case 7: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 7){_LL4A: _tmp9F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmp9E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL4B:
# 339
({struct Cyc_String_pa_PrintArg_struct _tmp7D;_tmp7D.tag=0;({struct _dyneither_ptr _tmp4D0=(struct _dyneither_ptr)({const char*_tmp7E=": enum merging not currently supported";_tag_dyneither(_tmp7E,sizeof(char),39);});_tmp7D.f1=_tmp4D0;});({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;({struct _dyneither_ptr _tmp4D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F->name));_tmp7C.f1=_tmp4D1;});({void*_tmp7A[2]={& _tmp7C,& _tmp7D};({struct Cyc___cycFILE*_tmp4D3=Cyc_stderr;struct _dyneither_ptr _tmp4D2=({const char*_tmp7B="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp7B,sizeof(char),62);});Cyc_fprintf(_tmp4D3,_tmp4D2,_tag_dyneither(_tmp7A,sizeof(void*),2));});});});});return;}else{goto _LL4E;}case 8: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f2)->tag == 8){_LL4C: _tmpA1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f1)->f1;_tmpA0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp27.f2)->f1;_LL4D:
# 341
 if(_tmpA0->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0;({struct _dyneither_ptr _tmp4D4=(struct _dyneither_ptr)({const char*_tmp83=": universal quantification in user definition not supported";_tag_dyneither(_tmp83,sizeof(char),60);});_tmp82.f1=_tmp4D4;});({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0;({struct _dyneither_ptr _tmp4D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp81.f1=_tmp4D5;});({void*_tmp7F[2]={& _tmp81,& _tmp82};({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _dyneither_ptr _tmp4D6=({const char*_tmp80="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp80,sizeof(char),62);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_dyneither(_tmp7F,sizeof(void*),2));});});});});return;}
# 344
if(_tmpA0->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp87;_tmp87.tag=0;({struct _dyneither_ptr _tmp4D8=(struct _dyneither_ptr)({const char*_tmp88=": no user definition";_tag_dyneither(_tmp88,sizeof(char),21);});_tmp87.f1=_tmp4D8;});({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;({struct _dyneither_ptr _tmp4D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp86.f1=_tmp4D9;});({void*_tmp84[2]={& _tmp86,& _tmp87};({struct Cyc___cycFILE*_tmp4DB=Cyc_stderr;struct _dyneither_ptr _tmp4DA=({const char*_tmp85="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp85,sizeof(char),62);});Cyc_fprintf(_tmp4DB,_tmp4DA,_tag_dyneither(_tmp84,sizeof(void*),2));});});});});return;}
if(_tmpA1->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0;({struct _dyneither_ptr _tmp4DC=(struct _dyneither_ptr)({const char*_tmp8D=": no definition for system version";_tag_dyneither(_tmp8D,sizeof(char),35);});_tmp8C.f1=_tmp4DC;});({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0;({struct _dyneither_ptr _tmp4DD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp8B.f1=_tmp4DD;});({void*_tmp89[2]={& _tmp8B,& _tmp8C};({struct Cyc___cycFILE*_tmp4DF=Cyc_stderr;struct _dyneither_ptr _tmp4DE=({const char*_tmp8A="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp8A,sizeof(char),71);});Cyc_fprintf(_tmp4DF,_tmp4DE,_tag_dyneither(_tmp89,sizeof(void*),2));});});});});
c_decl->r=user_decl->r;
return;}{
# 352
void*_tmp8E=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA1->defn));
void*_tmp8F=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA0->defn));
if(!Cyc_Tcutil_unify(_tmp8E,_tmp8F))
({struct Cyc_String_pa_PrintArg_struct _tmp93;_tmp93.tag=0;({struct _dyneither_ptr _tmp4E3=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0;({struct _dyneither_ptr _tmp4E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpA1->defn)));_tmp97.f1=_tmp4E0;});({struct Cyc_String_pa_PrintArg_struct _tmp96;_tmp96.tag=0;({struct _dyneither_ptr _tmp4E1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpA0->defn)));_tmp96.f1=_tmp4E1;});({void*_tmp94[2]={& _tmp96,& _tmp97};({struct _dyneither_ptr _tmp4E2=({const char*_tmp95=": type definition %s of user definition != %s";_tag_dyneither(_tmp95,sizeof(char),46);});Cyc_aprintf(_tmp4E2,_tag_dyneither(_tmp94,sizeof(void*),2));});});});}));_tmp93.f1=_tmp4E3;});({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0;({struct _dyneither_ptr _tmp4E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1->name));_tmp92.f1=_tmp4E4;});({void*_tmp90[2]={& _tmp92,& _tmp93};({struct Cyc___cycFILE*_tmp4E6=Cyc_stderr;struct _dyneither_ptr _tmp4E5=({const char*_tmp91="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp91,sizeof(char),71);});Cyc_fprintf(_tmp4E6,_tmp4E5,_tag_dyneither(_tmp90,sizeof(void*),2));});});});});else{
# 361
_tmpA1->defn=_tmpA0->defn;
_tmpA1->tq=_tmpA0->tq;}
# 364
goto _LL47;};}else{goto _LL4E;}default: _LL4E: _LL4F:
# 366
({struct Cyc_String_pa_PrintArg_struct _tmp9A;_tmp9A.tag=0;({struct _dyneither_ptr _tmp4E7=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 368
Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp9B[1];_tmp9B[0]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9B,sizeof(struct Cyc_Absyn_Decl*),1));})));_tmp9A.f1=_tmp4E7;});({void*_tmp98[1]={& _tmp9A};({struct Cyc___cycFILE*_tmp4E9=Cyc_stderr;struct _dyneither_ptr _tmp4E8=({const char*_tmp99="Error in .cys file: bad user-defined type %s\n";_tag_dyneither(_tmp99,sizeof(char),46);});Cyc_fprintf(_tmp4E9,_tmp4E8,_tag_dyneither(_tmp98,sizeof(void*),1));});});});
return;}_LL47:;}
# 372
c_decl->r=user_decl->r;}
# 377
const int Cyc_lex_base[431]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 384
int state;int base;int backtrk;
int c;
state=start_state;
# 388
if(state >= 0){
({int _tmp4EA=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp4EA;});
lbuf->lex_last_action=- 1;}else{
# 392
state=(- state)- 1;}
# 394
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(431,state)];
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
if(Cyc_lex_check[_check_known_subscript_notnull(3846,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3846,base + c)];else{
# 414
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct Cyc_Lexing_Error_exn_struct _tmp4EC=({struct Cyc_Lexing_Error_exn_struct _tmpA4;_tmpA4.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp4EB=({const char*_tmpA5="empty token";_tag_dyneither(_tmpA5,sizeof(char),12);});_tmpA4.f1=_tmp4EB;});_tmpA4;});_tmpA3[0]=_tmp4EC;});_tmpA3;}));else{
# 420
return lbuf->lex_last_action;}}else{
# 423
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 427
struct _tuple21*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp4ED=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp4ED;});
{int _tmpA6=lexstate;switch(_tmpA6){case 0: _LL56: _LL57:
# 386 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
({struct Cyc_Set_Set**_tmp4F0=({struct Cyc_Set_Set**_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct Cyc_Set_Set*_tmp4EF=({struct Cyc_Set_Set*_tmp4EE=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(_tmp4EE,(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});_tmpA7[0]=_tmp4EF;});_tmpA7;});Cyc_current_targets=_tmp4F0;});}
# 391
return({struct _tuple21*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmpA8->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmpA8;});case 1: _LL58: _LL59:
# 394 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL5A: _LL5B:
# 396
 return 0;default: _LL5C: _LL5D:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL55:;}
# 400
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Lexing_Error_exn_struct _tmp4F2=({struct Cyc_Lexing_Error_exn_struct _tmpAA;_tmpAA.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp4F1=({const char*_tmpAB="some action didn't return!";_tag_dyneither(_tmpAB,sizeof(char),27);});_tmpAA.f1=_tmp4F1;});_tmpAA;});_tmpA9[0]=_tmp4F2;});_tmpA9;}));}
# 402
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp4F3=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp4F3;});
{int _tmpAC=lexstate;switch(_tmpAC){case 0: _LL5F: _LL60:
# 400 "buildlib.cyl"
({struct _dyneither_ptr*_tmp4F7=({struct _dyneither_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _dyneither_ptr _tmp4F6=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp4F5=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp4F5,0,(unsigned long)(({
int _tmp4F4=Cyc_Lexing_lexeme_end(lexbuf);_tmp4F4 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});_tmpAD[0]=_tmp4F6;});_tmpAD;});
# 400
Cyc_current_source=_tmp4F7;});
# 402
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp4F9=({struct Cyc_Set_Set**_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct Cyc_Set_Set*_tmp4F8=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpAE[0]=_tmp4F8;});_tmpAE;});Cyc_current_targets=_tmp4F9;});
Cyc_token(lexbuf);
return 0;case 1: _LL61: _LL62:
# 408
({struct _dyneither_ptr*_tmp4FD=({struct _dyneither_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));({struct _dyneither_ptr _tmp4FC=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp4FB=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp4FB,0,(unsigned long)(({
int _tmp4FA=Cyc_Lexing_lexeme_end(lexbuf);_tmp4FA - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});_tmpAF[0]=_tmp4FC;});_tmpAF;});
# 408
Cyc_current_source=_tmp4FD;});
# 410
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp4FF=({struct Cyc_Set_Set**_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_Set_Set*_tmp4FE=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpB0[0]=_tmp4FE;});_tmpB0;});Cyc_current_targets=_tmp4FF;});
Cyc_args(lexbuf);
return 0;case 2: _LL63: _LL64:
# 416
({struct _dyneither_ptr*_tmp501=({struct _dyneither_ptr*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct _dyneither_ptr _tmp500=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpB1[0]=_tmp500;});_tmpB1;});Cyc_current_source=_tmp501;});
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp503=({struct Cyc_Set_Set**_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct Cyc_Set_Set*_tmp502=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpB2[0]=_tmp502;});_tmpB2;});Cyc_current_targets=_tmp503;});
Cyc_token(lexbuf);
return 0;default: _LL65: _LL66:
# 422
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL5E:;}
# 425
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct Cyc_Lexing_Error_exn_struct _tmp505=({struct Cyc_Lexing_Error_exn_struct _tmpB4;_tmpB4.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp504=({const char*_tmpB5="some action didn't return!";_tag_dyneither(_tmpB5,sizeof(char),27);});_tmpB4.f1=_tmp504;});_tmpB4;});_tmpB3[0]=_tmp505;});_tmpB3;}));}
# 427
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp506=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp506;});
{int _tmpB6=lexstate;switch(_tmpB6){case 0: _LL68: _LL69: {
# 425 "buildlib.cyl"
struct _dyneither_ptr*_tmpB7=({struct _dyneither_ptr*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct _dyneither_ptr _tmp509=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp508=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp508,0,(unsigned long)(({
int _tmp507=Cyc_Lexing_lexeme_end(lexbuf);_tmp507 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});_tmpB9[0]=_tmp509;});_tmpB9;});
({struct Cyc_List_List*_tmp50A=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->hd=_tmpB7;_tmpB8->tl=Cyc_current_args;_tmpB8;});Cyc_current_args=_tmp50A;});
return Cyc_args(lexbuf);}case 1: _LL6A: _LL6B: {
# 431
struct _dyneither_ptr*_tmpBA=({struct _dyneither_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _dyneither_ptr _tmp50D=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp50C=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp50C,0,(unsigned long)(({
int _tmp50B=Cyc_Lexing_lexeme_end(lexbuf);_tmp50B - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});_tmpBC[0]=_tmp50D;});_tmpBC;});
({struct Cyc_List_List*_tmp50E=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=_tmpBA;_tmpBB->tl=Cyc_current_args;_tmpBB;});Cyc_current_args=_tmp50E;});
return Cyc_args(lexbuf);}case 2: _LL6C: _LL6D: {
# 437
struct _dyneither_ptr*_tmpBD=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct _dyneither_ptr _tmp511=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp510=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp510,0,(unsigned long)(({
int _tmp50F=Cyc_Lexing_lexeme_end(lexbuf);_tmp50F - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});_tmpBF[0]=_tmp511;});_tmpBF;});
({struct Cyc_List_List*_tmp512=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=_tmpBD;_tmpBE->tl=Cyc_current_args;_tmpBE;});Cyc_current_args=_tmp512;});
return Cyc_token(lexbuf);}default: _LL6E: _LL6F:
# 442
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL67:;}
# 445
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));({struct Cyc_Lexing_Error_exn_struct _tmp514=({struct Cyc_Lexing_Error_exn_struct _tmpC1;_tmpC1.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp513=({const char*_tmpC2="some action didn't return!";_tag_dyneither(_tmpC2,sizeof(char),27);});_tmpC1.f1=_tmp513;});_tmpC1;});_tmpC0[0]=_tmp514;});_tmpC0;}));}
# 447
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp515=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp515;});
{int _tmpC3=lexstate;switch(_tmpC3){case 0: _LL71: _LL72:
# 446 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct _dyneither_ptr _tmp516=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpC4[0]=_tmp516;});_tmpC4;}));return Cyc_token(lexbuf);case 1: _LL73: _LL74:
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
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));({struct Cyc_Lexing_Error_exn_struct _tmp518=({struct Cyc_Lexing_Error_exn_struct _tmpC6;_tmpC6.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp517=({const char*_tmpC7="some action didn't return!";_tag_dyneither(_tmpC7,sizeof(char),27);});_tmpC6.f1=_tmp517;});_tmpC6;});_tmpC5[0]=_tmp518;});_tmpC5;}));}
# 557
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp519=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp519;});
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
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));({struct Cyc_Lexing_Error_exn_struct _tmp51B=({struct Cyc_Lexing_Error_exn_struct _tmpCA;_tmpCA.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp51A=({const char*_tmpCB="some action didn't return!";_tag_dyneither(_tmpCB,sizeof(char),27);});_tmpCA.f1=_tmp51A;});_tmpCA;});_tmpC9[0]=_tmp51B;});_tmpC9;}));}
# 576
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp51C=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp51C;});
{int _tmpCC=lexstate;switch(_tmpCC){case 0: _LLED: _LLEE:
# 579 "buildlib.cyl"
 return 0;case 1: _LLEF: _LLF0:
# 581
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LLF1: _LLF2:
# 588 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCD=0;({struct _dyneither_ptr _tmp51D=({const char*_tmpCE="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpCE,sizeof(char),44);});Cyc_log(_tmp51D,_tag_dyneither(_tmpCD,sizeof(void*),0));});});
return 1;case 3: _LLF3: _LLF4:
# 594 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCF=0;({struct _dyneither_ptr _tmp51E=({const char*_tmpD0="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpD0,sizeof(char),44);});Cyc_log(_tmp51E,_tag_dyneither(_tmpCF,sizeof(void*),0));});});
return 1;case 4: _LLF5: _LLF6:
# 600 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD1=0;({struct _dyneither_ptr _tmp51F=({const char*_tmpD2="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpD2,sizeof(char),44);});Cyc_log(_tmp51F,_tag_dyneither(_tmpD1,sizeof(void*),0));});});
return 1;case 5: _LLF7: _LLF8:
# 606 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD3=0;({struct _dyneither_ptr _tmp520=({const char*_tmpD4="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpD4,sizeof(char),44);});Cyc_log(_tmp520,_tag_dyneither(_tmpD3,sizeof(void*),0));});});
return 1;case 6: _LLF9: _LLFA:
# 610
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpD5=0;({struct _dyneither_ptr _tmp521=({const char*_tmpD6="Warning: use of region sidestepped\n";_tag_dyneither(_tmpD6,sizeof(char),36);});Cyc_log(_tmp521,_tag_dyneither(_tmpD5,sizeof(void*),0));});});
return 1;case 7: _LLFB: _LLFC:
# 614
({void*_tmpD7=0;({struct _dyneither_ptr _tmp522=({const char*_tmpD8="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmpD8,sizeof(char),39);});Cyc_log(_tmp522,_tag_dyneither(_tmpD7,sizeof(void*),0));});});
return 1;case 8: _LLFD: _LLFE:
# 619 "buildlib.cyl"
({void*_tmpD9=0;({struct _dyneither_ptr _tmp523=({const char*_tmpDA="Warning: use of mode HI deleted\n";_tag_dyneither(_tmpDA,sizeof(char),33);});Cyc_log(_tmp523,_tag_dyneither(_tmpD9,sizeof(void*),0));});});
return 1;case 9: _LLFF: _LL100:
# 622
({void*_tmpDB=0;({struct _dyneither_ptr _tmp524=({const char*_tmpDC="Warning: use of mode SI deleted\n";_tag_dyneither(_tmpDC,sizeof(char),33);});Cyc_log(_tmp524,_tag_dyneither(_tmpDB,sizeof(void*),0));});});
return 1;case 10: _LL101: _LL102:
# 625
({void*_tmpDD=0;({struct _dyneither_ptr _tmp525=({const char*_tmpDE="Warning: use of mode QI deleted\n";_tag_dyneither(_tmpDE,sizeof(char),33);});Cyc_log(_tmp525,_tag_dyneither(_tmpDD,sizeof(void*),0));});});
return 1;case 11: _LL103: _LL104:
# 628
({void*_tmpDF=0;({struct _dyneither_ptr _tmp526=({const char*_tmpE0="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpE0,sizeof(char),33);});Cyc_log(_tmp526,_tag_dyneither(_tmpDF,sizeof(void*),0));});});
return 1;case 12: _LL105: _LL106:
# 631
({void*_tmpE1=0;({struct _dyneither_ptr _tmp527=({const char*_tmpE2="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpE2,sizeof(char),33);});Cyc_log(_tmp527,_tag_dyneither(_tmpE1,sizeof(void*),0));});});
return 1;case 13: _LL107: _LL108:
# 634
({void*_tmpE3=0;({struct _dyneither_ptr _tmp528=({const char*_tmpE4="Warning: use of mode word deleted\n";_tag_dyneither(_tmpE4,sizeof(char),35);});Cyc_log(_tmp528,_tag_dyneither(_tmpE3,sizeof(void*),0));});});
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
({void*_tmpE5=0;({struct _dyneither_ptr _tmp529=({const char*_tmpE6="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmpE6,sizeof(char),42);});Cyc_log(_tmp529,_tag_dyneither(_tmpE5,sizeof(void*),0));});});
return 1;case 21: _LL117: _LL118:
# 658
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpE7=0;({struct _dyneither_ptr _tmp52A=({const char*_tmpE8="Warning: replacing use of __asm with 0\n";_tag_dyneither(_tmpE8,sizeof(char),40);});Cyc_log(_tmp52A,_tag_dyneither(_tmpE7,sizeof(void*),0));});});
return 1;case 22: _LL119: _LL11A:
# 664
({int _tmp52B=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp52B,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL11B: _LL11C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LLEC:;}
# 668
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct Cyc_Lexing_Error_exn_struct _tmp52D=({struct Cyc_Lexing_Error_exn_struct _tmpEA;_tmpEA.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp52C=({const char*_tmpEB="some action didn't return!";_tag_dyneither(_tmpEB,sizeof(char),27);});_tmpEA.f1=_tmp52C;});_tmpEA;});_tmpE9[0]=_tmp52D;});_tmpE9;}));}
# 670
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp52E=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp52E;});
{int _tmpEC=lexstate;switch(_tmpEC){case 0: _LL11E: _LL11F:
# 668 "buildlib.cyl"
 return 0;case 1: _LL120: _LL121:
# 670
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL122: _LL123:
# 672
({void*_tmpED=0;({struct _dyneither_ptr _tmp52F=({const char*_tmpEE="Warning: unclosed string\n";_tag_dyneither(_tmpEE,sizeof(char),26);});Cyc_log(_tmp52F,_tag_dyneither(_tmpED,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0;({struct _dyneither_ptr _tmp530=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpF1.f1=_tmp530;});({void*_tmpEF[1]={& _tmpF1};({struct Cyc___cycFILE*_tmp532=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp531=({const char*_tmpF0="%s";_tag_dyneither(_tmpF0,sizeof(char),3);});Cyc_fprintf(_tmp532,_tmp531,_tag_dyneither(_tmpEF,sizeof(void*),1));});});});return 1;case 3: _LL124: _LL125:
# 675
({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0;({struct _dyneither_ptr _tmp533=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpF4.f1=_tmp533;});({void*_tmpF2[1]={& _tmpF4};({struct Cyc___cycFILE*_tmp535=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp534=({const char*_tmpF3="%s";_tag_dyneither(_tmpF3,sizeof(char),3);});Cyc_fprintf(_tmp535,_tmp534,_tag_dyneither(_tmpF2,sizeof(void*),1));});});});return 1;case 4: _LL126: _LL127:
# 677
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0;({struct _dyneither_ptr _tmp536=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpF7.f1=_tmp536;});({void*_tmpF5[1]={& _tmpF7};({struct Cyc___cycFILE*_tmp538=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp537=({const char*_tmpF6="%s";_tag_dyneither(_tmpF6,sizeof(char),3);});Cyc_fprintf(_tmp538,_tmp537,_tag_dyneither(_tmpF5,sizeof(void*),1));});});});return 1;case 5: _LL128: _LL129:
# 679
({struct Cyc_String_pa_PrintArg_struct _tmpFA;_tmpFA.tag=0;({struct _dyneither_ptr _tmp539=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpFA.f1=_tmp539;});({void*_tmpF8[1]={& _tmpFA};({struct Cyc___cycFILE*_tmp53B=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp53A=({const char*_tmpF9="%s";_tag_dyneither(_tmpF9,sizeof(char),3);});Cyc_fprintf(_tmp53B,_tmp53A,_tag_dyneither(_tmpF8,sizeof(void*),1));});});});return 1;case 6: _LL12A: _LL12B:
# 681
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;({struct _dyneither_ptr _tmp53C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmpFD.f1=_tmp53C;});({void*_tmpFB[1]={& _tmpFD};({struct Cyc___cycFILE*_tmp53E=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp53D=({const char*_tmpFC="%s";_tag_dyneither(_tmpFC,sizeof(char),3);});Cyc_fprintf(_tmp53E,_tmp53D,_tag_dyneither(_tmpFB,sizeof(void*),1));});});});return 1;case 7: _LL12C: _LL12D:
# 683
({struct Cyc_String_pa_PrintArg_struct _tmp100;_tmp100.tag=0;({struct _dyneither_ptr _tmp53F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp100.f1=_tmp53F;});({void*_tmpFE[1]={& _tmp100};({struct Cyc___cycFILE*_tmp541=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp540=({const char*_tmpFF="%s";_tag_dyneither(_tmpFF,sizeof(char),3);});Cyc_fprintf(_tmp541,_tmp540,_tag_dyneither(_tmpFE,sizeof(void*),1));});});});return 1;case 8: _LL12E: _LL12F:
# 685
({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0;({struct _dyneither_ptr _tmp542=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp103.f1=_tmp542;});({void*_tmp101[1]={& _tmp103};({struct Cyc___cycFILE*_tmp544=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp543=({const char*_tmp102="%s";_tag_dyneither(_tmp102,sizeof(char),3);});Cyc_fprintf(_tmp544,_tmp543,_tag_dyneither(_tmp101,sizeof(void*),1));});});});return 1;default: _LL130: _LL131:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL11D:;}
# 689
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp104=_cycalloc(sizeof(*_tmp104));({struct Cyc_Lexing_Error_exn_struct _tmp546=({struct Cyc_Lexing_Error_exn_struct _tmp105;_tmp105.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp545=({const char*_tmp106="some action didn't return!";_tag_dyneither(_tmp106,sizeof(char),27);});_tmp105.f1=_tmp545;});_tmp105;});_tmp104[0]=_tmp546;});_tmp104;}));}
# 691
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp547=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp547;});
{int _tmp107=lexstate;switch(_tmp107){case 0: _LL133: _LL134:
# 695 "buildlib.cyl"
 return 0;case 1: _LL135: _LL136:
# 697
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LL137: _LL138:
# 701
 ++ Cyc_parens_to_match;
return 1;case 3: _LL139: _LL13A:
# 704
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LL13B: _LL13C:
# 707
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LL13D: _LL13E:
# 710
 return 1;case 6: _LL13F: _LL140:
# 712
 return 1;default: _LL141: _LL142:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL132:;}
# 716
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp108=_cycalloc(sizeof(*_tmp108));({struct Cyc_Lexing_Error_exn_struct _tmp549=({struct Cyc_Lexing_Error_exn_struct _tmp109;_tmp109.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp548=({const char*_tmp10A="some action didn't return!";_tag_dyneither(_tmp10A,sizeof(char),27);});_tmp109.f1=_tmp548;});_tmp109;});_tmp108[0]=_tmp549;});_tmp108;}));}
# 718
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp54A=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp54A;});
{int _tmp10B=lexstate;switch(_tmp10B){case 0: _LL144: _LL145:
# 716 "buildlib.cyl"
({void*_tmp10C=0;({struct _dyneither_ptr _tmp54B=({const char*_tmp10D="Warning: unclosed string\n";_tag_dyneither(_tmp10D,sizeof(char),26);});Cyc_log(_tmp54B,_tag_dyneither(_tmp10C,sizeof(void*),0));});});return 0;case 1: _LL146: _LL147:
# 718
 return 0;case 2: _LL148: _LL149:
# 720
({void*_tmp10E=0;({struct _dyneither_ptr _tmp54C=({const char*_tmp10F="Warning: unclosed string\n";_tag_dyneither(_tmp10F,sizeof(char),26);});Cyc_log(_tmp54C,_tag_dyneither(_tmp10E,sizeof(void*),0));});});return 1;case 3: _LL14A: _LL14B:
# 722
 return 1;case 4: _LL14C: _LL14D:
# 724
 return 1;case 5: _LL14E: _LL14F:
# 726
 return 1;case 6: _LL150: _LL151:
# 728
 return 1;case 7: _LL152: _LL153:
# 730
 return 1;case 8: _LL154: _LL155:
# 732
 return 1;default: _LL156: _LL157:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL143:;}
# 736
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Lexing_Error_exn_struct _tmp54E=({struct Cyc_Lexing_Error_exn_struct _tmp111;_tmp111.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp54D=({const char*_tmp112="some action didn't return!";_tag_dyneither(_tmp112,sizeof(char),27);});_tmp111.f1=_tmp54D;});_tmp111;});_tmp110[0]=_tmp54E;});_tmp110;}));}
# 738
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp54F=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp54F;});
{int _tmp113=lexstate;switch(_tmp113){case 0: _LL159: _LL15A:
# 736 "buildlib.cyl"
({void*_tmp114=0;({struct _dyneither_ptr _tmp550=({const char*_tmp115="Warning: unclosed comment\n";_tag_dyneither(_tmp115,sizeof(char),27);});Cyc_log(_tmp550,_tag_dyneither(_tmp114,sizeof(void*),0));});});return 0;case 1: _LL15B: _LL15C:
# 738
 return 0;case 2: _LL15D: _LL15E:
# 740
 return 1;default: _LL15F: _LL160:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL158:;}
# 744
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Lexing_Error_exn_struct _tmp552=({struct Cyc_Lexing_Error_exn_struct _tmp117;_tmp117.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp551=({const char*_tmp118="some action didn't return!";_tag_dyneither(_tmp118,sizeof(char),27);});_tmp117.f1=_tmp551;});_tmp117;});_tmp116[0]=_tmp552;});_tmp116;}));}
# 746
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple22*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp553=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp553;});
{int _tmp119=lexstate;switch(_tmp119){case 0: _LL162: _LL163:
# 748 "buildlib.cyl"
({struct _dyneither_ptr _tmp554=({const char*_tmp11A="#define ";_tag_dyneither(_tmp11A,sizeof(char),9);});Cyc_current_line=_tmp554;});
Cyc_suck_macroname(lexbuf);
return({struct _tuple22*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->f1=Cyc_current_line;_tmp11B->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp11B;});case 1: _LL164: _LL165:
# 752
 return Cyc_suck_line(lexbuf);case 2: _LL166: _LL167:
# 754
 return 0;default: _LL168: _LL169:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL161:;}
# 758
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_Lexing_Error_exn_struct _tmp556=({struct Cyc_Lexing_Error_exn_struct _tmp11D;_tmp11D.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp555=({const char*_tmp11E="some action didn't return!";_tag_dyneither(_tmp11E,sizeof(char),27);});_tmp11D.f1=_tmp555;});_tmp11D;});_tmp11C[0]=_tmp556;});_tmp11C;}));}
# 760
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp557=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp557;});
{int _tmp11F=lexstate;if(_tmp11F == 0){_LL16B: _LL16C:
# 758 "buildlib.cyl"
({struct _dyneither_ptr*_tmp559=({struct _dyneither_ptr*_tmp120=_cycalloc(sizeof(*_tmp120));({struct _dyneither_ptr _tmp558=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp120[0]=_tmp558;});_tmp120;});Cyc_current_source=_tmp559;});
({struct _dyneither_ptr _tmp55A=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));Cyc_current_line=_tmp55A;});
return Cyc_suck_restofline(lexbuf);}else{_LL16D: _LL16E:
# 762
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL16A:;}
# 765
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Lexing_Error_exn_struct _tmp55C=({struct Cyc_Lexing_Error_exn_struct _tmp122;_tmp122.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp55B=({const char*_tmp123="some action didn't return!";_tag_dyneither(_tmp123,sizeof(char),27);});_tmp122.f1=_tmp55B;});_tmp122;});_tmp121[0]=_tmp55C;});_tmp121;}));}
# 767
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp55D=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp55D;});
{int _tmp124=lexstate;if(_tmp124 == 0){_LL170: _LL171:
# 765 "buildlib.cyl"
({struct _dyneither_ptr _tmp55F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp55E=(struct _dyneither_ptr)Cyc_current_line;Cyc_strconcat(_tmp55E,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});Cyc_current_line=_tmp55F;});return 0;}else{_LL172: _LL173:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL16F:;}
# 769
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp125=_cycalloc(sizeof(*_tmp125));({struct Cyc_Lexing_Error_exn_struct _tmp561=({struct Cyc_Lexing_Error_exn_struct _tmp126;_tmp126.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp560=({const char*_tmp127="some action didn't return!";_tag_dyneither(_tmp127,sizeof(char),27);});_tmp126.f1=_tmp560;});_tmp126;});_tmp125[0]=_tmp561;});_tmp125;}));}
# 771
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple25*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp562=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp562;});
{int _tmp128=lexstate;switch(_tmp128){case 0: _LL175: _LL176:
# 772 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL177: _LL178:
# 774
({struct _dyneither_ptr _tmp565=(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp564=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp564,0,(unsigned long)(({
int _tmp563=Cyc_Lexing_lexeme_end(lexbuf);_tmp563 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 774
Cyc_current_headerfile=_tmp565;});
# 777
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
({struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_hstubs=_tmp566;});
({struct Cyc_List_List*_tmp567=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cstubs=_tmp567;});
({struct Cyc_List_List*_tmp568=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);Cyc_current_cycstubs=_tmp568;});
return({struct _tuple25*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->f1=Cyc_current_headerfile;_tmp129->f2=Cyc_current_symbols;_tmp129->f3=Cyc_current_user_defs;_tmp129->f4=Cyc_current_omit_symbols;_tmp129->f5=Cyc_current_hstubs;_tmp129->f6=Cyc_current_cstubs;_tmp129->f7=Cyc_current_cycstubs;_tmp129;});case 2: _LL179: _LL17A:
# 796
 return Cyc_spec(lexbuf);case 3: _LL17B: _LL17C:
# 798
 return 0;case 4: _LL17D: _LL17E:
# 800
({struct Cyc_Int_pa_PrintArg_struct _tmp12C;_tmp12C.tag=1;({unsigned long _tmp569=(unsigned long)((int)
# 802
Cyc_Lexing_lexeme_char(lexbuf,0));_tmp12C.f1=_tmp569;});({void*_tmp12A[1]={& _tmp12C};({struct Cyc___cycFILE*_tmp56B=Cyc_stderr;struct _dyneither_ptr _tmp56A=({const char*_tmp12B="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp12B,sizeof(char),67);});Cyc_fprintf(_tmp56B,_tmp56A,_tag_dyneither(_tmp12A,sizeof(void*),1));});});});
return 0;default: _LL17F: _LL180:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL174:;}
# 807
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct Cyc_Lexing_Error_exn_struct _tmp56D=({struct Cyc_Lexing_Error_exn_struct _tmp12E;_tmp12E.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp56C=({const char*_tmp12F="some action didn't return!";_tag_dyneither(_tmp12F,sizeof(char),27);});_tmp12E.f1=_tmp56C;});_tmp12E;});_tmp12D[0]=_tmp56D;});_tmp12D;}));}
# 809
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp56E=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp56E;});
{int _tmp130=lexstate;switch(_tmp130){case 0: _LL182: _LL183:
# 807 "buildlib.cyl"
 return 0;case 1: _LL184: _LL185:
# 809
 return 0;case 2: _LL186: _LL187:
# 811
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
({struct Cyc_List_List*_tmp56F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);Cyc_current_symbols=_tmp56F;});
return 1;case 3: _LL188: _LL189:
# 816
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp131=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(_tmp131 != Cyc_current_user_defs){
({void*_tmp132=0;({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _dyneither_ptr _tmp570=({const char*_tmp133="Error in .cys file: got optional definition in omitsymbols\n";_tag_dyneither(_tmp133,sizeof(char),60);});Cyc_fprintf(_tmp571,_tmp570,_tag_dyneither(_tmp132,sizeof(void*),0));});});
# 822
return 0;}
# 824
({struct Cyc_List_List*_tmp572=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);Cyc_current_omit_symbols=_tmp572;});
return 1;};case 4: _LL18A: _LL18B:
# 827
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp573=Cyc_Buffer_create(255);Cyc_specbuf=_tmp573;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp135=_cycalloc(sizeof(*_tmp135));({struct _dyneither_ptr _tmp574=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp135->f1=_tmp574;});({struct _dyneither_ptr _tmp575=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp135->f2=_tmp575;});_tmp135;});
({struct Cyc_List_List*_tmp576=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=x;_tmp134->tl=Cyc_current_hstubs;_tmp134;});Cyc_current_hstubs=_tmp576;});
return 1;};case 5: _LL18C: _LL18D: {
# 835
struct _dyneither_ptr _tmp136=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp136,sizeof(char),(int)Cyc_strlen(({const char*_tmp137="hstub";_tag_dyneither(_tmp137,sizeof(char),6);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp136,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp136,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp136;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp138=Cyc_substring((struct _dyneither_ptr)_tmp136,0,(unsigned long)((t.curr - _tmp136.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp577=Cyc_Buffer_create(255);Cyc_specbuf=_tmp577;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->f1=(struct _dyneither_ptr)_tmp138;({struct _dyneither_ptr _tmp578=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp13A->f2=_tmp578;});_tmp13A;});
({struct Cyc_List_List*_tmp579=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=x;_tmp139->tl=Cyc_current_hstubs;_tmp139;});Cyc_current_hstubs=_tmp579;});
return 1;};};};}case 6: _LL18E: _LL18F:
# 849
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp57A=Cyc_Buffer_create(255);Cyc_specbuf=_tmp57A;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp13C=_cycalloc(sizeof(*_tmp13C));({struct _dyneither_ptr _tmp57B=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp13C->f1=_tmp57B;});({struct _dyneither_ptr _tmp57C=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp13C->f2=_tmp57C;});_tmp13C;});
({struct Cyc_List_List*_tmp57D=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=x;_tmp13B->tl=Cyc_current_cstubs;_tmp13B;});Cyc_current_cstubs=_tmp57D;});
return 1;};case 7: _LL190: _LL191: {
# 857
struct _dyneither_ptr _tmp13D=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp13D,sizeof(char),(int)Cyc_strlen(({const char*_tmp13E="cstub";_tag_dyneither(_tmp13E,sizeof(char),6);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp13D,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp13D,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp13D;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp13F=Cyc_substring((struct _dyneither_ptr)_tmp13D,0,(unsigned long)((t.curr - _tmp13D.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp57E=Cyc_Buffer_create(255);Cyc_specbuf=_tmp57E;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=(struct _dyneither_ptr)_tmp13F;({struct _dyneither_ptr _tmp57F=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp141->f2=_tmp57F;});_tmp141;});
({struct Cyc_List_List*_tmp580=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=x;_tmp140->tl=Cyc_current_cstubs;_tmp140;});Cyc_current_cstubs=_tmp580;});
return 1;};};};}case 8: _LL192: _LL193:
# 871
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp581=Cyc_Buffer_create(255);Cyc_specbuf=_tmp581;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp143=_cycalloc(sizeof(*_tmp143));({struct _dyneither_ptr _tmp582=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp143->f1=_tmp582;});({struct _dyneither_ptr _tmp583=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp143->f2=_tmp583;});_tmp143;});
({struct Cyc_List_List*_tmp584=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=x;_tmp142->tl=Cyc_current_cycstubs;_tmp142;});Cyc_current_cycstubs=_tmp584;});
return 1;};case 9: _LL194: _LL195: {
# 879
struct _dyneither_ptr _tmp144=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp144,sizeof(char),(int)Cyc_strlen(({const char*_tmp145="cycstub";_tag_dyneither(_tmp145,sizeof(char),8);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp144,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp144,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp144;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp146=Cyc_substring((struct _dyneither_ptr)_tmp144,0,(unsigned long)((t.curr - _tmp144.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp585=Cyc_Buffer_create(255);Cyc_specbuf=_tmp585;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->f1=(struct _dyneither_ptr)_tmp146;({struct _dyneither_ptr _tmp586=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp148->f2=_tmp586;});_tmp148;});
({struct Cyc_List_List*_tmp587=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->hd=x;_tmp147->tl=Cyc_current_cycstubs;_tmp147;});Cyc_current_cycstubs=_tmp587;});
return 1;};};};}case 10: _LL196: _LL197:
# 893
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp588=Cyc_Buffer_create(255);Cyc_specbuf=_tmp588;});
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmp14A=_cycalloc(sizeof(*_tmp14A));({struct _dyneither_ptr _tmp589=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp14A[0]=_tmp589;});_tmp14A;});
({struct Cyc_List_List*_tmp58A=({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=x;_tmp149->tl=Cyc_current_cpp;_tmp149;});Cyc_current_cpp=_tmp58A;});
return 1;};case 11: _LL198: _LL199:
# 900
 return 1;case 12: _LL19A: _LL19B:
# 902
 return 1;case 13: _LL19C: _LL19D:
# 904
({struct Cyc_Int_pa_PrintArg_struct _tmp14D;_tmp14D.tag=1;({unsigned long _tmp58B=(unsigned long)((int)
# 906
Cyc_Lexing_lexeme_char(lexbuf,0));_tmp14D.f1=_tmp58B;});({void*_tmp14B[1]={& _tmp14D};({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _dyneither_ptr _tmp58C=({const char*_tmp14C="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmp14C,sizeof(char),58);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_dyneither(_tmp14B,sizeof(void*),1));});});});
return 0;default: _LL19E: _LL19F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL181:;}
# 911
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14E=_cycalloc(sizeof(*_tmp14E));({struct Cyc_Lexing_Error_exn_struct _tmp58F=({struct Cyc_Lexing_Error_exn_struct _tmp14F;_tmp14F.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp58E=({const char*_tmp150="some action didn't return!";_tag_dyneither(_tmp150,sizeof(char),27);});_tmp14F.f1=_tmp58E;});_tmp14F;});_tmp14E[0]=_tmp58F;});_tmp14E;}));}
# 913
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp590=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp590;});
{int _tmp151=lexstate;switch(_tmp151){case 0: _LL1A1: _LL1A2:
# 917 "buildlib.cyl"
({struct Cyc_List_List*_tmp593=({struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));({struct _dyneither_ptr*_tmp592=({struct _dyneither_ptr*_tmp153=_cycalloc(sizeof(*_tmp153));({struct _dyneither_ptr _tmp591=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp153[0]=_tmp591;});_tmp153;});_tmp152->hd=_tmp592;});_tmp152->tl=Cyc_snarfed_symbols;_tmp152;});Cyc_snarfed_symbols=_tmp593;});
return 1;case 1: _LL1A3: _LL1A4: {
# 920
struct _dyneither_ptr _tmp154=Cyc_Lexing_lexeme(lexbuf);
struct _dyneither_ptr t=_tmp154;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
({struct _dyneither_ptr _tmp594=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp154,0,(unsigned long)((t.curr - _tmp154.curr)/ sizeof(char)));Cyc_current_symbol=_tmp594;});
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp595=Cyc_Buffer_create(255);Cyc_specbuf=_tmp595;});
while(Cyc_block(lexbuf)){;}
# 929
Cyc_rename_current_symbol=0;{
struct _tuple24*user_def=({struct _tuple24*_tmp158=_cycalloc(sizeof(*_tmp158));({struct _dyneither_ptr*_tmp596=({struct _dyneither_ptr*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=Cyc_current_symbol;_tmp15A;});_tmp158->f1=_tmp596;});({struct _dyneither_ptr*_tmp598=({struct _dyneither_ptr*_tmp159=_cycalloc(sizeof(*_tmp159));({struct _dyneither_ptr _tmp597=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp159[0]=_tmp597;});_tmp159;});_tmp158->f2=_tmp598;});_tmp158;});
({struct Cyc_List_List*_tmp59A=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _dyneither_ptr*_tmp599=({struct _dyneither_ptr*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=(struct _dyneither_ptr)Cyc_current_symbol;_tmp156;});_tmp155->hd=_tmp599;});_tmp155->tl=Cyc_snarfed_symbols;_tmp155;});Cyc_snarfed_symbols=_tmp59A;});
({struct Cyc_List_List*_tmp59B=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->hd=user_def;_tmp157->tl=Cyc_current_user_defs;_tmp157;});Cyc_current_user_defs=_tmp59B;});
return 1;};}case 2: _LL1A5: _LL1A6:
# 936
 return 1;case 3: _LL1A7: _LL1A8:
# 938
 return 0;case 4: _LL1A9: _LL1AA:
# 940
({void*_tmp15B=0;({struct Cyc___cycFILE*_tmp59D=Cyc_stderr;struct _dyneither_ptr _tmp59C=({const char*_tmp15C="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmp15C,sizeof(char),44);});Cyc_fprintf(_tmp59D,_tmp59C,_tag_dyneither(_tmp15B,sizeof(void*),0));});});
# 942
return 0;case 5: _LL1AB: _LL1AC:
# 944
({struct Cyc_Int_pa_PrintArg_struct _tmp15F;_tmp15F.tag=1;({unsigned long _tmp59E=(unsigned long)((int)
# 946
Cyc_Lexing_lexeme_char(lexbuf,0));_tmp15F.f1=_tmp59E;});({void*_tmp15D[1]={& _tmp15F};({struct Cyc___cycFILE*_tmp5A0=Cyc_stderr;struct _dyneither_ptr _tmp59F=({const char*_tmp15E="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmp15E,sizeof(char),57);});Cyc_fprintf(_tmp5A0,_tmp59F,_tag_dyneither(_tmp15D,sizeof(void*),1));});});});
return 0;default: _LL1AD: _LL1AE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL1A0:;}
# 951
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp160=_cycalloc(sizeof(*_tmp160));({struct Cyc_Lexing_Error_exn_struct _tmp5A2=({struct Cyc_Lexing_Error_exn_struct _tmp161;_tmp161.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5A1=({const char*_tmp162="some action didn't return!";_tag_dyneither(_tmp162,sizeof(char),27);});_tmp161.f1=_tmp5A1;});_tmp161;});_tmp160[0]=_tmp5A2;});_tmp160;}));}
# 953
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5A3=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5A3;});
{int _tmp163=lexstate;switch(_tmp163){case 0: _LL1B0: _LL1B1:
# 957 "buildlib.cyl"
({void*_tmp164=0;({struct _dyneither_ptr _tmp5A4=({const char*_tmp165="Warning: unclosed brace\n";_tag_dyneither(_tmp165,sizeof(char),25);});Cyc_log(_tmp5A4,_tag_dyneither(_tmp164,sizeof(void*),0));});});return 0;case 1: _LL1B2: _LL1B3:
# 959
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LL1B4: _LL1B5:
# 964
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LL1B6: _LL1B7:
# 968
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LL1B8: _LL1B9:
# 972
({struct Cyc_Buffer_t*_tmp5A5=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5A5,({const char*_tmp166="/*";_tag_dyneither(_tmp166,sizeof(char),3);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LL1BA: _LL1BB:
# 976
({struct Cyc_Buffer_t*_tmp5A6=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5A6,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6: _LL1BC: _LL1BD: {
# 979
struct _dyneither_ptr _tmp167=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol  && !Cyc_strcmp((struct _dyneither_ptr)_tmp167,(struct _dyneither_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp5A7=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5A7,*Cyc_add_user_prefix(({struct _dyneither_ptr*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=_tmp167;_tmp168;})));});else{
# 983
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tmp167);}
return 1;}case 7: _LL1BE: _LL1BF:
# 986
({struct Cyc_Buffer_t*_tmp5A8=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp5A8,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL1C0: _LL1C1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL1AF:;}
# 991
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp169=_cycalloc(sizeof(*_tmp169));({struct Cyc_Lexing_Error_exn_struct _tmp5AA=({struct Cyc_Lexing_Error_exn_struct _tmp16A;_tmp16A.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5A9=({const char*_tmp16B="some action didn't return!";_tag_dyneither(_tmp16B,sizeof(char),27);});_tmp16A.f1=_tmp5A9;});_tmp16A;});_tmp169[0]=_tmp5AA;});_tmp169;}));}
# 993
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5AB=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5AB;});
{int _tmp16C=lexstate;switch(_tmp16C){case 0: _LL1C3: _LL1C4:
# 991 "buildlib.cyl"
({void*_tmp16D=0;({struct _dyneither_ptr _tmp5AC=({const char*_tmp16E="Warning: unclosed string\n";_tag_dyneither(_tmp16E,sizeof(char),26);});Cyc_log(_tmp5AC,_tag_dyneither(_tmp16D,sizeof(void*),0));});});return 0;case 1: _LL1C5: _LL1C6:
# 993
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LL1C7: _LL1C8:
# 995
({void*_tmp16F=0;({struct _dyneither_ptr _tmp5AD=({const char*_tmp170="Warning: unclosed string\n";_tag_dyneither(_tmp170,sizeof(char),26);});Cyc_log(_tmp5AD,_tag_dyneither(_tmp16F,sizeof(void*),0));});});
({struct Cyc_Buffer_t*_tmp5AE=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5AE,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3: _LL1C9: _LL1CA:
# 999
({struct Cyc_Buffer_t*_tmp5AF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5AF,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4: _LL1CB: _LL1CC:
# 1002
({struct Cyc_Buffer_t*_tmp5B0=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B0,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5: _LL1CD: _LL1CE:
# 1005
({struct Cyc_Buffer_t*_tmp5B1=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B1,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6: _LL1CF: _LL1D0:
# 1008
({struct Cyc_Buffer_t*_tmp5B2=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B2,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7: _LL1D1: _LL1D2:
# 1011
({struct Cyc_Buffer_t*_tmp5B3=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B3,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8: _LL1D3: _LL1D4:
# 1014
({struct Cyc_Buffer_t*_tmp5B4=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B4,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL1D5: _LL1D6:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL1C2:;}
# 1019
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp171=_cycalloc(sizeof(*_tmp171));({struct Cyc_Lexing_Error_exn_struct _tmp5B6=({struct Cyc_Lexing_Error_exn_struct _tmp172;_tmp172.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5B5=({const char*_tmp173="some action didn't return!";_tag_dyneither(_tmp173,sizeof(char),27);});_tmp172.f1=_tmp5B5;});_tmp172;});_tmp171[0]=_tmp5B6;});_tmp171;}));}
# 1021
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5B7=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5B7;});
{int _tmp174=lexstate;switch(_tmp174){case 0: _LL1D8: _LL1D9:
# 1019 "buildlib.cyl"
({void*_tmp175=0;({struct _dyneither_ptr _tmp5B8=({const char*_tmp176="Warning: unclosed comment\n";_tag_dyneither(_tmp176,sizeof(char),27);});Cyc_log(_tmp5B8,_tag_dyneither(_tmp175,sizeof(void*),0));});});return 0;case 1: _LL1DA: _LL1DB:
# 1021
({struct Cyc_Buffer_t*_tmp5B9=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B9,({const char*_tmp177="*/";_tag_dyneither(_tmp177,sizeof(char),3);}));});return 0;case 2: _LL1DC: _LL1DD:
# 1023
({struct Cyc_Buffer_t*_tmp5BA=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BA,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL1DE: _LL1DF:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL1D7:;}
# 1028
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp178=_cycalloc(sizeof(*_tmp178));({struct Cyc_Lexing_Error_exn_struct _tmp5BC=({struct Cyc_Lexing_Error_exn_struct _tmp179;_tmp179.tag=Cyc_Lexing_Error;({struct _dyneither_ptr _tmp5BB=({const char*_tmp17A="some action didn't return!";_tag_dyneither(_tmp17A,sizeof(char),27);});_tmp179.f1=_tmp5BB;});_tmp179;});_tmp178[0]=_tmp5BC;});_tmp178;}));}
# 1030
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 1033 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 1036
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmp17B=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmp17C=_tmp17B;struct Cyc_List_List*_tmp1C8;void*_tmp1C7;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct _dyneither_ptr*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct _dyneither_ptr*_tmp1C2;void*_tmp1C1;void*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;int _tmp1BE;struct Cyc_Absyn_Exp*_tmp1BD;void**_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BB;void*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_List_List*_tmp1A5;void*_tmp1A4;switch(*((int*)_tmp17C)){case 1: _LL1E1: _tmp1A4=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL1E2: {
# 1040
struct _dyneither_ptr*_tmp17D=(*Cyc_Absyn_binding2qvar(_tmp1A4)).f2;
Cyc_add_target(_tmp17D);
return;}case 2: _LL1E3: _tmp1A5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1E4:
# 1044
 for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1A5->hd,dep);}
# 1047
return;case 22: _LL1E5: _tmp1A7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1A6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1E6:
# 1049
 _tmp1A9=_tmp1A7;_tmp1A8=_tmp1A6;goto _LL1E8;case 8: _LL1E7: _tmp1A9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1A8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1E8:
# 1051
 _tmp1AB=_tmp1A9;_tmp1AA=_tmp1A8;goto _LL1EA;case 3: _LL1E9: _tmp1AB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1AA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17C)->f3;_LL1EA:
# 1053
 Cyc_scan_exp(_tmp1AB,dep);
Cyc_scan_exp(_tmp1AA,dep);
return;case 19: _LL1EB: _tmp1AC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL1EC:
# 1057
 _tmp1AD=_tmp1AC;goto _LL1EE;case 17: _LL1ED: _tmp1AD=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL1EE:
# 1059
 _tmp1AE=_tmp1AD;goto _LL1F0;case 14: _LL1EF: _tmp1AE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL1F0:
# 1061
 _tmp1AF=_tmp1AE;goto _LL1F2;case 4: _LL1F1: _tmp1AF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL1F2:
# 1063
 Cyc_scan_exp(_tmp1AF,dep);
return;case 5: _LL1F3: _tmp1B2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_tmp1B0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17C)->f3;_LL1F4:
# 1066
 Cyc_scan_exp(_tmp1B2,dep);
Cyc_scan_exp(_tmp1B1,dep);
Cyc_scan_exp(_tmp1B0,dep);
return;case 6: _LL1F5: _tmp1B4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1F6:
 _tmp1B6=_tmp1B4;_tmp1B5=_tmp1B3;goto _LL1F8;case 7: _LL1F7: _tmp1B6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1F8:
# 1072
 Cyc_scan_exp(_tmp1B6,dep);
Cyc_scan_exp(_tmp1B5,dep);
return;case 9: _LL1F9: _tmp1B8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1FA:
# 1076
 Cyc_scan_exp(_tmp1B8,dep);
for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1B7->hd,dep);}
# 1080
return;case 13: _LL1FB: _tmp1BA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1B9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL1FC:
# 1082
 Cyc_scan_type(_tmp1BA,dep);
Cyc_scan_exp(_tmp1B9,dep);
return;case 33: _LL1FD: _tmp1BE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).is_calloc;_tmp1BD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).rgn;_tmp1BC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).elt_type;_tmp1BB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17C)->f1).num_elts;_LL1FE:
# 1086
 if(_tmp1BD != 0)Cyc_scan_exp(_tmp1BD,dep);
if(_tmp1BC != 0)Cyc_scan_type(*_tmp1BC,dep);
Cyc_scan_exp(_tmp1BB,dep);
return;case 37: _LL1FF: _tmp1BF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL200:
# 1091
 Cyc_scan_exp(_tmp1BF,dep);return;case 38: _LL201: _tmp1C0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL202:
 _tmp1C1=_tmp1C0;goto _LL204;case 16: _LL203: _tmp1C1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_LL204:
# 1094
 Cyc_scan_type(_tmp1C1,dep);
return;case 20: _LL205: _tmp1C3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL206:
# 1097
 _tmp1C5=_tmp1C3;_tmp1C4=_tmp1C2;goto _LL208;case 21: _LL207: _tmp1C5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1C4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL208:
# 1099
 Cyc_scan_exp(_tmp1C5,dep);
Cyc_add_target(_tmp1C4);
return;case 18: _LL209: _tmp1C7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17C)->f1;_tmp1C6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL20A:
# 1103
 Cyc_scan_type(_tmp1C7,dep);
# 1105
{void*_tmp17E=(void*)((struct Cyc_List_List*)_check_null(_tmp1C6))->hd;void*_tmp17F=_tmp17E;struct _dyneither_ptr*_tmp180;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp17F)->tag == 0){_LL232: _tmp180=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp17F)->f1;_LL233:
 Cyc_add_target(_tmp180);goto _LL231;}else{_LL234: _LL235:
 goto _LL231;}_LL231:;}
# 1109
return;case 0: _LL20B: _LL20C:
# 1111
 return;case 35: _LL20D: _tmp1C8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp17C)->f2;_LL20E:
# 1113
 for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
struct _tuple28*_tmp181=(struct _tuple28*)_tmp1C8->hd;struct _tuple28*_tmp182=_tmp181;struct Cyc_Absyn_Exp*_tmp183;_LL237: _tmp183=_tmp182->f2;_LL238:;
Cyc_scan_exp(_tmp183,dep);}
# 1117
return;case 39: _LL20F: _LL210:
 return;case 34: _LL211: _LL212:
# 1120
({void*_tmp184=0;({struct Cyc___cycFILE*_tmp5BE=Cyc_stderr;struct _dyneither_ptr _tmp5BD=({const char*_tmp185="Error: unexpected Swap_e\n";_tag_dyneither(_tmp185,sizeof(char),26);});Cyc_fprintf(_tmp5BE,_tmp5BD,_tag_dyneither(_tmp184,sizeof(void*),0));});});
 exit(1);return;case 36: _LL213: _LL214:
# 1123
({void*_tmp186=0;({struct Cyc___cycFILE*_tmp5C0=Cyc_stderr;struct _dyneither_ptr _tmp5BF=({const char*_tmp187="Error: unexpected Stmt_e\n";_tag_dyneither(_tmp187,sizeof(char),26);});Cyc_fprintf(_tmp5C0,_tmp5BF,_tag_dyneither(_tmp186,sizeof(void*),0));});});
 exit(1);return;case 10: _LL215: _LL216:
# 1126
({void*_tmp188=0;({struct Cyc___cycFILE*_tmp5C2=Cyc_stderr;struct _dyneither_ptr _tmp5C1=({const char*_tmp189="Error: unexpected Throw_e\n";_tag_dyneither(_tmp189,sizeof(char),27);});Cyc_fprintf(_tmp5C2,_tmp5C1,_tag_dyneither(_tmp188,sizeof(void*),0));});});
 exit(1);return;case 11: _LL217: _LL218:
# 1129
({void*_tmp18A=0;({struct Cyc___cycFILE*_tmp5C4=Cyc_stderr;struct _dyneither_ptr _tmp5C3=({const char*_tmp18B="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp18B,sizeof(char),35);});Cyc_fprintf(_tmp5C4,_tmp5C3,_tag_dyneither(_tmp18A,sizeof(void*),0));});});
 exit(1);return;case 12: _LL219: _LL21A:
# 1132
({void*_tmp18C=0;({struct Cyc___cycFILE*_tmp5C6=Cyc_stderr;struct _dyneither_ptr _tmp5C5=({const char*_tmp18D="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp18D,sizeof(char),33);});Cyc_fprintf(_tmp5C6,_tmp5C5,_tag_dyneither(_tmp18C,sizeof(void*),0));});});
 exit(1);return;case 15: _LL21B: _LL21C:
# 1135
({void*_tmp18E=0;({struct Cyc___cycFILE*_tmp5C8=Cyc_stderr;struct _dyneither_ptr _tmp5C7=({const char*_tmp18F="Error: unexpected New_e\n";_tag_dyneither(_tmp18F,sizeof(char),25);});Cyc_fprintf(_tmp5C8,_tmp5C7,_tag_dyneither(_tmp18E,sizeof(void*),0));});});
 exit(1);return;case 23: _LL21D: _LL21E:
# 1138
({void*_tmp190=0;({struct Cyc___cycFILE*_tmp5CA=Cyc_stderr;struct _dyneither_ptr _tmp5C9=({const char*_tmp191="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp191,sizeof(char),27);});Cyc_fprintf(_tmp5CA,_tmp5C9,_tag_dyneither(_tmp190,sizeof(void*),0));});});
 exit(1);return;case 24: _LL21F: _LL220:
# 1141
({void*_tmp192=0;({struct Cyc___cycFILE*_tmp5CC=Cyc_stderr;struct _dyneither_ptr _tmp5CB=({const char*_tmp193="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp193,sizeof(char),33);});Cyc_fprintf(_tmp5CC,_tmp5CB,_tag_dyneither(_tmp192,sizeof(void*),0));});});
 exit(1);return;case 25: _LL221: _LL222:
# 1144
({void*_tmp194=0;({struct Cyc___cycFILE*_tmp5CE=Cyc_stderr;struct _dyneither_ptr _tmp5CD=({const char*_tmp195="Error: unexpected Array_e\n";_tag_dyneither(_tmp195,sizeof(char),27);});Cyc_fprintf(_tmp5CE,_tmp5CD,_tag_dyneither(_tmp194,sizeof(void*),0));});});
 exit(1);return;case 26: _LL223: _LL224:
# 1147
({void*_tmp196=0;({struct Cyc___cycFILE*_tmp5D0=Cyc_stderr;struct _dyneither_ptr _tmp5CF=({const char*_tmp197="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp197,sizeof(char),35);});Cyc_fprintf(_tmp5D0,_tmp5CF,_tag_dyneither(_tmp196,sizeof(void*),0));});});
 exit(1);return;case 27: _LL225: _LL226:
# 1150
({void*_tmp198=0;({struct Cyc___cycFILE*_tmp5D2=Cyc_stderr;struct _dyneither_ptr _tmp5D1=({const char*_tmp199="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmp199,sizeof(char),41);});Cyc_fprintf(_tmp5D2,_tmp5D1,_tag_dyneither(_tmp198,sizeof(void*),0));});});
 exit(1);return;case 28: _LL227: _LL228:
# 1153
({void*_tmp19A=0;({struct Cyc___cycFILE*_tmp5D4=Cyc_stderr;struct _dyneither_ptr _tmp5D3=({const char*_tmp19B="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp19B,sizeof(char),31);});Cyc_fprintf(_tmp5D4,_tmp5D3,_tag_dyneither(_tmp19A,sizeof(void*),0));});});
 exit(1);return;case 29: _LL229: _LL22A:
# 1156
({void*_tmp19C=0;({struct Cyc___cycFILE*_tmp5D6=Cyc_stderr;struct _dyneither_ptr _tmp5D5=({const char*_tmp19D="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp19D,sizeof(char),32);});Cyc_fprintf(_tmp5D6,_tmp5D5,_tag_dyneither(_tmp19C,sizeof(void*),0));});});
 exit(1);return;case 30: _LL22B: _LL22C:
# 1159
({void*_tmp19E=0;({struct Cyc___cycFILE*_tmp5D8=Cyc_stderr;struct _dyneither_ptr _tmp5D7=({const char*_tmp19F="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp19F,sizeof(char),30);});Cyc_fprintf(_tmp5D8,_tmp5D7,_tag_dyneither(_tmp19E,sizeof(void*),0));});});
 exit(1);return;case 31: _LL22D: _LL22E:
# 1162
({void*_tmp1A0=0;({struct Cyc___cycFILE*_tmp5DA=Cyc_stderr;struct _dyneither_ptr _tmp5D9=({const char*_tmp1A1="Error: unexpected Enum_e\n";_tag_dyneither(_tmp1A1,sizeof(char),26);});Cyc_fprintf(_tmp5DA,_tmp5D9,_tag_dyneither(_tmp1A0,sizeof(void*),0));});});
 exit(1);return;default: _LL22F: _LL230:
# 1165
({void*_tmp1A2=0;({struct Cyc___cycFILE*_tmp5DC=Cyc_stderr;struct _dyneither_ptr _tmp5DB=({const char*_tmp1A3="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp1A3,sizeof(char),30);});Cyc_fprintf(_tmp5DC,_tmp5DB,_tag_dyneither(_tmp1A2,sizeof(void*),0));});});
 exit(1);return;}_LL1E0:;}
# 1170
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 1175
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp1C9=t;struct Cyc_Absyn_Datatypedecl*_tmp207;struct Cyc_Absyn_Enumdecl*_tmp206;struct Cyc_Absyn_Aggrdecl*_tmp205;struct _dyneither_ptr*_tmp204;struct _dyneither_ptr*_tmp203;union Cyc_Absyn_AggrInfoU _tmp202;struct Cyc_List_List*_tmp201;struct Cyc_Absyn_FnInfo _tmp200;struct Cyc_Absyn_Exp*_tmp1FF;void*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FD;union Cyc_Absyn_Constraint*_tmp1FC;struct Cyc_Absyn_PtrInfo _tmp1FB;switch(*((int*)_tmp1C9)){case 0: _LL23A: _LL23B:
 goto _LL23D;case 6: _LL23C: _LL23D:
 goto _LL23F;case 28: _LL23E: _LL23F:
 goto _LL241;case 7: _LL240: _LL241:
# 1182
 return;case 5: _LL242: _tmp1FB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C9)->f1;_LL243:
# 1185
 Cyc_scan_type(_tmp1FB.elt_typ,dep);
return;case 8: _LL244: _tmp1FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C9)->f1).elt_type;_tmp1FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C9)->f1).num_elts;_tmp1FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C9)->f1).zero_term;_LL245:
# 1188
 Cyc_scan_type(_tmp1FE,dep);
Cyc_scan_exp_opt(_tmp1FD,dep);
return;case 27: _LL246: _tmp1FF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1C9)->f1;_LL247:
# 1192
 Cyc_scan_exp(_tmp1FF,dep);
return;case 9: _LL248: _tmp200=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1;_LL249:
# 1195
 Cyc_scan_type(_tmp200.ret_typ,dep);
{struct Cyc_List_List*_tmp1CA=_tmp200.args;for(0;_tmp1CA != 0;_tmp1CA=_tmp1CA->tl){
struct _tuple9*_tmp1CB=(struct _tuple9*)_tmp1CA->hd;struct _tuple9*_tmp1CC=_tmp1CB;void*_tmp1CD;_LL279: _tmp1CD=_tmp1CC->f3;_LL27A:;
Cyc_scan_type(_tmp1CD,dep);}}
# 1200
if(_tmp200.cyc_varargs != 0)
Cyc_scan_type((_tmp200.cyc_varargs)->type,dep);
return;case 12: _LL24A: _tmp201=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C9)->f2;_LL24B:
# 1204
 for(0;_tmp201 != 0;_tmp201=_tmp201->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp201->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp201->hd)->width,dep);}
# 1208
return;case 14: _LL24C: _LL24D:
# 1210
 return;case 11: _LL24E: _tmp202=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C9)->f1).aggr_info;_LL24F: {
# 1212
struct _tuple11 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp202);struct _tuple11 _tmp1CF=_tmp1CE;struct _dyneither_ptr*_tmp1D0;_LL27C: _tmp1D0=(_tmp1CF.f2)->f2;_LL27D:;
_tmp203=_tmp1D0;goto _LL251;}case 13: _LL250: _tmp203=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1C9)->f1)->f2;_LL251:
# 1215
 _tmp204=_tmp203;goto _LL253;case 17: _LL252: _tmp204=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1C9)->f1)->f2;_LL253:
# 1217
 Cyc_add_target(_tmp204);
return;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C9)->f1)->r)){case 0: _LL254: _tmp205=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C9)->f1)->r)->f1;_LL255:
# 1221
({struct Cyc_Absyn_Decl*_tmp5DE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp5DD=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D2;_tmp1D2.tag=5;_tmp1D2.f1=_tmp205;_tmp1D2;});_tmp1D1[0]=_tmp5DD;});_tmp1D1;}),0);Cyc_scan_decl(_tmp5DE,dep);});{
struct _tuple1*_tmp1D3=_tmp205->name;struct _tuple1*_tmp1D4=_tmp1D3;struct _dyneither_ptr*_tmp1D5;_LL27F: _tmp1D5=_tmp1D4->f2;_LL280:;
Cyc_add_target(_tmp1D5);
return;};case 1: _LL256: _tmp206=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C9)->f1)->r)->f1;_LL257:
# 1227
({struct Cyc_Absyn_Decl*_tmp5E0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp5DF=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1D7;_tmp1D7.tag=7;_tmp1D7.f1=_tmp206;_tmp1D7;});_tmp1D6[0]=_tmp5DF;});_tmp1D6;}),0);Cyc_scan_decl(_tmp5E0,dep);});{
struct _tuple1*_tmp1D8=_tmp206->name;struct _tuple1*_tmp1D9=_tmp1D8;struct _dyneither_ptr*_tmp1DA;_LL282: _tmp1DA=_tmp1D9->f2;_LL283:;
Cyc_add_target(_tmp1DA);
return;};default: _LL258: _tmp207=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1C9)->f1)->r)->f1;_LL259:
# 1233
({void*_tmp1DB=0;({struct Cyc___cycFILE*_tmp5E2=Cyc_stderr;struct _dyneither_ptr _tmp5E1=({const char*_tmp1DC="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp1DC,sizeof(char),40);});Cyc_fprintf(_tmp5E2,_tmp5E1,_tag_dyneither(_tmp1DB,sizeof(void*),0));});});
 exit(1);return;}case 1: _LL25A: _LL25B:
# 1236
({void*_tmp1DD=0;({struct Cyc___cycFILE*_tmp5E4=Cyc_stderr;struct _dyneither_ptr _tmp5E3=({const char*_tmp1DE="Error: unexpected Evar\n";_tag_dyneither(_tmp1DE,sizeof(char),24);});Cyc_fprintf(_tmp5E4,_tmp5E3,_tag_dyneither(_tmp1DD,sizeof(void*),0));});});
 exit(1);return;case 2: _LL25C: _LL25D:
# 1239
({void*_tmp1DF=0;({struct Cyc___cycFILE*_tmp5E6=Cyc_stderr;struct _dyneither_ptr _tmp5E5=({const char*_tmp1E0="Error: unexpected VarType\n";_tag_dyneither(_tmp1E0,sizeof(char),27);});Cyc_fprintf(_tmp5E6,_tmp5E5,_tag_dyneither(_tmp1DF,sizeof(void*),0));});});
 exit(1);return;case 3: _LL25E: _LL25F:
# 1242
({void*_tmp1E1=0;({struct Cyc___cycFILE*_tmp5E8=Cyc_stderr;struct _dyneither_ptr _tmp5E7=({const char*_tmp1E2="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp1E2,sizeof(char),32);});Cyc_fprintf(_tmp5E8,_tmp5E7,_tag_dyneither(_tmp1E1,sizeof(void*),0));});});
 exit(1);return;case 4: _LL260: _LL261:
# 1245
({void*_tmp1E3=0;({struct Cyc___cycFILE*_tmp5EA=Cyc_stderr;struct _dyneither_ptr _tmp5E9=({const char*_tmp1E4="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp1E4,sizeof(char),37);});Cyc_fprintf(_tmp5EA,_tmp5E9,_tag_dyneither(_tmp1E3,sizeof(void*),0));});});
 exit(1);return;case 10: _LL262: _LL263:
# 1248
({void*_tmp1E5=0;({struct Cyc___cycFILE*_tmp5EC=Cyc_stderr;struct _dyneither_ptr _tmp5EB=({const char*_tmp1E6="Error: unexpected TupleType\n";_tag_dyneither(_tmp1E6,sizeof(char),29);});Cyc_fprintf(_tmp5EC,_tmp5EB,_tag_dyneither(_tmp1E5,sizeof(void*),0));});});
 exit(1);return;case 15: _LL264: _LL265:
# 1251
({void*_tmp1E7=0;({struct Cyc___cycFILE*_tmp5EE=Cyc_stderr;struct _dyneither_ptr _tmp5ED=({const char*_tmp1E8="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp1E8,sizeof(char),33);});Cyc_fprintf(_tmp5EE,_tmp5ED,_tag_dyneither(_tmp1E7,sizeof(void*),0));});});
 exit(1);return;case 16: _LL266: _LL267:
# 1254
({void*_tmp1E9=0;({struct Cyc___cycFILE*_tmp5F0=Cyc_stderr;struct _dyneither_ptr _tmp5EF=({const char*_tmp1EA="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp1EA,sizeof(char),30);});Cyc_fprintf(_tmp5F0,_tmp5EF,_tag_dyneither(_tmp1E9,sizeof(void*),0));});});
 exit(1);return;case 20: _LL268: _LL269:
# 1257
({void*_tmp1EB=0;({struct Cyc___cycFILE*_tmp5F2=Cyc_stderr;struct _dyneither_ptr _tmp5F1=({const char*_tmp1EC="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp1EC,sizeof(char),27);});Cyc_fprintf(_tmp5F2,_tmp5F1,_tag_dyneither(_tmp1EB,sizeof(void*),0));});});
 exit(1);return;case 21: _LL26A: _LL26B:
# 1260
({void*_tmp1ED=0;({struct Cyc___cycFILE*_tmp5F4=Cyc_stderr;struct _dyneither_ptr _tmp5F3=({const char*_tmp1EE="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp1EE,sizeof(char),29);});Cyc_fprintf(_tmp5F4,_tmp5F3,_tag_dyneither(_tmp1ED,sizeof(void*),0));});});
 exit(1);return;case 22: _LL26C: _LL26D:
# 1263
({void*_tmp1EF=0;({struct Cyc___cycFILE*_tmp5F6=Cyc_stderr;struct _dyneither_ptr _tmp5F5=({const char*_tmp1F0="Error: unexpected RefCntRgn\n";_tag_dyneither(_tmp1F0,sizeof(char),29);});Cyc_fprintf(_tmp5F6,_tmp5F5,_tag_dyneither(_tmp1EF,sizeof(void*),0));});});
 exit(1);return;case 23: _LL26E: _LL26F:
# 1266
({void*_tmp1F1=0;({struct Cyc___cycFILE*_tmp5F8=Cyc_stderr;struct _dyneither_ptr _tmp5F7=({const char*_tmp1F2="Error: unexpected AccessEff\n";_tag_dyneither(_tmp1F2,sizeof(char),29);});Cyc_fprintf(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp1F1,sizeof(void*),0));});});
 exit(1);return;case 24: _LL270: _LL271:
# 1269
({void*_tmp1F3=0;({struct Cyc___cycFILE*_tmp5FA=Cyc_stderr;struct _dyneither_ptr _tmp5F9=({const char*_tmp1F4="Error: unexpected JoinEff\n";_tag_dyneither(_tmp1F4,sizeof(char),27);});Cyc_fprintf(_tmp5FA,_tmp5F9,_tag_dyneither(_tmp1F3,sizeof(void*),0));});});
 exit(1);return;case 25: _LL272: _LL273:
# 1272
({void*_tmp1F5=0;({struct Cyc___cycFILE*_tmp5FC=Cyc_stderr;struct _dyneither_ptr _tmp5FB=({const char*_tmp1F6="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp1F6,sizeof(char),27);});Cyc_fprintf(_tmp5FC,_tmp5FB,_tag_dyneither(_tmp1F5,sizeof(void*),0));});});
 exit(1);return;case 19: _LL274: _LL275:
# 1275
({void*_tmp1F7=0;({struct Cyc___cycFILE*_tmp5FE=Cyc_stderr;struct _dyneither_ptr _tmp5FD=({const char*_tmp1F8="Error: unexpected tag_t\n";_tag_dyneither(_tmp1F8,sizeof(char),25);});Cyc_fprintf(_tmp5FE,_tmp5FD,_tag_dyneither(_tmp1F7,sizeof(void*),0));});});
 exit(1);return;default: _LL276: _LL277:
# 1278
({void*_tmp1F9=0;({struct Cyc___cycFILE*_tmp600=Cyc_stderr;struct _dyneither_ptr _tmp5FF=({const char*_tmp1FA="Error: unexpected valueof_t\n";_tag_dyneither(_tmp1FA,sizeof(char),29);});Cyc_fprintf(_tmp600,_tmp5FF,_tag_dyneither(_tmp1F9,sizeof(void*),0));});});
 exit(1);return;}_LL239:;}
# 1283
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp208=Cyc_current_targets;
struct _dyneither_ptr*_tmp209=Cyc_current_source;
({struct Cyc_Set_Set**_tmp602=({struct Cyc_Set_Set**_tmp20A=_cycalloc(sizeof(*_tmp20A));({struct Cyc_Set_Set*_tmp601=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp20A[0]=_tmp601;});_tmp20A;});Cyc_current_targets=_tmp602;});
{void*_tmp20B=d->r;void*_tmp20C=_tmp20B;struct Cyc_Absyn_Typedefdecl*_tmp240;struct Cyc_Absyn_Enumdecl*_tmp23F;struct Cyc_Absyn_Aggrdecl*_tmp23E;struct Cyc_Absyn_Fndecl*_tmp23D;struct Cyc_Absyn_Vardecl*_tmp23C;switch(*((int*)_tmp20C)){case 0: _LL285: _tmp23C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp20C)->f1;_LL286: {
# 1289
struct _tuple1*_tmp20D=_tmp23C->name;struct _tuple1*_tmp20E=_tmp20D;struct _dyneither_ptr*_tmp20F;_LL2A4: _tmp20F=_tmp20E->f2;_LL2A5:;
Cyc_current_source=_tmp20F;
Cyc_scan_type(_tmp23C->type,dep);
Cyc_scan_exp_opt(_tmp23C->initializer,dep);
goto _LL284;}case 1: _LL287: _tmp23D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp20C)->f1;_LL288: {
# 1295
struct _tuple1*_tmp210=_tmp23D->name;struct _tuple1*_tmp211=_tmp210;struct _dyneither_ptr*_tmp218;_LL2A7: _tmp218=_tmp211->f2;_LL2A8:;
Cyc_current_source=_tmp218;
Cyc_scan_type(_tmp23D->ret_type,dep);
{struct Cyc_List_List*_tmp212=_tmp23D->args;for(0;_tmp212 != 0;_tmp212=_tmp212->tl){
struct _tuple9*_tmp213=(struct _tuple9*)_tmp212->hd;struct _tuple9*_tmp214=_tmp213;void*_tmp215;_LL2AA: _tmp215=_tmp214->f3;_LL2AB:;
Cyc_scan_type(_tmp215,dep);}}
# 1302
if(_tmp23D->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp23D->cyc_varargs))->type,dep);
if(_tmp23D->is_inline)
({void*_tmp216=0;({struct Cyc___cycFILE*_tmp604=Cyc_stderr;struct _dyneither_ptr _tmp603=({const char*_tmp217="Warning: ignoring inline function\n";_tag_dyneither(_tmp217,sizeof(char),35);});Cyc_fprintf(_tmp604,_tmp603,_tag_dyneither(_tmp216,sizeof(void*),0));});});
# 1307
goto _LL284;}case 5: _LL289: _tmp23E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp20C)->f1;_LL28A: {
# 1309
struct _tuple1*_tmp219=_tmp23E->name;struct _tuple1*_tmp21A=_tmp219;struct _dyneither_ptr*_tmp21E;_LL2AD: _tmp21E=_tmp21A->f2;_LL2AE:;
Cyc_current_source=_tmp21E;
if((unsigned int)_tmp23E->impl){
{struct Cyc_List_List*_tmp21B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){
struct Cyc_Absyn_Aggrfield*_tmp21C=(struct Cyc_Absyn_Aggrfield*)_tmp21B->hd;
Cyc_scan_type(_tmp21C->type,dep);
Cyc_scan_exp_opt(_tmp21C->width,dep);}}{
# 1319
struct Cyc_List_List*_tmp21D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){;}};}
# 1323
goto _LL284;}case 7: _LL28B: _tmp23F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp20C)->f1;_LL28C: {
# 1325
struct _tuple1*_tmp21F=_tmp23F->name;struct _tuple1*_tmp220=_tmp21F;struct _dyneither_ptr*_tmp224;_LL2B0: _tmp224=_tmp220->f2;_LL2B1:;
Cyc_current_source=_tmp224;
if((unsigned int)_tmp23F->fields){
{struct Cyc_List_List*_tmp221=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23F->fields))->v;for(0;_tmp221 != 0;_tmp221=_tmp221->tl){
struct Cyc_Absyn_Enumfield*_tmp222=(struct Cyc_Absyn_Enumfield*)_tmp221->hd;
Cyc_scan_exp_opt(_tmp222->tag,dep);}}{
# 1334
struct Cyc_List_List*_tmp223=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23F->fields))->v;for(0;_tmp223 != 0;_tmp223=_tmp223->tl){;}};}
# 1338
goto _LL284;}case 8: _LL28D: _tmp240=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp20C)->f1;_LL28E: {
# 1340
struct _tuple1*_tmp225=_tmp240->name;struct _tuple1*_tmp226=_tmp225;struct _dyneither_ptr*_tmp227;_LL2B3: _tmp227=_tmp226->f2;_LL2B4:;
Cyc_current_source=_tmp227;
if((unsigned int)_tmp240->defn)
Cyc_scan_type((void*)_check_null(_tmp240->defn),dep);
goto _LL284;}case 4: _LL28F: _LL290:
# 1346
({void*_tmp228=0;({struct Cyc___cycFILE*_tmp606=Cyc_stderr;struct _dyneither_ptr _tmp605=({const char*_tmp229="Error: unexpected region declaration";_tag_dyneither(_tmp229,sizeof(char),37);});Cyc_fprintf(_tmp606,_tmp605,_tag_dyneither(_tmp228,sizeof(void*),0));});});
 exit(1);case 13: _LL291: _LL292:
# 1349
({void*_tmp22A=0;({struct Cyc___cycFILE*_tmp608=Cyc_stderr;struct _dyneither_ptr _tmp607=({const char*_tmp22B="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp22B,sizeof(char),38);});Cyc_fprintf(_tmp608,_tmp607,_tag_dyneither(_tmp22A,sizeof(void*),0));});});
 exit(1);case 14: _LL293: _LL294:
# 1352
({void*_tmp22C=0;({struct Cyc___cycFILE*_tmp60A=Cyc_stderr;struct _dyneither_ptr _tmp609=({const char*_tmp22D="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp22D,sizeof(char),39);});Cyc_fprintf(_tmp60A,_tmp609,_tag_dyneither(_tmp22C,sizeof(void*),0));});});
 exit(1);case 2: _LL295: _LL296:
# 1355
({void*_tmp22E=0;({struct Cyc___cycFILE*_tmp60C=Cyc_stderr;struct _dyneither_ptr _tmp60B=({const char*_tmp22F="Error: unexpected let declaration\n";_tag_dyneither(_tmp22F,sizeof(char),35);});Cyc_fprintf(_tmp60C,_tmp60B,_tag_dyneither(_tmp22E,sizeof(void*),0));});});
 exit(1);case 6: _LL297: _LL298:
# 1358
({void*_tmp230=0;({struct Cyc___cycFILE*_tmp60E=Cyc_stderr;struct _dyneither_ptr _tmp60D=({const char*_tmp231="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp231,sizeof(char),40);});Cyc_fprintf(_tmp60E,_tmp60D,_tag_dyneither(_tmp230,sizeof(void*),0));});});
 exit(1);case 3: _LL299: _LL29A:
# 1361
({void*_tmp232=0;({struct Cyc___cycFILE*_tmp610=Cyc_stderr;struct _dyneither_ptr _tmp60F=({const char*_tmp233="Error: unexpected let declaration\n";_tag_dyneither(_tmp233,sizeof(char),35);});Cyc_fprintf(_tmp610,_tmp60F,_tag_dyneither(_tmp232,sizeof(void*),0));});});
 exit(1);case 9: _LL29B: _LL29C:
# 1364
({void*_tmp234=0;({struct Cyc___cycFILE*_tmp612=Cyc_stderr;struct _dyneither_ptr _tmp611=({const char*_tmp235="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp235,sizeof(char),41);});Cyc_fprintf(_tmp612,_tmp611,_tag_dyneither(_tmp234,sizeof(void*),0));});});
 exit(1);case 10: _LL29D: _LL29E:
# 1367
({void*_tmp236=0;({struct Cyc___cycFILE*_tmp614=Cyc_stderr;struct _dyneither_ptr _tmp613=({const char*_tmp237="Error: unexpected using declaration\n";_tag_dyneither(_tmp237,sizeof(char),37);});Cyc_fprintf(_tmp614,_tmp613,_tag_dyneither(_tmp236,sizeof(void*),0));});});
 exit(1);case 11: _LL29F: _LL2A0:
# 1370
({void*_tmp238=0;({struct Cyc___cycFILE*_tmp616=Cyc_stderr;struct _dyneither_ptr _tmp615=({const char*_tmp239="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp239,sizeof(char),42);});Cyc_fprintf(_tmp616,_tmp615,_tag_dyneither(_tmp238,sizeof(void*),0));});});
 exit(1);default: _LL2A1: _LL2A2:
# 1373
({void*_tmp23A=0;({struct Cyc___cycFILE*_tmp618=Cyc_stderr;struct _dyneither_ptr _tmp617=({const char*_tmp23B="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp23B,sizeof(char),50);});Cyc_fprintf(_tmp618,_tmp617,_tag_dyneither(_tmp23A,sizeof(void*),0));});});
 exit(1);}_LL284:;}{
# 1381
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp241=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp242;_push_handler(& _tmp242);{int _tmp244=0;if(setjmp(_tmp242.handler))_tmp244=1;if(!_tmp244){
({struct Cyc_Set_Set*_tmp619=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp241);old=_tmp619;});;_pop_handler();}else{void*_tmp243=(void*)_exn_thrown;void*_tmp245=_tmp243;void*_tmp246;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp245)->tag == Cyc_Core_Not_found){_LL2B6: _LL2B7:
# 1386
({struct Cyc_Set_Set*_tmp61A=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);old=_tmp61A;});goto _LL2B5;}else{_LL2B8: _tmp246=_tmp245;_LL2B9:(int)_rethrow(_tmp246);}_LL2B5:;}};}{
# 1388
struct Cyc_Set_Set*_tmp247=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp241,_tmp247);
# 1391
Cyc_current_targets=_tmp208;
Cyc_current_source=_tmp209;};};}
# 1395
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1399
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp248;_push_handler(& _tmp248);{int _tmp24A=0;if(setjmp(_tmp248.handler))_tmp24A=1;if(!_tmp24A){{struct Cyc_Set_Set*_tmp24B=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp24B;};_pop_handler();}else{void*_tmp249=(void*)_exn_thrown;void*_tmp24C=_tmp249;void*_tmp24D;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp24C)->tag == Cyc_Core_Not_found){_LL2BB: _LL2BC:
# 1402
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL2BD: _tmp24D=_tmp24C;_LL2BE:(int)_rethrow(_tmp24D);}_LL2BA:;}};}
# 1406
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1416 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
({struct Cyc_Set_Set*_tmp61B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);curr=_tmp61B;});}{
# 1421
struct Cyc_Set_Set*_tmp24E=curr;
# 1423
struct _dyneither_ptr*_tmp24F=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp24E)> 0){
struct Cyc_Set_Set*_tmp250=emptyset;
struct Cyc_Iter_Iter _tmp251=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp24E);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp251,& _tmp24F)){
({struct Cyc_Set_Set*_tmp61D=({struct Cyc_Set_Set*_tmp61C=_tmp250;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp61C,Cyc_find(t,_tmp24F));});_tmp250=_tmp61D;});}
({struct Cyc_Set_Set*_tmp61E=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp250,curr);_tmp24E=_tmp61E;});
({struct Cyc_Set_Set*_tmp61F=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp24E);curr=_tmp61F;});}
# 1432
return curr;};}
# 1435
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1441
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1445
if(Cyc_script_file == 0){
({void*_tmp253=0;({struct Cyc___cycFILE*_tmp621=Cyc_stderr;struct _dyneither_ptr _tmp620=({const char*_tmp254="Internal error: script file is NULL\n";_tag_dyneither(_tmp254,sizeof(char),37);});Cyc_fprintf(_tmp621,_tmp620,_tag_dyneither(_tmp253,sizeof(void*),0));});});
 exit(1);}
# 1449
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1452
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({struct Cyc_String_pa_PrintArg_struct _tmp257;_tmp257.tag=0;_tmp257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp255[2]={& _tmp257,& _tmp258};({struct _dyneither_ptr _tmp622=({const char*_tmp256="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp256,sizeof(char),36);});Cyc_prscript(_tmp622,_tag_dyneither(_tmp255,sizeof(void*),2));});});});});else{
# 1459
int _tmp259=({unsigned short _tmp25D[0];({const char*_tmp623=(const char*)_untag_dyneither_ptr(d,sizeof(char),1);Cyc_open(_tmp623,0,_tag_dyneither(_tmp25D,sizeof(unsigned short),0));});});
if(_tmp259 == - 1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp25A[1]={& _tmp25C};({struct Cyc___cycFILE*_tmp625=Cyc_stderr;struct _dyneither_ptr _tmp624=({const char*_tmp25B="Error: could not create directory %s\n";_tag_dyneither(_tmp25B,sizeof(char),38);});Cyc_fprintf(_tmp625,_tmp624,_tag_dyneither(_tmp25A,sizeof(void*),1));});});});
return 1;}}else{
# 1466
 close(_tmp259);}}
# 1468
return 0;}
# 1471
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1475
struct _dyneither_ptr _tmp25E=Cyc_strdup((struct _dyneither_ptr)file);
# 1477
struct Cyc_List_List*_tmp25F=0;
while(1){
({struct _dyneither_ptr _tmp626=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp25E);_tmp25E=_tmp626;});
if(Cyc_strlen((struct _dyneither_ptr)_tmp25E)== 0)break;
({struct Cyc_List_List*_tmp628=({struct Cyc_List_List*_tmp260=_cycalloc(sizeof(*_tmp260));({struct _dyneither_ptr*_tmp627=({struct _dyneither_ptr*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=(struct _dyneither_ptr)_tmp25E;_tmp261;});_tmp260->hd=_tmp627;});_tmp260->tl=_tmp25F;_tmp260;});_tmp25F=_tmp628;});}
# 1484
for(0;_tmp25F != 0;_tmp25F=_tmp25F->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp25F->hd)))return 1;}
# 1487
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1494
static int Cyc_is_other_special(char c){
char _tmp262=c;switch(_tmp262){case 92: _LL2C0: _LL2C1:
 goto _LL2C3;case 34: _LL2C2: _LL2C3:
 goto _LL2C5;case 59: _LL2C4: _LL2C5:
 goto _LL2C7;case 38: _LL2C6: _LL2C7:
 goto _LL2C9;case 40: _LL2C8: _LL2C9:
 goto _LL2CB;case 41: _LL2CA: _LL2CB:
 goto _LL2CD;case 124: _LL2CC: _LL2CD:
 goto _LL2CF;case 94: _LL2CE: _LL2CF:
 goto _LL2D1;case 60: _LL2D0: _LL2D1:
 goto _LL2D3;case 62: _LL2D2: _LL2D3:
 goto _LL2D5;case 10: _LL2D4: _LL2D5:
# 1509
 goto _LL2D7;case 9: _LL2D6: _LL2D7:
 return 1;default: _LL2D8: _LL2D9:
 return 0;}_LL2BF:;}
# 1515
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp263=Cyc_strlen((struct _dyneither_ptr)s);
# 1519
int _tmp264=0;
int _tmp265=0;
{int i=0;for(0;i < _tmp263;++ i){
char _tmp266=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp266 == '\'')++ _tmp264;else{
if(Cyc_is_other_special(_tmp266))++ _tmp265;}}}
# 1528
if(_tmp264 == 0  && _tmp265 == 0)
return s;
# 1532
if(_tmp264 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp267[3];({struct _dyneither_ptr*_tmp629=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);_tmp267[2]=_tmp629;});({struct _dyneither_ptr*_tmp62A=({struct _dyneither_ptr*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=(struct _dyneither_ptr)s;_tmp269;});_tmp267[1]=_tmp62A;});({struct _dyneither_ptr*_tmp62B=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);_tmp267[0]=_tmp62B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp267,sizeof(struct _dyneither_ptr*),3));}));{
# 1536
unsigned long _tmp26B=(_tmp263 + _tmp264)+ _tmp265;
struct _dyneither_ptr s2=({unsigned int _tmp275=_tmp26B + 1;char*_tmp276=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp275 + 1));struct _dyneither_ptr _tmp278=_tag_dyneither(_tmp276,sizeof(char),_tmp275 + 1);{unsigned int _tmp277=_tmp275;unsigned int i;for(i=0;i < _tmp277;i ++){_tmp276[i]=(char)'\000';}_tmp276[_tmp277]=(char)0;}_tmp278;});
int _tmp26C=0;
int _tmp26D=0;
for(0;_tmp26C < _tmp263;++ _tmp26C){
char _tmp26E=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp26C));
if(_tmp26E == '\''  || Cyc_is_other_special(_tmp26E))
({struct _dyneither_ptr _tmp26F=_dyneither_ptr_plus(s2,sizeof(char),_tmp26D ++);char _tmp270=*((char*)_check_dyneither_subscript(_tmp26F,sizeof(char),0));char _tmp271='\\';if(_get_dyneither_size(_tmp26F,sizeof(char))== 1  && (_tmp270 == '\000'  && _tmp271 != '\000'))_throw_arraybounds();*((char*)_tmp26F.curr)=_tmp271;});
({struct _dyneither_ptr _tmp272=_dyneither_ptr_plus(s2,sizeof(char),_tmp26D ++);char _tmp273=*((char*)_check_dyneither_subscript(_tmp272,sizeof(char),0));char _tmp274=_tmp26E;if(_get_dyneither_size(_tmp272,sizeof(char))== 1  && (_tmp273 == '\000'  && _tmp274 != '\000'))_throw_arraybounds();*((char*)_tmp272.curr)=_tmp274;});}
# 1546
return(struct _dyneither_ptr)s2;};}
# 1548
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp279=_cycalloc(sizeof(*_tmp279));({struct _dyneither_ptr _tmp62C=Cyc_sh_escape_string(*sp);_tmp279[0]=_tmp62C;});_tmp279;});}
# 1553
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1560
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1565
({struct Cyc_String_pa_PrintArg_struct _tmp27C;_tmp27C.tag=0;({struct _dyneither_ptr _tmp62D=(struct _dyneither_ptr)({const char*_tmp27D=filename;_tag_dyneither(_tmp27D,sizeof(char),_get_zero_arr_size_char((void*)_tmp27D,1));});_tmp27C.f1=_tmp62D;});({void*_tmp27A[1]={& _tmp27C};({struct Cyc___cycFILE*_tmp62F=Cyc_stderr;struct _dyneither_ptr _tmp62E=({const char*_tmp27B="********************************* %s...\n";_tag_dyneither(_tmp27B,sizeof(char),41);});Cyc_fprintf(_tmp62F,_tmp62E,_tag_dyneither(_tmp27A,sizeof(void*),1));});});});
# 1568
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp280;_tmp280.tag=0;({struct _dyneither_ptr _tmp630=(struct _dyneither_ptr)({const char*_tmp281=filename;_tag_dyneither(_tmp281,sizeof(char),_get_zero_arr_size_char((void*)_tmp281,1));});_tmp280.f1=_tmp630;});({void*_tmp27E[1]={& _tmp280};({struct _dyneither_ptr _tmp631=({const char*_tmp27F="\n%s:\n";_tag_dyneither(_tmp27F,sizeof(char),6);});Cyc_log(_tmp631,_tag_dyneither(_tmp27E,sizeof(void*),1));});});});{
# 1580 "buildlib.cyl"
struct _dyneither_ptr _tmp282=Cyc_Filename_basename(({const char*_tmp3D6=filename;_tag_dyneither(_tmp3D6,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D6,1));}));
struct _dyneither_ptr _tmp283=Cyc_Filename_dirname(({const char*_tmp3D5=filename;_tag_dyneither(_tmp3D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D5,1));}));
struct _dyneither_ptr _tmp284=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp282);
const char*_tmp285=(const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp632=(struct _dyneither_ptr)_tmp284;Cyc_strconcat(_tmp632,({const char*_tmp3D4=".iA";_tag_dyneither(_tmp3D4,sizeof(char),4);}));}),sizeof(char),1));
# 1585
const char*_tmp286=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp283,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0;_tmp3D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp284);({void*_tmp3CE[1]={& _tmp3D0};({struct _dyneither_ptr _tmp635=({const char*_tmp3CF="%s.iB";_tag_dyneither(_tmp3CF,sizeof(char),6);});Cyc_aprintf(_tmp635,_tag_dyneither(_tmp3CE,sizeof(void*),1));});});}):({struct _dyneither_ptr _tmp634=(struct _dyneither_ptr)_tmp283;Cyc_Filename_concat(_tmp634,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0;_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp284);({void*_tmp3D1[1]={& _tmp3D3};({struct _dyneither_ptr _tmp633=({const char*_tmp3D2="%s.iB";_tag_dyneither(_tmp3D2,sizeof(char),6);});Cyc_aprintf(_tmp633,_tag_dyneither(_tmp3D1,sizeof(void*),1));});});}));}),sizeof(char),1));
# 1590
const char*_tmp287=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp283,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0;_tmp3CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp284);({void*_tmp3C8[1]={& _tmp3CA};({struct _dyneither_ptr _tmp638=({const char*_tmp3C9="%s.iC";_tag_dyneither(_tmp3C9,sizeof(char),6);});Cyc_aprintf(_tmp638,_tag_dyneither(_tmp3C8,sizeof(void*),1));});});}):({struct _dyneither_ptr _tmp637=(struct _dyneither_ptr)_tmp283;Cyc_Filename_concat(_tmp637,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0;_tmp3CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp284);({void*_tmp3CB[1]={& _tmp3CD};({struct _dyneither_ptr _tmp636=({const char*_tmp3CC="%s.iC";_tag_dyneither(_tmp3CC,sizeof(char),6);});Cyc_aprintf(_tmp636,_tag_dyneither(_tmp3CB,sizeof(void*),1));});});}));}),sizeof(char),1));
# 1595
const char*_tmp288=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp283,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0;_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp284);({void*_tmp3C2[1]={& _tmp3C4};({struct _dyneither_ptr _tmp63B=({const char*_tmp3C3="%s.iD";_tag_dyneither(_tmp3C3,sizeof(char),6);});Cyc_aprintf(_tmp63B,_tag_dyneither(_tmp3C2,sizeof(void*),1));});});}):({struct _dyneither_ptr _tmp63A=(struct _dyneither_ptr)_tmp283;Cyc_Filename_concat(_tmp63A,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C7;_tmp3C7.tag=0;_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp284);({void*_tmp3C5[1]={& _tmp3C7};({struct _dyneither_ptr _tmp639=({const char*_tmp3C6="%s.iD";_tag_dyneither(_tmp3C6,sizeof(char),6);});Cyc_aprintf(_tmp639,_tag_dyneither(_tmp3C5,sizeof(void*),1));});});}));}),sizeof(char),1));
# 1600
{struct _handler_cons _tmp289;_push_handler(& _tmp289);{int _tmp28B=0;if(setjmp(_tmp289.handler))_tmp28B=1;if(!_tmp28B){
# 1603
if(Cyc_force_directory_prefixes(({const char*_tmp28C=filename;_tag_dyneither(_tmp28C,sizeof(char),_get_zero_arr_size_char((void*)_tmp28C,1));}))){
int _tmp28D=1;_npop_handler(0);return _tmp28D;}
# 1608
if(Cyc_mode != Cyc_FINISH){
({struct Cyc_List_List*_tmp63C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);Cyc_current_cpp=_tmp63C;});
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp290;_tmp290.tag=0;({struct _dyneither_ptr _tmp63D=(struct _dyneither_ptr)({const char*_tmp291=_tmp285;_tag_dyneither(_tmp291,sizeof(char),_get_zero_arr_size_char((void*)_tmp291,1));});_tmp290.f1=_tmp63D;});({void*_tmp28E[1]={& _tmp290};({struct _dyneither_ptr _tmp63E=({const char*_tmp28F="cat >%s <<XXX\n";_tag_dyneither(_tmp28F,sizeof(char),15);});Cyc_prscript(_tmp63E,_tag_dyneither(_tmp28E,sizeof(void*),1));});});});
{struct Cyc_List_List*_tmp292=Cyc_current_cpp;for(0;_tmp292 != 0;_tmp292=_tmp292->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp295;_tmp295.tag=0;_tmp295.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp292->hd));({void*_tmp293[1]={& _tmp295};({struct _dyneither_ptr _tmp63F=({const char*_tmp294="%s";_tag_dyneither(_tmp294,sizeof(char),3);});Cyc_prscript(_tmp63F,_tag_dyneither(_tmp293,sizeof(void*),1));});});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp298;_tmp298.tag=0;({struct _dyneither_ptr _tmp640=(struct _dyneither_ptr)({const char*_tmp299=filename;_tag_dyneither(_tmp299,sizeof(char),_get_zero_arr_size_char((void*)_tmp299,1));});_tmp298.f1=_tmp640;});({void*_tmp296[1]={& _tmp298};({struct _dyneither_ptr _tmp641=({const char*_tmp297="#include <%s>\n";_tag_dyneither(_tmp297,sizeof(char),15);});Cyc_prscript(_tmp641,_tag_dyneither(_tmp296,sizeof(void*),1));});});});
({void*_tmp29A=0;({struct _dyneither_ptr _tmp642=({const char*_tmp29B="XXX\n";_tag_dyneither(_tmp29B,sizeof(char),5);});Cyc_prscript(_tmp642,_tag_dyneither(_tmp29A,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0;({struct _dyneither_ptr _tmp643=(struct _dyneither_ptr)({const char*_tmp2A2=_tmp285;_tag_dyneither(_tmp2A2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A2,1));});_tmp2A0.f1=_tmp643;});({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0;({struct _dyneither_ptr _tmp644=(struct _dyneither_ptr)({const char*_tmp2A1=_tmp286;_tag_dyneither(_tmp2A1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A1,1));});_tmp29F.f1=_tmp644;});({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp29C[3]={& _tmp29E,& _tmp29F,& _tmp2A0};({struct _dyneither_ptr _tmp645=({const char*_tmp29D="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp29D,sizeof(char),35);});Cyc_prscript(_tmp645,_tag_dyneither(_tmp29C,sizeof(void*),3));});});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0;({struct _dyneither_ptr _tmp646=(struct _dyneither_ptr)({const char*_tmp2A9=_tmp285;_tag_dyneither(_tmp2A9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A9,1));});_tmp2A7.f1=_tmp646;});({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0;({struct _dyneither_ptr _tmp647=(struct _dyneither_ptr)({const char*_tmp2A8=_tmp287;_tag_dyneither(_tmp2A8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A8,1));});_tmp2A6.f1=_tmp647;});({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0;_tmp2A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp2A3[3]={& _tmp2A5,& _tmp2A6,& _tmp2A7};({struct _dyneither_ptr _tmp648=({const char*_tmp2A4="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp2A4,sizeof(char),31);});Cyc_prscript(_tmp648,_tag_dyneither(_tmp2A3,sizeof(void*),3));});});});});});
({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0;({struct _dyneither_ptr _tmp649=(struct _dyneither_ptr)({const char*_tmp2AD=_tmp285;_tag_dyneither(_tmp2AD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AD,1));});_tmp2AC.f1=_tmp649;});({void*_tmp2AA[1]={& _tmp2AC};({struct _dyneither_ptr _tmp64A=({const char*_tmp2AB="rm %s\n";_tag_dyneither(_tmp2AB,sizeof(char),7);});Cyc_prscript(_tmp64A,_tag_dyneither(_tmp2AA,sizeof(void*),1));});});});}else{
# 1621
({struct Cyc___cycFILE*_tmp64B=Cyc_fopen(_tmp285,"w");maybe=_tmp64B;});
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0;({struct _dyneither_ptr _tmp64C=(struct _dyneither_ptr)({const char*_tmp2B1=_tmp285;_tag_dyneither(_tmp2B1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B1,1));});_tmp2B0.f1=_tmp64C;});({void*_tmp2AE[1]={& _tmp2B0};({struct Cyc___cycFILE*_tmp64E=Cyc_stderr;struct _dyneither_ptr _tmp64D=({const char*_tmp2AF="Error: could not create file %s\n";_tag_dyneither(_tmp2AF,sizeof(char),33);});Cyc_fprintf(_tmp64E,_tmp64D,_tag_dyneither(_tmp2AE,sizeof(void*),1));});});});{
int _tmp2B2=1;_npop_handler(0);return _tmp2B2;};}
# 1626
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0;({struct _dyneither_ptr _tmp64F=(struct _dyneither_ptr)({const char*_tmp2B6=_tmp285;_tag_dyneither(_tmp2B6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B6,1));});_tmp2B5.f1=_tmp64F;});({void*_tmp2B3[1]={& _tmp2B5};({struct Cyc___cycFILE*_tmp651=Cyc_stderr;struct _dyneither_ptr _tmp650=({const char*_tmp2B4="Creating %s\n";_tag_dyneither(_tmp2B4,sizeof(char),13);});Cyc_fprintf(_tmp651,_tmp650,_tag_dyneither(_tmp2B3,sizeof(void*),1));});});});
out_file=maybe;
{struct Cyc_List_List*_tmp2B7=Cyc_current_cpp;for(0;_tmp2B7 != 0;_tmp2B7=_tmp2B7->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp2B7->hd),sizeof(char),1)),out_file);}}
# 1632
({struct Cyc_String_pa_PrintArg_struct _tmp2BA;_tmp2BA.tag=0;({struct _dyneither_ptr _tmp652=(struct _dyneither_ptr)({const char*_tmp2BB=filename;_tag_dyneither(_tmp2BB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BB,1));});_tmp2BA.f1=_tmp652;});({void*_tmp2B8[1]={& _tmp2BA};({struct Cyc___cycFILE*_tmp654=out_file;struct _dyneither_ptr _tmp653=({const char*_tmp2B9="#include <%s>\n";_tag_dyneither(_tmp2B9,sizeof(char),15);});Cyc_fprintf(_tmp654,_tmp653,_tag_dyneither(_tmp2B8,sizeof(void*),1));});});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp2BC=({
struct Cyc_List_List*_tmp658=({struct Cyc_List_List*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));({struct _dyneither_ptr*_tmp656=({struct _dyneither_ptr*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));({struct _dyneither_ptr _tmp655=(struct _dyneither_ptr)({const char*_tmp2DB="";_tag_dyneither(_tmp2DB,sizeof(char),1);});_tmp2DA[0]=_tmp655;});_tmp2DA;});_tmp2D9->hd=_tmp656;});({struct Cyc_List_List*_tmp657=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp2D9->tl=_tmp657;});_tmp2D9;});
# 1635
Cyc_str_sepstr(_tmp658,({const char*_tmp2DC=" ";_tag_dyneither(_tmp2DC,sizeof(char),2);}));});
# 1638
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2D6;_tmp2D6.tag=0;({struct _dyneither_ptr _tmp659=(struct _dyneither_ptr)({const char*_tmp2D8=_tmp285;_tag_dyneither(_tmp2D8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D8,1));});_tmp2D6.f1=_tmp659;});({struct Cyc_String_pa_PrintArg_struct _tmp2D5;_tmp2D5.tag=0;({struct _dyneither_ptr _tmp65A=(struct _dyneither_ptr)({const char*_tmp2D7=_tmp286;_tag_dyneither(_tmp2D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D7,1));});_tmp2D5.f1=_tmp65A;});({struct Cyc_String_pa_PrintArg_struct _tmp2D4;_tmp2D4.tag=0;_tmp2D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BC);({struct Cyc_String_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp2CF[6]={& _tmp2D1,& _tmp2D2,& _tmp2D3,& _tmp2D4,& _tmp2D5,& _tmp2D6};({struct _dyneither_ptr _tmp65B=({const char*_tmp2D0="%s %s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp2D0,sizeof(char),33);});Cyc_aprintf(_tmp65B,_tag_dyneither(_tmp2CF,sizeof(void*),6));});});});});});});});}),sizeof(char),1));
# 1641
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0;({struct _dyneither_ptr _tmp65C=(struct _dyneither_ptr)({char*_tmp2C0=cmd;_tag_dyneither(_tmp2C0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C0,1));});_tmp2BF.f1=_tmp65C;});({void*_tmp2BD[1]={& _tmp2BF};({struct Cyc___cycFILE*_tmp65E=Cyc_stderr;struct _dyneither_ptr _tmp65D=({const char*_tmp2BE="%s\n";_tag_dyneither(_tmp2BE,sizeof(char),4);});Cyc_fprintf(_tmp65E,_tmp65D,_tag_dyneither(_tmp2BD,sizeof(void*),1));});});});
if(! system((const char*)cmd)){
# 1646
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0;({struct _dyneither_ptr _tmp65F=(struct _dyneither_ptr)({const char*_tmp2CA=_tmp285;_tag_dyneither(_tmp2CA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2CA,1));});_tmp2C8.f1=_tmp65F;});({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0;({struct _dyneither_ptr _tmp660=(struct _dyneither_ptr)({const char*_tmp2C9=_tmp287;_tag_dyneither(_tmp2C9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C9,1));});_tmp2C7.f1=_tmp660;});({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BC);({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp2C1[6]={& _tmp2C3,& _tmp2C4,& _tmp2C5,& _tmp2C6,& _tmp2C7,& _tmp2C8};({struct _dyneither_ptr _tmp661=({const char*_tmp2C2="%s %s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp2C2,sizeof(char),29);});Cyc_aprintf(_tmp661,_tag_dyneither(_tmp2C1,sizeof(void*),6));});});});});});});});}),sizeof(char),1));
# 1649
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2CD;_tmp2CD.tag=0;({struct _dyneither_ptr _tmp662=(struct _dyneither_ptr)({char*_tmp2CE=cmd;_tag_dyneither(_tmp2CE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2CE,1));});_tmp2CD.f1=_tmp662;});({void*_tmp2CB[1]={& _tmp2CD};({struct Cyc___cycFILE*_tmp664=Cyc_stderr;struct _dyneither_ptr _tmp663=({const char*_tmp2CC="%s\n";_tag_dyneither(_tmp2CC,sizeof(char),4);});Cyc_fprintf(_tmp664,_tmp663,_tag_dyneither(_tmp2CB,sizeof(void*),1));});});});
 system((const char*)cmd);}};}}
# 1656
if(Cyc_gathering()){int _tmp2DD=0;_npop_handler(0);return _tmp2DD;}{
# 1659
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
({struct Cyc___cycFILE*_tmp665=Cyc_fopen(_tmp286,"r");maybe=_tmp665;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));({struct Cyc_NO_SUPPORT_exn_struct _tmp669=({struct Cyc_NO_SUPPORT_exn_struct _tmp2DF;_tmp2DF.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp668=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E2;_tmp2E2.tag=0;({struct _dyneither_ptr _tmp666=(struct _dyneither_ptr)({const char*_tmp2E3=_tmp286;_tag_dyneither(_tmp2E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E3,1));});_tmp2E2.f1=_tmp666;});({void*_tmp2E0[1]={& _tmp2E2};({struct _dyneither_ptr _tmp667=({const char*_tmp2E1="can't open macrosfile %s";_tag_dyneither(_tmp2E1,sizeof(char),25);});Cyc_aprintf(_tmp667,_tag_dyneither(_tmp2E0,sizeof(void*),1));});});});_tmp2DF.f1=_tmp668;});_tmp2DF;});_tmp2DE[0]=_tmp669;});_tmp2DE;}));
# 1664
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp2E4=Cyc_Lexing_from_file(in_file);
struct _tuple21*entry;
while(({struct _tuple21*_tmp66A=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp2E4);entry=_tmp66A;})!= 0){
struct _tuple21*_tmp2E5=(struct _tuple21*)_check_null(entry);struct _tuple21*_tmp2E6=_tmp2E5;struct _dyneither_ptr*_tmp2E8;struct Cyc_Set_Set*_tmp2E7;_LL2DB: _tmp2E8=_tmp2E6->f1;_tmp2E7=_tmp2E6->f2;_LL2DC:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp2E8,_tmp2E7);}
# 1673
Cyc_fclose(in_file);
# 1676
({struct Cyc___cycFILE*_tmp66B=Cyc_fopen(_tmp287,"r");maybe=_tmp66B;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));({struct Cyc_NO_SUPPORT_exn_struct _tmp66F=({struct Cyc_NO_SUPPORT_exn_struct _tmp2EA;_tmp2EA.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp66E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2ED;_tmp2ED.tag=0;({struct _dyneither_ptr _tmp66C=(struct _dyneither_ptr)({const char*_tmp2EE=_tmp287;_tag_dyneither(_tmp2EE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EE,1));});_tmp2ED.f1=_tmp66C;});({void*_tmp2EB[1]={& _tmp2ED};({struct _dyneither_ptr _tmp66D=({const char*_tmp2EC="can't open declsfile %s";_tag_dyneither(_tmp2EC,sizeof(char),24);});Cyc_aprintf(_tmp66D,_tag_dyneither(_tmp2EB,sizeof(void*),1));});});});_tmp2EA.f1=_tmp66E;});_tmp2EA;});_tmp2E9[0]=_tmp66F;});_tmp2E9;}));
# 1680
in_file=maybe;
({struct Cyc_Lexing_lexbuf*_tmp670=Cyc_Lexing_from_file(in_file);_tmp2E4=_tmp670;});
({struct Cyc___cycFILE*_tmp671=Cyc_fopen(_tmp288,"w");Cyc_slurp_out=_tmp671;});
if(!((unsigned int)Cyc_slurp_out)){int _tmp2EF=1;_npop_handler(0);return _tmp2EF;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp2E4)){;}{
# 1686
struct Cyc_List_List*_tmp2F0=user_defs;
while(_tmp2F0 != 0){
struct _tuple24*_tmp2F1=(struct _tuple24*)_tmp2F0->hd;struct _tuple24*_tmp2F2=_tmp2F1;struct _dyneither_ptr*_tmp2F6;_LL2DE: _tmp2F6=_tmp2F2->f2;_LL2DF:;
({struct Cyc_String_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F6);({void*_tmp2F3[1]={& _tmp2F5};({struct Cyc___cycFILE*_tmp673=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp672=({const char*_tmp2F4="%s";_tag_dyneither(_tmp2F4,sizeof(char),3);});Cyc_fprintf(_tmp673,_tmp672,_tag_dyneither(_tmp2F3,sizeof(void*),1));});});});
_tmp2F0=_tmp2F0->tl;}
# 1692
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1697
({struct Cyc___cycFILE*_tmp674=Cyc_fopen(_tmp288,"r");maybe=_tmp674;});
if(!((unsigned int)maybe)){int _tmp2F7=1;_npop_handler(0);return _tmp2F7;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp2F8=_tmp288;_tag_dyneither(_tmp2F8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F8,1));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp2F9=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1707
{struct Cyc_List_List*_tmp2FA=_tmp2F9;for(0;_tmp2FA != 0;_tmp2FA=_tmp2FA->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp2FA->hd,t);}}{
# 1711
struct Cyc_List_List*_tmp2FB=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);
struct Cyc_Set_Set*_tmp2FC=({struct Cyc_List_List*_tmp675=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,_tmp2FB);Cyc_reachable(_tmp675,t);});
# 1715
struct Cyc_List_List*_tmp2FD=0;
struct Cyc_List_List*_tmp2FE=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp2FF=_tmp2F9;for(0;_tmp2FF != 0;_tmp2FF=_tmp2FF->tl){
struct Cyc_Absyn_Decl*_tmp300=(struct Cyc_Absyn_Decl*)_tmp2FF->hd;
struct _dyneither_ptr*name;
{void*_tmp301=_tmp300->r;void*_tmp302=_tmp301;struct Cyc_Absyn_Typedefdecl*_tmp31D;struct Cyc_Absyn_Enumdecl*_tmp31C;struct Cyc_Absyn_Aggrdecl*_tmp31B;struct Cyc_Absyn_Fndecl*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp319;switch(*((int*)_tmp302)){case 0: _LL2E1: _tmp319=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2E2: {
# 1723
struct _tuple1*_tmp303=_tmp319->name;struct _tuple1*_tmp304=_tmp303;struct _dyneither_ptr*_tmp305;_LL300: _tmp305=_tmp304->f2;_LL301:;
({struct Cyc_Set_Set*_tmp676=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp305);defined_symbols=_tmp676;});
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp305))name=0;else{
name=_tmp305;}
goto _LL2E0;}case 1: _LL2E3: _tmp31A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2E4: {
# 1729
struct _tuple1*_tmp306=_tmp31A->name;struct _tuple1*_tmp307=_tmp306;struct _dyneither_ptr*_tmp308;_LL303: _tmp308=_tmp307->f2;_LL304:;
({struct Cyc_Set_Set*_tmp677=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp308);defined_symbols=_tmp677;});
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp308))name=0;else{
name=_tmp308;}
goto _LL2E0;}case 5: _LL2E5: _tmp31B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2E6: {
# 1735
struct _tuple1*_tmp309=_tmp31B->name;struct _tuple1*_tmp30A=_tmp309;struct _dyneither_ptr*_tmp30B;_LL306: _tmp30B=_tmp30A->f2;_LL307:;
name=_tmp30B;
goto _LL2E0;}case 7: _LL2E7: _tmp31C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2E8: {
# 1739
struct _tuple1*_tmp30C=_tmp31C->name;struct _tuple1*_tmp30D=_tmp30C;struct _dyneither_ptr*_tmp315;_LL309: _tmp315=_tmp30D->f2;_LL30A:;
name=_tmp315;
# 1743
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FC,name))
({struct Cyc_List_List*_tmp678=({struct Cyc_List_List*_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->hd=_tmp300;_tmp30E->tl=_tmp2FD;_tmp30E;});_tmp2FD=_tmp678;});else{
# 1746
if((unsigned int)_tmp31C->fields){
struct Cyc_List_List*_tmp30F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31C->fields))->v;for(0;_tmp30F != 0;_tmp30F=_tmp30F->tl){
struct Cyc_Absyn_Enumfield*_tmp310=(struct Cyc_Absyn_Enumfield*)_tmp30F->hd;
struct _tuple1*_tmp311=_tmp310->name;struct _tuple1*_tmp312=_tmp311;struct _dyneither_ptr*_tmp314;_LL30C: _tmp314=_tmp312->f2;_LL30D:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FC,_tmp314)){
({struct Cyc_List_List*_tmp679=({struct Cyc_List_List*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->hd=_tmp300;_tmp313->tl=_tmp2FD;_tmp313;});_tmp2FD=_tmp679;});
break;}}}}
# 1756
name=0;
goto _LL2E0;}case 8: _LL2E9: _tmp31D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2EA: {
# 1759
struct _tuple1*_tmp316=_tmp31D->name;struct _tuple1*_tmp317=_tmp316;struct _dyneither_ptr*_tmp318;_LL30F: _tmp318=_tmp317->f2;_LL310:;
name=_tmp318;
goto _LL2E0;}case 13: _LL2EB: _LL2EC:
 goto _LL2EE;case 14: _LL2ED: _LL2EE:
 goto _LL2F0;case 2: _LL2EF: _LL2F0:
 goto _LL2F2;case 6: _LL2F1: _LL2F2:
 goto _LL2F4;case 3: _LL2F3: _LL2F4:
 goto _LL2F6;case 9: _LL2F5: _LL2F6:
 goto _LL2F8;case 10: _LL2F7: _LL2F8:
 goto _LL2FA;case 11: _LL2F9: _LL2FA:
 goto _LL2FC;case 12: _LL2FB: _LL2FC:
 goto _LL2FE;default: _LL2FD: _LL2FE:
# 1772
 name=0;
goto _LL2E0;}_LL2E0:;}
# 1776
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FC,name)){
if(({struct _dyneither_ptr _tmp67B=(struct _dyneither_ptr)*name;struct _dyneither_ptr _tmp67A=(struct _dyneither_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp67B,_tmp67A,Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix));})!= 0)
({struct Cyc_List_List*_tmp67C=({struct Cyc_List_List*_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E->hd=_tmp300;_tmp31E->tl=_tmp2FD;_tmp31E;});_tmp2FD=_tmp67C;});else{
# 1781
Cyc_rename_decl(_tmp300);
({struct Cyc_List_List*_tmp67D=({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->hd=_tmp300;_tmp31F->tl=_tmp2FE;_tmp31F;});_tmp2FE=_tmp67D;});}}}}
# 1788
if(!Cyc_do_setjmp){
({struct Cyc___cycFILE*_tmp67E=Cyc_fopen(filename,"w");maybe=_tmp67E;});
if(!((unsigned int)maybe)){int _tmp320=1;_npop_handler(0);return _tmp320;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0;({struct _dyneither_ptr _tmp67F=(struct _dyneither_ptr)({const char*_tmp39B=filename;_tag_dyneither(_tmp39B,sizeof(char),_get_zero_arr_size_char((void*)_tmp39B,1));});_tmp39A.f1=_tmp67F;});({void*_tmp398[1]={& _tmp39A};({struct _dyneither_ptr _tmp680=({const char*_tmp399="_%s_";_tag_dyneither(_tmp399,sizeof(char),5);});Cyc_aprintf(_tmp680,_tag_dyneither(_tmp398,sizeof(void*),1));});});});
{int _tmp321=0;for(0;_tmp321 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp321){
if(((char*)ifdefmacro.curr)[_tmp321]== '.'  || ((char*)ifdefmacro.curr)[_tmp321]== '/')
({struct _dyneither_ptr _tmp322=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp321);char _tmp323=*((char*)_check_dyneither_subscript(_tmp322,sizeof(char),0));char _tmp324='_';if(_get_dyneither_size(_tmp322,sizeof(char))== 1  && (_tmp323 == '\000'  && _tmp324 != '\000'))_throw_arraybounds();*((char*)_tmp322.curr)=_tmp324;});else{
if(((char*)ifdefmacro.curr)[_tmp321]!= '_'  && ((char*)ifdefmacro.curr)[_tmp321]!= '/')
({struct _dyneither_ptr _tmp325=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp321);char _tmp326=*((char*)_check_dyneither_subscript(_tmp325,sizeof(char),0));char _tmp327=(char) toupper((int)((char*)ifdefmacro.curr)[_tmp321]);if(_get_dyneither_size(_tmp325,sizeof(char))== 1  && (_tmp326 == '\000'  && _tmp327 != '\000'))_throw_arraybounds();*((char*)_tmp325.curr)=_tmp327;});}}}
# 1800
({struct Cyc_String_pa_PrintArg_struct _tmp32B;_tmp32B.tag=0;_tmp32B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({struct Cyc_String_pa_PrintArg_struct _tmp32A;_tmp32A.tag=0;_tmp32A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp328[2]={& _tmp32A,& _tmp32B};({struct Cyc___cycFILE*_tmp682=out_file;struct _dyneither_ptr _tmp681=({const char*_tmp329="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp329,sizeof(char),23);});Cyc_fprintf(_tmp682,_tmp681,_tag_dyneither(_tmp328,sizeof(void*),2));});});});});{
# 1807
struct Cyc_List_List*_tmp32C=0;
struct Cyc_List_List*_tmp32D=0;
{struct Cyc_List_List*_tmp32E=_tmp2FD;for(0;_tmp32E != 0;_tmp32E=_tmp32E->tl){
struct Cyc_Absyn_Decl*_tmp32F=(struct Cyc_Absyn_Decl*)_tmp32E->hd;
int _tmp330=0;
struct _dyneither_ptr*name;
{void*_tmp331=_tmp32F->r;void*_tmp332=_tmp331;struct Cyc_Absyn_Typedefdecl*_tmp346;struct Cyc_Absyn_Enumdecl*_tmp345;struct Cyc_Absyn_Aggrdecl*_tmp344;struct Cyc_Absyn_Fndecl*_tmp343;struct Cyc_Absyn_Vardecl*_tmp342;switch(*((int*)_tmp332)){case 0: _LL312: _tmp342=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp332)->f1;_LL313: {
# 1815
struct _tuple1*_tmp333=_tmp342->name;struct _tuple1*_tmp334=_tmp333;struct _dyneither_ptr*_tmp335;_LL331: _tmp335=_tmp334->f2;_LL332:;
name=_tmp335;
goto _LL311;}case 1: _LL314: _tmp343=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp332)->f1;_LL315:
# 1819
 if(_tmp343->is_inline){name=0;goto _LL311;}{
struct _tuple1*_tmp336=_tmp343->name;struct _tuple1*_tmp337=_tmp336;struct _dyneither_ptr*_tmp338;_LL334: _tmp338=_tmp337->f2;_LL335:;
name=_tmp338;
goto _LL311;};case 5: _LL316: _tmp344=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp332)->f1;_LL317: {
# 1824
struct _tuple1*_tmp339=_tmp344->name;struct _tuple1*_tmp33A=_tmp339;struct _dyneither_ptr*_tmp33B;_LL337: _tmp33B=_tmp33A->f2;_LL338:;
name=_tmp33B;
goto _LL311;}case 7: _LL318: _tmp345=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp332)->f1;_LL319: {
# 1828
struct _tuple1*_tmp33C=_tmp345->name;struct _tuple1*_tmp33D=_tmp33C;struct _dyneither_ptr*_tmp33E;_LL33A: _tmp33E=_tmp33D->f2;_LL33B:;
name=_tmp33E;
goto _LL311;}case 8: _LL31A: _tmp346=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp332)->f1;_LL31B: {
# 1832
struct _tuple1*_tmp33F=_tmp346->name;struct _tuple1*_tmp340=_tmp33F;struct _dyneither_ptr*_tmp341;_LL33D: _tmp341=_tmp340->f2;_LL33E:;
name=_tmp341;
goto _LL311;}case 4: _LL31C: _LL31D:
 goto _LL31F;case 13: _LL31E: _LL31F:
 goto _LL321;case 14: _LL320: _LL321:
 goto _LL323;case 2: _LL322: _LL323:
 goto _LL325;case 6: _LL324: _LL325:
 goto _LL327;case 3: _LL326: _LL327:
 goto _LL329;case 9: _LL328: _LL329:
 goto _LL32B;case 10: _LL32A: _LL32B:
 goto _LL32D;case 11: _LL32C: _LL32D:
 goto _LL32F;default: _LL32E: _LL32F:
# 1845
 name=0;
goto _LL311;}_LL311:;}
# 1848
if(!((unsigned int)name) && !_tmp330)continue;
# 1853
if(({struct Cyc_Set_Set*_tmp683=_tmp2FC;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp683,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp347=Cyc_named_decl(_tmp2FE,name);
Cyc_merge_rep_decl(_tmp347,_tmp32F);
({struct Cyc_List_List*_tmp684=({struct Cyc_List_List*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->hd=_tmp32F;_tmp348->tl=_tmp32C;_tmp348;});_tmp32C=_tmp684;});}else{
# 1859
({struct Cyc_List_List*_tmp685=({struct Cyc_List_List*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->hd=_tmp32F;_tmp349->tl=_tmp32C;_tmp349;});_tmp32C=_tmp685;});}
({struct Cyc_List_List*_tmp686=({struct Cyc_List_List*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=name;_tmp34A->tl=_tmp32D;_tmp34A;});_tmp32D=_tmp686;});}}
# 1864
{struct _handler_cons _tmp34B;_push_handler(& _tmp34B);{int _tmp34D=0;if(setjmp(_tmp34B.handler))_tmp34D=1;if(!_tmp34D){
Cyc_Binding_resolve_all(_tmp32C);
({struct Cyc_Tcenv_Tenv*_tmp687=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp687,1,_tmp32C);});
# 1865
;_pop_handler();}else{void*_tmp34C=(void*)_exn_thrown;void*_tmp34E=_tmp34C;_LL340: _LL341:
# 1868
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp34F=_cycalloc(sizeof(*_tmp34F));({struct Cyc_NO_SUPPORT_exn_struct _tmp689=({struct Cyc_NO_SUPPORT_exn_struct _tmp350;_tmp350.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp688=({const char*_tmp351="can't typecheck acquired declarations";_tag_dyneither(_tmp351,sizeof(char),38);});_tmp350.f1=_tmp688;});_tmp350;});_tmp34F[0]=_tmp689;});_tmp34F;}));_LL33F:;}};}
# 1872
{struct _tuple0 _tmp352=({struct _tuple0 _tmp366;_tmp366.f1=_tmp32C;_tmp366.f2=_tmp32D;_tmp366;});struct _tuple0 _tmp353=_tmp352;struct Cyc_List_List*_tmp365;struct Cyc_List_List*_tmp364;_LL345: _tmp365=_tmp353.f1;_tmp364=_tmp353.f2;_LL346:;for(0;
_tmp365 != 0  && _tmp364 != 0;(_tmp365=_tmp365->tl,_tmp364=_tmp364->tl)){
struct Cyc_Absyn_Decl*_tmp354=(struct Cyc_Absyn_Decl*)_tmp365->hd;
struct _dyneither_ptr*_tmp355=(struct _dyneither_ptr*)_tmp364->hd;
int _tmp356=0;
if(!((unsigned int)_tmp355))
_tmp356=1;
# 1881
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp355){
({struct _dyneither_ptr _tmp68B=({struct Cyc_String_pa_PrintArg_struct _tmp359;_tmp359.tag=0;_tmp359.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp355);({void*_tmp357[1]={& _tmp359};({struct _dyneither_ptr _tmp68A=({const char*_tmp358="_%s_def_";_tag_dyneither(_tmp358,sizeof(char),9);});Cyc_aprintf(_tmp68A,_tag_dyneither(_tmp357,sizeof(void*),1));});});});ifdefmacro=_tmp68B;});
({struct Cyc_String_pa_PrintArg_struct _tmp35C;_tmp35C.tag=0;_tmp35C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp35A[1]={& _tmp35C};({struct Cyc___cycFILE*_tmp68D=out_file;struct _dyneither_ptr _tmp68C=({const char*_tmp35B="#ifndef %s\n";_tag_dyneither(_tmp35B,sizeof(char),12);});Cyc_fprintf(_tmp68D,_tmp68C,_tag_dyneither(_tmp35A,sizeof(void*),1));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp35F;_tmp35F.tag=0;_tmp35F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp35D[1]={& _tmp35F};({struct Cyc___cycFILE*_tmp68F=out_file;struct _dyneither_ptr _tmp68E=({const char*_tmp35E="#define %s\n";_tag_dyneither(_tmp35E,sizeof(char),12);});Cyc_fprintf(_tmp68F,_tmp68E,_tag_dyneither(_tmp35D,sizeof(void*),1));});});});
# 1887
({struct Cyc_List_List*_tmp690=({struct Cyc_Absyn_Decl*_tmp360[1];_tmp360[0]=_tmp354;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp360,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_tmp690,out_file);});
({void*_tmp361=0;({struct Cyc___cycFILE*_tmp692=out_file;struct _dyneither_ptr _tmp691=({const char*_tmp362="#endif\n";_tag_dyneither(_tmp362,sizeof(char),8);});Cyc_fprintf(_tmp692,_tmp691,_tag_dyneither(_tmp361,sizeof(void*),0));});});}else{
# 1892
({struct Cyc_List_List*_tmp693=({struct Cyc_Absyn_Decl*_tmp363[1];_tmp363[0]=_tmp354;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp363,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_tmp693,out_file);});}}}
# 1897
({struct Cyc___cycFILE*_tmp694=Cyc_fopen(_tmp286,"r");maybe=_tmp694;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp367=_cycalloc(sizeof(*_tmp367));({struct Cyc_NO_SUPPORT_exn_struct _tmp698=({struct Cyc_NO_SUPPORT_exn_struct _tmp368;_tmp368.tag=Cyc_NO_SUPPORT;({struct _dyneither_ptr _tmp697=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp36B;_tmp36B.tag=0;({struct _dyneither_ptr _tmp695=(struct _dyneither_ptr)({const char*_tmp36C=_tmp286;_tag_dyneither(_tmp36C,sizeof(char),_get_zero_arr_size_char((void*)_tmp36C,1));});_tmp36B.f1=_tmp695;});({void*_tmp369[1]={& _tmp36B};({struct _dyneither_ptr _tmp696=({const char*_tmp36A="can't open macrosfile %s";_tag_dyneither(_tmp36A,sizeof(char),25);});Cyc_aprintf(_tmp696,_tag_dyneither(_tmp369,sizeof(void*),1));});});});_tmp368.f1=_tmp697;});_tmp368;});_tmp367[0]=_tmp698;});_tmp367;}));
# 1900
in_file=maybe;
({struct Cyc_Lexing_lexbuf*_tmp699=Cyc_Lexing_from_file(in_file);_tmp2E4=_tmp699;});{
struct _tuple22*entry2;
while(({struct _tuple22*_tmp69A=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp2E4);entry2=_tmp69A;})!= 0){
struct _tuple22*_tmp36D=(struct _tuple22*)_check_null(entry2);struct _tuple22*_tmp36E=_tmp36D;struct _dyneither_ptr _tmp378;struct _dyneither_ptr*_tmp377;_LL348: _tmp378=_tmp36E->f1;_tmp377=_tmp36E->f2;_LL349:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2FC,_tmp377)){
({struct Cyc_String_pa_PrintArg_struct _tmp371;_tmp371.tag=0;_tmp371.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp377);({void*_tmp36F[1]={& _tmp371};({struct Cyc___cycFILE*_tmp69C=out_file;struct _dyneither_ptr _tmp69B=({const char*_tmp370="#ifndef %s\n";_tag_dyneither(_tmp370,sizeof(char),12);});Cyc_fprintf(_tmp69C,_tmp69B,_tag_dyneither(_tmp36F,sizeof(void*),1));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp374;_tmp374.tag=0;_tmp374.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp378);({void*_tmp372[1]={& _tmp374};({struct Cyc___cycFILE*_tmp69E=out_file;struct _dyneither_ptr _tmp69D=({const char*_tmp373="%s\n";_tag_dyneither(_tmp373,sizeof(char),4);});Cyc_fprintf(_tmp69E,_tmp69D,_tag_dyneither(_tmp372,sizeof(void*),1));});});});
({void*_tmp375=0;({struct Cyc___cycFILE*_tmp6A0=out_file;struct _dyneither_ptr _tmp69F=({const char*_tmp376="#endif\n";_tag_dyneither(_tmp376,sizeof(char),8);});Cyc_fprintf(_tmp6A0,_tmp69F,_tag_dyneither(_tmp375,sizeof(void*),0));});});}}
# 1911
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1914
if(hstubs != 0){
struct Cyc_List_List*_tmp379=hstubs;for(0;_tmp379 != 0;_tmp379=_tmp379->tl){
struct _tuple23*_tmp37A=(struct _tuple23*)_tmp379->hd;struct _tuple23*_tmp37B=_tmp37A;struct _dyneither_ptr _tmp381;struct _dyneither_ptr _tmp380;_LL34B: _tmp381=_tmp37B->f1;_tmp380=_tmp37B->f2;_LL34C:;
if(({char*_tmp6A3=(char*)_tmp380.curr;_tmp6A3 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6A2=(char*)_tmp381.curr;_tmp6A2 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6A1=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6A1,({struct _dyneither_ptr*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=_tmp381;_tmp37C;}));})))
# 1920
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp380,sizeof(char),1)),out_file);else{
# 1922
({struct Cyc_String_pa_PrintArg_struct _tmp37F;_tmp37F.tag=0;_tmp37F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp381);({void*_tmp37D[1]={& _tmp37F};({struct _dyneither_ptr _tmp6A4=({const char*_tmp37E="%s is not supported on this platform\n";_tag_dyneither(_tmp37E,sizeof(char),38);});Cyc_log(_tmp6A4,_tag_dyneither(_tmp37D,sizeof(void*),1));});});});}}}
# 1925
({void*_tmp382=0;({struct Cyc___cycFILE*_tmp6A6=out_file;struct _dyneither_ptr _tmp6A5=({const char*_tmp383="#endif\n";_tag_dyneither(_tmp383,sizeof(char),8);});Cyc_fprintf(_tmp6A6,_tmp6A5,_tag_dyneither(_tmp382,sizeof(void*),0));});});
if(Cyc_do_setjmp){int _tmp384=0;_npop_handler(0);return _tmp384;}else{
Cyc_fclose(out_file);}
# 1930
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp385=cstubs;for(0;_tmp385 != 0;_tmp385=_tmp385->tl){
struct _tuple23*_tmp386=(struct _tuple23*)_tmp385->hd;struct _tuple23*_tmp387=_tmp386;struct _dyneither_ptr _tmp38A;struct _dyneither_ptr _tmp389;_LL34E: _tmp38A=_tmp387->f1;_tmp389=_tmp387->f2;_LL34F:;
if(({char*_tmp6A9=(char*)_tmp389.curr;_tmp6A9 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6A8=(char*)_tmp38A.curr;_tmp6A8 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6A7=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6A7,({struct _dyneither_ptr*_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=_tmp38A;_tmp388;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp389,sizeof(char),1)),out_file);}};}
# 1941
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1943
({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0;({struct _dyneither_ptr _tmp6AA=(struct _dyneither_ptr)({const char*_tmp38E=filename;_tag_dyneither(_tmp38E,sizeof(char),_get_zero_arr_size_char((void*)_tmp38E,1));});_tmp38D.f1=_tmp6AA;});({void*_tmp38B[1]={& _tmp38D};({struct Cyc___cycFILE*_tmp6AC=out_file;struct _dyneither_ptr _tmp6AB=({const char*_tmp38C="#include <%s>\n\n";_tag_dyneither(_tmp38C,sizeof(char),16);});Cyc_fprintf(_tmp6AC,_tmp6AB,_tag_dyneither(_tmp38B,sizeof(void*),1));});});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp38F=cycstubs;for(0;_tmp38F != 0;_tmp38F=_tmp38F->tl){
struct _tuple23*_tmp390=(struct _tuple23*)_tmp38F->hd;struct _tuple23*_tmp391=_tmp390;struct _dyneither_ptr _tmp394;struct _dyneither_ptr _tmp393;_LL351: _tmp394=_tmp391->f1;_tmp393=_tmp391->f2;_LL352:;
if(({char*_tmp6AF=(char*)_tmp393.curr;_tmp6AF != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6AE=(char*)_tmp394.curr;_tmp6AE == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6AD=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6AD,({struct _dyneither_ptr*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392[0]=_tmp394;_tmp392;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp393,sizeof(char),1)),out_file);}}
# 1952
({void*_tmp395=0;({struct Cyc___cycFILE*_tmp6B1=out_file;struct _dyneither_ptr _tmp6B0=({const char*_tmp396="\n";_tag_dyneither(_tmp396,sizeof(char),2);});Cyc_fprintf(_tmp6B1,_tmp6B0,_tag_dyneither(_tmp395,sizeof(void*),0));});});}{
# 1955
int _tmp397=0;_npop_handler(0);return _tmp397;};};};};};};};};};
# 1603
;_pop_handler();}else{void*_tmp28A=(void*)_exn_thrown;void*_tmp39C=_tmp28A;void*_tmp3B3;struct _dyneither_ptr _tmp3B2;struct _dyneither_ptr _tmp3B1;struct _dyneither_ptr _tmp3B0;struct _dyneither_ptr _tmp3AF;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp39C)->tag == Cyc_Core_Impossible){_LL354: _tmp3AF=((struct Cyc_Core_Impossible_exn_struct*)_tmp39C)->f1;_LL355:
# 1959
({struct Cyc_String_pa_PrintArg_struct _tmp39F;_tmp39F.tag=0;_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AF);({void*_tmp39D[1]={& _tmp39F};({struct Cyc___cycFILE*_tmp6B3=Cyc_stderr;struct _dyneither_ptr _tmp6B2=({const char*_tmp39E="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp39E,sizeof(char),26);});Cyc_fprintf(_tmp6B3,_tmp6B2,_tag_dyneither(_tmp39D,sizeof(void*),1));});});});goto _LL353;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp39C)->tag == Cyc_Dict_Absent){_LL356: _LL357:
# 1961
({void*_tmp3A0=0;({struct Cyc___cycFILE*_tmp6B5=Cyc_stderr;struct _dyneither_ptr _tmp6B4=({const char*_tmp3A1="Got Dict::Absent\n";_tag_dyneither(_tmp3A1,sizeof(char),18);});Cyc_fprintf(_tmp6B5,_tmp6B4,_tag_dyneither(_tmp3A0,sizeof(void*),0));});});goto _LL353;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp39C)->tag == Cyc_Core_Failure){_LL358: _tmp3B0=((struct Cyc_Core_Failure_exn_struct*)_tmp39C)->f1;_LL359:
# 1963
({struct Cyc_String_pa_PrintArg_struct _tmp3A4;_tmp3A4.tag=0;_tmp3A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B0);({void*_tmp3A2[1]={& _tmp3A4};({struct Cyc___cycFILE*_tmp6B7=Cyc_stderr;struct _dyneither_ptr _tmp6B6=({const char*_tmp3A3="Got Core::Failure(%s)\n";_tag_dyneither(_tmp3A3,sizeof(char),23);});Cyc_fprintf(_tmp6B7,_tmp6B6,_tag_dyneither(_tmp3A2,sizeof(void*),1));});});});goto _LL353;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39C)->tag == Cyc_Core_Invalid_argument){_LL35A: _tmp3B1=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39C)->f1;_LL35B:
# 1965
({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B1);({void*_tmp3A5[1]={& _tmp3A7};({struct Cyc___cycFILE*_tmp6B9=Cyc_stderr;struct _dyneither_ptr _tmp6B8=({const char*_tmp3A6="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp3A6,sizeof(char),26);});Cyc_fprintf(_tmp6B9,_tmp6B8,_tag_dyneither(_tmp3A5,sizeof(void*),1));});});});goto _LL353;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp39C)->tag == Cyc_Core_Not_found){_LL35C: _LL35D:
# 1967
({void*_tmp3A8=0;({struct Cyc___cycFILE*_tmp6BB=Cyc_stderr;struct _dyneither_ptr _tmp6BA=({const char*_tmp3A9="Got Not_found\n";_tag_dyneither(_tmp3A9,sizeof(char),15);});Cyc_fprintf(_tmp6BB,_tmp6BA,_tag_dyneither(_tmp3A8,sizeof(void*),0));});});goto _LL353;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp39C)->tag == Cyc_NO_SUPPORT){_LL35E: _tmp3B2=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp39C)->f1;_LL35F:
# 1969
({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0;_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B2);({void*_tmp3AA[1]={& _tmp3AC};({struct Cyc___cycFILE*_tmp6BD=Cyc_stderr;struct _dyneither_ptr _tmp6BC=({const char*_tmp3AB="No support because %s\n";_tag_dyneither(_tmp3AB,sizeof(char),23);});Cyc_fprintf(_tmp6BD,_tmp6BC,_tag_dyneither(_tmp3AA,sizeof(void*),1));});});});goto _LL353;}else{_LL360: _tmp3B3=_tmp39C;_LL361:
# 1971
({void*_tmp3AD=0;({struct Cyc___cycFILE*_tmp6BF=Cyc_stderr;struct _dyneither_ptr _tmp6BE=({const char*_tmp3AE="Got unknown exception\n";_tag_dyneither(_tmp3AE,sizeof(char),23);});Cyc_fprintf(_tmp6BF,_tmp6BE,_tag_dyneither(_tmp3AD,sizeof(void*),0));});});
Cyc_Core_rethrow(_tmp3B3);}}}}}}_LL353:;}};}
# 1977
({struct Cyc___cycFILE*_tmp6C0=Cyc_fopen(filename,"w");maybe=_tmp6C0;});
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0;({struct _dyneither_ptr _tmp6C1=(struct _dyneither_ptr)({const char*_tmp3B7=filename;_tag_dyneither(_tmp3B7,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B7,1));});_tmp3B6.f1=_tmp6C1;});({void*_tmp3B4[1]={& _tmp3B6};({struct Cyc___cycFILE*_tmp6C3=Cyc_stderr;struct _dyneither_ptr _tmp6C2=({const char*_tmp3B5="Error: could not create file %s\n";_tag_dyneither(_tmp3B5,sizeof(char),33);});Cyc_fprintf(_tmp6C3,_tmp6C2,_tag_dyneither(_tmp3B4,sizeof(void*),1));});});});
return 1;}
# 1982
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0;({struct _dyneither_ptr _tmp6C4=(struct _dyneither_ptr)({const char*_tmp3BB=filename;_tag_dyneither(_tmp3BB,sizeof(char),_get_zero_arr_size_char((void*)_tmp3BB,1));});_tmp3BA.f1=_tmp6C4;});({void*_tmp3B8[1]={& _tmp3BA};({struct Cyc___cycFILE*_tmp6C6=out_file;struct _dyneither_ptr _tmp6C5=({const char*_tmp3B9="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp3B9,sizeof(char),48);});Cyc_fprintf(_tmp6C6,_tmp6C5,_tag_dyneither(_tmp3B8,sizeof(void*),1));});});});
# 1986
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0;({struct _dyneither_ptr _tmp6C7=(struct _dyneither_ptr)({const char*_tmp3BF=filename;_tag_dyneither(_tmp3BF,sizeof(char),_get_zero_arr_size_char((void*)_tmp3BF,1));});_tmp3BE.f1=_tmp6C7;});({void*_tmp3BC[1]={& _tmp3BE};({struct Cyc___cycFILE*_tmp6C9=Cyc_stderr;struct _dyneither_ptr _tmp6C8=({const char*_tmp3BD="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp3BD,sizeof(char),52);});Cyc_fprintf(_tmp6C9,_tmp6C8,_tag_dyneither(_tmp3BC,sizeof(void*),1));});});});
# 1989
({void*_tmp3C0=0;({struct _dyneither_ptr _tmp6CA=({const char*_tmp3C1="Not supported on this platform\n";_tag_dyneither(_tmp3C1,sizeof(char),32);});Cyc_log(_tmp6CA,_tag_dyneither(_tmp3C0,sizeof(void*),0));});});
# 1996
return 0;};}
# 2000
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0;({struct _dyneither_ptr _tmp6CB=(struct _dyneither_ptr)({const char*_tmp3DA=file;_tag_dyneither(_tmp3DA,sizeof(char),_get_zero_arr_size_char((void*)_tmp3DA,1));});_tmp3D9.f1=_tmp6CB;});({void*_tmp3D7[1]={& _tmp3D9};({struct Cyc___cycFILE*_tmp6CD=Cyc_stderr;struct _dyneither_ptr _tmp6CC=({const char*_tmp3D8="Processing %s\n";_tag_dyneither(_tmp3D8,sizeof(char),15);});Cyc_fprintf(_tmp6CD,_tmp6CC,_tag_dyneither(_tmp3D7,sizeof(void*),1));});});});{
struct Cyc___cycFILE*_tmp3DB=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp3DB)){
({struct Cyc_String_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=0;({struct _dyneither_ptr _tmp6CE=(struct _dyneither_ptr)({const char*_tmp3DF=file;_tag_dyneither(_tmp3DF,sizeof(char),_get_zero_arr_size_char((void*)_tmp3DF,1));});_tmp3DE.f1=_tmp6CE;});({void*_tmp3DC[1]={& _tmp3DE};({struct Cyc___cycFILE*_tmp6D0=Cyc_stderr;struct _dyneither_ptr _tmp6CF=({const char*_tmp3DD="Error: could not open %s\n";_tag_dyneither(_tmp3DD,sizeof(char),26);});Cyc_fprintf(_tmp6D0,_tmp6CF,_tag_dyneither(_tmp3DC,sizeof(void*),1));});});});
return 1;}{
# 2008
struct Cyc___cycFILE*_tmp3E0=_tmp3DB;
# 2012
struct _dyneither_ptr buf=({char*_tmp3FF=({unsigned int _tmp3FC=(unsigned int)1024;char*_tmp3FD=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp3FC + 1));{unsigned int _tmp3FE=_tmp3FC;unsigned int i;for(i=0;i < _tmp3FE;i ++){_tmp3FD[i]=(char)'\000';}_tmp3FD[_tmp3FE]=(char)0;}_tmp3FD;});_tag_dyneither(_tmp3FF,sizeof(char),_get_zero_arr_size_char((void*)_tmp3FF,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp3E1=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0;({struct _dyneither_ptr _tmp6D1=(struct _dyneither_ptr)({const char*_tmp3E5=dir;_tag_dyneither(_tmp3E5,sizeof(char),_get_zero_arr_size_char((void*)_tmp3E5,1));});_tmp3E4.f1=_tmp6D1;});({void*_tmp3E2[1]={& _tmp3E4};({struct Cyc___cycFILE*_tmp6D3=Cyc_stderr;struct _dyneither_ptr _tmp6D2=({const char*_tmp3E3="Error: can't change directory to %s\n";_tag_dyneither(_tmp3E3,sizeof(char),37);});Cyc_fprintf(_tmp6D3,_tmp6D2,_tag_dyneither(_tmp3E2,sizeof(void*),1));});});});
return 1;}}
# 2020
if(Cyc_mode == Cyc_GATHER){
# 2022
struct _dyneither_ptr _tmp3E6=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0;_tmp3EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0;_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0;_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp3EA[3]={& _tmp3EC,& _tmp3ED,& _tmp3EE};({struct _dyneither_ptr _tmp6D4=({const char*_tmp3EB="echo | %s %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp3EB,sizeof(char),42);});Cyc_aprintf(_tmp6D4,_tag_dyneither(_tmp3EA,sizeof(void*),3));});});});});});
# 2024
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0;_tmp3E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E6);({void*_tmp3E7[1]={& _tmp3E9};({struct Cyc___cycFILE*_tmp6D6=Cyc_stderr;struct _dyneither_ptr _tmp6D5=({const char*_tmp3E8="%s\n";_tag_dyneither(_tmp3E8,sizeof(char),4);});Cyc_fprintf(_tmp6D6,_tmp6D5,_tag_dyneither(_tmp3E7,sizeof(void*),1));});});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp3E6,sizeof(char),1)));}{
# 2029
struct Cyc_Lexing_lexbuf*_tmp3EF=Cyc_Lexing_from_file(_tmp3E0);
struct _tuple25*entry;
while(({struct _tuple25*_tmp6D7=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp3EF);entry=_tmp6D7;})!= 0){
struct _tuple25*_tmp3F0=(struct _tuple25*)_check_null(entry);struct _tuple25*_tmp3F1=_tmp3F0;struct _dyneither_ptr _tmp3F8;struct Cyc_List_List*_tmp3F7;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F4;struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*_tmp3F2;_LL365: _tmp3F8=_tmp3F1->f1;_tmp3F7=_tmp3F1->f2;_tmp3F6=_tmp3F1->f3;_tmp3F5=_tmp3F1->f4;_tmp3F4=_tmp3F1->f5;_tmp3F3=_tmp3F1->f6;_tmp3F2=_tmp3F1->f7;_LL366:;
# 2034
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp3F8,sizeof(char),1)),_tmp3F7,_tmp3F6,_tmp3F5,_tmp3F4,_tmp3F3,_tmp3F2))
# 2036
return 1;}
# 2038
Cyc_fclose(_tmp3E0);
# 2040
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3E1,sizeof(char),1))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3FB;_tmp3FB.tag=0;_tmp3FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E1);({void*_tmp3F9[1]={& _tmp3FB};({struct Cyc___cycFILE*_tmp6D9=Cyc_stderr;struct _dyneither_ptr _tmp6D8=({const char*_tmp3FA="Error: could not change directory to %s\n";_tag_dyneither(_tmp3FA,sizeof(char),41);});Cyc_fprintf(_tmp6D9,_tmp6D8,_tag_dyneither(_tmp3F9,sizeof(void*),1));});});});
return 1;}}
# 2046
return 0;};};};}
# 2050
int Cyc_process_setjmp(const char*dir){
# 2053
struct _dyneither_ptr buf=({char*_tmp411=({unsigned int _tmp40E=(unsigned int)1024;char*_tmp40F=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp40E + 1));{unsigned int _tmp410=_tmp40E;unsigned int i;for(i=0;i < _tmp410;i ++){_tmp40F[i]=(char)'\000';}_tmp40F[_tmp410]=(char)0;}_tmp40F;});_tag_dyneither(_tmp411,sizeof(char),_get_zero_arr_size_char((void*)_tmp411,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp400=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp403;_tmp403.tag=0;({struct _dyneither_ptr _tmp6DA=(struct _dyneither_ptr)({const char*_tmp404=dir;_tag_dyneither(_tmp404,sizeof(char),_get_zero_arr_size_char((void*)_tmp404,1));});_tmp403.f1=_tmp6DA;});({void*_tmp401[1]={& _tmp403};({struct Cyc___cycFILE*_tmp6DC=Cyc_stderr;struct _dyneither_ptr _tmp6DB=({const char*_tmp402="Error: can't change directory to %s\n";_tag_dyneither(_tmp402,sizeof(char),37);});Cyc_fprintf(_tmp6DC,_tmp6DB,_tag_dyneither(_tmp401,sizeof(void*),1));});});});
return 1;}
# 2059
if(({struct Cyc_List_List*_tmp6E1=({struct _dyneither_ptr*_tmp405[1];({struct _dyneither_ptr*_tmp6DD=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8);_tmp405[0]=_tmp6DD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp405,sizeof(struct _dyneither_ptr*),1));});Cyc_process_file("setjmp.h",_tmp6E1,0,0,({struct _tuple23*_tmp407[1];({struct _tuple23*_tmp6E0=({struct _tuple23*_tmp408=_cycalloc(sizeof(*_tmp408));({struct _dyneither_ptr _tmp6DE=({const char*_tmp40A="setjmp";_tag_dyneither(_tmp40A,sizeof(char),7);});_tmp408->f1=_tmp6DE;});({struct _dyneither_ptr _tmp6DF=({const char*_tmp409="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp409,sizeof(char),29);});_tmp408->f2=_tmp6DF;});_tmp408;});_tmp407[0]=_tmp6E0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp407,sizeof(struct _tuple23*),1));}),0,0);}))
# 2062
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp400,sizeof(char),1))))){
({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0;_tmp40D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp400);({void*_tmp40B[1]={& _tmp40D};({struct Cyc___cycFILE*_tmp6E3=Cyc_stderr;struct _dyneither_ptr _tmp6E2=({const char*_tmp40C="Error: could not change directory to %s\n";_tag_dyneither(_tmp40C,sizeof(char),41);});Cyc_fprintf(_tmp6E3,_tmp6E2,_tag_dyneither(_tmp40B,sizeof(void*),1));});});});
return 1;}
# 2067
return 0;}static char _tmp412[13]="BUILDLIB.OUT";
# 2071
static struct _dyneither_ptr Cyc_output_dir={_tmp412,_tmp412,_tmp412 + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 2075
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp6E4=({struct Cyc_List_List*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1));_tmp413->tl=Cyc_spec_files;_tmp413;});Cyc_spec_files=_tmp6E4;});}
# 2079
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 2083
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 2086
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 2089
static void Cyc_add_cpparg(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp6E6=({struct Cyc_List_List*_tmp414=_cycalloc(sizeof(*_tmp414));({struct _dyneither_ptr*_tmp6E5=({struct _dyneither_ptr*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=s;_tmp415;});_tmp414->hd=_tmp6E5;});_tmp414->tl=Cyc_cppargs;_tmp414;});Cyc_cppargs=_tmp6E6;});}
# 2092
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp418;_tmp418.tag=0;_tmp418.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp416[1]={& _tmp418};({struct Cyc___cycFILE*_tmp6E8=Cyc_stderr;struct _dyneither_ptr _tmp6E7=({const char*_tmp417="Unsupported option %s\n";_tag_dyneither(_tmp417,sizeof(char),23);});Cyc_fprintf(_tmp6E8,_tmp6E7,_tag_dyneither(_tmp416,sizeof(void*),1));});});});
Cyc_badparse=1;}
# 2101
void GC_blacklist_warn_clear();struct _tuple29{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 2105
struct Cyc_List_List*options=({struct _tuple29*_tmp450[9];({struct _tuple29*_tmp6EE=({struct _tuple29*_tmp481=_cycalloc(sizeof(*_tmp481));({struct _dyneither_ptr _tmp6E9=({const char*_tmp486="-";_tag_dyneither(_tmp486,sizeof(char),2);});_tmp481->f1=_tmp6E9;});_tmp481->f2=1;({struct _dyneither_ptr _tmp6EA=({const char*_tmp485="";_tag_dyneither(_tmp485,sizeof(char),1);});_tmp481->f3=_tmp6EA;});({void*_tmp6EC=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp483=_cycalloc(sizeof(*_tmp483));({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp6EB=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp484;_tmp484.tag=1;_tmp484.f1=Cyc_add_cpparg;_tmp484;});_tmp483[0]=_tmp6EB;});_tmp483;});_tmp481->f4=_tmp6EC;});({struct _dyneither_ptr _tmp6ED=({const char*_tmp482="";_tag_dyneither(_tmp482,sizeof(char),1);});_tmp481->f5=_tmp6ED;});_tmp481;});_tmp450[8]=_tmp6EE;});({struct _tuple29*_tmp6F4=({struct _tuple29*_tmp47B=_cycalloc(sizeof(*_tmp47B));({struct _dyneither_ptr _tmp6EF=({const char*_tmp480="-v";_tag_dyneither(_tmp480,sizeof(char),3);});_tmp47B->f1=_tmp6EF;});_tmp47B->f2=0;({struct _dyneither_ptr _tmp6F0=({const char*_tmp47F="";_tag_dyneither(_tmp47F,sizeof(char),1);});_tmp47B->f3=_tmp6F0;});({void*_tmp6F2=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp6F1=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp47E;_tmp47E.tag=3;_tmp47E.f1=& Cyc_verbose;_tmp47E;});_tmp47D[0]=_tmp6F1;});_tmp47D;});_tmp47B->f4=_tmp6F2;});({struct _dyneither_ptr _tmp6F3=({const char*_tmp47C="Verbose operation";_tag_dyneither(_tmp47C,sizeof(char),18);});_tmp47B->f5=_tmp6F3;});_tmp47B;});_tmp450[7]=_tmp6F4;});({struct _tuple29*_tmp6FA=({struct _tuple29*_tmp475=_cycalloc(sizeof(*_tmp475));({struct _dyneither_ptr _tmp6F5=({const char*_tmp47A="-B";_tag_dyneither(_tmp47A,sizeof(char),3);});_tmp475->f1=_tmp6F5;});_tmp475->f2=1;({struct _dyneither_ptr _tmp6F6=({const char*_tmp479="<file>";_tag_dyneither(_tmp479,sizeof(char),7);});_tmp475->f3=_tmp6F6;});({void*_tmp6F8=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp477=_cycalloc(sizeof(*_tmp477));({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp6F7=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp478;_tmp478.tag=1;_tmp478.f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp478;});_tmp477[0]=_tmp6F7;});_tmp477;});_tmp475->f4=_tmp6F8;});({struct _dyneither_ptr _tmp6F9=({const char*_tmp476="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp476,sizeof(char),60);});_tmp475->f5=_tmp6F9;});_tmp475;});_tmp450[6]=_tmp6FA;});({struct _tuple29*_tmp700=({struct _tuple29*_tmp46F=_cycalloc(sizeof(*_tmp46F));({struct _dyneither_ptr _tmp6FB=({const char*_tmp474="-b";_tag_dyneither(_tmp474,sizeof(char),3);});_tmp46F->f1=_tmp6FB;});_tmp46F->f2=0;({struct _dyneither_ptr _tmp6FC=({const char*_tmp473=" <machine>";_tag_dyneither(_tmp473,sizeof(char),11);});_tmp46F->f3=_tmp6FC;});({void*_tmp6FE=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp471=_cycalloc(sizeof(*_tmp471));({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp6FD=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp472;_tmp472.tag=5;_tmp472.f1=Cyc_Specsfile_set_target_arch;_tmp472;});_tmp471[0]=_tmp6FD;});_tmp471;});_tmp46F->f4=_tmp6FE;});({struct _dyneither_ptr _tmp6FF=({const char*_tmp470="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp470,sizeof(char),52);});_tmp46F->f5=_tmp6FF;});_tmp46F;});_tmp450[5]=_tmp700;});({struct _tuple29*_tmp706=({struct _tuple29*_tmp469=_cycalloc(sizeof(*_tmp469));({struct _dyneither_ptr _tmp701=({const char*_tmp46E="-setjmp";_tag_dyneither(_tmp46E,sizeof(char),8);});_tmp469->f1=_tmp701;});_tmp469->f2=0;({struct _dyneither_ptr _tmp702=({const char*_tmp46D="";_tag_dyneither(_tmp46D,sizeof(char),1);});_tmp469->f3=_tmp702;});({void*_tmp704=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp703=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp46C;_tmp46C.tag=3;_tmp46C.f1=& Cyc_do_setjmp;_tmp46C;});_tmp46B[0]=_tmp703;});_tmp46B;});_tmp469->f4=_tmp704;});({struct _dyneither_ptr _tmp705=({const char*_tmp46A="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp46A,sizeof(char),186);});_tmp469->f5=_tmp705;});_tmp469;});_tmp450[4]=_tmp706;});({struct _tuple29*_tmp70C=({struct _tuple29*_tmp463=_cycalloc(sizeof(*_tmp463));({struct _dyneither_ptr _tmp707=({const char*_tmp468="-finish";_tag_dyneither(_tmp468,sizeof(char),8);});_tmp463->f1=_tmp707;});_tmp463->f2=0;({struct _dyneither_ptr _tmp708=({const char*_tmp467="";_tag_dyneither(_tmp467,sizeof(char),1);});_tmp463->f3=_tmp708;});({void*_tmp70A=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp465=_cycalloc(sizeof(*_tmp465));({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp709=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp466;_tmp466.tag=0;_tmp466.f1=Cyc_set_FINISH;_tmp466;});_tmp465[0]=_tmp709;});_tmp465;});_tmp463->f4=_tmp70A;});({struct _dyneither_ptr _tmp70B=({const char*_tmp464="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp464,sizeof(char),57);});_tmp463->f5=_tmp70B;});_tmp463;});_tmp450[3]=_tmp70C;});({struct _tuple29*_tmp712=({struct _tuple29*_tmp45D=_cycalloc(sizeof(*_tmp45D));({struct _dyneither_ptr _tmp70D=({const char*_tmp462="-gatherscript";_tag_dyneither(_tmp462,sizeof(char),14);});_tmp45D->f1=_tmp70D;});_tmp45D->f2=0;({struct _dyneither_ptr _tmp70E=({const char*_tmp461="";_tag_dyneither(_tmp461,sizeof(char),1);});_tmp45D->f3=_tmp70E;});({void*_tmp710=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp70F=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp460;_tmp460.tag=0;_tmp460.f1=Cyc_set_GATHERSCRIPT;_tmp460;});_tmp45F[0]=_tmp70F;});_tmp45F;});_tmp45D->f4=_tmp710;});({struct _dyneither_ptr _tmp711=({const char*_tmp45E="Produce a script to gather C library info";_tag_dyneither(_tmp45E,sizeof(char),42);});_tmp45D->f5=_tmp711;});_tmp45D;});_tmp450[2]=_tmp712;});({struct _tuple29*_tmp718=({struct _tuple29*_tmp457=_cycalloc(sizeof(*_tmp457));({struct _dyneither_ptr _tmp713=({const char*_tmp45C="-gather";_tag_dyneither(_tmp45C,sizeof(char),8);});_tmp457->f1=_tmp713;});_tmp457->f2=0;({struct _dyneither_ptr _tmp714=({const char*_tmp45B="";_tag_dyneither(_tmp45B,sizeof(char),1);});_tmp457->f3=_tmp714;});({void*_tmp716=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp459=_cycalloc(sizeof(*_tmp459));({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp715=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp45A;_tmp45A.tag=0;_tmp45A.f1=Cyc_set_GATHER;_tmp45A;});_tmp459[0]=_tmp715;});_tmp459;});_tmp457->f4=_tmp716;});({struct _dyneither_ptr _tmp717=({const char*_tmp458="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp458,sizeof(char),56);});_tmp457->f5=_tmp717;});_tmp457;});_tmp450[1]=_tmp718;});({struct _tuple29*_tmp71E=({struct _tuple29*_tmp451=_cycalloc(sizeof(*_tmp451));({struct _dyneither_ptr _tmp719=({const char*_tmp456="-d";_tag_dyneither(_tmp456,sizeof(char),3);});_tmp451->f1=_tmp719;});_tmp451->f2=0;({struct _dyneither_ptr _tmp71A=({const char*_tmp455=" <file>";_tag_dyneither(_tmp455,sizeof(char),8);});_tmp451->f3=_tmp71A;});({void*_tmp71C=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp453=_cycalloc(sizeof(*_tmp453));({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp71B=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp454;_tmp454.tag=5;_tmp454.f1=Cyc_set_output_dir;_tmp454;});_tmp453[0]=_tmp71B;});_tmp453;});_tmp451->f4=_tmp71C;});({struct _dyneither_ptr _tmp71D=({const char*_tmp452="Set the output directory to <file>";_tag_dyneither(_tmp452,sizeof(char),35);});_tmp451->f5=_tmp71D;});_tmp451;});_tmp450[0]=_tmp71E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp450,sizeof(struct _tuple29*),9));});
# 2138
struct _dyneither_ptr _tmp419=({struct Cyc_List_List*_tmp720=options;struct _dyneither_ptr _tmp71F=({const char*_tmp44F="Options:";_tag_dyneither(_tmp44F,sizeof(char),9);});Cyc_Specsfile_parse_b(_tmp720,Cyc_add_spec_file,Cyc_no_other,_tmp71F,argv);});
# 2140
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp722=options;struct _dyneither_ptr _tmp721=({const char*_tmp41A="Options:";_tag_dyneither(_tmp41A,sizeof(char),9);});Cyc_Arg_parse(_tmp722,Cyc_add_spec_file,Cyc_no_other,_tmp721,_tmp419);});
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp723=options;Cyc_Arg_usage(_tmp723,({const char*_tmp41B="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp41B,sizeof(char),59);}));});
# 2150
return 1;}{
# 2156
struct _dyneither_ptr _tmp41C=Cyc_Specsfile_find_in_arch_path(({const char*_tmp44E="cycspecs";_tag_dyneither(_tmp44E,sizeof(char),9);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp41F;_tmp41F.tag=0;_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41C);({void*_tmp41D[1]={& _tmp41F};({struct Cyc___cycFILE*_tmp725=Cyc_stderr;struct _dyneither_ptr _tmp724=({const char*_tmp41E="Reading from specs file %s\n";_tag_dyneither(_tmp41E,sizeof(char),28);});Cyc_fprintf(_tmp725,_tmp724,_tag_dyneither(_tmp41D,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp420=Cyc_Specsfile_read_specs(_tmp41C);
({struct _dyneither_ptr _tmp727=({struct Cyc_List_List*_tmp726=_tmp420;Cyc_Specsfile_get_spec(_tmp726,({const char*_tmp421="cyclone_target_cflags";_tag_dyneither(_tmp421,sizeof(char),22);}));});Cyc_target_cflags=_tmp727;});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0;_tmp424.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp422[1]={& _tmp424};({struct Cyc___cycFILE*_tmp729=Cyc_stderr;struct _dyneither_ptr _tmp728=({const char*_tmp423="Target cflags are %s\n";_tag_dyneither(_tmp423,sizeof(char),22);});Cyc_fprintf(_tmp729,_tmp728,_tag_dyneither(_tmp422,sizeof(void*),1));});});});
({struct _dyneither_ptr _tmp72B=({struct Cyc_List_List*_tmp72A=_tmp420;Cyc_Specsfile_get_spec(_tmp72A,({const char*_tmp425="cyclone_cc";_tag_dyneither(_tmp425,sizeof(char),11);}));});Cyc_cyclone_cc=_tmp72B;});
if(!((unsigned int)Cyc_cyclone_cc.curr))({struct _dyneither_ptr _tmp72C=({const char*_tmp426="gcc";_tag_dyneither(_tmp426,sizeof(char),4);});Cyc_cyclone_cc=_tmp72C;});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0;_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);({void*_tmp427[1]={& _tmp429};({struct Cyc___cycFILE*_tmp72E=Cyc_stderr;struct _dyneither_ptr _tmp72D=({const char*_tmp428="C compiler is %s\n";_tag_dyneither(_tmp428,sizeof(char),18);});Cyc_fprintf(_tmp72E,_tmp72D,_tag_dyneither(_tmp427,sizeof(void*),1));});});});
# 2165
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp42A=0;({struct Cyc___cycFILE*_tmp730=Cyc_stderr;struct _dyneither_ptr _tmp72F=({const char*_tmp42B="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp42B,sizeof(char),22);});Cyc_fprintf(_tmp730,_tmp72F,_tag_dyneither(_tmp42A,sizeof(void*),0));});});
({struct Cyc___cycFILE*_tmp731=Cyc_fopen("BUILDLIB.sh","w");Cyc_script_file=_tmp731;});
if(!((unsigned int)Cyc_script_file)){
({void*_tmp42C=0;({struct Cyc___cycFILE*_tmp733=Cyc_stderr;struct _dyneither_ptr _tmp732=({const char*_tmp42D="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp42D,sizeof(char),35);});Cyc_fprintf(_tmp733,_tmp732,_tag_dyneither(_tmp42C,sizeof(void*),0));});});
 exit(1);}
# 2173
({void*_tmp42E=0;({struct _dyneither_ptr _tmp734=({const char*_tmp42F="#!/bin/sh\n";_tag_dyneither(_tmp42F,sizeof(char),11);});Cyc_prscript(_tmp734,_tag_dyneither(_tmp42E,sizeof(void*),0));});});
({void*_tmp430=0;({struct _dyneither_ptr _tmp735=({const char*_tmp431="GCC=\"gcc\"\n";_tag_dyneither(_tmp431,sizeof(char),11);});Cyc_prscript(_tmp735,_tag_dyneither(_tmp430,sizeof(void*),0));});});}
# 2178
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp434;_tmp434.tag=0;_tmp434.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp432[1]={& _tmp434};({struct Cyc___cycFILE*_tmp737=Cyc_stderr;struct _dyneither_ptr _tmp736=({const char*_tmp433="Error: could not create directory %s\n";_tag_dyneither(_tmp433,sizeof(char),38);});Cyc_fprintf(_tmp737,_tmp736,_tag_dyneither(_tmp432,sizeof(void*),1));});});});
return 1;}
# 2182
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp437;_tmp437.tag=0;_tmp437.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp435[1]={& _tmp437};({struct Cyc___cycFILE*_tmp739=Cyc_stderr;struct _dyneither_ptr _tmp738=({const char*_tmp436="Output directory is %s\n";_tag_dyneither(_tmp436,sizeof(char),24);});Cyc_fprintf(_tmp739,_tmp738,_tag_dyneither(_tmp435,sizeof(void*),1));});});});
# 2185
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp43A;_tmp43A.tag=0;_tmp43A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp438[1]={& _tmp43A};({struct _dyneither_ptr _tmp73A=({const char*_tmp439="cd %s\n";_tag_dyneither(_tmp439,sizeof(char),7);});Cyc_prscript(_tmp73A,_tag_dyneither(_tmp438,sizeof(void*),1));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp43D;_tmp43D.tag=0;_tmp43D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);({void*_tmp43B[1]={& _tmp43D};({struct _dyneither_ptr _tmp73B=({const char*_tmp43C="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp43C,sizeof(char),41);});Cyc_prscript(_tmp73B,_tag_dyneither(_tmp43B,sizeof(void*),1));});});});}
# 2191
if(!Cyc_gathering()){
# 2194
({struct Cyc___cycFILE*_tmp73D=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp73C=Cyc_output_dir;Cyc_Filename_concat(_tmp73C,({const char*_tmp43E="BUILDLIB.LOG";_tag_dyneither(_tmp43E,sizeof(char),13);}));}),sizeof(char),1)),"w");Cyc_log_file=_tmp73D;});
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp441;_tmp441.tag=0;_tmp441.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp43F[1]={& _tmp441};({struct Cyc___cycFILE*_tmp73F=Cyc_stderr;struct _dyneither_ptr _tmp73E=({const char*_tmp440="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp440,sizeof(char),50);});Cyc_fprintf(_tmp73F,_tmp73E,_tag_dyneither(_tmp43F,sizeof(void*),1));});});});
return 1;}
# 2200
if(!Cyc_do_setjmp){
# 2202
({struct Cyc___cycFILE*_tmp741=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp740=Cyc_output_dir;Cyc_Filename_concat(_tmp740,({const char*_tmp442="cstubs.c";_tag_dyneither(_tmp442,sizeof(char),9);}));}),sizeof(char),1)),"w");Cyc_cstubs_file=_tmp741;});
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp445;_tmp445.tag=0;_tmp445.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp443[1]={& _tmp445};({struct Cyc___cycFILE*_tmp743=Cyc_stderr;struct _dyneither_ptr _tmp742=({const char*_tmp444="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp444,sizeof(char),50);});Cyc_fprintf(_tmp743,_tmp742,_tag_dyneither(_tmp443,sizeof(void*),1));});});});
return 1;}
# 2209
({struct Cyc___cycFILE*_tmp745=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp744=Cyc_output_dir;Cyc_Filename_concat(_tmp744,({const char*_tmp446="cycstubs.cyc";_tag_dyneither(_tmp446,sizeof(char),13);}));}),sizeof(char),1)),"w");Cyc_cycstubs_file=_tmp745;});
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp449;_tmp449.tag=0;_tmp449.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp447[1]={& _tmp449};({struct Cyc___cycFILE*_tmp747=Cyc_stderr;struct _dyneither_ptr _tmp746=({const char*_tmp448="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp448,sizeof(char),52);});Cyc_fprintf(_tmp747,_tmp746,_tag_dyneither(_tmp447,sizeof(void*),1));});});});
# 2214
return 1;}
# 2216
({void*_tmp44A=0;({struct Cyc___cycFILE*_tmp749=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _dyneither_ptr _tmp748=({const char*_tmp44B="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp44B,sizeof(char),32);});Cyc_fprintf(_tmp749,_tmp748,_tag_dyneither(_tmp44A,sizeof(void*),0));});});}}{
# 2223
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 2229
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp44C=0;({struct Cyc___cycFILE*_tmp74B=Cyc_stderr;struct _dyneither_ptr _tmp74A=({const char*_tmp44D="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp44D,sizeof(char),22);});Cyc_fprintf(_tmp74B,_tmp74A,_tag_dyneither(_tmp44C,sizeof(void*),0));});});
 exit(1);}}}
# 2237
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2240
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2248
return 0;};};};};}

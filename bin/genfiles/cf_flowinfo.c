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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 214
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 224
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 287
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 49
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 430 "absyn.h"
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 55
void*Cyc_Tcutil_compress(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 163
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 265 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 306
int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 113
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 115
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 52
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};
# 58
typedef void*Cyc_CfFlowInfo_pathcon_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_path_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 72
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_AllIL  = 1};
# 78
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 93
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 98
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 106
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple10{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple10 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 137 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 152
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 155
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 161
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple11{void*f1;struct Cyc_List_List*f2;};
struct _tuple11 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 169
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
# 199
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 203
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple12 pr1,struct _tuple12 pr2);
# 209
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 37 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 40
Cyc_CfFlowInfo_anal_error=1;
Cyc_Position_post_error(({unsigned int _tmp289=loc;Cyc_Position_mk_err(_tmp289,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 53 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
# 55
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp1;(_tmp1.PlaceL).val=x;(_tmp1.PlaceL).tag=1;_tmp1;});}
# 58
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp2;(_tmp2.UnknownL).val=0;(_tmp2.UnknownL).tag=2;_tmp2;});}
# 61
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp3;(_tmp3.BottomFL).val=0;(_tmp3.BottomFL).tag=1;_tmp3;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp4;({struct _tuple10 _tmp28A=({struct _tuple10 _tmp5;_tmp5.f1=fd;_tmp5.f2=r;_tmp5;});(_tmp4.ReachableFL).val=_tmp28A;});(_tmp4.ReachableFL).tag=2;_tmp4;});}
# 67
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->r=r;({void*_tmp28C=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp14=_region_malloc(r,sizeof(*_tmp14));({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp28B=({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp15;_tmp15.tag=0;_tmp15;});_tmp14[0]=_tmp28B;});_tmp14;});_tmp6->zero=_tmp28C;});({void*_tmp28E=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp12=_region_malloc(r,sizeof(*_tmp12));({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp28D=({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp13;_tmp13.tag=1;_tmp13;});_tmp12[0]=_tmp28D;});_tmp12;});_tmp6->notzeroall=_tmp28E;});({void*_tmp290=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10=_region_malloc(r,sizeof(*_tmp10));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp28F=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp11;_tmp11.tag=2;_tmp11.f1=Cyc_CfFlowInfo_NoneIL;_tmp11;});_tmp10[0]=_tmp28F;});_tmp10;});_tmp6->unknown_none=_tmp290;});({void*_tmp292=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE=_region_malloc(r,sizeof(*_tmpE));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp291=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmpF;_tmpF.tag=2;_tmpF.f1=Cyc_CfFlowInfo_AllIL;_tmpF;});_tmpE[0]=_tmp291;});_tmpE;});_tmp6->unknown_all=_tmp292;});({void*_tmp294=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp293=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpD;_tmpD.tag=3;_tmpD.f1=Cyc_CfFlowInfo_NoneIL;_tmpD;});_tmpC[0]=_tmp293;});_tmpC;});_tmp6->esc_none=_tmp294;});({void*_tmp296=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpA=_region_malloc(r,sizeof(*_tmpA));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp295=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpB;_tmpB.tag=3;_tmpB.f1=Cyc_CfFlowInfo_AllIL;_tmpB;});_tmpA[0]=_tmp295;});_tmpA;});_tmp6->esc_all=_tmp296;});({struct Cyc_Dict_Dict _tmp297=
# 80
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp);_tmp6->mt_flowdict=_tmp297;});({struct Cyc_CfFlowInfo_Place*_tmp29A=({struct Cyc_CfFlowInfo_Place*_tmp7=_region_malloc(r,sizeof(*_tmp7));({void*_tmp299=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8=_region_malloc(r,sizeof(*_tmp8));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp298=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp9;_tmp9.tag=1;_tmp9.f1=& dummy_exp;_tmp9.f2=(void*)& Cyc_Absyn_VoidType_val;_tmp9;});_tmp8[0]=_tmp298;});_tmp8;});_tmp7->root=_tmp299;});_tmp7->path=0;_tmp7;});_tmp6->dummy_place=_tmp29A;});_tmp6;});}
# 86
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp17=*place;struct Cyc_CfFlowInfo_Place _tmp18=_tmp17;void*_tmp21;struct Cyc_List_List*_tmp20;_LL1: _tmp21=_tmp18.root;_tmp20=_tmp18.path;_LL2:;{
void*_tmp19=_tmp21;struct Cyc_Absyn_Vardecl*_tmp1F;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp19)->tag == 0){_LL4: _tmp1F=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp19)->f1;_LL5:
# 90
 if(_tmp20 == 0)
return Cyc_Absynpp_qvar2string(_tmp1F->name);else{
# 93
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0;({struct _dyneither_ptr _tmp29B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1F->name));_tmp1C.f1=_tmp29B;});({void*_tmp1A[1]={& _tmp1C};({struct _dyneither_ptr _tmp29C=({const char*_tmp1B="reachable from %s";_tag_dyneither(_tmp1B,sizeof(char),18);});Cyc_aprintf(_tmp29C,_tag_dyneither(_tmp1A,sizeof(void*),1));});});});}}else{_LL6: _LL7:
({void*_tmp1D=0;({struct _dyneither_ptr _tmp29D=({const char*_tmp1E="error locations not for VarRoots";_tag_dyneither(_tmp1E,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp29D,_tag_dyneither(_tmp1D,sizeof(void*),0));});});}_LL3:;};}
# 99
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp22=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp22,f)== 0)return n;
n=n + 1;}
# 106
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Core_Impossible_exn_struct _tmp2A0=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp29F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp25[1]={& _tmp27};({struct _dyneither_ptr _tmp29E=({const char*_tmp26="get_field_index_fs failed: %s";_tag_dyneither(_tmp26,sizeof(char),30);});Cyc_aprintf(_tmp29E,_tag_dyneither(_tmp25,sizeof(void*),1));});});});_tmp24.f1=_tmp29F;});_tmp24;});_tmp23[0]=_tmp2A0;});_tmp23;}));}
# 109
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp28=Cyc_Tcutil_compress(t);void*_tmp29=_tmp28;struct Cyc_List_List*_tmp31;union Cyc_Absyn_AggrInfoU _tmp30;switch(*((int*)_tmp29)){case 11: _LL9: _tmp30=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29)->f1).aggr_info;_LLA: {
# 112
struct Cyc_List_List*_tmp2A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp30))->impl))->fields;
_tmp31=_tmp2A;goto _LLC;}case 12: _LLB: _tmp31=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29)->f2;_LLC:
# 115
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp31,f);default: _LLD: _LLE:
# 117
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));({struct Cyc_Core_Impossible_exn_struct _tmp2A4=({struct Cyc_Core_Impossible_exn_struct _tmp2C;_tmp2C.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2A3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0;({struct _dyneither_ptr _tmp2A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp2F.f1=_tmp2A1;});({void*_tmp2D[1]={& _tmp2F};({struct _dyneither_ptr _tmp2A2=({const char*_tmp2E="get_field_index failed: %s";_tag_dyneither(_tmp2E,sizeof(char),27);});Cyc_aprintf(_tmp2A2,_tag_dyneither(_tmp2D,sizeof(void*),1));});});});_tmp2C.f1=_tmp2A3;});_tmp2C;});_tmp2B[0]=_tmp2A4;});_tmp2B;}));}_LL8:;}struct _tuple13{void*f1;void*f2;};
# 122
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple13 _tmp32=({struct _tuple13 _tmp3A;_tmp3A.f1=r1;_tmp3A.f2=r2;_tmp3A;});struct _tuple13 _tmp33=_tmp32;int _tmp39;int _tmp38;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Vardecl*_tmp35;struct Cyc_Absyn_Vardecl*_tmp34;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp33.f1)->tag == 0){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp33.f2)->tag == 0){_LL10: _tmp35=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp33.f1)->f1;_tmp34=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp33.f2)->f1;_LL11:
 return(int)_tmp35 - (int)_tmp34;}else{_LL12: _LL13:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp33.f2)->tag == 0){_LL14: _LL15:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp33.f1)->tag == 1){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp33.f2)->tag == 1){_LL16: _tmp37=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp33.f1)->f1;_tmp36=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp33.f2)->f1;_LL17:
 return(int)_tmp37 - (int)_tmp36;}else{_LL18: _LL19:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp33.f2)->tag == 1){_LL1A: _LL1B:
 return 1;}else{_LL1C: _tmp39=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp33.f1)->f1;_tmp38=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp33.f2)->f1;_LL1D:
 return _tmp39 - _tmp38;}}}}_LLF:;};}
# 136
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple13 _tmp3B=({struct _tuple13 _tmp3F;_tmp3F.f1=p1;_tmp3F.f2=p2;_tmp3F;});struct _tuple13 _tmp3C=_tmp3B;int _tmp3E;int _tmp3D;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp3C.f1)->tag == 0){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp3C.f2)->tag == 0){_LL1F: _tmp3E=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp3C.f1)->f1;_tmp3D=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp3C.f2)->f1;_LL20:
# 139
 if(_tmp3E == _tmp3D)return 0;
if(_tmp3E < _tmp3D)return - 1;else{
return 1;}}else{_LL25: _LL26:
# 144
 return 1;}}else{if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmp3C.f2)->tag == 1){_LL21: _LL22:
# 142
 return 0;}else{_LL23: _LL24:
 return - 1;}}_LL1E:;}
# 148
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 152
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp40=*p1;struct Cyc_CfFlowInfo_Place _tmp41=_tmp40;void*_tmp47;struct Cyc_List_List*_tmp46;_LL28: _tmp47=_tmp41.root;_tmp46=_tmp41.path;_LL29:;{
struct Cyc_CfFlowInfo_Place _tmp42=*p2;struct Cyc_CfFlowInfo_Place _tmp43=_tmp42;void*_tmp45;struct Cyc_List_List*_tmp44;_LL2B: _tmp45=_tmp43.root;_tmp44=_tmp43.path;_LL2C:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp47,_tmp45);
if(i != 0)
return i;
return Cyc_CfFlowInfo_path_cmp(_tmp46,_tmp44);};};};}
# 173 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 178
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 184
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _dyneither_ptr d=({unsigned int _tmp4C=sz;void**_tmp4D=(void**)({struct _RegionHandle*_tmp2A5=fenv->r;_region_malloc(_tmp2A5,_check_times(sizeof(void*),_tmp4C));});struct _dyneither_ptr _tmp4F=_tag_dyneither(_tmp4D,sizeof(void*),_tmp4C);{unsigned int _tmp4E=_tmp4C;unsigned int i;for(i=0;i < _tmp4E;i ++){_tmp4D[i]=(void*)fenv->unknown_all;}}_tmp4F;});
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp48=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp49=_tmp48;struct _dyneither_ptr*_tmp4B;void*_tmp4A;_LL2E: _tmp4B=_tmp49->name;_tmp4A=_tmp49->type;_LL2F:;
if(_get_dyneither_size(*_tmp4B,sizeof(char))!= 1)
({void*_tmp2A6=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp4A,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp2A6;});}}
# 192
return d;}struct _tuple14{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 196
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
# 200
struct _tuple14*_tmp50=env;struct _RegionHandle*_tmp56;struct Cyc_List_List*_tmp55;_LL31: _tmp56=_tmp50->f1;_tmp55=_tmp50->f2;_LL32:;{
void*_tmp51=Cyc_Tcutil_rsubstitute(_tmp56,_tmp55,f->type);
struct Cyc_Absyn_Exp*_tmp52=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp53=_tmp52 == 0?0: Cyc_Tcutil_rsubsexp(_tmp56,_tmp55,_tmp52);
return({struct Cyc_Absyn_Aggrfield*_tmp54=_region_malloc(_tmp56,sizeof(*_tmp54));_tmp54->name=f->name;_tmp54->tq=f->tq;_tmp54->type=_tmp51;_tmp54->width=f->width;_tmp54->attributes=f->attributes;_tmp54->requires_clause=_tmp53;_tmp54;});};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 211
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 217
struct _RegionHandle _tmp57=_new_region("temp");struct _RegionHandle*temp=& _tmp57;_push_region(temp);
# 221
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp58=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp59=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp5A=Cyc_Tcutil_tvar_kind(_tmp58,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp5B=_tmp5A;switch(((struct Cyc_Absyn_Kind*)_tmp5B)->kind){case Cyc_Absyn_RgnKind: _LL34: _LL35:
 goto _LL37;case Cyc_Absyn_EffKind: _LL36: _LL37:
 continue;default: _LL38: _LL39:
 goto _LL33;}_LL33:;}
# 230
({struct Cyc_List_List*_tmp2A8=({struct Cyc_List_List*_tmp5C=_region_malloc(temp,sizeof(*_tmp5C));({struct _tuple15*_tmp2A7=({struct _tuple15*_tmp5D=_region_malloc(temp,sizeof(*_tmp5D));_tmp5D->f1=_tmp58;_tmp5D->f2=_tmp59;_tmp5D;});_tmp5C->hd=_tmp2A7;});_tmp5C->tl=inst;_tmp5C;});inst=_tmp2A8;});}
# 232
if(inst != 0){
struct _tuple14 _tmp5E=({struct _tuple14 _tmp60;_tmp60.f1=temp;({struct Cyc_List_List*_tmp2A9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp60.f2=_tmp2A9;});_tmp60;});
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp5E,fs);
struct _dyneither_ptr _tmp5F=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp5F;}else{
# 237
struct _dyneither_ptr _tmp61=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp61;}}
# 221
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 241
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 247
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 249
{void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp63=_tmp62;union Cyc_Absyn_Constraint*_tmp7D;void*_tmp7C;void*_tmp7B;union Cyc_Absyn_Constraint*_tmp7A;enum Cyc_Absyn_AggrKind _tmp79;struct Cyc_List_List*_tmp78;union Cyc_Absyn_AggrInfoU _tmp77;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;struct Cyc_Absyn_Datatypefield*_tmp74;switch(*((int*)_tmp63)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp63)->f1).field_info).KnownDatatypefield).tag == 2){_LL3B: _tmp74=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp63)->f1).field_info).KnownDatatypefield).val).f2;_LL3C:
# 251
 if(_tmp74->typs == 0)
return leafval;
_tmp75=_tmp74->typs;goto _LL3E;}else{goto _LL49;}case 10: _LL3D: _tmp75=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp63)->f1;_LL3E: {
# 255
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp75);
# 258
struct _dyneither_ptr d=({unsigned int _tmp67=sz;void**_tmp68=(void**)({struct _RegionHandle*_tmp2AA=fenv->r;_region_malloc(_tmp2AA,_check_times(sizeof(void*),_tmp67));});struct _dyneither_ptr _tmp6A=_tag_dyneither(_tmp68,sizeof(void*),_tmp67);{unsigned int _tmp69=_tmp67;unsigned int i;for(i=0;i < _tmp69;i ++){_tmp68[i]=(void*)fenv->unknown_all;}}_tmp6A;});
{int i=0;for(0;i < sz;++ i){
({void*_tmp2AB=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)).f2,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp2AB;});
_tmp75=_tmp75->tl;}}
# 263
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp64=_region_malloc(fenv->r,sizeof(*_tmp64));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2AD=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp65;_tmp65.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2AC=({struct Cyc_CfFlowInfo_UnionRInfo _tmp66;_tmp66.is_union=0;_tmp66.fieldnum=- 1;_tmp66;});_tmp65.f1=_tmp2AC;});_tmp65.f2=d;_tmp65;});_tmp64[0]=_tmp2AD;});_tmp64;});}case 11: _LL3F: _tmp77=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp63)->f1).aggr_info;_tmp76=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp63)->f1).targs;_LL40: {
# 265
struct Cyc_Absyn_Aggrdecl*_tmp6B=Cyc_Absyn_get_known_aggrdecl(_tmp77);
if(_tmp6B->impl == 0)
goto _LL3A;{
struct Cyc_List_List*_tmp6C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6B->impl))->fields;
if(_tmp76 == 0){_tmp79=_tmp6B->kind;_tmp78=_tmp6C;goto _LL42;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6D=_region_malloc(fenv->r,sizeof(*_tmp6D));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2B0=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp6E;_tmp6E.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2AE=({struct Cyc_CfFlowInfo_UnionRInfo _tmp6F;_tmp6F.is_union=_tmp6B->kind == Cyc_Absyn_UnionA;_tmp6F.fieldnum=- 1;_tmp6F;});_tmp6E.f1=_tmp2AE;});({struct _dyneither_ptr _tmp2AF=
Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmp6B->tvs,_tmp76,_tmp6C,_tmp6B->kind == Cyc_Absyn_UnionA,leafval);_tmp6E.f2=_tmp2AF;});_tmp6E;});_tmp6D[0]=_tmp2B0;});_tmp6D;});};}case 12: _LL41: _tmp79=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp63)->f1;_tmp78=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp63)->f2;_LL42:
# 276
 return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp70=_region_malloc(fenv->r,sizeof(*_tmp70));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2B3=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp71;_tmp71.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2B1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp72;_tmp72.is_union=_tmp79 == Cyc_Absyn_UnionA;_tmp72.fieldnum=- 1;_tmp72;});_tmp71.f1=_tmp2B1;});({struct _dyneither_ptr _tmp2B2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp78,_tmp79 == Cyc_Absyn_UnionA,leafval);_tmp71.f2=_tmp2B2;});_tmp71;});_tmp70[0]=_tmp2B3;});_tmp70;});case 8: _LL43: _tmp7B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63)->f1).elt_type;_tmp7A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7A)){_LL44:
# 280
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmp7B)?fenv->unknown_all: leafval;}else{goto _LL49;}case 19: _LL45: _tmp7C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp63)->f1;_LL46:
 return leafval;case 5: _LL47: _tmp7D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63)->f1).ptr_atts).nullable;if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7D)){_LL48:
{void*_tmp73=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp73)->tag == 2){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp73)->f1 == Cyc_CfFlowInfo_AllIL){_LL4C: _LL4D:
 return fenv->notzeroall;}else{goto _LL4E;}}else{_LL4E: _LL4F:
 goto _LL4B;}_LL4B:;}
# 287
goto _LL3A;}else{goto _LL49;}default: _LL49: _LL4A:
 goto _LL3A;}_LL3A:;}
# 291
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 294
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 300
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmp7E=r;void*_tmp86;int _tmp85;int _tmp84;struct _dyneither_ptr _tmp83;struct Cyc_Absyn_Exp*_tmp82;int _tmp81;void*_tmp80;switch(*((int*)_tmp7E)){case 7: _LL51: _tmp82=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f1;_tmp81=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f2;_tmp80=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f3;_LL52:
# 303
 if(_tmp82 == e  && _tmp81 == env_iteration){
*needs_unconsume=1;
return 0;}
# 307
return 1;case 6: _LL53: _tmp85=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f1).is_union;_tmp84=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f1).fieldnum;_tmp83=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f2;_LL54:
# 309
 if(!_tmp85  || _tmp84 == - 1){
unsigned int _tmp7F=_get_dyneither_size(_tmp83,sizeof(void*));
{int i=0;for(0;i < _tmp7F;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmp83.curr)[i],needs_unconsume))
return 1;}}
# 315
return 0;}else{
# 318
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmp83,sizeof(void*),_tmp84)),needs_unconsume);}case 8: _LL55: _tmp86=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp7E)->f2;_LL56:
# 320
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmp86,needs_unconsume);default: _LL57: _LL58:
 return 0;}_LL50:;}
# 327
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmp87=r;struct Cyc_Absyn_Vardecl*_tmp98;void*_tmp97;struct Cyc_CfFlowInfo_UnionRInfo _tmp96;struct _dyneither_ptr _tmp95;struct Cyc_Absyn_Exp*_tmp94;int _tmp93;void*_tmp92;switch(*((int*)_tmp87)){case 7: _LL5A: _tmp94=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp87)->f1;_tmp93=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp87)->f2;_tmp92=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp87)->f3;_LL5B:
# 330
 return _tmp92;case 6: _LL5C: _tmp96=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp87)->f1;_tmp95=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp87)->f2;_LL5D: {
# 332
unsigned int _tmp88=_get_dyneither_size(_tmp95,sizeof(void*));
int change=0;
struct _dyneither_ptr d2=({unsigned int _tmp8B=_tmp88;void**_tmp8C=(void**)({struct _RegionHandle*_tmp2B4=fenv->r;_region_malloc(_tmp2B4,_check_times(sizeof(void*),_tmp8B));});struct _dyneither_ptr _tmp8E=_tag_dyneither(_tmp8C,sizeof(void*),_tmp8B);{unsigned int _tmp8D=_tmp8B;unsigned int i;for(i=0;i < _tmp8D;i ++){_tmp8C[i]=(void*)((void**)_tmp95.curr)[(int)i];}}_tmp8E;});
{int i=0;for(0;i < _tmp88;++ i){
({void*_tmp2B5=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmp95.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2B5;});
if(((void**)d2.curr)[i]!= ((void**)_tmp95.curr)[i])
change=1;}}
# 340
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp89=_region_malloc(fenv->r,sizeof(*_tmp89));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2B6=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8A;_tmp8A.tag=6;_tmp8A.f1=_tmp96;_tmp8A.f2=d2;_tmp8A;});_tmp89[0]=_tmp2B6;});_tmp89;});else{
return r;}}case 8: _LL5E: _tmp98=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp87)->f1;_tmp97=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp87)->f2;_LL5F: {
# 344
void*_tmp8F=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmp97);
if(_tmp8F != _tmp97)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp90=_region_malloc(fenv->r,sizeof(*_tmp90));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2B7=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp91;_tmp91.tag=8;_tmp91.f1=_tmp98;_tmp91.f2=_tmp8F;_tmp91;});_tmp90[0]=_tmp2B7;});_tmp90;});else{
return r;}}default: _LL60: _LL61:
 return r;}_LL59:;}
# 354
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple13 _tmp99=({struct _tuple13 _tmpBE;({void*_tmp2B8=Cyc_Tcutil_compress(t);_tmpBE.f1=_tmp2B8;});_tmpBE.f2=r;_tmpBE;});struct _tuple13 _tmp9A=_tmp99;enum Cyc_Absyn_AggrKind _tmpBD;struct Cyc_List_List*_tmpBC;struct Cyc_CfFlowInfo_UnionRInfo _tmpBB;struct _dyneither_ptr _tmpBA;union Cyc_Absyn_AggrInfoU _tmpB9;struct Cyc_CfFlowInfo_UnionRInfo _tmpB8;struct _dyneither_ptr _tmpB7;struct Cyc_List_List*_tmpB6;struct Cyc_CfFlowInfo_UnionRInfo _tmpB5;struct _dyneither_ptr _tmpB4;struct Cyc_Absyn_Vardecl*_tmpB3;void*_tmpB2;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->tag == 8){_LL63: _tmpB3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f1;_tmpB2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f2;_LL64: {
# 357
void*_tmp9B=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpB2);
if(_tmp9B != _tmpB2)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9C=_region_malloc(fenv->r,sizeof(*_tmp9C));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2B9=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9D;_tmp9D.tag=8;_tmp9D.f1=_tmpB3;_tmp9D.f2=_tmp9B;_tmp9D;});_tmp9C[0]=_tmp2B9;});_tmp9C;});else{
return r;}}}else{switch(*((int*)_tmp9A.f1)){case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->tag == 6){_LL65: _tmpB6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9A.f1)->f1;_tmpB5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f1;_tmpB4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f2;_LL66: {
# 361
unsigned int _tmp9E=_get_dyneither_size(_tmpB4,sizeof(void*));
struct _dyneither_ptr d2=({unsigned int _tmpA1=_tmp9E;void**_tmpA2=(void**)({struct _RegionHandle*_tmp2BA=fenv->r;_region_malloc(_tmp2BA,_check_times(sizeof(void*),_tmpA1));});struct _dyneither_ptr _tmpA4=_tag_dyneither(_tmpA2,sizeof(void*),_tmpA1);{unsigned int _tmpA3=_tmpA1;unsigned int i;for(i=0;i < _tmpA3;i ++){_tmpA2[i]=(void*)fenv->unknown_all;}}_tmpA4;});
{int i=0;for(0;i < _tmp9E;++ i){
({void*_tmp2BB=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpB6))->hd)).f2,consumer,iteration,((void**)_tmpB4.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2BB;});
_tmpB6=_tmpB6->tl;}}
# 367
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp9F=_region_malloc(fenv->r,sizeof(*_tmp9F));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2BC=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpA0;_tmpA0.tag=6;_tmpA0.f1=_tmpB5;_tmpA0.f2=d2;_tmpA0;});_tmp9F[0]=_tmp2BC;});_tmp9F;});}}else{goto _LL6B;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->tag == 6){_LL67: _tmpB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9A.f1)->f1).aggr_info;_tmpB8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f1;_tmpB7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f2;_LL68: {
# 369
struct Cyc_Absyn_Aggrdecl*_tmpA5=Cyc_Absyn_get_known_aggrdecl(_tmpB9);
if(_tmpA5->impl == 0)return r;
_tmpBD=_tmpA5->kind;_tmpBC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA5->impl))->fields;_tmpBB=_tmpB8;_tmpBA=_tmpB7;goto _LL6A;}}else{goto _LL6B;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->tag == 6){_LL69: _tmpBD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9A.f1)->f1;_tmpBC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9A.f1)->f2;_tmpBB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f1;_tmpBA=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp9A.f2)->f2;_LL6A: {
# 373
struct _dyneither_ptr d2=({unsigned int _tmpAC=_get_dyneither_size(_tmpBA,sizeof(void*));void**_tmpAD=(void**)({struct _RegionHandle*_tmp2BD=fenv->r;_region_malloc(_tmp2BD,_check_times(sizeof(void*),_tmpAC));});struct _dyneither_ptr _tmpAF=_tag_dyneither(_tmpAD,sizeof(void*),_tmpAC);{unsigned int _tmpAE=_tmpAC;unsigned int i;for(i=0;i < _tmpAE;i ++){_tmpAD[i]=(void*)((void**)_tmpBA.curr)[(int)i];}}_tmpAF;});
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBC);
{int i=0;for(0;i < sz;(i ++,_tmpBC=_tmpBC->tl)){
struct Cyc_Absyn_Aggrfield*_tmpA6=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpBC))->hd;struct Cyc_Absyn_Aggrfield*_tmpA7=_tmpA6;struct _dyneither_ptr*_tmpA9;void*_tmpA8;_LL6E: _tmpA9=_tmpA7->name;_tmpA8=_tmpA7->type;_LL6F:;
if(_get_dyneither_size(*_tmpA9,sizeof(char))!= 1)
({void*_tmp2BE=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpA8,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpBA,sizeof(void*),i)));*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2BE;});}}
# 380
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpAA=_region_malloc(fenv->r,sizeof(*_tmpAA));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2BF=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpAB;_tmpAB.tag=6;_tmpAB.f1=_tmpBB;_tmpAB.f2=d2;_tmpAB;});_tmpAA[0]=_tmp2BF;});_tmpAA;});}}else{goto _LL6B;}default: _LL6B: _LL6C:
# 382
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpB0=_region_malloc(fenv->r,sizeof(*_tmpB0));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2C0=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpB1;_tmpB1.tag=7;_tmpB1.f1=consumer;_tmpB1.f2=iteration;_tmpB1.f3=r;_tmpB1;});_tmpB0[0]=_tmp2C0;});_tmpB0;});else{
return r;}}}_LL62:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 388
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 391
struct _RegionHandle _tmpBF=_new_region("r");struct _RegionHandle*r=& _tmpBF;_push_region(r);
{struct _tuple17 elem=({struct _tuple17 _tmpCB;_tmpCB.f1=place;_tmpCB.f2=0;_tmpCB;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmpC0=elem.f1;
struct Cyc_CfFlowInfo_Place _tmpC1=*place;struct Cyc_CfFlowInfo_Place _tmpC2=_tmpC1;void*_tmpC9;struct Cyc_List_List*_tmpC8;_LL71: _tmpC9=_tmpC2.root;_tmpC8=_tmpC2.path;_LL72:;{
struct Cyc_CfFlowInfo_Place _tmpC3=*_tmpC0;struct Cyc_CfFlowInfo_Place _tmpC4=_tmpC3;void*_tmpC7;struct Cyc_List_List*_tmpC6;_LL74: _tmpC7=_tmpC4.root;_tmpC6=_tmpC4.path;_LL75:;
if(Cyc_CfFlowInfo_root_cmp(_tmpC9,_tmpC7)!= 0)
continue;
for(0;_tmpC8 != 0  && _tmpC6 != 0;(_tmpC8=_tmpC8->tl,_tmpC6=_tmpC6->tl)){
if((void*)_tmpC8->hd != (void*)_tmpC6->hd)break;}
if(_tmpC8 == 0){
int _tmpC5=1;_npop_handler(0);return _tmpC5;}};}{
# 405
int _tmpCA=0;_npop_handler(0);return _tmpCA;};}
# 392
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 418
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 420
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 423
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp2C1=({struct Cyc_List_List*_tmpCC=_region_malloc(pile->rgn,sizeof(*_tmpCC));_tmpCC->hd=place;_tmpCC->tl=pile->places;_tmpCC;});pile->places=_tmp2C1;});}
# 426
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmpCD=r;struct Cyc_CfFlowInfo_UnionRInfo _tmpD2;struct _dyneither_ptr _tmpD1;struct Cyc_CfFlowInfo_Place*_tmpD0;void*_tmpCF;void*_tmpCE;switch(*((int*)_tmpCD)){case 7: _LL77: _tmpCE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCD)->f3;_LL78:
 Cyc_CfFlowInfo_add_places(pile,_tmpCE);return;case 8: _LL79: _tmpCF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCD)->f2;_LL7A:
 Cyc_CfFlowInfo_add_places(pile,_tmpCF);return;case 4: _LL7B: _tmpD0=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpCD)->f1;_LL7C:
 Cyc_CfFlowInfo_add_place(pile,_tmpD0);return;case 6: _LL7D: _tmpD2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD)->f1;_tmpD1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD)->f2;_LL7E:
# 432
{int i=0;for(0;i < _get_dyneither_size(_tmpD1,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmpD1.curr)[i]);}}
return;default: _LL7F: _LL80:
 return;}_LL76:;}
# 442
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 444
void*_tmpD3=old_val;struct Cyc_Absyn_Vardecl*_tmpE5;void*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;int _tmpE2;void*_tmpE1;int _tmpE0;struct _dyneither_ptr _tmpDF;switch(*((int*)_tmpD3)){case 6: _LL82: _tmpE0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f1).is_union;_tmpDF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f2;_LL83: {
# 446
struct _dyneither_ptr d2=({unsigned int _tmpD7=_get_dyneither_size(_tmpDF,sizeof(void*));void**_tmpD8=(void**)({struct _RegionHandle*_tmp2C2=r;_region_malloc(_tmp2C2,_check_times(sizeof(void*),_tmpD7));});struct _dyneither_ptr _tmpDA=_tag_dyneither(_tmpD8,sizeof(void*),_tmpD7);{unsigned int _tmpD9=_tmpD7;unsigned int i;for(i=0;i < _tmpD9;i ++){({typeof((void*)
Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmpDF.curr)[(int)i]))_tmp2C3=(void*)Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmpDF.curr)[(int)i]);_tmpD8[i]=_tmp2C3;});}}_tmpDA;});
# 450
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD4=_region_malloc(r,sizeof(*_tmpD4));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2C5=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpD5;_tmpD5.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2C4=({struct Cyc_CfFlowInfo_UnionRInfo _tmpD6;_tmpD6.is_union=_tmpE0;_tmpD6.fieldnum=- 1;_tmpD6;});_tmpD5.f1=_tmp2C4;});_tmpD5.f2=d2;_tmpD5;});_tmpD4[0]=_tmp2C5;});_tmpD4;});}case 7: _LL84: _tmpE3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f1;_tmpE2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f2;_tmpE1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f3;_LL85:
# 452
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpDB=_region_malloc(r,sizeof(*_tmpDB));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2C7=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpDC;_tmpDC.tag=7;_tmpDC.f1=_tmpE3;_tmpDC.f2=_tmpE2;({void*_tmp2C6=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmpE1);_tmpDC.f3=_tmp2C6;});_tmpDC;});_tmpDB[0]=_tmp2C7;});_tmpDB;});case 8: _LL86: _tmpE5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f1;_tmpE4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD3)->f2;_LL87:
# 454
 return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDD=_region_malloc(r,sizeof(*_tmpDD));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2C9=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpDE;_tmpDE.tag=8;_tmpDE.f1=_tmpE5;({void*_tmp2C8=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmpE4);_tmpDE.f2=_tmp2C8;});_tmpDE;});_tmpDD[0]=_tmp2C9;});_tmpDD;});default: _LL88: _LL89:
 return new_val;}_LL81:;}
# 461
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 463
void*_tmpE6=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmpE6 == rval)return d;{
struct _dyneither_ptr res=({unsigned int _tmpE7=_get_dyneither_size(d,sizeof(void*));void**_tmpE8=(void**)({struct _RegionHandle*_tmp2CA=r;_region_malloc(_tmp2CA,_check_times(sizeof(void*),_tmpE7));});struct _dyneither_ptr _tmpEA=_tag_dyneither(_tmpE8,sizeof(void*),_tmpE7);{unsigned int _tmpE9=_tmpE7;unsigned int i;for(i=0;i < _tmpE9;i ++){_tmpE8[i]=(void*)((void**)d.curr)[(int)i];}}_tmpEA;});
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 475
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*path,void*old_val,void*new_val){
# 479
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple18 _tmpEB=({struct _tuple18 _tmp103;_tmp103.f1=path;_tmp103.f2=old_val;_tmp103;});struct _tuple18 _tmpEC=_tmpEB;struct Cyc_Absyn_Vardecl*_tmp102;void*_tmp101;struct Cyc_List_List*_tmp100;void*_tmpFF;int _tmpFE;struct Cyc_List_List*_tmpFD;int _tmpFC;struct _dyneither_ptr _tmpFB;if(_tmpEC.f1 != 0){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmpEC.f1)->hd)->tag == 0)switch(*((int*)_tmpEC.f2)){case 6: _LL8B: _tmpFE=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpEC.f1)->hd)->f1;_tmpFD=(_tmpEC.f1)->tl;_tmpFC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpEC.f2)->f1).is_union;_tmpFB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpEC.f2)->f2;_LL8C: {
# 483
void*_tmpED=Cyc_CfFlowInfo_insert_place_outer(r,_tmpFD,*((void**)_check_dyneither_subscript(_tmpFB,sizeof(void*),_tmpFE)),new_val);
struct _dyneither_ptr _tmpEE=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmpFB,_tmpFE,_tmpED);
if((void**)_tmpEE.curr == (void**)_tmpFB.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEF=_region_malloc(r,sizeof(*_tmpEF));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2CC=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpF0;_tmpF0.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2CB=({struct Cyc_CfFlowInfo_UnionRInfo _tmpF1;_tmpF1.is_union=_tmpFC;_tmpF1.fieldnum=- 1;_tmpF1;});_tmpF0.f1=_tmp2CB;});_tmpF0.f2=_tmpEE;_tmpF0;});_tmpEF[0]=_tmp2CC;});_tmpEF;});}}case 8: goto _LL8F;default: goto _LL91;}else{switch(*((int*)_tmpEC.f2)){case 5: _LL8D: _tmp100=(_tmpEC.f1)->tl;_tmpFF=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpEC.f2)->f1;_LL8E: {
# 488
void*_tmpF2=Cyc_CfFlowInfo_insert_place_outer(r,_tmp100,_tmpFF,new_val);
if(_tmpF2 == _tmpFF)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpF3=_region_malloc(r,sizeof(*_tmpF3));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp2CD=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmpF4;_tmpF4.tag=5;_tmpF4.f1=_tmpF2;_tmpF4;});_tmpF3[0]=_tmp2CD;});_tmpF3;});}}case 8: goto _LL8F;default: goto _LL91;}}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpEC.f2)->tag == 8){_LL8F: _tmp102=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpEC.f2)->f1;_tmp101=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpEC.f2)->f2;_LL90: {
# 492
void*_tmpF5=Cyc_CfFlowInfo_insert_place_outer(r,path,_tmp101,new_val);
if(_tmpF5 == _tmp101)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF6=_region_malloc(r,sizeof(*_tmpF6));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2CE=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpF7;_tmpF7.tag=8;_tmpF7.f1=_tmp102;_tmpF7.f2=_tmpF5;_tmpF7;});_tmpF6[0]=_tmp2CE;});_tmpF6;});}}}else{_LL91: _LL92:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));({struct Cyc_Core_Impossible_exn_struct _tmp2D0=({struct Cyc_Core_Impossible_exn_struct _tmpF9;_tmpF9.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2CF=({const char*_tmpFA="bad insert place";_tag_dyneither(_tmpFA,sizeof(char),17);});_tmpF9.f1=_tmp2CF;});_tmpF9;});_tmpF8[0]=_tmp2D0;});_tmpF8;}));}}_LL8A:;};}
# 502
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
# 505
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp104=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmp105;_push_handler(& _tmp105);{int _tmp107=0;if(setjmp(_tmp105.handler))_tmp107=1;if(!_tmp107){({void*_tmp2D1=Cyc_CfFlowInfo_lookup_place(d,_tmp104);oldval=_tmp2D1;});;_pop_handler();}else{void*_tmp106=(void*)_exn_thrown;void*_tmp108=_tmp106;void*_tmp109;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp108)->tag == Cyc_Dict_Absent){_LL94: _LL95:
 continue;}else{_LL96: _tmp109=_tmp108;_LL97:(int)_rethrow(_tmp109);}_LL93:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmp10A=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _tmp10B=_tmp10A;if(_tmp10B == Cyc_CfFlowInfo_AllIL){_LL99: _LL9A:
 newval=fenv->esc_all;goto _LL98;}else{_LL9B: _LL9C:
 newval=fenv->esc_none;goto _LL98;}_LL98:;}
# 515
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp10C=*_tmp104;struct Cyc_CfFlowInfo_Place _tmp10D=_tmp10C;void*_tmp10F;struct Cyc_List_List*_tmp10E;_LL9E: _tmp10F=_tmp10D.root;_tmp10E=_tmp10D.path;_LL9F:;
({struct Cyc_Dict_Dict _tmp2D7=({struct Cyc_Dict_Dict _tmp2D6=d;void*_tmp2D5=_tmp10F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2D6,_tmp2D5,({
struct _RegionHandle*_tmp2D4=fenv->r;struct Cyc_List_List*_tmp2D3=_tmp10E;void*_tmp2D2=
# 520
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp10F);
# 518
Cyc_CfFlowInfo_insert_place_outer(_tmp2D4,_tmp2D3,_tmp2D2,newval);}));});
# 517
d=_tmp2D7;});};};}
# 523
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 535
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp110=r;void*_tmp116;enum Cyc_CfFlowInfo_InitLevel _tmp115;enum Cyc_CfFlowInfo_InitLevel _tmp114;switch(*((int*)_tmp110)){case 2: _LLA1: _tmp114=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp110)->f1;_LLA2:
 return _tmp114;case 3: _LLA3: _tmp115=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp110)->f1;_LLA4:
 return _tmp115;case 0: _LLA5: _LLA6:
 goto _LLA8;case 1: _LLA7: _LLA8:
 return Cyc_CfFlowInfo_AllIL;case 7: _LLA9: _tmp116=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp110)->f3;_LLAA:
 return Cyc_CfFlowInfo_NoneIL;default: _LLAB: _LLAC:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp111=_cycalloc(sizeof(*_tmp111));({struct Cyc_Core_Impossible_exn_struct _tmp2D9=({struct Cyc_Core_Impossible_exn_struct _tmp112;_tmp112.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2D8=({const char*_tmp113="initlevel_approx";_tag_dyneither(_tmp113,sizeof(char),17);});_tmp112.f1=_tmp2D8;});_tmp112;});_tmp111[0]=_tmp2D9;});_tmp111;}));}_LLA0:;}
# 545
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 547
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp117=r;void*_tmp121;struct Cyc_CfFlowInfo_Place*_tmp120;struct _dyneither_ptr _tmp11F;int _tmp11E;int _tmp11D;struct _dyneither_ptr _tmp11C;void*_tmp11B;switch(*((int*)_tmp117)){case 8: _LLAE: _tmp11B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp117)->f2;_LLAF:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp11B,acc);case 6: _LLB0: _tmp11E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp117)->f1).is_union;_tmp11D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp117)->f1).fieldnum;_tmp11C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp117)->f2;if(_tmp11E){_LLB1: {
# 555
unsigned int _tmp118=_get_dyneither_size(_tmp11C,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp11D == - 1){
int i=0;for(0;i < _tmp118;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp11C.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 565
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp11C,sizeof(void*),_tmp11D)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LLAD;}}else{_LLB2: _tmp11F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp117)->f2;_LLB3: {
# 569
unsigned int _tmp119=_get_dyneither_size(_tmp11F,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp119;++ i){
({enum Cyc_CfFlowInfo_InitLevel _tmp2DA=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp11F.curr)[i],this_ans);this_ans=_tmp2DA;});}}
goto _LLAD;}}case 4: _LLB4: _tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp117)->f1;_LLB5:
# 575
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp120))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 578
({struct Cyc_List_List*_tmp2DB=({struct Cyc_List_List*_tmp11A=_region_malloc(env->e,sizeof(*_tmp11A));_tmp11A->hd=_tmp120;_tmp11A->tl=env->seen;_tmp11A;});env->seen=_tmp2DB;});
({enum Cyc_CfFlowInfo_InitLevel _tmp2DD=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp2DC=env;Cyc_CfFlowInfo_initlevel_rec(_tmp2DC,Cyc_CfFlowInfo_lookup_place(env->d,_tmp120),Cyc_CfFlowInfo_AllIL);});this_ans=_tmp2DD;});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;}
# 584
goto _LLAD;case 5: _LLB6: _tmp121=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp117)->f1;_LLB7:
({enum Cyc_CfFlowInfo_InitLevel _tmp2DE=Cyc_CfFlowInfo_initlevel_rec(env,_tmp121,Cyc_CfFlowInfo_AllIL);this_ans=_tmp2DE;});goto _LLAD;default: _LLB8: _LLB9:
({enum Cyc_CfFlowInfo_InitLevel _tmp2DF=Cyc_CfFlowInfo_initlevel_approx(r);this_ans=_tmp2DF;});goto _LLAD;}_LLAD:;}
# 588
return this_ans;}
# 590
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp122=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp123=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp124;_tmp124.e=_tmp122;_tmp124.d=d;_tmp124.seen=0;_tmp124;});
return Cyc_CfFlowInfo_initlevel_rec(& _tmp123,r,Cyc_CfFlowInfo_AllIL);}
# 597
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp125=*place;struct Cyc_CfFlowInfo_Place _tmp126=_tmp125;void*_tmp135;struct Cyc_List_List*_tmp134;_LLBB: _tmp135=_tmp126.root;_tmp134=_tmp126.path;_LLBC:;{
void*_tmp127=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp135);
for(0;_tmp134 != 0;_tmp134=_tmp134->tl){
retry: {struct _tuple13 _tmp128=({struct _tuple13 _tmp133;_tmp133.f1=_tmp127;_tmp133.f2=(void*)_tmp134->hd;_tmp133;});struct _tuple13 _tmp129=_tmp128;void*_tmp132;struct Cyc_CfFlowInfo_UnionRInfo _tmp131;struct _dyneither_ptr _tmp130;int _tmp12F;void*_tmp12E;void*_tmp12D;switch(*((int*)_tmp129.f1)){case 8: _LLBE: _tmp12D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp129.f1)->f2;_LLBF:
# 603
 _tmp127=_tmp12D;goto retry;case 7: _LLC0: _tmp12E=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp129.f1)->f3;_LLC1:
# 606
 _tmp127=_tmp12E;goto retry;case 6: if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp129.f2)->tag == 0){_LLC2: _tmp131=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp129.f1)->f1;_tmp130=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp129.f1)->f2;_tmp12F=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp129.f2)->f1;_LLC3:
# 608
 _tmp127=*((void**)_check_dyneither_subscript(_tmp130,sizeof(void*),_tmp12F));
goto _LLBD;}else{goto _LLC6;}case 5: if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmp129.f2)->tag == 1){_LLC4: _tmp132=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp129.f1)->f1;_LLC5:
# 611
 _tmp127=_tmp132;
goto _LLBD;}else{goto _LLC6;}default: _LLC6: _LLC7:
# 619
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct Cyc_Core_Impossible_exn_struct _tmp2E1=({struct Cyc_Core_Impossible_exn_struct _tmp12B;_tmp12B.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2E0=({const char*_tmp12C="bad lookup_place";_tag_dyneither(_tmp12C,sizeof(char),17);});_tmp12B.f1=_tmp2E0;});_tmp12B;});_tmp12A[0]=_tmp2E1;});_tmp12A;}));}_LLBD:;}}
# 621
return _tmp127;};}
# 624
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp136=rval;int _tmp13B;int _tmp13A;struct _dyneither_ptr _tmp139;void*_tmp138;void*_tmp137;switch(*((int*)_tmp136)){case 3: _LLC9: _LLCA:
 return 0;case 7: _LLCB: _tmp137=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp136)->f3;_LLCC:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp137);case 8: _LLCD: _tmp138=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp136)->f2;_LLCE:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp138);case 6: _LLCF: _tmp13B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp136)->f1).is_union;_tmp13A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp136)->f1).fieldnum;_tmp139=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp136)->f2;_LLD0:
# 630
 if(_tmp13B  && _tmp13A != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp139,sizeof(void*),_tmp13A)));else{
# 633
unsigned int sz=_get_dyneither_size(_tmp139,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp139.curr)[i]))return 0;}}
return 1;}default: _LLD1: _LLD2:
# 638
 return 1;}_LLC8:;}
# 641
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 646
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp13C=fenv->r;
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp13D=_region_malloc(_tmp13C,sizeof(*_tmp13D));_tmp13D->rgn=_tmp13C;_tmp13D->places=0;_tmp13D;});
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 663
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 668
struct _tuple13 _tmp13E=({struct _tuple13 _tmp155;_tmp155.f1=oldval;_tmp155.f2=newval;_tmp155;});struct _tuple13 _tmp13F=_tmp13E;enum Cyc_CfFlowInfo_InitLevel _tmp154;struct Cyc_CfFlowInfo_UnionRInfo _tmp153;struct _dyneither_ptr _tmp152;struct Cyc_CfFlowInfo_UnionRInfo _tmp151;struct _dyneither_ptr _tmp150;struct Cyc_CfFlowInfo_Place*_tmp14F;struct Cyc_Absyn_Vardecl*_tmp14E;void*_tmp14D;void*_tmp14C;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13F.f1)->tag == 8){_LLD4: _tmp14C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13F.f1)->f2;_LLD5:
# 670
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp14C,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->tag == 8){_LLD6: _tmp14E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->f1;_tmp14D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->f2;_LLD7: {
# 672
void*_tmp140=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp14D);
if(_tmp140 == _tmp14D)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp141=_region_malloc((env->fenv)->r,sizeof(*_tmp141));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2E2=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp142;_tmp142.tag=8;_tmp142.f1=_tmp14E;_tmp142.f2=_tmp140;_tmp142;});_tmp141[0]=_tmp2E2;});_tmp141;});}}}else{switch(*((int*)_tmp13F.f1)){case 3: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->tag == 4){_LLD8: _tmp14F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->f1;_LLD9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp14F);goto _LLDB;}else{_LLDA: _LLDB:
# 677
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp143=0;({unsigned int _tmp2E4=env->loc;struct _dyneither_ptr _tmp2E3=({const char*_tmp144="assignment puts possibly-uninitialized data in an escaped location";_tag_dyneither(_tmp144,sizeof(char),67);});Cyc_CfFlowInfo_aerr(_tmp2E4,_tmp2E3,_tag_dyneither(_tmp143,sizeof(void*),0));});});
# 680
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmp13F.f2)){case 6: _LLDC: _tmp153=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13F.f1)->f1;_tmp152=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13F.f1)->f2;_tmp151=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->f1;_tmp150=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->f2;_LLDD: {
# 682
struct _dyneither_ptr new_d=({unsigned int _tmp147=
_get_dyneither_size(_tmp152,sizeof(void*));void**_tmp148=(void**)({struct _RegionHandle*_tmp2E5=(env->fenv)->r;_region_malloc(_tmp2E5,_check_times(sizeof(void*),_tmp147));});struct _dyneither_ptr _tmp14A=_tag_dyneither(_tmp148,sizeof(void*),_tmp147);{unsigned int _tmp149=_tmp147;unsigned int i;for(i=0;i < _tmp149;i ++){({typeof((void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp152.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp150,sizeof(void*),(int)i))))_tmp2E6=(void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp152.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp150,sizeof(void*),(int)i)));_tmp148[i]=_tmp2E6;});}}_tmp14A;});
# 686
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp152,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp152.curr)[i]){
change=1;break;}}}
# 691
if(!change){
if(!_tmp153.is_union)return oldval;
new_d=_tmp152;}else{
# 696
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp152,sizeof(void*));++ i){
if(({void*_tmp2E7=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp2E7 != *((void**)_check_dyneither_subscript(_tmp150,sizeof(void*),i));})){
change=1;break;}}}
# 701
if(!change){
if(!_tmp153.is_union)return newval;
new_d=_tmp150;}}
# 706
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp145=_region_malloc((env->fenv)->r,sizeof(*_tmp145));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E8=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp146;_tmp146.tag=6;_tmp146.f1=_tmp151;_tmp146.f2=new_d;_tmp146;});_tmp145[0]=_tmp2E8;});_tmp145;});}case 3: goto _LLDE;default: goto _LLE0;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->tag == 3){_LLDE: _tmp154=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp13F.f2)->f1;_LLDF: {
# 708
enum Cyc_CfFlowInfo_InitLevel _tmp14B=_tmp154;if(_tmp14B == Cyc_CfFlowInfo_NoneIL){_LLE3: _LLE4:
 return(env->fenv)->unknown_none;}else{_LLE5: _LLE6:
 return(env->fenv)->unknown_all;}_LLE2:;}}else{_LLE0: _LLE1:
# 712
 return newval;}}}}_LLD3:;}
# 718
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 722
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 733
if(path == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple18 _tmp156=({struct _tuple18 _tmp17E;_tmp17E.f1=path;_tmp17E.f2=oldval;_tmp17E;});struct _tuple18 _tmp157=_tmp156;int _tmp17D;struct Cyc_List_List*_tmp17C;int _tmp17B;int _tmp17A;struct _dyneither_ptr _tmp179;struct Cyc_List_List*_tmp178;void*_tmp177;struct Cyc_Absyn_Exp*_tmp176;int _tmp175;void*_tmp174;struct Cyc_Absyn_Vardecl*_tmp173;void*_tmp172;switch(*((int*)_tmp157.f2)){case 8: _LLE8: _tmp173=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f1;_tmp172=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f2;_LLE9: {
# 736
void*_tmp158=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,_tmp172,newval);
if(_tmp158 == _tmp172)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp159=_region_malloc((env->fenv)->r,sizeof(*_tmp159));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2E9=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp15A;_tmp15A.tag=8;_tmp15A.f1=_tmp173;_tmp15A.f2=_tmp158;_tmp15A;});_tmp159[0]=_tmp2E9;});_tmp159;});}}case 7: _LLEA: _tmp176=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f1;_tmp175=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f2;_tmp174=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f3;_LLEB: {
# 740
void*_tmp15B=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,_tmp174,newval);
if(_tmp15B == _tmp174)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp15C=_region_malloc((env->fenv)->r,sizeof(*_tmp15C));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2EA=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp15D;_tmp15D.tag=7;_tmp15D.f1=_tmp176;_tmp15D.f2=_tmp175;_tmp15D.f3=_tmp15B;_tmp15D;});_tmp15C[0]=_tmp2EA;});_tmp15C;});}}default: if(_tmp157.f1 != 0){if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmp157.f1)->hd)->tag == 1){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->tag == 5){_LLEC: _tmp178=(_tmp157.f1)->tl;_tmp177=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f1;_LLED: {
# 744
void*_tmp15E=Cyc_CfFlowInfo_assign_place_outer(env,_tmp178,path_prefix + 1,_tmp177,newval);
if(_tmp15E == _tmp177)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp15F=_region_malloc((env->fenv)->r,sizeof(*_tmp15F));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp2EB=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp160;_tmp160.tag=5;_tmp160.f1=_tmp15E;_tmp160;});_tmp15F[0]=_tmp2EB;});_tmp15F;});}}}else{goto _LLF0;}}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->tag == 6){_LLEE: _tmp17D=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmp157.f1)->hd)->f1;_tmp17C=(_tmp157.f1)->tl;_tmp17B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f1).is_union;_tmp17A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f1).fieldnum;_tmp179=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp157.f2)->f2;_LLEF: {
# 748
void*_tmp161=Cyc_CfFlowInfo_assign_place_outer(env,_tmp17C,path_prefix + 1,*((void**)_check_dyneither_subscript(_tmp179,sizeof(void*),_tmp17D)),newval);
# 750
struct _dyneither_ptr _tmp162=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp179,_tmp17D,_tmp161);
if((void**)_tmp162.curr == (void**)_tmp179.curr  && (!_tmp17B  || _tmp17A == _tmp17D))return oldval;
_tmp179=_tmp162;
# 755
if(_tmp17B){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp179,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp17D){
struct _dyneither_ptr _tmp163=({
struct _RegionHandle*_tmp2EE=(env->fenv)->r;struct _dyneither_ptr _tmp2ED=_tmp179;int _tmp2EC=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp2EE,_tmp2ED,_tmp2EC,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp179,sizeof(void*),i))));});
# 765
if((void**)_tmp163.curr != (void**)_tmp179.curr){
_tmp179=_tmp163;
changed=1;}}}}
# 773
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp164=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp165=_tmp164;void*_tmp16B;struct Cyc_List_List*_tmp16A;_LLF3: _tmp16B=_tmp165->root;_tmp16A=_tmp165->path;_LLF4:;{
struct Cyc_List_List*_tmp166=({struct _RegionHandle*_tmp2F0=(env->pile)->rgn;int*_tmp2EF=({int*_tmp169=_cycalloc_atomic(sizeof(*_tmp169));_tmp169[0]=path_prefix;_tmp169;});((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,void*),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)(_tmp2F0,(int(*)(int*n,void*unused))Cyc_CfFlowInfo_nprefix,_tmp2EF,_tmp16A);});
struct Cyc_CfFlowInfo_Place*_tmp167=({struct Cyc_CfFlowInfo_Place*_tmp168=_region_malloc((env->pile)->rgn,sizeof(*_tmp168));_tmp168->root=_tmp16B;_tmp168->path=_tmp166;_tmp168;});;};}}
# 779
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp16C=_region_malloc((env->fenv)->r,sizeof(*_tmp16C));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2F2=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp16D;_tmp16D.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2F1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp16E;_tmp16E.is_union=_tmp17B;_tmp16E.fieldnum=_tmp17D;_tmp16E;});_tmp16D.f1=_tmp2F1;});_tmp16D.f2=_tmp179;_tmp16D;});_tmp16C[0]=_tmp2F2;});_tmp16C;});}}else{goto _LLF0;}}}else{_LLF0: _LLF1:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));({struct Cyc_Core_Impossible_exn_struct _tmp2F4=({struct Cyc_Core_Impossible_exn_struct _tmp170;_tmp170.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2F3=({const char*_tmp171="bad assign place";_tag_dyneither(_tmp171,sizeof(char),17);});_tmp170.f1=_tmp2F3;});_tmp170;});_tmp16F[0]=_tmp2F4;});_tmp16F;}));}}_LLE7:;};}
# 783
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 793
struct _RegionHandle*_tmp17F=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp180=place;void*_tmp184;struct Cyc_List_List*_tmp183;_LLF6: _tmp184=_tmp180->root;_tmp183=_tmp180->path;_LLF7:;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp181;_tmp181.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp2F5=({struct Cyc_CfFlowInfo_EscPile*_tmp182=_region_malloc(_tmp17F,sizeof(*_tmp182));_tmp182->rgn=_tmp17F;_tmp182->places=0;_tmp182;});_tmp181.pile=_tmp2F5;});_tmp181.d=d;_tmp181.loc=loc;_tmp181.root_place=place;_tmp181;});
void*newval=({struct Cyc_List_List*_tmp2F7=_tmp183;void*_tmp2F6=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp184);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp2F7,0,_tmp2F6,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp2F9=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp2F8=env.pile;Cyc_CfFlowInfo_escape_these(_tmp2F9,_tmp2F8,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp184,newval));});};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 808
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
# 810
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 820 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp185=(env->fenv)->r;
# 824
{struct _tuple13 _tmp186=({struct _tuple13 _tmp1C9;_tmp1C9.f1=r1;_tmp1C9.f2=r2;_tmp1C9;});struct _tuple13 _tmp187=_tmp186;int _tmp1C8;int _tmp1C7;struct _dyneither_ptr _tmp1C6;int _tmp1C5;int _tmp1C4;struct _dyneither_ptr _tmp1C3;struct Cyc_CfFlowInfo_Place*_tmp1C2;void*_tmp1C1;void*_tmp1C0;void*_tmp1BF;void*_tmp1BE;struct Cyc_CfFlowInfo_Place*_tmp1BD;struct Cyc_CfFlowInfo_Place*_tmp1BC;struct Cyc_CfFlowInfo_Place*_tmp1BB;struct Cyc_CfFlowInfo_Place*_tmp1BA;struct Cyc_CfFlowInfo_Place*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;int _tmp1B7;void*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B5;int _tmp1B4;void*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;int _tmp1B1;void*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1AF;int _tmp1AE;void*_tmp1AD;struct Cyc_Absyn_Vardecl*_tmp1AC;void*_tmp1AB;struct Cyc_Absyn_Vardecl*_tmp1AA;void*_tmp1A9;struct Cyc_Absyn_Vardecl*_tmp1A8;void*_tmp1A7;struct Cyc_Absyn_Vardecl*_tmp1A6;void*_tmp1A5;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->tag == 8){_LLF9: _tmp1A8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_tmp1A7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f2;_tmp1A6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_tmp1A5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f2;_LLFA:
# 827
 if(_tmp1A8 == _tmp1A6)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp188=_region_malloc(_tmp185,sizeof(*_tmp188));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2FB=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp189;_tmp189.tag=8;_tmp189.f1=_tmp1A8;({void*_tmp2FA=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1A7,_tmp1A5);_tmp189.f2=_tmp2FA;});_tmp189;});_tmp188[0]=_tmp2FB;});_tmp188;});else{
# 830
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1A7,_tmp1A5);}}else{_LLFB: _tmp1AA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_tmp1A9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f2;_LLFC:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1A9,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->tag == 8){_LLFD: _tmp1AC=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_tmp1AB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f2;_LLFE:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1AB);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->tag == 7){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->tag == 7){_LLFF: _tmp1B2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_tmp1B1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f2;_tmp1B0=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f3;_tmp1AF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_tmp1AE=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f2;_tmp1AD=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f3;_LL100:
# 835
 if(_tmp1B2 == _tmp1AF)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp18A=_region_malloc(_tmp185,sizeof(*_tmp18A));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2FD=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp18B;_tmp18B.tag=7;_tmp18B.f1=_tmp1B2;_tmp18B.f2=_tmp1B1 > _tmp1AE?_tmp1B1: _tmp1AE;({void*_tmp2FC=
Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B0,_tmp1AD);_tmp18B.f3=_tmp2FC;});_tmp18B;});_tmp18A[0]=_tmp2FD;});_tmp18A;});
{void*_tmp18C=_tmp1B0;struct Cyc_CfFlowInfo_Place*_tmp18D;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp18C)->tag == 4){_LL11A: _tmp18D=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp18C)->f1;_LL11B:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp18D);goto _LL119;}else{_LL11C: _LL11D:
 goto _LL119;}_LL119:;}
# 842
{void*_tmp18E=_tmp1AD;struct Cyc_CfFlowInfo_Place*_tmp18F;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp18E)->tag == 4){_LL11F: _tmp18F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp18E)->f1;_LL120:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp18F);goto _LL11E;}else{_LL121: _LL122:
 goto _LL11E;}_LL11E:;}
# 846
goto _LLF8;}else{_LL101: _tmp1B5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_tmp1B4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f2;_tmp1B3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f3;_LL102:
# 848
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp190=_region_malloc(_tmp185,sizeof(*_tmp190));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2FF=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp191;_tmp191.tag=7;_tmp191.f1=_tmp1B5;_tmp191.f2=_tmp1B4;({void*_tmp2FE=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B3,r2);_tmp191.f3=_tmp2FE;});_tmp191;});_tmp190[0]=_tmp2FF;});_tmp190;});}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->tag == 7){_LL103: _tmp1B8=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_tmp1B7=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f2;_tmp1B6=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f3;_LL104:
# 850
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp192=_region_malloc(_tmp185,sizeof(*_tmp192));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp301=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp193;_tmp193.tag=7;_tmp193.f1=_tmp1B8;_tmp193.f2=_tmp1B7;({void*_tmp300=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1B6);_tmp193.f3=_tmp300;});_tmp193;});_tmp192[0]=_tmp301;});_tmp192;});}else{switch(*((int*)_tmp187.f1)){case 4: switch(*((int*)_tmp187.f2)){case 4: _LL105: _tmp1BA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_tmp1B9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_LL106:
# 853
 if(Cyc_CfFlowInfo_place_cmp(_tmp1BA,_tmp1B9)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1BA);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1B9);
goto _LLF8;case 1: _LL107: _tmp1BB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_LL108:
# 859
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1BB);{
enum Cyc_CfFlowInfo_InitLevel _tmp194=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp195=_tmp194;if(_tmp195 == Cyc_CfFlowInfo_AllIL){_LL124: _LL125:
 return(env->fenv)->notzeroall;}else{_LL126: _LL127:
 return(env->fenv)->unknown_none;}_LL123:;};default: _LL111: _tmp1BC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_LL112:
# 885
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1BC);goto _LLF8;}case 1: switch(*((int*)_tmp187.f2)){case 4: _LL109: _tmp1BD=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_LL10A:
# 865
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1BD);{
enum Cyc_CfFlowInfo_InitLevel _tmp196=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp197=_tmp196;if(_tmp197 == Cyc_CfFlowInfo_AllIL){_LL129: _LL12A:
 return(env->fenv)->notzeroall;}else{_LL12B: _LL12C:
 return(env->fenv)->unknown_none;}_LL128:;};case 5: _LL10F: _tmp1BE=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_LL110: {
# 880
enum Cyc_CfFlowInfo_InitLevel _tmp19C=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,_tmp1BE);enum Cyc_CfFlowInfo_InitLevel _tmp19D=_tmp19C;if(_tmp19D == Cyc_CfFlowInfo_AllIL){_LL133: _LL134:
 return(env->fenv)->notzeroall;}else{_LL135: _LL136:
 return(env->fenv)->unknown_none;}_LL132:;}default: goto _LL117;}case 5: switch(*((int*)_tmp187.f2)){case 5: _LL10B: _tmp1C0=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_tmp1BF=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_LL10C:
# 872
 return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp198=_region_malloc(_tmp185,sizeof(*_tmp198));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp303=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp199;_tmp199.tag=5;({void*_tmp302=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C0,_tmp1BF);_tmp199.f1=_tmp302;});_tmp199;});_tmp198[0]=_tmp303;});_tmp198;});case 1: _LL10D: _tmp1C1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1;_LL10E: {
# 875
enum Cyc_CfFlowInfo_InitLevel _tmp19A=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,_tmp1C1);enum Cyc_CfFlowInfo_InitLevel _tmp19B=_tmp19A;if(_tmp19B == Cyc_CfFlowInfo_AllIL){_LL12E: _LL12F:
 return(env->fenv)->notzeroall;}else{_LL130: _LL131:
 return(env->fenv)->unknown_none;}_LL12D:;}case 4: goto _LL113;default: goto _LL117;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->tag == 4){_LL113: _tmp1C2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1;_LL114:
# 886
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1C2);goto _LLF8;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->tag == 6){_LL115: _tmp1C8=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1).is_union;_tmp1C7=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f1).fieldnum;_tmp1C6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f1)->f2;_tmp1C5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1).is_union;_tmp1C4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f1).fieldnum;_tmp1C3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp187.f2)->f2;_LL116: {
# 889
struct _dyneither_ptr new_d=({unsigned int _tmp1A1=
_get_dyneither_size(_tmp1C6,sizeof(void*));void**_tmp1A2=(void**)({struct _RegionHandle*_tmp304=_tmp185;_region_malloc(_tmp304,_check_times(sizeof(void*),_tmp1A1));});struct _dyneither_ptr _tmp1A4=_tag_dyneither(_tmp1A2,sizeof(void*),_tmp1A1);{unsigned int _tmp1A3=_tmp1A1;unsigned int i;for(i=0;i < _tmp1A3;i ++){({typeof((void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1C6.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1C3,sizeof(void*),(int)i))))_tmp305=(void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1C6.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1C3,sizeof(void*),(int)i)));_tmp1A2[i]=_tmp305;});}}_tmp1A4;});
# 894
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1C6,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1C6.curr)[i]){
change=1;break;}}}
# 899
if(!change){
if(!_tmp1C8)return r1;
new_d=_tmp1C6;}else{
# 904
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1C6,sizeof(void*));++ i){
if(({void*_tmp306=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp306 != *((void**)_check_dyneither_subscript(_tmp1C3,sizeof(void*),i));})){
change=1;break;}}}
# 909
if(!change){
if(!_tmp1C8)return r2;
new_d=_tmp1C3;}}
# 914
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp19E=_region_malloc(_tmp185,sizeof(*_tmp19E));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp308=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp19F;_tmp19F.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp307=({struct Cyc_CfFlowInfo_UnionRInfo _tmp1A0;_tmp1A0.is_union=_tmp1C8;_tmp1A0.fieldnum=_tmp1C7 == _tmp1C4?_tmp1C7: - 1;_tmp1A0;});_tmp19F.f1=_tmp307;});_tmp19F.f2=new_d;_tmp19F;});_tmp19E[0]=_tmp308;});_tmp19E;});}}else{goto _LL117;}}else{_LL117: _LL118:
# 916
 goto _LLF8;}}}}}}}_LLF8:;}{
# 918
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple13 _tmp1CA=({struct _tuple13 _tmp1D2;_tmp1D2.f1=r1;_tmp1D2.f2=r2;_tmp1D2;});struct _tuple13 _tmp1CB=_tmp1CA;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1CB.f1)->tag == 3){_LL138: _LL139:
 goto _LL13B;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1CB.f2)->tag == 3){_LL13A: _LL13B: {
# 923
struct _tuple19 _tmp1CC=({struct _tuple19 _tmp1CE;_tmp1CE.f1=il1;_tmp1CE.f2=il2;_tmp1CE;});struct _tuple19 _tmp1CD=_tmp1CC;if(_tmp1CD.f2 == Cyc_CfFlowInfo_NoneIL){_LL13F: _LL140:
 goto _LL142;}else{if(_tmp1CD.f1 == Cyc_CfFlowInfo_NoneIL){_LL141: _LL142: return(env->fenv)->esc_none;}else{_LL143: _LL144:
 return(env->fenv)->esc_all;}}_LL13E:;}}else{_LL13C: _LL13D: {
# 928
struct _tuple19 _tmp1CF=({struct _tuple19 _tmp1D1;_tmp1D1.f1=il1;_tmp1D1.f2=il2;_tmp1D1;});struct _tuple19 _tmp1D0=_tmp1CF;if(_tmp1D0.f2 == Cyc_CfFlowInfo_NoneIL){_LL146: _LL147:
 goto _LL149;}else{if(_tmp1D0.f1 == Cyc_CfFlowInfo_NoneIL){_LL148: _LL149: return(env->fenv)->unknown_none;}else{_LL14A: _LL14B:
 return(env->fenv)->unknown_all;}}_LL145:;}}}_LL137:;};};}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 935
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 937
struct _tuple20 _tmp1D3=({struct _tuple20 _tmp1DF;_tmp1DF.f1=f1;_tmp1DF.f2=f2;_tmp1DF;});struct _tuple20 _tmp1D4=_tmp1D3;struct Cyc_Dict_Dict _tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_Dict_Dict _tmp1DC;struct Cyc_List_List*_tmp1DB;if(((_tmp1D4.f1).BottomFL).tag == 1){_LL14D: _LL14E:
 return f2;}else{if(((_tmp1D4.f2).BottomFL).tag == 1){_LL14F: _LL150:
 return f1;}else{_LL151: _tmp1DE=(((_tmp1D4.f1).ReachableFL).val).f1;_tmp1DD=(((_tmp1D4.f1).ReachableFL).val).f2;_tmp1DC=(((_tmp1D4.f2).ReachableFL).val).f1;_tmp1DB=(((_tmp1D4.f2).ReachableFL).val).f2;_LL152:
# 943
 if(_tmp1DE.t == _tmp1DC.t  && _tmp1DD == _tmp1DB)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp1D5=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp1D6=({struct Cyc_CfFlowInfo_JoinEnv _tmp1D9;_tmp1D9.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp309=({struct Cyc_CfFlowInfo_EscPile*_tmp1DA=_region_malloc(_tmp1D5,sizeof(*_tmp1DA));_tmp1DA->rgn=_tmp1D5;_tmp1DA->places=0;_tmp1DA;});_tmp1D9.pile=_tmp309;});_tmp1D9.d1=_tmp1DE;_tmp1D9.d2=_tmp1DC;_tmp1D9;});
struct Cyc_Dict_Dict _tmp1D7=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp1D6,_tmp1DE,_tmp1DC);
struct Cyc_List_List*_tmp1D8=Cyc_Relations_join_relns(fenv->r,_tmp1DD,_tmp1DB);
return({struct Cyc_Dict_Dict _tmp30A=Cyc_CfFlowInfo_escape_these(fenv,_tmp1D6.pile,_tmp1D7);Cyc_CfFlowInfo_ReachableFL(_tmp30A,_tmp1D8);});};}}_LL14C:;}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 955
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple12 pr1,struct _tuple12 pr2){
# 961
struct _tuple12 _tmp1E0=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp1F9;void*_tmp1F8;_LL154: _tmp1F9=_tmp1E0.f1;_tmp1F8=_tmp1E0.f2;_LL155:;{
struct _tuple12 _tmp1E1=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp1F7;void*_tmp1F6;_LL157: _tmp1F7=_tmp1E1.f1;_tmp1F6=_tmp1E1.f2;_LL158:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,_tmp1F9,_tmp1F7);
struct _tuple21 _tmp1E2=({struct _tuple21 _tmp1F5;_tmp1F5.f1=_tmp1F9;_tmp1F5.f2=_tmp1F7;_tmp1F5.f3=outflow;_tmp1F5;});struct _tuple21 _tmp1E3=_tmp1E2;struct Cyc_Dict_Dict _tmp1F4;struct Cyc_Dict_Dict _tmp1F3;struct Cyc_Dict_Dict _tmp1F2;struct Cyc_List_List*_tmp1F1;if(((_tmp1E3.f1).BottomFL).tag == 1){_LL15A: _LL15B:
 return({struct _tuple12 _tmp1E4;_tmp1E4.f1=outflow;_tmp1E4.f2=_tmp1F6;_tmp1E4;});}else{if(((_tmp1E3.f2).BottomFL).tag == 1){_LL15C: _LL15D:
 return({struct _tuple12 _tmp1E5;_tmp1E5.f1=outflow;_tmp1E5.f2=_tmp1F8;_tmp1E5;});}else{if(((_tmp1E3.f3).ReachableFL).tag == 2){_LL15E: _tmp1F4=(((_tmp1E3.f1).ReachableFL).val).f1;_tmp1F3=(((_tmp1E3.f2).ReachableFL).val).f1;_tmp1F2=(((_tmp1E3.f3).ReachableFL).val).f1;_tmp1F1=(((_tmp1E3.f3).ReachableFL).val).f2;_LL15F:
# 969
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1F8,_tmp1F6))return({struct _tuple12 _tmp1E6;_tmp1E6.f1=outflow;_tmp1E6.f2=_tmp1F6;_tmp1E6;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1F6,_tmp1F8))return({struct _tuple12 _tmp1E7;_tmp1E7.f1=outflow;_tmp1E7.f2=_tmp1F8;_tmp1E7;});{
struct _RegionHandle*_tmp1E8=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp1E9=({struct Cyc_CfFlowInfo_JoinEnv _tmp1EC;_tmp1EC.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp30B=({struct Cyc_CfFlowInfo_EscPile*_tmp1ED=_region_malloc(_tmp1E8,sizeof(*_tmp1ED));_tmp1ED->rgn=_tmp1E8;_tmp1ED->places=0;_tmp1ED;});_tmp1EC.pile=_tmp30B;});_tmp1EC.d1=_tmp1F4;_tmp1EC.d2=_tmp1F3;_tmp1EC;});
void*_tmp1EA=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp1E9,0,_tmp1F8,_tmp1F6);
return({struct _tuple12 _tmp1EB;({union Cyc_CfFlowInfo_FlowInfo _tmp30D=({struct Cyc_Dict_Dict _tmp30C=Cyc_CfFlowInfo_escape_these(fenv,_tmp1E9.pile,_tmp1F2);Cyc_CfFlowInfo_ReachableFL(_tmp30C,_tmp1F1);});_tmp1EB.f1=_tmp30D;});_tmp1EB.f2=_tmp1EA;_tmp1EB;});};}else{_LL160: _LL161:
# 977
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));({struct Cyc_Core_Impossible_exn_struct _tmp30F=({struct Cyc_Core_Impossible_exn_struct _tmp1EF;_tmp1EF.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp30E=({const char*_tmp1F0="join_flow_and_rval: BottomFL outflow";_tag_dyneither(_tmp1F0,sizeof(char),37);});_tmp1EF.f1=_tmp30E;});_tmp1EF;});_tmp1EE[0]=_tmp30F;});_tmp1EE;}));}}}_LL159:;};};}
# 982
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 985
{struct _tuple13 _tmp1FA=({struct _tuple13 _tmp20A;_tmp20A.f1=r1;_tmp20A.f2=r2;_tmp20A;});struct _tuple13 _tmp1FB=_tmp1FA;int _tmp209;int _tmp208;struct _dyneither_ptr _tmp207;int _tmp206;int _tmp205;struct _dyneither_ptr _tmp204;void*_tmp203;void*_tmp202;struct Cyc_CfFlowInfo_Place*_tmp201;struct Cyc_CfFlowInfo_Place*_tmp200;struct Cyc_Absyn_Vardecl*_tmp1FF;void*_tmp1FE;struct Cyc_Absyn_Vardecl*_tmp1FD;void*_tmp1FC;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 8){_LL163: _tmp1FF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f1;_tmp1FE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f2;_tmp1FD=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f1;_tmp1FC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f2;_LL164:
# 987
 return _tmp1FF == _tmp1FD  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp1FE,_tmp1FC);}else{_LL165: _LL166:
 goto _LL168;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 8){_LL167: _LL168:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->tag == 4){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 4){_LL169: _tmp201=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f1;_tmp200=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f1;_LL16A:
 return Cyc_CfFlowInfo_place_cmp(_tmp201,_tmp200)== 0;}else{_LL16B: _LL16C:
 goto _LL16E;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 4){_LL16D: _LL16E:
 return 0;}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->tag == 5){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 5){_LL16F: _tmp203=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f1;_tmp202=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f1;_LL170:
 return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp203,_tmp202);}else{_LL171: _LL172:
 goto _LL174;}}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 5){_LL173: _LL174:
 return 0;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->tag == 6)switch(*((int*)_tmp1FB.f2)){case 6: _LL175: _tmp209=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f1).is_union;_tmp208=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f1).fieldnum;_tmp207=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->f2;_tmp206=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f1).is_union;_tmp205=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f1).fieldnum;_tmp204=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->f2;_LL176:
# 998
 if(_tmp209  && _tmp208 != _tmp205)return 0;
if((void**)_tmp207.curr == (void**)_tmp204.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp207,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp207.curr)[i],*((void**)_check_dyneither_subscript(_tmp204,sizeof(void*),i))))return 0;}}
return 1;case 0: goto _LL177;case 1: goto _LL179;default: goto _LL17F;}else{switch(*((int*)_tmp1FB.f2)){case 0: _LL177: _LL178:
 goto _LL17A;case 1: _LL179: _LL17A:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f1)->tag == 3){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1FB.f2)->tag == 3){_LL17B: _LL17C:
 goto _LL162;}else{_LL17D: _LL17E:
 return 0;}}else{_LL17F: _LL180:
# 1013
 goto _LL162;}}}}}}}}}_LL162:;}{
# 1015
struct _tuple19 _tmp20B=({struct _tuple19 _tmp20D;({enum Cyc_CfFlowInfo_InitLevel _tmp310=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp20D.f1=_tmp310;});({enum Cyc_CfFlowInfo_InitLevel _tmp311=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp20D.f2=_tmp311;});_tmp20D;});struct _tuple19 _tmp20C=_tmp20B;if(_tmp20C.f1 == Cyc_CfFlowInfo_AllIL){if(_tmp20C.f2 == Cyc_CfFlowInfo_AllIL){_LL182: _LL183:
 return 1;}else{goto _LL184;}}else{if(_tmp20C.f2 == Cyc_CfFlowInfo_NoneIL){_LL184: _LL185:
 return 1;}else{_LL186: _LL187:
 return 0;}}_LL181:;};}
# 1028
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1030
struct _tuple20 _tmp20E=({struct _tuple20 _tmp214;_tmp214.f1=f1;_tmp214.f2=f2;_tmp214;});struct _tuple20 _tmp20F=_tmp20E;struct Cyc_Dict_Dict _tmp213;struct Cyc_List_List*_tmp212;struct Cyc_Dict_Dict _tmp211;struct Cyc_List_List*_tmp210;if(((_tmp20F.f1).BottomFL).tag == 1){_LL189: _LL18A:
 return 1;}else{if(((_tmp20F.f2).BottomFL).tag == 1){_LL18B: _LL18C:
 return 0;}else{_LL18D: _tmp213=(((_tmp20F.f1).ReachableFL).val).f1;_tmp212=(((_tmp20F.f1).ReachableFL).val).f2;_tmp211=(((_tmp20F.f2).ReachableFL).val).f1;_tmp210=(((_tmp20F.f2).ReachableFL).val).f2;_LL18E:
# 1034
 if(_tmp213.t == _tmp211.t  && _tmp212 == _tmp210)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp213,_tmp211) && 
Cyc_Relations_relns_approx(_tmp212,_tmp210);}}_LL188:;}
# 1040
struct _tuple11 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv){
# 1042
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp215=rv;struct Cyc_Absyn_Vardecl*_tmp218;void*_tmp217;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp215)->tag == 8){_LL190: _tmp218=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp215)->f1;_tmp217=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp215)->f2;_LL191:
# 1047
({struct Cyc_List_List*_tmp312=({struct Cyc_List_List*_tmp216=_region_malloc(rgn,sizeof(*_tmp216));_tmp216->hd=_tmp218;_tmp216->tl=names;_tmp216;});names=_tmp312;});rv=_tmp217;goto _LL18F;}else{_LL192: _LL193:
# 1049
 done=1;goto _LL18F;}_LL18F:;}
# 1052
return({struct _tuple11 _tmp219;_tmp219.f1=rv;_tmp219.f2=names;_tmp219;});}
# 1055
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp21A=il;if(_tmp21A == Cyc_CfFlowInfo_NoneIL){_LL195: _LL196:
({void*_tmp21B=0;({struct Cyc___cycFILE*_tmp314=Cyc_stderr;struct _dyneither_ptr _tmp313=({const char*_tmp21C="uninitialized";_tag_dyneither(_tmp21C,sizeof(char),14);});Cyc_fprintf(_tmp314,_tmp313,_tag_dyneither(_tmp21B,sizeof(void*),0));});});goto _LL194;}else{_LL197: _LL198:
({void*_tmp21D=0;({struct Cyc___cycFILE*_tmp316=Cyc_stderr;struct _dyneither_ptr _tmp315=({const char*_tmp21E="all-initialized";_tag_dyneither(_tmp21E,sizeof(char),16);});Cyc_fprintf(_tmp316,_tmp315,_tag_dyneither(_tmp21D,sizeof(void*),0));});});goto _LL194;}_LL194:;}
# 1062
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp21F=root;struct Cyc_Absyn_Exp*_tmp22B;void*_tmp22A;struct Cyc_Absyn_Vardecl*_tmp229;switch(*((int*)_tmp21F)){case 0: _LL19A: _tmp229=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21F)->f1;_LL19B:
# 1065
({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0;({struct _dyneither_ptr _tmp317=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp229->name));_tmp222.f1=_tmp317;});({void*_tmp220[1]={& _tmp222};({struct Cyc___cycFILE*_tmp319=Cyc_stderr;struct _dyneither_ptr _tmp318=({const char*_tmp221="Root(%s)";_tag_dyneither(_tmp221,sizeof(char),9);});Cyc_fprintf(_tmp319,_tmp318,_tag_dyneither(_tmp220,sizeof(void*),1));});});});goto _LL199;case 1: _LL19C: _tmp22B=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21F)->f1;_tmp22A=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21F)->f2;_LL19D:
# 1067
({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0;({struct _dyneither_ptr _tmp31A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp22A));_tmp226.f1=_tmp31A;});({struct Cyc_String_pa_PrintArg_struct _tmp225;_tmp225.tag=0;({struct _dyneither_ptr _tmp31B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp22B));_tmp225.f1=_tmp31B;});({void*_tmp223[2]={& _tmp225,& _tmp226};({struct Cyc___cycFILE*_tmp31D=Cyc_stderr;struct _dyneither_ptr _tmp31C=({const char*_tmp224="MallocPt(%s,%s)";_tag_dyneither(_tmp224,sizeof(char),16);});Cyc_fprintf(_tmp31D,_tmp31C,_tag_dyneither(_tmp223,sizeof(void*),2));});});});});goto _LL199;default: _LL19E: _LL19F:
# 1070
({void*_tmp227=0;({struct Cyc___cycFILE*_tmp31F=Cyc_stderr;struct _dyneither_ptr _tmp31E=({const char*_tmp228="InitParam(_,_)";_tag_dyneither(_tmp228,sizeof(char),15);});Cyc_fprintf(_tmp31F,_tmp31E,_tag_dyneither(_tmp227,sizeof(void*),0));});});goto _LL199;}_LL199:;}
# 1074
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
void*_tmp22C=(void*)x->hd;void*_tmp22D=_tmp22C;int _tmp239;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp22D)->tag == 0){_LL1A1: _tmp239=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp22D)->f1;_LL1A2:
# 1078
({struct Cyc_Int_pa_PrintArg_struct _tmp230;_tmp230.tag=1;_tmp230.f1=(unsigned long)_tmp239;({void*_tmp22E[1]={& _tmp230};({struct Cyc___cycFILE*_tmp321=Cyc_stderr;struct _dyneither_ptr _tmp320=({const char*_tmp22F=".%d";_tag_dyneither(_tmp22F,sizeof(char),4);});Cyc_fprintf(_tmp321,_tmp320,_tag_dyneither(_tmp22E,sizeof(void*),1));});});});
goto _LL1A0;}else{_LL1A3: _LL1A4:
# 1081
 if(x->tl != 0){
void*_tmp231=(void*)((struct Cyc_List_List*)_check_null(x->tl))->hd;void*_tmp232=_tmp231;int _tmp236;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp232)->tag == 0){_LL1A6: _tmp236=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp232)->f1;_LL1A7:
# 1084
 x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _tmp235;_tmp235.tag=1;_tmp235.f1=(unsigned long)_tmp236;({void*_tmp233[1]={& _tmp235};({struct Cyc___cycFILE*_tmp323=Cyc_stderr;struct _dyneither_ptr _tmp322=({const char*_tmp234="->%d";_tag_dyneither(_tmp234,sizeof(char),5);});Cyc_fprintf(_tmp323,_tmp322,_tag_dyneither(_tmp233,sizeof(void*),1));});});});
continue;}else{_LL1A8: _LL1A9:
 goto _LL1A5;}_LL1A5:;}
# 1090
({void*_tmp237=0;({struct Cyc___cycFILE*_tmp325=Cyc_stderr;struct _dyneither_ptr _tmp324=({const char*_tmp238="*";_tag_dyneither(_tmp238,sizeof(char),2);});Cyc_fprintf(_tmp325,_tmp324,_tag_dyneither(_tmp237,sizeof(void*),0));});});}_LL1A0:;}}
# 1093
({void*_tmp23A=0;({struct Cyc___cycFILE*_tmp327=Cyc_stderr;struct _dyneither_ptr _tmp326=({const char*_tmp23B=" ";_tag_dyneither(_tmp23B,sizeof(char),2);});Cyc_fprintf(_tmp327,_tmp326,_tag_dyneither(_tmp23A,sizeof(void*),0));});});}
# 1096
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1101
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp23C=1;
while(x != 0){
if(!_tmp23C){({void*_tmp23D=0;({struct Cyc___cycFILE*_tmp329=Cyc_stderr;struct _dyneither_ptr _tmp328=({const char*_tmp23E=", ";_tag_dyneither(_tmp23E,sizeof(char),3);});Cyc_fprintf(_tmp329,_tmp328,_tag_dyneither(_tmp23D,sizeof(void*),0));});});_tmp23C=0;}
pr(x->hd);
x=x->tl;}
# 1108
({void*_tmp23F=0;({struct Cyc___cycFILE*_tmp32B=Cyc_stderr;struct _dyneither_ptr _tmp32A=({const char*_tmp240="\n";_tag_dyneither(_tmp240,sizeof(char),2);});Cyc_fprintf(_tmp32B,_tmp32A,_tag_dyneither(_tmp23F,sizeof(void*),0));});});}
# 1111
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp241=rval;struct Cyc_Absyn_Vardecl*_tmp277;void*_tmp276;struct Cyc_Absyn_Exp*_tmp275;int _tmp274;void*_tmp273;int _tmp272;int _tmp271;struct _dyneither_ptr _tmp270;void*_tmp26F;struct Cyc_CfFlowInfo_Place*_tmp26E;enum Cyc_CfFlowInfo_InitLevel _tmp26D;enum Cyc_CfFlowInfo_InitLevel _tmp26C;switch(*((int*)_tmp241)){case 0: _LL1AB: _LL1AC:
({void*_tmp242=0;({struct Cyc___cycFILE*_tmp32D=Cyc_stderr;struct _dyneither_ptr _tmp32C=({const char*_tmp243="Zero";_tag_dyneither(_tmp243,sizeof(char),5);});Cyc_fprintf(_tmp32D,_tmp32C,_tag_dyneither(_tmp242,sizeof(void*),0));});});goto _LL1AA;case 1: _LL1AD: _LL1AE:
({void*_tmp244=0;({struct Cyc___cycFILE*_tmp32F=Cyc_stderr;struct _dyneither_ptr _tmp32E=({const char*_tmp245="NotZeroAll";_tag_dyneither(_tmp245,sizeof(char),11);});Cyc_fprintf(_tmp32F,_tmp32E,_tag_dyneither(_tmp244,sizeof(void*),0));});});goto _LL1AA;case 2: _LL1AF: _tmp26C=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1;_LL1B0:
({void*_tmp246=0;({struct Cyc___cycFILE*_tmp331=Cyc_stderr;struct _dyneither_ptr _tmp330=({const char*_tmp247="Unknown(";_tag_dyneither(_tmp247,sizeof(char),9);});Cyc_fprintf(_tmp331,_tmp330,_tag_dyneither(_tmp246,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(_tmp26C);
({void*_tmp248=0;({struct Cyc___cycFILE*_tmp333=Cyc_stderr;struct _dyneither_ptr _tmp332=({const char*_tmp249=")";_tag_dyneither(_tmp249,sizeof(char),2);});Cyc_fprintf(_tmp333,_tmp332,_tag_dyneither(_tmp248,sizeof(void*),0));});});goto _LL1AA;case 3: _LL1B1: _tmp26D=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1;_LL1B2:
({void*_tmp24A=0;({struct Cyc___cycFILE*_tmp335=Cyc_stderr;struct _dyneither_ptr _tmp334=({const char*_tmp24B="Esc(";_tag_dyneither(_tmp24B,sizeof(char),5);});Cyc_fprintf(_tmp335,_tmp334,_tag_dyneither(_tmp24A,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(_tmp26D);
({void*_tmp24C=0;({struct Cyc___cycFILE*_tmp337=Cyc_stderr;struct _dyneither_ptr _tmp336=({const char*_tmp24D=")";_tag_dyneither(_tmp24D,sizeof(char),2);});Cyc_fprintf(_tmp337,_tmp336,_tag_dyneither(_tmp24C,sizeof(void*),0));});});goto _LL1AA;case 4: _LL1B3: _tmp26E=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1;_LL1B4:
({void*_tmp24E=0;({struct Cyc___cycFILE*_tmp339=Cyc_stderr;struct _dyneither_ptr _tmp338=({const char*_tmp24F="AddrOf(";_tag_dyneither(_tmp24F,sizeof(char),8);});Cyc_fprintf(_tmp339,_tmp338,_tag_dyneither(_tmp24E,sizeof(void*),0));});});Cyc_CfFlowInfo_print_place(_tmp26E);
({void*_tmp250=0;({struct Cyc___cycFILE*_tmp33B=Cyc_stderr;struct _dyneither_ptr _tmp33A=({const char*_tmp251=")";_tag_dyneither(_tmp251,sizeof(char),2);});Cyc_fprintf(_tmp33B,_tmp33A,_tag_dyneither(_tmp250,sizeof(void*),0));});});goto _LL1AA;case 5: _LL1B5: _tmp26F=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1;_LL1B6:
({void*_tmp252=0;({struct Cyc___cycFILE*_tmp33D=Cyc_stderr;struct _dyneither_ptr _tmp33C=({const char*_tmp253="UniquePtr(";_tag_dyneither(_tmp253,sizeof(char),11);});Cyc_fprintf(_tmp33D,_tmp33C,_tag_dyneither(_tmp252,sizeof(void*),0));});});Cyc_CfFlowInfo_print_absrval(_tmp26F);
({void*_tmp254=0;({struct Cyc___cycFILE*_tmp33F=Cyc_stderr;struct _dyneither_ptr _tmp33E=({const char*_tmp255=")";_tag_dyneither(_tmp255,sizeof(char),2);});Cyc_fprintf(_tmp33F,_tmp33E,_tag_dyneither(_tmp254,sizeof(void*),0));});});goto _LL1AA;case 6: _LL1B7: _tmp272=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1).is_union;_tmp271=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1).fieldnum;_tmp270=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241)->f2;_LL1B8:
# 1124
 if(_tmp272){
({void*_tmp256=0;({struct Cyc___cycFILE*_tmp341=Cyc_stderr;struct _dyneither_ptr _tmp340=({const char*_tmp257="AggrUnion{";_tag_dyneither(_tmp257,sizeof(char),11);});Cyc_fprintf(_tmp341,_tmp340,_tag_dyneither(_tmp256,sizeof(void*),0));});});
if(_tmp271 != - 1)
({struct Cyc_Int_pa_PrintArg_struct _tmp25A;_tmp25A.tag=1;_tmp25A.f1=(unsigned long)_tmp271;({void*_tmp258[1]={& _tmp25A};({struct Cyc___cycFILE*_tmp343=Cyc_stderr;struct _dyneither_ptr _tmp342=({const char*_tmp259="tag == %d;";_tag_dyneither(_tmp259,sizeof(char),11);});Cyc_fprintf(_tmp343,_tmp342,_tag_dyneither(_tmp258,sizeof(void*),1));});});});}else{
# 1129
({void*_tmp25B=0;({struct Cyc___cycFILE*_tmp345=Cyc_stderr;struct _dyneither_ptr _tmp344=({const char*_tmp25C="AggrStruct{";_tag_dyneither(_tmp25C,sizeof(char),12);});Cyc_fprintf(_tmp345,_tmp344,_tag_dyneither(_tmp25B,sizeof(void*),0));});});}
{int i=0;for(0;i < _get_dyneither_size(_tmp270,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp270.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp270,sizeof(void*)))({void*_tmp25D=0;({struct Cyc___cycFILE*_tmp347=Cyc_stderr;struct _dyneither_ptr _tmp346=({const char*_tmp25E=",";_tag_dyneither(_tmp25E,sizeof(char),2);});Cyc_fprintf(_tmp347,_tmp346,_tag_dyneither(_tmp25D,sizeof(void*),0));});});}}
# 1134
({void*_tmp25F=0;({struct Cyc___cycFILE*_tmp349=Cyc_stderr;struct _dyneither_ptr _tmp348=({const char*_tmp260="}";_tag_dyneither(_tmp260,sizeof(char),2);});Cyc_fprintf(_tmp349,_tmp348,_tag_dyneither(_tmp25F,sizeof(void*),0));});});
goto _LL1AA;case 7: _LL1B9: _tmp275=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1;_tmp274=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp241)->f2;_tmp273=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp241)->f3;_LL1BA:
# 1137
({struct Cyc_Int_pa_PrintArg_struct _tmp264;_tmp264.tag=1;_tmp264.f1=(unsigned long)_tmp274;({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0;({struct _dyneither_ptr _tmp34A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp275));_tmp263.f1=_tmp34A;});({void*_tmp261[2]={& _tmp263,& _tmp264};({struct Cyc___cycFILE*_tmp34C=Cyc_stderr;struct _dyneither_ptr _tmp34B=({const char*_tmp262="[Consumed(%s,%d,";_tag_dyneither(_tmp262,sizeof(char),17);});Cyc_fprintf(_tmp34C,_tmp34B,_tag_dyneither(_tmp261,sizeof(void*),2));});});});});
Cyc_CfFlowInfo_print_absrval(_tmp273);({void*_tmp265=0;({struct Cyc___cycFILE*_tmp34E=Cyc_stderr;struct _dyneither_ptr _tmp34D=({const char*_tmp266=")]";_tag_dyneither(_tmp266,sizeof(char),3);});Cyc_fprintf(_tmp34E,_tmp34D,_tag_dyneither(_tmp265,sizeof(void*),0));});});
goto _LL1AA;default: _LL1BB: _tmp277=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241)->f1;_tmp276=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241)->f2;_LL1BC:
# 1141
({struct Cyc_String_pa_PrintArg_struct _tmp269;_tmp269.tag=0;({struct _dyneither_ptr _tmp34F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp277->name));_tmp269.f1=_tmp34F;});({void*_tmp267[1]={& _tmp269};({struct Cyc___cycFILE*_tmp351=Cyc_stderr;struct _dyneither_ptr _tmp350=({const char*_tmp268="[NamedLocation(%s,";_tag_dyneither(_tmp268,sizeof(char),19);});Cyc_fprintf(_tmp351,_tmp350,_tag_dyneither(_tmp267,sizeof(void*),1));});});});
Cyc_CfFlowInfo_print_absrval(_tmp276);({void*_tmp26A=0;({struct Cyc___cycFILE*_tmp353=Cyc_stderr;struct _dyneither_ptr _tmp352=({const char*_tmp26B=")]";_tag_dyneither(_tmp26B,sizeof(char),3);});Cyc_fprintf(_tmp353,_tmp352,_tag_dyneither(_tmp26A,sizeof(void*),0));});});
goto _LL1AA;}_LL1AA:;}
# 1147
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp278=0;({struct Cyc___cycFILE*_tmp355=Cyc_stderr;struct _dyneither_ptr _tmp354=({const char*_tmp279="    ";_tag_dyneither(_tmp279,sizeof(char),5);});Cyc_fprintf(_tmp355,_tmp354,_tag_dyneither(_tmp278,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp27A=0;({struct Cyc___cycFILE*_tmp357=Cyc_stderr;struct _dyneither_ptr _tmp356=({const char*_tmp27B=" --> ";_tag_dyneither(_tmp27B,sizeof(char),6);});Cyc_fprintf(_tmp357,_tmp356,_tag_dyneither(_tmp27A,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp27C=0;({struct Cyc___cycFILE*_tmp359=Cyc_stderr;struct _dyneither_ptr _tmp358=({const char*_tmp27D="\n";_tag_dyneither(_tmp27D,sizeof(char),2);});Cyc_fprintf(_tmp359,_tmp358,_tag_dyneither(_tmp27C,sizeof(void*),0));});});}
# 1155
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1159
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp27E=f;struct Cyc_Dict_Dict _tmp288;struct Cyc_List_List*_tmp287;if((_tmp27E.BottomFL).tag == 1){_LL1BE: _LL1BF:
({void*_tmp27F=0;({struct Cyc___cycFILE*_tmp35B=Cyc_stderr;struct _dyneither_ptr _tmp35A=({const char*_tmp280="  BottomFL\n";_tag_dyneither(_tmp280,sizeof(char),12);});Cyc_fprintf(_tmp35B,_tmp35A,_tag_dyneither(_tmp27F,sizeof(void*),0));});});goto _LL1BD;}else{_LL1C0: _tmp288=((_tmp27E.ReachableFL).val).f1;_tmp287=((_tmp27E.ReachableFL).val).f2;_LL1C1:
# 1163
({void*_tmp281=0;({struct Cyc___cycFILE*_tmp35D=Cyc_stderr;struct _dyneither_ptr _tmp35C=({const char*_tmp282="  ReachableFL:\n";_tag_dyneither(_tmp282,sizeof(char),16);});Cyc_fprintf(_tmp35D,_tmp35C,_tag_dyneither(_tmp281,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_flowdict(_tmp288);
({void*_tmp283=0;({struct Cyc___cycFILE*_tmp35F=Cyc_stderr;struct _dyneither_ptr _tmp35E=({const char*_tmp284="\n  Relations: ";_tag_dyneither(_tmp284,sizeof(char),15);});Cyc_fprintf(_tmp35F,_tmp35E,_tag_dyneither(_tmp283,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp287);
({void*_tmp285=0;({struct Cyc___cycFILE*_tmp361=Cyc_stderr;struct _dyneither_ptr _tmp360=({const char*_tmp286="\n";_tag_dyneither(_tmp286,sizeof(char),2);});Cyc_fprintf(_tmp361,_tmp360,_tag_dyneither(_tmp285,sizeof(void*),0));});});
goto _LL1BD;}_LL1BD:;}

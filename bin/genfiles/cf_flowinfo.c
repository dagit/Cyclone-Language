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
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
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
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 55
void*Cyc_Tcutil_compress(void*t);
# 98
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 152
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 165
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 267 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 308
int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 113
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 115
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple10{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple10 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 153
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env();
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
struct _tuple11 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_init_pointer(void*r);
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
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple12 pr1,struct _tuple12 pr2);
# 206
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 38 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 41
Cyc_CfFlowInfo_anal_error=1;
Cyc_Warn_verr(loc,fmt,ap);}
# 46
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp1;(_tmp1.PlaceL).val=x;(_tmp1.PlaceL).tag=1;_tmp1;});}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp2;(_tmp2.UnknownL).val=0;(_tmp2.UnknownL).tag=2;_tmp2;});}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp3;(_tmp3.BottomFL).val=0;(_tmp3.BottomFL).tag=1;_tmp3;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp4;({struct _tuple10 _tmp27E=({struct _tuple10 _tmp5;_tmp5.f1=fd;_tmp5.f2=r;_tmp5;});(_tmp4.ReachableFL).val=_tmp27E;});(_tmp4.ReachableFL).tag=2;_tmp4;});}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0U,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp6=_cycalloc(sizeof(*_tmp6));({void*_tmp280=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp14=_cycalloc_atomic(sizeof(*_tmp14));({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp27F=({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp15;_tmp15.tag=0U;_tmp15;});_tmp14[0]=_tmp27F;});_tmp14;});_tmp6->zero=_tmp280;});({void*_tmp282=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp12=_cycalloc_atomic(sizeof(*_tmp12));({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp281=({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp13;_tmp13.tag=1U;_tmp13;});_tmp12[0]=_tmp281;});_tmp12;});_tmp6->notzeroall=_tmp282;});({void*_tmp284=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10=_cycalloc_atomic(sizeof(*_tmp10));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp283=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp11;_tmp11.tag=2U;_tmp11.f1=Cyc_CfFlowInfo_NoneIL;_tmp11;});_tmp10[0]=_tmp283;});_tmp10;});_tmp6->unknown_none=_tmp284;});({void*_tmp286=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE=_cycalloc_atomic(sizeof(*_tmpE));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp285=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmpF;_tmpF.tag=2U;_tmpF.f1=Cyc_CfFlowInfo_AllIL;_tmpF;});_tmpE[0]=_tmp285;});_tmpE;});_tmp6->unknown_all=_tmp286;});({void*_tmp288=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpC=_cycalloc_atomic(sizeof(*_tmpC));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp287=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpD;_tmpD.tag=3U;_tmpD.f1=Cyc_CfFlowInfo_NoneIL;_tmpD;});_tmpC[0]=_tmp287;});_tmpC;});_tmp6->esc_none=_tmp288;});({void*_tmp28A=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpA=_cycalloc_atomic(sizeof(*_tmpA));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp289=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpB;_tmpB.tag=3U;_tmpB.f1=Cyc_CfFlowInfo_AllIL;_tmpB;});_tmpA[0]=_tmp289;});_tmpA;});_tmp6->esc_all=_tmp28A;});({struct Cyc_Dict_Dict _tmp28B=
# 68
((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(Cyc_CfFlowInfo_root_cmp);_tmp6->mt_flowdict=_tmp28B;});({struct Cyc_CfFlowInfo_Place*_tmp28E=({struct Cyc_CfFlowInfo_Place*_tmp7=_cycalloc(sizeof(*_tmp7));({void*_tmp28D=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8=_cycalloc(sizeof(*_tmp8));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp28C=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp9;_tmp9.tag=1U;_tmp9.f1=& dummy_exp;_tmp9.f2=(void*)& Cyc_Absyn_VoidType_val;_tmp9;});_tmp8[0]=_tmp28C;});_tmp8;});_tmp7->root=_tmp28D;});_tmp7->path=0;_tmp7;});_tmp6->dummy_place=_tmp28E;});_tmp6;});}
# 74
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp17=*place;struct Cyc_CfFlowInfo_Place _tmp18=_tmp17;void*_tmp21;struct Cyc_List_List*_tmp20;_LL1: _tmp21=_tmp18.root;_tmp20=_tmp18.path;_LL2:;{
void*_tmp19=_tmp21;struct Cyc_Absyn_Vardecl*_tmp1F;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp19)->tag == 0U){_LL4: _tmp1F=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp19)->f1;_LL5:
# 78
 if(_tmp20 == 0)
return Cyc_Absynpp_qvar2string(_tmp1F->name);else{
# 81
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0U;({struct _dyneither_ptr _tmp28F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1F->name));_tmp1C.f1=_tmp28F;});({void*_tmp1A[1U]={& _tmp1C};({struct _dyneither_ptr _tmp290=({const char*_tmp1B="reachable from %s";_tag_dyneither(_tmp1B,sizeof(char),18U);});Cyc_aprintf(_tmp290,_tag_dyneither(_tmp1A,sizeof(void*),1U));});});});}}else{_LL6: _LL7:
({void*_tmp1D=0U;({struct _dyneither_ptr _tmp291=({const char*_tmp1E="error locations not for VarRoots";_tag_dyneither(_tmp1E,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp291,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});}_LL3:;};}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp22=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp22,f)== 0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _tmp25;_tmp25.tag=0U;_tmp25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp23[1U]={& _tmp25};({struct _dyneither_ptr _tmp292=({const char*_tmp24="get_field_index_fs failed: %s";_tag_dyneither(_tmp24,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp292,_tag_dyneither(_tmp23,sizeof(void*),1U));});});});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp26=Cyc_Tcutil_compress(t);void*_tmp27=_tmp26;struct Cyc_List_List*_tmp2D;union Cyc_Absyn_AggrInfoU _tmp2C;switch(*((int*)_tmp27)){case 11U: _LL1: _tmp2C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp27)->f1).aggr_info;_LL2: {
# 100
struct Cyc_List_List*_tmp28=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp2C))->impl))->fields;
_tmp2D=_tmp28;goto _LL4;}case 12U: _LL3: _tmp2D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27)->f2;_LL4:
# 103
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp2D,f);default: _LL5: _LL6:
# 105
({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0U;({struct _dyneither_ptr _tmp293=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp2B.f1=_tmp293;});({void*_tmp29[1U]={& _tmp2B};({struct _dyneither_ptr _tmp294=({const char*_tmp2A="get_field_index failed: %s";_tag_dyneither(_tmp2A,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp294,_tag_dyneither(_tmp29,sizeof(void*),1U));});});});}_LL0:;}struct _tuple13{void*f1;void*f2;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple13 _tmp2E=({struct _tuple13 _tmp36;_tmp36.f1=r1;_tmp36.f2=r2;_tmp36;});struct _tuple13 _tmp2F=_tmp2E;int _tmp35;int _tmp34;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Vardecl*_tmp31;struct Cyc_Absyn_Vardecl*_tmp30;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2F.f1)->tag == 0U){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2F.f2)->tag == 0U){_LL1: _tmp31=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2F.f1)->f1;_tmp30=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2F.f2)->f1;_LL2:
 return(int)_tmp31 - (int)_tmp30;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2F.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2F.f1)->tag == 1U){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2F.f2)->tag == 1U){_LL7: _tmp33=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2F.f1)->f1;_tmp32=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2F.f2)->f1;_LL8:
 return(int)_tmp33 - (int)_tmp32;}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2F.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{_LLD: _tmp35=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp2F.f1)->f1;_tmp34=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp2F.f2)->f1;_LLE:
 return _tmp35 - _tmp34;}}}}_LL0:;};}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple13 _tmp37=({struct _tuple13 _tmp3B;_tmp3B.f1=p1;_tmp3B.f2=p2;_tmp3B;});struct _tuple13 _tmp38=_tmp37;int _tmp3A;int _tmp39;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp38.f1)->tag == 0U){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp38.f2)->tag == 0U){_LL1: _tmp3A=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp38.f1)->f1;_tmp39=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp38.f2)->f1;_LL2:
# 126
 if(_tmp3A == _tmp39)return 0;
if(_tmp3A < _tmp39)return - 1;else{
return 1;}}else{_LL7: _LL8:
# 131
 return 1;}}else{if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmp38.f2)->tag == 1U){_LL3: _LL4:
# 129
 return 0;}else{_LL5: _LL6:
 return - 1;}}_LL0:;}
# 135
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 139
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp3C=*p1;struct Cyc_CfFlowInfo_Place _tmp3D=_tmp3C;void*_tmp43;struct Cyc_List_List*_tmp42;_LL1: _tmp43=_tmp3D.root;_tmp42=_tmp3D.path;_LL2:;{
struct Cyc_CfFlowInfo_Place _tmp3E=*p2;struct Cyc_CfFlowInfo_Place _tmp3F=_tmp3E;void*_tmp41;struct Cyc_List_List*_tmp40;_LL4: _tmp41=_tmp3F.root;_tmp40=_tmp3F.path;_LL5:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp43,_tmp41);
if(i != 0)
return i;
return Cyc_CfFlowInfo_path_cmp(_tmp42,_tmp40);};};};}
# 160 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 165
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 171
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _dyneither_ptr d=({unsigned int _tmp48=sz;void**_tmp49=(void**)_cycalloc(_check_times(sizeof(void*),_tmp48));struct _dyneither_ptr _tmp4B=_tag_dyneither(_tmp49,sizeof(void*),_tmp48);{unsigned int _tmp4A=_tmp48;unsigned int i;for(i=0;i < _tmp4A;i ++){_tmp49[i]=(void*)fenv->unknown_all;}}_tmp4B;});
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp44=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp45=_tmp44;struct _dyneither_ptr*_tmp47;void*_tmp46;_LL1: _tmp47=_tmp45->name;_tmp46=_tmp45->type;_LL2:;
if(_get_dyneither_size(*_tmp47,sizeof(char))!= 1)
({void*_tmp295=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp46,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp295;});}}
# 179
return d;}struct _tuple14{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 183
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
# 187
struct _tuple14*_tmp4C=env;struct _RegionHandle*_tmp52;struct Cyc_List_List*_tmp51;_LL1: _tmp52=_tmp4C->f1;_tmp51=_tmp4C->f2;_LL2:;{
void*_tmp4D=Cyc_Tcutil_rsubstitute(_tmp52,_tmp51,f->type);
struct Cyc_Absyn_Exp*_tmp4E=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp4F=_tmp4E == 0?0: Cyc_Tcutil_rsubsexp(_tmp52,_tmp51,_tmp4E);
return({struct Cyc_Absyn_Aggrfield*_tmp50=_region_malloc(_tmp52,sizeof(*_tmp50));_tmp50->name=f->name;_tmp50->tq=f->tq;_tmp50->type=_tmp4D;_tmp50->width=f->width;_tmp50->attributes=f->attributes;_tmp50->requires_clause=_tmp4F;_tmp50;});};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 198
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 204
struct _RegionHandle _tmp53=_new_region("temp");struct _RegionHandle*temp=& _tmp53;_push_region(temp);
# 208
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp54=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp55=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp56=Cyc_Tcutil_tvar_kind(_tmp54,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp57=_tmp56;switch(((struct Cyc_Absyn_Kind*)_tmp57)->kind){case Cyc_Absyn_RgnKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_EffKind: _LL3: _LL4:
 continue;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 217
({struct Cyc_List_List*_tmp297=({struct Cyc_List_List*_tmp58=_region_malloc(temp,sizeof(*_tmp58));({struct _tuple15*_tmp296=({struct _tuple15*_tmp59=_region_malloc(temp,sizeof(*_tmp59));_tmp59->f1=_tmp54;_tmp59->f2=_tmp55;_tmp59;});_tmp58->hd=_tmp296;});_tmp58->tl=inst;_tmp58;});inst=_tmp297;});}
# 219
if(inst != 0){
struct _tuple14 _tmp5A=({struct _tuple14 _tmp5C;_tmp5C.f1=temp;({struct Cyc_List_List*_tmp298=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp5C.f2=_tmp298;});_tmp5C;});
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp5A,fs);
struct _dyneither_ptr _tmp5B=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0U);return _tmp5B;}else{
# 224
struct _dyneither_ptr _tmp5D=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0U);return _tmp5D;}}
# 208
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 228
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 234
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 236
{void*_tmp5E=Cyc_Tcutil_compress(t);void*_tmp5F=_tmp5E;union Cyc_Absyn_Constraint*_tmp79;void*_tmp78;void*_tmp77;union Cyc_Absyn_Constraint*_tmp76;enum Cyc_Absyn_AggrKind _tmp75;struct Cyc_List_List*_tmp74;union Cyc_Absyn_AggrInfoU _tmp73;struct Cyc_List_List*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_Datatypefield*_tmp70;switch(*((int*)_tmp5F)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5F)->f1).field_info).KnownDatatypefield).tag == 2){_LL1: _tmp70=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5F)->f1).field_info).KnownDatatypefield).val).f2;_LL2:
# 238
 if(_tmp70->typs == 0)
return leafval;
_tmp71=_tmp70->typs;goto _LL4;}else{goto _LLF;}case 10U: _LL3: _tmp71=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5F)->f1;_LL4: {
# 242
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp71);
struct _dyneither_ptr d=({unsigned int _tmp63=sz;void**_tmp64=(void**)_cycalloc(_check_times(sizeof(void*),_tmp63));struct _dyneither_ptr _tmp66=_tag_dyneither(_tmp64,sizeof(void*),_tmp63);{unsigned int _tmp65=_tmp63;unsigned int i;for(i=0;i < _tmp65;i ++){_tmp64[i]=(void*)fenv->unknown_all;}}_tmp66;});
{int i=0;for(0;i < sz;++ i){
({void*_tmp299=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp71))->hd)).f2,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp299;});
_tmp71=_tmp71->tl;}}
# 248
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp60=_cycalloc(sizeof(*_tmp60));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp29B=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp61;_tmp61.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp29A=({struct Cyc_CfFlowInfo_UnionRInfo _tmp62;_tmp62.is_union=0;_tmp62.fieldnum=- 1;_tmp62;});_tmp61.f1=_tmp29A;});_tmp61.f2=d;_tmp61;});_tmp60[0]=_tmp29B;});_tmp60;});}case 11U: _LL5: _tmp73=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5F)->f1).aggr_info;_tmp72=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5F)->f1).targs;_LL6: {
# 250
struct Cyc_Absyn_Aggrdecl*_tmp67=Cyc_Absyn_get_known_aggrdecl(_tmp73);
if(_tmp67->impl == 0)
goto _LL0;{
struct Cyc_List_List*_tmp68=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->fields;
if(_tmp72 == 0){_tmp75=_tmp67->kind;_tmp74=_tmp68;goto _LL8;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp69=_cycalloc(sizeof(*_tmp69));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp29E=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp6A;_tmp6A.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp29C=({struct Cyc_CfFlowInfo_UnionRInfo _tmp6B;_tmp6B.is_union=_tmp67->kind == Cyc_Absyn_UnionA;_tmp6B.fieldnum=- 1;_tmp6B;});_tmp6A.f1=_tmp29C;});({struct _dyneither_ptr _tmp29D=
Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmp67->tvs,_tmp72,_tmp68,_tmp67->kind == Cyc_Absyn_UnionA,leafval);_tmp6A.f2=_tmp29D;});_tmp6A;});_tmp69[0]=_tmp29E;});_tmp69;});};}case 12U: _LL7: _tmp75=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5F)->f1;_tmp74=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5F)->f2;_LL8:
# 265
 return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2A1=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp6D;_tmp6D.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp29F=({struct Cyc_CfFlowInfo_UnionRInfo _tmp6E;_tmp6E.is_union=_tmp75 == Cyc_Absyn_UnionA;_tmp6E.fieldnum=- 1;_tmp6E;});_tmp6D.f1=_tmp29F;});({struct _dyneither_ptr _tmp2A0=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp74,_tmp75 == Cyc_Absyn_UnionA,leafval);_tmp6D.f2=_tmp2A0;});_tmp6D;});_tmp6C[0]=_tmp2A1;});_tmp6C;});case 8U: _LL9: _tmp77=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F)->f1).elt_type;_tmp76=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F)->f1).zero_term;_LLA:
# 268
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp76))
# 271
return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmp77)?fenv->unknown_all: leafval;
goto _LL0;case 19U: _LLB: _tmp78=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F)->f1;_LLC:
 return leafval;case 5U: _LLD: _tmp79=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F)->f1).ptr_atts).nullable;_LLE:
# 275
 if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp79)){
void*_tmp6F=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp6F)->tag == 2U){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp6F)->f1 == Cyc_CfFlowInfo_AllIL){_LL12: _LL13:
 return fenv->notzeroall;}else{goto _LL14;}}else{_LL14: _LL15:
 goto _LL11;}_LL11:;}
# 280
goto _LL0;default: _LLF: _LL10:
 goto _LL0;}_LL0:;}
# 284
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 287
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 293
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmp7A=r;void*_tmp82;int _tmp81;int _tmp80;struct _dyneither_ptr _tmp7F;struct Cyc_Absyn_Exp*_tmp7E;int _tmp7D;void*_tmp7C;switch(*((int*)_tmp7A)){case 7U: _LL1: _tmp7E=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f1;_tmp7D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f2;_tmp7C=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f3;_LL2:
# 296
 if(_tmp7E == e  && _tmp7D == env_iteration){
*needs_unconsume=1;
return 0;}
# 300
return 1;case 6U: _LL3: _tmp81=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f1).is_union;_tmp80=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f1).fieldnum;_tmp7F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f2;_LL4:
# 302
 if(!_tmp81  || _tmp80 == - 1){
unsigned int _tmp7B=_get_dyneither_size(_tmp7F,sizeof(void*));
{int i=0;for(0;i < _tmp7B;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmp7F.curr)[i],needs_unconsume))
return 1;}}
# 308
return 0;}else{
# 311
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmp7F,sizeof(void*),_tmp80)),needs_unconsume);}case 8U: _LL5: _tmp82=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp7A)->f2;_LL6:
# 313
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmp82,needs_unconsume);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 320
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmp83=r;struct Cyc_Absyn_Vardecl*_tmp94;void*_tmp93;struct Cyc_CfFlowInfo_UnionRInfo _tmp92;struct _dyneither_ptr _tmp91;struct Cyc_Absyn_Exp*_tmp90;int _tmp8F;void*_tmp8E;switch(*((int*)_tmp83)){case 7U: _LL1: _tmp90=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp83)->f1;_tmp8F=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp83)->f2;_tmp8E=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp83)->f3;_LL2:
# 323
 return _tmp8E;case 6U: _LL3: _tmp92=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp83)->f1;_tmp91=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp83)->f2;_LL4: {
# 325
unsigned int _tmp84=_get_dyneither_size(_tmp91,sizeof(void*));
int change=0;
struct _dyneither_ptr d2=({unsigned int _tmp87=_tmp84;void**_tmp88=(void**)_cycalloc(_check_times(sizeof(void*),_tmp87));struct _dyneither_ptr _tmp8A=_tag_dyneither(_tmp88,sizeof(void*),_tmp87);{unsigned int _tmp89=_tmp87;unsigned int i;for(i=0;i < _tmp89;i ++){_tmp88[i]=(void*)((void**)_tmp91.curr)[(int)i];}}_tmp8A;});
{int i=0;for(0;i < _tmp84;++ i){
({void*_tmp2A2=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmp91.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2A2;});
if(((void**)d2.curr)[i]!= ((void**)_tmp91.curr)[i])
change=1;}}
# 333
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp85=_cycalloc(sizeof(*_tmp85));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2A3=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp86;_tmp86.tag=6U;_tmp86.f1=_tmp92;_tmp86.f2=d2;_tmp86;});_tmp85[0]=_tmp2A3;});_tmp85;});else{
return r;}}case 8U: _LL5: _tmp94=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp83)->f1;_tmp93=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp83)->f2;_LL6: {
# 337
void*_tmp8B=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmp93);
if(_tmp8B != _tmp93)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2A4=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8D;_tmp8D.tag=8U;_tmp8D.f1=_tmp94;_tmp8D.f2=_tmp8B;_tmp8D;});_tmp8C[0]=_tmp2A4;});_tmp8C;});else{
return r;}}default: _LL7: _LL8:
 return r;}_LL0:;}
# 347
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple13 _tmp95=({struct _tuple13 _tmpBA;({void*_tmp2A5=Cyc_Tcutil_compress(t);_tmpBA.f1=_tmp2A5;});_tmpBA.f2=r;_tmpBA;});struct _tuple13 _tmp96=_tmp95;enum Cyc_Absyn_AggrKind _tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_CfFlowInfo_UnionRInfo _tmpB7;struct _dyneither_ptr _tmpB6;union Cyc_Absyn_AggrInfoU _tmpB5;struct Cyc_CfFlowInfo_UnionRInfo _tmpB4;struct _dyneither_ptr _tmpB3;struct Cyc_List_List*_tmpB2;struct Cyc_CfFlowInfo_UnionRInfo _tmpB1;struct _dyneither_ptr _tmpB0;struct Cyc_Absyn_Vardecl*_tmpAF;void*_tmpAE;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->tag == 8U){_LL1: _tmpAF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f1;_tmpAE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f2;_LL2: {
# 350
void*_tmp97=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpAE);
if(_tmp97 != _tmpAE)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp98=_cycalloc(sizeof(*_tmp98));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2A6=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp99;_tmp99.tag=8U;_tmp99.f1=_tmpAF;_tmp99.f2=_tmp97;_tmp99;});_tmp98[0]=_tmp2A6;});_tmp98;});else{
return r;}}}else{switch(*((int*)_tmp96.f1)){case 10U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->tag == 6U){_LL3: _tmpB2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp96.f1)->f1;_tmpB1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f1;_tmpB0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f2;_LL4: {
# 354
unsigned int _tmp9A=_get_dyneither_size(_tmpB0,sizeof(void*));
struct _dyneither_ptr d2=({unsigned int _tmp9D=_tmp9A;void**_tmp9E=(void**)_cycalloc(_check_times(sizeof(void*),_tmp9D));struct _dyneither_ptr _tmpA0=_tag_dyneither(_tmp9E,sizeof(void*),_tmp9D);{unsigned int _tmp9F=_tmp9D;unsigned int i;for(i=0;i < _tmp9F;i ++){_tmp9E[i]=(void*)fenv->unknown_all;}}_tmpA0;});
{int i=0;for(0;i < _tmp9A;++ i){
({void*_tmp2A7=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpB2))->hd)).f2,consumer,iteration,((void**)_tmpB0.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2A7;});
_tmpB2=_tmpB2->tl;}}
# 360
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2A8=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp9C;_tmp9C.tag=6U;_tmp9C.f1=_tmpB1;_tmp9C.f2=d2;_tmp9C;});_tmp9B[0]=_tmp2A8;});_tmp9B;});}}else{goto _LL9;}case 11U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->tag == 6U){_LL5: _tmpB5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp96.f1)->f1).aggr_info;_tmpB4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f1;_tmpB3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f2;_LL6: {
# 362
struct Cyc_Absyn_Aggrdecl*_tmpA1=Cyc_Absyn_get_known_aggrdecl(_tmpB5);
if(_tmpA1->impl == 0)return r;
_tmpB9=_tmpA1->kind;_tmpB8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA1->impl))->fields;_tmpB7=_tmpB4;_tmpB6=_tmpB3;goto _LL8;}}else{goto _LL9;}case 12U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->tag == 6U){_LL7: _tmpB9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp96.f1)->f1;_tmpB8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp96.f1)->f2;_tmpB7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f1;_tmpB6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp96.f2)->f2;_LL8: {
# 366
struct _dyneither_ptr d2=({unsigned int _tmpA8=_get_dyneither_size(_tmpB6,sizeof(void*));void**_tmpA9=(void**)_cycalloc(_check_times(sizeof(void*),_tmpA8));struct _dyneither_ptr _tmpAB=_tag_dyneither(_tmpA9,sizeof(void*),_tmpA8);{unsigned int _tmpAA=_tmpA8;unsigned int i;for(i=0;i < _tmpAA;i ++){_tmpA9[i]=(void*)((void**)_tmpB6.curr)[(int)i];}}_tmpAB;});
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB8);
{int i=0;for(0;i < sz;(i ++,_tmpB8=_tmpB8->tl)){
struct Cyc_Absyn_Aggrfield*_tmpA2=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpB8))->hd;struct Cyc_Absyn_Aggrfield*_tmpA3=_tmpA2;struct _dyneither_ptr*_tmpA5;void*_tmpA4;_LLC: _tmpA5=_tmpA3->name;_tmpA4=_tmpA3->type;_LLD:;
if(_get_dyneither_size(*_tmpA5,sizeof(char))!= 1)
({void*_tmp2A9=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpA4,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpB6,sizeof(void*),i)));*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2A9;});}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2AA=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpA7;_tmpA7.tag=6U;_tmpA7.f1=_tmpB7;_tmpA7.f2=d2;_tmpA7;});_tmpA6[0]=_tmp2AA;});_tmpA6;});}}else{goto _LL9;}default: _LL9: _LLA:
# 375
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2AB=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpAD;_tmpAD.tag=7U;_tmpAD.f1=consumer;_tmpAD.f2=iteration;_tmpAD.f3=r;_tmpAD;});_tmpAC[0]=_tmp2AB;});_tmpAC;});else{
return r;}}}_LL0:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 381
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 383
struct _RegionHandle _tmpBB=_new_region("r");struct _RegionHandle*r=& _tmpBB;_push_region(r);
{struct _tuple17 elem=({struct _tuple17 _tmpC7;_tmpC7.f1=place;_tmpC7.f2=0U;_tmpC7;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmpBC=elem.f1;
struct Cyc_CfFlowInfo_Place _tmpBD=*place;struct Cyc_CfFlowInfo_Place _tmpBE=_tmpBD;void*_tmpC5;struct Cyc_List_List*_tmpC4;_LL1: _tmpC5=_tmpBE.root;_tmpC4=_tmpBE.path;_LL2:;{
struct Cyc_CfFlowInfo_Place _tmpBF=*_tmpBC;struct Cyc_CfFlowInfo_Place _tmpC0=_tmpBF;void*_tmpC3;struct Cyc_List_List*_tmpC2;_LL4: _tmpC3=_tmpC0.root;_tmpC2=_tmpC0.path;_LL5:;
if(Cyc_CfFlowInfo_root_cmp(_tmpC5,_tmpC3)!= 0)
continue;
for(0;_tmpC4 != 0  && _tmpC2 != 0;(_tmpC4=_tmpC4->tl,_tmpC2=_tmpC2->tl)){
if((void*)_tmpC4->hd != (void*)_tmpC2->hd)break;}
if(_tmpC4 == 0){
int _tmpC1=1;_npop_handler(0U);return _tmpC1;}};}{
# 397
int _tmpC6=0;_npop_handler(0U);return _tmpC6;};}
# 384
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 407
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 410
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp2AC=({struct Cyc_List_List*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->hd=place;_tmpC8->tl=pile->places;_tmpC8;});pile->places=_tmp2AC;});}
# 413
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmpC9=r;struct Cyc_CfFlowInfo_UnionRInfo _tmpCE;struct _dyneither_ptr _tmpCD;struct Cyc_CfFlowInfo_Place*_tmpCC;void*_tmpCB;void*_tmpCA;switch(*((int*)_tmpC9)){case 7U: _LL1: _tmpCA=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC9)->f3;_LL2:
 Cyc_CfFlowInfo_add_places(pile,_tmpCA);return;case 8U: _LL3: _tmpCB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC9)->f2;_LL4:
 Cyc_CfFlowInfo_add_places(pile,_tmpCB);return;case 4U: _LL5: _tmpCC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC9)->f1;_LL6:
 Cyc_CfFlowInfo_add_place(pile,_tmpCC);return;case 6U: _LL7: _tmpCE=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC9)->f1;_tmpCD=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC9)->f2;_LL8:
# 419
{int i=0;for(0;i < _get_dyneither_size(_tmpCD,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmpCD.curr)[i]);}}
return;default: _LL9: _LLA:
 return;}_LL0:;}
# 429
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
void*_tmpCF=old_val;struct Cyc_Absyn_Vardecl*_tmpE1;void*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;int _tmpDE;void*_tmpDD;int _tmpDC;struct _dyneither_ptr _tmpDB;switch(*((int*)_tmpCF)){case 6U: _LL1: _tmpDC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f1).is_union;_tmpDB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f2;_LL2: {
# 432
struct _dyneither_ptr d2=({unsigned int _tmpD3=_get_dyneither_size(_tmpDB,sizeof(void*));void**_tmpD4=(void**)_cycalloc(_check_times(sizeof(void*),_tmpD3));struct _dyneither_ptr _tmpD6=_tag_dyneither(_tmpD4,sizeof(void*),_tmpD3);{unsigned int _tmpD5=_tmpD3;unsigned int i;for(i=0;i < _tmpD5;i ++){({typeof((void*)
Cyc_CfFlowInfo_insert_place_inner(new_val,((void**)_tmpDB.curr)[(int)i]))_tmp2AD=(void*)Cyc_CfFlowInfo_insert_place_inner(new_val,((void**)_tmpDB.curr)[(int)i]);_tmpD4[i]=_tmp2AD;});}}_tmpD6;});
# 436
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2AF=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpD1;_tmpD1.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2AE=({struct Cyc_CfFlowInfo_UnionRInfo _tmpD2;_tmpD2.is_union=_tmpDC;_tmpD2.fieldnum=- 1;_tmpD2;});_tmpD1.f1=_tmp2AE;});_tmpD1.f2=d2;_tmpD1;});_tmpD0[0]=_tmp2AF;});_tmpD0;});}case 7U: _LL3: _tmpDF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f1;_tmpDE=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f2;_tmpDD=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f3;_LL4:
# 438
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2B1=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpD8;_tmpD8.tag=7U;_tmpD8.f1=_tmpDF;_tmpD8.f2=_tmpDE;({void*_tmp2B0=Cyc_CfFlowInfo_insert_place_inner(new_val,_tmpDD);_tmpD8.f3=_tmp2B0;});_tmpD8;});_tmpD7[0]=_tmp2B1;});_tmpD7;});case 8U: _LL5: _tmpE1=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f1;_tmpE0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF)->f2;_LL6:
# 440
 return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2B3=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpDA;_tmpDA.tag=8U;_tmpDA.f1=_tmpE1;({void*_tmp2B2=Cyc_CfFlowInfo_insert_place_inner(new_val,_tmpE0);_tmpDA.f2=_tmp2B2;});_tmpDA;});_tmpD9[0]=_tmp2B3;});_tmpD9;});default: _LL7: _LL8:
 return new_val;}_LL0:;}
# 447
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _dyneither_ptr d,int n,void*rval){
void*_tmpE2=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmpE2 == rval)return d;{
struct _dyneither_ptr res=({unsigned int _tmpE3=_get_dyneither_size(d,sizeof(void*));void**_tmpE4=(void**)_cycalloc(_check_times(sizeof(void*),_tmpE3));struct _dyneither_ptr _tmpE6=_tag_dyneither(_tmpE4,sizeof(void*),_tmpE3);{unsigned int _tmpE5=_tmpE3;unsigned int i;for(i=0;i < _tmpE5;i ++){_tmpE4[i]=(void*)((void**)d.curr)[(int)i];}}_tmpE6;});
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 460
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 462
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple18 _tmpE7=({struct _tuple18 _tmpFE;_tmpFE.f1=path;_tmpFE.f2=old_val;_tmpFE;});struct _tuple18 _tmpE8=_tmpE7;struct Cyc_Absyn_Vardecl*_tmpFD;void*_tmpFC;struct Cyc_List_List*_tmpFB;void*_tmpFA;int _tmpF9;struct Cyc_List_List*_tmpF8;int _tmpF7;struct _dyneither_ptr _tmpF6;if(_tmpE8.f1 != 0){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmpE8.f1)->hd)->tag == 0U)switch(*((int*)_tmpE8.f2)){case 6U: _LL1: _tmpF9=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpE8.f1)->hd)->f1;_tmpF8=(_tmpE8.f1)->tl;_tmpF7=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE8.f2)->f1).is_union;_tmpF6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE8.f2)->f2;_LL2: {
# 466
void*_tmpE9=Cyc_CfFlowInfo_insert_place_outer(_tmpF8,*((void**)_check_dyneither_subscript(_tmpF6,sizeof(void*),_tmpF9)),new_val);
struct _dyneither_ptr _tmpEA=Cyc_CfFlowInfo_aggr_dict_insert(_tmpF6,_tmpF9,_tmpE9);
if((void**)_tmpEA.curr == (void**)_tmpF6.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2B5=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpEC;_tmpEC.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2B4=({struct Cyc_CfFlowInfo_UnionRInfo _tmpED;_tmpED.is_union=_tmpF7;_tmpED.fieldnum=- 1;_tmpED;});_tmpEC.f1=_tmp2B4;});_tmpEC.f2=_tmpEA;_tmpEC;});_tmpEB[0]=_tmp2B5;});_tmpEB;});}}case 8U: goto _LL5;default: goto _LL7;}else{switch(*((int*)_tmpE8.f2)){case 5U: _LL3: _tmpFB=(_tmpE8.f1)->tl;_tmpFA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpE8.f2)->f1;_LL4: {
# 471
void*_tmpEE=Cyc_CfFlowInfo_insert_place_outer(_tmpFB,_tmpFA,new_val);
if(_tmpEE == _tmpFA)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp2B6=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmpF0;_tmpF0.tag=5U;_tmpF0.f1=_tmpEE;_tmpF0;});_tmpEF[0]=_tmp2B6;});_tmpEF;});}}case 8U: goto _LL5;default: goto _LL7;}}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE8.f2)->tag == 8U){_LL5: _tmpFD=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE8.f2)->f1;_tmpFC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE8.f2)->f2;_LL6: {
# 475
void*_tmpF1=Cyc_CfFlowInfo_insert_place_outer(path,_tmpFC,new_val);
if(_tmpF1 == _tmpFC)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2B7=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpF3;_tmpF3.tag=8U;_tmpF3.f1=_tmpFD;_tmpF3.f2=_tmpF1;_tmpF3;});_tmpF2[0]=_tmp2B7;});_tmpF2;});}}}else{_LL7: _LL8:
({void*_tmpF4=0U;({struct _dyneither_ptr _tmp2B8=({const char*_tmpF5="bad insert place";_tag_dyneither(_tmpF5,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2B8,_tag_dyneither(_tmpF4,sizeof(void*),0U));});});}}_LL0:;};}
# 485
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmpFF=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmp100;_push_handler(& _tmp100);{int _tmp102=0;if(setjmp(_tmp100.handler))_tmp102=1;if(!_tmp102){({void*_tmp2B9=Cyc_CfFlowInfo_lookup_place(d,_tmpFF);oldval=_tmp2B9;});;_pop_handler();}else{void*_tmp101=(void*)_exn_thrown;void*_tmp103=_tmp101;void*_tmp104;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp103)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 continue;}else{_LL3: _tmp104=_tmp103;_LL4:(int)_rethrow(_tmp104);}_LL0:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmp105=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _tmp106=_tmp105;if(_tmp106 == Cyc_CfFlowInfo_AllIL){_LL6: _LL7:
 newval=fenv->esc_all;goto _LL5;}else{_LL8: _LL9:
 newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 496
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp107=*_tmpFF;struct Cyc_CfFlowInfo_Place _tmp108=_tmp107;void*_tmp10A;struct Cyc_List_List*_tmp109;_LLB: _tmp10A=_tmp108.root;_tmp109=_tmp108.path;_LLC:;
({struct Cyc_Dict_Dict _tmp2BE=({struct Cyc_Dict_Dict _tmp2BD=d;void*_tmp2BC=_tmp10A;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2BD,_tmp2BC,({
struct Cyc_List_List*_tmp2BB=_tmp109;void*_tmp2BA=
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp10A);
# 499
Cyc_CfFlowInfo_insert_place_outer(_tmp2BB,_tmp2BA,newval);}));});
# 498
d=_tmp2BE;});};};}
# 503
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp10B=r;void*_tmp110;enum Cyc_CfFlowInfo_InitLevel _tmp10F;enum Cyc_CfFlowInfo_InitLevel _tmp10E;switch(*((int*)_tmp10B)){case 2U: _LL1: _tmp10E=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp10B)->f1;_LL2:
 return _tmp10E;case 3U: _LL3: _tmp10F=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp10B)->f1;_LL4:
 return _tmp10F;case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 return Cyc_CfFlowInfo_AllIL;case 7U: _LL9: _tmp110=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10B)->f3;_LLA:
 return Cyc_CfFlowInfo_NoneIL;default: _LLB: _LLC:
({void*_tmp10C=0U;({struct _dyneither_ptr _tmp2BF=({const char*_tmp10D="initlevel_approx";_tag_dyneither(_tmp10D,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2BF,_tag_dyneither(_tmp10C,sizeof(void*),0U));});});}_LL0:;}
# 523
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 525
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp111=r;void*_tmp11B;struct Cyc_CfFlowInfo_Place*_tmp11A;struct _dyneither_ptr _tmp119;int _tmp118;int _tmp117;struct _dyneither_ptr _tmp116;void*_tmp115;switch(*((int*)_tmp111)){case 8U: _LL1: _tmp115=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp111)->f2;_LL2:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp115,acc);case 6U: _LL3: _tmp118=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp111)->f1).is_union;_tmp117=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp111)->f1).fieldnum;_tmp116=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp111)->f2;if(_tmp118){_LL4: {
# 533
unsigned int _tmp112=_get_dyneither_size(_tmp116,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp117 == - 1){
int i=0;for(0;i < _tmp112;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp116.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 543
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp116,sizeof(void*),_tmp117)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LL0;}}else{_LL5: _tmp119=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp111)->f2;_LL6: {
# 547
unsigned int _tmp113=_get_dyneither_size(_tmp119,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp113;++ i){
({enum Cyc_CfFlowInfo_InitLevel _tmp2C0=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp119.curr)[i],this_ans);this_ans=_tmp2C0;});}}
goto _LL0;}}case 4U: _LL7: _tmp11A=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp111)->f1;_LL8:
# 553
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp11A))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 556
({struct Cyc_List_List*_tmp2C1=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->hd=_tmp11A;_tmp114->tl=env->seen;_tmp114;});env->seen=_tmp2C1;});
({enum Cyc_CfFlowInfo_InitLevel _tmp2C3=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp2C2=env;Cyc_CfFlowInfo_initlevel_rec(_tmp2C2,Cyc_CfFlowInfo_lookup_place(env->d,_tmp11A),Cyc_CfFlowInfo_AllIL);});this_ans=_tmp2C3;});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;}
# 560
goto _LL0;case 5U: _LL9: _tmp11B=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp111)->f1;_LLA:
({enum Cyc_CfFlowInfo_InitLevel _tmp2C4=Cyc_CfFlowInfo_initlevel_rec(env,_tmp11B,Cyc_CfFlowInfo_AllIL);this_ans=_tmp2C4;});goto _LL0;default: _LLB: _LLC:
({enum Cyc_CfFlowInfo_InitLevel _tmp2C5=Cyc_CfFlowInfo_initlevel_approx(r);this_ans=_tmp2C5;});goto _LL0;}_LL0:;}
# 564
return this_ans;}
# 566
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp11C=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp11D;_tmp11D.d=d;_tmp11D.seen=0;_tmp11D;});
return Cyc_CfFlowInfo_initlevel_rec(& _tmp11C,r,Cyc_CfFlowInfo_AllIL);}
# 571
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp11E=*place;struct Cyc_CfFlowInfo_Place _tmp11F=_tmp11E;void*_tmp12D;struct Cyc_List_List*_tmp12C;_LL1: _tmp12D=_tmp11F.root;_tmp12C=_tmp11F.path;_LL2:;{
void*_tmp120=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp12D);
for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){
retry: {struct _tuple13 _tmp121=({struct _tuple13 _tmp12B;_tmp12B.f1=_tmp120;_tmp12B.f2=(void*)_tmp12C->hd;_tmp12B;});struct _tuple13 _tmp122=_tmp121;void*_tmp12A;struct Cyc_CfFlowInfo_UnionRInfo _tmp129;struct _dyneither_ptr _tmp128;int _tmp127;void*_tmp126;void*_tmp125;switch(*((int*)_tmp122.f1)){case 8U: _LL4: _tmp125=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp122.f1)->f2;_LL5:
# 577
 _tmp120=_tmp125;goto retry;case 7U: _LL6: _tmp126=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp122.f1)->f3;_LL7:
# 580
 _tmp120=_tmp126;goto retry;case 6U: if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp122.f2)->tag == 0U){_LL8: _tmp129=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp122.f1)->f1;_tmp128=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp122.f1)->f2;_tmp127=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp122.f2)->f1;_LL9:
# 582
 _tmp120=*((void**)_check_dyneither_subscript(_tmp128,sizeof(void*),_tmp127));
goto _LL3;}else{goto _LLC;}case 5U: if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmp122.f2)->tag == 1U){_LLA: _tmp12A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp122.f1)->f1;_LLB:
# 585
 _tmp120=_tmp12A;
goto _LL3;}else{goto _LLC;}default: _LLC: _LLD:
# 593
({void*_tmp123=0U;({struct _dyneither_ptr _tmp2C6=({const char*_tmp124="bad lookup_place";_tag_dyneither(_tmp124,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2C6,_tag_dyneither(_tmp123,sizeof(void*),0U));});});}_LL3:;}}
# 595
return _tmp120;};}
# 598
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp12E=rval;int _tmp133;int _tmp132;struct _dyneither_ptr _tmp131;void*_tmp130;void*_tmp12F;switch(*((int*)_tmp12E)){case 3U: _LL1: _LL2:
 return 0;case 7U: _LL3: _tmp12F=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp12E)->f3;_LL4:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp12F);case 8U: _LL5: _tmp130=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp12E)->f2;_LL6:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp130);case 6U: _LL7: _tmp133=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp12E)->f1).is_union;_tmp132=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp12E)->f1).fieldnum;_tmp131=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp12E)->f2;_LL8:
# 604
 if(_tmp133  && _tmp132 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp131,sizeof(void*),_tmp132)));else{
# 607
unsigned int sz=_get_dyneither_size(_tmp131,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp131.curr)[i]))return 0;}}
return 1;}default: _LL9: _LLA:
# 612
 return 1;}_LL0:;}
# 615
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 618
int Cyc_CfFlowInfo_is_init_pointer(void*rval){
void*_tmp134=rval;void*_tmp135;switch(*((int*)_tmp134)){case 8U: _LL1: _tmp135=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp134)->f2;_LL2:
 return Cyc_CfFlowInfo_is_init_pointer(_tmp135);case 4U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 628
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _tmp136=_new_region("rgn");struct _RegionHandle*rgn=& _tmp136;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->places=0;_tmp138;});
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _tmp137=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0U);return _tmp137;};}
# 630
;_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 642
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 647
struct _tuple13 _tmp139=({struct _tuple13 _tmp150;_tmp150.f1=oldval;_tmp150.f2=newval;_tmp150;});struct _tuple13 _tmp13A=_tmp139;enum Cyc_CfFlowInfo_InitLevel _tmp14F;struct Cyc_CfFlowInfo_UnionRInfo _tmp14E;struct _dyneither_ptr _tmp14D;struct Cyc_CfFlowInfo_UnionRInfo _tmp14C;struct _dyneither_ptr _tmp14B;struct Cyc_CfFlowInfo_Place*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp148;void*_tmp147;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13A.f1)->tag == 8U){_LL1: _tmp147=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13A.f1)->f2;_LL2:
# 649
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp147,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->tag == 8U){_LL3: _tmp149=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->f1;_tmp148=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->f2;_LL4: {
# 651
void*_tmp13B=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp148);
if(_tmp13B == _tmp148)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2C7=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp13D;_tmp13D.tag=8U;_tmp13D.f1=_tmp149;_tmp13D.f2=_tmp13B;_tmp13D;});_tmp13C[0]=_tmp2C7;});_tmp13C;});}}}else{switch(*((int*)_tmp13A.f1)){case 3U: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->tag == 4U){_LL5: _tmp14A=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->f1;_LL6:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp14A);goto _LL8;}else{_LL7: _LL8:
# 656
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp13E=0U;({unsigned int _tmp2C9=env->loc;struct _dyneither_ptr _tmp2C8=({const char*_tmp13F="assignment puts possibly-uninitialized data in an escaped location";_tag_dyneither(_tmp13F,sizeof(char),67U);});Cyc_CfFlowInfo_aerr(_tmp2C9,_tmp2C8,_tag_dyneither(_tmp13E,sizeof(void*),0U));});});
# 659
return(env->fenv)->esc_all;}case 6U: switch(*((int*)_tmp13A.f2)){case 6U: _LL9: _tmp14E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13A.f1)->f1;_tmp14D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13A.f1)->f2;_tmp14C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->f1;_tmp14B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->f2;_LLA: {
# 661
struct _dyneither_ptr new_d=({unsigned int _tmp142=
_get_dyneither_size(_tmp14D,sizeof(void*));void**_tmp143=(void**)_cycalloc(_check_times(sizeof(void*),_tmp142));struct _dyneither_ptr _tmp145=_tag_dyneither(_tmp143,sizeof(void*),_tmp142);{unsigned int _tmp144=_tmp142;unsigned int i;for(i=0;i < _tmp144;i ++){({typeof((void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp14D.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp14B,sizeof(void*),(int)i))))_tmp2CA=(void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp14D.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp14B,sizeof(void*),(int)i)));_tmp143[i]=_tmp2CA;});}}_tmp145;});
# 665
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp14D,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp14D.curr)[i]){
change=1;break;}}}
# 670
if(!change){
if(!_tmp14E.is_union)return oldval;
new_d=_tmp14D;}else{
# 675
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp14D,sizeof(void*));++ i){
if(({void*_tmp2CB=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp2CB != *((void**)_check_dyneither_subscript(_tmp14B,sizeof(void*),i));})){
change=1;break;}}}
# 680
if(!change){
if(!_tmp14E.is_union)return newval;
new_d=_tmp14B;}}
# 685
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2CC=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp141;_tmp141.tag=6U;_tmp141.f1=_tmp14C;_tmp141.f2=new_d;_tmp141;});_tmp140[0]=_tmp2CC;});_tmp140;});}case 3U: goto _LLB;default: goto _LLD;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->tag == 3U){_LLB: _tmp14F=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp13A.f2)->f1;_LLC: {
# 687
enum Cyc_CfFlowInfo_InitLevel _tmp146=_tmp14F;if(_tmp146 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 return(env->fenv)->unknown_none;}else{_LL12: _LL13:
 return(env->fenv)->unknown_all;}_LLF:;}}else{_LLD: _LLE:
# 691
 return newval;}}}}_LL0:;}
# 697
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 701
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 712
if(path == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple18 _tmp151=({struct _tuple18 _tmp178;_tmp178.f1=path;_tmp178.f2=oldval;_tmp178;});struct _tuple18 _tmp152=_tmp151;int _tmp177;struct Cyc_List_List*_tmp176;int _tmp175;int _tmp174;struct _dyneither_ptr _tmp173;struct Cyc_List_List*_tmp172;void*_tmp171;struct Cyc_Absyn_Exp*_tmp170;int _tmp16F;void*_tmp16E;struct Cyc_Absyn_Vardecl*_tmp16D;void*_tmp16C;switch(*((int*)_tmp152.f2)){case 8U: _LL1: _tmp16D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f1;_tmp16C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f2;_LL2: {
# 715
void*_tmp153=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,_tmp16C,newval);
if(_tmp153 == _tmp16C)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp154=_cycalloc(sizeof(*_tmp154));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2CD=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp155;_tmp155.tag=8U;_tmp155.f1=_tmp16D;_tmp155.f2=_tmp153;_tmp155;});_tmp154[0]=_tmp2CD;});_tmp154;});}}case 7U: _LL3: _tmp170=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f1;_tmp16F=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f2;_tmp16E=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f3;_LL4: {
# 719
void*_tmp156=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,_tmp16E,newval);
if(_tmp156 == _tmp16E)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2CE=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp158;_tmp158.tag=7U;_tmp158.f1=_tmp170;_tmp158.f2=_tmp16F;_tmp158.f3=_tmp156;_tmp158;});_tmp157[0]=_tmp2CE;});_tmp157;});}}default: if(_tmp152.f1 != 0){if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmp152.f1)->hd)->tag == 1U){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->tag == 5U){_LL5: _tmp172=(_tmp152.f1)->tl;_tmp171=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f1;_LL6: {
# 723
void*_tmp159=Cyc_CfFlowInfo_assign_place_outer(env,_tmp172,path_prefix + 1,_tmp171,newval);
if(_tmp159 == _tmp171)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp2CF=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp15B;_tmp15B.tag=5U;_tmp15B.f1=_tmp159;_tmp15B;});_tmp15A[0]=_tmp2CF;});_tmp15A;});}}}else{goto _LL9;}}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->tag == 6U){_LL7: _tmp177=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmp152.f1)->hd)->f1;_tmp176=(_tmp152.f1)->tl;_tmp175=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f1).is_union;_tmp174=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f1).fieldnum;_tmp173=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp152.f2)->f2;_LL8: {
# 727
void*_tmp15C=Cyc_CfFlowInfo_assign_place_outer(env,_tmp176,path_prefix + 1,*((void**)_check_dyneither_subscript(_tmp173,sizeof(void*),_tmp177)),newval);
# 729
struct _dyneither_ptr _tmp15D=Cyc_CfFlowInfo_aggr_dict_insert(_tmp173,_tmp177,_tmp15C);
if((void**)_tmp15D.curr == (void**)_tmp173.curr  && (!_tmp175  || _tmp174 == _tmp177))return oldval;
_tmp173=_tmp15D;
# 734
if(_tmp175){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp173,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp177){
struct _dyneither_ptr _tmp15E=({
struct _dyneither_ptr _tmp2D1=_tmp173;int _tmp2D0=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp2D1,_tmp2D0,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp173,sizeof(void*),i))));});
if((void**)_tmp15E.curr != (void**)_tmp173.curr){
_tmp173=_tmp15E;
changed=1;}}}}
# 750
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp15F=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp160=_tmp15F;void*_tmp166;struct Cyc_List_List*_tmp165;_LLC: _tmp166=_tmp160->root;_tmp165=_tmp160->path;_LLD:;{
struct Cyc_List_List*_tmp161=({int*_tmp2D2=({int*_tmp164=_cycalloc_atomic(sizeof(*_tmp164));_tmp164[0]=path_prefix;_tmp164;});((struct Cyc_List_List*(*)(int(*f)(int*,void*),int*env,struct Cyc_List_List*x))Cyc_List_filter_c)((int(*)(int*n,void*unused))Cyc_CfFlowInfo_nprefix,_tmp2D2,_tmp165);});
struct Cyc_CfFlowInfo_Place*_tmp162=({struct Cyc_CfFlowInfo_Place*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->root=_tmp166;_tmp163->path=_tmp161;_tmp163;});;};}}
# 756
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp167=_cycalloc(sizeof(*_tmp167));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2D4=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp168;_tmp168.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2D3=({struct Cyc_CfFlowInfo_UnionRInfo _tmp169;_tmp169.is_union=_tmp175;_tmp169.fieldnum=_tmp177;_tmp169;});_tmp168.f1=_tmp2D3;});_tmp168.f2=_tmp173;_tmp168;});_tmp167[0]=_tmp2D4;});_tmp167;});}}else{goto _LL9;}}}else{_LL9: _LLA:
({void*_tmp16A=0U;({struct _dyneither_ptr _tmp2D5=({const char*_tmp16B="bad assign place";_tag_dyneither(_tmp16B,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2D5,_tag_dyneither(_tmp16A,sizeof(void*),0U));});});}}_LL0:;};}
# 760
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 768
struct Cyc_CfFlowInfo_Place*_tmp179=place;void*_tmp17D;struct Cyc_List_List*_tmp17C;_LL1: _tmp17D=_tmp179->root;_tmp17C=_tmp179->path;_LL2:;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp17A;_tmp17A.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp2D6=({struct Cyc_CfFlowInfo_EscPile*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->places=0;_tmp17B;});_tmp17A.pile=_tmp2D6;});_tmp17A.d=d;_tmp17A.loc=loc;_tmp17A.root_place=place;_tmp17A;});
void*newval=({struct Cyc_List_List*_tmp2D8=_tmp17C;void*_tmp2D7=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp17D);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp2D8,0,_tmp2D7,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp2DA=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp2D9=env.pile;Cyc_CfFlowInfo_escape_these(_tmp2DA,_tmp2D9,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp17D,newval));});};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 783
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 791
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;
# 794
{struct _tuple13 _tmp17E=({struct _tuple13 _tmp1C1;_tmp1C1.f1=r1;_tmp1C1.f2=r2;_tmp1C1;});struct _tuple13 _tmp17F=_tmp17E;int _tmp1C0;int _tmp1BF;struct _dyneither_ptr _tmp1BE;int _tmp1BD;int _tmp1BC;struct _dyneither_ptr _tmp1BB;struct Cyc_CfFlowInfo_Place*_tmp1BA;void*_tmp1B9;void*_tmp1B8;void*_tmp1B7;void*_tmp1B6;struct Cyc_CfFlowInfo_Place*_tmp1B5;struct Cyc_CfFlowInfo_Place*_tmp1B4;struct Cyc_CfFlowInfo_Place*_tmp1B3;struct Cyc_CfFlowInfo_Place*_tmp1B2;struct Cyc_CfFlowInfo_Place*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;int _tmp1AF;void*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AD;int _tmp1AC;void*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AA;int _tmp1A9;void*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A7;int _tmp1A6;void*_tmp1A5;struct Cyc_Absyn_Vardecl*_tmp1A4;void*_tmp1A3;struct Cyc_Absyn_Vardecl*_tmp1A2;void*_tmp1A1;struct Cyc_Absyn_Vardecl*_tmp1A0;void*_tmp19F;struct Cyc_Absyn_Vardecl*_tmp19E;void*_tmp19D;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->tag == 8U){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->tag == 8U){_LL1: _tmp1A0=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_tmp19F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f2;_tmp19E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_tmp19D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f2;_LL2:
# 797
 if(_tmp1A0 == _tmp19E)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp180=_cycalloc(sizeof(*_tmp180));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2DC=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp181;_tmp181.tag=8U;_tmp181.f1=_tmp1A0;({void*_tmp2DB=Cyc_CfFlowInfo_join_absRval(env,a,_tmp19F,_tmp19D);_tmp181.f2=_tmp2DB;});_tmp181;});_tmp180[0]=_tmp2DC;});_tmp180;});else{
# 800
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp19F,_tmp19D);}}else{_LL3: _tmp1A2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_tmp1A1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f2;_LL4:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1A1,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->tag == 8U){_LL5: _tmp1A4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_tmp1A3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f2;_LL6:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1A3);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->tag == 7U){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->tag == 7U){_LL7: _tmp1AA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_tmp1A9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f2;_tmp1A8=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f3;_tmp1A7=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_tmp1A6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f2;_tmp1A5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f3;_LL8:
# 805
 if(_tmp1AA == _tmp1A7)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp182=_cycalloc(sizeof(*_tmp182));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2DE=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp183;_tmp183.tag=7U;_tmp183.f1=_tmp1AA;_tmp183.f2=_tmp1A9 > _tmp1A6?_tmp1A9: _tmp1A6;({void*_tmp2DD=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1A8,_tmp1A5);_tmp183.f3=_tmp2DD;});_tmp183;});_tmp182[0]=_tmp2DE;});_tmp182;});
{void*_tmp184=_tmp1A8;struct Cyc_CfFlowInfo_Place*_tmp185;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp184)->tag == 4U){_LL22: _tmp185=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp184)->f1;_LL23:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp185);goto _LL21;}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 811
{void*_tmp186=_tmp1A5;struct Cyc_CfFlowInfo_Place*_tmp187;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp186)->tag == 4U){_LL27: _tmp187=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp186)->f1;_LL28:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp187);goto _LL26;}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 815
goto _LL0;}else{_LL9: _tmp1AD=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_tmp1AC=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f2;_tmp1AB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f3;_LLA:
# 817
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp188=_cycalloc(sizeof(*_tmp188));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2E0=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp189;_tmp189.tag=7U;_tmp189.f1=_tmp1AD;_tmp189.f2=_tmp1AC;({void*_tmp2DF=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1AB,r2);_tmp189.f3=_tmp2DF;});_tmp189;});_tmp188[0]=_tmp2E0;});_tmp188;});}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->tag == 7U){_LLB: _tmp1B0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_tmp1AF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f2;_tmp1AE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f3;_LLC:
# 819
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp18A=_cycalloc(sizeof(*_tmp18A));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2E2=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp18B;_tmp18B.tag=7U;_tmp18B.f1=_tmp1B0;_tmp18B.f2=_tmp1AF;({void*_tmp2E1=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1AE);_tmp18B.f3=_tmp2E1;});_tmp18B;});_tmp18A[0]=_tmp2E2;});_tmp18A;});}else{switch(*((int*)_tmp17F.f1)){case 4U: switch(*((int*)_tmp17F.f2)){case 4U: _LLD: _tmp1B2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_tmp1B1=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_LLE:
# 822
 if(Cyc_CfFlowInfo_place_cmp(_tmp1B2,_tmp1B1)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1B2);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1B1);
goto _LL0;case 1U: _LLF: _tmp1B3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_LL10:
# 828
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B3);{
enum Cyc_CfFlowInfo_InitLevel _tmp18C=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp18D=_tmp18C;if(_tmp18D == Cyc_CfFlowInfo_AllIL){_LL2C: _LL2D:
 return(env->fenv)->notzeroall;}else{_LL2E: _LL2F:
 return(env->fenv)->unknown_none;}_LL2B:;};default: _LL19: _tmp1B4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_LL1A:
# 854
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B4);goto _LL0;}case 1U: switch(*((int*)_tmp17F.f2)){case 4U: _LL11: _tmp1B5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_LL12:
# 834
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B5);{
enum Cyc_CfFlowInfo_InitLevel _tmp18E=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp18F=_tmp18E;if(_tmp18F == Cyc_CfFlowInfo_AllIL){_LL31: _LL32:
 return(env->fenv)->notzeroall;}else{_LL33: _LL34:
 return(env->fenv)->unknown_none;}_LL30:;};case 5U: _LL17: _tmp1B6=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_LL18: {
# 849
enum Cyc_CfFlowInfo_InitLevel _tmp194=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,_tmp1B6);enum Cyc_CfFlowInfo_InitLevel _tmp195=_tmp194;if(_tmp195 == Cyc_CfFlowInfo_AllIL){_LL3B: _LL3C:
 return(env->fenv)->notzeroall;}else{_LL3D: _LL3E:
 return(env->fenv)->unknown_none;}_LL3A:;}default: goto _LL1F;}case 5U: switch(*((int*)_tmp17F.f2)){case 5U: _LL13: _tmp1B8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_tmp1B7=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_LL14:
# 841
 return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp190=_cycalloc(sizeof(*_tmp190));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp2E4=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp191;_tmp191.tag=5U;({void*_tmp2E3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B8,_tmp1B7);_tmp191.f1=_tmp2E3;});_tmp191;});_tmp190[0]=_tmp2E4;});_tmp190;});case 1U: _LL15: _tmp1B9=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1;_LL16: {
# 844
enum Cyc_CfFlowInfo_InitLevel _tmp192=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,_tmp1B9);enum Cyc_CfFlowInfo_InitLevel _tmp193=_tmp192;if(_tmp193 == Cyc_CfFlowInfo_AllIL){_LL36: _LL37:
 return(env->fenv)->notzeroall;}else{_LL38: _LL39:
 return(env->fenv)->unknown_none;}_LL35:;}case 4U: goto _LL1B;default: goto _LL1F;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->tag == 4U){_LL1B: _tmp1BA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1;_LL1C:
# 855
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1BA);goto _LL0;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->tag == 6U){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->tag == 6U){_LL1D: _tmp1C0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1).is_union;_tmp1BF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f1).fieldnum;_tmp1BE=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f1)->f2;_tmp1BD=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1).is_union;_tmp1BC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f1).fieldnum;_tmp1BB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F.f2)->f2;_LL1E: {
# 858
struct _dyneither_ptr new_d=({unsigned int _tmp199=
_get_dyneither_size(_tmp1BE,sizeof(void*));void**_tmp19A=(void**)_cycalloc(_check_times(sizeof(void*),_tmp199));struct _dyneither_ptr _tmp19C=_tag_dyneither(_tmp19A,sizeof(void*),_tmp199);{unsigned int _tmp19B=_tmp199;unsigned int i;for(i=0;i < _tmp19B;i ++){({typeof((void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1BE.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1BB,sizeof(void*),(int)i))))_tmp2E5=(void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1BE.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1BB,sizeof(void*),(int)i)));_tmp19A[i]=_tmp2E5;});}}_tmp19C;});
# 863
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1BE,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1BE.curr)[i]){
change=1;break;}}}
# 868
if(!change){
if(!_tmp1C0)return r1;
new_d=_tmp1BE;}else{
# 873
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1BE,sizeof(void*));++ i){
if(({void*_tmp2E6=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp2E6 != *((void**)_check_dyneither_subscript(_tmp1BB,sizeof(void*),i));})){
change=1;break;}}}
# 878
if(!change){
if(!_tmp1C0)return r2;
new_d=_tmp1BB;}}
# 883
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp196=_cycalloc(sizeof(*_tmp196));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E8=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp197;_tmp197.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E7=({struct Cyc_CfFlowInfo_UnionRInfo _tmp198;_tmp198.is_union=_tmp1C0;_tmp198.fieldnum=_tmp1BF == _tmp1BC?_tmp1BF: - 1;_tmp198;});_tmp197.f1=_tmp2E7;});_tmp197.f2=new_d;_tmp197;});_tmp196[0]=_tmp2E8;});_tmp196;});}}else{goto _LL1F;}}else{_LL1F: _LL20:
# 885
 goto _LL0;}}}}}}}_LL0:;}{
# 887
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple13 _tmp1C2=({struct _tuple13 _tmp1CA;_tmp1CA.f1=r1;_tmp1CA.f2=r2;_tmp1CA;});struct _tuple13 _tmp1C3=_tmp1C2;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1C3.f1)->tag == 3U){_LL40: _LL41:
 goto _LL43;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1C3.f2)->tag == 3U){_LL42: _LL43: {
# 892
struct _tuple19 _tmp1C4=({struct _tuple19 _tmp1C6;_tmp1C6.f1=il1;_tmp1C6.f2=il2;_tmp1C6;});struct _tuple19 _tmp1C5=_tmp1C4;if(_tmp1C5.f2 == Cyc_CfFlowInfo_NoneIL){_LL47: _LL48:
 goto _LL4A;}else{if(_tmp1C5.f1 == Cyc_CfFlowInfo_NoneIL){_LL49: _LL4A: return(env->fenv)->esc_none;}else{_LL4B: _LL4C:
 return(env->fenv)->esc_all;}}_LL46:;}}else{_LL44: _LL45: {
# 897
struct _tuple19 _tmp1C7=({struct _tuple19 _tmp1C9;_tmp1C9.f1=il1;_tmp1C9.f2=il2;_tmp1C9;});struct _tuple19 _tmp1C8=_tmp1C7;if(_tmp1C8.f2 == Cyc_CfFlowInfo_NoneIL){_LL4E: _LL4F:
 goto _LL51;}else{if(_tmp1C8.f1 == Cyc_CfFlowInfo_NoneIL){_LL50: _LL51: return(env->fenv)->unknown_none;}else{_LL52: _LL53:
 return(env->fenv)->unknown_all;}}_LL4D:;}}}_LL3F:;};}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 904
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 906
struct _tuple20 _tmp1CB=({struct _tuple20 _tmp1D6;_tmp1D6.f1=f1;_tmp1D6.f2=f2;_tmp1D6;});struct _tuple20 _tmp1CC=_tmp1CB;struct Cyc_Dict_Dict _tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_Dict_Dict _tmp1D3;struct Cyc_List_List*_tmp1D2;if(((_tmp1CC.f1).BottomFL).tag == 1){_LL1: _LL2:
 return f2;}else{if(((_tmp1CC.f2).BottomFL).tag == 1){_LL3: _LL4:
 return f1;}else{_LL5: _tmp1D5=(((_tmp1CC.f1).ReachableFL).val).f1;_tmp1D4=(((_tmp1CC.f1).ReachableFL).val).f2;_tmp1D3=(((_tmp1CC.f2).ReachableFL).val).f1;_tmp1D2=(((_tmp1CC.f2).ReachableFL).val).f2;_LL6:
# 912
 if(_tmp1D5.t == _tmp1D3.t  && _tmp1D4 == _tmp1D2)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv _tmp1CD=({struct Cyc_CfFlowInfo_JoinEnv _tmp1D0;_tmp1D0.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp2E9=({struct Cyc_CfFlowInfo_EscPile*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->places=0;_tmp1D1;});_tmp1D0.pile=_tmp2E9;});_tmp1D0.d1=_tmp1D5;_tmp1D0.d2=_tmp1D3;_tmp1D0;});
struct Cyc_Dict_Dict _tmp1CE=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp1CD,_tmp1D5,_tmp1D3);
struct Cyc_List_List*_tmp1CF=Cyc_Relations_join_relns(Cyc_Core_heap_region,_tmp1D4,_tmp1D2);
return({struct Cyc_Dict_Dict _tmp2EA=Cyc_CfFlowInfo_escape_these(fenv,_tmp1CD.pile,_tmp1CE);Cyc_CfFlowInfo_ReachableFL(_tmp2EA,_tmp1CF);});};}}_LL0:;}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 922
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple12 pr1,struct _tuple12 pr2){
# 927
struct _tuple12 _tmp1D7=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp1EE;void*_tmp1ED;_LL1: _tmp1EE=_tmp1D7.f1;_tmp1ED=_tmp1D7.f2;_LL2:;{
struct _tuple12 _tmp1D8=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp1EC;void*_tmp1EB;_LL4: _tmp1EC=_tmp1D8.f1;_tmp1EB=_tmp1D8.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,_tmp1EE,_tmp1EC);
struct _tuple21 _tmp1D9=({struct _tuple21 _tmp1EA;_tmp1EA.f1=_tmp1EE;_tmp1EA.f2=_tmp1EC;_tmp1EA.f3=outflow;_tmp1EA;});struct _tuple21 _tmp1DA=_tmp1D9;struct Cyc_Dict_Dict _tmp1E9;struct Cyc_Dict_Dict _tmp1E8;struct Cyc_Dict_Dict _tmp1E7;struct Cyc_List_List*_tmp1E6;if(((_tmp1DA.f1).BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple12 _tmp1DB;_tmp1DB.f1=outflow;_tmp1DB.f2=_tmp1EB;_tmp1DB;});}else{if(((_tmp1DA.f2).BottomFL).tag == 1){_LL9: _LLA:
 return({struct _tuple12 _tmp1DC;_tmp1DC.f1=outflow;_tmp1DC.f2=_tmp1ED;_tmp1DC;});}else{if(((_tmp1DA.f3).ReachableFL).tag == 2){_LLB: _tmp1E9=(((_tmp1DA.f1).ReachableFL).val).f1;_tmp1E8=(((_tmp1DA.f2).ReachableFL).val).f1;_tmp1E7=(((_tmp1DA.f3).ReachableFL).val).f1;_tmp1E6=(((_tmp1DA.f3).ReachableFL).val).f2;_LLC:
# 935
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1ED,_tmp1EB))return({struct _tuple12 _tmp1DD;_tmp1DD.f1=outflow;_tmp1DD.f2=_tmp1EB;_tmp1DD;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1EB,_tmp1ED))return({struct _tuple12 _tmp1DE;_tmp1DE.f1=outflow;_tmp1DE.f2=_tmp1ED;_tmp1DE;});{
struct Cyc_CfFlowInfo_JoinEnv _tmp1DF=({struct Cyc_CfFlowInfo_JoinEnv _tmp1E2;_tmp1E2.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp2EB=({struct Cyc_CfFlowInfo_EscPile*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->places=0;_tmp1E3;});_tmp1E2.pile=_tmp2EB;});_tmp1E2.d1=_tmp1E9;_tmp1E2.d2=_tmp1E8;_tmp1E2;});
void*_tmp1E0=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp1DF,0,_tmp1ED,_tmp1EB);
return({struct _tuple12 _tmp1E1;({union Cyc_CfFlowInfo_FlowInfo _tmp2ED=({struct Cyc_Dict_Dict _tmp2EC=Cyc_CfFlowInfo_escape_these(fenv,_tmp1DF.pile,_tmp1E7);Cyc_CfFlowInfo_ReachableFL(_tmp2EC,_tmp1E6);});_tmp1E1.f1=_tmp2ED;});_tmp1E1.f2=_tmp1E0;_tmp1E1;});};}else{_LLD: _LLE:
# 941
({void*_tmp1E4=0U;({struct _dyneither_ptr _tmp2EE=({const char*_tmp1E5="join_flow_and_rval: BottomFL outflow";_tag_dyneither(_tmp1E5,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2EE,_tag_dyneither(_tmp1E4,sizeof(void*),0U));});});}}}_LL6:;};};}
# 946
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 949
{struct _tuple13 _tmp1EF=({struct _tuple13 _tmp1FF;_tmp1FF.f1=r1;_tmp1FF.f2=r2;_tmp1FF;});struct _tuple13 _tmp1F0=_tmp1EF;int _tmp1FE;int _tmp1FD;struct _dyneither_ptr _tmp1FC;int _tmp1FB;int _tmp1FA;struct _dyneither_ptr _tmp1F9;void*_tmp1F8;void*_tmp1F7;struct Cyc_CfFlowInfo_Place*_tmp1F6;struct Cyc_CfFlowInfo_Place*_tmp1F5;struct Cyc_Absyn_Vardecl*_tmp1F4;void*_tmp1F3;struct Cyc_Absyn_Vardecl*_tmp1F2;void*_tmp1F1;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->tag == 8U){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 8U){_LL1: _tmp1F4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f1;_tmp1F3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f2;_tmp1F2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f1;_tmp1F1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f2;_LL2:
# 951
 return _tmp1F4 == _tmp1F2  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp1F3,_tmp1F1);}else{_LL3: _LL4:
 goto _LL6;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 8U){_LL5: _LL6:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->tag == 4U){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 4U){_LL7: _tmp1F6=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f1;_tmp1F5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f1;_LL8:
 return Cyc_CfFlowInfo_place_cmp(_tmp1F6,_tmp1F5)== 0;}else{_LL9: _LLA:
 goto _LLC;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 4U){_LLB: _LLC:
 return 0;}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->tag == 5U){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 5U){_LLD: _tmp1F8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f1;_tmp1F7=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f1;_LLE:
 return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp1F8,_tmp1F7);}else{_LLF: _LL10:
 goto _LL12;}}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 5U){_LL11: _LL12:
 return 0;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->tag == 6U)switch(*((int*)_tmp1F0.f2)){case 6U: _LL13: _tmp1FE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f1).is_union;_tmp1FD=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f1).fieldnum;_tmp1FC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->f2;_tmp1FB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f1).is_union;_tmp1FA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f1).fieldnum;_tmp1F9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->f2;_LL14:
# 962
 if(_tmp1FE  && _tmp1FD != _tmp1FA)return 0;
if((void**)_tmp1FC.curr == (void**)_tmp1F9.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp1FC,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp1FC.curr)[i],*((void**)_check_dyneither_subscript(_tmp1F9,sizeof(void*),i))))return 0;}}
return 1;case 0U: goto _LL15;case 1U: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_tmp1F0.f2)){case 0U: _LL15: _LL16:
 goto _LL18;case 1U: _LL17: _LL18:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f1)->tag == 3U){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1F0.f2)->tag == 3U){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 return 0;}}else{_LL1D: _LL1E:
# 977
 goto _LL0;}}}}}}}}}_LL0:;}{
# 979
struct _tuple19 _tmp200=({struct _tuple19 _tmp202;({enum Cyc_CfFlowInfo_InitLevel _tmp2EF=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp202.f1=_tmp2EF;});({enum Cyc_CfFlowInfo_InitLevel _tmp2F0=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp202.f2=_tmp2F0;});_tmp202;});struct _tuple19 _tmp201=_tmp200;if(_tmp201.f1 == Cyc_CfFlowInfo_AllIL){_LL20: _LL21:
 goto _LL23;}else{if(_tmp201.f2 == Cyc_CfFlowInfo_NoneIL){_LL22: _LL23:
 return 1;}else{_LL24: _LL25:
 return 0;}}_LL1F:;};}
# 992
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 994
struct _tuple20 _tmp203=({struct _tuple20 _tmp209;_tmp209.f1=f1;_tmp209.f2=f2;_tmp209;});struct _tuple20 _tmp204=_tmp203;struct Cyc_Dict_Dict _tmp208;struct Cyc_List_List*_tmp207;struct Cyc_Dict_Dict _tmp206;struct Cyc_List_List*_tmp205;if(((_tmp204.f1).BottomFL).tag == 1){_LL1: _LL2:
 return 1;}else{if(((_tmp204.f2).BottomFL).tag == 1){_LL3: _LL4:
 return 0;}else{_LL5: _tmp208=(((_tmp204.f1).ReachableFL).val).f1;_tmp207=(((_tmp204.f1).ReachableFL).val).f2;_tmp206=(((_tmp204.f2).ReachableFL).val).f1;_tmp205=(((_tmp204.f2).ReachableFL).val).f2;_LL6:
# 998
 if(_tmp208.t == _tmp206.t  && _tmp207 == _tmp205)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp208,_tmp206) && 
Cyc_Relations_relns_approx(_tmp207,_tmp205);}}_LL0:;}
# 1004
struct _tuple11 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp20A=rv;struct Cyc_Absyn_Vardecl*_tmp20D;void*_tmp20C;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp20A)->tag == 8U){_LL1: _tmp20D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp20A)->f1;_tmp20C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp20A)->f2;_LL2:
# 1010
({struct Cyc_List_List*_tmp2F1=({struct Cyc_List_List*_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->hd=_tmp20D;_tmp20B->tl=names;_tmp20B;});names=_tmp2F1;});rv=_tmp20C;goto _LL0;}else{_LL3: _LL4:
# 1012
 done=1;goto _LL0;}_LL0:;}
# 1015
return({struct _tuple11 _tmp20E;_tmp20E.f1=rv;_tmp20E.f2=names;_tmp20E;});}
# 1018
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp20F=il;if(_tmp20F == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
({void*_tmp210=0U;({struct Cyc___cycFILE*_tmp2F3=Cyc_stderr;struct _dyneither_ptr _tmp2F2=({const char*_tmp211="uninitialized";_tag_dyneither(_tmp211,sizeof(char),14U);});Cyc_fprintf(_tmp2F3,_tmp2F2,_tag_dyneither(_tmp210,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
({void*_tmp212=0U;({struct Cyc___cycFILE*_tmp2F5=Cyc_stderr;struct _dyneither_ptr _tmp2F4=({const char*_tmp213="all-initialized";_tag_dyneither(_tmp213,sizeof(char),16U);});Cyc_fprintf(_tmp2F5,_tmp2F4,_tag_dyneither(_tmp212,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 1025
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp214=root;struct Cyc_Absyn_Exp*_tmp220;void*_tmp21F;struct Cyc_Absyn_Vardecl*_tmp21E;switch(*((int*)_tmp214)){case 0U: _LL1: _tmp21E=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp214)->f1;_LL2:
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp217;_tmp217.tag=0U;({struct _dyneither_ptr _tmp2F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp21E->name));_tmp217.f1=_tmp2F6;});({void*_tmp215[1U]={& _tmp217};({struct Cyc___cycFILE*_tmp2F8=Cyc_stderr;struct _dyneither_ptr _tmp2F7=({const char*_tmp216="Root(%s)";_tag_dyneither(_tmp216,sizeof(char),9U);});Cyc_fprintf(_tmp2F8,_tmp2F7,_tag_dyneither(_tmp215,sizeof(void*),1U));});});});goto _LL0;case 1U: _LL3: _tmp220=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp214)->f1;_tmp21F=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp214)->f2;_LL4:
# 1030
({struct Cyc_String_pa_PrintArg_struct _tmp21B;_tmp21B.tag=0U;({struct _dyneither_ptr _tmp2F9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp21F));_tmp21B.f1=_tmp2F9;});({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0U;({struct _dyneither_ptr _tmp2FA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp220));_tmp21A.f1=_tmp2FA;});({void*_tmp218[2U]={& _tmp21A,& _tmp21B};({struct Cyc___cycFILE*_tmp2FC=Cyc_stderr;struct _dyneither_ptr _tmp2FB=({const char*_tmp219="MallocPt(%s,%s)";_tag_dyneither(_tmp219,sizeof(char),16U);});Cyc_fprintf(_tmp2FC,_tmp2FB,_tag_dyneither(_tmp218,sizeof(void*),2U));});});});});goto _LL0;default: _LL5: _LL6:
# 1033
({void*_tmp21C=0U;({struct Cyc___cycFILE*_tmp2FE=Cyc_stderr;struct _dyneither_ptr _tmp2FD=({const char*_tmp21D="InitParam(_,_)";_tag_dyneither(_tmp21D,sizeof(char),15U);});Cyc_fprintf(_tmp2FE,_tmp2FD,_tag_dyneither(_tmp21C,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 1037
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
void*_tmp221=(void*)x->hd;void*_tmp222=_tmp221;int _tmp22E;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp222)->tag == 0U){_LL1: _tmp22E=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp222)->f1;_LL2:
# 1041
({struct Cyc_Int_pa_PrintArg_struct _tmp225;_tmp225.tag=1U;_tmp225.f1=(unsigned long)_tmp22E;({void*_tmp223[1U]={& _tmp225};({struct Cyc___cycFILE*_tmp300=Cyc_stderr;struct _dyneither_ptr _tmp2FF=({const char*_tmp224=".%d";_tag_dyneither(_tmp224,sizeof(char),4U);});Cyc_fprintf(_tmp300,_tmp2FF,_tag_dyneither(_tmp223,sizeof(void*),1U));});});});
goto _LL0;}else{_LL3: _LL4:
# 1044
 if(x->tl != 0){
void*_tmp226=(void*)((struct Cyc_List_List*)_check_null(x->tl))->hd;void*_tmp227=_tmp226;int _tmp22B;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp227)->tag == 0U){_LL6: _tmp22B=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp227)->f1;_LL7:
# 1047
 x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _tmp22A;_tmp22A.tag=1U;_tmp22A.f1=(unsigned long)_tmp22B;({void*_tmp228[1U]={& _tmp22A};({struct Cyc___cycFILE*_tmp302=Cyc_stderr;struct _dyneither_ptr _tmp301=({const char*_tmp229="->%d";_tag_dyneither(_tmp229,sizeof(char),5U);});Cyc_fprintf(_tmp302,_tmp301,_tag_dyneither(_tmp228,sizeof(void*),1U));});});});
continue;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1053
({void*_tmp22C=0U;({struct Cyc___cycFILE*_tmp304=Cyc_stderr;struct _dyneither_ptr _tmp303=({const char*_tmp22D="*";_tag_dyneither(_tmp22D,sizeof(char),2U);});Cyc_fprintf(_tmp304,_tmp303,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});}_LL0:;}}
# 1056
({void*_tmp22F=0U;({struct Cyc___cycFILE*_tmp306=Cyc_stderr;struct _dyneither_ptr _tmp305=({const char*_tmp230=" ";_tag_dyneither(_tmp230,sizeof(char),2U);});Cyc_fprintf(_tmp306,_tmp305,_tag_dyneither(_tmp22F,sizeof(void*),0U));});});}
# 1059
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1064
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp231=1;
while(x != 0){
if(!_tmp231){({void*_tmp232=0U;({struct Cyc___cycFILE*_tmp308=Cyc_stderr;struct _dyneither_ptr _tmp307=({const char*_tmp233=", ";_tag_dyneither(_tmp233,sizeof(char),3U);});Cyc_fprintf(_tmp308,_tmp307,_tag_dyneither(_tmp232,sizeof(void*),0U));});});_tmp231=0;}
pr(x->hd);
x=x->tl;}
# 1071
({void*_tmp234=0U;({struct Cyc___cycFILE*_tmp30A=Cyc_stderr;struct _dyneither_ptr _tmp309=({const char*_tmp235="\n";_tag_dyneither(_tmp235,sizeof(char),2U);});Cyc_fprintf(_tmp30A,_tmp309,_tag_dyneither(_tmp234,sizeof(void*),0U));});});}
# 1074
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp236=rval;struct Cyc_Absyn_Vardecl*_tmp26C;void*_tmp26B;struct Cyc_Absyn_Exp*_tmp26A;int _tmp269;void*_tmp268;int _tmp267;int _tmp266;struct _dyneither_ptr _tmp265;void*_tmp264;struct Cyc_CfFlowInfo_Place*_tmp263;enum Cyc_CfFlowInfo_InitLevel _tmp262;enum Cyc_CfFlowInfo_InitLevel _tmp261;switch(*((int*)_tmp236)){case 0U: _LL1: _LL2:
({void*_tmp237=0U;({struct Cyc___cycFILE*_tmp30C=Cyc_stderr;struct _dyneither_ptr _tmp30B=({const char*_tmp238="Zero";_tag_dyneither(_tmp238,sizeof(char),5U);});Cyc_fprintf(_tmp30C,_tmp30B,_tag_dyneither(_tmp237,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp239=0U;({struct Cyc___cycFILE*_tmp30E=Cyc_stderr;struct _dyneither_ptr _tmp30D=({const char*_tmp23A="NotZeroAll";_tag_dyneither(_tmp23A,sizeof(char),11U);});Cyc_fprintf(_tmp30E,_tmp30D,_tag_dyneither(_tmp239,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp261=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1;_LL6:
({void*_tmp23B=0U;({struct Cyc___cycFILE*_tmp310=Cyc_stderr;struct _dyneither_ptr _tmp30F=({const char*_tmp23C="Unknown(";_tag_dyneither(_tmp23C,sizeof(char),9U);});Cyc_fprintf(_tmp310,_tmp30F,_tag_dyneither(_tmp23B,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_initlevel(_tmp261);
({void*_tmp23D=0U;({struct Cyc___cycFILE*_tmp312=Cyc_stderr;struct _dyneither_ptr _tmp311=({const char*_tmp23E=")";_tag_dyneither(_tmp23E,sizeof(char),2U);});Cyc_fprintf(_tmp312,_tmp311,_tag_dyneither(_tmp23D,sizeof(void*),0U));});});goto _LL0;case 3U: _LL7: _tmp262=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1;_LL8:
({void*_tmp23F=0U;({struct Cyc___cycFILE*_tmp314=Cyc_stderr;struct _dyneither_ptr _tmp313=({const char*_tmp240="Esc(";_tag_dyneither(_tmp240,sizeof(char),5U);});Cyc_fprintf(_tmp314,_tmp313,_tag_dyneither(_tmp23F,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_initlevel(_tmp262);
({void*_tmp241=0U;({struct Cyc___cycFILE*_tmp316=Cyc_stderr;struct _dyneither_ptr _tmp315=({const char*_tmp242=")";_tag_dyneither(_tmp242,sizeof(char),2U);});Cyc_fprintf(_tmp316,_tmp315,_tag_dyneither(_tmp241,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp263=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1;_LLA:
({void*_tmp243=0U;({struct Cyc___cycFILE*_tmp318=Cyc_stderr;struct _dyneither_ptr _tmp317=({const char*_tmp244="AddrOf(";_tag_dyneither(_tmp244,sizeof(char),8U);});Cyc_fprintf(_tmp318,_tmp317,_tag_dyneither(_tmp243,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_place(_tmp263);
({void*_tmp245=0U;({struct Cyc___cycFILE*_tmp31A=Cyc_stderr;struct _dyneither_ptr _tmp319=({const char*_tmp246=")";_tag_dyneither(_tmp246,sizeof(char),2U);});Cyc_fprintf(_tmp31A,_tmp319,_tag_dyneither(_tmp245,sizeof(void*),0U));});});goto _LL0;case 5U: _LLB: _tmp264=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1;_LLC:
({void*_tmp247=0U;({struct Cyc___cycFILE*_tmp31C=Cyc_stderr;struct _dyneither_ptr _tmp31B=({const char*_tmp248="UniquePtr(";_tag_dyneither(_tmp248,sizeof(char),11U);});Cyc_fprintf(_tmp31C,_tmp31B,_tag_dyneither(_tmp247,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_absrval(_tmp264);
({void*_tmp249=0U;({struct Cyc___cycFILE*_tmp31E=Cyc_stderr;struct _dyneither_ptr _tmp31D=({const char*_tmp24A=")";_tag_dyneither(_tmp24A,sizeof(char),2U);});Cyc_fprintf(_tmp31E,_tmp31D,_tag_dyneither(_tmp249,sizeof(void*),0U));});});goto _LL0;case 6U: _LLD: _tmp267=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1).is_union;_tmp266=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1).fieldnum;_tmp265=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp236)->f2;_LLE:
# 1087
 if(_tmp267){
({void*_tmp24B=0U;({struct Cyc___cycFILE*_tmp320=Cyc_stderr;struct _dyneither_ptr _tmp31F=({const char*_tmp24C="AggrUnion{";_tag_dyneither(_tmp24C,sizeof(char),11U);});Cyc_fprintf(_tmp320,_tmp31F,_tag_dyneither(_tmp24B,sizeof(void*),0U));});});
if(_tmp266 != - 1)
({struct Cyc_Int_pa_PrintArg_struct _tmp24F;_tmp24F.tag=1U;_tmp24F.f1=(unsigned long)_tmp266;({void*_tmp24D[1U]={& _tmp24F};({struct Cyc___cycFILE*_tmp322=Cyc_stderr;struct _dyneither_ptr _tmp321=({const char*_tmp24E="tag == %d;";_tag_dyneither(_tmp24E,sizeof(char),11U);});Cyc_fprintf(_tmp322,_tmp321,_tag_dyneither(_tmp24D,sizeof(void*),1U));});});});}else{
# 1092
({void*_tmp250=0U;({struct Cyc___cycFILE*_tmp324=Cyc_stderr;struct _dyneither_ptr _tmp323=({const char*_tmp251="AggrStruct{";_tag_dyneither(_tmp251,sizeof(char),12U);});Cyc_fprintf(_tmp324,_tmp323,_tag_dyneither(_tmp250,sizeof(void*),0U));});});}
{int i=0;for(0;i < _get_dyneither_size(_tmp265,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp265.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp265,sizeof(void*)))({void*_tmp252=0U;({struct Cyc___cycFILE*_tmp326=Cyc_stderr;struct _dyneither_ptr _tmp325=({const char*_tmp253=",";_tag_dyneither(_tmp253,sizeof(char),2U);});Cyc_fprintf(_tmp326,_tmp325,_tag_dyneither(_tmp252,sizeof(void*),0U));});});}}
# 1097
({void*_tmp254=0U;({struct Cyc___cycFILE*_tmp328=Cyc_stderr;struct _dyneither_ptr _tmp327=({const char*_tmp255="}";_tag_dyneither(_tmp255,sizeof(char),2U);});Cyc_fprintf(_tmp328,_tmp327,_tag_dyneither(_tmp254,sizeof(void*),0U));});});
goto _LL0;case 7U: _LLF: _tmp26A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1;_tmp269=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp236)->f2;_tmp268=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp236)->f3;_LL10:
# 1100
({struct Cyc_Int_pa_PrintArg_struct _tmp259;_tmp259.tag=1U;_tmp259.f1=(unsigned long)_tmp269;({struct Cyc_String_pa_PrintArg_struct _tmp258;_tmp258.tag=0U;({struct _dyneither_ptr _tmp329=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp26A));_tmp258.f1=_tmp329;});({void*_tmp256[2U]={& _tmp258,& _tmp259};({struct Cyc___cycFILE*_tmp32B=Cyc_stderr;struct _dyneither_ptr _tmp32A=({const char*_tmp257="[Consumed(%s,%d,";_tag_dyneither(_tmp257,sizeof(char),17U);});Cyc_fprintf(_tmp32B,_tmp32A,_tag_dyneither(_tmp256,sizeof(void*),2U));});});});});
Cyc_CfFlowInfo_print_absrval(_tmp268);({void*_tmp25A=0U;({struct Cyc___cycFILE*_tmp32D=Cyc_stderr;struct _dyneither_ptr _tmp32C=({const char*_tmp25B=")]";_tag_dyneither(_tmp25B,sizeof(char),3U);});Cyc_fprintf(_tmp32D,_tmp32C,_tag_dyneither(_tmp25A,sizeof(void*),0U));});});
goto _LL0;default: _LL11: _tmp26C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp236)->f1;_tmp26B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp236)->f2;_LL12:
# 1104
({struct Cyc_String_pa_PrintArg_struct _tmp25E;_tmp25E.tag=0U;({struct _dyneither_ptr _tmp32E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp26C->name));_tmp25E.f1=_tmp32E;});({void*_tmp25C[1U]={& _tmp25E};({struct Cyc___cycFILE*_tmp330=Cyc_stderr;struct _dyneither_ptr _tmp32F=({const char*_tmp25D="[NamedLocation(%s,";_tag_dyneither(_tmp25D,sizeof(char),19U);});Cyc_fprintf(_tmp330,_tmp32F,_tag_dyneither(_tmp25C,sizeof(void*),1U));});});});
Cyc_CfFlowInfo_print_absrval(_tmp26B);({void*_tmp25F=0U;({struct Cyc___cycFILE*_tmp332=Cyc_stderr;struct _dyneither_ptr _tmp331=({const char*_tmp260=")]";_tag_dyneither(_tmp260,sizeof(char),3U);});Cyc_fprintf(_tmp332,_tmp331,_tag_dyneither(_tmp25F,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 1110
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp26D=0U;({struct Cyc___cycFILE*_tmp334=Cyc_stderr;struct _dyneither_ptr _tmp333=({const char*_tmp26E="    ";_tag_dyneither(_tmp26E,sizeof(char),5U);});Cyc_fprintf(_tmp334,_tmp333,_tag_dyneither(_tmp26D,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp26F=0U;({struct Cyc___cycFILE*_tmp336=Cyc_stderr;struct _dyneither_ptr _tmp335=({const char*_tmp270=" --> ";_tag_dyneither(_tmp270,sizeof(char),6U);});Cyc_fprintf(_tmp336,_tmp335,_tag_dyneither(_tmp26F,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp271=0U;({struct Cyc___cycFILE*_tmp338=Cyc_stderr;struct _dyneither_ptr _tmp337=({const char*_tmp272="\n";_tag_dyneither(_tmp272,sizeof(char),2U);});Cyc_fprintf(_tmp338,_tmp337,_tag_dyneither(_tmp271,sizeof(void*),0U));});});}
# 1118
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1122
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp273=f;struct Cyc_Dict_Dict _tmp27D;struct Cyc_List_List*_tmp27C;if((_tmp273.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp274=0U;({struct Cyc___cycFILE*_tmp33A=Cyc_stderr;struct _dyneither_ptr _tmp339=({const char*_tmp275="  BottomFL\n";_tag_dyneither(_tmp275,sizeof(char),12U);});Cyc_fprintf(_tmp33A,_tmp339,_tag_dyneither(_tmp274,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp27D=((_tmp273.ReachableFL).val).f1;_tmp27C=((_tmp273.ReachableFL).val).f2;_LL4:
# 1126
({void*_tmp276=0U;({struct Cyc___cycFILE*_tmp33C=Cyc_stderr;struct _dyneither_ptr _tmp33B=({const char*_tmp277="  ReachableFL:\n";_tag_dyneither(_tmp277,sizeof(char),16U);});Cyc_fprintf(_tmp33C,_tmp33B,_tag_dyneither(_tmp276,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_flowdict(_tmp27D);
({void*_tmp278=0U;({struct Cyc___cycFILE*_tmp33E=Cyc_stderr;struct _dyneither_ptr _tmp33D=({const char*_tmp279="\n  Relations: ";_tag_dyneither(_tmp279,sizeof(char),15U);});Cyc_fprintf(_tmp33E,_tmp33D,_tag_dyneither(_tmp278,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp27C);
({void*_tmp27A=0U;({struct Cyc___cycFILE*_tmp340=Cyc_stderr;struct _dyneither_ptr _tmp33F=({const char*_tmp27B="\n";_tag_dyneither(_tmp27B,sizeof(char),2U);});Cyc_fprintf(_tmp340,_tmp33F,_tag_dyneither(_tmp27A,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}

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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 121 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1167
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1169
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
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 63
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 65
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 70
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 79
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 85
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 90
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 98
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple10{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple10 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 131 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 149
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 152
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 155
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 158
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple11{void*f1;struct Cyc_List_List*f2;};
struct _tuple11 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 166
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 171
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 191 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
# 194
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 198
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple12 pr1,struct _tuple12 pr2);
# 204
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 33 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 36
Cyc_CfFlowInfo_anal_error=1;
Cyc_Position_post_error(({unsigned int _tmp289=loc;Cyc_Position_mk_err(_tmp289,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 49 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
# 51
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp1;(_tmp1.PlaceL).val=x;(_tmp1.PlaceL).tag=1;_tmp1;});}
# 54
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp2;(_tmp2.UnknownL).val=0;(_tmp2.UnknownL).tag=2;_tmp2;});}
# 57
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp3;(_tmp3.BottomFL).val=0;(_tmp3.BottomFL).tag=1;_tmp3;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp4;({struct _tuple10 _tmp28A=({struct _tuple10 _tmp5;_tmp5.f1=fd;_tmp5.f2=r;_tmp5;});(_tmp4.ReachableFL).val=_tmp28A;});(_tmp4.ReachableFL).tag=2;_tmp4;});}
# 63
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->r=r;({void*_tmp28C=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=_region_malloc(r,sizeof(*_tmp1A));({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp28B=({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp1B;_tmp1B.tag=0;_tmp1B;});_tmp1A[0]=_tmp28B;});_tmp1A;});_tmp6->zero=_tmp28C;});({void*_tmp28E=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp18=_region_malloc(r,sizeof(*_tmp18));({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp28D=({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp19;_tmp19.tag=1;_tmp19;});_tmp18[0]=_tmp28D;});_tmp18;});_tmp6->notzeroall=_tmp28E;});({void*_tmp290=(void*)({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp16=_region_malloc(r,sizeof(*_tmp16));({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp28F=({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp17;_tmp17.tag=2;_tmp17;});_tmp16[0]=_tmp28F;});_tmp16;});_tmp6->notzerothis=_tmp290;});({void*_tmp292=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp14=_region_malloc(r,sizeof(*_tmp14));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp291=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp15;_tmp15.tag=3;_tmp15.f1=Cyc_CfFlowInfo_NoneIL;_tmp15;});_tmp14[0]=_tmp291;});_tmp14;});_tmp6->unknown_none=_tmp292;});({void*_tmp294=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp12=_region_malloc(r,sizeof(*_tmp12));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp293=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp13;_tmp13.tag=3;_tmp13.f1=Cyc_CfFlowInfo_ThisIL;_tmp13;});_tmp12[0]=_tmp293;});_tmp12;});_tmp6->unknown_this=_tmp294;});({void*_tmp296=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10=_region_malloc(r,sizeof(*_tmp10));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp295=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp11;_tmp11.tag=3;_tmp11.f1=Cyc_CfFlowInfo_AllIL;_tmp11;});_tmp10[0]=_tmp295;});_tmp10;});_tmp6->unknown_all=_tmp296;});({void*_tmp298=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpE=_region_malloc(r,sizeof(*_tmpE));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp297=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpF;_tmpF.tag=4;_tmpF.f1=Cyc_CfFlowInfo_NoneIL;_tmpF;});_tmpE[0]=_tmp297;});_tmpE;});_tmp6->esc_none=_tmp298;});({void*_tmp29A=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp299=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpD;_tmpD.tag=4;_tmpD.f1=Cyc_CfFlowInfo_ThisIL;_tmpD;});_tmpC[0]=_tmp299;});_tmpC;});_tmp6->esc_this=_tmp29A;});({void*_tmp29C=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpA=_region_malloc(r,sizeof(*_tmpA));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp29B=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpB;_tmpB.tag=4;_tmpB.f1=Cyc_CfFlowInfo_AllIL;_tmpB;});_tmpA[0]=_tmp29B;});_tmpA;});_tmp6->esc_all=_tmp29C;});({struct Cyc_Dict_Dict _tmp29D=
# 79
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp);_tmp6->mt_flowdict=_tmp29D;});({struct Cyc_CfFlowInfo_Place*_tmp2A0=({struct Cyc_CfFlowInfo_Place*_tmp7=_region_malloc(r,sizeof(*_tmp7));({void*_tmp29F=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8=_region_malloc(r,sizeof(*_tmp8));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp29E=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp9;_tmp9.tag=1;_tmp9.f1=& dummy_exp;_tmp9.f2=(void*)& Cyc_Absyn_VoidType_val;_tmp9;});_tmp8[0]=_tmp29E;});_tmp8;});_tmp7->root=_tmp29F;});_tmp7->fields=0;_tmp7;});_tmp6->dummy_place=_tmp2A0;});_tmp6;});}
# 85
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp1D=*place;struct Cyc_CfFlowInfo_Place _tmp1E=_tmp1D;void*_tmp27;struct Cyc_List_List*_tmp26;_LL1: _tmp27=_tmp1E.root;_tmp26=_tmp1E.fields;_LL2:;{
void*_tmp1F=_tmp27;struct Cyc_Absyn_Vardecl*_tmp25;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp1F)->tag == 0){_LL4: _tmp25=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp1F)->f1;_LL5:
# 89
 if(_tmp26 == 0)
return Cyc_Absynpp_qvar2string(_tmp25->name);else{
# 92
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;({struct _dyneither_ptr _tmp2A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp25->name));_tmp22.f1=_tmp2A1;});({void*_tmp20[1]={& _tmp22};({struct _dyneither_ptr _tmp2A2=({const char*_tmp21="reachable from %s";_tag_dyneither(_tmp21,sizeof(char),18);});Cyc_aprintf(_tmp2A2,_tag_dyneither(_tmp20,sizeof(void*),1));});});});}}else{_LL6: _LL7:
({void*_tmp23=0;({struct _dyneither_ptr _tmp2A3=({const char*_tmp24="error locations not for VarRoots";_tag_dyneither(_tmp24,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp2A3,_tag_dyneither(_tmp23,sizeof(void*),0));});});}_LL3:;};}
# 98
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp28=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp28,f)== 0)return n;
n=n + 1;}
# 105
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp29=_cycalloc(sizeof(*_tmp29));({struct Cyc_Core_Impossible_exn_struct _tmp2A6=({struct Cyc_Core_Impossible_exn_struct _tmp2A;_tmp2A.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2A5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B[1]={& _tmp2D};({struct _dyneither_ptr _tmp2A4=({const char*_tmp2C="get_field_index_fs failed: %s";_tag_dyneither(_tmp2C,sizeof(char),30);});Cyc_aprintf(_tmp2A4,_tag_dyneither(_tmp2B,sizeof(void*),1));});});});_tmp2A.f1=_tmp2A5;});_tmp2A;});_tmp29[0]=_tmp2A6;});_tmp29;}));}
# 108
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp2E=Cyc_Tcutil_compress(t);void*_tmp2F=_tmp2E;struct Cyc_List_List*_tmp37;union Cyc_Absyn_AggrInfoU _tmp36;switch(*((int*)_tmp2F)){case 11: _LL9: _tmp36=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).aggr_info;_LLA: {
# 111
struct Cyc_List_List*_tmp30=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp36))->impl))->fields;
_tmp37=_tmp30;goto _LLC;}case 12: _LLB: _tmp37=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LLC:
# 114
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp37,f);default: _LLD: _LLE:
# 116
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp31=_cycalloc(sizeof(*_tmp31));({struct Cyc_Core_Impossible_exn_struct _tmp2AA=({struct Cyc_Core_Impossible_exn_struct _tmp32;_tmp32.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2A9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0;({struct _dyneither_ptr _tmp2A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp35.f1=_tmp2A7;});({void*_tmp33[1]={& _tmp35};({struct _dyneither_ptr _tmp2A8=({const char*_tmp34="get_field_index failed: %s";_tag_dyneither(_tmp34,sizeof(char),27);});Cyc_aprintf(_tmp2A8,_tag_dyneither(_tmp33,sizeof(void*),1));});});});_tmp32.f1=_tmp2A9;});_tmp32;});_tmp31[0]=_tmp2AA;});_tmp31;}));}_LL8:;}struct _tuple13{void*f1;void*f2;};
# 121
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple13 _tmp38=({struct _tuple13 _tmp40;_tmp40.f1=r1;_tmp40.f2=r2;_tmp40;});struct _tuple13 _tmp39=_tmp38;int _tmp3F;int _tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Vardecl*_tmp3B;struct Cyc_Absyn_Vardecl*_tmp3A;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp39.f1)->tag == 0){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp39.f2)->tag == 0){_LL10: _tmp3B=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp39.f1)->f1;_tmp3A=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp39.f2)->f1;_LL11:
 return(int)_tmp3B - (int)_tmp3A;}else{_LL12: _LL13:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp39.f2)->tag == 0){_LL14: _LL15:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp39.f1)->tag == 1){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp39.f2)->tag == 1){_LL16: _tmp3D=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp39.f1)->f1;_tmp3C=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp39.f2)->f1;_LL17:
 return(int)_tmp3D - (int)_tmp3C;}else{_LL18: _LL19:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp39.f2)->tag == 1){_LL1A: _LL1B:
 return 1;}else{_LL1C: _tmp3F=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp39.f1)->f1;_tmp3E=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp39.f2)->f1;_LL1D:
 return _tmp3F - _tmp3E;}}}}_LLF:;};}
# 135
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 137
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 140
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp41=*p1;struct Cyc_CfFlowInfo_Place _tmp42=_tmp41;void*_tmp48;struct Cyc_List_List*_tmp47;_LL1F: _tmp48=_tmp42.root;_tmp47=_tmp42.fields;_LL20:;{
struct Cyc_CfFlowInfo_Place _tmp43=*p2;struct Cyc_CfFlowInfo_Place _tmp44=_tmp43;void*_tmp46;struct Cyc_List_List*_tmp45;_LL22: _tmp46=_tmp44.root;_tmp45=_tmp44.fields;_LL23:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp48,_tmp46);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp47,_tmp45);};};};}
# 152
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
struct Cyc_CfFlowInfo_Place _tmp49=*p;struct Cyc_CfFlowInfo_Place _tmp4A=_tmp49;void*_tmp6E;struct Cyc_List_List*_tmp6D;_LL25: _tmp6E=_tmp4A.root;_tmp6D=_tmp4A.fields;_LL26:;
{void*_tmp4B=_tmp6E;int _tmp5D;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_Absyn_Vardecl*_tmp5B;switch(*((int*)_tmp4B)){case 0: _LL28: _tmp5B=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4B)->f1;_LL29:
({struct Cyc_List_List*_tmp2AE=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));({struct _dyneither_ptr*_tmp2AD=({struct _dyneither_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _dyneither_ptr _tmp2AC=({struct Cyc_String_pa_PrintArg_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp5B->name).f2);({void*_tmp4E[1]={& _tmp50};({struct _dyneither_ptr _tmp2AB=({const char*_tmp4F="%s";_tag_dyneither(_tmp4F,sizeof(char),3);});Cyc_aprintf(_tmp2AB,_tag_dyneither(_tmp4E,sizeof(void*),1));});});});_tmp4D[0]=_tmp2AC;});_tmp4D;});_tmp4C->hd=_tmp2AD;});_tmp4C->tl=sl;_tmp4C;});sl=_tmp2AE;});goto _LL27;case 1: _LL2A: _tmp5C=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4B)->f1;_LL2B:
({struct Cyc_List_List*_tmp2B2=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({struct _dyneither_ptr*_tmp2B1=({struct _dyneither_ptr*_tmp52=_cycalloc(sizeof(*_tmp52));({struct _dyneither_ptr _tmp2B0=({struct Cyc_Int_pa_PrintArg_struct _tmp55;_tmp55.tag=1;_tmp55.f1=(unsigned long)((int)_tmp5C);({void*_tmp53[1]={& _tmp55};({struct _dyneither_ptr _tmp2AF=({const char*_tmp54="mpt%d";_tag_dyneither(_tmp54,sizeof(char),6);});Cyc_aprintf(_tmp2AF,_tag_dyneither(_tmp53,sizeof(void*),1));});});});_tmp52[0]=_tmp2B0;});_tmp52;});_tmp51->hd=_tmp2B1;});_tmp51->tl=sl;_tmp51;});sl=_tmp2B2;});goto _LL27;default: _LL2C: _tmp5D=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4B)->f1;_LL2D:
({struct Cyc_List_List*_tmp2B6=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));({struct _dyneither_ptr*_tmp2B5=({struct _dyneither_ptr*_tmp57=_cycalloc(sizeof(*_tmp57));({struct _dyneither_ptr _tmp2B4=({struct Cyc_Int_pa_PrintArg_struct _tmp5A;_tmp5A.tag=1;_tmp5A.f1=(unsigned long)_tmp5D;({void*_tmp58[1]={& _tmp5A};({struct _dyneither_ptr _tmp2B3=({const char*_tmp59="param%d";_tag_dyneither(_tmp59,sizeof(char),8);});Cyc_aprintf(_tmp2B3,_tag_dyneither(_tmp58,sizeof(void*),1));});});});_tmp57[0]=_tmp2B4;});_tmp57;});_tmp56->hd=_tmp2B5;});_tmp56->tl=sl;_tmp56;});sl=_tmp2B6;});goto _LL27;}_LL27:;}
# 160
for(0;_tmp6D != 0;_tmp6D=_tmp6D->tl){
({struct Cyc_List_List*_tmp2BA=({struct Cyc_List_List*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct _dyneither_ptr*_tmp2B9=({struct _dyneither_ptr*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct _dyneither_ptr _tmp2B8=({struct Cyc_Int_pa_PrintArg_struct _tmp62;_tmp62.tag=1;_tmp62.f1=(unsigned long)((int)_tmp6D->hd);({void*_tmp60[1]={& _tmp62};({struct _dyneither_ptr _tmp2B7=({const char*_tmp61="%d";_tag_dyneither(_tmp61,sizeof(char),3);});Cyc_aprintf(_tmp2B7,_tag_dyneither(_tmp60,sizeof(void*),1));});});});_tmp5F[0]=_tmp2B8;});_tmp5F;});_tmp5E->hd=_tmp2B9;});_tmp5E->tl=sl;_tmp5E;});sl=_tmp2BA;});}{
struct _dyneither_ptr*_tmp63=({struct _dyneither_ptr*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _dyneither_ptr _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0;({struct _dyneither_ptr _tmp2BB=(struct _dyneither_ptr)({const char*_tmp6C="";_tag_dyneither(_tmp6C,sizeof(char),1);});_tmp6B.f1=_tmp2BB;});({void*_tmp69[1]={& _tmp6B};({struct _dyneither_ptr _tmp2BC=({const char*_tmp6A="%s";_tag_dyneither(_tmp6A,sizeof(char),3);});Cyc_aprintf(_tmp2BC,_tag_dyneither(_tmp69,sizeof(void*),1));});});});_tmp68[0]=_tmp2BD;});_tmp68;});
for(0;sl != 0;sl=sl->tl){
({struct _dyneither_ptr _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp63);({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd));({void*_tmp64[2]={& _tmp66,& _tmp67};({struct _dyneither_ptr _tmp2BE=({const char*_tmp65="%s.%s";_tag_dyneither(_tmp65,sizeof(char),6);});Cyc_aprintf(_tmp2BE,_tag_dyneither(_tmp64,sizeof(void*),2));});});});});*_tmp63=_tmp2BF;});}
return _tmp63;};}
# 178 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 183
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 189
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _dyneither_ptr d=({unsigned int _tmp73=sz;void**_tmp74=(void**)({struct _RegionHandle*_tmp2C0=fenv->r;_region_malloc(_tmp2C0,_check_times(sizeof(void*),_tmp73));});struct _dyneither_ptr _tmp76=_tag_dyneither(_tmp74,sizeof(void*),_tmp73);{unsigned int _tmp75=_tmp73;unsigned int i;for(i=0;i < _tmp75;i ++){_tmp74[i]=(void*)fenv->unknown_all;}}_tmp76;});
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp6F=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp70=_tmp6F;struct _dyneither_ptr*_tmp72;void*_tmp71;_LL2F: _tmp72=_tmp70->name;_tmp71=_tmp70->type;_LL30:;
if(_get_dyneither_size(*_tmp72,sizeof(char))!= 1)
({void*_tmp2C1=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp71,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp2C1;});}}
# 197
return d;}struct _tuple14{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 201
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
# 205
struct _tuple14*_tmp77=env;struct _RegionHandle*_tmp7D;struct Cyc_List_List*_tmp7C;_LL32: _tmp7D=_tmp77->f1;_tmp7C=_tmp77->f2;_LL33:;{
void*_tmp78=Cyc_Tcutil_rsubstitute(_tmp7D,_tmp7C,f->type);
struct Cyc_Absyn_Exp*_tmp79=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp7A=_tmp79 == 0?0: Cyc_Tcutil_rsubsexp(_tmp7D,_tmp7C,_tmp79);
return({struct Cyc_Absyn_Aggrfield*_tmp7B=_region_malloc(_tmp7D,sizeof(*_tmp7B));_tmp7B->name=f->name;_tmp7B->tq=f->tq;_tmp7B->type=_tmp78;_tmp7B->width=f->width;_tmp7B->attributes=f->attributes;_tmp7B->requires_clause=_tmp7A;_tmp7B;});};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 216
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 222
struct _RegionHandle _tmp7E=_new_region("temp");struct _RegionHandle*temp=& _tmp7E;_push_region(temp);
# 226
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp7F=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp80=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp81=Cyc_Tcutil_tvar_kind(_tmp7F,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp82=_tmp81;switch(((struct Cyc_Absyn_Kind*)_tmp82)->kind){case Cyc_Absyn_RgnKind: _LL35: _LL36:
 goto _LL38;case Cyc_Absyn_EffKind: _LL37: _LL38:
 continue;default: _LL39: _LL3A:
 goto _LL34;}_LL34:;}
# 235
({struct Cyc_List_List*_tmp2C3=({struct Cyc_List_List*_tmp83=_region_malloc(temp,sizeof(*_tmp83));({struct _tuple15*_tmp2C2=({struct _tuple15*_tmp84=_region_malloc(temp,sizeof(*_tmp84));_tmp84->f1=_tmp7F;_tmp84->f2=_tmp80;_tmp84;});_tmp83->hd=_tmp2C2;});_tmp83->tl=inst;_tmp83;});inst=_tmp2C3;});}
# 237
if(inst != 0){
struct _tuple14 _tmp85=({struct _tuple14 _tmp87;_tmp87.f1=temp;({struct Cyc_List_List*_tmp2C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp87.f2=_tmp2C4;});_tmp87;});
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp85,fs);
struct _dyneither_ptr _tmp86=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp86;}else{
# 242
struct _dyneither_ptr _tmp88=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp88;}}
# 226
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 246
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 252
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 254
{void*_tmp89=Cyc_Tcutil_compress(t);void*_tmp8A=_tmp89;union Cyc_Absyn_Constraint*_tmpA4;void*_tmpA3;void*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;enum Cyc_Absyn_AggrKind _tmpA0;struct Cyc_List_List*_tmp9F;union Cyc_Absyn_AggrInfoU _tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Datatypefield*_tmp9B;switch(*((int*)_tmp8A)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp8A)->f1).field_info).KnownDatatypefield).tag == 2){_LL3C: _tmp9B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp8A)->f1).field_info).KnownDatatypefield).val).f2;_LL3D:
# 256
 if(_tmp9B->typs == 0)
return leafval;
_tmp9C=_tmp9B->typs;goto _LL3F;}else{goto _LL4A;}case 10: _LL3E: _tmp9C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8A)->f1;_LL3F: {
# 260
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9C);
# 263
struct _dyneither_ptr d=({unsigned int _tmp8E=sz;void**_tmp8F=(void**)({struct _RegionHandle*_tmp2C5=fenv->r;_region_malloc(_tmp2C5,_check_times(sizeof(void*),_tmp8E));});struct _dyneither_ptr _tmp91=_tag_dyneither(_tmp8F,sizeof(void*),_tmp8E);{unsigned int _tmp90=_tmp8E;unsigned int i;for(i=0;i < _tmp90;i ++){_tmp8F[i]=(void*)fenv->unknown_all;}}_tmp91;});
{int i=0;for(0;i < sz;++ i){
({void*_tmp2C6=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp9C))->hd)).f2,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp2C6;});
_tmp9C=_tmp9C->tl;}}
# 268
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8B=_region_malloc(fenv->r,sizeof(*_tmp8B));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2C8=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8C;_tmp8C.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2C7=({struct Cyc_CfFlowInfo_UnionRInfo _tmp8D;_tmp8D.is_union=0;_tmp8D.fieldnum=- 1;_tmp8D;});_tmp8C.f1=_tmp2C7;});_tmp8C.f2=d;_tmp8C;});_tmp8B[0]=_tmp2C8;});_tmp8B;});}case 11: _LL40: _tmp9E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp8A)->f1).aggr_info;_tmp9D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp8A)->f1).targs;_LL41: {
# 270
struct Cyc_Absyn_Aggrdecl*_tmp92=Cyc_Absyn_get_known_aggrdecl(_tmp9E);
if(_tmp92->impl == 0)
goto _LL3B;{
struct Cyc_List_List*_tmp93=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp92->impl))->fields;
if(_tmp9D == 0){_tmpA0=_tmp92->kind;_tmp9F=_tmp93;goto _LL43;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp94=_region_malloc(fenv->r,sizeof(*_tmp94));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2CB=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp95;_tmp95.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2C9=({struct Cyc_CfFlowInfo_UnionRInfo _tmp96;_tmp96.is_union=_tmp92->kind == Cyc_Absyn_UnionA;_tmp96.fieldnum=- 1;_tmp96;});_tmp95.f1=_tmp2C9;});({struct _dyneither_ptr _tmp2CA=
Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmp92->tvs,_tmp9D,_tmp93,_tmp92->kind == Cyc_Absyn_UnionA,leafval);_tmp95.f2=_tmp2CA;});_tmp95;});_tmp94[0]=_tmp2CB;});_tmp94;});};}case 12: _LL42: _tmpA0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8A)->f1;_tmp9F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8A)->f2;_LL43:
# 281
 return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp97=_region_malloc(fenv->r,sizeof(*_tmp97));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2CE=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp98;_tmp98.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2CC=({struct Cyc_CfFlowInfo_UnionRInfo _tmp99;_tmp99.is_union=_tmpA0 == Cyc_Absyn_UnionA;_tmp99.fieldnum=- 1;_tmp99;});_tmp98.f1=_tmp2CC;});({struct _dyneither_ptr _tmp2CD=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp9F,_tmpA0 == Cyc_Absyn_UnionA,leafval);_tmp98.f2=_tmp2CD;});_tmp98;});_tmp97[0]=_tmp2CE;});_tmp97;});case 8: _LL44: _tmpA2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8A)->f1).elt_type;_tmpA1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8A)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA1)){_LL45:
# 285
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpA2)?fenv->unknown_all: leafval;}else{goto _LL4A;}case 19: _LL46: _tmpA3=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp8A)->f1;_LL47:
 return leafval;case 5: _LL48: _tmpA4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A)->f1).ptr_atts).nullable;if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA4)){_LL49:
{void*_tmp9A=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp9A)->tag == 3)switch(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp9A)->f1){case Cyc_CfFlowInfo_ThisIL: _LL4D: _LL4E:
 return fenv->notzerothis;case Cyc_CfFlowInfo_AllIL: _LL4F: _LL50:
 return fenv->notzeroall;default: goto _LL51;}else{_LL51: _LL52:
 goto _LL4C;}_LL4C:;}
# 293
goto _LL3B;}else{goto _LL4A;}default: _LL4A: _LL4B:
 goto _LL3B;}_LL3B:;}
# 297
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 300
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 306
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpA5=r;void*_tmpAD;int _tmpAC;int _tmpAB;struct _dyneither_ptr _tmpAA;struct Cyc_Absyn_Exp*_tmpA9;int _tmpA8;void*_tmpA7;switch(*((int*)_tmpA5)){case 7: _LL54: _tmpA9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f1;_tmpA8=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f2;_tmpA7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f3;_LL55:
# 309
 if(_tmpA9 == e  && _tmpA8 == env_iteration){
*needs_unconsume=1;
return 0;}
# 313
return 1;case 6: _LL56: _tmpAC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f1).is_union;_tmpAB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f1).fieldnum;_tmpAA=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f2;_LL57:
# 315
 if(!_tmpAC  || _tmpAB == - 1){
unsigned int _tmpA6=_get_dyneither_size(_tmpAA,sizeof(void*));
{int i=0;for(0;i < _tmpA6;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmpAA.curr)[i],needs_unconsume))
return 1;}}
# 321
return 0;}else{
# 324
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpAA,sizeof(void*),_tmpAB)),needs_unconsume);}case 8: _LL58: _tmpAD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA5)->f2;_LL59:
# 326
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpAD,needs_unconsume);default: _LL5A: _LL5B:
 return 0;}_LL53:;}
# 333
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpAE=r;struct Cyc_Absyn_Vardecl*_tmpBF;void*_tmpBE;struct Cyc_CfFlowInfo_UnionRInfo _tmpBD;struct _dyneither_ptr _tmpBC;struct Cyc_Absyn_Exp*_tmpBB;int _tmpBA;void*_tmpB9;switch(*((int*)_tmpAE)){case 7: _LL5D: _tmpBB=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f1;_tmpBA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f2;_tmpB9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f3;_LL5E:
# 336
 return _tmpB9;case 6: _LL5F: _tmpBD=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f1;_tmpBC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f2;_LL60: {
# 338
unsigned int _tmpAF=_get_dyneither_size(_tmpBC,sizeof(void*));
int change=0;
struct _dyneither_ptr d2=({unsigned int _tmpB2=_tmpAF;void**_tmpB3=(void**)({struct _RegionHandle*_tmp2CF=fenv->r;_region_malloc(_tmp2CF,_check_times(sizeof(void*),_tmpB2));});struct _dyneither_ptr _tmpB5=_tag_dyneither(_tmpB3,sizeof(void*),_tmpB2);{unsigned int _tmpB4=_tmpB2;unsigned int i;for(i=0;i < _tmpB4;i ++){_tmpB3[i]=(void*)((void**)_tmpBC.curr)[(int)i];}}_tmpB5;});
{int i=0;for(0;i < _tmpAF;++ i){
({void*_tmp2D0=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmpBC.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2D0;});
if(((void**)d2.curr)[i]!= ((void**)_tmpBC.curr)[i])
change=1;}}
# 346
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpB0=_region_malloc(fenv->r,sizeof(*_tmpB0));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2D1=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpB1;_tmpB1.tag=6;_tmpB1.f1=_tmpBD;_tmpB1.f2=d2;_tmpB1;});_tmpB0[0]=_tmp2D1;});_tmpB0;});else{
return r;}}case 8: _LL61: _tmpBF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f1;_tmpBE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpAE)->f2;_LL62: {
# 350
void*_tmpB6=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpBE);
if(_tmpB6 != _tmpBE)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpB7=_region_malloc(fenv->r,sizeof(*_tmpB7));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2D2=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpB8;_tmpB8.tag=8;_tmpB8.f1=_tmpBF;_tmpB8.f2=_tmpB6;_tmpB8;});_tmpB7[0]=_tmp2D2;});_tmpB7;});else{
return r;}}default: _LL63: _LL64:
 return r;}_LL5C:;}
# 360
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple13 _tmpC0=({struct _tuple13 _tmpE5;({void*_tmp2D3=Cyc_Tcutil_compress(t);_tmpE5.f1=_tmp2D3;});_tmpE5.f2=r;_tmpE5;});struct _tuple13 _tmpC1=_tmpC0;enum Cyc_Absyn_AggrKind _tmpE4;struct Cyc_List_List*_tmpE3;struct Cyc_CfFlowInfo_UnionRInfo _tmpE2;struct _dyneither_ptr _tmpE1;union Cyc_Absyn_AggrInfoU _tmpE0;struct Cyc_CfFlowInfo_UnionRInfo _tmpDF;struct _dyneither_ptr _tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_CfFlowInfo_UnionRInfo _tmpDC;struct _dyneither_ptr _tmpDB;struct Cyc_Absyn_Vardecl*_tmpDA;void*_tmpD9;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->tag == 8){_LL66: _tmpDA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f1;_tmpD9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f2;_LL67: {
# 363
void*_tmpC2=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpD9);
if(_tmpC2 != _tmpD9)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC3=_region_malloc(fenv->r,sizeof(*_tmpC3));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2D4=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpC4;_tmpC4.tag=8;_tmpC4.f1=_tmpDA;_tmpC4.f2=_tmpC2;_tmpC4;});_tmpC3[0]=_tmp2D4;});_tmpC3;});else{
return r;}}}else{switch(*((int*)_tmpC1.f1)){case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->tag == 6){_LL68: _tmpDD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC1.f1)->f1;_tmpDC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f1;_tmpDB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f2;_LL69: {
# 367
unsigned int _tmpC5=_get_dyneither_size(_tmpDB,sizeof(void*));
struct _dyneither_ptr d2=({unsigned int _tmpC8=_tmpC5;void**_tmpC9=(void**)({struct _RegionHandle*_tmp2D5=fenv->r;_region_malloc(_tmp2D5,_check_times(sizeof(void*),_tmpC8));});struct _dyneither_ptr _tmpCB=_tag_dyneither(_tmpC9,sizeof(void*),_tmpC8);{unsigned int _tmpCA=_tmpC8;unsigned int i;for(i=0;i < _tmpCA;i ++){_tmpC9[i]=(void*)fenv->unknown_all;}}_tmpCB;});
{int i=0;for(0;i < _tmpC5;++ i){
({void*_tmp2D6=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpDD))->hd)).f2,consumer,iteration,((void**)_tmpDB.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2D6;});
_tmpDD=_tmpDD->tl;}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpC6=_region_malloc(fenv->r,sizeof(*_tmpC6));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2D7=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpC7;_tmpC7.tag=6;_tmpC7.f1=_tmpDC;_tmpC7.f2=d2;_tmpC7;});_tmpC6[0]=_tmp2D7;});_tmpC6;});}}else{goto _LL6E;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->tag == 6){_LL6A: _tmpE0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1.f1)->f1).aggr_info;_tmpDF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f1;_tmpDE=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f2;_LL6B: {
# 375
struct Cyc_Absyn_Aggrdecl*_tmpCC=Cyc_Absyn_get_known_aggrdecl(_tmpE0);
if(_tmpCC->impl == 0)return r;
_tmpE4=_tmpCC->kind;_tmpE3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCC->impl))->fields;_tmpE2=_tmpDF;_tmpE1=_tmpDE;goto _LL6D;}}else{goto _LL6E;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->tag == 6){_LL6C: _tmpE4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC1.f1)->f1;_tmpE3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC1.f1)->f2;_tmpE2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f1;_tmpE1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC1.f2)->f2;_LL6D: {
# 379
struct _dyneither_ptr d2=({unsigned int _tmpD3=_get_dyneither_size(_tmpE1,sizeof(void*));void**_tmpD4=(void**)({struct _RegionHandle*_tmp2D8=fenv->r;_region_malloc(_tmp2D8,_check_times(sizeof(void*),_tmpD3));});struct _dyneither_ptr _tmpD6=_tag_dyneither(_tmpD4,sizeof(void*),_tmpD3);{unsigned int _tmpD5=_tmpD3;unsigned int i;for(i=0;i < _tmpD5;i ++){_tmpD4[i]=(void*)((void**)_tmpE1.curr)[(int)i];}}_tmpD6;});
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE3);
{int i=0;for(0;i < sz;(i ++,_tmpE3=_tmpE3->tl)){
struct Cyc_Absyn_Aggrfield*_tmpCD=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpE3))->hd;struct Cyc_Absyn_Aggrfield*_tmpCE=_tmpCD;struct _dyneither_ptr*_tmpD0;void*_tmpCF;_LL71: _tmpD0=_tmpCE->name;_tmpCF=_tmpCE->type;_LL72:;
if(_get_dyneither_size(*_tmpD0,sizeof(char))!= 1)
({void*_tmp2D9=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpCF,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpE1,sizeof(void*),i)));*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp2D9;});}}
# 386
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD1=_region_malloc(fenv->r,sizeof(*_tmpD1));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2DA=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpD2;_tmpD2.tag=6;_tmpD2.f1=_tmpE2;_tmpD2.f2=d2;_tmpD2;});_tmpD1[0]=_tmp2DA;});_tmpD1;});}}else{goto _LL6E;}default: _LL6E: _LL6F:
# 388
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD7=_region_malloc(fenv->r,sizeof(*_tmpD7));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2DB=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpD8;_tmpD8.tag=7;_tmpD8.f1=consumer;_tmpD8.f2=iteration;_tmpD8.f3=r;_tmpD8;});_tmpD7[0]=_tmp2DB;});_tmpD7;});else{
return r;}}}_LL65:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 394
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 397
struct _RegionHandle _tmpE6=_new_region("r");struct _RegionHandle*r=& _tmpE6;_push_region(r);
{struct _tuple17 elem=({struct _tuple17 _tmpF2;_tmpF2.f1=place;_tmpF2.f2=0;_tmpF2;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmpE7=elem.f1;
struct Cyc_CfFlowInfo_Place _tmpE8=*place;struct Cyc_CfFlowInfo_Place _tmpE9=_tmpE8;void*_tmpF0;struct Cyc_List_List*_tmpEF;_LL74: _tmpF0=_tmpE9.root;_tmpEF=_tmpE9.fields;_LL75:;{
struct Cyc_CfFlowInfo_Place _tmpEA=*_tmpE7;struct Cyc_CfFlowInfo_Place _tmpEB=_tmpEA;void*_tmpEE;struct Cyc_List_List*_tmpED;_LL77: _tmpEE=_tmpEB.root;_tmpED=_tmpEB.fields;_LL78:;
if(Cyc_CfFlowInfo_root_cmp(_tmpF0,_tmpEE)!= 0)
continue;
for(0;_tmpEF != 0  && _tmpED != 0;(_tmpEF=_tmpEF->tl,_tmpED=_tmpED->tl)){
if((int)_tmpEF->hd != (int)_tmpED->hd)break;}
if(_tmpEF == 0){
int _tmpEC=1;_npop_handler(0);return _tmpEC;}};}{
# 411
int _tmpF1=0;_npop_handler(0);return _tmpF1;};}
# 398
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 424
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 426
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 429
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp2DC=({struct Cyc_List_List*_tmpF3=_region_malloc(pile->rgn,sizeof(*_tmpF3));_tmpF3->hd=place;_tmpF3->tl=pile->places;_tmpF3;});pile->places=_tmp2DC;});}
# 432
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmpF4=r;struct Cyc_CfFlowInfo_UnionRInfo _tmpF9;struct _dyneither_ptr _tmpF8;struct Cyc_CfFlowInfo_Place*_tmpF7;void*_tmpF6;void*_tmpF5;switch(*((int*)_tmpF4)){case 7: _LL7A: _tmpF5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF4)->f3;_LL7B:
 Cyc_CfFlowInfo_add_places(pile,_tmpF5);return;case 8: _LL7C: _tmpF6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF4)->f2;_LL7D:
 Cyc_CfFlowInfo_add_places(pile,_tmpF6);return;case 5: _LL7E: _tmpF7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpF4)->f1;_LL7F:
 Cyc_CfFlowInfo_add_place(pile,_tmpF7);return;case 6: _LL80: _tmpF9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF4)->f1;_tmpF8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF4)->f2;_LL81:
# 438
{int i=0;for(0;i < _get_dyneither_size(_tmpF8,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmpF8.curr)[i]);}}
return;default: _LL82: _LL83:
 return;}_LL79:;}
# 448
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 450
void*_tmpFA=old_val;struct Cyc_Absyn_Vardecl*_tmp10C;void*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;int _tmp109;void*_tmp108;int _tmp107;struct _dyneither_ptr _tmp106;switch(*((int*)_tmpFA)){case 6: _LL85: _tmp107=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f1).is_union;_tmp106=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f2;_LL86: {
# 452
struct _dyneither_ptr d2=({unsigned int _tmpFE=_get_dyneither_size(_tmp106,sizeof(void*));void**_tmpFF=(void**)({struct _RegionHandle*_tmp2DD=r;_region_malloc(_tmp2DD,_check_times(sizeof(void*),_tmpFE));});struct _dyneither_ptr _tmp101=_tag_dyneither(_tmpFF,sizeof(void*),_tmpFE);{unsigned int _tmp100=_tmpFE;unsigned int i;for(i=0;i < _tmp100;i ++){({typeof((void*)
Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmp106.curr)[(int)i]))_tmp2DE=(void*)Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmp106.curr)[(int)i]);_tmpFF[i]=_tmp2DE;});}}_tmp101;});
# 456
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpFB=_region_malloc(r,sizeof(*_tmpFB));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E0=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpFC;_tmpFC.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2DF=({struct Cyc_CfFlowInfo_UnionRInfo _tmpFD;_tmpFD.is_union=_tmp107;_tmpFD.fieldnum=- 1;_tmpFD;});_tmpFC.f1=_tmp2DF;});_tmpFC.f2=d2;_tmpFC;});_tmpFB[0]=_tmp2E0;});_tmpFB;});}case 7: _LL87: _tmp10A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f1;_tmp109=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f2;_tmp108=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f3;_LL88:
# 458
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp102=_region_malloc(r,sizeof(*_tmp102));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp2E2=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp103;_tmp103.tag=7;_tmp103.f1=_tmp10A;_tmp103.f2=_tmp109;({void*_tmp2E1=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp108);_tmp103.f3=_tmp2E1;});_tmp103;});_tmp102[0]=_tmp2E2;});_tmp102;});case 8: _LL89: _tmp10C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f1;_tmp10B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFA)->f2;_LL8A:
# 460
 return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp104=_region_malloc(r,sizeof(*_tmp104));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2E4=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp105;_tmp105.tag=8;_tmp105.f1=_tmp10C;({void*_tmp2E3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp10B);_tmp105.f2=_tmp2E3;});_tmp105;});_tmp104[0]=_tmp2E4;});_tmp104;});default: _LL8B: _LL8C:
 return new_val;}_LL84:;}
# 467
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 469
void*_tmp10D=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp10D == rval)return d;{
struct _dyneither_ptr res=({unsigned int _tmp10E=_get_dyneither_size(d,sizeof(void*));void**_tmp10F=(void**)({struct _RegionHandle*_tmp2E5=r;_region_malloc(_tmp2E5,_check_times(sizeof(void*),_tmp10E));});struct _dyneither_ptr _tmp111=_tag_dyneither(_tmp10F,sizeof(void*),_tmp10E);{unsigned int _tmp110=_tmp10E;unsigned int i;for(i=0;i < _tmp110;i ++){_tmp10F[i]=(void*)((void**)d.curr)[(int)i];}}_tmp111;});
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 481
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 485
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple18 _tmp112=({struct _tuple18 _tmp125;_tmp125.f1=fs;_tmp125.f2=old_val;_tmp125;});struct _tuple18 _tmp113=_tmp112;struct Cyc_Absyn_Vardecl*_tmp124;void*_tmp123;int _tmp122;struct Cyc_List_List*_tmp121;int _tmp120;struct _dyneither_ptr _tmp11F;if(_tmp113.f1 != 0)switch(*((int*)_tmp113.f2)){case 6: _LL8E: _tmp122=(int)(_tmp113.f1)->hd;_tmp121=(_tmp113.f1)->tl;_tmp120=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp113.f2)->f1).is_union;_tmp11F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp113.f2)->f2;_LL8F: {
# 489
void*_tmp114=Cyc_CfFlowInfo_insert_place_outer(r,_tmp121,*((void**)_check_dyneither_subscript(_tmp11F,sizeof(void*),_tmp122)),new_val);
struct _dyneither_ptr _tmp115=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp11F,_tmp122,_tmp114);
if((void**)_tmp115.curr == (void**)_tmp11F.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp116=_region_malloc(r,sizeof(*_tmp116));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E7=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp117;_tmp117.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E6=({struct Cyc_CfFlowInfo_UnionRInfo _tmp118;_tmp118.is_union=_tmp120;_tmp118.fieldnum=- 1;_tmp118;});_tmp117.f1=_tmp2E6;});_tmp117.f2=_tmp115;_tmp117;});_tmp116[0]=_tmp2E7;});_tmp116;});}}case 8: goto _LL90;default: goto _LL92;}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp113.f2)->tag == 8){_LL90: _tmp124=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp113.f2)->f1;_tmp123=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp113.f2)->f2;_LL91: {
# 494
void*_tmp119=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp123,new_val);
if(_tmp119 == _tmp123)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11A=_region_malloc(r,sizeof(*_tmp11A));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2E8=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11B;_tmp11B.tag=8;_tmp11B.f1=_tmp124;_tmp11B.f2=_tmp119;_tmp11B;});_tmp11A[0]=_tmp2E8;});_tmp11A;});}}}else{_LL92: _LL93:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_Core_Impossible_exn_struct _tmp2EA=({struct Cyc_Core_Impossible_exn_struct _tmp11D;_tmp11D.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2E9=({const char*_tmp11E="bad insert place";_tag_dyneither(_tmp11E,sizeof(char),17);});_tmp11D.f1=_tmp2E9;});_tmp11D;});_tmp11C[0]=_tmp2EA;});_tmp11C;}));}}_LL8D:;};}
# 504
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
# 507
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp126=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmp127;_push_handler(& _tmp127);{int _tmp129=0;if(setjmp(_tmp127.handler))_tmp129=1;if(!_tmp129){({void*_tmp2EB=Cyc_CfFlowInfo_lookup_place(d,_tmp126);oldval=_tmp2EB;});;_pop_handler();}else{void*_tmp128=(void*)_exn_thrown;void*_tmp12A=_tmp128;void*_tmp12B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp12A)->tag == Cyc_Dict_Absent){_LL95: _LL96:
 continue;}else{_LL97: _tmp12B=_tmp12A;_LL98:(int)_rethrow(_tmp12B);}_LL94:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmp12C=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _tmp12D=_tmp12C;switch(_tmp12D){case Cyc_CfFlowInfo_AllIL: _LL9A: _LL9B:
 newval=fenv->esc_all;goto _LL99;case Cyc_CfFlowInfo_ThisIL: _LL9C: _LL9D:
 newval=fenv->esc_this;goto _LL99;default: _LL9E: _LL9F:
 newval=fenv->esc_none;goto _LL99;}_LL99:;}
# 518
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp12E=*_tmp126;struct Cyc_CfFlowInfo_Place _tmp12F=_tmp12E;void*_tmp131;struct Cyc_List_List*_tmp130;_LLA1: _tmp131=_tmp12F.root;_tmp130=_tmp12F.fields;_LLA2:;
({struct Cyc_Dict_Dict _tmp2F1=({struct Cyc_Dict_Dict _tmp2F0=d;void*_tmp2EF=_tmp131;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2F0,_tmp2EF,({
struct _RegionHandle*_tmp2EE=fenv->r;struct Cyc_List_List*_tmp2ED=_tmp130;void*_tmp2EC=
# 523
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp131);
# 521
Cyc_CfFlowInfo_insert_place_outer(_tmp2EE,_tmp2ED,_tmp2EC,newval);}));});
# 520
d=_tmp2F1;});};};}
# 526
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 538
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp132=r;void*_tmp138;enum Cyc_CfFlowInfo_InitLevel _tmp137;enum Cyc_CfFlowInfo_InitLevel _tmp136;switch(*((int*)_tmp132)){case 3: _LLA4: _tmp136=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp132)->f1;_LLA5:
 return _tmp136;case 4: _LLA6: _tmp137=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp132)->f1;_LLA7:
 return _tmp137;case 0: _LLA8: _LLA9:
 goto _LLAB;case 1: _LLAA: _LLAB:
 return Cyc_CfFlowInfo_AllIL;case 2: _LLAC: _LLAD:
 return Cyc_CfFlowInfo_ThisIL;case 7: _LLAE: _tmp138=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp132)->f3;_LLAF:
 return Cyc_CfFlowInfo_NoneIL;default: _LLB0: _LLB1:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_Core_Impossible_exn_struct _tmp2F3=({struct Cyc_Core_Impossible_exn_struct _tmp134;_tmp134.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2F2=({const char*_tmp135="initlevel_approx";_tag_dyneither(_tmp135,sizeof(char),17);});_tmp134.f1=_tmp2F2;});_tmp134;});_tmp133[0]=_tmp2F3;});_tmp133;}));}_LLA3:;}
# 549
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 551
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp139=r;struct Cyc_CfFlowInfo_Place*_tmp142;struct _dyneither_ptr _tmp141;int _tmp140;int _tmp13F;struct _dyneither_ptr _tmp13E;void*_tmp13D;switch(*((int*)_tmp139)){case 8: _LLB3: _tmp13D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp139)->f2;_LLB4:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp13D,acc);case 6: _LLB5: _tmp140=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp139)->f1).is_union;_tmp13F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp139)->f1).fieldnum;_tmp13E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp139)->f2;if(_tmp140){_LLB6: {
# 559
unsigned int _tmp13A=_get_dyneither_size(_tmp13E,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp13F == - 1){
int i=0;for(0;i < _tmp13A;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp13E.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 569
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp13E,sizeof(void*),_tmp13F)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LLB2;}}else{_LLB7: _tmp141=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp139)->f2;_LLB8: {
# 573
unsigned int _tmp13B=_get_dyneither_size(_tmp141,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp13B;++ i){
({enum Cyc_CfFlowInfo_InitLevel _tmp2F4=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp141.curr)[i],this_ans);this_ans=_tmp2F4;});}}
goto _LLB2;}}case 5: _LLB9: _tmp142=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp139)->f1;_LLBA:
# 579
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp142))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 582
({struct Cyc_List_List*_tmp2F5=({struct Cyc_List_List*_tmp13C=_region_malloc(env->e,sizeof(*_tmp13C));_tmp13C->hd=_tmp142;_tmp13C->tl=env->seen;_tmp13C;});env->seen=_tmp2F5;});
({enum Cyc_CfFlowInfo_InitLevel _tmp2F7=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp2F6=env;Cyc_CfFlowInfo_initlevel_rec(_tmp2F6,Cyc_CfFlowInfo_lookup_place(env->d,_tmp142),Cyc_CfFlowInfo_AllIL);});this_ans=_tmp2F7;});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 588
goto _LLB2;default: _LLBB: _LLBC:
({enum Cyc_CfFlowInfo_InitLevel _tmp2F8=Cyc_CfFlowInfo_initlevel_approx(r);this_ans=_tmp2F8;});}_LLB2:;}
# 591
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 595
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp143=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp144=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp145;_tmp145.e=_tmp143;_tmp145.d=d;_tmp145.seen=0;_tmp145;});
return Cyc_CfFlowInfo_initlevel_rec(& _tmp144,r,Cyc_CfFlowInfo_AllIL);}struct _tuple19{void*f1;int f2;};
# 602
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp146=*place;struct Cyc_CfFlowInfo_Place _tmp147=_tmp146;void*_tmp155;struct Cyc_List_List*_tmp154;_LLBE: _tmp155=_tmp147.root;_tmp154=_tmp147.fields;_LLBF:;{
void*_tmp148=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp155);
for(0;_tmp154 != 0;_tmp154=_tmp154->tl){
retry: {struct _tuple19 _tmp149=({struct _tuple19 _tmp153;_tmp153.f1=_tmp148;_tmp153.f2=(int)_tmp154->hd;_tmp153;});struct _tuple19 _tmp14A=_tmp149;struct Cyc_CfFlowInfo_UnionRInfo _tmp152;struct _dyneither_ptr _tmp151;int _tmp150;void*_tmp14F;int _tmp14E;switch(*((int*)_tmp14A.f1)){case 8: _LLC1: _tmp14F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp14A.f1)->f2;_tmp14E=_tmp14A.f2;_LLC2:
# 608
 _tmp148=_tmp14F;goto retry;case 6: _LLC3: _tmp152=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp14A.f1)->f1;_tmp151=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp14A.f1)->f2;_tmp150=_tmp14A.f2;_LLC4:
# 610
 _tmp148=*((void**)_check_dyneither_subscript(_tmp151,sizeof(void*),_tmp150));
goto _LLC0;default: _LLC5: _LLC6:
# 616
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));({struct Cyc_Core_Impossible_exn_struct _tmp2FA=({struct Cyc_Core_Impossible_exn_struct _tmp14C;_tmp14C.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2F9=({const char*_tmp14D="bad lookup_place";_tag_dyneither(_tmp14D,sizeof(char),17);});_tmp14C.f1=_tmp2F9;});_tmp14C;});_tmp14B[0]=_tmp2FA;});_tmp14B;}));}_LLC0:;}}
# 618
return _tmp148;};}
# 621
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp156=rval;int _tmp15B;int _tmp15A;struct _dyneither_ptr _tmp159;void*_tmp158;void*_tmp157;switch(*((int*)_tmp156)){case 4: _LLC8: _LLC9:
 return 0;case 7: _LLCA: _tmp157=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp156)->f3;_LLCB:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp157);case 8: _LLCC: _tmp158=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp156)->f2;_LLCD:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp158);case 6: _LLCE: _tmp15B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp156)->f1).is_union;_tmp15A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp156)->f1).fieldnum;_tmp159=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp156)->f2;_LLCF:
# 627
 if(_tmp15B  && _tmp15A != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp159,sizeof(void*),_tmp15A)));else{
# 630
unsigned int sz=_get_dyneither_size(_tmp159,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp159.curr)[i]))return 0;}}
return 1;}default: _LLD0: _LLD1:
# 635
 return 1;}_LLC7:;}
# 638
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 643
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp15C=fenv->r;
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp15D=_region_malloc(_tmp15C,sizeof(*_tmp15D));_tmp15D->rgn=_tmp15C;_tmp15D->places=0;_tmp15D;});
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 660
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 665
struct _tuple13 _tmp15E=({struct _tuple13 _tmp175;_tmp175.f1=oldval;_tmp175.f2=newval;_tmp175;});struct _tuple13 _tmp15F=_tmp15E;enum Cyc_CfFlowInfo_InitLevel _tmp174;struct Cyc_CfFlowInfo_UnionRInfo _tmp173;struct _dyneither_ptr _tmp172;struct Cyc_CfFlowInfo_UnionRInfo _tmp171;struct _dyneither_ptr _tmp170;struct Cyc_CfFlowInfo_Place*_tmp16F;struct Cyc_Absyn_Vardecl*_tmp16E;void*_tmp16D;void*_tmp16C;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15F.f1)->tag == 8){_LLD3: _tmp16C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15F.f1)->f2;_LLD4:
# 667
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp16C,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->tag == 8){_LLD5: _tmp16E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->f1;_tmp16D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->f2;_LLD6: {
# 669
void*_tmp160=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp16D);
if(_tmp160 == _tmp16D)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp161=_region_malloc((env->fenv)->r,sizeof(*_tmp161));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp2FB=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp162;_tmp162.tag=8;_tmp162.f1=_tmp16E;_tmp162.f2=_tmp160;_tmp162;});_tmp161[0]=_tmp2FB;});_tmp161;});}}}else{switch(*((int*)_tmp15F.f1)){case 4: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->tag == 5){_LLD7: _tmp16F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->f1;_LLD8:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp16F);goto _LLDA;}else{_LLD9: _LLDA:
# 674
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp163=0;({unsigned int _tmp2FD=env->loc;struct _dyneither_ptr _tmp2FC=({const char*_tmp164="assignment puts possibly-uninitialized data in an escaped location";_tag_dyneither(_tmp164,sizeof(char),67);});Cyc_CfFlowInfo_aerr(_tmp2FD,_tmp2FC,_tag_dyneither(_tmp163,sizeof(void*),0));});});
# 677
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmp15F.f2)){case 6: _LLDB: _tmp173=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp15F.f1)->f1;_tmp172=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp15F.f1)->f2;_tmp171=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->f1;_tmp170=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->f2;_LLDC: {
# 679
struct _dyneither_ptr new_d=({unsigned int _tmp167=
_get_dyneither_size(_tmp172,sizeof(void*));void**_tmp168=(void**)({struct _RegionHandle*_tmp2FE=(env->fenv)->r;_region_malloc(_tmp2FE,_check_times(sizeof(void*),_tmp167));});struct _dyneither_ptr _tmp16A=_tag_dyneither(_tmp168,sizeof(void*),_tmp167);{unsigned int _tmp169=_tmp167;unsigned int i;for(i=0;i < _tmp169;i ++){({typeof((void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp172.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp170,sizeof(void*),(int)i))))_tmp2FF=(void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp172.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp170,sizeof(void*),(int)i)));_tmp168[i]=_tmp2FF;});}}_tmp16A;});
# 683
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp172,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp172.curr)[i]){
change=1;break;}}}
# 688
if(!change){
if(!_tmp173.is_union)return oldval;
new_d=_tmp172;}else{
# 693
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp172,sizeof(void*));++ i){
if(({void*_tmp300=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp300 != *((void**)_check_dyneither_subscript(_tmp170,sizeof(void*),i));})){
change=1;break;}}}
# 698
if(!change){
if(!_tmp173.is_union)return newval;
new_d=_tmp170;}}
# 703
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp165=_region_malloc((env->fenv)->r,sizeof(*_tmp165));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp301=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp166;_tmp166.tag=6;_tmp166.f1=_tmp171;_tmp166.f2=new_d;_tmp166;});_tmp165[0]=_tmp301;});_tmp165;});}case 4: goto _LLDD;default: goto _LLDF;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->tag == 4){_LLDD: _tmp174=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp15F.f2)->f1;_LLDE: {
# 705
enum Cyc_CfFlowInfo_InitLevel _tmp16B=_tmp174;switch(_tmp16B){case Cyc_CfFlowInfo_NoneIL: _LLE2: _LLE3:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLE4: _LLE5:
 return(env->fenv)->unknown_this;default: _LLE6: _LLE7:
 return(env->fenv)->unknown_all;}_LLE1:;}}else{_LLDF: _LLE0:
# 710
 return newval;}}}}_LLD2:;}
# 716
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 720
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 725
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple18 _tmp176=({struct _tuple18 _tmp193;_tmp193.f1=fs;_tmp193.f2=oldval;_tmp193;});struct _tuple18 _tmp177=_tmp176;int _tmp192;struct Cyc_List_List*_tmp191;int _tmp190;int _tmp18F;struct _dyneither_ptr _tmp18E;struct Cyc_Absyn_Vardecl*_tmp18D;void*_tmp18C;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->tag == 8){_LLE9: _tmp18D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->f1;_tmp18C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->f2;_LLEA: {
# 728
void*_tmp178=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp18C,newval);
if(_tmp178 == _tmp18C)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp179=_region_malloc((env->fenv)->r,sizeof(*_tmp179));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp302=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp17A;_tmp17A.tag=8;_tmp17A.f1=_tmp18D;_tmp17A.f2=_tmp178;_tmp17A;});_tmp179[0]=_tmp302;});_tmp179;});}}}else{if(_tmp177.f1 != 0){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->tag == 6){_LLEB: _tmp192=(int)(_tmp177.f1)->hd;_tmp191=(_tmp177.f1)->tl;_tmp190=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->f1).is_union;_tmp18F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->f1).fieldnum;_tmp18E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp177.f2)->f2;_LLEC: {
# 732
void*_tmp17B=Cyc_CfFlowInfo_assign_place_outer(env,_tmp191,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp18E,sizeof(void*),_tmp192)),newval);
# 734
struct _dyneither_ptr _tmp17C=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp18E,_tmp192,_tmp17B);
if((void**)_tmp17C.curr == (void**)_tmp18E.curr  && (!_tmp190  || _tmp18F == _tmp192))return oldval;
_tmp18E=_tmp17C;
# 739
if(_tmp190){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp18E,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp192){
struct _dyneither_ptr _tmp17D=({
struct _RegionHandle*_tmp305=(env->fenv)->r;struct _dyneither_ptr _tmp304=_tmp18E;int _tmp303=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp305,_tmp304,_tmp303,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp18E,sizeof(void*),i))));});
# 749
if((void**)_tmp17D.curr != (void**)_tmp18E.curr){
_tmp18E=_tmp17D;
changed=1;}}}}
# 757
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp17E=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp17F=_tmp17E;void*_tmp185;struct Cyc_List_List*_tmp184;_LLF0: _tmp185=_tmp17F->root;_tmp184=_tmp17F->fields;_LLF1:;{
struct Cyc_List_List*_tmp180=({struct _RegionHandle*_tmp307=(env->pile)->rgn;int*_tmp306=({int*_tmp183=_cycalloc_atomic(sizeof(*_tmp183));_tmp183[0]=fs_prefix;_tmp183;});((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)(_tmp307,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,_tmp306,_tmp184);});
struct Cyc_CfFlowInfo_Place*_tmp181=({struct Cyc_CfFlowInfo_Place*_tmp182=_region_malloc((env->pile)->rgn,sizeof(*_tmp182));_tmp182->root=_tmp185;_tmp182->fields=_tmp180;_tmp182;});;};}}
# 763
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp186=_region_malloc((env->fenv)->r,sizeof(*_tmp186));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp309=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp187;_tmp187.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp308=({struct Cyc_CfFlowInfo_UnionRInfo _tmp188;_tmp188.is_union=_tmp190;_tmp188.fieldnum=_tmp192;_tmp188;});_tmp187.f1=_tmp308;});_tmp187.f2=_tmp18E;_tmp187;});_tmp186[0]=_tmp309;});_tmp186;});}}else{goto _LLED;}}else{_LLED: _LLEE:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp189=_cycalloc(sizeof(*_tmp189));({struct Cyc_Core_Impossible_exn_struct _tmp30B=({struct Cyc_Core_Impossible_exn_struct _tmp18A;_tmp18A.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp30A=({const char*_tmp18B="bad assign place";_tag_dyneither(_tmp18B,sizeof(char),17);});_tmp18A.f1=_tmp30A;});_tmp18A;});_tmp189[0]=_tmp30B;});_tmp189;}));}}_LLE8:;};}
# 767
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 777
struct _RegionHandle*_tmp194=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp195=place;void*_tmp199;struct Cyc_List_List*_tmp198;_LLF3: _tmp199=_tmp195->root;_tmp198=_tmp195->fields;_LLF4:;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp196;_tmp196.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp30C=({struct Cyc_CfFlowInfo_EscPile*_tmp197=_region_malloc(_tmp194,sizeof(*_tmp197));_tmp197->rgn=_tmp194;_tmp197->places=0;_tmp197;});_tmp196.pile=_tmp30C;});_tmp196.d=d;_tmp196.loc=loc;_tmp196.root_place=place;_tmp196;});
void*newval=({struct Cyc_List_List*_tmp30E=_tmp198;void*_tmp30D=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp199);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp30E,0,_tmp30D,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp310=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp30F=env.pile;Cyc_CfFlowInfo_escape_these(_tmp310,_tmp30F,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp199,newval));});};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 792
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
# 794
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple20{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 804 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp19A=(env->fenv)->r;
# 808
{struct _tuple13 _tmp19B=({struct _tuple13 _tmp1D6;_tmp1D6.f1=r1;_tmp1D6.f2=r2;_tmp1D6;});struct _tuple13 _tmp19C=_tmp19B;int _tmp1D5;int _tmp1D4;struct _dyneither_ptr _tmp1D3;int _tmp1D2;int _tmp1D1;struct _dyneither_ptr _tmp1D0;struct Cyc_CfFlowInfo_Place*_tmp1CF;struct Cyc_CfFlowInfo_Place*_tmp1CE;struct Cyc_CfFlowInfo_Place*_tmp1CD;struct Cyc_CfFlowInfo_Place*_tmp1CC;struct Cyc_CfFlowInfo_Place*_tmp1CB;struct Cyc_CfFlowInfo_Place*_tmp1CA;struct Cyc_CfFlowInfo_Place*_tmp1C9;struct Cyc_CfFlowInfo_Place*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;int _tmp1C6;void*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C4;int _tmp1C3;void*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C0;void*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1BE;int _tmp1BD;void*_tmp1BC;struct Cyc_Absyn_Vardecl*_tmp1BB;void*_tmp1BA;struct Cyc_Absyn_Vardecl*_tmp1B9;void*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1B7;void*_tmp1B6;struct Cyc_Absyn_Vardecl*_tmp1B5;void*_tmp1B4;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->tag == 8){_LLF6: _tmp1B7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_tmp1B6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f2;_tmp1B5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_tmp1B4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f2;_LLF7:
# 811
 if(_tmp1B7 == _tmp1B5)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp19D=_region_malloc(_tmp19A,sizeof(*_tmp19D));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp312=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp19E;_tmp19E.tag=8;_tmp19E.f1=_tmp1B7;({void*_tmp311=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B6,_tmp1B4);_tmp19E.f2=_tmp311;});_tmp19E;});_tmp19D[0]=_tmp312;});_tmp19D;});else{
# 814
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B6,_tmp1B4);}}else{_LLF8: _tmp1B9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_tmp1B8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f2;_LLF9:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B8,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->tag == 8){_LLFA: _tmp1BB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_tmp1BA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f2;_LLFB:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1BA);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->tag == 7){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->tag == 7){_LLFC: _tmp1C1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_tmp1C0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f2;_tmp1BF=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f3;_tmp1BE=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_tmp1BD=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f2;_tmp1BC=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f3;_LLFD:
# 819
 if(_tmp1C1 == _tmp1BE)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp19F=_region_malloc(_tmp19A,sizeof(*_tmp19F));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp314=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1A0;_tmp1A0.tag=7;_tmp1A0.f1=_tmp1C1;_tmp1A0.f2=_tmp1C0 > _tmp1BD?_tmp1C0: _tmp1BD;({void*_tmp313=
Cyc_CfFlowInfo_join_absRval(env,a,_tmp1BF,_tmp1BC);_tmp1A0.f3=_tmp313;});_tmp1A0;});_tmp19F[0]=_tmp314;});_tmp19F;});
{void*_tmp1A1=_tmp1BF;struct Cyc_CfFlowInfo_Place*_tmp1A2;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->tag == 5){_LL119: _tmp1A2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;_LL11A:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A2);goto _LL118;}else{_LL11B: _LL11C:
 goto _LL118;}_LL118:;}
# 826
{void*_tmp1A3=_tmp1BC;struct Cyc_CfFlowInfo_Place*_tmp1A4;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A3)->tag == 5){_LL11E: _tmp1A4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A3)->f1;_LL11F:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A4);goto _LL11D;}else{_LL120: _LL121:
 goto _LL11D;}_LL11D:;}
# 830
goto _LLF5;}else{_LLFE: _tmp1C4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_tmp1C3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f2;_tmp1C2=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f3;_LLFF:
# 832
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1A5=_region_malloc(_tmp19A,sizeof(*_tmp1A5));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp316=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1A6;_tmp1A6.tag=7;_tmp1A6.f1=_tmp1C4;_tmp1A6.f2=_tmp1C3;({void*_tmp315=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C2,r2);_tmp1A6.f3=_tmp315;});_tmp1A6;});_tmp1A5[0]=_tmp316;});_tmp1A5;});}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->tag == 7){_LL100: _tmp1C7=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_tmp1C6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f2;_tmp1C5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f3;_LL101:
# 834
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1A7=_region_malloc(_tmp19A,sizeof(*_tmp1A7));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp318=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1A8;_tmp1A8.tag=7;_tmp1A8.f1=_tmp1C7;_tmp1A8.f2=_tmp1C6;({void*_tmp317=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1C5);_tmp1A8.f3=_tmp317;});_tmp1A8;});_tmp1A7[0]=_tmp318;});_tmp1A7;});}else{switch(*((int*)_tmp19C.f1)){case 5: switch(*((int*)_tmp19C.f2)){case 5: _LL102: _tmp1C9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_tmp1C8=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_LL103:
# 837
 if(Cyc_CfFlowInfo_place_cmp(_tmp1C9,_tmp1C8)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1C9);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1C8);
goto _LLF5;case 2: _LL104: _tmp1CA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_LL105:
# 842
 _tmp1CD=_tmp1CA;goto _LL107;case 1: _LL108: _tmp1CB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_LL109:
# 848
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CB);{
enum Cyc_CfFlowInfo_InitLevel _tmp1A9=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp1AA=_tmp1A9;if(_tmp1AA == Cyc_CfFlowInfo_AllIL){_LL123: _LL124:
 return(env->fenv)->notzeroall;}else{_LL125: _LL126:
 return(env->fenv)->notzerothis;}_LL122:;};default: _LL10C: _tmp1CC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1;_LL10D:
# 860
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CC);goto _LLF5;}case 2: switch(*((int*)_tmp19C.f2)){case 5: _LL106: _tmp1CD=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_LL107:
# 844
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CD);
return(env->fenv)->notzerothis;case 1: _LL112: _LL113:
# 865
 return(env->fenv)->notzerothis;default: goto _LL116;}case 1: switch(*((int*)_tmp19C.f2)){case 5: _LL10A: _tmp1CE=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_LL10B:
# 854
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CE);{
enum Cyc_CfFlowInfo_InitLevel _tmp1AB=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp1AC=_tmp1AB;if(_tmp1AC == Cyc_CfFlowInfo_AllIL){_LL128: _LL129:
 return(env->fenv)->notzeroall;}else{_LL12A: _LL12B:
 return(env->fenv)->notzerothis;}_LL127:;};case 2: _LL110: _LL111:
# 864
 goto _LL113;default: goto _LL116;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->tag == 5){_LL10E: _tmp1CF=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1;_LL10F:
# 861
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CF);goto _LLF5;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->tag == 6){_LL114: _tmp1D5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1).is_union;_tmp1D4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f1).fieldnum;_tmp1D3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1)->f2;_tmp1D2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1).is_union;_tmp1D1=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f1).fieldnum;_tmp1D0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2)->f2;_LL115: {
# 868
struct _dyneither_ptr new_d=({unsigned int _tmp1B0=
_get_dyneither_size(_tmp1D3,sizeof(void*));void**_tmp1B1=(void**)({struct _RegionHandle*_tmp319=_tmp19A;_region_malloc(_tmp319,_check_times(sizeof(void*),_tmp1B0));});struct _dyneither_ptr _tmp1B3=_tag_dyneither(_tmp1B1,sizeof(void*),_tmp1B0);{unsigned int _tmp1B2=_tmp1B0;unsigned int i;for(i=0;i < _tmp1B2;i ++){({typeof((void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1D3.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1D0,sizeof(void*),(int)i))))_tmp31A=(void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1D3.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1D0,sizeof(void*),(int)i)));_tmp1B1[i]=_tmp31A;});}}_tmp1B3;});
# 873
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1D3,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1D3.curr)[i]){
change=1;break;}}}
# 878
if(!change){
if(!_tmp1D5)return r1;
new_d=_tmp1D3;}else{
# 883
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1D3,sizeof(void*));++ i){
if(({void*_tmp31B=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp31B != *((void**)_check_dyneither_subscript(_tmp1D0,sizeof(void*),i));})){
change=1;break;}}}
# 888
if(!change){
if(!_tmp1D5)return r2;
new_d=_tmp1D0;}}
# 893
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AD=_region_malloc(_tmp19A,sizeof(*_tmp1AD));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp31D=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp1AE;_tmp1AE.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp31C=({struct Cyc_CfFlowInfo_UnionRInfo _tmp1AF;_tmp1AF.is_union=_tmp1D5;_tmp1AF.fieldnum=_tmp1D4 == _tmp1D1?_tmp1D4: - 1;_tmp1AF;});_tmp1AE.f1=_tmp31C;});_tmp1AE.f2=new_d;_tmp1AE;});_tmp1AD[0]=_tmp31D;});_tmp1AD;});}}else{goto _LL116;}}else{_LL116: _LL117:
# 895
 goto _LLF5;}}}}}}}_LLF5:;}{
# 897
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple13 _tmp1D7=({struct _tuple13 _tmp1DF;_tmp1DF.f1=r1;_tmp1DF.f2=r2;_tmp1DF;});struct _tuple13 _tmp1D8=_tmp1D7;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1D8.f1)->tag == 4){_LL12D: _LL12E:
 goto _LL130;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1D8.f2)->tag == 4){_LL12F: _LL130: {
# 902
struct _tuple20 _tmp1D9=({struct _tuple20 _tmp1DB;_tmp1DB.f1=il1;_tmp1DB.f2=il2;_tmp1DB;});struct _tuple20 _tmp1DA=_tmp1D9;if(_tmp1DA.f2 == Cyc_CfFlowInfo_NoneIL){_LL134: _LL135:
 goto _LL137;}else{if(_tmp1DA.f1 == Cyc_CfFlowInfo_NoneIL){_LL136: _LL137: return(env->fenv)->esc_none;}else{if(_tmp1DA.f2 == Cyc_CfFlowInfo_ThisIL){_LL138: _LL139:
 goto _LL13B;}else{if(_tmp1DA.f1 == Cyc_CfFlowInfo_ThisIL){_LL13A: _LL13B: return(env->fenv)->esc_this;}else{_LL13C: _LL13D:
 return(env->fenv)->esc_all;}}}}_LL133:;}}else{_LL131: _LL132: {
# 908
struct _tuple20 _tmp1DC=({struct _tuple20 _tmp1DE;_tmp1DE.f1=il1;_tmp1DE.f2=il2;_tmp1DE;});struct _tuple20 _tmp1DD=_tmp1DC;if(_tmp1DD.f2 == Cyc_CfFlowInfo_NoneIL){_LL13F: _LL140:
 goto _LL142;}else{if(_tmp1DD.f1 == Cyc_CfFlowInfo_NoneIL){_LL141: _LL142: return(env->fenv)->unknown_none;}else{if(_tmp1DD.f2 == Cyc_CfFlowInfo_ThisIL){_LL143: _LL144:
 goto _LL146;}else{if(_tmp1DD.f1 == Cyc_CfFlowInfo_ThisIL){_LL145: _LL146: return(env->fenv)->unknown_this;}else{_LL147: _LL148:
 return(env->fenv)->unknown_all;}}}}_LL13E:;}}}_LL12C:;};};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 916
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 918
struct _tuple21 _tmp1E0=({struct _tuple21 _tmp1EC;_tmp1EC.f1=f1;_tmp1EC.f2=f2;_tmp1EC;});struct _tuple21 _tmp1E1=_tmp1E0;struct Cyc_Dict_Dict _tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_Dict_Dict _tmp1E9;struct Cyc_List_List*_tmp1E8;if(((_tmp1E1.f1).BottomFL).tag == 1){_LL14A: _LL14B:
 return f2;}else{if(((_tmp1E1.f2).BottomFL).tag == 1){_LL14C: _LL14D:
 return f1;}else{_LL14E: _tmp1EB=(((_tmp1E1.f1).ReachableFL).val).f1;_tmp1EA=(((_tmp1E1.f1).ReachableFL).val).f2;_tmp1E9=(((_tmp1E1.f2).ReachableFL).val).f1;_tmp1E8=(((_tmp1E1.f2).ReachableFL).val).f2;_LL14F:
# 924
 if(_tmp1EB.t == _tmp1E9.t  && _tmp1EA == _tmp1E8)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp1E2=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp1E3=({struct Cyc_CfFlowInfo_JoinEnv _tmp1E6;_tmp1E6.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp31E=({struct Cyc_CfFlowInfo_EscPile*_tmp1E7=_region_malloc(_tmp1E2,sizeof(*_tmp1E7));_tmp1E7->rgn=_tmp1E2;_tmp1E7->places=0;_tmp1E7;});_tmp1E6.pile=_tmp31E;});_tmp1E6.d1=_tmp1EB;_tmp1E6.d2=_tmp1E9;_tmp1E6;});
struct Cyc_Dict_Dict _tmp1E4=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp1E3,_tmp1EB,_tmp1E9);
struct Cyc_List_List*_tmp1E5=Cyc_Relations_join_relns(fenv->r,_tmp1EA,_tmp1E8);
return({struct Cyc_Dict_Dict _tmp31F=Cyc_CfFlowInfo_escape_these(fenv,_tmp1E3.pile,_tmp1E4);Cyc_CfFlowInfo_ReachableFL(_tmp31F,_tmp1E5);});};}}_LL149:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 936
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple12 pr1,struct _tuple12 pr2){
# 942
struct _tuple12 _tmp1ED=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp206;void*_tmp205;_LL151: _tmp206=_tmp1ED.f1;_tmp205=_tmp1ED.f2;_LL152:;{
struct _tuple12 _tmp1EE=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp204;void*_tmp203;_LL154: _tmp204=_tmp1EE.f1;_tmp203=_tmp1EE.f2;_LL155:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,_tmp206,_tmp204);
struct _tuple22 _tmp1EF=({struct _tuple22 _tmp202;_tmp202.f1=_tmp206;_tmp202.f2=_tmp204;_tmp202.f3=outflow;_tmp202;});struct _tuple22 _tmp1F0=_tmp1EF;struct Cyc_Dict_Dict _tmp201;struct Cyc_Dict_Dict _tmp200;struct Cyc_Dict_Dict _tmp1FF;struct Cyc_List_List*_tmp1FE;if(((_tmp1F0.f1).BottomFL).tag == 1){_LL157: _LL158:
 return({struct _tuple12 _tmp1F1;_tmp1F1.f1=outflow;_tmp1F1.f2=_tmp203;_tmp1F1;});}else{if(((_tmp1F0.f2).BottomFL).tag == 1){_LL159: _LL15A:
 return({struct _tuple12 _tmp1F2;_tmp1F2.f1=outflow;_tmp1F2.f2=_tmp205;_tmp1F2;});}else{if(((_tmp1F0.f3).ReachableFL).tag == 2){_LL15B: _tmp201=(((_tmp1F0.f1).ReachableFL).val).f1;_tmp200=(((_tmp1F0.f2).ReachableFL).val).f1;_tmp1FF=(((_tmp1F0.f3).ReachableFL).val).f1;_tmp1FE=(((_tmp1F0.f3).ReachableFL).val).f2;_LL15C:
# 950
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp205,_tmp203))return({struct _tuple12 _tmp1F3;_tmp1F3.f1=outflow;_tmp1F3.f2=_tmp203;_tmp1F3;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp203,_tmp205))return({struct _tuple12 _tmp1F4;_tmp1F4.f1=outflow;_tmp1F4.f2=_tmp205;_tmp1F4;});{
struct _RegionHandle*_tmp1F5=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp1F6=({struct Cyc_CfFlowInfo_JoinEnv _tmp1F9;_tmp1F9.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp320=({struct Cyc_CfFlowInfo_EscPile*_tmp1FA=_region_malloc(_tmp1F5,sizeof(*_tmp1FA));_tmp1FA->rgn=_tmp1F5;_tmp1FA->places=0;_tmp1FA;});_tmp1F9.pile=_tmp320;});_tmp1F9.d1=_tmp201;_tmp1F9.d2=_tmp200;_tmp1F9;});
void*_tmp1F7=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp1F6,0,_tmp205,_tmp203);
return({struct _tuple12 _tmp1F8;({union Cyc_CfFlowInfo_FlowInfo _tmp322=({struct Cyc_Dict_Dict _tmp321=Cyc_CfFlowInfo_escape_these(fenv,_tmp1F6.pile,_tmp1FF);Cyc_CfFlowInfo_ReachableFL(_tmp321,_tmp1FE);});_tmp1F8.f1=_tmp322;});_tmp1F8.f2=_tmp1F7;_tmp1F8;});};}else{_LL15D: _LL15E:
# 958
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));({struct Cyc_Core_Impossible_exn_struct _tmp324=({struct Cyc_Core_Impossible_exn_struct _tmp1FC;_tmp1FC.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp323=({const char*_tmp1FD="join_flow_and_rval: BottomFL outflow";_tag_dyneither(_tmp1FD,sizeof(char),37);});_tmp1FC.f1=_tmp323;});_tmp1FC;});_tmp1FB[0]=_tmp324;});_tmp1FB;}));}}}_LL156:;};};}
# 963
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 967
{struct _tuple13 _tmp207=({struct _tuple13 _tmp215;_tmp215.f1=r1;_tmp215.f2=r2;_tmp215;});struct _tuple13 _tmp208=_tmp207;int _tmp214;int _tmp213;struct _dyneither_ptr _tmp212;int _tmp211;int _tmp210;struct _dyneither_ptr _tmp20F;struct Cyc_CfFlowInfo_Place*_tmp20E;struct Cyc_CfFlowInfo_Place*_tmp20D;struct Cyc_Absyn_Vardecl*_tmp20C;void*_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20A;void*_tmp209;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->tag == 8){_LL160: _tmp20C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->f1;_tmp20B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->f2;_tmp20A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->f1;_tmp209=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->f2;_LL161:
# 969
 return _tmp20C == _tmp20A  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp20B,_tmp209);}else{_LL162: _LL163:
 goto _LL165;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->tag == 8){_LL164: _LL165:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->tag == 5){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->tag == 5){_LL166: _tmp20E=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->f1;_tmp20D=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->f1;_LL167:
 return Cyc_CfFlowInfo_place_cmp(_tmp20E,_tmp20D)== 0;}else{_LL168: _LL169:
 goto _LL16B;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->tag == 5){_LL16A: _LL16B:
 return 0;}else{switch(*((int*)_tmp208.f1)){case 6: switch(*((int*)_tmp208.f2)){case 6: _LL16C: _tmp214=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->f1).is_union;_tmp213=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->f1).fieldnum;_tmp212=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->f2;_tmp211=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->f1).is_union;_tmp210=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->f1).fieldnum;_tmp20F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->f2;_LL16D:
# 977
 if(_tmp214  && _tmp213 != _tmp210)return 0;
if((void**)_tmp212.curr == (void**)_tmp20F.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp212,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp212.curr)[i],*((void**)_check_dyneither_subscript(_tmp20F,sizeof(void*),i))))return 0;}}
return 1;case 2: goto _LL170;case 0: goto _LL172;case 1: goto _LL174;default: goto _LL17A;}case 1: switch(*((int*)_tmp208.f2)){case 2: _LL16E: _LL16F:
 return 1;case 0: goto _LL172;case 1: goto _LL174;default: goto _LL17A;}default: switch(*((int*)_tmp208.f2)){case 2: _LL170: _LL171:
 return 0;case 0: _LL172: _LL173:
 goto _LL175;case 1: _LL174: _LL175:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp208.f1)->tag == 4){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp208.f2)->tag == 4){_LL176: _LL177:
 goto _LL15F;}else{_LL178: _LL179:
 return 0;}}else{_LL17A: _LL17B:
 goto _LL15F;}}}}}}}_LL15F:;}{
# 990
struct _tuple20 _tmp216=({struct _tuple20 _tmp218;({enum Cyc_CfFlowInfo_InitLevel _tmp325=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp218.f1=_tmp325;});({enum Cyc_CfFlowInfo_InitLevel _tmp326=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp218.f2=_tmp326;});_tmp218;});struct _tuple20 _tmp217=_tmp216;if(_tmp217.f1 == Cyc_CfFlowInfo_AllIL)switch(_tmp217.f2){case Cyc_CfFlowInfo_AllIL: _LL17D: _LL17E:
 return 1;case Cyc_CfFlowInfo_NoneIL: goto _LL17F;default: goto _LL183;}else{if(_tmp217.f2 == Cyc_CfFlowInfo_NoneIL){_LL17F: _LL180:
 return 1;}else{if(_tmp217.f1 == Cyc_CfFlowInfo_NoneIL){_LL181: _LL182:
 return 0;}else{if(_tmp217.f2 == Cyc_CfFlowInfo_ThisIL){_LL183: _LL184:
 return 1;}else{_LL185: _LL186:
 return 0;}}}}_LL17C:;};}
# 1005
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1007
struct _tuple21 _tmp219=({struct _tuple21 _tmp21F;_tmp21F.f1=f1;_tmp21F.f2=f2;_tmp21F;});struct _tuple21 _tmp21A=_tmp219;struct Cyc_Dict_Dict _tmp21E;struct Cyc_List_List*_tmp21D;struct Cyc_Dict_Dict _tmp21C;struct Cyc_List_List*_tmp21B;if(((_tmp21A.f1).BottomFL).tag == 1){_LL188: _LL189:
 return 1;}else{if(((_tmp21A.f2).BottomFL).tag == 1){_LL18A: _LL18B:
 return 0;}else{_LL18C: _tmp21E=(((_tmp21A.f1).ReachableFL).val).f1;_tmp21D=(((_tmp21A.f1).ReachableFL).val).f2;_tmp21C=(((_tmp21A.f2).ReachableFL).val).f1;_tmp21B=(((_tmp21A.f2).ReachableFL).val).f2;_LL18D:
# 1011
 if(_tmp21E.t == _tmp21C.t  && _tmp21D == _tmp21B)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp21E,_tmp21C) && 
Cyc_Relations_relns_approx(_tmp21D,_tmp21B);}}_LL187:;}
# 1017
struct _tuple11 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv){
# 1019
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp220=rv;struct Cyc_Absyn_Vardecl*_tmp223;void*_tmp222;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp220)->tag == 8){_LL18F: _tmp223=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp220)->f1;_tmp222=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp220)->f2;_LL190:
# 1024
({struct Cyc_List_List*_tmp327=({struct Cyc_List_List*_tmp221=_region_malloc(rgn,sizeof(*_tmp221));_tmp221->hd=_tmp223;_tmp221->tl=names;_tmp221;});names=_tmp327;});rv=_tmp222;goto _LL18E;}else{_LL191: _LL192:
# 1026
 done=1;goto _LL18E;}_LL18E:;}
# 1029
return({struct _tuple11 _tmp224;_tmp224.f1=rv;_tmp224.f2=names;_tmp224;});}
# 1032
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp225=il;switch(_tmp225){case Cyc_CfFlowInfo_NoneIL: _LL194: _LL195:
({void*_tmp226=0;({struct Cyc___cycFILE*_tmp329=Cyc_stderr;struct _dyneither_ptr _tmp328=({const char*_tmp227="uninitialized";_tag_dyneither(_tmp227,sizeof(char),14);});Cyc_fprintf(_tmp329,_tmp328,_tag_dyneither(_tmp226,sizeof(void*),0));});});goto _LL193;case Cyc_CfFlowInfo_ThisIL: _LL196: _LL197:
({void*_tmp228=0;({struct Cyc___cycFILE*_tmp32B=Cyc_stderr;struct _dyneither_ptr _tmp32A=({const char*_tmp229="this-initialized";_tag_dyneither(_tmp229,sizeof(char),17);});Cyc_fprintf(_tmp32B,_tmp32A,_tag_dyneither(_tmp228,sizeof(void*),0));});});goto _LL193;default: _LL198: _LL199:
({void*_tmp22A=0;({struct Cyc___cycFILE*_tmp32D=Cyc_stderr;struct _dyneither_ptr _tmp32C=({const char*_tmp22B="all-initialized";_tag_dyneither(_tmp22B,sizeof(char),16);});Cyc_fprintf(_tmp32D,_tmp32C,_tag_dyneither(_tmp22A,sizeof(void*),0));});});goto _LL193;}_LL193:;}
# 1040
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp22C=root;struct Cyc_Absyn_Exp*_tmp238;void*_tmp237;struct Cyc_Absyn_Vardecl*_tmp236;switch(*((int*)_tmp22C)){case 0: _LL19B: _tmp236=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp22C)->f1;_LL19C:
# 1043
({struct Cyc_String_pa_PrintArg_struct _tmp22F;_tmp22F.tag=0;({struct _dyneither_ptr _tmp32E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp236->name));_tmp22F.f1=_tmp32E;});({void*_tmp22D[1]={& _tmp22F};({struct Cyc___cycFILE*_tmp330=Cyc_stderr;struct _dyneither_ptr _tmp32F=({const char*_tmp22E="Root(%s)";_tag_dyneither(_tmp22E,sizeof(char),9);});Cyc_fprintf(_tmp330,_tmp32F,_tag_dyneither(_tmp22D,sizeof(void*),1));});});});goto _LL19A;case 1: _LL19D: _tmp238=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp22C)->f1;_tmp237=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp22C)->f2;_LL19E:
# 1045
({struct Cyc_String_pa_PrintArg_struct _tmp233;_tmp233.tag=0;({struct _dyneither_ptr _tmp331=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp237));_tmp233.f1=_tmp331;});({struct Cyc_String_pa_PrintArg_struct _tmp232;_tmp232.tag=0;({struct _dyneither_ptr _tmp332=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp238));_tmp232.f1=_tmp332;});({void*_tmp230[2]={& _tmp232,& _tmp233};({struct Cyc___cycFILE*_tmp334=Cyc_stderr;struct _dyneither_ptr _tmp333=({const char*_tmp231="MallocPt(%s,%s)";_tag_dyneither(_tmp231,sizeof(char),16);});Cyc_fprintf(_tmp334,_tmp333,_tag_dyneither(_tmp230,sizeof(void*),2));});});});});goto _LL19A;default: _LL19F: _LL1A0:
# 1048
({void*_tmp234=0;({struct Cyc___cycFILE*_tmp336=Cyc_stderr;struct _dyneither_ptr _tmp335=({const char*_tmp235="InitParam(_,_)";_tag_dyneither(_tmp235,sizeof(char),15);});Cyc_fprintf(_tmp336,_tmp335,_tag_dyneither(_tmp234,sizeof(void*),0));});});goto _LL19A;}_LL19A:;}
# 1052
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp239=p->fields;
while(_tmp239 != 0){
({struct Cyc_Int_pa_PrintArg_struct _tmp23C;_tmp23C.tag=1;_tmp23C.f1=(unsigned long)((int)_tmp239->hd);({void*_tmp23A[1]={& _tmp23C};({struct Cyc___cycFILE*_tmp338=Cyc_stderr;struct _dyneither_ptr _tmp337=({const char*_tmp23B=".%d";_tag_dyneither(_tmp23B,sizeof(char),4);});Cyc_fprintf(_tmp338,_tmp337,_tag_dyneither(_tmp23A,sizeof(void*),1));});});});
_tmp239=_tmp239->tl;}
# 1059
({void*_tmp23D=0;({struct Cyc___cycFILE*_tmp33A=Cyc_stderr;struct _dyneither_ptr _tmp339=({const char*_tmp23E=" ";_tag_dyneither(_tmp23E,sizeof(char),2);});Cyc_fprintf(_tmp33A,_tmp339,_tag_dyneither(_tmp23D,sizeof(void*),0));});});};}
# 1062
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp23F=1;
while(x != 0){
if(!_tmp23F){({void*_tmp240=0;({struct Cyc___cycFILE*_tmp33C=Cyc_stderr;struct _dyneither_ptr _tmp33B=({const char*_tmp241=", ";_tag_dyneither(_tmp241,sizeof(char),3);});Cyc_fprintf(_tmp33C,_tmp33B,_tag_dyneither(_tmp240,sizeof(void*),0));});});_tmp23F=0;}
pr(x->hd);
x=x->tl;}
# 1069
({void*_tmp242=0;({struct Cyc___cycFILE*_tmp33E=Cyc_stderr;struct _dyneither_ptr _tmp33D=({const char*_tmp243="\n";_tag_dyneither(_tmp243,sizeof(char),2);});Cyc_fprintf(_tmp33E,_tmp33D,_tag_dyneither(_tmp242,sizeof(void*),0));});});}
# 1072
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp244=rval;struct Cyc_Absyn_Vardecl*_tmp277;void*_tmp276;struct Cyc_Absyn_Exp*_tmp275;int _tmp274;void*_tmp273;int _tmp272;int _tmp271;struct _dyneither_ptr _tmp270;struct Cyc_CfFlowInfo_Place*_tmp26F;enum Cyc_CfFlowInfo_InitLevel _tmp26E;enum Cyc_CfFlowInfo_InitLevel _tmp26D;switch(*((int*)_tmp244)){case 0: _LL1A2: _LL1A3:
({void*_tmp245=0;({struct Cyc___cycFILE*_tmp340=Cyc_stderr;struct _dyneither_ptr _tmp33F=({const char*_tmp246="Zero";_tag_dyneither(_tmp246,sizeof(char),5);});Cyc_fprintf(_tmp340,_tmp33F,_tag_dyneither(_tmp245,sizeof(void*),0));});});goto _LL1A1;case 1: _LL1A4: _LL1A5:
({void*_tmp247=0;({struct Cyc___cycFILE*_tmp342=Cyc_stderr;struct _dyneither_ptr _tmp341=({const char*_tmp248="NotZeroAll";_tag_dyneither(_tmp248,sizeof(char),11);});Cyc_fprintf(_tmp342,_tmp341,_tag_dyneither(_tmp247,sizeof(void*),0));});});goto _LL1A1;case 2: _LL1A6: _LL1A7:
({void*_tmp249=0;({struct Cyc___cycFILE*_tmp344=Cyc_stderr;struct _dyneither_ptr _tmp343=({const char*_tmp24A="NotZeroThis";_tag_dyneither(_tmp24A,sizeof(char),12);});Cyc_fprintf(_tmp344,_tmp343,_tag_dyneither(_tmp249,sizeof(void*),0));});});goto _LL1A1;case 3: _LL1A8: _tmp26D=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1;_LL1A9:
({void*_tmp24B=0;({struct Cyc___cycFILE*_tmp346=Cyc_stderr;struct _dyneither_ptr _tmp345=({const char*_tmp24C="Unknown(";_tag_dyneither(_tmp24C,sizeof(char),9);});Cyc_fprintf(_tmp346,_tmp345,_tag_dyneither(_tmp24B,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(_tmp26D);
({void*_tmp24D=0;({struct Cyc___cycFILE*_tmp348=Cyc_stderr;struct _dyneither_ptr _tmp347=({const char*_tmp24E=")";_tag_dyneither(_tmp24E,sizeof(char),2);});Cyc_fprintf(_tmp348,_tmp347,_tag_dyneither(_tmp24D,sizeof(void*),0));});});goto _LL1A1;case 4: _LL1AA: _tmp26E=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1;_LL1AB:
({void*_tmp24F=0;({struct Cyc___cycFILE*_tmp34A=Cyc_stderr;struct _dyneither_ptr _tmp349=({const char*_tmp250="Esc(";_tag_dyneither(_tmp250,sizeof(char),5);});Cyc_fprintf(_tmp34A,_tmp349,_tag_dyneither(_tmp24F,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(_tmp26E);
({void*_tmp251=0;({struct Cyc___cycFILE*_tmp34C=Cyc_stderr;struct _dyneither_ptr _tmp34B=({const char*_tmp252=")";_tag_dyneither(_tmp252,sizeof(char),2);});Cyc_fprintf(_tmp34C,_tmp34B,_tag_dyneither(_tmp251,sizeof(void*),0));});});goto _LL1A1;case 5: _LL1AC: _tmp26F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1;_LL1AD:
({void*_tmp253=0;({struct Cyc___cycFILE*_tmp34E=Cyc_stderr;struct _dyneither_ptr _tmp34D=({const char*_tmp254="AddrOf(";_tag_dyneither(_tmp254,sizeof(char),8);});Cyc_fprintf(_tmp34E,_tmp34D,_tag_dyneither(_tmp253,sizeof(void*),0));});});Cyc_CfFlowInfo_print_place(_tmp26F);
({void*_tmp255=0;({struct Cyc___cycFILE*_tmp350=Cyc_stderr;struct _dyneither_ptr _tmp34F=({const char*_tmp256=")";_tag_dyneither(_tmp256,sizeof(char),2);});Cyc_fprintf(_tmp350,_tmp34F,_tag_dyneither(_tmp255,sizeof(void*),0));});});goto _LL1A1;case 6: _LL1AE: _tmp272=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1).is_union;_tmp271=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1).fieldnum;_tmp270=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->f2;_LL1AF:
# 1084
 if(_tmp272){
({void*_tmp257=0;({struct Cyc___cycFILE*_tmp352=Cyc_stderr;struct _dyneither_ptr _tmp351=({const char*_tmp258="AggrUnion{";_tag_dyneither(_tmp258,sizeof(char),11);});Cyc_fprintf(_tmp352,_tmp351,_tag_dyneither(_tmp257,sizeof(void*),0));});});
if(_tmp271 != - 1)
({struct Cyc_Int_pa_PrintArg_struct _tmp25B;_tmp25B.tag=1;_tmp25B.f1=(unsigned long)_tmp271;({void*_tmp259[1]={& _tmp25B};({struct Cyc___cycFILE*_tmp354=Cyc_stderr;struct _dyneither_ptr _tmp353=({const char*_tmp25A="tag == %d;";_tag_dyneither(_tmp25A,sizeof(char),11);});Cyc_fprintf(_tmp354,_tmp353,_tag_dyneither(_tmp259,sizeof(void*),1));});});});}else{
# 1089
({void*_tmp25C=0;({struct Cyc___cycFILE*_tmp356=Cyc_stderr;struct _dyneither_ptr _tmp355=({const char*_tmp25D="AggrStruct{";_tag_dyneither(_tmp25D,sizeof(char),12);});Cyc_fprintf(_tmp356,_tmp355,_tag_dyneither(_tmp25C,sizeof(void*),0));});});}
{int i=0;for(0;i < _get_dyneither_size(_tmp270,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp270.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp270,sizeof(void*)))({void*_tmp25E=0;({struct Cyc___cycFILE*_tmp358=Cyc_stderr;struct _dyneither_ptr _tmp357=({const char*_tmp25F=",";_tag_dyneither(_tmp25F,sizeof(char),2);});Cyc_fprintf(_tmp358,_tmp357,_tag_dyneither(_tmp25E,sizeof(void*),0));});});}}
# 1094
({void*_tmp260=0;({struct Cyc___cycFILE*_tmp35A=Cyc_stderr;struct _dyneither_ptr _tmp359=({const char*_tmp261="}";_tag_dyneither(_tmp261,sizeof(char),2);});Cyc_fprintf(_tmp35A,_tmp359,_tag_dyneither(_tmp260,sizeof(void*),0));});});
goto _LL1A1;case 7: _LL1B0: _tmp275=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1;_tmp274=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp244)->f2;_tmp273=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp244)->f3;_LL1B1:
# 1097
({struct Cyc_Int_pa_PrintArg_struct _tmp265;_tmp265.tag=1;_tmp265.f1=(unsigned long)_tmp274;({struct Cyc_String_pa_PrintArg_struct _tmp264;_tmp264.tag=0;({struct _dyneither_ptr _tmp35B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp275));_tmp264.f1=_tmp35B;});({void*_tmp262[2]={& _tmp264,& _tmp265};({struct Cyc___cycFILE*_tmp35D=Cyc_stderr;struct _dyneither_ptr _tmp35C=({const char*_tmp263="[Consumed(%s,%d,";_tag_dyneither(_tmp263,sizeof(char),17);});Cyc_fprintf(_tmp35D,_tmp35C,_tag_dyneither(_tmp262,sizeof(void*),2));});});});});
Cyc_CfFlowInfo_print_absrval(_tmp273);({void*_tmp266=0;({struct Cyc___cycFILE*_tmp35F=Cyc_stderr;struct _dyneither_ptr _tmp35E=({const char*_tmp267=")]";_tag_dyneither(_tmp267,sizeof(char),3);});Cyc_fprintf(_tmp35F,_tmp35E,_tag_dyneither(_tmp266,sizeof(void*),0));});});
goto _LL1A1;default: _LL1B2: _tmp277=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1;_tmp276=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp244)->f2;_LL1B3:
# 1101
({struct Cyc_String_pa_PrintArg_struct _tmp26A;_tmp26A.tag=0;({struct _dyneither_ptr _tmp360=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp277->name));_tmp26A.f1=_tmp360;});({void*_tmp268[1]={& _tmp26A};({struct Cyc___cycFILE*_tmp362=Cyc_stderr;struct _dyneither_ptr _tmp361=({const char*_tmp269="[NamedLocation(%s,";_tag_dyneither(_tmp269,sizeof(char),19);});Cyc_fprintf(_tmp362,_tmp361,_tag_dyneither(_tmp268,sizeof(void*),1));});});});
Cyc_CfFlowInfo_print_absrval(_tmp276);({void*_tmp26B=0;({struct Cyc___cycFILE*_tmp364=Cyc_stderr;struct _dyneither_ptr _tmp363=({const char*_tmp26C=")]";_tag_dyneither(_tmp26C,sizeof(char),3);});Cyc_fprintf(_tmp364,_tmp363,_tag_dyneither(_tmp26B,sizeof(void*),0));});});
goto _LL1A1;}_LL1A1:;}
# 1107
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp278=0;({struct Cyc___cycFILE*_tmp366=Cyc_stderr;struct _dyneither_ptr _tmp365=({const char*_tmp279="    ";_tag_dyneither(_tmp279,sizeof(char),5);});Cyc_fprintf(_tmp366,_tmp365,_tag_dyneither(_tmp278,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp27A=0;({struct Cyc___cycFILE*_tmp368=Cyc_stderr;struct _dyneither_ptr _tmp367=({const char*_tmp27B=" --> ";_tag_dyneither(_tmp27B,sizeof(char),6);});Cyc_fprintf(_tmp368,_tmp367,_tag_dyneither(_tmp27A,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp27C=0;({struct Cyc___cycFILE*_tmp36A=Cyc_stderr;struct _dyneither_ptr _tmp369=({const char*_tmp27D="\n";_tag_dyneither(_tmp27D,sizeof(char),2);});Cyc_fprintf(_tmp36A,_tmp369,_tag_dyneither(_tmp27C,sizeof(void*),0));});});}
# 1115
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1119
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp27E=f;struct Cyc_Dict_Dict _tmp288;struct Cyc_List_List*_tmp287;if((_tmp27E.BottomFL).tag == 1){_LL1B5: _LL1B6:
({void*_tmp27F=0;({struct Cyc___cycFILE*_tmp36C=Cyc_stderr;struct _dyneither_ptr _tmp36B=({const char*_tmp280="  BottomFL\n";_tag_dyneither(_tmp280,sizeof(char),12);});Cyc_fprintf(_tmp36C,_tmp36B,_tag_dyneither(_tmp27F,sizeof(void*),0));});});goto _LL1B4;}else{_LL1B7: _tmp288=((_tmp27E.ReachableFL).val).f1;_tmp287=((_tmp27E.ReachableFL).val).f2;_LL1B8:
# 1123
({void*_tmp281=0;({struct Cyc___cycFILE*_tmp36E=Cyc_stderr;struct _dyneither_ptr _tmp36D=({const char*_tmp282="  ReachableFL:\n";_tag_dyneither(_tmp282,sizeof(char),16);});Cyc_fprintf(_tmp36E,_tmp36D,_tag_dyneither(_tmp281,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_flowdict(_tmp288);
({void*_tmp283=0;({struct Cyc___cycFILE*_tmp370=Cyc_stderr;struct _dyneither_ptr _tmp36F=({const char*_tmp284="\n  Relations: ";_tag_dyneither(_tmp284,sizeof(char),15);});Cyc_fprintf(_tmp370,_tmp36F,_tag_dyneither(_tmp283,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp287);
({void*_tmp285=0;({struct Cyc___cycFILE*_tmp372=Cyc_stderr;struct _dyneither_ptr _tmp371=({const char*_tmp286="\n";_tag_dyneither(_tmp286,sizeof(char),2);});Cyc_fprintf(_tmp372,_tmp371,_tag_dyneither(_tmp285,sizeof(void*),0));});});
goto _LL1B4;}_LL1B4:;}

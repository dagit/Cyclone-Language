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
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
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
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char* getenv(const char*);
# 318
int system(const char*);
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
# 270 "core.h"
void Cyc_Core_rethrow(void*);
# 274
const char*Cyc_Core_get_exn_name(void*);
# 276
const char*Cyc_Core_get_exn_filename();
# 279
int Cyc_Core_get_exn_lineno();
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
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
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
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 119
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 33
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 46
extern int Cyc_Position_use_gcc_style_location;
# 48
extern int Cyc_Position_max_errors;
# 50
int Cyc_Position_error_p();struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 74
typedef void*Cyc_Tcpat_decision_opt_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 94
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 101
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 106
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
# 157
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 178
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 183
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 209
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 211
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 220
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 366
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 371
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 442 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 445
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 509
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 535
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 542
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 560
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1187 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1189
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;extern char Cyc_Parse_Exit[5];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 33
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 44
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 49
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_declarator_list_t;
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
typedef struct Cyc_List_List*Cyc_pointer_quals_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 26 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
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
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 38 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 49
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 65
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 69
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 80
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 96 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 78 "tcutil.h"
extern int Cyc_Tcutil_warn_alias_coerce;
# 81
extern int Cyc_Tcutil_warn_region_coerce;
# 32 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 34
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 31 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 34
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 41
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 29 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 44 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 46
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 85
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 90
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 98
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 119
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 126
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 142 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 161
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 33 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 36
struct Cyc_Interface_I*Cyc_Interface_empty();
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 87
typedef struct Cyc_Interface_I*Cyc_interface_t;
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
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 37 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 40
extern int Cyc_ParseErrors_print_state_and_token;
# 43
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 52
typedef struct Cyc_Hashtable_Table*Cyc_table_t;
# 54
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
# 85
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->hd=({struct _dyneither_ptr*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=s;_tmp1;});_tmp0->tl=Cyc_ccargs;_tmp0;});}
# 93
void Cyc_set_c_compiler(struct _dyneither_ptr s){
if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp2="vc";_tag_dyneither(_tmp2,sizeof(char),3);}))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_dyneither(_tmp3,sizeof(char),7);}));}else{
# 98
if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp4="gcc";_tag_dyneither(_tmp4,sizeof(char),4);}))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_dyneither(_tmp5,sizeof(char),8);}));}}}
# 105
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 112
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
Cyc_output_file=({struct _dyneither_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=s;_tmp6;});}
# 117
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 126
extern char* Cdef_inc_path;
extern char* Cdef_lib_path;
extern char* Carch;
extern char* Ccomp;
extern char* Cversion;static char _tmp7[1]="";
# 133
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 138
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=({struct _dyneither_ptr*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=s;_tmp9;});_tmp8->tl=Cyc_cppargs;_tmp8;});}
# 143
static void Cyc_add_cpp_and_ccarg(struct _dyneither_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 148
static void Cyc_add_iprefix(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmpA[1]={& _tmpC};Cyc_aprintf(({const char*_tmpB="-iprefix %s";_tag_dyneither(_tmpB,sizeof(char),12);}),_tag_dyneither(_tmpA,sizeof(void*),1));});}));}
# 151
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmpD[1]={& _tmpF};Cyc_aprintf(({const char*_tmpE="-iwithprefix %s";_tag_dyneither(_tmpE,sizeof(char),16);}),_tag_dyneither(_tmpD,sizeof(void*),1));});}));}
# 154
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp10[1]={& _tmp12};Cyc_aprintf(({const char*_tmp11="-iwithprefixbefore %s";_tag_dyneither(_tmp11,sizeof(char),22);}),_tag_dyneither(_tmp10,sizeof(void*),1));});}));}
# 157
static void Cyc_add_isystem(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp13[1]={& _tmp15};Cyc_aprintf(({const char*_tmp14="-isystem %s";_tag_dyneither(_tmp14,sizeof(char),12);}),_tag_dyneither(_tmp13,sizeof(void*),1));});}));}
# 160
static void Cyc_add_idirafter(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp16[1]={& _tmp18};Cyc_aprintf(({const char*_tmp17="-idirafter %s";_tag_dyneither(_tmp17,sizeof(char),14);}),_tag_dyneither(_tmp16,sizeof(void*),1));});}));}
# 164
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 167
Cyc_target_arch=s;
Cyc_add_ccarg(({const char*_tmp19="-b";_tag_dyneither(_tmp19,sizeof(char),3);}));
Cyc_add_ccarg(s);}
# 172
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 177
static void Cyc_print_version(){
({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)({char*_tmp1D= Cversion;_tag_dyneither(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1));});({void*_tmp1A[1]={& _tmp1C};Cyc_printf(({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_dyneither(_tmp1B,sizeof(char),34);}),_tag_dyneither(_tmp1A,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _dyneither_ptr)({char*_tmp23= Ccomp;_tag_dyneither(_tmp23,sizeof(char),_get_zero_arr_size_char((void*)_tmp23,1));});({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct _dyneither_ptr)({char*_tmp22= Carch;_tag_dyneither(_tmp22,sizeof(char),_get_zero_arr_size_char((void*)_tmp22,1));});({void*_tmp1E[2]={& _tmp20,& _tmp21};Cyc_printf(({const char*_tmp1F="Compiled for architecture: %s CC=\"%s\"\n";_tag_dyneither(_tmp1F,sizeof(char),39);}),_tag_dyneither(_tmp1E,sizeof(void*),2));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _dyneither_ptr)({char*_tmp27= Cdef_lib_path;_tag_dyneither(_tmp27,sizeof(char),_get_zero_arr_size_char((void*)_tmp27,1));});({void*_tmp24[1]={& _tmp26};Cyc_printf(({const char*_tmp25="Standard library directory: %s\n";_tag_dyneither(_tmp25,sizeof(char),32);}),_tag_dyneither(_tmp24,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)({char*_tmp2B= Cdef_inc_path;_tag_dyneither(_tmp2B,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B,1));});({void*_tmp28[1]={& _tmp2A};Cyc_printf(({const char*_tmp29="Standard include directory: %s\n";_tag_dyneither(_tmp29,sizeof(char),32);}),_tag_dyneither(_tmp28,sizeof(void*),1));});});
 exit(0);}
# 185
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp2C=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp2C <= 4)return 0;else{
return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp2C - 4)),({const char*_tmp2D=".cyc";_tag_dyneither(_tmp2D,sizeof(char),5);}))== 0;}}
# 191
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp2E=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp2E <= 2)return;{
struct _dyneither_ptr _tmp2F=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp2E - 2);
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=_tmp2F;_tmp31;});_tmp30->tl=Cyc_cyclone_exec_path;_tmp30;});};}
# 209 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp32="-lxml";_tag_dyneither(_tmp32,sizeof(char),6);}))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 215
Cyc_add_ccarg(({const char*_tmp33="-lxml_a";_tag_dyneither(_tmp33,sizeof(char),8);}));}}else{
if(Cyc_strncmp((struct _dyneither_ptr)s,({const char*_tmp34="-lcyc";_tag_dyneither(_tmp34,sizeof(char),6);}),5)== 0)
Cyc_add_ccarg(s);else{
# 219
Cyc_libargs=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=({struct _dyneither_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=s;_tmp36;});_tmp35->tl=Cyc_libargs;_tmp35;});}}}
# 222
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 226
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp37="-save-temps";_tag_dyneither(_tmp37,sizeof(char),12);}));}
# 231
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp38="-M";_tag_dyneither(_tmp38,sizeof(char),3);}));}
# 238
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
Cyc_dependencies_target=({struct _dyneither_ptr*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=s;_tmp39;});}
# 243
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp3A="-c";_tag_dyneither(_tmp3A,sizeof(char),3);}));}
# 248
static void Cyc_set_nocppprecomp(){
Cyc_add_cpp_and_ccarg(({const char*_tmp3B="-no-cpp-precomp";_tag_dyneither(_tmp3B,sizeof(char),16);}));}
# 252
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 256
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 259
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 263
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-DCYC_ANSI_OUTPUT";_tag_dyneither(_tmp3C,sizeof(char),18);}));
Cyc_set_elim_se();}
# 268
static void Cyc_set_noboundschecks(){
Cyc_add_ccarg(({const char*_tmp3D="-DNO_CYC_BOUNDS_CHECKS";_tag_dyneither(_tmp3D,sizeof(char),23);}));}
# 271
static void Cyc_set_nonullchecks(){
Cyc_add_ccarg(({const char*_tmp3E="-DNO_CYC_NULL_CHECKS";_tag_dyneither(_tmp3E,sizeof(char),21);}));}
# 274
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 280
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp3F="-DNO_CYC_PREFIX";_tag_dyneither(_tmp3F,sizeof(char),16);}));}
# 285
static void Cyc_set_pa(){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp40="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp40,sizeof(char),21);}));
Cyc_add_cpparg(({const char*_tmp41="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp41,sizeof(char),21);}));}
# 291
static void Cyc_set_pg(){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp42="-pg";_tag_dyneither(_tmp42,sizeof(char),4);}));}
# 296
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp43="-S";_tag_dyneither(_tmp43,sizeof(char),3);}));}
# 301
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;
Cyc_Tc_aggressive_warn=1;
Cyc_Binding_warn_override=1;}
# 312
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 316
typedef enum Cyc_inputtype Cyc_inputtype_t;
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp44="cyc";_tag_dyneither(_tmp44,sizeof(char),4);}))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp45="none";_tag_dyneither(_tmp45,sizeof(char),5);}))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 324
({struct Cyc_String_pa_PrintArg_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp46[1]={& _tmp48};Cyc_fprintf(Cyc_stderr,({const char*_tmp47="Input type '%s' not supported\n";_tag_dyneither(_tmp47,sizeof(char),31);}),_tag_dyneither(_tmp46,sizeof(void*),1));});});}}}
# 327
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp49=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp4A=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 333
struct _dyneither_ptr _tmp4B=Cyc_strlen((struct _dyneither_ptr)_tmp49)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp49,(struct _dyneither_ptr)_tmp4A): _tmp4A;
return(struct _dyneither_ptr)_tmp4B;}
# 339
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 341
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _dyneither_ptr s){
# 344
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 348
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 350
Cyc_cyclone_files=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=({struct _dyneither_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=s;_tmp4D;});_tmp4C->tl=Cyc_cyclone_files;_tmp4C;});{
# 353
struct _dyneither_ptr _tmp4E=Cyc_make_base_filename(s,Cyc_output_file);
struct _dyneither_ptr _tmp4F=Cyc_strconcat((struct _dyneither_ptr)_tmp4E,({const char*_tmp50=".c";_tag_dyneither(_tmp50,sizeof(char),3);}));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp4F);};}else{
# 357
Cyc_add_ccarg(s);}}}
# 362
static int Cyc_assume_gcc_flag=1;static char _tmp51[3]="-s";static char _tmp52[3]="-O";static char _tmp53[4]="-O0";static char _tmp54[4]="-O2";static char _tmp55[4]="-O3";static char _tmp56[21]="-fomit-frame-pointer";static char _tmp57[13]="-fno-builtin";static char _tmp58[3]="-g";static char _tmp59[4]="-pg";static char _tmp5A[3]="-p";static char _tmp5B[8]="-static";
static int Cyc_add_other_flag(struct _dyneither_ptr s){
static struct {struct _dyneither_ptr flag;int optargs;}known_gcc_flags[11]={{.flag={_tmp51,_tmp51,_tmp51 + 3},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 4},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 4},.optargs=0},{.flag={_tmp55,_tmp55,_tmp55 + 4},.optargs=0},{.flag={_tmp56,_tmp56,_tmp56 + 21},.optargs=0},{.flag={_tmp57,_tmp57,_tmp57 + 13},.optargs=0},{.flag={_tmp58,_tmp58,_tmp58 + 3},.optargs=0},{.flag={_tmp59,_tmp59,_tmp59 + 4},.optargs=0},{.flag={_tmp5A,_tmp5A,_tmp5A + 3},.optargs=0},{.flag={_tmp5B,_tmp5B,_tmp5B + 8},.optargs=0}};
# 377
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 380
{int i=0;for(0;i < 11;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)(known_gcc_flags[_check_known_subscript_notnull(11,i)]).flag,(struct _dyneither_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[_check_known_subscript_notnull(11,i)]).optargs;
break;}}}
# 387
return 0;}
# 389
return 1;}
# 392
static void Cyc_noassume_gcc_flag(){
Cyc_assume_gcc_flag=0;}
# 396
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
 remove((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 407
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp5C=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(file,sizeof(char),1)),"r");
# 415
if(spec_file == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp5D[1]={& _tmp5F};Cyc_fprintf(Cyc_stderr,({const char*_tmp5E="Error opening spec file %s\n";_tag_dyneither(_tmp5E,sizeof(char),28);}),_tag_dyneither(_tmp5D,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 0;}
# 421
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if( isspace(c))continue;
if(c == '*')break;
if(c != - 1){
({struct Cyc_Int_pa_PrintArg_struct _tmp63;_tmp63.tag=1;_tmp63.f1=(unsigned long)c;({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp60[2]={& _tmp62,& _tmp63};Cyc_fprintf(Cyc_stderr,({const char*_tmp61="Error reading spec file %s: unexpected character '%c'\n";_tag_dyneither(_tmp61,sizeof(char),55);}),_tag_dyneither(_tmp60,sizeof(void*),2));});});});
# 430
Cyc_fflush(Cyc_stderr);}
# 432
goto CLEANUP_AND_RETURN;}
# 434
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp64[1]={& _tmp66};Cyc_fprintf(Cyc_stderr,({const char*_tmp65="Error reading spec file %s: unexpected EOF\n";_tag_dyneither(_tmp65,sizeof(char),44);}),_tag_dyneither(_tmp64,sizeof(void*),1));});});
# 442
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 445
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 449
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp67[1]={& _tmp69};Cyc_fprintf(Cyc_stderr,({const char*_tmp68="Error reading spec file %s: string name too long\n";_tag_dyneither(_tmp68,sizeof(char),50);}),_tag_dyneither(_tmp67,sizeof(void*),1));});});
# 455
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 459
while(1){
c=Cyc_fgetc(spec_file);
if( isspace(c))continue;
break;}
# 464
if(c == '*'){
# 466
_tmp5C=({struct Cyc_List_List*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->hd=({struct _tuple21*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->f1=({struct _dyneither_ptr*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256));_tmp6E;});_tmp6B->f2=({struct _dyneither_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=(struct _dyneither_ptr)
Cyc_strdup(({const char*_tmp6D="";_tag_dyneither(_tmp6D,sizeof(char),1);}));_tmp6C;});_tmp6B;});_tmp6A->tl=_tmp5C;_tmp6A;});
goto JUST_AFTER_STAR;}
# 471
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)
# 481
_tag_dyneither(strname,sizeof(char),256);({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp6F[2]={& _tmp71,& _tmp72};Cyc_fprintf(Cyc_stderr,({const char*_tmp70="Error reading spec file %s: value of %s too long\n";_tag_dyneither(_tmp70,sizeof(char),50);}),_tag_dyneither(_tmp6F,sizeof(void*),2));});});});
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 487
_tmp5C=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=({struct _tuple21*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->f1=({struct _dyneither_ptr*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256));_tmp76;});_tmp74->f2=({struct _dyneither_ptr*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=(struct _dyneither_ptr)
Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096));_tmp75;});_tmp74;});_tmp73->tl=_tmp5C;_tmp73;});
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 492
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp5C;}
# 498
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp77=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp78=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp77 > 4096)goto DONE;
while(1){
# 508
while(1){
if(i >= _tmp77)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(! isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 514
j=0;
# 519
while(1){
if(i >= _tmp77)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if( isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 525
++ i;
if(i >= _tmp77)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 532
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 535
++ i;}
# 537
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';
# 540
_tmp78=({struct Cyc_List_List*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->hd=({struct _dyneither_ptr*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096));_tmp7A;});_tmp79->tl=_tmp78;_tmp79;});}
# 542
DONE:
 _tmp78=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp78);
_tmp78=({struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);_tmp7B->tl=_tmp78;_tmp7B;});{
struct _dyneither_ptr _tmp7D=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp78);
struct _dyneither_ptr _tmp7E=({unsigned int _tmp7F=_get_dyneither_size(_tmp7D,sizeof(struct _dyneither_ptr*));struct _dyneither_ptr*_tmp80=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp7F));struct _dyneither_ptr _tmp82=_tag_dyneither(_tmp80,sizeof(struct _dyneither_ptr),_tmp7F);{unsigned int _tmp81=_tmp7F;unsigned int k;for(k=0;k < _tmp81;k ++){_tmp80[k]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)_tmp7D.curr)[(int)k];}}_tmp82;});
return _tmp7E;};};}
# 552
int Cyc_compile_failure=0;
# 554
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp83;_push_handler(& _tmp83);{int _tmp85=0;if(setjmp(_tmp83.handler))_tmp85=1;if(!_tmp85){{struct Cyc___cycFILE*_tmp86=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp86;};_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*_tmp87=_tmp84;_LL1: _LL2:
# 558
 Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename);({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp88[2]={& _tmp8A,& _tmp8B};Cyc_fprintf(Cyc_stderr,({const char*_tmp89="\nError: couldn't open %s %s\n";_tag_dyneither(_tmp89,sizeof(char),29);}),_tag_dyneither(_tmp88,sizeof(void*),2));});});});
Cyc_fflush(Cyc_stderr);
return 0;_LL0:;}};}
# 565
void CYCALLOCPROFILE_mark(const char*s);
# 567
void*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void(*on_fail)(void*),void*failenv){
# 570
 CYCALLOCPROFILE_mark((const char*)_untag_dyneither_ptr(stage_name,sizeof(char),1));{
# 572
void*ans;
{struct _handler_cons _tmp8C;_push_handler(& _tmp8C);{int _tmp8E=0;if(setjmp(_tmp8C.handler))_tmp8E=1;if(!_tmp8E){ans=f(env,tds);;_pop_handler();}else{void*_tmp8D=(void*)_exn_thrown;void*_tmp8F=_tmp8D;void*_tmp93;_LL6: _tmp93=_tmp8F;_LL7:
# 576
({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);({void*_tmp90[1]={& _tmp92};Cyc_fprintf(Cyc_stderr,({const char*_tmp91="COMPILER STAGE %s\n";_tag_dyneither(_tmp91,sizeof(char),19);}),_tag_dyneither(_tmp90,sizeof(void*),1));});});
on_fail(failenv);
Cyc_Core_rethrow(_tmp93);_LL5:;}};}
# 580
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
on_fail(failenv);
({void*_tmp94=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp95="\nCOMPILATION FAILED!\n";_tag_dyneither(_tmp95,sizeof(char),22);}),_tag_dyneither(_tmp94,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
return ans;}else{
# 588
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp98;_tmp98.tag=0;_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);({void*_tmp96[1]={& _tmp98};Cyc_fprintf(Cyc_stderr,({const char*_tmp97="%s completed.\n";_tag_dyneither(_tmp97,sizeof(char),15);}),_tag_dyneither(_tmp96,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return ans;}}
# 593
return ans;};}
# 596
static void Cyc_ignore(void*x){;}
static void Cyc_remove_fileptr(struct _dyneither_ptr*s){Cyc_remove_file((struct _dyneither_ptr)*s);}
# 599
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp99;_push_handler(& _tmp99);{int _tmp9B=0;if(setjmp(_tmp99.handler))_tmp9B=1;if(!_tmp9B){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 604
;_pop_handler();}else{void*_tmp9A=(void*)_exn_thrown;void*_tmp9C=_tmp9A;void*_tmp9D;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp9C)->tag == Cyc_Parse_Exit){_LLB: _LLC:
# 607
 Cyc_file_close(f);Cyc_compile_failure=1;goto _LLA;}else{_LLD: _tmp9D=_tmp9C;_LLE:
 Cyc_file_close(f);Cyc_Core_rethrow(_tmp9D);}_LLA:;}};}
# 610
Cyc_Lex_lex_init(1);
return ans;};}
# 614
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 619
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 621
struct _tuple22 _tmp9E=*tcenv;struct _tuple22 _tmp9F=_tmp9E;struct _RegionHandle*_tmpA1;struct Cyc_Tcenv_Tenv*_tmpA0;_LL12: _tmpA1=_tmp9F.f1;_tmpA0=_tmp9F.f2;_LL13:;
Cyc_Tc_tc(_tmpA1,_tmpA0,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmpA0,tds);
return tds;}
# 628
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 630
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 633
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 635
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 641
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
# 643
struct _tuple23*_tmpA2=params;struct Cyc_Tcenv_Tenv*_tmpAA;struct Cyc___cycFILE*_tmpA9;struct Cyc___cycFILE*_tmpA8;_LL15: _tmpAA=_tmpA2->f1;_tmpA9=_tmpA2->f2;_tmpA8=_tmpA2->f3;_LL16:;{
struct Cyc_Interface_I*_tmpA3=Cyc_Interface_extract(_tmpAA->ae,tds);
if(_tmpA9 == 0)
Cyc_Interface_save(_tmpA3,_tmpA8);else{
# 648
struct Cyc_Interface_I*_tmpA4=Cyc_Interface_parse(_tmpA9);
if(!Cyc_Interface_is_subinterface(_tmpA4,_tmpA3,({struct _tuple20*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->f1=({const char*_tmpA7="written interface";_tag_dyneither(_tmpA7,sizeof(char),18);});_tmpA5->f2=({const char*_tmpA6="maximal interface";_tag_dyneither(_tmpA6,sizeof(char),18);});_tmpA5;})))
Cyc_compile_failure=1;else{
# 652
Cyc_Interface_save(_tmpA4,_tmpA8);}}
# 654
return tds;};}
# 656
void Cyc_interface_fail(struct _tuple23*params){
struct _tuple23*_tmpAB=params;struct Cyc___cycFILE*_tmpAD;struct Cyc___cycFILE*_tmpAC;_LL18: _tmpAD=_tmpAB->f2;_tmpAC=_tmpAB->f3;_LL19:;
if(_tmpAD != 0)
Cyc_file_close(_tmpAD);
Cyc_file_close(_tmpAC);}
# 663
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 666
return Cyc_Toc_toc(pops,tds);}
# 668
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 673
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 675
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 677
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 682
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 685
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
({void*_tmpAE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpAF="Internal error: copy_internal_file called with NULL\n";_tag_dyneither(_tmpAF,sizeof(char),53);}),_tag_dyneither(_tmpAE,sizeof(void*),0));});
return 1;}{
# 689
struct Cyc___cycFILE*_tmpB0=Cyc_try_file_open(file,({const char*_tmpB8="r";_tag_dyneither(_tmpB8,sizeof(char),2);}),({const char*_tmpB9="internal compiler file";_tag_dyneither(_tmpB9,sizeof(char),23);}));
if(_tmpB0 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpB7=1024;unsigned int i;for(i=0;i < _tmpB7;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpB0);
if(n_read != 1024  && !Cyc_feof(_tmpB0)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmpB1[1]={& _tmpB3};Cyc_fprintf(Cyc_stderr,({const char*_tmpB2="\nError: problem copying %s\n";_tag_dyneither(_tmpB2,sizeof(char),28);}),_tag_dyneither(_tmpB1,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 702
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmpB4[1]={& _tmpB6};Cyc_fprintf(Cyc_stderr,({const char*_tmpB5="\nError: problem copying %s\n";_tag_dyneither(_tmpB5,sizeof(char),28);}),_tag_dyneither(_tmpB4,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 710
Cyc_fclose(_tmpB0);
return 0;};};}
# 716
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}struct _tuple24{struct Cyc___cycFILE*f1;struct _dyneither_ptr f2;};
# 723
struct Cyc_List_List*Cyc_do_print(struct _tuple24*env,struct Cyc_List_List*tds){
# 725
struct _tuple24*_tmpBA=env;struct Cyc___cycFILE*_tmpC3;struct _dyneither_ptr _tmpC2;_LL1B: _tmpC3=_tmpBA->f1;_tmpC2=_tmpBA->f2;_LL1C:;{
struct Cyc_Absynpp_Params _tmpBB=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpBB.expand_typedefs=!Cyc_noexpand_r;
_tmpBB.to_VC=Cyc_Cyclone_tovc_r;
_tmpBB.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpBB.generate_line_directives=Cyc_generate_line_directives_r;
_tmpBB.print_full_evars=Cyc_print_full_evars_r;
_tmpBB.print_all_tvars=Cyc_print_all_tvars_r;
_tmpBB.print_all_kinds=Cyc_print_all_kinds_r;
_tmpBB.print_all_effects=Cyc_print_all_effects_r;
# 736
if(Cyc_inline_functions_r)
({void*_tmpBC=0;Cyc_fprintf(_tmpC3,({const char*_tmpBD="#define _INLINE_FUNCTIONS\n";_tag_dyneither(_tmpBD,sizeof(char),27);}),_tag_dyneither(_tmpBC,sizeof(void*),0));});
# 740
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r)
({void*_tmpBE=0;Cyc_fprintf(_tmpC3,({const char*_tmpBF="#include <setjmp.h>\n";_tag_dyneither(_tmpBF,sizeof(char),21);}),_tag_dyneither(_tmpBE,sizeof(void*),0));});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,_tmpC3))return tds;}}
# 746
if(Cyc_copy_internal_file(Cyc_cyc_include,_tmpC3))return tds;}
# 748
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpBB);
Cyc_Absynpp_decllist2file(tds,_tmpC3);}else{
# 752
Cyc_Absyndump_set_params(& _tmpBB);
Cyc_Absyndump_dumpdecllist2file(tds,_tmpC3);}
# 755
Cyc_fflush(_tmpC3);
Cyc_file_close(_tmpC3);
Cyc_cfiles=({struct Cyc_List_List*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->hd=({struct _dyneither_ptr*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=_tmpC2;_tmpC1;});_tmpC0->tl=Cyc_cfiles;_tmpC0;});
return tds;};}
# 760
void Cyc_print_fail(struct _tuple24*env){
struct _tuple24*_tmpC4=env;struct Cyc___cycFILE*_tmpC8;struct _dyneither_ptr _tmpC7;_LL1E: _tmpC8=_tmpC4->f1;_tmpC7=_tmpC4->f2;_LL1F:;
Cyc_file_close(_tmpC8);
Cyc_cfiles=({struct Cyc_List_List*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=({struct _dyneither_ptr*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=Cyc_strdup((struct _dyneither_ptr)_tmpC7);_tmpC6;});_tmpC5->tl=Cyc_cfiles;_tmpC5;});}
# 767
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpC9=0;
unsigned long _tmpCA=Cyc_strlen((struct _dyneither_ptr)s);
# 772
while(_tmpCA > 0){
struct _dyneither_ptr _tmpCB=Cyc_strchr(s,c);
if((char*)_tmpCB.curr == (char*)(_tag_dyneither(0,0,0)).curr){
_tmpC9=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=({struct _dyneither_ptr*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=s;_tmpCD;});_tmpCC->tl=_tmpC9;_tmpCC;});
break;}else{
# 779
_tmpC9=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=({struct _dyneither_ptr*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpCB).curr - s.curr)/ sizeof(char)));_tmpCF;});_tmpCE->tl=_tmpC9;_tmpCE;});
_tmpCA -=(((struct _dyneither_ptr)_tmpCB).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpCB,sizeof(char),1);}}
# 784
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC9);};}
# 788
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpD0;_push_handler(& _tmpD0);{int _tmpD2=0;if(setjmp(_tmpD0.handler))_tmpD2=1;if(!_tmpD2){f=Cyc_file_open(file,({const char*_tmpD3="r";_tag_dyneither(_tmpD3,sizeof(char),2);}));;_pop_handler();}else{void*_tmpD1=(void*)_exn_thrown;void*_tmpD4=_tmpD1;_LL21: _LL22: goto _LL20;_LL20:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 796
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
struct _dyneither_ptr tmp=({const char*_tmpD7="";_tag_dyneither(_tmpD7,sizeof(char),1);});
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpD5=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpD5.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpD5)== 0)continue;
_tmpD5=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpD5,({const char*_tmpD6=":";_tag_dyneither(_tmpD6,sizeof(char),2);}));
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpD5,(struct _dyneither_ptr)tmp);}
# 804
return tmp;}
# 809
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpD8=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpD8.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpD8)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpD8,file);
if(Cyc_file_exists(s))return({struct _dyneither_ptr*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=s;_tmpD9;});};}
# 817
return 0;}
# 820
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpDA=Cyc_find(dirs,file);
if(_tmpDA == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs));({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmpDB[2]={& _tmpDD,& _tmpDE};Cyc_fprintf(Cyc_stderr,({const char*_tmpDC="Error: can't find internal compiler file %s in path %s\n";_tag_dyneither(_tmpDC,sizeof(char),56);}),_tag_dyneither(_tmpDB,sizeof(void*),2));});});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Core_Failure_exn_struct _tmpE0;_tmpE0.tag=Cyc_Core_Failure;_tmpE0.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs));({struct Cyc_String_pa_PrintArg_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmpE1[2]={& _tmpE3,& _tmpE4};Cyc_aprintf(({const char*_tmpE2="Error: can't find internal compiler file %s in path %s\n";_tag_dyneither(_tmpE2,sizeof(char),56);}),_tag_dyneither(_tmpE1,sizeof(void*),2));});});});_tmpE0;});_tmpDF;}));}
# 832
return*_tmpDA;}
# 837
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 839
struct Cyc_List_List*_tmpE5=0;
for(0;dirs != 0;dirs=dirs->tl){
_tmpE5=({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=({struct _dyneither_ptr*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir);_tmpE7;});_tmpE6->tl=_tmpE5;_tmpE6;});}
# 843
_tmpE5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE5);
return _tmpE5;}
# 849
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 851
struct Cyc_List_List*_tmpE8=0;
for(0;dirs != 0;dirs=dirs->tl){
_tmpE8=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=(struct _dyneither_ptr*)dirs->hd;_tmpE9->tl=_tmpE8;_tmpE9;});
_tmpE8=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=({struct _dyneither_ptr*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir);_tmpEB;});_tmpEA->tl=_tmpE8;_tmpEA;});}
# 856
_tmpE8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE8);
return _tmpE8;}
# 861
static int Cyc_is_other_special(char c){
char _tmpEC=c;switch(_tmpEC){case 92: _LL26: _LL27:
 goto _LL29;case 34: _LL28: _LL29:
 goto _LL2B;case 59: _LL2A: _LL2B:
 goto _LL2D;case 38: _LL2C: _LL2D:
 goto _LL2F;case 40: _LL2E: _LL2F:
 goto _LL31;case 41: _LL30: _LL31:
 goto _LL33;case 124: _LL32: _LL33:
 goto _LL35;case 94: _LL34: _LL35:
 goto _LL37;case 60: _LL36: _LL37:
 goto _LL39;case 62: _LL38: _LL39:
 goto _LL3B;case 10: _LL3A: _LL3B:
# 876
 goto _LL3D;case 9: _LL3C: _LL3D:
 return 1;default: _LL3E: _LL3F:
 return 0;}_LL25:;}
# 883
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpED=Cyc_strlen((struct _dyneither_ptr)s);
# 887
int _tmpEE=0;
int _tmpEF=0;
{int i=0;for(0;i < _tmpED;++ i){
char _tmpF0=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpF0 == '\'')++ _tmpEE;else{
if(Cyc_is_other_special(_tmpF0))++ _tmpEF;}}}
# 896
if(_tmpEE == 0  && _tmpEF == 0)
return s;
# 900
if(_tmpEE == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmpF1[3];_tmpF1[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);_tmpF1[1]=({struct _dyneither_ptr*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=(struct _dyneither_ptr)s;_tmpF3;});_tmpF1[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF1,sizeof(struct _dyneither_ptr*),3));}));{
# 904
unsigned long _tmpF5=(_tmpED + _tmpEE)+ _tmpEF;
struct _dyneither_ptr s2=({unsigned int _tmpFF=_tmpF5 + 1;char*_tmp100=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpFF + 1));struct _dyneither_ptr _tmp102=_tag_dyneither(_tmp100,sizeof(char),_tmpFF + 1);{unsigned int _tmp101=_tmpFF;unsigned int i;for(i=0;i < _tmp101;i ++){_tmp100[i]=(char)'\000';}_tmp100[_tmp101]=(char)0;}_tmp102;});
int _tmpF6=0;
int _tmpF7=0;
for(0;_tmpF6 < _tmpED;++ _tmpF6){
char _tmpF8=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpF6));
if(_tmpF8 == '\''  || Cyc_is_other_special(_tmpF8))
({struct _dyneither_ptr _tmpF9=_dyneither_ptr_plus(s2,sizeof(char),_tmpF7 ++);char _tmpFA=*((char*)_check_dyneither_subscript(_tmpF9,sizeof(char),0));char _tmpFB='\\';if(_get_dyneither_size(_tmpF9,sizeof(char))== 1  && (_tmpFA == '\000'  && _tmpFB != '\000'))_throw_arraybounds();*((char*)_tmpF9.curr)=_tmpFB;});
({struct _dyneither_ptr _tmpFC=_dyneither_ptr_plus(s2,sizeof(char),_tmpF7 ++);char _tmpFD=*((char*)_check_dyneither_subscript(_tmpFC,sizeof(char),0));char _tmpFE=_tmpF8;if(_get_dyneither_size(_tmpFC,sizeof(char))== 1  && (_tmpFD == '\000'  && _tmpFE != '\000'))_throw_arraybounds();*((char*)_tmpFC.curr)=_tmpFE;});}
# 914
return(struct _dyneither_ptr)s2;};}
# 916
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=Cyc_sh_escape_string(*sp);_tmp103;});}
# 920
static void Cyc_process_file(struct _dyneither_ptr filename){
# 922
struct _dyneither_ptr _tmp104=Cyc_make_base_filename(filename,Cyc_output_file);
struct _dyneither_ptr _tmp105=Cyc_strconcat((struct _dyneither_ptr)_tmp104,({const char*_tmp15B=".cyp";_tag_dyneither(_tmp15B,sizeof(char),5);}));
struct _dyneither_ptr _tmp106=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmp104,({const char*_tmp15A=".cyci";_tag_dyneither(_tmp15A,sizeof(char),6);}));
struct _dyneither_ptr _tmp107=Cyc_strconcat((struct _dyneither_ptr)_tmp104,({const char*_tmp159=".cycio";_tag_dyneither(_tmp159,sizeof(char),7);}));
struct _dyneither_ptr _tmp108=Cyc_strconcat((struct _dyneither_ptr)_tmp104,({const char*_tmp158=".c";_tag_dyneither(_tmp158,sizeof(char),3);}));
struct _dyneither_ptr*_tmp109=({struct _dyneither_ptr*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157[0]=_tmp105;_tmp157;});
# 930
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename);({void*_tmp10A[1]={& _tmp10C};Cyc_fprintf(Cyc_stderr,({const char*_tmp10B="Compiling %s\n";_tag_dyneither(_tmp10B,sizeof(char),14);}),_tag_dyneither(_tmp10A,sizeof(void*),1));});});{
# 933
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp155="r";_tag_dyneither(_tmp155,sizeof(char),2);}),({const char*_tmp156="input file";_tag_dyneither(_tmp156,sizeof(char),11);}));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 940
struct _dyneither_ptr _tmp10D=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->hd=({struct _dyneither_ptr*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=(struct _dyneither_ptr)({const char*_tmp153="";_tag_dyneither(_tmp153,sizeof(char),1);});_tmp152;});_tmp151->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp151;}),({const char*_tmp154=" ";_tag_dyneither(_tmp154,sizeof(char),2);}));
# 950
struct _dyneither_ptr def_inc_path=({char*_tmp150= Cdef_inc_path;_tag_dyneither(_tmp150,sizeof(char),_get_zero_arr_size_char((void*)_tmp150,1));});
struct Cyc_List_List*_tmp10E=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
_tmp10E=Cyc_add_subdir(_tmp10E,({const char*_tmp10F="include";_tag_dyneither(_tmp10F,sizeof(char),8);}));
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0)
_tmp10E=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->hd=({struct _dyneither_ptr*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=def_inc_path;_tmp111;});_tmp110->tl=_tmp10E;_tmp110;});{
# 956
char*_tmp112= getenv("CYCLONE_INCLUDE_PATH");
if(_tmp112 != 0)
_tmp10E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({char*_tmp113=_tmp112;_tag_dyneither(_tmp113,sizeof(char),_get_zero_arr_size_char((void*)_tmp113,1));}),':'),_tmp10E);{
# 961
struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=({struct _dyneither_ptr*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=(struct _dyneither_ptr)({const char*_tmp14E="";_tag_dyneither(_tmp14E,sizeof(char),1);});_tmp14D;});_tmp14C->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp10E);_tmp14C;}),({const char*_tmp14F=" -I";_tag_dyneither(_tmp14F,sizeof(char),4);}));
# 965
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)
ofile_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0;_tmp116.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file)));({void*_tmp114[1]={& _tmp116};Cyc_aprintf(({const char*_tmp115=" > %s";_tag_dyneither(_tmp115,sizeof(char),6);}),_tag_dyneither(_tmp114,sizeof(void*),1));});});else{
# 970
ofile_string=({const char*_tmp117="";_tag_dyneither(_tmp117,sizeof(char),1);});}}else{
# 972
ofile_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmp105));({void*_tmp118[1]={& _tmp11A};Cyc_aprintf(({const char*_tmp119=" > %s";_tag_dyneither(_tmp119,sizeof(char),6);}),_tag_dyneither(_tmp118,sizeof(void*),1));});});}{
# 974
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 978
if(Cyc_dependencies_target == 0)
# 982
fixup_string=({const char*_tmp11B=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dyneither(_tmp11B,sizeof(char),35);});else{
# 987
fixup_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target)));({void*_tmp11C[1]={& _tmp11E};Cyc_aprintf(({const char*_tmp11D=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dyneither(_tmp11D,sizeof(char),29);}),_tag_dyneither(_tmp11C,sizeof(void*),1));});});}}else{
# 991
fixup_string=({const char*_tmp11F="";_tag_dyneither(_tmp11F,sizeof(char),1);});}{
# 993
struct _dyneither_ptr _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string);({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string);({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;_tmp149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 996
Cyc_sh_escape_string(filename));({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string);({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0;_tmp147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10D);({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0;_tmp146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp);({void*_tmp144[6]={& _tmp146,& _tmp147,& _tmp148,& _tmp149,& _tmp14A,& _tmp14B};Cyc_aprintf(({const char*_tmp145="%s %s%s %s%s%s";_tag_dyneither(_tmp145,sizeof(char),15);}),_tag_dyneither(_tmp144,sizeof(void*),6));});});});});});});});
# 999
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp120);({void*_tmp121[1]={& _tmp123};Cyc_fprintf(Cyc_stderr,({const char*_tmp122="%s\n";_tag_dyneither(_tmp122,sizeof(char),4);}),_tag_dyneither(_tmp121,sizeof(void*),1));});});
if( system((const char*)_check_null(_untag_dyneither_ptr(_tmp120,sizeof(char),1)))!= 0){
Cyc_compile_failure=1;
({void*_tmp124=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp125="\nError: preprocessing\n";_tag_dyneither(_tmp125,sizeof(char),23);}),_tag_dyneither(_tmp124,sizeof(void*),0));});
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmp105);
return;}
# 1006
if(Cyc_stop_after_cpp_r)return;
# 1009
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp105);{
struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmp105,({const char*_tmp142="r";_tag_dyneither(_tmp142,sizeof(char),2);}),({const char*_tmp143="file";_tag_dyneither(_tmp143,sizeof(char),5);}));
if(Cyc_compile_failure)return;{
# 1013
struct Cyc_List_List*tds=0;
# 1016
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(({const char*_tmp126="parsing";_tag_dyneither(_tmp126,sizeof(char),8);}),tds,Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),Cyc_remove_fileptr,_tmp109);
# 1018
if(Cyc_compile_failure)return;
# 1020
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}
# 1026
((int(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(({const char*_tmp127="binding";_tag_dyneither(_tmp127,sizeof(char),8);}),tds,Cyc_do_binding,1,Cyc_remove_fileptr,_tmp109);
if(Cyc_compile_failure)return;{
# 1029
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables;
{struct _RegionHandle _tmp128=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp128;_push_region(tc_rgn);
{struct Cyc_Tcenv_Tenv*_tmp129=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r){_npop_handler(0);goto PRINTC;}{
# 1035
struct _tuple22 _tmp12A=({struct _tuple22 _tmp136;_tmp136.f1=tc_rgn;_tmp136.f2=_tmp129;_tmp136;});
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(({const char*_tmp12B="type checking";_tag_dyneither(_tmp12B,sizeof(char),14);}),tds,Cyc_do_typecheck,& _tmp12A,Cyc_remove_fileptr,_tmp109);
# 1038
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1041
jump_tables=((struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(({const char*_tmp12C="jump checking";_tag_dyneither(_tmp12C,sizeof(char),14);}),tds,Cyc_do_jumpanalysis,1,Cyc_remove_fileptr,_tmp109);
# 1043
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1046
if(Cyc_tc_r){_npop_handler(0);goto PRINTC;}
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(({const char*_tmp12D="control-flow checking";_tag_dyneither(_tmp12D,sizeof(char),22);}),tds,Cyc_do_cfcheck,jump_tables,Cyc_remove_fileptr,_tmp109);
# 1049
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1052
if(Cyc_generate_interface_r){
struct Cyc___cycFILE*inter_file=Cyc_try_file_open(({const char*_tmp12E=(const char*)_check_null(_untag_dyneither_ptr(_tmp106,sizeof(char),1));_tag_dyneither(_tmp12E,sizeof(char),_get_zero_arr_size_char((void*)_tmp12E,1));}),({const char*_tmp12F="w";_tag_dyneither(_tmp12F,sizeof(char),2);}),({const char*_tmp130="interface file";_tag_dyneither(_tmp130,sizeof(char),15);}));
if(inter_file == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1058
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 1065
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(_tmp106,sizeof(char),1)),"r");
struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmp107,({const char*_tmp134="w";_tag_dyneither(_tmp134,sizeof(char),2);}),({const char*_tmp135="interface object file";_tag_dyneither(_tmp135,sizeof(char),22);}));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1072
Cyc_Position_reset_position(_tmp106);{
struct _tuple23 _tmp131=({struct _tuple23 _tmp133;_tmp133.f1=_tmp129;_tmp133.f2=inter_file;_tmp133.f3=inter_objfile;_tmp133;});
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,void(*on_fail)(struct _tuple23*),struct _tuple23*failenv))Cyc_do_stage)(({const char*_tmp132="interface checking";_tag_dyneither(_tmp132,sizeof(char),19);}),tds,Cyc_do_interface,& _tmp131,Cyc_interface_fail,& _tmp131);};}};}
# 1031
;_pop_region(tc_rgn);}
# 1078
if(Cyc_cf_r)goto PRINTC;
# 1081
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(({const char*_tmp137="translation to C";_tag_dyneither(_tmp137,sizeof(char),17);}),tds,Cyc_do_translate,jump_tables->pop_tables,Cyc_remove_fileptr,_tmp109);
# 1083
if(Cyc_compile_failure)return;
# 1086
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r)
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(({const char*_tmp138="post-pass to VC";_tag_dyneither(_tmp138,sizeof(char),16);}),tds,Cyc_do_tovc,1,(void(*)(int x))Cyc_ignore,1);
if(Cyc_compile_failure)return;
# 1090
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmp105);
# 1094
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0)
out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp139="w";_tag_dyneither(_tmp139,sizeof(char),2);}),({const char*_tmp13A="output file";_tag_dyneither(_tmp13A,sizeof(char),12);}));else{
# 1100
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0)
out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp13B="w";_tag_dyneither(_tmp13B,sizeof(char),2);}),({const char*_tmp13C="output file";_tag_dyneither(_tmp13C,sizeof(char),12);}));else{
# 1104
out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmp108,({const char*_tmp13D="w";_tag_dyneither(_tmp13D,sizeof(char),2);}),({const char*_tmp13E="output file";_tag_dyneither(_tmp13E,sizeof(char),12);}));}}
# 1106
if(Cyc_compile_failure  || !((unsigned int)out_file))return;
# 1108
if(!Cyc_noprint_r){
struct _tuple24*_tmp13F=({struct _tuple24*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=out_file;_tmp141->f2=_tmp108;_tmp141;});
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_List_List*),struct _tuple24*env,void(*on_fail)(struct _tuple24*),struct _tuple24*failenv))Cyc_do_stage)(({const char*_tmp140="printing";_tag_dyneither(_tmp140,sizeof(char),9);}),tds,Cyc_do_print,_tmp13F,Cyc_print_fail,_tmp13F);}}};};};};};};};};};}static char _tmp15C[8]="<final>";
# 1115
static struct _dyneither_ptr Cyc_final_str={_tmp15C,_tmp15C,_tmp15C + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1118
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1122
struct _dyneither_ptr basename;
{struct _handler_cons _tmp15D;_push_handler(& _tmp15D);{int _tmp15F=0;if(setjmp(_tmp15D.handler))_tmp15F=1;if(!_tmp15F){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp15E=(void*)_exn_thrown;void*_tmp160=_tmp15E;void*_tmp161;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp160)->tag == Cyc_Core_Invalid_argument){_LL41: _LL42:
# 1126
 basename=*n;goto _LL40;}else{_LL43: _tmp161=_tmp160;_LL44:(int)_rethrow(_tmp161);}_LL40:;}};}{
# 1128
struct _dyneither_ptr _tmp162=Cyc_strconcat((struct _dyneither_ptr)basename,({const char*_tmp167=".cycio";_tag_dyneither(_tmp167,sizeof(char),7);}));
struct Cyc___cycFILE*_tmp163=Cyc_try_file_open((struct _dyneither_ptr)_tmp162,({const char*_tmp165="rb";_tag_dyneither(_tmp165,sizeof(char),3);}),({const char*_tmp166="interface object file";_tag_dyneither(_tmp166,sizeof(char),22);}));
if(_tmp163 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
 exit(1);}
# 1135
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp162);{
# 1137
struct Cyc_Interface_I*_tmp164=Cyc_Interface_load(_tmp163);
Cyc_file_close(_tmp163);
return _tmp164;};};};}
# 1142
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1146
void GC_blacklist_warn_clear();struct _tuple25{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 1153
typedef struct _tuple25 Cyc_option_t;
# 1156
void Cyc_print_options();
# 1163
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3,& Cyc_v_r};static char _tmp169[3]="-v";static char _tmp16A[1]="";static char _tmp16B[35]="print compilation stages verbosely";struct _tuple25 Cyc_opt1_tuple={{_tmp169,_tmp169,_tmp169 + 3},0,{_tmp16A,_tmp16A,_tmp16A + 1},(void*)& Cyc_opt1_spec,{_tmp16B,_tmp16B,_tmp16B + 35}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 1165
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0,Cyc_print_version};static char _tmp16D[10]="--version";static char _tmp16E[1]="";static char _tmp16F[35]="Print version information and exit";struct _tuple25 Cyc_opt2_tuple={{_tmp16D,_tmp16D,_tmp16D + 10},0,{_tmp16E,_tmp16E,_tmp16E + 1},(void*)& Cyc_opt2_spec,{_tmp16F,_tmp16F,_tmp16F + 35}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 1168
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5,Cyc_set_output_file};static char _tmp171[3]="-o";static char _tmp172[8]=" <file>";static char _tmp173[35]="Set the output file name to <file>";struct _tuple25 Cyc_opt3_tuple={{_tmp171,_tmp171,_tmp171 + 3},0,{_tmp172,_tmp172,_tmp172 + 8},(void*)& Cyc_opt3_spec,{_tmp173,_tmp173,_tmp173 + 35}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 1171
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1,Cyc_add_cpparg};static char _tmp175[3]="-D";static char _tmp176[17]="<name>[=<value>]";static char _tmp177[32]="Pass definition to preprocessor";struct _tuple25 Cyc_opt4_tuple={{_tmp175,_tmp175,_tmp175 + 3},1,{_tmp176,_tmp176,_tmp176 + 17},(void*)& Cyc_opt4_spec,{_tmp177,_tmp177,_tmp177 + 32}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 1174
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1,Cyc_add_cyclone_exec_path};static char _tmp179[3]="-B";static char _tmp17A[7]="<file>";static char _tmp17B[60]="Add to the list of directories to search for compiler files";struct _tuple25 Cyc_opt5_tuple={{_tmp179,_tmp179,_tmp179 + 3},1,{_tmp17A,_tmp17A,_tmp17A + 7},(void*)& Cyc_opt5_spec,{_tmp17B,_tmp17B,_tmp17B + 60}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 1177
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1,Cyc_add_cpparg};static char _tmp17D[3]="-I";static char _tmp17E[6]="<dir>";static char _tmp17F[59]="Add to the list of directories to search for include files";struct _tuple25 Cyc_opt6_tuple={{_tmp17D,_tmp17D,_tmp17D + 3},1,{_tmp17E,_tmp17E,_tmp17E + 6},(void*)& Cyc_opt6_spec,{_tmp17F,_tmp17F,_tmp17F + 59}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 1180
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1,Cyc_add_ccarg};static char _tmp181[3]="-L";static char _tmp182[6]="<dir>";static char _tmp183[38]="Add to the list of directories for -l";struct _tuple25 Cyc_opt7_tuple={{_tmp181,_tmp181,_tmp181 + 3},1,{_tmp182,_tmp182,_tmp182 + 6},(void*)& Cyc_opt7_spec,{_tmp183,_tmp183,_tmp183 + 38}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 1183
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1,Cyc_add_libarg};static char _tmp185[3]="-l";static char _tmp186[10]="<libname>";static char _tmp187[13]="Library file";struct _tuple25 Cyc_opt8_tuple={{_tmp185,_tmp185,_tmp185 + 3},1,{_tmp186,_tmp186,_tmp186 + 10},(void*)& Cyc_opt8_spec,{_tmp187,_tmp187,_tmp187 + 13}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 1186
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1,Cyc_add_marg};static char _tmp189[3]="-m";static char _tmp18A[9]="<option>";static char _tmp18B[52]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple25 Cyc_opt9_tuple={{_tmp189,_tmp189,_tmp189 + 3},1,{_tmp18A,_tmp18A,_tmp18A + 9},(void*)& Cyc_opt9_spec,{_tmp18B,_tmp18B,_tmp18B + 52}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 1189
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0,Cyc_set_stop_after_objectfile};static char _tmp18D[3]="-c";static char _tmp18E[1]="";static char _tmp18F[61]="Produce an object file instead of an executable; do not link";struct _tuple25 Cyc_opt10_tuple={{_tmp18D,_tmp18D,_tmp18D + 3},0,{_tmp18E,_tmp18E,_tmp18E + 1},(void*)& Cyc_opt10_spec,{_tmp18F,_tmp18F,_tmp18F + 61}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1192
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5,Cyc_set_inputtype};static char _tmp191[3]="-x";static char _tmp192[12]=" <language>";static char _tmp193[49]="Specify <language> for the following input files";struct _tuple25 Cyc_opt11_tuple={{_tmp191,_tmp191,_tmp191 + 3},0,{_tmp192,_tmp192,_tmp192 + 12},(void*)& Cyc_opt11_spec,{_tmp193,_tmp193,_tmp193 + 49}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1195
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0,Cyc_set_pa};static char _tmp195[4]="-pa";static char _tmp196[1]="";static char _tmp197[33]="Compile for profiling with aprof";struct _tuple25 Cyc_opt12_tuple={{_tmp195,_tmp195,_tmp195 + 4},0,{_tmp196,_tmp196,_tmp196 + 1},(void*)& Cyc_opt12_spec,{_tmp197,_tmp197,_tmp197 + 33}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1198
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0,Cyc_set_stop_after_asmfile};static char _tmp199[3]="-S";static char _tmp19A[1]="";static char _tmp19B[35]="Stop after producing assembly code";struct _tuple25 Cyc_opt13_tuple={{_tmp199,_tmp199,_tmp199 + 3},0,{_tmp19A,_tmp19A,_tmp19A + 1},(void*)& Cyc_opt13_spec,{_tmp19B,_tmp19B,_tmp19B + 35}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1201
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0,Cyc_set_produce_dependencies};static char _tmp19D[3]="-M";static char _tmp19E[1]="";static char _tmp19F[21]="Produce dependencies";struct _tuple25 Cyc_opt14_tuple={{_tmp19D,_tmp19D,_tmp19D + 3},0,{_tmp19E,_tmp19E,_tmp19E + 1},(void*)& Cyc_opt14_spec,{_tmp19F,_tmp19F,_tmp19F + 21}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1204
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1,Cyc_add_cpparg};static char _tmp1A1[4]="-MG";static char _tmp1A2[1]="";static char _tmp1A3[68]="When producing dependencies assume that missing files are generated";struct _tuple25 Cyc_opt15_tuple={{_tmp1A1,_tmp1A1,_tmp1A1 + 4},0,{_tmp1A2,_tmp1A2,_tmp1A2 + 1},(void*)& Cyc_opt15_spec,{_tmp1A3,_tmp1A3,_tmp1A3 + 68}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1208
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5,Cyc_set_dependencies_target};static char _tmp1A5[4]="-MT";static char _tmp1A6[10]=" <target>";static char _tmp1A7[29]="Give target for dependencies";struct _tuple25 Cyc_opt16_tuple={{_tmp1A5,_tmp1A5,_tmp1A5 + 4},0,{_tmp1A6,_tmp1A6,_tmp1A6 + 10},(void*)& Cyc_opt16_spec,{_tmp1A7,_tmp1A7,_tmp1A7 + 29}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1211
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5,Cyc_set_target_arch};static char _tmp1A9[3]="-b";static char _tmp1AA[10]="<machine>";static char _tmp1AB[19]="Set target machine";struct _tuple25 Cyc_opt17_tuple={{_tmp1A9,_tmp1A9,_tmp1A9 + 3},0,{_tmp1AA,_tmp1AA,_tmp1AA + 10},(void*)& Cyc_opt17_spec,{_tmp1AB,_tmp1AB,_tmp1AB + 19}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1214
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3,& Cyc_NewControlFlow_warn_lose_unique};static char _tmp1AD[14]="-Wlose-unique";static char _tmp1AE[1]="";static char _tmp1AF[49]="Try to warn when a unique pointer might get lost";struct _tuple25 Cyc_opt18_tuple={{_tmp1AD,_tmp1AD,_tmp1AD + 14},0,{_tmp1AE,_tmp1AE,_tmp1AE + 1},(void*)& Cyc_opt18_spec,{_tmp1AF,_tmp1AF,_tmp1AF + 49}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1217
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3,& Cyc_Binding_warn_override};static char _tmp1B1[11]="-Woverride";static char _tmp1B2[1]="";static char _tmp1B3[58]="Warn when a local variable overrides an existing variable";struct _tuple25 Cyc_opt19_tuple={{_tmp1B1,_tmp1B1,_tmp1B1 + 11},0,{_tmp1B2,_tmp1B2,_tmp1B2 + 1},(void*)& Cyc_opt19_spec,{_tmp1B3,_tmp1B3,_tmp1B3 + 58}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1220
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0,Cyc_set_all_warnings};static char _tmp1B5[6]="-Wall";static char _tmp1B6[1]="";static char _tmp1B7[21]="Turn on all warnings";struct _tuple25 Cyc_opt20_tuple={{_tmp1B5,_tmp1B5,_tmp1B5 + 6},0,{_tmp1B6,_tmp1B6,_tmp1B6 + 1},(void*)& Cyc_opt20_spec,{_tmp1B7,_tmp1B7,_tmp1B7 + 21}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1223
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt21_spec={3,& Cyc_stop_after_cpp_r};static char _tmp1B9[3]="-E";static char _tmp1BA[1]="";static char _tmp1BB[25]="Stop after preprocessing";struct _tuple25 Cyc_opt21_tuple={{_tmp1B9,_tmp1B9,_tmp1B9 + 3},0,{_tmp1BA,_tmp1BA,_tmp1BA + 1},(void*)& Cyc_opt21_spec,{_tmp1BB,_tmp1BB,_tmp1BB + 25}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1226
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt22_spec={3,& Cyc_parseonly_r};static char _tmp1BD[17]="-stopafter-parse";static char _tmp1BE[1]="";static char _tmp1BF[19]="Stop after parsing";struct _tuple25 Cyc_opt22_tuple={{_tmp1BD,_tmp1BD,_tmp1BD + 17},0,{_tmp1BE,_tmp1BE,_tmp1BE + 1},(void*)& Cyc_opt22_spec,{_tmp1BF,_tmp1BF,_tmp1BF + 19}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1229
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt23_spec={3,& Cyc_tc_r};static char _tmp1C1[14]="-stopafter-tc";static char _tmp1C2[1]="";static char _tmp1C3[25]="Stop after type checking";struct _tuple25 Cyc_opt23_tuple={{_tmp1C1,_tmp1C1,_tmp1C1 + 14},0,{_tmp1C2,_tmp1C2,_tmp1C2 + 1},(void*)& Cyc_opt23_spec,{_tmp1C3,_tmp1C3,_tmp1C3 + 25}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1232
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3,& Cyc_noprint_r};static char _tmp1C5[9]="-noprint";static char _tmp1C6[1]="";static char _tmp1C7[42]="Do not print output (when stopping early)";struct _tuple25 Cyc_opt24_tuple={{_tmp1C5,_tmp1C5,_tmp1C5 + 9},0,{_tmp1C6,_tmp1C6,_tmp1C6 + 1},(void*)& Cyc_opt24_spec,{_tmp1C7,_tmp1C7,_tmp1C7 + 42}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1235
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt25_spec={3,& Cyc_cf_r};static char _tmp1C9[14]="-stopafter-cf";static char _tmp1CA[1]="";static char _tmp1CB[33]="Stop after control-flow checking";struct _tuple25 Cyc_opt25_tuple={{_tmp1C9,_tmp1C9,_tmp1C9 + 14},0,{_tmp1CA,_tmp1CA,_tmp1CA + 1},(void*)& Cyc_opt25_spec,{_tmp1CB,_tmp1CB,_tmp1CB + 33}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1238
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt26_spec={3,& Cyc_toc_r};static char _tmp1CD[15]="-stopafter-toc";static char _tmp1CE[1]="";static char _tmp1CF[28]="Stop after translation to C";struct _tuple25 Cyc_opt26_tuple={{_tmp1CD,_tmp1CD,_tmp1CD + 15},0,{_tmp1CE,_tmp1CE,_tmp1CE + 1},(void*)& Cyc_opt26_spec,{_tmp1CF,_tmp1CF,_tmp1CF + 28}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1241
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3,& Cyc_ic_r};static char _tmp1D1[4]="-ic";static char _tmp1D2[1]="";static char _tmp1D3[26]="Activate the link-checker";struct _tuple25 Cyc_opt27_tuple={{_tmp1D1,_tmp1D1,_tmp1D1 + 4},0,{_tmp1D2,_tmp1D2,_tmp1D2 + 1},(void*)& Cyc_opt27_spec,{_tmp1D3,_tmp1D3,_tmp1D3 + 26}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1244
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3,& Cyc_pp_r};static char _tmp1D5[4]="-pp";static char _tmp1D6[1]="";static char _tmp1D7[47]="Pretty print the C code that Cyclone generates";struct _tuple25 Cyc_opt28_tuple={{_tmp1D5,_tmp1D5,_tmp1D5 + 4},0,{_tmp1D6,_tmp1D6,_tmp1D6 + 1},(void*)& Cyc_opt28_spec,{_tmp1D7,_tmp1D7,_tmp1D7 + 47}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1247
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4,& Cyc_pp_r};static char _tmp1D9[4]="-up";static char _tmp1DA[1]="";static char _tmp1DB[55]="Ugly print the C code that Cyclone generates (default)";struct _tuple25 Cyc_opt29_tuple={{_tmp1D9,_tmp1D9,_tmp1D9 + 4},0,{_tmp1DA,_tmp1DA,_tmp1DA + 1},(void*)& Cyc_opt29_spec,{_tmp1DB,_tmp1DB,_tmp1DB + 55}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1250
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3,& Cyc_elim_se_r};static char _tmp1DD[28]="-elim-statement-expressions";static char _tmp1DE[1]="";static char _tmp1DF[66]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple25 Cyc_opt30_tuple={{_tmp1DD,_tmp1DD,_tmp1DD + 28},0,{_tmp1DE,_tmp1DE,_tmp1DE + 1},(void*)& Cyc_opt30_spec,{_tmp1DF,_tmp1DF,_tmp1DF + 66}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1254
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4,& Cyc_elim_se_r};static char _tmp1E1[31]="-no-elim-statement-expressions";static char _tmp1E2[1]="";static char _tmp1E3[47]="Do not avoid statement expressions in C output";struct _tuple25 Cyc_opt31_tuple={{_tmp1E1,_tmp1E1,_tmp1E1 + 31},0,{_tmp1E2,_tmp1E2,_tmp1E2 + 1},(void*)& Cyc_opt31_spec,{_tmp1E3,_tmp1E3,_tmp1E3 + 47}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1257
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0,Cyc_set_tovc};static char _tmp1E5[8]="-un-gcc";static char _tmp1E6[1]="";static char _tmp1E7[57]="Avoid gcc extensions in C output (except for attributes)";struct _tuple25 Cyc_opt32_tuple={{_tmp1E5,_tmp1E5,_tmp1E5 + 8},0,{_tmp1E6,_tmp1E6,_tmp1E6 + 1},(void*)& Cyc_opt32_spec,{_tmp1E7,_tmp1E7,_tmp1E7 + 57}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1260
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5,Cyc_set_c_compiler};static char _tmp1E9[8]="-c-comp";static char _tmp1EA[12]=" <compiler>";static char _tmp1EB[40]="Produce C output for the given compiler";struct _tuple25 Cyc_opt33_tuple={{_tmp1E9,_tmp1E9,_tmp1E9 + 8},0,{_tmp1EA,_tmp1EA,_tmp1EA + 12},(void*)& Cyc_opt33_spec,{_tmp1EB,_tmp1EB,_tmp1EB + 40}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1263
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0,Cyc_set_save_temps};static char _tmp1ED[12]="-save-temps";static char _tmp1EE[1]="";static char _tmp1EF[29]="Don't delete temporary files";struct _tuple25 Cyc_opt34_tuple={{_tmp1ED,_tmp1ED,_tmp1ED + 12},0,{_tmp1EE,_tmp1EE,_tmp1EE + 1},(void*)& Cyc_opt34_spec,{_tmp1EF,_tmp1EF,_tmp1EF + 29}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1266
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3,& Cyc_save_c_r};static char _tmp1F1[8]="-save-c";static char _tmp1F2[1]="";static char _tmp1F3[31]="Don't delete temporary C files";struct _tuple25 Cyc_opt35_tuple={{_tmp1F1,_tmp1F1,_tmp1F1 + 8},0,{_tmp1F2,_tmp1F2,_tmp1F2 + 1},(void*)& Cyc_opt35_spec,{_tmp1F3,_tmp1F3,_tmp1F3 + 31}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1269
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0,Cyc_clear_lineno};static char _tmp1F5[11]="--nolineno";static char _tmp1F6[1]="";static char _tmp1F7[63]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple25 Cyc_opt36_tuple={{_tmp1F5,_tmp1F5,_tmp1F5 + 11},0,{_tmp1F6,_tmp1F6,_tmp1F6 + 1},(void*)& Cyc_opt36_spec,{_tmp1F7,_tmp1F7,_tmp1F7 + 63}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1272
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0,Cyc_set_nochecks};static char _tmp1F9[11]="--nochecks";static char _tmp1FA[1]="";static char _tmp1FB[27]="Disable bounds/null checks";struct _tuple25 Cyc_opt37_tuple={{_tmp1F9,_tmp1F9,_tmp1F9 + 11},0,{_tmp1FA,_tmp1FA,_tmp1FA + 1},(void*)& Cyc_opt37_spec,{_tmp1FB,_tmp1FB,_tmp1FB + 27}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1275
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0,Cyc_set_nonullchecks};static char _tmp1FD[15]="--nonullchecks";static char _tmp1FE[1]="";static char _tmp1FF[20]="Disable null checks";struct _tuple25 Cyc_opt38_tuple={{_tmp1FD,_tmp1FD,_tmp1FD + 15},0,{_tmp1FE,_tmp1FE,_tmp1FE + 1},(void*)& Cyc_opt38_spec,{_tmp1FF,_tmp1FF,_tmp1FF + 20}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1278
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0,Cyc_set_noboundschecks};static char _tmp201[17]="--noboundschecks";static char _tmp202[1]="";static char _tmp203[22]="Disable bounds checks";struct _tuple25 Cyc_opt39_tuple={{_tmp201,_tmp201,_tmp201 + 17},0,{_tmp202,_tmp202,_tmp202 + 1},(void*)& Cyc_opt39_spec,{_tmp203,_tmp203,_tmp203 + 22}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1281
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt40_spec={0,Cyc_set_inline_functions};static char _tmp205[16]="--inline-checks";static char _tmp206[1]="";static char _tmp207[40]="Inline bounds checks instead of #define";struct _tuple25 Cyc_opt40_tuple={{_tmp205,_tmp205,_tmp205 + 16},0,{_tmp206,_tmp206,_tmp206 + 1},(void*)& Cyc_opt40_spec,{_tmp207,_tmp207,_tmp207 + 40}};struct Cyc_List_List Cyc_opt40={(void*)& Cyc_opt40_tuple,0};
# 1284
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5,Cyc_set_cpp};static char _tmp209[9]="-use-cpp";static char _tmp20A[7]="<path>";static char _tmp20B[35]="Indicate which preprocessor to use";struct _tuple25 Cyc_opt41_tuple={{_tmp209,_tmp209,_tmp209 + 9},0,{_tmp20A,_tmp20A,_tmp20A + 7},(void*)& Cyc_opt41_spec,{_tmp20B,_tmp20B,_tmp20B + 35}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1287
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0,Cyc_set_nocppprecomp};static char _tmp20D[16]="-no-cpp-precomp";static char _tmp20E[1]="";static char _tmp20F[40]="Don't do smart preprocessing (mac only)";struct _tuple25 Cyc_opt42_tuple={{_tmp20D,_tmp20D,_tmp20D + 16},0,{_tmp20E,_tmp20E,_tmp20E + 1},(void*)& Cyc_opt42_spec,{_tmp20F,_tmp20F,_tmp20F + 40}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1290
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0,Cyc_set_nocyc};static char _tmp211[7]="-nocyc";static char _tmp212[1]="";static char _tmp213[33]="Don't add implicit namespace Cyc";struct _tuple25 Cyc_opt43_tuple={{_tmp211,_tmp211,_tmp211 + 7},0,{_tmp212,_tmp212,_tmp212 + 1},(void*)& Cyc_opt43_spec,{_tmp213,_tmp213,_tmp213 + 33}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1293
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3,& Cyc_nogc_r};static char _tmp215[6]="-nogc";static char _tmp216[1]="";static char _tmp217[36]="Don't link in the garbage collector";struct _tuple25 Cyc_opt44_tuple={{_tmp215,_tmp215,_tmp215 + 6},0,{_tmp216,_tmp216,_tmp216 + 1},(void*)& Cyc_opt44_spec,{_tmp217,_tmp217,_tmp217 + 36}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1296
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3,& Cyc_noshake_r};static char _tmp219[16]="-noremoveunused";static char _tmp21A[1]="";static char _tmp21B[49]="Don't remove externed variables that aren't used";struct _tuple25 Cyc_opt45_tuple={{_tmp219,_tmp219,_tmp219 + 16},0,{_tmp21A,_tmp21A,_tmp21A + 1},(void*)& Cyc_opt45_spec,{_tmp21B,_tmp21B,_tmp21B + 49}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1299
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3,& Cyc_noexpand_r};static char _tmp21D[18]="-noexpandtypedefs";static char _tmp21E[1]="";static char _tmp21F[41]="Don't expand typedefs in pretty printing";struct _tuple25 Cyc_opt46_tuple={{_tmp21D,_tmp21D,_tmp21D + 18},0,{_tmp21E,_tmp21E,_tmp21E + 1},(void*)& Cyc_opt46_spec,{_tmp21F,_tmp21F,_tmp21F + 41}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1302
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3,& Cyc_print_all_tvars_r};static char _tmp221[15]="-printalltvars";static char _tmp222[1]="";static char _tmp223[57]="Print all type variables (even implicit default effects)";struct _tuple25 Cyc_opt47_tuple={{_tmp221,_tmp221,_tmp221 + 15},0,{_tmp222,_tmp222,_tmp222 + 1},(void*)& Cyc_opt47_spec,{_tmp223,_tmp223,_tmp223 + 57}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1305
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3,& Cyc_print_all_kinds_r};static char _tmp225[15]="-printallkinds";static char _tmp226[1]="";static char _tmp227[37]="Always print kinds of type variables";struct _tuple25 Cyc_opt48_tuple={{_tmp225,_tmp225,_tmp225 + 15},0,{_tmp226,_tmp226,_tmp226 + 1},(void*)& Cyc_opt48_spec,{_tmp227,_tmp227,_tmp227 + 37}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1308
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3,& Cyc_print_full_evars_r};static char _tmp229[16]="-printfullevars";static char _tmp22A[1]="";static char _tmp22B[50]="Print full information for evars (type debugging)";struct _tuple25 Cyc_opt49_tuple={{_tmp229,_tmp229,_tmp229 + 16},0,{_tmp22A,_tmp22A,_tmp22A + 1},(void*)& Cyc_opt49_spec,{_tmp22B,_tmp22B,_tmp22B + 50}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1311
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3,& Cyc_print_all_effects_r};static char _tmp22D[17]="-printalleffects";static char _tmp22E[1]="";static char _tmp22F[45]="Print effects for functions (type debugging)";struct _tuple25 Cyc_opt50_tuple={{_tmp22D,_tmp22D,_tmp22D + 17},0,{_tmp22E,_tmp22E,_tmp22E + 1},(void*)& Cyc_opt50_spec,{_tmp22F,_tmp22F,_tmp22F + 45}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1314
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3,& Cyc_nocyc_setjmp_r};static char _tmp231[14]="-nocyc_setjmp";static char _tmp232[1]="";static char _tmp233[46]="Do not use compiler special file cyc_setjmp.h";struct _tuple25 Cyc_opt51_tuple={{_tmp231,_tmp231,_tmp231 + 14},0,{_tmp232,_tmp232,_tmp232 + 1},(void*)& Cyc_opt51_spec,{_tmp233,_tmp233,_tmp233 + 46}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1317
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3,& Cyc_Lex_compile_for_boot_r};static char _tmp235[18]="-compile-for-boot";static char _tmp236[1]="";static char _tmp237[71]="Compile using the special boot library instead of the standard library";struct _tuple25 Cyc_opt52_tuple={{_tmp235,_tmp235,_tmp235 + 18},0,{_tmp236,_tmp236,_tmp236 + 1},(void*)& Cyc_opt52_spec,{_tmp237,_tmp237,_tmp237 + 71}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1320
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5,Cyc_set_cyc_include};static char _tmp239[4]="-CI";static char _tmp23A[8]=" <file>";static char _tmp23B[31]="Set cyc_include.h to be <file>";struct _tuple25 Cyc_opt53_tuple={{_tmp239,_tmp239,_tmp239 + 4},0,{_tmp23A,_tmp23A,_tmp23A + 8},(void*)& Cyc_opt53_spec,{_tmp23B,_tmp23B,_tmp23B + 31}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1323
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3,& Cyc_Toc_warn_bounds_checks};static char _tmp23D[18]="-warnboundschecks";static char _tmp23E[1]="";static char _tmp23F[44]="Warn when bounds checks can't be eliminated";struct _tuple25 Cyc_opt54_tuple={{_tmp23D,_tmp23D,_tmp23D + 18},0,{_tmp23E,_tmp23E,_tmp23E + 1},(void*)& Cyc_opt54_spec,{_tmp23F,_tmp23F,_tmp23F + 44}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1326
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3,& Cyc_Toc_warn_all_null_deref};static char _tmp241[16]="-warnnullchecks";static char _tmp242[1]="";static char _tmp243[45]="Warn when any null check can't be eliminated";struct _tuple25 Cyc_opt55_tuple={{_tmp241,_tmp241,_tmp241 + 16},0,{_tmp242,_tmp242,_tmp242 + 1},(void*)& Cyc_opt55_spec,{_tmp243,_tmp243,_tmp243 + 45}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1329
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3,& Cyc_Tcutil_warn_alias_coerce};static char _tmp245[19]="-warnaliascoercion";static char _tmp246[1]="";static char _tmp247[41]="Warn when any alias coercion is inserted";struct _tuple25 Cyc_opt56_tuple={{_tmp245,_tmp245,_tmp245 + 19},0,{_tmp246,_tmp246,_tmp246 + 1},(void*)& Cyc_opt56_spec,{_tmp247,_tmp247,_tmp247 + 41}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1332
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3,& Cyc_Tcutil_warn_region_coerce};static char _tmp249[18]="-warnregioncoerce";static char _tmp24A[1]="";static char _tmp24B[42]="Warn when any region coercion is inserted";struct _tuple25 Cyc_opt57_tuple={{_tmp249,_tmp249,_tmp249 + 18},0,{_tmp24A,_tmp24A,_tmp24A + 1},(void*)& Cyc_opt57_spec,{_tmp24B,_tmp24B,_tmp24B + 42}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1335
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3,& Cyc_Parse_no_register};static char _tmp24D[12]="-noregister";static char _tmp24E[1]="";static char _tmp24F[39]="Treat register storage class as public";struct _tuple25 Cyc_opt58_tuple={{_tmp24D,_tmp24D,_tmp24D + 12},0,{_tmp24E,_tmp24E,_tmp24E + 1},(void*)& Cyc_opt58_spec,{_tmp24F,_tmp24F,_tmp24F + 39}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1338
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4,& Cyc_Position_use_gcc_style_location};static char _tmp251[18]="-detailedlocation";static char _tmp252[1]="";static char _tmp253[58]="Try to give more detailed location information for errors";struct _tuple25 Cyc_opt59_tuple={{_tmp251,_tmp251,_tmp251 + 18},0,{_tmp252,_tmp252,_tmp252 + 1},(void*)& Cyc_opt59_spec,{_tmp253,_tmp253,_tmp253 + 58}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1341
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0,Cyc_set_port_c_code};static char _tmp255[6]="-port";static char _tmp256[1]="";static char _tmp257[38]="Suggest how to port C code to Cyclone";struct _tuple25 Cyc_opt60_tuple={{_tmp255,_tmp255,_tmp255 + 6},0,{_tmp256,_tmp256,_tmp256 + 1},(void*)& Cyc_opt60_spec,{_tmp257,_tmp257,_tmp257 + 38}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1344
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3,& Cyc_Absyn_no_regions};static char _tmp259[11]="-noregions";static char _tmp25A[1]="";static char _tmp25B[39]="Generate code that doesn't use regions";struct _tuple25 Cyc_opt61_tuple={{_tmp259,_tmp259,_tmp259 + 11},0,{_tmp25A,_tmp25A,_tmp25A + 1},(void*)& Cyc_opt61_spec,{_tmp25B,_tmp25B,_tmp25B + 39}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1347
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5,Cyc_add_iprefix};static char _tmp25D[9]="-iprefix";static char _tmp25E[9]="<prefix>";static char _tmp25F[67]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple25 Cyc_opt62_tuple={{_tmp25D,_tmp25D,_tmp25D + 9},0,{_tmp25E,_tmp25E,_tmp25E + 9},(void*)& Cyc_opt62_spec,{_tmp25F,_tmp25F,_tmp25F + 67}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1350
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5,Cyc_add_iwithprefix};static char _tmp261[13]="-iwithprefix";static char _tmp262[6]="<dir>";static char _tmp263[47]="Add <prefix>/<dir> to the second include path.";struct _tuple25 Cyc_opt63_tuple={{_tmp261,_tmp261,_tmp261 + 13},0,{_tmp262,_tmp262,_tmp262 + 6},(void*)& Cyc_opt63_spec,{_tmp263,_tmp263,_tmp263 + 47}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1353
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5,Cyc_add_iwithprefixbefore};static char _tmp265[19]="-iwithprefixbefore";static char _tmp266[6]="<dir>";static char _tmp267[45]="Add <prefix>/<dir> to the main include path.";struct _tuple25 Cyc_opt64_tuple={{_tmp265,_tmp265,_tmp265 + 19},0,{_tmp266,_tmp266,_tmp266 + 6},(void*)& Cyc_opt64_spec,{_tmp267,_tmp267,_tmp267 + 45}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1356
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5,Cyc_add_isystem};static char _tmp269[9]="-isystem";static char _tmp26A[6]="<dir>";static char _tmp26B[90]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple25 Cyc_opt65_tuple={{_tmp269,_tmp269,_tmp269 + 9},0,{_tmp26A,_tmp26A,_tmp26A + 6},(void*)& Cyc_opt65_spec,{_tmp26B,_tmp26B,_tmp26B + 90}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1359
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5,Cyc_add_idirafter};static char _tmp26D[11]="-idirafter";static char _tmp26E[6]="<dir>";static char _tmp26F[46]="Add the directory to the second include path.";struct _tuple25 Cyc_opt66_tuple={{_tmp26D,_tmp26D,_tmp26D + 11},0,{_tmp26E,_tmp26E,_tmp26E + 6},(void*)& Cyc_opt66_spec,{_tmp26F,_tmp26F,_tmp26F + 46}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1362
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3,& Cyc_generate_interface_r};static char _tmp271[15]="--geninterface";static char _tmp272[1]="";static char _tmp273[25]="Generate interface files";struct _tuple25 Cyc_opt67_tuple={{_tmp271,_tmp271,_tmp271 + 15},0,{_tmp272,_tmp272,_tmp272 + 1},(void*)& Cyc_opt67_spec,{_tmp273,_tmp273,_tmp273 + 25}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1365
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5,Cyc_set_specified_interface};static char _tmp275[12]="--interface";static char _tmp276[8]=" <file>";static char _tmp277[37]="Set the interface file to be <file>.";struct _tuple25 Cyc_opt68_tuple={{_tmp275,_tmp275,_tmp275 + 12},0,{_tmp276,_tmp276,_tmp276 + 8},(void*)& Cyc_opt68_spec,{_tmp277,_tmp277,_tmp277 + 37}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1368
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0,Cyc_set_many_errors};static char _tmp279[13]="--manyerrors";static char _tmp27A[1]="";static char _tmp27B[36]="don't stop after only a few errors.";struct _tuple25 Cyc_opt69_tuple={{_tmp279,_tmp279,_tmp279 + 13},0,{_tmp27A,_tmp27A,_tmp27A + 1},(void*)& Cyc_opt69_spec,{_tmp27B,_tmp27B,_tmp27B + 36}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1371
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3,& Cyc_ParseErrors_print_state_and_token};static char _tmp27D[13]="--parsestate";static char _tmp27E[1]="";static char _tmp27F[50]="print the parse state and token on a syntax error";struct _tuple25 Cyc_opt70_tuple={{_tmp27D,_tmp27D,_tmp27D + 13},0,{_tmp27E,_tmp27E,_tmp27E + 1},(void*)& Cyc_opt70_spec,{_tmp27F,_tmp27F,_tmp27F + 50}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1374
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0,Cyc_noassume_gcc_flag};static char _tmp281[22]="-known-gcc-flags-only";static char _tmp282[1]="";static char _tmp283[57]="do not assume that unknown flags should be passed to gcc";struct _tuple25 Cyc_opt71_tuple={{_tmp281,_tmp281,_tmp281 + 22},0,{_tmp282,_tmp282,_tmp282 + 1},(void*)& Cyc_opt71_spec,{_tmp283,_tmp283,_tmp283 + 57}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1377
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0,Cyc_print_options};static char _tmp285[6]="-help";static char _tmp286[1]="";static char _tmp287[32]="print out the available options";struct _tuple25 Cyc_opt72_tuple={{_tmp285,_tmp285,_tmp285 + 6},0,{_tmp286,_tmp286,_tmp286 + 1},(void*)& Cyc_opt72_spec,{_tmp287,_tmp287,_tmp287 + 32}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1380
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0,Cyc_print_options};static char _tmp289[7]="-usage";static char _tmp28A[1]="";static char _tmp28B[32]="print out the available options";struct _tuple25 Cyc_opt73_tuple={{_tmp289,_tmp289,_tmp289 + 7},0,{_tmp28A,_tmp28A,_tmp28A + 1},(void*)& Cyc_opt73_spec,{_tmp28B,_tmp28B,_tmp28B + 32}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1384
struct Cyc_List_List*Cyc_global_options[73]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt40,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73};
# 1395
void Cyc_print_options(){
Cyc_Arg_usage(Cyc_global_options[0],({const char*_tmp28C="<program.cyc>";_tag_dyneither(_tmp28C,sizeof(char),14);}));}
# 1400
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1405
 GC_blacklist_warn_clear();{
# 1407
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=({char*_tmp31D= Ccomp;_tag_dyneither(_tmp31D,sizeof(char),_get_zero_arr_size_char((void*)_tmp31D,1));});
# 1412
struct _RegionHandle _tmp28D=_new_region("r");struct _RegionHandle*r=& _tmp28D;_push_region(r);{
# 1414
struct _dyneither_ptr _tmp28E=({unsigned int _tmp2BB=(unsigned int)argc;int*_tmp2BC=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp2BB));struct _dyneither_ptr _tmp2BE=_tag_dyneither(_tmp2BC,sizeof(int),_tmp2BB);{unsigned int _tmp2BD=_tmp2BB;unsigned int i;for(i=0;i < _tmp2BD;i ++){_tmp2BC[i]=(int)0;}}_tmp2BE;});
int _tmp28F=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
if(Cyc_strncmp(({const char*_tmp290="-B";_tag_dyneither(_tmp290,sizeof(char),3);}),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp28E,sizeof(int),i))=1;
++ _tmp28F;}else{
# 1422
if(Cyc_strcmp(({const char*_tmp291="-b";_tag_dyneither(_tmp291,sizeof(char),3);}),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp28E,sizeof(int),i))=1;
++ _tmp28F;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp28E,sizeof(int),i))=1;
++ _tmp28F;}}}{
# 1434
struct _dyneither_ptr _tmp292=({unsigned int _tmp2B7=(unsigned int)(_tmp28F + 1);struct _dyneither_ptr*_tmp2B8=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp2B7));struct _dyneither_ptr _tmp2BA=_tag_dyneither(_tmp2B8,sizeof(struct _dyneither_ptr),_tmp2B7);{unsigned int _tmp2B9=_tmp2B7;unsigned int n;for(n=0;n < _tmp2B9;n ++){_tmp2B8[n]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}_tmp2BA;});
struct _dyneither_ptr _tmp293=({unsigned int _tmp2B3=(unsigned int)(argc - _tmp28F);struct _dyneither_ptr*_tmp2B4=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp2B3));struct _dyneither_ptr _tmp2B6=_tag_dyneither(_tmp2B4,sizeof(struct _dyneither_ptr),_tmp2B3);{unsigned int _tmp2B5=_tmp2B3;unsigned int n;for(n=0;n < _tmp2B5;n ++){_tmp2B4[n]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}_tmp2B6;});
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp292,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp293,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp28E,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp292,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp293,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}
# 1443
{int i=1;for(0;i < 73;++ i){
(Cyc_global_options[_check_known_subscript_notnull(73,i - 1)])->tl=Cyc_global_options[i];}}{
# 1446
struct Cyc_List_List*options=Cyc_global_options[0];
# 1448
struct _dyneither_ptr optstring=({const char*_tmp2B2="Options:";_tag_dyneither(_tmp2B2,sizeof(char),9);});
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp292);
# 1453
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=({char*_tmp294= Carch;_tag_dyneither(_tmp294,sizeof(char),_get_zero_arr_size_char((void*)_tmp294,1));});{
# 1461
struct _dyneither_ptr cyclone_exec_prefix=({char*_tmp2B1= getenv("CYCLONE_EXEC_PREFIX");_tag_dyneither(_tmp2B1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B1,1));});
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->hd=({struct _dyneither_ptr*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=cyclone_exec_prefix;_tmp296;});_tmp295->tl=Cyc_cyclone_exec_path;_tmp295;});
def_lib_path=({char*_tmp297= Cdef_lib_path;_tag_dyneither(_tmp297,sizeof(char),_get_zero_arr_size_char((void*)_tmp297,1));});
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->hd=({struct _dyneither_ptr*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299[0]=(struct _dyneither_ptr)
Cyc_Filename_concat(def_lib_path,({const char*_tmp29A="cyc-lib";_tag_dyneither(_tmp29A,sizeof(char),8);}));_tmp299;});_tmp298->tl=Cyc_cyclone_exec_path;_tmp298;});
# 1469
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1477
struct _dyneither_ptr _tmp29B=Cyc_do_find(cyclone_arch_path,({const char*_tmp2B0="cycspecs";_tag_dyneither(_tmp2B0,sizeof(char),9);}));
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp29B);({void*_tmp29C[1]={& _tmp29E};Cyc_fprintf(Cyc_stderr,({const char*_tmp29D="Reading from specs file %s\n";_tag_dyneither(_tmp29D,sizeof(char),28);}),_tag_dyneither(_tmp29C,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp29F=Cyc_read_specs(_tmp29B);
struct _dyneither_ptr _tmp2A0=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp2A1;_push_handler(& _tmp2A1);{int _tmp2A3=0;if(setjmp(_tmp2A1.handler))_tmp2A3=1;if(!_tmp2A3){
{struct _dyneither_ptr _tmp2A4=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp29F,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp2A0=Cyc_split_specs(_tmp2A4);}
# 1482
;_pop_handler();}else{void*_tmp2A2=(void*)_exn_thrown;void*_tmp2A6=_tmp2A2;void*_tmp2A7;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2A6)->tag == Cyc_Core_Not_found){_LL46: _LL47:
# 1485
 goto _LL45;}else{_LL48: _tmp2A7=_tmp2A6;_LL49:(int)_rethrow(_tmp2A7);}_LL45:;}};}
# 1487
if((struct _dyneither_ptr*)_tmp2A0.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp2A0);}
# 1492
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp293);
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,({const char*_tmp2A8="";_tag_dyneither(_tmp2A8,sizeof(char),1);}))== 0){
# 1510 "cyclone.cyc"
const char*_tmp2A9=Cyc_produce_dependencies?"":" -E";
Cyc_set_cpp((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp29B);({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0;_tmp2AD.f1=(struct _dyneither_ptr)({const char*_tmp2AF=_tmp2A9;_tag_dyneither(_tmp2AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AF,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp);({void*_tmp2AA[3]={& _tmp2AC,& _tmp2AD,& _tmp2AE};Cyc_aprintf(({const char*_tmp2AB="%s -w -x c%s -specs %s";_tag_dyneither(_tmp2AB,sizeof(char),23);}),_tag_dyneither(_tmp2AA,sizeof(void*),3));});});});}));}};};};};};}
# 1514
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
({void*_tmp2BF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C0="missing file\n";_tag_dyneither(_tmp2C0,sizeof(char),14);}),_tag_dyneither(_tmp2BF,sizeof(void*),0));});
 exit(1);}
# 1523
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,({const char*_tmp2C1="cyc_setjmp.h";_tag_dyneither(_tmp2C1,sizeof(char),13);}));
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0)
Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,({const char*_tmp2C2="cyc_include.h";_tag_dyneither(_tmp2C2,sizeof(char),14);}));
# 1529
{struct Cyc_List_List*_tmp2C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp2C3 != 0;_tmp2C3=_tmp2C3->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp2C3->hd));
if(Cyc_compile_failure){int _tmp2C4=1;_npop_handler(0);return _tmp2C4;}}}
# 1534
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp2C5=0;_npop_handler(0);return _tmp2C5;}
# 1540
if(Cyc_ccargs == 0){int _tmp2C6=0;_npop_handler(0);return _tmp2C6;}
# 1543
Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp2C7="-L";_tag_dyneither(_tmp2C7,sizeof(char),3);}),(struct _dyneither_ptr)def_lib_path));
# 1545
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _dyneither_ptr _tmp2C8=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp31C=" ";_tag_dyneither(_tmp31C,sizeof(char),2);}));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
struct _dyneither_ptr _tmp2C9=Cyc_str_sepstr(({struct Cyc_List_List*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);_tmp319->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp319;}),({const char*_tmp31B=" ";_tag_dyneither(_tmp31B,sizeof(char),2);}));
# 1551
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
int _tmp2CA=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1556
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp317=".a";_tag_dyneither(_tmp317,sizeof(char),3);}))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp318=".lib";_tag_dyneither(_tmp318,sizeof(char),5);}));
if(_tmp2CA){
stdlib=0;
stdlib_string=(struct _dyneither_ptr)({const char*_tmp2CB="";_tag_dyneither(_tmp2CB,sizeof(char),1);});}else{
# 1563
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_tmp2CC="-lcyc_a";_tag_dyneither(_tmp2CC,sizeof(char),8);});
nogc_filename=({const char*_tmp2CD="nogc_a.a";_tag_dyneither(_tmp2CD,sizeof(char),9);});
runtime_filename=({const char*_tmp2CE="runtime_cyc_a.a";_tag_dyneither(_tmp2CE,sizeof(char),16);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp2CF="-lcyc_nocheck";_tag_dyneither(_tmp2CF,sizeof(char),14);});
nogc_filename=({const char*_tmp2D0="nogc.a";_tag_dyneither(_tmp2D0,sizeof(char),7);});
runtime_filename=({const char*_tmp2D1="runtime_cyc.a";_tag_dyneither(_tmp2D1,sizeof(char),14);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_tmp2D2="-lcyc_pg";_tag_dyneither(_tmp2D2,sizeof(char),9);});
runtime_filename=({const char*_tmp2D3="runtime_cyc_pg.a";_tag_dyneither(_tmp2D3,sizeof(char),17);});
nogc_filename=({const char*_tmp2D4="nogc_pg.a";_tag_dyneither(_tmp2D4,sizeof(char),10);});}else{
if(Cyc_Lex_compile_for_boot_r){
# 1581
libcyc_flag=({const char*_tmp2D5="-lcycboot";_tag_dyneither(_tmp2D5,sizeof(char),10);});
nogc_filename=({const char*_tmp2D6="nogc.a";_tag_dyneither(_tmp2D6,sizeof(char),7);});
runtime_filename=({const char*_tmp2D7="runtime_cyc.a";_tag_dyneither(_tmp2D7,sizeof(char),14);});}else{
# 1585
libcyc_flag=({const char*_tmp2D8="-lcyc";_tag_dyneither(_tmp2D8,sizeof(char),6);});
nogc_filename=({const char*_tmp2D9="nogc.a";_tag_dyneither(_tmp2D9,sizeof(char),7);});
runtime_filename=({const char*_tmp2DA="runtime_cyc.a";_tag_dyneither(_tmp2DA,sizeof(char),14);});}}}}
# 1589
gc_filename=({const char*_tmp2DB="gc.a";_tag_dyneither(_tmp2DB,sizeof(char),5);});{
# 1591
struct _dyneither_ptr _tmp2DC=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp2DD=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1598
stdlib=0;
stdlib_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DC);({struct Cyc_String_pa_PrintArg_struct _tmp2E1;_tmp2E1.tag=0;_tmp2E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag);({struct Cyc_String_pa_PrintArg_struct _tmp2E0;_tmp2E0.tag=0;_tmp2E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD);({void*_tmp2DE[3]={& _tmp2E0,& _tmp2E1,& _tmp2E2};Cyc_aprintf(({const char*_tmp2DF=" %s %s %s";_tag_dyneither(_tmp2DF,sizeof(char),10);}),_tag_dyneither(_tmp2DE,sizeof(void*),3));});});});});};}
# 1602
if(Cyc_ic_r){struct _handler_cons _tmp2E3;_push_handler(& _tmp2E3);{int _tmp2E5=0;if(setjmp(_tmp2E3.handler))_tmp2E5=1;if(!_tmp2E5){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp2E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp2CA)
_tmp2E6=({struct Cyc_List_List*_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7->hd=Cyc_final_strptr;_tmp2E7->tl=_tmp2E6;_tmp2E7;});{
# 1610
struct Cyc_Interface_I*_tmp2E8=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp2E6,_tmp2E6);
if(_tmp2E8 == 0){
({void*_tmp2E9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EA="Error: interfaces incompatible\n";_tag_dyneither(_tmp2EA,sizeof(char),32);}),_tag_dyneither(_tmp2E9,sizeof(void*),0));});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp2EB=1;_npop_handler(1);return _tmp2EB;};}
# 1617
if(_tmp2CA){
if(Cyc_output_file != 0){
struct _dyneither_ptr _tmp2EC=({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0;_tmp2F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));({void*_tmp2F1[1]={& _tmp2F3};Cyc_aprintf(({const char*_tmp2F2="%s.cycio";_tag_dyneither(_tmp2F2,sizeof(char),9);}),_tag_dyneither(_tmp2F1,sizeof(void*),1));});});
struct Cyc___cycFILE*_tmp2ED=Cyc_try_file_open((struct _dyneither_ptr)_tmp2EC,({const char*_tmp2EF="wb";_tag_dyneither(_tmp2EF,sizeof(char),3);}),({const char*_tmp2F0="interface object file";_tag_dyneither(_tmp2F0,sizeof(char),22);}));
if(_tmp2ED == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp2EE=1;_npop_handler(1);return _tmp2EE;};}
# 1626
Cyc_Interface_save(_tmp2E8,_tmp2ED);
Cyc_file_close(_tmp2ED);}}else{
# 1630
if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp2E8,({struct _tuple20*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=({const char*_tmp2F6="empty interface";_tag_dyneither(_tmp2F6,sizeof(char),16);});_tmp2F4->f2=({const char*_tmp2F5="global interface";_tag_dyneither(_tmp2F5,sizeof(char),17);});_tmp2F4;}))){
# 1632
({void*_tmp2F7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F8="Error: some objects are still undefined\n";_tag_dyneither(_tmp2F8,sizeof(char),41);}),_tag_dyneither(_tmp2F7,sizeof(void*),0));});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp2F9=1;_npop_handler(1);return _tmp2F9;};}}};};
# 1603
;_pop_handler();}else{void*_tmp2E4=(void*)_exn_thrown;void*_tmp2FA=_tmp2E4;void*_tmp303;_LL4B: _tmp303=_tmp2FA;_LL4C:
# 1640
({struct Cyc_Int_pa_PrintArg_struct _tmp2FF;_tmp2FF.tag=1;_tmp2FF.f1=(unsigned long)Cyc_Core_get_exn_lineno();({struct Cyc_String_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=0;_tmp2FE.f1=(struct _dyneither_ptr)({const char*_tmp301=Cyc_Core_get_exn_filename();_tag_dyneither(_tmp301,sizeof(char),_get_zero_arr_size_char((void*)_tmp301,1));});({struct Cyc_String_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _dyneither_ptr)({const char*_tmp300=Cyc_Core_get_exn_name(_tmp303);_tag_dyneither(_tmp300,sizeof(char),_get_zero_arr_size_char((void*)_tmp300,1));});({void*_tmp2FB[3]={& _tmp2FD,& _tmp2FE,& _tmp2FF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2FC="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_dyneither(_tmp2FC,sizeof(char),131);}),_tag_dyneither(_tmp2FB,sizeof(void*),3));});});});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp302=1;_npop_handler(0);return _tmp302;};_LL4A:;}};}{
# 1646
struct _dyneither_ptr outfile_str=({const char*_tmp316="";_tag_dyneither(_tmp316,sizeof(char),1);});
if(Cyc_output_file != 0)
outfile_str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp306;_tmp306.tag=0;_tmp306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));({void*_tmp304[1]={& _tmp306};Cyc_aprintf(({const char*_tmp305=" -o %s";_tag_dyneither(_tmp305,sizeof(char),7);}),_tag_dyneither(_tmp304,sizeof(void*),1));});});{
# 1650
struct _dyneither_ptr _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp315;_tmp315.tag=0;_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2C9);({struct Cyc_String_pa_PrintArg_struct _tmp314;_tmp314.tag=0;_tmp314.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string);({struct Cyc_String_pa_PrintArg_struct _tmp313;_tmp313.tag=0;_tmp313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str);({struct Cyc_String_pa_PrintArg_struct _tmp312;_tmp312.tag=0;_tmp312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2C8);({struct Cyc_String_pa_PrintArg_struct _tmp311;_tmp311.tag=0;_tmp311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp);({void*_tmp30F[5]={& _tmp311,& _tmp312,& _tmp313,& _tmp314,& _tmp315};Cyc_aprintf(({const char*_tmp310="%s %s%s %s%s";_tag_dyneither(_tmp310,sizeof(char),13);}),_tag_dyneither(_tmp30F,sizeof(void*),5));});});});});});});
# 1653
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp30A;_tmp30A.tag=0;_tmp30A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp307);({void*_tmp308[1]={& _tmp30A};Cyc_fprintf(Cyc_stderr,({const char*_tmp309="%s\n";_tag_dyneither(_tmp309,sizeof(char),4);}),_tag_dyneither(_tmp308,sizeof(void*),1));});});Cyc_fflush(Cyc_stderr);}
if( system((const char*)_check_null(_untag_dyneither_ptr(_tmp307,sizeof(char),1)))!= 0){
({void*_tmp30B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp30C="Error: C compiler failed\n";_tag_dyneither(_tmp30C,sizeof(char),26);}),_tag_dyneither(_tmp30B,sizeof(void*),0));});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp30D=1;_npop_handler(0);return _tmp30D;};}
# 1660
Cyc_remove_cfiles();{
# 1662
int _tmp30E=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp30E;};};};};};
# 1412 "cyclone.cyc"
;_pop_region(r);};}

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
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
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
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1012 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
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
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 55
void*Cyc_Tcutil_compress(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
int Cyc_Tcutil_unify(void*,void*);
# 147
int Cyc_Tcutil_typecmp(void*,void*);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 34 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 36
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;enum Cyc_Absyn_Sign _tmp4;short _tmp3;enum Cyc_Absyn_Sign _tmp2;char _tmp1;switch((_tmp0.Short_c).tag){case 2: _LL1: _tmp2=((_tmp0.Char_c).val).f1;_tmp1=((_tmp0.Char_c).val).f2;_LL2:
 return Cyc_Absyn_Int_c(_tmp2,(int)_tmp1);case 4: _LL3: _tmp4=((_tmp0.Short_c).val).f1;_tmp3=((_tmp0.Short_c).val).f2;_LL4:
# 40
 return Cyc_Absyn_Int_c(_tmp4,(int)_tmp3);default: _LL5: _LL6:
 return cn;}_LL0:;}
# 45
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _tmp6=_tmp5;union Cyc_Absyn_Cnst _tmp14;int _tmp13;_LL8: _tmp14=_tmp6.f1;_tmp13=_tmp6.f2;_LL9:;
if(!_tmp13)
return({struct _tuple10 _tmp7;_tmp7.f1=0;_tmp7.f2=0;_tmp7;});{
union Cyc_Absyn_Cnst _tmp8=Cyc_Evexp_promote_const(_tmp14);union Cyc_Absyn_Cnst _tmp9=_tmp8;long long _tmp12;int _tmp11;switch((_tmp9.Float_c).tag){case 5: _LLB: _tmp11=((_tmp9.Int_c).val).f2;_LLC:
 return({struct _tuple10 _tmpA;_tmpA.f1=(unsigned int)_tmp11;_tmpA.f2=1;_tmpA;});case 6: _LLD: _tmp12=((_tmp9.LongLong_c).val).f2;_LLE: {
# 53
unsigned long long y=(unsigned long long)_tmp12 >> 32;
if(y != -1  && y != 0)
return({struct _tuple10 _tmpB;_tmpB.f1=0;_tmpB.f2=0;_tmpB;});else{
return({struct _tuple10 _tmpC;_tmpC.f1=(unsigned int)_tmp12;_tmpC.f2=1;_tmpC;});}}case 7: _LLF: _LL10:
 return({struct _tuple10 _tmpD;_tmpD.f1=0;_tmpD.f2=1;_tmpD;});default: _LL11: _LL12:
({void*_tmpE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpF="expecting unsigned int";_tag_dyneither(_tmpF,sizeof(char),23);}),_tag_dyneither(_tmpE,sizeof(void*),0));});return({struct _tuple10 _tmp10;_tmp10.f1=0;_tmp10.f2=1;_tmp10;});}_LLA:;};}struct _tuple12{int f1;int f2;};
# 62
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp15=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _tmp16=_tmp15;union Cyc_Absyn_Cnst _tmp24;int _tmp23;_LL14: _tmp24=_tmp16.f1;_tmp23=_tmp16.f2;_LL15:;
if(!_tmp23)
return({struct _tuple12 _tmp17;_tmp17.f1=0;_tmp17.f2=0;_tmp17;});{
union Cyc_Absyn_Cnst _tmp18=Cyc_Evexp_promote_const(_tmp24);union Cyc_Absyn_Cnst _tmp19=_tmp18;long long _tmp22;int _tmp21;switch((_tmp19.Float_c).tag){case 5: _LL17: _tmp21=((_tmp19.Int_c).val).f2;_LL18:
 return({struct _tuple12 _tmp1A;_tmp1A.f1=_tmp21 != 0;_tmp1A.f2=1;_tmp1A;});case 6: _LL19: _tmp22=((_tmp19.LongLong_c).val).f2;_LL1A:
 return({struct _tuple12 _tmp1B;_tmp1B.f1=_tmp22 != 0;_tmp1B.f2=1;_tmp1B;});case 1: _LL1B: _LL1C:
 return({struct _tuple12 _tmp1C;_tmp1C.f1=0;_tmp1C.f2=0;_tmp1C;});case 7: _LL1D: _LL1E:
 return({struct _tuple12 _tmp1D;_tmp1D.f1=0;_tmp1D.f2=1;_tmp1D;});default: _LL1F: _LL20:
({void*_tmp1E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp1F="expecting bool";_tag_dyneither(_tmp1F,sizeof(char),15);}),_tag_dyneither(_tmp1E,sizeof(void*),0));});return({struct _tuple12 _tmp20;_tmp20.f1=0;_tmp20.f2=0;_tmp20;});}_LL16:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 75
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp25=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _tmp26=_tmp25;union Cyc_Absyn_Cnst _tmp42;int _tmp41;_LL22: _tmp42=_tmp26.f1;_tmp41=_tmp26.f2;_LL23:;
if(!_tmp41)
return({struct _tuple11 _tmp27;_tmp27.f1=_tmp42;_tmp27.f2=0;_tmp27;});
{struct _tuple13 _tmp28=({struct _tuple13 _tmp3F;_tmp3F.f1=p;_tmp3F.f2=_tmp42;_tmp3F;});struct _tuple13 _tmp29=_tmp28;long long _tmp3E;char _tmp3D;short _tmp3C;int _tmp3B;enum Cyc_Absyn_Sign _tmp3A;long long _tmp39;enum Cyc_Absyn_Sign _tmp38;char _tmp37;enum Cyc_Absyn_Sign _tmp36;short _tmp35;enum Cyc_Absyn_Sign _tmp34;int _tmp33;enum Cyc_Absyn_Sign _tmp32;long long _tmp31;enum Cyc_Absyn_Sign _tmp30;char _tmp2F;enum Cyc_Absyn_Sign _tmp2E;short _tmp2D;enum Cyc_Absyn_Sign _tmp2C;int _tmp2B;switch(_tmp29.f1){case Cyc_Absyn_Plus: _LL25: _LL26:
 goto _LL24;case Cyc_Absyn_Minus: switch(((_tmp29.f2).LongLong_c).tag){case 5: _LL27: _tmp2C=(((_tmp29.f2).Int_c).val).f1;_tmp2B=(((_tmp29.f2).Int_c).val).f2;_LL28:
 _tmp42=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp2B);goto _LL24;case 4: _LL29: _tmp2E=(((_tmp29.f2).Short_c).val).f1;_tmp2D=(((_tmp29.f2).Short_c).val).f2;_LL2A:
 _tmp42=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,- _tmp2D);goto _LL24;case 2: _LL2B: _tmp30=(((_tmp29.f2).Char_c).val).f1;_tmp2F=(((_tmp29.f2).Char_c).val).f2;_LL2C:
 _tmp42=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,- _tmp2F);goto _LL24;case 6: _LL2D: _tmp32=(((_tmp29.f2).LongLong_c).val).f1;_tmp31=(((_tmp29.f2).LongLong_c).val).f2;_LL2E:
 _tmp42=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,- _tmp31);goto _LL24;default: goto _LL41;}case Cyc_Absyn_Bitnot: switch(((_tmp29.f2).LongLong_c).tag){case 5: _LL2F: _tmp34=(((_tmp29.f2).Int_c).val).f1;_tmp33=(((_tmp29.f2).Int_c).val).f2;_LL30:
 _tmp42=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp33);goto _LL24;case 4: _LL31: _tmp36=(((_tmp29.f2).Short_c).val).f1;_tmp35=(((_tmp29.f2).Short_c).val).f2;_LL32:
 _tmp42=Cyc_Absyn_Short_c(Cyc_Absyn_Unsigned,~ _tmp35);goto _LL24;case 2: _LL33: _tmp38=(((_tmp29.f2).Char_c).val).f1;_tmp37=(((_tmp29.f2).Char_c).val).f2;_LL34:
 _tmp42=Cyc_Absyn_Char_c(Cyc_Absyn_Unsigned,~ _tmp37);goto _LL24;case 6: _LL35: _tmp3A=(((_tmp29.f2).LongLong_c).val).f1;_tmp39=(((_tmp29.f2).LongLong_c).val).f2;_LL36:
 _tmp42=Cyc_Absyn_LongLong_c(Cyc_Absyn_Unsigned,~ _tmp39);goto _LL24;default: goto _LL41;}case Cyc_Absyn_Not: switch(((_tmp29.f2).Null_c).tag){case 5: _LL37: _tmp3B=(((_tmp29.f2).Int_c).val).f2;_LL38:
 _tmp42=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp3B == 0?1: 0);goto _LL24;case 4: _LL39: _tmp3C=(((_tmp29.f2).Short_c).val).f2;_LL3A:
 _tmp42=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,_tmp3C == 0?1: 0);goto _LL24;case 2: _LL3B: _tmp3D=(((_tmp29.f2).Char_c).val).f2;_LL3C:
 _tmp42=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,_tmp3D == 0?'\001':'\000');goto _LL24;case 6: _LL3D: _tmp3E=(((_tmp29.f2).LongLong_c).val).f2;_LL3E:
 _tmp42=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,(long long)(_tmp3E == 0?1: 0));goto _LL24;case 1: _LL3F: _LL40:
 _tmp42=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL24;default: goto _LL41;}default: _LL41: _LL42:
 return({struct _tuple11 _tmp2A;_tmp2A.f1=_tmp42;_tmp2A.f2=0;_tmp2A;});}_LL24:;}
# 96
return({struct _tuple11 _tmp40;_tmp40.f1=_tmp42;_tmp40.f2=1;_tmp40;});}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 100
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp43=Cyc_Evexp_eval_const_exp(e1);struct _tuple11 _tmp44=_tmp43;union Cyc_Absyn_Cnst _tmp64;int _tmp63;_LL44: _tmp64=_tmp44.f1;_tmp63=_tmp44.f2;_LL45:;{
struct _tuple11 _tmp45=Cyc_Evexp_eval_const_exp(e2);struct _tuple11 _tmp46=_tmp45;union Cyc_Absyn_Cnst _tmp62;int _tmp61;_LL47: _tmp62=_tmp46.f1;_tmp61=_tmp46.f2;_LL48:;
if(!_tmp63  || !_tmp61)
return({struct _tuple11 _tmp47;_tmp47.f1=_tmp64;_tmp47.f2=0;_tmp47;});
_tmp64=Cyc_Evexp_promote_const(_tmp64);
_tmp62=Cyc_Evexp_promote_const(_tmp62);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp48=_tmp64;enum Cyc_Absyn_Sign _tmp4B;int _tmp4A;if((_tmp48.Int_c).tag == 5){_LL4A: _tmp4B=((_tmp48.Int_c).val).f1;_tmp4A=((_tmp48.Int_c).val).f2;_LL4B:
 s1=_tmp4B;i1=_tmp4A;goto _LL49;}else{_LL4C: _LL4D:
 return({struct _tuple11 _tmp49;_tmp49.f1=_tmp64;_tmp49.f2=0;_tmp49;});}_LL49:;}
# 113
{union Cyc_Absyn_Cnst _tmp4C=_tmp62;enum Cyc_Absyn_Sign _tmp4F;int _tmp4E;if((_tmp4C.Int_c).tag == 5){_LL4F: _tmp4F=((_tmp4C.Int_c).val).f1;_tmp4E=((_tmp4C.Int_c).val).f2;_LL50:
 s2=_tmp4F;i2=_tmp4E;goto _LL4E;}else{_LL51: _LL52:
 return({struct _tuple11 _tmp4D;_tmp4D.f1=_tmp64;_tmp4D.f2=0;_tmp4D;});}_LL4E:;}
# 117
{enum Cyc_Absyn_Primop _tmp50=p;switch(_tmp50){case Cyc_Absyn_Div: _LL54: _LL55:
 goto _LL57;case Cyc_Absyn_Mod: _LL56: _LL57:
# 120
 if(i2 == 0){
({void*_tmp51=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmp52="division by zero in constant expression";_tag_dyneither(_tmp52,sizeof(char),40);}),_tag_dyneither(_tmp51,sizeof(void*),0));});
return({struct _tuple11 _tmp53;_tmp53.f1=_tmp64;_tmp53.f2=1;_tmp53;});}
# 124
goto _LL53;default: _LL58: _LL59:
 goto _LL53;}_LL53:;}{
# 127
int has_u_arg=s1 == Cyc_Absyn_Unsigned  || s2 == Cyc_Absyn_Unsigned;
unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;
int i3=0;
unsigned int u3=0;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple14 _tmp54=({struct _tuple14 _tmp5B;_tmp5B.f1=p;_tmp5B.f2=has_u_arg;_tmp5B;});struct _tuple14 _tmp55=_tmp54;switch(_tmp55.f1){case Cyc_Absyn_Plus: switch(_tmp55.f2){case 0: _LL5B: _LL5C:
 i3=i1 + i2;use_i3=1;goto _LL5A;case 1: _LL65: _LL66:
# 142
 u3=u1 + u2;use_u3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Times: switch(_tmp55.f2){case 0: _LL5D: _LL5E:
# 138
 i3=i1 * i2;use_i3=1;goto _LL5A;case 1: _LL67: _LL68:
# 143
 u3=u1 * u2;use_u3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Minus: switch(_tmp55.f2){case 0: _LL5F: _LL60:
# 139
 i3=i1 - i2;use_i3=1;goto _LL5A;case 1: _LL69: _LL6A:
# 144
 u3=u1 - u2;use_u3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Div: switch(_tmp55.f2){case 0: _LL61: _LL62:
# 140
 i3=i1 / i2;use_i3=1;goto _LL5A;case 1: _LL6B: _LL6C:
# 145
 u3=u1 / u2;use_u3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Mod: switch(_tmp55.f2){case 0: _LL63: _LL64:
# 141
 i3=i1 % i2;use_i3=1;goto _LL5A;case 1: _LL6D: _LL6E:
# 146
 u3=u1 % u2;use_u3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Eq: _LL6F: _LL70:
 b3=i1 == i2;use_b3=1;goto _LL5A;case Cyc_Absyn_Neq: _LL71: _LL72:
 b3=i1 != i2;use_b3=1;goto _LL5A;case Cyc_Absyn_Gt: switch(_tmp55.f2){case 0: _LL73: _LL74:
 b3=i1 > i2;use_b3=1;goto _LL5A;case 1: _LL7B: _LL7C:
# 153
 b3=u1 > u2;use_b3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Lt: switch(_tmp55.f2){case 0: _LL75: _LL76:
# 150
 b3=i1 < i2;use_b3=1;goto _LL5A;case 1: _LL7D: _LL7E:
# 154
 b3=u1 < u2;use_b3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Gte: switch(_tmp55.f2){case 0: _LL77: _LL78:
# 151
 b3=i1 >= i2;use_b3=1;goto _LL5A;case 1: _LL7F: _LL80:
# 155
 b3=u1 >= u2;use_b3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Lte: switch(_tmp55.f2){case 0: _LL79: _LL7A:
# 152
 b3=i1 <= i2;use_b3=1;goto _LL5A;case 1: _LL81: _LL82:
# 156
 b3=u1 <= u2;use_b3=1;goto _LL5A;default: goto _LL8F;}case Cyc_Absyn_Bitand: _LL83: _LL84:
 u3=u1 & u2;use_u3=1;goto _LL5A;case Cyc_Absyn_Bitor: _LL85: _LL86:
 u3=u1 | u2;use_u3=1;goto _LL5A;case Cyc_Absyn_Bitxor: _LL87: _LL88:
 u3=u1 ^ u2;use_u3=1;goto _LL5A;case Cyc_Absyn_Bitlshift: _LL89: _LL8A:
 u3=u1 << u2;use_u3=1;goto _LL5A;case Cyc_Absyn_Bitlrshift: _LL8B: _LL8C:
 u3=u1 >> u2;use_u3=1;goto _LL5A;case Cyc_Absyn_Bitarshift: _LL8D: _LL8E:
({void*_tmp56=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp57=">>> NOT IMPLEMENTED";_tag_dyneither(_tmp57,sizeof(char),20);}),_tag_dyneither(_tmp56,sizeof(void*),0));});goto _LL90;default: _LL8F: _LL90:
({void*_tmp58=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp59="bad constant expression";_tag_dyneither(_tmp59,sizeof(char),24);}),_tag_dyneither(_tmp58,sizeof(void*),0));});return({struct _tuple11 _tmp5A;_tmp5A.f1=_tmp64;_tmp5A.f2=1;_tmp5A;});}_LL5A:;}
# 165
if(use_i3)return({struct _tuple11 _tmp5C;_tmp5C.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp5C.f2=1;_tmp5C;});
if(use_u3)return({struct _tuple11 _tmp5D;_tmp5D.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3);_tmp5D.f2=1;_tmp5D;});
if(use_b3)return({struct _tuple11 _tmp5E;_tmp5E.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0);_tmp5E.f2=1;_tmp5E;});
({void*_tmp5F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp60="Evexp::eval_const_binop";_tag_dyneither(_tmp60,sizeof(char),24);}),_tag_dyneither(_tmp5F,sizeof(void*),0));});};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 174
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp65=e->r;void*_tmp66=_tmp65;struct Cyc_Absyn_Enumfield*_tmpB5;struct Cyc_Absyn_Enumfield*_tmpB4;void*_tmpB3;struct Cyc_Absyn_Exp*_tmpB2;void*_tmpB1;enum Cyc_Absyn_Primop _tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;union Cyc_Absyn_Cnst _tmpA7;switch(*((int*)_tmp66)){case 1: _LL92: _LL93:
# 178
 return({struct _tuple11 _tmp67;_tmp67.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp67.f2=0;_tmp67;});case 0: _LL94: _tmpA7=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL95:
 return({struct _tuple11 _tmp68;_tmp68.f1=_tmpA7;_tmp68.f2=1;_tmp68;});case 5: _LL96: _tmpAA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmpA9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_tmpA8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f3;_LL97: {
# 181
struct _tuple12 _tmp69=Cyc_Evexp_eval_const_bool_exp(_tmpAA);struct _tuple12 _tmp6A=_tmp69;int _tmp6D;int _tmp6C;_LLAF: _tmp6D=_tmp6A.f1;_tmp6C=_tmp6A.f2;_LLB0:;
if(!_tmp6C){
Cyc_Evexp_eval_const_exp(_tmpA9);
Cyc_Evexp_eval_const_exp(_tmpA8);
return({struct _tuple11 _tmp6B;_tmp6B.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp6B.f2=0;_tmp6B;});}
# 187
ans=_tmp6D?Cyc_Evexp_eval_const_exp(_tmpA9): Cyc_Evexp_eval_const_exp(_tmpA8);
goto _LL91;}case 6: _LL98: _tmpAC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmpAB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL99: {
# 190
struct _tuple12 _tmp6E=Cyc_Evexp_eval_const_bool_exp(_tmpAC);struct _tuple12 _tmp6F=_tmp6E;int _tmp73;int _tmp72;_LLB2: _tmp73=_tmp6F.f1;_tmp72=_tmp6F.f2;_LLB3:;
if(!_tmp72){
Cyc_Evexp_eval_const_exp(_tmpAB);
return({struct _tuple11 _tmp70;_tmp70.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp70.f2=0;_tmp70;});}
# 195
if(_tmp73)ans=Cyc_Evexp_eval_const_exp(_tmpAB);else{
ans=({struct _tuple11 _tmp71;_tmp71.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp71.f2=1;_tmp71;});}
goto _LL91;}case 7: _LL9A: _tmpAE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmpAD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL9B: {
# 199
struct _tuple12 _tmp74=Cyc_Evexp_eval_const_bool_exp(_tmpAE);struct _tuple12 _tmp75=_tmp74;int _tmp79;int _tmp78;_LLB5: _tmp79=_tmp75.f1;_tmp78=_tmp75.f2;_LLB6:;
if(!_tmp78){
Cyc_Evexp_eval_const_exp(_tmpAD);
return({struct _tuple11 _tmp76;_tmp76.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp76.f2=0;_tmp76;});}
# 204
if(!_tmp79)ans=Cyc_Evexp_eval_const_exp(_tmpAD);else{
ans=({struct _tuple11 _tmp77;_tmp77.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmp77.f2=1;_tmp77;});}
goto _LL91;}case 2: _LL9C: _tmpB0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmpAF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL9D:
# 208
 if(_tmpAF == 0){
({void*_tmp7A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp7B="bad static expression (no args to primop)";_tag_dyneither(_tmp7B,sizeof(char),42);}),_tag_dyneither(_tmp7A,sizeof(void*),0));});
return({struct _tuple11 _tmp7C;_tmp7C.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp7C.f2=1;_tmp7C;});}
# 212
if(_tmpAF->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(_tmpB0,(struct Cyc_Absyn_Exp*)_tmpAF->hd);
goto _LL91;}
# 216
if(((struct Cyc_List_List*)_check_null(_tmpAF->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(_tmpB0,(struct Cyc_Absyn_Exp*)_tmpAF->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpAF->tl))->hd);
goto _LL91;}
# 220
({void*_tmp7D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp7E="bad static expression (too many args to primop)";_tag_dyneither(_tmp7E,sizeof(char),48);}),_tag_dyneither(_tmp7D,sizeof(void*),0));});
return({struct _tuple11 _tmp7F;_tmp7F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp7F.f2=1;_tmp7F;});case 38: _LL9E: _tmpB1=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL9F:
# 224
{void*_tmp80=Cyc_Tcutil_compress(_tmpB1);void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp82;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp81)->tag == 18){_LLB8: _tmp82=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp81)->f1;_LLB9:
# 226
 e->r=_tmp82->r;
return Cyc_Evexp_eval_const_exp(_tmp82);}else{_LLBA: _LLBB:
 goto _LLB7;}_LLB7:;}
# 230
goto _LLA1;case 16: _LLA0: _LLA1:
 goto _LLA3;case 17: _LLA2: _LLA3:
 goto _LLA5;case 18: _LLA4: _LLA5:
 ans=({struct _tuple11 _tmp83;_tmp83.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp83.f2=0;_tmp83;});goto _LL91;case 13: _LLA6: _tmpB3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmpB2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LLA7:
# 236
 ans=Cyc_Evexp_eval_const_exp(_tmpB2);
if(ans.f2){
struct _tuple15 _tmp84=({struct _tuple15 _tmpA2;_tmpA2.f1=Cyc_Tcutil_compress(_tmpB3);_tmpA2.f2=ans.f1;_tmpA2;});struct _tuple15 _tmp85=_tmp84;void*_tmpA1;enum Cyc_Absyn_Sign _tmpA0;int _tmp9F;void*_tmp9E;enum Cyc_Absyn_Sign _tmp9D;short _tmp9C;void*_tmp9B;enum Cyc_Absyn_Sign _tmp9A;char _tmp99;enum Cyc_Absyn_Sign _tmp98;enum Cyc_Absyn_Size_of _tmp97;enum Cyc_Absyn_Sign _tmp96;int _tmp95;enum Cyc_Absyn_Sign _tmp94;enum Cyc_Absyn_Size_of _tmp93;enum Cyc_Absyn_Sign _tmp92;short _tmp91;enum Cyc_Absyn_Sign _tmp90;enum Cyc_Absyn_Size_of _tmp8F;enum Cyc_Absyn_Sign _tmp8E;char _tmp8D;switch(*((int*)_tmp85.f1)){case 6: switch(((_tmp85.f2).Int_c).tag){case 2: _LLBD: _tmp90=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85.f1)->f1;_tmp8F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85.f1)->f2;_tmp8E=(((_tmp85.f2).Char_c).val).f1;_tmp8D=(((_tmp85.f2).Char_c).val).f2;_LLBE:
# 240
 _tmp94=_tmp90;_tmp93=_tmp8F;_tmp92=_tmp8E;_tmp91=(short)_tmp8D;goto _LLC0;case 4: _LLBF: _tmp94=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85.f1)->f1;_tmp93=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85.f1)->f2;_tmp92=(((_tmp85.f2).Short_c).val).f1;_tmp91=(((_tmp85.f2).Short_c).val).f2;_LLC0:
 _tmp98=_tmp94;_tmp97=_tmp93;_tmp96=_tmp92;_tmp95=(int)_tmp91;goto _LLC2;case 5: _LLC1: _tmp98=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85.f1)->f1;_tmp97=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85.f1)->f2;_tmp96=(((_tmp85.f2).Int_c).val).f1;_tmp95=(((_tmp85.f2).Int_c).val).f2;_LLC2:
# 243
 if(_tmp98 != _tmp96)
ans.f1=Cyc_Absyn_Int_c(_tmp98,_tmp95);
goto _LLBC;default: goto _LLC9;}case 19: switch(((_tmp85.f2).Int_c).tag){case 2: _LLC3: _tmp9B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp85.f1)->f1;_tmp9A=(((_tmp85.f2).Char_c).val).f1;_tmp99=(((_tmp85.f2).Char_c).val).f2;_LLC4:
# 247
 _tmp9E=_tmp9B;_tmp9D=_tmp9A;_tmp9C=(short)_tmp99;goto _LLC6;case 4: _LLC5: _tmp9E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp85.f1)->f1;_tmp9D=(((_tmp85.f2).Short_c).val).f1;_tmp9C=(((_tmp85.f2).Short_c).val).f2;_LLC6:
 _tmpA1=_tmp9E;_tmpA0=_tmp9D;_tmp9F=(int)_tmp9C;goto _LLC8;case 5: _LLC7: _tmpA1=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp85.f1)->f1;_tmpA0=(((_tmp85.f2).Int_c).val).f1;_tmp9F=(((_tmp85.f2).Int_c).val).f2;_LLC8:
# 250
 if(_tmp9F < 0)
({void*_tmp86=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp87="cannot cast negative number to a tag type";_tag_dyneither(_tmp87,sizeof(char),42);}),_tag_dyneither(_tmp86,sizeof(void*),0));});
Cyc_Tcutil_unify(_tmpA1,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp89;_tmp89.tag=18;_tmp89.f1=Cyc_Absyn_const_exp(ans.f1,0);_tmp89;});_tmp88;}));
ans.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp9F);
goto _LLBC;default: goto _LLC9;}default: _LLC9: _LLCA:
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB3));({void*_tmp8A[1]={& _tmp8C};Cyc_Tcutil_terr(e->loc,({const char*_tmp8B="cannot cast to %s";_tag_dyneither(_tmp8B,sizeof(char),18);}),_tag_dyneither(_tmp8A,sizeof(void*),1));});});goto _LLBC;}_LLBC:;}
# 257
goto _LL91;case 32: _LLA8: _tmpB4=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LLA9:
 _tmpB5=_tmpB4;goto _LLAB;case 31: _LLAA: _tmpB5=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LLAB:
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->tag));goto _LL91;default: _LLAC: _LLAD:
# 261
({struct Cyc_String_pa_PrintArg_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmpA3[1]={& _tmpA5};Cyc_Tcutil_terr(e->loc,({const char*_tmpA4="bad static expression %s";_tag_dyneither(_tmpA4,sizeof(char),25);}),_tag_dyneither(_tmpA3,sizeof(void*),1));});});
return({struct _tuple11 _tmpA6;_tmpA6.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmpA6.f2=1;_tmpA6;});}_LL91:;}
# 264
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmpB6=ans.f1;
{union Cyc_Absyn_Cnst _tmpB7=_tmpB6;if((_tmpB7.Int_c).tag == 5)switch(((_tmpB7.Int_c).val).f1){case Cyc_Absyn_Unsigned: switch(((_tmpB7.Int_c).val).f2){case 0: _LLCC: _LLCD: {
# 269
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 271
c=(void*)& uzero;
goto _LLCB;}case 1: _LLCE: _LLCF: {
# 274
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 276
c=(void*)& uone;
goto _LLCB;}default: goto _LLD4;}case Cyc_Absyn_Signed: switch(((_tmpB7.Int_c).val).f2){case 0: _LLD0: _LLD1: {
# 279
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 281
c=(void*)& szero;
goto _LLCB;}case 1: _LLD2: _LLD3: {
# 284
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 286
c=(void*)& sone;
goto _LLCB;}default: goto _LLD4;}default: goto _LLD4;}else{_LLD4: _LLD5:
 c=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpBD;_tmpBD.tag=0;_tmpBD.f1=_tmpB6;_tmpBD;});_tmpBC;});goto _LLCB;}_LLCB:;}
# 290
e->r=c;}
# 292
return ans;}
# 295
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmpBE=e->r;void*_tmpBF=_tmpBE;struct Cyc_Absyn_Exp*_tmpC9;enum Cyc_Absyn_Primop _tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;switch(*((int*)_tmpBF)){case 32: _LLD7: _LLD8:
 goto _LLDA;case 31: _LLD9: _LLDA:
 goto _LLDC;case 16: _LLDB: _LLDC:
 goto _LLDE;case 17: _LLDD: _LLDE:
 goto _LLE0;case 18: _LLDF: _LLE0:
 goto _LLE2;case 0: _LLE1: _LLE2:
 return 1;case 5: _LLE3: _tmpC2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_tmpC1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_tmpC0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBF)->f3;_LLE4:
# 304
 return(Cyc_Evexp_c_can_eval(_tmpC2) && Cyc_Evexp_c_can_eval(_tmpC1)) && Cyc_Evexp_c_can_eval(_tmpC0);case 6: _LLE5: _tmpC4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_tmpC3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_LLE6:
 _tmpC6=_tmpC4;_tmpC5=_tmpC3;goto _LLE8;case 7: _LLE7: _tmpC6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_tmpC5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_LLE8:
# 307
 return Cyc_Evexp_c_can_eval(_tmpC6) && Cyc_Evexp_c_can_eval(_tmpC5);case 2: _LLE9: _tmpC8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_tmpC7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_LLEA:
# 309
 for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmpC7->hd))return 0;}
return 1;case 38: _LLEB: _LLEC:
 return 0;case 13: _LLED: _tmpC9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_LLEE:
 return Cyc_Evexp_c_can_eval(_tmpC9);default: _LLEF: _LLF0:
 return 0;}_LLD6:;}
# 318
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmpCA=e->r;void*_tmpCB=_tmpCA;switch(*((int*)_tmpCB)){case 0: _LLF2: _LLF3:
 return 1;case 5: _LLF4: _LLF5:
 return 2;case 2: _LLF6: _LLF7:
 return 3;case 16: _LLF8: _LLF9:
 goto _LLFB;case 17: _LLFA: _LLFB:
 return 4;case 18: _LLFC: _LLFD:
 return 5;case 13: _LLFE: _LLFF:
 return 6;case 6: _LL100: _LL101:
 return 7;case 7: _LL102: _LL103:
 return 8;case 38: _LL104: _LL105:
 return 9;default: _LL106: _LL107:
# 331
({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmpCC[1]={& _tmpCE};Cyc_Tcutil_terr(e->loc,({const char*_tmpCD="bad static expression %s";_tag_dyneither(_tmpCD,sizeof(char),25);}),_tag_dyneither(_tmpCC,sizeof(void*),1));});});return 0;}_LLF1:;}struct _tuple16{void*f1;void*f2;};
# 335
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmpCF=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple10 _tmpD0=_tmpCF;unsigned int _tmp11C;int _tmp11B;_LL109: _tmp11C=_tmpD0.f1;_tmp11B=_tmpD0.f2;_LL10A:;{
struct _tuple10 _tmpD1=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple10 _tmpD2=_tmpD1;unsigned int _tmp11A;int _tmp119;_LL10C: _tmp11A=_tmpD2.f1;_tmp119=_tmpD2.f2;_LL10D:;
if(_tmp11B  && _tmp119)
return(int)(_tmp11C - _tmp11A);{
int _tmpD3=Cyc_Evexp_const_exp_case_number(e1);
int _tmpD4=Cyc_Evexp_const_exp_case_number(e2);
if(_tmpD3 != _tmpD4)
return _tmpD3 - _tmpD4;{
struct _tuple16 _tmpD5=({struct _tuple16 _tmp118;_tmp118.f1=e1->r;_tmp118.f2=e2->r;_tmp118;});struct _tuple16 _tmpD6=_tmpD5;void*_tmp117;void*_tmp116;void*_tmp115;struct Cyc_Absyn_Exp*_tmp114;void*_tmp113;struct Cyc_Absyn_Exp*_tmp112;void*_tmp111;struct Cyc_List_List*_tmp110;void*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;void*_tmp10A;void*_tmp109;struct Cyc_Absyn_Exp*_tmp108;void*_tmp107;void*_tmp106;enum Cyc_Absyn_Primop _tmp105;struct Cyc_List_List*_tmp104;enum Cyc_Absyn_Primop _tmp103;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF4;switch(*((int*)_tmpD6.f1)){case 5: if(((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 5){_LL10F: _tmpF9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmpF8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f2;_tmpF7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f3;_tmpF6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_tmpF5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f2;_tmpF4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f3;_LL110: {
# 349
int _tmpD7=Cyc_Evexp_const_exp_cmp(_tmpF7,_tmpF4);
if(_tmpD7 != 0)return _tmpD7;
_tmpFD=_tmpF9;_tmpFC=_tmpF8;_tmpFB=_tmpF6;_tmpFA=_tmpF5;goto _LL112;}}else{goto _LL125;}case 6: if(((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 6){_LL111: _tmpFD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmpFC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f2;_tmpFB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_tmpFA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f2;_LL112:
 _tmp101=_tmpFD;_tmp100=_tmpFC;_tmpFF=_tmpFB;_tmpFE=_tmpFA;goto _LL114;}else{goto _LL125;}case 7: if(((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 7){_LL113: _tmp101=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp100=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f2;_tmpFF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_tmpFE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f2;_LL114: {
# 354
int _tmpD8=Cyc_Evexp_const_exp_cmp(_tmp101,_tmpFF);
if(_tmpD8 != 0)return _tmpD8;
return Cyc_Evexp_const_exp_cmp(_tmp100,_tmpFE);}}else{goto _LL125;}case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 2){_LL115: _tmp105=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp104=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f2;_tmp103=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_tmp102=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f2;_LL116: {
# 358
int _tmpD9=(int)_tmp105 - (int)_tmp103;
if(_tmpD9 != 0)return _tmpD9;
for(0;_tmp104 != 0  && _tmp102 != 0;(_tmp104=_tmp104->tl,_tmp102=_tmp102->tl)){
int _tmpDA=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmp104->hd,(struct Cyc_Absyn_Exp*)_tmp102->hd);
if(_tmpDA != 0)
return _tmpDA;}
# 365
return 0;}}else{goto _LL125;}case 16: switch(*((int*)_tmpD6.f2)){case 16: _LL117: _tmp107=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp106=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_LL118:
 return Cyc_Tcutil_typecmp(_tmp107,_tmp106);case 17: _LL119: _tmp109=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp108=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_LL11A: {
# 368
void*_tmpDB=_tmp108->topt;
if(_tmpDB == 0)
({void*_tmpDC=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmpDD="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpDD,sizeof(char),51);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});
return Cyc_Tcutil_typecmp(_tmp109,(void*)_check_null(_tmpDB));}default: goto _LL125;}case 17: switch(*((int*)_tmpD6.f2)){case 16: _LL11B: _tmp10B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp10A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_LL11C: {
# 373
void*_tmpDE=_tmp10B->topt;
if(_tmpDE == 0)
({void*_tmpDF=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpE0="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpE0,sizeof(char),51);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});
return Cyc_Tcutil_typecmp((void*)_check_null(_tmpDE),_tmp10A);}case 17: _LL11D: _tmp10D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp10C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_LL11E: {
# 378
void*_tmpE1=_tmp10D->topt;
void*_tmpE2=_tmp10C->topt;
if(_tmpE1 == 0)
({void*_tmpE3=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpE4="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpE4,sizeof(char),51);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});
if(_tmpE2 == 0)
({void*_tmpE5=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmpE6="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpE6,sizeof(char),51);}),_tag_dyneither(_tmpE5,sizeof(void*),0));});
return Cyc_Tcutil_typecmp((void*)_check_null(_tmpE1),(void*)_check_null(_tmpE2));}default: goto _LL125;}case 18: if(((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 18){_LL11F: _tmp111=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp110=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f2;_tmp10F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_tmp10E=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f2;_LL120: {
# 386
int _tmpE7=Cyc_Tcutil_typecmp(_tmp111,_tmp10F);
if(_tmpE7 != 0)return _tmpE7;{
int _tmpE8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp110);
int _tmpE9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10E);
if(_tmpE8 < _tmpE9)return - 1;
if(_tmpE9 < _tmpE8)return 1;
for(0;_tmp110 != 0  && _tmp10E != 0;(_tmp110=_tmp110->tl,_tmp10E=_tmp10E->tl)){
struct _tuple16 _tmpEA=({struct _tuple16 _tmpF0;_tmpF0.f1=(void*)_tmp110->hd;_tmpF0.f2=(void*)_tmp10E->hd;_tmpF0;});struct _tuple16 _tmpEB=_tmpEA;unsigned int _tmpEF;unsigned int _tmpEE;struct _dyneither_ptr*_tmpED;struct _dyneither_ptr*_tmpEC;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpEB.f1)->tag == 0){if(((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpEB.f2)->tag == 1){_LL128: _LL129:
 return - 1;}else{_LL12C: _tmpED=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpEB.f1)->f1;_tmpEC=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpEB.f2)->f1;_LL12D: {
# 397
int c=Cyc_strptrcmp(_tmpED,_tmpEC);
if(c != 0)return c;
goto _LL127;}}}else{if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpEB.f2)->tag == 0){_LL12A: _LL12B:
# 395
 return 1;}else{_LL12E: _tmpEF=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpEB.f1)->f1;_tmpEE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpEB.f2)->f1;_LL12F: {
# 401
int c=(int)(_tmpEF - _tmpEE);
if(c != 0)return c;
goto _LL127;}}}_LL127:;}
# 405
return 0;};}}else{goto _LL125;}case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 13){_LL121: _tmp115=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp114=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f2;_tmp113=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_tmp112=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f2;_LL122: {
# 407
int _tmpF1=Cyc_Tcutil_typecmp(_tmp115,_tmp113);
if(_tmpF1 != 0)return _tmpF1;
return Cyc_Evexp_const_exp_cmp(_tmp114,_tmp112);}}else{goto _LL125;}case 38: if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->tag == 38){_LL123: _tmp117=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD6.f1)->f1;_tmp116=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD6.f2)->f1;_LL124:
# 411
 if(Cyc_Tcutil_unify(_tmp117,_tmp116))return 0;
return Cyc_Tcutil_typecmp(_tmp117,_tmp116);}else{goto _LL125;}default: _LL125: _LL126:
({void*_tmpF2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF3="Evexp::const_exp_cmp, unexpected case";_tag_dyneither(_tmpF3,sizeof(char),38);}),_tag_dyneither(_tmpF2,sizeof(void*),0));});}_LL10E:;};};};}
# 416
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 419
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmp11D=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple10 _tmp11E=_tmp11D;unsigned int _tmp124;int _tmp123;_LL131: _tmp124=_tmp11E.f1;_tmp123=_tmp11E.f2;_LL132:;{
struct _tuple10 _tmp11F=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple10 _tmp120=_tmp11F;unsigned int _tmp122;int _tmp121;_LL134: _tmp122=_tmp120.f1;_tmp121=_tmp120.f2;_LL135:;
if(_tmp123  && _tmp121)
return _tmp124 <= _tmp122;
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 429
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp125=Cyc_Tcutil_compress(t);void*_tmp126=_tmp125;struct Cyc_Absyn_Typedefdecl*_tmp13E;struct Cyc_List_List*_tmp13D;union Cyc_Absyn_AggrInfoU _tmp13C;struct Cyc_List_List*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Datatypefield*_tmp137;struct Cyc_Absyn_Tvar*_tmp136;switch(*((int*)_tmp126)){case 0: _LL137: _LL138:
 return 0;case 2: _LL139: _tmp136=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp126)->f1;_LL13A: {
# 433
enum Cyc_Absyn_KindQual _tmp127=(Cyc_Tcutil_tvar_kind(_tmp136,& Cyc_Tcutil_bk))->kind;enum Cyc_Absyn_KindQual _tmp128=_tmp127;if(_tmp128 == Cyc_Absyn_BoxKind){_LL176: _LL177:
 return 1;}else{_LL178: _LL179:
 return 0;}_LL175:;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp126)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp126)->f1)->v)->kind == Cyc_Absyn_BoxKind){_LL13B: _LL13C:
# 437
 return 1;}else{goto _LL13D;}}else{_LL13D: _LL13E:
 return 0;}case 3: _LL13F: _LL140:
 return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp126)->f1).field_info).KnownDatatypefield).tag == 2){_LL141: _tmp137=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp126)->f1).field_info).KnownDatatypefield).val).f2;_LL142:
# 441
 _tmp138=_tmp137->typs;goto _LL144;}else{_LL145: _LL146:
# 448
 return 0;}case 10: _LL143: _tmp138=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp126)->f1;_LL144:
# 443
 for(0;_tmp138 != 0;_tmp138=_tmp138->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp138->hd)).f2))
return 0;}
return 1;case 5: _LL147: _LL148:
# 451
 return 1;case 19: _LL149: _LL14A:
# 453
 goto _LL14C;case 6: _LL14B: _LL14C:
 goto _LL14E;case 7: _LL14D: _LL14E:
 return 1;case 8: _LL14F: _tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp126)->f1).elt_type;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp126)->f1).num_elts;_LL150:
# 457
 return _tmp139 != 0;case 28: _LL151: _LL152:
 return 1;case 9: _LL153: _LL154:
 return 0;case 15: _LL155: _LL156:
 return 1;case 16: _LL157: _LL158:
 return 1;case 11: _LL159: _tmp13C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp126)->f1).aggr_info;_tmp13B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp126)->f1).targs;_LL15A: {
# 463
struct Cyc_Absyn_Aggrdecl*ad;
{union Cyc_Absyn_AggrInfoU _tmp129=_tmp13C;struct Cyc_Absyn_Aggrdecl*_tmp12C;if((_tmp129.UnknownAggr).tag == 1){_LL17B: _LL17C:
({void*_tmp12A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp12B="szof on unchecked StructType";_tag_dyneither(_tmp12B,sizeof(char),29);}),_tag_dyneither(_tmp12A,sizeof(void*),0));});}else{_LL17D: _tmp12C=*(_tmp129.KnownAggr).val;_LL17E:
 ad=_tmp12C;}_LL17A:;}
# 468
if(ad->impl == 0)
return 0;
{
struct _RegionHandle _tmp12D=_new_region("temp");struct _RegionHandle*temp=& _tmp12D;_push_region(temp);
{struct Cyc_List_List*_tmp12E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,_tmp13B);
{struct Cyc_List_List*_tmp12F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp12E,((struct Cyc_Absyn_Aggrfield*)_tmp12F->hd)->type))){int _tmp130=0;_npop_handler(0);return _tmp130;}}}{
int _tmp131=1;_npop_handler(0);return _tmp131;};}
# 472
;_pop_region(temp);}
# 477
_tmp13D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(ad))->impl))->fields;goto _LL15C;}case 12: _LL15B: _tmp13D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp126)->f2;_LL15C:
# 479
 for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp13D->hd)->type))
return 0;}
return 1;case 13: _LL15D: _LL15E:
 goto _LL160;case 14: _LL15F: _LL160:
 return 1;case 26: _LL161: _LL162:
 goto _LL164;case 20: _LL163: _LL164:
 goto _LL166;case 22: _LL165: _LL166:
 goto _LL168;case 21: _LL167: _LL168:
 goto _LL16A;case 24: _LL169: _LL16A:
 goto _LL16C;case 23: _LL16B: _LL16C:
 goto _LL16E;case 18: _LL16D: _LL16E:
 goto _LL170;case 27: _LL16F: _LL170:
 goto _LL172;case 25: _LL171: _LL172:
 return 0;default: _LL173: _tmp13E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp126)->f3;_LL174:
# 496
 if(_tmp13E == 0  || _tmp13E->kind == 0)
({void*_tmp132=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp133[1]={& _tmp135};Cyc_aprintf(({const char*_tmp134="szof typedeftype %s";_tag_dyneither(_tmp134,sizeof(char),20);}),_tag_dyneither(_tmp133,sizeof(void*),1));});}),_tag_dyneither(_tmp132,sizeof(void*),0));});
return((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp13E->kind))->v)->kind == Cyc_Absyn_BoxKind;}_LL136:;}

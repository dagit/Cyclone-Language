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

# 26 "cycboot.h"
 typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 119 "core.h"
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 131 "dict.h"
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
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
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 915 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 961
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1113
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1173
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};struct Cyc_Interface_I;
# 33 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 36
struct Cyc_Interface_I*Cyc_Interface_empty();
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 66 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 70
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo);
# 72
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 75
void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 87
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 33 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
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
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 32 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 85
int Cyc_Tcutil_is_function_type(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 129
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 34 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 38
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 59 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 62
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 66
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 68
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
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
# 45 "interface.cyc"
void Cyc_Lex_lex_init();
# 52
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Interface_xdatatypefielddecl_t;struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};
# 64
typedef struct Cyc_Interface_Ienv*Cyc_Interface_ienv_t;struct Cyc_Interface_I{struct Cyc_Interface_Ienv*imports;struct Cyc_Interface_Ienv*exports;struct Cyc_List_List*tds;};
# 73
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));({struct Cyc_Dict_Dict _tmp218=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->aggrdecls=_tmp218;});({struct Cyc_Dict_Dict _tmp219=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->datatypedecls=_tmp219;});({struct Cyc_Dict_Dict _tmp21A=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->enumdecls=_tmp21A;});({struct Cyc_Dict_Dict _tmp21B=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->typedefdecls=_tmp21B;});({struct Cyc_Dict_Dict _tmp21C=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->vardecls=_tmp21C;});({struct Cyc_Dict_Dict _tmp21D=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->xdatatypefielddecls=_tmp21D;});_tmp0;});}
# 83
struct Cyc_Interface_I*Cyc_Interface_empty(){
return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Interface_Ienv*_tmp21E=Cyc_Interface_new_ienv();_tmp1->imports=_tmp21E;});({struct Cyc_Interface_Ienv*_tmp21F=Cyc_Interface_new_ienv();_tmp1->exports=_tmp21F;});_tmp1->tds=0;_tmp1;});}
# 88
struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp2=Cyc_Interface_empty();
# 91
struct Cyc_Absyn_Datatypedecl*_tmp3=({struct Cyc_Absyn_Datatypedecl*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=*Cyc_Absyn_exn_tud;_tmp9;});
_tmp3->sc=Cyc_Absyn_Public;
({struct Cyc_Dict_Dict _tmp220=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)((_tmp2->exports)->datatypedecls,Cyc_Absyn_exn_name,_tmp3);(_tmp2->exports)->datatypedecls=_tmp220;});
# 95
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp4 != 0;_tmp4=_tmp4->tl){
struct Cyc_Absyn_Datatypefield*_tmp5=({struct Cyc_Absyn_Datatypefield*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=*((struct Cyc_Absyn_Datatypefield*)_tmp4->hd);_tmp8;});
_tmp5->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp6=({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->base=_tmp3;_tmp7->field=_tmp5;_tmp7;});
({struct Cyc_Dict_Dict _tmp221=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)((_tmp2->exports)->xdatatypefielddecls,((struct Cyc_Absyn_Datatypefield*)_tmp4->hd)->name,_tmp6);(_tmp2->exports)->xdatatypefielddecls=_tmp221;});};}}
# 120 "interface.cyc"
_tmp2->imports=_tmp2->exports;
return _tmp2;}
# 140 "interface.cyc"
static void Cyc_Interface_err(struct _dyneither_ptr msg){
({void*_tmpA=0;({struct _dyneither_ptr _tmp222=msg;Cyc_Tcutil_terr(0,_tmp222,_tag_dyneither(_tmpA,sizeof(void*),0));});});}
# 143
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpB=_cycalloc(sizeof(*_tmpB));({struct Cyc_Core_Invalid_argument_exn_struct _tmp223=({struct Cyc_Core_Invalid_argument_exn_struct _tmpC;_tmpC.tag=Cyc_Core_Invalid_argument;_tmpC.f1=s;_tmpC;});_tmpB[0]=_tmp223;});_tmpB;}));}
# 146
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11;_tmp11.tag=0;({struct _dyneither_ptr _tmp224=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n));_tmp11.f1=_tmp224;});({struct Cyc_String_pa_PrintArg_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);({void*_tmpD[3]={& _tmpF,& _tmp10,& _tmp11};({struct _dyneither_ptr _tmp225=({const char*_tmpE="fields of %s %s %s have never been defined";_tag_dyneither(_tmpE,sizeof(char),43);});Cyc_aprintf(_tmp225,_tag_dyneither(_tmpD,sizeof(void*),3));});});});});}));}
# 150
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;({struct _dyneither_ptr _tmp226=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n));_tmp15.f1=_tmp226;});({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);({void*_tmp12[2]={& _tmp14,& _tmp15};({struct _dyneither_ptr _tmp227=({const char*_tmp13="the body of %s function %s has never been defined";_tag_dyneither(_tmp13,sizeof(char),50);});Cyc_aprintf(_tmp227,_tag_dyneither(_tmp12,sizeof(void*),2));});});});}));}
# 156
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if(({char*_tmp228=(char*)obj1.curr;_tmp228 != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;({struct _dyneither_ptr _tmp229=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 160
Cyc_Absynpp_qvar2string(name2));_tmp1B.f1=_tmp229;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);({struct Cyc_String_pa_PrintArg_struct _tmp19;_tmp19.tag=0;({struct _dyneither_ptr _tmp22A=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 159
Cyc_Absynpp_qvar2string(name1));_tmp19.f1=_tmp22A;});({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);({void*_tmp16[4]={& _tmp18,& _tmp19,& _tmp1A,& _tmp1B};({struct _dyneither_ptr _tmp22B=({const char*_tmp17="declaration of %s %s relies on static %s %s";_tag_dyneither(_tmp17,sizeof(char),44);});Cyc_aprintf(_tmp22B,_tag_dyneither(_tmp16,sizeof(void*),4));});});});});});}));}
# 164
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if(({char*_tmp22C=(char*)obj1.curr;_tmp22C != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;({struct _dyneither_ptr _tmp22D=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 168
Cyc_Absynpp_qvar2string(name2));_tmp21.f1=_tmp22D;});({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);({struct Cyc_String_pa_PrintArg_struct _tmp1F;_tmp1F.tag=0;({struct _dyneither_ptr _tmp22E=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 167
Cyc_Absynpp_qvar2string(name1));_tmp1F.f1=_tmp22E;});({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);({void*_tmp1C[4]={& _tmp1E,& _tmp1F,& _tmp20,& _tmp21};({struct _dyneither_ptr _tmp22F=({const char*_tmp1D="declaration of %s %s relies on fields of abstract %s %s";_tag_dyneither(_tmp1D,sizeof(char),56);});Cyc_aprintf(_tmp22F,_tag_dyneither(_tmp1C,sizeof(void*),4));});});});});});}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 179
typedef struct Cyc_Interface_Seen*Cyc_Interface_seen_t;
# 181
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
return({struct Cyc_Interface_Seen*_tmp22=_cycalloc(sizeof(*_tmp22));({struct Cyc_Dict_Dict _tmp230=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp22->aggrs=_tmp230;});({struct Cyc_Dict_Dict _tmp231=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp22->datatypes=_tmp231;});_tmp22;});}
# 187
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);
# 190
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 192
int _tmp23=1;
for(0;l != 0;l=l->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp235=ae;struct Cyc_Interface_Seen*_tmp234=seen;struct _dyneither_ptr _tmp233=obj;struct _tuple0*_tmp232=name;Cyc_Interface_check_public_type(_tmp235,_tmp234,_tmp233,_tmp232,f(l->hd));}))
_tmp23=0;}
return _tmp23;}
# 199
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp24;_push_handler(& _tmp24);{int _tmp26=0;if(setjmp(_tmp24.handler))_tmp26=1;if(!_tmp26){{int _tmp27=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp27;};_pop_handler();}else{void*_tmp25=(void*)_exn_thrown;void*_tmp28=_tmp25;void*_tmp29;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp28)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp29=_tmp28;_LL4:(int)_rethrow(_tmp29);}_LL0:;}};}{
int _tmp2A=1;
({struct Cyc_Dict_Dict _tmp236=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp2A);seen->aggrs=_tmp236;});
if(d->impl != 0){
struct Cyc_List_List*_tmp2B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;for(0;_tmp2B != 0;_tmp2B=_tmp2B->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp23A=ae;struct Cyc_Interface_Seen*_tmp239=seen;struct _dyneither_ptr _tmp238=({const char*_tmp2C="type";_tag_dyneither(_tmp2C,sizeof(char),5);});struct _tuple0*_tmp237=d->name;Cyc_Interface_check_public_type(_tmp23A,_tmp239,_tmp238,_tmp237,((struct Cyc_Absyn_Aggrfield*)_tmp2B->hd)->type);}))
_tmp2A=0;}}
# 209
({struct Cyc_Dict_Dict _tmp23B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp2A);seen->aggrs=_tmp23B;});
return _tmp2A;};}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 213
static void*Cyc_Interface_get_type1(struct _tuple20*x){
return(*x).f2;}
# 216
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 220
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler))_tmp2F=1;if(!_tmp2F){{int _tmp30=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);return _tmp30;};_pop_handler();}else{void*_tmp2E=(void*)_exn_thrown;void*_tmp31=_tmp2E;void*_tmp32;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent){_LL6: _LL7:
 goto _LL5;}else{_LL8: _tmp32=_tmp31;_LL9:(int)_rethrow(_tmp32);}_LL5:;}};}{
int _tmp33=1;
({struct Cyc_Dict_Dict _tmp23C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp33);seen->datatypes=_tmp23C;});
if(d->fields != 0){
struct Cyc_List_List*_tmp34=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp240=ae;struct Cyc_Interface_Seen*_tmp23F=seen;struct _dyneither_ptr _tmp23E=({const char*_tmp35="datatype";_tag_dyneither(_tmp35,sizeof(char),9);});struct _tuple0*_tmp23D=d->name;((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(_tmp240,_tmp23F,_tmp23E,_tmp23D,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)_tmp34->hd)->typs);}))
# 229
_tmp33=0;}}
# 231
({struct Cyc_Dict_Dict _tmp241=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp33);seen->datatypes=_tmp241;});
return _tmp33;};}
# 235
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 239
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return({struct Cyc_Tcenv_Genv*_tmp245=ae;struct Cyc_Interface_Seen*_tmp244=seen;struct _dyneither_ptr _tmp243=_tag_dyneither(0,0,0);struct _tuple0*_tmp242=d->name;Cyc_Interface_check_public_type(_tmp245,_tmp244,_tmp243,_tmp242,(void*)_check_null(d->defn));});else{
return 1;}}
# 246
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
return({struct Cyc_Tcenv_Genv*_tmp249=ae;struct Cyc_Interface_Seen*_tmp248=seen;struct _dyneither_ptr _tmp247=({const char*_tmp36="variable";_tag_dyneither(_tmp36,sizeof(char),9);});struct _tuple0*_tmp246=d->name;Cyc_Interface_check_public_type(_tmp249,_tmp248,_tmp247,_tmp246,d->type);});}
# 250
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){
void*_tmp37=Cyc_Tcutil_compress(t);void*_tmp38=_tmp37;struct Cyc_Absyn_Datatypedecl*_tmp72;struct Cyc_Absyn_Datatypefield*_tmp71;struct Cyc_List_List*_tmp70;struct Cyc_Absyn_Datatypedecl*_tmp6F;struct Cyc_List_List*_tmp6E;struct _tuple0*_tmp6D;union Cyc_Absyn_AggrInfoU _tmp6C;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6A;void*_tmp69;struct Cyc_List_List*_tmp68;struct Cyc_Absyn_VarargInfo*_tmp67;void*_tmp66;void*_tmp65;void*_tmp64;switch(*((int*)_tmp38)){case 5: _LLB: _tmp64=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38)->f1).elt_typ;_LLC:
 _tmp65=_tmp64;goto _LLE;case 8: _LLD: _tmp65=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38)->f1).elt_type;_LLE:
 _tmp66=_tmp65;goto _LL10;case 17: _LLF: _tmp66=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp38)->f4;if(_tmp66 != 0){_LL10:
# 255
 return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp66);}else{goto _LL1D;}case 9: _LL11: _tmp69=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38)->f1).ret_typ;_tmp68=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38)->f1).args;_tmp67=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38)->f1).cyc_varargs;_LL12: {
# 259
int b=((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,_tmp68) && 
Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp69);
if(_tmp67 != 0){
struct Cyc_Absyn_VarargInfo _tmp39=*_tmp67;struct Cyc_Absyn_VarargInfo _tmp3A=_tmp39;void*_tmp3B;_LL20: _tmp3B=_tmp3A.type;_LL21:;
({int _tmp24A=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp3B);b=_tmp24A;});}
# 265
return b;}case 10: _LL13: _tmp6A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38)->f1;_LL14:
# 268
 return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,_tmp6A);case 11: _LL15: _tmp6C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38)->f1).aggr_info;_tmp6B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38)->f1).targs;_LL16: {
# 271
struct Cyc_Absyn_Aggrdecl*_tmp3C=Cyc_Absyn_get_known_aggrdecl(_tmp6C);
if(_tmp3C->sc == Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp24D=obj;struct _tuple0*_tmp24C=name;struct _dyneither_ptr _tmp24B=({const char*_tmp3D="type";_tag_dyneither(_tmp3D,sizeof(char),5);});Cyc_Interface_static_err(_tmp24D,_tmp24C,_tmp24B,_tmp3C->name);});
return 0;}
# 276
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6B) && 
Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp3C);}case 13: _LL17: _tmp6D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38)->f1;_LL18: {
# 280
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp3E;_push_handler(& _tmp3E);{int _tmp40=0;if(setjmp(_tmp3E.handler))_tmp40=1;if(!_tmp40){({struct Cyc_Absyn_Enumdecl*_tmp24E=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->enumdecls,_tmp6D);ed=_tmp24E;});;_pop_handler();}else{void*_tmp3F=(void*)_exn_thrown;void*_tmp41=_tmp3F;void*_tmp45;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp41)->tag == Cyc_Dict_Absent){_LL23: _LL24:
# 283
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp44;_tmp44.tag=0;({struct _dyneither_ptr _tmp24F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp6D));_tmp44.f1=_tmp24F;});({void*_tmp42[1]={& _tmp44};({struct _dyneither_ptr _tmp250=({const char*_tmp43="check_public_type (can't find enum %s)";_tag_dyneither(_tmp43,sizeof(char),39);});Cyc_aprintf(_tmp250,_tag_dyneither(_tmp42,sizeof(void*),1));});});}));}else{_LL25: _tmp45=_tmp41;_LL26:(int)_rethrow(_tmp45);}_LL22:;}};}
# 286
if(ed->sc == Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp253=obj;struct _tuple0*_tmp252=_tmp6D;struct _dyneither_ptr _tmp251=({const char*_tmp46="enum";_tag_dyneither(_tmp46,sizeof(char),5);});Cyc_Interface_static_err(_tmp253,_tmp252,_tmp251,ed->name);});
return 0;}
# 290
return 1;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38)->f1).datatype_info).KnownDatatype).tag == 2){_LL19: _tmp6F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38)->f1).datatype_info).KnownDatatype).val;_tmp6E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38)->f1).targs;_LL1A: {
# 293
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp47;_push_handler(& _tmp47);{int _tmp49=0;if(setjmp(_tmp47.handler))_tmp49=1;if(!_tmp49){({struct Cyc_Absyn_Datatypedecl*_tmp254=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp6F->name);tud=_tmp254;});;_pop_handler();}else{void*_tmp48=(void*)_exn_thrown;void*_tmp4A=_tmp48;void*_tmp4E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4A)->tag == Cyc_Dict_Absent){_LL28: _LL29:
# 296
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;({struct _dyneither_ptr _tmp255=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp6F->name));_tmp4D.f1=_tmp255;});({void*_tmp4B[1]={& _tmp4D};({struct _dyneither_ptr _tmp256=({const char*_tmp4C="check_public_type (can't find datatype %s)";_tag_dyneither(_tmp4C,sizeof(char),43);});Cyc_aprintf(_tmp256,_tag_dyneither(_tmp4B,sizeof(void*),1));});});}));}else{_LL2A: _tmp4E=_tmp4A;_LL2B:(int)_rethrow(_tmp4E);}_LL27:;}};}
# 299
if(tud->sc == Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp259=obj;struct _tuple0*_tmp258=name;struct _dyneither_ptr _tmp257=({const char*_tmp4F="datatype";_tag_dyneither(_tmp4F,sizeof(char),9);});Cyc_Interface_static_err(_tmp259,_tmp258,_tmp257,tud->name);});
return 0;}
# 303
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6E) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}else{goto _LL1D;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).field_info).KnownDatatypefield).tag == 2){_LL1B: _tmp72=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).field_info).KnownDatatypefield).val).f1;_tmp71=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).field_info).KnownDatatypefield).val).f2;_tmp70=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).targs;_LL1C: {
# 307
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp50;_push_handler(& _tmp50);{int _tmp52=0;if(setjmp(_tmp50.handler))_tmp52=1;if(!_tmp52){({struct Cyc_Absyn_Datatypedecl*_tmp25A=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp72->name);tud=_tmp25A;});;_pop_handler();}else{void*_tmp51=(void*)_exn_thrown;void*_tmp53=_tmp51;void*_tmp57;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp53)->tag == Cyc_Dict_Absent){_LL2D: _LL2E:
# 310
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;({struct _dyneither_ptr _tmp25B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp72->name));_tmp56.f1=_tmp25B;});({void*_tmp54[1]={& _tmp56};({struct _dyneither_ptr _tmp25C=({const char*_tmp55="check_public_type (can't find datatype %s and search its fields)";_tag_dyneither(_tmp55,sizeof(char),65);});Cyc_aprintf(_tmp25C,_tag_dyneither(_tmp54,sizeof(void*),1));});});}));}else{_LL2F: _tmp57=_tmp53;_LL30:(int)_rethrow(_tmp57);}_LL2C:;}};}
# 313
if(tud->fields == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;({struct _dyneither_ptr _tmp25D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name));_tmp5A.f1=_tmp25D;});({void*_tmp58[1]={& _tmp5A};({struct _dyneither_ptr _tmp25E=({const char*_tmp59="check_public_type (datatype %s has no fields)";_tag_dyneither(_tmp59,sizeof(char),46);});Cyc_aprintf(_tmp25E,_tag_dyneither(_tmp58,sizeof(void*),1));});});}));{
# 317
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp5B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp5B != 0;_tmp5B=_tmp5B->tl){
if(({Cyc_strptrcmp((*_tmp71->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp5B->hd)->name).f2)== 0;})){
tuf1=(struct Cyc_Absyn_Datatypefield*)_tmp5B->hd;
break;}}}
# 324
if(tuf1 == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;({struct _dyneither_ptr _tmp25F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp71->name));_tmp5E.f1=_tmp25F;});({void*_tmp5C[1]={& _tmp5E};({struct _dyneither_ptr _tmp260=({const char*_tmp5D="check_public_type (can't find datatypefield %s)";_tag_dyneither(_tmp5D,sizeof(char),48);});Cyc_aprintf(_tmp260,_tag_dyneither(_tmp5C,sizeof(void*),1));});});}));{
struct Cyc_Absyn_Datatypefield*tuf=tuf1;
# 329
if(tud->sc == Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp263=obj;struct _tuple0*_tmp262=name;struct _dyneither_ptr _tmp261=({const char*_tmp5F="datatype";_tag_dyneither(_tmp5F,sizeof(char),9);});Cyc_Interface_static_err(_tmp263,_tmp262,_tmp261,tud->name);});
return 0;}
# 333
if(tud->sc == Cyc_Absyn_Abstract){
({struct _dyneither_ptr _tmp266=obj;struct _tuple0*_tmp265=name;struct _dyneither_ptr _tmp264=({const char*_tmp60="datatype";_tag_dyneither(_tmp60,sizeof(char),9);});Cyc_Interface_abstract_err(_tmp266,_tmp265,_tmp264,tud->name);});
return 0;}
# 337
if(tuf->sc == Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp26B=obj;struct _tuple0*_tmp26A=name;struct _dyneither_ptr _tmp269=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0;({struct _dyneither_ptr _tmp267=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));_tmp63.f1=_tmp267;});({void*_tmp61[1]={& _tmp63};({struct _dyneither_ptr _tmp268=({const char*_tmp62="field %s of";_tag_dyneither(_tmp62,sizeof(char),12);});Cyc_aprintf(_tmp268,_tag_dyneither(_tmp61,sizeof(void*),1));});});});
# 338
Cyc_Interface_static_err(_tmp26B,_tmp26A,_tmp269,tud->name);});
# 340
return 0;}
# 342
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp70) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};}}else{goto _LL1D;}default: _LL1D: _LL1E:
# 345
 return 1;}_LLA:;}struct _tuple21{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Tcenv_Genv*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 351
static void Cyc_Interface_extract_aggrdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){
# 353
struct _tuple21*_tmp73=env;struct Cyc_Interface_Ienv*_tmp84;struct Cyc_Interface_Ienv*_tmp83;int _tmp82;struct Cyc_Tcenv_Genv*_tmp81;struct Cyc_Interface_Seen*_tmp80;_LL32: _tmp84=_tmp73->f1;_tmp83=_tmp73->f2;_tmp82=_tmp73->f3;_tmp81=_tmp73->f4;_tmp80=_tmp73->f5;_LL33:;{
struct Cyc_Absyn_Aggrdecl*_tmp74=*dp;
enum Cyc_Absyn_Scope _tmp75=_tmp74->sc;enum Cyc_Absyn_Scope _tmp76=_tmp75;switch(_tmp76){case Cyc_Absyn_Static: _LL35: _LL36:
# 357
 if(_tmp82  && _tmp74->impl == 0)
({struct _dyneither_ptr _tmp26D=({const char*_tmp77="static";_tag_dyneither(_tmp77,sizeof(char),7);});struct _dyneither_ptr _tmp26C=({const char*_tmp78="struct/union";_tag_dyneither(_tmp78,sizeof(char),13);});Cyc_Interface_fields_err(_tmp26D,_tmp26C,_tmp74->name);});
goto _LL34;case Cyc_Absyn_Abstract: _LL37: _LL38:
# 361
 if(_tmp74->impl == 0){
if(_tmp82)
({struct _dyneither_ptr _tmp26F=({const char*_tmp79="abstract";_tag_dyneither(_tmp79,sizeof(char),9);});struct _dyneither_ptr _tmp26E=({const char*_tmp7A="struct/union";_tag_dyneither(_tmp7A,sizeof(char),13);});Cyc_Interface_fields_err(_tmp26F,_tmp26E,_tmp74->name);});}else{
# 365
({struct Cyc_Absyn_Aggrdecl*_tmp270=({struct Cyc_Absyn_Aggrdecl*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=*_tmp74;_tmp7B;});_tmp74=_tmp270;});
_tmp74->impl=0;}
# 368
if(Cyc_Interface_check_public_aggrdecl(_tmp81,_tmp80,_tmp74))
({struct Cyc_Dict_Dict _tmp271=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp83->aggrdecls,x,_tmp74);_tmp83->aggrdecls=_tmp271;});
goto _LL34;case Cyc_Absyn_Public: _LL39: _LL3A:
# 372
 if(_tmp74->impl == 0){
({struct _dyneither_ptr _tmp273=({const char*_tmp7C="public";_tag_dyneither(_tmp7C,sizeof(char),7);});struct _dyneither_ptr _tmp272=({const char*_tmp7D="struct/union";_tag_dyneither(_tmp7D,sizeof(char),13);});Cyc_Interface_fields_err(_tmp273,_tmp272,_tmp74->name);});
({struct Cyc_Absyn_Aggrdecl*_tmp274=({struct Cyc_Absyn_Aggrdecl*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=*_tmp74;_tmp7E;});_tmp74=_tmp274;});
_tmp74->sc=Cyc_Absyn_Abstract;}
# 377
if(Cyc_Interface_check_public_aggrdecl(_tmp81,_tmp80,_tmp74))
({struct Cyc_Dict_Dict _tmp275=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp83->aggrdecls,x,_tmp74);_tmp83->aggrdecls=_tmp275;});
goto _LL34;case Cyc_Absyn_ExternC: _LL3B: _LL3C:
 goto _LL3E;case Cyc_Absyn_Extern: _LL3D: _LL3E:
# 382
 if(Cyc_Interface_check_public_aggrdecl(_tmp81,_tmp80,_tmp74))
({struct Cyc_Dict_Dict _tmp276=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp84->aggrdecls,x,_tmp74);_tmp84->aggrdecls=_tmp276;});
goto _LL34;default: _LL3F: _LL40:
# 386
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp7F="add_aggrdecl";_tag_dyneither(_tmp7F,sizeof(char),13);}));
goto _LL34;}_LL34:;};}
# 390
static void Cyc_Interface_extract_enumdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){
# 392
struct _tuple21*_tmp85=env;struct Cyc_Interface_Ienv*_tmp96;struct Cyc_Interface_Ienv*_tmp95;int _tmp94;struct Cyc_Tcenv_Genv*_tmp93;struct Cyc_Interface_Seen*_tmp92;_LL42: _tmp96=_tmp85->f1;_tmp95=_tmp85->f2;_tmp94=_tmp85->f3;_tmp93=_tmp85->f4;_tmp92=_tmp85->f5;_LL43:;{
struct Cyc_Absyn_Enumdecl*_tmp86=*dp;
enum Cyc_Absyn_Scope _tmp87=_tmp86->sc;enum Cyc_Absyn_Scope _tmp88=_tmp87;switch(_tmp88){case Cyc_Absyn_Static: _LL45: _LL46:
# 396
 if(_tmp94  && _tmp86->fields == 0)
({struct _dyneither_ptr _tmp278=({const char*_tmp89="static";_tag_dyneither(_tmp89,sizeof(char),7);});struct _dyneither_ptr _tmp277=({const char*_tmp8A="enum";_tag_dyneither(_tmp8A,sizeof(char),5);});Cyc_Interface_fields_err(_tmp278,_tmp277,_tmp86->name);});
goto _LL44;case Cyc_Absyn_Abstract: _LL47: _LL48:
# 400
 if(_tmp86->fields == 0){
if(_tmp94)
({struct _dyneither_ptr _tmp27A=({const char*_tmp8B="abstract";_tag_dyneither(_tmp8B,sizeof(char),9);});struct _dyneither_ptr _tmp279=({const char*_tmp8C="enum";_tag_dyneither(_tmp8C,sizeof(char),5);});Cyc_Interface_fields_err(_tmp27A,_tmp279,_tmp86->name);});}else{
# 404
({struct Cyc_Absyn_Enumdecl*_tmp27B=({struct Cyc_Absyn_Enumdecl*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=*_tmp86;_tmp8D;});_tmp86=_tmp27B;});
_tmp86->fields=0;}
# 407
if(Cyc_Interface_check_public_enumdecl(_tmp93,_tmp92,_tmp86))
({struct Cyc_Dict_Dict _tmp27C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp95->enumdecls,x,_tmp86);_tmp95->enumdecls=_tmp27C;});
goto _LL44;case Cyc_Absyn_Public: _LL49: _LL4A:
# 411
 if(_tmp86->fields == 0){
({struct _dyneither_ptr _tmp27E=({const char*_tmp8E="public";_tag_dyneither(_tmp8E,sizeof(char),7);});struct _dyneither_ptr _tmp27D=({const char*_tmp8F="enum";_tag_dyneither(_tmp8F,sizeof(char),5);});Cyc_Interface_fields_err(_tmp27E,_tmp27D,_tmp86->name);});
({struct Cyc_Absyn_Enumdecl*_tmp27F=({struct Cyc_Absyn_Enumdecl*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=*_tmp86;_tmp90;});_tmp86=_tmp27F;});
_tmp86->sc=Cyc_Absyn_Abstract;}
# 416
if(Cyc_Interface_check_public_enumdecl(_tmp93,_tmp92,_tmp86))
({struct Cyc_Dict_Dict _tmp280=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp95->enumdecls,x,_tmp86);_tmp95->enumdecls=_tmp280;});
goto _LL44;case Cyc_Absyn_ExternC: _LL4B: _LL4C:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4D: _LL4E:
# 421
 if(Cyc_Interface_check_public_enumdecl(_tmp93,_tmp92,_tmp86))
({struct Cyc_Dict_Dict _tmp281=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp96->enumdecls,x,_tmp86);_tmp96->enumdecls=_tmp281;});
goto _LL44;default: _LL4F: _LL50:
# 425
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp91="add_enumdecl";_tag_dyneither(_tmp91,sizeof(char),13);}));
goto _LL44;}_LL44:;};}
# 430
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 432
struct Cyc_Interface_Ienv*env;
{enum Cyc_Absyn_Scope _tmp97=f->sc;enum Cyc_Absyn_Scope _tmp98=_tmp97;switch(_tmp98){case Cyc_Absyn_Static: _LL52: _LL53:
 return;case Cyc_Absyn_Extern: _LL54: _LL55:
 env=i->imports;goto _LL51;case Cyc_Absyn_Public: _LL56: _LL57:
 env=i->exports;goto _LL51;default: _LL58: _LL59:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp99="add_xdatatypefielddecl";_tag_dyneither(_tmp99,sizeof(char),23);}));}_LL51:;}
# 440
({struct Cyc_Dict_Dict _tmp284=({
struct Cyc_Dict_Dict _tmp283=env->xdatatypefielddecls;struct _tuple0*_tmp282=f->name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(_tmp283,_tmp282,({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->base=d;_tmp9A->field=f;_tmp9A;}));});
# 440
env->xdatatypefielddecls=_tmp284;});}
# 445
static void Cyc_Interface_extract_datatypedecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){
# 447
struct _tuple21*_tmp9B=env;struct Cyc_Interface_Ienv*_tmpB3;struct Cyc_Interface_Ienv*_tmpB2;int _tmpB1;struct Cyc_Tcenv_Genv*_tmpB0;struct Cyc_Interface_Seen*_tmpAF;struct Cyc_Interface_I*_tmpAE;_LL5B: _tmpB3=_tmp9B->f1;_tmpB2=_tmp9B->f2;_tmpB1=_tmp9B->f3;_tmpB0=_tmp9B->f4;_tmpAF=_tmp9B->f5;_tmpAE=_tmp9B->f6;_LL5C:;{
struct Cyc_Absyn_Datatypedecl*_tmp9C=*dp;
# 450
enum Cyc_Absyn_Scope _tmp9D=_tmp9C->sc;enum Cyc_Absyn_Scope _tmp9E=_tmp9D;switch(_tmp9E){case Cyc_Absyn_Static: _LL5E: _LL5F:
# 452
 if((!_tmp9C->is_extensible  && _tmpB1) && _tmp9C->fields == 0)
({struct _dyneither_ptr _tmp286=({const char*_tmp9F="static";_tag_dyneither(_tmp9F,sizeof(char),7);});struct _dyneither_ptr _tmp285=({const char*_tmpA0="datatype";_tag_dyneither(_tmpA0,sizeof(char),9);});Cyc_Interface_fields_err(_tmp286,_tmp285,_tmp9C->name);});
# 455
goto _LL5D;case Cyc_Absyn_Abstract: _LL60: _LL61:
# 457
 if(_tmp9C->fields == 0){
if(!_tmp9C->is_extensible  && _tmpB1)
({struct _dyneither_ptr _tmp288=({const char*_tmpA1="abstract";_tag_dyneither(_tmpA1,sizeof(char),9);});struct _dyneither_ptr _tmp287=({const char*_tmpA2="datatype";_tag_dyneither(_tmpA2,sizeof(char),9);});Cyc_Interface_fields_err(_tmp288,_tmp287,_tmp9C->name);});}else{
# 462
({struct Cyc_Absyn_Datatypedecl*_tmp289=({struct Cyc_Absyn_Datatypedecl*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=*_tmp9C;_tmpA3;});_tmp9C=_tmp289;});
_tmp9C->fields=0;}
# 465
if(Cyc_Interface_check_public_datatypedecl(_tmpB0,_tmpAF,_tmp9C))
({struct Cyc_Dict_Dict _tmp28A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB2->datatypedecls,x,_tmp9C);_tmpB2->datatypedecls=_tmp28A;});
goto _LL5D;case Cyc_Absyn_Public: _LL62: _LL63:
# 469
({struct Cyc_Absyn_Datatypedecl*_tmp28B=({struct Cyc_Absyn_Datatypedecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=*_tmp9C;_tmpA4;});_tmp9C=_tmp28B;});
if(!_tmp9C->is_extensible  && _tmp9C->fields == 0){
({struct _dyneither_ptr _tmp28D=({const char*_tmpA5="public";_tag_dyneither(_tmpA5,sizeof(char),7);});struct _dyneither_ptr _tmp28C=({const char*_tmpA6="datatype";_tag_dyneither(_tmpA6,sizeof(char),9);});Cyc_Interface_fields_err(_tmp28D,_tmp28C,_tmp9C->name);});
_tmp9C->sc=Cyc_Absyn_Abstract;}
# 474
if(Cyc_Interface_check_public_datatypedecl(_tmpB0,_tmpAF,_tmp9C)){
if(_tmp9C->is_extensible  && _tmp9C->fields != 0){
struct Cyc_List_List*_tmpA7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->fields))->v;
_tmp9C->fields=0;{
struct Cyc_List_List*_tmpA8=_tmpA7;for(0;_tmpA8 != 0;_tmpA8=_tmpA8->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpAE,_tmp9C,(struct Cyc_Absyn_Datatypefield*)_tmpA8->hd);}};}
# 482
({struct Cyc_Dict_Dict _tmp28E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB2->datatypedecls,x,_tmp9C);_tmpB2->datatypedecls=_tmp28E;});}
# 484
goto _LL5D;case Cyc_Absyn_ExternC: _LL64: _LL65:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpA9="extract_datatypedecl";_tag_dyneither(_tmpA9,sizeof(char),21);}));case Cyc_Absyn_Extern: _LL66: _LL67:
# 487
 if(Cyc_Interface_check_public_datatypedecl(_tmpB0,_tmpAF,_tmp9C)){
if(_tmp9C->is_extensible  && _tmp9C->fields != 0){
({struct Cyc_Absyn_Datatypedecl*_tmp28F=({struct Cyc_Absyn_Datatypedecl*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=*_tmp9C;_tmpAA;});_tmp9C=_tmp28F;});{
struct Cyc_List_List*_tmpAB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->fields))->v;
_tmp9C->fields=0;{
struct Cyc_List_List*_tmpAC=_tmpAB;for(0;_tmpAC != 0;_tmpAC=_tmpAC->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpAE,_tmp9C,(struct Cyc_Absyn_Datatypefield*)_tmpAC->hd);}};};}
# 496
({struct Cyc_Dict_Dict _tmp290=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB3->datatypedecls,x,_tmp9C);_tmpB3->datatypedecls=_tmp290;});}
# 498
goto _LL5D;default: _LL68: _LL69:
# 500
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpAD="add_datatypedecl";_tag_dyneither(_tmpAD,sizeof(char),17);}));
goto _LL5D;}_LL5D:;};}
# 505
static void Cyc_Interface_extract_typedef(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){
# 507
struct _tuple21*_tmpB4=env;struct Cyc_Interface_Ienv*_tmpB7;struct Cyc_Tcenv_Genv*_tmpB6;struct Cyc_Interface_Seen*_tmpB5;_LL6B: _tmpB7=_tmpB4->f2;_tmpB6=_tmpB4->f4;_tmpB5=_tmpB4->f5;_LL6C:;
if(Cyc_Interface_check_public_typedefdecl(_tmpB6,_tmpB5,d))
({struct Cyc_Dict_Dict _tmp291=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpB7->typedefdecls,x,d);_tmpB7->typedefdecls=_tmp291;});}struct _tuple22{void*f1;int f2;};
# 512
static void Cyc_Interface_extract_ordinarie(struct _tuple21*env,struct _tuple0*x,struct _tuple22*v){
# 514
struct _tuple21*_tmpB8=env;struct Cyc_Interface_Ienv*_tmpC9;struct Cyc_Interface_Ienv*_tmpC8;int _tmpC7;struct Cyc_Tcenv_Genv*_tmpC6;struct Cyc_Interface_Seen*_tmpC5;_LL6E: _tmpC9=_tmpB8->f1;_tmpC8=_tmpB8->f2;_tmpC7=_tmpB8->f3;_tmpC6=_tmpB8->f4;_tmpC5=_tmpB8->f5;_LL6F:;{
# 516
void*_tmpB9=(*v).f1;void*_tmpBA=_tmpB9;struct Cyc_Absyn_Vardecl*_tmpC4;struct Cyc_Absyn_Fndecl*_tmpC3;switch(*((int*)_tmpBA)){case 2: _LL71: _tmpC3=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpBA)->f1;_LL72: {
# 518
struct Cyc_Absyn_Vardecl*_tmpBB=({struct Cyc_Absyn_Vardecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->sc=_tmpC3->sc;_tmpBC->name=_tmpC3->name;_tmpBC->varloc=0;({struct Cyc_Absyn_Tqual _tmp292=
# 522
Cyc_Absyn_empty_tqual(0);_tmpBC->tq=_tmp292;});_tmpBC->type=(void*)_check_null(_tmpC3->cached_typ);_tmpBC->initializer=0;_tmpBC->rgn=0;_tmpBC->attributes=0;_tmpBC->escapes=0;_tmpBC;});
# 529
_tmpC7=0;
_tmpC4=_tmpBB;goto _LL74;}case 1: _LL73: _tmpC4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpBA)->f1;_LL74:
# 532
 if(_tmpC4->initializer != 0){
({struct Cyc_Absyn_Vardecl*_tmp293=({struct Cyc_Absyn_Vardecl*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=*_tmpC4;_tmpBD;});_tmpC4=_tmp293;});
_tmpC4->initializer=0;}
# 536
{enum Cyc_Absyn_Scope _tmpBE=_tmpC4->sc;enum Cyc_Absyn_Scope _tmpBF=_tmpBE;switch(_tmpBF){case Cyc_Absyn_Static: _LL78: _LL79:
# 538
 if(_tmpC7  && Cyc_Tcutil_is_function_type(_tmpC4->type))
({struct _dyneither_ptr _tmp294=({const char*_tmpC0="static";_tag_dyneither(_tmpC0,sizeof(char),7);});Cyc_Interface_body_err(_tmp294,_tmpC4->name);});
# 541
goto _LL77;case Cyc_Absyn_Register: _LL7A: _LL7B:
 goto _LL7D;case Cyc_Absyn_Abstract: _LL7C: _LL7D:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpC1="extract_ordinarie";_tag_dyneither(_tmpC1,sizeof(char),18);}));case Cyc_Absyn_Public: _LL7E: _LL7F:
# 545
 if(_tmpC7  && Cyc_Tcutil_is_function_type(_tmpC4->type))
({struct _dyneither_ptr _tmp295=({const char*_tmpC2="public";_tag_dyneither(_tmpC2,sizeof(char),7);});Cyc_Interface_body_err(_tmp295,_tmpC4->name);});
# 548
if(Cyc_Interface_check_public_vardecl(_tmpC6,_tmpC5,_tmpC4))
({struct Cyc_Dict_Dict _tmp296=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpC8->vardecls,x,_tmpC4);_tmpC8->vardecls=_tmp296;});
goto _LL77;case Cyc_Absyn_ExternC: _LL80: _LL81:
 goto _LL83;default: _LL82: _LL83:
# 553
 if(Cyc_Interface_check_public_vardecl(_tmpC6,_tmpC5,_tmpC4))
({struct Cyc_Dict_Dict _tmp297=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpC9->vardecls,x,_tmpC4);_tmpC9->vardecls=_tmp297;});
goto _LL77;}_LL77:;}
# 557
goto _LL70;default: _LL75: _LL76:
 goto _LL70;}_LL70:;};}struct _tuple23{void*f1;void*f2;};
# 562
static struct Cyc_List_List*Cyc_Interface_remove_decl_from_list(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*d){
if(l == 0)return 0;
{struct _tuple23 _tmpCA=({struct _tuple23 _tmpD3;_tmpD3.f1=d->r;_tmpD3.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmpD3;});struct _tuple23 _tmpCB=_tmpCA;struct Cyc_Absyn_Vardecl**_tmpD2;struct Cyc_Absyn_Vardecl*_tmpD1;struct Cyc_Absyn_Enumdecl*_tmpD0;struct Cyc_Absyn_Enumdecl*_tmpCF;struct Cyc_Absyn_Typedefdecl*_tmpCE;struct Cyc_Absyn_Typedefdecl*_tmpCD;switch(*((int*)_tmpCB.f1)){case 8: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCB.f2)->tag == 8){_LL85: _tmpCE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCB.f1)->f1;_tmpCD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCB.f2)->f1;_LL86:
# 566
 if(({Cyc_Absyn_qvar_cmp(_tmpCE->name,_tmpCD->name)!= 0;}))goto _LL84;
return Cyc_Interface_remove_decl_from_list(l->tl,d);}else{goto _LL8B;}case 7: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCB.f2)->tag == 7){_LL87: _tmpD0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCB.f1)->f1;_tmpCF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCB.f2)->f1;_LL88:
# 569
 if(({Cyc_Absyn_qvar_cmp(_tmpD0->name,_tmpCF->name)!= 0;}))goto _LL84;
if(_tmpD0->sc == Cyc_Absyn_Extern)_tmpD0->sc=_tmpCF->sc;
return Cyc_Interface_remove_decl_from_list(l->tl,d);
goto _LL84;}else{goto _LL8B;}case 0: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCB.f2)->tag == 0){_LL89: _tmpD2=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCB.f1)->f1;_tmpD1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCB.f2)->f1;_LL8A:
# 574
 if(({Cyc_Absyn_qvar_cmp((*_tmpD2)->name,_tmpD1->name)!= 0;}))goto _LL84;
if((*_tmpD2)->sc == Cyc_Absyn_Extern)({struct Cyc_Absyn_Vardecl*_tmp298=({struct Cyc_Absyn_Vardecl*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=*_tmpD1;_tmpCC;});*_tmpD2=_tmp298;});
return Cyc_Interface_remove_decl_from_list(l->tl,d);}else{goto _LL8B;}default: _LL8B: _LL8C:
# 578
 goto _LL84;}_LL84:;}
# 580
return({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=(struct Cyc_Absyn_Decl*)l->hd;({struct Cyc_List_List*_tmp299=Cyc_Interface_remove_decl_from_list(l->tl,d);_tmpD4->tl=_tmp299;});_tmpD4;});}
# 583
static struct Cyc_List_List*Cyc_Interface_uniqify_decl_list(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
if(accum == 0)return({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->hd=d;_tmpD5->tl=0;_tmpD5;});{
struct Cyc_List_List*l;
for(l=accum;l != 0;l=l->tl){
struct _tuple23 _tmpD6=({struct _tuple23 _tmpE4;_tmpE4.f1=d->r;_tmpE4.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmpE4;});struct _tuple23 _tmpD7=_tmpD6;struct _dyneither_ptr*_tmpE3;struct Cyc_List_List**_tmpE2;struct _dyneither_ptr*_tmpE1;struct Cyc_List_List*_tmpE0;struct Cyc_Absyn_Enumdecl*_tmpDF;struct Cyc_Absyn_Enumdecl*_tmpDE;struct Cyc_Absyn_Typedefdecl*_tmpDD;struct Cyc_Absyn_Typedefdecl*_tmpDC;struct Cyc_Absyn_Vardecl*_tmpDB;struct Cyc_Absyn_Vardecl**_tmpDA;switch(*((int*)_tmpD7.f1)){case 0: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->tag == 0){_LL8E: _tmpDB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD7.f1)->f1;_tmpDA=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->f1;_LL8F:
# 589
 if(({Cyc_Absyn_qvar_cmp(_tmpDB->name,(*_tmpDA)->name)!= 0;}))goto _LL8D;
if((*_tmpDA)->sc == Cyc_Absyn_Extern)({struct Cyc_Absyn_Vardecl*_tmp29A=({struct Cyc_Absyn_Vardecl*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=*_tmpDB;_tmpD8;});*_tmpDA=_tmp29A;});
return accum;}else{goto _LL96;}case 8: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->tag == 8){_LL90: _tmpDD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD7.f1)->f1;_tmpDC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->f1;_LL91:
# 593
 if(({Cyc_Absyn_qvar_cmp(_tmpDD->name,_tmpDC->name)!= 0;}))goto _LL8D;
return accum;}else{goto _LL96;}case 7: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->tag == 7){_LL92: _tmpDF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD7.f1)->f1;_tmpDE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->f1;_LL93:
# 596
 if(({Cyc_Absyn_qvar_cmp(_tmpDF->name,_tmpDE->name)!= 0;}))goto _LL8D;
return accum;}else{goto _LL96;}case 9: if(((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->tag == 9){_LL94: _tmpE3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD7.f1)->f1;_tmpE2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD7.f1)->f2;_tmpE1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->f1;_tmpE0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD7.f2)->f2;_LL95:
# 599
 if(({Cyc_strptrcmp(_tmpE3,_tmpE1)!= 0;}))goto _LL8D;{
struct Cyc_List_List*_tmpD9=_tmpE0;
for(0;_tmpD9 != 0;_tmpD9=_tmpD9->tl){
({struct Cyc_List_List*_tmp29B=Cyc_Interface_remove_decl_from_list(*_tmpE2,(struct Cyc_Absyn_Decl*)_tmpD9->hd);*_tmpE2=_tmp29B;});}
# 604
goto _LL8D;};}else{goto _LL96;}default: _LL96: _LL97:
# 606
 goto _LL8D;}_LL8D:;}
# 609
return({struct Cyc_List_List*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=d;_tmpE5->tl=accum;_tmpE5;});};}
# 612
static struct Cyc_List_List*Cyc_Interface_filterstatics(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
{void*_tmpE6=d->r;void*_tmpE7=_tmpE6;struct Cyc_List_List*_tmpFD;struct _dyneither_ptr*_tmpFC;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Typedefdecl*_tmpFA;struct Cyc_Absyn_Enumdecl*_tmpF9;struct Cyc_Absyn_Datatypedecl*_tmpF8;struct Cyc_Absyn_Aggrdecl*_tmpF7;struct Cyc_Absyn_Fndecl*_tmpF6;struct Cyc_Absyn_Vardecl*_tmpF5;switch(*((int*)_tmpE7)){case 0: _LL99: _tmpF5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_LL9A:
# 615
 if(_tmpF5->sc == Cyc_Absyn_ExternC)return accum;
if(_tmpF5->sc == Cyc_Absyn_Static)return accum;{
struct Cyc_Absyn_Vardecl*_tmpE8=({struct Cyc_Absyn_Vardecl*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=*_tmpF5;_tmpEC;});
_tmpE8->initializer=0;
if(Cyc_Tcutil_is_function_type(_tmpE8->type) && _tmpE8->sc != Cyc_Absyn_Extern)
_tmpE8->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct Cyc_Absyn_Decl*_tmp29D=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp29C=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=_tmpE8;_tmpEB;});_tmpEA[0]=_tmp29C;});_tmpEA;}),0);});_tmpE9->hd=_tmp29D;});_tmpE9->tl=accum;_tmpE9;});};case 1: _LL9B: _tmpF6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_LL9C:
# 623
 if(_tmpF6->sc == Cyc_Absyn_Static)return accum;
if(_tmpF6->sc == Cyc_Absyn_ExternC)return accum;{
struct Cyc_Absyn_Vardecl*_tmpED=Cyc_Absyn_new_vardecl(0,_tmpF6->name,(void*)_check_null(_tmpF6->cached_typ),0);
_tmpED->sc=_tmpF6->sc;
return({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_Absyn_Decl*_tmp29F=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp29E=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpF0;_tmpF0.tag=0;_tmpF0.f1=_tmpED;_tmpF0;});_tmpEF[0]=_tmp29E;});_tmpEF;}),0);});_tmpEE->hd=_tmp29F;});_tmpEE->tl=accum;_tmpEE;});};case 5: _LL9D: _tmpF7=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_LL9E:
# 630
 if(_tmpF7->sc == Cyc_Absyn_ExternC)return accum;
goto _LL98;case 6: _LL9F: _tmpF8=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_LLA0:
# 634
 if(_tmpF8->sc == Cyc_Absyn_ExternC)return accum;
goto _LL98;case 7: _LLA1: _tmpF9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_LLA2:
# 637
 if(_tmpF9->sc == Cyc_Absyn_Static)return accum;
if(_tmpF9->sc == Cyc_Absyn_ExternC)return accum;
goto _LL98;case 8: _LLA3: _tmpFA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_LLA4:
# 641
 goto _LL98;case 9: _LLA5: _tmpFC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpE7)->f1;_tmpFB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpE7)->f2;_LLA6: {
# 643
struct Cyc_List_List*_tmpF1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,_tmpFB));
return({struct Cyc_List_List*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct Cyc_Absyn_Decl*_tmp2A1=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp2A0=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpF4;_tmpF4.tag=9;_tmpF4.f1=_tmpFC;_tmpF4.f2=_tmpF1;_tmpF4;});_tmpF3[0]=_tmp2A0;});_tmpF3;}),0);});_tmpF2->hd=_tmp2A1;});_tmpF2->tl=accum;_tmpF2;});}case 10: _LLA7: _tmpFD=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpE7)->f2;_LLA8:
# 646
 return({struct Cyc_List_List*_tmp2A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,_tmpFD);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2A2,accum);});case 2: _LLA9: _LLAA:
 goto _LLAC;case 3: _LLAB: _LLAC:
 goto _LLAE;case 4: _LLAD: _LLAE:
 goto _LLB0;case 11: _LLAF: _LLB0:
 goto _LLB2;case 12: _LLB1: _LLB2:
 goto _LLB4;case 13: _LLB3: _LLB4:
 goto _LLB6;default: _LLB5: _LLB6:
 return accum;}_LL98:;}
# 655
return({struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*_tmpFE));({struct Cyc_Absyn_Decl*_tmp2A3=({struct Cyc_Absyn_Decl*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=*d;_tmpFF;});_tmpFE->hd=_tmp2A3;});_tmpFE->tl=accum;_tmpFE;});}struct _tuple24{struct Cyc_Interface_I*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;};
# 658
static void Cyc_Interface_extract_f(struct _tuple24*env_f){
struct _tuple24*_tmp100=env_f;struct Cyc_Interface_I*_tmp106;int _tmp105;struct Cyc_Tcenv_Genv*_tmp104;struct Cyc_Interface_Seen*_tmp103;_LLB8: _tmp106=_tmp100->f1;_tmp105=_tmp100->f2;_tmp104=_tmp100->f3;_tmp103=_tmp100->f4;_LLB9:;{
struct _tuple21 _tmp101=({struct _tuple21 _tmp102;_tmp102.f1=_tmp106->imports;_tmp102.f2=_tmp106->exports;_tmp102.f3=_tmp105;_tmp102.f4=_tmp104;_tmp102.f5=_tmp103;_tmp102.f6=_tmp106;_tmp102;});
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp101,_tmp104->aggrdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& _tmp101,_tmp104->datatypedecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmp101,_tmp104->enumdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmp101,_tmp104->typedefs);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct _tuple22*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmp101,_tmp104->ordinaries);};}
# 668
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs,struct Cyc_List_List*tds){
struct _tuple24 env=({struct _tuple24 _tmp108;({struct Cyc_Interface_I*_tmp2A4=
Cyc_Interface_empty();_tmp108.f1=_tmp2A4;});_tmp108.f2=check_complete_defs;_tmp108.f3=ae;({struct Cyc_Interface_Seen*_tmp2A5=Cyc_Interface_new_seen();_tmp108.f4=_tmp2A5;});_tmp108;});
Cyc_Interface_extract_f(& env);{
struct Cyc_Interface_I*_tmp107=env.f1;
({struct Cyc_List_List*_tmp2A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,tds));_tmp107->tds=_tmp2A6;});
({struct Cyc_List_List*_tmp2A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,0,_tmp107->tds));_tmp107->tds=_tmp2A7;});
return _tmp107;};}
# 678
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae,struct Cyc_List_List*tds){
return Cyc_Interface_gen_extract(ae,1,tds);}
# 684
inline static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){
({struct Cyc_String_pa_PrintArg_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg2);({void*_tmp109[1]={& _tmp10B};({struct _dyneither_ptr*_tmp2A9=msg1;struct _dyneither_ptr _tmp2A8=({const char*_tmp10A="%s";_tag_dyneither(_tmp10A,sizeof(char),3);});Cyc_Tcdecl_merr(0,_tmp2A9,_tmp2A8,_tag_dyneither(_tmp109,sizeof(void*),1));});});});}struct _tuple25{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 688
static void Cyc_Interface_incl_dict_f(struct _tuple25*env,struct _tuple0*x,void*y1){
# 695
struct _tuple25*_tmp10C=env;int*_tmp11B;struct Cyc_Dict_Dict _tmp11A;int(*_tmp119)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp118;struct _dyneither_ptr*_tmp117;_LLBB: _tmp11B=(int*)& _tmp10C->f1;_tmp11A=_tmp10C->f2;_tmp119=_tmp10C->f3;_tmp118=_tmp10C->f4;_tmp117=_tmp10C->f5;_LLBC:;{
struct _handler_cons _tmp10D;_push_handler(& _tmp10D);{int _tmp10F=0;if(setjmp(_tmp10D.handler))_tmp10F=1;if(!_tmp10F){
{void*_tmp110=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp11A,x);
if(!_tmp119(y1,_tmp110,_tmp117))*_tmp11B=0;}
# 697
;_pop_handler();}else{void*_tmp10E=(void*)_exn_thrown;void*_tmp111=_tmp10E;void*_tmp116;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp111)->tag == Cyc_Dict_Absent){_LLBE: _LLBF:
# 701
({struct _dyneither_ptr*_tmp2AC=_tmp117;Cyc_Interface_check_err(_tmp2AC,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp115;_tmp115.tag=0;({struct _dyneither_ptr _tmp2AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));_tmp115.f1=_tmp2AA;});({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0;_tmp114.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp118);({void*_tmp112[2]={& _tmp114,& _tmp115};({struct _dyneither_ptr _tmp2AB=({const char*_tmp113="%s %s is missing";_tag_dyneither(_tmp113,sizeof(char),17);});Cyc_aprintf(_tmp2AB,_tag_dyneither(_tmp112,sizeof(void*),2));});});});}));});
*_tmp11B=0;
goto _LLBD;}else{_LLC0: _tmp116=_tmp111;_LLC1:(int)_rethrow(_tmp116);}_LLBD:;}};};}
# 707
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 712
struct _tuple25 _tmp11C=({struct _tuple25 _tmp11D;_tmp11D.f1=1;_tmp11D.f2=dic2;_tmp11D.f3=incl_f;_tmp11D.f4=t;_tmp11D.f5=msg;_tmp11D;});
((void(*)(void(*f)(struct _tuple25*,struct _tuple0*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp11C,dic1);
return _tmp11C.f1;}
# 729 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmp11E=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmp11E == 0)return 0;if(d0 != _tmp11E){({struct _dyneither_ptr*_tmp2AF=msg;Cyc_Interface_check_err(_tmp2AF,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp121;_tmp121.tag=0;({struct _dyneither_ptr _tmp2AD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp121.f1=_tmp2AD;});({void*_tmp11F[1]={& _tmp121};({struct _dyneither_ptr _tmp2AE=({const char*_tmp120="declaration of type %s discloses too much information";_tag_dyneither(_tmp120,sizeof(char),54);});Cyc_aprintf(_tmp2AE,_tag_dyneither(_tmp11F,sizeof(void*),1));});});}));});return 0;}return 1;}
# 731
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp122=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmp122 == 0)return 0;if(d0 != _tmp122){({struct _dyneither_ptr*_tmp2B2=msg;Cyc_Interface_check_err(_tmp2B2,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp125;_tmp125.tag=0;({struct _dyneither_ptr _tmp2B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp125.f1=_tmp2B0;});({void*_tmp123[1]={& _tmp125};({struct _dyneither_ptr _tmp2B1=({const char*_tmp124="declaration of datatype %s discloses too much information";_tag_dyneither(_tmp124,sizeof(char),58);});Cyc_aprintf(_tmp2B1,_tag_dyneither(_tmp123,sizeof(void*),1));});});}));});return 0;}return 1;}
# 733
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmp126=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp126 == 0)return 0;if(d0 != _tmp126){({struct _dyneither_ptr*_tmp2B5=msg;Cyc_Interface_check_err(_tmp2B5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp129;_tmp129.tag=0;({struct _dyneither_ptr _tmp2B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp129.f1=_tmp2B3;});({void*_tmp127[1]={& _tmp129};({struct _dyneither_ptr _tmp2B4=({const char*_tmp128="declaration of enum %s discloses too much information";_tag_dyneither(_tmp128,sizeof(char),54);});Cyc_aprintf(_tmp2B4,_tag_dyneither(_tmp127,sizeof(void*),1));});});}));});return 0;}return 1;}
# 735
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*_tmp12A=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp12A == 0)return 0;if(d0 != _tmp12A){({struct _dyneither_ptr*_tmp2B8=msg;Cyc_Interface_check_err(_tmp2B8,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12D;_tmp12D.tag=0;({struct _dyneither_ptr _tmp2B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp12D.f1=_tmp2B6;});({void*_tmp12B[1]={& _tmp12D};({struct _dyneither_ptr _tmp2B7=({const char*_tmp12C="declaration of variable %s discloses too much information";_tag_dyneither(_tmp12C,sizeof(char),58);});Cyc_aprintf(_tmp2B7,_tag_dyneither(_tmp12B,sizeof(void*),1));});});}));});return 0;}return 1;}
# 737
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp12E=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp12E == 0)return 0;if(d0 != _tmp12E){({struct _dyneither_ptr*_tmp2BB=msg;Cyc_Interface_check_err(_tmp2BB,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp131;_tmp131.tag=0;({struct _dyneither_ptr _tmp2B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp131.f1=_tmp2B9;});({void*_tmp12F[1]={& _tmp131};({struct _dyneither_ptr _tmp2BA=({const char*_tmp130="declaration of typedef %s discloses too much information";_tag_dyneither(_tmp130,sizeof(char),57);});Cyc_aprintf(_tmp2BA,_tag_dyneither(_tmp12F,sizeof(void*),1));});});}));});return 0;}return 1;}
# 739
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp132=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0,msg);if(_tmp132 == 0)return 0;if(d0 != _tmp132){({struct _dyneither_ptr*_tmp2BE=msg;Cyc_Interface_check_err(_tmp2BE,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp135;_tmp135.tag=0;({struct _dyneither_ptr _tmp2BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name));_tmp135.f1=_tmp2BC;});({void*_tmp133[1]={& _tmp135};({struct _dyneither_ptr _tmp2BD=({const char*_tmp134="declaration of xdatatypefield %s discloses too much information";_tag_dyneither(_tmp134,sizeof(char),64);});Cyc_aprintf(_tmp2BD,_tag_dyneither(_tmp133,sizeof(void*),1));});});}));});return 0;}return 1;}
# 744
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);
# 753
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
int _tmp136=({struct Cyc_Dict_Dict _tmp2C1=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2C0=ie2->aggrdecls;struct _dyneither_ptr _tmp2BF=({const char*_tmp141="type";_tag_dyneither(_tmp141,sizeof(char),5);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2C1,_tmp2C0,Cyc_Interface_incl_aggrdecl,_tmp2BF,msg);});
int _tmp137=({struct Cyc_Dict_Dict _tmp2C4=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2C3=ie2->datatypedecls;struct _dyneither_ptr _tmp2C2=({const char*_tmp140="datatype";_tag_dyneither(_tmp140,sizeof(char),9);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2C4,_tmp2C3,Cyc_Interface_incl_datatypedecl,_tmp2C2,msg);});
int _tmp138=({struct Cyc_Dict_Dict _tmp2C7=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2C6=ie2->enumdecls;struct _dyneither_ptr _tmp2C5=({const char*_tmp13F="enum";_tag_dyneither(_tmp13F,sizeof(char),5);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2C7,_tmp2C6,Cyc_Interface_incl_enumdecl,_tmp2C5,msg);});
# 758
int _tmp139=({({struct Cyc_Dict_Dict _tmp2CB=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2CA=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2C9=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 758
struct _dyneither_ptr _tmp2C8=({const char*_tmp13E="typedef";_tag_dyneither(_tmp13E,sizeof(char),8);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2CB,_tmp2CA,_tmp2C9,Cyc_Tcdecl_merge_typedefdecl,_tmp2C8,msg);})!= 0;});
# 761
int _tmp13A=({struct Cyc_Dict_Dict _tmp2CE=ie1->vardecls;struct Cyc_Dict_Dict _tmp2CD=ie2->vardecls;struct _dyneither_ptr _tmp2CC=({const char*_tmp13D="variable";_tag_dyneither(_tmp13D,sizeof(char),9);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2CE,_tmp2CD,Cyc_Interface_incl_vardecl,_tmp2CC,msg);});
int _tmp13B=({struct Cyc_Dict_Dict _tmp2D1=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2D0=ie2->xdatatypefielddecls;struct _dyneither_ptr _tmp2CF=({const char*_tmp13C="xdatatypefield";_tag_dyneither(_tmp13C,sizeof(char),15);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2D1,_tmp2D0,Cyc_Interface_incl_xdatatypefielddecl,_tmp2CF,msg);});
return((((_tmp136  && _tmp137) && _tmp138) && _tmp139) && _tmp13A) && _tmp13B;}
# 766
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp142;_push_handler(& _tmp142);{int _tmp144=0;if(setjmp(_tmp142.handler))_tmp144=1;if(!_tmp144){
{int _tmp145=1;
struct _dyneither_ptr*msg=0;
# 771
if(info != 0)
({struct _dyneither_ptr*_tmp2D4=({struct _dyneither_ptr*_tmp146=_cycalloc(sizeof(*_tmp146));({struct _dyneither_ptr _tmp2D3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;_tmp149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp147[2]={& _tmp149,& _tmp14A};({struct _dyneither_ptr _tmp2D2=({const char*_tmp148="checking inclusion of %s exports into %s exports,";_tag_dyneither(_tmp148,sizeof(char),50);});Cyc_aprintf(_tmp2D2,_tag_dyneither(_tmp147,sizeof(void*),2));});});});});_tmp146[0]=_tmp2D3;});_tmp146;});msg=_tmp2D4;});
if(!Cyc_Interface_incl_ienv(i1->exports,i2->exports,msg))_tmp145=0;
# 775
if(info != 0)
({struct _dyneither_ptr*_tmp2D7=({struct _dyneither_ptr*_tmp14B=_cycalloc(sizeof(*_tmp14B));({struct _dyneither_ptr _tmp2D6=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({void*_tmp14C[2]={& _tmp14E,& _tmp14F};({struct _dyneither_ptr _tmp2D5=({const char*_tmp14D="checking inclusion of %s imports into %s imports,";_tag_dyneither(_tmp14D,sizeof(char),50);});Cyc_aprintf(_tmp2D5,_tag_dyneither(_tmp14C,sizeof(void*),2));});});});});_tmp14B[0]=_tmp2D6;});_tmp14B;});msg=_tmp2D7;});
if(!Cyc_Interface_incl_ienv(i2->imports,i1->imports,msg))_tmp145=0;{
# 779
int _tmp150=_tmp145;_npop_handler(0);return _tmp150;};}
# 768
;_pop_handler();}else{void*_tmp143=(void*)_exn_thrown;void*_tmp151=_tmp143;void*_tmp152;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp151)->tag == Cyc_Tcdecl_Incompatible){_LLC3: _LLC4:
# 781
 return 0;}else{_LLC5: _tmp152=_tmp151;_LLC6:(int)_rethrow(_tmp152);}_LLC2:;}};}struct _tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr f6;struct _dyneither_ptr*f7;};
# 789
void Cyc_Interface_compat_merge_dict_f(struct _tuple26*env,struct _tuple0*x,void*y2){
# 796
struct _tuple26*_tmp153=env;int*_tmp16C;struct Cyc_Dict_Dict*_tmp16B;struct Cyc_Dict_Dict _tmp16A;struct Cyc_Dict_Dict _tmp169;void*(*_tmp168)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr _tmp167;struct _dyneither_ptr*_tmp166;_LLC8: _tmp16C=(int*)& _tmp153->f1;_tmp16B=(struct Cyc_Dict_Dict*)& _tmp153->f2;_tmp16A=_tmp153->f3;_tmp169=_tmp153->f4;_tmp168=_tmp153->f5;_tmp167=_tmp153->f6;_tmp166=_tmp153->f7;_LLC9:;{
void*y;
{struct _handler_cons _tmp154;_push_handler(& _tmp154);{int _tmp156=0;if(setjmp(_tmp154.handler))_tmp156=1;if(!_tmp156){
{void*_tmp157=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp16A,x);
# 802
void*_tmp158=_tmp168(_tmp157,y2,0,_tmp166);
if(!((unsigned int)_tmp158)){
*_tmp16C=0;
_npop_handler(0);return;}
# 807
y=_tmp158;}
# 799
;_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*_tmp159=_tmp155;void*_tmp15A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp159)->tag == Cyc_Dict_Absent){_LLCB: _LLCC:
# 810
 y=y2;
goto _LLCA;}else{_LLCD: _tmp15A=_tmp159;_LLCE:(int)_rethrow(_tmp15A);}_LLCA:;}};}{
# 814
struct _handler_cons _tmp15B;_push_handler(& _tmp15B);{int _tmp15D=0;if(setjmp(_tmp15B.handler))_tmp15D=1;if(!_tmp15D){
{void*_tmp15E=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp169,x);
# 819
void*_tmp15F=_tmp168(_tmp15E,y,0,_tmp166);
if(_tmp15F != _tmp15E){
if((unsigned int)_tmp15F)
({struct _dyneither_ptr*_tmp2DA=_tmp166;Cyc_Interface_check_err(_tmp2DA,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0;({struct _dyneither_ptr _tmp2D8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x));_tmp163.f1=_tmp2D8;});({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0;_tmp162.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp167);({void*_tmp160[2]={& _tmp162,& _tmp163};({struct _dyneither_ptr _tmp2D9=({const char*_tmp161="abstract %s %s is being imported as non-abstract";_tag_dyneither(_tmp161,sizeof(char),49);});Cyc_aprintf(_tmp2D9,_tag_dyneither(_tmp160,sizeof(void*),2));});});});}));});
*_tmp16C=0;}}
# 815
;_pop_handler();}else{void*_tmp15C=(void*)_exn_thrown;void*_tmp164=_tmp15C;void*_tmp165;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp164)->tag == Cyc_Dict_Absent){_LLD0: _LLD1:
# 828
 if(*_tmp16C)
({struct Cyc_Dict_Dict _tmp2DB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp16B,x,y);*_tmp16B=_tmp2DB;});
goto _LLCF;}else{_LLD2: _tmp165=_tmp164;_LLD3:(int)_rethrow(_tmp165);}_LLCF:;}};};};}
# 834
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 842
struct _tuple26 _tmp16D=({struct _tuple26 _tmp16F;_tmp16F.f1=1;_tmp16F.f2=dic1;_tmp16F.f3=dic1;_tmp16F.f4=excl;_tmp16F.f5=merge_f;_tmp16F.f6=t;_tmp16F.f7=msg;_tmp16F;});
# 844
((void(*)(void(*f)(struct _tuple26*,struct _tuple0*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& _tmp16D,dic2);
# 846
if(_tmp16D.f1)
return({struct Cyc_Dict_Dict*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=_tmp16D.f2;_tmp16E;});
return 0;}
# 856
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp170=({struct Cyc_Dict_Dict _tmp2DF=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2DE=ie2->aggrdecls;struct Cyc_Dict_Dict _tmp2DD=iexcl->aggrdecls;struct _dyneither_ptr _tmp2DC=({const char*_tmp17C="type";_tag_dyneither(_tmp17C,sizeof(char),5);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2DF,_tmp2DE,_tmp2DD,Cyc_Tcdecl_merge_aggrdecl,_tmp2DC,msg);});
struct Cyc_Dict_Dict*_tmp171=({struct Cyc_Dict_Dict _tmp2E3=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2E2=ie2->datatypedecls;struct Cyc_Dict_Dict _tmp2E1=iexcl->datatypedecls;struct _dyneither_ptr _tmp2E0=({const char*_tmp17B="datatype";_tag_dyneither(_tmp17B,sizeof(char),9);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2E3,_tmp2E2,_tmp2E1,Cyc_Tcdecl_merge_datatypedecl,_tmp2E0,msg);});
struct Cyc_Dict_Dict*_tmp172=({struct Cyc_Dict_Dict _tmp2E7=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2E6=ie2->enumdecls;struct Cyc_Dict_Dict _tmp2E5=iexcl->enumdecls;struct _dyneither_ptr _tmp2E4=({const char*_tmp17A="enum";_tag_dyneither(_tmp17A,sizeof(char),5);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2E7,_tmp2E6,_tmp2E5,Cyc_Tcdecl_merge_enumdecl,_tmp2E4,msg);});
struct Cyc_Dict_Dict*_tmp173=({struct Cyc_Dict_Dict _tmp2EB=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2EA=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2E9=iexcl->typedefdecls;struct _dyneither_ptr _tmp2E8=({const char*_tmp179="typedef";_tag_dyneither(_tmp179,sizeof(char),8);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2EB,_tmp2EA,_tmp2E9,Cyc_Tcdecl_merge_typedefdecl,_tmp2E8,msg);});
struct Cyc_Dict_Dict*_tmp174=({struct Cyc_Dict_Dict _tmp2EF=ie1->vardecls;struct Cyc_Dict_Dict _tmp2EE=ie2->vardecls;struct Cyc_Dict_Dict _tmp2ED=iexcl->vardecls;struct _dyneither_ptr _tmp2EC=({const char*_tmp178="variable";_tag_dyneither(_tmp178,sizeof(char),9);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2EF,_tmp2EE,_tmp2ED,Cyc_Tcdecl_merge_vardecl,_tmp2EC,msg);});
struct Cyc_Dict_Dict*_tmp175=({struct Cyc_Dict_Dict _tmp2F3=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2F2=ie2->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2F1=iexcl->xdatatypefielddecls;struct _dyneither_ptr _tmp2F0=({const char*_tmp177="xdatatypefield";_tag_dyneither(_tmp177,sizeof(char),15);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2F3,_tmp2F2,_tmp2F1,Cyc_Tcdecl_merge_xdatatypefielddecl,_tmp2F0,msg);});
if(((((!((unsigned int)_tmp170) || !((unsigned int)_tmp171)) || !((unsigned int)_tmp172)) || !((unsigned int)_tmp173)) || !((unsigned int)_tmp174)) || !((unsigned int)_tmp175))
return 0;
return({struct Cyc_Interface_Ienv*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->aggrdecls=*_tmp170;_tmp176->datatypedecls=*_tmp171;_tmp176->enumdecls=*_tmp172;_tmp176->typedefdecls=*_tmp173;_tmp176->vardecls=*_tmp174;_tmp176->xdatatypefielddecls=*_tmp175;_tmp176;});}struct _tuple27{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 869
void Cyc_Interface_disj_merge_dict_f(struct _tuple27*env,struct _tuple0*x,void*y){
# 872
struct _tuple27*_tmp17D=env;int*_tmp186;struct Cyc_Dict_Dict*_tmp185;struct Cyc_Dict_Dict _tmp184;struct _dyneither_ptr _tmp183;struct _dyneither_ptr*_tmp182;_LLD5: _tmp186=(int*)& _tmp17D->f1;_tmp185=(struct Cyc_Dict_Dict*)& _tmp17D->f2;_tmp184=_tmp17D->f3;_tmp183=_tmp17D->f4;_tmp182=_tmp17D->f5;_LLD6:;
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(_tmp184,x)){
({struct _dyneither_ptr*_tmp2F6=_tmp182;Cyc_Interface_check_err(_tmp2F6,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp181;_tmp181.tag=0;({struct _dyneither_ptr _tmp2F4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x));_tmp181.f1=_tmp2F4;});({struct Cyc_String_pa_PrintArg_struct _tmp180;_tmp180.tag=0;_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp183);({void*_tmp17E[2]={& _tmp180,& _tmp181};({struct _dyneither_ptr _tmp2F5=({const char*_tmp17F="%s %s is exported more than once";_tag_dyneither(_tmp17F,sizeof(char),33);});Cyc_aprintf(_tmp2F5,_tag_dyneither(_tmp17E,sizeof(void*),2));});});});}));});
*_tmp186=0;}else{
# 878
if(*_tmp186)({struct Cyc_Dict_Dict _tmp2F7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp185,x,y);*_tmp185=_tmp2F7;});}}
# 882
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 886
struct _tuple27 _tmp187=({struct _tuple27 _tmp189;_tmp189.f1=1;_tmp189.f2=dic1;_tmp189.f3=dic1;_tmp189.f4=t;_tmp189.f5=msg;_tmp189;});
((void(*)(void(*f)(struct _tuple27*,struct _tuple0*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp187,dic2);
if(_tmp187.f1)
return({struct Cyc_Dict_Dict*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=_tmp187.f2;_tmp188;});
return 0;}
# 893
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp18A=({struct Cyc_Dict_Dict _tmp2FB=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2FA=ie2->aggrdecls;struct Cyc_Dict_Dict _tmp2F9=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 894
struct _dyneither_ptr _tmp2F8=({const char*_tmp196="type";_tag_dyneither(_tmp196,sizeof(char),5);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2FB,_tmp2FA,_tmp2F9,Cyc_Tcdecl_merge_aggrdecl,_tmp2F8,msg);});
# 897
struct Cyc_Dict_Dict*_tmp18B=({struct Cyc_Dict_Dict _tmp2FE=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2FD=ie2->datatypedecls;struct _dyneither_ptr _tmp2FC=({const char*_tmp195="datatype";_tag_dyneither(_tmp195,sizeof(char),9);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp2FE,_tmp2FD,_tmp2FC,msg);});
struct Cyc_Dict_Dict*_tmp18C=({struct Cyc_Dict_Dict _tmp301=ie1->enumdecls;struct Cyc_Dict_Dict _tmp300=ie2->enumdecls;struct _dyneither_ptr _tmp2FF=({const char*_tmp194="enum";_tag_dyneither(_tmp194,sizeof(char),5);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp301,_tmp300,_tmp2FF,msg);});
# 900
struct Cyc_Dict_Dict*_tmp18D=({struct Cyc_Dict_Dict _tmp305=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp304=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp303=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 900
struct _dyneither_ptr _tmp302=({const char*_tmp193="typedef";_tag_dyneither(_tmp193,sizeof(char),8);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp305,_tmp304,_tmp303,Cyc_Tcdecl_merge_typedefdecl,_tmp302,msg);});
# 903
struct Cyc_Dict_Dict*_tmp18E=({struct Cyc_Dict_Dict _tmp308=ie1->vardecls;struct Cyc_Dict_Dict _tmp307=ie2->vardecls;struct _dyneither_ptr _tmp306=({const char*_tmp192="variable";_tag_dyneither(_tmp192,sizeof(char),9);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp308,_tmp307,_tmp306,msg);});
struct Cyc_Dict_Dict*_tmp18F=({struct Cyc_Dict_Dict _tmp30B=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp30A=ie2->xdatatypefielddecls;struct _dyneither_ptr _tmp309=({const char*_tmp191="xdatatypefield";_tag_dyneither(_tmp191,sizeof(char),15);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp30B,_tmp30A,_tmp309,msg);});
# 907
if(((((!((unsigned int)_tmp18A) || !((unsigned int)_tmp18B)) || !((unsigned int)_tmp18C)) || !((unsigned int)_tmp18D)) || !((unsigned int)_tmp18E)) || !((unsigned int)_tmp18F))
return 0;
return({struct Cyc_Interface_Ienv*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->aggrdecls=*_tmp18A;_tmp190->datatypedecls=*_tmp18B;_tmp190->enumdecls=*_tmp18C;_tmp190->typedefdecls=*_tmp18D;_tmp190->vardecls=*_tmp18E;_tmp190->xdatatypefielddecls=*_tmp18F;_tmp190;});}
# 913
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp197;_push_handler(& _tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){
{struct _dyneither_ptr*msg=0;
# 917
if(info != 0)
({struct _dyneither_ptr*_tmp30E=({struct _dyneither_ptr*_tmp19A=_cycalloc(sizeof(*_tmp19A));({struct _dyneither_ptr _tmp30D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp19D;_tmp19D.tag=0;_tmp19D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp19B[2]={& _tmp19D,& _tmp19E};({struct _dyneither_ptr _tmp30C=({const char*_tmp19C="merging exports of %s and %s,";_tag_dyneither(_tmp19C,sizeof(char),30);});Cyc_aprintf(_tmp30C,_tag_dyneither(_tmp19B,sizeof(void*),2));});});});});_tmp19A[0]=_tmp30D;});_tmp19A;});msg=_tmp30E;});{
# 921
struct Cyc_Interface_Ienv*_tmp19F=Cyc_Interface_disj_merge_ienv(i1->exports,i2->exports,msg);
if(_tmp19F == 0){struct Cyc_Interface_I*_tmp1A0=0;_npop_handler(0);return _tmp1A0;}
# 924
if(info != 0)
({struct _dyneither_ptr*_tmp311=({struct _dyneither_ptr*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));({struct _dyneither_ptr _tmp310=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp1A2[2]={& _tmp1A4,& _tmp1A5};({struct _dyneither_ptr _tmp30F=({const char*_tmp1A3="merging imports of %s and %s,";_tag_dyneither(_tmp1A3,sizeof(char),30);});Cyc_aprintf(_tmp30F,_tag_dyneither(_tmp1A2,sizeof(void*),2));});});});});_tmp1A1[0]=_tmp310;});_tmp1A1;});msg=_tmp311;});{
# 928
struct Cyc_Interface_Ienv*_tmp1A6=Cyc_Interface_compat_merge_ienv(i1->imports,i2->imports,_tmp19F,msg);
if(_tmp1A6 == 0){struct Cyc_Interface_I*_tmp1A7=0;_npop_handler(0);return _tmp1A7;}{
# 931
struct Cyc_List_List*newtds=0;struct Cyc_List_List*l=i2->tds;
for(0;l != 0;l=l->tl){
({struct Cyc_List_List*_tmp313=({struct Cyc_List_List*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));({struct Cyc_Absyn_Decl*_tmp312=({struct Cyc_Absyn_Decl*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=*((struct Cyc_Absyn_Decl*)l->hd);_tmp1A9;});_tmp1A8->hd=_tmp312;});_tmp1A8->tl=newtds;_tmp1A8;});newtds=_tmp313;});}
# 935
({struct Cyc_List_List*_tmp314=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,newtds,i1->tds));newtds=_tmp314;});{
struct Cyc_Interface_I*_tmp1AB=({struct Cyc_Interface_I*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->imports=_tmp1A6;_tmp1AA->exports=_tmp19F;_tmp1AA->tds=newtds;_tmp1AA;});_npop_handler(0);return _tmp1AB;};};};};}
# 915
;_pop_handler();}else{void*_tmp198=(void*)_exn_thrown;void*_tmp1AC=_tmp198;void*_tmp1AD;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp1AC)->tag == Cyc_Tcdecl_Incompatible){_LLD8: _LLD9:
# 937
 return 0;}else{_LLDA: _tmp1AD=_tmp1AC;_LLDB:(int)_rethrow(_tmp1AD);}_LLD7:;}};}
# 940
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 943
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 948
for(0;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp1AE=({struct Cyc_Interface_I*_tmp316=curr_i;struct Cyc_Interface_I*_tmp315=(struct Cyc_Interface_I*)li->hd;Cyc_Interface_merge(_tmp316,_tmp315,
curr_info != 0  && linfo != 0?({struct _tuple10*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=*curr_info;_tmp1B4->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp1B4;}): 0);});
if(_tmp1AE == 0)return 0;
curr_i=_tmp1AE;
if(curr_info != 0)
({struct _dyneither_ptr*_tmp319=linfo != 0?({struct _dyneither_ptr*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));({struct _dyneither_ptr _tmp318=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));({struct Cyc_String_pa_PrintArg_struct _tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);({void*_tmp1B0[2]={& _tmp1B2,& _tmp1B3};({struct _dyneither_ptr _tmp317=({const char*_tmp1B1="%s+%s";_tag_dyneither(_tmp1B1,sizeof(char),6);});Cyc_aprintf(_tmp317,_tag_dyneither(_tmp1B0,sizeof(void*),2));});});});});_tmp1AF[0]=_tmp318;});_tmp1AF;}): 0;curr_info=_tmp319;});
# 956
if(linfo != 0)linfo=linfo->tl;}
# 958
return curr_i;};}
# 961
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 964
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 969
for(0;la != 0;la=la->tl){
struct Cyc_Interface_I*_tmp1B5=({struct Cyc_Interface_I*_tmp31B=curr_i;struct Cyc_Interface_I*_tmp31A=get(la->hd);Cyc_Interface_merge(_tmp31B,_tmp31A,
curr_info != 0  && linfo != 0?({struct _tuple10*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->f1=*curr_info;_tmp1BB->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp1BB;}): 0);});
if(_tmp1B5 == 0)return 0;
curr_i=_tmp1B5;
if(curr_info != 0)
({struct _dyneither_ptr*_tmp31E=linfo != 0?({struct _dyneither_ptr*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));({struct _dyneither_ptr _tmp31D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));({struct Cyc_String_pa_PrintArg_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);({void*_tmp1B7[2]={& _tmp1B9,& _tmp1BA};({struct _dyneither_ptr _tmp31C=({const char*_tmp1B8="%s+%s";_tag_dyneither(_tmp1B8,sizeof(char),6);});Cyc_aprintf(_tmp31C,_tag_dyneither(_tmp1B7,sizeof(void*),2));});});});});_tmp1B6[0]=_tmp31D;});_tmp1B6;}): 0;curr_info=_tmp31E;});
# 977
if(linfo != 0)linfo=linfo->tl;}
# 979
return curr_i;};}
# 985
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){
struct Cyc_List_List*_tmp1BC=0;
for(0;tds != 0;tds=tds->tl){
struct _tuple0*qv;
struct Cyc_Absyn_Decl*_tmp1BD=(struct Cyc_Absyn_Decl*)tds->hd;
{void*_tmp1BE=_tmp1BD->r;void*_tmp1BF=_tmp1BE;struct Cyc_Absyn_Typedefdecl*_tmp1C6;struct Cyc_Absyn_Enumdecl*_tmp1C5;struct Cyc_Absyn_Datatypedecl*_tmp1C4;struct Cyc_Absyn_Aggrdecl*_tmp1C3;struct Cyc_Absyn_Fndecl*_tmp1C2;struct Cyc_Absyn_Vardecl*_tmp1C1;switch(*((int*)_tmp1BF)){case 0: _LLDD: _tmp1C1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1BF)->f1;_LLDE:
 qv=_tmp1C1->name;goto _LLDC;case 1: _LLDF: _tmp1C2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1BF)->f1;_LLE0:
 qv=_tmp1C2->name;goto _LLDC;case 5: _LLE1: _tmp1C3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1BF)->f1;_LLE2:
 qv=_tmp1C3->name;goto _LLDC;case 6: _LLE3: _tmp1C4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1BF)->f1;_LLE4:
 qv=_tmp1C4->name;goto _LLDC;case 7: _LLE5: _tmp1C5=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1BF)->f1;_LLE6:
 qv=_tmp1C5->name;goto _LLDC;case 8: _LLE7: _tmp1C6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1BF)->f1;_LLE8:
 qv=_tmp1C6->name;goto _LLDC;default: _LLE9: _LLEA:
 Cyc_Interface_err(({const char*_tmp1C0="bad decl form in Interface::add_namespace";_tag_dyneither(_tmp1C0,sizeof(char),42);}));return 0;}_LLDC:;}{
# 999
struct Cyc_List_List*vs;
{union Cyc_Absyn_Nmspace _tmp1C7=(*qv).f1;union Cyc_Absyn_Nmspace _tmp1C8=_tmp1C7;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CA;switch((_tmp1C8.C_n).tag){case 2: _LLEC: _tmp1CA=(_tmp1C8.Abs_n).val;_LLED:
 vs=_tmp1CA;goto _LLEB;case 3: _LLEE: _tmp1CB=(_tmp1C8.C_n).val;_LLEF:
 vs=_tmp1CB;goto _LLEB;default: _LLF0: _LLF1:
 Cyc_Interface_err(({const char*_tmp1C9="bad namespace in Interface::add_namespace";_tag_dyneither(_tmp1C9,sizeof(char),42);}));return 0;}_LLEB:;}
# 1005
({struct Cyc_List_List*_tmp31F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmp31F;});
({union Cyc_Absyn_Nmspace _tmp320=Cyc_Absyn_Rel_n(0);(*qv).f1=_tmp320;});
for(0;vs != 0;vs=vs->tl){
({struct Cyc_Absyn_Decl*_tmp324=({void*_tmp323=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp322=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp1CD;_tmp1CD.tag=9;_tmp1CD.f1=(struct _dyneither_ptr*)vs->hd;({struct Cyc_List_List*_tmp321=({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->hd=_tmp1BD;_tmp1CE->tl=0;_tmp1CE;});_tmp1CD.f2=_tmp321;});_tmp1CD;});_tmp1CC[0]=_tmp322;});_tmp1CC;});Cyc_Absyn_new_decl(_tmp323,_tmp1BD->loc);});_tmp1BD=_tmp324;});}
({struct Cyc_List_List*_tmp325=({struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->hd=_tmp1BD;_tmp1CF->tl=_tmp1BC;_tmp1CF;});_tmp1BC=_tmp325;});};}
# 1011
({struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1BC);_tmp1BC=_tmp326;});
return _tmp1BC;}
# 1015
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp328=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp327=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D2;_tmp1D2.tag=5;_tmp1D2.f1=d;_tmp1D2;});_tmp1D1[0]=_tmp327;});_tmp1D1;}),0);});_tmp1D0->hd=_tmp328;});_tmp1D0->tl=tds;_tmp1D0;});}
# 1019
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 1021
({struct Cyc_Absyn_Aggrdecl*_tmp329=({struct Cyc_Absyn_Aggrdecl*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=*d;_tmp1D3;});d=_tmp329;});
d->impl=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({struct Cyc_Absyn_Decl*_tmp32B=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp32A=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D6;_tmp1D6.tag=5;_tmp1D6.f1=d;_tmp1D6;});_tmp1D5[0]=_tmp32A;});_tmp1D5;}),0);});_tmp1D4->hd=_tmp32B;});_tmp1D4->tl=tds;_tmp1D4;});}
# 1027
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));({struct Cyc_Absyn_Decl*_tmp32D=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp32C=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1D9;_tmp1D9.tag=6;_tmp1D9.f1=d;_tmp1D9;});_tmp1D8[0]=_tmp32C;});_tmp1D8;}),0);});_tmp1D7->hd=_tmp32D;});_tmp1D7->tl=tds;_tmp1D7;});}static char _tmp1DA[2]="_";
# 1031
static struct _dyneither_ptr Cyc_Interface_us={_tmp1DA,_tmp1DA,_tmp1DA + 2};
static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1034
static struct _tuple20*Cyc_Interface_rewrite_datatypefield_type(struct _tuple20*x){
return({struct _tuple20*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));({struct Cyc_Absyn_Tqual _tmp32E=Cyc_Absyn_empty_tqual(0);_tmp1DB->f1=_tmp32E;});({void*_tmp332=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp331=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1DD;_tmp1DD.tag=2;({struct Cyc_Absyn_Tvar*_tmp330=({struct Cyc_Absyn_Tvar*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->name=Cyc_Interface_us_p;_tmp1DE->identity=- 1;({void*_tmp32F=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp1DE->kind=_tmp32F;});_tmp1DE;});_tmp1DD.f1=_tmp330;});_tmp1DD;});_tmp1DC[0]=_tmp331;});_tmp1DC;});_tmp1DB->f2=_tmp332;});_tmp1DB;});}
# 1039
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
({struct Cyc_Absyn_Datatypefield*_tmp333=({struct Cyc_Absyn_Datatypefield*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=*f;_tmp1DF;});f=_tmp333;});
({struct Cyc_List_List*_tmp334=((struct Cyc_List_List*(*)(struct _tuple20*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);f->typs=_tmp334;});
return f;}
# 1045
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1047
({struct Cyc_Absyn_Datatypedecl*_tmp335=({struct Cyc_Absyn_Datatypedecl*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=*d;_tmp1E0;});d=_tmp335;});
# 1049
if(d->fields != 0)({struct Cyc_Core_Opt*_tmp337=({struct Cyc_Core_Opt*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));({struct Cyc_List_List*_tmp336=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v);_tmp1E1->v=_tmp336;});_tmp1E1;});d->fields=_tmp337;});
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));({struct Cyc_Absyn_Decl*_tmp339=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp338=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1E4;_tmp1E4.tag=6;_tmp1E4.f1=d;_tmp1E4;});_tmp1E3[0]=_tmp338;});_tmp1E3;}),0);});_tmp1E2->hd=_tmp339;});_tmp1E2->tl=tds;_tmp1E2;});}
# 1054
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));({struct Cyc_Absyn_Decl*_tmp33B=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp33A=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1E7;_tmp1E7.tag=7;_tmp1E7.f1=d;_tmp1E7;});_tmp1E6[0]=_tmp33A;});_tmp1E6;}),0);});_tmp1E5->hd=_tmp33B;});_tmp1E5->tl=tds;_tmp1E5;});}
# 1058
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1060
({struct Cyc_Absyn_Enumdecl*_tmp33C=({struct Cyc_Absyn_Enumdecl*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=*d;_tmp1E8;});d=_tmp33C;});
d->fields=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));({struct Cyc_Absyn_Decl*_tmp33E=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp33D=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1EB;_tmp1EB.tag=7;_tmp1EB.f1=d;_tmp1EB;});_tmp1EA[0]=_tmp33D;});_tmp1EA;}),0);});_tmp1E9->hd=_tmp33E;});_tmp1E9->tl=tds;_tmp1E9;});}
# 1066
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1068
return({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));({struct Cyc_Absyn_Decl*_tmp340=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp33F=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp1EE;_tmp1EE.tag=8;_tmp1EE.f1=d;_tmp1EE;});_tmp1ED[0]=_tmp33F;});_tmp1ED;}),0);});_tmp1EC->hd=_tmp340;});_tmp1EC->tl=tds;_tmp1EC;});}
# 1071
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));({struct Cyc_Absyn_Decl*_tmp342=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp341=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=d;_tmp1F1;});_tmp1F0[0]=_tmp341;});_tmp1F0;}),0);});_tmp1EF->hd=_tmp342;});_tmp1EF->tl=tds;_tmp1EF;});}
# 1075
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1077
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1F2=d;struct Cyc_Absyn_Datatypedecl*_tmp1FA;struct Cyc_Absyn_Datatypefield*_tmp1F9;_LLF3: _tmp1FA=_tmp1F2->base;_tmp1F9=_tmp1F2->field;_LLF4:;
({struct Cyc_Absyn_Datatypedecl*_tmp343=({struct Cyc_Absyn_Datatypedecl*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3[0]=*_tmp1FA;_tmp1F3;});_tmp1FA=_tmp343;});
({struct Cyc_Core_Opt*_tmp345=({struct Cyc_Core_Opt*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));({struct Cyc_List_List*_tmp344=({struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=_tmp1F9;_tmp1F5->tl=0;_tmp1F5;});_tmp1F4->v=_tmp344;});_tmp1F4;});_tmp1FA->fields=_tmp345;});
_tmp1FA->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));({struct Cyc_Absyn_Decl*_tmp347=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp346=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1F8;_tmp1F8.tag=6;_tmp1F8.f1=_tmp1FA;_tmp1F8;});_tmp1F7[0]=_tmp346;});_tmp1F7;}),0);});_tmp1F6->hd=_tmp347;});_tmp1F6->tl=tds;_tmp1F6;});}
# 1084
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1086
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1FB=d;struct Cyc_Absyn_Datatypedecl*_tmp203;struct Cyc_Absyn_Datatypefield*_tmp202;_LLF6: _tmp203=_tmp1FB->base;_tmp202=_tmp1FB->field;_LLF7:;
({struct Cyc_Absyn_Datatypedecl*_tmp348=({struct Cyc_Absyn_Datatypedecl*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=*_tmp203;_tmp1FC;});_tmp203=_tmp348;});
({struct Cyc_Absyn_Datatypefield*_tmp349=Cyc_Interface_rewrite_datatypefield(_tmp202);_tmp202=_tmp349;});
_tmp202->sc=Cyc_Absyn_Extern;
({struct Cyc_Core_Opt*_tmp34B=({struct Cyc_Core_Opt*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({struct Cyc_List_List*_tmp34A=({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=_tmp202;_tmp1FE->tl=0;_tmp1FE;});_tmp1FD->v=_tmp34A;});_tmp1FD;});_tmp203->fields=_tmp34B;});
_tmp203->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));({struct Cyc_Absyn_Decl*_tmp34D=({Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp200=_cycalloc(sizeof(*_tmp200));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp34C=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp201;_tmp201.tag=6;_tmp201.f1=_tmp203;_tmp201;});_tmp200[0]=_tmp34C;});_tmp200;}),0);});_tmp1FF->hd=_tmp34D;});_tmp1FF->tl=tds;_tmp1FF;});}
# 1095
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp204=0;
({struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp204);_tmp204=_tmp34E;});
({struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp204);_tmp204=_tmp34F;});
({struct Cyc_List_List*_tmp350=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp204);_tmp204=_tmp350;});
# 1101
if(_tmp204 != 0){
({struct Cyc_List_List*_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp204);_tmp204=_tmp351;});
({struct Cyc_List_List*_tmp352=Cyc_Interface_add_namespace(_tmp204);_tmp204=_tmp352;});
Cyc_Absynpp_decllist2file(_tmp204,f);}}
# 1108
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp205=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(_tmp205 != 0){
({struct Cyc_List_List*_tmp353=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp205);_tmp205=_tmp353;});
({struct Cyc_List_List*_tmp354=Cyc_Interface_add_namespace(_tmp205);_tmp205=_tmp354;});
Cyc_Absynpp_decllist2file(_tmp205,f);}}
# 1117
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(_tmp206 != 0){
({struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp206);_tmp206=_tmp355;});
({struct Cyc_List_List*_tmp356=Cyc_Interface_add_namespace(_tmp206);_tmp206=_tmp356;});
Cyc_Absynpp_decllist2file(_tmp206,f);}}
# 1126
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp207=0;
({struct Cyc_List_List*_tmp357=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp207);_tmp207=_tmp357;});
({struct Cyc_List_List*_tmp358=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp207);_tmp207=_tmp358;});
({struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp207);_tmp207=_tmp359;});
({struct Cyc_List_List*_tmp35A=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,_tmp207);_tmp207=_tmp35A;});
({struct Cyc_List_List*_tmp35B=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp207);_tmp207=_tmp35B;});
# 1134
if(_tmp207 != 0){
({struct Cyc_List_List*_tmp35C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp207);_tmp207=_tmp35C;});
({struct Cyc_List_List*_tmp35D=Cyc_Interface_add_namespace(_tmp207);_tmp207=_tmp35D;});
Cyc_Absynpp_decllist2file(_tmp207,f);}}
# 1144
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
({void*_tmp208=0;({struct Cyc___cycFILE*_tmp35F=f;struct _dyneither_ptr _tmp35E=({const char*_tmp209="/****** needed (headers) ******/\n";_tag_dyneither(_tmp209,sizeof(char),34);});Cyc_fprintf(_tmp35F,_tmp35E,_tag_dyneither(_tmp208,sizeof(void*),0));});});
Cyc_Interface_print_ns_headers(f,i->imports);
# 1149
({void*_tmp20A=0;({struct Cyc___cycFILE*_tmp361=f;struct _dyneither_ptr _tmp360=({const char*_tmp20B="\n/****** provided (headers) ******/\n";_tag_dyneither(_tmp20B,sizeof(char),37);});Cyc_fprintf(_tmp361,_tmp360,_tag_dyneither(_tmp20A,sizeof(void*),0));});});
Cyc_Interface_print_ns_headers(f,i->exports);
# 1154
({void*_tmp20C=0;({struct Cyc___cycFILE*_tmp363=f;struct _dyneither_ptr _tmp362=({const char*_tmp20D="\n/****** needed (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp20D,sizeof(char),58);});Cyc_fprintf(_tmp363,_tmp362,_tag_dyneither(_tmp20C,sizeof(void*),0));});});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->imports);
# 1157
({void*_tmp20E=0;({struct Cyc___cycFILE*_tmp365=f;struct _dyneither_ptr _tmp364=({const char*_tmp20F="\n/****** provided (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp20F,sizeof(char),60);});Cyc_fprintf(_tmp365,_tmp364,_tag_dyneither(_tmp20E,sizeof(void*),0));});});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->exports);
# 1161
({void*_tmp210=0;({struct Cyc___cycFILE*_tmp367=f;struct _dyneither_ptr _tmp366=({const char*_tmp211="\n/****** provided (typedefs) ******/\n";_tag_dyneither(_tmp211,sizeof(char),38);});Cyc_fprintf(_tmp367,_tmp366,_tag_dyneither(_tmp210,sizeof(void*),0));});});
Cyc_Interface_print_ns_typedefs(f,i->exports);
# 1164
({void*_tmp212=0;({struct Cyc___cycFILE*_tmp369=f;struct _dyneither_ptr _tmp368=({const char*_tmp213="\n/****** needed (declarations) ******/\n";_tag_dyneither(_tmp213,sizeof(char),40);});Cyc_fprintf(_tmp369,_tmp368,_tag_dyneither(_tmp212,sizeof(void*),0));});});
Cyc_Interface_print_ns_decls(f,i->imports);
# 1167
({void*_tmp214=0;({struct Cyc___cycFILE*_tmp36B=f;struct _dyneither_ptr _tmp36A=({const char*_tmp215="\n/****** provided (declarations) ******/\n";_tag_dyneither(_tmp215,sizeof(char),42);});Cyc_fprintf(_tmp36B,_tmp36A,_tag_dyneither(_tmp214,sizeof(void*),0));});});
Cyc_Interface_print_ns_decls(f,i->exports);}
# 1174
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp216=Cyc_Parse_parse_file(f);
Cyc_Binding_resolve_all(_tmp216);{
struct Cyc_Tcenv_Tenv*_tmp217=Cyc_Tcenv_tc_init();
Cyc_Tc_tc(_tmp217,0,_tmp216);
return Cyc_Interface_gen_extract(_tmp217->ae,0,_tmp216);};};}
# 1187
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1193
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}

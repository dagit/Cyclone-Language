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
typedef void*Cyc_sarg_t;
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 233
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_ukey();
# 241
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion*k);
# 251
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));
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
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 127
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 73
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 108 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_Parse_Exit[5];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 33 "parse.h"
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
typedef struct Cyc_Yyltype Cyc_yyltype;
# 83
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Lex_Trie;
# 44 "lex.cyl"
typedef struct Cyc_Lex_Trie*Cyc_Lex_trie_t;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple19 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 50
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){return({union Cyc_Lex_TrieChildren _tmp0;(_tmp0.Zero).val=0;(_tmp0.Zero).tag=1;_tmp0;});}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
return({union Cyc_Lex_TrieChildren _tmp1;(_tmp1.One).val=({struct _tuple19 _tmp2;_tmp2.f1=i;_tmp2.f2=t;_tmp2;});(_tmp1.One).tag=2;_tmp1;});}
# 54
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
return({union Cyc_Lex_TrieChildren _tmp3;(_tmp3.Many).val=ts;(_tmp3.Many).tag=3;_tmp3;});}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};
# 66
typedef struct Cyc_Lex_DynTrie*Cyc_Lex_dyn_trie_t;struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 73
typedef struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_dyn_trie_symbols_t;
# 78
static int Cyc_Lex_num_kws=0;struct Cyc_Lex_KeyWordInfo{int token_index;int is_c_keyword;};
# 80
static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};
# 82
int Cyc_Lex_compile_for_boot_r=0;
int Cyc_Lex_expand_specials=0;
# 85
static int Cyc_Lex_in_extern_c=0;
static struct Cyc_List_List*Cyc_Lex_prev_extern_c=0;
void Cyc_Lex_enter_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp4=x;struct Cyc_List_List*_tmp5=Cyc_Lex_prev_extern_c;x=_tmp5;Cyc_Lex_prev_extern_c=_tmp4;});
Cyc_Lex_prev_extern_c=({struct Cyc_List_List*_tmp6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6));_tmp6->hd=(void*)Cyc_Lex_in_extern_c;_tmp6->tl=x;_tmp6;});
Cyc_Lex_in_extern_c=1;}
# 93
void Cyc_Lex_leave_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp7=x;struct Cyc_List_List*_tmp8=Cyc_Lex_prev_extern_c;x=_tmp8;Cyc_Lex_prev_extern_c=_tmp7;});
Cyc_Lex_in_extern_c=(int)((struct Cyc_List_List*)_check_null(x))->hd;
Cyc_Lex_prev_extern_c=((struct Cyc_List_List*)_check_null(x))->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);}
# 101
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 104
static int Cyc_Lex_comment_depth=0;
# 107
static union Cyc_Absyn_Cnst Cyc_Lex_token_int={.Int_c={5,{Cyc_Absyn_Signed,0}}};static char _tmp9[8]="*bogus*";
static struct _dyneither_ptr Cyc_Lex_bogus_string={_tmp9,_tmp9,_tmp9 + 8};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 112
static char Cyc_Lex_token_char='\000';static char _tmpA[1]="";
static struct _dyneither_ptr Cyc_Lex_token_string={_tmpA,_tmpA,_tmpA + 1};
static struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmpB[1]="";
static struct _tuple12 Cyc_Lex_token_asm={0,{_tmpB,_tmpB,_tmpB + 1}};
# 117
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 120
static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lexing_lexeme_start(lb);
({void*_tmpC=0;Cyc_Warn_err(s,msg,_tag_dyneither(_tmpC,sizeof(void*),0));});}
# 124
static void Cyc_Lex_runaway_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lex_runaway_start;
({void*_tmpD=0;Cyc_Warn_err(s,msg,_tag_dyneither(_tmpD,sizeof(void*),0));});}
# 128
static char Cyc_Lex_cnst2char(union Cyc_Absyn_Cnst cnst,struct Cyc_Lexing_lexbuf*lb){
union Cyc_Absyn_Cnst _tmpE=cnst;char _tmp12;long long _tmp11;int _tmp10;switch((_tmpE.Char_c).tag){case 5: _LL1: _tmp10=((_tmpE.Int_c).val).f2;_LL2:
 return(char)_tmp10;case 6: _LL3: _tmp11=((_tmpE.LongLong_c).val).f2;_LL4:
 return(char)_tmp11;case 2: _LL5: _tmp12=((_tmpE.Char_c).val).f2;_LL6:
 return _tmp12;default: _LL7: _LL8:
 Cyc_Lex_err(({const char*_tmpF="couldn't convert constant to char!";_tag_dyneither(_tmpF,sizeof(char),35);}),lb);return'\000';}_LL0:;}struct _tuple20{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp13[14]="__attribute__";static char _tmp14[9]="abstract";static char _tmp15[5]="auto";static char _tmp16[6]="break";static char _tmp17[18]="__builtin_va_list";static char _tmp18[7]="calloc";static char _tmp19[5]="case";static char _tmp1A[6]="catch";static char _tmp1B[5]="char";static char _tmp1C[6]="const";static char _tmp1D[10]="__const__";static char _tmp1E[9]="continue";static char _tmp1F[9]="datatype";static char _tmp20[8]="default";static char _tmp21[3]="do";static char _tmp22[7]="double";static char _tmp23[12]="dynregion_t";static char _tmp24[5]="else";static char _tmp25[5]="enum";static char _tmp26[7]="export";static char _tmp27[7]="extern";static char _tmp28[9]="fallthru";static char _tmp29[6]="float";static char _tmp2A[4]="for";static char _tmp2B[5]="goto";static char _tmp2C[3]="if";static char _tmp2D[7]="inline";static char _tmp2E[11]="__inline__";static char _tmp2F[4]="int";static char _tmp30[4]="let";static char _tmp31[5]="long";static char _tmp32[7]="malloc";static char _tmp33[10]="namespace";static char _tmp34[4]="new";static char _tmp35[5]="NULL";static char _tmp36[8]="numelts";static char _tmp37[9]="offsetof";static char _tmp38[20]="__cyclone_port_on__";static char _tmp39[21]="__cyclone_port_off__";static char _tmp3A[8]="rcalloc";static char _tmp3B[9]="region_t";static char _tmp3C[7]="region";static char _tmp3D[8]="regions";static char _tmp3E[9]="register";static char _tmp3F[13]="reset_region";static char _tmp40[9]="restrict";static char _tmp41[7]="return";static char _tmp42[8]="rmalloc";static char _tmp43[15]="rmalloc_inline";static char _tmp44[5]="rnew";static char _tmp45[6]="short";static char _tmp46[7]="signed";static char _tmp47[11]="__signed__";static char _tmp48[7]="sizeof";static char _tmp49[7]="static";static char _tmp4A[7]="struct";static char _tmp4B[7]="switch";static char _tmp4C[9]="tagcheck";static char _tmp4D[6]="tag_t";static char _tmp4E[6]="throw";static char _tmp4F[4]="try";static char _tmp50[8]="typedef";static char _tmp51[7]="typeof";static char _tmp52[11]="__typeof__";static char _tmp53[6]="union";static char _tmp54[9]="unsigned";static char _tmp55[13]="__unsigned__";static char _tmp56[6]="using";static char _tmp57[8]="valueof";static char _tmp58[10]="valueof_t";static char _tmp59[5]="void";static char _tmp5A[9]="volatile";static char _tmp5B[13]="__volatile__";static char _tmp5C[6]="while";
# 139
static struct _tuple20 Cyc_Lex_rw_array[74]={{{_tmp13,_tmp13,_tmp13 + 14},374,1},{{_tmp14,_tmp14,_tmp14 + 9},302,0},{{_tmp15,_tmp15,_tmp15 + 5},258,1},{{_tmp16,_tmp16,_tmp16 + 6},290,1},{{_tmp17,_tmp17,_tmp17 + 18},294,1},{{_tmp18,_tmp18,_tmp18 + 7},310,0},{{_tmp19,_tmp19,_tmp19 + 5},277,1},{{_tmp1A,_tmp1A,_tmp1A + 6},299,1},{{_tmp1B,_tmp1B,_tmp1B + 5},264,1},{{_tmp1C,_tmp1C,_tmp1C + 6},272,1},{{_tmp1D,_tmp1D,_tmp1D + 10},272,1},{{_tmp1E,_tmp1E,_tmp1E + 9},289,1},{{_tmp1F,_tmp1F,_tmp1F + 9},306,0},{{_tmp20,_tmp20,_tmp20 + 8},278,1},{{_tmp21,_tmp21,_tmp21 + 3},286,1},{{_tmp22,_tmp22,_tmp22 + 7},269,1},{{_tmp23,_tmp23,_tmp23 + 12},321,0},{{_tmp24,_tmp24,_tmp24 + 5},283,1},{{_tmp25,_tmp25,_tmp25 + 5},292,1},{{_tmp26,_tmp26,_tmp26 + 7},300,0},{{_tmp27,_tmp27,_tmp27 + 7},261,1},{{_tmp28,_tmp28,_tmp28 + 9},303,0},{{_tmp29,_tmp29,_tmp29 + 6},268,1},{{_tmp2A,_tmp2A,_tmp2A + 4},287,1},{{_tmp2B,_tmp2B,_tmp2B + 5},288,1},{{_tmp2C,_tmp2C,_tmp2C + 3},282,1},{{_tmp2D,_tmp2D,_tmp2D + 7},279,1},{{_tmp2E,_tmp2E,_tmp2E + 11},279,1},{{_tmp2F,_tmp2F,_tmp2F + 4},266,1},{{_tmp30,_tmp30,_tmp30 + 4},296,0},{{_tmp31,_tmp31,_tmp31 + 5},267,1},{{_tmp32,_tmp32,_tmp32 + 7},307,0},{{_tmp33,_tmp33,_tmp33 + 10},305,0},{{_tmp34,_tmp34,_tmp34 + 4},301,0},{{_tmp35,_tmp35,_tmp35 + 5},295,0},{{_tmp36,_tmp36,_tmp36 + 8},322,0},{{_tmp37,_tmp37,_tmp37 + 9},281,1},{{_tmp38,_tmp38,_tmp38 + 20},319,0},{{_tmp39,_tmp39,_tmp39 + 21},320,0},{{_tmp3A,_tmp3A,_tmp3A + 8},311,0},{{_tmp3B,_tmp3B,_tmp3B + 9},313,0},{{_tmp3C,_tmp3C,_tmp3C + 7},315,0},{{_tmp3D,_tmp3D,_tmp3D + 8},317,0},{{_tmp3E,_tmp3E,_tmp3E + 9},259,1},{{_tmp3F,_tmp3F,_tmp3F + 13},318,0},{{_tmp40,_tmp40,_tmp40 + 9},274,1},{{_tmp41,_tmp41,_tmp41 + 7},291,1},{{_tmp42,_tmp42,_tmp42 + 8},308,0},{{_tmp43,_tmp43,_tmp43 + 15},309,0},{{_tmp44,_tmp44,_tmp44 + 5},316,0},{{_tmp45,_tmp45,_tmp45 + 6},265,1},{{_tmp46,_tmp46,_tmp46 + 7},270,1},{{_tmp47,_tmp47,_tmp47 + 11},270,1},{{_tmp48,_tmp48,_tmp48 + 7},280,1},{{_tmp49,_tmp49,_tmp49 + 7},260,1},{{_tmp4A,_tmp4A,_tmp4A + 7},275,1},{{_tmp4B,_tmp4B,_tmp4B + 7},284,1},{{_tmp4C,_tmp4C,_tmp4C + 9},325,0},{{_tmp4D,_tmp4D,_tmp4D + 6},314,0},{{_tmp4E,_tmp4E,_tmp4E + 6},297,0},{{_tmp4F,_tmp4F,_tmp4F + 4},298,0},{{_tmp50,_tmp50,_tmp50 + 8},262,1},{{_tmp51,_tmp51,_tmp51 + 7},293,1},{{_tmp52,_tmp52,_tmp52 + 11},293,1},{{_tmp53,_tmp53,_tmp53 + 6},276,1},{{_tmp54,_tmp54,_tmp54 + 9},271,1},{{_tmp55,_tmp55,_tmp55 + 13},271,1},{{_tmp56,_tmp56,_tmp56 + 6},304,0},{{_tmp57,_tmp57,_tmp57 + 8},323,0},{{_tmp58,_tmp58,_tmp58 + 10},324,0},{{_tmp59,_tmp59,_tmp59 + 5},263,1},{{_tmp5A,_tmp5A,_tmp5A + 9},273,1},{{_tmp5B,_tmp5B,_tmp5B + 13},273,1},{{_tmp5C,_tmp5C,_tmp5C + 6},285,1}};
# 217
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned int i=0;for(0;i < 74;++ i){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 226
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
return({struct Cyc_Lex_Trie*_tmp5D=_region_malloc(d,sizeof(*_tmp5D));_tmp5D->children=Cyc_Lex_Zero();_tmp5D->shared_str=0;_tmp5D;});}
# 230
static int Cyc_Lex_trie_char(int c){
# 232
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}
# 237
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){
# 239
int i=offset;
int last=(offset + len)- 1;
while(i <= last){
union Cyc_Lex_TrieChildren _tmp5E=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _tmp5F=_tmp5E;int _tmp6B;struct Cyc_Lex_Trie*_tmp6A;struct Cyc_Lex_Trie**_tmp69;switch((_tmp5F.One).tag){case 3: _LLA: _tmp69=(_tmp5F.Many).val;_LLB: {
# 245
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
if(_tmp69[_check_known_subscript_notnull(64,ch)]== 0)
_tmp69[_check_known_subscript_notnull(64,ch)]=({struct Cyc_Lex_Trie*_tmp60=_region_malloc(r,sizeof(*_tmp60));_tmp60->children=Cyc_Lex_Zero();_tmp60->shared_str=0;_tmp60;});
t=_tmp69[_check_known_subscript_notnull(64,ch)];
++ i;
goto _LL9;}case 2: _LLC: _tmp6B=((_tmp5F.One).val).f1;_tmp6A=((_tmp5F.One).val).f2;_LLD:
# 253
 if(_tmp6B == *((const char*)_check_dyneither_subscript(buff,sizeof(char),i)))
t=_tmp6A;else{
# 256
struct Cyc_Lex_Trie**_tmp61=({unsigned int _tmp64=64;struct Cyc_Lex_Trie**_tmp65=(struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp64));{unsigned int _tmp66=_tmp64;unsigned int j;for(j=0;j < _tmp66;j ++){_tmp65[j]=(struct Cyc_Lex_Trie*)0;}}_tmp65;});
_tmp61[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp6B))]=_tmp6A;{
int _tmp62=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
_tmp61[_tmp62]=({struct Cyc_Lex_Trie*_tmp63=_region_malloc(r,sizeof(*_tmp63));_tmp63->children=Cyc_Lex_Zero();_tmp63->shared_str=0;_tmp63;});
t->children=Cyc_Lex_Many(_tmp61);
t=_tmp61[_tmp62];};}
# 263
++ i;
goto _LL9;default: _LLE: _LLF:
# 267
 while(i <= last){
struct Cyc_Lex_Trie*_tmp67=({struct Cyc_Lex_Trie*_tmp68=_region_malloc(r,sizeof(*_tmp68));_tmp68->children=Cyc_Lex_Zero();_tmp68->shared_str=0;_tmp68;});
t->children=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),_tmp67);
t=_tmp67;}
# 272
return t;}_LL9:;}
# 275
return t;}struct _tuple21{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;struct _dyneither_ptr f3;int f4;int f5;};
# 278
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple21*env){
# 281
struct _tuple21 _tmp6C=*env;struct _tuple21 _tmp6D=_tmp6C;struct Cyc_Lex_Trie*_tmp75;struct Cyc_Xarray_Xarray*_tmp74;struct _dyneither_ptr _tmp73;int _tmp72;int _tmp71;_LL11: _tmp75=_tmp6D.f1;_tmp74=_tmp6D.f2;_tmp73=_tmp6D.f3;_tmp72=_tmp6D.f4;_tmp71=_tmp6D.f5;_LL12:;{
struct Cyc_Lex_Trie*_tmp6E=Cyc_Lex_trie_lookup(d,_tmp75,_tmp73,_tmp72,_tmp71);
# 284
if(_tmp6E->shared_str == 0){
struct _dyneither_ptr _tmp6F=Cyc_Core_new_string((unsigned int)(_tmp71 + 1));
Cyc_zstrncpy(_dyneither_ptr_decrease_size(_tmp6F,sizeof(char),1),(struct _dyneither_ptr)_dyneither_ptr_plus(_tmp73,sizeof(char),_tmp72),(unsigned long)_tmp71);{
int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp74,({struct _dyneither_ptr*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=(struct _dyneither_ptr)_tmp6F;_tmp70;}));
_tmp6E->shared_str=ans;};}
# 290
return _tmp6E->shared_str;};}
# 293
static int Cyc_Lex_str_index(struct _dyneither_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp76=idt;struct Cyc_Lex_DynTrieSymbols*_tmp77=Cyc_Lex_ids_trie;idt=_tmp77;Cyc_Lex_ids_trie=_tmp76;});{
struct Cyc_Lex_DynTrieSymbols _tmp78=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp79=_tmp78;struct Cyc_Core_DynamicRegion*_tmp82;struct Cyc_Lex_Trie*_tmp81;struct Cyc_Xarray_Xarray*_tmp80;_LL14: _tmp82=_tmp79.dyn;_tmp81=_tmp79.t;_tmp80=_tmp79.symbols;_LL15:;{
struct _tuple21 _tmp7A=({struct _tuple21 _tmp7F;_tmp7F.f1=_tmp81;_tmp7F.f2=_tmp80;_tmp7F.f3=buff;_tmp7F.f4=offset;_tmp7F.f5=len;_tmp7F;});
int _tmp7B=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple21*arg,int(*body)(struct _RegionHandle*h,struct _tuple21*arg)))Cyc_Core_open_region)(_tmp82,& _tmp7A,Cyc_Lex_str_index_body);
*idt=({struct Cyc_Lex_DynTrieSymbols _tmp7C;_tmp7C.dyn=(struct Cyc_Core_DynamicRegion*)_tmp82;_tmp7C.t=(struct Cyc_Lex_Trie*)_tmp81;_tmp7C.symbols=(struct Cyc_Xarray_Xarray*)_tmp80;_tmp7C;});
({struct Cyc_Lex_DynTrieSymbols*_tmp7D=idt;struct Cyc_Lex_DynTrieSymbols*_tmp7E=Cyc_Lex_ids_trie;idt=_tmp7E;Cyc_Lex_ids_trie=_tmp7D;});
return _tmp7B;};};}
# 304
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple22{struct Cyc_Lex_Trie*f1;struct _dyneither_ptr f2;};
# 310
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple22*arg){
# 312
struct _tuple22 _tmp83=*arg;struct _tuple22 _tmp84=_tmp83;struct Cyc_Lex_Trie*_tmp87;struct _dyneither_ptr _tmp86;_LL17: _tmp87=_tmp84.f1;_tmp86=_tmp84.f2;_LL18:;{
struct Cyc_Lex_Trie*_tmp85=Cyc_Lex_trie_lookup(h,_tmp87,_tmp86,0,(int)(_get_dyneither_size(_tmp86,sizeof(char))- 1));
((struct Cyc_Lex_Trie*)_check_null(_tmp85))->shared_str=1;
return 0;};}
# 319
static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp88=*sptr;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp89=tdefs;struct Cyc_Lex_DynTrie*_tmp8A=Cyc_Lex_typedefs_trie;tdefs=_tmp8A;Cyc_Lex_typedefs_trie=_tmp89;});{
struct Cyc_Lex_DynTrie _tmp8B=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmp8C=_tmp8B;struct Cyc_Core_DynamicRegion*_tmp93;struct Cyc_Lex_Trie*_tmp92;_LL1A: _tmp93=_tmp8C.dyn;_tmp92=_tmp8C.t;_LL1B:;{
struct _tuple22 _tmp8D=({struct _tuple22 _tmp91;_tmp91.f1=_tmp92;_tmp91.f2=_tmp88;_tmp91;});
((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple22*arg,int(*body)(struct _RegionHandle*h,struct _tuple22*arg)))Cyc_Core_open_region)(_tmp93,& _tmp8D,Cyc_Lex_insert_typedef_body);
*tdefs=({struct Cyc_Lex_DynTrie _tmp8E;_tmp8E.dyn=(struct Cyc_Core_DynamicRegion*)_tmp93;_tmp8E.t=(struct Cyc_Lex_Trie*)_tmp92;_tmp8E;});
({struct Cyc_Lex_DynTrie*_tmp8F=tdefs;struct Cyc_Lex_DynTrie*_tmp90=Cyc_Lex_typedefs_trie;tdefs=_tmp90;Cyc_Lex_typedefs_trie=_tmp8F;});
return;};};}struct _tuple23{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;int f3;};
# 331
static struct _dyneither_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple23*env){
struct _tuple23 _tmp94=*env;struct _tuple23 _tmp95=_tmp94;struct Cyc_Lex_Trie*_tmp98;struct Cyc_Xarray_Xarray*_tmp97;int _tmp96;_LL1D: _tmp98=_tmp95.f1;_tmp97=_tmp95.f2;_tmp96=_tmp95.f3;_LL1E:;
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp97,_tmp96);}
# 336
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp99=idt;struct Cyc_Lex_DynTrieSymbols*_tmp9A=Cyc_Lex_ids_trie;idt=_tmp9A;Cyc_Lex_ids_trie=_tmp99;});{
struct Cyc_Lex_DynTrieSymbols _tmp9B=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp9C=_tmp9B;struct Cyc_Core_DynamicRegion*_tmpA5;struct Cyc_Lex_Trie*_tmpA4;struct Cyc_Xarray_Xarray*_tmpA3;_LL20: _tmpA5=_tmp9C.dyn;_tmpA4=_tmp9C.t;_tmpA3=_tmp9C.symbols;_LL21:;{
struct _tuple23 _tmp9D=({struct _tuple23 _tmpA2;_tmpA2.f1=_tmpA4;_tmpA2.f2=_tmpA3;_tmpA2.f3=symbol_num;_tmpA2;});
struct _dyneither_ptr*_tmp9E=((struct _dyneither_ptr*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple23*arg,struct _dyneither_ptr*(*body)(struct _RegionHandle*h,struct _tuple23*arg)))Cyc_Core_open_region)(_tmpA5,& _tmp9D,Cyc_Lex_get_symbol_body);
*idt=({struct Cyc_Lex_DynTrieSymbols _tmp9F;_tmp9F.dyn=(struct Cyc_Core_DynamicRegion*)_tmpA5;_tmp9F.t=(struct Cyc_Lex_Trie*)_tmpA4;_tmp9F.symbols=(struct Cyc_Xarray_Xarray*)_tmpA3;_tmp9F;});
({struct Cyc_Lex_DynTrieSymbols*_tmpA0=idt;struct Cyc_Lex_DynTrieSymbols*_tmpA1=Cyc_Lex_ids_trie;idt=_tmpA1;Cyc_Lex_ids_trie=_tmpA0;});
return _tmp9E;};};}
# 355 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if('0' <= c  && c <= '9')return c - '0';else{
if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{
if('A' <= c  && c <= 'F')return(10 + c)- 'A';else{
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmpA7;_tmpA7.tag=Cyc_Core_Invalid_argument;_tmpA7.f1=({const char*_tmpA8="string to integer conversion";_tag_dyneither(_tmpA8,sizeof(char),29);});_tmpA7;});_tmpA6;}));}}}}
# 363
static union Cyc_Absyn_Cnst Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
enum Cyc_Absyn_Sign sn=Cyc_Absyn_None;
start +=lbuf->lex_start_pos;{
struct _dyneither_ptr buff=_dyneither_ptr_plus(lbuf->lex_buffer,sizeof(char),start);
int end2=lbuf->lex_curr_pos - end;
int len=end2 - start;
enum Cyc_Absyn_Size_of size=Cyc_Absyn_Int_sz;
int declared_size=0;
union Cyc_Absyn_Cnst res;
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'l'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'L')){
# 374
len -=1;
declared_size=1;
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'l'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'L')){
len -=1;
size=Cyc_Absyn_LongLong_sz;}}
# 381
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'u'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'U')){
len -=1;
sn=Cyc_Absyn_Unsigned;}
# 385
if(sn == Cyc_Absyn_Unsigned){
unsigned long long n=(unsigned long long)0;
{int i=0;for(0;i < len;++ i){
n=n * base + (unsigned long long)Cyc_Lex_int_of_char(*((char*)_check_dyneither_subscript(buff,sizeof(char),i)));}}
if(n > (unsigned long long)-1){
if(declared_size  && size == Cyc_Absyn_Int_sz)
Cyc_Lex_err(({const char*_tmpA9="integer constant too large";_tag_dyneither(_tmpA9,sizeof(char),27);}),lbuf);
size=Cyc_Absyn_LongLong_sz;}
# 394
if(size == Cyc_Absyn_Int_sz)
res=({union Cyc_Absyn_Cnst _tmpAA;(_tmpAA.Int_c).val=({struct _tuple5 _tmpAB;_tmpAB.f1=sn;_tmpAB.f2=(int)((unsigned int)n);_tmpAB;});(_tmpAA.Int_c).tag=5;_tmpAA;});else{
# 397
res=({union Cyc_Absyn_Cnst _tmpAC;(_tmpAC.LongLong_c).val=({struct _tuple6 _tmpAD;_tmpAD.f1=sn;_tmpAD.f2=(long long)n;_tmpAD;});(_tmpAC.LongLong_c).tag=6;_tmpAC;});}}else{
# 400
long long n=(long long)0;
{int i=0;for(0;i < len;++ i){
n=n * base + (long long)Cyc_Lex_int_of_char(*((char*)_check_dyneither_subscript(buff,sizeof(char),i)));}}{
unsigned long long x=(unsigned long long)n >> 32;
if(x != -1  && x != 0){
if(declared_size  && size == Cyc_Absyn_Int_sz)
Cyc_Lex_err(({const char*_tmpAE="integer constant too large";_tag_dyneither(_tmpAE,sizeof(char),27);}),lbuf);
size=Cyc_Absyn_LongLong_sz;}
# 409
if(size == Cyc_Absyn_Int_sz)
res=({union Cyc_Absyn_Cnst _tmpAF;(_tmpAF.Int_c).val=({struct _tuple5 _tmpB0;_tmpB0.f1=sn;_tmpB0.f2=(int)n;_tmpB0;});(_tmpAF.Int_c).tag=5;_tmpAF;});else{
# 412
res=({union Cyc_Absyn_Cnst _tmpB1;(_tmpB1.LongLong_c).val=({struct _tuple6 _tmpB2;_tmpB2.f1=sn;_tmpB2.f2=n;_tmpB2;});(_tmpB1.LongLong_c).tag=6;_tmpB1;});}};}
# 415
return res;};}
# 420
char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};
int Cyc_Lex_string_pos=0;
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
struct _dyneither_ptr str=({unsigned int _tmpB3=(unsigned int)newsz;char*_tmpB4=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpB3 + 1));struct _dyneither_ptr _tmpB6=_tag_dyneither(_tmpB4,sizeof(char),_tmpB3 + 1);{unsigned int _tmpB5=_tmpB3;unsigned int i;for(i=0;i < _tmpB5;i ++){_tmpB4[i]=(char)(i < sz?*((char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):'\000');}_tmpB4[_tmpB5]=(char)0;}_tmpB6;});
Cyc_Lex_string_buffer=str;};}
# 431
({struct _dyneither_ptr _tmpB7=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);char _tmpB8=*((char*)_check_dyneither_subscript(_tmpB7,sizeof(char),0));char _tmpB9=c;if(_get_dyneither_size(_tmpB7,sizeof(char))== 1  && (_tmpB8 == '\000'  && _tmpB9 != '\000'))_throw_arraybounds();*((char*)_tmpB7.curr)=_tmpB9;});
++ Cyc_Lex_string_pos;}
# 434
void Cyc_Lex_store_string(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));}}
# 439
struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;};
# 455 "lex.cyl"
typedef struct Cyc_Lex_Ldecls*Cyc_Lex_ldecls_t;
# 457
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 459
static struct Cyc_Lex_Ldecls*Cyc_Lex_mk_empty_ldecls(int ignore){
return({struct Cyc_Lex_Ldecls*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpBA;});}
# 463
static void Cyc_Lex_typedef_init(){
Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->v=((struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Lex_mk_empty_ldecls);_tmpBB;});}
# 467
static void Cyc_Lex_recompute_typedefs(){
# 469
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpBC=tdefs;struct Cyc_Lex_DynTrie*_tmpBD=Cyc_Lex_typedefs_trie;tdefs=_tmpBD;Cyc_Lex_typedefs_trie=_tmpBC;});{
struct Cyc_Lex_DynTrie _tmpBE=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmpBF=_tmpBE;struct Cyc_Core_DynamicRegion*_tmpCD;struct Cyc_Lex_Trie*_tmpCC;_LL23: _tmpCD=_tmpBF.dyn;_tmpCC=_tmpBF.t;_LL24:;
Cyc_Core_free_ukey(_tmpCD);{
struct Cyc_Core_NewDynamicRegion _tmpC0=Cyc_Core_new_ukey();struct Cyc_Core_NewDynamicRegion _tmpC1=_tmpC0;struct Cyc_Core_DynamicRegion*_tmpCB;_LL26: _tmpCB=_tmpC1.key;_LL27:;{
struct Cyc_Lex_Trie*t2=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpCB,0,Cyc_Lex_empty_trie);
*tdefs=({struct Cyc_Lex_DynTrie _tmpC2;_tmpC2.dyn=(struct Cyc_Core_DynamicRegion*)_tmpCB;_tmpC2.t=(struct Cyc_Lex_Trie*)t2;_tmpC2;});
({struct Cyc_Lex_DynTrie*_tmpC3=Cyc_Lex_typedefs_trie;struct Cyc_Lex_DynTrie*_tmpC4=tdefs;Cyc_Lex_typedefs_trie=_tmpC4;tdefs=_tmpC3;});{
# 479
struct Cyc_List_List*_tmpC5=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->availables;for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){
void*_tmpC6=(void*)_tmpC5->hd;void*_tmpC7=_tmpC6;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC7)->tag == 1){_LL29: _tmpC9=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC7)->f1;_LL2A:
 _tmpCA=_tmpC9;goto _LL2C;}else{_LL2B: _tmpCA=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC7)->f1;_LL2C: {
# 483
struct Cyc_Lex_Ldecls*_tmpC8=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,_tmpCA);
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpC8->typedefs);
goto _LL28;}}_LL28:;}};};};};}
# 491
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
struct Cyc_List_List*ans=((struct Cyc_List_List*(*)(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*))Cyc_Binding_resolve_rel_ns)(0,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,ns);
struct Cyc_Lex_Ldecls*_tmpCE=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,ans);
return((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmpCE->typedefs,v);}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Lex_Trie*f2;struct _dyneither_ptr f3;};
# 497
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple24*env){
# 500
struct _tuple24 _tmpCF=*env;struct _tuple24 _tmpD0=_tmpCF;struct Cyc_List_List*_tmpDA;struct Cyc_Lex_Trie*_tmpD9;struct _dyneither_ptr _tmpD8;_LL2E: _tmpDA=_tmpD0.f1;_tmpD9=_tmpD0.f2;_tmpD8=_tmpD0.f3;_LL2F:;{
int len=(int)(_get_dyneither_size(_tmpD8,sizeof(char))- 1);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpD1=((struct Cyc_Lex_Trie*)_check_null(_tmpD9))->children;union Cyc_Lex_TrieChildren _tmpD2=_tmpD1;struct Cyc_Lex_Trie**_tmpD7;struct Cyc_Lex_Trie*_tmpD6;int _tmpD5;struct Cyc_Lex_Trie*_tmpD4;switch((_tmpD2.One).tag){case 1: _LL31: _LL32:
 return 0;case 2: _LL33: _tmpD5=((_tmpD2.One).val).f1;_tmpD4=((_tmpD2.One).val).f2;if(_tmpD5 != *((const char*)_check_dyneither_subscript(_tmpD8,sizeof(char),i))){_LL34:
 return 0;}else{_LL35: _tmpD6=((_tmpD2.One).val).f2;_LL36:
 _tmpD9=_tmpD6;goto _LL30;}default: _LL37: _tmpD7=(_tmpD2.Many).val;_LL38: {
# 508
struct Cyc_Lex_Trie*_tmpD3=_tmpD7[_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(_tmpD8,sizeof(char),i))))];
if(_tmpD3 == 0)
return 0;
_tmpD9=_tmpD3;
goto _LL30;}}_LL30:;}}
# 514
return((struct Cyc_Lex_Trie*)_check_null(_tmpD9))->shared_str;};}
# 516
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 521
struct _dyneither_ptr _tmpDB=*v;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpDC=tdefs;struct Cyc_Lex_DynTrie*_tmpDD=Cyc_Lex_typedefs_trie;tdefs=_tmpDD;Cyc_Lex_typedefs_trie=_tmpDC;});{
struct Cyc_Lex_DynTrie _tmpDE=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmpDF=_tmpDE;struct Cyc_Core_DynamicRegion*_tmpE7;struct Cyc_Lex_Trie*_tmpE6;_LL3A: _tmpE7=_tmpDF.dyn;_tmpE6=_tmpDF.t;_LL3B:;{
struct _tuple24 _tmpE0=({struct _tuple24 _tmpE5;_tmpE5.f1=ns;_tmpE5.f2=_tmpE6;_tmpE5.f3=_tmpDB;_tmpE5;});
int _tmpE1=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple24*arg,int(*body)(struct _RegionHandle*h,struct _tuple24*arg)))Cyc_Core_open_region)(_tmpE7,& _tmpE0,Cyc_Lex_is_typedef_body);
*tdefs=({struct Cyc_Lex_DynTrie _tmpE2;_tmpE2.dyn=(struct Cyc_Core_DynamicRegion*)_tmpE7;_tmpE2.t=(struct Cyc_Lex_Trie*)_tmpE6;_tmpE2;});
({struct Cyc_Lex_DynTrie*_tmpE3=tdefs;struct Cyc_Lex_DynTrie*_tmpE4=Cyc_Lex_typedefs_trie;tdefs=_tmpE4;Cyc_Lex_typedefs_trie=_tmpE3;});
return _tmpE1;};};};}
# 532
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
((void(*)(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_enter_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,s,1,Cyc_Lex_mk_empty_ldecls);{
struct Cyc_Lex_Ldecls*_tmpE8=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);
# 537
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpE8->typedefs);};}
# 539
void Cyc_Lex_leave_namespace(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
Cyc_Lex_recompute_typedefs();}
# 543
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(0,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,q);
struct Cyc_Lex_Ldecls*_tmpEA=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,_tmpE9);
# 548
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpEA->typedefs);}
# 550
void Cyc_Lex_leave_using(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
# 553
Cyc_Lex_recompute_typedefs();}
# 556
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 558
struct Cyc_Lex_Ldecls*_tmpEB=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);
_tmpEB->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpEB->typedefs,(*q).f2);
# 563
Cyc_Lex_insert_typedef((*q).f2);}
# 566
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 570
if(symbol_num <= Cyc_Lex_num_kws){
# 572
if(!Cyc_Lex_in_extern_c  || (*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).is_c_keyword){
short res=(short)(*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).token_index;
return res;}}{
# 578
struct _dyneither_ptr*_tmpEC=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpEC;
# 582
if(Cyc_Lex_is_typedef(0,_tmpEC))
return 371;
# 586
return 363;};}
# 589
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
if(Cyc_Lex_in_extern_c)
Cyc_Lex_err(({const char*_tmpED="qualified identifiers not allowed in C code";_tag_dyneither(_tmpED,sizeof(char),44);}),lbuf);{
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _dyneither_ptr s=lbuf->lex_buffer;
# 596
struct _dyneither_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 599
while(i < end){
int start=i;
for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){
;}
if(start == i)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Core_Impossible_exn_struct _tmpEF;_tmpEF.tag=Cyc_Core_Impossible;_tmpEF.f1=({const char*_tmpF0="bad namespace";_tag_dyneither(_tmpF0,sizeof(char),14);});_tmpEF;});_tmpEE;}));{
int vlen=i - start;
if(v != 0)
rev_vs=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=v;_tmpF1->tl=rev_vs;_tmpF1;});
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));
i +=2;};}
# 611
if(v == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Core_Impossible_exn_struct _tmpF3;_tmpF3.tag=Cyc_Core_Impossible;_tmpF3.f1=({const char*_tmpF4="bad namespace";_tag_dyneither(_tmpF4,sizeof(char),14);});_tmpF3;});_tmpF2;}));{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 616
if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),({const char*_tmpF5="Cyc";_tag_dyneither(_tmpF5,sizeof(char),4);}))== 0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->f1=Cyc_Absyn_Abs_n(vs,0);_tmpF6->f2=v;_tmpF6;});}else{
# 620
if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),({const char*_tmpF7="C";_tag_dyneither(_tmpF7,sizeof(char),2);}))== 0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=Cyc_Absyn_Abs_n(vs,1);_tmpF8->f2=v;_tmpF8;});}else{
# 624
Cyc_Lex_token_qvar=({struct _tuple0*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=Cyc_Absyn_Rel_n(vs);_tmpF9->f2=v;_tmpF9;});}}
if(Cyc_Lex_is_typedef(vs,v))
return 373;
return 372;};};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned int starting_line;struct _dyneither_ptr filename;struct _dyneither_ptr linenumpos;unsigned int linenumpos_offset;};
# 640
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_t;
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_opt_t;
# 643
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 645
static int Cyc_Lex_linenumber=1;
# 648
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _dyneither_ptr filename,unsigned int starting_line,struct Cyc_Lex_PosInfo*next){
# 652
struct _dyneither_ptr linenumpos=({unsigned int _tmpFB=10;unsigned int*_tmpFC=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmpFB);_tag_dyneither(_tmpFC,sizeof(unsigned int),_tmpFB);});
*((unsigned int*)_check_dyneither_subscript(linenumpos,sizeof(unsigned int),0))=(unsigned int)Cyc_yylloc.first_line;
return({struct Cyc_Lex_PosInfo*_tmpFA=_region_malloc(r,sizeof(*_tmpFA));_tmpFA->next=next;_tmpFA->starting_line=starting_line;_tmpFA->filename=(struct _dyneither_ptr)filename;_tmpFA->linenumpos=linenumpos;_tmpFA->linenumpos_offset=1;_tmpFA;});}
# 661
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0)(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Core_Impossible_exn_struct _tmpFE;_tmpFE.tag=Cyc_Core_Impossible;_tmpFE.f1=({const char*_tmpFF="empty position info!";_tag_dyneither(_tmpFF,sizeof(char),21);});_tmpFE;});_tmpFD;}));{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _dyneither_ptr linenums=_tag_dyneither(0,0,0);
({struct _dyneither_ptr _tmp100=p->linenumpos;struct _dyneither_ptr _tmp101=linenums;p->linenumpos=_tmp101;linenums=_tmp100;});{
unsigned int offset=p->linenumpos_offset;
unsigned int n=_get_dyneither_size(linenums,sizeof(unsigned int));
# 669
if(offset >= n){
# 671
struct _dyneither_ptr newlinenums=({unsigned int _tmp104=n * 2;unsigned int*_tmp105=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp104);_tag_dyneither(_tmp105,sizeof(unsigned int),_tmp104);});
{unsigned int i=0;for(0;i < n;++ i){
*((unsigned int*)_check_dyneither_subscript(newlinenums,sizeof(unsigned int),(int)i))=((unsigned int*)linenums.curr)[(int)i];}}
({struct _dyneither_ptr _tmp102=linenums;struct _dyneither_ptr _tmp103=newlinenums;linenums=_tmp103;newlinenums=_tmp102;});
# 676
((void(*)(unsigned int*ptr))Cyc_Core_ufree)((unsigned int*)_untag_dyneither_ptr(newlinenums,sizeof(unsigned int),1));}
# 679
*((unsigned int*)_check_dyneither_subscript(linenums,sizeof(unsigned int),(int)offset))=(unsigned int)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + 1;
++ Cyc_Lex_linenumber;
({struct _dyneither_ptr _tmp106=p->linenumpos;struct _dyneither_ptr _tmp107=linenums;p->linenumpos=_tmp107;linenums=_tmp106;});};};}
# 686
static void Cyc_Lex_process_directive(struct _dyneither_ptr line){
int i;
char buf[100];
struct _dyneither_ptr filename=({const char*_tmp115="";_tag_dyneither(_tmp115,sizeof(char),1);});
if(({struct Cyc_CharPtr_sa_ScanfArg_struct _tmp10B;_tmp10B.tag=7;_tmp10B.f1=_tag_dyneither(buf,sizeof(char),100);({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp10A;_tmp10A.tag=2;_tmp10A.f1=& i;({void*_tmp108[2]={& _tmp10A,& _tmp10B};Cyc_sscanf((struct _dyneither_ptr)line,({const char*_tmp109="# %d \"%s";_tag_dyneither(_tmp109,sizeof(char),9);}),_tag_dyneither(_tmp108,sizeof(void*),2));});});})== 2){
if(Cyc_Lex_compile_for_boot_r){
# 694
int i=(int)(Cyc_strlen(_tag_dyneither(buf,sizeof(char),100))- 1);
int last_slash=-1;
while(i >= 0){
if(buf[_check_known_subscript_notnull(100,i)]== '/'){last_slash=i;break;}
-- i;}
# 700
filename=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(_tag_dyneither(buf,sizeof(char),100),sizeof(char),last_slash),sizeof(char),1));({void*_tmp10C[1]={& _tmp10E};Cyc_aprintf(({const char*_tmp10D="\"%s";_tag_dyneither(_tmp10D,sizeof(char),4);}),_tag_dyneither(_tmp10C,sizeof(void*),1));});});}else{
# 702
filename=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0;_tmp111.f1=(struct _dyneither_ptr)_tag_dyneither(buf,sizeof(char),100);({void*_tmp10F[1]={& _tmp111};Cyc_aprintf(({const char*_tmp110="\"%s";_tag_dyneither(_tmp110,sizeof(char),4);}),_tag_dyneither(_tmp10F,sizeof(void*),1));});});}
if((Cyc_Lex_linenumber == i  && Cyc_Lex_pos_info != 0) && Cyc_strcmp((struct _dyneither_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _dyneither_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp114;_tmp114.tag=2;_tmp114.f1=& i;({void*_tmp112[1]={& _tmp114};Cyc_sscanf((struct _dyneither_ptr)line,({const char*_tmp113="# %d";_tag_dyneither(_tmp113,sizeof(char),5);}),_tag_dyneither(_tmp112,sizeof(void*),1));});})== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 710
++ Cyc_Lex_linenumber;
return;}}
# 713
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned int)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple25{struct _dyneither_ptr f1;unsigned int f2;};
# 719
struct _tuple25 Cyc_Lex_xlate_pos(unsigned int char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned int first_char_offset=*((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),0));
if(char_offset < first_char_offset  && p->next != 0)continue;{
# 725
unsigned int base=0;
unsigned int size=p->linenumpos_offset;
while(size > 1){
int half=(int)(size / 2);
int mid=(int)(base + half);
if(char_offset > *((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),mid))){
base=base + half;
size=size - half;}else{
# 734
size=(unsigned int)half;}}
# 737
return({struct _tuple25 _tmp116;_tmp116.f1=p->filename;_tmp116.f2=p->starting_line + base;_tmp116;});};}}
# 739
return({struct _tuple25 _tmp117;_tmp117.f1=_tag_dyneither(0,0,0);_tmp117.f2=0;_tmp117;});}
# 744
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);
# 756
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 758
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
Cyc_yylloc.first_line=(yyllocptr->first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf));
Cyc_yylloc.last_line=(yyllocptr->last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf));
{int _tmp118=ans;switch(_tmp118){case 363: _LL3D: _LL3E:
# 763
*yylval=({union Cyc_YYSTYPE _tmp119;(_tmp119.String_tok).val=Cyc_Lex_token_string;(_tmp119.String_tok).tag=3;_tmp119;});goto _LL3C;case 371: _LL3F: _LL40:
# 765
*yylval=({union Cyc_YYSTYPE _tmp11A;(_tmp11A.String_tok).val=Cyc_Lex_token_string;(_tmp11A.String_tok).tag=3;_tmp11A;});goto _LL3C;case 372: _LL41: _LL42:
# 767
*yylval=({union Cyc_YYSTYPE _tmp11B;(_tmp11B.QualId_tok).val=Cyc_Lex_token_qvar;(_tmp11B.QualId_tok).tag=5;_tmp11B;});goto _LL3C;case 373: _LL43: _LL44:
# 769
*yylval=({union Cyc_YYSTYPE _tmp11C;(_tmp11C.QualId_tok).val=Cyc_Lex_token_qvar;(_tmp11C.QualId_tok).tag=5;_tmp11C;});goto _LL3C;case 370: _LL45: _LL46:
# 771
*yylval=({union Cyc_YYSTYPE _tmp11D;(_tmp11D.String_tok).val=Cyc_Lex_token_string;(_tmp11D.String_tok).tag=3;_tmp11D;});goto _LL3C;case 364: _LL47: _LL48:
# 773
*yylval=({union Cyc_YYSTYPE _tmp11E;(_tmp11E.Int_tok).val=Cyc_Lex_token_int;(_tmp11E.Int_tok).tag=1;_tmp11E;});goto _LL3C;case 367: _LL49: _LL4A:
# 775
*yylval=({union Cyc_YYSTYPE _tmp11F;(_tmp11F.Char_tok).val=Cyc_Lex_token_char;(_tmp11F.Char_tok).tag=2;_tmp11F;});goto _LL3C;case 368: _LL4B: _LL4C:
# 777
*yylval=({union Cyc_YYSTYPE _tmp120;(_tmp120.String_tok).val=Cyc_Lex_token_string;(_tmp120.String_tok).tag=3;_tmp120;});goto _LL3C;case 369: _LL4D: _LL4E:
# 779
*yylval=({union Cyc_YYSTYPE _tmp121;(_tmp121.String_tok).val=Cyc_Lex_token_string;(_tmp121.String_tok).tag=3;_tmp121;});goto _LL3C;case 365: _LL4F: _LL50:
# 781
*yylval=({union Cyc_YYSTYPE _tmp122;(_tmp122.String_tok).val=Cyc_Lex_token_string;(_tmp122.String_tok).tag=3;_tmp122;});goto _LL3C;case 366: _LL51: _LL52:
# 783
*yylval=({union Cyc_YYSTYPE _tmp123;(_tmp123.String_tok).val=Cyc_Lex_token_string;(_tmp123.String_tok).tag=3;_tmp123;});goto _LL3C;case 375: _LL53: _LL54:
# 785
*yylval=({union Cyc_YYSTYPE _tmp124;(_tmp124.Asm_tok).val=Cyc_Lex_token_asm;(_tmp124.Asm_tok).tag=6;_tmp124;});goto _LL3C;default: _LL55: _LL56:
 goto _LL3C;}_LL3C:;}
# 788
return ans;}
# 795
const int Cyc_Lex_lex_base[330]={0,113,119,120,125,126,127,132,136,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,175,- 1,30,16,23,50,32,45,47,55,181,40,48,53,65,47,60,58,66,73,78,187,169,10,12,21,168,- 4,194,- 1,186,- 5,259,287,345,267,198,- 4,- 3,- 2,11,2,199,- 17,213,- 1,422,- 18,12,- 12,- 11,275,- 13,- 10,- 9,495,518,310,- 14,240,- 17,13,- 1,- 15,- 16,14,- 1,573,368,646,721,438,- 16,- 70,233,- 50,15,30,- 52,16,31,142,171,32,163,169,449,206,798,841,168,197,114,190,407,911,1000,128,1075,1133,1208,156,- 69,- 34,- 40,1283,137,1358,1433,1491,1566,1641,164,1716,1791,1866,1941,2016,2091,2166,2241,- 35,- 53,- 54,144,146,158,145,169,154,154,160,160,176,165,172,2316,179,180,177,2391,186,191,194,2476,264,190,180,197,183,204,205,223,232,2551,229,231,245,232,2636,237,239,2711,250,242,260,269,263,257,259,2796,281,282,282,290,2871,- 15,293,303,291,295,291,307,295,317,2956,311,321,322,3031,315,3116,318,318,335,327,323,334,342,333,347,3191,332,337,351,338,3276,- 26,396,- 38,- 23,397,- 25,- 43,- 37,- 46,401,- 47,3351,3380,589,- 22,571,386,402,- 21,394,808,3419,3450,3485,3525,573,404,3595,3633,795,405,407,399,431,- 19,437,796,449,450,- 20,442,797,500,501,- 19,493,- 51,450,- 32,- 50,17,418,3565,- 45,- 28,- 30,- 44,- 27,- 29,- 31,1,3673,2,426,550,427,428,429,443,479,489,491,492,494,3746,3830,- 67,- 61,- 60,- 59,- 58,- 57,- 56,- 55,- 62,- 65,- 66,880,495,- 63,- 64,- 68,- 39,- 36,- 33,620,- 48,18,- 24,641};
const int Cyc_Lex_lex_backtrk[330]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,2,- 1,2,- 1,5,- 1,2,- 1,2,2,2,- 1,- 1,- 1,1,3,15,- 1,15,- 1,18,- 1,1,- 1,- 1,13,- 1,- 1,- 1,- 1,14,13,- 1,15,- 1,1,- 1,- 1,- 1,14,- 1,17,12,- 1,13,12,- 1,- 1,48,- 1,49,69,- 1,69,69,69,69,69,69,69,69,69,18,20,69,69,69,69,69,1,1,69,69,69,1,69,- 1,- 1,- 1,1,- 1,1,0,- 1,2,2,- 1,3,1,1,1,1,1,1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,21,20,- 1,- 1,20,20,20,- 1,20,- 1,21,21,18,19,18,18,- 1,17,17,17,17,17,18,- 1,18,19,19,19,- 1,19,18,18,18,- 1,18,- 1,- 1,- 1,- 1,49,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,47,- 1,48};
const int Cyc_Lex_lex_default[330]={96,86,60,65,60,49,45,- 1,9,0,0,- 1,0,19,0,0,- 1,0,0,19,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,59,0,59,0,- 1,0,- 1,- 1,- 1,- 1,59,0,0,0,- 1,- 1,82,0,82,0,- 1,0,- 1,0,0,- 1,0,0,0,- 1,- 1,- 1,0,82,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,0,- 1,- 1,0,325,- 1,- 1,290,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,22,- 1,- 1,- 1,75,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,76,- 1,- 1,- 1,- 1,95,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,199,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,9,- 1,- 1,- 1,17,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,77,- 1,- 1,- 1,- 1,83,0,- 1,0,0,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,0,- 1,0,277,0,0,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,325,0,- 1,0,- 1};
const int Cyc_Lex_lex_trans[4087]={0,0,0,0,0,0,0,0,0,97,98,97,97,99,22,20,20,20,21,21,17,18,18,18,95,279,326,279,326,327,0,0,97,100,101,102,24,103,104,105,321,320,106,107,18,108,109,110,111,112,112,112,112,112,112,112,112,112,113,18,114,115,116,24,117,118,118,118,118,118,118,118,118,118,118,118,119,118,118,118,118,118,118,118,118,118,118,118,118,118,118,328,324,289,120,121,122,123,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,87,124,34,88,27,61,62,66,28,84,67,61,62,50,17,63,51,46,23,23,23,23,23,10,29,30,11,31,61,89,85,68,32,33,61,35,36,52,37,38,39,23,40,41,42,43,47,12,13,24,44,48,233,14,15,- 1,- 1,322,- 1,- 1,16,23,23,23,23,23,144,33,33,33,33,33,132,44,44,44,44,44,64,- 1,323,60,90,287,23,- 1,- 1,- 1,- 1,69,33,284,24,- 1,126,53,44,18,18,132,83,288,18,238,20,- 1,239,285,286,- 1,- 1,55,55,55,55,55,55,55,55,329,213,329,329,168,- 1,276,18,- 1,230,231,277,215,153,125,234,235,236,- 1,- 1,- 1,291,216,329,160,278,154,186,25,155,- 1,161,- 1,187,156,157,18,158,127,159,164,18,18,162,- 1,163,18,165,- 1,- 1,166,26,167,183,18,178,172,173,18,174,18,175,18,- 1,56,58,58,58,58,58,58,58,58,20,20,20,20,20,20,20,20,80,80,80,80,80,80,80,80,176,- 1,177,179,57,57,57,57,57,57,57,57,57,57,180,181,182,184,185,188,189,57,57,57,57,57,57,81,81,81,81,81,81,81,81,200,169,194,83,190,191,201,192,193,- 1,70,170,195,171,196,- 1,54,20,57,57,57,57,57,57,197,198,17,57,57,57,57,57,57,57,57,57,57,209,202,203,204,205,206,207,57,57,57,57,57,57,94,94,94,94,94,94,94,94,- 1,- 1,208,- 1,210,211,212,214,18,225,217,71,218,219,220,221,222,223,57,57,57,57,57,57,224,226,- 1,227,228,229,- 1,- 1,72,232,237,240,279,73,247,280,283,319,315,314,313,- 1,74,74,74,74,74,74,74,74,248,248,263,261,312,70,70,75,75,75,75,75,75,75,75,75,247,281,- 1,282,282,282,282,282,282,282,282,282,282,264,147,148,248,248,263,261,76,70,70,149,311,20,60,150,264,151,54,268,269,269,310,152,309,308,17,307,316,0,22,0,9,264,77,0,78,79,79,79,79,79,79,79,79,79,79,0,264,0,0,268,269,269,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,273,274,274,0,0,0,0,79,79,79,79,79,79,316,0,0,79,79,79,79,79,79,317,317,317,317,317,317,317,317,0,73,273,274,274,0,76,0,0,79,79,79,79,79,79,91,91,91,91,91,91,91,91,0,326,0,250,327,250,0,72,251,251,251,251,251,251,251,251,251,251,249,0,265,329,0,329,329,0,0,248,0,264,0,0,0,0,0,0,77,0,0,0,0,18,20,0,329,0,60,0,0,0,249,0,265,0,54,0,0,0,17,248,22,264,9,0,92,93,93,93,93,93,93,93,93,93,93,0,0,- 1,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,241,0,253,253,253,253,253,253,253,253,254,254,251,251,251,251,251,251,251,251,251,251,0,243,244,0,0,262,270,275,255,0,- 1,0,0,0,70,269,274,256,0,0,257,241,0,242,242,242,242,242,242,242,242,242,242,243,244,0,0,262,270,275,255,0,0,0,243,244,70,269,274,256,0,245,257,316,0,0,0,0,0,0,246,0,318,318,318,318,318,318,318,318,0,0,0,0,0,0,243,244,0,0,0,0,0,245,0,0,0,0,0,0,0,0,246,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,145,0,0,0,0,146,0,0,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,138,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,130,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,131,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,134,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,135,0,0,0,0,0,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,133,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,0,0,0,0,134,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,139,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,140,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,141,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,142,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,143,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,252,252,252,252,252,252,252,252,252,252,0,0,0,0,0,0,0,0,0,0,0,243,244,0,0,0,0,241,244,242,242,242,242,242,242,242,242,242,242,0,0,0,0,0,0,0,0,0,- 1,0,243,244,0,243,244,0,0,245,0,0,244,0,0,0,0,0,246,0,251,251,251,251,251,251,251,251,251,251,0,0,0,0,243,244,0,0,0,0,0,245,244,0,0,0,0,0,244,0,246,252,252,252,252,252,252,252,252,252,252,0,0,0,0,0,0,0,0,0,0,0,243,244,244,0,0,0,0,244,244,0,0,0,241,- 1,253,253,253,253,253,253,253,253,254,254,0,0,0,0,0,0,0,0,243,244,0,243,244,0,0,244,0,0,271,0,0,0,0,0,0,0,0,272,241,0,254,254,254,254,254,254,254,254,254,254,0,0,0,243,244,0,0,0,0,0,271,243,244,0,0,0,0,0,266,272,0,0,0,0,0,0,0,267,0,0,282,282,282,282,282,282,282,282,282,282,0,0,0,243,244,0,0,0,0,0,266,243,244,0,0,0,0,0,244,267,258,258,258,258,258,258,258,258,258,258,0,0,0,0,0,0,0,258,258,258,258,258,258,243,244,0,0,0,0,0,244,0,0,0,0,0,0,0,258,258,258,258,258,258,258,258,258,258,0,258,258,258,258,258,258,258,258,258,258,258,258,0,0,0,292,0,259,0,0,293,0,0,0,0,0,260,0,0,294,294,294,294,294,294,294,294,0,258,258,258,258,258,258,295,0,0,0,0,259,0,0,0,0,0,0,0,0,260,0,0,0,0,0,0,0,0,0,0,0,0,0,0,296,0,0,0,0,297,298,0,0,0,299,0,0,0,0,0,0,0,300,0,0,0,301,0,302,0,303,0,304,305,305,305,305,305,305,305,305,305,305,0,0,0,0,0,0,0,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,0,0,0,0,0,0,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,306,0,0,0,0,0,0,0,0,305,305,305,305,305,305,305,305,305,305,0,0,0,0,0,0,0,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,0,0,0,0,0,0,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[4087]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,13,19,21,13,19,46,63,71,84,88,99,102,280,327,102,- 1,- 1,0,0,0,0,64,0,0,0,290,292,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,47,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,103,106,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,25,1,26,2,2,3,27,2,3,4,4,5,6,4,5,6,7,7,7,7,7,8,28,29,8,30,2,1,2,3,31,32,4,34,35,5,36,37,38,7,39,40,41,42,6,8,8,7,43,6,115,8,8,49,45,104,105,45,8,23,23,23,23,23,120,33,33,33,33,33,129,44,44,44,44,44,4,49,104,51,1,107,23,59,65,105,45,3,33,108,23,45,124,5,44,53,33,135,67,107,53,113,44,51,113,108,108,59,65,53,53,53,53,53,53,53,53,97,148,97,97,150,67,110,53,82,116,116,110,147,152,0,114,114,114,49,13,19,105,147,97,151,110,153,149,23,154,102,151,82,149,155,156,53,157,124,158,160,53,53,161,51,162,53,164,59,65,165,23,166,169,53,170,171,172,53,173,53,174,53,67,53,55,55,55,55,55,55,55,55,58,58,58,58,58,58,58,58,74,74,74,74,74,74,74,74,175,82,176,178,56,56,56,56,56,56,56,56,56,56,179,180,181,183,184,187,187,56,56,56,56,56,56,80,80,80,80,80,80,80,80,186,168,188,1,189,190,186,191,192,2,3,168,194,168,195,4,5,6,56,56,56,56,56,56,196,197,8,57,57,57,57,57,57,57,57,57,57,200,201,202,203,204,205,206,57,57,57,57,57,57,91,91,91,91,91,91,91,91,49,45,207,105,209,210,211,213,69,215,216,69,217,218,219,220,221,222,57,57,57,57,57,57,223,225,51,226,227,228,59,65,69,231,234,239,277,69,246,277,281,293,295,296,297,67,69,69,69,69,69,69,69,69,247,249,256,260,298,261,262,69,94,94,94,94,94,94,94,94,246,109,82,109,109,109,109,109,109,109,109,109,109,263,117,117,247,249,256,260,69,261,262,117,299,69,69,117,265,117,69,267,268,270,300,117,301,302,69,303,318,- 1,69,- 1,69,263,69,- 1,69,78,78,78,78,78,78,78,78,78,78,- 1,265,- 1,- 1,267,268,270,78,78,78,78,78,78,79,79,79,79,79,79,79,79,79,79,272,273,275,- 1,- 1,- 1,- 1,79,79,79,79,79,79,294,- 1,- 1,78,78,78,78,78,78,294,294,294,294,294,294,294,294,- 1,90,272,273,275,- 1,90,- 1,- 1,79,79,79,79,79,79,90,90,90,90,90,90,90,90,- 1,325,- 1,243,325,243,- 1,90,243,243,243,243,243,243,243,243,243,243,245,- 1,255,329,- 1,329,329,- 1,- 1,245,- 1,255,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,90,90,- 1,329,- 1,90,- 1,- 1,- 1,245,- 1,255,- 1,90,- 1,- 1,- 1,90,245,90,255,90,- 1,90,92,92,92,92,92,92,92,92,92,92,- 1,- 1,277,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,111,- 1,111,111,111,111,111,111,111,111,111,111,250,250,250,250,250,250,250,250,250,250,- 1,111,111,- 1,- 1,259,266,271,111,- 1,325,- 1,- 1,- 1,259,266,271,111,- 1,- 1,111,112,- 1,112,112,112,112,112,112,112,112,112,112,111,111,- 1,- 1,259,266,271,111,- 1,- 1,- 1,112,112,259,266,271,111,- 1,112,111,317,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,112,112,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,118,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,- 1,- 1,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,118,- 1,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,119,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,119,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,121,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,128,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,- 1,130,- 1,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,- 1,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,131,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,132,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,133,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,- 1,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,133,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,134,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,136,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,137,- 1,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,138,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,139,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,140,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,141,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,142,142,142,142,142,142,142,142,142,142,- 1,- 1,- 1,- 1,- 1,- 1,- 1,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,- 1,- 1,- 1,- 1,142,- 1,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,143,143,143,143,143,143,143,143,143,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,- 1,- 1,- 1,- 1,143,- 1,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,- 1,- 1,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,159,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,163,- 1,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,167,159,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,177,177,177,177,177,177,177,177,177,177,- 1,- 1,- 1,- 1,- 1,- 1,- 1,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,- 1,- 1,- 1,- 1,177,163,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,182,182,182,182,182,182,182,182,182,182,- 1,- 1,- 1,- 1,- 1,- 1,- 1,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,- 1,- 1,- 1,- 1,182,167,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,- 1,- 1,- 1,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,185,177,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,- 1,- 1,- 1,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,193,182,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,- 1,- 1,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,198,185,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,208,208,208,208,208,208,208,208,208,208,- 1,- 1,- 1,- 1,- 1,- 1,- 1,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,- 1,- 1,- 1,- 1,208,193,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,- 1,- 1,- 1,- 1,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,- 1,212,198,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,214,214,214,214,214,214,214,214,214,214,- 1,- 1,- 1,- 1,- 1,- 1,- 1,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,- 1,- 1,- 1,- 1,214,208,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,224,224,224,224,224,224,224,224,224,224,- 1,- 1,- 1,- 1,- 1,- 1,- 1,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,- 1,- 1,- 1,- 1,224,212,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,229,229,229,229,229,229,229,229,229,229,- 1,- 1,- 1,- 1,- 1,- 1,- 1,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,- 1,- 1,- 1,- 1,229,214,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,241,241,241,241,241,241,241,241,241,241,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,241,241,- 1,- 1,- 1,- 1,242,241,242,242,242,242,242,242,242,242,242,242,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,224,- 1,242,242,- 1,241,241,- 1,- 1,242,- 1,- 1,241,- 1,- 1,- 1,- 1,- 1,242,- 1,251,251,251,251,251,251,251,251,251,251,- 1,- 1,- 1,- 1,242,242,- 1,- 1,- 1,- 1,- 1,242,251,- 1,- 1,- 1,- 1,- 1,251,- 1,242,252,252,252,252,252,252,252,252,252,252,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,252,252,251,- 1,- 1,- 1,- 1,252,251,- 1,- 1,- 1,253,229,253,253,253,253,253,253,253,253,253,253,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,252,252,- 1,253,253,- 1,- 1,252,- 1,- 1,253,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,253,254,- 1,254,254,254,254,254,254,254,254,254,254,- 1,- 1,- 1,253,253,- 1,- 1,- 1,- 1,- 1,253,254,254,- 1,- 1,- 1,- 1,- 1,254,253,- 1,- 1,- 1,- 1,- 1,- 1,- 1,254,- 1,- 1,282,282,282,282,282,282,282,282,282,282,- 1,- 1,- 1,254,254,- 1,- 1,- 1,- 1,- 1,254,282,282,- 1,- 1,- 1,- 1,- 1,282,254,257,257,257,257,257,257,257,257,257,257,- 1,- 1,- 1,- 1,- 1,- 1,- 1,257,257,257,257,257,257,282,282,- 1,- 1,- 1,- 1,- 1,282,- 1,- 1,- 1,- 1,- 1,- 1,- 1,258,258,258,258,258,258,258,258,258,258,- 1,257,257,257,257,257,257,258,258,258,258,258,258,- 1,- 1,- 1,291,- 1,258,- 1,- 1,291,- 1,- 1,- 1,- 1,- 1,258,- 1,- 1,291,291,291,291,291,291,291,291,- 1,258,258,258,258,258,258,291,- 1,- 1,- 1,- 1,258,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,258,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,291,- 1,- 1,- 1,- 1,291,291,- 1,- 1,- 1,291,- 1,- 1,- 1,- 1,- 1,- 1,- 1,291,- 1,- 1,- 1,291,- 1,291,- 1,291,- 1,291,304,304,304,304,304,304,304,304,304,304,- 1,- 1,- 1,- 1,- 1,- 1,- 1,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,- 1,- 1,- 1,- 1,- 1,- 1,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,304,305,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,305,305,305,305,305,305,305,305,305,305,- 1,- 1,- 1,- 1,- 1,- 1,- 1,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,- 1,- 1,- 1,- 1,- 1,- 1,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,305,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 802
int state;int base;int backtrk;
int c;
state=start_state;
# 806
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 810
state=(- state)- 1;}
# 812
while(1){
base=Cyc_Lex_lex_base[_check_known_subscript_notnull(330,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(330,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 820
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 824
c=256;}}else{
# 826
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 829
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(4087,base + c)]== state)
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(4087,base + c)];else{
# 832
state=Cyc_Lex_lex_default[_check_known_subscript_notnull(330,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Lexing_Error_exn_struct _tmp126;_tmp126.tag=Cyc_Lexing_Error;_tmp126.f1=({const char*_tmp127="empty token";_tag_dyneither(_tmp127,sizeof(char),12);});_tmp126;});_tmp125;}));else{
# 838
return lbuf->lex_last_action;}}else{
# 841
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 845
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp128=lexstate;switch(_tmp128){case 0: _LL58: _LL59:
# 804 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 1: _LL5A: _LL5B:
# 806
 return(int)Cyc_Lex_process_id(lexbuf);case 2: _LL5C: _LL5D:
# 809 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 3: _LL5E: _LL5F:
# 812 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 370;case 4: _LL60: _LL61:
# 819 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 336;case 5: _LL62: _LL63:
# 820 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 329;case 6: _LL64: _LL65:
# 821 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 335;case 7: _LL66: _LL67:
# 822 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 334;case 8: _LL68: _LL69:
# 823 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 337;case 9: _LL6A: _LL6B:
# 824 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 338;case 10: _LL6C: _LL6D:
# 825 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 333;case 11: _LL6E: _LL6F:
# 826 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 326;case 12: _LL70: _LL71:
# 827 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 327;case 13: _LL72: _LL73:
# 828 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 328;case 14: _LL74: _LL75:
# 829 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 330;case 15: _LL76: _LL77:
# 830 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 331;case 16: _LL78: _LL79:
# 831 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 332;case 17: _LL7A: _LL7B:
# 835 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 364;case 18: _LL7C: _LL7D:
# 838
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 364;case 19: _LL7E: _LL7F:
# 844 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 364;case 20: _LL80: _LL81:
# 847
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 364;case 21: _LL82: _LL83:
# 852 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 369;case 22: _LL84: _LL85:
# 855
 return 346;case 23: _LL86: _LL87:
# 856 "lex.cyl"
 return 347;case 24: _LL88: _LL89:
# 857 "lex.cyl"
 return 344;case 25: _LL8A: _LL8B:
# 858 "lex.cyl"
 return 345;case 26: _LL8C: _LL8D:
# 859 "lex.cyl"
 return 340;case 27: _LL8E: _LL8F:
# 860 "lex.cyl"
 return 341;case 28: _LL90: _LL91:
# 861 "lex.cyl"
 return 353;case 29: _LL92: _LL93:
# 862 "lex.cyl"
 return 354;case 30: _LL94: _LL95:
# 863 "lex.cyl"
 return 350;case 31: _LL96: _LL97:
# 864 "lex.cyl"
 return 351;case 32: _LL98: _LL99:
# 865 "lex.cyl"
 return 352;case 33: _LL9A: _LL9B:
# 866 "lex.cyl"
 return 359;case 34: _LL9C: _LL9D:
# 867 "lex.cyl"
 return 358;case 35: _LL9E: _LL9F:
# 868 "lex.cyl"
 return 357;case 36: _LLA0: _LLA1:
# 869 "lex.cyl"
 return 355;case 37: _LLA2: _LLA3:
# 870 "lex.cyl"
 return 356;case 38: _LLA4: _LLA5:
# 871 "lex.cyl"
 return 348;case 39: _LLA6: _LLA7:
# 872 "lex.cyl"
 return 349;case 40: _LLA8: _LLA9:
# 873 "lex.cyl"
 return 342;case 41: _LLAA: _LLAB:
# 875
 return 343;case 42: _LLAC: _LLAD:
# 876 "lex.cyl"
 return 361;case 43: _LLAE: _LLAF:
# 877 "lex.cyl"
 return 339;case 44: _LLB0: _LLB1:
# 878 "lex.cyl"
 return 360;case 45: _LLB2: _LLB3:
# 879 "lex.cyl"
 return 362;case 46: _LLB4: _LLB5:
# 881
 return 312;case 47: _LLB6: _LLB7:
# 884 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 48: _LLB8: _LLB9:
# 885 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 49: _LLBA: _LLBB:
# 886 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 50: _LLBC: _LLBD:
# 887 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 51: _LLBE: _LLBF:
# 893 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 365;case 52: _LLC0: _LLC1:
# 901 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 366;case 53: _LLC2: _LLC3:
# 909 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 368;case 54: _LLC4: _LLC5:
# 916
 Cyc_Lex_token_char='\a';return 367;case 55: _LLC6: _LLC7:
# 917 "lex.cyl"
 Cyc_Lex_token_char='\b';return 367;case 56: _LLC8: _LLC9:
# 918 "lex.cyl"
 Cyc_Lex_token_char='\f';return 367;case 57: _LLCA: _LLCB:
# 919 "lex.cyl"
 Cyc_Lex_token_char='\n';return 367;case 58: _LLCC: _LLCD:
# 920 "lex.cyl"
 Cyc_Lex_token_char='\r';return 367;case 59: _LLCE: _LLCF:
# 921 "lex.cyl"
 Cyc_Lex_token_char='\t';return 367;case 60: _LLD0: _LLD1:
# 922 "lex.cyl"
 Cyc_Lex_token_char='\v';return 367;case 61: _LLD2: _LLD3:
# 923 "lex.cyl"
 Cyc_Lex_token_char='\\';return 367;case 62: _LLD4: _LLD5:
# 924 "lex.cyl"
 Cyc_Lex_token_char='\'';return 367;case 63: _LLD6: _LLD7:
# 925 "lex.cyl"
 Cyc_Lex_token_char='"';return 367;case 64: _LLD8: _LLD9:
# 926 "lex.cyl"
 Cyc_Lex_token_char='?';return 367;case 65: _LLDA: _LLDB:
# 929 "lex.cyl"
 Cyc_Lex_token_char=Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,2,1,8),lexbuf);
return 367;case 66: _LLDC: _LLDD:
# 934 "lex.cyl"
 Cyc_Lex_token_char=Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,3,1,16),lexbuf);
return 367;case 67: _LLDE: _LLDF:
# 938
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 367;case 68: _LLE0: _LLE1:
# 942
 return - 1;case 69: _LLE2: _LLE3:
# 944
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LLE4: _LLE5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}_LL57:;}
# 948
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({struct Cyc_Lexing_Error_exn_struct _tmp12A;_tmp12A.tag=Cyc_Lexing_Error;_tmp12A.f1=({const char*_tmp12B="some action didn't return!";_tag_dyneither(_tmp12B,sizeof(char),27);});_tmp12A;});_tmp129;}));}
# 950
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp12C=lexstate;switch(_tmp12C){case 0: _LLE7: _LLE8:
# 947 "lex.cyl"
 return 0;case 1: _LLE9: _LLEA:
# 949
 Cyc_Lex_store_string_char('\a');return 1;case 2: _LLEB: _LLEC:
# 950 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3: _LLED: _LLEE:
# 951 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4: _LLEF: _LLF0:
# 952 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5: _LLF1: _LLF2:
# 953 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6: _LLF3: _LLF4:
# 954 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7: _LLF5: _LLF6:
# 955 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8: _LLF7: _LLF8:
# 956 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9: _LLF9: _LLFA:
# 957 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10: _LLFB: _LLFC:
# 958 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11: _LLFD: _LLFE:
# 959 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12: _LLFF: _LL100:
# 962 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13: _LL101: _LL102:
# 965 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14: _LL103: _LL104:
# 967
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15: _LL105: _LL106:
# 969
 Cyc_Lex_inc_linenumber();Cyc_Lex_runaway_err(({const char*_tmp12D="wide character ends in newline";_tag_dyneither(_tmp12D,sizeof(char),31);}),lexbuf);return 0;case 16: _LL107: _LL108:
# 970 "lex.cyl"
 Cyc_Lex_runaway_err(({const char*_tmp12E="unterminated wide character";_tag_dyneither(_tmp12E,sizeof(char),28);}),lexbuf);return 0;case 17: _LL109: _LL10A:
# 971 "lex.cyl"
 Cyc_Lex_err(({const char*_tmp12F="bad character following backslash in wide character";_tag_dyneither(_tmp12F,sizeof(char),52);}),lexbuf);return 1;default: _LL10B: _LL10C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}_LLE6:;}
# 975
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_Lexing_Error_exn_struct _tmp131;_tmp131.tag=Cyc_Lexing_Error;_tmp131.f1=({const char*_tmp132="some action didn't return!";_tag_dyneither(_tmp132,sizeof(char),27);});_tmp131;});_tmp130;}));}
# 977
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,1);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp133=lexstate;switch(_tmp133){case 0: _LL10E: _LL10F:
# 976 "lex.cyl"
 return 1;case 1: _LL110: _LL111:
# 977 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2: _LL112: _LL113:
# 978 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3: _LL114: _LL115:
# 980
 lexbuf->lex_curr_pos -=1;return 0;default: _LL116: _LL117:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}_LL10D:;}
# 984
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134[0]=({struct Cyc_Lexing_Error_exn_struct _tmp135;_tmp135.tag=Cyc_Lexing_Error;_tmp135.f1=({const char*_tmp136="some action didn't return!";_tag_dyneither(_tmp136,sizeof(char),27);});_tmp135;});_tmp134;}));}
# 986
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,2);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp137=lexstate;switch(_tmp137){case 0: _LL119: _LL11A:
# 984 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1: _LL11B: _LL11C:
# 985 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2: _LL11D: _LL11E:
# 986 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3: _LL11F: _LL120:
# 987 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4: _LL121: _LL122:
# 988 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5: _LL123: _LL124:
# 989 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('n');}else{
# 993
Cyc_Lex_store_string_char('\n');}
return 1;case 6: _LL125: _LL126:
# 996 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7: _LL127: _LL128:
# 997 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('t');}else{
# 1001
Cyc_Lex_store_string_char('\t');}
return 1;case 8: _LL129: _LL12A:
# 1003 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9: _LL12B: _LL12C:
# 1004 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10: _LL12D: _LL12E:
# 1005 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11: _LL12F: _LL130:
# 1006 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12: _LL131: _LL132:
# 1007 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13: _LL133: _LL134:
# 1010 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,1,0,8),lexbuf));
return 1;case 14: _LL135: _LL136:
# 1015 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,2,0,16),lexbuf));
return 1;case 15: _LL137: _LL138:
# 1019
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 16: _LL139: _LL13A:
# 1021 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_runaway_err(({const char*_tmp138="string ends in newline";_tag_dyneither(_tmp138,sizeof(char),23);}),lexbuf);
return 0;case 17: _LL13B: _LL13C:
# 1025 "lex.cyl"
 Cyc_Lex_runaway_err(({const char*_tmp139="unterminated string";_tag_dyneither(_tmp139,sizeof(char),20);}),lexbuf);
return 0;case 18: _LL13D: _LL13E:
# 1028 "lex.cyl"
 Cyc_Lex_err(({const char*_tmp13A="bad character following backslash in string";_tag_dyneither(_tmp13A,sizeof(char),44);}),lexbuf);
return 1;default: _LL13F: _LL140:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}_LL118:;}
# 1033
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B[0]=({struct Cyc_Lexing_Error_exn_struct _tmp13C;_tmp13C.tag=Cyc_Lexing_Error;_tmp13C.f1=({const char*_tmp13D="some action didn't return!";_tag_dyneither(_tmp13D,sizeof(char),27);});_tmp13C;});_tmp13B;}));}
# 1035
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,3);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp13E=lexstate;switch(_tmp13E){case 0: _LL142: _LL143:
# 1038 "lex.cyl"
 Cyc_Lex_store_string(({const char*_tmp13F="\" L\"";_tag_dyneither(_tmp13F,sizeof(char),5);}));return 1;case 1: _LL144: _LL145:
# 1039 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2: _LL146: _LL147:
# 1040 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3: _LL148: _LL149:
# 1042
 lexbuf->lex_curr_pos -=1;return 0;default: _LL14A: _LL14B:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}_LL141:;}
# 1046
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_Lexing_Error_exn_struct _tmp141;_tmp141.tag=Cyc_Lexing_Error;_tmp141.f1=({const char*_tmp142="some action didn't return!";_tag_dyneither(_tmp142,sizeof(char),27);});_tmp141;});_tmp140;}));}
# 1048
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,4);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp143=lexstate;switch(_tmp143){case 0: _LL14D: _LL14E:
# 1045 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1: _LL14F: _LL150:
# 1047
 Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,1));
return 1;case 2: _LL151: _LL152:
# 1053 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LL153: _LL154:
# 1055 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_runaway_err(({const char*_tmp144="string ends in newline";_tag_dyneither(_tmp144,sizeof(char),23);}),lexbuf);
return 0;case 4: _LL155: _LL156:
# 1058 "lex.cyl"
 Cyc_Lex_runaway_err(({const char*_tmp145="unterminated string";_tag_dyneither(_tmp145,sizeof(char),20);}),lexbuf);
return 0;case 5: _LL157: _LL158:
# 1060 "lex.cyl"
 Cyc_Lex_err(({const char*_tmp146="bad character following backslash in string";_tag_dyneither(_tmp146,sizeof(char),44);}),lexbuf);
return 1;default: _LL159: _LL15A:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}_LL14C:;}
# 1065
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Lexing_Error_exn_struct _tmp148;_tmp148.tag=Cyc_Lexing_Error;_tmp148.f1=({const char*_tmp149="some action didn't return!";_tag_dyneither(_tmp149,sizeof(char),27);});_tmp148;});_tmp147;}));}
# 1067
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,5);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp14A=lexstate;switch(_tmp14A){case 0: _LL15C: _LL15D:
# 1063 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LL15E: _LL15F:
# 1064 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2: _LL160: _LL161:
# 1069 "lex.cyl"
 Cyc_Lex_runaway_err(({const char*_tmp14B="unterminated comment";_tag_dyneither(_tmp14B,sizeof(char),21);}),lexbuf);
return 0;case 3: _LL162: _LL163:
# 1072 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4: _LL164: _LL165:
# 1073 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5: _LL166: _LL167:
# 1074 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6: _LL168: _LL169:
# 1075 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LL16A: _LL16B:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}_LL15B:;}
# 1079
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_Lexing_Error_exn_struct _tmp14D;_tmp14D.tag=Cyc_Lexing_Error;_tmp14D.f1=({const char*_tmp14E="some action didn't return!";_tag_dyneither(_tmp14E,sizeof(char),27);});_tmp14D;});_tmp14C;}));}
# 1081
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,6);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp14F=lexstate;switch(_tmp14F){case 0: _LL16D: _LL16E:
# 1082 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 375;case 1: _LL16F: _LL170:
# 1092 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 375;case 2: _LL171: _LL172:
# 1102 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 375;default: _LL173: _LL174:
# 1110
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}_LL16C:;}
# 1113
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Lexing_Error_exn_struct _tmp151;_tmp151.tag=Cyc_Lexing_Error;_tmp151.f1=({const char*_tmp152="some action didn't return!";_tag_dyneither(_tmp152,sizeof(char),27);});_tmp151;});_tmp150;}));}
# 1115
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp153=lexstate;switch(_tmp153){case 0: _LL176: _LL177:
# 1112 "lex.cyl"
 Cyc_Lex_store_string_char('"');
Cyc_Lex_expand_specials=1;
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_expand_specials=0;
Cyc_Lex_store_string_char('"');
return 1;case 1: _LL178: _LL179:
# 1118 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2: _LL17A: _LL17B:
# 1122 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3: _LL17C: _LL17D:
# 1124 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4: _LL17E: _LL17F:
# 1125 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1128
Cyc_Lex_store_string_char(')');
return 1;}case 5: _LL180: _LL181:
# 1131 "lex.cyl"
 Cyc_Lex_runaway_err(({const char*_tmp154="unterminated asm";_tag_dyneither(_tmp154,sizeof(char),17);}),lexbuf);return 0;case 6: _LL182: _LL183:
# 1132 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7: _LL184: _LL185:
# 1133 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LL186: _LL187:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}_LL175:;}
# 1137
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Lexing_Error_exn_struct _tmp156;_tmp156.tag=Cyc_Lexing_Error;_tmp156.f1=({const char*_tmp157="some action didn't return!";_tag_dyneither(_tmp157,sizeof(char),27);});_tmp156;});_tmp155;}));}
# 1139
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,8);}
# 1142 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1147
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
struct Cyc_Xarray_Xarray*symbols=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,({struct _dyneither_ptr*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=(struct _dyneither_ptr)({const char*_tmp159="";_tag_dyneither(_tmp159,sizeof(char),1);});_tmp158;}));
# 1151
((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);
return symbols;}
# 1155
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1157
Cyc_Lex_in_extern_c=0;{
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp15A=Cyc_Lex_prev_extern_c;struct Cyc_List_List*_tmp15B=x;Cyc_Lex_prev_extern_c=_tmp15B;x=_tmp15A;});
while(x != 0){
struct Cyc_List_List*t=((struct Cyc_List_List*)_check_null(x))->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);
x=t;}
# 1166
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp15C=idt;struct Cyc_Lex_DynTrieSymbols*_tmp15D=Cyc_Lex_ids_trie;idt=_tmp15D;Cyc_Lex_ids_trie=_tmp15C;});{
struct Cyc_Lex_DynTrieSymbols _tmp15E=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp15F=_tmp15E;struct Cyc_Core_DynamicRegion*_tmp160;_LL189: _tmp160=_tmp15F.dyn;_LL18A:;
Cyc_Core_free_ukey(_tmp160);
((void(*)(struct Cyc_Lex_DynTrieSymbols*ptr))Cyc_Core_ufree)(idt);};}
# 1173
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp161=tdefs;struct Cyc_Lex_DynTrie*_tmp162=Cyc_Lex_typedefs_trie;tdefs=_tmp162;Cyc_Lex_typedefs_trie=_tmp161;});{
struct Cyc_Lex_DynTrie _tmp163=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmp164=_tmp163;struct Cyc_Core_DynamicRegion*_tmp165;_LL18C: _tmp165=_tmp164.dyn;_LL18D:;
Cyc_Core_free_ukey(_tmp165);
((void(*)(struct Cyc_Lex_DynTrie*ptr))Cyc_Core_ufree)(tdefs);};}{
# 1181
struct Cyc_Core_NewDynamicRegion _tmp166=Cyc_Core_new_ukey();struct Cyc_Core_NewDynamicRegion _tmp167=_tmp166;struct Cyc_Core_DynamicRegion*_tmp176;_LL18F: _tmp176=_tmp167.key;_LL190:;{
struct Cyc_Lex_Trie*_tmp168=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp176,0,Cyc_Lex_empty_trie);
struct Cyc_Xarray_Xarray*_tmp169=((struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Xarray_Xarray*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp176,0,Cyc_Lex_empty_xarray);
Cyc_Lex_ids_trie=({struct Cyc_Lex_DynTrieSymbols*_tmp16A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp16A));_tmp16A->dyn=(struct Cyc_Core_DynamicRegion*)_tmp176;_tmp16A->t=(struct Cyc_Lex_Trie*)_tmp168;_tmp16A->symbols=(struct Cyc_Xarray_Xarray*)_tmp169;_tmp16A;});{
# 1186
struct Cyc_Core_NewDynamicRegion _tmp16B=Cyc_Core_new_ukey();struct Cyc_Core_NewDynamicRegion _tmp16C=_tmp16B;struct Cyc_Core_DynamicRegion*_tmp175;_LL192: _tmp175=_tmp16C.key;_LL193:;{
struct Cyc_Lex_Trie*t=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp175,0,Cyc_Lex_empty_trie);
Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*_tmp16D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp16D));_tmp16D->dyn=(struct Cyc_Core_DynamicRegion*)_tmp175;_tmp16D->t=(struct Cyc_Lex_Trie*)t;_tmp16D;});
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
Cyc_Lex_kw_nums=({unsigned int _tmp16E=(unsigned int)Cyc_Lex_num_kws;struct Cyc_Lex_KeyWordInfo*_tmp16F=(struct Cyc_Lex_KeyWordInfo*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Lex_KeyWordInfo),_tmp16E));struct _dyneither_ptr _tmp172=_tag_dyneither(_tmp16F,sizeof(struct Cyc_Lex_KeyWordInfo),_tmp16E);{unsigned int _tmp170=_tmp16E;unsigned int i;for(i=0;i < _tmp170;i ++){_tmp16F[i]=(struct Cyc_Lex_KeyWordInfo)({struct Cyc_Lex_KeyWordInfo _tmp171;_tmp171.token_index=0;_tmp171.is_c_keyword=0;_tmp171;});}}_tmp172;});{
unsigned int i=0;
unsigned int rwsze=74;
{unsigned int j=0;for(0;j < rwsze;++ j){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){
struct _dyneither_ptr _tmp173=(Cyc_Lex_rw_array[(int)j]).f1;
Cyc_Lex_str_index(_tmp173,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp173));
*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),(int)i))=({struct Cyc_Lex_KeyWordInfo _tmp174;_tmp174.token_index=(int)(Cyc_Lex_rw_array[(int)j]).f2;_tmp174.is_c_keyword=(Cyc_Lex_rw_array[(int)j]).f3;_tmp174;});
++ i;}}}
# 1201
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};};};};};};}

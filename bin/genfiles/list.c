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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 148 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 64
void*Cyc_List_hd(struct Cyc_List_List*x);
# 67
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 100
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 104
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 112
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 117
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x);
# 120
void Cyc_List_app_c(void*(*f)(void*,void*),void*,struct Cyc_List_List*x);
# 124
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 129
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 139
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 141
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 145
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 149
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*,void*accum,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 157
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 167
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 201
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 214
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 220
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 227
struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 230
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);
# 234
struct Cyc_List_List*Cyc_List_imp_merge(int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 254
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 281
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 288
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 298
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x);
# 303
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 310
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 328
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 342
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x);
# 345
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 349
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);
# 354
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 359
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 367
struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r,struct Cyc_List_List*x);
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 374
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dyneither_ptr arr);
# 378
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 397
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*x);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 32 "list.cyc"
int Cyc_List_length(struct Cyc_List_List*x){
int i=0;
# 35
while(x != 0){
++ i;
x=x->tl;}
# 39
return i;}
# 43
void*Cyc_List_hd(struct Cyc_List_List*x){
return x->hd;}
# 48
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x){
return x->tl;}
# 54
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*r,struct _dyneither_ptr argv){
struct Cyc_List_List*result=0;
{unsigned int i=_get_dyneither_size(argv,sizeof(void*))- 1;for(0;i < _get_dyneither_size(argv,sizeof(void*));-- i){
({struct Cyc_List_List*_tmp3C=({struct Cyc_List_List*_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->hd=((void**)argv.curr)[(int)i];_tmp0->tl=result;_tmp0;});result=_tmp3C;});}}
return result;}
# 63
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr argv){
struct Cyc_List_List*result=0;
{int i=(int)(_get_dyneither_size(argv,sizeof(void*))- 1);for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp3D=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=*((void**)_check_dyneither_subscript(argv,sizeof(void*),i));_tmp1->tl=result;_tmp1;});result=_tmp3D;});}}
return result;}
# 71
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*r2,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 74
if(x == 0)return 0;
({struct Cyc_List_List*_tmp3E=({struct Cyc_List_List*_tmp2=_region_malloc(r2,sizeof(*_tmp2));_tmp2->hd=x->hd;_tmp2->tl=0;_tmp2;});result=_tmp3E;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmp3=_region_malloc(r2,sizeof(*_tmp3));_tmp3->hd=x->hd;_tmp3->tl=0;_tmp3;});
prev->tl=temp;
prev=temp;}
# 82
return result;}
# 85
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x){
return Cyc_List_rcopy(Cyc_Core_heap_region,x);}
# 90
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*r2,void*(*f)(void*),struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 93
if(x == 0)return 0;
({struct Cyc_List_List*_tmp40=({struct Cyc_List_List*_tmp4=_region_malloc(r2,sizeof(*_tmp4));({void*_tmp3F=f(x->hd);_tmp4->hd=_tmp3F;});_tmp4->tl=0;_tmp4;});result=_tmp40;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmp5=_region_malloc(r2,sizeof(*_tmp5));({void*_tmp41=f(x->hd);_tmp5->hd=_tmp41;});_tmp5->tl=0;_tmp5;});
prev->tl=temp;
prev=temp;}
# 101
return result;}
# 104
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x){
return Cyc_List_rmap(Cyc_Core_heap_region,f,x);}
# 109
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 112
if(x == 0)return 0;
({struct Cyc_List_List*_tmp43=({struct Cyc_List_List*_tmp6=_region_malloc(r2,sizeof(*_tmp6));({void*_tmp42=f(env,x->hd);_tmp6->hd=_tmp42;});_tmp6->tl=0;_tmp6;});result=_tmp43;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*e=({struct Cyc_List_List*_tmp7=_region_malloc(r2,sizeof(*_tmp7));({void*_tmp44=f(env,x->hd);_tmp7->hd=_tmp44;});_tmp7->tl=0;_tmp7;});
prev->tl=e;
prev=e;}
# 120
return result;}
# 123
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_List_rmap_c(Cyc_Core_heap_region,f,env,x);}char Cyc_List_List_mismatch[14]="List_mismatch";
# 129
struct Cyc_List_List_mismatch_exn_struct Cyc_List_List_mismatch_val={Cyc_List_List_mismatch};
# 134
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*r3,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
# 136
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 138
if(x == 0  && y == 0)return 0;
if(x == 0  || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 141
({struct Cyc_List_List*_tmp46=({struct Cyc_List_List*_tmp9=_region_malloc(r3,sizeof(*_tmp9));({void*_tmp45=f(x->hd,y->hd);_tmp9->hd=_tmp45;});_tmp9->tl=0;_tmp9;});result=_tmp46;});
prev=result;
# 144
x=x->tl;
y=y->tl;
# 147
while(x != 0  && y != 0){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmpA=_region_malloc(r3,sizeof(*_tmpA));({void*_tmp47=f(x->hd,y->hd);_tmpA->hd=_tmp47;});_tmpA->tl=0;_tmpA;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 154
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 158
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}
# 163
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*r3,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 165
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 167
if((x == 0  && y == 0) && z == 0)return 0;
if((x == 0  || y == 0) || z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 170
({struct Cyc_List_List*_tmp49=({struct Cyc_List_List*_tmpB=_region_malloc(r3,sizeof(*_tmpB));({void*_tmp48=f(x->hd,y->hd,z->hd);_tmpB->hd=_tmp48;});_tmpB->tl=0;_tmpB;});result=_tmp49;});
prev=result;
# 173
x=x->tl;
y=y->tl;
z=z->tl;
# 177
while((x != 0  && y != 0) && z != 0){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmpC=_region_malloc(r3,sizeof(*_tmpC));({void*_tmp4A=f(x->hd,y->hd,z->hd);_tmpC->hd=_tmp4A;});_tmpC->tl=0;_tmpC;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 185
if((x != 0  || y != 0) || z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 189
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rmap3(Cyc_Core_heap_region,f,x,y,z);}
# 196
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){
while(x != 0){
f(x->hd);
x=x->tl;}}
# 203
void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0){
f(env,x->hd);
x=x->tl;}}
# 213
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){
while(x != 0){
f(x->hd);
x=x->tl;}}
# 220
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0){
f(env,x->hd);
x=x->tl;}}
# 229
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 235
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 238
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(env,x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 244
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 249
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 255
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 257
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(env,x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 263
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 269
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){
while(x != 0){
({void*_tmp4B=f(accum,x->hd);accum=_tmp4B;});
x=x->tl;}
# 274
return accum;}
# 277
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){
while(x != 0){
({void*_tmp4C=f(env,accum,x->hd);accum=_tmp4C;});
x=x->tl;}
# 282
return accum;}
# 288
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum){
if(x == 0)return accum;else{
return({void*(*_tmp4E)(void*,void*)=f;void*_tmp4D=x->hd;_tmp4E(_tmp4D,Cyc_List_fold_right(f,x->tl,accum));});}}
# 292
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){
if(x == 0)return accum;else{
return({void*(*_tmp51)(void*,void*,void*)=f;void*_tmp50=env;void*_tmp4F=x->hd;_tmp51(_tmp50,_tmp4F,Cyc_List_fold_right_c(f,env,x->tl,accum));});}}
# 299
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 301
while(x != 0){
({struct Cyc_List_List*_tmp52=({struct Cyc_List_List*_tmpD=_region_malloc(r2,sizeof(*_tmpD));_tmpD->hd=x->hd;_tmpD->tl=y;_tmpD;});y=_tmp52;});
x=x->tl;}
# 305
return y;}
# 308
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);}
# 313
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){
# 315
if(x == 0)
return 0;
return Cyc_List_rrevappend(r2,x,0);}
# 320
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x){
return Cyc_List_rrev(Cyc_Core_heap_region,x);}
# 325
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x){
if(x == 0)return x;else{
# 328
struct Cyc_List_List*first=x;
struct Cyc_List_List*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_List_List*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 337
return first;}}
# 342
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 345
if(x == 0)return y;
if(y == 0)return Cyc_List_rcopy(r2,x);
({struct Cyc_List_List*_tmp53=({struct Cyc_List_List*_tmpE=_region_malloc(r2,sizeof(*_tmpE));_tmpE->hd=x->hd;_tmpE->tl=0;_tmpE;});result=_tmp53;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmpF=_region_malloc(r2,sizeof(*_tmpF));_tmpF->hd=x->hd;_tmpF->tl=0;_tmpF;});
prev->tl=temp;
prev=temp;}
# 354
prev->tl=y;
return result;}
# 358
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rappend(Cyc_Core_heap_region,x,y);}
# 365
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*z;
# 368
if(x == 0)return y;
if(y == 0)return x;
for(z=x;z->tl != 0;z=z->tl){
;}
z->tl=y;
return x;}
# 377
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r3,struct Cyc_List_List*x){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_List_List*x,struct Cyc_List_List*accum))Cyc_List_fold_right_c)(Cyc_List_rappend,r3,x,0);}
# 381
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){
return Cyc_List_rflatten(Cyc_Core_heap_region,x);}
# 386
struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 389
if(a == 0)return b;
if(b == 0)return a;
# 396
if(less_eq(a->hd,b->hd)<= 0){
c=a;
a=a->tl;}else{
# 400
c=b;
b=b->tl;}
# 403
d=c;
# 405
while(a != 0  && b != 0){
# 407
if(less_eq(a->hd,b->hd)<= 0){
c->tl=a;
c=a;
a=a->tl;}else{
# 412
c->tl=b;
c=b;
b=b->tl;}}
# 418
if(a == 0)
c->tl=b;else{
# 421
c->tl=a;}
return d;}
# 426
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
if(x == 0)return x;{
struct Cyc_List_List*temp=x->tl;
if(temp == 0)return x;{
# 432
struct Cyc_List_List*a=x;struct Cyc_List_List*aptr=a;
struct Cyc_List_List*b=temp;struct Cyc_List_List*bptr=b;
x=b->tl;
while(x != 0){
aptr->tl=x;
aptr=x;
x=x->tl;
if(x != 0){
bptr->tl=x;
bptr=x;
x=x->tl;}}
# 445
aptr->tl=0;
bptr->tl=0;
return({int(*_tmp55)(void*,void*)=less_eq;struct Cyc_List_List*_tmp54=
Cyc_List_rimp_merge_sort(less_eq,a);
# 447
Cyc_List_imp_merge(_tmp55,_tmp54,
Cyc_List_rimp_merge_sort(less_eq,b));});};};}
# 455
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r2,int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return({int(*_tmp56)(void*,void*)=less_eq;Cyc_List_rimp_merge_sort(_tmp56,Cyc_List_rcopy(r2,x));});}
# 460
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*r3,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 464
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 467
if(a == 0)return Cyc_List_rcopy(r3,b);
if(b == 0)return Cyc_List_rcopy(r3,a);
# 474
if(less_eq(a->hd,b->hd)<= 0){
({struct Cyc_List_List*_tmp57=({struct Cyc_List_List*_tmp10=_region_malloc(r3,sizeof(*_tmp10));_tmp10->hd=a->hd;_tmp10->tl=0;_tmp10;});c=_tmp57;});
a=a->tl;}else{
# 478
({struct Cyc_List_List*_tmp58=({struct Cyc_List_List*_tmp11=_region_malloc(r3,sizeof(*_tmp11));_tmp11->hd=b->hd;_tmp11->tl=0;_tmp11;});c=_tmp58;});
b=b->tl;}
# 481
d=c;
# 483
while(a != 0  && b != 0){
# 485
struct Cyc_List_List*temp;
if(less_eq(a->hd,b->hd)<= 0){
({struct Cyc_List_List*_tmp59=({struct Cyc_List_List*_tmp12=_region_malloc(r3,sizeof(*_tmp12));_tmp12->hd=a->hd;_tmp12->tl=0;_tmp12;});temp=_tmp59;});
c->tl=temp;
c=temp;
a=a->tl;}else{
# 492
({struct Cyc_List_List*_tmp5A=({struct Cyc_List_List*_tmp13=_region_malloc(r3,sizeof(*_tmp13));_tmp13->hd=b->hd;_tmp13->tl=0;_tmp13;});temp=_tmp5A;});
c->tl=temp;
c=temp;
b=b->tl;}}
# 499
if(a == 0)
({struct Cyc_List_List*_tmp5B=Cyc_List_rcopy(r3,b);c->tl=_tmp5B;});else{
# 502
({struct Cyc_List_List*_tmp5C=Cyc_List_rcopy(r3,a);c->tl=_tmp5C;});}
return d;}
# 506
struct Cyc_List_List*Cyc_List_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return Cyc_List_rmerge_sort(Cyc_Core_heap_region,less_eq,x);}
# 510
struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
return Cyc_List_rmerge(Cyc_Core_heap_region,less_eq,a,b);}char Cyc_List_Nth[4]="Nth";
# 516
struct Cyc_List_Nth_exn_struct Cyc_List_Nth_val={Cyc_List_Nth};
# 521
void*Cyc_List_nth(struct Cyc_List_List*x,int i){
# 523
while(i > 0  && x != 0){
-- i;
x=x->tl;}
# 527
if(i < 0  || x == 0)(int)_throw((void*)& Cyc_List_Nth_val);
return x->hd;}
# 533
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i){
if(i < 0)(int)_throw((void*)& Cyc_List_Nth_val);
while(i != 0){
if(x == 0)(int)_throw((void*)& Cyc_List_Nth_val);
x=x->tl;
-- i;}
# 540
return x;}
# 545
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x){
while(x != 0  && pred(x->hd)){x=x->tl;}
return x == 0;}
# 549
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0  && pred(env,x->hd)){x=x->tl;}
return x == 0;}
# 556
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x){
while(x != 0  && !pred(x->hd)){x=x->tl;}
return x != 0;}
# 560
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0  && !pred(env,x->hd)){x=x->tl;}
return x != 0;}
# 565
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
void*v=0;
for(0;x != 0;x=x->tl){
({void*_tmp5D=pred(env,x->hd);v=_tmp5D;});
if((unsigned int)v)break;}
# 571
return v;}struct _tuple2{void*f1;void*f2;};
# 576
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 578
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 580
if(x == 0  && y == 0)return 0;
if(x == 0  || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 583
({struct Cyc_List_List*_tmp5F=({struct Cyc_List_List*_tmp15=_region_malloc(r3,sizeof(*_tmp15));({struct _tuple2*_tmp5E=({struct _tuple2*_tmp16=_region_malloc(r4,sizeof(*_tmp16));_tmp16->f1=x->hd;_tmp16->f2=y->hd;_tmp16;});_tmp15->hd=_tmp5E;});_tmp15->tl=0;_tmp15;});result=_tmp5F;});
prev=result;
# 586
x=x->tl;
y=y->tl;
# 589
while(x != 0  && y != 0){
({struct Cyc_List_List*_tmp61=({struct Cyc_List_List*_tmp17=_region_malloc(r3,sizeof(*_tmp17));({struct _tuple2*_tmp60=({struct _tuple2*_tmp18=_region_malloc(r4,sizeof(*_tmp18));_tmp18->f1=x->hd;_tmp18->f2=y->hd;_tmp18;});_tmp17->hd=_tmp60;});_tmp17->tl=0;_tmp17;});temp=_tmp61;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 596
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 600
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct _tuple3{void*f1;void*f2;void*f3;};
# 604
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 606
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 608
if((x == 0  && y == 0) && z == 0)return 0;
if((x == 0  || y == 0) || z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 611
({struct Cyc_List_List*_tmp63=({struct Cyc_List_List*_tmp19=_region_malloc(r3,sizeof(*_tmp19));({struct _tuple3*_tmp62=({struct _tuple3*_tmp1A=_region_malloc(r4,sizeof(*_tmp1A));_tmp1A->f1=x->hd;_tmp1A->f2=y->hd;_tmp1A->f3=z->hd;_tmp1A;});_tmp19->hd=_tmp62;});_tmp19->tl=0;_tmp19;});result=_tmp63;});
prev=result;
# 614
x=x->tl;
y=y->tl;
z=z->tl;
# 618
while((x != 0  && y != 0) && z != 0){
({struct Cyc_List_List*_tmp65=({struct Cyc_List_List*_tmp1B=_region_malloc(r3,sizeof(*_tmp1B));({struct _tuple3*_tmp64=({struct _tuple3*_tmp1C=_region_malloc(r4,sizeof(*_tmp1C));_tmp1C->f1=x->hd;_tmp1C->f2=y->hd;_tmp1C->f3=z->hd;_tmp1C;});_tmp1B->hd=_tmp64;});_tmp1B->tl=0;_tmp1B;});temp=_tmp65;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 626
if((x != 0  || y != 0) || z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 630
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y,z);}
# 636
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x){
# 638
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
# 641
if(x == 0)return({struct _tuple0 _tmp1D;_tmp1D.f1=0;_tmp1D.f2=0;_tmp1D;});
# 643
({struct Cyc_List_List*_tmp67=({struct Cyc_List_List*_tmp66=({struct Cyc_List_List*_tmp1E=_region_malloc(r3,sizeof(*_tmp1E));_tmp1E->hd=(((struct _tuple2*)x->hd)[0]).f1;_tmp1E->tl=0;_tmp1E;});result1=_tmp66;});prev1=_tmp67;});
({struct Cyc_List_List*_tmp69=({struct Cyc_List_List*_tmp68=({struct Cyc_List_List*_tmp1F=_region_malloc(r4,sizeof(*_tmp1F));_tmp1F->hd=(((struct _tuple2*)x->hd)[0]).f2;_tmp1F->tl=0;_tmp1F;});result2=_tmp68;});prev2=_tmp69;});
# 646
for(x=x->tl;x != 0;x=x->tl){
({struct Cyc_List_List*_tmp6A=({struct Cyc_List_List*_tmp20=_region_malloc(r3,sizeof(*_tmp20));_tmp20->hd=(((struct _tuple2*)x->hd)[0]).f1;_tmp20->tl=0;_tmp20;});temp1=_tmp6A;});
({struct Cyc_List_List*_tmp6B=({struct Cyc_List_List*_tmp21=_region_malloc(r4,sizeof(*_tmp21));_tmp21->hd=(((struct _tuple2*)x->hd)[0]).f2;_tmp21->tl=0;_tmp21;});temp2=_tmp6B;});
prev1->tl=temp1;
prev2->tl=temp2;
prev1=temp1;
prev2=temp2;}
# 654
return({struct _tuple0 _tmp22;_tmp22.f1=result1;_tmp22.f2=result2;_tmp22;});}
# 657
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x){
return Cyc_List_rsplit(Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 663
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x){
# 666
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
struct Cyc_List_List*result3;struct Cyc_List_List*prev3;struct Cyc_List_List*temp3;
# 670
if(x == 0)return({struct _tuple1 _tmp23;_tmp23.f1=0;_tmp23.f2=0;_tmp23.f3=0;_tmp23;});
# 672
({struct Cyc_List_List*_tmp6D=({struct Cyc_List_List*_tmp6C=({struct Cyc_List_List*_tmp24=_region_malloc(r3,sizeof(*_tmp24));_tmp24->hd=(((struct _tuple3*)x->hd)[0]).f1;_tmp24->tl=0;_tmp24;});result1=_tmp6C;});prev1=_tmp6D;});
({struct Cyc_List_List*_tmp6F=({struct Cyc_List_List*_tmp6E=({struct Cyc_List_List*_tmp25=_region_malloc(r4,sizeof(*_tmp25));_tmp25->hd=(((struct _tuple3*)x->hd)[0]).f2;_tmp25->tl=0;_tmp25;});result2=_tmp6E;});prev2=_tmp6F;});
({struct Cyc_List_List*_tmp71=({struct Cyc_List_List*_tmp70=({struct Cyc_List_List*_tmp26=_region_malloc(r5,sizeof(*_tmp26));_tmp26->hd=(((struct _tuple3*)x->hd)[0]).f3;_tmp26->tl=0;_tmp26;});result3=_tmp70;});prev3=_tmp71;});
# 676
for(x=x->tl;x != 0;x=x->tl){
({struct Cyc_List_List*_tmp72=({struct Cyc_List_List*_tmp27=_region_malloc(r3,sizeof(*_tmp27));_tmp27->hd=(((struct _tuple3*)x->hd)[0]).f1;_tmp27->tl=0;_tmp27;});temp1=_tmp72;});
({struct Cyc_List_List*_tmp73=({struct Cyc_List_List*_tmp28=_region_malloc(r4,sizeof(*_tmp28));_tmp28->hd=(((struct _tuple3*)x->hd)[0]).f2;_tmp28->tl=0;_tmp28;});temp2=_tmp73;});
({struct Cyc_List_List*_tmp74=({struct Cyc_List_List*_tmp29=_region_malloc(r5,sizeof(*_tmp29));_tmp29->hd=(((struct _tuple3*)x->hd)[0]).f3;_tmp29->tl=0;_tmp29;});temp3=_tmp74;});
prev1->tl=temp1;
prev2->tl=temp2;
prev3->tl=temp3;
prev1=temp1;
prev2=temp2;
prev3=temp3;}
# 687
return({struct _tuple1 _tmp2A;_tmp2A.f1=result1;_tmp2A.f2=result2;_tmp2A.f3=result3;_tmp2A;});}
# 690
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){
return Cyc_List_rsplit3(Cyc_Core_heap_region,Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 697
int Cyc_List_memq(struct Cyc_List_List*l,void*x){
while(l != 0){
if(l->hd == x)return 1;
l=l->tl;}
# 702
return 0;}
# 705
int Cyc_List_mem(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
while(l != 0){
if(cmp(l->hd,x)== 0)return 1;
l=l->tl;}
# 710
return 0;}
# 719
void*Cyc_List_assoc(struct Cyc_List_List*l,void*x){
while(l != 0){
if((((struct _tuple2*)l->hd)[0]).f1 == x)return(((struct _tuple2*)l->hd)[0]).f2;
l=l->tl;}
# 724
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 727
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
while(l != 0){
if(cmp((*((struct _tuple2*)l->hd)).f1,x)== 0)return(*((struct _tuple2*)l->hd)).f2;
l=l->tl;}
# 732
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 739
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
struct Cyc_List_List*prev=0;
struct Cyc_List_List*_tmp2B=l;
while(_tmp2B != 0){
if(cmp(_tmp2B->hd,x)== 0){
if(prev == 0)
return((struct Cyc_List_List*)_check_null(l))->tl;else{
# 747
prev->tl=_tmp2B->tl;
return l;}}
# 751
prev=_tmp2B;
_tmp2B=_tmp2B->tl;}
# 754
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 757
static int Cyc_List_ptrequal(void*x,void*y){
return !(x == y);}
# 764
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x){
return Cyc_List_delete_cmp(Cyc_List_ptrequal,l,x);}
# 770
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){
while(l != 0){
if((((struct _tuple2*)l->hd)[0]).f1 == x)return 1;
l=l->tl;}
# 775
return 0;}
# 780
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
while(l != 0){
if(cmp((((struct _tuple2*)l->hd)[0]).f1,x)== 0)return 1;
l=l->tl;}
# 785
return 0;}
# 791
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x){
while(x != 0){
struct Cyc_List_List*_tmp2C=x->tl;
if(_tmp2C != 0){
if(cmp(x->hd,_tmp2C->hd)== 0)return({struct Cyc_Core_Opt*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->v=x->hd;_tmp2D;});}
x=_tmp2C;}
# 798
return 0;}
# 802
struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x){
int s;
struct _dyneither_ptr arr;
# 806
({int _tmp75=Cyc_List_length(x);s=_tmp75;});
({struct _dyneither_ptr _tmp78=({unsigned int _tmp2E=(unsigned int)s;void**_tmp2F=(void**)({struct _RegionHandle*_tmp76=r2;_region_malloc(_tmp76,_check_times(sizeof(void*),_tmp2E));});struct _dyneither_ptr _tmp32=_tag_dyneither(_tmp2F,sizeof(void*),_tmp2E);{unsigned int _tmp30=_tmp2E;unsigned int i;for(i=0;i < _tmp30;i ++){({typeof((void*)({void*_tmp31=((struct Cyc_List_List*)_check_null(x))->hd;x=x->tl;_tmp31;}))_tmp77=(void*)({void*_tmp31=((struct Cyc_List_List*)_check_null(x))->hd;x=x->tl;_tmp31;});_tmp2F[i]=_tmp77;});}}_tmp32;});arr=_tmp78;});
# 810
return arr;}
# 813
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x){
return Cyc_List_rto_array(Cyc_Core_heap_region,x);}
# 818
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dyneither_ptr arr){
struct Cyc_List_List*ans=0;
{int i=(int)(_get_dyneither_size(arr,sizeof(void*))- 1);for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp79=({struct Cyc_List_List*_tmp33=_region_malloc(r2,sizeof(*_tmp33));_tmp33->hd=*((void**)_check_dyneither_subscript(arr,sizeof(void*),i));_tmp33->tl=ans;_tmp33;});ans=_tmp79;});}}
return ans;}
# 825
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr){
return Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);}
# 829
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
({struct Cyc_List_List*_tmp7B=({struct Cyc_List_List*_tmp34=_region_malloc(r,sizeof(*_tmp34));({void*_tmp7A=f(i);_tmp34->hd=_tmp7A;});_tmp34->tl=res;_tmp34;});res=_tmp7B;});}}
# 834
return Cyc_List_imp_rev(res);}
# 837
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int)){
return Cyc_List_rtabulate(Cyc_Core_heap_region,n,f);}
# 841
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
({struct Cyc_List_List*_tmp7D=({struct Cyc_List_List*_tmp35=_region_malloc(r,sizeof(*_tmp35));({void*_tmp7C=f(env,i);_tmp35->hd=_tmp7C;});_tmp35->tl=res;_tmp35;});res=_tmp7D;});}}
# 846
return Cyc_List_imp_rev(res);}
# 849
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){
return Cyc_List_rtabulate_c(Cyc_Core_heap_region,n,f,env);}
# 853
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 856
if((unsigned int)l1 == (unsigned int)l2)
return 0;{
int _tmp36=cmp(l1->hd,l2->hd);
if(_tmp36 != 0)
return _tmp36;};}
# 862
if(l1 != 0)
return 1;
if(l2 != 0)
return - 1;
return 0;}
# 870
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 874
if((unsigned int)l1 == (unsigned int)l2)
return 1;{
int _tmp37=cmp(l1->hd,l2->hd);
if(_tmp37 != 0)
return 0;};}
# 880
return l1 == 0;}
# 883
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
if(l == 0)
return 0;{
# 887
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp39=_region_malloc(r,sizeof(*_tmp39));_tmp39->hd=l->hd;_tmp39->tl=0;_tmp39;});
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(0;l != 0;l=l->tl){
if(f(env,l->hd)){
({struct Cyc_List_List*_tmp7E=({struct Cyc_List_List*_tmp38=_region_malloc(r,sizeof(*_tmp38));_tmp38->hd=l->hd;_tmp38->tl=0;_tmp38;});temp=_tmp7E;});
end->tl=temp;
end=temp;}}
# 897
return result->tl;};}
# 900
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
return Cyc_List_rfilter_c(Cyc_Core_heap_region,f,env,l);}
# 904
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){
if(l == 0)
return 0;{
# 908
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp3B=_region_malloc(r,sizeof(*_tmp3B));_tmp3B->hd=l->hd;_tmp3B->tl=0;_tmp3B;});
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(0;l != 0;l=l->tl){
if(f(l->hd)){
({struct Cyc_List_List*_tmp7F=({struct Cyc_List_List*_tmp3A=_region_malloc(r,sizeof(*_tmp3A));_tmp3A->hd=l->hd;_tmp3A->tl=0;_tmp3A;});temp=_tmp7F;});
end->tl=temp;
end=temp;}}
# 918
return result->tl;};}
# 921
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){
return Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}

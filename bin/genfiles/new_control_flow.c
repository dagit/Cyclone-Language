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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 265 "core.h"
void Cyc_Core_rethrow(void*);
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 977 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1159
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1161
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1173
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1177
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1179
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 90
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 163
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 233 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 265
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 326
int Cyc_Tcutil_is_noreturn(void*);
# 344
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 63
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 67
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 99
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 102
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 107
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 111
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 120
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 29 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 44 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 46
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
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
# 158
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 166
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 169
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 171
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 191 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
# 194
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 198
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple16 pr1,struct _tuple16 pr2);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 70
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 78
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 88
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 90
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 98
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 100
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 52 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
# 55
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};
# 62
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
# 96 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 104
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 106
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 113
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 118
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp5;}else{_LL3: _LL4:
({void*_tmp655=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp654=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;({struct Cyc_NewControlFlow_CFStmtAnnot _tmp653=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3.f1=_tmp653;});_tmp3;});_tmp2[0]=_tmp654;});_tmp2;});s->annot=_tmp655;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 125
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_region_malloc(env->r,sizeof(*_tmp6));({union Cyc_CfFlowInfo_FlowInfo _tmp656=Cyc_CfFlowInfo_BottomFL();_tmp6[0]=_tmp656;});_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 132
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 135
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 143
({union Cyc_CfFlowInfo_FlowInfo _tmp657=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp657;});
# 149
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp9;_tmp9.f1=_tmp7;_tmp9.f2=_tmp8;_tmp9;});}
# 159
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 167
({union Cyc_CfFlowInfo_FlowInfo _tmp658=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp658;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 174
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 176
struct _tuple21*_tmpA=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp14;unsigned int _tmp13;struct Cyc_Dict_Dict _tmp12;_LL6: _tmp14=_tmpA->f1;_tmp13=_tmpA->f2;_tmp12=_tmpA->f3;_LL7:;{
void*_tmpB=root;struct Cyc_Absyn_Vardecl*_tmp11;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->tag == 0){_LL9: _tmp11=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->f1;_LLA:
# 179
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp11->type)){
retry: {void*_tmpC=rval;void*_tmp10;switch(*((int*)_tmpC)){case 8: _LLE: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC)->f2;_LLF:
 rval=_tmp10;goto retry;case 7: _LL10: _LL11:
 goto _LL13;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL12: _LL13:
 goto _LL15;}else{goto _LL16;}case 0: _LL14: _LL15:
 goto _LLD;default: _LL16: _LL17:
# 188
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;({struct _dyneither_ptr _tmp659=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));_tmpF.f1=_tmp659;});({void*_tmpD[1]={& _tmpF};({unsigned int _tmp65B=_tmp13;struct _dyneither_ptr _tmp65A=({const char*_tmpE="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpE,sizeof(char),64);});Cyc_Tcutil_warn(_tmp65B,_tmp65A,_tag_dyneither(_tmpD,sizeof(void*),1));});});});
goto _LLD;}_LLD:;}}
# 192
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}
# 200
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp15=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp16=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp17=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp16);
# 206
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp18=({struct _tuple19 _tmp1E;_tmp1E.f1=flow;_tmp1E.f2=_tmp17;_tmp1E;});struct _tuple19 _tmp19=_tmp18;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1C;if(((_tmp19.f1).ReachableFL).tag == 2){if(((_tmp19.f2).ReachableFL).tag == 2){_LL19: _tmp1D=(((_tmp19.f1).ReachableFL).val).f1;_tmp1C=(((_tmp19.f2).ReachableFL).val).f1;_LL1A: {
# 209
struct _tuple21 _tmp1A=({struct _tuple21 _tmp1B;_tmp1B.f1=env;_tmp1B.f2=s->loc;_tmp1B.f3=_tmp1C;_tmp1B;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1A,_tmp1D);
goto _LL18;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 215
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp17,*_tmp16)){
# 222
*_tmp16=_tmp17;
# 226
if(_tmp15->visited == env->iteration_num)
# 228
env->iterate_again=1;}}
# 233
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 237
union Cyc_CfFlowInfo_FlowInfo _tmp1F=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp26;if((_tmp1F.BottomFL).tag == 1){_LL1E: _LL1F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL20: _tmp27=((_tmp1F.ReachableFL).val).f1;_tmp26=((_tmp1F.ReachableFL).val).f2;_LL21:
# 240
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=_region_malloc(fenv->r,sizeof(*_tmp24));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp65C=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25;});_tmp24[0]=_tmp65C;});_tmp24;});
void*_tmp21=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp65E=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp22=_region_malloc(fenv->r,sizeof(*_tmp22));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp65D=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp23;_tmp23.tag=8;_tmp23.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp23.f2=_tmp21;_tmp23;});_tmp22[0]=_tmp65D;});_tmp22;});_tmp21=_tmp65E;});
# 247
({struct Cyc_Dict_Dict _tmp65F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp20,_tmp21);_tmp27=_tmp65F;});}
# 249
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp26);}_LL1D:;}
# 253
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 260
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp28=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp29=_region_malloc(r,sizeof(*_tmp29));_tmp29->hd=_tmp28;_tmp29->tl=assume;_tmp29;})))
return 0;}
# 265
return 1;}
# 268
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2A=e->r;void*_tmp2B=_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->tag == 13){_LL23: _tmp2C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->f2;_LL24:
 return _tmp2C;}else{_LL25: _LL26:
 return e;}_LL22:;}
# 275
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 277
union Cyc_CfFlowInfo_FlowInfo _tmp2D=inflow;struct Cyc_List_List*_tmp4B;if((_tmp2D.BottomFL).tag == 1){_LL28: _LL29:
 return;}else{_LL2A: _tmp4B=((_tmp2D.ReachableFL).val).f2;_LL2B:
# 280
{void*_tmp2E=Cyc_Tcutil_compress(t);void*_tmp2F=_tmp2E;struct Cyc_List_List*_tmp4A;union Cyc_Absyn_AggrInfoU _tmp49;struct Cyc_List_List*_tmp48;switch(*((int*)_tmp2F)){case 11: _LL2D: _tmp49=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).aggr_info;_tmp48=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).targs;_LL2E: {
# 282
struct Cyc_Absyn_Aggrdecl*_tmp30=Cyc_Absyn_get_known_aggrdecl(_tmp49);
struct Cyc_Absyn_Aggrfield*_tmp31=Cyc_Absyn_lookup_decl_field(_tmp30,f);
struct Cyc_Absyn_Exp*_tmp32=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31))->requires_clause;
if(_tmp32 != 0){
struct _RegionHandle _tmp33=_new_region("temp");struct _RegionHandle*temp=& _tmp33;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp34=({struct _RegionHandle*_tmp661=temp;struct Cyc_List_List*_tmp660=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp30->tvs,_tmp48);Cyc_Tcutil_rsubsexp(_tmp661,_tmp660,_tmp32);});
# 289
if(!({struct _RegionHandle*_tmp663=r;struct Cyc_List_List*_tmp662=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp663,_tmp662,Cyc_Relations_exp2relns(temp,_tmp34));})){
({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp37;_tmp37.tag=0;({struct _dyneither_ptr _tmp664=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp34)));_tmp37.f1=_tmp664;});({void*_tmp35[2]={& _tmp37,& _tmp38};({unsigned int _tmp666=loc;struct _dyneither_ptr _tmp665=({const char*_tmp36="unable to prove %s, required to access %s";_tag_dyneither(_tmp36,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp666,_tmp665,_tag_dyneither(_tmp35,sizeof(void*),2));});});});});
({void*_tmp39=0;({struct Cyc___cycFILE*_tmp668=Cyc_stderr;struct _dyneither_ptr _tmp667=({const char*_tmp3A="  [recorded facts on path: ";_tag_dyneither(_tmp3A,sizeof(char),28);});Cyc_fprintf(_tmp668,_tmp667,_tag_dyneither(_tmp39,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp3B=0;({struct Cyc___cycFILE*_tmp66A=Cyc_stderr;struct _dyneither_ptr _tmp669=({const char*_tmp3C="]\n";_tag_dyneither(_tmp3C,sizeof(char),3);});Cyc_fprintf(_tmp66A,_tmp669,_tag_dyneither(_tmp3B,sizeof(void*),0));});});}}
# 287
;_pop_region(temp);}
# 296
goto _LL2C;}case 12: _LL2F: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LL30: {
# 298
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_field(_tmp4A,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
if(!({struct _RegionHandle*_tmp66C=r;struct Cyc_List_List*_tmp66B=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp66C,_tmp66B,Cyc_Relations_exp2relns(temp,_tmp3E));})){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0;({struct _dyneither_ptr _tmp66D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3E)));_tmp42.f1=_tmp66D;});({void*_tmp40[2]={& _tmp42,& _tmp43};({unsigned int _tmp66F=loc;struct _dyneither_ptr _tmp66E=({const char*_tmp41="unable to prove %s, required to access %s";_tag_dyneither(_tmp41,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp66F,_tmp66E,_tag_dyneither(_tmp40,sizeof(void*),2));});});});});
({void*_tmp44=0;({struct Cyc___cycFILE*_tmp671=Cyc_stderr;struct _dyneither_ptr _tmp670=({const char*_tmp45="  [recorded facts on path: ";_tag_dyneither(_tmp45,sizeof(char),28);});Cyc_fprintf(_tmp671,_tmp670,_tag_dyneither(_tmp44,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp46=0;({struct Cyc___cycFILE*_tmp673=Cyc_stderr;struct _dyneither_ptr _tmp672=({const char*_tmp47="]\n";_tag_dyneither(_tmp47,sizeof(char),3);});Cyc_fprintf(_tmp673,_tmp672,_tag_dyneither(_tmp46,sizeof(void*),0));});});}
# 302
;_pop_region(temp);}
# 309
goto _LL2C;}default: _LL31: _LL32:
 goto _LL2C;}_LL2C:;}
# 312
goto _LL27;}_LL27:;}
# 316
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4C=inflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*_tmp51;if((_tmp4C.BottomFL).tag == 1){_LL34: _LL35:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL36: _tmp52=((_tmp4C.ReachableFL).val).f1;_tmp51=((_tmp4C.ReachableFL).val).f2;_LL37:
# 320
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp52,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D=0;({unsigned int _tmp675=loc;struct _dyneither_ptr _tmp674=({const char*_tmp4E="expression may not be fully initialized";_tag_dyneither(_tmp4E,sizeof(char),40);});Cyc_CfFlowInfo_aerr(_tmp675,_tmp674,_tag_dyneither(_tmp4D,sizeof(void*),0));});});{
struct Cyc_Dict_Dict _tmp4F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp52,r);
if(_tmp52.t == _tmp4F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp50=Cyc_CfFlowInfo_ReachableFL(_tmp4F,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp50);
return _tmp50;};};}_LL33:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 330
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp53=({struct _tuple0 _tmp66;({void*_tmp676=Cyc_Tcutil_compress(t);_tmp66.f1=_tmp676;});_tmp66.f2=r;_tmp66;});struct _tuple0 _tmp54=_tmp53;enum Cyc_Absyn_AggrKind _tmp65;struct Cyc_List_List*_tmp64;struct _dyneither_ptr _tmp63;union Cyc_Absyn_AggrInfoU _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_Absyn_Datatypefield*_tmp5D;struct _dyneither_ptr _tmp5C;void*_tmp5B;switch(*((int*)_tmp54.f2)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL39: _LL3A:
 return;}else{switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL4B;else{goto _LL4B;}case 10: goto _LL4B;case 11: goto _LL4B;case 12: goto _LL4B;case 5: goto _LL49;default: goto _LL4B;}}case 0: _LL3B: _LL3C:
 return;case 7: _LL3D: _LL3E:
 return;case 8: _LL3F: _tmp5B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL40:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp5B);return;default: switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL41: _tmp5D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL42:
# 337
 if(_tmp5D->typs == 0)
return;
_tmp5F=_tmp5D->typs;_tmp5E=_tmp5C;goto _LL44;}else{goto _LL4B;}}else{goto _LL4B;}case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL43: _tmp5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL44: {
# 341
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5F);
{int i=0;for(0;i < sz;(i ++,_tmp5F=_tmp5F->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp679=env;unsigned int _tmp678=loc;void*_tmp677=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp679,_tmp678,_tmp677,*((void**)_check_dyneither_subscript(_tmp5E,sizeof(void*),i)));});}}
# 345
return;}}else{goto _LL4B;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL45: _tmp62=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).aggr_info;_tmp61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).targs;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL46: {
# 348
struct Cyc_Absyn_Aggrdecl*_tmp55=Cyc_Absyn_get_known_aggrdecl(_tmp62);
if(_tmp55->impl == 0)return;{
struct Cyc_List_List*_tmp56=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55->impl))->fields;
struct _RegionHandle _tmp57=_new_region("temp");struct _RegionHandle*temp=& _tmp57;_push_region(temp);
{struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp55->tvs,_tmp61);
{int i=0;for(0;i < _get_dyneither_size(_tmp60,sizeof(void*));(i ++,_tmp56=_tmp56->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp56))->hd)->type;
if(_tmp58 != 0)({void*_tmp67A=Cyc_Tcutil_rsubstitute(temp,_tmp58,t);t=_tmp67A;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp60.curr)[i]);}}
# 358
_npop_handler(0);return;}
# 352
;_pop_region(temp);};}}else{goto _LL4B;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL47: _tmp65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f2;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL48:
# 361
{int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(void*));(i ++,_tmp64=_tmp64->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp64))->hd)->type,((void**)_tmp63.curr)[i]);}}
return;}else{goto _LL4B;}case 5: _LL49: if(
Cyc_Tcutil_is_noalias_pointer(t,0)){_LL4A:
({void*_tmp59=0;({unsigned int _tmp67C=loc;struct _dyneither_ptr _tmp67B=({const char*_tmp5A="argument may still contain alias-free pointers";_tag_dyneither(_tmp5A,sizeof(char),47);});Cyc_Tcutil_warn(_tmp67C,_tmp67B,_tag_dyneither(_tmp59,sizeof(void*),0));});});
return;}else{goto _LL4B;}default: _LL4B: _LL4C:
 return;}}_LL38:;}
# 371
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp67=inflow;struct Cyc_Dict_Dict _tmp73;struct Cyc_List_List*_tmp72;if((_tmp67.BottomFL).tag == 1){_LL4E: _LL4F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL50: _tmp73=((_tmp67.ReachableFL).val).f1;_tmp72=((_tmp67.ReachableFL).val).f2;_LL51:
# 375
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp68=0;({struct _dyneither_ptr _tmp67D=({const char*_tmp69="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp69,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp67D,_tag_dyneither(_tmp68,sizeof(void*),0));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 379
void*_tmp6A=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6B=r;struct Cyc_CfFlowInfo_Place*_tmp6F;void*_tmp6E;switch(*((int*)_tmp6B)){case 8: _LL53: _tmp6E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f2;_LL54:
 r=_tmp6E;goto retry;case 5: _LL55: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL56:
# 383
({struct Cyc_NewControlFlow_AnalEnv*_tmp680=env;unsigned int _tmp67F=loc;void*_tmp67E=_tmp6A;Cyc_NewControlFlow_check_nounique(_tmp680,_tmp67F,_tmp67E,Cyc_CfFlowInfo_lookup_place(_tmp73,_tmp6F));});
goto _LL52;default: _LL57: _LL58:
# 386
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6A))
({void*_tmp6C=0;({unsigned int _tmp682=loc;struct _dyneither_ptr _tmp681=({const char*_tmp6D="argument may contain live alias-free pointers";_tag_dyneither(_tmp6D,sizeof(char),46);});Cyc_Tcutil_warn(_tmp682,_tmp681,_tag_dyneither(_tmp6C,sizeof(void*),0));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL52:;}{
# 390
struct Cyc_Dict_Dict _tmp70=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp73,r);
if(_tmp73.t == _tmp70.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp71=Cyc_CfFlowInfo_ReachableFL(_tmp70,_tmp72);
Cyc_NewControlFlow_update_tryflow(env,_tmp71);
return _tmp71;};};};}_LL4D:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 400
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 407
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp74;_tmp74.f1=inflow;_tmp74.f2=0;_tmp74;});
# 412
do{
struct _tuple16 _tmp75=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp76=_tmp75;union Cyc_CfFlowInfo_FlowInfo _tmp79;void*_tmp78;_LL5A: _tmp79=_tmp76.f1;_tmp78=_tmp76.f2;_LL5B:;
inflow=_tmp79;
({struct Cyc_List_List*_tmp683=({struct Cyc_List_List*_tmp77=_region_malloc(rgn,sizeof(*_tmp77));_tmp77->hd=_tmp78;_tmp77->tl=rvals;_tmp77;});rvals=_tmp683;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 420
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp7A;_tmp7A.f1=inflow;({struct Cyc_List_List*_tmp684=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp7A.f2=_tmp684;});_tmp7A;});}
# 426
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 429
struct _RegionHandle _tmp7B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7B;_push_region(rgn);
{struct _tuple23 _tmp7C=
Cyc_NewControlFlow_anal_Rexps(rgn,env,inflow,es,0,0);
# 430
struct _tuple23 _tmp7D=_tmp7C;union Cyc_CfFlowInfo_FlowInfo _tmp85;struct Cyc_List_List*_tmp84;_LL5D: _tmp85=_tmp7D.f1;_tmp84=_tmp7D.f2;_LL5E:;
# 432
{union Cyc_CfFlowInfo_FlowInfo _tmp7E=_tmp85;struct Cyc_Dict_Dict _tmp81;if((_tmp7E.ReachableFL).tag == 2){_LL60: _tmp81=((_tmp7E.ReachableFL).val).f1;_LL61:
# 434
 for(0;_tmp84 != 0;(_tmp84=_tmp84->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp81,(void*)_tmp84->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp7F=0;({unsigned int _tmp686=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp685=({const char*_tmp80="expression may not be initialized";_tag_dyneither(_tmp80,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp686,_tmp685,_tag_dyneither(_tmp7F,sizeof(void*),0));});});}
goto _LL5F;}else{_LL62: _LL63:
 goto _LL5F;}_LL5F:;}{
# 440
struct _tuple16 _tmp83=({struct _tuple16 _tmp82;_tmp82.f1=_tmp85;_tmp82.f2=(env->fenv)->unknown_all;_tmp82;});_npop_handler(0);return _tmp83;};}
# 430
;_pop_region(rgn);}
# 448
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 455
int needs_unconsume=0;
void*_tmp86=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp86,& needs_unconsume))
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Core_Impossible_exn_struct _tmp688=({struct Cyc_Core_Impossible_exn_struct _tmp88;_tmp88.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp687=({const char*_tmp89="consume_zero_ral";_tag_dyneither(_tmp89,sizeof(char),17);});_tmp88.f1=_tmp687;});_tmp88;});_tmp87[0]=_tmp688;});_tmp87;}));else{
# 460
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 464
return new_rval;}}}
# 477 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 483
union Cyc_CfFlowInfo_FlowInfo _tmp8A=outflow;struct Cyc_Dict_Dict _tmp92;struct Cyc_List_List*_tmp91;if((_tmp8A.BottomFL).tag == 1){_LL65: _LL66:
 return outflow;}else{_LL67: _tmp92=((_tmp8A.ReachableFL).val).f1;_tmp91=((_tmp8A.ReachableFL).val).f2;_LL68: {
# 486
union Cyc_CfFlowInfo_AbsLVal _tmp8B=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp8C=_tmp8B;struct Cyc_CfFlowInfo_Place*_tmp90;if((_tmp8C.UnknownL).tag == 2){_LL6A: _LL6B:
# 490
 return outflow;}else{_LL6C: _tmp90=(_tmp8C.PlaceL).val;_LL6D: {
# 494
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
({void*_tmp68A=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8D=_region_malloc((env->fenv)->r,sizeof(*_tmp8D));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp689=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8E;_tmp8E.tag=8;_tmp8E.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp8E.f2=nzval;_tmp8E;});_tmp8D[0]=_tmp689;});_tmp8D;});nzval=_tmp68A;});}
# 498
({void*_tmp68B=Cyc_NewControlFlow_consume_zero_rval(env,_tmp92,_tmp90,e,nzval);nzval=_tmp68B;});{
union Cyc_CfFlowInfo_FlowInfo _tmp8F=({struct Cyc_Dict_Dict _tmp68C=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp92,_tmp90,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp68C,_tmp91);});
# 503
return _tmp8F;};}}_LL69:;}}_LL64:;}
# 512
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 515
union Cyc_CfFlowInfo_FlowInfo _tmp93=outflow;struct Cyc_Dict_Dict _tmp9F;struct Cyc_List_List*_tmp9E;if((_tmp93.BottomFL).tag == 1){_LL6F: _LL70:
 return({struct _tuple19 _tmp94;_tmp94.f1=outflow;_tmp94.f2=outflow;_tmp94;});}else{_LL71: _tmp9F=((_tmp93.ReachableFL).val).f1;_tmp9E=((_tmp93.ReachableFL).val).f2;_LL72: {
# 518
union Cyc_CfFlowInfo_AbsLVal _tmp95=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp96=_tmp95;struct Cyc_CfFlowInfo_Place*_tmp9D;if((_tmp96.UnknownL).tag == 2){_LL74: _LL75:
 return({struct _tuple19 _tmp97;_tmp97.f1=outflow;_tmp97.f2=outflow;_tmp97;});}else{_LL76: _tmp9D=(_tmp96.PlaceL).val;_LL77: {
# 521
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp68E=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp98=_region_malloc((env->fenv)->r,sizeof(*_tmp98));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp68D=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp99;_tmp99.tag=8;_tmp99.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp99.f2=nzval;_tmp99;});_tmp98[0]=_tmp68D;});_tmp98;});nzval=_tmp68E;});
({void*_tmp690=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9A=_region_malloc((env->fenv)->r,sizeof(*_tmp9A));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp68F=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9B;_tmp9B.tag=8;_tmp9B.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9B.f2=zval;_tmp9B;});_tmp9A[0]=_tmp68F;});_tmp9A;});zval=_tmp690;});}
# 527
({void*_tmp691=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9F,_tmp9D,e,nzval);nzval=_tmp691;});
({void*_tmp692=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9F,_tmp9D,e,zval);zval=_tmp692;});
return({struct _tuple19 _tmp9C;({union Cyc_CfFlowInfo_FlowInfo _tmp694=({
struct Cyc_Dict_Dict _tmp693=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9F,_tmp9D,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp693,_tmp9E);});_tmp9C.f1=_tmp694;});({union Cyc_CfFlowInfo_FlowInfo _tmp696=({
struct Cyc_Dict_Dict _tmp695=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9F,_tmp9D,zval);Cyc_CfFlowInfo_ReachableFL(_tmp695,_tmp9E);});_tmp9C.f2=_tmp696;});_tmp9C;});}}_LL73:;}}_LL6E:;}
# 536
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 544
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 548
void*_tmpA2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA3=_tmpA2;void*_tmpE1;union Cyc_Absyn_Constraint*_tmpE0;union Cyc_Absyn_Constraint*_tmpDF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->tag == 5){_LL79: _tmpE1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).elt_typ;_tmpE0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).ptr_atts).bounds;_tmpDF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).ptr_atts).zero_term;_LL7A: {
# 550
union Cyc_CfFlowInfo_FlowInfo _tmpA4=f;struct Cyc_Dict_Dict _tmpDB;struct Cyc_List_List*_tmpDA;if((_tmpA4.BottomFL).tag == 1){_LL7E: _LL7F:
# 552
 return({struct _tuple16 _tmpA5;_tmpA5.f1=f;({void*_tmp697=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE1,0,(env->fenv)->unknown_all);_tmpA5.f2=_tmp697;});_tmpA5;});}else{_LL80: _tmpDB=((_tmpA4.ReachableFL).val).f1;_tmpDA=((_tmpA4.ReachableFL).val).f2;_LL81:
# 555
 if(Cyc_Tcutil_is_bound_one(_tmpE0)){
struct _tuple15 _tmpA6=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpA7=_tmpA6;void*_tmpC8;struct Cyc_List_List*_tmpC7;_LL83: _tmpC8=_tmpA7.f1;_tmpC7=_tmpA7.f2;_LL84:;{
void*_tmpA8=_tmpC8;enum Cyc_CfFlowInfo_InitLevel _tmpC6;struct Cyc_CfFlowInfo_Place*_tmpC5;struct Cyc_Absyn_Vardecl*_tmpC4;void*_tmpC3;switch(*((int*)_tmpA8)){case 8: _LL86: _tmpC4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA8)->f1;_tmpC3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA8)->f2;_LL87:
# 559
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Core_Impossible_exn_struct _tmp699=({struct Cyc_Core_Impossible_exn_struct _tmpAA;_tmpAA.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp698=({const char*_tmpAB="named location in anal_derefR";_tag_dyneither(_tmpAB,sizeof(char),30);});_tmpAA.f1=_tmp698;});_tmpAA;});_tmpA9[0]=_tmp699;});_tmpA9;}));case 1: _LL88: _LL89:
 goto _LL8B;case 2: _LL8A: _LL8B:
# 562
{void*_tmpAC=e->annot;void*_tmpAD=_tmpAC;struct Cyc_List_List*_tmpB2;if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpAD)->tag == Cyc_CfFlowInfo_NotZero){_LL95: _tmpB2=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpAD)->f1;_LL96:
# 564
 if(!Cyc_Relations_same_relns(_tmpDA,_tmpB2))goto _LL98;
goto _LL94;}else{_LL97: _LL98:
# 568
{void*_tmpAE=e->r;void*_tmpAF=_tmpAE;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAF)->tag == 22){_LL9A: _LL9B:
# 570
({void*_tmp69C=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp69B=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpB1;_tmpB1.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp69A=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpDA);_tmpB1.f1=_tmp69A;});_tmpB1;});_tmpB0[0]=_tmp69B;});_tmpB0;});e->annot=_tmp69C;});
goto _LL99;}else{_LL9C: _LL9D:
# 573
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL99;}_LL99:;}
# 576
goto _LL94;}_LL94:;}
# 578
goto _LL85;case 5: _LL8C: _tmpC5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA8)->f1;_LL8D: {
# 582
int possibly_null=0;
{void*_tmpB3=e->annot;void*_tmpB4=_tmpB3;struct Cyc_List_List*_tmpBC;struct Cyc_List_List*_tmpBB;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpB4)->tag == Cyc_CfFlowInfo_UnknownZ){_LL9F: _tmpBB=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpB4)->f1;_LLA0:
# 585
 possibly_null=1;
_tmpBC=_tmpBB;goto _LLA2;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpB4)->tag == Cyc_CfFlowInfo_NotZero){_LLA1: _tmpBC=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpB4)->f1;_LLA2:
# 588
 if(!Cyc_Relations_same_relns(_tmpDA,_tmpBC))goto _LLA4;
goto _LL9E;}else{_LLA3: _LLA4:
# 592
{void*_tmpB5=e->r;void*_tmpB6=_tmpB5;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB6)->tag == 22){_LLA6: _LLA7:
# 594
 if(possibly_null)
({void*_tmp69F=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp69E=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpB8;_tmpB8.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp69D=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpDA);_tmpB8.f1=_tmp69D;});_tmpB8;});_tmpB7[0]=_tmp69E;});_tmpB7;});e->annot=_tmp69F;});else{
# 597
({void*_tmp6A2=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6A1=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpBA;_tmpBA.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp6A0=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpDA);_tmpBA.f1=_tmp6A0;});_tmpBA;});_tmpB9[0]=_tmp6A1;});_tmpB9;});e->annot=_tmp6A2;});}
goto _LLA5;}else{_LLA8: _LLA9:
# 600
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 603
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LLA5;}_LLA5:;}
# 606
goto _LL9E;}}_LL9E:;}
# 608
return({struct _tuple16 _tmpBD;_tmpBD.f1=f;({void*_tmp6A3=Cyc_CfFlowInfo_lookup_place(_tmpDB,_tmpC5);_tmpBD.f2=_tmp6A3;});_tmpBD;});}case 0: _LL8E: _LL8F:
# 610
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpBE;({union Cyc_CfFlowInfo_FlowInfo _tmp6A4=Cyc_CfFlowInfo_BottomFL();_tmpBE.f1=_tmp6A4;});({void*_tmp6A5=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE1,0,(env->fenv)->unknown_all);_tmpBE.f2=_tmp6A5;});_tmpBE;});case 3: _LL90: _tmpC6=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA8)->f1;_LL91:
# 613
({union Cyc_CfFlowInfo_FlowInfo _tmp6A6=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC6,_tmpC7);f=_tmp6A6;});goto _LL93;default: _LL92: _LL93:
# 615
{void*_tmpBF=e->r;void*_tmpC0=_tmpBF;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC0)->tag == 22){_LLAB: _LLAC:
# 617
({void*_tmp6A9=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6A8=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC2;_tmpC2.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6A7=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpDA);_tmpC2.f1=_tmp6A7;});_tmpC2;});_tmpC1[0]=_tmp6A8;});_tmpC1;});e->annot=_tmp6A9;});
goto _LLAA;}else{_LLAD: _LLAE:
# 620
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLAA;}_LLAA:;}
# 623
goto _LL85;}_LL85:;};}else{
# 626
void*_tmpC9=e->annot;void*_tmpCA=_tmpC9;struct Cyc_List_List*_tmpCD;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpCA)->tag == Cyc_CfFlowInfo_UnknownZ){_LLB0: _tmpCD=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpCA)->f1;_LLB1:
# 628
 if(!Cyc_Relations_same_relns(_tmpDA,_tmpCD))goto _LLB3;
goto _LLAF;}else{_LLB2: _LLB3:
# 631
({void*_tmp6AC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6AB=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpCC;_tmpCC.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6AA=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpDA);_tmpCC.f1=_tmp6AA;});_tmpCC;});_tmpCB[0]=_tmp6AB;});_tmpCB;});e->annot=_tmp6AC;});
goto _LLAF;}_LLAF:;}{
# 635
enum Cyc_CfFlowInfo_InitLevel _tmpCE=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpDB,r);enum Cyc_CfFlowInfo_InitLevel _tmpCF=_tmpCE;switch(_tmpCF){case Cyc_CfFlowInfo_NoneIL: _LLB5: _LLB6: {
# 637
struct _tuple15 _tmpD0=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpD1=_tmpD0;void*_tmpD7;_LLBC: _tmpD7=_tmpD1.f1;_LLBD:;
{void*_tmpD2=_tmpD7;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD2)->tag == 7){_LLBF: _LLC0:
# 640
({void*_tmpD3=0;({unsigned int _tmp6AE=e->loc;struct _dyneither_ptr _tmp6AD=({const char*_tmpD4="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpD4,sizeof(char),53);});Cyc_CfFlowInfo_aerr(_tmp6AE,_tmp6AD,_tag_dyneither(_tmpD3,sizeof(void*),0));});});
goto _LLBE;}else{_LLC1: _LLC2:
# 643
({void*_tmpD5=0;({unsigned int _tmp6B0=e->loc;struct _dyneither_ptr _tmp6AF=({const char*_tmpD6="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpD6,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp6B0,_tmp6AF,_tag_dyneither(_tmpD5,sizeof(void*),0));});});}_LLBE:;}
# 645
goto _LLB8;}case Cyc_CfFlowInfo_AllIL: _LLB7: _LLB8:
# 647
 return({struct _tuple16 _tmpD8;_tmpD8.f1=f;({void*_tmp6B1=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE1,0,(env->fenv)->unknown_all);_tmpD8.f2=_tmp6B1;});_tmpD8;});default: _LLB9: _LLBA:
# 649
 return({struct _tuple16 _tmpD9;_tmpD9.f1=f;({void*_tmp6B2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE1,0,(env->fenv)->unknown_none);_tmpD9.f2=_tmp6B2;});_tmpD9;});}_LLB4:;};}_LL7D:;}}else{_LL7B: _LL7C:
# 652
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Core_Impossible_exn_struct _tmp6B4=({struct Cyc_Core_Impossible_exn_struct _tmpDD;_tmpDD.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6B3=({const char*_tmpDE="right deref of non-pointer-type";_tag_dyneither(_tmpDE,sizeof(char),32);});_tmpDD.f1=_tmp6B3;});_tmpDD;});_tmpDC[0]=_tmp6B4;});_tmpDC;}));}_LL78:;}
# 659
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 662
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 665
{void*_tmpE2=e1->r;void*_tmpE3=_tmpE2;void*_tmpE5;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE3)->tag == 1){_LLC4: _tmpE5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LLC5: {
# 667
struct Cyc_Absyn_Vardecl*_tmpE4=Cyc_Tcutil_nonesc_vardecl(_tmpE5);
if(_tmpE4 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpE4);
if(e2_valid_op)
({struct Cyc_List_List*_tmp6B5=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp6B5;});}
# 673
goto _LLC3;}}else{_LLC6: _LLC7:
 goto _LLC3;}_LLC3:;}{
# 677
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp6B6=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp6B6;});}
# 683
return relns;};}
# 691
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 696
struct _tuple18 _tmpE6=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpE7=_tmpE6;union Cyc_CfFlowInfo_AbsLVal _tmpF7;_LLC9: _tmpF7=_tmpE7.f2;_LLCA:;
{struct _tuple18 _tmpE8=({struct _tuple18 _tmpF6;_tmpF6.f1=inflow;_tmpF6.f2=_tmpF7;_tmpF6;});struct _tuple18 _tmpE9=_tmpE8;struct Cyc_Dict_Dict _tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_CfFlowInfo_Place*_tmpF3;if(((_tmpE9.f1).ReachableFL).tag == 2){if(((_tmpE9.f2).PlaceL).tag == 1){_LLCC: _tmpF5=(((_tmpE9.f1).ReachableFL).val).f1;_tmpF4=(((_tmpE9.f1).ReachableFL).val).f2;_tmpF3=((_tmpE9.f2).PlaceL).val;_LLCD: {
# 699
void*_tmpEA=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 701
struct _tuple15 _tmpEB=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);struct _tuple15 _tmpEC=_tmpEB;void*_tmpF0;struct Cyc_List_List*_tmpEF;_LLD3: _tmpF0=_tmpEC.f1;_tmpEF=_tmpEC.f2;_LLD4:;
for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){
({void*_tmp6B8=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpED=_region_malloc((env->fenv)->r,sizeof(*_tmpED));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6B7=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpEE;_tmpEE.tag=8;_tmpEE.f1=(struct Cyc_Absyn_Vardecl*)_tmpEF->hd;_tmpEE.f2=_tmpEA;_tmpEE;});_tmpED[0]=_tmp6B7;});_tmpED;});_tmpEA=_tmp6B8;});}
# 706
({struct Cyc_Dict_Dict _tmp6B9=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpF5,_tmpF3,_tmpEA);_tmpF5=_tmp6B9;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6BA=Cyc_CfFlowInfo_ReachableFL(_tmpF5,_tmpF4);inflow=_tmp6BA;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 712
goto _LLCB;}}else{_LLD0: _LLD1:
# 715
({void*_tmpF1=0;({struct _dyneither_ptr _tmp6BB=({const char*_tmpF2="noconsume() parameters must be l-values";_tag_dyneither(_tmpF2,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6BB,_tag_dyneither(_tmpF1,sizeof(void*),0));});});
goto _LLCB;}}else{_LLCE: _LLCF:
# 713
 goto _LLCB;}_LLCB:;}
# 718
return inflow;}
# 723
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 731
union Cyc_CfFlowInfo_FlowInfo _tmpF8=outflow;struct Cyc_Dict_Dict _tmp106;struct Cyc_List_List*_tmp105;if((_tmpF8.BottomFL).tag == 1){_LLD6: _LLD7:
# 733
 return({struct _tuple16 _tmpF9;({union Cyc_CfFlowInfo_FlowInfo _tmp6BC=Cyc_CfFlowInfo_BottomFL();_tmpF9.f1=_tmp6BC;});_tmpF9.f2=rval;_tmpF9;});}else{_LLD8: _tmp106=((_tmpF8.ReachableFL).val).f1;_tmp105=((_tmpF8.ReachableFL).val).f2;_LLD9: {
# 735
union Cyc_CfFlowInfo_AbsLVal _tmpFA=lval;struct Cyc_CfFlowInfo_Place*_tmp104;if((_tmpFA.PlaceL).tag == 1){_LLDB: _tmp104=(_tmpFA.PlaceL).val;_LLDC: {
# 737
struct Cyc_Dict_Dict _tmpFB=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp106,_tmp104,rval);
({struct Cyc_List_List*_tmp6BD=Cyc_Relations_reln_assign_exp(fenv->r,_tmp105,lexp,rexp);_tmp105=_tmp6BD;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6BE=Cyc_CfFlowInfo_ReachableFL(_tmpFB,_tmp105);outflow=_tmp6BE;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 743
struct _tuple15 _tmpFC=({struct _RegionHandle*_tmp6BF=fenv->r;Cyc_CfFlowInfo_unname_rval(_tmp6BF,Cyc_CfFlowInfo_lookup_place(_tmp106,_tmp104));});struct _tuple15 _tmpFD=_tmpFC;void*_tmp101;_LLE0: _tmp101=_tmpFD.f1;_LLE1:;{
void*_tmpFE=_tmp101;switch(*((int*)_tmpFE)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpFE)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE3: _LLE4:
 goto _LLE6;}else{goto _LLE9;}case 7: _LLE5: _LLE6:
 goto _LLE8;case 0: _LLE7: _LLE8:
 goto _LLE2;default: _LLE9: _LLEA:
# 749
({void*_tmpFF=0;({unsigned int _tmp6C1=lexp->loc;struct _dyneither_ptr _tmp6C0=({const char*_tmp100="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmp100,sizeof(char),47);});Cyc_Tcutil_warn(_tmp6C1,_tmp6C0,_tag_dyneither(_tmpFF,sizeof(void*),0));});});
goto _LLE2;}_LLE2:;};}
# 754
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmp102;_tmp102.f1=outflow;_tmp102.f2=rval;_tmp102;});}}else{_LLDD: _LLDE:
# 757
 return({struct _tuple16 _tmp103;({union Cyc_CfFlowInfo_FlowInfo _tmp6C2=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp103.f1=_tmp6C2;});_tmp103.f2=rval;_tmp103;});}_LLDA:;}}_LLD5:;}
# 764
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 770
union Cyc_CfFlowInfo_FlowInfo _tmp107=f;struct Cyc_Dict_Dict _tmp10D;struct Cyc_List_List*_tmp10C;if((_tmp107.BottomFL).tag == 1){_LLEC: _LLED:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLEE: _tmp10D=((_tmp107.ReachableFL).val).f1;_tmp10C=((_tmp107.ReachableFL).val).f2;_LLEF:
# 775
({struct Cyc_Dict_Dict _tmp6C9=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6C8=fenv;unsigned int _tmp6C7=loc;struct Cyc_Dict_Dict _tmp6C6=_tmp10D;struct Cyc_CfFlowInfo_Place*_tmp6C5=({struct Cyc_CfFlowInfo_Place*_tmp108=_region_malloc(env->r,sizeof(*_tmp108));({void*_tmp6C4=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp109=_region_malloc(env->r,sizeof(*_tmp109));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6C3=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp10A;_tmp10A.tag=0;_tmp10A.f1=vd;_tmp10A;});_tmp109[0]=_tmp6C3;});_tmp109;});_tmp108->root=_tmp6C4;});_tmp108->fields=0;_tmp108;});Cyc_CfFlowInfo_assign_place(_tmp6C8,_tmp6C7,_tmp6C6,_tmp6C5,rval);});_tmp10D=_tmp6C9;});
# 778
({struct Cyc_List_List*_tmp6CA=Cyc_Relations_reln_assign_var(env->r,_tmp10C,vd,rexp);_tmp10C=_tmp6CA;});{
union Cyc_CfFlowInfo_FlowInfo _tmp10B=Cyc_CfFlowInfo_ReachableFL(_tmp10D,_tmp10C);
Cyc_NewControlFlow_update_tryflow(env,_tmp10B);
# 783
return _tmp10B;};}_LLEB:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 787
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 794
if(vds == 0)return inflow;{
# 797
struct Cyc_List_List*_tmp10E=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp6CB=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp10F->tl=es;_tmp10F;});es=_tmp6CB;});}}
# 803
({union Cyc_CfFlowInfo_FlowInfo _tmp6CC=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp10E,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp6CC;});
# 805
({union Cyc_CfFlowInfo_FlowInfo _tmp6CD=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp6CD;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 809
struct _tuple16 _tmp110=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp111=_tmp110;union Cyc_CfFlowInfo_FlowInfo _tmp113;void*_tmp112;_LLF1: _tmp113=_tmp111.f1;_tmp112=_tmp111.f2;_LLF2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp6CE=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp113,_tmp112);inflow=_tmp6CE;});}}{
# 817
struct Cyc_List_List*_tmp114=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp114 != 0;_tmp114=_tmp114->tl){
struct _tuple24*_tmp115=(struct _tuple24*)_tmp114->hd;struct _tuple24*_tmp116=_tmp115;struct Cyc_Absyn_Vardecl**_tmp131;struct Cyc_Absyn_Exp*_tmp130;_LLF4: _tmp131=_tmp116->f1;_tmp130=_tmp116->f2;_LLF5:;
if(_tmp131 != 0  && _tmp130 != 0){
if(_tmp130->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp119;_tmp119.tag=0;({struct _dyneither_ptr _tmp6CF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp130));_tmp119.f1=_tmp6CF;});({void*_tmp117[1]={& _tmp119};({struct _dyneither_ptr _tmp6D0=({const char*_tmp118="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp118,sizeof(char),41);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6D0,_tag_dyneither(_tmp117,sizeof(void*),1));});});});{
# 831
struct Cyc_List_List l=({struct Cyc_List_List _tmp12F;_tmp12F.hd=_tmp130;_tmp12F.tl=0;_tmp12F;});
union Cyc_CfFlowInfo_FlowInfo _tmp11A=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp11B=Cyc_NewControlFlow_anal_Lexp(env,_tmp11A,0,0,_tmp130);struct _tuple18 _tmp11C=_tmp11B;union Cyc_CfFlowInfo_AbsLVal _tmp12E;_LLF7: _tmp12E=_tmp11C.f2;_LLF8:;{
struct _tuple16 _tmp11D=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp11A,_tmp130);struct _tuple16 _tmp11E=_tmp11D;union Cyc_CfFlowInfo_FlowInfo _tmp12D;void*_tmp12C;_LLFA: _tmp12D=_tmp11E.f1;_tmp12C=_tmp11E.f2;_LLFB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp11F=_tmp12D;struct Cyc_Dict_Dict _tmp12B;struct Cyc_List_List*_tmp12A;if((_tmp11F.ReachableFL).tag == 2){_LLFD: _tmp12B=((_tmp11F.ReachableFL).val).f1;_tmp12A=((_tmp11F.ReachableFL).val).f2;_LLFE:
# 837
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp120=_tmp12E;struct Cyc_CfFlowInfo_Place*_tmp129;if((_tmp120.PlaceL).tag == 1){_LL102: _tmp129=(_tmp120.PlaceL).val;_LL103:
# 840
({void*_tmp6D2=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp121=_region_malloc(fenv->r,sizeof(*_tmp121));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D1=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp122;_tmp122.tag=8;_tmp122.f1=*_tmp131;_tmp122.f2=_tmp12C;_tmp122;});_tmp121[0]=_tmp6D1;});_tmp121;});_tmp12C=_tmp6D2;});{
# 843
void*_tmp123=Cyc_CfFlowInfo_lookup_place(_tmp12B,_tmp129);
({void*_tmp6D4=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp124=_region_malloc(fenv->r,sizeof(*_tmp124));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D3=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp125;_tmp125.tag=8;_tmp125.f1=*_tmp131;_tmp125.f2=_tmp123;_tmp125;});_tmp124[0]=_tmp6D3;});_tmp124;});_tmp123=_tmp6D4;});
({struct Cyc_Dict_Dict _tmp6D5=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp12B,_tmp129,_tmp123);_tmp12B=_tmp6D5;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6D6=Cyc_CfFlowInfo_ReachableFL(_tmp12B,_tmp12A);_tmp12D=_tmp6D6;});
goto _LL101;};}else{_LL104: _LL105:
# 850
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp130->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp131)->type))
({struct Cyc_String_pa_PrintArg_struct _tmp128;_tmp128.tag=0;({struct _dyneither_ptr _tmp6D7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp130));_tmp128.f1=_tmp6D7;});({void*_tmp126[1]={& _tmp128};({unsigned int _tmp6D9=exp_loc;struct _dyneither_ptr _tmp6D8=({const char*_tmp127="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp127,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp6D9,_tmp6D8,_tag_dyneither(_tmp126,sizeof(void*),1));});});});}_LL101:;}
# 860
({union Cyc_CfFlowInfo_FlowInfo _tmp6DA=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp12D,*_tmp131,_tmp130,_tmp12C,pat_loc);inflow=_tmp6DA;});
goto _LLFC;}else{_LLFF: _LL100:
# 863
 goto _LLFC;}_LLFC:;};};};}}
# 868
return inflow;};};}
# 871
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 873
void*_tmp132=e->r;void*_tmp133=_tmp132;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp136;switch(*((int*)_tmp133)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp133)->f1)){case 4: _LL107: _LL108:
 goto _LL10A;case 3: _LL109: _LL10A:
 goto _LL10C;case 5: _LL10B: _LL10C:
 return 1;default: goto _LL117;}case 19: _LL10D: _tmp136=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp133)->f1;_LL10E:
 _tmp137=_tmp136;goto _LL110;case 20: _LL10F: _tmp137=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp133)->f1;_LL110:
 _tmp138=_tmp137;goto _LL112;case 21: _LL111: _tmp138=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp133)->f1;_LL112:
# 880
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp138,cast_ok);case 22: _LL113: _tmp139=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp133)->f1;_LL114: {
# 882
void*_tmp134=Cyc_Tcutil_compress((void*)_check_null(_tmp139->topt));void*_tmp135=_tmp134;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp135)->tag == 10){_LL11A: _LL11B:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp139,cast_ok);}else{_LL11C: _LL11D:
 return 0;}_LL119:;}case 13: _LL115: _tmp13A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp133)->f2;_LL116:
# 887
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13A,cast_ok);else{
return 0;}default: _LL117: _LL118:
 return 0;}_LL106:;}
# 893
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp13B=*rop;union Cyc_Relations_RelnOp _tmp13C=_tmp13B;unsigned int _tmp140;unsigned int _tmp13F;switch((_tmp13C.RParamNumelts).tag){case 5: _LL11F: _tmp13F=(_tmp13C.RParam).val;_LL120: {
# 896
struct Cyc_Absyn_Exp*_tmp13D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13F);
return Cyc_Relations_exp2relnop(_tmp13D,rop);}case 6: _LL121: _tmp140=(_tmp13C.RParamNumelts).val;_LL122: {
# 899
struct Cyc_Absyn_Exp*_tmp13E=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp140);
return({struct Cyc_Absyn_Exp*_tmp6DB=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp13E,0);Cyc_Relations_exp2relnop(_tmp6DB,rop);});}default: _LL123: _LL124:
 return 1;}_LL11E:;}
# 905
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp141=rop;unsigned int _tmp143;unsigned int _tmp142;switch((_tmp141.RParamNumelts).tag){case 5: _LL126: _tmp142=(_tmp141.RParam).val;_LL127:
# 908
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp142));case 6: _LL128: _tmp143=(_tmp141.RParamNumelts).val;_LL129:
# 910
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp143));default: _LL12A: _LL12B:
 return Cyc_Relations_rop2string(rop);}_LL125:;}
# 915
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 918
union Cyc_CfFlowInfo_FlowInfo _tmp144=inflow;struct Cyc_Dict_Dict _tmp14E;struct Cyc_List_List*_tmp14D;if((_tmp144.BottomFL).tag == 1){_LL12D: _LL12E:
 return;}else{_LL12F: _tmp14E=((_tmp144.ReachableFL).val).f1;_tmp14D=((_tmp144.ReachableFL).val).f2;_LL130:
# 921
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp145=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp145->rop1;
union Cyc_Relations_RelnOp rop2=_tmp145->rop2;
enum Cyc_Relations_Relation _tmp146=Cyc_Relations_flip_relation(_tmp145->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(env->r,rop2,_tmp146,rop1,_tmp14D))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp145->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0;({struct _dyneither_ptr _tmp6DC=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp14D));_tmp14C.f1=_tmp6DC;});({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;_tmp149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp147[4]={& _tmp149,& _tmp14A,& _tmp14B,& _tmp14C};({unsigned int _tmp6DE=loc;struct _dyneither_ptr _tmp6DD=({const char*_tmp148="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp148,sizeof(char),77);});Cyc_Tcutil_terr(_tmp6DE,_tmp6DD,_tag_dyneither(_tmp147,sizeof(void*),4));});});});});});});
break;}}
# 936
goto _LL12C;}_LL12C:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 941
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 945
struct Cyc_CfFlowInfo_FlowEnv*_tmp14F=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 958
{union Cyc_CfFlowInfo_FlowInfo _tmp150=inflow;struct Cyc_Dict_Dict _tmp153;struct Cyc_List_List*_tmp152;if((_tmp150.BottomFL).tag == 1){_LL132: _LL133:
 return({struct _tuple16 _tmp151;({union Cyc_CfFlowInfo_FlowInfo _tmp6DF=Cyc_CfFlowInfo_BottomFL();_tmp151.f1=_tmp6DF;});_tmp151.f2=_tmp14F->unknown_all;_tmp151;});}else{_LL134: _tmp153=((_tmp150.ReachableFL).val).f1;_tmp152=((_tmp150.ReachableFL).val).f2;_LL135:
 d=_tmp153;relns=_tmp152;}_LL131:;}
# 973 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 994 "new_control_flow.cyc"
struct _tuple18 _tmp154=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp155=_tmp154;union Cyc_CfFlowInfo_FlowInfo _tmp165;union Cyc_CfFlowInfo_AbsLVal _tmp164;_LL137: _tmp165=_tmp155.f1;_tmp164=_tmp155.f2;_LL138:;{
struct _tuple18 _tmp156=({struct _tuple18 _tmp163;_tmp163.f1=_tmp165;_tmp163.f2=_tmp164;_tmp163;});struct _tuple18 _tmp157=_tmp156;struct Cyc_Dict_Dict _tmp162;struct Cyc_List_List*_tmp161;struct Cyc_CfFlowInfo_Place*_tmp160;if(((_tmp157.f1).ReachableFL).tag == 2){if(((_tmp157.f2).PlaceL).tag == 1){_LL13A: _tmp162=(((_tmp157.f1).ReachableFL).val).f1;_tmp161=(((_tmp157.f1).ReachableFL).val).f2;_tmp160=((_tmp157.f2).PlaceL).val;_LL13B: {
# 997
void*_tmp158=Cyc_CfFlowInfo_lookup_place(_tmp162,_tmp160);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp158,& needs_unconsume)){
({void*_tmp159=0;({unsigned int _tmp6E1=e->loc;struct _dyneither_ptr _tmp6E0=({const char*_tmp15A="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp15A,sizeof(char),56);});Cyc_CfFlowInfo_aerr(_tmp6E1,_tmp6E0,_tag_dyneither(_tmp159,sizeof(void*),0));});});
return({struct _tuple16 _tmp15B;({union Cyc_CfFlowInfo_FlowInfo _tmp6E2=Cyc_CfFlowInfo_BottomFL();_tmp15B.f1=_tmp6E2;});_tmp15B.f2=_tmp14F->unknown_all;_tmp15B;});}else{
# 1003
if(needs_unconsume)
# 1005
return({struct _tuple16 _tmp15C;_tmp15C.f1=_tmp165;({void*_tmp6E3=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14F,_tmp158);_tmp15C.f2=_tmp6E3;});_tmp15C;});else{
# 1008
void*_tmp15D=Cyc_CfFlowInfo_make_unique_consumed(_tmp14F,(void*)_check_null(e->topt),e,env->iteration_num,_tmp158);
struct Cyc_Dict_Dict _tmp15E=Cyc_CfFlowInfo_assign_place(_tmp14F,e->loc,_tmp162,_tmp160,_tmp15D);
# 1020
return({struct _tuple16 _tmp15F;({union Cyc_CfFlowInfo_FlowInfo _tmp6E4=Cyc_CfFlowInfo_ReachableFL(_tmp15E,_tmp161);_tmp15F.f1=_tmp6E4;});_tmp15F.f2=_tmp158;_tmp15F;});}}}}else{goto _LL13C;}}else{_LL13C: _LL13D:
# 1022
 goto _LL139;}_LL139:;};}{
# 1025
void*_tmp166=e->r;void*_tmp167=_tmp166;struct Cyc_Absyn_Stmt*_tmp36A;struct Cyc_Absyn_Exp*_tmp369;void*_tmp368;int _tmp367;struct Cyc_Absyn_Vardecl*_tmp366;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Exp*_tmp364;int _tmp363;struct Cyc_List_List*_tmp362;struct Cyc_List_List*_tmp361;enum Cyc_Absyn_AggrKind _tmp360;struct Cyc_List_List*_tmp35F;struct Cyc_List_List*_tmp35E;struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Exp*_tmp35C;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp353;struct _dyneither_ptr*_tmp352;struct Cyc_Absyn_Exp*_tmp351;struct _dyneither_ptr*_tmp350;struct Cyc_Absyn_Exp*_tmp34F;struct _dyneither_ptr*_tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Absyn_Exp*_tmp348;int _tmp347;struct Cyc_Absyn_Exp*_tmp346;void**_tmp345;struct Cyc_Absyn_Exp*_tmp344;int _tmp343;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_Absyn_Exp*_tmp339;enum Cyc_Absyn_Primop _tmp338;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Datatypedecl*_tmp335;struct Cyc_Absyn_Vardecl*_tmp334;struct Cyc_Absyn_Vardecl*_tmp333;struct Cyc_Absyn_Vardecl*_tmp332;struct Cyc_Absyn_Exp*_tmp331;struct Cyc_Absyn_Exp*_tmp330;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*_tmp32E;switch(*((int*)_tmp167)){case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp167)->f4 == Cyc_Absyn_Null_to_NonNull){_LL13F: _tmp32E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL140: {
# 1029
struct _tuple16 _tmp168=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp32E);struct _tuple16 _tmp169=_tmp168;union Cyc_CfFlowInfo_FlowInfo _tmp170;void*_tmp16F;_LL1A8: _tmp170=_tmp169.f1;_tmp16F=_tmp169.f2;_LL1A9:;{
struct _tuple16 _tmp16A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp170,_tmp32E,_tmp16F);struct _tuple16 _tmp16B=_tmp16A;union Cyc_CfFlowInfo_FlowInfo _tmp16E;void*_tmp16D;_LL1AB: _tmp16E=_tmp16B.f1;_tmp16D=_tmp16B.f2;_LL1AC:;
return({struct _tuple16 _tmp16C;_tmp16C.f1=_tmp16E;_tmp16C.f2=_tmp16F;_tmp16C;});};}}else{_LL141: _tmp32F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL142:
# 1035
 _tmp330=_tmp32F;goto _LL144;}case 11: _LL143: _tmp330=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL144:
 _tmp331=_tmp330;goto _LL146;case 12: _LL145: _tmp331=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL146:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp331);case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp167)->f1).Int_c).tag){case 1: _LL147: _LL148:
# 1039
 goto _LL14A;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp167)->f1).Int_c).val).f2 == 0){_LL149: _LL14A:
 return({struct _tuple16 _tmp171;_tmp171.f1=inflow;_tmp171.f2=_tmp14F->zero;_tmp171;});}else{_LL14B: _LL14C:
 goto _LL14E;}default: _LL151: _LL152:
# 1045
 goto _LL154;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167)->f1)){case 2: _LL14D: _LL14E:
# 1042
 return({struct _tuple16 _tmp172;_tmp172.f1=inflow;_tmp172.f2=_tmp14F->notzeroall;_tmp172;});case 1: _LL15D: _LL15E:
# 1053
 return({struct _tuple16 _tmp174;_tmp174.f1=inflow;({void*_tmp6E5=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp174.f2=_tmp6E5;});_tmp174;});case 3: _LL15F: _tmp332=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167)->f1)->f1;_LL160:
# 1056
 _tmp333=_tmp332;goto _LL162;case 4: _LL161: _tmp333=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167)->f1)->f1;_LL162:
 _tmp334=_tmp333;goto _LL164;case 5: _LL163: _tmp334=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167)->f1)->f1;_LL164: {
# 1060
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp176;_tmp176.tag=0;_tmp176.f1=_tmp334;_tmp176;});
return({struct _tuple16 _tmp175;_tmp175.f1=inflow;({void*_tmp6E6=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);_tmp175.f2=_tmp6E6;});_tmp175;});}default: _LL19D: _LL19E:
# 1637
 goto _LL1A0;}case 30: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp167)->f1 == 0){_LL14F: _LL150:
# 1044
 goto _LL152;}else{_LL18B: _tmp336=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp335=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL18C:
# 1494
 _tmp35D=_tmp336;goto _LL18E;}case 17: _LL153: _LL154:
# 1046
 goto _LL156;case 16: _LL155: _LL156:
 goto _LL158;case 18: _LL157: _LL158:
 goto _LL15A;case 32: _LL159: _LL15A:
 goto _LL15C;case 31: _LL15B: _LL15C:
 return({struct _tuple16 _tmp173;_tmp173.f1=inflow;_tmp173.f2=_tmp14F->unknown_all;_tmp173;});case 2: _LL165: _tmp338=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp337=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL166: {
# 1067
struct _tuple16 _tmp177=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp337);struct _tuple16 _tmp178=_tmp177;union Cyc_CfFlowInfo_FlowInfo _tmp17B;void*_tmp17A;_LL1AE: _tmp17B=_tmp178.f1;_tmp17A=_tmp178.f2;_LL1AF:;
return({struct _tuple16 _tmp179;_tmp179.f1=_tmp17B;_tmp179.f2=_tmp17A;_tmp179;});}case 4: _LL167: _tmp339=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL168: {
# 1071
struct Cyc_List_List _tmp17C=({struct Cyc_List_List _tmp18A;_tmp18A.hd=_tmp339;_tmp18A.tl=0;_tmp18A;});
struct _tuple16 _tmp17D=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp17C);struct _tuple16 _tmp17E=_tmp17D;union Cyc_CfFlowInfo_FlowInfo _tmp189;_LL1B1: _tmp189=_tmp17E.f1;_LL1B2:;{
struct _tuple18 _tmp17F=Cyc_NewControlFlow_anal_Lexp(env,_tmp189,0,0,_tmp339);struct _tuple18 _tmp180=_tmp17F;union Cyc_CfFlowInfo_AbsLVal _tmp188;_LL1B4: _tmp188=_tmp180.f2;_LL1B5:;
{struct _tuple25 _tmp181=({struct _tuple25 _tmp186;_tmp186.f1=_tmp188;_tmp186.f2=_tmp189;_tmp186;});struct _tuple25 _tmp182=_tmp181;struct Cyc_CfFlowInfo_Place*_tmp185;struct Cyc_Dict_Dict _tmp184;struct Cyc_List_List*_tmp183;if(((_tmp182.f1).PlaceL).tag == 1){if(((_tmp182.f2).ReachableFL).tag == 2){_LL1B7: _tmp185=((_tmp182.f1).PlaceL).val;_tmp184=(((_tmp182.f2).ReachableFL).val).f1;_tmp183=(((_tmp182.f2).ReachableFL).val).f2;_LL1B8:
# 1076
({struct Cyc_List_List*_tmp6E7=Cyc_Relations_reln_kill_exp(_tmp14F->r,_tmp183,_tmp339);_tmp183=_tmp6E7;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6E9=({struct Cyc_Dict_Dict _tmp6E8=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp339->loc,_tmp184,_tmp185,_tmp14F->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6E8,_tmp183);});_tmp189=_tmp6E9;});
# 1081
goto _LL1B6;}else{goto _LL1B9;}}else{_LL1B9: _LL1BA:
 goto _LL1B6;}_LL1B6:;}
# 1084
return({struct _tuple16 _tmp187;_tmp187.f1=_tmp189;_tmp187.f2=_tmp14F->unknown_all;_tmp187;});};}case 3: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp167)->f2 != 0){_LL169: _tmp33B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp33A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp167)->f3;_LL16A:
# 1087
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp18B=0;({unsigned int _tmp6EB=e->loc;struct _dyneither_ptr _tmp6EA=({const char*_tmp18C="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp18C,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp6EB,_tmp6EA,_tag_dyneither(_tmp18B,sizeof(void*),0));});});
return({struct _tuple16 _tmp18D;({union Cyc_CfFlowInfo_FlowInfo _tmp6EC=Cyc_CfFlowInfo_BottomFL();_tmp18D.f1=_tmp6EC;});_tmp18D.f2=_tmp14F->unknown_all;_tmp18D;});}{
# 1091
struct _tuple16 _tmp18E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp33A);struct _tuple16 _tmp18F=_tmp18E;union Cyc_CfFlowInfo_FlowInfo _tmp19B;void*_tmp19A;_LL1BC: _tmp19B=_tmp18F.f1;_tmp19A=_tmp18F.f2;_LL1BD:;{
struct _tuple18 _tmp190=Cyc_NewControlFlow_anal_Lexp(env,_tmp19B,0,0,_tmp33B);struct _tuple18 _tmp191=_tmp190;union Cyc_CfFlowInfo_FlowInfo _tmp199;union Cyc_CfFlowInfo_AbsLVal _tmp198;_LL1BF: _tmp199=_tmp191.f1;_tmp198=_tmp191.f2;_LL1C0:;
{union Cyc_CfFlowInfo_FlowInfo _tmp192=_tmp199;struct Cyc_Dict_Dict _tmp196;struct Cyc_List_List*_tmp195;if((_tmp192.ReachableFL).tag == 2){_LL1C2: _tmp196=((_tmp192.ReachableFL).val).f1;_tmp195=((_tmp192.ReachableFL).val).f2;_LL1C3:
# 1095
{union Cyc_CfFlowInfo_AbsLVal _tmp193=_tmp198;struct Cyc_CfFlowInfo_Place*_tmp194;if((_tmp193.PlaceL).tag == 1){_LL1C7: _tmp194=(_tmp193.PlaceL).val;_LL1C8:
# 1099
({struct Cyc_List_List*_tmp6ED=Cyc_Relations_reln_kill_exp(_tmp14F->r,_tmp195,_tmp33B);_tmp195=_tmp6ED;});
({struct Cyc_Dict_Dict _tmp6EE=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp33B->loc,_tmp196,_tmp194,_tmp14F->unknown_all);_tmp196=_tmp6EE;});
# 1102
({union Cyc_CfFlowInfo_FlowInfo _tmp6EF=Cyc_CfFlowInfo_ReachableFL(_tmp196,_tmp195);_tmp199=_tmp6EF;});
# 1106
goto _LL1C6;}else{_LL1C9: _LL1CA:
# 1109
 goto _LL1C6;}_LL1C6:;}
# 1111
goto _LL1C1;}else{_LL1C4: _LL1C5:
 goto _LL1C1;}_LL1C1:;}
# 1114
return({struct _tuple16 _tmp197;_tmp197.f1=_tmp199;_tmp197.f2=_tmp14F->unknown_all;_tmp197;});};};}else{_LL16B: _tmp33D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp33C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp167)->f3;_LL16C:
# 1118
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp19C=0;({unsigned int _tmp6F1=e->loc;struct _dyneither_ptr _tmp6F0=({const char*_tmp19D="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp19D,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp6F1,_tmp6F0,_tag_dyneither(_tmp19C,sizeof(void*),0));});});
return({struct _tuple16 _tmp19E;({union Cyc_CfFlowInfo_FlowInfo _tmp6F2=Cyc_CfFlowInfo_BottomFL();_tmp19E.f1=_tmp6F2;});_tmp19E.f2=_tmp14F->unknown_all;_tmp19E;});}{
# 1125
struct _tuple16 _tmp19F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp33C);struct _tuple16 _tmp1A0=_tmp19F;union Cyc_CfFlowInfo_FlowInfo _tmp1A6;void*_tmp1A5;_LL1CC: _tmp1A6=_tmp1A0.f1;_tmp1A5=_tmp1A0.f2;_LL1CD:;{
struct _tuple18 _tmp1A1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A6,0,0,_tmp33D);struct _tuple18 _tmp1A2=_tmp1A1;union Cyc_CfFlowInfo_FlowInfo _tmp1A4;union Cyc_CfFlowInfo_AbsLVal _tmp1A3;_LL1CF: _tmp1A4=_tmp1A2.f1;_tmp1A3=_tmp1A2.f2;_LL1D0:;
return Cyc_NewControlFlow_do_assign(_tmp14F,env,_tmp1A4,_tmp33D,_tmp1A3,_tmp33C,_tmp1A5,e->loc);};};}case 8: _LL16D: _tmp33F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp33E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL16E: {
# 1130
struct _tuple16 _tmp1A7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp33F);struct _tuple16 _tmp1A8=_tmp1A7;union Cyc_CfFlowInfo_FlowInfo _tmp1AA;void*_tmp1A9;_LL1D2: _tmp1AA=_tmp1A8.f1;_tmp1A9=_tmp1A8.f2;_LL1D3:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1AA,_tmp33E);}case 10: _LL16F: _tmp340=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL170: {
# 1134
struct _tuple16 _tmp1AB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp340);struct _tuple16 _tmp1AC=_tmp1AB;union Cyc_CfFlowInfo_FlowInfo _tmp1AF;void*_tmp1AE;_LL1D5: _tmp1AF=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;_LL1D6:;
Cyc_NewControlFlow_use_Rval(env,_tmp340->loc,_tmp1AF,_tmp1AE);
return({struct _tuple16 _tmp1AD;({union Cyc_CfFlowInfo_FlowInfo _tmp6F3=Cyc_CfFlowInfo_BottomFL();_tmp1AD.f1=_tmp6F3;});({void*_tmp6F4=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp1AD.f2=_tmp6F4;});_tmp1AD;});}case 9: _LL171: _tmp342=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp341=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL172: {
# 1139
struct _RegionHandle _tmp1B0=_new_region("temp");struct _RegionHandle*temp=& _tmp1B0;_push_region(temp);
{struct Cyc_List_List*_tmp1B1=_tmp341;
struct _tuple23 _tmp1B2=({struct _RegionHandle*_tmp6F8=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp6F7=env;union Cyc_CfFlowInfo_FlowInfo _tmp6F6=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6F8,_tmp6F7,_tmp6F6,({struct Cyc_List_List*_tmp1DF=_region_malloc(temp,sizeof(*_tmp1DF));_tmp1DF->hd=_tmp342;({struct Cyc_List_List*_tmp6F5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp341);_tmp1DF->tl=_tmp6F5;});_tmp1DF;}),0,1);});struct _tuple23 _tmp1B3=_tmp1B2;union Cyc_CfFlowInfo_FlowInfo _tmp1DE;struct Cyc_List_List*_tmp1DD;_LL1D8: _tmp1DE=_tmp1B3.f1;_tmp1DD=_tmp1B3.f2;_LL1D9:;{
# 1143
union Cyc_CfFlowInfo_FlowInfo _tmp1B4=Cyc_NewControlFlow_use_Rval(env,_tmp342->loc,_tmp1DE,(void*)((struct Cyc_List_List*)_check_null(_tmp1DD))->hd);
_tmp1DD=_tmp1DD->tl;{
# 1146
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1B5=Cyc_Tcutil_compress((void*)_check_null(_tmp342->topt));void*_tmp1B6=_tmp1B5;void*_tmp1C8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B6)->tag == 5){_LL1DB: _tmp1C8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B6)->f1).elt_typ;_LL1DC:
# 1153
{void*_tmp1B7=Cyc_Tcutil_compress(_tmp1C8);void*_tmp1B8=_tmp1B7;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->tag == 9){_LL1E0: _tmp1C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).attributes;_tmp1C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).requires_relns;_tmp1C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).ensures_relns;_LL1E1:
# 1155
 requires=_tmp1C4;
ensures=_tmp1C3;
for(0;_tmp1C5 != 0;_tmp1C5=_tmp1C5->tl){
# 1159
void*_tmp1B9=(void*)_tmp1C5->hd;void*_tmp1BA=_tmp1B9;int _tmp1C0;int _tmp1BF;int _tmp1BE;switch(*((int*)_tmp1BA)){case 20: _LL1E5: _tmp1BE=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1BA)->f1;_LL1E6:
# 1161
({struct Cyc_List_List*_tmp6F9=({struct Cyc_List_List*_tmp1BB=_region_malloc(temp,sizeof(*_tmp1BB));_tmp1BB->hd=(void*)_tmp1BE;_tmp1BB->tl=init_params;_tmp1BB;});init_params=_tmp6F9;});goto _LL1E4;case 21: _LL1E7: _tmp1BF=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1BA)->f1;_LL1E8:
# 1163
({struct Cyc_List_List*_tmp6FA=({struct Cyc_List_List*_tmp1BC=_region_malloc(temp,sizeof(*_tmp1BC));_tmp1BC->hd=(void*)_tmp1BF;_tmp1BC->tl=nolive_unique_params;_tmp1BC;});nolive_unique_params=_tmp6FA;});
goto _LL1E4;case 22: _LL1E9: _tmp1C0=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1BA)->f1;_LL1EA:
# 1167
({struct Cyc_List_List*_tmp6FB=({struct Cyc_List_List*_tmp1BD=_region_malloc(temp,sizeof(*_tmp1BD));_tmp1BD->hd=(void*)_tmp1C0;_tmp1BD->tl=noconsume_params;_tmp1BD;});noconsume_params=_tmp6FB;});
goto _LL1E4;default: _LL1EB: _LL1EC:
 goto _LL1E4;}_LL1E4:;}
# 1171
goto _LL1DF;}else{_LL1E2: _LL1E3:
({void*_tmp1C1=0;({struct _dyneither_ptr _tmp6FC=({const char*_tmp1C2="anal_Rexp: bad function type";_tag_dyneither(_tmp1C2,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6FC,_tag_dyneither(_tmp1C1,sizeof(void*),0));});});}_LL1DF:;}
# 1174
goto _LL1DA;}else{_LL1DD: _LL1DE:
({void*_tmp1C6=0;({struct _dyneither_ptr _tmp6FD=({const char*_tmp1C7="anal_Rexp: bad function type";_tag_dyneither(_tmp1C7,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6FD,_tag_dyneither(_tmp1C6,sizeof(void*),0));});});}_LL1DA:;}
# 1177
{int i=1;for(0;_tmp1DD != 0;(((_tmp1DD=_tmp1DD->tl,_tmp341=((struct Cyc_List_List*)_check_null(_tmp341))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1C9=_tmp1DE;struct Cyc_Dict_Dict _tmp1D8;if((_tmp1C9.BottomFL).tag == 1){_LL1EE: _LL1EF:
 goto _LL1ED;}else{_LL1F0: _tmp1D8=((_tmp1C9.ReachableFL).val).f1;_LL1F1:
# 1182
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1D8,(void*)_tmp1DD->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp1CA=0;({unsigned int _tmp6FF=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp341))->hd)->loc;struct _dyneither_ptr _tmp6FE=({const char*_tmp1CB="expression may not be initialized";_tag_dyneither(_tmp1CB,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp6FF,_tmp6FE,_tag_dyneither(_tmp1CA,sizeof(void*),0));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1CC=_tmp1B4;struct Cyc_Dict_Dict _tmp1D7;struct Cyc_List_List*_tmp1D6;if((_tmp1CC.BottomFL).tag == 1){_LL1F3: _LL1F4:
 goto _LL1F2;}else{_LL1F5: _tmp1D7=((_tmp1CC.ReachableFL).val).f1;_tmp1D6=((_tmp1CC.ReachableFL).val).f2;_LL1F6: {
# 1189
struct Cyc_Dict_Dict _tmp1CD=Cyc_CfFlowInfo_escape_deref(_tmp14F,_tmp1D7,(void*)_tmp1DD->hd);
{void*_tmp1CE=(void*)_tmp1DD->hd;void*_tmp1CF=_tmp1CE;struct Cyc_CfFlowInfo_Place*_tmp1D5;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1CF)->tag == 5){_LL1F8: _tmp1D5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1CF)->f1;_LL1F9:
# 1192
{void*_tmp1D0=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp341))->hd)->topt));void*_tmp1D1=_tmp1D0;void*_tmp1D4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D1)->tag == 5){_LL1FD: _tmp1D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D1)->f1).elt_typ;_LL1FE:
# 1194
({struct Cyc_Dict_Dict _tmp704=({struct Cyc_CfFlowInfo_FlowEnv*_tmp703=_tmp14F;unsigned int _tmp702=((struct Cyc_Absyn_Exp*)_tmp341->hd)->loc;struct Cyc_Dict_Dict _tmp701=_tmp1CD;struct Cyc_CfFlowInfo_Place*_tmp700=_tmp1D5;Cyc_CfFlowInfo_assign_place(_tmp703,_tmp702,_tmp701,_tmp700,
Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,_tmp1D4,0,_tmp14F->esc_all));});
# 1194
_tmp1CD=_tmp704;});
# 1198
goto _LL1FC;}else{_LL1FF: _LL200:
({void*_tmp1D2=0;({struct _dyneither_ptr _tmp705=({const char*_tmp1D3="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1D3,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp705,_tag_dyneither(_tmp1D2,sizeof(void*),0));});});}_LL1FC:;}
# 1201
goto _LL1F7;}else{_LL1FA: _LL1FB:
 goto _LL1F7;}_LL1F7:;}
# 1204
({union Cyc_CfFlowInfo_FlowInfo _tmp706=Cyc_CfFlowInfo_ReachableFL(_tmp1CD,_tmp1D6);_tmp1B4=_tmp706;});
goto _LL1F2;}}_LL1F2:;}
# 1207
goto _LL1ED;}_LL1ED:;}else{
# 1210
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1215
({union Cyc_CfFlowInfo_FlowInfo _tmp70B=({struct Cyc_NewControlFlow_AnalEnv*_tmp70A=env;unsigned int _tmp709=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp341))->hd)->loc;void*_tmp708=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp341->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp707=_tmp1B4;Cyc_NewControlFlow_use_nounique_Rval(_tmp70A,_tmp709,_tmp708,_tmp707,(void*)_tmp1DD->hd);});_tmp1B4=_tmp70B;});else{
# 1225
({union Cyc_CfFlowInfo_FlowInfo _tmp70C=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp341))->hd)->loc,_tmp1B4,(void*)_tmp1DD->hd);_tmp1B4=_tmp70C;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp341->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp70D=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1B4,(struct Cyc_Absyn_Exp*)_tmp341->hd,((struct Cyc_Absyn_Exp*)_tmp341->hd)->loc,(void*)_tmp1DD->hd);_tmp1B4=_tmp70D;});}}}}
# 1232
Cyc_NewControlFlow_check_fn_requires(env,_tmp1B4,_tmp1B1,requires,e->loc);
# 1235
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp342->topt))){
struct _tuple16 _tmp1DA=({struct _tuple16 _tmp1D9;({union Cyc_CfFlowInfo_FlowInfo _tmp70E=Cyc_CfFlowInfo_BottomFL();_tmp1D9.f1=_tmp70E;});({void*_tmp70F=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp1D9.f2=_tmp70F;});_tmp1D9;});_npop_handler(0);return _tmp1DA;}else{
# 1238
struct _tuple16 _tmp1DC=({struct _tuple16 _tmp1DB;_tmp1DB.f1=_tmp1B4;({void*_tmp710=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp1DB.f2=_tmp710;});_tmp1DB;});_npop_handler(0);return _tmp1DC;}};};}
# 1140
;_pop_region(temp);}case 33: _LL173: _tmp347=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp167)->f1).is_calloc;_tmp346=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp167)->f1).rgn;_tmp345=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp167)->f1).elt_type;_tmp344=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp167)->f1).num_elts;_tmp343=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp167)->f1).fat_result;_LL174: {
# 1241
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1F1=_region_malloc(_tmp14F->r,sizeof(*_tmp1F1));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp711=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp1F2;_tmp1F2.tag=1;_tmp1F2.f1=_tmp344;_tmp1F2.f2=(void*)_check_null(e->topt);_tmp1F2;});_tmp1F1[0]=_tmp711;});_tmp1F1;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1F0=_region_malloc(_tmp14F->r,sizeof(*_tmp1F0));_tmp1F0->root=root;_tmp1F0->fields=0;_tmp1F0;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1EE=_region_malloc(_tmp14F->r,sizeof(*_tmp1EE));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp712=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp1EF;_tmp1EF.tag=5;_tmp1EF.f1=place;_tmp1EF;});_tmp1EE[0]=_tmp712;});_tmp1EE;});
void*place_val;
if(_tmp343)place_val=_tmp14F->notzeroall;else{
if(_tmp347)({void*_tmp713=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,*((void**)_check_null(_tmp345)),0,_tmp14F->zero);place_val=_tmp713;});else{
({void*_tmp714=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,*((void**)_check_null(_tmp345)),0,_tmp14F->unknown_none);place_val=_tmp714;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp346 != 0){
struct _RegionHandle _tmp1E0=_new_region("temp");struct _RegionHandle*temp=& _tmp1E0;_push_region(temp);
{struct _tuple23 _tmp1E1=({struct _RegionHandle*_tmp718=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp717=env;union Cyc_CfFlowInfo_FlowInfo _tmp716=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp718,_tmp717,_tmp716,({struct Cyc_Absyn_Exp*_tmp1E5[2];_tmp1E5[1]=_tmp344;_tmp1E5[0]=_tmp346;({struct _RegionHandle*_tmp715=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp715,_tag_dyneither(_tmp1E5,sizeof(struct Cyc_Absyn_Exp*),2));});}),1,1);});struct _tuple23 _tmp1E2=_tmp1E1;union Cyc_CfFlowInfo_FlowInfo _tmp1E4;struct Cyc_List_List*_tmp1E3;_LL202: _tmp1E4=_tmp1E2.f1;_tmp1E3=_tmp1E2.f2;_LL203:;
# 1253
outflow=_tmp1E4;}
# 1251
;_pop_region(temp);}else{
# 1256
struct _tuple16 _tmp1E6=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp344);struct _tuple16 _tmp1E7=_tmp1E6;union Cyc_CfFlowInfo_FlowInfo _tmp1E8;_LL205: _tmp1E8=_tmp1E7.f1;_LL206:;
outflow=_tmp1E8;}{
# 1259
union Cyc_CfFlowInfo_FlowInfo _tmp1E9=outflow;struct Cyc_Dict_Dict _tmp1ED;struct Cyc_List_List*_tmp1EC;if((_tmp1E9.BottomFL).tag == 1){_LL208: _LL209:
 return({struct _tuple16 _tmp1EA;_tmp1EA.f1=outflow;_tmp1EA.f2=rval;_tmp1EA;});}else{_LL20A: _tmp1ED=((_tmp1E9.ReachableFL).val).f1;_tmp1EC=((_tmp1E9.ReachableFL).val).f2;_LL20B:
# 1262
 return({struct _tuple16 _tmp1EB;({union Cyc_CfFlowInfo_FlowInfo _tmp71A=({struct Cyc_Dict_Dict _tmp719=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1ED,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp719,_tmp1EC);});_tmp1EB.f1=_tmp71A;});_tmp1EB.f2=rval;_tmp1EB;});}_LL207:;};};}case 34: _LL175: _tmp349=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp348=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL176: {
# 1266
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1272
struct _RegionHandle _tmp1F3=_new_region("temp");struct _RegionHandle*temp=& _tmp1F3;_push_region(temp);{
struct _tuple23 _tmp1F4=({struct _RegionHandle*_tmp71E=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp71D=env;union Cyc_CfFlowInfo_FlowInfo _tmp71C=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp71E,_tmp71D,_tmp71C,({struct Cyc_Absyn_Exp*_tmp1F8[2];_tmp1F8[1]=_tmp348;_tmp1F8[0]=_tmp349;({struct _RegionHandle*_tmp71B=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp71B,_tag_dyneither(_tmp1F8,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,0);});struct _tuple23 _tmp1F5=_tmp1F4;union Cyc_CfFlowInfo_FlowInfo _tmp1F7;struct Cyc_List_List*_tmp1F6;_LL20D: _tmp1F7=_tmp1F5.f1;_tmp1F6=_tmp1F5.f2;_LL20E:;
# 1275
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1F6))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1F6->tl))->hd;
outflow=_tmp1F7;}{
# 1280
struct _tuple18 _tmp1F9=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp349);struct _tuple18 _tmp1FA=_tmp1F9;union Cyc_CfFlowInfo_AbsLVal _tmp20B;_LL210: _tmp20B=_tmp1FA.f2;_LL211:;{
struct _tuple18 _tmp1FB=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp348);struct _tuple18 _tmp1FC=_tmp1FB;union Cyc_CfFlowInfo_AbsLVal _tmp20A;_LL213: _tmp20A=_tmp1FC.f2;_LL214:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1FD=outflow;struct Cyc_Dict_Dict _tmp207;struct Cyc_List_List*_tmp206;if((_tmp1FD.ReachableFL).tag == 2){_LL216: _tmp207=((_tmp1FD.ReachableFL).val).f1;_tmp206=((_tmp1FD.ReachableFL).val).f2;_LL217:
# 1284
{union Cyc_CfFlowInfo_AbsLVal _tmp1FE=_tmp20B;struct Cyc_CfFlowInfo_Place*_tmp201;if((_tmp1FE.PlaceL).tag == 1){_LL21B: _tmp201=(_tmp1FE.PlaceL).val;_LL21C:
# 1286
({struct Cyc_Dict_Dict _tmp71F=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp349->loc,_tmp207,_tmp201,right_rval);_tmp207=_tmp71F;});
goto _LL21A;}else{_LL21D: _LL21E:
# 1292
 if(Cyc_CfFlowInfo_initlevel(_tmp14F,_tmp207,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1FF=0;({unsigned int _tmp721=_tmp348->loc;struct _dyneither_ptr _tmp720=({const char*_tmp200="expression may not be fully initialized";_tag_dyneither(_tmp200,sizeof(char),40);});Cyc_Tcutil_terr(_tmp721,_tmp720,_tag_dyneither(_tmp1FF,sizeof(void*),0));});});
goto _LL21A;}_LL21A:;}
# 1296
{union Cyc_CfFlowInfo_AbsLVal _tmp202=_tmp20A;struct Cyc_CfFlowInfo_Place*_tmp205;if((_tmp202.PlaceL).tag == 1){_LL220: _tmp205=(_tmp202.PlaceL).val;_LL221:
# 1298
({struct Cyc_Dict_Dict _tmp722=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp348->loc,_tmp207,_tmp205,left_rval);_tmp207=_tmp722;});
goto _LL21F;}else{_LL222: _LL223:
# 1301
 if(Cyc_CfFlowInfo_initlevel(_tmp14F,_tmp207,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp203=0;({unsigned int _tmp724=_tmp349->loc;struct _dyneither_ptr _tmp723=({const char*_tmp204="expression may not be fully initialized";_tag_dyneither(_tmp204,sizeof(char),40);});Cyc_Tcutil_terr(_tmp724,_tmp723,_tag_dyneither(_tmp203,sizeof(void*),0));});});
goto _LL21F;}_LL21F:;}
# 1306
({struct Cyc_List_List*_tmp725=Cyc_Relations_reln_kill_exp(_tmp14F->r,_tmp206,_tmp349);_tmp206=_tmp725;});
({struct Cyc_List_List*_tmp726=Cyc_Relations_reln_kill_exp(_tmp14F->r,_tmp206,_tmp348);_tmp206=_tmp726;});
# 1309
({union Cyc_CfFlowInfo_FlowInfo _tmp727=Cyc_CfFlowInfo_ReachableFL(_tmp207,_tmp206);outflow=_tmp727;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL215;}else{_LL218: _LL219:
 goto _LL215;}_LL215:;}{
# 1316
struct _tuple16 _tmp209=({struct _tuple16 _tmp208;_tmp208.f1=outflow;_tmp208.f2=_tmp14F->unknown_all;_tmp208;});_npop_handler(0);return _tmp209;};};};
# 1272
;_pop_region(temp);}case 15: _LL177: _tmp34B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp34A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL178: {
# 1319
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp21E=_region_malloc(_tmp14F->r,sizeof(*_tmp21E));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp728=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp21F;_tmp21F.tag=1;_tmp21F.f1=_tmp34A;_tmp21F.f2=(void*)_check_null(e->topt);_tmp21F;});_tmp21E[0]=_tmp728;});_tmp21E;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp21D=_region_malloc(_tmp14F->r,sizeof(*_tmp21D));_tmp21D->root=root;_tmp21D->fields=0;_tmp21D;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp21B=_region_malloc(_tmp14F->r,sizeof(*_tmp21B));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp729=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp21C;_tmp21C.tag=5;_tmp21C.f1=place;_tmp21C;});_tmp21B[0]=_tmp729;});_tmp21B;});
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp34B != 0){
struct _RegionHandle _tmp20C=_new_region("temp");struct _RegionHandle*temp=& _tmp20C;_push_region(temp);
{struct _tuple23 _tmp20D=({struct _RegionHandle*_tmp72D=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp72C=env;union Cyc_CfFlowInfo_FlowInfo _tmp72B=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp72D,_tmp72C,_tmp72B,({struct Cyc_Absyn_Exp*_tmp211[2];_tmp211[1]=_tmp34A;_tmp211[0]=_tmp34B;({struct _RegionHandle*_tmp72A=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp72A,_tag_dyneither(_tmp211,sizeof(struct Cyc_Absyn_Exp*),2));});}),1,1);});struct _tuple23 _tmp20E=_tmp20D;union Cyc_CfFlowInfo_FlowInfo _tmp210;struct Cyc_List_List*_tmp20F;_LL225: _tmp210=_tmp20E.f1;_tmp20F=_tmp20E.f2;_LL226:;
# 1328
outflow=_tmp210;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp20F))->tl))->hd;}
# 1326
;_pop_region(temp);}else{
# 1332
struct _tuple16 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp34A);struct _tuple16 _tmp213=_tmp212;union Cyc_CfFlowInfo_FlowInfo _tmp215;void*_tmp214;_LL228: _tmp215=_tmp213.f1;_tmp214=_tmp213.f2;_LL229:;
outflow=_tmp215;
place_val=_tmp214;}{
# 1336
union Cyc_CfFlowInfo_FlowInfo _tmp216=outflow;struct Cyc_Dict_Dict _tmp21A;struct Cyc_List_List*_tmp219;if((_tmp216.BottomFL).tag == 1){_LL22B: _LL22C:
 return({struct _tuple16 _tmp217;_tmp217.f1=outflow;_tmp217.f2=rval;_tmp217;});}else{_LL22D: _tmp21A=((_tmp216.ReachableFL).val).f1;_tmp219=((_tmp216.ReachableFL).val).f2;_LL22E:
# 1339
 return({struct _tuple16 _tmp218;({union Cyc_CfFlowInfo_FlowInfo _tmp72F=({struct Cyc_Dict_Dict _tmp72E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp21A,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp72E,_tmp219);});_tmp218.f1=_tmp72F;});_tmp218.f2=rval;_tmp218;});}_LL22A:;};}case 14: _LL179: _tmp34C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL17A: {
# 1343
struct _tuple18 _tmp220=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp34C);struct _tuple18 _tmp221=_tmp220;union Cyc_CfFlowInfo_FlowInfo _tmp229;union Cyc_CfFlowInfo_AbsLVal _tmp228;_LL230: _tmp229=_tmp221.f1;_tmp228=_tmp221.f2;_LL231:;{
union Cyc_CfFlowInfo_AbsLVal _tmp222=_tmp228;struct Cyc_CfFlowInfo_Place*_tmp227;if((_tmp222.UnknownL).tag == 2){_LL233: _LL234:
 return({struct _tuple16 _tmp223;_tmp223.f1=_tmp229;_tmp223.f2=_tmp14F->notzeroall;_tmp223;});}else{_LL235: _tmp227=(_tmp222.PlaceL).val;_LL236:
 return({struct _tuple16 _tmp224;_tmp224.f1=_tmp229;({void*_tmp731=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp225=_region_malloc(env->r,sizeof(*_tmp225));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp730=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp226;_tmp226.tag=5;_tmp226.f1=_tmp227;_tmp226;});_tmp225[0]=_tmp730;});_tmp225;});_tmp224.f2=_tmp731;});_tmp224;});}_LL232:;};}case 19: _LL17B: _tmp34D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL17C: {
# 1350
struct _tuple16 _tmp22A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp34D);struct _tuple16 _tmp22B=_tmp22A;union Cyc_CfFlowInfo_FlowInfo _tmp22D;void*_tmp22C;_LL238: _tmp22D=_tmp22B.f1;_tmp22C=_tmp22B.f2;_LL239:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp22D,_tmp34D,_tmp22C);}case 20: _LL17D: _tmp34F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp34E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL17E: {
# 1354
struct _tuple16 _tmp22E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp34F);struct _tuple16 _tmp22F=_tmp22E;union Cyc_CfFlowInfo_FlowInfo _tmp24C;void*_tmp24B;_LL23B: _tmp24C=_tmp22F.f1;_tmp24B=_tmp22F.f2;_LL23C:;
if(_tmp34F->topt == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp232;_tmp232.tag=0;({struct _dyneither_ptr _tmp732=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp232.f1=_tmp732;});({void*_tmp230[1]={& _tmp232};({struct Cyc___cycFILE*_tmp734=Cyc_stderr;struct _dyneither_ptr _tmp733=({const char*_tmp231="aggrmember exp %s\n";_tag_dyneither(_tmp231,sizeof(char),19);});Cyc_fprintf(_tmp734,_tmp733,_tag_dyneither(_tmp230,sizeof(void*),1));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp235;_tmp235.tag=0;({struct _dyneither_ptr _tmp735=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp34F));_tmp235.f1=_tmp735;});({void*_tmp233[1]={& _tmp235};({struct Cyc___cycFILE*_tmp737=Cyc_stderr;struct _dyneither_ptr _tmp736=({const char*_tmp234="oops! %s.topt = null!\n";_tag_dyneither(_tmp234,sizeof(char),23);});Cyc_fprintf(_tmp737,_tmp736,_tag_dyneither(_tmp233,sizeof(void*),1));});});});}{
# 1359
void*_tmp236=(void*)_check_null(_tmp34F->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp236))
# 1362
return({struct _tuple16 _tmp237;_tmp237.f1=_tmp24C;({void*_tmp738=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp237.f2=_tmp738;});_tmp237;});
# 1364
if(Cyc_Absyn_is_require_union_type(_tmp236))
Cyc_NewControlFlow_check_union_requires(_tmp34F->loc,_tmp14F->r,_tmp236,_tmp34E,_tmp24C);{
# 1367
struct _tuple15 _tmp238=Cyc_CfFlowInfo_unname_rval(_tmp14F->r,_tmp24B);struct _tuple15 _tmp239=_tmp238;void*_tmp24A;_LL23E: _tmp24A=_tmp239.f1;_LL23F:;{
void*_tmp23A=_tmp24A;int _tmp249;int _tmp248;struct _dyneither_ptr _tmp247;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23A)->tag == 6){_LL241: _tmp249=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23A)->f1).is_union;_tmp248=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23A)->f1).fieldnum;_tmp247=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23A)->f2;_LL242: {
# 1370
int _tmp23B=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp34F->topt),_tmp34E);
if((_tmp249  && _tmp248 != - 1) && _tmp248 != _tmp23B)
return({struct _tuple16 _tmp23C;_tmp23C.f1=_tmp24C;({void*_tmp739=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),1,_tmp14F->unknown_none);_tmp23C.f2=_tmp739;});_tmp23C;});
return({struct _tuple16 _tmp23D;_tmp23D.f1=_tmp24C;_tmp23D.f2=*((void**)_check_dyneither_subscript(_tmp247,sizeof(void*),_tmp23B));_tmp23D;});}}else{_LL243: _LL244:
# 1375
({void*_tmp23E=0;({struct Cyc___cycFILE*_tmp73B=Cyc_stderr;struct _dyneither_ptr _tmp73A=({const char*_tmp23F="the bad rexp is :";_tag_dyneither(_tmp23F,sizeof(char),18);});Cyc_fprintf(_tmp73B,_tmp73A,_tag_dyneither(_tmp23E,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(_tmp24A);
({void*_tmp240=0;({struct Cyc___cycFILE*_tmp73D=Cyc_stderr;struct _dyneither_ptr _tmp73C=({const char*_tmp241="\n";_tag_dyneither(_tmp241,sizeof(char),2);});Cyc_fprintf(_tmp73D,_tmp73C,_tag_dyneither(_tmp240,sizeof(void*),0));});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp242=_cycalloc(sizeof(*_tmp242));({struct Cyc_Core_Impossible_exn_struct _tmp741=({struct Cyc_Core_Impossible_exn_struct _tmp243;_tmp243.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp740=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp246;_tmp246.tag=0;({struct _dyneither_ptr _tmp73E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp246.f1=_tmp73E;});({void*_tmp244[1]={& _tmp246};({struct _dyneither_ptr _tmp73F=({const char*_tmp245="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp245,sizeof(char),26);});Cyc_aprintf(_tmp73F,_tag_dyneither(_tmp244,sizeof(void*),1));});});});_tmp243.f1=_tmp740;});_tmp243;});_tmp242[0]=_tmp741;});_tmp242;}));}_LL240:;};};};}case 37: _LL17F: _tmp351=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp350=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL180: {
# 1385
struct _tuple16 _tmp24D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp351);struct _tuple16 _tmp24E=_tmp24D;union Cyc_CfFlowInfo_FlowInfo _tmp261;void*_tmp260;_LL246: _tmp261=_tmp24E.f1;_tmp260=_tmp24E.f2;_LL247:;
# 1387
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp351->topt)))
return({struct _tuple16 _tmp24F;_tmp24F.f1=_tmp261;_tmp24F.f2=_tmp14F->unknown_all;_tmp24F;});{
struct _tuple15 _tmp250=Cyc_CfFlowInfo_unname_rval(_tmp14F->r,_tmp260);struct _tuple15 _tmp251=_tmp250;void*_tmp25F;_LL249: _tmp25F=_tmp251.f1;_LL24A:;{
void*_tmp252=_tmp25F;int _tmp25E;int _tmp25D;struct _dyneither_ptr _tmp25C;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->tag == 6){_LL24C: _tmp25E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->f1).is_union;_tmp25D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->f1).fieldnum;_tmp25C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->f2;_LL24D: {
# 1392
int _tmp253=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp351->topt),_tmp350);
if(_tmp25E  && _tmp25D != - 1){
if(_tmp25D != _tmp253)
return({struct _tuple16 _tmp254;_tmp254.f1=_tmp261;_tmp254.f2=_tmp14F->zero;_tmp254;});else{
# 1397
return({struct _tuple16 _tmp255;_tmp255.f1=_tmp261;_tmp255.f2=_tmp14F->notzeroall;_tmp255;});}}else{
# 1399
return({struct _tuple16 _tmp256;_tmp256.f1=_tmp261;_tmp256.f2=_tmp14F->unknown_all;_tmp256;});}}}else{_LL24E: _LL24F:
# 1401
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp257=_cycalloc(sizeof(*_tmp257));({struct Cyc_Core_Impossible_exn_struct _tmp745=({struct Cyc_Core_Impossible_exn_struct _tmp258;_tmp258.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp744=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp25B;_tmp25B.tag=0;({struct _dyneither_ptr _tmp742=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp25B.f1=_tmp742;});({void*_tmp259[1]={& _tmp25B};({struct _dyneither_ptr _tmp743=({const char*_tmp25A="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp25A,sizeof(char),26);});Cyc_aprintf(_tmp743,_tag_dyneither(_tmp259,sizeof(void*),1));});});});_tmp258.f1=_tmp744;});_tmp258;});_tmp257[0]=_tmp745;});_tmp257;}));}_LL24B:;};};}case 21: _LL181: _tmp353=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp352=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL182: {
# 1406
struct _tuple16 _tmp262=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp353);struct _tuple16 _tmp263=_tmp262;union Cyc_CfFlowInfo_FlowInfo _tmp27D;void*_tmp27C;_LL251: _tmp27D=_tmp263.f1;_tmp27C=_tmp263.f2;_LL252:;{
# 1409
struct _tuple16 _tmp264=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp27D,_tmp353,_tmp27C);struct _tuple16 _tmp265=_tmp264;union Cyc_CfFlowInfo_FlowInfo _tmp27B;void*_tmp27A;_LL254: _tmp27B=_tmp265.f1;_tmp27A=_tmp265.f2;_LL255:;{
# 1412
void*_tmp266=Cyc_Tcutil_compress((void*)_check_null(_tmp353->topt));void*_tmp267=_tmp266;void*_tmp279;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp267)->tag == 5){_LL257: _tmp279=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp267)->f1).elt_typ;_LL258:
# 1414
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp279))
# 1416
return({struct _tuple16 _tmp268;_tmp268.f1=_tmp27B;({void*_tmp746=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp268.f2=_tmp746;});_tmp268;});
# 1418
if(Cyc_Absyn_is_require_union_type(_tmp279))
Cyc_NewControlFlow_check_union_requires(_tmp353->loc,_tmp14F->r,_tmp279,_tmp352,_tmp27D);{
# 1421
struct _tuple15 _tmp269=Cyc_CfFlowInfo_unname_rval(_tmp14F->r,_tmp27A);struct _tuple15 _tmp26A=_tmp269;void*_tmp275;_LL25C: _tmp275=_tmp26A.f1;_LL25D:;{
void*_tmp26B=_tmp275;int _tmp274;int _tmp273;struct _dyneither_ptr _tmp272;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26B)->tag == 6){_LL25F: _tmp274=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26B)->f1).is_union;_tmp273=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26B)->f1).fieldnum;_tmp272=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26B)->f2;_LL260: {
# 1424
int _tmp26C=Cyc_CfFlowInfo_get_field_index(_tmp279,_tmp352);
if((_tmp274  && _tmp273 != - 1) && _tmp273 != _tmp26C)
return({struct _tuple16 _tmp26D;_tmp26D.f1=_tmp27B;({void*_tmp747=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),1,_tmp14F->unknown_none);_tmp26D.f2=_tmp747;});_tmp26D;});
return({struct _tuple16 _tmp26E;_tmp26E.f1=_tmp27B;_tmp26E.f2=*((void**)_check_dyneither_subscript(_tmp272,sizeof(void*),_tmp26C));_tmp26E;});}}else{_LL261: _LL262:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));({struct Cyc_Core_Impossible_exn_struct _tmp749=({struct Cyc_Core_Impossible_exn_struct _tmp270;_tmp270.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp748=({const char*_tmp271="anal_Rexp: AggrArrow";_tag_dyneither(_tmp271,sizeof(char),21);});_tmp270.f1=_tmp748;});_tmp270;});_tmp26F[0]=_tmp749;});_tmp26F;}));}_LL25E:;};};}else{_LL259: _LL25A:
# 1430
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp276=_cycalloc(sizeof(*_tmp276));({struct Cyc_Core_Impossible_exn_struct _tmp74B=({struct Cyc_Core_Impossible_exn_struct _tmp277;_tmp277.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp74A=({const char*_tmp278="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp278,sizeof(char),25);});_tmp277.f1=_tmp74A;});_tmp277;});_tmp276[0]=_tmp74B;});_tmp276;}));}_LL256:;};};}case 5: _LL183: _tmp356=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp355=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_tmp354=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp167)->f3;_LL184: {
# 1434
struct _tuple19 _tmp27E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp356);struct _tuple19 _tmp27F=_tmp27E;union Cyc_CfFlowInfo_FlowInfo _tmp283;union Cyc_CfFlowInfo_FlowInfo _tmp282;_LL264: _tmp283=_tmp27F.f1;_tmp282=_tmp27F.f2;_LL265:;{
struct _tuple16 _tmp280=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp283,_tmp355);
struct _tuple16 _tmp281=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp282,_tmp354);
# 1438
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14F,_tmp280,_tmp281);};}case 6: _LL185: _tmp358=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp357=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL186: {
# 1441
struct _tuple19 _tmp284=Cyc_NewControlFlow_anal_test(env,inflow,_tmp358);struct _tuple19 _tmp285=_tmp284;union Cyc_CfFlowInfo_FlowInfo _tmp28F;union Cyc_CfFlowInfo_FlowInfo _tmp28E;_LL267: _tmp28F=_tmp285.f1;_tmp28E=_tmp285.f2;_LL268:;{
struct _tuple16 _tmp286=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp28F,_tmp357);struct _tuple16 _tmp287=_tmp286;union Cyc_CfFlowInfo_FlowInfo _tmp28D;void*_tmp28C;_LL26A: _tmp28D=_tmp287.f1;_tmp28C=_tmp287.f2;_LL26B:;{
struct _tuple16 _tmp288=({struct _tuple16 _tmp28B;_tmp28B.f1=_tmp28D;_tmp28B.f2=_tmp28C;_tmp28B;});
struct _tuple16 _tmp289=({struct _tuple16 _tmp28A;_tmp28A.f1=_tmp28E;_tmp28A.f2=_tmp14F->zero;_tmp28A;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14F,_tmp288,_tmp289);};};}case 7: _LL187: _tmp35A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp359=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL188: {
# 1448
struct _tuple19 _tmp290=Cyc_NewControlFlow_anal_test(env,inflow,_tmp35A);struct _tuple19 _tmp291=_tmp290;union Cyc_CfFlowInfo_FlowInfo _tmp29B;union Cyc_CfFlowInfo_FlowInfo _tmp29A;_LL26D: _tmp29B=_tmp291.f1;_tmp29A=_tmp291.f2;_LL26E:;{
struct _tuple16 _tmp292=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29A,_tmp359);struct _tuple16 _tmp293=_tmp292;union Cyc_CfFlowInfo_FlowInfo _tmp299;void*_tmp298;_LL270: _tmp299=_tmp293.f1;_tmp298=_tmp293.f2;_LL271:;{
struct _tuple16 _tmp294=({struct _tuple16 _tmp297;_tmp297.f1=_tmp299;_tmp297.f2=_tmp298;_tmp297;});
struct _tuple16 _tmp295=({struct _tuple16 _tmp296;_tmp296.f1=_tmp29B;_tmp296.f2=_tmp14F->notzeroall;_tmp296;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14F,_tmp294,_tmp295);};};}case 22: _LL189: _tmp35C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp35B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL18A: {
# 1455
struct _RegionHandle _tmp29C=_new_region("temp");struct _RegionHandle*temp=& _tmp29C;_push_region(temp);
{struct _tuple23 _tmp29D=({struct _RegionHandle*_tmp74F=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp74E=env;union Cyc_CfFlowInfo_FlowInfo _tmp74D=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp74F,_tmp74E,_tmp74D,({struct Cyc_Absyn_Exp*_tmp2C2[2];_tmp2C2[1]=_tmp35B;_tmp2C2[0]=_tmp35C;({struct _RegionHandle*_tmp74C=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp74C,_tag_dyneither(_tmp2C2,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,1);});struct _tuple23 _tmp29E=_tmp29D;union Cyc_CfFlowInfo_FlowInfo _tmp2C1;struct Cyc_List_List*_tmp2C0;_LL273: _tmp2C1=_tmp29E.f1;_tmp2C0=_tmp29E.f2;_LL274:;{
# 1461
union Cyc_CfFlowInfo_FlowInfo _tmp29F=_tmp2C1;
{union Cyc_CfFlowInfo_FlowInfo _tmp2A0=_tmp2C1;struct Cyc_Dict_Dict _tmp2A5;struct Cyc_List_List*_tmp2A4;if((_tmp2A0.ReachableFL).tag == 2){_LL276: _tmp2A5=((_tmp2A0.ReachableFL).val).f1;_tmp2A4=((_tmp2A0.ReachableFL).val).f2;_LL277:
# 1466
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2A5,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C0))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2A1=0;({unsigned int _tmp751=_tmp35B->loc;struct _dyneither_ptr _tmp750=({const char*_tmp2A2="expression may not be initialized";_tag_dyneither(_tmp2A2,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp751,_tmp750,_tag_dyneither(_tmp2A1,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp2A3=Cyc_NewControlFlow_add_subscript_reln(_tmp14F->r,_tmp2A4,_tmp35C,_tmp35B);
if(_tmp2A4 != _tmp2A3)
({union Cyc_CfFlowInfo_FlowInfo _tmp752=Cyc_CfFlowInfo_ReachableFL(_tmp2A5,_tmp2A3);_tmp29F=_tmp752;});
goto _LL275;};}else{_LL278: _LL279:
 goto _LL275;}_LL275:;}{
# 1474
void*_tmp2A6=Cyc_Tcutil_compress((void*)_check_null(_tmp35C->topt));void*_tmp2A7=_tmp2A6;union Cyc_Absyn_Constraint*_tmp2BF;struct Cyc_List_List*_tmp2BE;switch(*((int*)_tmp2A7)){case 10: _LL27B: _tmp2BE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2A7)->f1;_LL27C: {
# 1476
struct _tuple15 _tmp2A8=Cyc_CfFlowInfo_unname_rval(_tmp14F->r,(void*)((struct Cyc_List_List*)_check_null(_tmp2C0))->hd);struct _tuple15 _tmp2A9=_tmp2A8;void*_tmp2B1;_LL282: _tmp2B1=_tmp2A9.f1;_LL283:;{
void*_tmp2AA=_tmp2B1;struct _dyneither_ptr _tmp2B0;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AA)->tag == 6){_LL285: _tmp2B0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AA)->f2;_LL286: {
# 1479
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp35B)).f1;
struct _tuple16 _tmp2AC=({struct _tuple16 _tmp2AB;_tmp2AB.f1=_tmp29F;_tmp2AB.f2=*((void**)_check_dyneither_subscript(_tmp2B0,sizeof(void*),(int)i));_tmp2AB;});_npop_handler(0);return _tmp2AC;}}else{_LL287: _LL288:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));({struct Cyc_Core_Impossible_exn_struct _tmp754=({struct Cyc_Core_Impossible_exn_struct _tmp2AE;_tmp2AE.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp753=({const char*_tmp2AF="anal_Rexp: Subscript";_tag_dyneither(_tmp2AF,sizeof(char),21);});_tmp2AE.f1=_tmp753;});_tmp2AE;});_tmp2AD[0]=_tmp754;});_tmp2AD;}));}_LL284:;};}case 5: _LL27D: _tmp2BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1).ptr_atts).bounds;_LL27E: {
# 1484
struct _tuple16 _tmp2B2=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C1,_tmp35C,(void*)((struct Cyc_List_List*)_check_null(_tmp2C0))->hd);struct _tuple16 _tmp2B3=_tmp2B2;union Cyc_CfFlowInfo_FlowInfo _tmp2BA;void*_tmp2B9;_LL28A: _tmp2BA=_tmp2B3.f1;_tmp2B9=_tmp2B3.f2;_LL28B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2B4=_tmp2BA;if((_tmp2B4.BottomFL).tag == 1){_LL28D: _LL28E: {
struct _tuple16 _tmp2B6=({struct _tuple16 _tmp2B5;_tmp2B5.f1=_tmp2BA;_tmp2B5.f2=_tmp2B9;_tmp2B5;});_npop_handler(0);return _tmp2B6;}}else{_LL28F: _LL290: {
struct _tuple16 _tmp2B8=({struct _tuple16 _tmp2B7;_tmp2B7.f1=_tmp29F;_tmp2B7.f2=_tmp2B9;_tmp2B7;});_npop_handler(0);return _tmp2B8;}}_LL28C:;};}default: _LL27F: _LL280:
# 1489
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));({struct Cyc_Core_Impossible_exn_struct _tmp756=({struct Cyc_Core_Impossible_exn_struct _tmp2BC;_tmp2BC.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp755=({const char*_tmp2BD="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2BD,sizeof(char),33);});_tmp2BC.f1=_tmp755;});_tmp2BC;});_tmp2BB[0]=_tmp756;});_tmp2BB;}));}_LL27A:;};};}
# 1456
;_pop_region(temp);}case 23: _LL18D: _tmp35D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL18E: {
# 1496
struct _RegionHandle _tmp2C3=_new_region("temp");struct _RegionHandle*temp=& _tmp2C3;_push_region(temp);
{struct _tuple23 _tmp2C4=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp35D,1,1);struct _tuple23 _tmp2C5=_tmp2C4;union Cyc_CfFlowInfo_FlowInfo _tmp2D1;struct Cyc_List_List*_tmp2D0;_LL292: _tmp2D1=_tmp2C5.f1;_tmp2D0=_tmp2C5.f2;_LL293:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2CB=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp35D);void**_tmp2CC=(void**)({struct _RegionHandle*_tmp757=env->r;_region_malloc(_tmp757,_check_times(sizeof(void*),_tmp2CB));});struct _dyneither_ptr _tmp2CF=_tag_dyneither(_tmp2CC,sizeof(void*),_tmp2CB);{unsigned int _tmp2CD=_tmp2CB;unsigned int i;for(i=0;i < _tmp2CD;i ++){({typeof((void*)({
void*_tmp2CE=(void*)((struct Cyc_List_List*)_check_null(_tmp2D0))->hd;
_tmp2D0=_tmp2D0->tl;
_tmp2CE;}))_tmp758=(void*)({
# 1500
void*_tmp2CE=(void*)((struct Cyc_List_List*)_check_null(_tmp2D0))->hd;
_tmp2D0=_tmp2D0->tl;
_tmp2CE;});_tmp2CC[i]=_tmp758;});}}_tmp2CF;});
# 1504
struct _tuple16 _tmp2CA=({struct _tuple16 _tmp2C6;_tmp2C6.f1=_tmp2D1;({void*_tmp75B=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C7=_region_malloc(env->r,sizeof(*_tmp2C7));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp75A=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2C8;_tmp2C8.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp759=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2C9;_tmp2C9.is_union=0;_tmp2C9.fieldnum=- 1;_tmp2C9;});_tmp2C8.f1=_tmp759;});_tmp2C8.f2=aggrdict;_tmp2C8;});_tmp2C7[0]=_tmp75A;});_tmp2C7;});_tmp2C6.f2=_tmp75B;});_tmp2C6;});_npop_handler(0);return _tmp2CA;};}
# 1497
;_pop_region(temp);}case 29: _LL18F: _tmp35E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_LL190: {
# 1507
struct Cyc_List_List*fs;
{void*_tmp2D2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2D3=_tmp2D2;struct Cyc_List_List*_tmp2D7;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3)->tag == 12){_LL295: _tmp2D7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3)->f2;_LL296:
 fs=_tmp2D7;goto _LL294;}else{_LL297: _LL298:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));({struct Cyc_Core_Impossible_exn_struct _tmp75D=({struct Cyc_Core_Impossible_exn_struct _tmp2D5;_tmp2D5.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp75C=({const char*_tmp2D6="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2D6,sizeof(char),35);});_tmp2D5.f1=_tmp75C;});_tmp2D5;});_tmp2D4[0]=_tmp75D;});_tmp2D4;}));}_LL294:;}
# 1512
_tmp361=_tmp35E;_tmp360=Cyc_Absyn_StructA;_tmp35F=fs;goto _LL192;}case 28: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp167)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp167)->f4)->impl != 0){_LL191: _tmp361=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp167)->f3;_tmp360=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp167)->f4)->kind;_tmp35F=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp167)->f4)->impl)->fields;_LL192: {
# 1514
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp2D8=_new_region("temp");struct _RegionHandle*temp=& _tmp2D8;_push_region(temp);
{struct _tuple23 _tmp2D9=({struct _RegionHandle*_tmp760=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp75F=env;union Cyc_CfFlowInfo_FlowInfo _tmp75E=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp760,_tmp75F,_tmp75E,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp361),1,1);});
# 1516
struct _tuple23 _tmp2DA=_tmp2D9;union Cyc_CfFlowInfo_FlowInfo _tmp2E9;struct Cyc_List_List*_tmp2E8;_LL29A: _tmp2E9=_tmp2DA.f1;_tmp2E8=_tmp2DA.f2;_LL29B:;{
# 1518
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp14F,_tmp35F,_tmp360 == Cyc_Absyn_UnionA,_tmp14F->unknown_all);
# 1521
int field_no=-1;
{int i=0;for(0;_tmp2E8 != 0;(((_tmp2E8=_tmp2E8->tl,_tmp361=_tmp361->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp361))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2DB=(void*)ds->hd;void*_tmp2DC=_tmp2DB;struct _dyneither_ptr*_tmp2E1;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DC)->tag == 0){_LL29D: _LL29E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));({struct Cyc_Core_Impossible_exn_struct _tmp762=({struct Cyc_Core_Impossible_exn_struct _tmp2DE;_tmp2DE.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp761=({const char*_tmp2DF="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2DF,sizeof(char),22);});_tmp2DE.f1=_tmp761;});_tmp2DE;});_tmp2DD[0]=_tmp762;});_tmp2DD;}));}else{_LL29F: _tmp2E1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2DC)->f1;_LL2A0:
# 1528
({int _tmp763=Cyc_CfFlowInfo_get_field_index_fs(_tmp35F,_tmp2E1);field_no=_tmp763;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp2E8->hd;
# 1531
if(_tmp360 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2E0=(*((struct _tuple26*)_tmp361->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp764=Cyc_NewControlFlow_use_Rval(env,_tmp2E0->loc,_tmp2E9,(void*)_tmp2E8->hd);_tmp2E9=_tmp764;});
# 1535
Cyc_NewControlFlow_check_union_requires(_tmp2E0->loc,_tmp14F->r,exp_type,_tmp2E1,_tmp2E9);}}_LL29C:;}}}{
# 1538
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E2=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E5=_region_malloc(env->r,sizeof(*_tmp2E5));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp766=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E6;_tmp2E6.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp765=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E7;_tmp2E7.is_union=_tmp360 == Cyc_Absyn_UnionA;_tmp2E7.fieldnum=field_no;_tmp2E7;});_tmp2E6.f1=_tmp765;});_tmp2E6.f2=aggrdict;_tmp2E6;});_tmp2E5[0]=_tmp766;});_tmp2E5;});
struct _tuple16 _tmp2E4=({struct _tuple16 _tmp2E3;_tmp2E3.f1=_tmp2E9;_tmp2E3.f2=(void*)_tmp2E2;_tmp2E3;});_npop_handler(0);return _tmp2E4;};};}
# 1516
;_pop_region(temp);}}else{goto _LL193;}}else{_LL193: _LL194:
# 1542
({struct Cyc_String_pa_PrintArg_struct _tmp2EC;_tmp2EC.tag=0;({struct _dyneither_ptr _tmp767=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp2EC.f1=_tmp767;});({void*_tmp2EA[1]={& _tmp2EC};({struct _dyneither_ptr _tmp768=({const char*_tmp2EB="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp2EB,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp768,_tag_dyneither(_tmp2EA,sizeof(void*),1));});});});}case 25: _LL195: _tmp362=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL196: {
# 1544
struct _RegionHandle _tmp2ED=_new_region("temp");struct _RegionHandle*temp=& _tmp2ED;_push_region(temp);
{struct Cyc_List_List*_tmp2EE=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp362);
struct _tuple23 _tmp2EF=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp2EE,1,1);struct _tuple23 _tmp2F0=_tmp2EF;union Cyc_CfFlowInfo_FlowInfo _tmp2F4;struct Cyc_List_List*_tmp2F3;_LL2A2: _tmp2F4=_tmp2F0.f1;_tmp2F3=_tmp2F0.f2;_LL2A3:;
for(0;_tmp2F3 != 0;(_tmp2F3=_tmp2F3->tl,_tmp2EE=_tmp2EE->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp769=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2EE))->hd)->loc,_tmp2F4,(void*)_tmp2F3->hd);_tmp2F4=_tmp769;});}{
struct _tuple16 _tmp2F2=({struct _tuple16 _tmp2F1;_tmp2F1.f1=_tmp2F4;({void*_tmp76A=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all);_tmp2F1.f2=_tmp76A;});_tmp2F1;});_npop_handler(0);return _tmp2F2;};}
# 1545
;_pop_region(temp);}case 26: _LL197: _tmp366=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp365=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_tmp364=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp167)->f3;_tmp363=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp167)->f4;_LL198: {
# 1553
struct _tuple16 _tmp2F5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp365);struct _tuple16 _tmp2F6=_tmp2F5;union Cyc_CfFlowInfo_FlowInfo _tmp319;void*_tmp318;_LL2A5: _tmp319=_tmp2F6.f1;_tmp318=_tmp2F6.f2;_LL2A6:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2F7=_tmp319;struct Cyc_Dict_Dict _tmp317;struct Cyc_List_List*_tmp316;if((_tmp2F7.BottomFL).tag == 1){_LL2A8: _LL2A9:
 return({struct _tuple16 _tmp2F8;_tmp2F8.f1=_tmp319;_tmp2F8.f2=_tmp14F->unknown_all;_tmp2F8;});}else{_LL2AA: _tmp317=((_tmp2F7.ReachableFL).val).f1;_tmp316=((_tmp2F7.ReachableFL).val).f2;_LL2AB:
# 1557
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp317,_tmp318)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2F9=0;({unsigned int _tmp76C=_tmp365->loc;struct _dyneither_ptr _tmp76B=({const char*_tmp2FA="expression may not be initialized";_tag_dyneither(_tmp2FA,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp76C,_tmp76B,_tag_dyneither(_tmp2F9,sizeof(void*),0));});});{
# 1561
struct Cyc_List_List*new_relns=_tmp316;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp366);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp365,& n2))
({struct Cyc_List_List*_tmp76D=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp316);new_relns=_tmp76D;});
# 1567
if(_tmp316 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp76E=Cyc_CfFlowInfo_ReachableFL(_tmp317,new_relns);_tmp319=_tmp76E;});{
# 1571
void*_tmp2FB=_tmp318;switch(*((int*)_tmp2FB)){case 0: _LL2AD: _LL2AE:
 return({struct _tuple16 _tmp2FC;_tmp2FC.f1=_tmp319;_tmp2FC.f2=_tmp14F->unknown_all;_tmp2FC;});case 2: _LL2AF: _LL2B0:
 goto _LL2B2;case 1: _LL2B1: _LL2B2:
 goto _LL2B4;case 5: _LL2B3: _LL2B4: {
# 1576
struct Cyc_List_List _tmp2FD=({struct Cyc_List_List _tmp308;_tmp308.hd=_tmp366;_tmp308.tl=0;_tmp308;});
({union Cyc_CfFlowInfo_FlowInfo _tmp76F=Cyc_NewControlFlow_add_vars(_tmp14F,_tmp319,& _tmp2FD,_tmp14F->unknown_all,e->loc,0);_tmp319=_tmp76F;});{
# 1579
struct _tuple16 _tmp2FE=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp319,_tmp364);struct _tuple16 _tmp2FF=_tmp2FE;union Cyc_CfFlowInfo_FlowInfo _tmp307;void*_tmp306;_LL2B8: _tmp307=_tmp2FF.f1;_tmp306=_tmp2FF.f2;_LL2B9:;
{union Cyc_CfFlowInfo_FlowInfo _tmp300=_tmp307;struct Cyc_Dict_Dict _tmp305;if((_tmp300.BottomFL).tag == 1){_LL2BB: _LL2BC:
 return({struct _tuple16 _tmp301;_tmp301.f1=_tmp307;_tmp301.f2=_tmp14F->unknown_all;_tmp301;});}else{_LL2BD: _tmp305=((_tmp300.ReachableFL).val).f1;_LL2BE:
# 1583
 if(Cyc_CfFlowInfo_initlevel(_tmp14F,_tmp305,_tmp306)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp302=0;({unsigned int _tmp771=_tmp365->loc;struct _dyneither_ptr _tmp770=({const char*_tmp303="expression may not be initialized";_tag_dyneither(_tmp303,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp771,_tmp770,_tag_dyneither(_tmp302,sizeof(void*),0));});});
return({struct _tuple16 _tmp304;({union Cyc_CfFlowInfo_FlowInfo _tmp772=Cyc_CfFlowInfo_BottomFL();_tmp304.f1=_tmp772;});_tmp304.f2=_tmp14F->unknown_all;_tmp304;});}}_LL2BA:;}
# 1588
_tmp319=_tmp307;
goto _LL2B6;};}default: _LL2B5: _LL2B6:
# 1591
 while(1){
struct Cyc_List_List _tmp309=({struct Cyc_List_List _tmp314;_tmp314.hd=_tmp366;_tmp314.tl=0;_tmp314;});
({union Cyc_CfFlowInfo_FlowInfo _tmp773=Cyc_NewControlFlow_add_vars(_tmp14F,_tmp319,& _tmp309,_tmp14F->unknown_all,e->loc,0);_tmp319=_tmp773;});{
struct _tuple16 _tmp30A=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp319,_tmp364);struct _tuple16 _tmp30B=_tmp30A;union Cyc_CfFlowInfo_FlowInfo _tmp313;void*_tmp312;_LL2C0: _tmp313=_tmp30B.f1;_tmp312=_tmp30B.f2;_LL2C1:;
{union Cyc_CfFlowInfo_FlowInfo _tmp30C=_tmp313;struct Cyc_Dict_Dict _tmp310;if((_tmp30C.BottomFL).tag == 1){_LL2C3: _LL2C4:
 goto _LL2C2;}else{_LL2C5: _tmp310=((_tmp30C.ReachableFL).val).f1;_LL2C6:
# 1598
 if(Cyc_CfFlowInfo_initlevel(_tmp14F,_tmp310,_tmp312)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp30D=0;({unsigned int _tmp775=_tmp365->loc;struct _dyneither_ptr _tmp774=({const char*_tmp30E="expression may not be initialized";_tag_dyneither(_tmp30E,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp775,_tmp774,_tag_dyneither(_tmp30D,sizeof(void*),0));});});
return({struct _tuple16 _tmp30F;({union Cyc_CfFlowInfo_FlowInfo _tmp776=Cyc_CfFlowInfo_BottomFL();_tmp30F.f1=_tmp776;});_tmp30F.f2=_tmp14F->unknown_all;_tmp30F;});}}_LL2C2:;}{
# 1603
union Cyc_CfFlowInfo_FlowInfo _tmp311=Cyc_CfFlowInfo_join_flow(_tmp14F,_tmp319,_tmp313);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp311,_tmp319))
break;
_tmp319=_tmp311;};};}
# 1608
return({struct _tuple16 _tmp315;_tmp315.f1=_tmp319;_tmp315.f2=_tmp14F->unknown_all;_tmp315;});}_LL2AC:;};};}_LL2A7:;};}case 27: _LL199: _tmp369=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_tmp368=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp167)->f2;_tmp367=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp167)->f3;_LL19A: {
# 1614
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp325=_region_malloc(_tmp14F->r,sizeof(*_tmp325));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp777=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp326;_tmp326.tag=1;_tmp326.f1=_tmp369;_tmp326.f2=(void*)_check_null(e->topt);_tmp326;});_tmp325[0]=_tmp777;});_tmp325;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp324=_region_malloc(_tmp14F->r,sizeof(*_tmp324));_tmp324->root=root;_tmp324->fields=0;_tmp324;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp322=_region_malloc(_tmp14F->r,sizeof(*_tmp322));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp778=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp323;_tmp323.tag=5;_tmp323.f1=place;_tmp323;});_tmp322[0]=_tmp778;});_tmp322;});
void*place_val;
# 1622
({void*_tmp779=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,_tmp368,0,_tmp14F->unknown_none);place_val=_tmp779;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp31A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp369);struct _tuple16 _tmp31B=_tmp31A;union Cyc_CfFlowInfo_FlowInfo _tmp321;_LL2C8: _tmp321=_tmp31B.f1;_LL2C9:;
outflow=_tmp321;{
union Cyc_CfFlowInfo_FlowInfo _tmp31C=outflow;struct Cyc_Dict_Dict _tmp320;struct Cyc_List_List*_tmp31F;if((_tmp31C.BottomFL).tag == 1){_LL2CB: _LL2CC:
 return({struct _tuple16 _tmp31D;_tmp31D.f1=outflow;_tmp31D.f2=rval;_tmp31D;});}else{_LL2CD: _tmp320=((_tmp31C.ReachableFL).val).f1;_tmp31F=((_tmp31C.ReachableFL).val).f2;_LL2CE:
# 1629
 return({struct _tuple16 _tmp31E;({union Cyc_CfFlowInfo_FlowInfo _tmp77B=({struct Cyc_Dict_Dict _tmp77A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp320,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp77A,_tmp31F);});_tmp31E.f1=_tmp77B;});_tmp31E.f2=rval;_tmp31E;});}_LL2CA:;};};}case 36: _LL19B: _tmp36A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp167)->f1;_LL19C: {
# 1633
struct _tuple17 _tmp327=({struct _tuple17 _tmp32A;_tmp32A.f1=(env->fenv)->unknown_all;_tmp32A.f2=copy_ctxt;_tmp32A;});
union Cyc_CfFlowInfo_FlowInfo _tmp328=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp36A,& _tmp327);
return({struct _tuple16 _tmp329;_tmp329.f1=_tmp328;_tmp329.f2=_tmp327.f1;_tmp329;});}case 35: _LL19F: _LL1A0:
# 1638
 goto _LL1A2;case 24: _LL1A1: _LL1A2:
 goto _LL1A4;case 38: _LL1A3: _LL1A4:
 goto _LL1A6;default: _LL1A5: _LL1A6:
# 1642
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));({struct Cyc_Core_Impossible_exn_struct _tmp77D=({struct Cyc_Core_Impossible_exn_struct _tmp32C;_tmp32C.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp77C=({const char*_tmp32D="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp32D,sizeof(char),31);});_tmp32C.f1=_tmp77C;});_tmp32C;});_tmp32B[0]=_tmp77D;});_tmp32B;}));}_LL13E:;};}
# 1652
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*flds){
# 1661
struct Cyc_CfFlowInfo_FlowEnv*_tmp36B=env->fenv;
void*_tmp36C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp36D=_tmp36C;void*_tmp39C;union Cyc_Absyn_Constraint*_tmp39B;union Cyc_Absyn_Constraint*_tmp39A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36D)->tag == 5){_LL2D0: _tmp39C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36D)->f1).elt_typ;_tmp39B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36D)->f1).ptr_atts).bounds;_tmp39A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36D)->f1).ptr_atts).zero_term;_LL2D1: {
# 1664
union Cyc_CfFlowInfo_FlowInfo _tmp36E=f;struct Cyc_Dict_Dict _tmp396;struct Cyc_List_List*_tmp395;if((_tmp36E.BottomFL).tag == 1){_LL2D5: _LL2D6:
 return({struct _tuple18 _tmp36F;_tmp36F.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp77E=Cyc_CfFlowInfo_UnknownL();_tmp36F.f2=_tmp77E;});_tmp36F;});}else{_LL2D7: _tmp396=((_tmp36E.ReachableFL).val).f1;_tmp395=((_tmp36E.ReachableFL).val).f2;_LL2D8: {
# 1668
struct _tuple15 _tmp370=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp371=_tmp370;void*_tmp394;struct Cyc_List_List*_tmp393;_LL2DA: _tmp394=_tmp371.f1;_tmp393=_tmp371.f2;_LL2DB:;
retry: {void*_tmp372=_tmp394;enum Cyc_CfFlowInfo_InitLevel _tmp389;void*_tmp388;struct Cyc_List_List*_tmp387;switch(*((int*)_tmp372)){case 8: _LL2DD: _LL2DE:
# 1671
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp373=_cycalloc(sizeof(*_tmp373));({struct Cyc_Core_Impossible_exn_struct _tmp780=({struct Cyc_Core_Impossible_exn_struct _tmp374;_tmp374.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp77F=({const char*_tmp375="named location in anal_derefL";_tag_dyneither(_tmp375,sizeof(char),30);});_tmp374.f1=_tmp77F;});_tmp374;});_tmp373[0]=_tmp780;});_tmp373;}));case 1: _LL2DF: _LL2E0:
 goto _LL2E2;case 2: _LL2E1: _LL2E2:
# 1674
({void*_tmp783=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp376=_cycalloc(sizeof(*_tmp376));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp782=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp377;_tmp377.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp781=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp395);_tmp377.f1=_tmp781;});_tmp377;});_tmp376[0]=_tmp782;});_tmp376;});e->annot=_tmp783;});goto _LL2DC;case 5: _LL2E3: _tmp388=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp372)->f1)->root;_tmp387=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp372)->f1)->fields;_LL2E4:
# 1677
 if(expanded_place_possibly_null)
# 1680
({void*_tmp786=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp378=_cycalloc(sizeof(*_tmp378));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp785=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp379;_tmp379.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp784=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp395);_tmp379.f1=_tmp784;});_tmp379;});_tmp378[0]=_tmp785;});_tmp378;});e->annot=_tmp786;});else{
# 1683
void*_tmp37A=e->annot;void*_tmp37B=_tmp37A;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp37B)->tag == Cyc_CfFlowInfo_UnknownZ){_LL2EC: _LL2ED:
# 1687
({void*_tmp789=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp37C=_cycalloc(sizeof(*_tmp37C));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp788=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp37D;_tmp37D.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp787=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp395);_tmp37D.f1=_tmp787;});_tmp37D;});_tmp37C[0]=_tmp788;});_tmp37C;});e->annot=_tmp789;});
goto _LL2EB;}else{_LL2EE: _LL2EF:
# 1690
({void*_tmp78C=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp37E=_cycalloc(sizeof(*_tmp37E));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp78B=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp37F;_tmp37F.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp78A=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp395);_tmp37F.f1=_tmp78A;});_tmp37F;});_tmp37E[0]=_tmp78B;});_tmp37E;});e->annot=_tmp78C;});
goto _LL2EB;}_LL2EB:;}
# 1694
if(Cyc_Tcutil_is_bound_one(_tmp39B))
return({struct _tuple18 _tmp380;_tmp380.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp78E=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp381=_region_malloc(_tmp36B->r,sizeof(*_tmp381));_tmp381->root=_tmp388;({struct Cyc_List_List*_tmp78D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp36B->r,_tmp387,flds);_tmp381->fields=_tmp78D;});_tmp381;}));_tmp380.f2=_tmp78E;});_tmp380;});
goto _LL2DC;case 0: _LL2E5: _LL2E6:
# 1698
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp382;({union Cyc_CfFlowInfo_FlowInfo _tmp78F=Cyc_CfFlowInfo_BottomFL();_tmp382.f1=_tmp78F;});({union Cyc_CfFlowInfo_AbsLVal _tmp790=Cyc_CfFlowInfo_UnknownL();_tmp382.f2=_tmp790;});_tmp382;});case 3: _LL2E7: _tmp389=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp372)->f1;_LL2E8:
# 1701
 if(Cyc_Tcutil_is_bound_one(_tmp39B))
({union Cyc_CfFlowInfo_FlowInfo _tmp791=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp389,_tmp393);f=_tmp791;});
goto _LL2EA;default: _LL2E9: _LL2EA:
# 1706
 if(passthrough_consumes){
void*_tmp383=_tmp394;void*_tmp384;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp383)->tag == 7){_LL2F1: _tmp384=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp383)->f3;_LL2F2:
 _tmp394=_tmp384;goto retry;}else{_LL2F3: _LL2F4:
 goto _LL2F0;}_LL2F0:;}
# 1712
({void*_tmp794=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp385=_cycalloc(sizeof(*_tmp385));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp793=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp386;_tmp386.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp792=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp395);_tmp386.f1=_tmp792;});_tmp386;});_tmp385[0]=_tmp793;});_tmp385;});e->annot=_tmp794;});}_LL2DC:;}
# 1714
if(Cyc_CfFlowInfo_initlevel(_tmp36B,_tmp396,_tmp394)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp38A=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp394);struct _tuple15 _tmp38B=_tmp38A;void*_tmp391;_LL2F6: _tmp391=_tmp38B.f1;_LL2F7:;{
void*_tmp38C=_tmp391;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp38C)->tag == 7){_LL2F9: _LL2FA:
# 1718
({void*_tmp38D=0;({unsigned int _tmp796=e->loc;struct _dyneither_ptr _tmp795=({const char*_tmp38E="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp38E,sizeof(char),66);});Cyc_CfFlowInfo_aerr(_tmp796,_tmp795,_tag_dyneither(_tmp38D,sizeof(void*),0));});});
goto _LL2F8;}else{_LL2FB: _LL2FC:
# 1721
({void*_tmp38F=0;({unsigned int _tmp798=e->loc;struct _dyneither_ptr _tmp797=({const char*_tmp390="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp390,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp798,_tmp797,_tag_dyneither(_tmp38F,sizeof(void*),0));});});
goto _LL2F8;}_LL2F8:;};}
# 1725
return({struct _tuple18 _tmp392;_tmp392.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp799=Cyc_CfFlowInfo_UnknownL();_tmp392.f2=_tmp799;});_tmp392;});}}_LL2D4:;}}else{_LL2D2: _LL2D3:
# 1727
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp397=_cycalloc(sizeof(*_tmp397));({struct Cyc_Core_Impossible_exn_struct _tmp79B=({struct Cyc_Core_Impossible_exn_struct _tmp398;_tmp398.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp79A=({const char*_tmp399="left deref of non-pointer-type";_tag_dyneither(_tmp399,sizeof(char),31);});_tmp398.f1=_tmp79A;});_tmp398;});_tmp397[0]=_tmp79B;});_tmp397;}));}_LL2CF:;}
# 1735
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1738
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp39D=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp39E=inflow;struct Cyc_Dict_Dict _tmp3A1;struct Cyc_List_List*_tmp3A0;if((_tmp39E.BottomFL).tag == 1){_LL2FE: _LL2FF:
 return({struct _tuple18 _tmp39F;({union Cyc_CfFlowInfo_FlowInfo _tmp79C=Cyc_CfFlowInfo_BottomFL();_tmp39F.f1=_tmp79C;});({union Cyc_CfFlowInfo_AbsLVal _tmp79D=Cyc_CfFlowInfo_UnknownL();_tmp39F.f2=_tmp79D;});_tmp39F;});}else{_LL300: _tmp3A1=((_tmp39E.ReachableFL).val).f1;_tmp3A0=((_tmp39E.ReachableFL).val).f2;_LL301:
# 1743
 d=_tmp3A1;}_LL2FD:;}{
# 1747
void*_tmp3A2=e->r;void*_tmp3A3=_tmp3A2;struct Cyc_Absyn_Exp*_tmp401;struct _dyneither_ptr*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;struct _dyneither_ptr*_tmp3FB;struct Cyc_Absyn_Vardecl*_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3F8;struct Cyc_Absyn_Vardecl*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F4;switch(*((int*)_tmp3A3)){case 13: _LL303: _tmp3F4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL304:
 _tmp3F5=_tmp3F4;goto _LL306;case 11: _LL305: _tmp3F5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_LL306:
 _tmp3F6=_tmp3F5;goto _LL308;case 12: _LL307: _tmp3F6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_LL308:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3F6,flds);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)){case 3: _LL309: _tmp3F7=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LL30A:
# 1752
 _tmp3F8=_tmp3F7;goto _LL30C;case 4: _LL30B: _tmp3F8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LL30C:
 _tmp3F9=_tmp3F8;goto _LL30E;case 5: _LL30D: _tmp3F9=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LL30E:
# 1755
 return({struct _tuple18 _tmp3A4;_tmp3A4.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7A0=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3A5=_region_malloc(env->r,sizeof(*_tmp3A5));({void*_tmp79F=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3A6=_region_malloc(env->r,sizeof(*_tmp3A6));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp79E=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=_tmp3F9;_tmp3A7;});_tmp3A6[0]=_tmp79E;});_tmp3A6;});_tmp3A5->root=_tmp79F;});_tmp3A5->fields=flds;_tmp3A5;}));_tmp3A4.f2=_tmp7A0;});_tmp3A4;});case 1: _LL30F: _tmp3FA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LL310:
# 1757
 return({struct _tuple18 _tmp3A8;_tmp3A8.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7A1=Cyc_CfFlowInfo_UnknownL();_tmp3A8.f2=_tmp7A1;});_tmp3A8;});default: goto _LL319;}case 21: _LL311: _tmp3FC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_tmp3FB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL312:
# 1760
{void*_tmp3A9=Cyc_Tcutil_compress((void*)_check_null(_tmp3FC->topt));void*_tmp3AA=_tmp3A9;void*_tmp3AF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->tag == 5){_LL31C: _tmp3AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).elt_typ;_LL31D:
# 1762
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3AF)){
Cyc_NewControlFlow_check_union_requires(_tmp3FC->loc,_tmp39D->r,_tmp3AF,_tmp3FB,inflow);
({struct Cyc_List_List*_tmp7A3=({struct Cyc_List_List*_tmp3AB=_region_malloc(env->r,sizeof(*_tmp3AB));({typeof((void*)Cyc_CfFlowInfo_get_field_index(_tmp3AF,_tmp3FB))_tmp7A2=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3AF,_tmp3FB);_tmp3AB->hd=_tmp7A2;});_tmp3AB->tl=flds;_tmp3AB;});flds=_tmp7A3;});}
# 1766
goto _LL31B;}else{_LL31E: _LL31F:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({struct Cyc_Core_Impossible_exn_struct _tmp7A5=({struct Cyc_Core_Impossible_exn_struct _tmp3AD;_tmp3AD.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7A4=({const char*_tmp3AE="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3AE,sizeof(char),25);});_tmp3AD.f1=_tmp7A4;});_tmp3AD;});_tmp3AC[0]=_tmp7A5;});_tmp3AC;}));}_LL31B:;}
# 1769
_tmp3FD=_tmp3FC;goto _LL314;case 19: _LL313: _tmp3FD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_LL314:
# 1772
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3FD->topt),1)){
# 1774
struct _tuple18 _tmp3B0=({
struct Cyc_NewControlFlow_AnalEnv*_tmp7A9=env;union Cyc_CfFlowInfo_FlowInfo _tmp7A8=inflow;int _tmp7A7=
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3FD->topt),1);
# 1775
int _tmp7A6=passthrough_consumes;Cyc_NewControlFlow_anal_Lexp(_tmp7A9,_tmp7A8,_tmp7A7,_tmp7A6,_tmp3FD);});
# 1774
struct _tuple18 _tmp3B1=_tmp3B0;union Cyc_CfFlowInfo_FlowInfo _tmp3CD;union Cyc_CfFlowInfo_AbsLVal _tmp3CC;_LL321: _tmp3CD=_tmp3B1.f1;_tmp3CC=_tmp3B1.f2;_LL322:;{
# 1778
int _tmp3B2=1;
struct _tuple18 _tmp3B3=({struct _tuple18 _tmp3CB;_tmp3CB.f1=_tmp3CD;_tmp3CB.f2=_tmp3CC;_tmp3CB;});struct _tuple18 _tmp3B4=_tmp3B3;struct Cyc_Dict_Dict _tmp3CA;struct Cyc_List_List*_tmp3C9;struct Cyc_CfFlowInfo_Place*_tmp3C8;if(((_tmp3B4.f1).ReachableFL).tag == 2){if(((_tmp3B4.f2).PlaceL).tag == 1){_LL324: _tmp3CA=(((_tmp3B4.f1).ReachableFL).val).f1;_tmp3C9=(((_tmp3B4.f1).ReachableFL).val).f2;_tmp3C8=((_tmp3B4.f2).PlaceL).val;_LL325: {
# 1781
void*_tmp3B5=Cyc_CfFlowInfo_lookup_place(_tmp3CA,_tmp3C8);
struct _tuple15 _tmp3B6=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3B5);struct _tuple15 _tmp3B7=_tmp3B6;void*_tmp3C7;struct Cyc_List_List*_tmp3C6;_LL329: _tmp3C7=_tmp3B7.f1;_tmp3C6=_tmp3B7.f2;_LL32A:;{
void*_tmp3B8=_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C5;void*_tmp3C4;switch(*((int*)_tmp3B8)){case 8: _LL32C: _tmp3C5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B8)->f1;_tmp3C4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B8)->f2;_LL32D:
# 1785
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct Cyc_Core_Impossible_exn_struct _tmp7AB=({struct Cyc_Core_Impossible_exn_struct _tmp3BA;_tmp3BA.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7AA=({const char*_tmp3BB="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3BB,sizeof(char),38);});_tmp3BA.f1=_tmp7AA;});_tmp3BA;});_tmp3B9[0]=_tmp7AB;});_tmp3B9;}));case 7: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3B8)->f3)->tag == 5){_LL32E: _LL32F:
 goto _LL331;}else{goto _LL336;}case 5: _LL330: _LL331:
# 1790
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3CD,_tmp3FD,_tmp3CD,_tmp3B5,passthrough_consumes,0,flds);case 1: _LL332: _LL333:
 goto _LL335;case 2: _LL334: _LL335:
# 1793
 _tmp3B2=0;goto _LL337;default: _LL336: _LL337: {
# 1800
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp39D,_tmp3CA,_tmp3C7);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp39D->unknown_all: _tmp39D->unknown_none;
void*new_rval=({struct Cyc_CfFlowInfo_FlowEnv*_tmp7AD=_tmp39D;void*_tmp7AC=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3FD->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp7AD,_tmp7AC,0,leaf);});
void*new_root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3C2=_region_malloc(_tmp39D->r,sizeof(*_tmp3C2));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7AE=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp3C3;_tmp3C3.tag=1;_tmp3C3.f1=e;_tmp3C3.f2=(void*)_check_null(e->topt);_tmp3C3;});_tmp3C2[0]=_tmp7AE;});_tmp3C2;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp3C1=_region_malloc(_tmp39D->r,sizeof(*_tmp3C1));_tmp3C1->root=new_root;_tmp3C1->fields=0;_tmp3C1;});
void*res=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3BF=_region_malloc(_tmp39D->r,sizeof(*_tmp3BF));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7AF=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp3C0;_tmp3C0.tag=5;_tmp3C0.f1=place;_tmp3C0;});_tmp3BF[0]=_tmp7AF;});_tmp3BF;});
for(0;_tmp3C6 != 0;_tmp3C6=_tmp3C6->tl){
({void*_tmp7B1=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3BC=_region_malloc(_tmp39D->r,sizeof(*_tmp3BC));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B0=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3BD;_tmp3BD.tag=8;_tmp3BD.f1=(struct Cyc_Absyn_Vardecl*)_tmp3C6->hd;_tmp3BD.f2=res;_tmp3BD;});_tmp3BC[0]=_tmp7B0;});_tmp3BC;});res=_tmp7B1;});}
({struct Cyc_Dict_Dict _tmp7B2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3CA,new_root,new_rval);_tmp3CA=_tmp7B2;});
({struct Cyc_Dict_Dict _tmp7B3=Cyc_CfFlowInfo_assign_place(_tmp39D,e->loc,_tmp3CA,_tmp3C8,res);_tmp3CA=_tmp7B3;});{
union Cyc_CfFlowInfo_FlowInfo _tmp3BE=Cyc_CfFlowInfo_ReachableFL(_tmp3CA,_tmp3C9);
# 1817
return Cyc_NewControlFlow_anal_derefL(env,_tmp3BE,_tmp3FD,_tmp3BE,res,passthrough_consumes,_tmp3B2,flds);};}}_LL32B:;};}}else{goto _LL326;}}else{_LL326: _LL327:
# 1820
 goto _LL323;}_LL323:;};}{
# 1823
struct _tuple16 _tmp3CE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3FD);struct _tuple16 _tmp3CF=_tmp3CE;union Cyc_CfFlowInfo_FlowInfo _tmp3D1;void*_tmp3D0;_LL339: _tmp3D1=_tmp3CF.f1;_tmp3D0=_tmp3CF.f2;_LL33A:;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3FD,_tmp3D1,_tmp3D0,passthrough_consumes,0,flds);};case 22: _LL315: _tmp3FF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_tmp3FE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL316: {
# 1827
void*_tmp3D2=Cyc_Tcutil_compress((void*)_check_null(_tmp3FF->topt));void*_tmp3D3=_tmp3D2;union Cyc_Absyn_Constraint*_tmp3EF;switch(*((int*)_tmp3D3)){case 10: _LL33C: _LL33D: {
# 1829
unsigned int _tmp3D4=(Cyc_Evexp_eval_const_uint_exp(_tmp3FE)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp7B8=env;union Cyc_CfFlowInfo_FlowInfo _tmp7B7=inflow;int _tmp7B6=expand_unique;int _tmp7B5=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp7B4=_tmp3FF;Cyc_NewControlFlow_anal_Lexp_rec(_tmp7B8,_tmp7B7,_tmp7B6,_tmp7B5,_tmp7B4,({struct Cyc_List_List*_tmp3D5=_region_malloc(env->r,sizeof(*_tmp3D5));_tmp3D5->hd=(void*)_tmp3D4;_tmp3D5->tl=flds;_tmp3D5;}));});}case 5: _LL33E: _tmp3EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D3)->f1).ptr_atts).bounds;_LL33F: {
# 1832
struct _RegionHandle _tmp3D6=_new_region("temp");struct _RegionHandle*temp=& _tmp3D6;_push_region(temp);
{struct _tuple23 _tmp3D7=({struct _RegionHandle*_tmp7BC=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp7BB=env;union Cyc_CfFlowInfo_FlowInfo _tmp7BA=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp7BC,_tmp7BB,_tmp7BA,({struct Cyc_Absyn_Exp*_tmp3EB[2];_tmp3EB[1]=_tmp3FE;_tmp3EB[0]=_tmp3FF;({struct _RegionHandle*_tmp7B9=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp7B9,_tag_dyneither(_tmp3EB,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,1);});struct _tuple23 _tmp3D8=_tmp3D7;union Cyc_CfFlowInfo_FlowInfo _tmp3EA;struct Cyc_List_List*_tmp3E9;_LL343: _tmp3EA=_tmp3D8.f1;_tmp3E9=_tmp3D8.f2;_LL344:;{
# 1835
union Cyc_CfFlowInfo_FlowInfo _tmp3D9=_tmp3EA;
{union Cyc_CfFlowInfo_FlowInfo _tmp3DA=_tmp3EA;struct Cyc_Dict_Dict _tmp3DF;struct Cyc_List_List*_tmp3DE;if((_tmp3DA.ReachableFL).tag == 2){_LL346: _tmp3DF=((_tmp3DA.ReachableFL).val).f1;_tmp3DE=((_tmp3DA.ReachableFL).val).f2;_LL347:
# 1838
 if(Cyc_CfFlowInfo_initlevel(_tmp39D,_tmp3DF,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3E9))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp3DB=0;({unsigned int _tmp7BE=_tmp3FE->loc;struct _dyneither_ptr _tmp7BD=({const char*_tmp3DC="expression may not be initialized";_tag_dyneither(_tmp3DC,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7BE,_tmp7BD,_tag_dyneither(_tmp3DB,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp3DD=Cyc_NewControlFlow_add_subscript_reln(_tmp39D->r,_tmp3DE,_tmp3FF,_tmp3FE);
if(_tmp3DE != _tmp3DD)
({union Cyc_CfFlowInfo_FlowInfo _tmp7BF=Cyc_CfFlowInfo_ReachableFL(_tmp3DF,_tmp3DD);_tmp3D9=_tmp7BF;});
goto _LL345;};}else{_LL348: _LL349:
 goto _LL345;}_LL345:;}{
# 1846
struct _tuple18 _tmp3E0=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3FF,_tmp3EA,(void*)((struct Cyc_List_List*)_check_null(_tmp3E9))->hd,passthrough_consumes,0,flds);struct _tuple18 _tmp3E1=_tmp3E0;union Cyc_CfFlowInfo_FlowInfo _tmp3E8;union Cyc_CfFlowInfo_AbsLVal _tmp3E7;_LL34B: _tmp3E8=_tmp3E1.f1;_tmp3E7=_tmp3E1.f2;_LL34C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E2=_tmp3E8;if((_tmp3E2.BottomFL).tag == 1){_LL34E: _LL34F: {
struct _tuple18 _tmp3E4=({struct _tuple18 _tmp3E3;_tmp3E3.f1=_tmp3E8;_tmp3E3.f2=_tmp3E7;_tmp3E3;});_npop_handler(0);return _tmp3E4;}}else{_LL350: _LL351: {
struct _tuple18 _tmp3E6=({struct _tuple18 _tmp3E5;_tmp3E5.f1=_tmp3D9;_tmp3E5.f2=_tmp3E7;_tmp3E5;});_npop_handler(0);return _tmp3E6;}}_LL34D:;};};};}
# 1833
;_pop_region(temp);}default: _LL340: _LL341:
# 1852
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct Cyc_Core_Impossible_exn_struct _tmp7C1=({struct Cyc_Core_Impossible_exn_struct _tmp3ED;_tmp3ED.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7C0=({const char*_tmp3EE="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp3EE,sizeof(char),33);});_tmp3ED.f1=_tmp7C0;});_tmp3ED;});_tmp3EC[0]=_tmp7C1;});_tmp3EC;}));}_LL33B:;}case 20: _LL317: _tmp401=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_tmp400=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL318: {
# 1856
void*_tmp3F0=(void*)_check_null(_tmp401->topt);
if(Cyc_Absyn_is_require_union_type(_tmp3F0))
Cyc_NewControlFlow_check_union_requires(_tmp401->loc,_tmp39D->r,_tmp3F0,_tmp400,inflow);
# 1860
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3F0))
return({struct _tuple18 _tmp3F1;_tmp3F1.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7C2=Cyc_CfFlowInfo_UnknownL();_tmp3F1.f2=_tmp7C2;});_tmp3F1;});
# 1863
return({struct Cyc_NewControlFlow_AnalEnv*_tmp7C8=env;union Cyc_CfFlowInfo_FlowInfo _tmp7C7=inflow;int _tmp7C6=expand_unique;int _tmp7C5=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp7C4=_tmp401;Cyc_NewControlFlow_anal_Lexp_rec(_tmp7C8,_tmp7C7,_tmp7C6,_tmp7C5,_tmp7C4,({struct Cyc_List_List*_tmp3F2=_region_malloc(env->r,sizeof(*_tmp3F2));({typeof((void*)
Cyc_CfFlowInfo_get_field_index(_tmp3F0,_tmp400))_tmp7C3=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3F0,_tmp400);_tmp3F2->hd=_tmp7C3;});_tmp3F2->tl=flds;_tmp3F2;}));});}default: _LL319: _LL31A:
# 1866
 return({struct _tuple18 _tmp3F3;({union Cyc_CfFlowInfo_FlowInfo _tmp7C9=Cyc_CfFlowInfo_BottomFL();_tmp3F3.f1=_tmp7C9;});({union Cyc_CfFlowInfo_AbsLVal _tmp7CA=Cyc_CfFlowInfo_UnknownL();_tmp3F3.f2=_tmp7CA;});_tmp3F3;});}_LL302:;};}
# 1870
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1873
struct _tuple18 _tmp402=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp403=_tmp402;union Cyc_CfFlowInfo_FlowInfo _tmp406;union Cyc_CfFlowInfo_AbsLVal _tmp405;_LL353: _tmp406=_tmp403.f1;_tmp405=_tmp403.f2;_LL354:;
return({struct _tuple18 _tmp404;_tmp404.f1=_tmp406;_tmp404.f2=_tmp405;_tmp404;});}
# 1880
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1883
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp407=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp408=_tmp407;union Cyc_CfFlowInfo_FlowInfo _tmp409;_LL356: _tmp409=_tmp408.f1;_LL357:;
inflow=_tmp409;}}}
# 1891
return inflow;}
# 1897
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1900
struct Cyc_CfFlowInfo_FlowEnv*_tmp40A=env->fenv;
# 1903
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp40B=_new_region("temp");struct _RegionHandle*temp=& _tmp40B;_push_region(temp);{
struct _tuple23 _tmp40C=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,es,0,0);struct _tuple23 _tmp40D=_tmp40C;union Cyc_CfFlowInfo_FlowInfo _tmp40F;struct Cyc_List_List*_tmp40E;_LL359: _tmp40F=_tmp40D.f1;_tmp40E=_tmp40D.f2;_LL35A:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp40E))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp40E->tl))->hd;
f=_tmp40F;}{
# 1913
union Cyc_CfFlowInfo_FlowInfo _tmp410=f;struct Cyc_Dict_Dict _tmp45A;struct Cyc_List_List*_tmp459;if((_tmp410.BottomFL).tag == 1){_LL35C: _LL35D: {
struct _tuple19 _tmp412=({struct _tuple19 _tmp411;_tmp411.f1=f;_tmp411.f2=f;_tmp411;});_npop_handler(0);return _tmp412;}}else{_LL35E: _tmp45A=((_tmp410.ReachableFL).val).f1;_tmp459=((_tmp410.ReachableFL).val).f2;_LL35F: {
# 1916
struct Cyc_Absyn_Exp*_tmp413=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp414=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp45A,r1)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp415=0;({unsigned int _tmp7CC=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp7CB=({const char*_tmp416="expression may not be initialized";_tag_dyneither(_tmp416,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7CC,_tmp7CB,_tag_dyneither(_tmp415,sizeof(void*),0));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp45A,r2)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp417=0;({unsigned int _tmp7CE=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp7CD=({const char*_tmp418="expression may not be initialized";_tag_dyneither(_tmp418,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7CE,_tmp7CD,_tag_dyneither(_tmp417,sizeof(void*),0));});});{
# 1923
union Cyc_CfFlowInfo_FlowInfo _tmp419=f;
union Cyc_CfFlowInfo_FlowInfo _tmp41A=f;
# 1928
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp41B=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);struct _tuple15 _tmp41C=_tmp41B;void*_tmp437;struct Cyc_List_List*_tmp436;_LL361: _tmp437=_tmp41C.f1;_tmp436=_tmp41C.f2;_LL362:;{
struct _tuple15 _tmp41D=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);struct _tuple15 _tmp41E=_tmp41D;void*_tmp435;struct Cyc_List_List*_tmp434;_LL364: _tmp435=_tmp41E.f1;_tmp434=_tmp41E.f2;_LL365:;{
struct _tuple0 _tmp41F=({struct _tuple0 _tmp433;_tmp433.f1=_tmp437;_tmp433.f2=_tmp435;_tmp433;});struct _tuple0 _tmp420=_tmp41F;enum Cyc_CfFlowInfo_InitLevel _tmp432;enum Cyc_CfFlowInfo_InitLevel _tmp431;switch(*((int*)_tmp420.f1)){case 3: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL367: _tmp431=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp420.f1)->f1;_LL368: {
# 1935
struct _tuple19 _tmp421=Cyc_NewControlFlow_splitzero(env,f,f,_tmp413,_tmp431,_tmp436);struct _tuple19 _tmp422=_tmp421;union Cyc_CfFlowInfo_FlowInfo _tmp428;union Cyc_CfFlowInfo_FlowInfo _tmp427;_LL37C: _tmp428=_tmp422.f1;_tmp427=_tmp422.f2;_LL37D:;
{enum Cyc_Absyn_Primop _tmp423=p;switch(_tmp423){case Cyc_Absyn_Eq: _LL37F: _LL380:
 _tmp419=_tmp427;_tmp41A=_tmp428;goto _LL37E;case Cyc_Absyn_Neq: _LL381: _LL382:
 _tmp419=_tmp428;_tmp41A=_tmp427;goto _LL37E;default: _LL383: _LL384:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp424=_cycalloc(sizeof(*_tmp424));({struct Cyc_Core_Impossible_exn_struct _tmp7D0=({struct Cyc_Core_Impossible_exn_struct _tmp425;_tmp425.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7CF=({const char*_tmp426="anal_test, zero-split";_tag_dyneither(_tmp426,sizeof(char),22);});_tmp425.f1=_tmp7CF;});_tmp425;});_tmp424[0]=_tmp7D0;});_tmp424;}));}_LL37E:;}
# 1941
goto _LL366;}}else{goto _LL379;}case 0: switch(*((int*)_tmp420.f2)){case 3: _LL369: _tmp432=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->f1;_LL36A: {
# 1943
struct _tuple19 _tmp429=Cyc_NewControlFlow_splitzero(env,f,f,_tmp414,_tmp432,_tmp434);struct _tuple19 _tmp42A=_tmp429;union Cyc_CfFlowInfo_FlowInfo _tmp430;union Cyc_CfFlowInfo_FlowInfo _tmp42F;_LL386: _tmp430=_tmp42A.f1;_tmp42F=_tmp42A.f2;_LL387:;
{enum Cyc_Absyn_Primop _tmp42B=p;switch(_tmp42B){case Cyc_Absyn_Eq: _LL389: _LL38A:
 _tmp419=_tmp42F;_tmp41A=_tmp430;goto _LL388;case Cyc_Absyn_Neq: _LL38B: _LL38C:
 _tmp419=_tmp430;_tmp41A=_tmp42F;goto _LL388;default: _LL38D: _LL38E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));({struct Cyc_Core_Impossible_exn_struct _tmp7D2=({struct Cyc_Core_Impossible_exn_struct _tmp42D;_tmp42D.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7D1=({const char*_tmp42E="anal_test, zero-split";_tag_dyneither(_tmp42E,sizeof(char),22);});_tmp42D.f1=_tmp7D1;});_tmp42D;});_tmp42C[0]=_tmp7D2;});_tmp42C;}));}_LL388:;}
# 1949
goto _LL366;}case 0: _LL36B: _LL36C:
# 1951
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp7D3=Cyc_CfFlowInfo_BottomFL();_tmp41A=_tmp7D3;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7D4=Cyc_CfFlowInfo_BottomFL();_tmp419=_tmp7D4;});}
goto _LL366;case 1: _LL36D: _LL36E:
 goto _LL370;case 2: _LL36F: _LL370:
 goto _LL372;case 5: _LL371: _LL372:
 goto _LL374;default: goto _LL379;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL373: _LL374:
 goto _LL376;}else{goto _LL379;}case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL375: _LL376:
 goto _LL378;}else{goto _LL379;}case 5: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL377: _LL378:
# 1960
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp7D5=Cyc_CfFlowInfo_BottomFL();_tmp41A=_tmp7D5;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7D6=Cyc_CfFlowInfo_BottomFL();_tmp419=_tmp7D6;});}
goto _LL366;}else{goto _LL379;}default: _LL379: _LL37A:
 goto _LL366;}_LL366:;};};}
# 1971
{struct _tuple0 _tmp438=({struct _tuple0 _tmp43C;({void*_tmp7D7=Cyc_Tcutil_compress((void*)_check_null(_tmp413->topt));_tmp43C.f1=_tmp7D7;});({void*_tmp7D8=Cyc_Tcutil_compress((void*)_check_null(_tmp414->topt));_tmp43C.f2=_tmp7D8;});_tmp43C;});struct _tuple0 _tmp439=_tmp438;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp439.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp439.f1)->f1 == Cyc_Absyn_Unsigned){_LL390: _LL391:
 goto _LL393;}else{switch(*((int*)_tmp439.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp439.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL392;else{goto _LL398;}case 19: goto _LL396;default: goto _LL398;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp439.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp439.f2)->f1 == Cyc_Absyn_Unsigned){_LL392: _LL393:
 goto _LL395;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp439.f1)->tag == 19)goto _LL394;else{goto _LL398;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp439.f1)->tag == 19){_LL394: _LL395:
 goto _LL397;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp439.f2)->tag == 19){_LL396: _LL397:
 goto _LL38F;}else{_LL398: _LL399: {
struct _tuple19 _tmp43B=({struct _tuple19 _tmp43A;_tmp43A.f1=_tmp419;_tmp43A.f2=_tmp41A;_tmp43A;});_npop_handler(0);return _tmp43B;}}}}}_LL38F:;}
# 1980
{enum Cyc_Absyn_Primop _tmp43D=p;switch(_tmp43D){case Cyc_Absyn_Eq: _LL39B: _LL39C:
 goto _LL39E;case Cyc_Absyn_Neq: _LL39D: _LL39E: goto _LL3A0;case Cyc_Absyn_Gt: _LL39F: _LL3A0: goto _LL3A2;case Cyc_Absyn_Gte: _LL3A1: _LL3A2: goto _LL3A4;case Cyc_Absyn_Lt: _LL3A3: _LL3A4: goto _LL3A6;case Cyc_Absyn_Lte: _LL3A5: _LL3A6: goto _LL39A;default: _LL3A7: _LL3A8: {
struct _tuple19 _tmp43F=({struct _tuple19 _tmp43E;_tmp43E.f1=_tmp419;_tmp43E.f2=_tmp41A;_tmp43E;});_npop_handler(0);return _tmp43F;}}_LL39A:;}{
# 1985
struct _RegionHandle*_tmp440=(env->fenv)->r;
struct _tuple13 _tmp441=Cyc_Relations_primop2relation(_tmp413,p,_tmp414);struct _tuple13 _tmp442=_tmp441;struct Cyc_Absyn_Exp*_tmp458;enum Cyc_Relations_Relation _tmp457;struct Cyc_Absyn_Exp*_tmp456;_LL3AA: _tmp458=_tmp442.f1;_tmp457=_tmp442.f2;_tmp456=_tmp442.f3;_LL3AB:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1990
if(Cyc_Relations_exp2relnop(_tmp458,& n1) && Cyc_Relations_exp2relnop(_tmp456,& n2)){
# 1992
struct Cyc_List_List*_tmp443=Cyc_Relations_add_relation(_tmp440,n1,_tmp457,n2,_tmp459);
# 1996
struct Cyc_List_List*_tmp444=({struct _RegionHandle*_tmp7DC=_tmp440;union Cyc_Relations_RelnOp _tmp7DB=n2;enum Cyc_Relations_Relation _tmp7DA=Cyc_Relations_flip_relation(_tmp457);union Cyc_Relations_RelnOp _tmp7D9=n1;Cyc_Relations_add_relation(_tmp7DC,_tmp7DB,_tmp7DA,_tmp7D9,_tmp459);});
struct _tuple19 _tmp445=({struct _tuple19 _tmp453;_tmp453.f1=_tmp419;_tmp453.f2=_tmp41A;_tmp453;});struct _tuple19 _tmp446=_tmp445;struct Cyc_Dict_Dict _tmp452;struct Cyc_Dict_Dict _tmp451;struct Cyc_Dict_Dict _tmp450;struct Cyc_Dict_Dict _tmp44F;if(((_tmp446.f1).ReachableFL).tag == 2){if(((_tmp446.f2).ReachableFL).tag == 2){_LL3AD: _tmp450=(((_tmp446.f1).ReachableFL).val).f1;_tmp44F=(((_tmp446.f2).ReachableFL).val).f1;_LL3AE: {
# 1999
struct _tuple19 _tmp448=({struct _tuple19 _tmp447;({union Cyc_CfFlowInfo_FlowInfo _tmp7DD=Cyc_CfFlowInfo_ReachableFL(_tmp450,_tmp443);_tmp447.f1=_tmp7DD;});({union Cyc_CfFlowInfo_FlowInfo _tmp7DE=Cyc_CfFlowInfo_ReachableFL(_tmp44F,_tmp444);_tmp447.f2=_tmp7DE;});_tmp447;});_npop_handler(0);return _tmp448;}}else{_LL3B1: _tmp451=(((_tmp446.f1).ReachableFL).val).f1;_LL3B2: {
# 2003
struct _tuple19 _tmp44C=({struct _tuple19 _tmp44B;({union Cyc_CfFlowInfo_FlowInfo _tmp7DF=Cyc_CfFlowInfo_ReachableFL(_tmp451,_tmp443);_tmp44B.f1=_tmp7DF;});_tmp44B.f2=_tmp41A;_tmp44B;});_npop_handler(0);return _tmp44C;}}}else{if(((_tmp446.f2).ReachableFL).tag == 2){_LL3AF: _tmp452=(((_tmp446.f2).ReachableFL).val).f1;_LL3B0: {
# 2001
struct _tuple19 _tmp44A=({struct _tuple19 _tmp449;_tmp449.f1=_tmp419;({union Cyc_CfFlowInfo_FlowInfo _tmp7E0=Cyc_CfFlowInfo_ReachableFL(_tmp452,_tmp444);_tmp449.f2=_tmp7E0;});_tmp449;});_npop_handler(0);return _tmp44A;}}else{_LL3B3: _LL3B4: {
# 2005
struct _tuple19 _tmp44E=({struct _tuple19 _tmp44D;_tmp44D.f1=_tmp419;_tmp44D.f2=_tmp41A;_tmp44D;});_npop_handler(0);return _tmp44E;}}}_LL3AC:;}else{
# 2008
struct _tuple19 _tmp455=({struct _tuple19 _tmp454;_tmp454.f1=_tmp419;_tmp454.f2=_tmp41A;_tmp454;});_npop_handler(0);return _tmp455;}};};};}}_LL35B:;};
# 1905
;_pop_region(temp);}
# 2014
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2016
struct Cyc_CfFlowInfo_FlowEnv*_tmp45B=env->fenv;
void*_tmp45C=e->r;void*_tmp45D=_tmp45C;enum Cyc_Absyn_Primop _tmp4AC;struct Cyc_List_List*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;switch(*((int*)_tmp45D)){case 5: _LL3B6: _tmp4A3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp45D)->f1;_tmp4A2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp45D)->f2;_tmp4A1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp45D)->f3;_LL3B7: {
# 2019
struct _tuple19 _tmp45E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A3);struct _tuple19 _tmp45F=_tmp45E;union Cyc_CfFlowInfo_FlowInfo _tmp46A;union Cyc_CfFlowInfo_FlowInfo _tmp469;_LL3C5: _tmp46A=_tmp45F.f1;_tmp469=_tmp45F.f2;_LL3C6:;{
struct _tuple19 _tmp460=Cyc_NewControlFlow_anal_test(env,_tmp46A,_tmp4A2);struct _tuple19 _tmp461=_tmp460;union Cyc_CfFlowInfo_FlowInfo _tmp468;union Cyc_CfFlowInfo_FlowInfo _tmp467;_LL3C8: _tmp468=_tmp461.f1;_tmp467=_tmp461.f2;_LL3C9:;{
struct _tuple19 _tmp462=Cyc_NewControlFlow_anal_test(env,_tmp469,_tmp4A1);struct _tuple19 _tmp463=_tmp462;union Cyc_CfFlowInfo_FlowInfo _tmp466;union Cyc_CfFlowInfo_FlowInfo _tmp465;_LL3CB: _tmp466=_tmp463.f1;_tmp465=_tmp463.f2;_LL3CC:;
return({struct _tuple19 _tmp464;({union Cyc_CfFlowInfo_FlowInfo _tmp7E1=Cyc_CfFlowInfo_join_flow(_tmp45B,_tmp468,_tmp466);_tmp464.f1=_tmp7E1;});({union Cyc_CfFlowInfo_FlowInfo _tmp7E2=
Cyc_CfFlowInfo_join_flow(_tmp45B,_tmp467,_tmp465);_tmp464.f2=_tmp7E2;});_tmp464;});};};}case 6: _LL3B8: _tmp4A5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp45D)->f1;_tmp4A4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp45D)->f2;_LL3B9: {
# 2025
struct _tuple19 _tmp46B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A5);struct _tuple19 _tmp46C=_tmp46B;union Cyc_CfFlowInfo_FlowInfo _tmp473;union Cyc_CfFlowInfo_FlowInfo _tmp472;_LL3CE: _tmp473=_tmp46C.f1;_tmp472=_tmp46C.f2;_LL3CF:;{
struct _tuple19 _tmp46D=Cyc_NewControlFlow_anal_test(env,_tmp473,_tmp4A4);struct _tuple19 _tmp46E=_tmp46D;union Cyc_CfFlowInfo_FlowInfo _tmp471;union Cyc_CfFlowInfo_FlowInfo _tmp470;_LL3D1: _tmp471=_tmp46E.f1;_tmp470=_tmp46E.f2;_LL3D2:;
return({struct _tuple19 _tmp46F;_tmp46F.f1=_tmp471;({union Cyc_CfFlowInfo_FlowInfo _tmp7E3=Cyc_CfFlowInfo_join_flow(_tmp45B,_tmp472,_tmp470);_tmp46F.f2=_tmp7E3;});_tmp46F;});};}case 7: _LL3BA: _tmp4A7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp45D)->f1;_tmp4A6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp45D)->f2;_LL3BB: {
# 2029
struct _tuple19 _tmp474=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A7);struct _tuple19 _tmp475=_tmp474;union Cyc_CfFlowInfo_FlowInfo _tmp47C;union Cyc_CfFlowInfo_FlowInfo _tmp47B;_LL3D4: _tmp47C=_tmp475.f1;_tmp47B=_tmp475.f2;_LL3D5:;{
struct _tuple19 _tmp476=Cyc_NewControlFlow_anal_test(env,_tmp47B,_tmp4A6);struct _tuple19 _tmp477=_tmp476;union Cyc_CfFlowInfo_FlowInfo _tmp47A;union Cyc_CfFlowInfo_FlowInfo _tmp479;_LL3D7: _tmp47A=_tmp477.f1;_tmp479=_tmp477.f2;_LL3D8:;
return({struct _tuple19 _tmp478;({union Cyc_CfFlowInfo_FlowInfo _tmp7E4=Cyc_CfFlowInfo_join_flow(_tmp45B,_tmp47C,_tmp47A);_tmp478.f1=_tmp7E4;});_tmp478.f2=_tmp479;_tmp478;});};}case 8: _LL3BC: _tmp4A9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp45D)->f1;_tmp4A8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp45D)->f2;_LL3BD: {
# 2033
struct _tuple16 _tmp47D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4A9);struct _tuple16 _tmp47E=_tmp47D;union Cyc_CfFlowInfo_FlowInfo _tmp480;void*_tmp47F;_LL3DA: _tmp480=_tmp47E.f1;_tmp47F=_tmp47E.f2;_LL3DB:;
return Cyc_NewControlFlow_anal_test(env,_tmp480,_tmp4A8);}case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45D)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45D)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45D)->f2)->tl == 0){_LL3BE: _tmp4AA=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45D)->f2)->hd;_LL3BF: {
# 2036
struct _tuple19 _tmp481=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4AA);struct _tuple19 _tmp482=_tmp481;union Cyc_CfFlowInfo_FlowInfo _tmp485;union Cyc_CfFlowInfo_FlowInfo _tmp484;_LL3DD: _tmp485=_tmp482.f1;_tmp484=_tmp482.f2;_LL3DE:;
return({struct _tuple19 _tmp483;_tmp483.f1=_tmp484;_tmp483.f2=_tmp485;_tmp483;});}}else{goto _LL3C0;}}else{goto _LL3C0;}}else{_LL3C0: _tmp4AC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45D)->f1;_tmp4AB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45D)->f2;_LL3C1:
# 2039
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4AC,_tmp4AB);}default: _LL3C2: _LL3C3: {
# 2043
struct _tuple16 _tmp486=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp487=_tmp486;union Cyc_CfFlowInfo_FlowInfo _tmp4A0;void*_tmp49F;_LL3E0: _tmp4A0=_tmp487.f1;_tmp49F=_tmp487.f2;_LL3E1:;{
union Cyc_CfFlowInfo_FlowInfo _tmp488=_tmp4A0;struct Cyc_Dict_Dict _tmp49E;if((_tmp488.BottomFL).tag == 1){_LL3E3: _LL3E4:
 return({struct _tuple19 _tmp489;_tmp489.f1=_tmp4A0;_tmp489.f2=_tmp4A0;_tmp489;});}else{_LL3E5: _tmp49E=((_tmp488.ReachableFL).val).f1;_LL3E6: {
# 2047
struct _tuple15 _tmp48A=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp49F);struct _tuple15 _tmp48B=_tmp48A;void*_tmp49D;struct Cyc_List_List*_tmp49C;_LL3E8: _tmp49D=_tmp48B.f1;_tmp49C=_tmp48B.f2;_LL3E9:;{
void*_tmp48C=_tmp49D;enum Cyc_CfFlowInfo_InitLevel _tmp49B;struct Cyc_Absyn_Vardecl*_tmp49A;void*_tmp499;switch(*((int*)_tmp48C)){case 8: _LL3EB: _tmp49A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp48C)->f1;_tmp499=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp48C)->f2;_LL3EC:
# 2050
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));({struct Cyc_Core_Impossible_exn_struct _tmp7E6=({struct Cyc_Core_Impossible_exn_struct _tmp48E;_tmp48E.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7E5=({const char*_tmp48F="anal_test: bad namedlocation";_tag_dyneither(_tmp48F,sizeof(char),29);});_tmp48E.f1=_tmp7E5;});_tmp48E;});_tmp48D[0]=_tmp7E6;});_tmp48D;}));case 0: _LL3ED: _LL3EE:
 return({struct _tuple19 _tmp490;({union Cyc_CfFlowInfo_FlowInfo _tmp7E7=Cyc_CfFlowInfo_BottomFL();_tmp490.f1=_tmp7E7;});_tmp490.f2=_tmp4A0;_tmp490;});case 2: _LL3EF: _LL3F0:
 goto _LL3F2;case 1: _LL3F1: _LL3F2:
 goto _LL3F4;case 5: _LL3F3: _LL3F4:
 return({struct _tuple19 _tmp491;_tmp491.f1=_tmp4A0;({union Cyc_CfFlowInfo_FlowInfo _tmp7E8=Cyc_CfFlowInfo_BottomFL();_tmp491.f2=_tmp7E8;});_tmp491;});case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp48C)->f1 == Cyc_CfFlowInfo_NoneIL){_LL3F5: _LL3F6:
 goto _LL3F8;}else{_LL3FB: _tmp49B=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp48C)->f1;_LL3FC:
# 2060
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A0,e,_tmp49B,_tmp49C);}case 4: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp48C)->f1 == Cyc_CfFlowInfo_NoneIL){_LL3F7: _LL3F8:
# 2056
 goto _LL3FA;}else{_LL3FD: _LL3FE:
# 2061
 return({struct _tuple19 _tmp495;_tmp495.f1=_tmp4A0;_tmp495.f2=_tmp4A0;_tmp495;});}case 7: _LL3F9: _LL3FA:
# 2058
({void*_tmp492=0;({unsigned int _tmp7EA=e->loc;struct _dyneither_ptr _tmp7E9=({const char*_tmp493="expression may not be initialized";_tag_dyneither(_tmp493,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7EA,_tmp7E9,_tag_dyneither(_tmp492,sizeof(void*),0));});});
return({struct _tuple19 _tmp494;({union Cyc_CfFlowInfo_FlowInfo _tmp7EB=Cyc_CfFlowInfo_BottomFL();_tmp494.f1=_tmp7EB;});({union Cyc_CfFlowInfo_FlowInfo _tmp7EC=Cyc_CfFlowInfo_BottomFL();_tmp494.f2=_tmp7EC;});_tmp494;});default: _LL3FF: _LL400:
# 2062
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp496=_cycalloc(sizeof(*_tmp496));({struct Cyc_Core_Impossible_exn_struct _tmp7EE=({struct Cyc_Core_Impossible_exn_struct _tmp497;_tmp497.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7ED=({const char*_tmp498="anal_test";_tag_dyneither(_tmp498,sizeof(char),10);});_tmp497.f1=_tmp7ED;});_tmp497;});_tmp496[0]=_tmp7EE;});_tmp496;}));}_LL3EA:;};}}_LL3E2:;};}}_LL3B5:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2068
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2070
struct _tuple27*_tmp4AD=ckenv;unsigned int _tmp4B9;struct Cyc_NewControlFlow_AnalEnv*_tmp4B8;struct Cyc_Dict_Dict _tmp4B7;_LL402: _tmp4B9=_tmp4AD->f1;_tmp4B8=_tmp4AD->f2;_tmp4B7=_tmp4AD->f3;_LL403:;{
void*_tmp4AE=root;struct Cyc_Absyn_Vardecl*_tmp4B6;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4AE)->tag == 0){_LL405: _tmp4B6=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4AE)->f1;_LL406:
# 2073
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4B6->type)){
struct _tuple15 _tmp4AF=Cyc_CfFlowInfo_unname_rval((_tmp4B8->fenv)->r,rval);struct _tuple15 _tmp4B0=_tmp4AF;void*_tmp4B5;_LL40A: _tmp4B5=_tmp4B0.f1;_LL40B:;{
void*_tmp4B1=_tmp4B5;switch(*((int*)_tmp4B1)){case 7: _LL40D: _LL40E:
 goto _LL410;case 0: _LL40F: _LL410:
 goto _LL412;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B1)->f1 == Cyc_CfFlowInfo_NoneIL){_LL411: _LL412:
 goto _LL40C;}else{goto _LL413;}default: _LL413: _LL414:
# 2080
({struct Cyc_String_pa_PrintArg_struct _tmp4B4;_tmp4B4.tag=0;({struct _dyneither_ptr _tmp7EF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4B6->name));_tmp4B4.f1=_tmp7EF;});({void*_tmp4B2[1]={& _tmp4B4};({unsigned int _tmp7F1=_tmp4B9;struct _dyneither_ptr _tmp7F0=({const char*_tmp4B3="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4B3,sizeof(char),57);});Cyc_Tcutil_warn(_tmp7F1,_tmp7F0,_tag_dyneither(_tmp4B2,sizeof(void*),1));});});});
goto _LL40C;}_LL40C:;};}
# 2084
goto _LL404;}else{_LL407: _LL408:
 goto _LL404;}_LL404:;};}
# 2089
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4BA=flow;struct Cyc_Dict_Dict _tmp4C0;if((_tmp4BA.BottomFL).tag == 1){_LL416: _LL417:
 return;}else{_LL418: _tmp4C0=((_tmp4BA.ReachableFL).val).f1;_LL419:
# 2093
{struct Cyc_List_List*_tmp4BB=env->param_roots;for(0;_tmp4BB != 0;_tmp4BB=_tmp4BB->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp7F3=env->fenv;struct Cyc_Dict_Dict _tmp7F2=_tmp4C0;Cyc_CfFlowInfo_initlevel(_tmp7F3,_tmp7F2,Cyc_CfFlowInfo_lookup_place(_tmp4C0,(struct Cyc_CfFlowInfo_Place*)_tmp4BB->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4BC=0;({unsigned int _tmp7F5=loc;struct _dyneither_ptr _tmp7F4=({const char*_tmp4BD="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4BD,sizeof(char),76);});Cyc_CfFlowInfo_aerr(_tmp7F5,_tmp7F4,_tag_dyneither(_tmp4BC,sizeof(void*),0));});});}}
# 2097
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4BE=({struct _tuple27 _tmp4BF;_tmp4BF.f1=loc;_tmp4BF.f2=env;_tmp4BF.f3=_tmp4C0;_tmp4BF;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4BE,_tmp4C0);}
# 2101
return;}_LL415:;}
# 2110
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2113
struct Cyc_List_List*_tmp4C1=0;
struct Cyc_List_List*_tmp4C2=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4C3=(struct _tuple24*)x->hd;struct _tuple24*_tmp4C4=_tmp4C3;struct Cyc_Absyn_Vardecl**_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;_LL41B: _tmp4CD=_tmp4C4->f1;_tmp4CC=_tmp4C4->f2;_LL41C:;
if((_tmp4CD != 0  && _tmp4CC != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4CC->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4C5=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4CA=_region_malloc(frgn,sizeof(*_tmp4CA));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7F6=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4CB;_tmp4CB.tag=0;_tmp4CB.f1=*_tmp4CD;_tmp4CB;});_tmp4CA[0]=_tmp7F6;});_tmp4CA;});
struct Cyc_CfFlowInfo_Place*_tmp4C6=({struct Cyc_CfFlowInfo_Place*_tmp4C9=_region_malloc(frgn,sizeof(*_tmp4C9));_tmp4C9->root=(void*)_tmp4C5;_tmp4C9->fields=0;_tmp4C9;});
({struct Cyc_List_List*_tmp7F7=({struct Cyc_List_List*_tmp4C7=_region_malloc(frgn,sizeof(*_tmp4C7));_tmp4C7->hd=_tmp4C6;_tmp4C7->tl=_tmp4C1;_tmp4C7;});_tmp4C1=_tmp7F7;});
({struct Cyc_List_List*_tmp7F8=({struct Cyc_List_List*_tmp4C8=_region_malloc(frgn,sizeof(*_tmp4C8));_tmp4C8->hd=_tmp4CC;_tmp4C8->tl=_tmp4C2;_tmp4C8;});_tmp4C2=_tmp7F8;});}}}
# 2125
return({struct _tuple1 _tmp4CE;_tmp4CE.f1=_tmp4C1;_tmp4CE.f2=_tmp4C2;_tmp4CE;});}struct _tuple28{int f1;void*f2;};
# 2131
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2138
union Cyc_CfFlowInfo_FlowInfo _tmp4CF=flow;struct Cyc_Dict_Dict _tmp4E4;if((_tmp4CF.BottomFL).tag == 1){_LL41E: _LL41F:
# 2140
({void*_tmp4D0=0;({struct _dyneither_ptr _tmp7F9=({const char*_tmp4D1="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp4D1,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F9,_tag_dyneither(_tmp4D0,sizeof(void*),0));});});}else{_LL420: _tmp4E4=((_tmp4CF.ReachableFL).val).f1;_LL421: {
# 2147
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Absyn_exp_unsigned_one;
int _tmp4D3=0;
int _tmp4D4=1;
void*_tmp4D5=Cyc_CfFlowInfo_lookup_place(_tmp4E4,place);
void*_tmp4D6=_tmp4D5;
# 2159
int varok=0;
{void*_tmp4D7=_tmp4D5;struct Cyc_Absyn_Vardecl*_tmp4E2;void*_tmp4E1;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D7)->tag == 8){_LL423: _tmp4E2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D7)->f1;_tmp4E1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D7)->f2;_LL424:
# 2162
 if(vd == _tmp4E2){
_tmp4D6=_tmp4E1;
# 2165
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2167
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4D2,_tmp4D4,_tmp4D6,& _tmp4D3)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0;({struct _dyneither_ptr _tmp7FA=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2171
Cyc_Absynpp_qvar2string(vd->name));_tmp4DA.f1=_tmp7FA;});({void*_tmp4D8[1]={& _tmp4DA};({unsigned int _tmp7FC=loc;struct _dyneither_ptr _tmp7FB=({const char*_tmp4D9="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp4D9,sizeof(char),58);});Cyc_CfFlowInfo_aerr(_tmp7FC,_tmp7FB,_tag_dyneither(_tmp4D8,sizeof(void*),1));});});});}else{
# 2174
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E4,_tmp4D6)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=0;({struct _dyneither_ptr _tmp7FD=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2177
Cyc_Absynpp_qvar2string(vd->name));_tmp4DD.f1=_tmp7FD;});({void*_tmp4DB[1]={& _tmp4DD};({unsigned int _tmp7FF=loc;struct _dyneither_ptr _tmp7FE=({const char*_tmp4DC="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp4DC,sizeof(char),84);});Cyc_CfFlowInfo_aerr(_tmp7FF,_tmp7FE,_tag_dyneither(_tmp4DB,sizeof(void*),1));});});});else{
# 2179
varok=1;}}}else{
# 2182
varok=1;}}else{
# 2185
goto _LL426;}
goto _LL422;}else{_LL425: _LL426:
# 2189
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0;({struct _dyneither_ptr _tmp800=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2194
Cyc_Absynpp_qvar2string(vd->name));_tmp4E0.f1=_tmp800;});({void*_tmp4DE[1]={& _tmp4E0};({unsigned int _tmp802=loc;struct _dyneither_ptr _tmp801=({const char*_tmp4DF="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp4DF,sizeof(char),85);});Cyc_CfFlowInfo_aerr(_tmp802,_tmp801,_tag_dyneither(_tmp4DE,sizeof(void*),1));});});});}
goto _LL422;}_LL422:;}
# 2198
return({struct _tuple28 _tmp4E3;_tmp4E3.f1=varok;_tmp4E3.f2=_tmp4D6;_tmp4E3;});}}_LL41D:;}
# 2204
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp4E5=p;void*_tmp4ED;struct Cyc_List_List*_tmp4EC;_LL428: _tmp4ED=_tmp4E5->root;_tmp4EC=_tmp4E5->fields;_LL429:;
if(_tmp4EC != 0)
({void*_tmp4E6=0;({struct _dyneither_ptr _tmp803=({const char*_tmp4E7="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp4E7,sizeof(char),53);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp803,_tag_dyneither(_tmp4E6,sizeof(void*),0));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp4E8=_tmp4ED;struct Cyc_Absyn_Vardecl*_tmp4EB;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E8)->tag == 0){_LL42B: _tmp4EB=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E8)->f1;_LL42C:
 return _tmp4EB;}else{_LL42D: _LL42E:
({void*_tmp4E9=0;({struct _dyneither_ptr _tmp804=({const char*_tmp4EA="unconsume_params: root is not a varroot";_tag_dyneither(_tmp4EA,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp804,_tag_dyneither(_tmp4E9,sizeof(void*),0));});});}_LL42A:;};}
# 2223 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2231
{union Cyc_CfFlowInfo_FlowInfo _tmp4EE=flow;struct Cyc_Dict_Dict _tmp507;if((_tmp4EE.BottomFL).tag == 1){_LL430: _LL431:
 return flow;}else{_LL432: _tmp507=((_tmp4EE.ReachableFL).val).f1;_LL433: {
# 2238
struct _tuple18 _tmp4EF=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp4F0=_tmp4EF;union Cyc_CfFlowInfo_FlowInfo _tmp506;union Cyc_CfFlowInfo_AbsLVal _tmp505;_LL435: _tmp506=_tmp4F0.f1;_tmp505=_tmp4F0.f2;_LL436:;
# 2241
{union Cyc_CfFlowInfo_AbsLVal _tmp4F1=_tmp505;struct Cyc_CfFlowInfo_Place*_tmp504;if((_tmp4F1.PlaceL).tag == 1){_LL438: _tmp504=(_tmp4F1.PlaceL).val;_LL439: {
# 2245
void*_tmp4F2=Cyc_CfFlowInfo_lookup_place(_tmp507,_tmp504);
{void*_tmp4F3=_tmp4F2;struct Cyc_Absyn_Vardecl*_tmp503;void*_tmp502;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F3)->tag == 8){_LL43D: _tmp503=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F3)->f1;_tmp502=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F3)->f2;_LL43E: {
# 2251
void*new_rval;
if(_tmp503 == vd){
# 2255
if(varok){
# 2257
({void*_tmp805=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp502);_tmp4F2=_tmp805;});
# 2262
if(ropt != 0){
# 2268
struct _tuple16 _tmp4F4=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp807=env->fenv;struct _tuple16 _tmp806=({struct _tuple16 _tmp4F8;_tmp4F8.f1=_tmp506;_tmp4F8.f2=_tmp4F2;_tmp4F8;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp807,_tmp806,({struct _tuple16 _tmp4F9;_tmp4F9.f1=_tmp506;_tmp4F9.f2=ropt;_tmp4F9;}));});
# 2268
struct _tuple16 _tmp4F5=_tmp4F4;union Cyc_CfFlowInfo_FlowInfo _tmp4F7;void*_tmp4F6;_LL442: _tmp4F7=_tmp4F5.f1;_tmp4F6=_tmp4F5.f2;_LL443:;
# 2272
_tmp506=_tmp4F7;new_rval=_tmp4F6;}else{
# 2277
new_rval=_tmp4F2;}}else{
# 2280
new_rval=_tmp502;}
# 2282
{union Cyc_CfFlowInfo_FlowInfo _tmp4FA=_tmp506;struct Cyc_Dict_Dict _tmp4FE;struct Cyc_List_List*_tmp4FD;if((_tmp4FA.ReachableFL).tag == 2){_LL445: _tmp4FE=((_tmp4FA.ReachableFL).val).f1;_tmp4FD=((_tmp4FA.ReachableFL).val).f2;_LL446:
# 2284
({union Cyc_CfFlowInfo_FlowInfo _tmp809=({struct Cyc_Dict_Dict _tmp808=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp4FE,_tmp504,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp808,_tmp4FD);});flow=_tmp809;});
goto _LL444;}else{_LL447: _LL448:
# 2287
({void*_tmp4FB=0;({struct _dyneither_ptr _tmp80A=({const char*_tmp4FC="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp4FC,sizeof(char),42);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp80A,_tag_dyneither(_tmp4FB,sizeof(void*),0));});});}_LL444:;}
# 2289
goto _LL43C;}else{
# 2291
goto _LL440;}
goto _LL43C;}}else{_LL43F: _LL440:
# 2298
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0;({struct _dyneither_ptr _tmp80B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));_tmp501.f1=_tmp80B;});({void*_tmp4FF[1]={& _tmp501};({unsigned int _tmp80D=loc;struct _dyneither_ptr _tmp80C=({const char*_tmp500="aliased expression %s was overwritten";_tag_dyneither(_tmp500,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp80D,_tmp80C,_tag_dyneither(_tmp4FF,sizeof(void*),1));});});});
# 2315 "new_control_flow.cyc"
goto _LL43C;}_LL43C:;}
# 2317
goto _LL437;}}else{_LL43A: _LL43B:
# 2323
 goto _LL437;}_LL437:;}
# 2325
goto _LL42F;}}_LL42F:;}
# 2327
return flow;}
# 2340 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2346
{union Cyc_CfFlowInfo_FlowInfo _tmp508=flow;if((_tmp508.BottomFL).tag == 1){_LL44A: _LL44B:
 return flow;}else{_LL44C: _LL44D:
 goto _LL449;}_LL449:;}{
# 2350
int _tmp509=unconsume_exps != 0;
{struct Cyc_List_List*_tmp50A=consumed_vals;for(0;_tmp50A != 0;
(_tmp50A=_tmp50A->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2357
struct Cyc_Absyn_Vardecl*_tmp50B=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp50A->hd);
struct _tuple28 _tmp50C=
is_region_open?({struct _tuple28 _tmp510;_tmp510.f1=1;_tmp510.f2=0;_tmp510;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp50A->hd,_tmp509,_tmp50B,flow,loc);
# 2358
struct _tuple28 _tmp50D=_tmp50C;int _tmp50F;void*_tmp50E;_LL44F: _tmp50F=_tmp50D.f1;_tmp50E=_tmp50D.f2;_LL450:;
# 2364
if(_tmp509)
({union Cyc_CfFlowInfo_FlowInfo _tmp80E=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,_tmp50B,_tmp50F,_tmp50E,flow,loc);flow=_tmp80E;});}}
# 2367
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2371
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2374
struct Cyc_CfFlowInfo_FlowEnv*_tmp511=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp512=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp513=_tmp512;struct Cyc_Core_Opt*_tmp527;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Stmt*_tmp525;unsigned int _tmp524;_LL452: _tmp527=_tmp513->pat_vars;_tmp526=_tmp513->where_clause;_tmp525=_tmp513->body;_tmp524=_tmp513->loc;_LL453:;{
struct _tuple1 _tmp514=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp527))->v);struct _tuple1 _tmp515=_tmp514;struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp522;_LL455: _tmp523=_tmp515.f1;_tmp522=_tmp515.f2;_LL456:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp527->v,_tmp523 != 0,_tmp524,exp_loc);
# 2382
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp516=env->unique_pat_vars;
# 2385
if(Cyc_Tcpat_has_vars(_tmp527))
({struct Cyc_List_List*_tmp810=({struct Cyc_List_List*_tmp517=_region_malloc(env->r,sizeof(*_tmp517));({struct _tuple29*_tmp80F=({struct _tuple29*_tmp518=_region_malloc(env->r,sizeof(*_tmp518));_tmp518->f1=0;_tmp518->f2=_tmp525;_tmp518->f3=_tmp523;_tmp518->f4=_tmp522;_tmp518;});_tmp517->hd=_tmp80F;});_tmp517->tl=_tmp516;_tmp517;});env->unique_pat_vars=_tmp810;});
# 2390
if(_tmp526 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp526;
struct _tuple19 _tmp519=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp51A=_tmp519;union Cyc_CfFlowInfo_FlowInfo _tmp51C;union Cyc_CfFlowInfo_FlowInfo _tmp51B;_LL458: _tmp51C=_tmp51A.f1;_tmp51B=_tmp51A.f2;_LL459:;
inflow=_tmp51B;
({union Cyc_CfFlowInfo_FlowInfo _tmp811=Cyc_NewControlFlow_anal_stmt(env,_tmp51C,_tmp525,0);clause_outflow=_tmp811;});}else{
# 2396
({union Cyc_CfFlowInfo_FlowInfo _tmp812=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp525,0);clause_outflow=_tmp812;});}
# 2398
env->unique_pat_vars=_tmp516;{
union Cyc_CfFlowInfo_FlowInfo _tmp51D=clause_outflow;if((_tmp51D.BottomFL).tag == 1){_LL45B: _LL45C:
 goto _LL45A;}else{_LL45D: _LL45E:
# 2403
({union Cyc_CfFlowInfo_FlowInfo _tmp813=Cyc_NewControlFlow_unconsume_params(env,_tmp523,_tmp522,0,clause_outflow,_tmp524);clause_outflow=_tmp813;});
# 2405
if(scs->tl == 0)
return clause_outflow;else{
# 2410
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp51E=0;({unsigned int _tmp815=_tmp525->loc;struct _dyneither_ptr _tmp814=({const char*_tmp51F="switch clause may implicitly fallthru";_tag_dyneither(_tmp51F,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp815,_tmp814,_tag_dyneither(_tmp51E,sizeof(void*),0));});});else{
# 2413
({void*_tmp520=0;({unsigned int _tmp817=_tmp525->loc;struct _dyneither_ptr _tmp816=({const char*_tmp521="switch clause may implicitly fallthru";_tag_dyneither(_tmp521,sizeof(char),38);});Cyc_Tcutil_warn(_tmp817,_tmp816,_tag_dyneither(_tmp520,sizeof(void*),0));});});}
# 2415
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2417
goto _LL45A;}_LL45A:;};};};}
# 2420
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2425
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2427
struct _tuple30*_tmp528=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp533;struct Cyc_Dict_Dict _tmp532;unsigned int _tmp531;_LL460: _tmp533=_tmp528->f1;_tmp532=_tmp528->f2;_tmp531=_tmp528->f3;_LL461:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2430
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp530;_tmp530.tag=0;_tmp530.f1=vd;_tmp530;});
# 2432
struct _tuple15 _tmp529=({struct _RegionHandle*_tmp818=(_tmp533->fenv)->r;Cyc_CfFlowInfo_unname_rval(_tmp818,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp532,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));});
# 2432
struct _tuple15 _tmp52A=_tmp529;void*_tmp52F;_LL463: _tmp52F=_tmp52A.f1;_LL464:;{
# 2434
void*_tmp52B=_tmp52F;switch(*((int*)_tmp52B)){case 7: _LL466: _LL467:
 goto _LL469;case 0: _LL468: _LL469:
 goto _LL46B;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp52B)->f1 == Cyc_CfFlowInfo_NoneIL){_LL46A: _LL46B:
 goto _LL465;}else{goto _LL46C;}default: _LL46C: _LL46D:
# 2439
({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0;({struct _dyneither_ptr _tmp819=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp52E.f1=_tmp819;});({void*_tmp52C[1]={& _tmp52E};({unsigned int _tmp81B=_tmp531;struct _dyneither_ptr _tmp81A=({const char*_tmp52D="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp52D,sizeof(char),68);});Cyc_Tcutil_warn(_tmp81B,_tmp81A,_tag_dyneither(_tmp52C,sizeof(void*),1));});});});
# 2441
goto _LL465;}_LL465:;};}}
# 2446
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2448
{union Cyc_CfFlowInfo_FlowInfo _tmp534=inflow;struct Cyc_Dict_Dict _tmp541;if((_tmp534.ReachableFL).tag == 2){_LL46F: _tmp541=((_tmp534.ReachableFL).val).f1;_LL470: {
# 2450
struct _tuple30 _tmp535=({struct _tuple30 _tmp540;_tmp540.f1=env;_tmp540.f2=_tmp541;_tmp540.f3=decl->loc;_tmp540;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp536=env->fenv;
{void*_tmp537=decl->r;void*_tmp538=_tmp537;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp53E;struct Cyc_Absyn_Vardecl*_tmp53D;switch(*((int*)_tmp538)){case 0: _LL474: _tmp53D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp538)->f1;_LL475:
# 2454
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp535,_tmp53D);
goto _LL473;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp538)->f2 != 0){_LL476: _tmp53E=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp538)->f2)->v;_LL477: {
# 2457
struct _tuple1 _tmp539=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp53E);struct _tuple1 _tmp53A=_tmp539;struct Cyc_List_List*_tmp53C;_LL47D: _tmp53C=_tmp53A.f1;_LL47E:;{
struct Cyc_List_List*_tmp53B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp53C);
_tmp53F=_tmp53B;goto _LL479;};}}else{goto _LL47A;}case 3: _LL478: _tmp53F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp538)->f1;_LL479:
# 2461
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp535,_tmp53F);
goto _LL473;default: _LL47A: _LL47B:
 goto _LL473;}_LL473:;}
# 2465
goto _LL46E;}}else{_LL471: _LL472:
 goto _LL46E;}_LL46E:;}
# 2468
return;}
# 2474
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2478
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp542=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp543=_tmp542;int _tmp547;struct Cyc_Absyn_Stmt*_tmp546;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp544;_LL480: _tmp547=_tmp543->f1;_tmp546=_tmp543->f2;_tmp545=_tmp543->f3;_tmp544=_tmp543->f4;_LL481:;
({union Cyc_CfFlowInfo_FlowInfo _tmp81C=Cyc_NewControlFlow_unconsume_params(env,_tmp545,_tmp544,_tmp547,inflow,dest->loc);inflow=_tmp81C;});}}
# 2483
return inflow;}
# 2489
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2492
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp548=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp549=_tmp548;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5EC;union Cyc_CfFlowInfo_FlowInfo*_tmp5EB;_LL483: _tmp5EC=_tmp549.f1;_tmp5EB=_tmp549.f2;_LL484:;
inflow=*_tmp5EB;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp54A=env->fenv;
# 2499
void*_tmp54B=s->r;void*_tmp54C=_tmp54B;struct Cyc_Absyn_Stmt*_tmp5EA;struct Cyc_Absyn_Decl*_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5E8;struct Cyc_Absyn_Vardecl*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E6;unsigned int _tmp5E5;struct Cyc_Absyn_Stmt*_tmp5E4;struct Cyc_List_List*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E2;unsigned int _tmp5E1;struct Cyc_Absyn_Stmt*_tmp5E0;struct Cyc_Absyn_Stmt*_tmp5DF;struct Cyc_List_List*_tmp5DE;void*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_List_List*_tmp5DB;void*_tmp5DA;struct Cyc_List_List*_tmp5D9;struct Cyc_Absyn_Switch_clause*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D6;struct Cyc_Absyn_Stmt*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D4;struct Cyc_Absyn_Stmt*_tmp5D3;struct Cyc_Absyn_Stmt*_tmp5D2;struct Cyc_Absyn_Stmt*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Stmt*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;struct Cyc_Absyn_Stmt*_tmp5CD;struct Cyc_Absyn_Stmt*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CB;struct Cyc_Absyn_Stmt*_tmp5CA;struct Cyc_Absyn_Stmt*_tmp5C9;struct Cyc_Absyn_Stmt*_tmp5C8;struct Cyc_Absyn_Stmt*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C5;switch(*((int*)_tmp54C)){case 0: _LL486: _LL487:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1 == 0){_LL488: _LL489:
# 2503
 if(env->noreturn)
({void*_tmp54D=0;({unsigned int _tmp81E=s->loc;struct _dyneither_ptr _tmp81D=({const char*_tmp54E="`noreturn' function might return";_tag_dyneither(_tmp54E,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp81E,_tmp81D,_tag_dyneither(_tmp54D,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL48A: _tmp5C5=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_LL48B:
# 2509
 if(env->noreturn)
({void*_tmp54F=0;({unsigned int _tmp820=s->loc;struct _dyneither_ptr _tmp81F=({const char*_tmp550="`noreturn' function might return";_tag_dyneither(_tmp550,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp820,_tmp81F,_tag_dyneither(_tmp54F,sizeof(void*),0));});});{
struct _tuple16 _tmp551=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C5));struct _tuple16 _tmp552=_tmp551;union Cyc_CfFlowInfo_FlowInfo _tmp554;void*_tmp553;_LL4AF: _tmp554=_tmp552.f1;_tmp553=_tmp552.f2;_LL4B0:;
({union Cyc_CfFlowInfo_FlowInfo _tmp821=Cyc_NewControlFlow_use_Rval(env,_tmp5C5->loc,_tmp554,_tmp553);_tmp554=_tmp821;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp554);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp554,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL48C: _tmp5C6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_LL48D: {
# 2518
struct _tuple17*_tmp555=rval_opt;void**_tmp55B;int _tmp55A;if(_tmp555 != 0){_LL4B2: _tmp55B=(void**)& _tmp555->f1;_tmp55A=_tmp555->f2;_LL4B3: {
# 2520
struct _tuple16 _tmp556=Cyc_NewControlFlow_anal_Rexp(env,_tmp55A,inflow,_tmp5C6);struct _tuple16 _tmp557=_tmp556;union Cyc_CfFlowInfo_FlowInfo _tmp559;void*_tmp558;_LL4B7: _tmp559=_tmp557.f1;_tmp558=_tmp557.f2;_LL4B8:;
*_tmp55B=_tmp558;
return _tmp559;}}else{_LL4B4: _LL4B5:
# 2524
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5C6)).f1;}_LL4B1:;}case 2: _LL48E: _tmp5C8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5C7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL48F:
# 2528
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp824=env;union Cyc_CfFlowInfo_FlowInfo _tmp823=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5C8,0);struct Cyc_Absyn_Stmt*_tmp822=_tmp5C7;Cyc_NewControlFlow_anal_stmt(_tmp824,_tmp823,_tmp822,rval_opt);});case 4: _LL490: _tmp5CB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5CA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_tmp5C9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp54C)->f3;_LL491: {
# 2531
struct _tuple19 _tmp55C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5CB);struct _tuple19 _tmp55D=_tmp55C;union Cyc_CfFlowInfo_FlowInfo _tmp561;union Cyc_CfFlowInfo_FlowInfo _tmp560;_LL4BA: _tmp561=_tmp55D.f1;_tmp560=_tmp55D.f2;_LL4BB:;{
# 2538
union Cyc_CfFlowInfo_FlowInfo _tmp55E=Cyc_NewControlFlow_anal_stmt(env,_tmp560,_tmp5C9,0);
union Cyc_CfFlowInfo_FlowInfo _tmp55F=Cyc_NewControlFlow_anal_stmt(env,_tmp561,_tmp5CA,0);
return Cyc_CfFlowInfo_join_flow(_tmp54A,_tmp55F,_tmp55E);};}case 5: _LL492: _tmp5CE=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1).f1;_tmp5CD=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1).f2;_tmp5CC=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL493: {
# 2546
struct _tuple20 _tmp562=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5CD);struct _tuple20 _tmp563=_tmp562;union Cyc_CfFlowInfo_FlowInfo*_tmp56A;_LL4BD: _tmp56A=_tmp563.f2;_LL4BE:;{
union Cyc_CfFlowInfo_FlowInfo _tmp564=*_tmp56A;
struct _tuple19 _tmp565=Cyc_NewControlFlow_anal_test(env,_tmp564,_tmp5CE);struct _tuple19 _tmp566=_tmp565;union Cyc_CfFlowInfo_FlowInfo _tmp569;union Cyc_CfFlowInfo_FlowInfo _tmp568;_LL4C0: _tmp569=_tmp566.f1;_tmp568=_tmp566.f2;_LL4C1:;{
union Cyc_CfFlowInfo_FlowInfo _tmp567=Cyc_NewControlFlow_anal_stmt(env,_tmp569,_tmp5CC,0);
Cyc_NewControlFlow_update_flow(env,_tmp5CD,_tmp567);
return _tmp568;};};}case 14: _LL494: _tmp5D1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5D0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2).f1;_tmp5CF=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2).f2;_LL495: {
# 2556
union Cyc_CfFlowInfo_FlowInfo _tmp56B=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5D1,0);
struct _tuple20 _tmp56C=Cyc_NewControlFlow_pre_stmt_check(env,_tmp56B,_tmp5CF);struct _tuple20 _tmp56D=_tmp56C;union Cyc_CfFlowInfo_FlowInfo*_tmp573;_LL4C3: _tmp573=_tmp56D.f2;_LL4C4:;{
union Cyc_CfFlowInfo_FlowInfo _tmp56E=*_tmp573;
struct _tuple19 _tmp56F=Cyc_NewControlFlow_anal_test(env,_tmp56E,_tmp5D0);struct _tuple19 _tmp570=_tmp56F;union Cyc_CfFlowInfo_FlowInfo _tmp572;union Cyc_CfFlowInfo_FlowInfo _tmp571;_LL4C6: _tmp572=_tmp570.f1;_tmp571=_tmp570.f2;_LL4C7:;
Cyc_NewControlFlow_update_flow(env,_tmp5D1,_tmp572);
return _tmp571;};}case 9: _LL496: _tmp5D7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5D6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2).f1;_tmp5D5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2).f2;_tmp5D4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54C)->f3).f1;_tmp5D3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54C)->f3).f2;_tmp5D2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54C)->f4;_LL497: {
# 2565
union Cyc_CfFlowInfo_FlowInfo _tmp574=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5D7)).f1;
struct _tuple20 _tmp575=Cyc_NewControlFlow_pre_stmt_check(env,_tmp574,_tmp5D5);struct _tuple20 _tmp576=_tmp575;union Cyc_CfFlowInfo_FlowInfo*_tmp582;_LL4C9: _tmp582=_tmp576.f2;_LL4CA:;{
union Cyc_CfFlowInfo_FlowInfo _tmp577=*_tmp582;
struct _tuple19 _tmp578=Cyc_NewControlFlow_anal_test(env,_tmp577,_tmp5D6);struct _tuple19 _tmp579=_tmp578;union Cyc_CfFlowInfo_FlowInfo _tmp581;union Cyc_CfFlowInfo_FlowInfo _tmp580;_LL4CC: _tmp581=_tmp579.f1;_tmp580=_tmp579.f2;_LL4CD:;{
union Cyc_CfFlowInfo_FlowInfo _tmp57A=Cyc_NewControlFlow_anal_stmt(env,_tmp581,_tmp5D2,0);
struct _tuple20 _tmp57B=Cyc_NewControlFlow_pre_stmt_check(env,_tmp57A,_tmp5D3);struct _tuple20 _tmp57C=_tmp57B;union Cyc_CfFlowInfo_FlowInfo*_tmp57F;_LL4CF: _tmp57F=_tmp57C.f2;_LL4D0:;{
union Cyc_CfFlowInfo_FlowInfo _tmp57D=*_tmp57F;
union Cyc_CfFlowInfo_FlowInfo _tmp57E=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp57D,_tmp5D4)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5D5,_tmp57E);
return _tmp580;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2 != 0){_LL498: _tmp5D9=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5D8=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL499: {
# 2577
struct _RegionHandle _tmp583=_new_region("temp");struct _RegionHandle*temp=& _tmp583;_push_region(temp);
{struct _tuple23 _tmp584=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp5D9,1,1);struct _tuple23 _tmp585=_tmp584;union Cyc_CfFlowInfo_FlowInfo _tmp58D;struct Cyc_List_List*_tmp58C;_LL4D2: _tmp58D=_tmp585.f1;_tmp58C=_tmp585.f2;_LL4D3:;
# 2580
({union Cyc_CfFlowInfo_FlowInfo _tmp825=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp5D8->body);inflow=_tmp825;});{
# 2582
struct Cyc_List_List*_tmp586=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D8->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp826=Cyc_NewControlFlow_add_vars(_tmp54A,_tmp58D,_tmp586,_tmp54A->unknown_all,s->loc,0);_tmp58D=_tmp826;});
# 2585
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D8->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp587=(struct _tuple24*)x->hd;struct _tuple24*_tmp588=_tmp587;struct Cyc_Absyn_Vardecl**_tmp58A;struct Cyc_Absyn_Exp*_tmp589;_LL4D5: _tmp58A=_tmp588->f1;_tmp589=_tmp588->f2;_LL4D6:;
if(_tmp58A != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp82D=({struct Cyc_CfFlowInfo_FlowEnv*_tmp82C=_tmp54A;struct Cyc_NewControlFlow_AnalEnv*_tmp82B=env;union Cyc_CfFlowInfo_FlowInfo _tmp82A=_tmp58D;struct Cyc_Absyn_Vardecl*_tmp829=*_tmp58A;struct Cyc_Absyn_Exp*_tmp828=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5D9))->hd;void*_tmp827=(void*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp82C,_tmp82B,_tmp82A,_tmp829,_tmp828,_tmp827,s->loc);});_tmp58D=_tmp82D;});
_tmp58C=_tmp58C->tl;
_tmp5D9=_tmp5D9->tl;}}}
# 2593
Cyc_NewControlFlow_update_flow(env,_tmp5D8->body,_tmp58D);{
union Cyc_CfFlowInfo_FlowInfo _tmp58B=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp58B;};};}
# 2578
;_pop_region(temp);}}else{_LL4AC: _LL4AD:
# 2762
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));({struct Cyc_Core_Impossible_exn_struct _tmp82F=({struct Cyc_Core_Impossible_exn_struct _tmp5C3;_tmp5C3.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp82E=({const char*_tmp5C4="anal_stmt -- bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5C4,sizeof(char),56);});_tmp5C3.f1=_tmp82E;});_tmp5C3;});_tmp5C2[0]=_tmp82F;});_tmp5C2;}));}case 6: _LL49A: _LL49B:
# 2600
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp58E=inflow;if((_tmp58E.ReachableFL).tag == 2){_LL4D8: _LL4D9:
# 2603
{void*_tmp58F=Cyc_Tcutil_compress(env->return_type);void*_tmp590=_tmp58F;switch(*((int*)_tmp590)){case 0: _LL4DD: _LL4DE:
 goto _LL4DC;case 7: _LL4DF: _LL4E0:
 goto _LL4E2;case 6: _LL4E1: _LL4E2:
# 2607
({void*_tmp591=0;({unsigned int _tmp831=s->loc;struct _dyneither_ptr _tmp830=({const char*_tmp592="break may cause function not to return a value";_tag_dyneither(_tmp592,sizeof(char),47);});Cyc_Tcutil_warn(_tmp831,_tmp830,_tag_dyneither(_tmp591,sizeof(void*),0));});});
goto _LL4DC;default: _LL4E3: _LL4E4:
# 2610
({void*_tmp593=0;({unsigned int _tmp833=s->loc;struct _dyneither_ptr _tmp832=({const char*_tmp594="break may cause function not to return a value";_tag_dyneither(_tmp594,sizeof(char),47);});Cyc_Tcutil_terr(_tmp833,_tmp832,_tag_dyneither(_tmp593,sizeof(void*),0));});});
goto _LL4DC;}_LL4DC:;}
# 2613
goto _LL4D7;}else{_LL4DA: _LL4DB:
 goto _LL4D7;}_LL4D7:;}
# 2616
if(env->noreturn)
({void*_tmp595=0;({unsigned int _tmp835=s->loc;struct _dyneither_ptr _tmp834=({const char*_tmp596="`noreturn' function might return";_tag_dyneither(_tmp596,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp835,_tmp834,_tag_dyneither(_tmp595,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2622
goto _LL49D;case 7: _LL49C: _LL49D:
 goto _LL49F;case 8: _LL49E: _LL49F: {
# 2626
struct Cyc_Absyn_Stmt*_tmp597=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp597 == 0)
({void*_tmp598=0;({unsigned int _tmp837=s->loc;struct _dyneither_ptr _tmp836=({const char*_tmp599="jump has no target (should be impossible)";_tag_dyneither(_tmp599,sizeof(char),42);});Cyc_Tcutil_terr(_tmp837,_tmp836,_tag_dyneither(_tmp598,sizeof(void*),0));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp838=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp597));inflow=_tmp838;});
# 2631
Cyc_NewControlFlow_update_flow(env,_tmp597,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _LL4A0: _tmp5DC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5DB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_tmp5DA=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp54C)->f3;_LL4A1:
# 2637
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp5DB,_tmp5DC->loc);case 15: _LL4A2: _tmp5DF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5DE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_tmp5DD=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp54C)->f3;_LL4A3: {
# 2642
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5DF,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2650
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2653
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5DE,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp59A=scs_outflow;if((_tmp59A.BottomFL).tag == 1){_LL4E6: _LL4E7:
 goto _LL4E5;}else{_LL4E8: _LL4E9:
({void*_tmp59B=0;({unsigned int _tmp83A=s->loc;struct _dyneither_ptr _tmp839=({const char*_tmp59C="last catch clause may implicitly fallthru";_tag_dyneither(_tmp59C,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp83A,_tmp839,_tag_dyneither(_tmp59B,sizeof(void*),0));});});}_LL4E5:;}
# 2659
outflow=s1_outflow;
# 2661
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->r)->f2 != 0){_LL4A4: _tmp5E3=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->r)->f2)->v;_tmp5E2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->r)->f3;_tmp5E1=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->loc;_tmp5E0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL4A5: {
# 2671
struct _tuple1 _tmp59D=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp5E3);struct _tuple1 _tmp59E=_tmp59D;struct Cyc_List_List*_tmp5A3;struct Cyc_List_List*_tmp5A2;_LL4EB: _tmp5A3=_tmp59E.f1;_tmp5A2=_tmp59E.f2;_LL4EC:;
({union Cyc_CfFlowInfo_FlowInfo _tmp83B=Cyc_NewControlFlow_initialize_pat_vars(_tmp54A,env,inflow,_tmp5E3,_tmp5A3 != 0,_tmp5E1,_tmp5E2->loc);inflow=_tmp83B;});{
struct Cyc_List_List*_tmp59F=env->unique_pat_vars;
# 2675
({struct Cyc_List_List*_tmp83D=({struct Cyc_List_List*_tmp5A0=_region_malloc(env->r,sizeof(*_tmp5A0));({struct _tuple29*_tmp83C=({struct _tuple29*_tmp5A1=_region_malloc(env->r,sizeof(*_tmp5A1));_tmp5A1->f1=0;_tmp5A1->f2=s;_tmp5A1->f3=_tmp5A3;_tmp5A1->f4=_tmp5A2;_tmp5A1;});_tmp5A0->hd=_tmp83C;});_tmp5A0->tl=_tmp59F;_tmp5A0;});env->unique_pat_vars=_tmp83D;});
# 2680
({union Cyc_CfFlowInfo_FlowInfo _tmp83E=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E0,rval_opt);inflow=_tmp83E;});
env->unique_pat_vars=_tmp59F;
# 2684
({union Cyc_CfFlowInfo_FlowInfo _tmp83F=Cyc_NewControlFlow_unconsume_params(env,_tmp5A3,_tmp5A2,0,inflow,_tmp5E1);inflow=_tmp83F;});
# 2688
return inflow;};}}else{goto _LL4A8;}case 4: _LL4A6: _tmp5E7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->r)->f2;_tmp5E6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->r)->f3;_tmp5E5=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1)->loc;_tmp5E4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;if(_tmp5E6 != 0){_LL4A7: {
# 2700
struct Cyc_List_List l=({struct Cyc_List_List _tmp5C1;_tmp5C1.hd=_tmp5E6;_tmp5C1.tl=0;_tmp5C1;});
union Cyc_CfFlowInfo_FlowInfo _tmp5A4=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5A5=Cyc_NewControlFlow_anal_Lexp(env,_tmp5A4,0,0,_tmp5E6);struct _tuple18 _tmp5A6=_tmp5A5;union Cyc_CfFlowInfo_AbsLVal _tmp5C0;_LL4EE: _tmp5C0=_tmp5A6.f2;_LL4EF:;{
struct _tuple16 _tmp5A7=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5A4,_tmp5E6);struct _tuple16 _tmp5A8=_tmp5A7;union Cyc_CfFlowInfo_FlowInfo _tmp5BF;_LL4F1: _tmp5BF=_tmp5A8.f1;_LL4F2:;{
struct Cyc_List_List*_tmp5A9=0;
struct Cyc_List_List*_tmp5AA=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5AB=_tmp5BF;struct Cyc_Dict_Dict _tmp5B9;struct Cyc_List_List*_tmp5B8;if((_tmp5AB.ReachableFL).tag == 2){_LL4F4: _tmp5B9=((_tmp5AB.ReachableFL).val).f1;_tmp5B8=((_tmp5AB.ReachableFL).val).f2;_LL4F5:
# 2708
{union Cyc_CfFlowInfo_AbsLVal _tmp5AC=_tmp5C0;struct Cyc_CfFlowInfo_Place*_tmp5B7;if((_tmp5AC.PlaceL).tag == 1){_LL4F9: _tmp5B7=(_tmp5AC.PlaceL).val;_LL4FA: {
# 2712
void*_tmp5AD=Cyc_CfFlowInfo_lookup_place(_tmp5B9,_tmp5B7);
({void*_tmp841=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5AE=_region_malloc(_tmp54A->r,sizeof(*_tmp5AE));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp840=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5AF;_tmp5AF.tag=8;_tmp5AF.f1=_tmp5E7;_tmp5AF.f2=_tmp5AD;_tmp5AF;});_tmp5AE[0]=_tmp840;});_tmp5AE;});_tmp5AD=_tmp841;});
({struct Cyc_Dict_Dict _tmp842=Cyc_CfFlowInfo_assign_place(_tmp54A,_tmp5E6->loc,_tmp5B9,_tmp5B7,_tmp5AD);_tmp5B9=_tmp842;});
({union Cyc_CfFlowInfo_FlowInfo _tmp843=Cyc_CfFlowInfo_ReachableFL(_tmp5B9,_tmp5B8);_tmp5BF=_tmp843;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B0=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B5=_region_malloc(_tmp54A->r,sizeof(*_tmp5B5));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp844=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5B6;_tmp5B6.tag=0;_tmp5B6.f1=_tmp5E7;_tmp5B6;});_tmp5B5[0]=_tmp844;});_tmp5B5;});
struct Cyc_CfFlowInfo_Place*_tmp5B1=({struct Cyc_CfFlowInfo_Place*_tmp5B4=_region_malloc(_tmp54A->r,sizeof(*_tmp5B4));_tmp5B4->root=(void*)_tmp5B0;_tmp5B4->fields=0;_tmp5B4;});
({struct Cyc_List_List*_tmp845=({struct Cyc_List_List*_tmp5B2=_region_malloc(_tmp54A->r,sizeof(*_tmp5B2));_tmp5B2->hd=_tmp5B1;_tmp5B2->tl=_tmp5A9;_tmp5B2;});_tmp5A9=_tmp845;});
({struct Cyc_List_List*_tmp846=({struct Cyc_List_List*_tmp5B3=_region_malloc(_tmp54A->r,sizeof(*_tmp5B3));_tmp5B3->hd=_tmp5E6;_tmp5B3->tl=_tmp5AA;_tmp5B3;});_tmp5AA=_tmp846;});
goto _LL4F8;};}}else{_LL4FB: _LL4FC:
# 2727
 goto _LL4F8;}_LL4F8:;}
# 2729
goto _LL4F3;}else{_LL4F6: _LL4F7:
# 2731
 goto _LL4F3;}_LL4F3:;}{
# 2734
struct Cyc_List_List _tmp5BA=({struct Cyc_List_List _tmp5BE;_tmp5BE.hd=_tmp5E7;_tmp5BE.tl=0;_tmp5BE;});
({union Cyc_CfFlowInfo_FlowInfo _tmp847=Cyc_NewControlFlow_add_vars(_tmp54A,_tmp5BF,& _tmp5BA,_tmp54A->unknown_all,_tmp5E5,0);_tmp5BF=_tmp847;});{
# 2738
struct Cyc_List_List*_tmp5BB=env->unique_pat_vars;
({struct Cyc_List_List*_tmp849=({struct Cyc_List_List*_tmp5BC=_region_malloc(env->r,sizeof(*_tmp5BC));({struct _tuple29*_tmp848=({struct _tuple29*_tmp5BD=_region_malloc(env->r,sizeof(*_tmp5BD));_tmp5BD->f1=1;_tmp5BD->f2=s;_tmp5BD->f3=_tmp5A9;_tmp5BD->f4=_tmp5AA;_tmp5BD;});_tmp5BC->hd=_tmp848;});_tmp5BC->tl=_tmp5BB;_tmp5BC;});env->unique_pat_vars=_tmp849;});
# 2744
({union Cyc_CfFlowInfo_FlowInfo _tmp84A=Cyc_NewControlFlow_anal_stmt(env,_tmp5BF,_tmp5E4,rval_opt);_tmp5BF=_tmp84A;});
env->unique_pat_vars=_tmp5BB;
# 2748
({union Cyc_CfFlowInfo_FlowInfo _tmp84B=Cyc_NewControlFlow_unconsume_params(env,_tmp5A9,_tmp5AA,1,_tmp5BF,_tmp5E5);_tmp5BF=_tmp84B;});
# 2752
return _tmp5BF;};};};};}}else{goto _LL4A8;}default: _LL4A8: _tmp5E9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp5E8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL4A9:
# 2755
({union Cyc_CfFlowInfo_FlowInfo _tmp84F=({struct Cyc_NewControlFlow_AnalEnv*_tmp84E=env;union Cyc_CfFlowInfo_FlowInfo _tmp84D=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5E9);struct Cyc_Absyn_Stmt*_tmp84C=_tmp5E8;Cyc_NewControlFlow_anal_stmt(_tmp84E,_tmp84D,_tmp84C,rval_opt);});outflow=_tmp84F;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5E9);
return outflow;}default: _LL4AA: _tmp5EA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL4AB:
# 2760
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5EA,rval_opt);}_LL485:;};}
# 2767
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2771
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp5ED=env->fenv;
void*_tmp5EE=decl->r;void*_tmp5EF=_tmp5EE;struct Cyc_Absyn_Tvar*_tmp606;struct Cyc_Absyn_Vardecl*_tmp605;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Fndecl*_tmp603;struct Cyc_List_List*_tmp602;struct Cyc_Absyn_Vardecl*_tmp601;switch(*((int*)_tmp5EF)){case 0: _LL4FE: _tmp601=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_LL4FF: {
# 2781
struct Cyc_List_List _tmp5F0=({struct Cyc_List_List _tmp5F6;_tmp5F6.hd=_tmp601;_tmp5F6.tl=0;_tmp5F6;});
({union Cyc_CfFlowInfo_FlowInfo _tmp850=Cyc_NewControlFlow_add_vars(_tmp5ED,inflow,& _tmp5F0,_tmp5ED->unknown_none,decl->loc,0);inflow=_tmp850;});{
struct Cyc_Absyn_Exp*_tmp5F1=_tmp601->initializer;
if(_tmp5F1 == 0)
return inflow;{
struct _tuple16 _tmp5F2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5F1);struct _tuple16 _tmp5F3=_tmp5F2;union Cyc_CfFlowInfo_FlowInfo _tmp5F5;void*_tmp5F4;_LL509: _tmp5F5=_tmp5F3.f1;_tmp5F4=_tmp5F3.f2;_LL50A:;
return Cyc_NewControlFlow_do_initialize_var(_tmp5ED,env,_tmp5F5,_tmp601,_tmp5F1,_tmp5F4,decl->loc);};};}case 3: _LL500: _tmp602=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_LL501:
# 2790
 return Cyc_NewControlFlow_add_vars(_tmp5ED,inflow,_tmp602,_tmp5ED->unknown_none,decl->loc,0);case 1: _LL502: _tmp603=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_LL503:
# 2793
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp5ED,inflow,_tmp603);{
void*t=(void*)_check_null(_tmp603->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp5F7=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp603->fn_vardecl);
# 2799
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp854=_tmp5ED;union Cyc_CfFlowInfo_FlowInfo _tmp853=inflow;struct Cyc_List_List*_tmp852=({struct Cyc_List_List*_tmp5F8=_region_malloc(env->r,sizeof(*_tmp5F8));_tmp5F8->hd=_tmp5F7;_tmp5F8->tl=0;_tmp5F8;});void*_tmp851=_tmp5ED->unknown_all;Cyc_NewControlFlow_add_vars(_tmp854,_tmp853,_tmp852,_tmp851,decl->loc,0);});};case 4: _LL504: _tmp606=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_tmp605=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EF)->f2;_tmp604=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EF)->f3;_LL505:
# 2802
 if(_tmp604 != 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));({struct Cyc_Core_Impossible_exn_struct _tmp856=({struct Cyc_Core_Impossible_exn_struct _tmp5FA;_tmp5FA.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp855=({const char*_tmp5FB="found open expression in declaration!";_tag_dyneither(_tmp5FB,sizeof(char),38);});_tmp5FA.f1=_tmp855;});_tmp5FA;});_tmp5F9[0]=_tmp856;});_tmp5F9;}));{
struct Cyc_List_List _tmp5FC=({struct Cyc_List_List _tmp5FD;_tmp5FD.hd=_tmp605;_tmp5FD.tl=0;_tmp5FD;});
return Cyc_NewControlFlow_add_vars(_tmp5ED,inflow,& _tmp5FC,_tmp5ED->unknown_all,decl->loc,0);};default: _LL506: _LL507:
# 2808
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));({struct Cyc_Core_Impossible_exn_struct _tmp858=({struct Cyc_Core_Impossible_exn_struct _tmp5FF;_tmp5FF.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp857=({const char*_tmp600="anal_decl: unexpected decl variant";_tag_dyneither(_tmp600,sizeof(char),35);});_tmp5FF.f1=_tmp857;});_tmp5FF;});_tmp5FE[0]=_tmp858;});_tmp5FE;}));}_LL4FD:;}
# 2816
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp607;_push_handler(& _tmp607);{int _tmp609=0;if(setjmp(_tmp607.handler))_tmp609=1;if(!_tmp609){
{struct _RegionHandle _tmp60A=_new_region("frgn");struct _RegionHandle*frgn=& _tmp60A;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp85B=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp85A=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp859=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp85B,_tmp85A,_tmp859,fd);});}
# 2819
;_pop_region(frgn);}
# 2818
;_pop_handler();}else{void*_tmp608=(void*)_exn_thrown;void*_tmp60B=_tmp608;void*_tmp60E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp60B)->tag == Cyc_Dict_Absent){_LL50C: _LL50D:
# 2825
 if(Cyc_Position_num_errors > 0)
goto _LL50B;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp60C=_cycalloc_atomic(sizeof(*_tmp60C));({struct Cyc_Dict_Absent_exn_struct _tmp85C=({struct Cyc_Dict_Absent_exn_struct _tmp60D;_tmp60D.tag=Cyc_Dict_Absent;_tmp60D;});_tmp60C[0]=_tmp85C;});_tmp60C;}));}}else{_LL50E: _tmp60E=_tmp60B;_LL50F:(int)_rethrow(_tmp60E);}_LL50B:;}};}
# 2831
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2836
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp60F=r;unsigned int _tmp611;if((_tmp60F.RParam).tag == 5){_LL511: _tmp611=(_tmp60F.RParam).val;_LL512: {
# 2839
struct Cyc_Absyn_Vardecl*_tmp610=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp611);
if(!_tmp610->escapes)
return Cyc_Relations_RVar(_tmp610);
return r;}}else{_LL513: _LL514:
 return r;}_LL510:;}
# 2847
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2851
struct _RegionHandle*_tmp612=fenv->r;
unsigned int _tmp613=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp85D=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp613,1);inflow=_tmp85D;});{
# 2857
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp614=({union Cyc_CfFlowInfo_FlowInfo _tmp64C=inflow;if((_tmp64C.ReachableFL).tag != 2)_throw_match();(_tmp64C.ReachableFL).val;});struct _tuple14 _tmp615=_tmp614;struct Cyc_Dict_Dict _tmp64B;struct Cyc_List_List*_tmp64A;_LL516: _tmp64B=_tmp615.f1;_tmp64A=_tmp615.f2;_LL517:;{
# 2862
struct Cyc_List_List*_tmp616=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp617=fd->requires_relns;for(0;_tmp617 != 0;_tmp617=_tmp617->tl){
struct Cyc_Relations_Reln*_tmp618=(struct Cyc_Relations_Reln*)_tmp617->hd;
({struct Cyc_List_List*_tmp862=({struct _RegionHandle*_tmp861=_tmp612;union Cyc_Relations_RelnOp _tmp860=Cyc_NewControlFlow_translate_rop(_tmp616,_tmp618->rop1);enum Cyc_Relations_Relation _tmp85F=_tmp618->relation;union Cyc_Relations_RelnOp _tmp85E=
Cyc_NewControlFlow_translate_rop(_tmp616,_tmp618->rop2);
# 2865
Cyc_Relations_add_relation(_tmp861,_tmp860,_tmp85F,_tmp85E,_tmp64A);});_tmp64A=_tmp862;});}}{
# 2871
struct Cyc_List_List*atts;
{void*_tmp619=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp61A=_tmp619;struct Cyc_List_List*_tmp61D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp61A)->tag == 9){_LL519: _tmp61D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp61A)->f1).attributes;_LL51A:
 atts=_tmp61D;goto _LL518;}else{_LL51B: _LL51C:
({void*_tmp61B=0;({struct _dyneither_ptr _tmp863=({const char*_tmp61C="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp61C,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp863,_tag_dyneither(_tmp61B,sizeof(void*),0));});});}_LL518:;}
# 2878
for(0;atts != 0;atts=atts->tl){
void*_tmp61E=(void*)atts->hd;void*_tmp61F=_tmp61E;int _tmp63F;int _tmp63E;int _tmp63D;switch(*((int*)_tmp61F)){case 21: _LL51E: _tmp63D=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp61F)->f1;_LL51F: {
# 2881
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp620=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp63D - 1);
void*t=Cyc_Tcutil_compress(_tmp620->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp621=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp866=fenv;void*_tmp865=elttype;struct Cyc_Absyn_Exp*_tmp864=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp866,_tmp865,_tmp864,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp622=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp629=_region_malloc(_tmp612,sizeof(*_tmp629));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp867=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp62A;_tmp62A.tag=2;_tmp62A.f1=_tmp63D;_tmp62A.f2=t;_tmp62A;});_tmp629[0]=_tmp867;});_tmp629;});
struct Cyc_CfFlowInfo_Place*_tmp623=({struct Cyc_CfFlowInfo_Place*_tmp628=_region_malloc(_tmp612,sizeof(*_tmp628));_tmp628->root=(void*)_tmp622;_tmp628->fields=0;_tmp628;});
({struct Cyc_Dict_Dict _tmp868=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp64B,(void*)_tmp622,_tmp621);_tmp64B=_tmp868;});
({struct Cyc_Dict_Dict _tmp86D=({struct Cyc_Dict_Dict _tmp86C=_tmp64B;void*_tmp86B=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp624=_region_malloc(_tmp612,sizeof(*_tmp624));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp869=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp625;_tmp625.tag=0;_tmp625.f1=_tmp620;_tmp625;});_tmp624[0]=_tmp869;});_tmp624;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp86C,_tmp86B,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp626=_region_malloc(_tmp612,sizeof(*_tmp626));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp86A=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp627;_tmp627.tag=5;_tmp627.f1=_tmp623;_tmp627;});_tmp626[0]=_tmp86A;});_tmp626;}));});_tmp64B=_tmp86D;});
goto _LL51D;}case 20: _LL520: _tmp63E=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp61F)->f1;_LL521: {
# 2894
struct Cyc_Absyn_Vardecl*_tmp62B=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp63E - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp62B->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp62C=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp634=_region_malloc(_tmp612,sizeof(*_tmp634));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp86E=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp635;_tmp635.tag=2;_tmp635.f1=_tmp63E;_tmp635.f2=elttype;_tmp635;});_tmp634[0]=_tmp86E;});_tmp634;});
struct Cyc_CfFlowInfo_Place*_tmp62D=({struct Cyc_CfFlowInfo_Place*_tmp633=_region_malloc(_tmp612,sizeof(*_tmp633));_tmp633->root=(void*)_tmp62C;_tmp633->fields=0;_tmp633;});
({struct Cyc_Dict_Dict _tmp871=({struct Cyc_Dict_Dict _tmp870=_tmp64B;void*_tmp86F=(void*)_tmp62C;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp870,_tmp86F,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp64B=_tmp871;});
({struct Cyc_Dict_Dict _tmp876=({struct Cyc_Dict_Dict _tmp875=_tmp64B;void*_tmp874=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp62E=_region_malloc(_tmp612,sizeof(*_tmp62E));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp872=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp62F;_tmp62F.tag=0;_tmp62F.f1=_tmp62B;_tmp62F;});_tmp62E[0]=_tmp872;});_tmp62E;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp875,_tmp874,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp630=_region_malloc(_tmp612,sizeof(*_tmp630));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp873=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp631;_tmp631.tag=5;_tmp631.f1=_tmp62D;_tmp631;});_tmp630[0]=_tmp873;});_tmp630;}));});_tmp64B=_tmp876;});
({struct Cyc_List_List*_tmp877=({struct Cyc_List_List*_tmp632=_region_malloc(_tmp612,sizeof(*_tmp632));_tmp632->hd=_tmp62D;_tmp632->tl=param_roots;_tmp632;});param_roots=_tmp877;});
goto _LL51D;}case 22: _LL522: _tmp63F=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp61F)->f1;_LL523: {
# 2903
struct Cyc_Absyn_Vardecl*_tmp636=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp63F - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp636->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp637=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp63B=_region_malloc(_tmp612,sizeof(*_tmp63B));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp878=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp63C;_tmp63C.tag=0;_tmp63C.f1=_tmp636;_tmp63C;});_tmp63B[0]=_tmp878;});_tmp63B;});
struct Cyc_CfFlowInfo_Place*_tmp638=({struct Cyc_CfFlowInfo_Place*_tmp63A=_region_malloc(_tmp612,sizeof(*_tmp63A));_tmp63A->root=(void*)_tmp637;_tmp63A->fields=0;_tmp63A;});
({struct Cyc_List_List*_tmp879=({struct Cyc_List_List*_tmp639=_region_malloc(_tmp612,sizeof(*_tmp639));_tmp639->hd=_tmp638;_tmp639->tl=noconsume_roots;_tmp639;});noconsume_roots=_tmp879;});}
# 2909
goto _LL51D;}default: _LL524: _LL525:
 goto _LL51D;}_LL51D:;}
# 2912
({union Cyc_CfFlowInfo_FlowInfo _tmp87A=Cyc_CfFlowInfo_ReachableFL(_tmp64B,_tmp64A);inflow=_tmp87A;});{
# 2914
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp612,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp649=_region_malloc(_tmp612,sizeof(*_tmp649));_tmp649->all_tables=tables;({struct Cyc_Hashtable_Table*_tmp87B=
# 2919
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp649->succ_table=_tmp87B;});({struct Cyc_Hashtable_Table*_tmp87C=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp649->pat_pop_table=_tmp87C;});_tmp649->r=_tmp612;_tmp649->fenv=fenv;_tmp649->iterate_again=1;_tmp649->iteration_num=0;_tmp649->in_try=0;_tmp649->tryflow=inflow;_tmp649->noreturn=noreturn;_tmp649->return_type=fd->ret_type;_tmp649->unique_pat_vars=0;_tmp649->param_roots=param_roots;_tmp649->noconsume_params=noconsume_roots;_tmp649->flow_table=flow_table;_tmp649->return_relns=fd->ensures_relns;_tmp649;});
# 2924
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2930
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp87D=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp87D;});}{
# 2933
union Cyc_CfFlowInfo_FlowInfo _tmp640=outflow;if((_tmp640.BottomFL).tag == 1){_LL527: _LL528:
 goto _LL526;}else{_LL529: _LL52A:
# 2936
 Cyc_NewControlFlow_check_init_params(_tmp613,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp613);
# 2940
if(noreturn)
({void*_tmp641=0;({unsigned int _tmp87F=_tmp613;struct _dyneither_ptr _tmp87E=({const char*_tmp642="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp642,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp87F,_tmp87E,_tag_dyneither(_tmp641,sizeof(void*),0));});});else{
# 2943
void*_tmp643=Cyc_Tcutil_compress(fd->ret_type);void*_tmp644=_tmp643;switch(*((int*)_tmp644)){case 0: _LL52C: _LL52D:
 goto _LL52B;case 7: _LL52E: _LL52F:
 goto _LL531;case 6: _LL530: _LL531:
# 2947
({void*_tmp645=0;({unsigned int _tmp881=_tmp613;struct _dyneither_ptr _tmp880=({const char*_tmp646="function may not return a value";_tag_dyneither(_tmp646,sizeof(char),32);});Cyc_Tcutil_warn(_tmp881,_tmp880,_tag_dyneither(_tmp645,sizeof(void*),0));});});goto _LL52B;default: _LL532: _LL533:
# 2949
({void*_tmp647=0;({unsigned int _tmp883=_tmp613;struct _dyneither_ptr _tmp882=({const char*_tmp648="function may not return a value";_tag_dyneither(_tmp648,sizeof(char),32);});Cyc_CfFlowInfo_aerr(_tmp883,_tmp882,_tag_dyneither(_tmp647,sizeof(void*),0));});});goto _LL52B;}_LL52B:;}
# 2951
goto _LL526;}_LL526:;};};};};};}
# 2955
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp64D=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp64E=_tmp64D;struct Cyc_List_List*_tmp652;struct Cyc_List_List*_tmp651;struct Cyc_List_List*_tmp650;struct Cyc_Absyn_Fndecl*_tmp64F;switch(*((int*)_tmp64E)){case 1: _LL535: _tmp64F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp64E)->f1;_LL536:
 Cyc_NewControlFlow_check_fun(tables,_tmp64F);goto _LL534;case 11: _LL537: _tmp650=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp64E)->f1;_LL538:
 _tmp651=_tmp650;goto _LL53A;case 10: _LL539: _tmp651=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp64E)->f2;_LL53A:
 _tmp652=_tmp651;goto _LL53C;case 9: _LL53B: _tmp652=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp64E)->f2;_LL53C:
 Cyc_NewControlFlow_cf_check(tables,_tmp652);goto _LL534;case 12: _LL53D: _LL53E:
 goto _LL534;default: _LL53F: _LL540:
 goto _LL534;}_LL534:;};}}

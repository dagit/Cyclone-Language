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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
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
typedef unsigned int Cyc_Position_seg_t;
# 37
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 972 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1173
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
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
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
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
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
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};
# 58
typedef void*Cyc_CfFlowInfo_pathcon_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_path_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 72
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_AllIL  = 1};
# 78
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 137 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 152
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env();
# 155
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 161
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 171
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 181
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
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
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;
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
# 48 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
# 51
static int Cyc_NewControlFlow_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;
# 59
while(i < minsz){
char c1=*((const char*)_check_dyneither_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_dyneither_subscript(s2,sizeof(char),i));
if(c1 == '\000'){
if(c2 == '\000')return 0;else{
return - 1;}}else{
if(c2 == '\000')return 1;else{
# 67
int diff=c1 - c2;
if(diff != 0)return diff;}}
# 70
++ i;}
# 72
if(sz1 == sz2)return 0;
if(minsz < sz2){
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{
# 77
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
return 1;}}};}
# 83
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};
# 90
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
# 123 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 125
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*,struct Cyc_CfFlowInfo_Place*,void*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp5;}else{_LL3: _LL4:
({void*_tmp648=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp647=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;({struct Cyc_NewControlFlow_CFStmtAnnot _tmp646=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3.f1=_tmp646;});_tmp3;});_tmp2[0]=_tmp647;});_tmp2;});s->annot=_tmp648;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_cycalloc(sizeof(*_tmp6));({union Cyc_CfFlowInfo_FlowInfo _tmp649=Cyc_CfFlowInfo_BottomFL();_tmp6[0]=_tmp649;});_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 152
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 160
({union Cyc_CfFlowInfo_FlowInfo _tmp64A=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp64A;});
# 166
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp9;_tmp9.f1=_tmp7;_tmp9.f2=_tmp8;_tmp9;});}
# 176
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 184
({union Cyc_CfFlowInfo_FlowInfo _tmp64B=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp64B;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 191
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 193
struct _tuple21*_tmpA=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp14;unsigned int _tmp13;struct Cyc_Dict_Dict _tmp12;_LL6: _tmp14=_tmpA->f1;_tmp13=_tmpA->f2;_tmp12=_tmpA->f3;_LL7:;{
void*_tmpB=root;struct Cyc_Absyn_Vardecl*_tmp11;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->tag == 0){_LL9: _tmp11=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->f1;_LLA:
# 196
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp11->type)){
retry: {void*_tmpC=rval;void*_tmp10;switch(*((int*)_tmpC)){case 8: _LLE: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC)->f2;_LLF:
 rval=_tmp10;goto retry;case 7: _LL10: _LL11:
 goto _LL13;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL12: _LL13:
 goto _LL15;}else{goto _LL16;}case 0: _LL14: _LL15:
 goto _LLD;default: _LL16: _LL17:
# 205
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;({struct _dyneither_ptr _tmp64C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));_tmpF.f1=_tmp64C;});({void*_tmpD[1]={& _tmpF};({unsigned int _tmp64E=_tmp13;struct _dyneither_ptr _tmp64D=({const char*_tmpE="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpE,sizeof(char),64);});Cyc_Warn_warn(_tmp64E,_tmp64D,_tag_dyneither(_tmpD,sizeof(void*),1));});});});
goto _LLD;}_LLD:;}}
# 209
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}
# 217
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp15=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp16=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp17=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp16);
# 223
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp18=({struct _tuple19 _tmp1E;_tmp1E.f1=flow;_tmp1E.f2=_tmp17;_tmp1E;});struct _tuple19 _tmp19=_tmp18;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1C;if(((_tmp19.f1).ReachableFL).tag == 2){if(((_tmp19.f2).ReachableFL).tag == 2){_LL19: _tmp1D=(((_tmp19.f1).ReachableFL).val).f1;_tmp1C=(((_tmp19.f2).ReachableFL).val).f1;_LL1A: {
# 226
struct _tuple21 _tmp1A=({struct _tuple21 _tmp1B;_tmp1B.f1=env;_tmp1B.f2=s->loc;_tmp1B.f3=_tmp1C;_tmp1B;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1A,_tmp1D);
goto _LL18;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 232
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp17,*_tmp16)){
# 239
*_tmp16=_tmp17;
# 243
if(_tmp15->visited == env->iteration_num)
# 245
env->iterate_again=1;}}
# 250
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 254
union Cyc_CfFlowInfo_FlowInfo _tmp1F=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp26;if((_tmp1F.BottomFL).tag == 1){_LL1E: _LL1F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL20: _tmp27=((_tmp1F.ReachableFL).val).f1;_tmp26=((_tmp1F.ReachableFL).val).f2;_LL21:
# 257
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=_cycalloc(sizeof(*_tmp24));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp64F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25;});_tmp24[0]=_tmp64F;});_tmp24;});
void*_tmp21=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp651=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp22=_cycalloc(sizeof(*_tmp22));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp650=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp23;_tmp23.tag=8;_tmp23.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp23.f2=_tmp21;_tmp23;});_tmp22[0]=_tmp650;});_tmp22;});_tmp21=_tmp651;});
# 264
({struct Cyc_Dict_Dict _tmp652=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp20,_tmp21);_tmp27=_tmp652;});}
# 266
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp26);}_LL1D:;}
# 270
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 277
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp28=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=_tmp28;_tmp29->tl=assume;_tmp29;})))
return 0;}
# 282
return 1;}
# 285
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2A=e->r;void*_tmp2B=_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->tag == 14){_LL23: _tmp2C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->f2;_LL24:
 return _tmp2C;}else{_LL25: _LL26:
 return e;}_LL22:;}
# 292
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 294
union Cyc_CfFlowInfo_FlowInfo _tmp2D=inflow;struct Cyc_List_List*_tmp4B;if((_tmp2D.BottomFL).tag == 1){_LL28: _LL29:
 return;}else{_LL2A: _tmp4B=((_tmp2D.ReachableFL).val).f2;_LL2B:
# 297
{void*_tmp2E=Cyc_Tcutil_compress(t);void*_tmp2F=_tmp2E;struct Cyc_List_List*_tmp4A;union Cyc_Absyn_AggrInfoU _tmp49;struct Cyc_List_List*_tmp48;switch(*((int*)_tmp2F)){case 11: _LL2D: _tmp49=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).aggr_info;_tmp48=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).targs;_LL2E: {
# 299
struct Cyc_Absyn_Aggrdecl*_tmp30=Cyc_Absyn_get_known_aggrdecl(_tmp49);
struct Cyc_Absyn_Aggrfield*_tmp31=Cyc_Absyn_lookup_decl_field(_tmp30,f);
struct Cyc_Absyn_Exp*_tmp32=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31))->requires_clause;
if(_tmp32 != 0){
struct _RegionHandle _tmp33=_new_region("temp");struct _RegionHandle*temp=& _tmp33;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp34=({struct _RegionHandle*_tmp654=temp;struct Cyc_List_List*_tmp653=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp30->tvs,_tmp48);Cyc_Tcutil_rsubsexp(_tmp654,_tmp653,_tmp32);});
# 306
if(!({struct Cyc_List_List*_tmp655=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp655,Cyc_Relations_exp2relns(temp,_tmp34));})){
({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp37;_tmp37.tag=0;({struct _dyneither_ptr _tmp656=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp34)));_tmp37.f1=_tmp656;});({void*_tmp35[2]={& _tmp37,& _tmp38};({unsigned int _tmp658=loc;struct _dyneither_ptr _tmp657=({const char*_tmp36="unable to prove %s, required to access %s";_tag_dyneither(_tmp36,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp658,_tmp657,_tag_dyneither(_tmp35,sizeof(void*),2));});});});});
({void*_tmp39=0;({struct Cyc___cycFILE*_tmp65A=Cyc_stderr;struct _dyneither_ptr _tmp659=({const char*_tmp3A="  [recorded facts on path: ";_tag_dyneither(_tmp3A,sizeof(char),28);});Cyc_fprintf(_tmp65A,_tmp659,_tag_dyneither(_tmp39,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp3B=0;({struct Cyc___cycFILE*_tmp65C=Cyc_stderr;struct _dyneither_ptr _tmp65B=({const char*_tmp3C="]\n";_tag_dyneither(_tmp3C,sizeof(char),3);});Cyc_fprintf(_tmp65C,_tmp65B,_tag_dyneither(_tmp3B,sizeof(void*),0));});});}}
# 304
;_pop_region(temp);}
# 313
goto _LL2C;}case 12: _LL2F: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LL30: {
# 315
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_field(_tmp4A,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
if(!({struct Cyc_List_List*_tmp65D=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp65D,Cyc_Relations_exp2relns(temp,_tmp3E));})){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0;({struct _dyneither_ptr _tmp65E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3E)));_tmp42.f1=_tmp65E;});({void*_tmp40[2]={& _tmp42,& _tmp43};({unsigned int _tmp660=loc;struct _dyneither_ptr _tmp65F=({const char*_tmp41="unable to prove %s, required to access %s";_tag_dyneither(_tmp41,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp660,_tmp65F,_tag_dyneither(_tmp40,sizeof(void*),2));});});});});
({void*_tmp44=0;({struct Cyc___cycFILE*_tmp662=Cyc_stderr;struct _dyneither_ptr _tmp661=({const char*_tmp45="  [recorded facts on path: ";_tag_dyneither(_tmp45,sizeof(char),28);});Cyc_fprintf(_tmp662,_tmp661,_tag_dyneither(_tmp44,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp46=0;({struct Cyc___cycFILE*_tmp664=Cyc_stderr;struct _dyneither_ptr _tmp663=({const char*_tmp47="]\n";_tag_dyneither(_tmp47,sizeof(char),3);});Cyc_fprintf(_tmp664,_tmp663,_tag_dyneither(_tmp46,sizeof(void*),0));});});}
# 319
;_pop_region(temp);}
# 326
goto _LL2C;}default: _LL31: _LL32:
 goto _LL2C;}_LL2C:;}
# 329
goto _LL27;}_LL27:;}
# 333
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4C=inflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*_tmp51;if((_tmp4C.BottomFL).tag == 1){_LL34: _LL35:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL36: _tmp52=((_tmp4C.ReachableFL).val).f1;_tmp51=((_tmp4C.ReachableFL).val).f2;_LL37:
# 337
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp52,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D=0;({unsigned int _tmp666=loc;struct _dyneither_ptr _tmp665=({const char*_tmp4E="expression may not be fully initialized";_tag_dyneither(_tmp4E,sizeof(char),40);});Cyc_CfFlowInfo_aerr(_tmp666,_tmp665,_tag_dyneither(_tmp4D,sizeof(void*),0));});});{
struct Cyc_Dict_Dict _tmp4F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp52,r);
if(_tmp52.t == _tmp4F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp50=Cyc_CfFlowInfo_ReachableFL(_tmp4F,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp50);
return _tmp50;};};}_LL33:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 347
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp53=({struct _tuple0 _tmp66;({void*_tmp667=Cyc_Tcutil_compress(t);_tmp66.f1=_tmp667;});_tmp66.f2=r;_tmp66;});struct _tuple0 _tmp54=_tmp53;enum Cyc_Absyn_AggrKind _tmp65;struct Cyc_List_List*_tmp64;struct _dyneither_ptr _tmp63;union Cyc_Absyn_AggrInfoU _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_Absyn_Datatypefield*_tmp5D;struct _dyneither_ptr _tmp5C;void*_tmp5B;switch(*((int*)_tmp54.f2)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL39: _LL3A:
 return;}else{switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL4B;else{goto _LL4B;}case 10: goto _LL4B;case 11: goto _LL4B;case 12: goto _LL4B;case 5: goto _LL49;default: goto _LL4B;}}case 0: _LL3B: _LL3C:
 return;case 7: _LL3D: _LL3E:
 return;case 8: _LL3F: _tmp5B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL40:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp5B);return;default: switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL41: _tmp5D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL42:
# 354
 if(_tmp5D->typs == 0)
return;
_tmp5F=_tmp5D->typs;_tmp5E=_tmp5C;goto _LL44;}else{goto _LL4B;}}else{goto _LL4B;}case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL43: _tmp5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL44: {
# 358
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5F);
{int i=0;for(0;i < sz;(i ++,_tmp5F=_tmp5F->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp66A=env;unsigned int _tmp669=loc;void*_tmp668=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp66A,_tmp669,_tmp668,*((void**)_check_dyneither_subscript(_tmp5E,sizeof(void*),i)));});}}
# 362
return;}}else{goto _LL4B;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL45: _tmp62=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).aggr_info;_tmp61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).targs;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL46: {
# 365
struct Cyc_Absyn_Aggrdecl*_tmp55=Cyc_Absyn_get_known_aggrdecl(_tmp62);
if(_tmp55->impl == 0)return;{
struct Cyc_List_List*_tmp56=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55->impl))->fields;
struct _RegionHandle _tmp57=_new_region("temp");struct _RegionHandle*temp=& _tmp57;_push_region(temp);
{struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp55->tvs,_tmp61);
{int i=0;for(0;i < _get_dyneither_size(_tmp60,sizeof(void*));(i ++,_tmp56=_tmp56->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp56))->hd)->type;
if(_tmp58 != 0)({void*_tmp66B=Cyc_Tcutil_rsubstitute(temp,_tmp58,t);t=_tmp66B;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp60.curr)[i]);}}
# 375
_npop_handler(0);return;}
# 369
;_pop_region(temp);};}}else{goto _LL4B;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL47: _tmp65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f2;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL48:
# 378
{int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(void*));(i ++,_tmp64=_tmp64->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp64))->hd)->type,((void**)_tmp63.curr)[i]);}}
return;}else{goto _LL4B;}case 5: _LL49: if(
Cyc_Tcutil_is_noalias_pointer(t,0)){_LL4A:
({void*_tmp59=0;({unsigned int _tmp66D=loc;struct _dyneither_ptr _tmp66C=({const char*_tmp5A="argument may still contain alias-free pointers";_tag_dyneither(_tmp5A,sizeof(char),47);});Cyc_Warn_warn(_tmp66D,_tmp66C,_tag_dyneither(_tmp59,sizeof(void*),0));});});
return;}else{goto _LL4B;}default: _LL4B: _LL4C:
 return;}}_LL38:;}
# 388
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp67=inflow;struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*_tmp73;if((_tmp67.BottomFL).tag == 1){_LL4E: _LL4F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL50: _tmp74=((_tmp67.ReachableFL).val).f1;_tmp73=((_tmp67.ReachableFL).val).f2;_LL51:
# 392
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp68=0;({struct _dyneither_ptr _tmp66E=({const char*_tmp69="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp69,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp66E,_tag_dyneither(_tmp68,sizeof(void*),0));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 396
void*_tmp6A=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6B=r;void*_tmp70;struct Cyc_CfFlowInfo_Place*_tmp6F;void*_tmp6E;switch(*((int*)_tmp6B)){case 8: _LL53: _tmp6E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f2;_LL54:
 r=_tmp6E;goto retry;case 4: _LL55: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL56:
({void*_tmp66F=Cyc_CfFlowInfo_lookup_place(_tmp74,_tmp6F);_tmp70=_tmp66F;});goto _LL58;case 5: _LL57: _tmp70=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL58:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6A,_tmp70);goto _LL52;default: _LL59: _LL5A:
# 402
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6A))
({void*_tmp6C=0;({unsigned int _tmp671=loc;struct _dyneither_ptr _tmp670=({const char*_tmp6D="argument may contain live alias-free pointers";_tag_dyneither(_tmp6D,sizeof(char),46);});Cyc_Warn_warn(_tmp671,_tmp670,_tag_dyneither(_tmp6C,sizeof(void*),0));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL52:;}{
# 406
struct Cyc_Dict_Dict _tmp71=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp74,r);
if(_tmp74.t == _tmp71.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp72=Cyc_CfFlowInfo_ReachableFL(_tmp71,_tmp73);
Cyc_NewControlFlow_update_tryflow(env,_tmp72);
return _tmp72;};};};}_LL4D:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 416
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 421
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp75;_tmp75.f1=inflow;_tmp75.f2=0;_tmp75;});
# 426
do{
struct _tuple16 _tmp76=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp77=_tmp76;union Cyc_CfFlowInfo_FlowInfo _tmp7A;void*_tmp79;_LL5C: _tmp7A=_tmp77.f1;_tmp79=_tmp77.f2;_LL5D:;
inflow=_tmp7A;
({struct Cyc_List_List*_tmp672=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=_tmp79;_tmp78->tl=rvals;_tmp78;});rvals=_tmp672;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 434
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp7B;_tmp7B.f1=inflow;({struct Cyc_List_List*_tmp673=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp7B.f2=_tmp673;});_tmp7B;});}
# 440
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 442
struct _tuple23 _tmp7C=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 442
struct _tuple23 _tmp7D=_tmp7C;union Cyc_CfFlowInfo_FlowInfo _tmp84;struct Cyc_List_List*_tmp83;_LL5F: _tmp84=_tmp7D.f1;_tmp83=_tmp7D.f2;_LL60:;
# 444
{union Cyc_CfFlowInfo_FlowInfo _tmp7E=_tmp84;struct Cyc_Dict_Dict _tmp81;if((_tmp7E.ReachableFL).tag == 2){_LL62: _tmp81=((_tmp7E.ReachableFL).val).f1;_LL63:
# 446
 for(0;_tmp83 != 0;(_tmp83=_tmp83->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp81,(void*)_tmp83->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp7F=0;({unsigned int _tmp675=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp674=({const char*_tmp80="expression may not be initialized";_tag_dyneither(_tmp80,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp675,_tmp674,_tag_dyneither(_tmp7F,sizeof(void*),0));});});}
goto _LL61;}else{_LL64: _LL65:
 goto _LL61;}_LL61:;}
# 452
return({struct _tuple16 _tmp82;_tmp82.f1=_tmp84;_tmp82.f2=(env->fenv)->unknown_all;_tmp82;});}
# 459
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 466
int needs_unconsume=0;
void*_tmp85=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp85,& needs_unconsume))
({void*_tmp86=0;({struct _dyneither_ptr _tmp676=({const char*_tmp87="consume_zero_rval";_tag_dyneither(_tmp87,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp676,_tag_dyneither(_tmp86,sizeof(void*),0));});});else{
# 471
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 475
return new_rval;}}}
# 488 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 493
union Cyc_CfFlowInfo_FlowInfo _tmp88=outflow;struct Cyc_Dict_Dict _tmp90;struct Cyc_List_List*_tmp8F;if((_tmp88.BottomFL).tag == 1){_LL67: _LL68:
 return outflow;}else{_LL69: _tmp90=((_tmp88.ReachableFL).val).f1;_tmp8F=((_tmp88.ReachableFL).val).f2;_LL6A: {
# 496
union Cyc_CfFlowInfo_AbsLVal _tmp89=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp8A=_tmp89;struct Cyc_CfFlowInfo_Place*_tmp8E;if((_tmp8A.UnknownL).tag == 2){_LL6C: _LL6D:
# 500
 return outflow;}else{_LL6E: _tmp8E=(_tmp8A.PlaceL).val;_LL6F: {
# 504
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
({void*_tmp678=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp677=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8C;_tmp8C.tag=8;_tmp8C.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp8C.f2=nzval;_tmp8C;});_tmp8B[0]=_tmp677;});_tmp8B;});nzval=_tmp678;});}
# 508
({void*_tmp679=Cyc_NewControlFlow_consume_zero_rval(env,_tmp90,_tmp8E,e,nzval);nzval=_tmp679;});{
union Cyc_CfFlowInfo_FlowInfo _tmp8D=({struct Cyc_Dict_Dict _tmp67A=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp90,_tmp8E,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp67A,_tmp8F);});
# 513
return _tmp8D;};}}_LL6B:;}}_LL66:;}
# 522
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 524
union Cyc_CfFlowInfo_FlowInfo _tmp91=outflow;struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9C;if((_tmp91.BottomFL).tag == 1){_LL71: _LL72:
 return({struct _tuple19 _tmp92;_tmp92.f1=outflow;_tmp92.f2=outflow;_tmp92;});}else{_LL73: _tmp9D=((_tmp91.ReachableFL).val).f1;_tmp9C=((_tmp91.ReachableFL).val).f2;_LL74: {
# 527
union Cyc_CfFlowInfo_AbsLVal _tmp93=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp94=_tmp93;struct Cyc_CfFlowInfo_Place*_tmp9B;if((_tmp94.UnknownL).tag == 2){_LL76: _LL77:
 return({struct _tuple19 _tmp95;_tmp95.f1=outflow;_tmp95.f2=outflow;_tmp95;});}else{_LL78: _tmp9B=(_tmp94.PlaceL).val;_LL79: {
# 530
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp67C=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp67B=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp97;_tmp97.tag=8;_tmp97.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp97.f2=nzval;_tmp97;});_tmp96[0]=_tmp67B;});_tmp96;});nzval=_tmp67C;});
({void*_tmp67E=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp98=_cycalloc(sizeof(*_tmp98));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp67D=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp99;_tmp99.tag=8;_tmp99.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp99.f2=zval;_tmp99;});_tmp98[0]=_tmp67D;});_tmp98;});zval=_tmp67E;});}
# 536
({void*_tmp67F=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9D,_tmp9B,e,nzval);nzval=_tmp67F;});
({void*_tmp680=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9D,_tmp9B,e,zval);zval=_tmp680;});
return({struct _tuple19 _tmp9A;({union Cyc_CfFlowInfo_FlowInfo _tmp682=({
struct Cyc_Dict_Dict _tmp681=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9D,_tmp9B,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp681,_tmp9C);});_tmp9A.f1=_tmp682;});({union Cyc_CfFlowInfo_FlowInfo _tmp684=({
struct Cyc_Dict_Dict _tmp683=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9D,_tmp9B,zval);Cyc_CfFlowInfo_ReachableFL(_tmp683,_tmp9C);});_tmp9A.f2=_tmp684;});_tmp9A;});}}_LL75:;}}_LL70:;}
# 545
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 548
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
void*_tmpA0=e->r;void*_tmpA1=_tmpA0;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1)->tag == 23){_LL7B: _LL7C:
# 552
{void*_tmpA2=e->annot;void*_tmpA3=_tmpA2;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA4;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA3)->tag == Cyc_CfFlowInfo_UnknownZ){_LL80: _tmpA4=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA3)->f1;_LL81:
# 554
 if(possibly_null  && Cyc_Relations_same_relns(relns,_tmpA4))return;
goto _LL7F;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA3)->tag == Cyc_CfFlowInfo_NotZero){_LL82: _tmpA5=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA3)->f1;_LL83:
# 557
 possibly_null=0;
if(Cyc_Relations_same_relns(relns,_tmpA5))return;
goto _LL7F;}else{_LL84: _LL85:
 goto _LL7F;}}_LL7F:;}
# 562
if(possibly_null)
({void*_tmp686=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp685=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpA7;_tmpA7.tag=Cyc_CfFlowInfo_UnknownZ;_tmpA7.f1=relns;_tmpA7;});_tmpA6[0]=_tmp685;});_tmpA6;});e->annot=_tmp686;});else{
# 566
({void*_tmp688=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp687=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpA9;_tmpA9.tag=Cyc_CfFlowInfo_NotZero;_tmpA9.f1=relns;_tmpA9;});_tmpA8[0]=_tmp687;});_tmpA8;});e->annot=_tmp688;});}
# 568
goto _LL7A;}else{_LL7D: _LL7E:
# 572
 if(possibly_null){
void*_tmpAA=e->annot;void*_tmpAB=_tmpAA;struct Cyc_List_List*_tmpAC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAB)->tag == Cyc_CfFlowInfo_UnknownZ){_LL87: _tmpAC=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAB)->f1;_LL88:
# 575
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
return;}else{_LL89: _LL8A:
 goto _LL86;}_LL86:;}
# 580
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7A;}_LL7A:;}
# 590
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 593
void*_tmpAD=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpAE=_tmpAD;void*_tmpD9;union Cyc_Absyn_Constraint*_tmpD8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->tag == 5){_LL8C: _tmpD9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->f1).elt_typ;_tmpD8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->f1).ptr_atts).bounds;_LL8D: {
# 595
union Cyc_CfFlowInfo_FlowInfo _tmpAF=f;struct Cyc_Dict_Dict _tmpD5;struct Cyc_List_List*_tmpD4;if((_tmpAF.BottomFL).tag == 1){_LL91: _LL92:
# 597
 return({struct _tuple16 _tmpB0;_tmpB0.f1=f;({void*_tmp689=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpD9,0,(env->fenv)->unknown_all);_tmpB0.f2=_tmp689;});_tmpB0;});}else{_LL93: _tmpD5=((_tmpAF.ReachableFL).val).f1;_tmpD4=((_tmpAF.ReachableFL).val).f2;_LL94:
# 600
 if(Cyc_Tcutil_is_bound_one(_tmpD8)){
struct _tuple15 _tmpB1=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpB2=_tmpB1;void*_tmpC3;struct Cyc_List_List*_tmpC2;_LL96: _tmpC3=_tmpB2.f1;_tmpC2=_tmpB2.f2;_LL97:;{
void*_tmpB3=_tmpC3;enum Cyc_CfFlowInfo_InitLevel _tmpC1;void*_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpBF;struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBD;switch(*((int*)_tmpB3)){case 8: _LL99: _tmpBE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_tmpBD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_LL9A:
# 604
({void*_tmpB4=0;({struct _dyneither_ptr _tmp68A=({const char*_tmpB5="named location in anal_derefR";_tag_dyneither(_tmpB5,sizeof(char),30);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp68A,_tag_dyneither(_tmpB4,sizeof(void*),0));});});case 1: _LL9B: _LL9C:
# 606
 Cyc_NewControlFlow_update_relns(e,0,_tmpD4);
goto _LL98;case 4: _LL9D: _tmpBF=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_LL9E:
# 609
 Cyc_NewControlFlow_update_relns(e,0,_tmpD4);
return({struct _tuple16 _tmpB6;_tmpB6.f1=f;({void*_tmp68B=Cyc_CfFlowInfo_lookup_place(_tmpD5,_tmpBF);_tmpB6.f2=_tmp68B;});_tmpB6;});case 5: _LL9F: _tmpC0=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_LLA0:
# 612
 Cyc_NewControlFlow_update_relns(e,1,_tmpD4);
return({struct _tuple16 _tmpB7;_tmpB7.f1=f;_tmpB7.f2=_tmpC0;_tmpB7;});case 0: _LLA1: _LLA2:
# 615
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpB8;({union Cyc_CfFlowInfo_FlowInfo _tmp68C=Cyc_CfFlowInfo_BottomFL();_tmpB8.f1=_tmp68C;});({void*_tmp68D=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpD9,0,(env->fenv)->unknown_all);_tmpB8.f2=_tmp68D;});_tmpB8;});case 2: _LLA3: _tmpC1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_LLA4:
# 619
({union Cyc_CfFlowInfo_FlowInfo _tmp68E=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC1,_tmpC2);f=_tmp68E;});
goto _LLA6;default: _LLA5: _LLA6:
# 622
{void*_tmpB9=e->r;void*_tmpBA=_tmpB9;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBA)->tag == 23){_LLA8: _LLA9:
# 624
({void*_tmp690=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp68F=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpBC;_tmpBC.tag=Cyc_CfFlowInfo_UnknownZ;_tmpBC.f1=_tmpD4;_tmpBC;});_tmpBB[0]=_tmp68F;});_tmpBB;});e->annot=_tmp690;});
goto _LLA7;}else{_LLAA: _LLAB:
# 627
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLA7;}_LLA7:;}
# 630
goto _LL98;}_LL98:;};}else{
# 633
void*_tmpC4=e->annot;void*_tmpC5=_tmpC4;struct Cyc_List_List*_tmpC8;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC5)->tag == Cyc_CfFlowInfo_UnknownZ){_LLAD: _tmpC8=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC5)->f1;_LLAE:
# 635
 if(!Cyc_Relations_same_relns(_tmpD4,_tmpC8))goto _LLB0;
goto _LLAC;}else{_LLAF: _LLB0:
# 638
({void*_tmp692=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp691=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC7;_tmpC7.tag=Cyc_CfFlowInfo_UnknownZ;_tmpC7.f1=_tmpD4;_tmpC7;});_tmpC6[0]=_tmp691;});_tmpC6;});e->annot=_tmp692;});
goto _LLAC;}_LLAC:;}{
# 642
enum Cyc_CfFlowInfo_InitLevel _tmpC9=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD5,r);enum Cyc_CfFlowInfo_InitLevel _tmpCA=_tmpC9;if(_tmpCA == Cyc_CfFlowInfo_NoneIL){_LLB2: _LLB3: {
# 644
struct _tuple15 _tmpCB=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpCC=_tmpCB;void*_tmpD2;_LLB7: _tmpD2=_tmpCC.f1;_LLB8:;
{void*_tmpCD=_tmpD2;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCD)->tag == 7){_LLBA: _LLBB:
# 647
({void*_tmpCE=0;({unsigned int _tmp694=e->loc;struct _dyneither_ptr _tmp693=({const char*_tmpCF="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpCF,sizeof(char),53);});Cyc_CfFlowInfo_aerr(_tmp694,_tmp693,_tag_dyneither(_tmpCE,sizeof(void*),0));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 650
({void*_tmpD0=0;({unsigned int _tmp696=e->loc;struct _dyneither_ptr _tmp695=({const char*_tmpD1="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpD1,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp696,_tmp695,_tag_dyneither(_tmpD0,sizeof(void*),0));});});}_LLB9:;}
# 652
goto _LLB5;}}else{_LLB4: _LLB5:
# 654
 return({struct _tuple16 _tmpD3;_tmpD3.f1=f;({void*_tmp697=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpD9,0,(env->fenv)->unknown_all);_tmpD3.f2=_tmp697;});_tmpD3;});}_LLB1:;};}_LL90:;}}else{_LL8E: _LL8F:
# 657
({void*_tmpD6=0;({struct _dyneither_ptr _tmp698=({const char*_tmpD7="right deref of non-pointer-type";_tag_dyneither(_tmpD7,sizeof(char),32);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp698,_tag_dyneither(_tmpD6,sizeof(void*),0));});});}_LL8B:;}
# 664
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 666
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 669
{void*_tmpDA=e1->r;void*_tmpDB=_tmpDA;void*_tmpDD;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->tag == 1){_LLBF: _tmpDD=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLC0: {
# 671
struct Cyc_Absyn_Vardecl*_tmpDC=Cyc_Tcutil_nonesc_vardecl(_tmpDD);
if(_tmpDC != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpDC);
if(e2_valid_op)
({struct Cyc_List_List*_tmp699=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp699;});}
# 677
goto _LLBE;}}else{_LLC1: _LLC2:
 goto _LLBE;}_LLBE:;}{
# 681
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp69A=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp69A;});}
# 687
return relns;};}
# 695
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 700
struct _tuple18 _tmpDE=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpDF=_tmpDE;union Cyc_CfFlowInfo_AbsLVal _tmpEF;_LLC4: _tmpEF=_tmpDF.f2;_LLC5:;
{struct _tuple18 _tmpE0=({struct _tuple18 _tmpEE;_tmpEE.f1=inflow;_tmpEE.f2=_tmpEF;_tmpEE;});struct _tuple18 _tmpE1=_tmpE0;struct Cyc_Dict_Dict _tmpED;struct Cyc_List_List*_tmpEC;struct Cyc_CfFlowInfo_Place*_tmpEB;if(((_tmpE1.f1).ReachableFL).tag == 2){if(((_tmpE1.f2).PlaceL).tag == 1){_LLC7: _tmpED=(((_tmpE1.f1).ReachableFL).val).f1;_tmpEC=(((_tmpE1.f1).ReachableFL).val).f2;_tmpEB=((_tmpE1.f2).PlaceL).val;_LLC8: {
# 703
void*_tmpE2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 705
struct _tuple15 _tmpE3=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple15 _tmpE4=_tmpE3;void*_tmpE8;struct Cyc_List_List*_tmpE7;_LLCE: _tmpE8=_tmpE4.f1;_tmpE7=_tmpE4.f2;_LLCF:;
for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
({void*_tmp69C=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp69B=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpE6;_tmpE6.tag=8;_tmpE6.f1=(struct Cyc_Absyn_Vardecl*)_tmpE7->hd;_tmpE6.f2=_tmpE2;_tmpE6;});_tmpE5[0]=_tmp69B;});_tmpE5;});_tmpE2=_tmp69C;});}
# 710
({struct Cyc_Dict_Dict _tmp69D=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpED,_tmpEB,_tmpE2);_tmpED=_tmp69D;});
({union Cyc_CfFlowInfo_FlowInfo _tmp69E=Cyc_CfFlowInfo_ReachableFL(_tmpED,_tmpEC);inflow=_tmp69E;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 716
goto _LLC6;}}else{_LLCB: _LLCC:
# 719
({void*_tmpE9=0;({struct _dyneither_ptr _tmp69F=({const char*_tmpEA="noconsume() parameters must be l-values";_tag_dyneither(_tmpEA,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp69F,_tag_dyneither(_tmpE9,sizeof(void*),0));});});
goto _LLC6;}}else{_LLC9: _LLCA:
# 717
 goto _LLC6;}_LLC6:;}
# 722
return inflow;}
# 727
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 734
union Cyc_CfFlowInfo_FlowInfo _tmpF0=outflow;struct Cyc_Dict_Dict _tmpFE;struct Cyc_List_List*_tmpFD;if((_tmpF0.BottomFL).tag == 1){_LLD1: _LLD2:
# 736
 return({struct _tuple16 _tmpF1;({union Cyc_CfFlowInfo_FlowInfo _tmp6A0=Cyc_CfFlowInfo_BottomFL();_tmpF1.f1=_tmp6A0;});_tmpF1.f2=rval;_tmpF1;});}else{_LLD3: _tmpFE=((_tmpF0.ReachableFL).val).f1;_tmpFD=((_tmpF0.ReachableFL).val).f2;_LLD4: {
# 738
union Cyc_CfFlowInfo_AbsLVal _tmpF2=lval;struct Cyc_CfFlowInfo_Place*_tmpFC;if((_tmpF2.PlaceL).tag == 1){_LLD6: _tmpFC=(_tmpF2.PlaceL).val;_LLD7: {
# 746
struct Cyc_Dict_Dict _tmpF3=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpFE,_tmpFC,rval);
({struct Cyc_List_List*_tmp6A1=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpFD,lexp,rexp);_tmpFD=_tmp6A1;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6A2=Cyc_CfFlowInfo_ReachableFL(_tmpF3,_tmpFD);outflow=_tmp6A2;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 752
struct _tuple15 _tmpF4=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmpFE,_tmpFC));struct _tuple15 _tmpF5=_tmpF4;void*_tmpF9;_LLDB: _tmpF9=_tmpF5.f1;_LLDC:;{
void*_tmpF6=_tmpF9;switch(*((int*)_tmpF6)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f1 == Cyc_CfFlowInfo_NoneIL){_LLDE: _LLDF:
 goto _LLE1;}else{goto _LLE4;}case 7: _LLE0: _LLE1:
 goto _LLE3;case 0: _LLE2: _LLE3:
 goto _LLDD;default: _LLE4: _LLE5:
# 758
({void*_tmpF7=0;({unsigned int _tmp6A4=lexp->loc;struct _dyneither_ptr _tmp6A3=({const char*_tmpF8="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpF8,sizeof(char),47);});Cyc_Warn_warn(_tmp6A4,_tmp6A3,_tag_dyneither(_tmpF7,sizeof(void*),0));});});
goto _LLDD;}_LLDD:;};}
# 763
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmpFA;_tmpFA.f1=outflow;_tmpFA.f2=rval;_tmpFA;});}}else{_LLD8: _LLD9:
# 769
 return({struct _tuple16 _tmpFB;({union Cyc_CfFlowInfo_FlowInfo _tmp6A5=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmpFB.f1=_tmp6A5;});_tmpFB.f2=rval;_tmpFB;});}_LLD5:;}}_LLD0:;}
# 776
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 782
union Cyc_CfFlowInfo_FlowInfo _tmpFF=f;struct Cyc_Dict_Dict _tmp105;struct Cyc_List_List*_tmp104;if((_tmpFF.BottomFL).tag == 1){_LLE7: _LLE8:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLE9: _tmp105=((_tmpFF.ReachableFL).val).f1;_tmp104=((_tmpFF.ReachableFL).val).f2;_LLEA:
# 787
({struct Cyc_Dict_Dict _tmp6AC=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6AB=fenv;unsigned int _tmp6AA=loc;struct Cyc_Dict_Dict _tmp6A9=_tmp105;struct Cyc_CfFlowInfo_Place*_tmp6A8=({struct Cyc_CfFlowInfo_Place*_tmp100=_cycalloc(sizeof(*_tmp100));({void*_tmp6A7=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp101=_cycalloc(sizeof(*_tmp101));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6A6=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp102;_tmp102.tag=0;_tmp102.f1=vd;_tmp102;});_tmp101[0]=_tmp6A6;});_tmp101;});_tmp100->root=_tmp6A7;});_tmp100->path=0;_tmp100;});Cyc_CfFlowInfo_assign_place(_tmp6AB,_tmp6AA,_tmp6A9,_tmp6A8,rval);});_tmp105=_tmp6AC;});
# 790
({struct Cyc_List_List*_tmp6AD=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmp104,vd,rexp);_tmp104=_tmp6AD;});{
union Cyc_CfFlowInfo_FlowInfo _tmp103=Cyc_CfFlowInfo_ReachableFL(_tmp105,_tmp104);
Cyc_NewControlFlow_update_tryflow(env,_tmp103);
# 795
return _tmp103;};}_LLE6:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 799
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 806
if(vds == 0)return inflow;{
# 809
struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp6AE=({struct Cyc_List_List*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp107->tl=es;_tmp107;});es=_tmp6AE;});}}
# 815
({union Cyc_CfFlowInfo_FlowInfo _tmp6AF=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp106,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp6AF;});
# 817
({union Cyc_CfFlowInfo_FlowInfo _tmp6B0=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp6B0;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 821
struct _tuple16 _tmp108=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp109=_tmp108;union Cyc_CfFlowInfo_FlowInfo _tmp10B;void*_tmp10A;_LLEC: _tmp10B=_tmp109.f1;_tmp10A=_tmp109.f2;_LLED:;
({union Cyc_CfFlowInfo_FlowInfo _tmp6B1=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp10B,_tmp10A);inflow=_tmp6B1;});}}{
# 829
struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){
struct _tuple24*_tmp10D=(struct _tuple24*)_tmp10C->hd;struct _tuple24*_tmp10E=_tmp10D;struct Cyc_Absyn_Vardecl**_tmp129;struct Cyc_Absyn_Exp*_tmp128;_LLEF: _tmp129=_tmp10E->f1;_tmp128=_tmp10E->f2;_LLF0:;
if(_tmp129 != 0  && _tmp128 != 0){
if(_tmp128->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0;({struct _dyneither_ptr _tmp6B2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp128));_tmp111.f1=_tmp6B2;});({void*_tmp10F[1]={& _tmp111};({struct _dyneither_ptr _tmp6B3=({const char*_tmp110="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp110,sizeof(char),41);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6B3,_tag_dyneither(_tmp10F,sizeof(void*),1));});});});{
# 843
struct Cyc_List_List l=({struct Cyc_List_List _tmp127;_tmp127.hd=_tmp128;_tmp127.tl=0;_tmp127;});
union Cyc_CfFlowInfo_FlowInfo _tmp112=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp113=Cyc_NewControlFlow_anal_Lexp(env,_tmp112,0,0,_tmp128);struct _tuple18 _tmp114=_tmp113;union Cyc_CfFlowInfo_AbsLVal _tmp126;_LLF2: _tmp126=_tmp114.f2;_LLF3:;{
struct _tuple16 _tmp115=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp112,_tmp128);struct _tuple16 _tmp116=_tmp115;union Cyc_CfFlowInfo_FlowInfo _tmp125;void*_tmp124;_LLF5: _tmp125=_tmp116.f1;_tmp124=_tmp116.f2;_LLF6:;{
union Cyc_CfFlowInfo_FlowInfo _tmp117=_tmp125;struct Cyc_Dict_Dict _tmp123;struct Cyc_List_List*_tmp122;if((_tmp117.ReachableFL).tag == 2){_LLF8: _tmp123=((_tmp117.ReachableFL).val).f1;_tmp122=((_tmp117.ReachableFL).val).f2;_LLF9:
# 849
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp118=_tmp126;struct Cyc_CfFlowInfo_Place*_tmp121;if((_tmp118.PlaceL).tag == 1){_LLFD: _tmp121=(_tmp118.PlaceL).val;_LLFE:
# 852
({void*_tmp6B5=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp119=_cycalloc(sizeof(*_tmp119));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6B4=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11A;_tmp11A.tag=8;_tmp11A.f1=*_tmp129;_tmp11A.f2=_tmp124;_tmp11A;});_tmp119[0]=_tmp6B4;});_tmp119;});_tmp124=_tmp6B5;});{
# 855
void*_tmp11B=Cyc_CfFlowInfo_lookup_place(_tmp123,_tmp121);
({void*_tmp6B7=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6B6=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11D;_tmp11D.tag=8;_tmp11D.f1=*_tmp129;_tmp11D.f2=_tmp11B;_tmp11D;});_tmp11C[0]=_tmp6B6;});_tmp11C;});_tmp11B=_tmp6B7;});
({struct Cyc_Dict_Dict _tmp6B8=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp123,_tmp121,_tmp11B);_tmp123=_tmp6B8;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6B9=Cyc_CfFlowInfo_ReachableFL(_tmp123,_tmp122);_tmp125=_tmp6B9;});
goto _LLFC;};}else{_LLFF: _LL100:
# 862
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp128->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp129)->type))
({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0;({struct _dyneither_ptr _tmp6BA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp128));_tmp120.f1=_tmp6BA;});({void*_tmp11E[1]={& _tmp120};({unsigned int _tmp6BC=exp_loc;struct _dyneither_ptr _tmp6BB=({const char*_tmp11F="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp11F,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp6BC,_tmp6BB,_tag_dyneither(_tmp11E,sizeof(void*),1));});});});}_LLFC:;}
# 872
({union Cyc_CfFlowInfo_FlowInfo _tmp6BD=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp125,*_tmp129,_tmp128,_tmp124,pat_loc);inflow=_tmp6BD;});
goto _LLF7;}else{_LLFA: _LLFB:
# 875
 goto _LLF7;}_LLF7:;};};};}}
# 880
return inflow;};};}
# 883
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 885
void*_tmp12A=e->r;void*_tmp12B=_tmp12A;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;switch(*((int*)_tmp12B)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12B)->f1)){case 4: _LL102: _LL103:
 goto _LL105;case 3: _LL104: _LL105:
 goto _LL107;case 5: _LL106: _LL107:
 return 1;default: goto _LL112;}case 20: _LL108: _tmp12E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LL109:
 _tmp12F=_tmp12E;goto _LL10B;case 21: _LL10A: _tmp12F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LL10B:
 _tmp130=_tmp12F;goto _LL10D;case 22: _LL10C: _tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LL10D:
# 892
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp130,cast_ok);case 23: _LL10E: _tmp131=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LL10F: {
# 894
void*_tmp12C=Cyc_Tcutil_compress((void*)_check_null(_tmp131->topt));void*_tmp12D=_tmp12C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12D)->tag == 10){_LL115: _LL116:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp131,cast_ok);}else{_LL117: _LL118:
 return 0;}_LL114:;}case 14: _LL110: _tmp132=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12B)->f2;_LL111:
# 899
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp132,cast_ok);else{
return 0;}default: _LL112: _LL113:
 return 0;}_LL101:;}
# 905
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp133=*rop;union Cyc_Relations_RelnOp _tmp134=_tmp133;unsigned int _tmp138;unsigned int _tmp137;switch((_tmp134.RParamNumelts).tag){case 5: _LL11A: _tmp137=(_tmp134.RParam).val;_LL11B: {
# 908
struct Cyc_Absyn_Exp*_tmp135=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp137);
return Cyc_Relations_exp2relnop(_tmp135,rop);}case 6: _LL11C: _tmp138=(_tmp134.RParamNumelts).val;_LL11D: {
# 911
struct Cyc_Absyn_Exp*_tmp136=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp138);
return({struct Cyc_Absyn_Exp*_tmp6BE=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp136,0);Cyc_Relations_exp2relnop(_tmp6BE,rop);});}default: _LL11E: _LL11F:
 return 1;}_LL119:;}
# 917
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp139=rop;unsigned int _tmp13B;unsigned int _tmp13A;switch((_tmp139.RParamNumelts).tag){case 5: _LL121: _tmp13A=(_tmp139.RParam).val;_LL122:
# 920
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13A));case 6: _LL123: _tmp13B=(_tmp139.RParamNumelts).val;_LL124:
# 922
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13B));default: _LL125: _LL126:
 return Cyc_Relations_rop2string(rop);}_LL120:;}
# 927
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 930
union Cyc_CfFlowInfo_FlowInfo _tmp13C=inflow;struct Cyc_Dict_Dict _tmp146;struct Cyc_List_List*_tmp145;if((_tmp13C.BottomFL).tag == 1){_LL128: _LL129:
 return;}else{_LL12A: _tmp146=((_tmp13C.ReachableFL).val).f1;_tmp145=((_tmp13C.ReachableFL).val).f2;_LL12B:
# 933
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp13D=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp13D->rop1;
union Cyc_Relations_RelnOp rop2=_tmp13D->rop2;
enum Cyc_Relations_Relation _tmp13E=Cyc_Relations_flip_relation(_tmp13D->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,_tmp13E,rop1,_tmp145))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp13D->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp144;_tmp144.tag=0;({struct _dyneither_ptr _tmp6BF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp145));_tmp144.f1=_tmp6BF;});({struct Cyc_String_pa_PrintArg_struct _tmp143;_tmp143.tag=0;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp142;_tmp142.tag=0;_tmp142.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp141;_tmp141.tag=0;_tmp141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp13F[4]={& _tmp141,& _tmp142,& _tmp143,& _tmp144};({unsigned int _tmp6C1=loc;struct _dyneither_ptr _tmp6C0=({const char*_tmp140="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp140,sizeof(char),77);});Cyc_CfFlowInfo_aerr(_tmp6C1,_tmp6C0,_tag_dyneither(_tmp13F,sizeof(void*),4));});});});});});});
break;}}
# 948
goto _LL127;}_LL127:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 953
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 957
struct Cyc_CfFlowInfo_FlowEnv*_tmp147=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 970
{union Cyc_CfFlowInfo_FlowInfo _tmp148=inflow;struct Cyc_Dict_Dict _tmp14B;struct Cyc_List_List*_tmp14A;if((_tmp148.BottomFL).tag == 1){_LL12D: _LL12E:
 return({struct _tuple16 _tmp149;({union Cyc_CfFlowInfo_FlowInfo _tmp6C2=Cyc_CfFlowInfo_BottomFL();_tmp149.f1=_tmp6C2;});_tmp149.f2=_tmp147->unknown_all;_tmp149;});}else{_LL12F: _tmp14B=((_tmp148.ReachableFL).val).f1;_tmp14A=((_tmp148.ReachableFL).val).f2;_LL130:
 d=_tmp14B;relns=_tmp14A;}_LL12C:;}
# 985 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1006 "new_control_flow.cyc"
struct _tuple18 _tmp14C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp14D=_tmp14C;union Cyc_CfFlowInfo_FlowInfo _tmp15D;union Cyc_CfFlowInfo_AbsLVal _tmp15C;_LL132: _tmp15D=_tmp14D.f1;_tmp15C=_tmp14D.f2;_LL133:;{
struct _tuple18 _tmp14E=({struct _tuple18 _tmp15B;_tmp15B.f1=_tmp15D;_tmp15B.f2=_tmp15C;_tmp15B;});struct _tuple18 _tmp14F=_tmp14E;struct Cyc_Dict_Dict _tmp15A;struct Cyc_List_List*_tmp159;struct Cyc_CfFlowInfo_Place*_tmp158;if(((_tmp14F.f1).ReachableFL).tag == 2){if(((_tmp14F.f2).PlaceL).tag == 1){_LL135: _tmp15A=(((_tmp14F.f1).ReachableFL).val).f1;_tmp159=(((_tmp14F.f1).ReachableFL).val).f2;_tmp158=((_tmp14F.f2).PlaceL).val;_LL136: {
# 1009
void*_tmp150=Cyc_CfFlowInfo_lookup_place(_tmp15A,_tmp158);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp150,& needs_unconsume)){
({void*_tmp151=0;({unsigned int _tmp6C4=e->loc;struct _dyneither_ptr _tmp6C3=({const char*_tmp152="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp152,sizeof(char),56);});Cyc_CfFlowInfo_aerr(_tmp6C4,_tmp6C3,_tag_dyneither(_tmp151,sizeof(void*),0));});});
return({struct _tuple16 _tmp153;({union Cyc_CfFlowInfo_FlowInfo _tmp6C5=Cyc_CfFlowInfo_BottomFL();_tmp153.f1=_tmp6C5;});_tmp153.f2=_tmp147->unknown_all;_tmp153;});}else{
# 1015
if(needs_unconsume)
# 1017
return({struct _tuple16 _tmp154;_tmp154.f1=_tmp15D;({void*_tmp6C6=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp147,_tmp150);_tmp154.f2=_tmp6C6;});_tmp154;});else{
# 1020
void*_tmp155=Cyc_CfFlowInfo_make_unique_consumed(_tmp147,(void*)_check_null(e->topt),e,env->iteration_num,_tmp150);
struct Cyc_Dict_Dict _tmp156=Cyc_CfFlowInfo_assign_place(_tmp147,e->loc,_tmp15A,_tmp158,_tmp155);
# 1032
return({struct _tuple16 _tmp157;({union Cyc_CfFlowInfo_FlowInfo _tmp6C7=Cyc_CfFlowInfo_ReachableFL(_tmp156,_tmp159);_tmp157.f1=_tmp6C7;});_tmp157.f2=_tmp150;_tmp157;});}}}}else{goto _LL137;}}else{_LL137: _LL138:
# 1036
 goto _LL134;}_LL134:;};}{
# 1039
void*_tmp15E=e->r;void*_tmp15F=_tmp15E;struct Cyc_Absyn_Stmt*_tmp35D;struct Cyc_Absyn_Exp*_tmp35C;void*_tmp35B;int _tmp35A;struct Cyc_Absyn_Vardecl*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Exp*_tmp357;int _tmp356;struct Cyc_List_List*_tmp355;struct Cyc_List_List*_tmp354;enum Cyc_Absyn_AggrKind _tmp353;struct Cyc_List_List*_tmp352;struct Cyc_List_List*_tmp351;struct Cyc_List_List*_tmp350;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Absyn_Exp*_tmp348;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp346;struct _dyneither_ptr*_tmp345;struct Cyc_Absyn_Exp*_tmp344;struct _dyneither_ptr*_tmp343;struct Cyc_Absyn_Exp*_tmp342;struct _dyneither_ptr*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;int _tmp33A;struct Cyc_Absyn_Exp*_tmp339;void**_tmp338;struct Cyc_Absyn_Exp*_tmp337;int _tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Exp*_tmp331;struct Cyc_Absyn_Exp*_tmp330;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32C;enum Cyc_Absyn_Primop _tmp32B;struct Cyc_List_List*_tmp32A;struct Cyc_List_List*_tmp329;struct Cyc_Absyn_Datatypedecl*_tmp328;struct Cyc_Absyn_Vardecl*_tmp327;struct Cyc_Absyn_Vardecl*_tmp326;struct Cyc_Absyn_Vardecl*_tmp325;struct _dyneither_ptr _tmp324;struct Cyc_Absyn_Exp*_tmp323;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp320;switch(*((int*)_tmp15F)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15F)->f4 == Cyc_Absyn_Null_to_NonNull){_LL13A: _tmp320=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL13B: {
# 1043
struct _tuple16 _tmp160=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp320);struct _tuple16 _tmp161=_tmp160;union Cyc_CfFlowInfo_FlowInfo _tmp168;void*_tmp167;_LL1A5: _tmp168=_tmp161.f1;_tmp167=_tmp161.f2;_LL1A6:;{
struct _tuple16 _tmp162=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp168,_tmp320,_tmp167);struct _tuple16 _tmp163=_tmp162;union Cyc_CfFlowInfo_FlowInfo _tmp166;void*_tmp165;_LL1A8: _tmp166=_tmp163.f1;_tmp165=_tmp163.f2;_LL1A9:;
return({struct _tuple16 _tmp164;_tmp164.f1=_tmp166;_tmp164.f2=_tmp167;_tmp164;});};}}else{_LL13C: _tmp321=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL13D:
# 1049
 _tmp322=_tmp321;goto _LL13F;}case 12: _LL13E: _tmp322=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL13F:
 _tmp323=_tmp322;goto _LL141;case 13: _LL140: _tmp323=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL141:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp323);case 2: _LL142: _tmp324=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL143:
# 1054
 if(!({struct _dyneither_ptr _tmp6C8=(struct _dyneither_ptr)_tmp324;Cyc_NewControlFlow_strcmp(_tmp6C8,({const char*_tmp169="print_flow";_tag_dyneither(_tmp169,sizeof(char),11);}));})){
struct _dyneither_ptr _tmp16A=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp16D;_tmp16D.tag=0;_tmp16D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16A);({void*_tmp16B[1]={& _tmp16D};({struct Cyc___cycFILE*_tmp6CA=Cyc_stderr;struct _dyneither_ptr _tmp6C9=({const char*_tmp16C="%s: current flow is\n";_tag_dyneither(_tmp16C,sizeof(char),21);});Cyc_fprintf(_tmp6CA,_tmp6C9,_tag_dyneither(_tmp16B,sizeof(void*),1));});});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp16E=0;({struct Cyc___cycFILE*_tmp6CC=Cyc_stderr;struct _dyneither_ptr _tmp6CB=({const char*_tmp16F="\n";_tag_dyneither(_tmp16F,sizeof(char),2);});Cyc_fprintf(_tmp6CC,_tmp6CB,_tag_dyneither(_tmp16E,sizeof(void*),0));});});}else{
# 1060
if(!({struct _dyneither_ptr _tmp6CD=(struct _dyneither_ptr)_tmp324;Cyc_NewControlFlow_strcmp(_tmp6CD,({const char*_tmp170="debug_on";_tag_dyneither(_tmp170,sizeof(char),9);}));}));else{
# 1065
if(!({struct _dyneither_ptr _tmp6CE=(struct _dyneither_ptr)_tmp324;Cyc_NewControlFlow_strcmp(_tmp6CE,({const char*_tmp171="debug_off";_tag_dyneither(_tmp171,sizeof(char),10);}));}));else{
# 1071
({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp324);({void*_tmp172[1]={& _tmp174};({unsigned int _tmp6D0=e->loc;struct _dyneither_ptr _tmp6CF=({const char*_tmp173="unknown pragma %s";_tag_dyneither(_tmp173,sizeof(char),18);});Cyc_Warn_warn(_tmp6D0,_tmp6CF,_tag_dyneither(_tmp172,sizeof(void*),1));});});});}}}
return({struct _tuple16 _tmp175;_tmp175.f1=inflow;_tmp175.f2=_tmp147->zero;_tmp175;});case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).Int_c).tag){case 1: _LL144: _LL145:
# 1074
 goto _LL147;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).Int_c).val).f2 == 0){_LL146: _LL147:
 return({struct _tuple16 _tmp176;_tmp176.f1=inflow;_tmp176.f2=_tmp147->zero;_tmp176;});}else{_LL148: _LL149:
 goto _LL14B;}default: _LL14E: _LL14F:
# 1080
 goto _LL151;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)){case 2: _LL14A: _LL14B:
# 1077
 return({struct _tuple16 _tmp177;_tmp177.f1=inflow;_tmp177.f2=_tmp147->notzeroall;_tmp177;});case 1: _LL15A: _LL15B:
# 1088
 return({struct _tuple16 _tmp179;_tmp179.f1=inflow;({void*_tmp6D1=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp179.f2=_tmp6D1;});_tmp179;});case 3: _LL15C: _tmp325=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)->f1;_LL15D:
# 1091
 _tmp326=_tmp325;goto _LL15F;case 4: _LL15E: _tmp326=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)->f1;_LL15F:
 _tmp327=_tmp326;goto _LL161;case 5: _LL160: _tmp327=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)->f1;_LL161: {
# 1095
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp17C;_tmp17C.tag=0;_tmp17C.f1=_tmp327;_tmp17C;});
return({struct _tuple16 _tmp17A;_tmp17A.f1=inflow;({void*_tmp6D3=({struct Cyc_Dict_Dict _tmp6D2=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp6D2,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=vdroot;_tmp17B;}));});_tmp17A.f2=_tmp6D3;});_tmp17A;});}default: _LL19A: _LL19B:
# 1665
 goto _LL19D;}case 31: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15F)->f1 == 0){_LL14C: _LL14D:
# 1079
 goto _LL14F;}else{_LL188: _tmp329=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp328=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL189:
# 1528
 _tmp350=_tmp329;goto _LL18B;}case 18: _LL150: _LL151:
# 1081
 goto _LL153;case 17: _LL152: _LL153:
 goto _LL155;case 19: _LL154: _LL155:
 goto _LL157;case 33: _LL156: _LL157:
 goto _LL159;case 32: _LL158: _LL159:
 return({struct _tuple16 _tmp178;_tmp178.f1=inflow;_tmp178.f2=_tmp147->unknown_all;_tmp178;});case 3: _LL162: _tmp32B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp32A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL163: {
# 1102
struct _tuple16 _tmp17D=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp32A);struct _tuple16 _tmp17E=_tmp17D;union Cyc_CfFlowInfo_FlowInfo _tmp181;void*_tmp180;_LL1AB: _tmp181=_tmp17E.f1;_tmp180=_tmp17E.f2;_LL1AC:;
return({struct _tuple16 _tmp17F;_tmp17F.f1=_tmp181;_tmp17F.f2=_tmp180;_tmp17F;});}case 5: _LL164: _tmp32C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL165: {
# 1106
struct Cyc_List_List _tmp182=({struct Cyc_List_List _tmp190;_tmp190.hd=_tmp32C;_tmp190.tl=0;_tmp190;});
struct _tuple16 _tmp183=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp182);struct _tuple16 _tmp184=_tmp183;union Cyc_CfFlowInfo_FlowInfo _tmp18F;_LL1AE: _tmp18F=_tmp184.f1;_LL1AF:;{
struct _tuple18 _tmp185=Cyc_NewControlFlow_anal_Lexp(env,_tmp18F,0,0,_tmp32C);struct _tuple18 _tmp186=_tmp185;union Cyc_CfFlowInfo_AbsLVal _tmp18E;_LL1B1: _tmp18E=_tmp186.f2;_LL1B2:;
{struct _tuple25 _tmp187=({struct _tuple25 _tmp18C;_tmp18C.f1=_tmp18E;_tmp18C.f2=_tmp18F;_tmp18C;});struct _tuple25 _tmp188=_tmp187;struct Cyc_CfFlowInfo_Place*_tmp18B;struct Cyc_Dict_Dict _tmp18A;struct Cyc_List_List*_tmp189;if(((_tmp188.f1).PlaceL).tag == 1){if(((_tmp188.f2).ReachableFL).tag == 2){_LL1B4: _tmp18B=((_tmp188.f1).PlaceL).val;_tmp18A=(((_tmp188.f2).ReachableFL).val).f1;_tmp189=(((_tmp188.f2).ReachableFL).val).f2;_LL1B5:
# 1111
({struct Cyc_List_List*_tmp6D4=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp189,_tmp32C);_tmp189=_tmp6D4;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6D6=({struct Cyc_Dict_Dict _tmp6D5=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp32C->loc,_tmp18A,_tmp18B,_tmp147->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6D5,_tmp189);});_tmp18F=_tmp6D6;});
# 1116
goto _LL1B3;}else{goto _LL1B6;}}else{_LL1B6: _LL1B7:
 goto _LL1B3;}_LL1B3:;}
# 1119
return({struct _tuple16 _tmp18D;_tmp18D.f1=_tmp18F;_tmp18D.f2=_tmp147->unknown_all;_tmp18D;});};}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f2 != 0){_LL166: _tmp32E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp32D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL167:
# 1122
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp191=0;({unsigned int _tmp6D8=e->loc;struct _dyneither_ptr _tmp6D7=({const char*_tmp192="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp192,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp6D8,_tmp6D7,_tag_dyneither(_tmp191,sizeof(void*),0));});});
return({struct _tuple16 _tmp193;({union Cyc_CfFlowInfo_FlowInfo _tmp6D9=Cyc_CfFlowInfo_BottomFL();_tmp193.f1=_tmp6D9;});_tmp193.f2=_tmp147->unknown_all;_tmp193;});}{
# 1126
struct _tuple16 _tmp194=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp32D);struct _tuple16 _tmp195=_tmp194;union Cyc_CfFlowInfo_FlowInfo _tmp1A1;void*_tmp1A0;_LL1B9: _tmp1A1=_tmp195.f1;_tmp1A0=_tmp195.f2;_LL1BA:;{
struct _tuple18 _tmp196=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A1,0,0,_tmp32E);struct _tuple18 _tmp197=_tmp196;union Cyc_CfFlowInfo_FlowInfo _tmp19F;union Cyc_CfFlowInfo_AbsLVal _tmp19E;_LL1BC: _tmp19F=_tmp197.f1;_tmp19E=_tmp197.f2;_LL1BD:;
{union Cyc_CfFlowInfo_FlowInfo _tmp198=_tmp19F;struct Cyc_Dict_Dict _tmp19C;struct Cyc_List_List*_tmp19B;if((_tmp198.ReachableFL).tag == 2){_LL1BF: _tmp19C=((_tmp198.ReachableFL).val).f1;_tmp19B=((_tmp198.ReachableFL).val).f2;_LL1C0:
# 1130
{union Cyc_CfFlowInfo_AbsLVal _tmp199=_tmp19E;struct Cyc_CfFlowInfo_Place*_tmp19A;if((_tmp199.PlaceL).tag == 1){_LL1C4: _tmp19A=(_tmp199.PlaceL).val;_LL1C5:
# 1134
({struct Cyc_List_List*_tmp6DA=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp19B,_tmp32E);_tmp19B=_tmp6DA;});
({struct Cyc_Dict_Dict _tmp6DB=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp32E->loc,_tmp19C,_tmp19A,_tmp147->unknown_all);_tmp19C=_tmp6DB;});
# 1137
({union Cyc_CfFlowInfo_FlowInfo _tmp6DC=Cyc_CfFlowInfo_ReachableFL(_tmp19C,_tmp19B);_tmp19F=_tmp6DC;});
# 1141
goto _LL1C3;}else{_LL1C6: _LL1C7:
# 1144
 goto _LL1C3;}_LL1C3:;}
# 1146
goto _LL1BE;}else{_LL1C1: _LL1C2:
 goto _LL1BE;}_LL1BE:;}
# 1149
return({struct _tuple16 _tmp19D;_tmp19D.f1=_tmp19F;_tmp19D.f2=_tmp147->unknown_all;_tmp19D;});};};}else{_LL168: _tmp330=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp32F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL169:
# 1152
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp1A2=0;({unsigned int _tmp6DE=e->loc;struct _dyneither_ptr _tmp6DD=({const char*_tmp1A3="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1A3,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp6DE,_tmp6DD,_tag_dyneither(_tmp1A2,sizeof(void*),0));});});
return({struct _tuple16 _tmp1A4;({union Cyc_CfFlowInfo_FlowInfo _tmp6DF=Cyc_CfFlowInfo_BottomFL();_tmp1A4.f1=_tmp6DF;});_tmp1A4.f2=_tmp147->unknown_all;_tmp1A4;});}
# 1158
({union Cyc_CfFlowInfo_FlowInfo _tmp6E2=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E1=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E0=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6E1,_tmp6E0,({struct Cyc_Absyn_Exp*_tmp1A5[2];_tmp1A5[1]=_tmp32F;_tmp1A5[0]=_tmp330;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1A5,sizeof(struct Cyc_Absyn_Exp*),2));}));});inflow=_tmp6E2;});{
# 1160
struct _tuple16 _tmp1A6=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp32F);struct _tuple16 _tmp1A7=_tmp1A6;union Cyc_CfFlowInfo_FlowInfo _tmp1B2;void*_tmp1B1;_LL1C9: _tmp1B2=_tmp1A7.f1;_tmp1B1=_tmp1A7.f2;_LL1CA:;{
struct _tuple18 _tmp1A8=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B2,0,0,_tmp330);struct _tuple18 _tmp1A9=_tmp1A8;union Cyc_CfFlowInfo_FlowInfo _tmp1B0;union Cyc_CfFlowInfo_AbsLVal _tmp1AF;_LL1CC: _tmp1B0=_tmp1A9.f1;_tmp1AF=_tmp1A9.f2;_LL1CD:;{
struct _tuple16 _tmp1AA=Cyc_NewControlFlow_do_assign(_tmp147,env,_tmp1B0,_tmp330,_tmp1AF,_tmp32F,_tmp1B1,e->loc);struct _tuple16 _tmp1AB=_tmp1AA;union Cyc_CfFlowInfo_FlowInfo _tmp1AE;void*_tmp1AD;_LL1CF: _tmp1AE=_tmp1AB.f1;_tmp1AD=_tmp1AB.f2;_LL1D0:;
# 1166
return({struct _tuple16 _tmp1AC;_tmp1AC.f1=_tmp1AE;_tmp1AC.f2=_tmp1AD;_tmp1AC;});};};};}case 9: _LL16A: _tmp332=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp331=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL16B: {
# 1169
struct _tuple16 _tmp1B3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp332);struct _tuple16 _tmp1B4=_tmp1B3;union Cyc_CfFlowInfo_FlowInfo _tmp1B6;void*_tmp1B5;_LL1D2: _tmp1B6=_tmp1B4.f1;_tmp1B5=_tmp1B4.f2;_LL1D3:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1B6,_tmp331);}case 11: _LL16C: _tmp333=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL16D: {
# 1173
struct _tuple16 _tmp1B7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp333);struct _tuple16 _tmp1B8=_tmp1B7;union Cyc_CfFlowInfo_FlowInfo _tmp1BB;void*_tmp1BA;_LL1D5: _tmp1BB=_tmp1B8.f1;_tmp1BA=_tmp1B8.f2;_LL1D6:;
Cyc_NewControlFlow_use_Rval(env,_tmp333->loc,_tmp1BB,_tmp1BA);
return({struct _tuple16 _tmp1B9;({union Cyc_CfFlowInfo_FlowInfo _tmp6E3=Cyc_CfFlowInfo_BottomFL();_tmp1B9.f1=_tmp6E3;});({void*_tmp6E4=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp1B9.f2=_tmp6E4;});_tmp1B9;});}case 10: _LL16E: _tmp335=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp334=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL16F: {
# 1178
struct Cyc_List_List*_tmp1BC=_tmp334;
struct _tuple23 _tmp1BD=({
struct Cyc_NewControlFlow_AnalEnv*_tmp6E7=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E6=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E7,_tmp6E6,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=_tmp335;({struct Cyc_List_List*_tmp6E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp334);_tmp1ED->tl=_tmp6E5;});_tmp1ED;}),0,1);});
# 1179
struct _tuple23 _tmp1BE=_tmp1BD;union Cyc_CfFlowInfo_FlowInfo _tmp1EC;struct Cyc_List_List*_tmp1EB;_LL1D8: _tmp1EC=_tmp1BE.f1;_tmp1EB=_tmp1BE.f2;_LL1D9:;{
# 1182
union Cyc_CfFlowInfo_FlowInfo _tmp1BF=Cyc_NewControlFlow_use_Rval(env,_tmp335->loc,_tmp1EC,(void*)((struct Cyc_List_List*)_check_null(_tmp1EB))->hd);
_tmp1EB=_tmp1EB->tl;{
# 1185
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1C0=Cyc_Tcutil_compress((void*)_check_null(_tmp335->topt));void*_tmp1C1=_tmp1C0;void*_tmp1D3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C1)->tag == 5){_LL1DB: _tmp1D3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C1)->f1).elt_typ;_LL1DC:
# 1192
{void*_tmp1C2=Cyc_Tcutil_compress(_tmp1D3);void*_tmp1C3=_tmp1C2;struct Cyc_List_List*_tmp1D0;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*_tmp1CE;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C3)->tag == 9){_LL1E0: _tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C3)->f1).attributes;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C3)->f1).requires_relns;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C3)->f1).ensures_relns;_LL1E1:
# 1194
 requires=_tmp1CF;
ensures=_tmp1CE;
for(0;_tmp1D0 != 0;_tmp1D0=_tmp1D0->tl){
# 1198
void*_tmp1C4=(void*)_tmp1D0->hd;void*_tmp1C5=_tmp1C4;int _tmp1CB;int _tmp1CA;int _tmp1C9;switch(*((int*)_tmp1C5)){case 20: _LL1E5: _tmp1C9=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1C5)->f1;_LL1E6:
# 1200
({struct Cyc_List_List*_tmp6E8=({struct Cyc_List_List*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->hd=(void*)_tmp1C9;_tmp1C6->tl=init_params;_tmp1C6;});init_params=_tmp6E8;});goto _LL1E4;case 21: _LL1E7: _tmp1CA=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1C5)->f1;_LL1E8:
# 1202
({struct Cyc_List_List*_tmp6E9=({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=(void*)_tmp1CA;_tmp1C7->tl=nolive_unique_params;_tmp1C7;});nolive_unique_params=_tmp6E9;});
goto _LL1E4;case 22: _LL1E9: _tmp1CB=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1C5)->f1;_LL1EA:
# 1206
({struct Cyc_List_List*_tmp6EA=({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->hd=(void*)_tmp1CB;_tmp1C8->tl=noconsume_params;_tmp1C8;});noconsume_params=_tmp6EA;});
goto _LL1E4;default: _LL1EB: _LL1EC:
 goto _LL1E4;}_LL1E4:;}
# 1210
goto _LL1DF;}else{_LL1E2: _LL1E3:
({void*_tmp1CC=0;({struct _dyneither_ptr _tmp6EB=({const char*_tmp1CD="anal_Rexp: bad function type";_tag_dyneither(_tmp1CD,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6EB,_tag_dyneither(_tmp1CC,sizeof(void*),0));});});}_LL1DF:;}
# 1213
goto _LL1DA;}else{_LL1DD: _LL1DE:
({void*_tmp1D1=0;({struct _dyneither_ptr _tmp6EC=({const char*_tmp1D2="anal_Rexp: bad function type";_tag_dyneither(_tmp1D2,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6EC,_tag_dyneither(_tmp1D1,sizeof(void*),0));});});}_LL1DA:;}
# 1216
{int i=1;for(0;_tmp1EB != 0;(((_tmp1EB=_tmp1EB->tl,_tmp334=((struct Cyc_List_List*)_check_null(_tmp334))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1D4=_tmp1EC;struct Cyc_Dict_Dict _tmp1E8;if((_tmp1D4.BottomFL).tag == 1){_LL1EE: _LL1EF:
 goto _LL1ED;}else{_LL1F0: _tmp1E8=((_tmp1D4.ReachableFL).val).f1;_LL1F1: {
# 1221
struct _tuple15 _tmp1D5=Cyc_CfFlowInfo_unname_rval((void*)_tmp1EB->hd);struct _tuple15 _tmp1D6=_tmp1D5;void*_tmp1E7;_LL1F3: _tmp1E7=_tmp1D6.f1;_LL1F4:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1E8,(void*)_tmp1EB->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1EB->hd))
({void*_tmp1D7=0;({unsigned int _tmp6EE=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp334))->hd)->loc;struct _dyneither_ptr _tmp6ED=({const char*_tmp1D8="expression may not be initialized";_tag_dyneither(_tmp1D8,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp6EE,_tmp6ED,_tag_dyneither(_tmp1D7,sizeof(void*),0));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1D9=_tmp1BF;struct Cyc_Dict_Dict _tmp1E6;struct Cyc_List_List*_tmp1E5;if((_tmp1D9.BottomFL).tag == 1){_LL1F6: _LL1F7:
 goto _LL1F5;}else{_LL1F8: _tmp1E6=((_tmp1D9.ReachableFL).val).f1;_tmp1E5=((_tmp1D9.ReachableFL).val).f2;_LL1F9: {
# 1229
struct Cyc_Dict_Dict _tmp1DA=Cyc_CfFlowInfo_escape_deref(_tmp147,_tmp1E6,(void*)_tmp1EB->hd);
{void*_tmp1DB=(void*)_tmp1EB->hd;void*_tmp1DC=_tmp1DB;struct Cyc_CfFlowInfo_Place*_tmp1E4;switch(*((int*)_tmp1DC)){case 4: _LL1FB: _tmp1E4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DC)->f1;_LL1FC:
# 1232
{void*_tmp1DD=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp334))->hd)->topt));void*_tmp1DE=_tmp1DD;void*_tmp1E1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->tag == 5){_LL202: _tmp1E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).elt_typ;_LL203:
# 1234
({struct Cyc_Dict_Dict _tmp6F3=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6F2=_tmp147;unsigned int _tmp6F1=((struct Cyc_Absyn_Exp*)_tmp334->hd)->loc;struct Cyc_Dict_Dict _tmp6F0=_tmp1DA;struct Cyc_CfFlowInfo_Place*_tmp6EF=_tmp1E4;Cyc_CfFlowInfo_assign_place(_tmp6F2,_tmp6F1,_tmp6F0,_tmp6EF,
Cyc_CfFlowInfo_typ_to_absrval(_tmp147,_tmp1E1,0,_tmp147->esc_all));});
# 1234
_tmp1DA=_tmp6F3;});
# 1238
goto _LL201;}else{_LL204: _LL205:
({void*_tmp1DF=0;({struct _dyneither_ptr _tmp6F4=({const char*_tmp1E0="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1E0,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F4,_tag_dyneither(_tmp1DF,sizeof(void*),0));});});}_LL201:;}
# 1241
goto _LL1FA;case 5: _LL1FD: _LL1FE:
# 1243
({void*_tmp1E2=0;({struct _dyneither_ptr _tmp6F5=({const char*_tmp1E3="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1E3,sizeof(char),68);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F5,_tag_dyneither(_tmp1E2,sizeof(void*),0));});});default: _LL1FF: _LL200:
# 1245
 goto _LL1FA;}_LL1FA:;}
# 1247
({union Cyc_CfFlowInfo_FlowInfo _tmp6F6=Cyc_CfFlowInfo_ReachableFL(_tmp1DA,_tmp1E5);_tmp1BF=_tmp6F6;});
goto _LL1F5;}}_LL1F5:;}
# 1250
goto _LL1ED;}}_LL1ED:;}else{
# 1253
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1258
({union Cyc_CfFlowInfo_FlowInfo _tmp6FB=({struct Cyc_NewControlFlow_AnalEnv*_tmp6FA=env;unsigned int _tmp6F9=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp334))->hd)->loc;void*_tmp6F8=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp334->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6F7=_tmp1BF;Cyc_NewControlFlow_use_nounique_Rval(_tmp6FA,_tmp6F9,_tmp6F8,_tmp6F7,(void*)_tmp1EB->hd);});_tmp1BF=_tmp6FB;});else{
# 1268
({union Cyc_CfFlowInfo_FlowInfo _tmp6FC=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp334))->hd)->loc,_tmp1BF,(void*)_tmp1EB->hd);_tmp1BF=_tmp6FC;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp334->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp6FD=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1BF,(struct Cyc_Absyn_Exp*)_tmp334->hd,((struct Cyc_Absyn_Exp*)_tmp334->hd)->loc,(void*)_tmp1EB->hd);_tmp1BF=_tmp6FD;});}}}}
# 1275
Cyc_NewControlFlow_check_fn_requires(env,_tmp1BF,_tmp1BC,requires,e->loc);
# 1278
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp335->topt)))
return({struct _tuple16 _tmp1E9;({union Cyc_CfFlowInfo_FlowInfo _tmp6FE=Cyc_CfFlowInfo_BottomFL();_tmp1E9.f1=_tmp6FE;});({void*_tmp6FF=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp1E9.f2=_tmp6FF;});_tmp1E9;});else{
# 1281
return({struct _tuple16 _tmp1EA;_tmp1EA.f1=_tmp1BF;({void*_tmp700=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp1EA.f2=_tmp700;});_tmp1EA;});}};};}case 34: _LL170: _tmp33A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).is_calloc;_tmp339=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).rgn;_tmp338=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).elt_type;_tmp337=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).num_elts;_tmp336=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).fat_result;_LL171: {
# 1284
void*place_val;
if(_tmp336)place_val=_tmp147->notzeroall;else{
if(_tmp33A)({void*_tmp701=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,*((void**)_check_null(_tmp338)),0,_tmp147->zero);place_val=_tmp701;});else{
({void*_tmp702=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,*((void**)_check_null(_tmp338)),0,_tmp147->unknown_none);place_val=_tmp702;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp339 != 0){
struct _tuple23 _tmp1EE=({struct Cyc_NewControlFlow_AnalEnv*_tmp704=env;union Cyc_CfFlowInfo_FlowInfo _tmp703=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp704,_tmp703,({struct Cyc_Absyn_Exp*_tmp1F2[2];_tmp1F2[1]=_tmp337;_tmp1F2[0]=_tmp339;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1F2,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple23 _tmp1EF=_tmp1EE;union Cyc_CfFlowInfo_FlowInfo _tmp1F1;struct Cyc_List_List*_tmp1F0;_LL207: _tmp1F1=_tmp1EF.f1;_tmp1F0=_tmp1EF.f2;_LL208:;
outflow=_tmp1F1;}else{
# 1293
struct _tuple16 _tmp1F3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp337);struct _tuple16 _tmp1F4=_tmp1F3;union Cyc_CfFlowInfo_FlowInfo _tmp1F5;_LL20A: _tmp1F5=_tmp1F4.f1;_LL20B:;
outflow=_tmp1F5;}
# 1298
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp1F6;_tmp1F6.f1=outflow;({void*_tmp706=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp705=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp1F8;_tmp1F8.tag=5;_tmp1F8.f1=place_val;_tmp1F8;});_tmp1F7[0]=_tmp705;});_tmp1F7;});_tmp1F6.f2=_tmp706;});_tmp1F6;});else{
# 1302
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp201=_cycalloc(sizeof(*_tmp201));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp707=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp202;_tmp202.tag=1;_tmp202.f1=_tmp337;_tmp202.f2=(void*)_check_null(e->topt);_tmp202;});_tmp201[0]=_tmp707;});_tmp201;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->root=root;_tmp200->path=0;_tmp200;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp708=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp1FF;_tmp1FF.tag=4;_tmp1FF.f1=place;_tmp1FF;});_tmp1FE[0]=_tmp708;});_tmp1FE;});
union Cyc_CfFlowInfo_FlowInfo _tmp1F9=outflow;struct Cyc_Dict_Dict _tmp1FD;struct Cyc_List_List*_tmp1FC;if((_tmp1F9.BottomFL).tag == 1){_LL20D: _LL20E:
 return({struct _tuple16 _tmp1FA;_tmp1FA.f1=outflow;_tmp1FA.f2=rval;_tmp1FA;});}else{_LL20F: _tmp1FD=((_tmp1F9.ReachableFL).val).f1;_tmp1FC=((_tmp1F9.ReachableFL).val).f2;_LL210:
# 1308
 return({struct _tuple16 _tmp1FB;({union Cyc_CfFlowInfo_FlowInfo _tmp70A=({struct Cyc_Dict_Dict _tmp709=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1FD,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp709,_tmp1FC);});_tmp1FB.f1=_tmp70A;});_tmp1FB.f2=rval;_tmp1FB;});}_LL20C:;}};}case 35: _LL172: _tmp33C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp33B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL173: {
# 1313
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1319
struct _tuple23 _tmp203=({struct Cyc_NewControlFlow_AnalEnv*_tmp70C=env;union Cyc_CfFlowInfo_FlowInfo _tmp70B=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp70C,_tmp70B,({struct Cyc_Absyn_Exp*_tmp219[2];_tmp219[1]=_tmp33B;_tmp219[0]=_tmp33C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp219,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);});struct _tuple23 _tmp204=_tmp203;union Cyc_CfFlowInfo_FlowInfo _tmp218;struct Cyc_List_List*_tmp217;_LL212: _tmp218=_tmp204.f1;_tmp217=_tmp204.f2;_LL213:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp217))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp217->tl))->hd;
outflow=_tmp218;{
# 1324
struct _tuple18 _tmp205=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp33C);struct _tuple18 _tmp206=_tmp205;union Cyc_CfFlowInfo_AbsLVal _tmp216;_LL215: _tmp216=_tmp206.f2;_LL216:;{
struct _tuple18 _tmp207=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp33B);struct _tuple18 _tmp208=_tmp207;union Cyc_CfFlowInfo_AbsLVal _tmp215;_LL218: _tmp215=_tmp208.f2;_LL219:;
{union Cyc_CfFlowInfo_FlowInfo _tmp209=outflow;struct Cyc_Dict_Dict _tmp213;struct Cyc_List_List*_tmp212;if((_tmp209.ReachableFL).tag == 2){_LL21B: _tmp213=((_tmp209.ReachableFL).val).f1;_tmp212=((_tmp209.ReachableFL).val).f2;_LL21C:
# 1328
{union Cyc_CfFlowInfo_AbsLVal _tmp20A=_tmp216;struct Cyc_CfFlowInfo_Place*_tmp20D;if((_tmp20A.PlaceL).tag == 1){_LL220: _tmp20D=(_tmp20A.PlaceL).val;_LL221:
# 1330
({struct Cyc_Dict_Dict _tmp70D=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp33C->loc,_tmp213,_tmp20D,right_rval);_tmp213=_tmp70D;});
goto _LL21F;}else{_LL222: _LL223:
# 1336
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp213,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp20B=0;({unsigned int _tmp70F=_tmp33B->loc;struct _dyneither_ptr _tmp70E=({const char*_tmp20C="expression may not be fully initialized";_tag_dyneither(_tmp20C,sizeof(char),40);});Cyc_CfFlowInfo_aerr(_tmp70F,_tmp70E,_tag_dyneither(_tmp20B,sizeof(void*),0));});});
goto _LL21F;}_LL21F:;}
# 1340
{union Cyc_CfFlowInfo_AbsLVal _tmp20E=_tmp215;struct Cyc_CfFlowInfo_Place*_tmp211;if((_tmp20E.PlaceL).tag == 1){_LL225: _tmp211=(_tmp20E.PlaceL).val;_LL226:
# 1342
({struct Cyc_Dict_Dict _tmp710=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp33B->loc,_tmp213,_tmp211,left_rval);_tmp213=_tmp710;});
goto _LL224;}else{_LL227: _LL228:
# 1345
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp213,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp20F=0;({unsigned int _tmp712=_tmp33C->loc;struct _dyneither_ptr _tmp711=({const char*_tmp210="expression may not be fully initialized";_tag_dyneither(_tmp210,sizeof(char),40);});Cyc_CfFlowInfo_aerr(_tmp712,_tmp711,_tag_dyneither(_tmp20F,sizeof(void*),0));});});
goto _LL224;}_LL224:;}
# 1350
({struct Cyc_List_List*_tmp713=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp212,_tmp33C);_tmp212=_tmp713;});
({struct Cyc_List_List*_tmp714=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp212,_tmp33B);_tmp212=_tmp714;});
# 1353
({union Cyc_CfFlowInfo_FlowInfo _tmp715=Cyc_CfFlowInfo_ReachableFL(_tmp213,_tmp212);outflow=_tmp715;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL21A;}else{_LL21D: _LL21E:
 goto _LL21A;}_LL21A:;}
# 1360
return({struct _tuple16 _tmp214;_tmp214.f1=outflow;_tmp214.f2=_tmp147->unknown_all;_tmp214;});};};}case 16: _LL174: _tmp33E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp33D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL175: {
# 1363
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp33E != 0){
struct _tuple23 _tmp21A=({struct Cyc_NewControlFlow_AnalEnv*_tmp717=env;union Cyc_CfFlowInfo_FlowInfo _tmp716=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp717,_tmp716,({struct Cyc_Absyn_Exp*_tmp21E[2];_tmp21E[1]=_tmp33D;_tmp21E[0]=_tmp33E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple23 _tmp21B=_tmp21A;union Cyc_CfFlowInfo_FlowInfo _tmp21D;struct Cyc_List_List*_tmp21C;_LL22A: _tmp21D=_tmp21B.f1;_tmp21C=_tmp21B.f2;_LL22B:;
outflow=_tmp21D;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp21C))->tl))->hd;}else{
# 1370
struct _tuple16 _tmp21F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp33D);struct _tuple16 _tmp220=_tmp21F;union Cyc_CfFlowInfo_FlowInfo _tmp222;void*_tmp221;_LL22D: _tmp222=_tmp220.f1;_tmp221=_tmp220.f2;_LL22E:;
outflow=_tmp222;
place_val=_tmp221;}
# 1375
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp223;_tmp223.f1=outflow;({void*_tmp719=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp224=_cycalloc(sizeof(*_tmp224));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp718=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp225;_tmp225.tag=5;_tmp225.f1=place_val;_tmp225;});_tmp224[0]=_tmp718;});_tmp224;});_tmp223.f2=_tmp719;});_tmp223;});else{
# 1379
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp22E=_cycalloc(sizeof(*_tmp22E));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp71A=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp22F;_tmp22F.tag=1;_tmp22F.f1=_tmp33D;_tmp22F.f2=(void*)_check_null(e->topt);_tmp22F;});_tmp22E[0]=_tmp71A;});_tmp22E;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->root=root;_tmp22D->path=0;_tmp22D;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp71B=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp22C;_tmp22C.tag=4;_tmp22C.f1=place;_tmp22C;});_tmp22B[0]=_tmp71B;});_tmp22B;});
union Cyc_CfFlowInfo_FlowInfo _tmp226=outflow;struct Cyc_Dict_Dict _tmp22A;struct Cyc_List_List*_tmp229;if((_tmp226.BottomFL).tag == 1){_LL230: _LL231:
 return({struct _tuple16 _tmp227;_tmp227.f1=outflow;_tmp227.f2=rval;_tmp227;});}else{_LL232: _tmp22A=((_tmp226.ReachableFL).val).f1;_tmp229=((_tmp226.ReachableFL).val).f2;_LL233:
# 1385
 return({struct _tuple16 _tmp228;({union Cyc_CfFlowInfo_FlowInfo _tmp71D=({struct Cyc_Dict_Dict _tmp71C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp22A,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp71C,_tmp229);});_tmp228.f1=_tmp71D;});_tmp228.f2=rval;_tmp228;});}_LL22F:;}}case 15: _LL176: _tmp33F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL177: {
# 1390
struct _tuple18 _tmp230=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp33F);struct _tuple18 _tmp231=_tmp230;union Cyc_CfFlowInfo_FlowInfo _tmp239;union Cyc_CfFlowInfo_AbsLVal _tmp238;_LL235: _tmp239=_tmp231.f1;_tmp238=_tmp231.f2;_LL236:;{
union Cyc_CfFlowInfo_AbsLVal _tmp232=_tmp238;struct Cyc_CfFlowInfo_Place*_tmp237;if((_tmp232.UnknownL).tag == 2){_LL238: _LL239:
 return({struct _tuple16 _tmp233;_tmp233.f1=_tmp239;_tmp233.f2=_tmp147->notzeroall;_tmp233;});}else{_LL23A: _tmp237=(_tmp232.PlaceL).val;_LL23B:
 return({struct _tuple16 _tmp234;_tmp234.f1=_tmp239;({void*_tmp71F=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp235=_cycalloc(sizeof(*_tmp235));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp71E=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp236;_tmp236.tag=4;_tmp236.f1=_tmp237;_tmp236;});_tmp235[0]=_tmp71E;});_tmp235;});_tmp234.f2=_tmp71F;});_tmp234;});}_LL237:;};}case 20: _LL178: _tmp340=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL179: {
# 1397
struct _tuple16 _tmp23A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp340);struct _tuple16 _tmp23B=_tmp23A;union Cyc_CfFlowInfo_FlowInfo _tmp23D;void*_tmp23C;_LL23D: _tmp23D=_tmp23B.f1;_tmp23C=_tmp23B.f2;_LL23E:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp23D,_tmp340,_tmp23C);}case 21: _LL17A: _tmp342=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp341=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL17B: {
# 1401
struct _tuple16 _tmp23E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp342);struct _tuple16 _tmp23F=_tmp23E;union Cyc_CfFlowInfo_FlowInfo _tmp252;void*_tmp251;_LL240: _tmp252=_tmp23F.f1;_tmp251=_tmp23F.f2;_LL241:;{
void*_tmp240=(void*)_check_null(_tmp342->topt);
# 1404
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp240))
return({struct _tuple16 _tmp241;_tmp241.f1=_tmp252;({void*_tmp720=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp241.f2=_tmp720;});_tmp241;});
if(Cyc_Absyn_is_require_union_type(_tmp240))
Cyc_NewControlFlow_check_union_requires(_tmp342->loc,_tmp240,_tmp341,_tmp252);{
struct _tuple15 _tmp242=Cyc_CfFlowInfo_unname_rval(_tmp251);struct _tuple15 _tmp243=_tmp242;void*_tmp250;_LL243: _tmp250=_tmp243.f1;_LL244:;{
void*_tmp244=_tmp250;int _tmp24F;int _tmp24E;struct _dyneither_ptr _tmp24D;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->tag == 6){_LL246: _tmp24F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1).is_union;_tmp24E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->f1).fieldnum;_tmp24D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp244)->f2;_LL247: {
# 1411
int _tmp245=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp342->topt),_tmp341);
if((_tmp24F  && _tmp24E != - 1) && _tmp24E != _tmp245)
return({struct _tuple16 _tmp246;_tmp246.f1=_tmp252;({void*_tmp721=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),1,_tmp147->unknown_none);_tmp246.f2=_tmp721;});_tmp246;});
return({struct _tuple16 _tmp247;_tmp247.f1=_tmp252;_tmp247.f2=*((void**)_check_dyneither_subscript(_tmp24D,sizeof(void*),_tmp245));_tmp247;});}}else{_LL248: _LL249:
# 1416
({void*_tmp248=0;({struct Cyc___cycFILE*_tmp723=Cyc_stderr;struct _dyneither_ptr _tmp722=({const char*_tmp249="the bad rexp is :";_tag_dyneither(_tmp249,sizeof(char),18);});Cyc_fprintf(_tmp723,_tmp722,_tag_dyneither(_tmp248,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(_tmp250);
({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0;({struct _dyneither_ptr _tmp724=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp24C.f1=_tmp724;});({void*_tmp24A[1]={& _tmp24C};({struct _dyneither_ptr _tmp725=({const char*_tmp24B="\nanal_Rexp: AggrMember: %s";_tag_dyneither(_tmp24B,sizeof(char),27);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp725,_tag_dyneither(_tmp24A,sizeof(void*),1));});});});}_LL245:;};};};}case 38: _LL17C: _tmp344=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp343=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL17D: {
# 1424
struct _tuple16 _tmp253=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp344);struct _tuple16 _tmp254=_tmp253;union Cyc_CfFlowInfo_FlowInfo _tmp265;void*_tmp264;_LL24B: _tmp265=_tmp254.f1;_tmp264=_tmp254.f2;_LL24C:;
# 1426
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp344->topt)))
return({struct _tuple16 _tmp255;_tmp255.f1=_tmp265;_tmp255.f2=_tmp147->unknown_all;_tmp255;});{
struct _tuple15 _tmp256=Cyc_CfFlowInfo_unname_rval(_tmp264);struct _tuple15 _tmp257=_tmp256;void*_tmp263;_LL24E: _tmp263=_tmp257.f1;_LL24F:;{
void*_tmp258=_tmp263;int _tmp262;int _tmp261;struct _dyneither_ptr _tmp260;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp258)->tag == 6){_LL251: _tmp262=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp258)->f1).is_union;_tmp261=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp258)->f1).fieldnum;_tmp260=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp258)->f2;_LL252: {
# 1431
int _tmp259=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp344->topt),_tmp343);
if(_tmp262  && _tmp261 != - 1){
if(_tmp261 != _tmp259)
return({struct _tuple16 _tmp25A;_tmp25A.f1=_tmp265;_tmp25A.f2=_tmp147->zero;_tmp25A;});else{
# 1436
return({struct _tuple16 _tmp25B;_tmp25B.f1=_tmp265;_tmp25B.f2=_tmp147->notzeroall;_tmp25B;});}}else{
# 1438
return({struct _tuple16 _tmp25C;_tmp25C.f1=_tmp265;_tmp25C.f2=_tmp147->unknown_all;_tmp25C;});}}}else{_LL253: _LL254:
# 1440
({struct Cyc_String_pa_PrintArg_struct _tmp25F;_tmp25F.tag=0;({struct _dyneither_ptr _tmp726=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp25F.f1=_tmp726;});({void*_tmp25D[1]={& _tmp25F};({struct _dyneither_ptr _tmp727=({const char*_tmp25E="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp25E,sizeof(char),26);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp727,_tag_dyneither(_tmp25D,sizeof(void*),1));});});});}_LL250:;};};}case 22: _LL17E: _tmp346=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp345=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL17F: {
# 1444
struct _tuple16 _tmp266=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp346);struct _tuple16 _tmp267=_tmp266;union Cyc_CfFlowInfo_FlowInfo _tmp27F;void*_tmp27E;_LL256: _tmp27F=_tmp267.f1;_tmp27E=_tmp267.f2;_LL257:;{
# 1447
struct _tuple16 _tmp268=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp27F,_tmp346,_tmp27E);struct _tuple16 _tmp269=_tmp268;union Cyc_CfFlowInfo_FlowInfo _tmp27D;void*_tmp27C;_LL259: _tmp27D=_tmp269.f1;_tmp27C=_tmp269.f2;_LL25A:;{
# 1450
void*_tmp26A=Cyc_Tcutil_compress((void*)_check_null(_tmp346->topt));void*_tmp26B=_tmp26A;void*_tmp27B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B)->tag == 5){_LL25C: _tmp27B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B)->f1).elt_typ;_LL25D:
# 1452
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp27B))
# 1454
return({struct _tuple16 _tmp26C;_tmp26C.f1=_tmp27D;({void*_tmp728=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp26C.f2=_tmp728;});_tmp26C;});
# 1456
if(Cyc_Absyn_is_require_union_type(_tmp27B))
Cyc_NewControlFlow_check_union_requires(_tmp346->loc,_tmp27B,_tmp345,_tmp27F);{
# 1459
struct _tuple15 _tmp26D=Cyc_CfFlowInfo_unname_rval(_tmp27C);struct _tuple15 _tmp26E=_tmp26D;void*_tmp278;_LL261: _tmp278=_tmp26E.f1;_LL262:;{
void*_tmp26F=_tmp278;int _tmp277;int _tmp276;struct _dyneither_ptr _tmp275;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->tag == 6){_LL264: _tmp277=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->f1).is_union;_tmp276=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->f1).fieldnum;_tmp275=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->f2;_LL265: {
# 1462
int _tmp270=Cyc_CfFlowInfo_get_field_index(_tmp27B,_tmp345);
if((_tmp277  && _tmp276 != - 1) && _tmp276 != _tmp270)
return({struct _tuple16 _tmp271;_tmp271.f1=_tmp27D;({void*_tmp729=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),1,_tmp147->unknown_none);_tmp271.f2=_tmp729;});_tmp271;});
return({struct _tuple16 _tmp272;_tmp272.f1=_tmp27D;_tmp272.f2=*((void**)_check_dyneither_subscript(_tmp275,sizeof(void*),_tmp270));_tmp272;});}}else{_LL266: _LL267:
({void*_tmp273=0;({struct _dyneither_ptr _tmp72A=({const char*_tmp274="anal_Rexp: AggrArrow";_tag_dyneither(_tmp274,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72A,_tag_dyneither(_tmp273,sizeof(void*),0));});});}_LL263:;};};}else{_LL25E: _LL25F:
# 1468
({void*_tmp279=0;({struct _dyneither_ptr _tmp72B=({const char*_tmp27A="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp27A,sizeof(char),25);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72B,_tag_dyneither(_tmp279,sizeof(void*),0));});});}_LL25B:;};};}case 6: _LL180: _tmp349=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp348=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_tmp347=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL181: {
# 1472
struct _tuple19 _tmp280=Cyc_NewControlFlow_anal_test(env,inflow,_tmp349);struct _tuple19 _tmp281=_tmp280;union Cyc_CfFlowInfo_FlowInfo _tmp285;union Cyc_CfFlowInfo_FlowInfo _tmp284;_LL269: _tmp285=_tmp281.f1;_tmp284=_tmp281.f2;_LL26A:;{
struct _tuple16 _tmp282=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp285,_tmp348);
struct _tuple16 _tmp283=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp284,_tmp347);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,_tmp282,_tmp283);};}case 7: _LL182: _tmp34B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp34A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL183: {
# 1478
struct _tuple19 _tmp286=Cyc_NewControlFlow_anal_test(env,inflow,_tmp34B);struct _tuple19 _tmp287=_tmp286;union Cyc_CfFlowInfo_FlowInfo _tmp291;union Cyc_CfFlowInfo_FlowInfo _tmp290;_LL26C: _tmp291=_tmp287.f1;_tmp290=_tmp287.f2;_LL26D:;{
struct _tuple16 _tmp288=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp291,_tmp34A);struct _tuple16 _tmp289=_tmp288;union Cyc_CfFlowInfo_FlowInfo _tmp28F;void*_tmp28E;_LL26F: _tmp28F=_tmp289.f1;_tmp28E=_tmp289.f2;_LL270:;{
struct _tuple16 _tmp28A=({struct _tuple16 _tmp28D;_tmp28D.f1=_tmp28F;_tmp28D.f2=_tmp28E;_tmp28D;});
struct _tuple16 _tmp28B=({struct _tuple16 _tmp28C;_tmp28C.f1=_tmp290;_tmp28C.f2=_tmp147->zero;_tmp28C;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,_tmp28A,_tmp28B);};};}case 8: _LL184: _tmp34D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp34C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL185: {
# 1485
struct _tuple19 _tmp292=Cyc_NewControlFlow_anal_test(env,inflow,_tmp34D);struct _tuple19 _tmp293=_tmp292;union Cyc_CfFlowInfo_FlowInfo _tmp29D;union Cyc_CfFlowInfo_FlowInfo _tmp29C;_LL272: _tmp29D=_tmp293.f1;_tmp29C=_tmp293.f2;_LL273:;{
struct _tuple16 _tmp294=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29C,_tmp34C);struct _tuple16 _tmp295=_tmp294;union Cyc_CfFlowInfo_FlowInfo _tmp29B;void*_tmp29A;_LL275: _tmp29B=_tmp295.f1;_tmp29A=_tmp295.f2;_LL276:;{
struct _tuple16 _tmp296=({struct _tuple16 _tmp299;_tmp299.f1=_tmp29B;_tmp299.f2=_tmp29A;_tmp299;});
struct _tuple16 _tmp297=({struct _tuple16 _tmp298;_tmp298.f1=_tmp29D;_tmp298.f2=_tmp147->notzeroall;_tmp298;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,_tmp296,_tmp297);};};}case 23: _LL186: _tmp34F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp34E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL187: {
# 1492
struct _tuple23 _tmp29E=({struct Cyc_NewControlFlow_AnalEnv*_tmp72D=env;union Cyc_CfFlowInfo_FlowInfo _tmp72C=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp72D,_tmp72C,({struct Cyc_Absyn_Exp*_tmp2BD[2];_tmp2BD[1]=_tmp34E;_tmp2BD[0]=_tmp34F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2BD,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple23 _tmp29F=_tmp29E;union Cyc_CfFlowInfo_FlowInfo _tmp2BC;struct Cyc_List_List*_tmp2BB;_LL278: _tmp2BC=_tmp29F.f1;_tmp2BB=_tmp29F.f2;_LL279:;{
# 1496
union Cyc_CfFlowInfo_FlowInfo _tmp2A0=_tmp2BC;
{union Cyc_CfFlowInfo_FlowInfo _tmp2A1=_tmp2BC;struct Cyc_Dict_Dict _tmp2A6;struct Cyc_List_List*_tmp2A5;if((_tmp2A1.ReachableFL).tag == 2){_LL27B: _tmp2A6=((_tmp2A1.ReachableFL).val).f1;_tmp2A5=((_tmp2A1.ReachableFL).val).f2;_LL27C:
# 1501
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2A6,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2BB))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2A2=0;({unsigned int _tmp72F=_tmp34E->loc;struct _dyneither_ptr _tmp72E=({const char*_tmp2A3="expression may not be initialized";_tag_dyneither(_tmp2A3,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp72F,_tmp72E,_tag_dyneither(_tmp2A2,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp2A4=Cyc_NewControlFlow_add_subscript_reln(_tmp2A5,_tmp34F,_tmp34E);
if(_tmp2A5 != _tmp2A4)
({union Cyc_CfFlowInfo_FlowInfo _tmp730=Cyc_CfFlowInfo_ReachableFL(_tmp2A6,_tmp2A4);_tmp2A0=_tmp730;});
goto _LL27A;};}else{_LL27D: _LL27E:
 goto _LL27A;}_LL27A:;}{
# 1509
void*_tmp2A7=Cyc_Tcutil_compress((void*)_check_null(_tmp34F->topt));void*_tmp2A8=_tmp2A7;struct Cyc_List_List*_tmp2BA;switch(*((int*)_tmp2A8)){case 10: _LL280: _tmp2BA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2A8)->f1;_LL281: {
# 1511
struct _tuple15 _tmp2A9=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp2BB))->hd);struct _tuple15 _tmp2AA=_tmp2A9;void*_tmp2B0;_LL287: _tmp2B0=_tmp2AA.f1;_LL288:;{
void*_tmp2AB=_tmp2B0;struct _dyneither_ptr _tmp2AF;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->tag == 6){_LL28A: _tmp2AF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->f2;_LL28B: {
# 1514
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp34E)).f1;
return({struct _tuple16 _tmp2AC;_tmp2AC.f1=_tmp2A0;_tmp2AC.f2=*((void**)_check_dyneither_subscript(_tmp2AF,sizeof(void*),(int)i));_tmp2AC;});}}else{_LL28C: _LL28D:
({void*_tmp2AD=0;({struct _dyneither_ptr _tmp731=({const char*_tmp2AE="anal_Rexp: Subscript";_tag_dyneither(_tmp2AE,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp731,_tag_dyneither(_tmp2AD,sizeof(void*),0));});});}_LL289:;};}case 5: _LL282: _LL283: {
# 1519
struct _tuple16 _tmp2B1=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BC,_tmp34F,(void*)((struct Cyc_List_List*)_check_null(_tmp2BB))->hd);struct _tuple16 _tmp2B2=_tmp2B1;union Cyc_CfFlowInfo_FlowInfo _tmp2B7;void*_tmp2B6;_LL28F: _tmp2B7=_tmp2B2.f1;_tmp2B6=_tmp2B2.f2;_LL290:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2B3=_tmp2B7;if((_tmp2B3.BottomFL).tag == 1){_LL292: _LL293:
 return({struct _tuple16 _tmp2B4;_tmp2B4.f1=_tmp2B7;_tmp2B4.f2=_tmp2B6;_tmp2B4;});}else{_LL294: _LL295:
 return({struct _tuple16 _tmp2B5;_tmp2B5.f1=_tmp2A0;_tmp2B5.f2=_tmp2B6;_tmp2B5;});}_LL291:;};}default: _LL284: _LL285:
# 1524
({void*_tmp2B8=0;({struct _dyneither_ptr _tmp732=({const char*_tmp2B9="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2B9,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp732,_tag_dyneither(_tmp2B8,sizeof(void*),0));});});}_LL27F:;};};}case 24: _LL18A: _tmp350=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL18B: {
# 1530
struct _tuple23 _tmp2BE=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp350,1,1);struct _tuple23 _tmp2BF=_tmp2BE;union Cyc_CfFlowInfo_FlowInfo _tmp2CA;struct Cyc_List_List*_tmp2C9;_LL297: _tmp2CA=_tmp2BF.f1;_tmp2C9=_tmp2BF.f2;_LL298:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2C4=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp350);void**_tmp2C5=(void**)_cycalloc(_check_times(sizeof(void*),_tmp2C4));struct _dyneither_ptr _tmp2C8=_tag_dyneither(_tmp2C5,sizeof(void*),_tmp2C4);{unsigned int _tmp2C6=_tmp2C4;unsigned int i;for(i=0;i < _tmp2C6;i ++){({typeof((void*)({
void*_tmp2C7=(void*)((struct Cyc_List_List*)_check_null(_tmp2C9))->hd;
_tmp2C9=_tmp2C9->tl;
_tmp2C7;}))_tmp733=(void*)({
# 1533
void*_tmp2C7=(void*)((struct Cyc_List_List*)_check_null(_tmp2C9))->hd;
_tmp2C9=_tmp2C9->tl;
_tmp2C7;});_tmp2C5[i]=_tmp733;});}}_tmp2C8;});
# 1537
return({struct _tuple16 _tmp2C0;_tmp2C0.f1=_tmp2CA;({void*_tmp736=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp735=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2C2;_tmp2C2.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp734=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2C3;_tmp2C3.is_union=0;_tmp2C3.fieldnum=- 1;_tmp2C3;});_tmp2C2.f1=_tmp734;});_tmp2C2.f2=aggrdict;_tmp2C2;});_tmp2C1[0]=_tmp735;});_tmp2C1;});_tmp2C0.f2=_tmp736;});_tmp2C0;});};}case 30: _LL18C: _tmp351=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL18D: {
# 1540
struct Cyc_List_List*fs;
{void*_tmp2CB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2CC=_tmp2CB;struct Cyc_List_List*_tmp2CF;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CC)->tag == 12){_LL29A: _tmp2CF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CC)->f2;_LL29B:
 fs=_tmp2CF;goto _LL299;}else{_LL29C: _LL29D:
({void*_tmp2CD=0;({struct _dyneither_ptr _tmp737=({const char*_tmp2CE="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2CE,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp737,_tag_dyneither(_tmp2CD,sizeof(void*),0));});});}_LL299:;}
# 1545
_tmp354=_tmp351;_tmp353=Cyc_Absyn_StructA;_tmp352=fs;goto _LL18F;}case 29: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4)->impl != 0){_LL18E: _tmp354=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_tmp353=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4)->kind;_tmp352=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4)->impl)->fields;_LL18F: {
# 1547
void*exp_type=(void*)_check_null(e->topt);
struct _tuple23 _tmp2D0=({struct Cyc_NewControlFlow_AnalEnv*_tmp739=env;union Cyc_CfFlowInfo_FlowInfo _tmp738=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp739,_tmp738,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp354),1,1);});struct _tuple23 _tmp2D1=_tmp2D0;union Cyc_CfFlowInfo_FlowInfo _tmp2DE;struct Cyc_List_List*_tmp2DD;_LL29F: _tmp2DE=_tmp2D1.f1;_tmp2DD=_tmp2D1.f2;_LL2A0:;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp147,_tmp352,_tmp353 == Cyc_Absyn_UnionA,_tmp147->unknown_all);
# 1552
int field_no=-1;
{int i=0;for(0;_tmp2DD != 0;(((_tmp2DD=_tmp2DD->tl,_tmp354=_tmp354->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp354))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2D2=(void*)ds->hd;void*_tmp2D3=_tmp2D2;struct _dyneither_ptr*_tmp2D7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->tag == 0){_LL2A2: _LL2A3:
({void*_tmp2D4=0;({struct _dyneither_ptr _tmp73A=({const char*_tmp2D5="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2D5,sizeof(char),22);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp73A,_tag_dyneither(_tmp2D4,sizeof(void*),0));});});}else{_LL2A4: _tmp2D7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D3)->f1;_LL2A5:
# 1559
({int _tmp73B=Cyc_CfFlowInfo_get_field_index_fs(_tmp352,_tmp2D7);field_no=_tmp73B;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp2DD->hd;
# 1562
if(_tmp353 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2D6=(*((struct _tuple26*)_tmp354->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp73C=Cyc_NewControlFlow_use_Rval(env,_tmp2D6->loc,_tmp2DE,(void*)_tmp2DD->hd);_tmp2DE=_tmp73C;});
# 1566
Cyc_NewControlFlow_check_union_requires(_tmp2D6->loc,exp_type,_tmp2D7,_tmp2DE);}}_LL2A1:;}}}{
# 1569
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D8=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp73E=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2DB;_tmp2DB.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp73D=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2DC;_tmp2DC.is_union=_tmp353 == Cyc_Absyn_UnionA;_tmp2DC.fieldnum=field_no;_tmp2DC;});_tmp2DB.f1=_tmp73D;});_tmp2DB.f2=aggrdict;_tmp2DB;});_tmp2DA[0]=_tmp73E;});_tmp2DA;});
return({struct _tuple16 _tmp2D9;_tmp2D9.f1=_tmp2DE;_tmp2D9.f2=(void*)_tmp2D8;_tmp2D9;});};};}}else{goto _LL190;}}else{_LL190: _LL191:
# 1573
({struct Cyc_String_pa_PrintArg_struct _tmp2E1;_tmp2E1.tag=0;({struct _dyneither_ptr _tmp73F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp2E1.f1=_tmp73F;});({void*_tmp2DF[1]={& _tmp2E1};({struct _dyneither_ptr _tmp740=({const char*_tmp2E0="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp2E0,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp740,_tag_dyneither(_tmp2DF,sizeof(void*),1));});});});}case 26: _LL192: _tmp355=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL193: {
# 1575
struct Cyc_List_List*_tmp2E2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp355);
struct _tuple23 _tmp2E3=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E2,1,1);struct _tuple23 _tmp2E4=_tmp2E3;union Cyc_CfFlowInfo_FlowInfo _tmp2E7;struct Cyc_List_List*_tmp2E6;_LL2A7: _tmp2E7=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;_LL2A8:;
for(0;_tmp2E6 != 0;(_tmp2E6=_tmp2E6->tl,_tmp2E2=_tmp2E2->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp741=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2E2))->hd)->loc,_tmp2E7,(void*)_tmp2E6->hd);_tmp2E7=_tmp741;});}
return({struct _tuple16 _tmp2E5;_tmp2E5.f1=_tmp2E7;({void*_tmp742=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp2E5.f2=_tmp742;});_tmp2E5;});}case 27: _LL194: _tmp359=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp358=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_tmp357=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_tmp356=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f4;_LL195: {
# 1583
struct _tuple16 _tmp2E8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp358);struct _tuple16 _tmp2E9=_tmp2E8;union Cyc_CfFlowInfo_FlowInfo _tmp30C;void*_tmp30B;_LL2AA: _tmp30C=_tmp2E9.f1;_tmp30B=_tmp2E9.f2;_LL2AB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2EA=_tmp30C;struct Cyc_Dict_Dict _tmp30A;struct Cyc_List_List*_tmp309;if((_tmp2EA.BottomFL).tag == 1){_LL2AD: _LL2AE:
 return({struct _tuple16 _tmp2EB;_tmp2EB.f1=_tmp30C;_tmp2EB.f2=_tmp147->unknown_all;_tmp2EB;});}else{_LL2AF: _tmp30A=((_tmp2EA.ReachableFL).val).f1;_tmp309=((_tmp2EA.ReachableFL).val).f2;_LL2B0:
# 1587
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp30A,_tmp30B)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2EC=0;({unsigned int _tmp744=_tmp358->loc;struct _dyneither_ptr _tmp743=({const char*_tmp2ED="expression may not be initialized";_tag_dyneither(_tmp2ED,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp744,_tmp743,_tag_dyneither(_tmp2EC,sizeof(void*),0));});});{
# 1591
struct Cyc_List_List*new_relns=_tmp309;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp359);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp358,& n2))
({struct Cyc_List_List*_tmp745=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp309);new_relns=_tmp745;});
if(_tmp309 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp746=Cyc_CfFlowInfo_ReachableFL(_tmp30A,new_relns);_tmp30C=_tmp746;});{
# 1600
void*_tmp2EE=_tmp30B;switch(*((int*)_tmp2EE)){case 0: _LL2B2: _LL2B3:
 return({struct _tuple16 _tmp2EF;_tmp2EF.f1=_tmp30C;_tmp2EF.f2=_tmp147->unknown_all;_tmp2EF;});case 1: _LL2B4: _LL2B5:
 goto _LL2B7;case 4: _LL2B6: _LL2B7: {
# 1604
struct Cyc_List_List _tmp2F0=({struct Cyc_List_List _tmp2FB;_tmp2FB.hd=_tmp359;_tmp2FB.tl=0;_tmp2FB;});
({union Cyc_CfFlowInfo_FlowInfo _tmp747=Cyc_NewControlFlow_add_vars(_tmp147,_tmp30C,& _tmp2F0,_tmp147->unknown_all,e->loc,0);_tmp30C=_tmp747;});{
# 1607
struct _tuple16 _tmp2F1=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp30C,_tmp357);struct _tuple16 _tmp2F2=_tmp2F1;union Cyc_CfFlowInfo_FlowInfo _tmp2FA;void*_tmp2F9;_LL2BB: _tmp2FA=_tmp2F2.f1;_tmp2F9=_tmp2F2.f2;_LL2BC:;
{union Cyc_CfFlowInfo_FlowInfo _tmp2F3=_tmp2FA;struct Cyc_Dict_Dict _tmp2F8;if((_tmp2F3.BottomFL).tag == 1){_LL2BE: _LL2BF:
 return({struct _tuple16 _tmp2F4;_tmp2F4.f1=_tmp2FA;_tmp2F4.f2=_tmp147->unknown_all;_tmp2F4;});}else{_LL2C0: _tmp2F8=((_tmp2F3.ReachableFL).val).f1;_LL2C1:
# 1611
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp2F8,_tmp2F9)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp2F5=0;({unsigned int _tmp749=_tmp358->loc;struct _dyneither_ptr _tmp748=({const char*_tmp2F6="expression may not be initialized";_tag_dyneither(_tmp2F6,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp749,_tmp748,_tag_dyneither(_tmp2F5,sizeof(void*),0));});});
return({struct _tuple16 _tmp2F7;({union Cyc_CfFlowInfo_FlowInfo _tmp74A=Cyc_CfFlowInfo_BottomFL();_tmp2F7.f1=_tmp74A;});_tmp2F7.f2=_tmp147->unknown_all;_tmp2F7;});}}_LL2BD:;}
# 1616
_tmp30C=_tmp2FA;
goto _LL2B9;};}default: _LL2B8: _LL2B9:
# 1619
 while(1){
struct Cyc_List_List _tmp2FC=({struct Cyc_List_List _tmp307;_tmp307.hd=_tmp359;_tmp307.tl=0;_tmp307;});
({union Cyc_CfFlowInfo_FlowInfo _tmp74B=Cyc_NewControlFlow_add_vars(_tmp147,_tmp30C,& _tmp2FC,_tmp147->unknown_all,e->loc,0);_tmp30C=_tmp74B;});{
struct _tuple16 _tmp2FD=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp30C,_tmp357);struct _tuple16 _tmp2FE=_tmp2FD;union Cyc_CfFlowInfo_FlowInfo _tmp306;void*_tmp305;_LL2C3: _tmp306=_tmp2FE.f1;_tmp305=_tmp2FE.f2;_LL2C4:;
{union Cyc_CfFlowInfo_FlowInfo _tmp2FF=_tmp306;struct Cyc_Dict_Dict _tmp303;if((_tmp2FF.BottomFL).tag == 1){_LL2C6: _LL2C7:
 goto _LL2C5;}else{_LL2C8: _tmp303=((_tmp2FF.ReachableFL).val).f1;_LL2C9:
# 1626
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp303,_tmp305)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp300=0;({unsigned int _tmp74D=_tmp358->loc;struct _dyneither_ptr _tmp74C=({const char*_tmp301="expression may not be initialized";_tag_dyneither(_tmp301,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp74D,_tmp74C,_tag_dyneither(_tmp300,sizeof(void*),0));});});
return({struct _tuple16 _tmp302;({union Cyc_CfFlowInfo_FlowInfo _tmp74E=Cyc_CfFlowInfo_BottomFL();_tmp302.f1=_tmp74E;});_tmp302.f2=_tmp147->unknown_all;_tmp302;});}}_LL2C5:;}{
# 1631
union Cyc_CfFlowInfo_FlowInfo _tmp304=Cyc_CfFlowInfo_join_flow(_tmp147,_tmp30C,_tmp306);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp304,_tmp30C))
break;
_tmp30C=_tmp304;};};}
# 1636
return({struct _tuple16 _tmp308;_tmp308.f1=_tmp30C;_tmp308.f2=_tmp147->unknown_all;_tmp308;});}_LL2B1:;};};}_LL2AC:;};}case 28: _LL196: _tmp35C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp35B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_tmp35A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL197: {
# 1642
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp318=_cycalloc(sizeof(*_tmp318));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp74F=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp319;_tmp319.tag=1;_tmp319.f1=_tmp35C;_tmp319.f2=(void*)_check_null(e->topt);_tmp319;});_tmp318[0]=_tmp74F;});_tmp318;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->root=root;_tmp317->path=0;_tmp317;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp315=_cycalloc(sizeof(*_tmp315));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp750=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp316;_tmp316.tag=4;_tmp316.f1=place;_tmp316;});_tmp315[0]=_tmp750;});_tmp315;});
void*place_val;
# 1650
({void*_tmp751=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,_tmp35B,0,_tmp147->unknown_none);place_val=_tmp751;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp30D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35C);struct _tuple16 _tmp30E=_tmp30D;union Cyc_CfFlowInfo_FlowInfo _tmp314;_LL2CB: _tmp314=_tmp30E.f1;_LL2CC:;
outflow=_tmp314;{
union Cyc_CfFlowInfo_FlowInfo _tmp30F=outflow;struct Cyc_Dict_Dict _tmp313;struct Cyc_List_List*_tmp312;if((_tmp30F.BottomFL).tag == 1){_LL2CE: _LL2CF:
 return({struct _tuple16 _tmp310;_tmp310.f1=outflow;_tmp310.f2=rval;_tmp310;});}else{_LL2D0: _tmp313=((_tmp30F.ReachableFL).val).f1;_tmp312=((_tmp30F.ReachableFL).val).f2;_LL2D1:
# 1657
 return({struct _tuple16 _tmp311;({union Cyc_CfFlowInfo_FlowInfo _tmp753=({struct Cyc_Dict_Dict _tmp752=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp313,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp752,_tmp312);});_tmp311.f1=_tmp753;});_tmp311.f2=rval;_tmp311;});}_LL2CD:;};};}case 37: _LL198: _tmp35D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL199: {
# 1661
struct _tuple17 _tmp31A=({struct _tuple17 _tmp31D;_tmp31D.f1=(env->fenv)->unknown_all;_tmp31D.f2=copy_ctxt;_tmp31D;});
union Cyc_CfFlowInfo_FlowInfo _tmp31B=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp35D,& _tmp31A);
return({struct _tuple16 _tmp31C;_tmp31C.f1=_tmp31B;_tmp31C.f2=_tmp31A.f1;_tmp31C;});}case 36: _LL19C: _LL19D:
# 1666
 goto _LL19F;case 25: _LL19E: _LL19F:
 goto _LL1A1;case 39: _LL1A0: _LL1A1:
 goto _LL1A3;default: _LL1A2: _LL1A3:
({void*_tmp31E=0;({struct _dyneither_ptr _tmp754=({const char*_tmp31F="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp31F,sizeof(char),31);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp754,_tag_dyneither(_tmp31E,sizeof(void*),0));});});}_LL139:;};}
# 1681 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1690
struct Cyc_CfFlowInfo_FlowEnv*_tmp35E=env->fenv;
void*_tmp35F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp360=_tmp35F;union Cyc_Absyn_Constraint*_tmp390;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360)->tag == 5){_LL2D3: _tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360)->f1).ptr_atts).bounds;_LL2D4: {
# 1693
union Cyc_CfFlowInfo_FlowInfo _tmp361=f;struct Cyc_Dict_Dict _tmp38D;struct Cyc_List_List*_tmp38C;if((_tmp361.BottomFL).tag == 1){_LL2D8: _LL2D9:
 return({struct _tuple18 _tmp362;_tmp362.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp755=Cyc_CfFlowInfo_UnknownL();_tmp362.f2=_tmp755;});_tmp362;});}else{_LL2DA: _tmp38D=((_tmp361.ReachableFL).val).f1;_tmp38C=((_tmp361.ReachableFL).val).f2;_LL2DB: {
# 1697
struct _tuple15 _tmp363=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp364=_tmp363;void*_tmp38B;struct Cyc_List_List*_tmp38A;_LL2DD: _tmp38B=_tmp364.f1;_tmp38A=_tmp364.f2;_LL2DE:;
retry: {void*_tmp365=_tmp38B;void*_tmp380;enum Cyc_CfFlowInfo_InitLevel _tmp37F;void*_tmp37E;struct Cyc_List_List*_tmp37D;void*_tmp37C;switch(*((int*)_tmp365)){case 8: _LL2E0: _LL2E1:
# 1700
({void*_tmp366=0;({struct _dyneither_ptr _tmp756=({const char*_tmp367="named location in anal_derefL";_tag_dyneither(_tmp367,sizeof(char),30);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp756,_tag_dyneither(_tmp366,sizeof(void*),0));});});case 1: _LL2E2: _LL2E3:
# 1702
({void*_tmp758=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp368=_cycalloc(sizeof(*_tmp368));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp757=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp369;_tmp369.tag=Cyc_CfFlowInfo_NotZero;_tmp369.f1=_tmp38C;_tmp369;});_tmp368[0]=_tmp757;});_tmp368;});e->annot=_tmp758;});goto _LL2DF;case 5: _LL2E4: _tmp37C=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp365)->f1;_LL2E5:
# 1705
 if(expanded_place_possibly_null)
# 1708
({void*_tmp75A=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp36A=_cycalloc(sizeof(*_tmp36A));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp759=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp36B;_tmp36B.tag=Cyc_CfFlowInfo_UnknownZ;_tmp36B.f1=_tmp38C;_tmp36B;});_tmp36A[0]=_tmp759;});_tmp36A;});e->annot=_tmp75A;});else{
# 1711
void*_tmp36C=e->annot;void*_tmp36D=_tmp36C;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp36D)->tag == Cyc_CfFlowInfo_UnknownZ){_LL2F1: _LL2F2:
# 1715
({void*_tmp75C=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp75B=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp36F;_tmp36F.tag=Cyc_CfFlowInfo_UnknownZ;_tmp36F.f1=_tmp38C;_tmp36F;});_tmp36E[0]=_tmp75B;});_tmp36E;});e->annot=_tmp75C;});
goto _LL2F0;}else{_LL2F3: _LL2F4:
# 1718
({void*_tmp75E=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp370=_cycalloc(sizeof(*_tmp370));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp75D=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp371;_tmp371.tag=Cyc_CfFlowInfo_NotZero;_tmp371.f1=_tmp38C;_tmp371;});_tmp370[0]=_tmp75D;});_tmp370;});e->annot=_tmp75E;});
goto _LL2F0;}_LL2F0:;}
# 1723
if(Cyc_Tcutil_is_bound_one(_tmp390))return({struct _tuple18 _tmp372;_tmp372.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp75F=Cyc_CfFlowInfo_UnknownL();_tmp372.f2=_tmp75F;});_tmp372;});
goto _LL2DF;case 4: _LL2E6: _tmp37E=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp365)->f1)->root;_tmp37D=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp365)->f1)->path;_LL2E7:
# 1727
({void*_tmp761=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp373=_cycalloc(sizeof(*_tmp373));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp760=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp374;_tmp374.tag=Cyc_CfFlowInfo_NotZero;_tmp374.f1=_tmp38C;_tmp374;});_tmp373[0]=_tmp760;});_tmp373;});e->annot=_tmp761;});
if(Cyc_Tcutil_is_bound_one(_tmp390))
return({struct _tuple18 _tmp375;_tmp375.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp763=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->root=_tmp37E;({struct Cyc_List_List*_tmp762=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp37D,path);_tmp376->path=_tmp762;});_tmp376;}));_tmp375.f2=_tmp763;});_tmp375;});
goto _LL2DF;case 0: _LL2E8: _LL2E9:
# 1732
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp377;({union Cyc_CfFlowInfo_FlowInfo _tmp764=Cyc_CfFlowInfo_BottomFL();_tmp377.f1=_tmp764;});({union Cyc_CfFlowInfo_AbsLVal _tmp765=Cyc_CfFlowInfo_UnknownL();_tmp377.f2=_tmp765;});_tmp377;});case 2: _LL2EA: _tmp37F=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp365)->f1;_LL2EB:
# 1737
 if(Cyc_Tcutil_is_bound_one(_tmp390))
({union Cyc_CfFlowInfo_FlowInfo _tmp766=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp37F,_tmp38A);f=_tmp766;});
({void*_tmp768=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp378=_cycalloc(sizeof(*_tmp378));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp767=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp379;_tmp379.tag=Cyc_CfFlowInfo_UnknownZ;_tmp379.f1=_tmp38C;_tmp379;});_tmp378[0]=_tmp767;});_tmp378;});e->annot=_tmp768;});
goto _LL2DF;case 7: _LL2EC: _tmp380=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp365)->f3;_LL2ED:
# 1742
 if(passthrough_consumes){
_tmp38B=_tmp380;goto retry;}
# 1745
goto _LL2EF;default: _LL2EE: _LL2EF:
# 1747
({void*_tmp76A=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp37A=_cycalloc(sizeof(*_tmp37A));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp769=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp37B;_tmp37B.tag=Cyc_CfFlowInfo_UnknownZ;_tmp37B.f1=_tmp38C;_tmp37B;});_tmp37A[0]=_tmp769;});_tmp37A;});e->annot=_tmp76A;});
goto _LL2DF;}_LL2DF:;}
# 1750
if(Cyc_CfFlowInfo_initlevel(_tmp35E,_tmp38D,_tmp38B)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp381=Cyc_CfFlowInfo_unname_rval(_tmp38B);struct _tuple15 _tmp382=_tmp381;void*_tmp388;_LL2F6: _tmp388=_tmp382.f1;_LL2F7:;{
void*_tmp383=_tmp388;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp383)->tag == 7){_LL2F9: _LL2FA:
# 1754
({void*_tmp384=0;({unsigned int _tmp76C=e->loc;struct _dyneither_ptr _tmp76B=({const char*_tmp385="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp385,sizeof(char),66);});Cyc_CfFlowInfo_aerr(_tmp76C,_tmp76B,_tag_dyneither(_tmp384,sizeof(void*),0));});});
goto _LL2F8;}else{_LL2FB: _LL2FC:
# 1757
({void*_tmp386=0;({unsigned int _tmp76E=e->loc;struct _dyneither_ptr _tmp76D=({const char*_tmp387="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp387,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp76E,_tmp76D,_tag_dyneither(_tmp386,sizeof(void*),0));});});
goto _LL2F8;}_LL2F8:;};}
# 1761
return({struct _tuple18 _tmp389;_tmp389.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp76F=Cyc_CfFlowInfo_UnknownL();_tmp389.f2=_tmp76F;});_tmp389;});}}_LL2D7:;}}else{_LL2D5: _LL2D6:
# 1763
({void*_tmp38E=0;({struct _dyneither_ptr _tmp770=({const char*_tmp38F="left deref of non-pointer-type";_tag_dyneither(_tmp38F,sizeof(char),31);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp770,_tag_dyneither(_tmp38E,sizeof(void*),0));});});}_LL2D2:;}
# 1769
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp391=p;void*_tmp397;struct Cyc_List_List*_tmp396;_LL2FE: _tmp397=_tmp391->root;_tmp396=_tmp391->path;_LL2FF:;
({struct Cyc_List_List*_tmp773=({struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));({void*_tmp772=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp393=_cycalloc_atomic(sizeof(*_tmp393));({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp771=({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp394;_tmp394.tag=1;_tmp394;});_tmp393[0]=_tmp771;});_tmp393;});_tmp392->hd=_tmp772;});_tmp392->tl=path;_tmp392;});path=_tmp773;});
return({struct Cyc_CfFlowInfo_Place*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->root=_tmp397;({struct Cyc_List_List*_tmp774=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp396,path);_tmp395->path=_tmp774;});_tmp395;});}
# 1779
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1783
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp398=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp399=inflow;struct Cyc_Dict_Dict _tmp39C;struct Cyc_List_List*_tmp39B;if((_tmp399.BottomFL).tag == 1){_LL301: _LL302:
 return({struct _tuple18 _tmp39A;({union Cyc_CfFlowInfo_FlowInfo _tmp775=Cyc_CfFlowInfo_BottomFL();_tmp39A.f1=_tmp775;});({union Cyc_CfFlowInfo_AbsLVal _tmp776=Cyc_CfFlowInfo_UnknownL();_tmp39A.f2=_tmp776;});_tmp39A;});}else{_LL303: _tmp39C=((_tmp399.ReachableFL).val).f1;_tmp39B=((_tmp399.ReachableFL).val).f2;_LL304:
# 1788
 d=_tmp39C;}_LL300:;}{
# 1792
void*_tmp39D=e->r;void*_tmp39E=_tmp39D;struct Cyc_Absyn_Exp*_tmp405;struct _dyneither_ptr*_tmp404;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct _dyneither_ptr*_tmp3FF;struct Cyc_Absyn_Vardecl*_tmp3FE;struct Cyc_Absyn_Vardecl*_tmp3FD;struct Cyc_Absyn_Vardecl*_tmp3FC;struct Cyc_Absyn_Vardecl*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Exp*_tmp3F8;switch(*((int*)_tmp39E)){case 14: _LL306: _tmp3F8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp39E)->f2;_LL307:
 _tmp3F9=_tmp3F8;goto _LL309;case 12: _LL308: _tmp3F9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp39E)->f1;_LL309:
 _tmp3FA=_tmp3F9;goto _LL30B;case 13: _LL30A: _tmp3FA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp39E)->f1;_LL30B:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3FA,path);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39E)->f1)){case 3: _LL30C: _tmp3FB=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39E)->f1)->f1;_LL30D:
# 1797
 _tmp3FC=_tmp3FB;goto _LL30F;case 4: _LL30E: _tmp3FC=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39E)->f1)->f1;_LL30F:
 _tmp3FD=_tmp3FC;goto _LL311;case 5: _LL310: _tmp3FD=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39E)->f1)->f1;_LL311:
# 1800
 return({struct _tuple18 _tmp39F;_tmp39F.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp779=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({void*_tmp778=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp777=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3A2;_tmp3A2.tag=0;_tmp3A2.f1=_tmp3FD;_tmp3A2;});_tmp3A1[0]=_tmp777;});_tmp3A1;});_tmp3A0->root=_tmp778;});_tmp3A0->path=path;_tmp3A0;}));_tmp39F.f2=_tmp779;});_tmp39F;});case 1: _LL312: _tmp3FE=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39E)->f1)->f1;_LL313:
# 1802
 return({struct _tuple18 _tmp3A3;_tmp3A3.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp77A=Cyc_CfFlowInfo_UnknownL();_tmp3A3.f2=_tmp77A;});_tmp3A3;});default: goto _LL31C;}case 22: _LL314: _tmp400=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp39E)->f1;_tmp3FF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp39E)->f2;_LL315:
# 1805
{void*_tmp3A4=Cyc_Tcutil_compress((void*)_check_null(_tmp400->topt));void*_tmp3A5=_tmp3A4;void*_tmp3AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A5)->tag == 5){_LL31F: _tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A5)->f1).elt_typ;_LL320:
# 1807
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3AB)){
Cyc_NewControlFlow_check_union_requires(_tmp400->loc,_tmp3AB,_tmp3FF,inflow);
({struct Cyc_List_List*_tmp77E=({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({void*_tmp77D=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3A7=_cycalloc_atomic(sizeof(*_tmp3A7));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp77C=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3A8;_tmp3A8.tag=0;({int _tmp77B=Cyc_CfFlowInfo_get_field_index(_tmp3AB,_tmp3FF);_tmp3A8.f1=_tmp77B;});_tmp3A8;});_tmp3A7[0]=_tmp77C;});_tmp3A7;});_tmp3A6->hd=_tmp77D;});_tmp3A6->tl=path;_tmp3A6;});path=_tmp77E;});}
# 1811
goto _LL31E;}else{_LL321: _LL322:
({void*_tmp3A9=0;({struct _dyneither_ptr _tmp77F=({const char*_tmp3AA="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3AA,sizeof(char),25);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp77F,_tag_dyneither(_tmp3A9,sizeof(void*),0));});});}_LL31E:;}
# 1814
_tmp401=_tmp400;goto _LL317;case 20: _LL316: _tmp401=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp39E)->f1;_LL317:
# 1818
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp401->topt),1)){
# 1820
struct _tuple18 _tmp3AC=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp401);struct _tuple18 _tmp3AD=_tmp3AC;union Cyc_CfFlowInfo_FlowInfo _tmp3D2;union Cyc_CfFlowInfo_AbsLVal _tmp3D1;_LL324: _tmp3D2=_tmp3AD.f1;_tmp3D1=_tmp3AD.f2;_LL325:;{
struct _tuple18 _tmp3AE=({struct _tuple18 _tmp3D0;_tmp3D0.f1=_tmp3D2;_tmp3D0.f2=_tmp3D1;_tmp3D0;});struct _tuple18 _tmp3AF=_tmp3AE;struct Cyc_Dict_Dict _tmp3CF;struct Cyc_List_List*_tmp3CE;struct Cyc_CfFlowInfo_Place*_tmp3CD;if(((_tmp3AF.f1).ReachableFL).tag == 2){if(((_tmp3AF.f2).PlaceL).tag == 1){_LL327: _tmp3CF=(((_tmp3AF.f1).ReachableFL).val).f1;_tmp3CE=(((_tmp3AF.f1).ReachableFL).val).f2;_tmp3CD=((_tmp3AF.f2).PlaceL).val;_LL328: {
# 1823
void*_tmp3B0=Cyc_CfFlowInfo_lookup_place(_tmp3CF,_tmp3CD);
struct _tuple15 _tmp3B1=Cyc_CfFlowInfo_unname_rval(_tmp3B0);struct _tuple15 _tmp3B2=_tmp3B1;void*_tmp3CC;struct Cyc_List_List*_tmp3CB;_LL32C: _tmp3CC=_tmp3B2.f1;_tmp3CB=_tmp3B2.f2;_LL32D:;
if(expand_unique){
int _tmp3B3=1;
void*_tmp3B4=_tmp3CC;struct Cyc_Absyn_Vardecl*_tmp3C5;void*_tmp3C4;switch(*((int*)_tmp3B4)){case 8: _LL32F: _tmp3C5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B4)->f1;_tmp3C4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B4)->f2;_LL330:
# 1829
({void*_tmp3B5=0;({struct _dyneither_ptr _tmp780=({const char*_tmp3B6="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3B6,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp780,_tag_dyneither(_tmp3B5,sizeof(void*),0));});});case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3B4)->f3)){case 4: _LL331: _LL332:
 goto _LL334;case 5: _LL335: _LL336:
# 1833
 goto _LL338;default: goto _LL33B;}case 4: _LL333: _LL334:
# 1832
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3D2,_tmp401,_tmp3D2,_tmp3B0,passthrough_consumes,0,path);case 5: _LL337: _LL338: {
# 1837
struct _tuple18 _tmp3B7=Cyc_NewControlFlow_anal_derefL(env,_tmp3D2,_tmp401,_tmp3D2,_tmp3B0,passthrough_consumes,0,path);struct _tuple18 _tmp3B8=_tmp3B7;union Cyc_CfFlowInfo_FlowInfo _tmp3BA;_LL33E: _tmp3BA=_tmp3B8.f1;_LL33F:;
return({struct _tuple18 _tmp3B9;_tmp3B9.f1=_tmp3BA;({union Cyc_CfFlowInfo_AbsLVal _tmp781=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3CD,path));_tmp3B9.f2=_tmp781;});_tmp3B9;});}case 1: _LL339: _LL33A:
# 1840
 _tmp3B3=0;goto _LL33C;default: _LL33B: _LL33C: {
# 1847
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp398,_tmp3CF,_tmp3CC);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp398->unknown_all: _tmp398->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp785=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp3C3;_tmp3C3.tag=5;({void*_tmp784=({struct Cyc_CfFlowInfo_FlowEnv*_tmp783=_tmp398;void*_tmp782=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp401->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp783,_tmp782,0,leaf);});_tmp3C3.f1=_tmp784;});_tmp3C3;});_tmp3C2[0]=_tmp785;});_tmp3C2;});
for(0;_tmp3CB != 0;_tmp3CB=_tmp3CB->tl){
({void*_tmp787=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp786=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3BC;_tmp3BC.tag=8;_tmp3BC.f1=(struct Cyc_Absyn_Vardecl*)_tmp3CB->hd;_tmp3BC.f2=res;_tmp3BC;});_tmp3BB[0]=_tmp786;});_tmp3BB;});res=_tmp787;});}
({struct Cyc_Dict_Dict _tmp788=Cyc_CfFlowInfo_assign_place(_tmp398,e->loc,_tmp3CF,_tmp3CD,res);_tmp3CF=_tmp788;});{
union Cyc_CfFlowInfo_FlowInfo _tmp3BD=Cyc_CfFlowInfo_ReachableFL(_tmp3CF,_tmp3CE);
# 1860
struct _tuple18 _tmp3BE=Cyc_NewControlFlow_anal_derefL(env,_tmp3BD,_tmp401,_tmp3BD,res,passthrough_consumes,_tmp3B3,path);struct _tuple18 _tmp3BF=_tmp3BE;union Cyc_CfFlowInfo_FlowInfo _tmp3C1;_LL341: _tmp3C1=_tmp3BF.f1;_LL342:;
# 1862
return({struct _tuple18 _tmp3C0;_tmp3C0.f1=_tmp3C1;({union Cyc_CfFlowInfo_AbsLVal _tmp789=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3CD,path));_tmp3C0.f2=_tmp789;});_tmp3C0;});};}}_LL32E:;}else{
# 1866
void*_tmp3C6=_tmp3CC;switch(*((int*)_tmp3C6)){case 7: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3C6)->f3)->tag == 5){_LL344: _LL345:
 goto _LL347;}else{goto _LL348;}case 5: _LL346: _LL347: {
# 1871
struct _tuple18 _tmp3C7=Cyc_NewControlFlow_anal_derefL(env,_tmp3D2,_tmp401,_tmp3D2,_tmp3B0,passthrough_consumes,0,path);struct _tuple18 _tmp3C8=_tmp3C7;union Cyc_CfFlowInfo_FlowInfo _tmp3CA;_LL34B: _tmp3CA=_tmp3C8.f1;_LL34C:;
return({struct _tuple18 _tmp3C9;_tmp3C9.f1=_tmp3CA;({union Cyc_CfFlowInfo_AbsLVal _tmp78A=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3CD,path));_tmp3C9.f2=_tmp78A;});_tmp3C9;});}default: _LL348: _LL349:
# 1875
 goto _LL343;}_LL343:;}
# 1878
goto _LL326;}}else{goto _LL329;}}else{_LL329: _LL32A:
 goto _LL326;}_LL326:;};}{
# 1883
struct _tuple16 _tmp3D3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp401);struct _tuple16 _tmp3D4=_tmp3D3;union Cyc_CfFlowInfo_FlowInfo _tmp3D6;void*_tmp3D5;_LL34E: _tmp3D6=_tmp3D4.f1;_tmp3D5=_tmp3D4.f2;_LL34F:;
# 1885
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp401,_tmp3D6,_tmp3D5,passthrough_consumes,0,path);};case 23: _LL318: _tmp403=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp39E)->f1;_tmp402=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp39E)->f2;_LL319: {
# 1888
void*_tmp3D7=Cyc_Tcutil_compress((void*)_check_null(_tmp403->topt));void*_tmp3D8=_tmp3D7;switch(*((int*)_tmp3D8)){case 10: _LL351: _LL352: {
# 1890
unsigned int _tmp3D9=(Cyc_Evexp_eval_const_uint_exp(_tmp402)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp791=env;union Cyc_CfFlowInfo_FlowInfo _tmp790=inflow;int _tmp78F=expand_unique;int _tmp78E=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp78D=_tmp403;Cyc_NewControlFlow_anal_Lexp_rec(_tmp791,_tmp790,_tmp78F,_tmp78E,_tmp78D,({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({void*_tmp78C=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3DB=_cycalloc_atomic(sizeof(*_tmp3DB));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp78B=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3DC;_tmp3DC.tag=0;_tmp3DC.f1=(int)_tmp3D9;_tmp3DC;});_tmp3DB[0]=_tmp78B;});_tmp3DB;});_tmp3DA->hd=_tmp78C;});_tmp3DA->tl=path;_tmp3DA;}));});}case 5: _LL353: _LL354: {
# 1894
struct _tuple23 _tmp3DD=({struct Cyc_NewControlFlow_AnalEnv*_tmp793=env;union Cyc_CfFlowInfo_FlowInfo _tmp792=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp793,_tmp792,({struct Cyc_Absyn_Exp*_tmp3EF[2];_tmp3EF[1]=_tmp402;_tmp3EF[0]=_tmp403;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EF,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple23 _tmp3DE=_tmp3DD;union Cyc_CfFlowInfo_FlowInfo _tmp3EE;struct Cyc_List_List*_tmp3ED;_LL358: _tmp3EE=_tmp3DE.f1;_tmp3ED=_tmp3DE.f2;_LL359:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3DF=_tmp3EE;
{union Cyc_CfFlowInfo_FlowInfo _tmp3E0=_tmp3EE;struct Cyc_Dict_Dict _tmp3E5;struct Cyc_List_List*_tmp3E4;if((_tmp3E0.ReachableFL).tag == 2){_LL35B: _tmp3E5=((_tmp3E0.ReachableFL).val).f1;_tmp3E4=((_tmp3E0.ReachableFL).val).f2;_LL35C:
# 1898
 if(Cyc_CfFlowInfo_initlevel(_tmp398,_tmp3E5,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3ED))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp3E1=0;({unsigned int _tmp795=_tmp402->loc;struct _dyneither_ptr _tmp794=({const char*_tmp3E2="expression may not be initialized";_tag_dyneither(_tmp3E2,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp795,_tmp794,_tag_dyneither(_tmp3E1,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp3E3=Cyc_NewControlFlow_add_subscript_reln(_tmp3E4,_tmp403,_tmp402);
if(_tmp3E4 != _tmp3E3)
({union Cyc_CfFlowInfo_FlowInfo _tmp796=Cyc_CfFlowInfo_ReachableFL(_tmp3E5,_tmp3E3);_tmp3DF=_tmp796;});
goto _LL35A;};}else{_LL35D: _LL35E:
 goto _LL35A;}_LL35A:;}{
# 1907
struct _tuple18 _tmp3E6=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp403,_tmp3EE,(void*)((struct Cyc_List_List*)_check_null(_tmp3ED))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp3E7=_tmp3E6;union Cyc_CfFlowInfo_FlowInfo _tmp3EC;union Cyc_CfFlowInfo_AbsLVal _tmp3EB;_LL360: _tmp3EC=_tmp3E7.f1;_tmp3EB=_tmp3E7.f2;_LL361:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E8=_tmp3EC;if((_tmp3E8.BottomFL).tag == 1){_LL363: _LL364:
 return({struct _tuple18 _tmp3E9;_tmp3E9.f1=_tmp3EC;_tmp3E9.f2=_tmp3EB;_tmp3E9;});}else{_LL365: _LL366:
 return({struct _tuple18 _tmp3EA;_tmp3EA.f1=_tmp3DF;_tmp3EA.f2=_tmp3EB;_tmp3EA;});}_LL362:;};};};}default: _LL355: _LL356:
# 1912
({void*_tmp3F0=0;({struct _dyneither_ptr _tmp797=({const char*_tmp3F1="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp3F1,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp797,_tag_dyneither(_tmp3F0,sizeof(void*),0));});});}_LL350:;}case 21: _LL31A: _tmp405=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp39E)->f1;_tmp404=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp39E)->f2;_LL31B: {
# 1916
void*_tmp3F2=(void*)_check_null(_tmp405->topt);
if(Cyc_Absyn_is_require_union_type(_tmp3F2))
Cyc_NewControlFlow_check_union_requires(_tmp405->loc,_tmp3F2,_tmp404,inflow);
# 1920
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3F2))
return({struct _tuple18 _tmp3F3;_tmp3F3.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp798=Cyc_CfFlowInfo_UnknownL();_tmp3F3.f2=_tmp798;});_tmp3F3;});
# 1923
return({struct Cyc_NewControlFlow_AnalEnv*_tmp7A0=env;union Cyc_CfFlowInfo_FlowInfo _tmp79F=inflow;int _tmp79E=expand_unique;int _tmp79D=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp79C=_tmp405;Cyc_NewControlFlow_anal_Lexp_rec(_tmp7A0,_tmp79F,_tmp79E,_tmp79D,_tmp79C,({struct Cyc_List_List*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));({void*_tmp79B=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3F5=_cycalloc_atomic(sizeof(*_tmp3F5));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp79A=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3F6;_tmp3F6.tag=0;({int _tmp799=
Cyc_CfFlowInfo_get_field_index(_tmp3F2,_tmp404);_tmp3F6.f1=_tmp799;});_tmp3F6;});_tmp3F5[0]=_tmp79A;});_tmp3F5;});_tmp3F4->hd=_tmp79B;});_tmp3F4->tl=path;_tmp3F4;}));});}default: _LL31C: _LL31D:
# 1927
 return({struct _tuple18 _tmp3F7;({union Cyc_CfFlowInfo_FlowInfo _tmp7A1=Cyc_CfFlowInfo_BottomFL();_tmp3F7.f1=_tmp7A1;});({union Cyc_CfFlowInfo_AbsLVal _tmp7A2=Cyc_CfFlowInfo_UnknownL();_tmp3F7.f2=_tmp7A2;});_tmp3F7;});}_LL305:;};}
# 1931
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1935
struct _tuple18 _tmp406=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp407=_tmp406;union Cyc_CfFlowInfo_FlowInfo _tmp40A;union Cyc_CfFlowInfo_AbsLVal _tmp409;_LL368: _tmp40A=_tmp407.f1;_tmp409=_tmp407.f2;_LL369:;
return({struct _tuple18 _tmp408;_tmp408.f1=_tmp40A;_tmp408.f2=_tmp409;_tmp408;});}
# 1942
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1945
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp40B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp40C=_tmp40B;union Cyc_CfFlowInfo_FlowInfo _tmp40D;_LL36B: _tmp40D=_tmp40C.f1;_LL36C:;
inflow=_tmp40D;}}}
# 1953
return inflow;}
# 1959
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1961
struct Cyc_CfFlowInfo_FlowEnv*_tmp40E=env->fenv;
# 1964
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _tuple23 _tmp40F=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);struct _tuple23 _tmp410=_tmp40F;union Cyc_CfFlowInfo_FlowInfo _tmp453;struct Cyc_List_List*_tmp452;_LL36E: _tmp453=_tmp410.f1;_tmp452=_tmp410.f2;_LL36F:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp452))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp452->tl))->hd;
f=_tmp453;{
# 1972
union Cyc_CfFlowInfo_FlowInfo _tmp411=f;struct Cyc_Dict_Dict _tmp451;struct Cyc_List_List*_tmp450;if((_tmp411.BottomFL).tag == 1){_LL371: _LL372:
 return({struct _tuple19 _tmp412;_tmp412.f1=f;_tmp412.f2=f;_tmp412;});}else{_LL373: _tmp451=((_tmp411.ReachableFL).val).f1;_tmp450=((_tmp411.ReachableFL).val).f2;_LL374: {
# 1975
struct Cyc_Absyn_Exp*_tmp413=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp414=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp451,r1)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp415=0;({unsigned int _tmp7A4=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp7A3=({const char*_tmp416="expression may not be initialized";_tag_dyneither(_tmp416,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7A4,_tmp7A3,_tag_dyneither(_tmp415,sizeof(void*),0));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp451,r2)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp417=0;({unsigned int _tmp7A6=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp7A5=({const char*_tmp418="expression may not be initialized";_tag_dyneither(_tmp418,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7A6,_tmp7A5,_tag_dyneither(_tmp417,sizeof(void*),0));});});{
# 1982
union Cyc_CfFlowInfo_FlowInfo _tmp419=f;
union Cyc_CfFlowInfo_FlowInfo _tmp41A=f;
# 1987
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp41B=Cyc_CfFlowInfo_unname_rval(r1);struct _tuple15 _tmp41C=_tmp41B;void*_tmp435;struct Cyc_List_List*_tmp434;_LL376: _tmp435=_tmp41C.f1;_tmp434=_tmp41C.f2;_LL377:;{
struct _tuple15 _tmp41D=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple15 _tmp41E=_tmp41D;void*_tmp433;struct Cyc_List_List*_tmp432;_LL379: _tmp433=_tmp41E.f1;_tmp432=_tmp41E.f2;_LL37A:;{
struct _tuple0 _tmp41F=({struct _tuple0 _tmp431;_tmp431.f1=_tmp435;_tmp431.f2=_tmp433;_tmp431;});struct _tuple0 _tmp420=_tmp41F;enum Cyc_CfFlowInfo_InitLevel _tmp430;enum Cyc_CfFlowInfo_InitLevel _tmp42F;switch(*((int*)_tmp420.f1)){case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL37C: _tmp42F=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp420.f1)->f1;_LL37D: {
# 1992
struct _tuple19 _tmp421=Cyc_NewControlFlow_splitzero(env,f,f,_tmp413,_tmp42F,_tmp434);struct _tuple19 _tmp422=_tmp421;union Cyc_CfFlowInfo_FlowInfo _tmp427;union Cyc_CfFlowInfo_FlowInfo _tmp426;_LL38D: _tmp427=_tmp422.f1;_tmp426=_tmp422.f2;_LL38E:;
{enum Cyc_Absyn_Primop _tmp423=p;switch(_tmp423){case Cyc_Absyn_Eq: _LL390: _LL391:
 _tmp419=_tmp426;_tmp41A=_tmp427;goto _LL38F;case Cyc_Absyn_Neq: _LL392: _LL393:
 _tmp419=_tmp427;_tmp41A=_tmp426;goto _LL38F;default: _LL394: _LL395:
({void*_tmp424=0;({struct _dyneither_ptr _tmp7A7=({const char*_tmp425="anal_test, zero-split";_tag_dyneither(_tmp425,sizeof(char),22);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A7,_tag_dyneither(_tmp424,sizeof(void*),0));});});}_LL38F:;}
# 1998
goto _LL37B;}}else{goto _LL38A;}case 0: switch(*((int*)_tmp420.f2)){case 2: _LL37E: _tmp430=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->f1;_LL37F: {
# 2000
struct _tuple19 _tmp428=Cyc_NewControlFlow_splitzero(env,f,f,_tmp414,_tmp430,_tmp432);struct _tuple19 _tmp429=_tmp428;union Cyc_CfFlowInfo_FlowInfo _tmp42E;union Cyc_CfFlowInfo_FlowInfo _tmp42D;_LL397: _tmp42E=_tmp429.f1;_tmp42D=_tmp429.f2;_LL398:;
{enum Cyc_Absyn_Primop _tmp42A=p;switch(_tmp42A){case Cyc_Absyn_Eq: _LL39A: _LL39B:
 _tmp419=_tmp42D;_tmp41A=_tmp42E;goto _LL399;case Cyc_Absyn_Neq: _LL39C: _LL39D:
 _tmp419=_tmp42E;_tmp41A=_tmp42D;goto _LL399;default: _LL39E: _LL39F:
({void*_tmp42B=0;({struct _dyneither_ptr _tmp7A8=({const char*_tmp42C="anal_test, zero-split";_tag_dyneither(_tmp42C,sizeof(char),22);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A8,_tag_dyneither(_tmp42B,sizeof(void*),0));});});}_LL399:;}
# 2006
goto _LL37B;}case 0: _LL380: _LL381:
# 2008
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp7A9=Cyc_CfFlowInfo_BottomFL();_tmp41A=_tmp7A9;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7AA=Cyc_CfFlowInfo_BottomFL();_tmp419=_tmp7AA;});}
goto _LL37B;case 1: _LL382: _LL383:
 goto _LL385;case 4: _LL384: _LL385:
 goto _LL387;default: goto _LL38A;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL386: _LL387:
 goto _LL389;}else{goto _LL38A;}case 4: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp420.f2)->tag == 0){_LL388: _LL389:
# 2015
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp7AB=Cyc_CfFlowInfo_BottomFL();_tmp41A=_tmp7AB;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7AC=Cyc_CfFlowInfo_BottomFL();_tmp419=_tmp7AC;});}
goto _LL37B;}else{goto _LL38A;}default: _LL38A: _LL38B:
 goto _LL37B;}_LL37B:;};};}
# 2026
{struct _tuple0 _tmp436=({struct _tuple0 _tmp439;({void*_tmp7AD=Cyc_Tcutil_compress((void*)_check_null(_tmp413->topt));_tmp439.f1=_tmp7AD;});({void*_tmp7AE=
Cyc_Tcutil_compress((void*)_check_null(_tmp414->topt));_tmp439.f2=_tmp7AE;});_tmp439;});
# 2026
struct _tuple0 _tmp437=_tmp436;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f1)->f1 == Cyc_Absyn_Unsigned){_LL3A1: _LL3A2:
# 2028
 goto _LL3A4;}else{switch(*((int*)_tmp437.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL3A3;else{goto _LL3A9;}case 19: goto _LL3A7;default: goto _LL3A9;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f2)->f1 == Cyc_Absyn_Unsigned){_LL3A3: _LL3A4:
 goto _LL3A6;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp437.f1)->tag == 19)goto _LL3A5;else{goto _LL3A9;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp437.f1)->tag == 19){_LL3A5: _LL3A6:
 goto _LL3A8;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp437.f2)->tag == 19){_LL3A7: _LL3A8:
 goto _LL3A0;}else{_LL3A9: _LL3AA:
 return({struct _tuple19 _tmp438;_tmp438.f1=_tmp419;_tmp438.f2=_tmp41A;_tmp438;});}}}}_LL3A0:;}
# 2036
{enum Cyc_Absyn_Primop _tmp43A=p;switch(_tmp43A){case Cyc_Absyn_Eq: _LL3AC: _LL3AD:
 goto _LL3AF;case Cyc_Absyn_Neq: _LL3AE: _LL3AF: goto _LL3B1;case Cyc_Absyn_Gt: _LL3B0: _LL3B1: goto _LL3B3;case Cyc_Absyn_Gte: _LL3B2: _LL3B3: goto _LL3B5;case Cyc_Absyn_Lt: _LL3B4: _LL3B5: goto _LL3B7;case Cyc_Absyn_Lte: _LL3B6: _LL3B7: goto _LL3AB;default: _LL3B8: _LL3B9:
 return({struct _tuple19 _tmp43B;_tmp43B.f1=_tmp419;_tmp43B.f2=_tmp41A;_tmp43B;});}_LL3AB:;}{
# 2041
struct _RegionHandle*_tmp43C=Cyc_Core_heap_region;
struct _tuple13 _tmp43D=Cyc_Relations_primop2relation(_tmp413,p,_tmp414);struct _tuple13 _tmp43E=_tmp43D;struct Cyc_Absyn_Exp*_tmp44F;enum Cyc_Relations_Relation _tmp44E;struct Cyc_Absyn_Exp*_tmp44D;_LL3BB: _tmp44F=_tmp43E.f1;_tmp44E=_tmp43E.f2;_tmp44D=_tmp43E.f3;_LL3BC:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2046
if(Cyc_Relations_exp2relnop(_tmp44F,& n1) && Cyc_Relations_exp2relnop(_tmp44D,& n2)){
# 2048
struct Cyc_List_List*_tmp43F=Cyc_Relations_add_relation(_tmp43C,n1,_tmp44E,n2,_tmp450);
# 2052
struct Cyc_List_List*_tmp440=({struct _RegionHandle*_tmp7B2=_tmp43C;union Cyc_Relations_RelnOp _tmp7B1=n2;enum Cyc_Relations_Relation _tmp7B0=Cyc_Relations_flip_relation(_tmp44E);union Cyc_Relations_RelnOp _tmp7AF=n1;Cyc_Relations_add_relation(_tmp7B2,_tmp7B1,_tmp7B0,_tmp7AF,_tmp450);});
struct _tuple19 _tmp441=({struct _tuple19 _tmp44B;_tmp44B.f1=_tmp419;_tmp44B.f2=_tmp41A;_tmp44B;});struct _tuple19 _tmp442=_tmp441;struct Cyc_Dict_Dict _tmp44A;struct Cyc_Dict_Dict _tmp449;struct Cyc_Dict_Dict _tmp448;struct Cyc_Dict_Dict _tmp447;if(((_tmp442.f1).ReachableFL).tag == 2){if(((_tmp442.f2).ReachableFL).tag == 2){_LL3BE: _tmp448=(((_tmp442.f1).ReachableFL).val).f1;_tmp447=(((_tmp442.f2).ReachableFL).val).f1;_LL3BF:
# 2055
 return({struct _tuple19 _tmp443;({union Cyc_CfFlowInfo_FlowInfo _tmp7B3=Cyc_CfFlowInfo_ReachableFL(_tmp448,_tmp43F);_tmp443.f1=_tmp7B3;});({union Cyc_CfFlowInfo_FlowInfo _tmp7B4=Cyc_CfFlowInfo_ReachableFL(_tmp447,_tmp440);_tmp443.f2=_tmp7B4;});_tmp443;});}else{_LL3C2: _tmp449=(((_tmp442.f1).ReachableFL).val).f1;_LL3C3:
# 2059
 return({struct _tuple19 _tmp445;({union Cyc_CfFlowInfo_FlowInfo _tmp7B5=Cyc_CfFlowInfo_ReachableFL(_tmp449,_tmp43F);_tmp445.f1=_tmp7B5;});_tmp445.f2=_tmp41A;_tmp445;});}}else{if(((_tmp442.f2).ReachableFL).tag == 2){_LL3C0: _tmp44A=(((_tmp442.f2).ReachableFL).val).f1;_LL3C1:
# 2057
 return({struct _tuple19 _tmp444;_tmp444.f1=_tmp419;({union Cyc_CfFlowInfo_FlowInfo _tmp7B6=Cyc_CfFlowInfo_ReachableFL(_tmp44A,_tmp440);_tmp444.f2=_tmp7B6;});_tmp444;});}else{_LL3C4: _LL3C5:
# 2061
 return({struct _tuple19 _tmp446;_tmp446.f1=_tmp419;_tmp446.f2=_tmp41A;_tmp446;});}}_LL3BD:;}else{
# 2064
return({struct _tuple19 _tmp44C;_tmp44C.f1=_tmp419;_tmp44C.f2=_tmp41A;_tmp44C;});}};};};}}_LL370:;};}
# 2070
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp454=env->fenv;
void*_tmp455=e->r;void*_tmp456=_tmp455;enum Cyc_Absyn_Primop _tmp4A3;struct Cyc_List_List*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp498;switch(*((int*)_tmp456)){case 6: _LL3C7: _tmp49A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_tmp499=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_tmp498=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp456)->f3;_LL3C8: {
# 2074
struct _tuple19 _tmp457=Cyc_NewControlFlow_anal_test(env,inflow,_tmp49A);struct _tuple19 _tmp458=_tmp457;union Cyc_CfFlowInfo_FlowInfo _tmp463;union Cyc_CfFlowInfo_FlowInfo _tmp462;_LL3D6: _tmp463=_tmp458.f1;_tmp462=_tmp458.f2;_LL3D7:;{
struct _tuple19 _tmp459=Cyc_NewControlFlow_anal_test(env,_tmp463,_tmp499);struct _tuple19 _tmp45A=_tmp459;union Cyc_CfFlowInfo_FlowInfo _tmp461;union Cyc_CfFlowInfo_FlowInfo _tmp460;_LL3D9: _tmp461=_tmp45A.f1;_tmp460=_tmp45A.f2;_LL3DA:;{
struct _tuple19 _tmp45B=Cyc_NewControlFlow_anal_test(env,_tmp462,_tmp498);struct _tuple19 _tmp45C=_tmp45B;union Cyc_CfFlowInfo_FlowInfo _tmp45F;union Cyc_CfFlowInfo_FlowInfo _tmp45E;_LL3DC: _tmp45F=_tmp45C.f1;_tmp45E=_tmp45C.f2;_LL3DD:;
return({struct _tuple19 _tmp45D;({union Cyc_CfFlowInfo_FlowInfo _tmp7B7=Cyc_CfFlowInfo_join_flow(_tmp454,_tmp461,_tmp45F);_tmp45D.f1=_tmp7B7;});({union Cyc_CfFlowInfo_FlowInfo _tmp7B8=
Cyc_CfFlowInfo_join_flow(_tmp454,_tmp460,_tmp45E);_tmp45D.f2=_tmp7B8;});_tmp45D;});};};}case 7: _LL3C9: _tmp49C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_tmp49B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_LL3CA: {
# 2080
struct _tuple19 _tmp464=Cyc_NewControlFlow_anal_test(env,inflow,_tmp49C);struct _tuple19 _tmp465=_tmp464;union Cyc_CfFlowInfo_FlowInfo _tmp46C;union Cyc_CfFlowInfo_FlowInfo _tmp46B;_LL3DF: _tmp46C=_tmp465.f1;_tmp46B=_tmp465.f2;_LL3E0:;{
struct _tuple19 _tmp466=Cyc_NewControlFlow_anal_test(env,_tmp46C,_tmp49B);struct _tuple19 _tmp467=_tmp466;union Cyc_CfFlowInfo_FlowInfo _tmp46A;union Cyc_CfFlowInfo_FlowInfo _tmp469;_LL3E2: _tmp46A=_tmp467.f1;_tmp469=_tmp467.f2;_LL3E3:;
return({struct _tuple19 _tmp468;_tmp468.f1=_tmp46A;({union Cyc_CfFlowInfo_FlowInfo _tmp7B9=Cyc_CfFlowInfo_join_flow(_tmp454,_tmp46B,_tmp469);_tmp468.f2=_tmp7B9;});_tmp468;});};}case 8: _LL3CB: _tmp49E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_tmp49D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_LL3CC: {
# 2084
struct _tuple19 _tmp46D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp49E);struct _tuple19 _tmp46E=_tmp46D;union Cyc_CfFlowInfo_FlowInfo _tmp475;union Cyc_CfFlowInfo_FlowInfo _tmp474;_LL3E5: _tmp475=_tmp46E.f1;_tmp474=_tmp46E.f2;_LL3E6:;{
struct _tuple19 _tmp46F=Cyc_NewControlFlow_anal_test(env,_tmp474,_tmp49D);struct _tuple19 _tmp470=_tmp46F;union Cyc_CfFlowInfo_FlowInfo _tmp473;union Cyc_CfFlowInfo_FlowInfo _tmp472;_LL3E8: _tmp473=_tmp470.f1;_tmp472=_tmp470.f2;_LL3E9:;
return({struct _tuple19 _tmp471;({union Cyc_CfFlowInfo_FlowInfo _tmp7BA=Cyc_CfFlowInfo_join_flow(_tmp454,_tmp475,_tmp473);_tmp471.f1=_tmp7BA;});_tmp471.f2=_tmp472;_tmp471;});};}case 9: _LL3CD: _tmp4A0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_tmp49F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_LL3CE: {
# 2088
struct _tuple16 _tmp476=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4A0);struct _tuple16 _tmp477=_tmp476;union Cyc_CfFlowInfo_FlowInfo _tmp479;void*_tmp478;_LL3EB: _tmp479=_tmp477.f1;_tmp478=_tmp477.f2;_LL3EC:;
return Cyc_NewControlFlow_anal_test(env,_tmp479,_tmp49F);}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp456)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp456)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp456)->f2)->tl == 0){_LL3CF: _tmp4A1=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp456)->f2)->hd;_LL3D0: {
# 2091
struct _tuple19 _tmp47A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A1);struct _tuple19 _tmp47B=_tmp47A;union Cyc_CfFlowInfo_FlowInfo _tmp47E;union Cyc_CfFlowInfo_FlowInfo _tmp47D;_LL3EE: _tmp47E=_tmp47B.f1;_tmp47D=_tmp47B.f2;_LL3EF:;
return({struct _tuple19 _tmp47C;_tmp47C.f1=_tmp47D;_tmp47C.f2=_tmp47E;_tmp47C;});}}else{goto _LL3D1;}}else{goto _LL3D1;}}else{_LL3D1: _tmp4A3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_tmp4A2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_LL3D2:
# 2094
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4A3,_tmp4A2);}default: _LL3D3: _LL3D4: {
# 2098
struct _tuple16 _tmp47F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp480=_tmp47F;union Cyc_CfFlowInfo_FlowInfo _tmp497;void*_tmp496;_LL3F1: _tmp497=_tmp480.f1;_tmp496=_tmp480.f2;_LL3F2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp481=_tmp497;struct Cyc_Dict_Dict _tmp495;if((_tmp481.BottomFL).tag == 1){_LL3F4: _LL3F5:
 return({struct _tuple19 _tmp482;_tmp482.f1=_tmp497;_tmp482.f2=_tmp497;_tmp482;});}else{_LL3F6: _tmp495=((_tmp481.ReachableFL).val).f1;_LL3F7: {
# 2102
struct _tuple15 _tmp483=Cyc_CfFlowInfo_unname_rval(_tmp496);struct _tuple15 _tmp484=_tmp483;void*_tmp494;struct Cyc_List_List*_tmp493;_LL3F9: _tmp494=_tmp484.f1;_tmp493=_tmp484.f2;_LL3FA:;{
void*_tmp485=_tmp494;enum Cyc_CfFlowInfo_InitLevel _tmp492;struct Cyc_Absyn_Vardecl*_tmp491;void*_tmp490;switch(*((int*)_tmp485)){case 8: _LL3FC: _tmp491=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp485)->f1;_tmp490=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp485)->f2;_LL3FD:
# 2105
({void*_tmp486=0;({struct _dyneither_ptr _tmp7BB=({const char*_tmp487="anal_test: bad namedlocation";_tag_dyneither(_tmp487,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7BB,_tag_dyneither(_tmp486,sizeof(void*),0));});});case 0: _LL3FE: _LL3FF:
 return({struct _tuple19 _tmp488;({union Cyc_CfFlowInfo_FlowInfo _tmp7BC=Cyc_CfFlowInfo_BottomFL();_tmp488.f1=_tmp7BC;});_tmp488.f2=_tmp497;_tmp488;});case 1: _LL400: _LL401:
 goto _LL403;case 5: _LL402: _LL403:
 goto _LL405;case 4: _LL404: _LL405:
 return({struct _tuple19 _tmp489;_tmp489.f1=_tmp497;({union Cyc_CfFlowInfo_FlowInfo _tmp7BD=Cyc_CfFlowInfo_BottomFL();_tmp489.f2=_tmp7BD;});_tmp489;});case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp485)->f1 == Cyc_CfFlowInfo_NoneIL){_LL406: _LL407:
 goto _LL409;}else{_LL40C: _tmp492=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp485)->f1;_LL40D:
# 2115
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp497,e,_tmp492,_tmp493);}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp485)->f1 == Cyc_CfFlowInfo_NoneIL){_LL408: _LL409:
# 2111
 goto _LL40B;}else{_LL40E: _LL40F:
# 2116
 return({struct _tuple19 _tmp48D;_tmp48D.f1=_tmp497;_tmp48D.f2=_tmp497;_tmp48D;});}case 7: _LL40A: _LL40B:
# 2113
({void*_tmp48A=0;({unsigned int _tmp7BF=e->loc;struct _dyneither_ptr _tmp7BE=({const char*_tmp48B="expression may not be initialized";_tag_dyneither(_tmp48B,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7BF,_tmp7BE,_tag_dyneither(_tmp48A,sizeof(void*),0));});});
return({struct _tuple19 _tmp48C;({union Cyc_CfFlowInfo_FlowInfo _tmp7C0=Cyc_CfFlowInfo_BottomFL();_tmp48C.f1=_tmp7C0;});({union Cyc_CfFlowInfo_FlowInfo _tmp7C1=Cyc_CfFlowInfo_BottomFL();_tmp48C.f2=_tmp7C1;});_tmp48C;});default: _LL410: _LL411:
# 2117
({void*_tmp48E=0;({struct _dyneither_ptr _tmp7C2=({const char*_tmp48F="anal_test";_tag_dyneither(_tmp48F,sizeof(char),10);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7C2,_tag_dyneither(_tmp48E,sizeof(void*),0));});});}_LL3FB:;};}}_LL3F3:;};}}_LL3C6:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2123
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2125
struct _tuple27*_tmp4A4=ckenv;unsigned int _tmp4B0;struct Cyc_NewControlFlow_AnalEnv*_tmp4AF;struct Cyc_Dict_Dict _tmp4AE;_LL413: _tmp4B0=_tmp4A4->f1;_tmp4AF=_tmp4A4->f2;_tmp4AE=_tmp4A4->f3;_LL414:;{
void*_tmp4A5=root;struct Cyc_Absyn_Vardecl*_tmp4AD;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A5)->tag == 0){_LL416: _tmp4AD=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A5)->f1;_LL417:
# 2128
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4AD->type)){
struct _tuple15 _tmp4A6=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple15 _tmp4A7=_tmp4A6;void*_tmp4AC;_LL41B: _tmp4AC=_tmp4A7.f1;_LL41C:;{
void*_tmp4A8=_tmp4AC;switch(*((int*)_tmp4A8)){case 7: _LL41E: _LL41F:
 goto _LL421;case 0: _LL420: _LL421:
 goto _LL423;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4A8)->f1 == Cyc_CfFlowInfo_NoneIL){_LL422: _LL423:
 goto _LL41D;}else{goto _LL424;}default: _LL424: _LL425:
# 2135
({struct Cyc_String_pa_PrintArg_struct _tmp4AB;_tmp4AB.tag=0;({struct _dyneither_ptr _tmp7C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4AD->name));_tmp4AB.f1=_tmp7C3;});({void*_tmp4A9[1]={& _tmp4AB};({unsigned int _tmp7C5=_tmp4B0;struct _dyneither_ptr _tmp7C4=({const char*_tmp4AA="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4AA,sizeof(char),57);});Cyc_Warn_warn(_tmp7C5,_tmp7C4,_tag_dyneither(_tmp4A9,sizeof(void*),1));});});});
goto _LL41D;}_LL41D:;};}
# 2139
goto _LL415;}else{_LL418: _LL419:
 goto _LL415;}_LL415:;};}
# 2144
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4B1=flow;struct Cyc_Dict_Dict _tmp4B7;if((_tmp4B1.BottomFL).tag == 1){_LL427: _LL428:
 return;}else{_LL429: _tmp4B7=((_tmp4B1.ReachableFL).val).f1;_LL42A:
# 2148
{struct Cyc_List_List*_tmp4B2=env->param_roots;for(0;_tmp4B2 != 0;_tmp4B2=_tmp4B2->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp7C7=env->fenv;struct Cyc_Dict_Dict _tmp7C6=_tmp4B7;Cyc_CfFlowInfo_initlevel(_tmp7C7,_tmp7C6,Cyc_CfFlowInfo_lookup_place(_tmp4B7,(struct Cyc_CfFlowInfo_Place*)_tmp4B2->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4B3=0;({unsigned int _tmp7C9=loc;struct _dyneither_ptr _tmp7C8=({const char*_tmp4B4="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4B4,sizeof(char),76);});Cyc_CfFlowInfo_aerr(_tmp7C9,_tmp7C8,_tag_dyneither(_tmp4B3,sizeof(void*),0));});});}}
# 2152
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4B5=({struct _tuple27 _tmp4B6;_tmp4B6.f1=loc;_tmp4B6.f2=env;_tmp4B6.f3=_tmp4B7;_tmp4B6;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4B5,_tmp4B7);}
# 2156
return;}_LL426:;}
# 2165
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2167
struct Cyc_List_List*_tmp4B8=0;
struct Cyc_List_List*_tmp4B9=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4BA=(struct _tuple24*)x->hd;struct _tuple24*_tmp4BB=_tmp4BA;struct Cyc_Absyn_Vardecl**_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C3;_LL42C: _tmp4C4=_tmp4BB->f1;_tmp4C3=_tmp4BB->f2;_LL42D:;
if((_tmp4C4 != 0  && _tmp4C3 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4C3->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4BC=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7CA=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4C2;_tmp4C2.tag=0;_tmp4C2.f1=*_tmp4C4;_tmp4C2;});_tmp4C1[0]=_tmp7CA;});_tmp4C1;});
struct Cyc_CfFlowInfo_Place*_tmp4BD=({struct Cyc_CfFlowInfo_Place*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->root=(void*)_tmp4BC;_tmp4C0->path=0;_tmp4C0;});
({struct Cyc_List_List*_tmp7CB=({struct Cyc_List_List*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->hd=_tmp4BD;_tmp4BE->tl=_tmp4B8;_tmp4BE;});_tmp4B8=_tmp7CB;});
({struct Cyc_List_List*_tmp7CC=({struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->hd=_tmp4C3;_tmp4BF->tl=_tmp4B9;_tmp4BF;});_tmp4B9=_tmp7CC;});}}}
# 2179
return({struct _tuple1 _tmp4C5;_tmp4C5.f1=_tmp4B8;_tmp4C5.f2=_tmp4B9;_tmp4C5;});}struct _tuple28{int f1;void*f2;};
# 2185
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2192
union Cyc_CfFlowInfo_FlowInfo _tmp4C6=flow;struct Cyc_Dict_Dict _tmp4DB;if((_tmp4C6.BottomFL).tag == 1){_LL42F: _LL430:
({void*_tmp4C7=0;({struct _dyneither_ptr _tmp7CD=({const char*_tmp4C8="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp4C8,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7CD,_tag_dyneither(_tmp4C7,sizeof(void*),0));});});}else{_LL431: _tmp4DB=((_tmp4C6.ReachableFL).val).f1;_LL432: {
# 2200
struct Cyc_Absyn_Exp*_tmp4C9=Cyc_Absyn_exp_unsigned_one;
int _tmp4CA=0;
int _tmp4CB=1;
void*_tmp4CC=Cyc_CfFlowInfo_lookup_place(_tmp4DB,place);
void*_tmp4CD=_tmp4CC;
# 2212
int varok=0;
{void*_tmp4CE=_tmp4CC;struct Cyc_Absyn_Vardecl*_tmp4D9;void*_tmp4D8;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4CE)->tag == 8){_LL434: _tmp4D9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4CE)->f1;_tmp4D8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4CE)->f2;_LL435:
# 2215
 if(vd == _tmp4D9){
_tmp4CD=_tmp4D8;
# 2218
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2220
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4C9,_tmp4CB,_tmp4CD,& _tmp4CA)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4D1;_tmp4D1.tag=0;({struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2224
Cyc_Absynpp_qvar2string(vd->name));_tmp4D1.f1=_tmp7CE;});({void*_tmp4CF[1]={& _tmp4D1};({unsigned int _tmp7D0=loc;struct _dyneither_ptr _tmp7CF=({const char*_tmp4D0="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp4D0,sizeof(char),58);});Cyc_CfFlowInfo_aerr(_tmp7D0,_tmp7CF,_tag_dyneither(_tmp4CF,sizeof(void*),1));});});});}else{
# 2227
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4DB,_tmp4CD)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4D4;_tmp4D4.tag=0;({struct _dyneither_ptr _tmp7D1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2230
Cyc_Absynpp_qvar2string(vd->name));_tmp4D4.f1=_tmp7D1;});({void*_tmp4D2[1]={& _tmp4D4};({unsigned int _tmp7D3=loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp4D3="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp4D3,sizeof(char),84);});Cyc_CfFlowInfo_aerr(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp4D2,sizeof(void*),1));});});});else{
# 2232
varok=1;}}}else{
# 2235
varok=1;}}else{
# 2238
goto _LL437;}
goto _LL433;}else{_LL436: _LL437:
# 2242
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0;({struct _dyneither_ptr _tmp7D4=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2247
Cyc_Absynpp_qvar2string(vd->name));_tmp4D7.f1=_tmp7D4;});({void*_tmp4D5[1]={& _tmp4D7};({unsigned int _tmp7D6=loc;struct _dyneither_ptr _tmp7D5=({const char*_tmp4D6="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp4D6,sizeof(char),85);});Cyc_CfFlowInfo_aerr(_tmp7D6,_tmp7D5,_tag_dyneither(_tmp4D5,sizeof(void*),1));});});});}
goto _LL433;}_LL433:;}
# 2254
return({struct _tuple28 _tmp4DA;_tmp4DA.f1=varok;_tmp4DA.f2=_tmp4CD;_tmp4DA;});}}_LL42E:;}
# 2260
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp4DC=p;void*_tmp4E4;struct Cyc_List_List*_tmp4E3;_LL439: _tmp4E4=_tmp4DC->root;_tmp4E3=_tmp4DC->path;_LL43A:;
if(_tmp4E3 != 0)
({void*_tmp4DD=0;({struct _dyneither_ptr _tmp7D7=({const char*_tmp4DE="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp4DE,sizeof(char),53);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7D7,_tag_dyneither(_tmp4DD,sizeof(void*),0));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp4DF=_tmp4E4;struct Cyc_Absyn_Vardecl*_tmp4E2;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4DF)->tag == 0){_LL43C: _tmp4E2=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4DF)->f1;_LL43D:
 return _tmp4E2;}else{_LL43E: _LL43F:
({void*_tmp4E0=0;({struct _dyneither_ptr _tmp7D8=({const char*_tmp4E1="unconsume_params: root is not a varroot";_tag_dyneither(_tmp4E1,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7D8,_tag_dyneither(_tmp4E0,sizeof(void*),0));});});}_LL43B:;};}
# 2279 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2287
{union Cyc_CfFlowInfo_FlowInfo _tmp4E5=flow;struct Cyc_Dict_Dict _tmp4FE;if((_tmp4E5.BottomFL).tag == 1){_LL441: _LL442:
 return flow;}else{_LL443: _tmp4FE=((_tmp4E5.ReachableFL).val).f1;_LL444: {
# 2294
struct _tuple18 _tmp4E6=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp4E7=_tmp4E6;union Cyc_CfFlowInfo_FlowInfo _tmp4FD;union Cyc_CfFlowInfo_AbsLVal _tmp4FC;_LL446: _tmp4FD=_tmp4E7.f1;_tmp4FC=_tmp4E7.f2;_LL447:;
# 2297
{union Cyc_CfFlowInfo_AbsLVal _tmp4E8=_tmp4FC;struct Cyc_CfFlowInfo_Place*_tmp4FB;if((_tmp4E8.PlaceL).tag == 1){_LL449: _tmp4FB=(_tmp4E8.PlaceL).val;_LL44A: {
# 2301
void*_tmp4E9=Cyc_CfFlowInfo_lookup_place(_tmp4FE,_tmp4FB);
{void*_tmp4EA=_tmp4E9;struct Cyc_Absyn_Vardecl*_tmp4FA;void*_tmp4F9;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4EA)->tag == 8){_LL44E: _tmp4FA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4EA)->f1;_tmp4F9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4EA)->f2;_LL44F: {
# 2307
void*new_rval;
if(_tmp4FA == vd){
# 2311
if(varok){
# 2313
({void*_tmp7D9=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp4F9);_tmp4E9=_tmp7D9;});
# 2318
if(ropt != 0){
# 2324
struct _tuple16 _tmp4EB=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7DB=env->fenv;struct _tuple16 _tmp7DA=({struct _tuple16 _tmp4EF;_tmp4EF.f1=_tmp4FD;_tmp4EF.f2=_tmp4E9;_tmp4EF;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp7DB,_tmp7DA,({struct _tuple16 _tmp4F0;_tmp4F0.f1=_tmp4FD;_tmp4F0.f2=ropt;_tmp4F0;}));});
# 2324
struct _tuple16 _tmp4EC=_tmp4EB;union Cyc_CfFlowInfo_FlowInfo _tmp4EE;void*_tmp4ED;_LL453: _tmp4EE=_tmp4EC.f1;_tmp4ED=_tmp4EC.f2;_LL454:;
# 2328
_tmp4FD=_tmp4EE;new_rval=_tmp4ED;}else{
# 2333
new_rval=_tmp4E9;}}else{
# 2336
new_rval=_tmp4F9;}
# 2338
{union Cyc_CfFlowInfo_FlowInfo _tmp4F1=_tmp4FD;struct Cyc_Dict_Dict _tmp4F5;struct Cyc_List_List*_tmp4F4;if((_tmp4F1.ReachableFL).tag == 2){_LL456: _tmp4F5=((_tmp4F1.ReachableFL).val).f1;_tmp4F4=((_tmp4F1.ReachableFL).val).f2;_LL457:
# 2340
({union Cyc_CfFlowInfo_FlowInfo _tmp7DD=({struct Cyc_Dict_Dict _tmp7DC=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp4F5,_tmp4FB,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp7DC,_tmp4F4);});flow=_tmp7DD;});
# 2344
goto _LL455;}else{_LL458: _LL459:
# 2346
({void*_tmp4F2=0;({struct _dyneither_ptr _tmp7DE=({const char*_tmp4F3="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp4F3,sizeof(char),42);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7DE,_tag_dyneither(_tmp4F2,sizeof(void*),0));});});}_LL455:;}
# 2348
goto _LL44D;}else{
# 2350
goto _LL451;}
goto _LL44D;}}else{_LL450: _LL451:
# 2357
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0;({struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));_tmp4F8.f1=_tmp7DF;});({void*_tmp4F6[1]={& _tmp4F8};({unsigned int _tmp7E1=loc;struct _dyneither_ptr _tmp7E0=({const char*_tmp4F7="aliased expression %s was overwritten";_tag_dyneither(_tmp4F7,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp7E1,_tmp7E0,_tag_dyneither(_tmp4F6,sizeof(void*),1));});});});
# 2374 "new_control_flow.cyc"
goto _LL44D;}_LL44D:;}
# 2376
goto _LL448;}}else{_LL44B: _LL44C:
# 2382
 goto _LL448;}_LL448:;}
# 2384
goto _LL440;}}_LL440:;}
# 2386
return flow;}
# 2399 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2405
{union Cyc_CfFlowInfo_FlowInfo _tmp4FF=flow;if((_tmp4FF.BottomFL).tag == 1){_LL45B: _LL45C:
 return flow;}else{_LL45D: _LL45E:
 goto _LL45A;}_LL45A:;}{
# 2409
int _tmp500=unconsume_exps != 0;
struct Cyc_List_List*_tmp501=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp502=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp503=_tmp501;for(0;_tmp503 != 0;
(_tmp503=_tmp503->tl,
_tmp502 != 0?_tmp502=_tmp502->tl: 0)){
# 2418
struct Cyc_Absyn_Vardecl*_tmp504=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp503->hd);
struct _tuple28 _tmp505=
is_region_open?({struct _tuple28 _tmp509;_tmp509.f1=1;_tmp509.f2=0;_tmp509;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp503->hd,_tmp500,_tmp504,flow,loc);
# 2419
struct _tuple28 _tmp506=_tmp505;int _tmp508;void*_tmp507;_LL460: _tmp508=_tmp506.f1;_tmp507=_tmp506.f2;_LL461:;
# 2425
if(_tmp500)
({union Cyc_CfFlowInfo_FlowInfo _tmp7E2=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp502))->hd,_tmp504,_tmp508,_tmp507,flow,loc);flow=_tmp7E2;});}}
# 2428
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2432
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2434
struct Cyc_CfFlowInfo_FlowEnv*_tmp50A=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp50B=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp50C=_tmp50B;struct Cyc_Core_Opt*_tmp520;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Stmt*_tmp51E;unsigned int _tmp51D;_LL463: _tmp520=_tmp50C->pat_vars;_tmp51F=_tmp50C->where_clause;_tmp51E=_tmp50C->body;_tmp51D=_tmp50C->loc;_LL464:;{
struct _tuple1 _tmp50D=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp520))->v);struct _tuple1 _tmp50E=_tmp50D;struct Cyc_List_List*_tmp51C;struct Cyc_List_List*_tmp51B;_LL466: _tmp51C=_tmp50E.f1;_tmp51B=_tmp50E.f2;_LL467:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp520->v,_tmp51C != 0,_tmp51D,exp_loc);
# 2442
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp50F=env->unique_pat_vars;
# 2445
if(Cyc_Tcpat_has_vars(_tmp520))
({struct Cyc_List_List*_tmp7E4=({struct Cyc_List_List*_tmp510=_cycalloc(sizeof(*_tmp510));({struct _tuple29*_tmp7E3=({struct _tuple29*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->f1=0;_tmp511->f2=_tmp51E;_tmp511->f3=_tmp51C;_tmp511->f4=_tmp51B;_tmp511;});_tmp510->hd=_tmp7E3;});_tmp510->tl=_tmp50F;_tmp510;});env->unique_pat_vars=_tmp7E4;});
# 2449
if(_tmp51F != 0){
struct Cyc_Absyn_Exp*wexp=_tmp51F;
struct _tuple19 _tmp512=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp513=_tmp512;union Cyc_CfFlowInfo_FlowInfo _tmp515;union Cyc_CfFlowInfo_FlowInfo _tmp514;_LL469: _tmp515=_tmp513.f1;_tmp514=_tmp513.f2;_LL46A:;
inflow=_tmp514;
({union Cyc_CfFlowInfo_FlowInfo _tmp7E5=Cyc_NewControlFlow_anal_stmt(env,_tmp515,_tmp51E,0);clause_outflow=_tmp7E5;});}else{
# 2455
({union Cyc_CfFlowInfo_FlowInfo _tmp7E6=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp51E,0);clause_outflow=_tmp7E6;});}
# 2457
env->unique_pat_vars=_tmp50F;{
union Cyc_CfFlowInfo_FlowInfo _tmp516=clause_outflow;if((_tmp516.BottomFL).tag == 1){_LL46C: _LL46D:
 goto _LL46B;}else{_LL46E: _LL46F:
# 2462
({union Cyc_CfFlowInfo_FlowInfo _tmp7E7=Cyc_NewControlFlow_unconsume_params(env,_tmp51C,_tmp51B,0,clause_outflow,_tmp51D);clause_outflow=_tmp7E7;});
# 2464
if(scs->tl == 0)
return clause_outflow;else{
# 2469
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp517=0;({unsigned int _tmp7E9=_tmp51E->loc;struct _dyneither_ptr _tmp7E8=({const char*_tmp518="switch clause may implicitly fallthru";_tag_dyneither(_tmp518,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp7E9,_tmp7E8,_tag_dyneither(_tmp517,sizeof(void*),0));});});else{
# 2472
({void*_tmp519=0;({unsigned int _tmp7EB=_tmp51E->loc;struct _dyneither_ptr _tmp7EA=({const char*_tmp51A="switch clause may implicitly fallthru";_tag_dyneither(_tmp51A,sizeof(char),38);});Cyc_Warn_warn(_tmp7EB,_tmp7EA,_tag_dyneither(_tmp519,sizeof(void*),0));});});}
# 2474
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2476
goto _LL46B;}_LL46B:;};};};}
# 2479
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2484
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2486
struct _tuple30*_tmp521=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp52D;struct Cyc_Dict_Dict _tmp52C;unsigned int _tmp52B;_LL471: _tmp52D=_tmp521->f1;_tmp52C=_tmp521->f2;_tmp52B=_tmp521->f3;_LL472:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2489
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp52A;_tmp52A.tag=0;_tmp52A.f1=vd;_tmp52A;});
struct _tuple15 _tmp522=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp7EC=_tmp52C;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp7EC,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529[0]=vdroot;_tmp529;}));}));struct _tuple15 _tmp523=_tmp522;void*_tmp528;_LL474: _tmp528=_tmp523.f1;_LL475:;{
void*_tmp524=_tmp528;switch(*((int*)_tmp524)){case 7: _LL477: _LL478:
 goto _LL47A;case 0: _LL479: _LL47A:
 goto _LL47C;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp524)->f1 == Cyc_CfFlowInfo_NoneIL){_LL47B: _LL47C:
 goto _LL476;}else{goto _LL47D;}default: _LL47D: _LL47E:
# 2496
({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0;({struct _dyneither_ptr _tmp7ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp527.f1=_tmp7ED;});({void*_tmp525[1]={& _tmp527};({unsigned int _tmp7EF=_tmp52B;struct _dyneither_ptr _tmp7EE=({const char*_tmp526="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp526,sizeof(char),68);});Cyc_Warn_warn(_tmp7EF,_tmp7EE,_tag_dyneither(_tmp525,sizeof(void*),1));});});});
# 2498
goto _LL476;}_LL476:;};}}
# 2503
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp52E=inflow;struct Cyc_Dict_Dict _tmp53B;if((_tmp52E.ReachableFL).tag == 2){_LL480: _tmp53B=((_tmp52E.ReachableFL).val).f1;_LL481: {
# 2506
struct _tuple30 _tmp52F=({struct _tuple30 _tmp53A;_tmp53A.f1=env;_tmp53A.f2=_tmp53B;_tmp53A.f3=decl->loc;_tmp53A;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp530=env->fenv;
{void*_tmp531=decl->r;void*_tmp532=_tmp531;struct Cyc_List_List*_tmp539;struct Cyc_List_List*_tmp538;struct Cyc_Absyn_Vardecl*_tmp537;switch(*((int*)_tmp532)){case 0: _LL485: _tmp537=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp532)->f1;_LL486:
# 2510
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp52F,_tmp537);
goto _LL484;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp532)->f2 != 0){_LL487: _tmp538=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp532)->f2)->v;_LL488: {
# 2513
struct _tuple1 _tmp533=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp538);struct _tuple1 _tmp534=_tmp533;struct Cyc_List_List*_tmp536;_LL48E: _tmp536=_tmp534.f1;_LL48F:;{
struct Cyc_List_List*_tmp535=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp536);
_tmp539=_tmp535;goto _LL48A;};}}else{goto _LL48B;}case 3: _LL489: _tmp539=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp532)->f1;_LL48A:
# 2517
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp52F,_tmp539);
goto _LL484;default: _LL48B: _LL48C:
 goto _LL484;}_LL484:;}
# 2521
goto _LL47F;}}else{_LL482: _LL483:
 goto _LL47F;}_LL47F:;}
# 2524
return;}
# 2530
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2534
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp53C=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp53D=_tmp53C;int _tmp541;struct Cyc_Absyn_Stmt*_tmp540;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp53E;_LL491: _tmp541=_tmp53D->f1;_tmp540=_tmp53D->f2;_tmp53F=_tmp53D->f3;_tmp53E=_tmp53D->f4;_LL492:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7F0=Cyc_NewControlFlow_unconsume_params(env,_tmp53F,_tmp53E,_tmp541,inflow,dest->loc);inflow=_tmp7F0;});}}
# 2539
return inflow;}
# 2545
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2547
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp542=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp543=_tmp542;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5E3;union Cyc_CfFlowInfo_FlowInfo*_tmp5E2;_LL494: _tmp5E3=_tmp543.f1;_tmp5E2=_tmp543.f2;_LL495:;
inflow=*_tmp5E2;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp544=env->fenv;
# 2554
void*_tmp545=s->r;void*_tmp546=_tmp545;struct Cyc_Absyn_Stmt*_tmp5E1;struct Cyc_Absyn_Decl*_tmp5E0;struct Cyc_Absyn_Stmt*_tmp5DF;struct Cyc_Absyn_Vardecl*_tmp5DE;struct Cyc_Absyn_Exp*_tmp5DD;unsigned int _tmp5DC;struct Cyc_Absyn_Stmt*_tmp5DB;struct Cyc_List_List*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5D9;unsigned int _tmp5D8;struct Cyc_Absyn_Stmt*_tmp5D7;struct Cyc_Absyn_Stmt*_tmp5D6;struct Cyc_List_List*_tmp5D5;void*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_List_List*_tmp5D2;void*_tmp5D1;struct Cyc_List_List*_tmp5D0;struct Cyc_Absyn_Switch_clause*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;struct Cyc_Absyn_Exp*_tmp5CD;struct Cyc_Absyn_Stmt*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CB;struct Cyc_Absyn_Stmt*_tmp5CA;struct Cyc_Absyn_Stmt*_tmp5C9;struct Cyc_Absyn_Stmt*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Stmt*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Stmt*_tmp5C4;struct Cyc_Absyn_Stmt*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_Stmt*_tmp5C1;struct Cyc_Absyn_Stmt*_tmp5C0;struct Cyc_Absyn_Stmt*_tmp5BF;struct Cyc_Absyn_Stmt*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BC;switch(*((int*)_tmp546)){case 0: _LL497: _LL498:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp546)->f1 == 0){_LL499: _LL49A:
# 2558
 if(env->noreturn)
({void*_tmp547=0;({unsigned int _tmp7F2=s->loc;struct _dyneither_ptr _tmp7F1=({const char*_tmp548="`noreturn' function might return";_tag_dyneither(_tmp548,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp7F2,_tmp7F1,_tag_dyneither(_tmp547,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL49B: _tmp5BC=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_LL49C:
# 2564
 if(env->noreturn)
({void*_tmp549=0;({unsigned int _tmp7F4=s->loc;struct _dyneither_ptr _tmp7F3=({const char*_tmp54A="`noreturn' function might return";_tag_dyneither(_tmp54A,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp7F4,_tmp7F3,_tag_dyneither(_tmp549,sizeof(void*),0));});});{
struct _tuple16 _tmp54B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5BC));struct _tuple16 _tmp54C=_tmp54B;union Cyc_CfFlowInfo_FlowInfo _tmp54E;void*_tmp54D;_LL4C0: _tmp54E=_tmp54C.f1;_tmp54D=_tmp54C.f2;_LL4C1:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7F5=Cyc_NewControlFlow_use_Rval(env,_tmp5BC->loc,_tmp54E,_tmp54D);_tmp54E=_tmp7F5;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp54E);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp54E,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL49D: _tmp5BD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_LL49E: {
# 2573
struct _tuple17*_tmp54F=rval_opt;void**_tmp555;int _tmp554;if(_tmp54F != 0){_LL4C3: _tmp555=(void**)& _tmp54F->f1;_tmp554=_tmp54F->f2;_LL4C4: {
# 2575
struct _tuple16 _tmp550=Cyc_NewControlFlow_anal_Rexp(env,_tmp554,inflow,_tmp5BD);struct _tuple16 _tmp551=_tmp550;union Cyc_CfFlowInfo_FlowInfo _tmp553;void*_tmp552;_LL4C8: _tmp553=_tmp551.f1;_tmp552=_tmp551.f2;_LL4C9:;
*_tmp555=_tmp552;
return _tmp553;}}else{_LL4C5: _LL4C6:
# 2579
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5BD)).f1;}_LL4C2:;}case 2: _LL49F: _tmp5BF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5BE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_LL4A0:
# 2583
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp7F8=env;union Cyc_CfFlowInfo_FlowInfo _tmp7F7=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5BF,0);struct Cyc_Absyn_Stmt*_tmp7F6=_tmp5BE;Cyc_NewControlFlow_anal_stmt(_tmp7F8,_tmp7F7,_tmp7F6,rval_opt);});case 4: _LL4A1: _tmp5C2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5C1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_tmp5C0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp546)->f3;_LL4A2: {
# 2586
struct _tuple19 _tmp556=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5C2);struct _tuple19 _tmp557=_tmp556;union Cyc_CfFlowInfo_FlowInfo _tmp55B;union Cyc_CfFlowInfo_FlowInfo _tmp55A;_LL4CB: _tmp55B=_tmp557.f1;_tmp55A=_tmp557.f2;_LL4CC:;{
# 2593
union Cyc_CfFlowInfo_FlowInfo _tmp558=Cyc_NewControlFlow_anal_stmt(env,_tmp55A,_tmp5C0,0);
union Cyc_CfFlowInfo_FlowInfo _tmp559=Cyc_NewControlFlow_anal_stmt(env,_tmp55B,_tmp5C1,0);
return Cyc_CfFlowInfo_join_flow(_tmp544,_tmp559,_tmp558);};}case 5: _LL4A3: _tmp5C5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp546)->f1).f1;_tmp5C4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp546)->f1).f2;_tmp5C3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_LL4A4: {
# 2601
struct _tuple20 _tmp55C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5C4);struct _tuple20 _tmp55D=_tmp55C;union Cyc_CfFlowInfo_FlowInfo*_tmp564;_LL4CE: _tmp564=_tmp55D.f2;_LL4CF:;{
union Cyc_CfFlowInfo_FlowInfo _tmp55E=*_tmp564;
struct _tuple19 _tmp55F=Cyc_NewControlFlow_anal_test(env,_tmp55E,_tmp5C5);struct _tuple19 _tmp560=_tmp55F;union Cyc_CfFlowInfo_FlowInfo _tmp563;union Cyc_CfFlowInfo_FlowInfo _tmp562;_LL4D1: _tmp563=_tmp560.f1;_tmp562=_tmp560.f2;_LL4D2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp561=Cyc_NewControlFlow_anal_stmt(env,_tmp563,_tmp5C3,0);
Cyc_NewControlFlow_update_flow(env,_tmp5C4,_tmp561);
return _tmp562;};};}case 14: _LL4A5: _tmp5C8=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5C7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp546)->f2).f1;_tmp5C6=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp546)->f2).f2;_LL4A6: {
# 2611
union Cyc_CfFlowInfo_FlowInfo _tmp565=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5C8,0);
struct _tuple20 _tmp566=Cyc_NewControlFlow_pre_stmt_check(env,_tmp565,_tmp5C6);struct _tuple20 _tmp567=_tmp566;union Cyc_CfFlowInfo_FlowInfo*_tmp56D;_LL4D4: _tmp56D=_tmp567.f2;_LL4D5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp568=*_tmp56D;
struct _tuple19 _tmp569=Cyc_NewControlFlow_anal_test(env,_tmp568,_tmp5C7);struct _tuple19 _tmp56A=_tmp569;union Cyc_CfFlowInfo_FlowInfo _tmp56C;union Cyc_CfFlowInfo_FlowInfo _tmp56B;_LL4D7: _tmp56C=_tmp56A.f1;_tmp56B=_tmp56A.f2;_LL4D8:;
Cyc_NewControlFlow_update_flow(env,_tmp5C8,_tmp56C);
return _tmp56B;};}case 9: _LL4A7: _tmp5CE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5CD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546)->f2).f1;_tmp5CC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546)->f2).f2;_tmp5CB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546)->f3).f1;_tmp5CA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546)->f3).f2;_tmp5C9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546)->f4;_LL4A8: {
# 2620
union Cyc_CfFlowInfo_FlowInfo _tmp56E=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5CE)).f1;
struct _tuple20 _tmp56F=Cyc_NewControlFlow_pre_stmt_check(env,_tmp56E,_tmp5CC);struct _tuple20 _tmp570=_tmp56F;union Cyc_CfFlowInfo_FlowInfo*_tmp57C;_LL4DA: _tmp57C=_tmp570.f2;_LL4DB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp571=*_tmp57C;
struct _tuple19 _tmp572=Cyc_NewControlFlow_anal_test(env,_tmp571,_tmp5CD);struct _tuple19 _tmp573=_tmp572;union Cyc_CfFlowInfo_FlowInfo _tmp57B;union Cyc_CfFlowInfo_FlowInfo _tmp57A;_LL4DD: _tmp57B=_tmp573.f1;_tmp57A=_tmp573.f2;_LL4DE:;{
union Cyc_CfFlowInfo_FlowInfo _tmp574=Cyc_NewControlFlow_anal_stmt(env,_tmp57B,_tmp5C9,0);
struct _tuple20 _tmp575=Cyc_NewControlFlow_pre_stmt_check(env,_tmp574,_tmp5CA);struct _tuple20 _tmp576=_tmp575;union Cyc_CfFlowInfo_FlowInfo*_tmp579;_LL4E0: _tmp579=_tmp576.f2;_LL4E1:;{
union Cyc_CfFlowInfo_FlowInfo _tmp577=*_tmp579;
union Cyc_CfFlowInfo_FlowInfo _tmp578=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp577,_tmp5CB)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5CC,_tmp578);
return _tmp57A;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp546)->f2 != 0){_LL4A9: _tmp5D0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5CF=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_LL4AA: {
# 2632
struct _tuple23 _tmp57D=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp5D0,1,1);struct _tuple23 _tmp57E=_tmp57D;union Cyc_CfFlowInfo_FlowInfo _tmp585;struct Cyc_List_List*_tmp584;_LL4E3: _tmp585=_tmp57E.f1;_tmp584=_tmp57E.f2;_LL4E4:;
# 2634
({union Cyc_CfFlowInfo_FlowInfo _tmp7F9=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp5CF->body);inflow=_tmp7F9;});{
# 2636
struct Cyc_List_List*_tmp57F=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CF->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp7FA=Cyc_NewControlFlow_add_vars(_tmp544,_tmp585,_tmp57F,_tmp544->unknown_all,s->loc,0);_tmp585=_tmp7FA;});
# 2639
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CF->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp580=(struct _tuple24*)x->hd;struct _tuple24*_tmp581=_tmp580;struct Cyc_Absyn_Vardecl**_tmp583;struct Cyc_Absyn_Exp*_tmp582;_LL4E6: _tmp583=_tmp581->f1;_tmp582=_tmp581->f2;_LL4E7:;
if(_tmp583 != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp801=({struct Cyc_CfFlowInfo_FlowEnv*_tmp800=_tmp544;struct Cyc_NewControlFlow_AnalEnv*_tmp7FF=env;union Cyc_CfFlowInfo_FlowInfo _tmp7FE=_tmp585;struct Cyc_Absyn_Vardecl*_tmp7FD=*_tmp583;struct Cyc_Absyn_Exp*_tmp7FC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5D0))->hd;void*_tmp7FB=(void*)((struct Cyc_List_List*)_check_null(_tmp584))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp800,_tmp7FF,_tmp7FE,_tmp7FD,_tmp7FC,_tmp7FB,s->loc);});_tmp585=_tmp801;});
_tmp584=_tmp584->tl;
_tmp5D0=_tmp5D0->tl;}}}
# 2647
Cyc_NewControlFlow_update_flow(env,_tmp5CF->body,_tmp585);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL4BD: _LL4BE:
# 2814
({void*_tmp5BA=0;({struct _dyneither_ptr _tmp802=({const char*_tmp5BB="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5BB,sizeof(char),54);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp802,_tag_dyneither(_tmp5BA,sizeof(void*),0));});});}case 6: _LL4AB: _LL4AC:
# 2653
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp586=inflow;if((_tmp586.ReachableFL).tag == 2){_LL4E9: _LL4EA:
# 2656
{void*_tmp587=Cyc_Tcutil_compress(env->return_type);void*_tmp588=_tmp587;switch(*((int*)_tmp588)){case 0: _LL4EE: _LL4EF:
 goto _LL4ED;case 7: _LL4F0: _LL4F1:
 goto _LL4F3;case 6: _LL4F2: _LL4F3:
# 2660
({void*_tmp589=0;({unsigned int _tmp804=s->loc;struct _dyneither_ptr _tmp803=({const char*_tmp58A="break may cause function not to return a value";_tag_dyneither(_tmp58A,sizeof(char),47);});Cyc_Warn_warn(_tmp804,_tmp803,_tag_dyneither(_tmp589,sizeof(void*),0));});});
goto _LL4ED;default: _LL4F4: _LL4F5:
# 2663
({void*_tmp58B=0;({unsigned int _tmp806=s->loc;struct _dyneither_ptr _tmp805=({const char*_tmp58C="break may cause function not to return a value";_tag_dyneither(_tmp58C,sizeof(char),47);});Cyc_CfFlowInfo_aerr(_tmp806,_tmp805,_tag_dyneither(_tmp58B,sizeof(void*),0));});});
goto _LL4ED;}_LL4ED:;}
# 2666
goto _LL4E8;}else{_LL4EB: _LL4EC:
 goto _LL4E8;}_LL4E8:;}
# 2669
if(env->noreturn)
({void*_tmp58D=0;({unsigned int _tmp808=s->loc;struct _dyneither_ptr _tmp807=({const char*_tmp58E="`noreturn' function might return";_tag_dyneither(_tmp58E,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp808,_tmp807,_tag_dyneither(_tmp58D,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2675
goto _LL4AE;case 7: _LL4AD: _LL4AE:
 goto _LL4B0;case 8: _LL4AF: _LL4B0: {
# 2679
struct Cyc_Absyn_Stmt*_tmp58F=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp58F == 0)
({void*_tmp590=0;({unsigned int _tmp80A=s->loc;struct _dyneither_ptr _tmp809=({const char*_tmp591="jump has no target (should be impossible)";_tag_dyneither(_tmp591,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp80A,_tmp809,_tag_dyneither(_tmp590,sizeof(void*),0));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp80B=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp58F));inflow=_tmp80B;});
# 2684
Cyc_NewControlFlow_update_flow(env,_tmp58F,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _LL4B1: _tmp5D3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5D2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_tmp5D1=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp546)->f3;_LL4B2:
# 2690
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp5D2,_tmp5D3->loc);case 15: _LL4B3: _tmp5D6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5D5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_tmp5D4=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp546)->f3;_LL4B4: {
# 2695
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5D6,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2703
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2706
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5D5,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp592=scs_outflow;if((_tmp592.BottomFL).tag == 1){_LL4F7: _LL4F8:
 goto _LL4F6;}else{_LL4F9: _LL4FA:
({void*_tmp593=0;({unsigned int _tmp80D=s->loc;struct _dyneither_ptr _tmp80C=({const char*_tmp594="last catch clause may implicitly fallthru";_tag_dyneither(_tmp594,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp80D,_tmp80C,_tag_dyneither(_tmp593,sizeof(void*),0));});});}_LL4F6:;}
# 2712
outflow=s1_outflow;
# 2714
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->r)->f2 != 0){_LL4B5: _tmp5DA=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->r)->f2)->v;_tmp5D9=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->r)->f3;_tmp5D8=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->loc;_tmp5D7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_LL4B6: {
# 2724
struct _tuple1 _tmp595=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp5DA);struct _tuple1 _tmp596=_tmp595;struct Cyc_List_List*_tmp59B;struct Cyc_List_List*_tmp59A;_LL4FC: _tmp59B=_tmp596.f1;_tmp59A=_tmp596.f2;_LL4FD:;
({union Cyc_CfFlowInfo_FlowInfo _tmp80E=Cyc_NewControlFlow_initialize_pat_vars(_tmp544,env,inflow,_tmp5DA,_tmp59B != 0,_tmp5D8,_tmp5D9->loc);inflow=_tmp80E;});{
struct Cyc_List_List*_tmp597=env->unique_pat_vars;
# 2728
({struct Cyc_List_List*_tmp810=({struct Cyc_List_List*_tmp598=_cycalloc(sizeof(*_tmp598));({struct _tuple29*_tmp80F=({struct _tuple29*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->f1=0;_tmp599->f2=s;_tmp599->f3=_tmp59B;_tmp599->f4=_tmp59A;_tmp599;});_tmp598->hd=_tmp80F;});_tmp598->tl=_tmp597;_tmp598;});env->unique_pat_vars=_tmp810;});
# 2733
({union Cyc_CfFlowInfo_FlowInfo _tmp811=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5D7,rval_opt);inflow=_tmp811;});
env->unique_pat_vars=_tmp597;
# 2737
({union Cyc_CfFlowInfo_FlowInfo _tmp812=Cyc_NewControlFlow_unconsume_params(env,_tmp59B,_tmp59A,0,inflow,_tmp5D8);inflow=_tmp812;});
# 2741
return inflow;};}}else{goto _LL4B9;}case 4: _LL4B7: _tmp5DE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->r)->f2;_tmp5DD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->r)->f3;_tmp5DC=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1)->loc;_tmp5DB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;if(_tmp5DD != 0){_LL4B8: {
# 2753
struct Cyc_List_List l=({struct Cyc_List_List _tmp5B9;_tmp5B9.hd=_tmp5DD;_tmp5B9.tl=0;_tmp5B9;});
union Cyc_CfFlowInfo_FlowInfo _tmp59C=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp59D=Cyc_NewControlFlow_anal_Lexp(env,_tmp59C,0,0,_tmp5DD);struct _tuple18 _tmp59E=_tmp59D;union Cyc_CfFlowInfo_AbsLVal _tmp5B8;_LL4FF: _tmp5B8=_tmp59E.f2;_LL500:;{
struct _tuple16 _tmp59F=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp59C,_tmp5DD);struct _tuple16 _tmp5A0=_tmp59F;union Cyc_CfFlowInfo_FlowInfo _tmp5B7;_LL502: _tmp5B7=_tmp5A0.f1;_LL503:;{
struct Cyc_List_List*_tmp5A1=0;
struct Cyc_List_List*_tmp5A2=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5A3=_tmp5B7;struct Cyc_Dict_Dict _tmp5B1;struct Cyc_List_List*_tmp5B0;if((_tmp5A3.ReachableFL).tag == 2){_LL505: _tmp5B1=((_tmp5A3.ReachableFL).val).f1;_tmp5B0=((_tmp5A3.ReachableFL).val).f2;_LL506:
# 2761
{union Cyc_CfFlowInfo_AbsLVal _tmp5A4=_tmp5B8;struct Cyc_CfFlowInfo_Place*_tmp5AF;if((_tmp5A4.PlaceL).tag == 1){_LL50A: _tmp5AF=(_tmp5A4.PlaceL).val;_LL50B: {
# 2765
void*_tmp5A5=Cyc_CfFlowInfo_lookup_place(_tmp5B1,_tmp5AF);
({void*_tmp814=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp813=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5A7;_tmp5A7.tag=8;_tmp5A7.f1=_tmp5DE;_tmp5A7.f2=_tmp5A5;_tmp5A7;});_tmp5A6[0]=_tmp813;});_tmp5A6;});_tmp5A5=_tmp814;});
({struct Cyc_Dict_Dict _tmp815=Cyc_CfFlowInfo_assign_place(_tmp544,_tmp5DD->loc,_tmp5B1,_tmp5AF,_tmp5A5);_tmp5B1=_tmp815;});
({union Cyc_CfFlowInfo_FlowInfo _tmp816=Cyc_CfFlowInfo_ReachableFL(_tmp5B1,_tmp5B0);_tmp5B7=_tmp816;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5A8=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp817=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5AE;_tmp5AE.tag=0;_tmp5AE.f1=_tmp5DE;_tmp5AE;});_tmp5AD[0]=_tmp817;});_tmp5AD;});
struct Cyc_CfFlowInfo_Place*_tmp5A9=({struct Cyc_CfFlowInfo_Place*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->root=(void*)_tmp5A8;_tmp5AC->path=0;_tmp5AC;});
({struct Cyc_List_List*_tmp818=({struct Cyc_List_List*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->hd=_tmp5A9;_tmp5AA->tl=_tmp5A1;_tmp5AA;});_tmp5A1=_tmp818;});
({struct Cyc_List_List*_tmp819=({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->hd=_tmp5DD;_tmp5AB->tl=_tmp5A2;_tmp5AB;});_tmp5A2=_tmp819;});
goto _LL509;};}}else{_LL50C: _LL50D:
# 2780
 goto _LL509;}_LL509:;}
# 2782
goto _LL504;}else{_LL507: _LL508:
# 2784
 goto _LL504;}_LL504:;}{
# 2787
struct Cyc_List_List _tmp5B2=({struct Cyc_List_List _tmp5B6;_tmp5B6.hd=_tmp5DE;_tmp5B6.tl=0;_tmp5B6;});
({union Cyc_CfFlowInfo_FlowInfo _tmp81A=Cyc_NewControlFlow_add_vars(_tmp544,_tmp5B7,& _tmp5B2,_tmp544->unknown_all,_tmp5DC,0);_tmp5B7=_tmp81A;});{
# 2791
struct Cyc_List_List*_tmp5B3=env->unique_pat_vars;
({struct Cyc_List_List*_tmp81C=({struct Cyc_List_List*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));({struct _tuple29*_tmp81B=({struct _tuple29*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->f1=1;_tmp5B5->f2=s;_tmp5B5->f3=_tmp5A1;_tmp5B5->f4=_tmp5A2;_tmp5B5;});_tmp5B4->hd=_tmp81B;});_tmp5B4->tl=_tmp5B3;_tmp5B4;});env->unique_pat_vars=_tmp81C;});
# 2796
({union Cyc_CfFlowInfo_FlowInfo _tmp81D=Cyc_NewControlFlow_anal_stmt(env,_tmp5B7,_tmp5DB,rval_opt);_tmp5B7=_tmp81D;});
env->unique_pat_vars=_tmp5B3;
# 2800
({union Cyc_CfFlowInfo_FlowInfo _tmp81E=Cyc_NewControlFlow_unconsume_params(env,_tmp5A1,_tmp5A2,1,_tmp5B7,_tmp5DC);_tmp5B7=_tmp81E;});
# 2804
return _tmp5B7;};};};};}}else{goto _LL4B9;}default: _LL4B9: _tmp5E0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f1;_tmp5DF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_LL4BA:
# 2807
({union Cyc_CfFlowInfo_FlowInfo _tmp822=({struct Cyc_NewControlFlow_AnalEnv*_tmp821=env;union Cyc_CfFlowInfo_FlowInfo _tmp820=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5E0);struct Cyc_Absyn_Stmt*_tmp81F=_tmp5DF;Cyc_NewControlFlow_anal_stmt(_tmp821,_tmp820,_tmp81F,rval_opt);});outflow=_tmp822;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5E0);
return outflow;}default: _LL4BB: _tmp5E1=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp546)->f2;_LL4BC:
# 2812
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E1,rval_opt);}_LL496:;};}
# 2818
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2822
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp5E4=env->fenv;
void*_tmp5E5=decl->r;void*_tmp5E6=_tmp5E5;struct Cyc_Absyn_Tvar*_tmp5FB;struct Cyc_Absyn_Vardecl*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Fndecl*_tmp5F8;struct Cyc_List_List*_tmp5F7;struct Cyc_Absyn_Vardecl*_tmp5F6;switch(*((int*)_tmp5E6)){case 0: _LL50F: _tmp5F6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5E6)->f1;_LL510: {
# 2832
struct Cyc_List_List _tmp5E7=({struct Cyc_List_List _tmp5ED;_tmp5ED.hd=_tmp5F6;_tmp5ED.tl=0;_tmp5ED;});
({union Cyc_CfFlowInfo_FlowInfo _tmp823=Cyc_NewControlFlow_add_vars(_tmp5E4,inflow,& _tmp5E7,_tmp5E4->unknown_none,decl->loc,0);inflow=_tmp823;});{
struct Cyc_Absyn_Exp*_tmp5E8=_tmp5F6->initializer;
if(_tmp5E8 == 0)
return inflow;{
struct _tuple16 _tmp5E9=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5E8);struct _tuple16 _tmp5EA=_tmp5E9;union Cyc_CfFlowInfo_FlowInfo _tmp5EC;void*_tmp5EB;_LL51A: _tmp5EC=_tmp5EA.f1;_tmp5EB=_tmp5EA.f2;_LL51B:;
return Cyc_NewControlFlow_do_initialize_var(_tmp5E4,env,_tmp5EC,_tmp5F6,_tmp5E8,_tmp5EB,decl->loc);};};}case 3: _LL511: _tmp5F7=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5E6)->f1;_LL512:
# 2841
 return Cyc_NewControlFlow_add_vars(_tmp5E4,inflow,_tmp5F7,_tmp5E4->unknown_none,decl->loc,0);case 1: _LL513: _tmp5F8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5E6)->f1;_LL514:
# 2844
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp5E4,inflow,_tmp5F8);{
void*t=(void*)_check_null(_tmp5F8->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp5EE=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp5F8->fn_vardecl);
# 2850
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp827=_tmp5E4;union Cyc_CfFlowInfo_FlowInfo _tmp826=inflow;struct Cyc_List_List*_tmp825=({struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=_tmp5EE;_tmp5EF->tl=0;_tmp5EF;});void*_tmp824=_tmp5E4->unknown_all;Cyc_NewControlFlow_add_vars(_tmp827,_tmp826,_tmp825,_tmp824,decl->loc,0);});};case 4: _LL515: _tmp5FB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5E6)->f1;_tmp5FA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5E6)->f2;_tmp5F9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5E6)->f3;_LL516:
# 2853
 if(_tmp5F9 != 0)
({void*_tmp5F0=0;({struct _dyneither_ptr _tmp828=({const char*_tmp5F1="found open expression in declaration!";_tag_dyneither(_tmp5F1,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp828,_tag_dyneither(_tmp5F0,sizeof(void*),0));});});{
struct Cyc_List_List _tmp5F2=({struct Cyc_List_List _tmp5F3;_tmp5F3.hd=_tmp5FA;_tmp5F3.tl=0;_tmp5F3;});
return Cyc_NewControlFlow_add_vars(_tmp5E4,inflow,& _tmp5F2,_tmp5E4->unknown_all,decl->loc,0);};default: _LL517: _LL518:
# 2859
({void*_tmp5F4=0;({struct _dyneither_ptr _tmp829=({const char*_tmp5F5="anal_decl: unexpected decl variant";_tag_dyneither(_tmp5F5,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp829,_tag_dyneither(_tmp5F4,sizeof(void*),0));});});}_LL50E:;}
# 2867
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp5FC;_push_handler(& _tmp5FC);{int _tmp5FE=0;if(setjmp(_tmp5FC.handler))_tmp5FE=1;if(!_tmp5FE){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp82C=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp82B=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp82A=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp82C,_tmp82B,_tmp82A,fd);});}
# 2869
;_pop_handler();}else{void*_tmp5FD=(void*)_exn_thrown;void*_tmp5FF=_tmp5FD;void*_tmp602;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp5FF)->tag == Cyc_Dict_Absent){_LL51D: _LL51E:
# 2875
 if(Cyc_Position_num_errors > 0)
goto _LL51C;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp600=_cycalloc_atomic(sizeof(*_tmp600));({struct Cyc_Dict_Absent_exn_struct _tmp82D=({struct Cyc_Dict_Absent_exn_struct _tmp601;_tmp601.tag=Cyc_Dict_Absent;_tmp601;});_tmp600[0]=_tmp82D;});_tmp600;}));}}else{_LL51F: _tmp602=_tmp5FF;_LL520:(int)_rethrow(_tmp602);}_LL51C:;}};}
# 2881
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2886
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp603=r;unsigned int _tmp605;if((_tmp603.RParam).tag == 5){_LL522: _tmp605=(_tmp603.RParam).val;_LL523: {
# 2889
struct Cyc_Absyn_Vardecl*_tmp604=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp605);
if(!_tmp604->escapes)
return Cyc_Relations_RVar(_tmp604);
return r;}}else{_LL524: _LL525:
 return r;}_LL521:;}
# 2897
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2901
unsigned int _tmp606=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp82E=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp606,1);inflow=_tmp82E;});{
# 2906
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp607=({union Cyc_CfFlowInfo_FlowInfo _tmp63F=inflow;if((_tmp63F.ReachableFL).tag != 2)_throw_match();(_tmp63F.ReachableFL).val;});struct _tuple14 _tmp608=_tmp607;struct Cyc_Dict_Dict _tmp63E;struct Cyc_List_List*_tmp63D;_LL527: _tmp63E=_tmp608.f1;_tmp63D=_tmp608.f2;_LL528:;{
# 2911
struct Cyc_List_List*_tmp609=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp60A=fd->requires_relns;for(0;_tmp60A != 0;_tmp60A=_tmp60A->tl){
struct Cyc_Relations_Reln*_tmp60B=(struct Cyc_Relations_Reln*)_tmp60A->hd;
({struct Cyc_List_List*_tmp833=({struct _RegionHandle*_tmp832=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp831=Cyc_NewControlFlow_translate_rop(_tmp609,_tmp60B->rop1);enum Cyc_Relations_Relation _tmp830=_tmp60B->relation;union Cyc_Relations_RelnOp _tmp82F=
Cyc_NewControlFlow_translate_rop(_tmp609,_tmp60B->rop2);
# 2914
Cyc_Relations_add_relation(_tmp832,_tmp831,_tmp830,_tmp82F,_tmp63D);});_tmp63D=_tmp833;});}}{
# 2920
struct Cyc_List_List*atts;
{void*_tmp60C=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp60D=_tmp60C;struct Cyc_List_List*_tmp610;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60D)->tag == 9){_LL52A: _tmp610=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60D)->f1).attributes;_LL52B:
 atts=_tmp610;goto _LL529;}else{_LL52C: _LL52D:
({void*_tmp60E=0;({struct _dyneither_ptr _tmp834=({const char*_tmp60F="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp60F,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp834,_tag_dyneither(_tmp60E,sizeof(void*),0));});});}_LL529:;}
# 2927
for(0;atts != 0;atts=atts->tl){
void*_tmp611=(void*)atts->hd;void*_tmp612=_tmp611;int _tmp632;int _tmp631;int _tmp630;switch(*((int*)_tmp612)){case 21: _LL52F: _tmp630=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp612)->f1;_LL530: {
# 2930
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp613=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp630 - 1);
void*t=Cyc_Tcutil_compress(_tmp613->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp614=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp837=fenv;void*_tmp836=elttype;struct Cyc_Absyn_Exp*_tmp835=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp837,_tmp836,_tmp835,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp615=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp838=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp61D;_tmp61D.tag=2;_tmp61D.f1=_tmp630;_tmp61D.f2=t;_tmp61D;});_tmp61C[0]=_tmp838;});_tmp61C;});
struct Cyc_CfFlowInfo_Place*_tmp616=({struct Cyc_CfFlowInfo_Place*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->root=(void*)_tmp615;_tmp61B->path=0;_tmp61B;});
({struct Cyc_Dict_Dict _tmp839=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63E,(void*)_tmp615,_tmp614);_tmp63E=_tmp839;});
({struct Cyc_Dict_Dict _tmp83E=({struct Cyc_Dict_Dict _tmp83D=_tmp63E;void*_tmp83C=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp617=_cycalloc(sizeof(*_tmp617));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp83A=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp618;_tmp618.tag=0;_tmp618.f1=_tmp613;_tmp618;});_tmp617[0]=_tmp83A;});_tmp617;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp83D,_tmp83C,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp619=_cycalloc(sizeof(*_tmp619));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp83B=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp61A;_tmp61A.tag=4;_tmp61A.f1=_tmp616;_tmp61A;});_tmp619[0]=_tmp83B;});_tmp619;}));});_tmp63E=_tmp83E;});
goto _LL52E;}case 20: _LL531: _tmp631=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp612)->f1;_LL532: {
# 2943
struct Cyc_Absyn_Vardecl*_tmp61E=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp631 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp61E->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp61F=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp627=_cycalloc(sizeof(*_tmp627));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp83F=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp628;_tmp628.tag=2;_tmp628.f1=_tmp631;_tmp628.f2=elttype;_tmp628;});_tmp627[0]=_tmp83F;});_tmp627;});
struct Cyc_CfFlowInfo_Place*_tmp620=({struct Cyc_CfFlowInfo_Place*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->root=(void*)_tmp61F;_tmp626->path=0;_tmp626;});
({struct Cyc_Dict_Dict _tmp842=({struct Cyc_Dict_Dict _tmp841=_tmp63E;void*_tmp840=(void*)_tmp61F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp841,_tmp840,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp63E=_tmp842;});
({struct Cyc_Dict_Dict _tmp847=({struct Cyc_Dict_Dict _tmp846=_tmp63E;void*_tmp845=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp621=_cycalloc(sizeof(*_tmp621));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp843=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp622;_tmp622.tag=0;_tmp622.f1=_tmp61E;_tmp622;});_tmp621[0]=_tmp843;});_tmp621;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp846,_tmp845,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp623=_cycalloc(sizeof(*_tmp623));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp844=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp624;_tmp624.tag=4;_tmp624.f1=_tmp620;_tmp624;});_tmp623[0]=_tmp844;});_tmp623;}));});_tmp63E=_tmp847;});
({struct Cyc_List_List*_tmp848=({struct Cyc_List_List*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->hd=_tmp620;_tmp625->tl=param_roots;_tmp625;});param_roots=_tmp848;});
goto _LL52E;}case 22: _LL533: _tmp632=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp612)->f1;_LL534: {
# 2952
struct Cyc_Absyn_Vardecl*_tmp629=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp632 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp629->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp62A=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp849=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp62F;_tmp62F.tag=0;_tmp62F.f1=_tmp629;_tmp62F;});_tmp62E[0]=_tmp849;});_tmp62E;});
struct Cyc_CfFlowInfo_Place*_tmp62B=({struct Cyc_CfFlowInfo_Place*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->root=(void*)_tmp62A;_tmp62D->path=0;_tmp62D;});
({struct Cyc_List_List*_tmp84A=({struct Cyc_List_List*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->hd=_tmp62B;_tmp62C->tl=noconsume_roots;_tmp62C;});noconsume_roots=_tmp84A;});}
# 2958
goto _LL52E;}default: _LL535: _LL536:
 goto _LL52E;}_LL52E:;}
# 2961
({union Cyc_CfFlowInfo_FlowInfo _tmp84B=Cyc_CfFlowInfo_ReachableFL(_tmp63E,_tmp63D);inflow=_tmp84B;});{
# 2963
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->all_tables=tables;({struct Cyc_Hashtable_Table*_tmp84C=
# 2968
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp63C->succ_table=_tmp84C;});({struct Cyc_Hashtable_Table*_tmp84D=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp63C->pat_pop_table=_tmp84D;});_tmp63C->fenv=fenv;_tmp63C->iterate_again=1;_tmp63C->iteration_num=0;_tmp63C->in_try=0;_tmp63C->tryflow=inflow;_tmp63C->noreturn=noreturn;_tmp63C->return_type=fd->ret_type;_tmp63C->unique_pat_vars=0;_tmp63C->param_roots=param_roots;_tmp63C->noconsume_params=noconsume_roots;_tmp63C->flow_table=flow_table;_tmp63C->return_relns=fd->ensures_relns;_tmp63C;});
# 2973
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2979
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp84E=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp84E;});}{
# 2982
union Cyc_CfFlowInfo_FlowInfo _tmp633=outflow;if((_tmp633.BottomFL).tag == 1){_LL538: _LL539:
 goto _LL537;}else{_LL53A: _LL53B:
# 2985
 Cyc_NewControlFlow_check_init_params(_tmp606,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp606);
# 2989
if(noreturn)
({void*_tmp634=0;({unsigned int _tmp850=_tmp606;struct _dyneither_ptr _tmp84F=({const char*_tmp635="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp635,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp850,_tmp84F,_tag_dyneither(_tmp634,sizeof(void*),0));});});else{
# 2992
void*_tmp636=Cyc_Tcutil_compress(fd->ret_type);void*_tmp637=_tmp636;switch(*((int*)_tmp637)){case 0: _LL53D: _LL53E:
 goto _LL53C;case 7: _LL53F: _LL540:
 goto _LL542;case 6: _LL541: _LL542:
# 2996
({void*_tmp638=0;({unsigned int _tmp852=_tmp606;struct _dyneither_ptr _tmp851=({const char*_tmp639="function may not return a value";_tag_dyneither(_tmp639,sizeof(char),32);});Cyc_Warn_warn(_tmp852,_tmp851,_tag_dyneither(_tmp638,sizeof(void*),0));});});goto _LL53C;default: _LL543: _LL544:
# 2998
({void*_tmp63A=0;({unsigned int _tmp854=_tmp606;struct _dyneither_ptr _tmp853=({const char*_tmp63B="function may not return a value";_tag_dyneither(_tmp63B,sizeof(char),32);});Cyc_CfFlowInfo_aerr(_tmp854,_tmp853,_tag_dyneither(_tmp63A,sizeof(void*),0));});});goto _LL53C;}_LL53C:;}
# 3000
goto _LL537;}_LL537:;};};};};};}
# 3004
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp640=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp641=_tmp640;struct Cyc_List_List*_tmp645;struct Cyc_List_List*_tmp644;struct Cyc_List_List*_tmp643;struct Cyc_Absyn_Fndecl*_tmp642;switch(*((int*)_tmp641)){case 1: _LL546: _tmp642=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp641)->f1;_LL547:
 Cyc_NewControlFlow_check_fun(tables,_tmp642);goto _LL545;case 11: _LL548: _tmp643=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp641)->f1;_LL549:
 _tmp644=_tmp643;goto _LL54B;case 10: _LL54A: _tmp644=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp641)->f2;_LL54B:
 _tmp645=_tmp644;goto _LL54D;case 9: _LL54C: _tmp645=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp641)->f2;_LL54D:
 Cyc_NewControlFlow_cf_check(tables,_tmp645);goto _LL545;case 12: _LL54E: _LL54F:
 goto _LL545;default: _LL550: _LL551:
 goto _LL545;}_LL545:;};}}

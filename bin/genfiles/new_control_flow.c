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
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 265 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1173
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 55
void*Cyc_Tcutil_compress(void*t);
# 87
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 92
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 167
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 237 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 269
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 330
int Cyc_Tcutil_is_noreturn(void*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
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
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
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
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 51 "new_control_flow.cyc"
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
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple17{void*f1;int f2;};
# 125 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*,struct Cyc_CfFlowInfo_Place*,void*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp3;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp3=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp3;}else{_LL3: _LL4:
({void*_tmp630=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_NewControlFlow_CFAnnot,(_tmp2->f1).visited=0;_tmp2;});s->annot=_tmp630;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp631=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp631;});_tmp4;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}
# 152
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp5=f;struct Cyc_List_List*_tmp6;if((_tmp5.BottomFL).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _tmp6=((_tmp5.ReachableFL).val).f2;_LL4:
 return _tmp6;}_LL0:;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 159
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 163
({union Cyc_CfFlowInfo_FlowInfo _tmp632=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp632;});
# 169
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp592;_tmp592.f1=_tmp7,_tmp592.f2=_tmp8;_tmp592;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp633=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp633;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 194
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 196
struct _tuple21*_tmp9=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp13;unsigned int _tmp12;struct Cyc_Dict_Dict _tmp11;_LL1: _tmp13=_tmp9->f1;_tmp12=_tmp9->f2;_tmp11=_tmp9->f3;_LL2:;{
void*_tmpA=root;struct Cyc_Absyn_Vardecl*_tmp10;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpA)->tag == 0U){_LL4: _tmp10=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpA)->f1;_LL5:
# 199
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp10->type)){
retry: {void*_tmpB=rval;void*_tmpF;switch(*((int*)_tmpB)){case 8U: _LL9: _tmpF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB)->f2;_LLA:
 rval=_tmpF;goto retry;case 7U: _LLB: _LLC:
 goto _LLE;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB)->f1 == Cyc_CfFlowInfo_NoneIL){_LLD: _LLE:
 goto _LL10;}else{goto _LL11;}case 0U: _LLF: _LL10:
 goto _LL8;default: _LL11: _LL12:
# 208
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp593;_tmp593.tag=0U,({struct _dyneither_ptr _tmp634=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10->name));_tmp593.f1=_tmp634;});_tmp593;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned int _tmp636=_tmp12;struct _dyneither_ptr _tmp635=({const char*_tmpD="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpD,sizeof(char),64U);});Cyc_Warn_warn(_tmp636,_tmp635,_tag_dyneither(_tmpC,sizeof(void*),1U));});});
goto _LL8;}_LL8:;}}
# 212
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 220
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp14=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp15=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp16=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp15);
# 226
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp17=({struct _tuple19 _tmp595;_tmp595.f1=flow,_tmp595.f2=_tmp16;_tmp595;});struct _tuple19 _tmp18=_tmp17;struct Cyc_Dict_Dict _tmp1B;struct Cyc_Dict_Dict _tmp1A;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_LL1: _tmp1B=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;_LL2: {
# 229
struct _tuple21 _tmp19=({struct _tuple21 _tmp594;_tmp594.f1=env,_tmp594.f2=s->loc,_tmp594.f3=_tmp1A;_tmp594;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp19,_tmp1B);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 235
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp16,*_tmp15)){
# 242
*_tmp15=_tmp16;
# 246
if(_tmp14->visited == env->iteration_num)
# 248
env->iterate_again=1;}}
# 253
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 257
union Cyc_CfFlowInfo_FlowInfo _tmp1C=inflow;struct Cyc_Dict_Dict _tmp22;struct Cyc_List_List*_tmp21;if((_tmp1C.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp22=((_tmp1C.ReachableFL).val).f1;_tmp21=((_tmp1C.ReachableFL).val).f2;_LL4:
# 260
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp1D=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=0U,_tmp20->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp20;});
void*_tmp1E=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp637=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=8U,_tmp1F->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp1F->f2=_tmp1E;_tmp1F;});_tmp1E=_tmp637;});
# 267
({struct Cyc_Dict_Dict _tmp638=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp22,(void*)_tmp1D,_tmp1E);_tmp22=_tmp638;});}
# 269
return Cyc_CfFlowInfo_ReachableFL(_tmp22,_tmp21);}_LL0:;}
# 273
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 280
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp23=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->hd=_tmp23,_tmp24->tl=assume;_tmp24;})))
return 0;}
# 285
return 1;}
# 288
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp25=e->r;void*_tmp26=_tmp25;struct Cyc_Absyn_Exp*_tmp27;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->tag == 14U){_LL1: _tmp27=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->f2;_LL2:
 return _tmp27;}else{_LL3: _LL4:
 return e;}_LL0:;}
# 295
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 297
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_List_List*_tmp46;if((_tmp28.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp46=((_tmp28.ReachableFL).val).f2;_LL4:
# 300
{void*_tmp29=Cyc_Tcutil_compress(t);void*_tmp2A=_tmp29;struct Cyc_List_List*_tmp45;union Cyc_Absyn_AggrInfoU _tmp44;struct Cyc_List_List*_tmp43;switch(*((int*)_tmp2A)){case 11U: _LL6: _tmp44=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A)->f1).aggr_info;_tmp43=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A)->f1).targs;_LL7: {
# 302
struct Cyc_Absyn_Aggrdecl*_tmp2B=Cyc_Absyn_get_known_aggrdecl(_tmp44);
struct Cyc_Absyn_Aggrfield*_tmp2C=Cyc_Absyn_lookup_decl_field(_tmp2B,f);
struct Cyc_Absyn_Exp*_tmp2D=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp2C))->requires_clause;
if(_tmp2D != 0){
struct _RegionHandle _tmp2E=_new_region("temp");struct _RegionHandle*temp=& _tmp2E;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp2F=({struct _RegionHandle*_tmp63A=temp;struct Cyc_List_List*_tmp639=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp2B->tvs,_tmp43);Cyc_Tcutil_rsubsexp(_tmp63A,_tmp639,_tmp2D);});
# 309
if(!({struct Cyc_List_List*_tmp63B=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp63B,Cyc_Relations_exp2relns(temp,_tmp2F));})){
({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp597;_tmp597.tag=0U,({struct _dyneither_ptr _tmp63C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp2F)));_tmp597.f1=_tmp63C;});_tmp597;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp596;_tmp596.tag=0U,_tmp596.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp596;});void*_tmp30[2U];_tmp30[0]=& _tmp32,_tmp30[1]=& _tmp33;({unsigned int _tmp63E=loc;struct _dyneither_ptr _tmp63D=({const char*_tmp31="unable to prove %s, required to access %s";_tag_dyneither(_tmp31,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp63E,_tmp63D,_tag_dyneither(_tmp30,sizeof(void*),2U));});});
({void*_tmp34=0U;({struct Cyc___cycFILE*_tmp640=Cyc_stderr;struct _dyneither_ptr _tmp63F=({const char*_tmp35="  [recorded facts on path: ";_tag_dyneither(_tmp35,sizeof(char),28U);});Cyc_fprintf(_tmp640,_tmp63F,_tag_dyneither(_tmp34,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp36=0U;({struct Cyc___cycFILE*_tmp642=Cyc_stderr;struct _dyneither_ptr _tmp641=({const char*_tmp37="]\n";_tag_dyneither(_tmp37,sizeof(char),3U);});Cyc_fprintf(_tmp642,_tmp641,_tag_dyneither(_tmp36,sizeof(void*),0U));});});}}
# 307
;_pop_region(temp);}
# 316
goto _LL5;}case 12U: _LL8: _tmp45=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A)->f2;_LL9: {
# 318
struct Cyc_Absyn_Aggrfield*_tmp38=Cyc_Absyn_lookup_field(_tmp45,f);
struct Cyc_Absyn_Exp*_tmp39=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp38))->requires_clause;
if(_tmp39 != 0){
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
if(!({struct Cyc_List_List*_tmp643=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp643,Cyc_Relations_exp2relns(temp,_tmp39));})){
({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp599;_tmp599.tag=0U,({struct _dyneither_ptr _tmp644=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp39)));_tmp599.f1=_tmp644;});_tmp599;});struct Cyc_String_pa_PrintArg_struct _tmp3E=({struct Cyc_String_pa_PrintArg_struct _tmp598;_tmp598.tag=0U,_tmp598.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp598;});void*_tmp3B[2U];_tmp3B[0]=& _tmp3D,_tmp3B[1]=& _tmp3E;({unsigned int _tmp646=loc;struct _dyneither_ptr _tmp645=({const char*_tmp3C="unable to prove %s, required to access %s";_tag_dyneither(_tmp3C,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp646,_tmp645,_tag_dyneither(_tmp3B,sizeof(void*),2U));});});
({void*_tmp3F=0U;({struct Cyc___cycFILE*_tmp648=Cyc_stderr;struct _dyneither_ptr _tmp647=({const char*_tmp40="  [recorded facts on path: ";_tag_dyneither(_tmp40,sizeof(char),28U);});Cyc_fprintf(_tmp648,_tmp647,_tag_dyneither(_tmp3F,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp41=0U;({struct Cyc___cycFILE*_tmp64A=Cyc_stderr;struct _dyneither_ptr _tmp649=({const char*_tmp42="]\n";_tag_dyneither(_tmp42,sizeof(char),3U);});Cyc_fprintf(_tmp64A,_tmp649,_tag_dyneither(_tmp41,sizeof(void*),0U));});});}
# 322
;_pop_region(temp);}
# 329
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 332
goto _LL0;}_LL0:;}
# 336
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp47=inflow;struct Cyc_Dict_Dict _tmp4D;struct Cyc_List_List*_tmp4C;if((_tmp47.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp4D=((_tmp47.ReachableFL).val).f1;_tmp4C=((_tmp47.ReachableFL).val).f2;_LL4:
# 340
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp48=0U;({unsigned int _tmp64C=loc;struct _dyneither_ptr _tmp64B=({const char*_tmp49="expression may not be fully initialized";_tag_dyneither(_tmp49,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp64C,_tmp64B,_tag_dyneither(_tmp48,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp4A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp4D,r);
if(_tmp4D.t == _tmp4A.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B=Cyc_CfFlowInfo_ReachableFL(_tmp4A,_tmp4C);
Cyc_NewControlFlow_update_tryflow(env,_tmp4B);
return _tmp4B;};};}_LL0:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 350
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp4E=({struct _tuple0 _tmp59A;({void*_tmp64D=Cyc_Tcutil_compress(t);_tmp59A.f1=_tmp64D;}),_tmp59A.f2=r;_tmp59A;});struct _tuple0 _tmp4F=_tmp4E;enum Cyc_Absyn_AggrKind _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;union Cyc_Absyn_AggrInfoU _tmp5D;struct Cyc_List_List*_tmp5C;struct _dyneither_ptr _tmp5B;struct Cyc_List_List*_tmp5A;struct _dyneither_ptr _tmp59;struct Cyc_Absyn_Datatypefield*_tmp58;struct _dyneither_ptr _tmp57;void*_tmp56;switch(*((int*)_tmp4F.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp4F.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 10U: goto _LL13;case 11U: goto _LL13;case 12U: goto _LL13;case 5U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp56=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL8:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp56);return;default: switch(*((int*)_tmp4F.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LL9: _tmp58=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp57=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLA:
# 357
 if(_tmp58->typs == 0)
return;
_tmp5A=_tmp58->typs;_tmp59=_tmp57;goto _LLC;}else{goto _LL13;}}else{goto _LL13;}case 10U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLB: _tmp5A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLC: {
# 361
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5A);
{int i=0;for(0;i < sz;(i ++,_tmp5A=_tmp5A->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp650=env;unsigned int _tmp64F=loc;void*_tmp64E=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5A))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp650,_tmp64F,_tmp64E,*((void**)_check_dyneither_subscript(_tmp59,sizeof(void*),i)));});}}
# 365
return;}}else{goto _LL13;}case 11U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLD: _tmp5D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F.f1)->f1).aggr_info;_tmp5C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F.f1)->f1).targs;_tmp5B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLE: {
# 368
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp5D);
if(_tmp50->impl == 0)return;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
struct _RegionHandle _tmp52=_new_region("temp");struct _RegionHandle*temp=& _tmp52;_push_region(temp);
{struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp50->tvs,_tmp5C);
{int i=0;for(0;i < _get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp51=_tmp51->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->type;
if(_tmp53 != 0)({void*_tmp651=Cyc_Tcutil_rsubstitute(temp,_tmp53,t);t=_tmp651;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp5B.curr)[i]);}}
# 378
_npop_handler(0U);return;}
# 372
;_pop_region(temp);};}}else{goto _LL13;}case 12U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLF: _tmp60=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp5F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL10:
# 381
{int i=0;for(0;i < _get_dyneither_size(_tmp5E,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((void**)_tmp5E.curr)[i]);}}
return;}else{goto _LL13;}case 5U: _LL11: _LL12:
# 385
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp54=0U;({unsigned int _tmp653=loc;struct _dyneither_ptr _tmp652=({const char*_tmp55="argument may still contain alias-free pointers";_tag_dyneither(_tmp55,sizeof(char),47U);});Cyc_Warn_warn(_tmp653,_tmp652,_tag_dyneither(_tmp54,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 392
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp61=inflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6D;if((_tmp61.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp6E=((_tmp61.ReachableFL).val).f1;_tmp6D=((_tmp61.ReachableFL).val).f2;_LL4:
# 396
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp62=0U;({struct _dyneither_ptr _tmp654=({const char*_tmp63="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp63,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp654,_tag_dyneither(_tmp62,sizeof(void*),0U));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 400
void*_tmp64=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp65=r;void*_tmp6A;struct Cyc_CfFlowInfo_Place*_tmp69;void*_tmp68;switch(*((int*)_tmp65)){case 8U: _LL6: _tmp68=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp65)->f2;_LL7:
 r=_tmp68;goto retry;case 4U: _LL8: _tmp69=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LL9:
({void*_tmp655=Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp69);_tmp6A=_tmp655;});goto _LLB;case 5U: _LLA: _tmp6A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LLB:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp64,_tmp6A);goto _LL5;default: _LLC: _LLD:
# 406
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp64))
({void*_tmp66=0U;({unsigned int _tmp657=loc;struct _dyneither_ptr _tmp656=({const char*_tmp67="argument may contain live alias-free pointers";_tag_dyneither(_tmp67,sizeof(char),46U);});Cyc_Warn_warn(_tmp657,_tmp656,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 410
struct Cyc_Dict_Dict _tmp6B=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6E,r);
if(_tmp6E.t == _tmp6B.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp6C=Cyc_CfFlowInfo_ReachableFL(_tmp6B,_tmp6D);
Cyc_NewControlFlow_update_tryflow(env,_tmp6C);
return _tmp6C;};};};}_LL0:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 420
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 425
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp59B;_tmp59B.f1=inflow,_tmp59B.f2=0;_tmp59B;});
# 430
do{
struct _tuple16 _tmp6F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp70=_tmp6F;union Cyc_CfFlowInfo_FlowInfo _tmp73;void*_tmp72;_LL1: _tmp73=_tmp70.f1;_tmp72=_tmp70.f2;_LL2:;
inflow=_tmp73;
({struct Cyc_List_List*_tmp658=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=_tmp72,_tmp71->tl=rvals;_tmp71;});rvals=_tmp658;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 438
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp59C;_tmp59C.f1=inflow,({struct Cyc_List_List*_tmp659=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp59C.f2=_tmp659;});_tmp59C;});}
# 444
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 446
struct _tuple23 _tmp74=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 446
struct _tuple23 _tmp75=_tmp74;union Cyc_CfFlowInfo_FlowInfo _tmp7B;struct Cyc_List_List*_tmp7A;_LL1: _tmp7B=_tmp75.f1;_tmp7A=_tmp75.f2;_LL2:;
# 448
{union Cyc_CfFlowInfo_FlowInfo _tmp76=_tmp7B;struct Cyc_Dict_Dict _tmp79;if((_tmp76.ReachableFL).tag == 2){_LL4: _tmp79=((_tmp76.ReachableFL).val).f1;_LL5:
# 450
 for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp79,(void*)_tmp7A->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp77=0U;({unsigned int _tmp65B=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp65A=({const char*_tmp78="expression may not be initialized";_tag_dyneither(_tmp78,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65B,_tmp65A,_tag_dyneither(_tmp77,sizeof(void*),0U));});});}
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 456
return({struct _tuple16 _tmp59D;_tmp59D.f1=_tmp7B,_tmp59D.f2=(env->fenv)->unknown_all;_tmp59D;});}
# 463
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 470
int needs_unconsume=0;
void*_tmp7C=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp7C,& needs_unconsume))
({void*_tmp7D=0U;({struct _dyneither_ptr _tmp65C=({const char*_tmp7E="consume_zero_rval";_tag_dyneither(_tmp7E,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp65C,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});else{
# 475
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 479
return new_rval;}}}
# 492 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp7F=outflow;struct Cyc_Dict_Dict _tmp86;struct Cyc_List_List*_tmp85;if((_tmp7F.BottomFL).tag == 1){_LL1: _LL2:
 return outflow;}else{_LL3: _tmp86=((_tmp7F.ReachableFL).val).f1;_tmp85=((_tmp7F.ReachableFL).val).f2;_LL4: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp80=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp81=_tmp80;struct Cyc_CfFlowInfo_Place*_tmp84;if((_tmp81.UnknownL).tag == 2){_LL6: _LL7:
# 504
 return outflow;}else{_LL8: _tmp84=(_tmp81.PlaceL).val;_LL9: {
# 508
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
({void*_tmp65D=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=8U,_tmp82->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp82->f2=nzval;_tmp82;});nzval=_tmp65D;});}
# 512
({void*_tmp65E=Cyc_NewControlFlow_consume_zero_rval(env,_tmp86,_tmp84,e,nzval);nzval=_tmp65E;});{
union Cyc_CfFlowInfo_FlowInfo _tmp83=({struct Cyc_Dict_Dict _tmp65F=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp86,_tmp84,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp65F,_tmp85);});
# 517
return _tmp83;};}}_LL5:;}}_LL0:;}
# 526
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 528
union Cyc_CfFlowInfo_FlowInfo _tmp87=outflow;struct Cyc_Dict_Dict _tmp8E;struct Cyc_List_List*_tmp8D;if((_tmp87.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple19 _tmp59E;_tmp59E.f1=outflow,_tmp59E.f2=outflow;_tmp59E;});}else{_LL3: _tmp8E=((_tmp87.ReachableFL).val).f1;_tmp8D=((_tmp87.ReachableFL).val).f2;_LL4: {
# 531
union Cyc_CfFlowInfo_AbsLVal _tmp88=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp89=_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8C;if((_tmp89.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple19 _tmp59F;_tmp59F.f1=outflow,_tmp59F.f2=outflow;_tmp59F;});}else{_LL8: _tmp8C=(_tmp89.PlaceL).val;_LL9: {
# 534
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp660=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=8U,_tmp8A->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8A->f2=nzval;_tmp8A;});nzval=_tmp660;});
({void*_tmp661=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=8U,_tmp8B->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8B->f2=zval;_tmp8B;});zval=_tmp661;});}
# 540
({void*_tmp662=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,nzval);nzval=_tmp662;});
({void*_tmp663=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,zval);zval=_tmp663;});
return({struct _tuple19 _tmp5A0;({
union Cyc_CfFlowInfo_FlowInfo _tmp667=({struct Cyc_Dict_Dict _tmp666=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp666,_tmp8D);});_tmp5A0.f1=_tmp667;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp665=({struct Cyc_Dict_Dict _tmp664=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,zval);Cyc_CfFlowInfo_ReachableFL(_tmp664,_tmp8D);});_tmp5A0.f2=_tmp665;});_tmp5A0;});}}_LL5:;}}_LL0:;}
# 549
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 552
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
void*_tmp8F=e->r;void*_tmp90=_tmp8F;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp90)->tag == 23U){_LL1: _LL2:
# 556
{void*_tmp91=e->annot;void*_tmp92=_tmp91;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp93;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp92)->tag == Cyc_CfFlowInfo_UnknownZ){_LL6: _tmp93=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp92)->f1;_LL7:
# 558
 if(possibly_null  && Cyc_Relations_same_relns(relns,_tmp93))return;
goto _LL5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp92)->tag == Cyc_CfFlowInfo_NotZero){_LL8: _tmp94=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp92)->f1;_LL9:
# 561
 possibly_null=0;
if(Cyc_Relations_same_relns(relns,_tmp94))return;
goto _LL5;}else{_LLA: _LLB:
 goto _LL5;}}_LL5:;}
# 566
if(possibly_null)
({void*_tmp668=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=Cyc_CfFlowInfo_UnknownZ,_tmp95->f1=relns;_tmp95;});e->annot=_tmp668;});else{
# 570
({void*_tmp669=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=Cyc_CfFlowInfo_NotZero,_tmp96->f1=relns;_tmp96;});e->annot=_tmp669;});}
# 572
goto _LL0;}else{_LL3: _LL4:
# 576
 if(possibly_null){
void*_tmp97=e->annot;void*_tmp98=_tmp97;struct Cyc_List_List*_tmp99;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp98)->tag == Cyc_CfFlowInfo_UnknownZ){_LLD: _tmp99=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp98)->f1;_LLE:
# 579
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 584
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL0;}_LL0:;}
# 594
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 597
void*_tmp9A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp9B=_tmp9A;void*_tmpBF;union Cyc_Absyn_Constraint*_tmpBE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->tag == 5U){_LL1: _tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->f1).elt_typ;_tmpBE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->f1).ptr_atts).bounds;_LL2: {
# 599
union Cyc_CfFlowInfo_FlowInfo _tmp9C=f;struct Cyc_Dict_Dict _tmpBB;struct Cyc_List_List*_tmpBA;if((_tmp9C.BottomFL).tag == 1){_LL6: _LL7:
# 601
 return({struct _tuple16 _tmp5A1;_tmp5A1.f1=f,({void*_tmp66A=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A1.f2=_tmp66A;});_tmp5A1;});}else{_LL8: _tmpBB=((_tmp9C.ReachableFL).val).f1;_tmpBA=((_tmp9C.ReachableFL).val).f2;_LL9:
# 604
 if(Cyc_Tcutil_is_bound_one(_tmpBE)){
struct _tuple15 _tmp9D=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp9E=_tmp9D;void*_tmpAB;struct Cyc_List_List*_tmpAA;_LLB: _tmpAB=_tmp9E.f1;_tmpAA=_tmp9E.f2;_LLC:;{
void*_tmp9F=_tmpAB;enum Cyc_CfFlowInfo_InitLevel _tmpA9;void*_tmpA8;struct Cyc_CfFlowInfo_Place*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA6;void*_tmpA5;switch(*((int*)_tmp9F)){case 8U: _LLE: _tmpA6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_tmpA5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f2;_LLF:
# 608
({void*_tmpA0=0U;({struct _dyneither_ptr _tmp66B=({const char*_tmpA1="named location in anal_derefR";_tag_dyneither(_tmpA1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp66B,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 610
 Cyc_NewControlFlow_update_relns(e,0,_tmpBA);
goto _LLD;case 4U: _LL12: _tmpA7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL13:
# 613
 Cyc_NewControlFlow_update_relns(e,0,_tmpBA);
return({struct _tuple16 _tmp5A2;_tmp5A2.f1=f,({void*_tmp66C=Cyc_CfFlowInfo_lookup_place(_tmpBB,_tmpA7);_tmp5A2.f2=_tmp66C;});_tmp5A2;});case 5U: _LL14: _tmpA8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL15:
# 616
 Cyc_NewControlFlow_update_relns(e,1,_tmpBA);
return({struct _tuple16 _tmp5A3;_tmp5A3.f1=f,_tmp5A3.f2=_tmpA8;_tmp5A3;});case 0U: _LL16: _LL17:
# 619
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmp5A4;({union Cyc_CfFlowInfo_FlowInfo _tmp66E=Cyc_CfFlowInfo_BottomFL();_tmp5A4.f1=_tmp66E;}),({void*_tmp66D=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A4.f2=_tmp66D;});_tmp5A4;});case 2U: _LL18: _tmpA9=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL19:
# 623
({union Cyc_CfFlowInfo_FlowInfo _tmp66F=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA9,_tmpAA);f=_tmp66F;});
goto _LL1B;default: _LL1A: _LL1B:
# 626
{void*_tmpA2=e->r;void*_tmpA3=_tmpA2;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA3)->tag == 23U){_LL1D: _LL1E:
# 628
({void*_tmp670=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=Cyc_CfFlowInfo_UnknownZ,_tmpA4->f1=_tmpBA;_tmpA4;});e->annot=_tmp670;});
goto _LL1C;}else{_LL1F: _LL20:
# 631
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL1C;}_LL1C:;}
# 634
goto _LLD;}_LLD:;};}else{
# 637
void*_tmpAC=e->annot;void*_tmpAD=_tmpAC;struct Cyc_List_List*_tmpAF;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL22: _tmpAF=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAD)->f1;_LL23:
# 639
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpAF))goto _LL25;
goto _LL21;}else{_LL24: _LL25:
# 642
({void*_tmp671=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=Cyc_CfFlowInfo_UnknownZ,_tmpAE->f1=_tmpBA;_tmpAE;});e->annot=_tmp671;});
goto _LL21;}_LL21:;}{
# 646
enum Cyc_CfFlowInfo_InitLevel _tmpB0=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpBB,r);enum Cyc_CfFlowInfo_InitLevel _tmpB1=_tmpB0;if(_tmpB1 == Cyc_CfFlowInfo_NoneIL){_LL27: _LL28: {
# 648
struct _tuple15 _tmpB2=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpB3=_tmpB2;void*_tmpB9;_LL2C: _tmpB9=_tmpB3.f1;_LL2D:;
{void*_tmpB4=_tmpB9;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB4)->tag == 7U){_LL2F: _LL30:
# 651
({void*_tmpB5=0U;({unsigned int _tmp673=e->loc;struct _dyneither_ptr _tmp672=({const char*_tmpB6="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpB6,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp673,_tmp672,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});
goto _LL2E;}else{_LL31: _LL32:
# 654
({void*_tmpB7=0U;({unsigned int _tmp675=e->loc;struct _dyneither_ptr _tmp674=({const char*_tmpB8="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpB8,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp675,_tmp674,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});}_LL2E:;}
# 656
goto _LL2A;}}else{_LL29: _LL2A:
# 658
 return({struct _tuple16 _tmp5A5;_tmp5A5.f1=f,({void*_tmp676=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A5.f2=_tmp676;});_tmp5A5;});}_LL26:;};}_LL5:;}}else{_LL3: _LL4:
# 661
({void*_tmpBC=0U;({struct _dyneither_ptr _tmp677=({const char*_tmpBD="right deref of non-pointer-type";_tag_dyneither(_tmpBD,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp677,_tag_dyneither(_tmpBC,sizeof(void*),0U));});});}_LL0:;}
# 668
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 670
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 673
{void*_tmpC0=e1->r;void*_tmpC1=_tmpC0;void*_tmpC3;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC1)->tag == 1U){_LL1: _tmpC3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL2: {
# 675
struct Cyc_Absyn_Vardecl*_tmpC2=Cyc_Tcutil_nonesc_vardecl(_tmpC3);
if(_tmpC2 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpC2);
if(e2_valid_op)
({struct Cyc_List_List*_tmp678=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp678;});}
# 681
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 685
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp679=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp679;});}
# 691
return relns;};}
# 699
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 704
struct _tuple18 _tmpC4=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpC5=_tmpC4;union Cyc_CfFlowInfo_AbsLVal _tmpD3;_LL1: _tmpD3=_tmpC5.f2;_LL2:;
{struct _tuple18 _tmpC6=({struct _tuple18 _tmp5A6;_tmp5A6.f1=inflow,_tmp5A6.f2=_tmpD3;_tmp5A6;});struct _tuple18 _tmpC7=_tmpC6;struct Cyc_Dict_Dict _tmpD2;struct Cyc_List_List*_tmpD1;struct Cyc_CfFlowInfo_Place*_tmpD0;if(((_tmpC7.f1).ReachableFL).tag == 2){if(((_tmpC7.f2).PlaceL).tag == 1){_LL4: _tmpD2=(((_tmpC7.f1).ReachableFL).val).f1;_tmpD1=(((_tmpC7.f1).ReachableFL).val).f2;_tmpD0=((_tmpC7.f2).PlaceL).val;_LL5: {
# 707
void*_tmpC8=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 709
struct _tuple15 _tmpC9=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple15 _tmpCA=_tmpC9;void*_tmpCD;struct Cyc_List_List*_tmpCC;_LLB: _tmpCD=_tmpCA.f1;_tmpCC=_tmpCA.f2;_LLC:;
for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
({void*_tmp67A=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=8U,_tmpCB->f1=(struct Cyc_Absyn_Vardecl*)_tmpCC->hd,_tmpCB->f2=_tmpC8;_tmpCB;});_tmpC8=_tmp67A;});}
# 714
({struct Cyc_Dict_Dict _tmp67B=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpD2,_tmpD0,_tmpC8);_tmpD2=_tmp67B;});
({union Cyc_CfFlowInfo_FlowInfo _tmp67C=Cyc_CfFlowInfo_ReachableFL(_tmpD2,_tmpD1);inflow=_tmp67C;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 720
goto _LL3;}}else{_LL8: _LL9:
# 723
({void*_tmpCE=0U;({struct _dyneither_ptr _tmp67D=({const char*_tmpCF="noconsume() parameters must be l-values";_tag_dyneither(_tmpCF,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp67D,_tag_dyneither(_tmpCE,sizeof(void*),0U));});});
goto _LL3;}}else{_LL6: _LL7:
# 721
 goto _LL3;}_LL3:;}
# 726
return inflow;}
# 731
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 738
union Cyc_CfFlowInfo_FlowInfo _tmpD4=outflow;struct Cyc_Dict_Dict _tmpDF;struct Cyc_List_List*_tmpDE;if((_tmpD4.BottomFL).tag == 1){_LL1: _LL2:
# 740
 return({struct _tuple16 _tmp5A7;({union Cyc_CfFlowInfo_FlowInfo _tmp67E=Cyc_CfFlowInfo_BottomFL();_tmp5A7.f1=_tmp67E;}),_tmp5A7.f2=rval;_tmp5A7;});}else{_LL3: _tmpDF=((_tmpD4.ReachableFL).val).f1;_tmpDE=((_tmpD4.ReachableFL).val).f2;_LL4: {
# 742
union Cyc_CfFlowInfo_AbsLVal _tmpD5=lval;struct Cyc_CfFlowInfo_Place*_tmpDD;if((_tmpD5.PlaceL).tag == 1){_LL6: _tmpDD=(_tmpD5.PlaceL).val;_LL7: {
# 750
struct Cyc_Dict_Dict _tmpD6=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpDF,_tmpDD,rval);
({struct Cyc_List_List*_tmp67F=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpDE,lexp,rexp);_tmpDE=_tmp67F;});
({union Cyc_CfFlowInfo_FlowInfo _tmp680=Cyc_CfFlowInfo_ReachableFL(_tmpD6,_tmpDE);outflow=_tmp680;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 756
struct _tuple15 _tmpD7=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmpDF,_tmpDD));struct _tuple15 _tmpD8=_tmpD7;void*_tmpDC;_LLB: _tmpDC=_tmpD8.f1;_LLC:;{
void*_tmpD9=_tmpDC;switch(*((int*)_tmpD9)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD9)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 762
({void*_tmpDA=0U;({unsigned int _tmp682=lexp->loc;struct _dyneither_ptr _tmp681=({const char*_tmpDB="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpDB,sizeof(char),47U);});Cyc_Warn_warn(_tmp682,_tmp681,_tag_dyneither(_tmpDA,sizeof(void*),0U));});});
goto _LLD;}_LLD:;};}
# 767
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmp5A8;_tmp5A8.f1=outflow,_tmp5A8.f2=rval;_tmp5A8;});}}else{_LL8: _LL9:
# 773
 return({struct _tuple16 _tmp5A9;({union Cyc_CfFlowInfo_FlowInfo _tmp683=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp5A9.f1=_tmp683;}),_tmp5A9.f2=rval;_tmp5A9;});}_LL5:;}}_LL0:;}
# 780
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 786
union Cyc_CfFlowInfo_FlowInfo _tmpE0=f;struct Cyc_Dict_Dict _tmpE5;struct Cyc_List_List*_tmpE4;if((_tmpE0.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmpE5=((_tmpE0.ReachableFL).val).f1;_tmpE4=((_tmpE0.ReachableFL).val).f2;_LL4:
# 791
({struct Cyc_Dict_Dict _tmp689=({struct Cyc_CfFlowInfo_FlowEnv*_tmp688=fenv;unsigned int _tmp687=loc;struct Cyc_Dict_Dict _tmp686=_tmpE5;struct Cyc_CfFlowInfo_Place*_tmp685=({struct Cyc_CfFlowInfo_Place*_tmpE2=_cycalloc(sizeof(*_tmpE2));
({void*_tmp684=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->tag=0U,_tmpE1->f1=vd;_tmpE1;});_tmpE2->root=_tmp684;}),_tmpE2->path=0;_tmpE2;});
# 791
Cyc_CfFlowInfo_assign_place(_tmp688,_tmp687,_tmp686,_tmp685,rval);});_tmpE5=_tmp689;});
# 794
({struct Cyc_List_List*_tmp68A=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmpE4,vd,rexp);_tmpE4=_tmp68A;});{
union Cyc_CfFlowInfo_FlowInfo _tmpE3=Cyc_CfFlowInfo_ReachableFL(_tmpE5,_tmpE4);
Cyc_NewControlFlow_update_tryflow(env,_tmpE3);
# 799
return _tmpE3;};}_LL0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 803
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 810
if(vds == 0)return inflow;{
# 813
struct Cyc_List_List*_tmpE6=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp68B=({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),_tmpE7->tl=es;_tmpE7;});es=_tmp68B;});}}
# 819
({union Cyc_CfFlowInfo_FlowInfo _tmp68C=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmpE6,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp68C;});
# 821
({union Cyc_CfFlowInfo_FlowInfo _tmp68D=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp68D;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 825
struct _tuple16 _tmpE8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmpE9=_tmpE8;union Cyc_CfFlowInfo_FlowInfo _tmpEB;void*_tmpEA;_LL1: _tmpEB=_tmpE9.f1;_tmpEA=_tmpE9.f2;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp68E=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmpEB,_tmpEA);inflow=_tmp68E;});}}{
# 833
struct Cyc_List_List*_tmpEC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmpEC != 0;_tmpEC=_tmpEC->tl){
struct _tuple24*_tmpED=(struct _tuple24*)_tmpEC->hd;struct _tuple24*_tmpEE=_tmpED;struct Cyc_Absyn_Vardecl**_tmp106;struct Cyc_Absyn_Exp*_tmp105;_LL4: _tmp106=_tmpEE->f1;_tmp105=_tmpEE->f2;_LL5:;
if(_tmp106 != 0  && _tmp105 != 0){
if(_tmp105->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpF1=({struct Cyc_String_pa_PrintArg_struct _tmp5AA;_tmp5AA.tag=0U,({
struct _dyneither_ptr _tmp68F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp105));_tmp5AA.f1=_tmp68F;});_tmp5AA;});void*_tmpEF[1U];_tmpEF[0]=& _tmpF1;({struct _dyneither_ptr _tmp690=({const char*_tmpF0="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmpF0,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp690,_tag_dyneither(_tmpEF,sizeof(void*),1U));});});{
# 847
struct Cyc_List_List l=({struct Cyc_List_List _tmp5AC;_tmp5AC.hd=_tmp105,_tmp5AC.tl=0;_tmp5AC;});
union Cyc_CfFlowInfo_FlowInfo _tmpF2=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmpF3=Cyc_NewControlFlow_anal_Lexp(env,_tmpF2,0,0,_tmp105);struct _tuple18 _tmpF4=_tmpF3;union Cyc_CfFlowInfo_AbsLVal _tmp104;_LL7: _tmp104=_tmpF4.f2;_LL8:;{
struct _tuple16 _tmpF5=Cyc_NewControlFlow_anal_Rexp(env,1,_tmpF2,_tmp105);struct _tuple16 _tmpF6=_tmpF5;union Cyc_CfFlowInfo_FlowInfo _tmp103;void*_tmp102;_LLA: _tmp103=_tmpF6.f1;_tmp102=_tmpF6.f2;_LLB:;{
union Cyc_CfFlowInfo_FlowInfo _tmpF7=_tmp103;struct Cyc_Dict_Dict _tmp101;struct Cyc_List_List*_tmp100;if((_tmpF7.ReachableFL).tag == 2){_LLD: _tmp101=((_tmpF7.ReachableFL).val).f1;_tmp100=((_tmpF7.ReachableFL).val).f2;_LLE:
# 853
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmpF8=_tmp104;struct Cyc_CfFlowInfo_Place*_tmpFF;if((_tmpF8.PlaceL).tag == 1){_LL12: _tmpFF=(_tmpF8.PlaceL).val;_LL13:
# 856
({void*_tmp691=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->tag=8U,_tmpF9->f1=*_tmp106,_tmpF9->f2=_tmp102;_tmpF9;});_tmp102=_tmp691;});{
# 859
void*_tmpFA=Cyc_CfFlowInfo_lookup_place(_tmp101,_tmpFF);
({void*_tmp692=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=8U,_tmpFB->f1=*_tmp106,_tmpFB->f2=_tmpFA;_tmpFB;});_tmpFA=_tmp692;});
({struct Cyc_Dict_Dict _tmp693=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp101,_tmpFF,_tmpFA);_tmp101=_tmp693;});
({union Cyc_CfFlowInfo_FlowInfo _tmp694=Cyc_CfFlowInfo_ReachableFL(_tmp101,_tmp100);_tmp103=_tmp694;});
goto _LL11;};}else{_LL14: _LL15:
# 866
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp105->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp106)->type))
({struct Cyc_String_pa_PrintArg_struct _tmpFE=({struct Cyc_String_pa_PrintArg_struct _tmp5AB;_tmp5AB.tag=0U,({
struct _dyneither_ptr _tmp695=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp105));_tmp5AB.f1=_tmp695;});_tmp5AB;});void*_tmpFC[1U];_tmpFC[0]=& _tmpFE;({unsigned int _tmp697=exp_loc;struct _dyneither_ptr _tmp696=({const char*_tmpFD="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmpFD,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp697,_tmp696,_tag_dyneither(_tmpFC,sizeof(void*),1U));});});}_LL11:;}
# 876
({union Cyc_CfFlowInfo_FlowInfo _tmp698=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp103,*_tmp106,_tmp105,_tmp102,pat_loc);inflow=_tmp698;});
goto _LLC;}else{_LLF: _LL10:
# 879
 goto _LLC;}_LLC:;};};};}}
# 884
return inflow;};};}
# 887
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 889
void*_tmp107=e->r;void*_tmp108=_tmp107;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;switch(*((int*)_tmp108)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp108)->f1)){case 4U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: goto _LL11;}case 20U: _LL7: _tmp10B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp108)->f1;_LL8:
 _tmp10C=_tmp10B;goto _LLA;case 21U: _LL9: _tmp10C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp108)->f1;_LLA:
 _tmp10D=_tmp10C;goto _LLC;case 22U: _LLB: _tmp10D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp108)->f1;_LLC:
# 896
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp10D,cast_ok);case 23U: _LLD: _tmp10E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp108)->f1;_LLE: {
# 898
void*_tmp109=Cyc_Tcutil_compress((void*)_check_null(_tmp10E->topt));void*_tmp10A=_tmp109;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp10A)->tag == 10U){_LL14: _LL15:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp10E,cast_ok);}else{_LL16: _LL17:
 return 0;}_LL13:;}case 14U: _LLF: _tmp10F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp108)->f2;_LL10:
# 903
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp10F,cast_ok);else{
return 0;}default: _LL11: _LL12:
 return 0;}_LL0:;}
# 909
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp110=*rop;union Cyc_Relations_RelnOp _tmp111=_tmp110;unsigned int _tmp115;unsigned int _tmp114;switch((_tmp111.RParamNumelts).tag){case 5U: _LL1: _tmp114=(_tmp111.RParam).val;_LL2: {
# 912
struct Cyc_Absyn_Exp*_tmp112=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp114);
return Cyc_Relations_exp2relnop(_tmp112,rop);}case 6U: _LL3: _tmp115=(_tmp111.RParamNumelts).val;_LL4: {
# 915
struct Cyc_Absyn_Exp*_tmp113=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp115);
return({struct Cyc_Absyn_Exp*_tmp699=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp113,0U);Cyc_Relations_exp2relnop(_tmp699,rop);});}default: _LL5: _LL6:
 return 1;}_LL0:;}
# 921
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp116=rop;unsigned int _tmp118;unsigned int _tmp117;switch((_tmp116.RParamNumelts).tag){case 5U: _LL1: _tmp117=(_tmp116.RParam).val;_LL2:
# 924
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp117));case 6U: _LL3: _tmp118=(_tmp116.RParamNumelts).val;_LL4:
# 926
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp118));default: _LL5: _LL6:
 return Cyc_Relations_rop2string(rop);}_LL0:;}
# 931
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 934
union Cyc_CfFlowInfo_FlowInfo _tmp119=inflow;struct Cyc_Dict_Dict _tmp123;struct Cyc_List_List*_tmp122;if((_tmp119.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp123=((_tmp119.ReachableFL).val).f1;_tmp122=((_tmp119.ReachableFL).val).f2;_LL4:
# 937
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp11A=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp11A->rop1;
union Cyc_Relations_RelnOp rop2=_tmp11A->rop2;
enum Cyc_Relations_Relation _tmp11B=Cyc_Relations_flip_relation(_tmp11A->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,_tmp11B,rop1,_tmp122))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp11A->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp11E=({struct Cyc_String_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=0U,_tmp5B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp5B0;});struct Cyc_String_pa_PrintArg_struct _tmp11F=({struct Cyc_String_pa_PrintArg_struct _tmp5AF;_tmp5AF.tag=0U,_tmp5AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp5AF;});struct Cyc_String_pa_PrintArg_struct _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp5AE;_tmp5AE.tag=0U,_tmp5AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp5AE;});struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp5AD;_tmp5AD.tag=0U,({
struct _dyneither_ptr _tmp69A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_relns2string(_tmp122));_tmp5AD.f1=_tmp69A;});_tmp5AD;});void*_tmp11C[4U];_tmp11C[0]=& _tmp11E,_tmp11C[1]=& _tmp11F,_tmp11C[2]=& _tmp120,_tmp11C[3]=& _tmp121;({unsigned int _tmp69C=loc;struct _dyneither_ptr _tmp69B=({const char*_tmp11D="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp11D,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp69C,_tmp69B,_tag_dyneither(_tmp11C,sizeof(void*),4U));});});
break;}}
# 952
goto _LL0;}_LL0:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 957
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 961
struct Cyc_CfFlowInfo_FlowEnv*_tmp124=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 974
{union Cyc_CfFlowInfo_FlowInfo _tmp125=inflow;struct Cyc_Dict_Dict _tmp127;struct Cyc_List_List*_tmp126;if((_tmp125.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple16 _tmp5B1;({union Cyc_CfFlowInfo_FlowInfo _tmp69D=Cyc_CfFlowInfo_BottomFL();_tmp5B1.f1=_tmp69D;}),_tmp5B1.f2=_tmp124->unknown_all;_tmp5B1;});}else{_LL3: _tmp127=((_tmp125.ReachableFL).val).f1;_tmp126=((_tmp125.ReachableFL).val).f2;_LL4:
 d=_tmp127;relns=_tmp126;}_LL0:;}
# 989 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1010 "new_control_flow.cyc"
struct _tuple18 _tmp128=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp129=_tmp128;union Cyc_CfFlowInfo_FlowInfo _tmp135;union Cyc_CfFlowInfo_AbsLVal _tmp134;_LL6: _tmp135=_tmp129.f1;_tmp134=_tmp129.f2;_LL7:;{
struct _tuple18 _tmp12A=({struct _tuple18 _tmp5B5;_tmp5B5.f1=_tmp135,_tmp5B5.f2=_tmp134;_tmp5B5;});struct _tuple18 _tmp12B=_tmp12A;struct Cyc_Dict_Dict _tmp133;struct Cyc_List_List*_tmp132;struct Cyc_CfFlowInfo_Place*_tmp131;if(((_tmp12B.f1).ReachableFL).tag == 2){if(((_tmp12B.f2).PlaceL).tag == 1){_LL9: _tmp133=(((_tmp12B.f1).ReachableFL).val).f1;_tmp132=(((_tmp12B.f1).ReachableFL).val).f2;_tmp131=((_tmp12B.f2).PlaceL).val;_LLA: {
# 1013
void*_tmp12C=Cyc_CfFlowInfo_lookup_place(_tmp133,_tmp131);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp12C,& needs_unconsume)){
({void*_tmp12D=0U;({unsigned int _tmp69F=e->loc;struct _dyneither_ptr _tmp69E=({const char*_tmp12E="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp12E,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp69F,_tmp69E,_tag_dyneither(_tmp12D,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5B2;({union Cyc_CfFlowInfo_FlowInfo _tmp6A0=Cyc_CfFlowInfo_BottomFL();_tmp5B2.f1=_tmp6A0;}),_tmp5B2.f2=_tmp124->unknown_all;_tmp5B2;});}else{
# 1019
if(needs_unconsume)
# 1021
return({struct _tuple16 _tmp5B3;_tmp5B3.f1=_tmp135,({void*_tmp6A1=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp124,_tmp12C);_tmp5B3.f2=_tmp6A1;});_tmp5B3;});else{
# 1024
void*_tmp12F=Cyc_CfFlowInfo_make_unique_consumed(_tmp124,(void*)_check_null(e->topt),e,env->iteration_num,_tmp12C);
struct Cyc_Dict_Dict _tmp130=Cyc_CfFlowInfo_assign_place(_tmp124,e->loc,_tmp133,_tmp131,_tmp12F);
# 1036
return({struct _tuple16 _tmp5B4;({union Cyc_CfFlowInfo_FlowInfo _tmp6A2=Cyc_CfFlowInfo_ReachableFL(_tmp130,_tmp132);_tmp5B4.f1=_tmp6A2;}),_tmp5B4.f2=_tmp12C;_tmp5B4;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1040
 goto _LL8;}_LL8:;};}{
# 1043
void*_tmp136=e->r;void*_tmp137=_tmp136;struct Cyc_Absyn_Stmt*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2ED;void*_tmp2EC;int _tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2E8;int _tmp2E7;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List*_tmp2E5;enum Cyc_Absyn_AggrKind _tmp2E4;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E2;struct Cyc_List_List*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;struct _dyneither_ptr*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct _dyneither_ptr*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;struct _dyneither_ptr*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;int _tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;void**_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;int _tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;enum Cyc_Absyn_Primop _tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Datatypedecl*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B7;struct Cyc_Absyn_Vardecl*_tmp2B6;struct _dyneither_ptr _tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B1;switch(*((int*)_tmp137)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp137)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp2B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LLF: {
# 1047
struct _tuple16 _tmp138=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B1);struct _tuple16 _tmp139=_tmp138;union Cyc_CfFlowInfo_FlowInfo _tmp13F;void*_tmp13E;_LL7D: _tmp13F=_tmp139.f1;_tmp13E=_tmp139.f2;_LL7E:;{
struct _tuple16 _tmp13A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp13F,_tmp2B1,_tmp13E);struct _tuple16 _tmp13B=_tmp13A;union Cyc_CfFlowInfo_FlowInfo _tmp13D;void*_tmp13C;_LL80: _tmp13D=_tmp13B.f1;_tmp13C=_tmp13B.f2;_LL81:;
return({struct _tuple16 _tmp5B6;_tmp5B6.f1=_tmp13D,_tmp5B6.f2=_tmp13E;_tmp5B6;});};}}else{_LL10: _tmp2B2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL11:
# 1053
 _tmp2B3=_tmp2B2;goto _LL13;}case 12U: _LL12: _tmp2B3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL13:
 _tmp2B4=_tmp2B3;goto _LL15;case 13U: _LL14: _tmp2B4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL15:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B4);case 2U: _LL16: _tmp2B5=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL17:
# 1058
 if(!({struct _dyneither_ptr _tmp6A3=(struct _dyneither_ptr)_tmp2B5;Cyc_NewControlFlow_strcmp(_tmp6A3,({const char*_tmp140="print_flow";_tag_dyneither(_tmp140,sizeof(char),11U);}));})){
struct _dyneither_ptr _tmp141=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp144=({struct Cyc_String_pa_PrintArg_struct _tmp5B7;_tmp5B7.tag=0U,_tmp5B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp141);_tmp5B7;});void*_tmp142[1U];_tmp142[0]=& _tmp144;({struct Cyc___cycFILE*_tmp6A5=Cyc_stderr;struct _dyneither_ptr _tmp6A4=({const char*_tmp143="%s: current flow is\n";_tag_dyneither(_tmp143,sizeof(char),21U);});Cyc_fprintf(_tmp6A5,_tmp6A4,_tag_dyneither(_tmp142,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp145=0U;({struct Cyc___cycFILE*_tmp6A7=Cyc_stderr;struct _dyneither_ptr _tmp6A6=({const char*_tmp146="\n";_tag_dyneither(_tmp146,sizeof(char),2U);});Cyc_fprintf(_tmp6A7,_tmp6A6,_tag_dyneither(_tmp145,sizeof(void*),0U));});});}else{
# 1064
if(!({struct _dyneither_ptr _tmp6A8=(struct _dyneither_ptr)_tmp2B5;Cyc_NewControlFlow_strcmp(_tmp6A8,({const char*_tmp147="debug_on";_tag_dyneither(_tmp147,sizeof(char),9U);}));}));else{
# 1069
if(!({struct _dyneither_ptr _tmp6A9=(struct _dyneither_ptr)_tmp2B5;Cyc_NewControlFlow_strcmp(_tmp6A9,({const char*_tmp148="debug_off";_tag_dyneither(_tmp148,sizeof(char),10U);}));}));else{
# 1075
({struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp5B8;_tmp5B8.tag=0U,_tmp5B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2B5);_tmp5B8;});void*_tmp149[1U];_tmp149[0]=& _tmp14B;({unsigned int _tmp6AB=e->loc;struct _dyneither_ptr _tmp6AA=({const char*_tmp14A="unknown pragma %s";_tag_dyneither(_tmp14A,sizeof(char),18U);});Cyc_Warn_warn(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp149,sizeof(void*),1U));});});}}}
return({struct _tuple16 _tmp5B9;_tmp5B9.f1=inflow,_tmp5B9.f2=_tmp124->zero;_tmp5B9;});case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp137)->f1).Wstring_c).tag){case 1U: _LL18: _LL19:
# 1078
 goto _LL1B;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp137)->f1).Int_c).val).f2 == 0){_LL1A: _LL1B:
 return({struct _tuple16 _tmp5BA;_tmp5BA.f1=inflow,_tmp5BA.f2=_tmp124->zero;_tmp5BA;});}else{_LL1C: _LL1D:
 goto _LL1F;}case 8U: _LL1E: _LL1F:
 goto _LL21;case 9U: _LL20: _LL21:
 goto _LL23;default: _LL26: _LL27:
# 1086
 goto _LL29;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp137)->f1)){case 2U: _LL22: _LL23:
# 1083
 return({struct _tuple16 _tmp5BB;_tmp5BB.f1=inflow,_tmp5BB.f2=_tmp124->notzeroall;_tmp5BB;});case 1U: _LL32: _LL33:
# 1094
 return({struct _tuple16 _tmp5BC;_tmp5BC.f1=inflow,({void*_tmp6AC=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5BC.f2=_tmp6AC;});_tmp5BC;});case 3U: _LL34: _tmp2B6=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp137)->f1)->f1;_LL35:
# 1097
 _tmp2B7=_tmp2B6;goto _LL37;case 4U: _LL36: _tmp2B7=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp137)->f1)->f1;_LL37:
 _tmp2B8=_tmp2B7;goto _LL39;case 5U: _LL38: _tmp2B8=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp137)->f1)->f1;_LL39: {
# 1101
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5BE;_tmp5BE.tag=0U,_tmp5BE.f1=_tmp2B8;_tmp5BE;});
return({struct _tuple16 _tmp5BD;_tmp5BD.f1=inflow,({void*_tmp6AE=({struct Cyc_Dict_Dict _tmp6AD=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp6AD,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));*_tmp14C=vdroot;_tmp14C;}));});_tmp5BD.f2=_tmp6AE;});_tmp5BD;});}default: _LL72: _LL73:
# 1681
 goto _LL75;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp137)->f1 == 0){_LL24: _LL25:
# 1085
 goto _LL27;}else{_LL60: _tmp2BA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2B9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL61:
# 1544
 _tmp2E1=_tmp2BA;goto _LL63;}case 18U: _LL28: _LL29:
# 1087
 goto _LL2B;case 17U: _LL2A: _LL2B:
 goto _LL2D;case 19U: _LL2C: _LL2D:
 goto _LL2F;case 33U: _LL2E: _LL2F:
 goto _LL31;case 32U: _LL30: _LL31:
 return({struct _tuple16 _tmp5BF;_tmp5BF.f1=inflow,_tmp5BF.f2=_tmp124->unknown_all;_tmp5BF;});case 3U: _LL3A: _tmp2BC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2BB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL3B: {
# 1108
struct _tuple16 _tmp14D=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp2BB);struct _tuple16 _tmp14E=_tmp14D;union Cyc_CfFlowInfo_FlowInfo _tmp150;void*_tmp14F;_LL83: _tmp150=_tmp14E.f1;_tmp14F=_tmp14E.f2;_LL84:;
return({struct _tuple16 _tmp5C0;_tmp5C0.f1=_tmp150,_tmp5C0.f2=_tmp14F;_tmp5C0;});}case 5U: _LL3C: _tmp2BD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL3D: {
# 1112
struct Cyc_List_List _tmp151=({struct Cyc_List_List _tmp5C4;_tmp5C4.hd=_tmp2BD,_tmp5C4.tl=0;_tmp5C4;});
struct _tuple16 _tmp152=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp151);struct _tuple16 _tmp153=_tmp152;union Cyc_CfFlowInfo_FlowInfo _tmp160;_LL86: _tmp160=_tmp153.f1;_LL87:;{
struct _tuple18 _tmp154=Cyc_NewControlFlow_anal_Lexp(env,_tmp160,0,0,_tmp2BD);struct _tuple18 _tmp155=_tmp154;union Cyc_CfFlowInfo_AbsLVal _tmp15F;_LL89: _tmp15F=_tmp155.f2;_LL8A:;{
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp2BD->topt));
if(iszt){
# 1119
struct _tuple16 _tmp156=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2BD);struct _tuple16 _tmp157=_tmp156;union Cyc_CfFlowInfo_FlowInfo _tmp159;void*_tmp158;_LL8C: _tmp159=_tmp157.f1;_tmp158=_tmp157.f2;_LL8D:;
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp159,_tmp2BD,_tmp158);}
# 1122
{struct _tuple25 _tmp15A=({struct _tuple25 _tmp5C1;_tmp5C1.f1=_tmp15F,_tmp5C1.f2=_tmp160;_tmp5C1;});struct _tuple25 _tmp15B=_tmp15A;struct Cyc_CfFlowInfo_Place*_tmp15E;struct Cyc_Dict_Dict _tmp15D;struct Cyc_List_List*_tmp15C;if(((_tmp15B.f1).PlaceL).tag == 1){if(((_tmp15B.f2).ReachableFL).tag == 2){_LL8F: _tmp15E=((_tmp15B.f1).PlaceL).val;_tmp15D=(((_tmp15B.f2).ReachableFL).val).f1;_tmp15C=(((_tmp15B.f2).ReachableFL).val).f2;_LL90:
# 1124
({struct Cyc_List_List*_tmp6AF=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp15C,_tmp2BD);_tmp15C=_tmp6AF;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6B1=({struct Cyc_Dict_Dict _tmp6B0=Cyc_CfFlowInfo_assign_place(_tmp124,_tmp2BD->loc,_tmp15D,_tmp15E,_tmp124->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6B0,_tmp15C);});_tmp160=_tmp6B1;});
# 1129
goto _LL8E;}else{goto _LL91;}}else{_LL91: _LL92:
 goto _LL8E;}_LL8E:;}
# 1133
if(iszt)
return({struct _tuple16 _tmp5C2;_tmp5C2.f1=_tmp160,_tmp5C2.f2=_tmp124->notzeroall;_tmp5C2;});else{
return({struct _tuple16 _tmp5C3;_tmp5C3.f1=_tmp160,_tmp5C3.f2=_tmp124->unknown_all;_tmp5C3;});}};};}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp137)->f2 != 0){_LL3E: _tmp2BF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2BE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp137)->f3;_LL3F:
# 1138
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp161=0U;({unsigned int _tmp6B3=e->loc;struct _dyneither_ptr _tmp6B2=({const char*_tmp162="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp162,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6B3,_tmp6B2,_tag_dyneither(_tmp161,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5C5;({union Cyc_CfFlowInfo_FlowInfo _tmp6B4=Cyc_CfFlowInfo_BottomFL();_tmp5C5.f1=_tmp6B4;}),_tmp5C5.f2=_tmp124->unknown_all;_tmp5C5;});}{
# 1142
struct _tuple16 _tmp163=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2BE);struct _tuple16 _tmp164=_tmp163;union Cyc_CfFlowInfo_FlowInfo _tmp16F;void*_tmp16E;_LL94: _tmp16F=_tmp164.f1;_tmp16E=_tmp164.f2;_LL95:;{
struct _tuple18 _tmp165=Cyc_NewControlFlow_anal_Lexp(env,_tmp16F,0,0,_tmp2BF);struct _tuple18 _tmp166=_tmp165;union Cyc_CfFlowInfo_FlowInfo _tmp16D;union Cyc_CfFlowInfo_AbsLVal _tmp16C;_LL97: _tmp16D=_tmp166.f1;_tmp16C=_tmp166.f2;_LL98:;
{union Cyc_CfFlowInfo_FlowInfo _tmp167=_tmp16D;struct Cyc_Dict_Dict _tmp16B;struct Cyc_List_List*_tmp16A;if((_tmp167.ReachableFL).tag == 2){_LL9A: _tmp16B=((_tmp167.ReachableFL).val).f1;_tmp16A=((_tmp167.ReachableFL).val).f2;_LL9B:
# 1146
{union Cyc_CfFlowInfo_AbsLVal _tmp168=_tmp16C;struct Cyc_CfFlowInfo_Place*_tmp169;if((_tmp168.PlaceL).tag == 1){_LL9F: _tmp169=(_tmp168.PlaceL).val;_LLA0:
# 1150
({struct Cyc_List_List*_tmp6B5=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp16A,_tmp2BF);_tmp16A=_tmp6B5;});
({struct Cyc_Dict_Dict _tmp6B6=Cyc_CfFlowInfo_assign_place(_tmp124,_tmp2BF->loc,_tmp16B,_tmp169,_tmp124->unknown_all);_tmp16B=_tmp6B6;});
# 1153
({union Cyc_CfFlowInfo_FlowInfo _tmp6B7=Cyc_CfFlowInfo_ReachableFL(_tmp16B,_tmp16A);_tmp16D=_tmp6B7;});
# 1157
goto _LL9E;}else{_LLA1: _LLA2:
# 1160
 goto _LL9E;}_LL9E:;}
# 1162
goto _LL99;}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 1165
return({struct _tuple16 _tmp5C6;_tmp5C6.f1=_tmp16D,_tmp5C6.f2=_tmp124->unknown_all;_tmp5C6;});};};}else{_LL40: _tmp2C1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2C0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp137)->f3;_LL41:
# 1168
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp170=0U;({unsigned int _tmp6B9=e->loc;struct _dyneither_ptr _tmp6B8=({const char*_tmp171="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp171,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6B9,_tmp6B8,_tag_dyneither(_tmp170,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5C7;({union Cyc_CfFlowInfo_FlowInfo _tmp6BA=Cyc_CfFlowInfo_BottomFL();_tmp5C7.f1=_tmp6BA;}),_tmp5C7.f2=_tmp124->unknown_all;_tmp5C7;});}
# 1174
({union Cyc_CfFlowInfo_FlowInfo _tmp6BD=({struct Cyc_NewControlFlow_AnalEnv*_tmp6BC=env;union Cyc_CfFlowInfo_FlowInfo _tmp6BB=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6BC,_tmp6BB,({struct Cyc_Absyn_Exp*_tmp172[2U];_tmp172[0]=_tmp2C1,_tmp172[1]=_tmp2C0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp172,sizeof(struct Cyc_Absyn_Exp*),2U));}));});inflow=_tmp6BD;});{
# 1176
struct _tuple16 _tmp173=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C0);struct _tuple16 _tmp174=_tmp173;union Cyc_CfFlowInfo_FlowInfo _tmp17E;void*_tmp17D;_LLA4: _tmp17E=_tmp174.f1;_tmp17D=_tmp174.f2;_LLA5:;{
struct _tuple18 _tmp175=Cyc_NewControlFlow_anal_Lexp(env,_tmp17E,0,0,_tmp2C1);struct _tuple18 _tmp176=_tmp175;union Cyc_CfFlowInfo_FlowInfo _tmp17C;union Cyc_CfFlowInfo_AbsLVal _tmp17B;_LLA7: _tmp17C=_tmp176.f1;_tmp17B=_tmp176.f2;_LLA8:;{
struct _tuple16 _tmp177=Cyc_NewControlFlow_do_assign(_tmp124,env,_tmp17C,_tmp2C1,_tmp17B,_tmp2C0,_tmp17D,e->loc);struct _tuple16 _tmp178=_tmp177;union Cyc_CfFlowInfo_FlowInfo _tmp17A;void*_tmp179;_LLAA: _tmp17A=_tmp178.f1;_tmp179=_tmp178.f2;_LLAB:;
# 1182
return({struct _tuple16 _tmp5C8;_tmp5C8.f1=_tmp17A,_tmp5C8.f2=_tmp179;_tmp5C8;});};};};}case 9U: _LL42: _tmp2C3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2C2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL43: {
# 1185
struct _tuple16 _tmp17F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2C3);struct _tuple16 _tmp180=_tmp17F;union Cyc_CfFlowInfo_FlowInfo _tmp182;void*_tmp181;_LLAD: _tmp182=_tmp180.f1;_tmp181=_tmp180.f2;_LLAE:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp182,_tmp2C2);}case 11U: _LL44: _tmp2C4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL45: {
# 1189
struct _tuple16 _tmp183=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C4);struct _tuple16 _tmp184=_tmp183;union Cyc_CfFlowInfo_FlowInfo _tmp186;void*_tmp185;_LLB0: _tmp186=_tmp184.f1;_tmp185=_tmp184.f2;_LLB1:;
Cyc_NewControlFlow_use_Rval(env,_tmp2C4->loc,_tmp186,_tmp185);
return({struct _tuple16 _tmp5C9;({union Cyc_CfFlowInfo_FlowInfo _tmp6BF=Cyc_CfFlowInfo_BottomFL();_tmp5C9.f1=_tmp6BF;}),({void*_tmp6BE=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5C9.f2=_tmp6BE;});_tmp5C9;});}case 10U: _LL46: _tmp2C6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2C5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL47: {
# 1194
struct Cyc_List_List*_tmp187=_tmp2C5;
struct _tuple23 _tmp188=({
struct Cyc_NewControlFlow_AnalEnv*_tmp6C2=env;union Cyc_CfFlowInfo_FlowInfo _tmp6C1=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6C2,_tmp6C1,({struct Cyc_List_List*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->hd=_tmp2C6,({struct Cyc_List_List*_tmp6C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp2C5);_tmp1B6->tl=_tmp6C0;});_tmp1B6;}),0,1);});
# 1195
struct _tuple23 _tmp189=_tmp188;union Cyc_CfFlowInfo_FlowInfo _tmp1B5;struct Cyc_List_List*_tmp1B4;_LLB3: _tmp1B5=_tmp189.f1;_tmp1B4=_tmp189.f2;_LLB4:;{
# 1198
union Cyc_CfFlowInfo_FlowInfo _tmp18A=Cyc_NewControlFlow_use_Rval(env,_tmp2C6->loc,_tmp1B5,(void*)((struct Cyc_List_List*)_check_null(_tmp1B4))->hd);
_tmp1B4=_tmp1B4->tl;{
# 1201
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp18B=Cyc_Tcutil_compress((void*)_check_null(_tmp2C6->topt));void*_tmp18C=_tmp18B;void*_tmp19E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18C)->tag == 5U){_LLB6: _tmp19E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18C)->f1).elt_typ;_LLB7:
# 1208
{void*_tmp18D=Cyc_Tcutil_compress(_tmp19E);void*_tmp18E=_tmp18D;struct Cyc_List_List*_tmp19B;struct Cyc_List_List*_tmp19A;struct Cyc_List_List*_tmp199;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18E)->tag == 9U){_LLBB: _tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18E)->f1).attributes;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18E)->f1).requires_relns;_tmp199=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18E)->f1).ensures_relns;_LLBC:
# 1210
 requires=_tmp19A;
ensures=_tmp199;
for(0;_tmp19B != 0;_tmp19B=_tmp19B->tl){
# 1214
void*_tmp18F=(void*)_tmp19B->hd;void*_tmp190=_tmp18F;int _tmp196;int _tmp195;int _tmp194;switch(*((int*)_tmp190)){case 20U: _LLC0: _tmp194=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp190)->f1;_LLC1:
# 1216
({struct Cyc_List_List*_tmp6C3=({struct Cyc_List_List*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->hd=(void*)_tmp194,_tmp191->tl=init_params;_tmp191;});init_params=_tmp6C3;});goto _LLBF;case 21U: _LLC2: _tmp195=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp190)->f1;_LLC3:
# 1218
({struct Cyc_List_List*_tmp6C4=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=(void*)_tmp195,_tmp192->tl=nolive_unique_params;_tmp192;});nolive_unique_params=_tmp6C4;});
goto _LLBF;case 22U: _LLC4: _tmp196=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp190)->f1;_LLC5:
# 1222
({struct Cyc_List_List*_tmp6C5=({struct Cyc_List_List*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=(void*)_tmp196,_tmp193->tl=noconsume_params;_tmp193;});noconsume_params=_tmp6C5;});
goto _LLBF;default: _LLC6: _LLC7:
 goto _LLBF;}_LLBF:;}
# 1226
goto _LLBA;}else{_LLBD: _LLBE:
({void*_tmp197=0U;({struct _dyneither_ptr _tmp6C6=({const char*_tmp198="anal_Rexp: bad function type";_tag_dyneither(_tmp198,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6C6,_tag_dyneither(_tmp197,sizeof(void*),0U));});});}_LLBA:;}
# 1229
goto _LLB5;}else{_LLB8: _LLB9:
({void*_tmp19C=0U;({struct _dyneither_ptr _tmp6C7=({const char*_tmp19D="anal_Rexp: bad function type";_tag_dyneither(_tmp19D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6C7,_tag_dyneither(_tmp19C,sizeof(void*),0U));});});}_LLB5:;}
# 1232
{int i=1;for(0;_tmp1B4 != 0;(_tmp1B4=_tmp1B4->tl,_tmp2C5=((struct Cyc_List_List*)_check_null(_tmp2C5))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp19F=_tmp1B5;struct Cyc_Dict_Dict _tmp1B3;if((_tmp19F.BottomFL).tag == 1){_LLC9: _LLCA:
 goto _LLC8;}else{_LLCB: _tmp1B3=((_tmp19F.ReachableFL).val).f1;_LLCC: {
# 1237
struct _tuple15 _tmp1A0=Cyc_CfFlowInfo_unname_rval((void*)_tmp1B4->hd);struct _tuple15 _tmp1A1=_tmp1A0;void*_tmp1B2;_LLCE: _tmp1B2=_tmp1A1.f1;_LLCF:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1B3,(void*)_tmp1B4->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1B4->hd))
({void*_tmp1A2=0U;({unsigned int _tmp6C9=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->loc;struct _dyneither_ptr _tmp6C8=({const char*_tmp1A3="expression may not be initialized";_tag_dyneither(_tmp1A3,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6C9,_tmp6C8,_tag_dyneither(_tmp1A2,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1A4=_tmp18A;struct Cyc_Dict_Dict _tmp1B1;struct Cyc_List_List*_tmp1B0;if((_tmp1A4.BottomFL).tag == 1){_LLD1: _LLD2:
 goto _LLD0;}else{_LLD3: _tmp1B1=((_tmp1A4.ReachableFL).val).f1;_tmp1B0=((_tmp1A4.ReachableFL).val).f2;_LLD4: {
# 1245
struct Cyc_Dict_Dict _tmp1A5=Cyc_CfFlowInfo_escape_deref(_tmp124,_tmp1B1,(void*)_tmp1B4->hd);
{void*_tmp1A6=(void*)_tmp1B4->hd;void*_tmp1A7=_tmp1A6;struct Cyc_CfFlowInfo_Place*_tmp1AF;switch(*((int*)_tmp1A7)){case 4U: _LLD6: _tmp1AF=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A7)->f1;_LLD7:
# 1248
{void*_tmp1A8=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->topt));void*_tmp1A9=_tmp1A8;void*_tmp1AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A9)->tag == 5U){_LLDD: _tmp1AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A9)->f1).elt_typ;_LLDE:
# 1250
({struct Cyc_Dict_Dict _tmp6CE=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6CD=_tmp124;unsigned int _tmp6CC=((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->loc;struct Cyc_Dict_Dict _tmp6CB=_tmp1A5;struct Cyc_CfFlowInfo_Place*_tmp6CA=_tmp1AF;Cyc_CfFlowInfo_assign_place(_tmp6CD,_tmp6CC,_tmp6CB,_tmp6CA,
Cyc_CfFlowInfo_typ_to_absrval(_tmp124,_tmp1AC,0,_tmp124->esc_all));});
# 1250
_tmp1A5=_tmp6CE;});
# 1254
goto _LLDC;}else{_LLDF: _LLE0:
({void*_tmp1AA=0U;({struct _dyneither_ptr _tmp6CF=({const char*_tmp1AB="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1AB,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6CF,_tag_dyneither(_tmp1AA,sizeof(void*),0U));});});}_LLDC:;}
# 1257
goto _LLD5;case 5U: _LLD8: _LLD9:
# 1259
({void*_tmp1AD=0U;({struct _dyneither_ptr _tmp6D0=({const char*_tmp1AE="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1AE,sizeof(char),68U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6D0,_tag_dyneither(_tmp1AD,sizeof(void*),0U));});});default: _LLDA: _LLDB:
# 1261
 goto _LLD5;}_LLD5:;}
# 1263
({union Cyc_CfFlowInfo_FlowInfo _tmp6D1=Cyc_CfFlowInfo_ReachableFL(_tmp1A5,_tmp1B0);_tmp18A=_tmp6D1;});
goto _LLD0;}}_LLD0:;}
# 1266
goto _LLC8;}}_LLC8:;}else{
# 1269
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1274
({union Cyc_CfFlowInfo_FlowInfo _tmp6D6=({struct Cyc_NewControlFlow_AnalEnv*_tmp6D5=env;unsigned int _tmp6D4=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->loc;void*_tmp6D3=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6D2=_tmp18A;Cyc_NewControlFlow_use_nounique_Rval(_tmp6D5,_tmp6D4,_tmp6D3,_tmp6D2,(void*)_tmp1B4->hd);});_tmp18A=_tmp6D6;});else{
# 1284
({union Cyc_CfFlowInfo_FlowInfo _tmp6D7=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->loc,_tmp18A,(void*)_tmp1B4->hd);_tmp18A=_tmp6D7;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp6D8=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp18A,(struct Cyc_Absyn_Exp*)_tmp2C5->hd,((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->loc,(void*)_tmp1B4->hd);_tmp18A=_tmp6D8;});}}}}
# 1291
Cyc_NewControlFlow_check_fn_requires(env,_tmp18A,_tmp187,requires,e->loc);
# 1294
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp2C6->topt)))
return({struct _tuple16 _tmp5CA;({union Cyc_CfFlowInfo_FlowInfo _tmp6DA=Cyc_CfFlowInfo_BottomFL();_tmp5CA.f1=_tmp6DA;}),({void*_tmp6D9=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5CA.f2=_tmp6D9;});_tmp5CA;});else{
# 1297
return({struct _tuple16 _tmp5CB;_tmp5CB.f1=_tmp18A,({void*_tmp6DB=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5CB.f2=_tmp6DB;});_tmp5CB;});}};};}case 34U: _LL48: _tmp2CB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp137)->f1).is_calloc;_tmp2CA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp137)->f1).rgn;_tmp2C9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp137)->f1).elt_type;_tmp2C8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp137)->f1).num_elts;_tmp2C7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp137)->f1).fat_result;_LL49: {
# 1300
void*place_val;
if(_tmp2C7)place_val=_tmp124->notzeroall;else{
if(_tmp2CB)({void*_tmp6DC=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,*((void**)_check_null(_tmp2C9)),0,_tmp124->zero);place_val=_tmp6DC;});else{
({void*_tmp6DD=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,*((void**)_check_null(_tmp2C9)),0,_tmp124->unknown_none);place_val=_tmp6DD;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp2CA != 0){
struct _tuple23 _tmp1B7=({struct Cyc_NewControlFlow_AnalEnv*_tmp6DF=env;union Cyc_CfFlowInfo_FlowInfo _tmp6DE=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6DF,_tmp6DE,({struct Cyc_Absyn_Exp*_tmp1BB[2U];_tmp1BB[0]=_tmp2CA,_tmp1BB[1]=_tmp2C8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1BB,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1B8=_tmp1B7;union Cyc_CfFlowInfo_FlowInfo _tmp1BA;struct Cyc_List_List*_tmp1B9;_LLE2: _tmp1BA=_tmp1B8.f1;_tmp1B9=_tmp1B8.f2;_LLE3:;
outflow=_tmp1BA;}else{
# 1309
struct _tuple16 _tmp1BC=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C8);struct _tuple16 _tmp1BD=_tmp1BC;union Cyc_CfFlowInfo_FlowInfo _tmp1BE;_LLE5: _tmp1BE=_tmp1BD.f1;_LLE6:;
outflow=_tmp1BE;}
# 1314
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp5CC;_tmp5CC.f1=outflow,({void*_tmp6E0=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->tag=5U,_tmp1BF->f1=place_val;_tmp1BF;});_tmp5CC.f2=_tmp6E0;});_tmp5CC;});else{
# 1318
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->tag=1U,_tmp1C5->f1=_tmp2C8,_tmp1C5->f2=(void*)_check_null(e->topt);_tmp1C5;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->root=root,_tmp1C4->path=0;_tmp1C4;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->tag=4U,_tmp1C3->f1=place;_tmp1C3;});
union Cyc_CfFlowInfo_FlowInfo _tmp1C0=outflow;struct Cyc_Dict_Dict _tmp1C2;struct Cyc_List_List*_tmp1C1;if((_tmp1C0.BottomFL).tag == 1){_LLE8: _LLE9:
 return({struct _tuple16 _tmp5CD;_tmp5CD.f1=outflow,_tmp5CD.f2=rval;_tmp5CD;});}else{_LLEA: _tmp1C2=((_tmp1C0.ReachableFL).val).f1;_tmp1C1=((_tmp1C0.ReachableFL).val).f2;_LLEB:
# 1324
 return({struct _tuple16 _tmp5CE;({union Cyc_CfFlowInfo_FlowInfo _tmp6E2=({struct Cyc_Dict_Dict _tmp6E1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1C2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6E1,_tmp1C1);});_tmp5CE.f1=_tmp6E2;}),_tmp5CE.f2=rval;_tmp5CE;});}_LLE7:;}};}case 35U: _LL4A: _tmp2CD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2CC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL4B: {
# 1329
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1335
struct _tuple23 _tmp1C6=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E4=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E3=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E4,_tmp6E3,({struct Cyc_Absyn_Exp*_tmp1DB[2U];_tmp1DB[0]=_tmp2CD,_tmp1DB[1]=_tmp2CC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1DB,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple23 _tmp1C7=_tmp1C6;union Cyc_CfFlowInfo_FlowInfo _tmp1DA;struct Cyc_List_List*_tmp1D9;_LLED: _tmp1DA=_tmp1C7.f1;_tmp1D9=_tmp1C7.f2;_LLEE:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1D9))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1D9->tl))->hd;
outflow=_tmp1DA;{
# 1340
struct _tuple18 _tmp1C8=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2CD);struct _tuple18 _tmp1C9=_tmp1C8;union Cyc_CfFlowInfo_AbsLVal _tmp1D8;_LLF0: _tmp1D8=_tmp1C9.f2;_LLF1:;{
struct _tuple18 _tmp1CA=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2CC);struct _tuple18 _tmp1CB=_tmp1CA;union Cyc_CfFlowInfo_AbsLVal _tmp1D7;_LLF3: _tmp1D7=_tmp1CB.f2;_LLF4:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1CC=outflow;struct Cyc_Dict_Dict _tmp1D6;struct Cyc_List_List*_tmp1D5;if((_tmp1CC.ReachableFL).tag == 2){_LLF6: _tmp1D6=((_tmp1CC.ReachableFL).val).f1;_tmp1D5=((_tmp1CC.ReachableFL).val).f2;_LLF7:
# 1344
{union Cyc_CfFlowInfo_AbsLVal _tmp1CD=_tmp1D8;struct Cyc_CfFlowInfo_Place*_tmp1D0;if((_tmp1CD.PlaceL).tag == 1){_LLFB: _tmp1D0=(_tmp1CD.PlaceL).val;_LLFC:
# 1346
({struct Cyc_Dict_Dict _tmp6E5=Cyc_CfFlowInfo_assign_place(_tmp124,_tmp2CD->loc,_tmp1D6,_tmp1D0,right_rval);_tmp1D6=_tmp6E5;});
goto _LLFA;}else{_LLFD: _LLFE:
# 1352
 if(Cyc_CfFlowInfo_initlevel(_tmp124,_tmp1D6,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1CE=0U;({unsigned int _tmp6E7=_tmp2CC->loc;struct _dyneither_ptr _tmp6E6=({const char*_tmp1CF="expression may not be fully initialized";_tag_dyneither(_tmp1CF,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6E7,_tmp6E6,_tag_dyneither(_tmp1CE,sizeof(void*),0U));});});
goto _LLFA;}_LLFA:;}
# 1356
{union Cyc_CfFlowInfo_AbsLVal _tmp1D1=_tmp1D7;struct Cyc_CfFlowInfo_Place*_tmp1D4;if((_tmp1D1.PlaceL).tag == 1){_LL100: _tmp1D4=(_tmp1D1.PlaceL).val;_LL101:
# 1358
({struct Cyc_Dict_Dict _tmp6E8=Cyc_CfFlowInfo_assign_place(_tmp124,_tmp2CC->loc,_tmp1D6,_tmp1D4,left_rval);_tmp1D6=_tmp6E8;});
goto _LLFF;}else{_LL102: _LL103:
# 1361
 if(Cyc_CfFlowInfo_initlevel(_tmp124,_tmp1D6,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1D2=0U;({unsigned int _tmp6EA=_tmp2CD->loc;struct _dyneither_ptr _tmp6E9=({const char*_tmp1D3="expression may not be fully initialized";_tag_dyneither(_tmp1D3,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6EA,_tmp6E9,_tag_dyneither(_tmp1D2,sizeof(void*),0U));});});
goto _LLFF;}_LLFF:;}
# 1366
({struct Cyc_List_List*_tmp6EB=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1D5,_tmp2CD);_tmp1D5=_tmp6EB;});
({struct Cyc_List_List*_tmp6EC=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1D5,_tmp2CC);_tmp1D5=_tmp6EC;});
# 1369
({union Cyc_CfFlowInfo_FlowInfo _tmp6ED=Cyc_CfFlowInfo_ReachableFL(_tmp1D6,_tmp1D5);outflow=_tmp6ED;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LLF5;}else{_LLF8: _LLF9:
 goto _LLF5;}_LLF5:;}
# 1376
return({struct _tuple16 _tmp5CF;_tmp5CF.f1=outflow,_tmp5CF.f2=_tmp124->unknown_all;_tmp5CF;});};};}case 16U: _LL4C: _tmp2CF=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2CE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL4D: {
# 1379
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp2CF != 0){
struct _tuple23 _tmp1DC=({struct Cyc_NewControlFlow_AnalEnv*_tmp6EF=env;union Cyc_CfFlowInfo_FlowInfo _tmp6EE=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6EF,_tmp6EE,({struct Cyc_Absyn_Exp*_tmp1E0[2U];_tmp1E0[0]=_tmp2CF,_tmp1E0[1]=_tmp2CE;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E0,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1DD=_tmp1DC;union Cyc_CfFlowInfo_FlowInfo _tmp1DF;struct Cyc_List_List*_tmp1DE;_LL105: _tmp1DF=_tmp1DD.f1;_tmp1DE=_tmp1DD.f2;_LL106:;
outflow=_tmp1DF;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1DE))->tl))->hd;}else{
# 1386
struct _tuple16 _tmp1E1=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2CE);struct _tuple16 _tmp1E2=_tmp1E1;union Cyc_CfFlowInfo_FlowInfo _tmp1E4;void*_tmp1E3;_LL108: _tmp1E4=_tmp1E2.f1;_tmp1E3=_tmp1E2.f2;_LL109:;
outflow=_tmp1E4;
place_val=_tmp1E3;}
# 1391
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp5D0;_tmp5D0.f1=outflow,({void*_tmp6F0=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->tag=5U,_tmp1E5->f1=place_val;_tmp1E5;});_tmp5D0.f2=_tmp6F0;});_tmp5D0;});else{
# 1395
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->tag=1U,_tmp1EB->f1=_tmp2CE,_tmp1EB->f2=(void*)_check_null(e->topt);_tmp1EB;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->root=root,_tmp1EA->path=0;_tmp1EA;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->tag=4U,_tmp1E9->f1=place;_tmp1E9;});
union Cyc_CfFlowInfo_FlowInfo _tmp1E6=outflow;struct Cyc_Dict_Dict _tmp1E8;struct Cyc_List_List*_tmp1E7;if((_tmp1E6.BottomFL).tag == 1){_LL10B: _LL10C:
 return({struct _tuple16 _tmp5D1;_tmp5D1.f1=outflow,_tmp5D1.f2=rval;_tmp5D1;});}else{_LL10D: _tmp1E8=((_tmp1E6.ReachableFL).val).f1;_tmp1E7=((_tmp1E6.ReachableFL).val).f2;_LL10E:
# 1401
 return({struct _tuple16 _tmp5D2;({union Cyc_CfFlowInfo_FlowInfo _tmp6F2=({struct Cyc_Dict_Dict _tmp6F1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1E8,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6F1,_tmp1E7);});_tmp5D2.f1=_tmp6F2;}),_tmp5D2.f2=rval;_tmp5D2;});}_LL10A:;}}case 15U: _LL4E: _tmp2D0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL4F: {
# 1406
struct _tuple18 _tmp1EC=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp2D0);struct _tuple18 _tmp1ED=_tmp1EC;union Cyc_CfFlowInfo_FlowInfo _tmp1F2;union Cyc_CfFlowInfo_AbsLVal _tmp1F1;_LL110: _tmp1F2=_tmp1ED.f1;_tmp1F1=_tmp1ED.f2;_LL111:;{
union Cyc_CfFlowInfo_AbsLVal _tmp1EE=_tmp1F1;struct Cyc_CfFlowInfo_Place*_tmp1F0;if((_tmp1EE.UnknownL).tag == 2){_LL113: _LL114:
 return({struct _tuple16 _tmp5D3;_tmp5D3.f1=_tmp1F2,_tmp5D3.f2=_tmp124->notzeroall;_tmp5D3;});}else{_LL115: _tmp1F0=(_tmp1EE.PlaceL).val;_LL116:
 return({struct _tuple16 _tmp5D4;_tmp5D4.f1=_tmp1F2,({void*_tmp6F3=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->tag=4U,_tmp1EF->f1=_tmp1F0;_tmp1EF;});_tmp5D4.f2=_tmp6F3;});_tmp5D4;});}_LL112:;};}case 20U: _LL50: _tmp2D1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL51: {
# 1413
struct _tuple16 _tmp1F3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D1);struct _tuple16 _tmp1F4=_tmp1F3;union Cyc_CfFlowInfo_FlowInfo _tmp1F6;void*_tmp1F5;_LL118: _tmp1F6=_tmp1F4.f1;_tmp1F5=_tmp1F4.f2;_LL119:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1F6,_tmp2D1,_tmp1F5);}case 21U: _LL52: _tmp2D3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2D2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL53: {
# 1417
struct _tuple16 _tmp1F7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D3);struct _tuple16 _tmp1F8=_tmp1F7;union Cyc_CfFlowInfo_FlowInfo _tmp208;void*_tmp207;_LL11B: _tmp208=_tmp1F8.f1;_tmp207=_tmp1F8.f2;_LL11C:;{
void*_tmp1F9=(void*)_check_null(_tmp2D3->topt);
# 1420
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp1F9))
return({struct _tuple16 _tmp5D5;_tmp5D5.f1=_tmp208,({void*_tmp6F4=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5D5.f2=_tmp6F4;});_tmp5D5;});
if(Cyc_Absyn_is_require_union_type(_tmp1F9))
Cyc_NewControlFlow_check_union_requires(_tmp2D3->loc,_tmp1F9,_tmp2D2,_tmp208);{
struct _tuple15 _tmp1FA=Cyc_CfFlowInfo_unname_rval(_tmp207);struct _tuple15 _tmp1FB=_tmp1FA;void*_tmp206;_LL11E: _tmp206=_tmp1FB.f1;_LL11F:;{
void*_tmp1FC=_tmp206;int _tmp205;int _tmp204;struct _dyneither_ptr _tmp203;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FC)->tag == 6U){_LL121: _tmp205=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FC)->f1).is_union;_tmp204=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FC)->f1).fieldnum;_tmp203=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FC)->f2;_LL122: {
# 1427
int _tmp1FD=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2D3->topt),_tmp2D2);
if((_tmp205  && _tmp204 != - 1) && _tmp204 != _tmp1FD)
return({struct _tuple16 _tmp5D6;_tmp5D6.f1=_tmp208,({void*_tmp6F5=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),1,_tmp124->unknown_none);_tmp5D6.f2=_tmp6F5;});_tmp5D6;});
return({struct _tuple16 _tmp5D7;_tmp5D7.f1=_tmp208,_tmp5D7.f2=*((void**)_check_dyneither_subscript(_tmp203,sizeof(void*),_tmp1FD));_tmp5D7;});}}else{_LL123: _LL124:
# 1432
({void*_tmp1FE=0U;({struct Cyc___cycFILE*_tmp6F7=Cyc_stderr;struct _dyneither_ptr _tmp6F6=({const char*_tmp1FF="the bad rexp is :";_tag_dyneither(_tmp1FF,sizeof(char),18U);});Cyc_fprintf(_tmp6F7,_tmp6F6,_tag_dyneither(_tmp1FE,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(_tmp206);
({struct Cyc_String_pa_PrintArg_struct _tmp202=({struct Cyc_String_pa_PrintArg_struct _tmp5D8;_tmp5D8.tag=0U,({struct _dyneither_ptr _tmp6F8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5D8.f1=_tmp6F8;});_tmp5D8;});void*_tmp200[1U];_tmp200[0]=& _tmp202;({struct _dyneither_ptr _tmp6F9=({const char*_tmp201="\nanal_Rexp: AggrMember: %s";_tag_dyneither(_tmp201,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F9,_tag_dyneither(_tmp200,sizeof(void*),1U));});});}_LL120:;};};};}case 38U: _LL54: _tmp2D5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2D4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL55: {
# 1440
struct _tuple16 _tmp209=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D5);struct _tuple16 _tmp20A=_tmp209;union Cyc_CfFlowInfo_FlowInfo _tmp217;void*_tmp216;_LL126: _tmp217=_tmp20A.f1;_tmp216=_tmp20A.f2;_LL127:;
# 1442
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp2D5->topt)))
return({struct _tuple16 _tmp5D9;_tmp5D9.f1=_tmp217,_tmp5D9.f2=_tmp124->unknown_all;_tmp5D9;});{
struct _tuple15 _tmp20B=Cyc_CfFlowInfo_unname_rval(_tmp216);struct _tuple15 _tmp20C=_tmp20B;void*_tmp215;_LL129: _tmp215=_tmp20C.f1;_LL12A:;{
void*_tmp20D=_tmp215;int _tmp214;int _tmp213;struct _dyneither_ptr _tmp212;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20D)->tag == 6U){_LL12C: _tmp214=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20D)->f1).is_union;_tmp213=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20D)->f1).fieldnum;_tmp212=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20D)->f2;_LL12D: {
# 1447
int _tmp20E=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2D5->topt),_tmp2D4);
if(_tmp214  && _tmp213 != - 1){
if(_tmp213 != _tmp20E)
return({struct _tuple16 _tmp5DA;_tmp5DA.f1=_tmp217,_tmp5DA.f2=_tmp124->zero;_tmp5DA;});else{
# 1452
return({struct _tuple16 _tmp5DB;_tmp5DB.f1=_tmp217,_tmp5DB.f2=_tmp124->notzeroall;_tmp5DB;});}}else{
# 1454
return({struct _tuple16 _tmp5DC;_tmp5DC.f1=_tmp217,_tmp5DC.f2=_tmp124->unknown_all;_tmp5DC;});}}}else{_LL12E: _LL12F:
# 1456
({struct Cyc_String_pa_PrintArg_struct _tmp211=({struct Cyc_String_pa_PrintArg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _dyneither_ptr _tmp6FA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5DD.f1=_tmp6FA;});_tmp5DD;});void*_tmp20F[1U];_tmp20F[0]=& _tmp211;({struct _dyneither_ptr _tmp6FB=({const char*_tmp210="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp210,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6FB,_tag_dyneither(_tmp20F,sizeof(void*),1U));});});}_LL12B:;};};}case 22U: _LL56: _tmp2D7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2D6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL57: {
# 1460
struct _tuple16 _tmp218=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D7);struct _tuple16 _tmp219=_tmp218;union Cyc_CfFlowInfo_FlowInfo _tmp22E;void*_tmp22D;_LL131: _tmp22E=_tmp219.f1;_tmp22D=_tmp219.f2;_LL132:;{
# 1463
struct _tuple16 _tmp21A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp22E,_tmp2D7,_tmp22D);struct _tuple16 _tmp21B=_tmp21A;union Cyc_CfFlowInfo_FlowInfo _tmp22C;void*_tmp22B;_LL134: _tmp22C=_tmp21B.f1;_tmp22B=_tmp21B.f2;_LL135:;{
# 1466
void*_tmp21C=Cyc_Tcutil_compress((void*)_check_null(_tmp2D7->topt));void*_tmp21D=_tmp21C;void*_tmp22A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21D)->tag == 5U){_LL137: _tmp22A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21D)->f1).elt_typ;_LL138:
# 1468
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp22A))
# 1470
return({struct _tuple16 _tmp5DE;_tmp5DE.f1=_tmp22C,({void*_tmp6FC=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5DE.f2=_tmp6FC;});_tmp5DE;});
# 1472
if(Cyc_Absyn_is_require_union_type(_tmp22A))
Cyc_NewControlFlow_check_union_requires(_tmp2D7->loc,_tmp22A,_tmp2D6,_tmp22E);{
# 1475
struct _tuple15 _tmp21E=Cyc_CfFlowInfo_unname_rval(_tmp22B);struct _tuple15 _tmp21F=_tmp21E;void*_tmp227;_LL13C: _tmp227=_tmp21F.f1;_LL13D:;{
void*_tmp220=_tmp227;int _tmp226;int _tmp225;struct _dyneither_ptr _tmp224;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp220)->tag == 6U){_LL13F: _tmp226=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp220)->f1).is_union;_tmp225=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp220)->f1).fieldnum;_tmp224=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp220)->f2;_LL140: {
# 1478
int _tmp221=Cyc_CfFlowInfo_get_field_index(_tmp22A,_tmp2D6);
if((_tmp226  && _tmp225 != - 1) && _tmp225 != _tmp221)
return({struct _tuple16 _tmp5DF;_tmp5DF.f1=_tmp22C,({void*_tmp6FD=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),1,_tmp124->unknown_none);_tmp5DF.f2=_tmp6FD;});_tmp5DF;});
return({struct _tuple16 _tmp5E0;_tmp5E0.f1=_tmp22C,_tmp5E0.f2=*((void**)_check_dyneither_subscript(_tmp224,sizeof(void*),_tmp221));_tmp5E0;});}}else{_LL141: _LL142:
({void*_tmp222=0U;({struct _dyneither_ptr _tmp6FE=({const char*_tmp223="anal_Rexp: AggrArrow";_tag_dyneither(_tmp223,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6FE,_tag_dyneither(_tmp222,sizeof(void*),0U));});});}_LL13E:;};};}else{_LL139: _LL13A:
# 1484
({void*_tmp228=0U;({struct _dyneither_ptr _tmp6FF=({const char*_tmp229="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp229,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6FF,_tag_dyneither(_tmp228,sizeof(void*),0U));});});}_LL136:;};};}case 6U: _LL58: _tmp2DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2D9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_tmp2D8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp137)->f3;_LL59: {
# 1488
struct _tuple19 _tmp22F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DA);struct _tuple19 _tmp230=_tmp22F;union Cyc_CfFlowInfo_FlowInfo _tmp234;union Cyc_CfFlowInfo_FlowInfo _tmp233;_LL144: _tmp234=_tmp230.f1;_tmp233=_tmp230.f2;_LL145:;{
struct _tuple16 _tmp231=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp234,_tmp2D9);
struct _tuple16 _tmp232=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp233,_tmp2D8);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp124,_tmp231,_tmp232);};}case 7U: _LL5A: _tmp2DC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL5B: {
# 1494
struct _tuple19 _tmp235=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DC);struct _tuple19 _tmp236=_tmp235;union Cyc_CfFlowInfo_FlowInfo _tmp23E;union Cyc_CfFlowInfo_FlowInfo _tmp23D;_LL147: _tmp23E=_tmp236.f1;_tmp23D=_tmp236.f2;_LL148:;{
struct _tuple16 _tmp237=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp23E,_tmp2DB);struct _tuple16 _tmp238=_tmp237;union Cyc_CfFlowInfo_FlowInfo _tmp23C;void*_tmp23B;_LL14A: _tmp23C=_tmp238.f1;_tmp23B=_tmp238.f2;_LL14B:;{
struct _tuple16 _tmp239=({struct _tuple16 _tmp5E2;_tmp5E2.f1=_tmp23C,_tmp5E2.f2=_tmp23B;_tmp5E2;});
struct _tuple16 _tmp23A=({struct _tuple16 _tmp5E1;_tmp5E1.f1=_tmp23D,_tmp5E1.f2=_tmp124->zero;_tmp5E1;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp124,_tmp239,_tmp23A);};};}case 8U: _LL5C: _tmp2DE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2DD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL5D: {
# 1501
struct _tuple19 _tmp23F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DE);struct _tuple19 _tmp240=_tmp23F;union Cyc_CfFlowInfo_FlowInfo _tmp248;union Cyc_CfFlowInfo_FlowInfo _tmp247;_LL14D: _tmp248=_tmp240.f1;_tmp247=_tmp240.f2;_LL14E:;{
struct _tuple16 _tmp241=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp247,_tmp2DD);struct _tuple16 _tmp242=_tmp241;union Cyc_CfFlowInfo_FlowInfo _tmp246;void*_tmp245;_LL150: _tmp246=_tmp242.f1;_tmp245=_tmp242.f2;_LL151:;{
struct _tuple16 _tmp243=({struct _tuple16 _tmp5E4;_tmp5E4.f1=_tmp246,_tmp5E4.f2=_tmp245;_tmp5E4;});
struct _tuple16 _tmp244=({struct _tuple16 _tmp5E3;_tmp5E3.f1=_tmp248,_tmp5E3.f2=_tmp124->notzeroall;_tmp5E3;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp124,_tmp243,_tmp244);};};}case 23U: _LL5E: _tmp2E0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2DF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL5F: {
# 1508
struct _tuple23 _tmp249=({struct Cyc_NewControlFlow_AnalEnv*_tmp701=env;union Cyc_CfFlowInfo_FlowInfo _tmp700=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp701,_tmp700,({struct Cyc_Absyn_Exp*_tmp265[2U];_tmp265[0]=_tmp2E0,_tmp265[1]=_tmp2DF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp265,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp24A=_tmp249;union Cyc_CfFlowInfo_FlowInfo _tmp264;struct Cyc_List_List*_tmp263;_LL153: _tmp264=_tmp24A.f1;_tmp263=_tmp24A.f2;_LL154:;{
# 1512
union Cyc_CfFlowInfo_FlowInfo _tmp24B=_tmp264;
{union Cyc_CfFlowInfo_FlowInfo _tmp24C=_tmp264;struct Cyc_Dict_Dict _tmp251;struct Cyc_List_List*_tmp250;if((_tmp24C.ReachableFL).tag == 2){_LL156: _tmp251=((_tmp24C.ReachableFL).val).f1;_tmp250=((_tmp24C.ReachableFL).val).f2;_LL157:
# 1517
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp251,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp263))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp24D=0U;({unsigned int _tmp703=_tmp2DF->loc;struct _dyneither_ptr _tmp702=({const char*_tmp24E="expression may not be initialized";_tag_dyneither(_tmp24E,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp703,_tmp702,_tag_dyneither(_tmp24D,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp24F=Cyc_NewControlFlow_add_subscript_reln(_tmp250,_tmp2E0,_tmp2DF);
if(_tmp250 != _tmp24F)
({union Cyc_CfFlowInfo_FlowInfo _tmp704=Cyc_CfFlowInfo_ReachableFL(_tmp251,_tmp24F);_tmp24B=_tmp704;});
goto _LL155;};}else{_LL158: _LL159:
 goto _LL155;}_LL155:;}{
# 1525
void*_tmp252=Cyc_Tcutil_compress((void*)_check_null(_tmp2E0->topt));void*_tmp253=_tmp252;struct Cyc_List_List*_tmp262;switch(*((int*)_tmp253)){case 10U: _LL15B: _tmp262=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp253)->f1;_LL15C: {
# 1527
struct _tuple15 _tmp254=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp263))->hd);struct _tuple15 _tmp255=_tmp254;void*_tmp25A;_LL162: _tmp25A=_tmp255.f1;_LL163:;{
void*_tmp256=_tmp25A;struct _dyneither_ptr _tmp259;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp256)->tag == 6U){_LL165: _tmp259=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp256)->f2;_LL166: {
# 1530
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp2DF)).f1;
return({struct _tuple16 _tmp5E5;_tmp5E5.f1=_tmp24B,_tmp5E5.f2=*((void**)_check_dyneither_subscript(_tmp259,sizeof(void*),(int)i));_tmp5E5;});}}else{_LL167: _LL168:
({void*_tmp257=0U;({struct _dyneither_ptr _tmp705=({const char*_tmp258="anal_Rexp: Subscript";_tag_dyneither(_tmp258,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp705,_tag_dyneither(_tmp257,sizeof(void*),0U));});});}_LL164:;};}case 5U: _LL15D: _LL15E: {
# 1535
struct _tuple16 _tmp25B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp264,_tmp2E0,(void*)((struct Cyc_List_List*)_check_null(_tmp263))->hd);struct _tuple16 _tmp25C=_tmp25B;union Cyc_CfFlowInfo_FlowInfo _tmp25F;void*_tmp25E;_LL16A: _tmp25F=_tmp25C.f1;_tmp25E=_tmp25C.f2;_LL16B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp25D=_tmp25F;if((_tmp25D.BottomFL).tag == 1){_LL16D: _LL16E:
 return({struct _tuple16 _tmp5E6;_tmp5E6.f1=_tmp25F,_tmp5E6.f2=_tmp25E;_tmp5E6;});}else{_LL16F: _LL170:
 return({struct _tuple16 _tmp5E7;_tmp5E7.f1=_tmp24B,_tmp5E7.f2=_tmp25E;_tmp5E7;});}_LL16C:;};}default: _LL15F: _LL160:
# 1540
({void*_tmp260=0U;({struct _dyneither_ptr _tmp706=({const char*_tmp261="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp261,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp706,_tag_dyneither(_tmp260,sizeof(void*),0U));});});}_LL15A:;};};}case 24U: _LL62: _tmp2E1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL63: {
# 1546
struct _tuple23 _tmp266=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E1,1,1);struct _tuple23 _tmp267=_tmp266;union Cyc_CfFlowInfo_FlowInfo _tmp26D;struct Cyc_List_List*_tmp26C;_LL172: _tmp26D=_tmp267.f1;_tmp26C=_tmp267.f2;_LL173:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp26B=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E1);void**_tmp26A=_cycalloc(_check_times(_tmp26B,sizeof(void*)));({{unsigned int _tmp5E9=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E1);unsigned int i;for(i=0;i < _tmp5E9;++ i){({
void*_tmp707=({void*_tmp269=(void*)((struct Cyc_List_List*)_check_null(_tmp26C))->hd;
_tmp26C=_tmp26C->tl;
_tmp269;});
# 1549
_tmp26A[i]=_tmp707;});}}0;});_tag_dyneither(_tmp26A,sizeof(void*),_tmp26B);});
# 1553
return({struct _tuple16 _tmp5E8;_tmp5E8.f1=_tmp26D,({void*_tmp708=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->tag=6U,(_tmp268->f1).is_union=0,(_tmp268->f1).fieldnum=- 1,_tmp268->f2=aggrdict;_tmp268;});_tmp5E8.f2=_tmp708;});_tmp5E8;});};}case 30U: _LL64: _tmp2E2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_LL65: {
# 1556
struct Cyc_List_List*fs;
{void*_tmp26E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp26F=_tmp26E;struct Cyc_List_List*_tmp272;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26F)->tag == 12U){_LL175: _tmp272=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26F)->f2;_LL176:
 fs=_tmp272;goto _LL174;}else{_LL177: _LL178:
({void*_tmp270=0U;({struct _dyneither_ptr _tmp709=({const char*_tmp271="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp271,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp709,_tag_dyneither(_tmp270,sizeof(void*),0U));});});}_LL174:;}
# 1561
_tmp2E5=_tmp2E2;_tmp2E4=Cyc_Absyn_StructA;_tmp2E3=fs;goto _LL67;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp137)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp137)->f4)->impl != 0){_LL66: _tmp2E5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp137)->f3;_tmp2E4=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp137)->f4)->kind;_tmp2E3=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp137)->f4)->impl)->fields;_LL67: {
# 1563
void*exp_type=(void*)_check_null(e->topt);
struct _tuple23 _tmp273=({struct Cyc_NewControlFlow_AnalEnv*_tmp70B=env;union Cyc_CfFlowInfo_FlowInfo _tmp70A=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp70B,_tmp70A,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp2E5),1,1);});struct _tuple23 _tmp274=_tmp273;union Cyc_CfFlowInfo_FlowInfo _tmp27E;struct Cyc_List_List*_tmp27D;_LL17A: _tmp27E=_tmp274.f1;_tmp27D=_tmp274.f2;_LL17B:;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp124,_tmp2E3,_tmp2E4 == Cyc_Absyn_UnionA,_tmp124->unknown_all);
# 1568
int field_no=-1;
{int i=0;for(0;_tmp27D != 0;(_tmp27D=_tmp27D->tl,_tmp2E5=_tmp2E5->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp2E5))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp275=(void*)ds->hd;void*_tmp276=_tmp275;struct _dyneither_ptr*_tmp27A;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp276)->tag == 0U){_LL17D: _LL17E:
({void*_tmp277=0U;({struct _dyneither_ptr _tmp70C=({const char*_tmp278="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp278,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp70C,_tag_dyneither(_tmp277,sizeof(void*),0U));});});}else{_LL17F: _tmp27A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp276)->f1;_LL180:
# 1575
({int _tmp70D=Cyc_CfFlowInfo_get_field_index_fs(_tmp2E3,_tmp27A);field_no=_tmp70D;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp27D->hd;
# 1578
if(_tmp2E4 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp279=(*((struct _tuple26*)_tmp2E5->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp70E=Cyc_NewControlFlow_use_Rval(env,_tmp279->loc,_tmp27E,(void*)_tmp27D->hd);_tmp27E=_tmp70E;});
# 1582
Cyc_NewControlFlow_check_union_requires(_tmp279->loc,exp_type,_tmp27A,_tmp27E);}}_LL17C:;}}}{
# 1585
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27B=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->tag=6U,(_tmp27C->f1).is_union=_tmp2E4 == Cyc_Absyn_UnionA,(_tmp27C->f1).fieldnum=field_no,_tmp27C->f2=aggrdict;_tmp27C;});
return({struct _tuple16 _tmp5EA;_tmp5EA.f1=_tmp27E,_tmp5EA.f2=(void*)_tmp27B;_tmp5EA;});};};}}else{goto _LL68;}}else{_LL68: _LL69:
# 1589
({struct Cyc_String_pa_PrintArg_struct _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp5EB;_tmp5EB.tag=0U,({struct _dyneither_ptr _tmp70F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5EB.f1=_tmp70F;});_tmp5EB;});void*_tmp27F[1U];_tmp27F[0]=& _tmp281;({struct _dyneither_ptr _tmp710=({const char*_tmp280="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp280,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp710,_tag_dyneither(_tmp27F,sizeof(void*),1U));});});}case 26U: _LL6A: _tmp2E6=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL6B: {
# 1591
struct Cyc_List_List*_tmp282=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp2E6);
struct _tuple23 _tmp283=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp282,1,1);struct _tuple23 _tmp284=_tmp283;union Cyc_CfFlowInfo_FlowInfo _tmp286;struct Cyc_List_List*_tmp285;_LL182: _tmp286=_tmp284.f1;_tmp285=_tmp284.f2;_LL183:;
for(0;_tmp285 != 0;(_tmp285=_tmp285->tl,_tmp282=_tmp282->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp711=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp282))->hd)->loc,_tmp286,(void*)_tmp285->hd);_tmp286=_tmp711;});}
return({struct _tuple16 _tmp5EC;_tmp5EC.f1=_tmp286,({void*_tmp712=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,(void*)_check_null(e->topt),0,_tmp124->unknown_all);_tmp5EC.f2=_tmp712;});_tmp5EC;});}case 27U: _LL6C: _tmp2EA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2E9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_tmp2E8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp137)->f3;_tmp2E7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp137)->f4;_LL6D: {
# 1599
struct _tuple16 _tmp287=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2E9);struct _tuple16 _tmp288=_tmp287;union Cyc_CfFlowInfo_FlowInfo _tmp2A3;void*_tmp2A2;_LL185: _tmp2A3=_tmp288.f1;_tmp2A2=_tmp288.f2;_LL186:;{
union Cyc_CfFlowInfo_FlowInfo _tmp289=_tmp2A3;struct Cyc_Dict_Dict _tmp2A1;struct Cyc_List_List*_tmp2A0;if((_tmp289.BottomFL).tag == 1){_LL188: _LL189:
 return({struct _tuple16 _tmp5ED;_tmp5ED.f1=_tmp2A3,_tmp5ED.f2=_tmp124->unknown_all;_tmp5ED;});}else{_LL18A: _tmp2A1=((_tmp289.ReachableFL).val).f1;_tmp2A0=((_tmp289.ReachableFL).val).f2;_LL18B:
# 1603
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2A1,_tmp2A2)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp28A=0U;({unsigned int _tmp714=_tmp2E9->loc;struct _dyneither_ptr _tmp713=({const char*_tmp28B="expression may not be initialized";_tag_dyneither(_tmp28B,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp714,_tmp713,_tag_dyneither(_tmp28A,sizeof(void*),0U));});});{
# 1607
struct Cyc_List_List*new_relns=_tmp2A0;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp2EA);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp2E9,& n2))
({struct Cyc_List_List*_tmp715=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp2A0);new_relns=_tmp715;});
if(_tmp2A0 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp716=Cyc_CfFlowInfo_ReachableFL(_tmp2A1,new_relns);_tmp2A3=_tmp716;});{
# 1616
void*_tmp28C=_tmp2A2;switch(*((int*)_tmp28C)){case 0U: _LL18D: _LL18E:
 return({struct _tuple16 _tmp5EE;_tmp5EE.f1=_tmp2A3,_tmp5EE.f2=_tmp124->unknown_all;_tmp5EE;});case 1U: _LL18F: _LL190:
 goto _LL192;case 4U: _LL191: _LL192: {
# 1620
struct Cyc_List_List _tmp28D=({struct Cyc_List_List _tmp5F1;_tmp5F1.hd=_tmp2EA,_tmp5F1.tl=0;_tmp5F1;});
({union Cyc_CfFlowInfo_FlowInfo _tmp717=Cyc_NewControlFlow_add_vars(_tmp124,_tmp2A3,& _tmp28D,_tmp124->unknown_all,e->loc,0);_tmp2A3=_tmp717;});{
# 1623
struct _tuple16 _tmp28E=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2A3,_tmp2E8);struct _tuple16 _tmp28F=_tmp28E;union Cyc_CfFlowInfo_FlowInfo _tmp295;void*_tmp294;_LL196: _tmp295=_tmp28F.f1;_tmp294=_tmp28F.f2;_LL197:;
{union Cyc_CfFlowInfo_FlowInfo _tmp290=_tmp295;struct Cyc_Dict_Dict _tmp293;if((_tmp290.BottomFL).tag == 1){_LL199: _LL19A:
 return({struct _tuple16 _tmp5EF;_tmp5EF.f1=_tmp295,_tmp5EF.f2=_tmp124->unknown_all;_tmp5EF;});}else{_LL19B: _tmp293=((_tmp290.ReachableFL).val).f1;_LL19C:
# 1627
 if(Cyc_CfFlowInfo_initlevel(_tmp124,_tmp293,_tmp294)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp291=0U;({unsigned int _tmp719=_tmp2E9->loc;struct _dyneither_ptr _tmp718=({const char*_tmp292="expression may not be initialized";_tag_dyneither(_tmp292,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp719,_tmp718,_tag_dyneither(_tmp291,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5F0;({union Cyc_CfFlowInfo_FlowInfo _tmp71A=Cyc_CfFlowInfo_BottomFL();_tmp5F0.f1=_tmp71A;}),_tmp5F0.f2=_tmp124->unknown_all;_tmp5F0;});}}_LL198:;}
# 1632
_tmp2A3=_tmp295;
goto _LL194;};}default: _LL193: _LL194:
# 1635
 while(1){
struct Cyc_List_List _tmp296=({struct Cyc_List_List _tmp5F3;_tmp5F3.hd=_tmp2EA,_tmp5F3.tl=0;_tmp5F3;});
({union Cyc_CfFlowInfo_FlowInfo _tmp71B=Cyc_NewControlFlow_add_vars(_tmp124,_tmp2A3,& _tmp296,_tmp124->unknown_all,e->loc,0);_tmp2A3=_tmp71B;});{
struct _tuple16 _tmp297=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2A3,_tmp2E8);struct _tuple16 _tmp298=_tmp297;union Cyc_CfFlowInfo_FlowInfo _tmp29F;void*_tmp29E;_LL19E: _tmp29F=_tmp298.f1;_tmp29E=_tmp298.f2;_LL19F:;
{union Cyc_CfFlowInfo_FlowInfo _tmp299=_tmp29F;struct Cyc_Dict_Dict _tmp29C;if((_tmp299.BottomFL).tag == 1){_LL1A1: _LL1A2:
 goto _LL1A0;}else{_LL1A3: _tmp29C=((_tmp299.ReachableFL).val).f1;_LL1A4:
# 1642
 if(Cyc_CfFlowInfo_initlevel(_tmp124,_tmp29C,_tmp29E)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp29A=0U;({unsigned int _tmp71D=_tmp2E9->loc;struct _dyneither_ptr _tmp71C=({const char*_tmp29B="expression may not be initialized";_tag_dyneither(_tmp29B,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp71D,_tmp71C,_tag_dyneither(_tmp29A,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5F2;({union Cyc_CfFlowInfo_FlowInfo _tmp71E=Cyc_CfFlowInfo_BottomFL();_tmp5F2.f1=_tmp71E;}),_tmp5F2.f2=_tmp124->unknown_all;_tmp5F2;});}}_LL1A0:;}{
# 1647
union Cyc_CfFlowInfo_FlowInfo _tmp29D=Cyc_CfFlowInfo_join_flow(_tmp124,_tmp2A3,_tmp29F);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp29D,_tmp2A3))
break;
_tmp2A3=_tmp29D;};};}
# 1652
return({struct _tuple16 _tmp5F4;_tmp5F4.f1=_tmp2A3,_tmp5F4.f2=_tmp124->unknown_all;_tmp5F4;});}_LL18C:;};};}_LL187:;};}case 28U: _LL6E: _tmp2ED=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_tmp2EC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp137)->f2;_tmp2EB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp137)->f3;_LL6F: {
# 1658
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->tag=1U,_tmp2AC->f1=_tmp2ED,_tmp2AC->f2=(void*)_check_null(e->topt);_tmp2AC;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->root=root,_tmp2AB->path=0;_tmp2AB;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->tag=4U,_tmp2AA->f1=place;_tmp2AA;});
void*place_val;
# 1666
({void*_tmp71F=Cyc_CfFlowInfo_typ_to_absrval(_tmp124,_tmp2EC,0,_tmp124->unknown_none);place_val=_tmp71F;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp2A4=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2ED);struct _tuple16 _tmp2A5=_tmp2A4;union Cyc_CfFlowInfo_FlowInfo _tmp2A9;_LL1A6: _tmp2A9=_tmp2A5.f1;_LL1A7:;
outflow=_tmp2A9;{
union Cyc_CfFlowInfo_FlowInfo _tmp2A6=outflow;struct Cyc_Dict_Dict _tmp2A8;struct Cyc_List_List*_tmp2A7;if((_tmp2A6.BottomFL).tag == 1){_LL1A9: _LL1AA:
 return({struct _tuple16 _tmp5F5;_tmp5F5.f1=outflow,_tmp5F5.f2=rval;_tmp5F5;});}else{_LL1AB: _tmp2A8=((_tmp2A6.ReachableFL).val).f1;_tmp2A7=((_tmp2A6.ReachableFL).val).f2;_LL1AC:
# 1673
 return({struct _tuple16 _tmp5F6;({union Cyc_CfFlowInfo_FlowInfo _tmp721=({struct Cyc_Dict_Dict _tmp720=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A8,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp720,_tmp2A7);});_tmp5F6.f1=_tmp721;}),_tmp5F6.f2=rval;_tmp5F6;});}_LL1A8:;};};}case 37U: _LL70: _tmp2EE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp137)->f1;_LL71: {
# 1677
struct _tuple17 _tmp2AD=({struct _tuple17 _tmp5F8;_tmp5F8.f1=(env->fenv)->unknown_all,_tmp5F8.f2=copy_ctxt;_tmp5F8;});
union Cyc_CfFlowInfo_FlowInfo _tmp2AE=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp2EE,& _tmp2AD);
return({struct _tuple16 _tmp5F7;_tmp5F7.f1=_tmp2AE,_tmp5F7.f2=_tmp2AD.f1;_tmp5F7;});}case 36U: _LL74: _LL75:
# 1682
 goto _LL77;case 25U: _LL76: _LL77:
 goto _LL79;case 39U: _LL78: _LL79:
 goto _LL7B;default: _LL7A: _LL7B:
({void*_tmp2AF=0U;({struct _dyneither_ptr _tmp722=({const char*_tmp2B0="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp2B0,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp722,_tag_dyneither(_tmp2AF,sizeof(void*),0U));});});}_LLD:;};}
# 1697 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1706
struct Cyc_CfFlowInfo_FlowEnv*_tmp2EF=env->fenv;
void*_tmp2F0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2F1=_tmp2F0;union Cyc_Absyn_Constraint*_tmp315;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->tag == 5U){_LL1: _tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).bounds;_LL2: {
# 1709
union Cyc_CfFlowInfo_FlowInfo _tmp2F2=f;struct Cyc_Dict_Dict _tmp312;struct Cyc_List_List*_tmp311;if((_tmp2F2.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple18 _tmp5F9;_tmp5F9.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp723=Cyc_CfFlowInfo_UnknownL();_tmp5F9.f2=_tmp723;});_tmp5F9;});}else{_LL8: _tmp312=((_tmp2F2.ReachableFL).val).f1;_tmp311=((_tmp2F2.ReachableFL).val).f2;_LL9: {
# 1713
struct _tuple15 _tmp2F3=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp2F4=_tmp2F3;void*_tmp310;struct Cyc_List_List*_tmp30F;_LLB: _tmp310=_tmp2F4.f1;_tmp30F=_tmp2F4.f2;_LLC:;
retry: {void*_tmp2F5=_tmp310;void*_tmp306;enum Cyc_CfFlowInfo_InitLevel _tmp305;void*_tmp304;struct Cyc_List_List*_tmp303;void*_tmp302;switch(*((int*)_tmp2F5)){case 8U: _LLE: _LLF:
# 1716
({void*_tmp2F6=0U;({struct _dyneither_ptr _tmp724=({const char*_tmp2F7="named location in anal_derefL";_tag_dyneither(_tmp2F7,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp724,_tag_dyneither(_tmp2F6,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1718
({void*_tmp725=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->tag=Cyc_CfFlowInfo_NotZero,_tmp2F8->f1=_tmp311;_tmp2F8;});e->annot=_tmp725;});goto _LLD;case 5U: _LL12: _tmp302=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1;_LL13:
# 1721
 if(expanded_place_possibly_null)
# 1724
({void*_tmp726=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2F9->f1=_tmp311;_tmp2F9;});e->annot=_tmp726;});else{
# 1727
void*_tmp2FA=e->annot;void*_tmp2FB=_tmp2FA;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2FB)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1731
({void*_tmp727=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2FC->f1=_tmp311;_tmp2FC;});e->annot=_tmp727;});
goto _LL1E;}else{_LL21: _LL22:
# 1734
({void*_tmp728=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->tag=Cyc_CfFlowInfo_NotZero,_tmp2FD->f1=_tmp311;_tmp2FD;});e->annot=_tmp728;});
goto _LL1E;}_LL1E:;}
# 1739
if(Cyc_Tcutil_is_bound_one(_tmp315))return({struct _tuple18 _tmp5FA;_tmp5FA.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp729=Cyc_CfFlowInfo_UnknownL();_tmp5FA.f2=_tmp729;});_tmp5FA;});
goto _LLD;case 4U: _LL14: _tmp304=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1)->root;_tmp303=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1)->path;_LL15:
# 1743
({void*_tmp72A=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->tag=Cyc_CfFlowInfo_NotZero,_tmp2FE->f1=_tmp311;_tmp2FE;});e->annot=_tmp72A;});
if(Cyc_Tcutil_is_bound_one(_tmp315))
return({struct _tuple18 _tmp5FB;_tmp5FB.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp72C=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->root=_tmp304,({struct Cyc_List_List*_tmp72B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp303,path);_tmp2FF->path=_tmp72B;});_tmp2FF;}));_tmp5FB.f2=_tmp72C;});_tmp5FB;});
goto _LLD;case 0U: _LL16: _LL17:
# 1748
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp5FC;({union Cyc_CfFlowInfo_FlowInfo _tmp72E=Cyc_CfFlowInfo_BottomFL();_tmp5FC.f1=_tmp72E;}),({union Cyc_CfFlowInfo_AbsLVal _tmp72D=Cyc_CfFlowInfo_UnknownL();_tmp5FC.f2=_tmp72D;});_tmp5FC;});case 2U: _LL18: _tmp305=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1;_LL19:
# 1753
 if(Cyc_Tcutil_is_bound_one(_tmp315))
({union Cyc_CfFlowInfo_FlowInfo _tmp72F=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp305,_tmp30F);f=_tmp72F;});
({void*_tmp730=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->tag=Cyc_CfFlowInfo_UnknownZ,_tmp300->f1=_tmp311;_tmp300;});e->annot=_tmp730;});
goto _LLD;case 7U: _LL1A: _tmp306=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f3;_LL1B:
# 1758
 if(passthrough_consumes){
_tmp310=_tmp306;goto retry;}
# 1761
goto _LL1D;default: _LL1C: _LL1D:
# 1763
({void*_tmp731=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->tag=Cyc_CfFlowInfo_UnknownZ,_tmp301->f1=_tmp311;_tmp301;});e->annot=_tmp731;});
goto _LLD;}_LLD:;}
# 1766
if(Cyc_CfFlowInfo_initlevel(_tmp2EF,_tmp312,_tmp310)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp307=Cyc_CfFlowInfo_unname_rval(_tmp310);struct _tuple15 _tmp308=_tmp307;void*_tmp30E;_LL24: _tmp30E=_tmp308.f1;_LL25:;{
void*_tmp309=_tmp30E;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp309)->tag == 7U){_LL27: _LL28:
# 1770
({void*_tmp30A=0U;({unsigned int _tmp733=e->loc;struct _dyneither_ptr _tmp732=({const char*_tmp30B="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp30B,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp733,_tmp732,_tag_dyneither(_tmp30A,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1773
({void*_tmp30C=0U;({unsigned int _tmp735=e->loc;struct _dyneither_ptr _tmp734=({const char*_tmp30D="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp30D,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp735,_tmp734,_tag_dyneither(_tmp30C,sizeof(void*),0U));});});
goto _LL26;}_LL26:;};}
# 1777
return({struct _tuple18 _tmp5FD;_tmp5FD.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp736=Cyc_CfFlowInfo_UnknownL();_tmp5FD.f2=_tmp736;});_tmp5FD;});}}_LL5:;}}else{_LL3: _LL4:
# 1779
({void*_tmp313=0U;({struct _dyneither_ptr _tmp737=({const char*_tmp314="left deref of non-pointer-type";_tag_dyneither(_tmp314,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp737,_tag_dyneither(_tmp313,sizeof(void*),0U));});});}_LL0:;}
# 1785
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp316=p;void*_tmp31B;struct Cyc_List_List*_tmp31A;_LL1: _tmp31B=_tmp316->root;_tmp31A=_tmp316->path;_LL2:;
({struct Cyc_List_List*_tmp739=({struct Cyc_List_List*_tmp318=_cycalloc(sizeof(*_tmp318));({void*_tmp738=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->tag=1U;_tmp317;});_tmp318->hd=_tmp738;}),_tmp318->tl=path;_tmp318;});path=_tmp739;});
return({struct Cyc_CfFlowInfo_Place*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->root=_tmp31B,({struct Cyc_List_List*_tmp73A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp31A,path);_tmp319->path=_tmp73A;});_tmp319;});}
# 1795
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1799
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp31C=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp31D=inflow;struct Cyc_Dict_Dict _tmp31F;struct Cyc_List_List*_tmp31E;if((_tmp31D.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple18 _tmp5FE;({union Cyc_CfFlowInfo_FlowInfo _tmp73C=Cyc_CfFlowInfo_BottomFL();_tmp5FE.f1=_tmp73C;}),({union Cyc_CfFlowInfo_AbsLVal _tmp73B=Cyc_CfFlowInfo_UnknownL();_tmp5FE.f2=_tmp73B;});_tmp5FE;});}else{_LL3: _tmp31F=((_tmp31D.ReachableFL).val).f1;_tmp31E=((_tmp31D.ReachableFL).val).f2;_LL4:
# 1804
 d=_tmp31F;}_LL0:;}{
# 1808
void*_tmp320=e->r;void*_tmp321=_tmp320;struct Cyc_Absyn_Exp*_tmp378;struct _dyneither_ptr*_tmp377;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp373;struct _dyneither_ptr*_tmp372;struct Cyc_Absyn_Vardecl*_tmp371;struct Cyc_Absyn_Vardecl*_tmp370;struct Cyc_Absyn_Vardecl*_tmp36F;struct Cyc_Absyn_Vardecl*_tmp36E;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_Absyn_Exp*_tmp36B;switch(*((int*)_tmp321)){case 14U: _LL6: _tmp36B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL7:
 _tmp36C=_tmp36B;goto _LL9;case 12U: _LL8: _tmp36C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LL9:
 _tmp36D=_tmp36C;goto _LLB;case 13U: _LLA: _tmp36D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LLB:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp36D,path);case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)){case 3U: _LLC: _tmp36E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LLD:
# 1813
 _tmp36F=_tmp36E;goto _LLF;case 4U: _LLE: _tmp36F=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LLF:
 _tmp370=_tmp36F;goto _LL11;case 5U: _LL10: _tmp370=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LL11:
# 1816
 return({struct _tuple18 _tmp5FF;_tmp5FF.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp73E=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp323=_cycalloc(sizeof(*_tmp323));({void*_tmp73D=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->tag=0U,_tmp322->f1=_tmp370;_tmp322;});_tmp323->root=_tmp73D;}),_tmp323->path=path;_tmp323;}));_tmp5FF.f2=_tmp73E;});_tmp5FF;});case 1U: _LL12: _tmp371=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LL13:
# 1818
 return({struct _tuple18 _tmp600;_tmp600.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp73F=Cyc_CfFlowInfo_UnknownL();_tmp600.f2=_tmp73F;});_tmp600;});default: goto _LL1C;}case 22U: _LL14: _tmp373=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp372=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL15:
# 1821
{void*_tmp324=Cyc_Tcutil_compress((void*)_check_null(_tmp373->topt));void*_tmp325=_tmp324;void*_tmp32A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325)->tag == 5U){_LL1F: _tmp32A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325)->f1).elt_typ;_LL20:
# 1823
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp32A)){
Cyc_NewControlFlow_check_union_requires(_tmp373->loc,_tmp32A,_tmp372,inflow);
({struct Cyc_List_List*_tmp742=({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));({void*_tmp741=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->tag=0U,({int _tmp740=Cyc_CfFlowInfo_get_field_index(_tmp32A,_tmp372);_tmp326->f1=_tmp740;});_tmp326;});_tmp327->hd=_tmp741;}),_tmp327->tl=path;_tmp327;});path=_tmp742;});}
# 1827
goto _LL1E;}else{_LL21: _LL22:
({void*_tmp328=0U;({struct _dyneither_ptr _tmp743=({const char*_tmp329="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp329,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp743,_tag_dyneither(_tmp328,sizeof(void*),0U));});});}_LL1E:;}
# 1830
_tmp374=_tmp373;goto _LL17;case 20U: _LL16: _tmp374=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LL17:
# 1834
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp374->topt),1)){
# 1836
struct _tuple18 _tmp32B=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp374);struct _tuple18 _tmp32C=_tmp32B;union Cyc_CfFlowInfo_FlowInfo _tmp34B;union Cyc_CfFlowInfo_AbsLVal _tmp34A;_LL24: _tmp34B=_tmp32C.f1;_tmp34A=_tmp32C.f2;_LL25:;{
struct _tuple18 _tmp32D=({struct _tuple18 _tmp604;_tmp604.f1=_tmp34B,_tmp604.f2=_tmp34A;_tmp604;});struct _tuple18 _tmp32E=_tmp32D;struct Cyc_Dict_Dict _tmp349;struct Cyc_List_List*_tmp348;struct Cyc_CfFlowInfo_Place*_tmp347;if(((_tmp32E.f1).ReachableFL).tag == 2){if(((_tmp32E.f2).PlaceL).tag == 1){_LL27: _tmp349=(((_tmp32E.f1).ReachableFL).val).f1;_tmp348=(((_tmp32E.f1).ReachableFL).val).f2;_tmp347=((_tmp32E.f2).PlaceL).val;_LL28: {
# 1839
void*_tmp32F=Cyc_CfFlowInfo_lookup_place(_tmp349,_tmp347);
struct _tuple15 _tmp330=Cyc_CfFlowInfo_unname_rval(_tmp32F);struct _tuple15 _tmp331=_tmp330;void*_tmp346;struct Cyc_List_List*_tmp345;_LL2C: _tmp346=_tmp331.f1;_tmp345=_tmp331.f2;_LL2D:;
if(expand_unique){
int _tmp332=1;
void*_tmp333=_tmp346;struct Cyc_Absyn_Vardecl*_tmp340;void*_tmp33F;switch(*((int*)_tmp333)){case 8U: _LL2F: _tmp340=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp333)->f1;_tmp33F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp333)->f2;_LL30:
# 1845
({void*_tmp334=0U;({struct _dyneither_ptr _tmp744=({const char*_tmp335="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp335,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp744,_tag_dyneither(_tmp334,sizeof(void*),0U));});});case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp333)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1849
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1848
 return Cyc_NewControlFlow_anal_derefL(env,_tmp34B,_tmp374,_tmp34B,_tmp32F,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1853
struct _tuple18 _tmp336=Cyc_NewControlFlow_anal_derefL(env,_tmp34B,_tmp374,_tmp34B,_tmp32F,passthrough_consumes,0,path);struct _tuple18 _tmp337=_tmp336;union Cyc_CfFlowInfo_FlowInfo _tmp338;_LL3E: _tmp338=_tmp337.f1;_LL3F:;
return({struct _tuple18 _tmp601;_tmp601.f1=_tmp338,({union Cyc_CfFlowInfo_AbsLVal _tmp745=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp347,path));_tmp601.f2=_tmp745;});_tmp601;});}case 1U: _LL39: _LL3A:
# 1856
 _tmp332=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1863
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp31C,_tmp349,_tmp346);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp31C->unknown_all: _tmp31C->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->tag=5U,({void*_tmp748=({struct Cyc_CfFlowInfo_FlowEnv*_tmp747=_tmp31C;void*_tmp746=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp374->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp747,_tmp746,0,leaf);});_tmp33E->f1=_tmp748;});_tmp33E;});
for(0;_tmp345 != 0;_tmp345=_tmp345->tl){
({void*_tmp749=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->tag=8U,_tmp339->f1=(struct Cyc_Absyn_Vardecl*)_tmp345->hd,_tmp339->f2=res;_tmp339;});res=_tmp749;});}
({struct Cyc_Dict_Dict _tmp74A=Cyc_CfFlowInfo_assign_place(_tmp31C,e->loc,_tmp349,_tmp347,res);_tmp349=_tmp74A;});{
union Cyc_CfFlowInfo_FlowInfo _tmp33A=Cyc_CfFlowInfo_ReachableFL(_tmp349,_tmp348);
# 1876
struct _tuple18 _tmp33B=Cyc_NewControlFlow_anal_derefL(env,_tmp33A,_tmp374,_tmp33A,res,passthrough_consumes,_tmp332,path);struct _tuple18 _tmp33C=_tmp33B;union Cyc_CfFlowInfo_FlowInfo _tmp33D;_LL41: _tmp33D=_tmp33C.f1;_LL42:;
# 1878
return({struct _tuple18 _tmp602;_tmp602.f1=_tmp33D,({union Cyc_CfFlowInfo_AbsLVal _tmp74B=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp347,path));_tmp602.f2=_tmp74B;});_tmp602;});};}}_LL2E:;}else{
# 1882
void*_tmp341=_tmp346;switch(*((int*)_tmp341)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp341)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1887
struct _tuple18 _tmp342=Cyc_NewControlFlow_anal_derefL(env,_tmp34B,_tmp374,_tmp34B,_tmp32F,passthrough_consumes,0,path);struct _tuple18 _tmp343=_tmp342;union Cyc_CfFlowInfo_FlowInfo _tmp344;_LL4B: _tmp344=_tmp343.f1;_LL4C:;
return({struct _tuple18 _tmp603;_tmp603.f1=_tmp344,({union Cyc_CfFlowInfo_AbsLVal _tmp74C=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp347,path));_tmp603.f2=_tmp74C;});_tmp603;});}default: _LL48: _LL49:
# 1891
 goto _LL43;}_LL43:;}
# 1894
goto _LL26;}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;};}{
# 1899
struct _tuple16 _tmp34C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp374);struct _tuple16 _tmp34D=_tmp34C;union Cyc_CfFlowInfo_FlowInfo _tmp34F;void*_tmp34E;_LL4E: _tmp34F=_tmp34D.f1;_tmp34E=_tmp34D.f2;_LL4F:;
# 1901
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp374,_tmp34F,_tmp34E,passthrough_consumes,0,path);};case 23U: _LL18: _tmp376=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp375=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL19: {
# 1904
void*_tmp350=Cyc_Tcutil_compress((void*)_check_null(_tmp376->topt));void*_tmp351=_tmp350;switch(*((int*)_tmp351)){case 10U: _LL51: _LL52: {
# 1906
unsigned int _tmp352=(Cyc_Evexp_eval_const_uint_exp(_tmp375)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp752=env;union Cyc_CfFlowInfo_FlowInfo _tmp751=inflow;int _tmp750=expand_unique;int _tmp74F=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp74E=_tmp376;Cyc_NewControlFlow_anal_Lexp_rec(_tmp752,_tmp751,_tmp750,_tmp74F,_tmp74E,({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));
({void*_tmp74D=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->tag=0U,_tmp353->f1=(int)_tmp352;_tmp353;});_tmp354->hd=_tmp74D;}),_tmp354->tl=path;_tmp354;}));});}case 5U: _LL53: _LL54: {
# 1910
struct _tuple23 _tmp355=({struct Cyc_NewControlFlow_AnalEnv*_tmp754=env;union Cyc_CfFlowInfo_FlowInfo _tmp753=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp754,_tmp753,({struct Cyc_Absyn_Exp*_tmp365[2U];_tmp365[0]=_tmp376,_tmp365[1]=_tmp375;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp365,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp356=_tmp355;union Cyc_CfFlowInfo_FlowInfo _tmp364;struct Cyc_List_List*_tmp363;_LL58: _tmp364=_tmp356.f1;_tmp363=_tmp356.f2;_LL59:;{
union Cyc_CfFlowInfo_FlowInfo _tmp357=_tmp364;
{union Cyc_CfFlowInfo_FlowInfo _tmp358=_tmp364;struct Cyc_Dict_Dict _tmp35D;struct Cyc_List_List*_tmp35C;if((_tmp358.ReachableFL).tag == 2){_LL5B: _tmp35D=((_tmp358.ReachableFL).val).f1;_tmp35C=((_tmp358.ReachableFL).val).f2;_LL5C:
# 1914
 if(Cyc_CfFlowInfo_initlevel(_tmp31C,_tmp35D,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp363))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp359=0U;({unsigned int _tmp756=_tmp375->loc;struct _dyneither_ptr _tmp755=({const char*_tmp35A="expression may not be initialized";_tag_dyneither(_tmp35A,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp756,_tmp755,_tag_dyneither(_tmp359,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp35B=Cyc_NewControlFlow_add_subscript_reln(_tmp35C,_tmp376,_tmp375);
if(_tmp35C != _tmp35B)
({union Cyc_CfFlowInfo_FlowInfo _tmp757=Cyc_CfFlowInfo_ReachableFL(_tmp35D,_tmp35B);_tmp357=_tmp757;});
goto _LL5A;};}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1923
struct _tuple18 _tmp35E=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp376,_tmp364,(void*)((struct Cyc_List_List*)_check_null(_tmp363))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp35F=_tmp35E;union Cyc_CfFlowInfo_FlowInfo _tmp362;union Cyc_CfFlowInfo_AbsLVal _tmp361;_LL60: _tmp362=_tmp35F.f1;_tmp361=_tmp35F.f2;_LL61:;{
union Cyc_CfFlowInfo_FlowInfo _tmp360=_tmp362;if((_tmp360.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple18 _tmp605;_tmp605.f1=_tmp362,_tmp605.f2=_tmp361;_tmp605;});}else{_LL65: _LL66:
 return({struct _tuple18 _tmp606;_tmp606.f1=_tmp357,_tmp606.f2=_tmp361;_tmp606;});}_LL62:;};};};}default: _LL55: _LL56:
# 1928
({void*_tmp366=0U;({struct _dyneither_ptr _tmp758=({const char*_tmp367="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp367,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp758,_tag_dyneither(_tmp366,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp378=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp377=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL1B: {
# 1932
void*_tmp368=(void*)_check_null(_tmp378->topt);
if(Cyc_Absyn_is_require_union_type(_tmp368))
Cyc_NewControlFlow_check_union_requires(_tmp378->loc,_tmp368,_tmp377,inflow);
# 1936
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp368))
return({struct _tuple18 _tmp607;_tmp607.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp759=Cyc_CfFlowInfo_UnknownL();_tmp607.f2=_tmp759;});_tmp607;});
# 1939
return({struct Cyc_NewControlFlow_AnalEnv*_tmp760=env;union Cyc_CfFlowInfo_FlowInfo _tmp75F=inflow;int _tmp75E=expand_unique;int _tmp75D=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp75C=_tmp378;Cyc_NewControlFlow_anal_Lexp_rec(_tmp760,_tmp75F,_tmp75E,_tmp75D,_tmp75C,({struct Cyc_List_List*_tmp36A=_cycalloc(sizeof(*_tmp36A));
({void*_tmp75B=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->tag=0U,({int _tmp75A=Cyc_CfFlowInfo_get_field_index(_tmp368,_tmp377);_tmp369->f1=_tmp75A;});_tmp369;});_tmp36A->hd=_tmp75B;}),_tmp36A->tl=path;_tmp36A;}));});}default: _LL1C: _LL1D:
# 1943
 return({struct _tuple18 _tmp608;({union Cyc_CfFlowInfo_FlowInfo _tmp762=Cyc_CfFlowInfo_BottomFL();_tmp608.f1=_tmp762;}),({union Cyc_CfFlowInfo_AbsLVal _tmp761=Cyc_CfFlowInfo_UnknownL();_tmp608.f2=_tmp761;});_tmp608;});}_LL5:;};}
# 1947
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1951
struct _tuple18 _tmp379=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp37A=_tmp379;union Cyc_CfFlowInfo_FlowInfo _tmp37C;union Cyc_CfFlowInfo_AbsLVal _tmp37B;_LL1: _tmp37C=_tmp37A.f1;_tmp37B=_tmp37A.f2;_LL2:;
return({struct _tuple18 _tmp609;_tmp609.f1=_tmp37C,_tmp609.f2=_tmp37B;_tmp609;});}
# 1958
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1961
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp37D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp37E=_tmp37D;union Cyc_CfFlowInfo_FlowInfo _tmp37F;_LL1: _tmp37F=_tmp37E.f1;_LL2:;
inflow=_tmp37F;}}}
# 1969
return inflow;}
# 1975
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1977
struct Cyc_CfFlowInfo_FlowEnv*_tmp380=env->fenv;
# 1980
struct _tuple16 _tmp381=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);struct _tuple16 _tmp382=_tmp381;union Cyc_CfFlowInfo_FlowInfo _tmp3BF;void*_tmp3BE;_LL1: _tmp3BF=_tmp382.f1;_tmp3BE=_tmp382.f2;_LL2:;{
struct _tuple16 _tmp383=Cyc_NewControlFlow_anal_Rexp(env,0,_tmp3BF,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);struct _tuple16 _tmp384=_tmp383;union Cyc_CfFlowInfo_FlowInfo _tmp3BD;void*_tmp3BC;_LL4: _tmp3BD=_tmp384.f1;_tmp3BC=_tmp384.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp385=_tmp3BD;
# 1985
union Cyc_CfFlowInfo_FlowInfo _tmp386=_tmp385;struct Cyc_Dict_Dict _tmp3BB;struct Cyc_List_List*_tmp3BA;if((_tmp386.BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple19 _tmp60A;_tmp60A.f1=_tmp385,_tmp60A.f2=_tmp385;_tmp60A;});}else{_LL9: _tmp3BB=((_tmp386.ReachableFL).val).f1;_tmp3BA=((_tmp386.ReachableFL).val).f2;_LLA: {
# 1988
struct Cyc_Absyn_Exp*_tmp387=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*_tmp388=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BB,_tmp3BE)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp3BE))
({void*_tmp389=0U;({unsigned int _tmp764=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp763=({const char*_tmp38A="expression may not be initialized";_tag_dyneither(_tmp38A,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp764,_tmp763,_tag_dyneither(_tmp389,sizeof(void*),0U));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BB,_tmp3BC)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp3BE))
({void*_tmp38B=0U;({unsigned int _tmp766=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp765=({const char*_tmp38C="expression may not be initialized";_tag_dyneither(_tmp38C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp766,_tmp765,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});{
# 1995
union Cyc_CfFlowInfo_FlowInfo _tmp38D=_tmp385;
union Cyc_CfFlowInfo_FlowInfo _tmp38E=_tmp385;
# 2000
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp38F=Cyc_CfFlowInfo_unname_rval(_tmp3BE);struct _tuple15 _tmp390=_tmp38F;void*_tmp3A8;struct Cyc_List_List*_tmp3A7;_LLC: _tmp3A8=_tmp390.f1;_tmp3A7=_tmp390.f2;_LLD:;{
struct _tuple15 _tmp391=Cyc_CfFlowInfo_unname_rval(_tmp3BC);struct _tuple15 _tmp392=_tmp391;void*_tmp3A6;struct Cyc_List_List*_tmp3A5;_LLF: _tmp3A6=_tmp392.f1;_tmp3A5=_tmp392.f2;_LL10:;{
struct _tuple0 _tmp393=({struct _tuple0 _tmp60B;_tmp60B.f1=_tmp3A8,_tmp60B.f2=_tmp3A6;_tmp60B;});struct _tuple0 _tmp394=_tmp393;enum Cyc_CfFlowInfo_InitLevel _tmp3A4;enum Cyc_CfFlowInfo_InitLevel _tmp3A3;switch(*((int*)_tmp394.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->tag == 0U){_LL12: _tmp3A3=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp394.f1)->f1;_LL13: {
# 2005
struct _tuple19 _tmp395=Cyc_NewControlFlow_splitzero(env,inflow,_tmp385,_tmp387,_tmp3A3,_tmp3A7);struct _tuple19 _tmp396=_tmp395;union Cyc_CfFlowInfo_FlowInfo _tmp39B;union Cyc_CfFlowInfo_FlowInfo _tmp39A;_LL23: _tmp39B=_tmp396.f1;_tmp39A=_tmp396.f2;_LL24:;
{enum Cyc_Absyn_Primop _tmp397=p;switch(_tmp397){case Cyc_Absyn_Eq: _LL26: _LL27:
 _tmp38D=_tmp39A;_tmp38E=_tmp39B;goto _LL25;case Cyc_Absyn_Neq: _LL28: _LL29:
 _tmp38D=_tmp39B;_tmp38E=_tmp39A;goto _LL25;default: _LL2A: _LL2B:
({void*_tmp398=0U;({struct _dyneither_ptr _tmp767=({const char*_tmp399="anal_test, zero-split";_tag_dyneither(_tmp399,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp767,_tag_dyneither(_tmp398,sizeof(void*),0U));});});}_LL25:;}
# 2011
goto _LL11;}}else{goto _LL20;}case 0U: switch(*((int*)_tmp394.f2)){case 2U: _LL14: _tmp3A4=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->f1;_LL15: {
# 2013
struct _tuple19 _tmp39C=Cyc_NewControlFlow_splitzero(env,_tmp3BD,_tmp385,_tmp388,_tmp3A4,_tmp3A5);struct _tuple19 _tmp39D=_tmp39C;union Cyc_CfFlowInfo_FlowInfo _tmp3A2;union Cyc_CfFlowInfo_FlowInfo _tmp3A1;_LL2D: _tmp3A2=_tmp39D.f1;_tmp3A1=_tmp39D.f2;_LL2E:;
{enum Cyc_Absyn_Primop _tmp39E=p;switch(_tmp39E){case Cyc_Absyn_Eq: _LL30: _LL31:
 _tmp38D=_tmp3A1;_tmp38E=_tmp3A2;goto _LL2F;case Cyc_Absyn_Neq: _LL32: _LL33:
 _tmp38D=_tmp3A2;_tmp38E=_tmp3A1;goto _LL2F;default: _LL34: _LL35:
({void*_tmp39F=0U;({struct _dyneither_ptr _tmp768=({const char*_tmp3A0="anal_test, zero-split";_tag_dyneither(_tmp3A0,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp768,_tag_dyneither(_tmp39F,sizeof(void*),0U));});});}_LL2F:;}
# 2019
goto _LL11;}case 0U: _LL16: _LL17:
# 2021
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp769=Cyc_CfFlowInfo_BottomFL();_tmp38E=_tmp769;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp76A=Cyc_CfFlowInfo_BottomFL();_tmp38D=_tmp76A;});}
goto _LL11;case 1U: _LL18: _LL19:
 goto _LL1B;case 4U: _LL1A: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->tag == 0U){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL20;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->tag == 0U){_LL1E: _LL1F:
# 2028
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp76B=Cyc_CfFlowInfo_BottomFL();_tmp38E=_tmp76B;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp76C=Cyc_CfFlowInfo_BottomFL();_tmp38D=_tmp76C;});}
goto _LL11;}else{goto _LL20;}default: _LL20: _LL21:
 goto _LL11;}_LL11:;};};}
# 2039
{struct _tuple0 _tmp3A9=({struct _tuple0 _tmp60D;({void*_tmp76E=Cyc_Tcutil_compress((void*)_check_null(_tmp387->topt));_tmp60D.f1=_tmp76E;}),({
void*_tmp76D=Cyc_Tcutil_compress((void*)_check_null(_tmp388->topt));_tmp60D.f2=_tmp76D;});_tmp60D;});
# 2039
struct _tuple0 _tmp3AA=_tmp3A9;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AA.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AA.f1)->f1 == Cyc_Absyn_Unsigned){_LL37: _LL38:
# 2041
 goto _LL3A;}else{switch(*((int*)_tmp3AA.f2)){case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AA.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL39;else{goto _LL3F;}case 19U: goto _LL3D;default: goto _LL3F;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AA.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AA.f2)->f1 == Cyc_Absyn_Unsigned){_LL39: _LL3A:
 goto _LL3C;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3AA.f1)->tag == 19U)goto _LL3B;else{goto _LL3F;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3AA.f1)->tag == 19U){_LL3B: _LL3C:
 goto _LL3E;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3AA.f2)->tag == 19U){_LL3D: _LL3E:
 goto _LL36;}else{_LL3F: _LL40:
 return({struct _tuple19 _tmp60C;_tmp60C.f1=_tmp38D,_tmp60C.f2=_tmp38E;_tmp60C;});}}}}_LL36:;}
# 2049
{enum Cyc_Absyn_Primop _tmp3AB=p;switch(_tmp3AB){case Cyc_Absyn_Eq: _LL42: _LL43:
 goto _LL45;case Cyc_Absyn_Neq: _LL44: _LL45: goto _LL47;case Cyc_Absyn_Gt: _LL46: _LL47: goto _LL49;case Cyc_Absyn_Gte: _LL48: _LL49: goto _LL4B;case Cyc_Absyn_Lt: _LL4A: _LL4B: goto _LL4D;case Cyc_Absyn_Lte: _LL4C: _LL4D: goto _LL41;default: _LL4E: _LL4F:
 return({struct _tuple19 _tmp60E;_tmp60E.f1=_tmp38D,_tmp60E.f2=_tmp38E;_tmp60E;});}_LL41:;}{
# 2054
struct _RegionHandle*_tmp3AC=Cyc_Core_heap_region;
struct _tuple13 _tmp3AD=Cyc_Relations_primop2relation(_tmp387,p,_tmp388);struct _tuple13 _tmp3AE=_tmp3AD;struct Cyc_Absyn_Exp*_tmp3B9;enum Cyc_Relations_Relation _tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;_LL51: _tmp3B9=_tmp3AE.f1;_tmp3B8=_tmp3AE.f2;_tmp3B7=_tmp3AE.f3;_LL52:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2059
if(Cyc_Relations_exp2relnop(_tmp3B9,& n1) && Cyc_Relations_exp2relnop(_tmp3B7,& n2)){
# 2061
struct Cyc_List_List*_tmp3AF=Cyc_Relations_add_relation(_tmp3AC,n1,_tmp3B8,n2,_tmp3BA);
# 2065
struct Cyc_List_List*_tmp3B0=({struct _RegionHandle*_tmp772=_tmp3AC;union Cyc_Relations_RelnOp _tmp771=n2;enum Cyc_Relations_Relation _tmp770=Cyc_Relations_flip_relation(_tmp3B8);union Cyc_Relations_RelnOp _tmp76F=n1;Cyc_Relations_add_relation(_tmp772,_tmp771,_tmp770,_tmp76F,_tmp3BA);});
struct _tuple19 _tmp3B1=({struct _tuple19 _tmp613;_tmp613.f1=_tmp38D,_tmp613.f2=_tmp38E;_tmp613;});struct _tuple19 _tmp3B2=_tmp3B1;struct Cyc_Dict_Dict _tmp3B6;struct Cyc_Dict_Dict _tmp3B5;struct Cyc_Dict_Dict _tmp3B4;struct Cyc_Dict_Dict _tmp3B3;if(((_tmp3B2.f1).ReachableFL).tag == 2){if(((_tmp3B2.f2).ReachableFL).tag == 2){_LL54: _tmp3B4=(((_tmp3B2.f1).ReachableFL).val).f1;_tmp3B3=(((_tmp3B2.f2).ReachableFL).val).f1;_LL55:
# 2068
 return({struct _tuple19 _tmp60F;({union Cyc_CfFlowInfo_FlowInfo _tmp774=Cyc_CfFlowInfo_ReachableFL(_tmp3B4,_tmp3AF);_tmp60F.f1=_tmp774;}),({union Cyc_CfFlowInfo_FlowInfo _tmp773=Cyc_CfFlowInfo_ReachableFL(_tmp3B3,_tmp3B0);_tmp60F.f2=_tmp773;});_tmp60F;});}else{_LL58: _tmp3B5=(((_tmp3B2.f1).ReachableFL).val).f1;_LL59:
# 2072
 return({struct _tuple19 _tmp610;({union Cyc_CfFlowInfo_FlowInfo _tmp775=Cyc_CfFlowInfo_ReachableFL(_tmp3B5,_tmp3AF);_tmp610.f1=_tmp775;}),_tmp610.f2=_tmp38E;_tmp610;});}}else{if(((_tmp3B2.f2).ReachableFL).tag == 2){_LL56: _tmp3B6=(((_tmp3B2.f2).ReachableFL).val).f1;_LL57:
# 2070
 return({struct _tuple19 _tmp611;_tmp611.f1=_tmp38D,({union Cyc_CfFlowInfo_FlowInfo _tmp776=Cyc_CfFlowInfo_ReachableFL(_tmp3B6,_tmp3B0);_tmp611.f2=_tmp776;});_tmp611;});}else{_LL5A: _LL5B:
# 2074
 return({struct _tuple19 _tmp612;_tmp612.f1=_tmp38D,_tmp612.f2=_tmp38E;_tmp612;});}}_LL53:;}else{
# 2077
return({struct _tuple19 _tmp614;_tmp614.f1=_tmp38D,_tmp614.f2=_tmp38E;_tmp614;});}};};};}}_LL6:;};};}
# 2083
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C0=env->fenv;
void*_tmp3C1=e->r;void*_tmp3C2=_tmp3C1;enum Cyc_Absyn_Primop _tmp406;struct Cyc_List_List*_tmp405;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;switch(*((int*)_tmp3C2)){case 6U: _LL1: _tmp3FD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3FC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_tmp3FB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f3;_LL2: {
# 2087
struct _tuple19 _tmp3C3=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3FD);struct _tuple19 _tmp3C4=_tmp3C3;union Cyc_CfFlowInfo_FlowInfo _tmp3CE;union Cyc_CfFlowInfo_FlowInfo _tmp3CD;_LL10: _tmp3CE=_tmp3C4.f1;_tmp3CD=_tmp3C4.f2;_LL11:;{
struct _tuple19 _tmp3C5=Cyc_NewControlFlow_anal_test(env,_tmp3CE,_tmp3FC);struct _tuple19 _tmp3C6=_tmp3C5;union Cyc_CfFlowInfo_FlowInfo _tmp3CC;union Cyc_CfFlowInfo_FlowInfo _tmp3CB;_LL13: _tmp3CC=_tmp3C6.f1;_tmp3CB=_tmp3C6.f2;_LL14:;{
struct _tuple19 _tmp3C7=Cyc_NewControlFlow_anal_test(env,_tmp3CD,_tmp3FB);struct _tuple19 _tmp3C8=_tmp3C7;union Cyc_CfFlowInfo_FlowInfo _tmp3CA;union Cyc_CfFlowInfo_FlowInfo _tmp3C9;_LL16: _tmp3CA=_tmp3C8.f1;_tmp3C9=_tmp3C8.f2;_LL17:;
return({struct _tuple19 _tmp615;({union Cyc_CfFlowInfo_FlowInfo _tmp778=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3CC,_tmp3CA);_tmp615.f1=_tmp778;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp777=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3CB,_tmp3C9);_tmp615.f2=_tmp777;});_tmp615;});};};}case 7U: _LL3: _tmp3FF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3FE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL4: {
# 2093
struct _tuple19 _tmp3CF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3FF);struct _tuple19 _tmp3D0=_tmp3CF;union Cyc_CfFlowInfo_FlowInfo _tmp3D6;union Cyc_CfFlowInfo_FlowInfo _tmp3D5;_LL19: _tmp3D6=_tmp3D0.f1;_tmp3D5=_tmp3D0.f2;_LL1A:;{
struct _tuple19 _tmp3D1=Cyc_NewControlFlow_anal_test(env,_tmp3D6,_tmp3FE);struct _tuple19 _tmp3D2=_tmp3D1;union Cyc_CfFlowInfo_FlowInfo _tmp3D4;union Cyc_CfFlowInfo_FlowInfo _tmp3D3;_LL1C: _tmp3D4=_tmp3D2.f1;_tmp3D3=_tmp3D2.f2;_LL1D:;
return({struct _tuple19 _tmp616;_tmp616.f1=_tmp3D4,({union Cyc_CfFlowInfo_FlowInfo _tmp779=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3D5,_tmp3D3);_tmp616.f2=_tmp779;});_tmp616;});};}case 8U: _LL5: _tmp401=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp400=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL6: {
# 2097
struct _tuple19 _tmp3D7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp401);struct _tuple19 _tmp3D8=_tmp3D7;union Cyc_CfFlowInfo_FlowInfo _tmp3DE;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;_LL1F: _tmp3DE=_tmp3D8.f1;_tmp3DD=_tmp3D8.f2;_LL20:;{
struct _tuple19 _tmp3D9=Cyc_NewControlFlow_anal_test(env,_tmp3DD,_tmp400);struct _tuple19 _tmp3DA=_tmp3D9;union Cyc_CfFlowInfo_FlowInfo _tmp3DC;union Cyc_CfFlowInfo_FlowInfo _tmp3DB;_LL22: _tmp3DC=_tmp3DA.f1;_tmp3DB=_tmp3DA.f2;_LL23:;
return({struct _tuple19 _tmp617;({union Cyc_CfFlowInfo_FlowInfo _tmp77A=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3DE,_tmp3DC);_tmp617.f1=_tmp77A;}),_tmp617.f2=_tmp3DB;_tmp617;});};}case 9U: _LL7: _tmp403=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp402=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL8: {
# 2101
struct _tuple16 _tmp3DF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp403);struct _tuple16 _tmp3E0=_tmp3DF;union Cyc_CfFlowInfo_FlowInfo _tmp3E2;void*_tmp3E1;_LL25: _tmp3E2=_tmp3E0.f1;_tmp3E1=_tmp3E0.f2;_LL26:;
return Cyc_NewControlFlow_anal_test(env,_tmp3E2,_tmp402);}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->tl == 0){_LL9: _tmp404=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->hd;_LLA: {
# 2104
struct _tuple19 _tmp3E3=Cyc_NewControlFlow_anal_test(env,inflow,_tmp404);struct _tuple19 _tmp3E4=_tmp3E3;union Cyc_CfFlowInfo_FlowInfo _tmp3E6;union Cyc_CfFlowInfo_FlowInfo _tmp3E5;_LL28: _tmp3E6=_tmp3E4.f1;_tmp3E5=_tmp3E4.f2;_LL29:;
return({struct _tuple19 _tmp618;_tmp618.f1=_tmp3E5,_tmp618.f2=_tmp3E6;_tmp618;});}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp406=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp405=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LLC:
# 2107
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp406,_tmp405);}default: _LLD: _LLE: {
# 2111
struct _tuple16 _tmp3E7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp3E8=_tmp3E7;union Cyc_CfFlowInfo_FlowInfo _tmp3FA;void*_tmp3F9;_LL2B: _tmp3FA=_tmp3E8.f1;_tmp3F9=_tmp3E8.f2;_LL2C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E9=_tmp3FA;struct Cyc_Dict_Dict _tmp3F8;if((_tmp3E9.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple19 _tmp619;_tmp619.f1=_tmp3FA,_tmp619.f2=_tmp3FA;_tmp619;});}else{_LL30: _tmp3F8=((_tmp3E9.ReachableFL).val).f1;_LL31: {
# 2115
struct _tuple15 _tmp3EA=Cyc_CfFlowInfo_unname_rval(_tmp3F9);struct _tuple15 _tmp3EB=_tmp3EA;void*_tmp3F7;struct Cyc_List_List*_tmp3F6;_LL33: _tmp3F7=_tmp3EB.f1;_tmp3F6=_tmp3EB.f2;_LL34:;{
void*_tmp3EC=_tmp3F7;enum Cyc_CfFlowInfo_InitLevel _tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F4;void*_tmp3F3;switch(*((int*)_tmp3EC)){case 8U: _LL36: _tmp3F4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1;_tmp3F3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f2;_LL37:
# 2118
({void*_tmp3ED=0U;({struct _dyneither_ptr _tmp77B=({const char*_tmp3EE="anal_test: bad namedlocation";_tag_dyneither(_tmp3EE,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp77B,_tag_dyneither(_tmp3ED,sizeof(void*),0U));});});case 0U: _LL38: _LL39:
 return({struct _tuple19 _tmp61A;({union Cyc_CfFlowInfo_FlowInfo _tmp77C=Cyc_CfFlowInfo_BottomFL();_tmp61A.f1=_tmp77C;}),_tmp61A.f2=_tmp3FA;_tmp61A;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple19 _tmp61B;_tmp61B.f1=_tmp3FA,({union Cyc_CfFlowInfo_FlowInfo _tmp77D=Cyc_CfFlowInfo_BottomFL();_tmp61B.f2=_tmp77D;});_tmp61B;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp3F5=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1;_LL47:
# 2128
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp3FA,e,_tmp3F5,_tmp3F6);}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2124
 goto _LL45;}else{_LL48: _LL49:
# 2129
 return({struct _tuple19 _tmp61C;_tmp61C.f1=_tmp3FA,_tmp61C.f2=_tmp3FA;_tmp61C;});}case 7U: _LL44: _LL45:
# 2126
({void*_tmp3EF=0U;({unsigned int _tmp77F=e->loc;struct _dyneither_ptr _tmp77E=({const char*_tmp3F0="expression may not be initialized";_tag_dyneither(_tmp3F0,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp77F,_tmp77E,_tag_dyneither(_tmp3EF,sizeof(void*),0U));});});
return({struct _tuple19 _tmp61D;({union Cyc_CfFlowInfo_FlowInfo _tmp781=Cyc_CfFlowInfo_BottomFL();_tmp61D.f1=_tmp781;}),({union Cyc_CfFlowInfo_FlowInfo _tmp780=Cyc_CfFlowInfo_BottomFL();_tmp61D.f2=_tmp780;});_tmp61D;});default: _LL4A: _LL4B:
# 2130
({void*_tmp3F1=0U;({struct _dyneither_ptr _tmp782=({const char*_tmp3F2="anal_test";_tag_dyneither(_tmp3F2,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp782,_tag_dyneither(_tmp3F1,sizeof(void*),0U));});});}_LL35:;};}}_LL2D:;};}}_LL0:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2136
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2138
struct _tuple27*_tmp407=ckenv;unsigned int _tmp413;struct Cyc_NewControlFlow_AnalEnv*_tmp412;struct Cyc_Dict_Dict _tmp411;_LL1: _tmp413=_tmp407->f1;_tmp412=_tmp407->f2;_tmp411=_tmp407->f3;_LL2:;{
void*_tmp408=root;struct Cyc_Absyn_Vardecl*_tmp410;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp408)->tag == 0U){_LL4: _tmp410=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp408)->f1;_LL5:
# 2141
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp410->type)){
struct _tuple15 _tmp409=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple15 _tmp40A=_tmp409;void*_tmp40F;_LL9: _tmp40F=_tmp40A.f1;_LLA:;{
void*_tmp40B=_tmp40F;switch(*((int*)_tmp40B)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp40B)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2148
({struct Cyc_String_pa_PrintArg_struct _tmp40E=({struct Cyc_String_pa_PrintArg_struct _tmp61E;_tmp61E.tag=0U,({struct _dyneither_ptr _tmp783=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp410->name));_tmp61E.f1=_tmp783;});_tmp61E;});void*_tmp40C[1U];_tmp40C[0]=& _tmp40E;({unsigned int _tmp785=_tmp413;struct _dyneither_ptr _tmp784=({const char*_tmp40D="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp40D,sizeof(char),57U);});Cyc_Warn_warn(_tmp785,_tmp784,_tag_dyneither(_tmp40C,sizeof(void*),1U));});});
goto _LLB;}_LLB:;};}
# 2152
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 2157
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp414=flow;struct Cyc_Dict_Dict _tmp419;if((_tmp414.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp419=((_tmp414.ReachableFL).val).f1;_LL4:
# 2161
{struct Cyc_List_List*_tmp415=env->param_roots;for(0;_tmp415 != 0;_tmp415=_tmp415->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp787=env->fenv;struct Cyc_Dict_Dict _tmp786=_tmp419;Cyc_CfFlowInfo_initlevel(_tmp787,_tmp786,Cyc_CfFlowInfo_lookup_place(_tmp419,(struct Cyc_CfFlowInfo_Place*)_tmp415->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp416=0U;({unsigned int _tmp789=loc;struct _dyneither_ptr _tmp788=({const char*_tmp417="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp417,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp789,_tmp788,_tag_dyneither(_tmp416,sizeof(void*),0U));});});}}
# 2165
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp418=({struct _tuple27 _tmp61F;_tmp61F.f1=loc,_tmp61F.f2=env,_tmp61F.f3=_tmp419;_tmp61F;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp418,_tmp419);}
# 2169
return;}_LL0:;}
# 2178
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2180
struct Cyc_List_List*_tmp41A=0;
struct Cyc_List_List*_tmp41B=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp41C=(struct _tuple24*)x->hd;struct _tuple24*_tmp41D=_tmp41C;struct Cyc_Absyn_Vardecl**_tmp425;struct Cyc_Absyn_Exp*_tmp424;_LL1: _tmp425=_tmp41D->f1;_tmp424=_tmp41D->f2;_LL2:;
if((_tmp425 != 0  && _tmp424 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp424->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp41E=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=0U,_tmp423->f1=*_tmp425;_tmp423;});
struct Cyc_CfFlowInfo_Place*_tmp41F=({struct Cyc_CfFlowInfo_Place*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->root=(void*)_tmp41E,_tmp422->path=0;_tmp422;});
({struct Cyc_List_List*_tmp78A=({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->hd=_tmp41F,_tmp420->tl=_tmp41A;_tmp420;});_tmp41A=_tmp78A;});
({struct Cyc_List_List*_tmp78B=({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->hd=_tmp424,_tmp421->tl=_tmp41B;_tmp421;});_tmp41B=_tmp78B;});}}}
# 2192
return({struct _tuple1 _tmp620;_tmp620.f1=_tmp41A,_tmp620.f2=_tmp41B;_tmp620;});}struct _tuple28{int f1;void*f2;};
# 2198
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2205
union Cyc_CfFlowInfo_FlowInfo _tmp426=flow;struct Cyc_Dict_Dict _tmp43A;if((_tmp426.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp427=0U;({struct _dyneither_ptr _tmp78C=({const char*_tmp428="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp428,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp78C,_tag_dyneither(_tmp427,sizeof(void*),0U));});});}else{_LL3: _tmp43A=((_tmp426.ReachableFL).val).f1;_LL4: {
# 2213
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_exp_unsigned_one;
int _tmp42A=0;
int _tmp42B=1;
void*_tmp42C=Cyc_CfFlowInfo_lookup_place(_tmp43A,place);
void*_tmp42D=_tmp42C;
# 2225
int varok=0;
{void*_tmp42E=_tmp42C;struct Cyc_Absyn_Vardecl*_tmp439;void*_tmp438;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42E)->tag == 8U){_LL6: _tmp439=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42E)->f1;_tmp438=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42E)->f2;_LL7:
# 2228
 if(vd == _tmp439){
_tmp42D=_tmp438;
# 2231
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2233
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp429,_tmp42B,_tmp42D,& _tmp42A)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp431=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,({
# 2237
struct _dyneither_ptr _tmp78D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp621.f1=_tmp78D;});_tmp621;});void*_tmp42F[1U];_tmp42F[0]=& _tmp431;({unsigned int _tmp78F=loc;struct _dyneither_ptr _tmp78E=({const char*_tmp430="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp430,sizeof(char),58U);});Cyc_CfFlowInfo_aerr(_tmp78F,_tmp78E,_tag_dyneither(_tmp42F,sizeof(void*),1U));});});}else{
# 2240
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp43A,_tmp42D)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp434=({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U,({
# 2243
struct _dyneither_ptr _tmp790=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp622.f1=_tmp790;});_tmp622;});void*_tmp432[1U];_tmp432[0]=& _tmp434;({unsigned int _tmp792=loc;struct _dyneither_ptr _tmp791=({const char*_tmp433="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp433,sizeof(char),84U);});Cyc_CfFlowInfo_aerr(_tmp792,_tmp791,_tag_dyneither(_tmp432,sizeof(void*),1U));});});else{
# 2245
varok=1;}}}else{
# 2248
varok=1;}}else{
# 2251
goto _LL9;}
goto _LL5;}else{_LL8: _LL9:
# 2255
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp437=({struct Cyc_String_pa_PrintArg_struct _tmp623;_tmp623.tag=0U,({
# 2260
struct _dyneither_ptr _tmp793=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp623.f1=_tmp793;});_tmp623;});void*_tmp435[1U];_tmp435[0]=& _tmp437;({unsigned int _tmp795=loc;struct _dyneither_ptr _tmp794=({const char*_tmp436="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp436,sizeof(char),85U);});Cyc_CfFlowInfo_aerr(_tmp795,_tmp794,_tag_dyneither(_tmp435,sizeof(void*),1U));});});}
goto _LL5;}_LL5:;}
# 2267
return({struct _tuple28 _tmp624;_tmp624.f1=varok,_tmp624.f2=_tmp42D;_tmp624;});}}_LL0:;}
# 2273
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp43B=p;void*_tmp443;struct Cyc_List_List*_tmp442;_LL1: _tmp443=_tmp43B->root;_tmp442=_tmp43B->path;_LL2:;
if(_tmp442 != 0)
({void*_tmp43C=0U;({struct _dyneither_ptr _tmp796=({const char*_tmp43D="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp43D,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp796,_tag_dyneither(_tmp43C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp43E=_tmp443;struct Cyc_Absyn_Vardecl*_tmp441;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp43E)->tag == 0U){_LL4: _tmp441=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp43E)->f1;_LL5:
 return _tmp441;}else{_LL6: _LL7:
({void*_tmp43F=0U;({struct _dyneither_ptr _tmp797=({const char*_tmp440="unconsume_params: root is not a varroot";_tag_dyneither(_tmp440,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp797,_tag_dyneither(_tmp43F,sizeof(void*),0U));});});}_LL3:;};}
# 2292 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2300
{union Cyc_CfFlowInfo_FlowInfo _tmp444=flow;struct Cyc_Dict_Dict _tmp45B;if((_tmp444.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp45B=((_tmp444.ReachableFL).val).f1;_LL4: {
# 2307
struct _tuple18 _tmp445=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp446=_tmp445;union Cyc_CfFlowInfo_FlowInfo _tmp45A;union Cyc_CfFlowInfo_AbsLVal _tmp459;_LL6: _tmp45A=_tmp446.f1;_tmp459=_tmp446.f2;_LL7:;
# 2310
{union Cyc_CfFlowInfo_AbsLVal _tmp447=_tmp459;struct Cyc_CfFlowInfo_Place*_tmp458;if((_tmp447.PlaceL).tag == 1){_LL9: _tmp458=(_tmp447.PlaceL).val;_LLA: {
# 2314
void*_tmp448=Cyc_CfFlowInfo_lookup_place(_tmp45B,_tmp458);
{void*_tmp449=_tmp448;struct Cyc_Absyn_Vardecl*_tmp457;void*_tmp456;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp449)->tag == 8U){_LLE: _tmp457=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp449)->f1;_tmp456=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp449)->f2;_LLF: {
# 2320
void*new_rval;
if(_tmp457 == vd){
# 2324
if(varok){
# 2326
({void*_tmp798=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp456);_tmp448=_tmp798;});
# 2331
if(ropt != 0){
# 2337
struct _tuple16 _tmp44A=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp79A=env->fenv;struct _tuple16 _tmp799=({struct _tuple16 _tmp625;_tmp625.f1=_tmp45A,_tmp625.f2=_tmp448;_tmp625;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp79A,_tmp799,({struct _tuple16 _tmp626;_tmp626.f1=_tmp45A,_tmp626.f2=ropt;_tmp626;}));});
# 2337
struct _tuple16 _tmp44B=_tmp44A;union Cyc_CfFlowInfo_FlowInfo _tmp44D;void*_tmp44C;_LL13: _tmp44D=_tmp44B.f1;_tmp44C=_tmp44B.f2;_LL14:;
# 2341
_tmp45A=_tmp44D;new_rval=_tmp44C;}else{
# 2346
new_rval=_tmp448;}}else{
# 2349
new_rval=_tmp456;}
# 2351
{union Cyc_CfFlowInfo_FlowInfo _tmp44E=_tmp45A;struct Cyc_Dict_Dict _tmp452;struct Cyc_List_List*_tmp451;if((_tmp44E.ReachableFL).tag == 2){_LL16: _tmp452=((_tmp44E.ReachableFL).val).f1;_tmp451=((_tmp44E.ReachableFL).val).f2;_LL17:
# 2353
({union Cyc_CfFlowInfo_FlowInfo _tmp79C=({struct Cyc_Dict_Dict _tmp79B=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp452,_tmp458,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp79B,_tmp451);});flow=_tmp79C;});
# 2357
goto _LL15;}else{_LL18: _LL19:
# 2359
({void*_tmp44F=0U;({struct _dyneither_ptr _tmp79D=({const char*_tmp450="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp450,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp79D,_tag_dyneither(_tmp44F,sizeof(void*),0U));});});}_LL15:;}
# 2361
goto _LLD;}else{
# 2363
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2370
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp455=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,({
struct _dyneither_ptr _tmp79E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp627.f1=_tmp79E;});_tmp627;});void*_tmp453[1U];_tmp453[0]=& _tmp455;({unsigned int _tmp7A0=loc;struct _dyneither_ptr _tmp79F=({const char*_tmp454="aliased expression %s was overwritten";_tag_dyneither(_tmp454,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7A0,_tmp79F,_tag_dyneither(_tmp453,sizeof(void*),1U));});});
# 2387 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2389
goto _LL8;}}else{_LLB: _LLC:
# 2395
 goto _LL8;}_LL8:;}
# 2397
goto _LL0;}}_LL0:;}
# 2399
return flow;}
# 2412 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2418
{union Cyc_CfFlowInfo_FlowInfo _tmp45C=flow;if((_tmp45C.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2422
int _tmp45D=unconsume_exps != 0;
struct Cyc_List_List*_tmp45E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp460=_tmp45E;for(0;_tmp460 != 0;(
_tmp460=_tmp460->tl,
_tmp45F != 0?_tmp45F=_tmp45F->tl: 0)){
# 2431
struct Cyc_Absyn_Vardecl*_tmp461=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp460->hd);
struct _tuple28 _tmp462=
is_region_open?({struct _tuple28 _tmp628;_tmp628.f1=1,_tmp628.f2=0;_tmp628;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp460->hd,_tmp45D,_tmp461,flow,loc);
# 2432
struct _tuple28 _tmp463=_tmp462;int _tmp465;void*_tmp464;_LL6: _tmp465=_tmp463.f1;_tmp464=_tmp463.f2;_LL7:;
# 2438
if(_tmp45D)
({union Cyc_CfFlowInfo_FlowInfo _tmp7A1=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45F))->hd,_tmp461,_tmp465,_tmp464,flow,loc);flow=_tmp7A1;});}}
# 2441
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2445
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2447
struct Cyc_CfFlowInfo_FlowEnv*_tmp466=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp467=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp468=_tmp467;struct Cyc_Core_Opt*_tmp47C;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Stmt*_tmp47A;unsigned int _tmp479;_LL1: _tmp47C=_tmp468->pat_vars;_tmp47B=_tmp468->where_clause;_tmp47A=_tmp468->body;_tmp479=_tmp468->loc;_LL2:;{
struct _tuple1 _tmp469=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47C))->v);struct _tuple1 _tmp46A=_tmp469;struct Cyc_List_List*_tmp478;struct Cyc_List_List*_tmp477;_LL4: _tmp478=_tmp46A.f1;_tmp477=_tmp46A.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp47C->v,_tmp478 != 0,_tmp479,exp_loc);
# 2455
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp46B=env->unique_pat_vars;
# 2458
if(Cyc_Tcpat_has_vars(_tmp47C))
({struct Cyc_List_List*_tmp7A3=({struct Cyc_List_List*_tmp46D=_cycalloc(sizeof(*_tmp46D));
({struct _tuple29*_tmp7A2=({struct _tuple29*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->f1=0,_tmp46C->f2=_tmp47A,_tmp46C->f3=_tmp478,_tmp46C->f4=_tmp477;_tmp46C;});_tmp46D->hd=_tmp7A2;}),_tmp46D->tl=_tmp46B;_tmp46D;});
# 2459
env->unique_pat_vars=_tmp7A3;});
# 2462
if(_tmp47B != 0){
struct Cyc_Absyn_Exp*wexp=_tmp47B;
struct _tuple19 _tmp46E=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp46F=_tmp46E;union Cyc_CfFlowInfo_FlowInfo _tmp471;union Cyc_CfFlowInfo_FlowInfo _tmp470;_LL7: _tmp471=_tmp46F.f1;_tmp470=_tmp46F.f2;_LL8:;
inflow=_tmp470;
({union Cyc_CfFlowInfo_FlowInfo _tmp7A4=Cyc_NewControlFlow_anal_stmt(env,_tmp471,_tmp47A,0);clause_outflow=_tmp7A4;});}else{
# 2468
({union Cyc_CfFlowInfo_FlowInfo _tmp7A5=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp47A,0);clause_outflow=_tmp7A5;});}
# 2470
env->unique_pat_vars=_tmp46B;{
union Cyc_CfFlowInfo_FlowInfo _tmp472=clause_outflow;if((_tmp472.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2475
({union Cyc_CfFlowInfo_FlowInfo _tmp7A6=Cyc_NewControlFlow_unconsume_params(env,_tmp478,_tmp477,0,clause_outflow,_tmp479);clause_outflow=_tmp7A6;});
# 2477
if(scs->tl == 0)
return clause_outflow;else{
# 2482
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp473=0U;({unsigned int _tmp7A8=_tmp47A->loc;struct _dyneither_ptr _tmp7A7=({const char*_tmp474="switch clause may implicitly fallthru";_tag_dyneither(_tmp474,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7A8,_tmp7A7,_tag_dyneither(_tmp473,sizeof(void*),0U));});});else{
# 2485
({void*_tmp475=0U;({unsigned int _tmp7AA=_tmp47A->loc;struct _dyneither_ptr _tmp7A9=({const char*_tmp476="switch clause may implicitly fallthru";_tag_dyneither(_tmp476,sizeof(char),38U);});Cyc_Warn_warn(_tmp7AA,_tmp7A9,_tag_dyneither(_tmp475,sizeof(void*),0U));});});}
# 2487
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2489
goto _LL9;}_LL9:;};};};}
# 2492
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2497
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2499
struct _tuple30*_tmp47D=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp488;struct Cyc_Dict_Dict _tmp487;unsigned int _tmp486;_LL1: _tmp488=_tmp47D->f1;_tmp487=_tmp47D->f2;_tmp486=_tmp47D->f3;_LL2:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2502
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp62A;_tmp62A.tag=0U,_tmp62A.f1=vd;_tmp62A;});
struct _tuple15 _tmp47E=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp7AB=_tmp487;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp7AB,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp485=_cycalloc(sizeof(*_tmp485));*_tmp485=vdroot;_tmp485;}));}));struct _tuple15 _tmp47F=_tmp47E;void*_tmp484;_LL4: _tmp484=_tmp47F.f1;_LL5:;{
void*_tmp480=_tmp484;switch(*((int*)_tmp480)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp480)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2509
({struct Cyc_String_pa_PrintArg_struct _tmp483=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,({struct _dyneither_ptr _tmp7AC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp629.f1=_tmp7AC;});_tmp629;});void*_tmp481[1U];_tmp481[0]=& _tmp483;({unsigned int _tmp7AE=_tmp486;struct _dyneither_ptr _tmp7AD=({const char*_tmp482="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp482,sizeof(char),68U);});Cyc_Warn_warn(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp481,sizeof(void*),1U));});});
# 2511
goto _LL6;}_LL6:;};}}
# 2516
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp489=inflow;struct Cyc_Dict_Dict _tmp495;if((_tmp489.ReachableFL).tag == 2){_LL1: _tmp495=((_tmp489.ReachableFL).val).f1;_LL2: {
# 2519
struct _tuple30 _tmp48A=({struct _tuple30 _tmp62B;_tmp62B.f1=env,_tmp62B.f2=_tmp495,_tmp62B.f3=decl->loc;_tmp62B;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp48B=env->fenv;
{void*_tmp48C=decl->r;void*_tmp48D=_tmp48C;struct Cyc_List_List*_tmp494;struct Cyc_List_List*_tmp493;struct Cyc_Absyn_Vardecl*_tmp492;switch(*((int*)_tmp48D)){case 0U: _LL6: _tmp492=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp48D)->f1;_LL7:
# 2523
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp48A,_tmp492);
goto _LL5;case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48D)->f2 != 0){_LL8: _tmp493=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48D)->f2)->v;_LL9: {
# 2526
struct _tuple1 _tmp48E=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp493);struct _tuple1 _tmp48F=_tmp48E;struct Cyc_List_List*_tmp491;_LLF: _tmp491=_tmp48F.f1;_LL10:;{
struct Cyc_List_List*_tmp490=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp491);
_tmp494=_tmp490;goto _LLB;};}}else{goto _LLC;}case 3U: _LLA: _tmp494=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp48D)->f1;_LLB:
# 2530
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp48A,_tmp494);
goto _LL5;default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 2534
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2537
return;}
# 2543
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2547
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp496=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp497=_tmp496;int _tmp49B;struct Cyc_Absyn_Stmt*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_List_List*_tmp498;_LL1: _tmp49B=_tmp497->f1;_tmp49A=_tmp497->f2;_tmp499=_tmp497->f3;_tmp498=_tmp497->f4;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7AF=Cyc_NewControlFlow_unconsume_params(env,_tmp499,_tmp498,_tmp49B,inflow,dest->loc);inflow=_tmp7AF;});}}
# 2552
return inflow;}
# 2558
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2560
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp49C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp49D=_tmp49C;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp539;union Cyc_CfFlowInfo_FlowInfo*_tmp538;_LL1: _tmp539=_tmp49D.f1;_tmp538=_tmp49D.f2;_LL2:;
inflow=*_tmp538;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp49E=env->fenv;
# 2567
void*_tmp49F=s->r;void*_tmp4A0=_tmp49F;struct Cyc_Absyn_Stmt*_tmp537;struct Cyc_Absyn_Decl*_tmp536;struct Cyc_Absyn_Stmt*_tmp535;struct Cyc_Absyn_Vardecl*_tmp534;struct Cyc_Absyn_Exp*_tmp533;unsigned int _tmp532;struct Cyc_Absyn_Stmt*_tmp531;struct Cyc_List_List*_tmp530;struct Cyc_Absyn_Exp*_tmp52F;unsigned int _tmp52E;struct Cyc_Absyn_Stmt*_tmp52D;struct Cyc_Absyn_Stmt*_tmp52C;struct Cyc_List_List*_tmp52B;void*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_List_List*_tmp528;void*_tmp527;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_Switch_clause*_tmp525;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Stmt*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Stmt*_tmp520;struct Cyc_Absyn_Stmt*_tmp51F;struct Cyc_Absyn_Stmt*_tmp51E;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Stmt*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Stmt*_tmp51A;struct Cyc_Absyn_Stmt*_tmp519;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Stmt*_tmp517;struct Cyc_Absyn_Stmt*_tmp516;struct Cyc_Absyn_Stmt*_tmp515;struct Cyc_Absyn_Stmt*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp512;switch(*((int*)_tmp4A0)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1 == 0){_LL6: _LL7:
# 2571
 if(env->noreturn)
({void*_tmp4A1=0U;({unsigned int _tmp7B1=s->loc;struct _dyneither_ptr _tmp7B0=({const char*_tmp4A2="`noreturn' function might return";_tag_dyneither(_tmp4A2,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7B1,_tmp7B0,_tag_dyneither(_tmp4A1,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp512=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_LL9:
# 2577
 if(env->noreturn)
({void*_tmp4A3=0U;({unsigned int _tmp7B3=s->loc;struct _dyneither_ptr _tmp7B2=({const char*_tmp4A4="`noreturn' function might return";_tag_dyneither(_tmp4A4,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7B3,_tmp7B2,_tag_dyneither(_tmp4A3,sizeof(void*),0U));});});{
struct _tuple16 _tmp4A5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp512));struct _tuple16 _tmp4A6=_tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A8;void*_tmp4A7;_LL2D: _tmp4A8=_tmp4A6.f1;_tmp4A7=_tmp4A6.f2;_LL2E:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7B4=Cyc_NewControlFlow_use_Rval(env,_tmp512->loc,_tmp4A8,_tmp4A7);_tmp4A8=_tmp7B4;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp4A8);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp4A8,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1U: _LLA: _tmp513=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_LLB: {
# 2586
struct _tuple17*_tmp4A9=rval_opt;void**_tmp4AF;int _tmp4AE;if(_tmp4A9 != 0){_LL30: _tmp4AF=(void**)& _tmp4A9->f1;_tmp4AE=_tmp4A9->f2;_LL31: {
# 2588
struct _tuple16 _tmp4AA=Cyc_NewControlFlow_anal_Rexp(env,_tmp4AE,inflow,_tmp513);struct _tuple16 _tmp4AB=_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;void*_tmp4AC;_LL35: _tmp4AD=_tmp4AB.f1;_tmp4AC=_tmp4AB.f2;_LL36:;
*_tmp4AF=_tmp4AC;
return _tmp4AD;}}else{_LL32: _LL33:
# 2592
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp513)).f1;}_LL2F:;}case 2U: _LLC: _tmp515=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp514=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LLD:
# 2596
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp7B7=env;union Cyc_CfFlowInfo_FlowInfo _tmp7B6=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp515,0);struct Cyc_Absyn_Stmt*_tmp7B5=_tmp514;Cyc_NewControlFlow_anal_stmt(_tmp7B7,_tmp7B6,_tmp7B5,rval_opt);});case 4U: _LLE: _tmp518=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp517=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_tmp516=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3;_LLF: {
# 2599
struct _tuple19 _tmp4B0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp518);struct _tuple19 _tmp4B1=_tmp4B0;union Cyc_CfFlowInfo_FlowInfo _tmp4B5;union Cyc_CfFlowInfo_FlowInfo _tmp4B4;_LL38: _tmp4B5=_tmp4B1.f1;_tmp4B4=_tmp4B1.f2;_LL39:;{
# 2606
union Cyc_CfFlowInfo_FlowInfo _tmp4B2=Cyc_NewControlFlow_anal_stmt(env,_tmp4B4,_tmp516,0);
union Cyc_CfFlowInfo_FlowInfo _tmp4B3=Cyc_NewControlFlow_anal_stmt(env,_tmp4B5,_tmp517,0);
return Cyc_CfFlowInfo_join_flow(_tmp49E,_tmp4B3,_tmp4B2);};}case 5U: _LL10: _tmp51B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1).f1;_tmp51A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1).f2;_tmp519=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL11: {
# 2614
struct _tuple20 _tmp4B6=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp51A);struct _tuple20 _tmp4B7=_tmp4B6;union Cyc_CfFlowInfo_FlowInfo*_tmp4BE;_LL3B: _tmp4BE=_tmp4B7.f2;_LL3C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B8=*_tmp4BE;
struct _tuple19 _tmp4B9=Cyc_NewControlFlow_anal_test(env,_tmp4B8,_tmp51B);struct _tuple19 _tmp4BA=_tmp4B9;union Cyc_CfFlowInfo_FlowInfo _tmp4BD;union Cyc_CfFlowInfo_FlowInfo _tmp4BC;_LL3E: _tmp4BD=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;_LL3F:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BB=Cyc_NewControlFlow_anal_stmt(env,_tmp4BD,_tmp519,0);
Cyc_NewControlFlow_update_flow(env,_tmp51A,_tmp4BB);
return _tmp4BC;};};}case 14U: _LL12: _tmp51E=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp51D=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f1;_tmp51C=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f2;_LL13: {
# 2624
union Cyc_CfFlowInfo_FlowInfo _tmp4BF=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp51E,0);
struct _tuple20 _tmp4C0=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4BF,_tmp51C);struct _tuple20 _tmp4C1=_tmp4C0;union Cyc_CfFlowInfo_FlowInfo*_tmp4C7;_LL41: _tmp4C7=_tmp4C1.f2;_LL42:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C2=*_tmp4C7;
struct _tuple19 _tmp4C3=Cyc_NewControlFlow_anal_test(env,_tmp4C2,_tmp51D);struct _tuple19 _tmp4C4=_tmp4C3;union Cyc_CfFlowInfo_FlowInfo _tmp4C6;union Cyc_CfFlowInfo_FlowInfo _tmp4C5;_LL44: _tmp4C6=_tmp4C4.f1;_tmp4C5=_tmp4C4.f2;_LL45:;
Cyc_NewControlFlow_update_flow(env,_tmp51E,_tmp4C6);
return _tmp4C5;};}case 9U: _LL14: _tmp524=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp523=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f1;_tmp522=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f2;_tmp521=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3).f1;_tmp520=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3).f2;_tmp51F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f4;_LL15: {
# 2633
union Cyc_CfFlowInfo_FlowInfo _tmp4C8=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp524)).f1;
struct _tuple20 _tmp4C9=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4C8,_tmp522);struct _tuple20 _tmp4CA=_tmp4C9;union Cyc_CfFlowInfo_FlowInfo*_tmp4D6;_LL47: _tmp4D6=_tmp4CA.f2;_LL48:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4CB=*_tmp4D6;
struct _tuple19 _tmp4CC=Cyc_NewControlFlow_anal_test(env,_tmp4CB,_tmp523);struct _tuple19 _tmp4CD=_tmp4CC;union Cyc_CfFlowInfo_FlowInfo _tmp4D5;union Cyc_CfFlowInfo_FlowInfo _tmp4D4;_LL4A: _tmp4D5=_tmp4CD.f1;_tmp4D4=_tmp4CD.f2;_LL4B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4CE=Cyc_NewControlFlow_anal_stmt(env,_tmp4D5,_tmp51F,0);
struct _tuple20 _tmp4CF=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4CE,_tmp520);struct _tuple20 _tmp4D0=_tmp4CF;union Cyc_CfFlowInfo_FlowInfo*_tmp4D3;_LL4D: _tmp4D3=_tmp4D0.f2;_LL4E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D1=*_tmp4D3;
union Cyc_CfFlowInfo_FlowInfo _tmp4D2=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp4D1,_tmp521)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp522,_tmp4D2);
return _tmp4D4;};};};}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2 != 0){_LL16: _tmp526=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp525=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL17: {
# 2645
struct _tuple23 _tmp4D7=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp526,1,1);struct _tuple23 _tmp4D8=_tmp4D7;union Cyc_CfFlowInfo_FlowInfo _tmp4DF;struct Cyc_List_List*_tmp4DE;_LL50: _tmp4DF=_tmp4D8.f1;_tmp4DE=_tmp4D8.f2;_LL51:;
# 2647
({union Cyc_CfFlowInfo_FlowInfo _tmp7B8=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp525->body);inflow=_tmp7B8;});{
# 2649
struct Cyc_List_List*_tmp4D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp525->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp7B9=Cyc_NewControlFlow_add_vars(_tmp49E,_tmp4DF,_tmp4D9,_tmp49E->unknown_all,s->loc,0);_tmp4DF=_tmp7B9;});
# 2652
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp525->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4DA=(struct _tuple24*)x->hd;struct _tuple24*_tmp4DB=_tmp4DA;struct Cyc_Absyn_Vardecl**_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;_LL53: _tmp4DD=_tmp4DB->f1;_tmp4DC=_tmp4DB->f2;_LL54:;
if(_tmp4DD != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp7C0=({struct Cyc_CfFlowInfo_FlowEnv*_tmp7BF=_tmp49E;struct Cyc_NewControlFlow_AnalEnv*_tmp7BE=env;union Cyc_CfFlowInfo_FlowInfo _tmp7BD=_tmp4DF;struct Cyc_Absyn_Vardecl*_tmp7BC=*_tmp4DD;struct Cyc_Absyn_Exp*_tmp7BB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp526))->hd;void*_tmp7BA=(void*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp7BF,_tmp7BE,_tmp7BD,_tmp7BC,_tmp7BB,_tmp7BA,s->loc);});_tmp4DF=_tmp7C0;});
_tmp4DE=_tmp4DE->tl;
_tmp526=_tmp526->tl;}}}
# 2660
Cyc_NewControlFlow_update_flow(env,_tmp525->body,_tmp4DF);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL2A: _LL2B:
# 2827
({void*_tmp510=0U;({struct _dyneither_ptr _tmp7C1=({const char*_tmp511="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp511,sizeof(char),54U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7C1,_tag_dyneither(_tmp510,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2666
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp4E0=inflow;if((_tmp4E0.ReachableFL).tag == 2){_LL56: _LL57:
# 2669
{void*_tmp4E1=Cyc_Tcutil_compress(env->return_type);void*_tmp4E2=_tmp4E1;switch(*((int*)_tmp4E2)){case 0U: _LL5B: _LL5C:
 goto _LL5A;case 7U: _LL5D: _LL5E:
 goto _LL60;case 6U: _LL5F: _LL60:
# 2673
({void*_tmp4E3=0U;({unsigned int _tmp7C3=s->loc;struct _dyneither_ptr _tmp7C2=({const char*_tmp4E4="break may cause function not to return a value";_tag_dyneither(_tmp4E4,sizeof(char),47U);});Cyc_Warn_warn(_tmp7C3,_tmp7C2,_tag_dyneither(_tmp4E3,sizeof(void*),0U));});});
goto _LL5A;default: _LL61: _LL62:
# 2676
({void*_tmp4E5=0U;({unsigned int _tmp7C5=s->loc;struct _dyneither_ptr _tmp7C4=({const char*_tmp4E6="break may cause function not to return a value";_tag_dyneither(_tmp4E6,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp7C5,_tmp7C4,_tag_dyneither(_tmp4E5,sizeof(void*),0U));});});
goto _LL5A;}_LL5A:;}
# 2679
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2682
if(env->noreturn)
({void*_tmp4E7=0U;({unsigned int _tmp7C7=s->loc;struct _dyneither_ptr _tmp7C6=({const char*_tmp4E8="`noreturn' function might return";_tag_dyneither(_tmp4E8,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7C7,_tmp7C6,_tag_dyneither(_tmp4E7,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2688
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2692
struct Cyc_Absyn_Stmt*_tmp4E9=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp4E9 == 0)
({void*_tmp4EA=0U;({unsigned int _tmp7C9=s->loc;struct _dyneither_ptr _tmp7C8=({const char*_tmp4EB="jump has no target (should be impossible)";_tag_dyneither(_tmp4EB,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp7C9,_tmp7C8,_tag_dyneither(_tmp4EA,sizeof(void*),0U));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp7CA=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp4E9));inflow=_tmp7CA;});
# 2697
Cyc_NewControlFlow_update_flow(env,_tmp4E9,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp529=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp528=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_tmp527=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3;_LL1F:
# 2703
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp528,_tmp529->loc);case 15U: _LL20: _tmp52C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp52B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_tmp52A=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3;_LL21: {
# 2708
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52C,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2716
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2719
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp52B,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp4EC=scs_outflow;if((_tmp4EC.BottomFL).tag == 1){_LL64: _LL65:
 goto _LL63;}else{_LL66: _LL67:
({void*_tmp4ED=0U;({unsigned int _tmp7CC=s->loc;struct _dyneither_ptr _tmp7CB=({const char*_tmp4EE="last catch clause may implicitly fallthru";_tag_dyneither(_tmp4EE,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp7CC,_tmp7CB,_tag_dyneither(_tmp4ED,sizeof(void*),0U));});});}_LL63:;}
# 2725
outflow=s1_outflow;
# 2727
return outflow;};};}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f2 != 0){_LL22: _tmp530=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f2)->v;_tmp52F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f3;_tmp52E=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->loc;_tmp52D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL23: {
# 2737
struct _tuple1 _tmp4EF=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp530);struct _tuple1 _tmp4F0=_tmp4EF;struct Cyc_List_List*_tmp4F5;struct Cyc_List_List*_tmp4F4;_LL69: _tmp4F5=_tmp4F0.f1;_tmp4F4=_tmp4F0.f2;_LL6A:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7CD=Cyc_NewControlFlow_initialize_pat_vars(_tmp49E,env,inflow,_tmp530,_tmp4F5 != 0,_tmp52E,_tmp52F->loc);inflow=_tmp7CD;});{
struct Cyc_List_List*_tmp4F1=env->unique_pat_vars;
# 2741
({struct Cyc_List_List*_tmp7CF=({struct Cyc_List_List*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));
({struct _tuple29*_tmp7CE=({struct _tuple29*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->f1=0,_tmp4F2->f2=s,_tmp4F2->f3=_tmp4F5,_tmp4F2->f4=_tmp4F4;_tmp4F2;});_tmp4F3->hd=_tmp7CE;}),_tmp4F3->tl=_tmp4F1;_tmp4F3;});
# 2741
env->unique_pat_vars=_tmp7CF;});
# 2746
({union Cyc_CfFlowInfo_FlowInfo _tmp7D0=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52D,rval_opt);inflow=_tmp7D0;});
env->unique_pat_vars=_tmp4F1;
# 2750
({union Cyc_CfFlowInfo_FlowInfo _tmp7D1=Cyc_NewControlFlow_unconsume_params(env,_tmp4F5,_tmp4F4,0,inflow,_tmp52E);inflow=_tmp7D1;});
# 2754
return inflow;};}}else{goto _LL26;}case 4U: _LL24: _tmp534=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f2;_tmp533=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f3;_tmp532=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->loc;_tmp531=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;if(_tmp533 != 0){_LL25: {
# 2766
struct Cyc_List_List l=({struct Cyc_List_List _tmp62D;_tmp62D.hd=_tmp533,_tmp62D.tl=0;_tmp62D;});
union Cyc_CfFlowInfo_FlowInfo _tmp4F6=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp4F7=Cyc_NewControlFlow_anal_Lexp(env,_tmp4F6,0,0,_tmp533);struct _tuple18 _tmp4F8=_tmp4F7;union Cyc_CfFlowInfo_AbsLVal _tmp50F;_LL6C: _tmp50F=_tmp4F8.f2;_LL6D:;{
struct _tuple16 _tmp4F9=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp4F6,_tmp533);struct _tuple16 _tmp4FA=_tmp4F9;union Cyc_CfFlowInfo_FlowInfo _tmp50E;_LL6F: _tmp50E=_tmp4FA.f1;_LL70:;{
struct Cyc_List_List*_tmp4FB=0;
struct Cyc_List_List*_tmp4FC=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp4FD=_tmp50E;struct Cyc_Dict_Dict _tmp509;struct Cyc_List_List*_tmp508;if((_tmp4FD.ReachableFL).tag == 2){_LL72: _tmp509=((_tmp4FD.ReachableFL).val).f1;_tmp508=((_tmp4FD.ReachableFL).val).f2;_LL73:
# 2774
{union Cyc_CfFlowInfo_AbsLVal _tmp4FE=_tmp50F;struct Cyc_CfFlowInfo_Place*_tmp507;if((_tmp4FE.PlaceL).tag == 1){_LL77: _tmp507=(_tmp4FE.PlaceL).val;_LL78: {
# 2778
void*_tmp4FF=Cyc_CfFlowInfo_lookup_place(_tmp509,_tmp507);
({void*_tmp7D2=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->tag=8U,_tmp500->f1=_tmp534,_tmp500->f2=_tmp4FF;_tmp500;});_tmp4FF=_tmp7D2;});
({struct Cyc_Dict_Dict _tmp7D3=Cyc_CfFlowInfo_assign_place(_tmp49E,_tmp533->loc,_tmp509,_tmp507,_tmp4FF);_tmp509=_tmp7D3;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7D4=Cyc_CfFlowInfo_ReachableFL(_tmp509,_tmp508);_tmp50E=_tmp7D4;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp501=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->tag=0U,_tmp506->f1=_tmp534;_tmp506;});
struct Cyc_CfFlowInfo_Place*_tmp502=({struct Cyc_CfFlowInfo_Place*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->root=(void*)_tmp501,_tmp505->path=0;_tmp505;});
({struct Cyc_List_List*_tmp7D5=({struct Cyc_List_List*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->hd=_tmp502,_tmp503->tl=_tmp4FB;_tmp503;});_tmp4FB=_tmp7D5;});
({struct Cyc_List_List*_tmp7D6=({struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=_tmp533,_tmp504->tl=_tmp4FC;_tmp504;});_tmp4FC=_tmp7D6;});
goto _LL76;};}}else{_LL79: _LL7A:
# 2793
 goto _LL76;}_LL76:;}
# 2795
goto _LL71;}else{_LL74: _LL75:
# 2797
 goto _LL71;}_LL71:;}{
# 2800
struct Cyc_List_List _tmp50A=({struct Cyc_List_List _tmp62C;_tmp62C.hd=_tmp534,_tmp62C.tl=0;_tmp62C;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7D7=Cyc_NewControlFlow_add_vars(_tmp49E,_tmp50E,& _tmp50A,_tmp49E->unknown_all,_tmp532,0);_tmp50E=_tmp7D7;});{
# 2804
struct Cyc_List_List*_tmp50B=env->unique_pat_vars;
({struct Cyc_List_List*_tmp7D9=({struct Cyc_List_List*_tmp50D=_cycalloc(sizeof(*_tmp50D));({struct _tuple29*_tmp7D8=({struct _tuple29*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->f1=1,_tmp50C->f2=s,_tmp50C->f3=_tmp4FB,_tmp50C->f4=_tmp4FC;_tmp50C;});_tmp50D->hd=_tmp7D8;}),_tmp50D->tl=_tmp50B;_tmp50D;});env->unique_pat_vars=_tmp7D9;});
# 2809
({union Cyc_CfFlowInfo_FlowInfo _tmp7DA=Cyc_NewControlFlow_anal_stmt(env,_tmp50E,_tmp531,rval_opt);_tmp50E=_tmp7DA;});
env->unique_pat_vars=_tmp50B;
# 2813
({union Cyc_CfFlowInfo_FlowInfo _tmp7DB=Cyc_NewControlFlow_unconsume_params(env,_tmp4FB,_tmp4FC,1,_tmp50E,_tmp532);_tmp50E=_tmp7DB;});
# 2817
return _tmp50E;};};};};}}else{goto _LL26;}default: _LL26: _tmp536=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp535=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL27:
# 2820
({union Cyc_CfFlowInfo_FlowInfo _tmp7DF=({struct Cyc_NewControlFlow_AnalEnv*_tmp7DE=env;union Cyc_CfFlowInfo_FlowInfo _tmp7DD=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp536);struct Cyc_Absyn_Stmt*_tmp7DC=_tmp535;Cyc_NewControlFlow_anal_stmt(_tmp7DE,_tmp7DD,_tmp7DC,rval_opt);});outflow=_tmp7DF;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp536);
return outflow;}default: _LL28: _tmp537=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL29:
# 2825
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp537,rval_opt);}_LL3:;};}
# 2831
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2835
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp53A=env->fenv;
void*_tmp53B=decl->r;void*_tmp53C=_tmp53B;struct Cyc_Absyn_Tvar*_tmp54F;struct Cyc_Absyn_Vardecl*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Fndecl*_tmp54C;struct Cyc_List_List*_tmp54B;struct Cyc_Absyn_Vardecl*_tmp54A;switch(*((int*)_tmp53C)){case 0U: _LL1: _tmp54A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp53C)->f1;_LL2: {
# 2845
struct Cyc_List_List _tmp53D=({struct Cyc_List_List _tmp62E;_tmp62E.hd=_tmp54A,_tmp62E.tl=0;_tmp62E;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7E0=Cyc_NewControlFlow_add_vars(_tmp53A,inflow,& _tmp53D,_tmp53A->unknown_none,decl->loc,0);inflow=_tmp7E0;});{
struct Cyc_Absyn_Exp*_tmp53E=_tmp54A->initializer;
if(_tmp53E == 0)
return inflow;{
struct _tuple16 _tmp53F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp53E);struct _tuple16 _tmp540=_tmp53F;union Cyc_CfFlowInfo_FlowInfo _tmp542;void*_tmp541;_LLC: _tmp542=_tmp540.f1;_tmp541=_tmp540.f2;_LLD:;
return Cyc_NewControlFlow_do_initialize_var(_tmp53A,env,_tmp542,_tmp54A,_tmp53E,_tmp541,decl->loc);};};}case 3U: _LL3: _tmp54B=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp53C)->f1;_LL4:
# 2854
 return Cyc_NewControlFlow_add_vars(_tmp53A,inflow,_tmp54B,_tmp53A->unknown_none,decl->loc,0);case 1U: _LL5: _tmp54C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp53C)->f1;_LL6:
# 2857
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp53A,inflow,_tmp54C);{
void*t=(void*)_check_null(_tmp54C->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp543=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp54C->fn_vardecl);
# 2863
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp7E4=_tmp53A;union Cyc_CfFlowInfo_FlowInfo _tmp7E3=inflow;struct Cyc_List_List*_tmp7E2=({struct Cyc_List_List*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->hd=_tmp543,_tmp544->tl=0;_tmp544;});void*_tmp7E1=_tmp53A->unknown_all;Cyc_NewControlFlow_add_vars(_tmp7E4,_tmp7E3,_tmp7E2,_tmp7E1,decl->loc,0);});};case 4U: _LL7: _tmp54F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53C)->f1;_tmp54E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53C)->f2;_tmp54D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53C)->f3;_LL8:
# 2866
 if(_tmp54D != 0)
({void*_tmp545=0U;({struct _dyneither_ptr _tmp7E5=({const char*_tmp546="found open expression in declaration!";_tag_dyneither(_tmp546,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7E5,_tag_dyneither(_tmp545,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp547=({struct Cyc_List_List _tmp62F;_tmp62F.hd=_tmp54E,_tmp62F.tl=0;_tmp62F;});
return Cyc_NewControlFlow_add_vars(_tmp53A,inflow,& _tmp547,_tmp53A->unknown_all,decl->loc,0);};default: _LL9: _LLA:
# 2872
({void*_tmp548=0U;({struct _dyneither_ptr _tmp7E6=({const char*_tmp549="anal_decl: unexpected decl variant";_tag_dyneither(_tmp549,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7E6,_tag_dyneither(_tmp548,sizeof(void*),0U));});});}_LL0:;}
# 2880
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp550;_push_handler(& _tmp550);{int _tmp552=0;if(setjmp(_tmp550.handler))_tmp552=1;if(!_tmp552){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp7E9=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp7E8=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp7E7=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp7E9,_tmp7E8,_tmp7E7,fd);});}
# 2882
;_pop_handler();}else{void*_tmp551=(void*)_exn_thrown;void*_tmp553=_tmp551;void*_tmp555;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp553)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2888
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->tag=Cyc_Dict_Absent;_tmp554;}));}}else{_LL3: _tmp555=_tmp553;_LL4:(int)_rethrow(_tmp555);}_LL0:;}};}
# 2894
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2899
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp556=r;unsigned int _tmp558;if((_tmp556.RParam).tag == 5){_LL1: _tmp558=(_tmp556.RParam).val;_LL2: {
# 2902
struct Cyc_Absyn_Vardecl*_tmp557=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp558);
if(!_tmp557->escapes)
return Cyc_Relations_RVar(_tmp557);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2910
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2914
unsigned int _tmp559=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp7EA=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp559,1);inflow=_tmp7EA;});{
# 2919
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp55A=({union Cyc_CfFlowInfo_FlowInfo _tmp58B=inflow;if((_tmp58B.ReachableFL).tag != 2)_throw_match();(_tmp58B.ReachableFL).val;});struct _tuple14 _tmp55B=_tmp55A;struct Cyc_Dict_Dict _tmp58A;struct Cyc_List_List*_tmp589;_LL1: _tmp58A=_tmp55B.f1;_tmp589=_tmp55B.f2;_LL2:;{
# 2924
struct Cyc_List_List*_tmp55C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp55D=fd->requires_relns;for(0;_tmp55D != 0;_tmp55D=_tmp55D->tl){
struct Cyc_Relations_Reln*_tmp55E=(struct Cyc_Relations_Reln*)_tmp55D->hd;
({struct Cyc_List_List*_tmp7EF=({struct _RegionHandle*_tmp7EE=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp7ED=Cyc_NewControlFlow_translate_rop(_tmp55C,_tmp55E->rop1);enum Cyc_Relations_Relation _tmp7EC=_tmp55E->relation;union Cyc_Relations_RelnOp _tmp7EB=
Cyc_NewControlFlow_translate_rop(_tmp55C,_tmp55E->rop2);
# 2927
Cyc_Relations_add_relation(_tmp7EE,_tmp7ED,_tmp7EC,_tmp7EB,_tmp589);});_tmp589=_tmp7EF;});}}{
# 2933
struct Cyc_List_List*atts;
{void*_tmp55F=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp560=_tmp55F;struct Cyc_List_List*_tmp563;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->tag == 9U){_LL4: _tmp563=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).attributes;_LL5:
 atts=_tmp563;goto _LL3;}else{_LL6: _LL7:
({void*_tmp561=0U;({struct _dyneither_ptr _tmp7F0=({const char*_tmp562="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp562,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F0,_tag_dyneither(_tmp561,sizeof(void*),0U));});});}_LL3:;}
# 2940
for(0;atts != 0;atts=atts->tl){
void*_tmp564=(void*)atts->hd;void*_tmp565=_tmp564;int _tmp57E;int _tmp57D;int _tmp57C;switch(*((int*)_tmp565)){case 21U: _LL9: _tmp57C=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp565)->f1;_LLA: {
# 2943
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp566=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp57C - 1);
void*t=Cyc_Tcutil_compress(_tmp566->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp567=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7F3=fenv;void*_tmp7F2=elttype;struct Cyc_Absyn_Exp*_tmp7F1=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp7F3,_tmp7F2,_tmp7F1,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp568=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->tag=2U,_tmp56D->f1=_tmp57C,_tmp56D->f2=t;_tmp56D;});
struct Cyc_CfFlowInfo_Place*_tmp569=({struct Cyc_CfFlowInfo_Place*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->root=(void*)_tmp568,_tmp56C->path=0;_tmp56C;});
({struct Cyc_Dict_Dict _tmp7F4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp58A,(void*)_tmp568,_tmp567);_tmp58A=_tmp7F4;});
({struct Cyc_Dict_Dict _tmp7F7=({struct Cyc_Dict_Dict _tmp7F6=_tmp58A;void*_tmp7F5=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->tag=0U,_tmp56A->f1=_tmp566;_tmp56A;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7F6,_tmp7F5,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->tag=4U,_tmp56B->f1=_tmp569;_tmp56B;}));});_tmp58A=_tmp7F7;});
goto _LL8;}case 20U: _LLB: _tmp57D=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp565)->f1;_LLC: {
# 2956
struct Cyc_Absyn_Vardecl*_tmp56E=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp57D - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp56E->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp56F=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->tag=2U,_tmp575->f1=_tmp57D,_tmp575->f2=elttype;_tmp575;});
struct Cyc_CfFlowInfo_Place*_tmp570=({struct Cyc_CfFlowInfo_Place*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->root=(void*)_tmp56F,_tmp574->path=0;_tmp574;});
({struct Cyc_Dict_Dict _tmp7FA=({struct Cyc_Dict_Dict _tmp7F9=_tmp58A;void*_tmp7F8=(void*)_tmp56F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7F9,_tmp7F8,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp58A=_tmp7FA;});
({struct Cyc_Dict_Dict _tmp7FD=({struct Cyc_Dict_Dict _tmp7FC=_tmp58A;void*_tmp7FB=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->tag=0U,_tmp571->f1=_tmp56E;_tmp571;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7FC,_tmp7FB,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=4U,_tmp572->f1=_tmp570;_tmp572;}));});_tmp58A=_tmp7FD;});
({struct Cyc_List_List*_tmp7FE=({struct Cyc_List_List*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->hd=_tmp570,_tmp573->tl=param_roots;_tmp573;});param_roots=_tmp7FE;});
goto _LL8;}case 22U: _LLD: _tmp57E=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp565)->f1;_LLE: {
# 2965
struct Cyc_Absyn_Vardecl*_tmp576=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp57E - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp576->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp577=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=0U,_tmp57B->f1=_tmp576;_tmp57B;});
struct Cyc_CfFlowInfo_Place*_tmp578=({struct Cyc_CfFlowInfo_Place*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->root=(void*)_tmp577,_tmp57A->path=0;_tmp57A;});
({struct Cyc_List_List*_tmp7FF=({struct Cyc_List_List*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->hd=_tmp578,_tmp579->tl=noconsume_roots;_tmp579;});noconsume_roots=_tmp7FF;});}
# 2971
goto _LL8;}default: _LLF: _LL10:
 goto _LL8;}_LL8:;}
# 2974
({union Cyc_CfFlowInfo_FlowInfo _tmp800=Cyc_CfFlowInfo_ReachableFL(_tmp58A,_tmp589);inflow=_tmp800;});{
# 2976
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp588=_cycalloc(sizeof(*_tmp588));
_tmp588->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp802=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp588->succ_table=_tmp802;}),({
struct Cyc_Hashtable_Table*_tmp801=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp588->pat_pop_table=_tmp801;}),_tmp588->fenv=fenv,_tmp588->iterate_again=1,_tmp588->iteration_num=0,_tmp588->in_try=0,_tmp588->tryflow=inflow,_tmp588->noreturn=noreturn,_tmp588->return_type=fd->ret_type,_tmp588->unique_pat_vars=0,_tmp588->param_roots=param_roots,_tmp588->noconsume_params=noconsume_roots,_tmp588->flow_table=flow_table,_tmp588->return_relns=fd->ensures_relns;_tmp588;});
# 2986
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2992
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp803=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp803;});}{
# 2995
union Cyc_CfFlowInfo_FlowInfo _tmp57F=outflow;if((_tmp57F.BottomFL).tag == 1){_LL12: _LL13:
 goto _LL11;}else{_LL14: _LL15:
# 2998
 Cyc_NewControlFlow_check_init_params(_tmp559,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp559);
# 3002
if(noreturn)
({void*_tmp580=0U;({unsigned int _tmp805=_tmp559;struct _dyneither_ptr _tmp804=({const char*_tmp581="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp581,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp805,_tmp804,_tag_dyneither(_tmp580,sizeof(void*),0U));});});else{
# 3005
void*_tmp582=Cyc_Tcutil_compress(fd->ret_type);void*_tmp583=_tmp582;switch(*((int*)_tmp583)){case 0U: _LL17: _LL18:
 goto _LL16;case 7U: _LL19: _LL1A:
 goto _LL1C;case 6U: _LL1B: _LL1C:
# 3009
({void*_tmp584=0U;({unsigned int _tmp807=_tmp559;struct _dyneither_ptr _tmp806=({const char*_tmp585="function may not return a value";_tag_dyneither(_tmp585,sizeof(char),32U);});Cyc_Warn_warn(_tmp807,_tmp806,_tag_dyneither(_tmp584,sizeof(void*),0U));});});goto _LL16;default: _LL1D: _LL1E:
# 3011
({void*_tmp586=0U;({unsigned int _tmp809=_tmp559;struct _dyneither_ptr _tmp808=({const char*_tmp587="function may not return a value";_tag_dyneither(_tmp587,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp809,_tmp808,_tag_dyneither(_tmp586,sizeof(void*),0U));});});goto _LL16;}_LL16:;}
# 3013
goto _LL11;}_LL11:;};};};};};}
# 3017
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp58C=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp58D=_tmp58C;struct Cyc_List_List*_tmp591;struct Cyc_List_List*_tmp590;struct Cyc_List_List*_tmp58F;struct Cyc_Absyn_Fndecl*_tmp58E;switch(*((int*)_tmp58D)){case 1U: _LL1: _tmp58E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp58D)->f1;_LL2:
 Cyc_NewControlFlow_check_fun(tables,_tmp58E);goto _LL0;case 11U: _LL3: _tmp58F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp58D)->f1;_LL4:
 _tmp590=_tmp58F;goto _LL6;case 10U: _LL5: _tmp590=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp58D)->f2;_LL6:
 _tmp591=_tmp590;goto _LL8;case 9U: _LL7: _tmp591=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp58D)->f2;_LL8:
 Cyc_NewControlFlow_cf_check(tables,_tmp591);goto _LL0;case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;};}}

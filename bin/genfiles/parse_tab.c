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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 108 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 482
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 489
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 895
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 922
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 940
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 945
void*Cyc_Absyn_compress_kb(void*);
# 950
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 952
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 955
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 957
extern void*Cyc_Absyn_sint_typ;
# 959
void*Cyc_Absyn_float_typ(int);
# 1007
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1087
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1100
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1106
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1110
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1115
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1117
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1124
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1133
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1138
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1144
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1169
int Cyc_Absyn_fntype_att(void*a);
# 1188
extern int Cyc_Absyn_porting_c_code;struct Cyc_RgnOrder_RgnPO;
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
# 42 "tcutil.h"
void*Cyc_Tcutil_copy_type(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 111
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 115
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 128
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 290 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 354
int Cyc_Tcutil_is_array(void*t);
# 358
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 66 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 91 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 93
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 96
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 105
return first;}}
# 110
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 125
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 127
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 130
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 139
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 145
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 159
typedef void*Cyc_Parse_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Parse_pointer_quals_t;
# 162
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 166
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
declarators=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators;_tmp2->hd=_tmp5;_tmp2;});
exprs=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4;_tmp3->tl=exprs;_tmp3;});}
# 173
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
*decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 180
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 183
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 185
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 190
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 192
static void*Cyc_Parse_parse_abort(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 194
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc_atomic(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Parse_Exit_exn_struct _tmp7;_tmp7.tag=Cyc_Parse_Exit;_tmp7;});_tmp6;}));}
# 198
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 200
struct _tuple8*_tmp8=tqt;struct Cyc_Absyn_Tqual _tmpC;void*_tmpB;_LL4: _tmpC=_tmp8->f2;_tmpB=_tmp8->f3;_LL5:;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;
({void*_tmp9=0;Cyc_Warn_warn(loc,({const char*_tmpA="qualifier on type is ignored";_tag_dyneither(_tmpA,sizeof(char),29);}),_tag_dyneither(_tmp9,sizeof(void*),0));});}
# 205
return _tmpB;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 208
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 214
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpD=(void*)pqs->hd;void*_tmpE=_tmpD;void*_tmp12;struct Cyc_Absyn_Exp*_tmp11;switch(*((int*)_tmpE)){case 4: _LL7: _LL8:
 zeroterm=Cyc_Absyn_true_conref;goto _LL6;case 5: _LL9: _LLA:
 zeroterm=Cyc_Absyn_false_conref;goto _LL6;case 7: _LLB: _LLC:
 nullable=Cyc_Absyn_true_conref;goto _LL6;case 6: _LLD: _LLE:
 nullable=Cyc_Absyn_false_conref;goto _LL6;case 3: _LLF: _LL10:
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL6;case 2: _LL11: _LL12:
 bound=Cyc_Absyn_bounds_one_conref;goto _LL6;case 0: _LL13: _tmp11=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpE)->f1;_LL14:
 bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10;_tmp10.tag=1;_tmp10.f1=_tmp11;_tmp10;});_tmpF;}));goto _LL6;default: _LL15: _tmp12=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpE)->f1;_LL16:
 rgn=_tmp12;goto _LL6;}_LL6:;}
# 226
return({struct _tuple14 _tmp13;_tmp13.f1=nullable;_tmp13.f2=bound;_tmp13.f3=zeroterm;_tmp13.f4=rgn;_tmp13;});}
# 232
struct _tuple0*Cyc_Parse_gensym_enum(){
# 234
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->f1=Cyc_Absyn_Rel_n(0);_tmp14->f2=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp18;_tmp18.tag=1;_tmp18.f1=(unsigned long)enum_counter ++;({void*_tmp16[1]={& _tmp18};Cyc_aprintf(({const char*_tmp17="__anonymous_enum_%d__";_tag_dyneither(_tmp17,sizeof(char),22);}),_tag_dyneither(_tmp16,sizeof(void*),1));});});_tmp15;});_tmp14;});}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 239
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 244
struct _tuple17*_tmp19=field_info;struct _tuple0*_tmp25;struct Cyc_Absyn_Tqual _tmp24;void*_tmp23;struct Cyc_List_List*_tmp22;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;_LL18: _tmp25=(_tmp19->f1)->f1;_tmp24=(_tmp19->f1)->f2;_tmp23=(_tmp19->f1)->f3;_tmp22=(_tmp19->f1)->f4;_tmp21=(_tmp19->f1)->f5;_tmp20=(_tmp19->f2)->f1;_tmp1F=(_tmp19->f2)->f2;_LL19:;
if(_tmp22 != 0)
({void*_tmp1A=0;Cyc_Warn_err(loc,({const char*_tmp1B="bad type params in struct field";_tag_dyneither(_tmp1B,sizeof(char),32);}),_tag_dyneither(_tmp1A,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp25))
({void*_tmp1C=0;Cyc_Warn_err(loc,({const char*_tmp1D="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp1D,sizeof(char),59);}),_tag_dyneither(_tmp1C,sizeof(void*),0));});
return({struct Cyc_Absyn_Aggrfield*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->name=(*_tmp25).f2;_tmp1E->tq=_tmp24;_tmp1E->type=_tmp23;_tmp1E->width=_tmp20;_tmp1E->attributes=_tmp21;_tmp1E->requires_clause=_tmp1F;_tmp1E;});}
# 254
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp26;_tmp26.Signed_spec=0;_tmp26.Unsigned_spec=0;_tmp26.Short_spec=0;_tmp26.Long_spec=0;_tmp26.Long_Long_spec=0;_tmp26.Valid_type_spec=0;_tmp26.Type_spec=Cyc_Absyn_sint_typ;_tmp26.loc=loc;_tmp26;});}
# 265
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);
_tmp27.Type_spec=t;
_tmp27.Valid_type_spec=1;
return _tmp27;}
# 271
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp28=Cyc_Parse_empty_spec(loc);
_tmp28.Signed_spec=1;
return _tmp28;}
# 276
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp29=Cyc_Parse_empty_spec(loc);
_tmp29.Unsigned_spec=1;
return _tmp29;}
# 281
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2A=Cyc_Parse_empty_spec(loc);
_tmp2A.Short_spec=1;
return _tmp2A;}
# 286
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2B=Cyc_Parse_empty_spec(loc);
_tmp2B.Long_spec=1;
return _tmp2B;}
# 293
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 295
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 308 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp2C=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp2D=(struct _tuple8*)x->hd;struct _tuple8*_tmp2E=_tmp2D;struct _dyneither_ptr _tmp48;void**_tmp47;struct _dyneither_ptr*_tmp46;void*_tmp45;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp2E)->f3)->tag == 19){_LL1B: _tmp46=_tmp2E->f1;_tmp45=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2E->f3)->f1;if(_tmp46 != 0){_LL1C:
# 313
{void*_tmp2F=_tmp45;void**_tmp39;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1){_LL22: _tmp39=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL23: {
# 317
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46);({void*_tmp36[1]={& _tmp38};Cyc_aprintf(({const char*_tmp37="`%s";_tag_dyneither(_tmp37,sizeof(char),4);}),_tag_dyneither(_tmp36,sizeof(void*),1));});});_tmp35;});
*_tmp39=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp31;_tmp31.tag=2;_tmp31.f1=({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm;_tmp32->identity=- 1;_tmp32->kind=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp34;_tmp34.tag=0;_tmp34.f1=& Cyc_Tcutil_ik;_tmp34;});_tmp33;});_tmp32;});_tmp31;});_tmp30;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 322
_tmp2C=({struct Cyc_List_List*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->hd=({struct _tuple18*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=_tmp46;_tmp3B->f2=_tmp45;_tmp3B;});_tmp3A->tl=_tmp2C;_tmp3A;});goto _LL1A;}else{if(((struct _tuple8*)_tmp2E)->f1 != 0)goto _LL1F;else{goto _LL1F;}}}else{if(((struct _tuple8*)_tmp2E)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2E)->f3)->tag == 15){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2E)->f3)->f1)->tag == 1){_LL1D: _tmp48=*_tmp2E->f1;_tmp47=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2E->f3)->f1)->f2;_LL1E: {
# 326
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp48);({void*_tmp42[1]={& _tmp44};Cyc_aprintf(({const char*_tmp43="`%s";_tag_dyneither(_tmp43,sizeof(char),4);}),_tag_dyneither(_tmp42,sizeof(void*),1));});});_tmp41;});
*_tmp47=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3D;_tmp3D.tag=2;_tmp3D.f1=({struct Cyc_Absyn_Tvar*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->name=nm;_tmp3E->identity=- 1;_tmp3E->kind=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp40;_tmp40.tag=0;_tmp40.f1=& Cyc_Tcutil_rk;_tmp40;});_tmp3F;});_tmp3E;});_tmp3D;});_tmp3C;});
goto _LL1A;}}else{goto _LL1F;}}else{goto _LL1F;}}else{_LL1F: _LL20:
 goto _LL1A;}}_LL1A:;}
# 332
return _tmp2C;}
# 336
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp49=0;
for(0;x != 0;x=x->tl){
void*_tmp4A=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp4B=_tmp4A;void*_tmp4E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4B)->tag == 19){_LL27: _tmp4E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4B)->f1;_LL28:
# 341
 _tmp49=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=({struct _tuple18*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name;_tmp4D->f2=_tmp4E;_tmp4D;});_tmp4C->tl=_tmp49;_tmp4C;});goto _LL26;}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 345
return _tmp49;}
# 349
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp4F=e->r;void*_tmp50=_tmp4F;struct _dyneither_ptr*_tmp58;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50)->tag == 1){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50)->f1)->tag == 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50)->f1)->f1)->f1).Rel_n).val == 0){_LL2C: _tmp58=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50)->f1)->f1)->f2;_LL2D:
# 352
{struct Cyc_List_List*_tmp51=tags;for(0;_tmp51 != 0;_tmp51=_tmp51->tl){
struct _tuple18*_tmp52=(struct _tuple18*)_tmp51->hd;struct _tuple18*_tmp53=_tmp52;struct _dyneither_ptr*_tmp57;void*_tmp56;_LL31: _tmp57=_tmp53->f1;_tmp56=_tmp53->f2;_LL32:;
if(Cyc_strptrcmp(_tmp57,_tmp58)== 0)
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp55;_tmp55.tag=38;_tmp55.f1=Cyc_Tcutil_copy_type(_tmp56);_tmp55;});_tmp54;}),e->loc);}}
# 357
goto _LL2B;}else{goto _LL2E;}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 360
return e;}
# 365
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp59=t;void*_tmp74;struct Cyc_Absyn_Tqual _tmp73;void*_tmp72;union Cyc_Absyn_Constraint*_tmp71;union Cyc_Absyn_Constraint*_tmp70;union Cyc_Absyn_Constraint*_tmp6F;struct Cyc_Absyn_PtrLoc*_tmp6E;void*_tmp6D;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_Exp*_tmp6B;union Cyc_Absyn_Constraint*_tmp6A;unsigned int _tmp69;switch(*((int*)_tmp59)){case 8: _LL34: _tmp6D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).elt_type;_tmp6C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).tq;_tmp6B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).num_elts;_tmp6A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).zero_term;_tmp69=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).zt_loc;_LL35: {
# 368
struct Cyc_Absyn_Exp*nelts2=_tmp6B;
if(_tmp6B != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6B);{
# 372
void*_tmp5A=Cyc_Parse_substitute_tags(tags,_tmp6D);
if(_tmp6B != nelts2  || _tmp6D != _tmp5A)
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5C;_tmp5C.tag=8;_tmp5C.f1=({struct Cyc_Absyn_ArrayInfo _tmp5D;_tmp5D.elt_type=_tmp5A;_tmp5D.tq=_tmp6C;_tmp5D.num_elts=nelts2;_tmp5D.zero_term=_tmp6A;_tmp5D.zt_loc=_tmp69;_tmp5D;});_tmp5C;});_tmp5B;});
goto _LL33;};}case 5: _LL36: _tmp74=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).elt_typ;_tmp73=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).elt_tq;_tmp72=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).rgn;_tmp71=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).nullable;_tmp70=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).bounds;_tmp6F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).zero_term;_tmp6E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).ptrloc;_LL37: {
# 377
void*_tmp5E=Cyc_Parse_substitute_tags(tags,_tmp74);
union Cyc_Absyn_Constraint*_tmp5F=_tmp70;
{union Cyc_Absyn_Constraint _tmp60=*_tmp70;union Cyc_Absyn_Constraint _tmp61=_tmp60;struct Cyc_Absyn_Exp*_tmp65;if((_tmp61.Eq_constr).tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp61.Eq_constr).val)->tag == 1){_LL3B: _tmp65=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp61.Eq_constr).val)->f1;_LL3C:
# 381
 _tmp65=Cyc_Parse_substitute_tags_exp(tags,_tmp65);
_tmp5F=({union Cyc_Absyn_Constraint*_tmp62=_cycalloc(sizeof(*_tmp62));(_tmp62->Eq_constr).val=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp64;_tmp64.tag=1;_tmp64.f1=_tmp65;_tmp64;});_tmp63;});(_tmp62->Eq_constr).tag=1;_tmp62;});
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}
# 386
if(_tmp5E != _tmp74  || _tmp5F != _tmp70)
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp67;_tmp67.tag=5;_tmp67.f1=({struct Cyc_Absyn_PtrInfo _tmp68;_tmp68.elt_typ=_tmp5E;_tmp68.elt_tq=_tmp73;_tmp68.ptr_atts=({(_tmp68.ptr_atts).rgn=_tmp72;(_tmp68.ptr_atts).nullable=_tmp71;(_tmp68.ptr_atts).bounds=_tmp5F;(_tmp68.ptr_atts).zero_term=_tmp6F;(_tmp68.ptr_atts).ptrloc=_tmp6E;_tmp68.ptr_atts;});_tmp68;});_tmp67;});_tmp66;});
goto _LL33;}default: _LL38: _LL39:
# 391
 goto _LL33;}_LL33:;}
# 393
return t;}
# 398
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 405
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 407
return({struct _tuple19*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->f1=(*t).f2;_tmp75->f2=(*t).f3;_tmp75;});}
# 410
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp76=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp76)->tag == 4){_LL40: _LL41:
 return 1;}else{_LL42: _LL43:
 return 0;}_LL3F:;}
# 419
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp77="`H";_tag_dyneither(_tmp77,sizeof(char),3);}))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp78="`U";_tag_dyneither(_tmp78,sizeof(char),3);}))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp79="`RC";_tag_dyneither(_tmp79,sizeof(char),4);}))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 427
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7B;_tmp7B.tag=2;_tmp7B.f1=({struct Cyc_Absyn_Tvar*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->name=({struct _dyneither_ptr*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=s;_tmp7D;});_tmp7C->identity=- 1;_tmp7C->kind=k;_tmp7C;});_tmp7B;});_tmp7A;});}}}}
# 434
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp7E=t;struct Cyc_Absyn_Tvar*_tmp81;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7E)->tag == 2){_LL45: _tmp81=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7E)->f1;_LL46:
 return _tmp81;}else{_LL47: _LL48:
({void*_tmp7F=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmp80="expecting a list of type variables, not types";_tag_dyneither(_tmp80,sizeof(char),46);}),_tag_dyneither(_tmp7F,sizeof(void*),0));});}_LL44:;}
# 442
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp82=Cyc_Tcutil_compress(t);void*_tmp83=_tmp82;void**_tmp88;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp83)->tag == 2){_LL4A: _tmp88=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp83)->f1)->kind;_LL4B: {
# 445
void*_tmp84=Cyc_Absyn_compress_kb(*_tmp88);void*_tmp85=_tmp84;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp85)->tag == 1){_LL4F: _LL50:
# 447
 if(!leq)*_tmp88=Cyc_Tcutil_kind_to_bound(k);else{
*_tmp88=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp87;_tmp87.tag=2;_tmp87.f1=0;_tmp87.f2=k;_tmp87;});_tmp86;});}
return;}else{_LL51: _LL52:
 return;}_LL4E:;}}else{_LL4C: _LL4D:
# 452
 return;}_LL49:;}
# 457
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 463
if(tds == 0)return tms;
# 468
if(tms == 0)return 0;{
# 470
void*_tmp89=(void*)tms->hd;void*_tmp8A=_tmp89;void*_tmpA7;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp8A)->tag == 3){_LL54: _tmpA7=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp8A)->f1;_LL55:
# 473
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 476
void*_tmp8B=_tmpA7;struct Cyc_List_List*_tmpA5;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp8B)->tag == 1){_LL59: _LL5A:
# 478
({void*_tmp8C=0;Cyc_Warn_warn(loc,({const char*_tmp8D="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp8D,sizeof(char),93);}),_tag_dyneither(_tmp8C,sizeof(void*),0));});
# 480
return tms;}else{_LL5B: _tmpA5=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp8B)->f1;_LL5C:
# 482
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds))
({void*_tmp8E=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmp8F="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp8F,sizeof(char),73);}),_tag_dyneither(_tmp8E,sizeof(void*),0));});{
# 486
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){
struct Cyc_List_List*_tmp90=tds;
for(0;_tmp90 != 0;_tmp90=_tmp90->tl){
struct Cyc_Absyn_Decl*_tmp91=(struct Cyc_Absyn_Decl*)_tmp90->hd;
void*_tmp92=_tmp91->r;void*_tmp93=_tmp92;struct Cyc_Absyn_Vardecl*_tmp9C;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp93)->tag == 0){_LL5E: _tmp9C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp93)->f1;_LL5F:
# 493
 if(Cyc_zstrptrcmp((*_tmp9C->name).f2,(struct _dyneither_ptr*)_tmpA5->hd)!= 0)
continue;
if(_tmp9C->initializer != 0)
({void*_tmp94=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp91->loc,({const char*_tmp95="initializer found in parameter declaration";_tag_dyneither(_tmp95,sizeof(char),43);}),_tag_dyneither(_tmp94,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp9C->name))
({void*_tmp96=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp91->loc,({const char*_tmp97="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp97,sizeof(char),47);}),_tag_dyneither(_tmp96,sizeof(void*),0));});
rev_new_params=({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->hd=({struct _tuple8*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->f1=(*_tmp9C->name).f2;_tmp99->f2=_tmp9C->tq;_tmp99->f3=_tmp9C->type;_tmp99;});_tmp98->tl=rev_new_params;_tmp98;});
# 502
goto L;}else{_LL60: _LL61:
({void*_tmp9A=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp91->loc,({const char*_tmp9B="nonvariable declaration in parameter type";_tag_dyneither(_tmp9B,sizeof(char),42);}),_tag_dyneither(_tmp9A,sizeof(void*),0));});}_LL5D:;}
# 506
L: if(_tmp90 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpA5->hd));({void*_tmp9D[1]={& _tmp9F};((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmp9E="%s is not given a type";_tag_dyneither(_tmp9E,sizeof(char),23);}),_tag_dyneither(_tmp9D,sizeof(void*),1));});});}
# 509
return({struct Cyc_List_List*_tmpA0=_region_malloc(yy,sizeof(*_tmpA0));_tmpA0->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA1=_region_malloc(yy,sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA2;_tmpA2.tag=3;_tmpA2.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA3=_region_malloc(yy,sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA4;_tmpA4.tag=1;_tmpA4.f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmpA4.f2=0;_tmpA4.f3=0;_tmpA4.f4=0;_tmpA4.f5=0;_tmpA4.f6=0;_tmpA4.f7=0;_tmpA4;});_tmpA3;});_tmpA2;});_tmpA1;});_tmpA0->tl=0;_tmpA0;});};}_LL58:;}
# 516
goto _LL57;}else{_LL56: _LL57:
 return({struct Cyc_List_List*_tmpA6=_region_malloc(yy,sizeof(*_tmpA6));_tmpA6->hd=(void*)tms->hd;_tmpA6->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmpA6;});}_LL53:;};}
# 524
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 528
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmpA8;_tmpA8.id=d.id;_tmpA8.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmpA8;});{
# 531
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 537
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 543
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpA9=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmpAA=_tmpA9;switch(_tmpAA){case Cyc_Parse_Extern_sc: _LL63: _LL64:
 sc=Cyc_Absyn_Extern;goto _LL62;case Cyc_Parse_Static_sc: _LL65: _LL66:
 sc=Cyc_Absyn_Static;goto _LL62;default: _LL67: _LL68:
({void*_tmpAB=0;Cyc_Warn_err(loc,({const char*_tmpAC="bad storage class on function";_tag_dyneither(_tmpAC,sizeof(char),30);}),_tag_dyneither(_tmpAB,sizeof(void*),0));});goto _LL62;}_LL62:;}}{
# 550
void*_tmpAD=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpAE=Cyc_Parse_apply_tms(tq,_tmpAD,atts,d.tms);struct _tuple13 _tmpAF=_tmpAE;struct Cyc_Absyn_Tqual _tmpD4;void*_tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD1;_LL6A: _tmpD4=_tmpAF.f1;_tmpD3=_tmpAF.f2;_tmpD2=_tmpAF.f3;_tmpD1=_tmpAF.f4;_LL6B:;
# 555
if(_tmpD2 != 0)
# 558
({void*_tmpB0=0;Cyc_Warn_warn(loc,({const char*_tmpB1="bad type params, ignoring";_tag_dyneither(_tmpB1,sizeof(char),26);}),_tag_dyneither(_tmpB0,sizeof(void*),0));});{
# 560
void*_tmpB2=_tmpD3;struct Cyc_List_List*_tmpD0;void*_tmpCF;struct Cyc_Absyn_Tqual _tmpCE;void*_tmpCD;struct Cyc_List_List*_tmpCC;int _tmpCB;struct Cyc_Absyn_VarargInfo*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*_tmpC4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->tag == 9){_LL6D: _tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).tvars;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).effect;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).ret_tqual;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).ret_typ;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).args;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).c_varargs;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).cyc_varargs;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).rgn_po;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).attributes;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).requires_clause;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).requires_relns;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).ensures_clause;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2)->f1).ensures_relns;_LL6E: {
# 564
struct Cyc_List_List*_tmpB3=0;
{struct Cyc_List_List*_tmpB4=_tmpCC;for(0;_tmpB4 != 0;_tmpB4=_tmpB4->tl){
struct _tuple8*_tmpB5=(struct _tuple8*)_tmpB4->hd;struct _tuple8*_tmpB6=_tmpB5;struct _dyneither_ptr*_tmpC0;struct Cyc_Absyn_Tqual _tmpBF;void*_tmpBE;_LL72: _tmpC0=_tmpB6->f1;_tmpBF=_tmpB6->f2;_tmpBE=_tmpB6->f3;_LL73:;
if(_tmpC0 == 0){
({void*_tmpB7=0;Cyc_Warn_err(loc,({const char*_tmpB8="missing argument variable in function prototype";_tag_dyneither(_tmpB8,sizeof(char),48);}),_tag_dyneither(_tmpB7,sizeof(void*),0));});
_tmpB3=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=({struct _tuple8*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2);_tmpBA->f2=_tmpBF;_tmpBA->f3=_tmpBE;_tmpBA;});_tmpB9->tl=_tmpB3;_tmpB9;});}else{
# 571
_tmpB3=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=({struct _tuple8*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->f1=_tmpC0;_tmpBD->f2=_tmpBF;_tmpBD->f3=_tmpBE;_tmpBD;});_tmpBC->tl=_tmpB3;_tmpBC;});}}}
# 575
return({struct Cyc_Absyn_Fndecl*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->sc=sc;_tmpC1->name=d.id;_tmpC1->tvs=_tmpD0;_tmpC1->is_inline=is_inline;_tmpC1->effect=_tmpCF;_tmpC1->ret_tqual=_tmpCE;_tmpC1->ret_type=_tmpCD;_tmpC1->args=
# 578
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB3);_tmpC1->c_varargs=_tmpCB;_tmpC1->cyc_varargs=_tmpCA;_tmpC1->rgn_po=_tmpC9;_tmpC1->body=body;_tmpC1->cached_typ=0;_tmpC1->param_vardecls=0;_tmpC1->fn_vardecl=0;_tmpC1->attributes=
# 585
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpC8,_tmpD1);_tmpC1->requires_clause=_tmpC7;_tmpC1->requires_relns=0;_tmpC1->ensures_clause=_tmpC5;_tmpC1->ensures_relns=0;_tmpC1;});}}else{_LL6F: _LL70:
# 590
({void*_tmpC2=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmpC3="declarator is not a function prototype";_tag_dyneither(_tmpC3,sizeof(char),39);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});}_LL6C:;};};};}static char _tmpD5[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 594
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpD5,_tmpD5,_tmpD5 + 76};static char _tmpD6[87]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 596
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpD6,_tmpD6,_tmpD6 + 87};static char _tmpD7[74]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 598
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpD7,_tmpD7,_tmpD7 + 74};static char _tmpD8[84]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 600
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpD8,_tmpD8,_tmpD8 + 84};
# 607
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 610
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpD9=0;Cyc_Warn_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpD9,sizeof(void*),0));});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpDA=0;Cyc_Warn_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpDA,sizeof(void*),0));});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpDB=0;Cyc_Warn_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpDB,sizeof(void*),0));});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpDC=0;Cyc_Warn_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpDC,sizeof(void*),0));});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpDD=0;Cyc_Warn_err(loc,Cyc_Parse_msg1,_tag_dyneither(_tmpDD,sizeof(void*),0));});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 632
return s1;}
# 638
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 641
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 648
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpDE=0;Cyc_Warn_err(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpDE,sizeof(void*),0));});
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpDF=0;Cyc_Warn_err(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpDF,sizeof(void*),0));});
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 660
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpE0=0;Cyc_Warn_warn(loc,({const char*_tmpE1="missing type within specifier";_tag_dyneither(_tmpE1,sizeof(char),30);}),_tag_dyneither(_tmpE0,sizeof(void*),0));});
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 665
if(seen_sign){
void*_tmpE2=t;enum Cyc_Absyn_Sign _tmpE6;enum Cyc_Absyn_Size_of _tmpE5;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->tag == 6){_LL75: _tmpE6=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->f1;_tmpE5=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->f2;_LL76:
# 668
 if(_tmpE6 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmpE5);
goto _LL74;}else{_LL77: _LL78:
({void*_tmpE3=0;Cyc_Warn_err(loc,({const char*_tmpE4="sign specification on non-integral type";_tag_dyneither(_tmpE4,sizeof(char),40);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});goto _LL74;}_LL74:;}
# 673
if(seen_size){
void*_tmpE7=t;enum Cyc_Absyn_Sign _tmpEB;enum Cyc_Absyn_Size_of _tmpEA;switch(*((int*)_tmpE7)){case 6: _LL7A: _tmpEB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE7)->f1;_tmpEA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE7)->f2;_LL7B:
# 676
 if(_tmpEA != sz)
t=Cyc_Absyn_int_typ(_tmpEB,sz);
goto _LL79;case 7: _LL7C: _LL7D:
# 680
 t=Cyc_Absyn_float_typ(2);goto _LL79;default: _LL7E: _LL7F:
({void*_tmpE8=0;Cyc_Warn_err(loc,({const char*_tmpE9="size qualifier on non-integral type";_tag_dyneither(_tmpE9,sizeof(char),36);}),_tag_dyneither(_tmpE8,sizeof(void*),0));});goto _LL79;}_LL79:;}}
# 684
return t;}
# 687
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 691
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpEC=d.id;
struct _tuple13 _tmpED=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpEE=_tmpED;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_List_List*_tmpF3;_LL81: _tmpF6=_tmpEE.f1;_tmpF5=_tmpEE.f2;_tmpF4=_tmpEE.f3;_tmpF3=_tmpEE.f4;_LL82:;
# 697
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpEF=_region_malloc(r,sizeof(*_tmpEF));_tmpEF->hd=({struct _tuple15*_tmpF0=_region_malloc(r,sizeof(*_tmpF0));_tmpF0->f1=_tmpEC;_tmpF0->f2=_tmpF6;_tmpF0->f3=_tmpF5;_tmpF0->f4=_tmpF4;_tmpF0->f5=_tmpF3;_tmpF0;});_tmpEF->tl=0;_tmpEF;});else{
# 700
return({struct Cyc_List_List*_tmpF1=_region_malloc(r,sizeof(*_tmpF1));_tmpF1->hd=({struct _tuple15*_tmpF2=_region_malloc(r,sizeof(*_tmpF2));_tmpF2->f1=_tmpEC;_tmpF2->f2=_tmpF6;_tmpF2->f3=_tmpF5;_tmpF2->f4=_tmpF4;_tmpF2->f5=_tmpF3;_tmpF2;});_tmpF1->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpF1;});}};}
# 704
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 707
if(tms == 0)return({struct _tuple13 _tmpF7;_tmpF7.f1=tq;_tmpF7.f2=t;_tmpF7.f3=0;_tmpF7.f4=atts;_tmpF7;});{
void*_tmpF8=(void*)tms->hd;void*_tmpF9=_tmpF8;unsigned int _tmp122;struct Cyc_List_List*_tmp121;struct Cyc_Absyn_PtrAtts _tmp120;struct Cyc_Absyn_Tqual _tmp11F;struct Cyc_List_List*_tmp11E;unsigned int _tmp11D;void*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;union Cyc_Absyn_Constraint*_tmp11A;unsigned int _tmp119;union Cyc_Absyn_Constraint*_tmp118;unsigned int _tmp117;switch(*((int*)_tmpF9)){case 0: _LL84: _tmp118=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF9)->f1;_tmp117=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF9)->f2;_LL85:
# 710
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp118,_tmp117),atts,tms->tl);case 1: _LL86: _tmp11B=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF9)->f1;_tmp11A=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF9)->f2;_tmp119=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF9)->f3;_LL87:
# 713
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp11B,_tmp11A,_tmp119),atts,tms->tl);case 3: _LL88: _tmp11C=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF9)->f1;_LL89: {
# 716
void*_tmpFA=_tmp11C;unsigned int _tmp110;struct Cyc_List_List*_tmp10F;int _tmp10E;struct Cyc_Absyn_VarargInfo*_tmp10D;void*_tmp10C;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->tag == 1){_LL91: _tmp10F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f1;_tmp10E=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f2;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f3;_tmp10C=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f4;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f5;_tmp10A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f6;_tmp109=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFA)->f7;_LL92: {
# 718
struct Cyc_List_List*typvars=0;
# 720
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->hd=(void*)as->hd;_tmpFB->tl=fn_atts;_tmpFB;});else{
# 725
new_atts=({struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=(void*)as->hd;_tmpFC->tl=new_atts;_tmpFC;});}}}
# 728
if(tms->tl != 0){
void*_tmpFD=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpFE=_tmpFD;struct Cyc_List_List*_tmpFF;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFE)->tag == 4){_LL96: _tmpFF=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFE)->f1;_LL97:
# 731
 typvars=_tmpFF;
tms=tms->tl;
goto _LL95;}else{_LL98: _LL99:
# 735
 goto _LL95;}_LL95:;}
# 739
if(((((!_tmp10E  && _tmp10D == 0) && _tmp10F != 0) && _tmp10F->tl == 0) && (*((struct _tuple8*)_tmp10F->hd)).f1 == 0) && (*((struct _tuple8*)_tmp10F->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 744
_tmp10F=0;{
# 747
struct Cyc_List_List*_tmp100=Cyc_Parse_get_arg_tags(_tmp10F);
# 749
if(_tmp100 != 0)
t=Cyc_Parse_substitute_tags(_tmp100,t);
t=Cyc_Parse_array2ptr(t,0);
# 754
{struct Cyc_List_List*_tmp101=_tmp10F;for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
struct _tuple8*_tmp102=(struct _tuple8*)_tmp101->hd;struct _tuple8*_tmp103=_tmp102;struct _dyneither_ptr*_tmp106;struct Cyc_Absyn_Tqual _tmp105;void**_tmp104;_LL9B: _tmp106=_tmp103->f1;_tmp105=_tmp103->f2;_tmp104=(void**)& _tmp103->f3;_LL9C:;
if(_tmp100 != 0)
*_tmp104=Cyc_Parse_substitute_tags(_tmp100,*_tmp104);
*_tmp104=Cyc_Parse_array2ptr(*_tmp104,1);}}
# 766
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp10C,tq,t,_tmp10F,_tmp10E,_tmp10D,_tmp10B,fn_atts,_tmp10A,_tmp109),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}}else{_LL93: _tmp110=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpFA)->f2;_LL94:
# 773
({void*_tmp107=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp110,({const char*_tmp108="function declaration without parameter types";_tag_dyneither(_tmp108,sizeof(char),45);}),_tag_dyneither(_tmp107,sizeof(void*),0));});}_LL90:;}case 4: _LL8A: _tmp11E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF9)->f1;_tmp11D=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF9)->f2;_LL8B:
# 780
 if(tms->tl == 0)
return({struct _tuple13 _tmp111;_tmp111.f1=tq;_tmp111.f2=t;_tmp111.f3=_tmp11E;_tmp111.f4=atts;_tmp111;});
# 785
({void*_tmp112=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp11D,({const char*_tmp113="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmp113,sizeof(char),68);}),_tag_dyneither(_tmp112,sizeof(void*),0));});case 2: _LL8C: _tmp120=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF9)->f1;_tmp11F=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF9)->f2;_LL8D:
# 788
 return Cyc_Parse_apply_tms(_tmp11F,(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp115;_tmp115.tag=5;_tmp115.f1=({struct Cyc_Absyn_PtrInfo _tmp116;_tmp116.elt_typ=t;_tmp116.elt_tq=tq;_tmp116.ptr_atts=_tmp120;_tmp116;});_tmp115;});_tmp114;}),atts,tms->tl);default: _LL8E: _tmp122=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF9)->f1;_tmp121=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF9)->f2;_LL8F:
# 793
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp121),tms->tl);}_LL83:;};}
# 799
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 808
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp123=t;struct _tuple0*_tmp12D;struct Cyc_Absyn_Tqual _tmp12C;void*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp129;_LL9E: _tmp12D=_tmp123->f1;_tmp12C=_tmp123->f2;_tmp12B=_tmp123->f3;_tmp12A=_tmp123->f4;_tmp129=_tmp123->f5;_LL9F:;
# 811
Cyc_Lex_register_typedef(_tmp12D);{
# 813
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp124=_tmp12B;struct Cyc_Core_Opt*_tmp125;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp124)->tag == 1){_LLA1: _tmp125=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp124)->f1;_LLA2:
# 817
 type=0;
if(_tmp125 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp125;}
goto _LLA0;}else{_LLA3: _LLA4:
 kind=0;type=_tmp12B;goto _LLA0;}_LLA0:;}
# 823
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp127;_tmp127.tag=8;_tmp127.f1=({struct Cyc_Absyn_Typedefdecl*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->name=_tmp12D;_tmp128->tvs=_tmp12A;_tmp128->kind=kind;_tmp128->defn=type;_tmp128->atts=_tmp129;_tmp128->tq=_tmp12C;_tmp128->extern_c=0;_tmp128;});_tmp127;});_tmp126;}),loc);};}
# 830
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E[0]=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp12F;_tmp12F.tag=12;_tmp12F.f1=d;_tmp12F.f2=s;_tmp12F;});_tmp12E;}),d->loc);}
# 836
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 845
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 849
struct _RegionHandle _tmp130=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp130;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp131=ds;struct Cyc_Absyn_Tqual _tmp199;struct Cyc_Parse_Type_specifier _tmp198;struct Cyc_List_List*_tmp197;_LLA6: _tmp199=_tmp131.tq;_tmp198=_tmp131.type_specs;_tmp197=_tmp131.attributes;_LLA7:;
if(_tmp199.loc == 0)_tmp199.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp132=0;Cyc_Warn_warn(loc,({const char*_tmp133="inline qualifier on non-function definition";_tag_dyneither(_tmp133,sizeof(char),44);}),_tag_dyneither(_tmp132,sizeof(void*),0));});{
# 855
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp134=*ds.sc;enum Cyc_Parse_Storage_class _tmp135=_tmp134;switch(_tmp135){case Cyc_Parse_Typedef_sc: _LLA9: _LLAA:
 istypedef=1;goto _LLA8;case Cyc_Parse_Extern_sc: _LLAB: _LLAC:
 s=Cyc_Absyn_Extern;goto _LLA8;case Cyc_Parse_ExternC_sc: _LLAD: _LLAE:
 s=Cyc_Absyn_ExternC;goto _LLA8;case Cyc_Parse_Static_sc: _LLAF: _LLB0:
 s=Cyc_Absyn_Static;goto _LLA8;case Cyc_Parse_Auto_sc: _LLB1: _LLB2:
 s=Cyc_Absyn_Public;goto _LLA8;case Cyc_Parse_Register_sc: _LLB3: _LLB4:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}goto _LLA8;default: _LLB5: _LLB6:
 s=Cyc_Absyn_Abstract;goto _LLA8;}_LLA8:;}{
# 871
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 875
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 883
void*_tmp136=Cyc_Parse_collapse_type_specifiers(_tmp198,loc);
if(declarators == 0){
# 887
void*_tmp137=_tmp136;struct Cyc_List_List*_tmp17E;struct _tuple0*_tmp17D;struct _tuple0*_tmp17C;int _tmp17B;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Datatypedecl**_tmp179;enum Cyc_Absyn_AggrKind _tmp178;struct _tuple0*_tmp177;struct Cyc_List_List*_tmp176;struct Cyc_Absyn_Datatypedecl*_tmp175;struct Cyc_Absyn_Enumdecl*_tmp174;struct Cyc_Absyn_Aggrdecl*_tmp173;switch(*((int*)_tmp137)){case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp137)->f1)->r)){case 0: _LLB8: _tmp173=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp137)->f1)->r)->f1;_LLB9:
# 889
 _tmp173->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp173->attributes,_tmp197);
_tmp173->sc=s;{
struct Cyc_List_List*_tmp13B=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp13A;_tmp13A.tag=5;_tmp13A.f1=_tmp173;_tmp13A;});_tmp139;}),loc);_tmp138->tl=0;_tmp138;});_npop_handler(0);return _tmp13B;};case 1: _LLBA: _tmp174=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp137)->f1)->r)->f1;_LLBB:
# 893
 if(_tmp197 != 0)({void*_tmp13C=0;Cyc_Warn_err(loc,({const char*_tmp13D="attributes on enum not supported";_tag_dyneither(_tmp13D,sizeof(char),33);}),_tag_dyneither(_tmp13C,sizeof(void*),0));});
_tmp174->sc=s;{
struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp140;_tmp140.tag=7;_tmp140.f1=_tmp174;_tmp140;});_tmp13F;}),loc);_tmp13E->tl=0;_tmp13E;});_npop_handler(0);return _tmp141;};default: _LLBC: _tmp175=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp137)->f1)->r)->f1;_LLBD:
# 897
 if(_tmp197 != 0)({void*_tmp142=0;Cyc_Warn_err(loc,({const char*_tmp143="attributes on datatypes not supported";_tag_dyneither(_tmp143,sizeof(char),38);}),_tag_dyneither(_tmp142,sizeof(void*),0));});
_tmp175->sc=s;{
struct Cyc_List_List*_tmp147=({struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp146;_tmp146.tag=6;_tmp146.f1=_tmp175;_tmp146;});_tmp145;}),loc);_tmp144->tl=0;_tmp144;});_npop_handler(0);return _tmp147;};}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp137)->f1).aggr_info).UnknownAggr).tag == 1){_LLBE: _tmp178=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp137)->f1).aggr_info).UnknownAggr).val).f1;_tmp177=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp137)->f1).aggr_info).UnknownAggr).val).f2;_tmp176=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp137)->f1).targs;_LLBF: {
# 901
struct Cyc_List_List*_tmp148=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp176);
struct Cyc_Absyn_Aggrdecl*_tmp149=({struct Cyc_Absyn_Aggrdecl*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->kind=_tmp178;_tmp150->sc=s;_tmp150->name=_tmp177;_tmp150->tvs=_tmp148;_tmp150->impl=0;_tmp150->attributes=0;_tmp150->expected_mem_kind=0;_tmp150;});
if(_tmp197 != 0)({void*_tmp14A=0;Cyc_Warn_err(loc,({const char*_tmp14B="bad attributes on type declaration";_tag_dyneither(_tmp14B,sizeof(char),35);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});{
struct Cyc_List_List*_tmp14F=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp14E;_tmp14E.tag=5;_tmp14E.f1=_tmp149;_tmp14E;});_tmp14D;}),loc);_tmp14C->tl=0;_tmp14C;});_npop_handler(0);return _tmp14F;};}}else{goto _LLC8;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp137)->f1).datatype_info).KnownDatatype).tag == 2){_LLC0: _tmp179=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp137)->f1).datatype_info).KnownDatatype).val;_LLC1:
# 906
 if(_tmp197 != 0)({void*_tmp151=0;Cyc_Warn_err(loc,({const char*_tmp152="bad attributes on datatype";_tag_dyneither(_tmp152,sizeof(char),27);}),_tag_dyneither(_tmp151,sizeof(void*),0));});{
struct Cyc_List_List*_tmp156=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp155;_tmp155.tag=6;_tmp155.f1=*_tmp179;_tmp155;});_tmp154;}),loc);_tmp153->tl=0;_tmp153;});_npop_handler(0);return _tmp156;};}else{_LLC2: _tmp17C=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp137)->f1).datatype_info).UnknownDatatype).val).name;_tmp17B=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp137)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp17A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp137)->f1).targs;_LLC3: {
# 909
struct Cyc_List_List*_tmp157=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17A);
struct Cyc_Absyn_Decl*_tmp158=Cyc_Absyn_datatype_decl(s,_tmp17C,_tmp157,0,_tmp17B,loc);
if(_tmp197 != 0)({void*_tmp159=0;Cyc_Warn_err(loc,({const char*_tmp15A="bad attributes on datatype";_tag_dyneither(_tmp15A,sizeof(char),27);}),_tag_dyneither(_tmp159,sizeof(void*),0));});{
struct Cyc_List_List*_tmp15C=({struct Cyc_List_List*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->hd=_tmp158;_tmp15B->tl=0;_tmp15B;});_npop_handler(0);return _tmp15C;};}}case 13: _LLC4: _tmp17D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp137)->f1;_LLC5: {
# 914
struct Cyc_Absyn_Enumdecl*_tmp15D=({struct Cyc_Absyn_Enumdecl*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->sc=s;_tmp165->name=_tmp17D;_tmp165->fields=0;_tmp165;});
if(_tmp197 != 0)({void*_tmp15E=0;Cyc_Warn_err(loc,({const char*_tmp15F="bad attributes on enum";_tag_dyneither(_tmp15F,sizeof(char),23);}),_tag_dyneither(_tmp15E,sizeof(void*),0));});{
struct Cyc_List_List*_tmp164=({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->hd=({struct Cyc_Absyn_Decl*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp163;_tmp163.tag=7;_tmp163.f1=_tmp15D;_tmp163;});_tmp162;});_tmp161->loc=loc;_tmp161;});_tmp160->tl=0;_tmp160;});_npop_handler(0);return _tmp164;};}case 14: _LLC6: _tmp17E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp137)->f1;_LLC7: {
# 920
struct Cyc_Absyn_Enumdecl*_tmp166=({struct Cyc_Absyn_Enumdecl*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->sc=s;_tmp16E->name=Cyc_Parse_gensym_enum();_tmp16E->fields=({struct Cyc_Core_Opt*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->v=_tmp17E;_tmp16F;});_tmp16E;});
if(_tmp197 != 0)({void*_tmp167=0;Cyc_Warn_err(loc,({const char*_tmp168="bad attributes on enum";_tag_dyneither(_tmp168,sizeof(char),23);}),_tag_dyneither(_tmp167,sizeof(void*),0));});{
struct Cyc_List_List*_tmp16D=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=({struct Cyc_Absyn_Decl*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp16C;_tmp16C.tag=7;_tmp16C.f1=_tmp166;_tmp16C;});_tmp16B;});_tmp16A->loc=loc;_tmp16A;});_tmp169->tl=0;_tmp169;});_npop_handler(0);return _tmp16D;};}default: _LLC8: _LLC9:
({void*_tmp170=0;Cyc_Warn_err(loc,({const char*_tmp171="missing declarator";_tag_dyneither(_tmp171,sizeof(char),19);}),_tag_dyneither(_tmp170,sizeof(void*),0));});{struct Cyc_List_List*_tmp172=0;_npop_handler(0);return _tmp172;};}_LLB7:;}else{
# 927
struct Cyc_List_List*_tmp17F=Cyc_Parse_apply_tmss(mkrgn,_tmp199,_tmp136,declarators,_tmp197);
if(istypedef){
# 932
if(!exps_empty)
({void*_tmp180=0;Cyc_Warn_err(loc,({const char*_tmp181="initializer in typedef declaration";_tag_dyneither(_tmp181,sizeof(char),35);}),_tag_dyneither(_tmp180,sizeof(void*),0));});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp17F);
struct Cyc_List_List*_tmp182=decls;_npop_handler(0);return _tmp182;};}else{
# 938
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp183=_tmp17F;for(0;_tmp183 != 0;(_tmp183=_tmp183->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp184=(struct _tuple15*)_tmp183->hd;struct _tuple15*_tmp185=_tmp184;struct _tuple0*_tmp195;struct Cyc_Absyn_Tqual _tmp194;void*_tmp193;struct Cyc_List_List*_tmp192;struct Cyc_List_List*_tmp191;_LLCB: _tmp195=_tmp185->f1;_tmp194=_tmp185->f2;_tmp193=_tmp185->f3;_tmp192=_tmp185->f4;_tmp191=_tmp185->f5;_LLCC:;
if(_tmp192 != 0)
({void*_tmp186=0;Cyc_Warn_warn(loc,({const char*_tmp187="bad type params, ignoring";_tag_dyneither(_tmp187,sizeof(char),26);}),_tag_dyneither(_tmp186,sizeof(void*),0));});
if(exprs == 0)
({void*_tmp188=0;((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmp189="unexpected NULL in parse!";_tag_dyneither(_tmp189,sizeof(char),26);}),_tag_dyneither(_tmp188,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp18A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp18B=Cyc_Absyn_new_vardecl(_tmp195,_tmp193,_tmp18A);
_tmp18B->tq=_tmp194;
_tmp18B->sc=s;
_tmp18B->attributes=_tmp191;{
struct Cyc_Absyn_Decl*_tmp18C=({struct Cyc_Absyn_Decl*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp190;_tmp190.tag=0;_tmp190.f1=_tmp18B;_tmp190;});_tmp18F;});_tmp18E->loc=loc;_tmp18E;});
decls=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=_tmp18C;_tmp18D->tl=decls;_tmp18D;});};};}}{
# 953
struct Cyc_List_List*_tmp196=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp196;};}}};};};};}
# 850
;_pop_region(mkrgn);}
# 959
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp19A=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp19B=_tmp19A;switch(_tmp19B){case 65: _LLCE: _LLCF:
 return& Cyc_Tcutil_ak;case 77: _LLD0: _LLD1:
 return& Cyc_Tcutil_mk;case 66: _LLD2: _LLD3:
 return& Cyc_Tcutil_bk;case 82: _LLD4: _LLD5:
 return& Cyc_Tcutil_rk;case 69: _LLD6: _LLD7:
 return& Cyc_Tcutil_ek;case 73: _LLD8: _LLD9:
 return& Cyc_Tcutil_ik;case 85: _LLDA: _LLDB:
# 969
{char _tmp19C=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp19D=_tmp19C;switch(_tmp19D){case 82: _LLE1: _LLE2:
 return& Cyc_Tcutil_urk;case 65: _LLE3: _LLE4:
 return& Cyc_Tcutil_uak;case 77: _LLE5: _LLE6:
 return& Cyc_Tcutil_umk;case 66: _LLE7: _LLE8:
 return& Cyc_Tcutil_ubk;default: _LLE9: _LLEA:
 goto _LLE0;}_LLE0:;}
# 976
goto _LLCD;case 84: _LLDC: _LLDD:
# 978
{char _tmp19E=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp19F=_tmp19E;switch(_tmp19F){case 82: _LLEC: _LLED:
 return& Cyc_Tcutil_trk;case 65: _LLEE: _LLEF:
 return& Cyc_Tcutil_tak;case 77: _LLF0: _LLF1:
 return& Cyc_Tcutil_tmk;case 66: _LLF2: _LLF3:
 return& Cyc_Tcutil_tbk;default: _LLF4: _LLF5:
 goto _LLEB;}_LLEB:;}
# 985
goto _LLCD;default: _LLDE: _LLDF:
 goto _LLCD;}_LLCD:;}
# 988
({struct Cyc_Int_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=1;_tmp1A3.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));({struct Cyc_String_pa_PrintArg_struct _tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1A0[2]={& _tmp1A2,& _tmp1A3};Cyc_Warn_err(loc,({const char*_tmp1A1="bad kind: %s; strlen=%d";_tag_dyneither(_tmp1A1,sizeof(char),24);}),_tag_dyneither(_tmp1A0,sizeof(void*),2));});});});
return& Cyc_Tcutil_bk;}
# 993
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A4=e->r;void*_tmp1A5=_tmp1A4;int _tmp1A8;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A5)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A5)->f1).Int_c).tag == 5){_LLF7: _tmp1A8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A5)->f1).Int_c).val).f2;_LLF8:
 return _tmp1A8;}else{goto _LLF9;}}else{_LLF9: _LLFA:
# 997
({void*_tmp1A6=0;Cyc_Warn_err(loc,({const char*_tmp1A7="expecting integer constant";_tag_dyneither(_tmp1A7,sizeof(char),27);}),_tag_dyneither(_tmp1A6,sizeof(void*),0));});
return 0;}_LLF6:;}
# 1003
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A9=e->r;void*_tmp1AA=_tmp1A9;struct _dyneither_ptr _tmp1AD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AA)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AA)->f1).String_c).tag == 8){_LLFC: _tmp1AD=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AA)->f1).String_c).val;_LLFD:
 return _tmp1AD;}else{goto _LLFE;}}else{_LLFE: _LLFF:
# 1007
({void*_tmp1AB=0;Cyc_Warn_err(loc,({const char*_tmp1AC="expecting string constant";_tag_dyneither(_tmp1AC,sizeof(char),26);}),_tag_dyneither(_tmp1AB,sizeof(void*),0));});
return _tag_dyneither(0,0,0);}_LLFB:;}
# 1013
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1AE=x;long long _tmp1B6;char _tmp1B5;int _tmp1B4;switch((_tmp1AE.LongLong_c).tag){case 5: _LL101: _tmp1B4=((_tmp1AE.Int_c).val).f2;_LL102:
 return(unsigned int)_tmp1B4;case 2: _LL103: _tmp1B5=((_tmp1AE.Char_c).val).f2;_LL104:
 return(unsigned int)_tmp1B5;case 6: _LL105: _tmp1B6=((_tmp1AE.LongLong_c).val).f2;_LL106: {
# 1018
unsigned long long y=(unsigned long long)_tmp1B6;
if(y > -1)
({void*_tmp1AF=0;Cyc_Warn_err(loc,({const char*_tmp1B0="integer constant too large";_tag_dyneither(_tmp1B0,sizeof(char),27);}),_tag_dyneither(_tmp1AF,sizeof(void*),0));});
return(unsigned int)_tmp1B6;}default: _LL107: _LL108:
# 1023
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));({void*_tmp1B1[1]={& _tmp1B3};Cyc_Warn_err(loc,({const char*_tmp1B2="expected integer constant but found %s";_tag_dyneither(_tmp1B2,sizeof(char),39);}),_tag_dyneither(_tmp1B1,sizeof(void*),1));});});
return 0;}_LL100:;}
# 1029
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1B7=p->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C5;struct _tuple0*_tmp1C4;struct Cyc_List_List*_tmp1C3;struct _dyneither_ptr _tmp1C2;int _tmp1C1;char _tmp1C0;enum Cyc_Absyn_Sign _tmp1BF;int _tmp1BE;struct Cyc_Absyn_Pat*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BC;struct _tuple0*_tmp1BB;switch(*((int*)_tmp1B8)){case 15: _LL10A: _tmp1BB=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_LL10B:
 return Cyc_Absyn_unknownid_exp(_tmp1BB,p->loc);case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B8)->f2)->r)->tag == 0){_LL10C: _tmp1BC=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_LL10D:
# 1033
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1BC->name,p->loc),p->loc);}else{goto _LL11C;}case 6: _LL10E: _tmp1BD=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_LL10F:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1BD),p->loc);case 9: _LL110: _LL111:
 return Cyc_Absyn_null_exp(p->loc);case 10: _LL112: _tmp1BF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_tmp1BE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B8)->f2;_LL113:
 return Cyc_Absyn_int_exp(_tmp1BF,_tmp1BE,p->loc);case 11: _LL114: _tmp1C0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_LL115:
 return Cyc_Absyn_char_exp(_tmp1C0,p->loc);case 12: _LL116: _tmp1C2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B8)->f2;_LL117:
 return Cyc_Absyn_float_exp(_tmp1C2,_tmp1C1,p->loc);case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B8)->f3 == 0){_LL118: _tmp1C4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_tmp1C3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B8)->f2;_LL119: {
# 1040
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1C4,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1C3);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL11C;}case 17: _LL11A: _tmp1C5=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B8)->f1;_LL11B:
 return _tmp1C5;default: _LL11C: _LL11D:
# 1045
({void*_tmp1B9=0;Cyc_Warn_err(p->loc,({const char*_tmp1BA="cannot mix patterns and expressions in case";_tag_dyneither(_tmp1BA,sizeof(char),44);}),_tag_dyneither(_tmp1B9,sizeof(void*),0));});
return Cyc_Absyn_null_exp(p->loc);}_LL109:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1C7[7]="cnst_t";
# 1102 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1C7,_tmp1C7,_tmp1C7 + 7}};
# 1104
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1C8=yy1;union Cyc_Absyn_Cnst _tmp1C9;if((_tmp1C8.Int_tok).tag == 1){_LL11F: _tmp1C9=(_tmp1C8.Int_tok).val;_LL120:
# 1108
 yyzzz=_tmp1C9;
goto _LL11E;}else{_LL121: _LL122:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL11E:;}
# 1112
return yyzzz;}
# 1114
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp1CA;(_tmp1CA.Int_tok).val=yy1;(_tmp1CA.Int_tok).tag=1;_tmp1CA;});}static char _tmp1CC[5]="char";
# 1103 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1CC,_tmp1CC,_tmp1CC + 5}};
# 1105
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1CD=yy1;char _tmp1CE;if((_tmp1CD.Char_tok).tag == 2){_LL124: _tmp1CE=(_tmp1CD.Char_tok).val;_LL125:
# 1109
 yyzzz=_tmp1CE;
goto _LL123;}else{_LL126: _LL127:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL123:;}
# 1113
return yyzzz;}
# 1115
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp1CF;(_tmp1CF.Char_tok).val=yy1;(_tmp1CF.Char_tok).tag=2;_tmp1CF;});}static char _tmp1D1[13]="string_t<`H>";
# 1104 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1D1,_tmp1D1,_tmp1D1 + 13}};
# 1106
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1D2=yy1;struct _dyneither_ptr _tmp1D3;if((_tmp1D2.String_tok).tag == 3){_LL129: _tmp1D3=(_tmp1D2.String_tok).val;_LL12A:
# 1110
 yyzzz=_tmp1D3;
goto _LL128;}else{_LL12B: _LL12C:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL128:;}
# 1114
return yyzzz;}
# 1116
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp1D4;(_tmp1D4.String_tok).val=yy1;(_tmp1D4.String_tok).tag=3;_tmp1D4;});}static char _tmp1D6[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1D6,_tmp1D6,_tmp1D6 + 56}};
# 1109
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1D7=yy1;struct _tuple21*_tmp1D8;if((_tmp1D7.YY1).tag == 9){_LL12E: _tmp1D8=(_tmp1D7.YY1).val;_LL12F:
# 1113
 yyzzz=_tmp1D8;
goto _LL12D;}else{_LL130: _LL131:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL12D:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp1D9;(_tmp1D9.YY1).val=yy1;(_tmp1D9.YY1).tag=9;_tmp1D9;});}static char _tmp1DB[19]="conref_t<bounds_t>";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 19}};
# 1110
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1DC=yy1;union Cyc_Absyn_Constraint*_tmp1DD;if((_tmp1DC.YY2).tag == 10){_LL133: _tmp1DD=(_tmp1DC.YY2).val;_LL134:
# 1114
 yyzzz=_tmp1DD;
goto _LL132;}else{_LL135: _LL136:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL132:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp1DE;(_tmp1DE.YY2).val=yy1;(_tmp1DE.YY2).tag=10;_tmp1DE;});}static char _tmp1E0[28]="list_t<offsetof_field_t,`H>";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 28}};
# 1111
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E1=yy1;struct Cyc_List_List*_tmp1E2;if((_tmp1E1.YY3).tag == 11){_LL138: _tmp1E2=(_tmp1E1.YY3).val;_LL139:
# 1115
 yyzzz=_tmp1E2;
goto _LL137;}else{_LL13A: _LL13B:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL137:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1E3;(_tmp1E3.YY3).val=yy1;(_tmp1E3.YY3).tag=11;_tmp1E3;});}static char _tmp1E5[6]="exp_t";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1E5,_tmp1E5,_tmp1E5 + 6}};
# 1112
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1E6=yy1;struct Cyc_Absyn_Exp*_tmp1E7;if((_tmp1E6.Exp_tok).tag == 7){_LL13D: _tmp1E7=(_tmp1E6.Exp_tok).val;_LL13E:
# 1116
 yyzzz=_tmp1E7;
goto _LL13C;}else{_LL13F: _LL140:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL13C:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp1E8;(_tmp1E8.Exp_tok).val=yy1;(_tmp1E8.Exp_tok).tag=7;_tmp1E8;});}static char _tmp1EA[17]="list_t<exp_t,`H>";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 17}};
# 1120
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EB=yy1;struct Cyc_List_List*_tmp1EC;if((_tmp1EB.YY4).tag == 12){_LL142: _tmp1EC=(_tmp1EB.YY4).val;_LL143:
# 1124
 yyzzz=_tmp1EC;
goto _LL141;}else{_LL144: _LL145:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL141:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1ED;(_tmp1ED.YY4).val=yy1;(_tmp1ED.YY4).tag=12;_tmp1ED;});}static char _tmp1EF[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 47}};
# 1121
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F0=yy1;struct Cyc_List_List*_tmp1F1;if((_tmp1F0.YY5).tag == 13){_LL147: _tmp1F1=(_tmp1F0.YY5).val;_LL148:
# 1125
 yyzzz=_tmp1F1;
goto _LL146;}else{_LL149: _LL14A:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL146:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1F2;(_tmp1F2.YY5).val=yy1;(_tmp1F2.YY5).tag=13;_tmp1F2;});}static char _tmp1F4[9]="primop_t";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F4,_tmp1F4,_tmp1F4 + 9}};
# 1122
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1F5=yy1;enum Cyc_Absyn_Primop _tmp1F6;if((_tmp1F5.YY6).tag == 14){_LL14C: _tmp1F6=(_tmp1F5.YY6).val;_LL14D:
# 1126
 yyzzz=_tmp1F6;
goto _LL14B;}else{_LL14E: _LL14F:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL14B:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp1F7;(_tmp1F7.YY6).val=yy1;(_tmp1F7.YY6).tag=14;_tmp1F7;});}static char _tmp1F9[19]="opt_t<primop_t,`H>";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1F9,_tmp1F9,_tmp1F9 + 19}};
# 1123
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1FA=yy1;struct Cyc_Core_Opt*_tmp1FB;if((_tmp1FA.YY7).tag == 15){_LL151: _tmp1FB=(_tmp1FA.YY7).val;_LL152:
# 1127
 yyzzz=_tmp1FB;
goto _LL150;}else{_LL153: _LL154:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL150:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp1FC;(_tmp1FC.YY7).val=yy1;(_tmp1FC.YY7).tag=15;_tmp1FC;});}static char _tmp1FE[7]="qvar_t";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 7}};
# 1124
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1FF=yy1;struct _tuple0*_tmp200;if((_tmp1FF.QualId_tok).tag == 5){_LL156: _tmp200=(_tmp1FF.QualId_tok).val;_LL157:
# 1128
 yyzzz=_tmp200;
goto _LL155;}else{_LL158: _LL159:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL155:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp201;(_tmp201.QualId_tok).val=yy1;(_tmp201.QualId_tok).tag=5;_tmp201;});}static char _tmp203[7]="stmt_t";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp203,_tmp203,_tmp203 + 7}};
# 1127
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp204=yy1;struct Cyc_Absyn_Stmt*_tmp205;if((_tmp204.Stmt_tok).tag == 8){_LL15B: _tmp205=(_tmp204.Stmt_tok).val;_LL15C:
# 1131
 yyzzz=_tmp205;
goto _LL15A;}else{_LL15D: _LL15E:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL15A:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp206;(_tmp206.Stmt_tok).val=yy1;(_tmp206.Stmt_tok).tag=8;_tmp206;});}static char _tmp208[27]="list_t<switch_clause_t,`H>";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 27}};
# 1131
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp209=yy1;struct Cyc_List_List*_tmp20A;if((_tmp209.YY8).tag == 16){_LL160: _tmp20A=(_tmp209.YY8).val;_LL161:
# 1135
 yyzzz=_tmp20A;
goto _LL15F;}else{_LL162: _LL163:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL15F:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp20B;(_tmp20B.YY8).val=yy1;(_tmp20B.YY8).tag=16;_tmp20B;});}static char _tmp20D[6]="pat_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 6}};
# 1132
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp20E=yy1;struct Cyc_Absyn_Pat*_tmp20F;if((_tmp20E.YY9).tag == 17){_LL165: _tmp20F=(_tmp20E.YY9).val;_LL166:
# 1136
 yyzzz=_tmp20F;
goto _LL164;}else{_LL167: _LL168:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL164:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp210;(_tmp210.YY9).val=yy1;(_tmp210.YY9).tag=17;_tmp210;});}static char _tmp212[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp212,_tmp212,_tmp212 + 28}};
# 1137
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp213=yy1;struct _tuple22*_tmp214;if((_tmp213.YY10).tag == 18){_LL16A: _tmp214=(_tmp213.YY10).val;_LL16B:
# 1141
 yyzzz=_tmp214;
goto _LL169;}else{_LL16C: _LL16D:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL169:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp215;(_tmp215.YY10).val=yy1;(_tmp215.YY10).tag=18;_tmp215;});}static char _tmp217[17]="list_t<pat_t,`H>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 17}};
# 1138
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp218=yy1;struct Cyc_List_List*_tmp219;if((_tmp218.YY11).tag == 19){_LL16F: _tmp219=(_tmp218.YY11).val;_LL170:
# 1142
 yyzzz=_tmp219;
goto _LL16E;}else{_LL171: _LL172:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL16E:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp21A;(_tmp21A.YY11).val=yy1;(_tmp21A.YY11).tag=19;_tmp21A;});}static char _tmp21C[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 36}};
# 1139
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp21D=yy1;struct _tuple23*_tmp21E;if((_tmp21D.YY12).tag == 20){_LL174: _tmp21E=(_tmp21D.YY12).val;_LL175:
# 1143
 yyzzz=_tmp21E;
goto _LL173;}else{_LL176: _LL177:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL173:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp21F;(_tmp21F.YY12).val=yy1;(_tmp21F.YY12).tag=20;_tmp21F;});}static char _tmp221[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp221,_tmp221,_tmp221 + 47}};
# 1140
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp222=yy1;struct Cyc_List_List*_tmp223;if((_tmp222.YY13).tag == 21){_LL179: _tmp223=(_tmp222.YY13).val;_LL17A:
# 1144
 yyzzz=_tmp223;
goto _LL178;}else{_LL17B: _LL17C:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL178:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp224;(_tmp224.YY13).val=yy1;(_tmp224.YY13).tag=21;_tmp224;});}static char _tmp226[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp226,_tmp226,_tmp226 + 58}};
# 1141
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp227=yy1;struct _tuple22*_tmp228;if((_tmp227.YY14).tag == 22){_LL17E: _tmp228=(_tmp227.YY14).val;_LL17F:
# 1145
 yyzzz=_tmp228;
goto _LL17D;}else{_LL180: _LL181:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL17D:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp229;(_tmp229.YY14).val=yy1;(_tmp229.YY14).tag=22;_tmp229;});}static char _tmp22B[9]="fndecl_t";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp22B,_tmp22B,_tmp22B + 9}};
# 1142
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp22C=yy1;struct Cyc_Absyn_Fndecl*_tmp22D;if((_tmp22C.YY15).tag == 23){_LL183: _tmp22D=(_tmp22C.YY15).val;_LL184:
# 1146
 yyzzz=_tmp22D;
goto _LL182;}else{_LL185: _LL186:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL182:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp22E;(_tmp22E.YY15).val=yy1;(_tmp22E.YY15).tag=23;_tmp22E;});}static char _tmp230[18]="list_t<decl_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp230,_tmp230,_tmp230 + 18}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp231=yy1;struct Cyc_List_List*_tmp232;if((_tmp231.YY16).tag == 24){_LL188: _tmp232=(_tmp231.YY16).val;_LL189:
# 1147
 yyzzz=_tmp232;
goto _LL187;}else{_LL18A: _LL18B:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL187:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp233;(_tmp233.YY16).val=yy1;(_tmp233.YY16).tag=24;_tmp233;});}static char _tmp235[12]="decl_spec_t";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp235,_tmp235,_tmp235 + 12}};
# 1145
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp236=yy1;struct Cyc_Parse_Declaration_spec _tmp237;if((_tmp236.YY17).tag == 25){_LL18D: _tmp237=(_tmp236.YY17).val;_LL18E:
# 1149
 yyzzz=_tmp237;
goto _LL18C;}else{_LL18F: _LL190:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL18C:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp238;(_tmp238.YY17).val=yy1;(_tmp238.YY17).tag=25;_tmp238;});}static char _tmp23A[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp23A,_tmp23A,_tmp23A + 31}};
# 1146
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp23B=yy1;struct _tuple11 _tmp23C;if((_tmp23B.YY18).tag == 26){_LL192: _tmp23C=(_tmp23B.YY18).val;_LL193:
# 1150
 yyzzz=_tmp23C;
goto _LL191;}else{_LL194: _LL195:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL191:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp23D;(_tmp23D.YY18).val=yy1;(_tmp23D.YY18).tag=26;_tmp23D;});}static char _tmp23F[23]="declarator_list_t<`yy>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 23}};
# 1147
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp240=yy1;struct _tuple12*_tmp241;if((_tmp240.YY19).tag == 27){_LL197: _tmp241=(_tmp240.YY19).val;_LL198:
# 1151
 yyzzz=_tmp241;
goto _LL196;}else{_LL199: _LL19A:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL196:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp242;(_tmp242.YY19).val=yy1;(_tmp242.YY19).tag=27;_tmp242;});}static char _tmp244[19]="storage_class_t@`H";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp244,_tmp244,_tmp244 + 19}};
# 1148
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp245=yy1;enum Cyc_Parse_Storage_class*_tmp246;if((_tmp245.YY20).tag == 28){_LL19C: _tmp246=(_tmp245.YY20).val;_LL19D:
# 1152
 yyzzz=_tmp246;
goto _LL19B;}else{_LL19E: _LL19F:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL19B:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp247;(_tmp247.YY20).val=yy1;(_tmp247.YY20).tag=28;_tmp247;});}static char _tmp249[17]="type_specifier_t";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp249,_tmp249,_tmp249 + 17}};
# 1149
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp24A=yy1;struct Cyc_Parse_Type_specifier _tmp24B;if((_tmp24A.YY21).tag == 29){_LL1A1: _tmp24B=(_tmp24A.YY21).val;_LL1A2:
# 1153
 yyzzz=_tmp24B;
goto _LL1A0;}else{_LL1A3: _LL1A4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL1A0:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp24C;(_tmp24C.YY21).val=yy1;(_tmp24C.YY21).tag=29;_tmp24C;});}static char _tmp24E[12]="aggr_kind_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp24E,_tmp24E,_tmp24E + 12}};
# 1151
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp24F=yy1;enum Cyc_Absyn_AggrKind _tmp250;if((_tmp24F.YY22).tag == 30){_LL1A6: _tmp250=(_tmp24F.YY22).val;_LL1A7:
# 1155
 yyzzz=_tmp250;
goto _LL1A5;}else{_LL1A8: _LL1A9:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL1A5:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp251;(_tmp251.YY22).val=yy1;(_tmp251.YY22).tag=30;_tmp251;});}static char _tmp253[8]="tqual_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 8}};
# 1152
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp254=yy1;struct Cyc_Absyn_Tqual _tmp255;if((_tmp254.YY23).tag == 31){_LL1AB: _tmp255=(_tmp254.YY23).val;_LL1AC:
# 1156
 yyzzz=_tmp255;
goto _LL1AA;}else{_LL1AD: _LL1AE:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL1AA:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp256;(_tmp256.YY23).val=yy1;(_tmp256.YY23).tag=31;_tmp256;});}static char _tmp258[23]="list_t<aggrfield_t,`H>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp258,_tmp258,_tmp258 + 23}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp259=yy1;struct Cyc_List_List*_tmp25A;if((_tmp259.YY24).tag == 32){_LL1B0: _tmp25A=(_tmp259.YY24).val;_LL1B1:
# 1157
 yyzzz=_tmp25A;
goto _LL1AF;}else{_LL1B2: _LL1B3:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL1AF:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp25B;(_tmp25B.YY24).val=yy1;(_tmp25B.YY24).tag=32;_tmp25B;});}static char _tmp25D[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp25D,_tmp25D,_tmp25D + 34}};
# 1154
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25E=yy1;struct Cyc_List_List*_tmp25F;if((_tmp25E.YY25).tag == 33){_LL1B5: _tmp25F=(_tmp25E.YY25).val;_LL1B6:
# 1158
 yyzzz=_tmp25F;
goto _LL1B4;}else{_LL1B7: _LL1B8:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL1B4:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp260;(_tmp260.YY25).val=yy1;(_tmp260.YY25).tag=33;_tmp260;});}static char _tmp262[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp262,_tmp262,_tmp262 + 33}};
# 1155
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp263=yy1;struct Cyc_List_List*_tmp264;if((_tmp263.YY26).tag == 34){_LL1BA: _tmp264=(_tmp263.YY26).val;_LL1BB:
# 1159
 yyzzz=_tmp264;
goto _LL1B9;}else{_LL1BC: _LL1BD:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL1B9:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp265;(_tmp265.YY26).val=yy1;(_tmp265.YY26).tag=34;_tmp265;});}static char _tmp267[18]="declarator_t<`yy>";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 18}};
# 1156
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp268=yy1;struct Cyc_Parse_Declarator _tmp269;if((_tmp268.YY27).tag == 35){_LL1BF: _tmp269=(_tmp268.YY27).val;_LL1C0:
# 1160
 yyzzz=_tmp269;
goto _LL1BE;}else{_LL1C1: _LL1C2:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL1BE:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp26A;(_tmp26A.YY27).val=yy1;(_tmp26A.YY27).tag=35;_tmp26A;});}static char _tmp26C[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp26C,_tmp26C,_tmp26C + 45}};
# 1157
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp26D=yy1;struct _tuple24*_tmp26E;if((_tmp26D.YY28).tag == 36){_LL1C4: _tmp26E=(_tmp26D.YY28).val;_LL1C5:
# 1161
 yyzzz=_tmp26E;
goto _LL1C3;}else{_LL1C6: _LL1C7:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL1C3:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp26F;(_tmp26F.YY28).val=yy1;(_tmp26F.YY28).tag=36;_tmp26F;});}static char _tmp271[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp271,_tmp271,_tmp271 + 57}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp272=yy1;struct Cyc_List_List*_tmp273;if((_tmp272.YY29).tag == 37){_LL1C9: _tmp273=(_tmp272.YY29).val;_LL1CA:
# 1162
 yyzzz=_tmp273;
goto _LL1C8;}else{_LL1CB: _LL1CC:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL1C8:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp274;(_tmp274.YY29).val=yy1;(_tmp274.YY29).tag=37;_tmp274;});}static char _tmp276[26]="abstractdeclarator_t<`yy>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp276,_tmp276,_tmp276 + 26}};
# 1159
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp277=yy1;struct Cyc_Parse_Abstractdeclarator _tmp278;if((_tmp277.YY30).tag == 38){_LL1CE: _tmp278=(_tmp277.YY30).val;_LL1CF:
# 1163
 yyzzz=_tmp278;
goto _LL1CD;}else{_LL1D0: _LL1D1:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL1CD:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp279;(_tmp279.YY30).val=yy1;(_tmp279.YY30).tag=38;_tmp279;});}static char _tmp27B[5]="bool";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp27B,_tmp27B,_tmp27B + 5}};
# 1160
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp27C=yy1;int _tmp27D;if((_tmp27C.YY31).tag == 39){_LL1D3: _tmp27D=(_tmp27C.YY31).val;_LL1D4:
# 1164
 yyzzz=_tmp27D;
goto _LL1D2;}else{_LL1D5: _LL1D6:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL1D2:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp27E;(_tmp27E.YY31).val=yy1;(_tmp27E.YY31).tag=39;_tmp27E;});}static char _tmp280[8]="scope_t";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp280,_tmp280,_tmp280 + 8}};
# 1161
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp281=yy1;enum Cyc_Absyn_Scope _tmp282;if((_tmp281.YY32).tag == 40){_LL1D8: _tmp282=(_tmp281.YY32).val;_LL1D9:
# 1165
 yyzzz=_tmp282;
goto _LL1D7;}else{_LL1DA: _LL1DB:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL1D7:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp283;(_tmp283.YY32).val=yy1;(_tmp283.YY32).tag=40;_tmp283;});}static char _tmp285[16]="datatypefield_t";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp285,_tmp285,_tmp285 + 16}};
# 1162
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp286=yy1;struct Cyc_Absyn_Datatypefield*_tmp287;if((_tmp286.YY33).tag == 41){_LL1DD: _tmp287=(_tmp286.YY33).val;_LL1DE:
# 1166
 yyzzz=_tmp287;
goto _LL1DC;}else{_LL1DF: _LL1E0:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL1DC:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp288;(_tmp288.YY33).val=yy1;(_tmp288.YY33).tag=41;_tmp288;});}static char _tmp28A[27]="list_t<datatypefield_t,`H>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp28A,_tmp28A,_tmp28A + 27}};
# 1163
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp28B=yy1;struct Cyc_List_List*_tmp28C;if((_tmp28B.YY34).tag == 42){_LL1E2: _tmp28C=(_tmp28B.YY34).val;_LL1E3:
# 1167
 yyzzz=_tmp28C;
goto _LL1E1;}else{_LL1E4: _LL1E5:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL1E1:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp28D;(_tmp28D.YY34).val=yy1;(_tmp28D.YY34).tag=42;_tmp28D;});}static char _tmp28F[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp28F,_tmp28F,_tmp28F + 41}};
# 1164
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp290=yy1;struct _tuple25 _tmp291;if((_tmp290.YY35).tag == 43){_LL1E7: _tmp291=(_tmp290.YY35).val;_LL1E8:
# 1168
 yyzzz=_tmp291;
goto _LL1E6;}else{_LL1E9: _LL1EA:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL1E6:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp292;(_tmp292.YY35).val=yy1;(_tmp292.YY35).tag=43;_tmp292;});}static char _tmp294[17]="list_t<var_t,`H>";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp294,_tmp294,_tmp294 + 17}};
# 1165
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp295=yy1;struct Cyc_List_List*_tmp296;if((_tmp295.YY36).tag == 44){_LL1EC: _tmp296=(_tmp295.YY36).val;_LL1ED:
# 1169
 yyzzz=_tmp296;
goto _LL1EB;}else{_LL1EE: _LL1EF:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL1EB:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp297;(_tmp297.YY36).val=yy1;(_tmp297.YY36).tag=44;_tmp297;});}static char _tmp299[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp299,_tmp299,_tmp299 + 31}};
# 1166
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp29A=yy1;struct _tuple8*_tmp29B;if((_tmp29A.YY37).tag == 45){_LL1F1: _tmp29B=(_tmp29A.YY37).val;_LL1F2:
# 1170
 yyzzz=_tmp29B;
goto _LL1F0;}else{_LL1F3: _LL1F4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL1F0:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp29C;(_tmp29C.YY37).val=yy1;(_tmp29C.YY37).tag=45;_tmp29C;});}static char _tmp29E[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp29E,_tmp29E,_tmp29E + 42}};
# 1167
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29F=yy1;struct Cyc_List_List*_tmp2A0;if((_tmp29F.YY38).tag == 46){_LL1F6: _tmp2A0=(_tmp29F.YY38).val;_LL1F7:
# 1171
 yyzzz=_tmp2A0;
goto _LL1F5;}else{_LL1F8: _LL1F9:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL1F5:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2A1;(_tmp2A1.YY38).val=yy1;(_tmp2A1.YY38).tag=46;_tmp2A1;});}static char _tmp2A3[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A3,_tmp2A3,_tmp2A3 + 115}};
# 1168
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2A4=yy1;struct _tuple26*_tmp2A5;if((_tmp2A4.YY39).tag == 47){_LL1FB: _tmp2A5=(_tmp2A4.YY39).val;_LL1FC:
# 1172
 yyzzz=_tmp2A5;
goto _LL1FA;}else{_LL1FD: _LL1FE:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL1FA:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp2A6;(_tmp2A6.YY39).val=yy1;(_tmp2A6.YY39).tag=47;_tmp2A6;});}static char _tmp2A8[18]="list_t<type_t,`H>";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2A8,_tmp2A8,_tmp2A8 + 18}};
# 1169
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A9=yy1;struct Cyc_List_List*_tmp2AA;if((_tmp2A9.YY40).tag == 48){_LL200: _tmp2AA=(_tmp2A9.YY40).val;_LL201:
# 1173
 yyzzz=_tmp2AA;
goto _LL1FF;}else{_LL202: _LL203:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL1FF:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2AB;(_tmp2AB.YY40).val=yy1;(_tmp2AB.YY40).tag=48;_tmp2AB;});}static char _tmp2AD[24]="list_t<designator_t,`H>";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2AD,_tmp2AD,_tmp2AD + 24}};
# 1171
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AE=yy1;struct Cyc_List_List*_tmp2AF;if((_tmp2AE.YY41).tag == 49){_LL205: _tmp2AF=(_tmp2AE.YY41).val;_LL206:
# 1175
 yyzzz=_tmp2AF;
goto _LL204;}else{_LL207: _LL208:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL204:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2B0;(_tmp2B0.YY41).val=yy1;(_tmp2B0.YY41).tag=49;_tmp2B0;});}static char _tmp2B2[13]="designator_t";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B2,_tmp2B2,_tmp2B2 + 13}};
# 1172
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2B3=yy1;void*_tmp2B4;if((_tmp2B3.YY42).tag == 50){_LL20A: _tmp2B4=(_tmp2B3.YY42).val;_LL20B:
# 1176
 yyzzz=_tmp2B4;
goto _LL209;}else{_LL20C: _LL20D:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL209:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp2B5;(_tmp2B5.YY42).val=yy1;(_tmp2B5.YY42).tag=50;_tmp2B5;});}static char _tmp2B7[7]="kind_t";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2B7,_tmp2B7,_tmp2B7 + 7}};
# 1173
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2B8=yy1;struct Cyc_Absyn_Kind*_tmp2B9;if((_tmp2B8.YY43).tag == 51){_LL20F: _tmp2B9=(_tmp2B8.YY43).val;_LL210:
# 1177
 yyzzz=_tmp2B9;
goto _LL20E;}else{_LL211: _LL212:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL20E:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp2BA;(_tmp2BA.YY43).val=yy1;(_tmp2BA.YY43).tag=51;_tmp2BA;});}static char _tmp2BC[7]="type_t";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2BC,_tmp2BC,_tmp2BC + 7}};
# 1174
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2BD=yy1;void*_tmp2BE;if((_tmp2BD.YY44).tag == 52){_LL214: _tmp2BE=(_tmp2BD.YY44).val;_LL215:
# 1178
 yyzzz=_tmp2BE;
goto _LL213;}else{_LL216: _LL217:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL213:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp2BF;(_tmp2BF.YY44).val=yy1;(_tmp2BF.YY44).tag=52;_tmp2BF;});}static char _tmp2C1[23]="list_t<attribute_t,`H>";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2C1,_tmp2C1,_tmp2C1 + 23}};
# 1175
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C2=yy1;struct Cyc_List_List*_tmp2C3;if((_tmp2C2.YY45).tag == 53){_LL219: _tmp2C3=(_tmp2C2.YY45).val;_LL21A:
# 1179
 yyzzz=_tmp2C3;
goto _LL218;}else{_LL21B: _LL21C:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL218:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2C4;(_tmp2C4.YY45).val=yy1;(_tmp2C4.YY45).tag=53;_tmp2C4;});}static char _tmp2C6[12]="attribute_t";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2C6,_tmp2C6,_tmp2C6 + 12}};
# 1176
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C7=yy1;void*_tmp2C8;if((_tmp2C7.YY46).tag == 54){_LL21E: _tmp2C8=(_tmp2C7.YY46).val;_LL21F:
# 1180
 yyzzz=_tmp2C8;
goto _LL21D;}else{_LL220: _LL221:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL21D:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp2C9;(_tmp2C9.YY46).val=yy1;(_tmp2C9.YY46).tag=54;_tmp2C9;});}static char _tmp2CB[12]="enumfield_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2CB,_tmp2CB,_tmp2CB + 12}};
# 1177
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2CC=yy1;struct Cyc_Absyn_Enumfield*_tmp2CD;if((_tmp2CC.YY47).tag == 55){_LL223: _tmp2CD=(_tmp2CC.YY47).val;_LL224:
# 1181
 yyzzz=_tmp2CD;
goto _LL222;}else{_LL225: _LL226:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL222:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp2CE;(_tmp2CE.YY47).val=yy1;(_tmp2CE.YY47).tag=55;_tmp2CE;});}static char _tmp2D0[23]="list_t<enumfield_t,`H>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2D0,_tmp2D0,_tmp2D0 + 23}};
# 1178
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D1=yy1;struct Cyc_List_List*_tmp2D2;if((_tmp2D1.YY48).tag == 56){_LL228: _tmp2D2=(_tmp2D1.YY48).val;_LL229:
# 1182
 yyzzz=_tmp2D2;
goto _LL227;}else{_LL22A: _LL22B:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL227:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2D3;(_tmp2D3.YY48).val=yy1;(_tmp2D3.YY48).tag=56;_tmp2D3;});}static char _tmp2D5[11]="type_opt_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2D5,_tmp2D5,_tmp2D5 + 11}};
# 1179
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D6=yy1;void*_tmp2D7;if((_tmp2D6.YY49).tag == 57){_LL22D: _tmp2D7=(_tmp2D6.YY49).val;_LL22E:
# 1183
 yyzzz=_tmp2D7;
goto _LL22C;}else{_LL22F: _LL230:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL22C:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp2D8;(_tmp2D8.YY49).val=yy1;(_tmp2D8.YY49).tag=57;_tmp2D8;});}static char _tmp2DA[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2DA,_tmp2DA,_tmp2DA + 31}};
# 1180
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2DB=yy1;struct Cyc_List_List*_tmp2DC;if((_tmp2DB.YY50).tag == 58){_LL232: _tmp2DC=(_tmp2DB.YY50).val;_LL233:
# 1184
 yyzzz=_tmp2DC;
goto _LL231;}else{_LL234: _LL235:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL231:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2DD;(_tmp2DD.YY50).val=yy1;(_tmp2DD.YY50).tag=58;_tmp2DD;});}static char _tmp2DF[15]="conref_t<bool>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2DF,_tmp2DF,_tmp2DF + 15}};
# 1181
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2E0=yy1;union Cyc_Absyn_Constraint*_tmp2E1;if((_tmp2E0.YY51).tag == 59){_LL237: _tmp2E1=(_tmp2E0.YY51).val;_LL238:
# 1185
 yyzzz=_tmp2E1;
goto _LL236;}else{_LL239: _LL23A:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL236:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp2E2;(_tmp2E2.YY51).val=yy1;(_tmp2E2.YY51).tag=59;_tmp2E2;});}static char _tmp2E4[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E4,_tmp2E4,_tmp2E4 + 45}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E5=yy1;struct Cyc_List_List*_tmp2E6;if((_tmp2E5.YY52).tag == 60){_LL23C: _tmp2E6=(_tmp2E5.YY52).val;_LL23D:
# 1186
 yyzzz=_tmp2E6;
goto _LL23B;}else{_LL23E: _LL23F:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL23B:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2E7;(_tmp2E7.YY52).val=yy1;(_tmp2E7.YY52).tag=60;_tmp2E7;});}static char _tmp2E9[20]="pointer_qual_t<`yy>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2E9,_tmp2E9,_tmp2E9 + 20}};
# 1183
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2EA=yy1;void*_tmp2EB;if((_tmp2EA.YY53).tag == 61){_LL241: _tmp2EB=(_tmp2EA.YY53).val;_LL242:
# 1187
 yyzzz=_tmp2EB;
goto _LL240;}else{_LL243: _LL244:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL240:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp2EC;(_tmp2EC.YY53).val=yy1;(_tmp2EC.YY53).tag=61;_tmp2EC;});}static char _tmp2EE[21]="pointer_quals_t<`yy>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2EE,_tmp2EE,_tmp2EE + 21}};
# 1184
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2EF=yy1;struct Cyc_List_List*_tmp2F0;if((_tmp2EF.YY54).tag == 62){_LL246: _tmp2F0=(_tmp2EF.YY54).val;_LL247:
# 1188
 yyzzz=_tmp2F0;
goto _LL245;}else{_LL248: _LL249:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL245:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2F1;(_tmp2F1.YY54).val=yy1;(_tmp2F1.YY54).tag=62;_tmp2F1;});}static char _tmp2F3[21]="$(bool,string_t<`H>)";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2F3,_tmp2F3,_tmp2F3 + 21}};
# 1185
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp2F4=yy1;struct _tuple20 _tmp2F5;if((_tmp2F4.Asm_tok).tag == 6){_LL24B: _tmp2F5=(_tmp2F4.Asm_tok).val;_LL24C:
# 1189
 yyzzz=_tmp2F5;
goto _LL24A;}else{_LL24D: _LL24E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL24A:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp2F6;(_tmp2F6.Asm_tok).val=yy1;(_tmp2F6.Asm_tok).tag=6;_tmp2F6;});}static char _tmp2F8[10]="exp_opt_t";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2F8,_tmp2F8,_tmp2F8 + 10}};
# 1186
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp2F9=yy1;struct Cyc_Absyn_Exp*_tmp2FA;if((_tmp2F9.YY55).tag == 63){_LL250: _tmp2FA=(_tmp2F9.YY55).val;_LL251:
# 1190
 yyzzz=_tmp2FA;
goto _LL24F;}else{_LL252: _LL253:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL24F:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp2FB;(_tmp2FB.YY55).val=yy1;(_tmp2FB.YY55).tag=63;_tmp2FB;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1208
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1212
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp2FC;_tmp2FC.timestamp=0;_tmp2FC.first_line=0;_tmp2FC.first_column=0;_tmp2FC.last_line=0;_tmp2FC.last_column=0;_tmp2FC;});}
# 1215
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1226 "parse.y"
static short Cyc_yytranslate[376]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,2,2,131,145,142,2,128,129,136,139,124,143,133,144,2,2,2,2,2,2,2,2,2,2,132,121,126,125,127,138,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,134,2,135,141,130,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,122,140,123,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};static char _tmp2FD[2]="$";static char _tmp2FE[6]="error";static char _tmp2FF[12]="$undefined.";static char _tmp300[5]="AUTO";static char _tmp301[9]="REGISTER";static char _tmp302[7]="STATIC";static char _tmp303[7]="EXTERN";static char _tmp304[8]="TYPEDEF";static char _tmp305[5]="VOID";static char _tmp306[5]="CHAR";static char _tmp307[6]="SHORT";static char _tmp308[4]="INT";static char _tmp309[5]="LONG";static char _tmp30A[6]="FLOAT";static char _tmp30B[7]="DOUBLE";static char _tmp30C[7]="SIGNED";static char _tmp30D[9]="UNSIGNED";static char _tmp30E[6]="CONST";static char _tmp30F[9]="VOLATILE";static char _tmp310[9]="RESTRICT";static char _tmp311[7]="STRUCT";static char _tmp312[6]="UNION";static char _tmp313[5]="CASE";static char _tmp314[8]="DEFAULT";static char _tmp315[7]="INLINE";static char _tmp316[7]="SIZEOF";static char _tmp317[9]="OFFSETOF";static char _tmp318[3]="IF";static char _tmp319[5]="ELSE";static char _tmp31A[7]="SWITCH";static char _tmp31B[6]="WHILE";static char _tmp31C[3]="DO";static char _tmp31D[4]="FOR";static char _tmp31E[5]="GOTO";static char _tmp31F[9]="CONTINUE";static char _tmp320[6]="BREAK";static char _tmp321[7]="RETURN";static char _tmp322[5]="ENUM";static char _tmp323[7]="TYPEOF";static char _tmp324[16]="BUILTIN_VA_LIST";static char _tmp325[8]="NULL_kw";static char _tmp326[4]="LET";static char _tmp327[6]="THROW";static char _tmp328[4]="TRY";static char _tmp329[6]="CATCH";static char _tmp32A[7]="EXPORT";static char _tmp32B[4]="NEW";static char _tmp32C[9]="ABSTRACT";static char _tmp32D[9]="FALLTHRU";static char _tmp32E[6]="USING";static char _tmp32F[10]="NAMESPACE";static char _tmp330[9]="DATATYPE";static char _tmp331[7]="MALLOC";static char _tmp332[8]="RMALLOC";static char _tmp333[15]="RMALLOC_INLINE";static char _tmp334[7]="CALLOC";static char _tmp335[8]="RCALLOC";static char _tmp336[5]="SWAP";static char _tmp337[9]="REGION_T";static char _tmp338[6]="TAG_T";static char _tmp339[7]="REGION";static char _tmp33A[5]="RNEW";static char _tmp33B[8]="REGIONS";static char _tmp33C[13]="RESET_REGION";static char _tmp33D[7]="PORTON";static char _tmp33E[8]="PORTOFF";static char _tmp33F[12]="DYNREGION_T";static char _tmp340[8]="NUMELTS";static char _tmp341[8]="VALUEOF";static char _tmp342[10]="VALUEOF_T";static char _tmp343[9]="TAGCHECK";static char _tmp344[13]="NUMELTS_QUAL";static char _tmp345[10]="THIN_QUAL";static char _tmp346[9]="FAT_QUAL";static char _tmp347[13]="NOTNULL_QUAL";static char _tmp348[14]="NULLABLE_QUAL";static char _tmp349[14]="REQUIRES_QUAL";static char _tmp34A[13]="ENSURES_QUAL";static char _tmp34B[12]="REGION_QUAL";static char _tmp34C[16]="NOZEROTERM_QUAL";static char _tmp34D[14]="ZEROTERM_QUAL";static char _tmp34E[12]="TAGGED_QUAL";static char _tmp34F[16]="EXTENSIBLE_QUAL";static char _tmp350[15]="RESETABLE_QUAL";static char _tmp351[7]="PTR_OP";static char _tmp352[7]="INC_OP";static char _tmp353[7]="DEC_OP";static char _tmp354[8]="LEFT_OP";static char _tmp355[9]="RIGHT_OP";static char _tmp356[6]="LE_OP";static char _tmp357[6]="GE_OP";static char _tmp358[6]="EQ_OP";static char _tmp359[6]="NE_OP";static char _tmp35A[7]="AND_OP";static char _tmp35B[6]="OR_OP";static char _tmp35C[11]="MUL_ASSIGN";static char _tmp35D[11]="DIV_ASSIGN";static char _tmp35E[11]="MOD_ASSIGN";static char _tmp35F[11]="ADD_ASSIGN";static char _tmp360[11]="SUB_ASSIGN";static char _tmp361[12]="LEFT_ASSIGN";static char _tmp362[13]="RIGHT_ASSIGN";static char _tmp363[11]="AND_ASSIGN";static char _tmp364[11]="XOR_ASSIGN";static char _tmp365[10]="OR_ASSIGN";static char _tmp366[9]="ELLIPSIS";static char _tmp367[11]="LEFT_RIGHT";static char _tmp368[12]="COLON_COLON";static char _tmp369[11]="IDENTIFIER";static char _tmp36A[17]="INTEGER_CONSTANT";static char _tmp36B[7]="STRING";static char _tmp36C[8]="WSTRING";static char _tmp36D[19]="CHARACTER_CONSTANT";static char _tmp36E[20]="WCHARACTER_CONSTANT";static char _tmp36F[18]="FLOATING_CONSTANT";static char _tmp370[9]="TYPE_VAR";static char _tmp371[13]="TYPEDEF_NAME";static char _tmp372[16]="QUAL_IDENTIFIER";static char _tmp373[18]="QUAL_TYPEDEF_NAME";static char _tmp374[10]="ATTRIBUTE";static char _tmp375[4]="ASM";static char _tmp376[4]="';'";static char _tmp377[4]="'{'";static char _tmp378[4]="'}'";static char _tmp379[4]="','";static char _tmp37A[4]="'='";static char _tmp37B[4]="'<'";static char _tmp37C[4]="'>'";static char _tmp37D[4]="'('";static char _tmp37E[4]="')'";static char _tmp37F[4]="'_'";static char _tmp380[4]="'$'";static char _tmp381[4]="':'";static char _tmp382[4]="'.'";static char _tmp383[4]="'['";static char _tmp384[4]="']'";static char _tmp385[4]="'*'";static char _tmp386[4]="'@'";static char _tmp387[4]="'?'";static char _tmp388[4]="'+'";static char _tmp389[4]="'|'";static char _tmp38A[4]="'^'";static char _tmp38B[4]="'&'";static char _tmp38C[4]="'-'";static char _tmp38D[4]="'/'";static char _tmp38E[4]="'%'";static char _tmp38F[4]="'~'";static char _tmp390[4]="'!'";static char _tmp391[5]="prog";static char _tmp392[17]="translation_unit";static char _tmp393[16]="extern_c_action";static char _tmp394[13]="end_extern_c";static char _tmp395[12]="export_list";static char _tmp396[19]="export_list_values";static char _tmp397[21]="external_declaration";static char _tmp398[15]="optional_comma";static char _tmp399[20]="function_definition";static char _tmp39A[21]="function_definition2";static char _tmp39B[13]="using_action";static char _tmp39C[15]="unusing_action";static char _tmp39D[17]="namespace_action";static char _tmp39E[19]="unnamespace_action";static char _tmp39F[12]="declaration";static char _tmp3A0[19]="resetable_qual_opt";static char _tmp3A1[17]="declaration_list";static char _tmp3A2[23]="declaration_specifiers";static char _tmp3A3[24]="storage_class_specifier";static char _tmp3A4[15]="attributes_opt";static char _tmp3A5[11]="attributes";static char _tmp3A6[15]="attribute_list";static char _tmp3A7[10]="attribute";static char _tmp3A8[15]="type_specifier";static char _tmp3A9[25]="type_specifier_notypedef";static char _tmp3AA[5]="kind";static char _tmp3AB[15]="type_qualifier";static char _tmp3AC[15]="enum_specifier";static char _tmp3AD[11]="enum_field";static char _tmp3AE[22]="enum_declaration_list";static char _tmp3AF[26]="struct_or_union_specifier";static char _tmp3B0[16]="type_params_opt";static char _tmp3B1[16]="struct_or_union";static char _tmp3B2[24]="struct_declaration_list";static char _tmp3B3[25]="struct_declaration_list0";static char _tmp3B4[21]="init_declarator_list";static char _tmp3B5[22]="init_declarator_list0";static char _tmp3B6[16]="init_declarator";static char _tmp3B7[19]="struct_declaration";static char _tmp3B8[25]="specifier_qualifier_list";static char _tmp3B9[35]="notypedef_specifier_qualifier_list";static char _tmp3BA[23]="struct_declarator_list";static char _tmp3BB[24]="struct_declarator_list0";static char _tmp3BC[18]="struct_declarator";static char _tmp3BD[20]="requires_clause_opt";static char _tmp3BE[19]="ensures_clause_opt";static char _tmp3BF[19]="datatype_specifier";static char _tmp3C0[14]="qual_datatype";static char _tmp3C1[19]="datatypefield_list";static char _tmp3C2[20]="datatypefield_scope";static char _tmp3C3[14]="datatypefield";static char _tmp3C4[11]="declarator";static char _tmp3C5[23]="declarator_withtypedef";static char _tmp3C6[18]="direct_declarator";static char _tmp3C7[30]="direct_declarator_withtypedef";static char _tmp3C8[8]="pointer";static char _tmp3C9[12]="one_pointer";static char _tmp3CA[14]="pointer_quals";static char _tmp3CB[13]="pointer_qual";static char _tmp3CC[23]="pointer_null_and_bound";static char _tmp3CD[14]="pointer_bound";static char _tmp3CE[18]="zeroterm_qual_opt";static char _tmp3CF[8]="rgn_opt";static char _tmp3D0[11]="tqual_list";static char _tmp3D1[20]="parameter_type_list";static char _tmp3D2[9]="type_var";static char _tmp3D3[16]="optional_effect";static char _tmp3D4[19]="optional_rgn_order";static char _tmp3D5[10]="rgn_order";static char _tmp3D6[16]="optional_inject";static char _tmp3D7[11]="effect_set";static char _tmp3D8[14]="atomic_effect";static char _tmp3D9[11]="region_set";static char _tmp3DA[15]="parameter_list";static char _tmp3DB[22]="parameter_declaration";static char _tmp3DC[16]="identifier_list";static char _tmp3DD[17]="identifier_list0";static char _tmp3DE[12]="initializer";static char _tmp3DF[18]="array_initializer";static char _tmp3E0[17]="initializer_list";static char _tmp3E1[12]="designation";static char _tmp3E2[16]="designator_list";static char _tmp3E3[11]="designator";static char _tmp3E4[10]="type_name";static char _tmp3E5[14]="any_type_name";static char _tmp3E6[15]="type_name_list";static char _tmp3E7[20]="abstract_declarator";static char _tmp3E8[27]="direct_abstract_declarator";static char _tmp3E9[10]="statement";static char _tmp3EA[18]="labeled_statement";static char _tmp3EB[21]="expression_statement";static char _tmp3EC[19]="compound_statement";static char _tmp3ED[16]="block_item_list";static char _tmp3EE[20]="selection_statement";static char _tmp3EF[15]="switch_clauses";static char _tmp3F0[20]="iteration_statement";static char _tmp3F1[15]="jump_statement";static char _tmp3F2[12]="exp_pattern";static char _tmp3F3[20]="conditional_pattern";static char _tmp3F4[19]="logical_or_pattern";static char _tmp3F5[20]="logical_and_pattern";static char _tmp3F6[21]="inclusive_or_pattern";static char _tmp3F7[21]="exclusive_or_pattern";static char _tmp3F8[12]="and_pattern";static char _tmp3F9[17]="equality_pattern";static char _tmp3FA[19]="relational_pattern";static char _tmp3FB[14]="shift_pattern";static char _tmp3FC[17]="additive_pattern";static char _tmp3FD[23]="multiplicative_pattern";static char _tmp3FE[13]="cast_pattern";static char _tmp3FF[14]="unary_pattern";static char _tmp400[16]="postfix_pattern";static char _tmp401[16]="primary_pattern";static char _tmp402[8]="pattern";static char _tmp403[19]="tuple_pattern_list";static char _tmp404[20]="tuple_pattern_list0";static char _tmp405[14]="field_pattern";static char _tmp406[19]="field_pattern_list";static char _tmp407[20]="field_pattern_list0";static char _tmp408[11]="expression";static char _tmp409[22]="assignment_expression";static char _tmp40A[20]="assignment_operator";static char _tmp40B[23]="conditional_expression";static char _tmp40C[20]="constant_expression";static char _tmp40D[22]="logical_or_expression";static char _tmp40E[23]="logical_and_expression";static char _tmp40F[24]="inclusive_or_expression";static char _tmp410[24]="exclusive_or_expression";static char _tmp411[15]="and_expression";static char _tmp412[20]="equality_expression";static char _tmp413[22]="relational_expression";static char _tmp414[17]="shift_expression";static char _tmp415[20]="additive_expression";static char _tmp416[26]="multiplicative_expression";static char _tmp417[16]="cast_expression";static char _tmp418[17]="unary_expression";static char _tmp419[15]="unary_operator";static char _tmp41A[19]="postfix_expression";static char _tmp41B[17]="field_expression";static char _tmp41C[19]="primary_expression";static char _tmp41D[25]="argument_expression_list";static char _tmp41E[26]="argument_expression_list0";static char _tmp41F[9]="constant";static char _tmp420[20]="qual_opt_identifier";static char _tmp421[17]="qual_opt_typedef";static char _tmp422[18]="struct_union_name";static char _tmp423[11]="field_name";static char _tmp424[12]="right_angle";
# 1580 "parse.y"
static struct _dyneither_ptr Cyc_yytname[296]={{_tmp2FD,_tmp2FD,_tmp2FD + 2},{_tmp2FE,_tmp2FE,_tmp2FE + 6},{_tmp2FF,_tmp2FF,_tmp2FF + 12},{_tmp300,_tmp300,_tmp300 + 5},{_tmp301,_tmp301,_tmp301 + 9},{_tmp302,_tmp302,_tmp302 + 7},{_tmp303,_tmp303,_tmp303 + 7},{_tmp304,_tmp304,_tmp304 + 8},{_tmp305,_tmp305,_tmp305 + 5},{_tmp306,_tmp306,_tmp306 + 5},{_tmp307,_tmp307,_tmp307 + 6},{_tmp308,_tmp308,_tmp308 + 4},{_tmp309,_tmp309,_tmp309 + 5},{_tmp30A,_tmp30A,_tmp30A + 6},{_tmp30B,_tmp30B,_tmp30B + 7},{_tmp30C,_tmp30C,_tmp30C + 7},{_tmp30D,_tmp30D,_tmp30D + 9},{_tmp30E,_tmp30E,_tmp30E + 6},{_tmp30F,_tmp30F,_tmp30F + 9},{_tmp310,_tmp310,_tmp310 + 9},{_tmp311,_tmp311,_tmp311 + 7},{_tmp312,_tmp312,_tmp312 + 6},{_tmp313,_tmp313,_tmp313 + 5},{_tmp314,_tmp314,_tmp314 + 8},{_tmp315,_tmp315,_tmp315 + 7},{_tmp316,_tmp316,_tmp316 + 7},{_tmp317,_tmp317,_tmp317 + 9},{_tmp318,_tmp318,_tmp318 + 3},{_tmp319,_tmp319,_tmp319 + 5},{_tmp31A,_tmp31A,_tmp31A + 7},{_tmp31B,_tmp31B,_tmp31B + 6},{_tmp31C,_tmp31C,_tmp31C + 3},{_tmp31D,_tmp31D,_tmp31D + 4},{_tmp31E,_tmp31E,_tmp31E + 5},{_tmp31F,_tmp31F,_tmp31F + 9},{_tmp320,_tmp320,_tmp320 + 6},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 5},{_tmp323,_tmp323,_tmp323 + 7},{_tmp324,_tmp324,_tmp324 + 16},{_tmp325,_tmp325,_tmp325 + 8},{_tmp326,_tmp326,_tmp326 + 4},{_tmp327,_tmp327,_tmp327 + 6},{_tmp328,_tmp328,_tmp328 + 4},{_tmp329,_tmp329,_tmp329 + 6},{_tmp32A,_tmp32A,_tmp32A + 7},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 9},{_tmp32D,_tmp32D,_tmp32D + 9},{_tmp32E,_tmp32E,_tmp32E + 6},{_tmp32F,_tmp32F,_tmp32F + 10},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 7},{_tmp332,_tmp332,_tmp332 + 8},{_tmp333,_tmp333,_tmp333 + 15},{_tmp334,_tmp334,_tmp334 + 7},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 5},{_tmp337,_tmp337,_tmp337 + 9},{_tmp338,_tmp338,_tmp338 + 6},{_tmp339,_tmp339,_tmp339 + 7},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 13},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 8},{_tmp33F,_tmp33F,_tmp33F + 12},{_tmp340,_tmp340,_tmp340 + 8},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 10},{_tmp343,_tmp343,_tmp343 + 9},{_tmp344,_tmp344,_tmp344 + 13},{_tmp345,_tmp345,_tmp345 + 10},{_tmp346,_tmp346,_tmp346 + 9},{_tmp347,_tmp347,_tmp347 + 13},{_tmp348,_tmp348,_tmp348 + 14},{_tmp349,_tmp349,_tmp349 + 14},{_tmp34A,_tmp34A,_tmp34A + 13},{_tmp34B,_tmp34B,_tmp34B + 12},{_tmp34C,_tmp34C,_tmp34C + 16},{_tmp34D,_tmp34D,_tmp34D + 14},{_tmp34E,_tmp34E,_tmp34E + 12},{_tmp34F,_tmp34F,_tmp34F + 16},{_tmp350,_tmp350,_tmp350 + 15},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 7},{_tmp354,_tmp354,_tmp354 + 8},{_tmp355,_tmp355,_tmp355 + 9},{_tmp356,_tmp356,_tmp356 + 6},{_tmp357,_tmp357,_tmp357 + 6},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 7},{_tmp35B,_tmp35B,_tmp35B + 6},{_tmp35C,_tmp35C,_tmp35C + 11},{_tmp35D,_tmp35D,_tmp35D + 11},{_tmp35E,_tmp35E,_tmp35E + 11},{_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 11},{_tmp361,_tmp361,_tmp361 + 12},{_tmp362,_tmp362,_tmp362 + 13},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 11},{_tmp365,_tmp365,_tmp365 + 10},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 11},{_tmp368,_tmp368,_tmp368 + 12},{_tmp369,_tmp369,_tmp369 + 11},{_tmp36A,_tmp36A,_tmp36A + 17},{_tmp36B,_tmp36B,_tmp36B + 7},{_tmp36C,_tmp36C,_tmp36C + 8},{_tmp36D,_tmp36D,_tmp36D + 19},{_tmp36E,_tmp36E,_tmp36E + 20},{_tmp36F,_tmp36F,_tmp36F + 18},{_tmp370,_tmp370,_tmp370 + 9},{_tmp371,_tmp371,_tmp371 + 13},{_tmp372,_tmp372,_tmp372 + 16},{_tmp373,_tmp373,_tmp373 + 18},{_tmp374,_tmp374,_tmp374 + 10},{_tmp375,_tmp375,_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 5},{_tmp392,_tmp392,_tmp392 + 17},{_tmp393,_tmp393,_tmp393 + 16},{_tmp394,_tmp394,_tmp394 + 13},{_tmp395,_tmp395,_tmp395 + 12},{_tmp396,_tmp396,_tmp396 + 19},{_tmp397,_tmp397,_tmp397 + 21},{_tmp398,_tmp398,_tmp398 + 15},{_tmp399,_tmp399,_tmp399 + 20},{_tmp39A,_tmp39A,_tmp39A + 21},{_tmp39B,_tmp39B,_tmp39B + 13},{_tmp39C,_tmp39C,_tmp39C + 15},{_tmp39D,_tmp39D,_tmp39D + 17},{_tmp39E,_tmp39E,_tmp39E + 19},{_tmp39F,_tmp39F,_tmp39F + 12},{_tmp3A0,_tmp3A0,_tmp3A0 + 19},{_tmp3A1,_tmp3A1,_tmp3A1 + 17},{_tmp3A2,_tmp3A2,_tmp3A2 + 23},{_tmp3A3,_tmp3A3,_tmp3A3 + 24},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 11},{_tmp3A6,_tmp3A6,_tmp3A6 + 15},{_tmp3A7,_tmp3A7,_tmp3A7 + 10},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 25},{_tmp3AA,_tmp3AA,_tmp3AA + 5},{_tmp3AB,_tmp3AB,_tmp3AB + 15},{_tmp3AC,_tmp3AC,_tmp3AC + 15},{_tmp3AD,_tmp3AD,_tmp3AD + 11},{_tmp3AE,_tmp3AE,_tmp3AE + 22},{_tmp3AF,_tmp3AF,_tmp3AF + 26},{_tmp3B0,_tmp3B0,_tmp3B0 + 16},{_tmp3B1,_tmp3B1,_tmp3B1 + 16},{_tmp3B2,_tmp3B2,_tmp3B2 + 24},{_tmp3B3,_tmp3B3,_tmp3B3 + 25},{_tmp3B4,_tmp3B4,_tmp3B4 + 21},{_tmp3B5,_tmp3B5,_tmp3B5 + 22},{_tmp3B6,_tmp3B6,_tmp3B6 + 16},{_tmp3B7,_tmp3B7,_tmp3B7 + 19},{_tmp3B8,_tmp3B8,_tmp3B8 + 25},{_tmp3B9,_tmp3B9,_tmp3B9 + 35},{_tmp3BA,_tmp3BA,_tmp3BA + 23},{_tmp3BB,_tmp3BB,_tmp3BB + 24},{_tmp3BC,_tmp3BC,_tmp3BC + 18},{_tmp3BD,_tmp3BD,_tmp3BD + 20},{_tmp3BE,_tmp3BE,_tmp3BE + 19},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,_tmp3C0 + 14},{_tmp3C1,_tmp3C1,_tmp3C1 + 19},{_tmp3C2,_tmp3C2,_tmp3C2 + 20},{_tmp3C3,_tmp3C3,_tmp3C3 + 14},{_tmp3C4,_tmp3C4,_tmp3C4 + 11},{_tmp3C5,_tmp3C5,_tmp3C5 + 23},{_tmp3C6,_tmp3C6,_tmp3C6 + 18},{_tmp3C7,_tmp3C7,_tmp3C7 + 30},{_tmp3C8,_tmp3C8,_tmp3C8 + 8},{_tmp3C9,_tmp3C9,_tmp3C9 + 12},{_tmp3CA,_tmp3CA,_tmp3CA + 14},{_tmp3CB,_tmp3CB,_tmp3CB + 13},{_tmp3CC,_tmp3CC,_tmp3CC + 23},{_tmp3CD,_tmp3CD,_tmp3CD + 14},{_tmp3CE,_tmp3CE,_tmp3CE + 18},{_tmp3CF,_tmp3CF,_tmp3CF + 8},{_tmp3D0,_tmp3D0,_tmp3D0 + 11},{_tmp3D1,_tmp3D1,_tmp3D1 + 20},{_tmp3D2,_tmp3D2,_tmp3D2 + 9},{_tmp3D3,_tmp3D3,_tmp3D3 + 16},{_tmp3D4,_tmp3D4,_tmp3D4 + 19},{_tmp3D5,_tmp3D5,_tmp3D5 + 10},{_tmp3D6,_tmp3D6,_tmp3D6 + 16},{_tmp3D7,_tmp3D7,_tmp3D7 + 11},{_tmp3D8,_tmp3D8,_tmp3D8 + 14},{_tmp3D9,_tmp3D9,_tmp3D9 + 11},{_tmp3DA,_tmp3DA,_tmp3DA + 15},{_tmp3DB,_tmp3DB,_tmp3DB + 22},{_tmp3DC,_tmp3DC,_tmp3DC + 16},{_tmp3DD,_tmp3DD,_tmp3DD + 17},{_tmp3DE,_tmp3DE,_tmp3DE + 12},{_tmp3DF,_tmp3DF,_tmp3DF + 18},{_tmp3E0,_tmp3E0,_tmp3E0 + 17},{_tmp3E1,_tmp3E1,_tmp3E1 + 12},{_tmp3E2,_tmp3E2,_tmp3E2 + 16},{_tmp3E3,_tmp3E3,_tmp3E3 + 11},{_tmp3E4,_tmp3E4,_tmp3E4 + 10},{_tmp3E5,_tmp3E5,_tmp3E5 + 14},{_tmp3E6,_tmp3E6,_tmp3E6 + 15},{_tmp3E7,_tmp3E7,_tmp3E7 + 20},{_tmp3E8,_tmp3E8,_tmp3E8 + 27},{_tmp3E9,_tmp3E9,_tmp3E9 + 10},{_tmp3EA,_tmp3EA,_tmp3EA + 18},{_tmp3EB,_tmp3EB,_tmp3EB + 21},{_tmp3EC,_tmp3EC,_tmp3EC + 19},{_tmp3ED,_tmp3ED,_tmp3ED + 16},{_tmp3EE,_tmp3EE,_tmp3EE + 20},{_tmp3EF,_tmp3EF,_tmp3EF + 15},{_tmp3F0,_tmp3F0,_tmp3F0 + 20},{_tmp3F1,_tmp3F1,_tmp3F1 + 15},{_tmp3F2,_tmp3F2,_tmp3F2 + 12},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 19},{_tmp3F5,_tmp3F5,_tmp3F5 + 20},{_tmp3F6,_tmp3F6,_tmp3F6 + 21},{_tmp3F7,_tmp3F7,_tmp3F7 + 21},{_tmp3F8,_tmp3F8,_tmp3F8 + 12},{_tmp3F9,_tmp3F9,_tmp3F9 + 17},{_tmp3FA,_tmp3FA,_tmp3FA + 19},{_tmp3FB,_tmp3FB,_tmp3FB + 14},{_tmp3FC,_tmp3FC,_tmp3FC + 17},{_tmp3FD,_tmp3FD,_tmp3FD + 23},{_tmp3FE,_tmp3FE,_tmp3FE + 13},{_tmp3FF,_tmp3FF,_tmp3FF + 14},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,_tmp401 + 16},{_tmp402,_tmp402,_tmp402 + 8},{_tmp403,_tmp403,_tmp403 + 19},{_tmp404,_tmp404,_tmp404 + 20},{_tmp405,_tmp405,_tmp405 + 14},{_tmp406,_tmp406,_tmp406 + 19},{_tmp407,_tmp407,_tmp407 + 20},{_tmp408,_tmp408,_tmp408 + 11},{_tmp409,_tmp409,_tmp409 + 22},{_tmp40A,_tmp40A,_tmp40A + 20},{_tmp40B,_tmp40B,_tmp40B + 23},{_tmp40C,_tmp40C,_tmp40C + 20},{_tmp40D,_tmp40D,_tmp40D + 22},{_tmp40E,_tmp40E,_tmp40E + 23},{_tmp40F,_tmp40F,_tmp40F + 24},{_tmp410,_tmp410,_tmp410 + 24},{_tmp411,_tmp411,_tmp411 + 15},{_tmp412,_tmp412,_tmp412 + 20},{_tmp413,_tmp413,_tmp413 + 22},{_tmp414,_tmp414,_tmp414 + 17},{_tmp415,_tmp415,_tmp415 + 20},{_tmp416,_tmp416,_tmp416 + 26},{_tmp417,_tmp417,_tmp417 + 16},{_tmp418,_tmp418,_tmp418 + 17},{_tmp419,_tmp419,_tmp419 + 15},{_tmp41A,_tmp41A,_tmp41A + 19},{_tmp41B,_tmp41B,_tmp41B + 17},{_tmp41C,_tmp41C,_tmp41C + 19},{_tmp41D,_tmp41D,_tmp41D + 25},{_tmp41E,_tmp41E,_tmp41E + 26},{_tmp41F,_tmp41F,_tmp41F + 9},{_tmp420,_tmp420,_tmp420 + 20},{_tmp421,_tmp421,_tmp421 + 17},{_tmp422,_tmp422,_tmp422 + 18},{_tmp423,_tmp423,_tmp423 + 11},{_tmp424,_tmp424,_tmp424 + 12}};
# 1636
static short Cyc_yyr1[530]={0,148,149,149,149,149,149,149,149,149,149,149,150,151,152,152,153,153,153,154,154,154,155,155,156,156,156,156,157,157,158,159,160,161,162,162,162,162,162,162,162,163,163,164,164,165,165,165,165,165,165,165,165,165,165,166,166,166,166,166,166,166,167,167,168,169,169,170,170,170,170,171,171,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,173,174,174,174,175,175,175,176,176,177,177,177,178,178,178,178,178,179,179,180,180,181,181,182,182,183,184,184,185,185,186,187,187,187,187,187,187,188,188,188,188,188,188,189,190,190,191,191,191,191,192,192,193,193,194,194,194,195,195,196,196,196,196,197,197,197,198,198,199,199,200,200,201,201,201,201,201,201,201,201,201,201,202,202,202,202,202,202,202,202,202,202,202,203,203,204,205,205,206,206,206,206,206,206,206,206,207,207,207,208,208,209,209,209,210,210,210,211,211,212,212,212,212,213,213,214,214,215,215,216,216,217,217,218,218,219,219,219,219,220,220,221,221,222,222,222,223,224,224,225,225,226,226,226,226,226,227,227,227,227,228,228,229,229,230,230,231,231,232,232,232,232,232,233,233,234,234,234,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,236,236,236,237,238,238,239,239,240,240,240,240,240,240,240,240,241,241,241,241,241,241,242,242,242,242,242,242,243,243,243,243,243,243,243,243,243,243,243,243,243,243,244,244,244,244,244,244,244,244,245,246,246,247,247,248,248,249,249,250,250,251,251,252,252,252,253,253,253,253,253,254,254,254,255,255,255,256,256,256,256,257,257,258,258,258,258,258,258,259,260,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,262,262,262,263,263,264,264,265,265,265,266,266,267,267,268,268,268,269,269,269,269,269,269,269,269,269,269,269,270,270,270,270,270,270,270,271,272,272,273,273,274,274,275,275,276,276,277,277,277,278,278,278,278,278,279,279,279,280,280,280,281,281,281,281,282,282,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,284,284,284,285,285,285,285,285,285,285,285,285,285,285,286,286,286,286,287,287,287,287,287,287,287,287,287,287,288,289,289,290,290,290,290,290,291,291,292,292,293,293,294,294,295,295};
# 1692
static short Cyc_yyr2[530]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1748
static short Cyc_yydefact[1077]={0,21,55,56,57,58,60,73,74,75,76,77,78,79,80,81,99,100,101,117,118,51,0,0,85,0,61,0,0,155,92,96,0,0,0,0,0,0,42,520,221,522,521,523,0,0,88,0,207,207,206,1,0,0,19,0,0,20,0,0,45,53,47,71,49,82,83,0,86,0,41,166,0,191,194,87,170,115,59,58,52,0,103,0,519,0,520,515,516,517,518,115,0,382,0,0,0,0,244,0,384,385,30,32,0,0,0,0,0,0,0,156,0,0,0,0,0,0,0,204,205,0,2,0,0,0,0,0,34,0,123,124,126,46,54,48,50,119,524,525,115,115,41,43,41,0,23,0,223,0,179,167,192,0,198,199,202,203,0,201,200,212,194,0,72,59,107,0,105,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,504,505,483,0,0,0,0,0,486,484,485,0,410,412,426,434,436,438,440,442,444,447,452,455,458,462,0,464,487,503,502,520,393,0,0,0,0,0,394,392,37,0,0,115,0,0,0,133,129,131,264,266,0,0,9,10,0,0,115,526,527,222,98,0,0,171,89,242,0,239,0,0,3,0,5,0,0,0,35,0,0,41,23,0,120,121,146,114,0,153,0,0,0,0,0,0,0,0,0,0,0,0,520,295,297,0,305,299,0,303,287,288,289,0,290,291,292,0,44,23,126,22,24,271,0,229,245,0,0,225,223,0,209,0,0,0,214,62,213,195,0,108,104,0,0,0,472,0,428,462,0,429,430,0,0,0,0,0,0,0,0,0,0,465,466,41,0,0,0,468,469,467,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,416,417,418,419,420,421,422,423,424,425,415,0,470,0,493,494,0,0,0,507,0,115,386,0,0,407,520,0,0,0,0,260,403,408,0,405,0,383,400,401,0,398,0,246,0,0,0,0,267,0,237,134,139,135,137,130,132,223,0,273,265,274,529,528,0,91,95,0,93,97,113,68,67,0,65,172,223,241,168,273,243,180,181,0,90,208,13,0,31,0,33,0,39,0,0,125,127,248,247,23,25,110,122,0,0,0,141,142,149,0,115,115,161,0,0,0,0,0,520,0,41,0,334,335,336,0,0,338,0,0,41,0,306,300,126,304,298,296,26,0,178,230,0,0,0,236,224,231,149,0,0,0,225,177,211,210,173,209,0,0,215,63,116,109,433,106,102,0,0,0,520,249,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,506,513,0,512,411,435,0,437,439,441,443,445,446,450,451,448,449,453,454,456,457,459,460,461,414,413,492,489,0,491,0,0,0,396,397,263,0,404,258,261,391,0,259,388,0,395,36,0,389,0,268,0,140,136,138,0,225,0,209,0,275,0,223,0,286,270,0,115,0,0,0,133,0,115,0,223,0,190,169,240,0,7,0,4,6,0,0,27,0,145,128,146,0,0,144,225,154,163,162,0,0,157,0,0,0,313,0,0,0,0,0,0,333,337,0,0,0,0,301,294,41,28,272,223,0,233,0,0,151,226,0,149,229,217,174,196,197,215,193,471,0,0,250,0,255,474,0,0,0,0,0,479,482,0,0,0,0,0,463,509,0,0,490,488,0,0,0,262,406,409,399,402,390,269,238,149,0,276,277,209,0,0,225,209,0,94,225,520,0,64,66,0,182,0,0,225,0,209,0,0,8,0,0,143,0,147,119,152,164,161,161,0,0,0,0,0,0,0,0,0,0,0,0,0,313,339,0,0,302,29,225,0,234,232,0,175,0,151,225,0,216,499,0,498,0,251,256,0,0,0,0,0,431,432,492,491,511,495,0,514,427,508,510,0,387,151,149,279,285,149,0,278,209,119,0,69,183,189,149,0,188,184,209,15,0,16,0,38,0,0,0,160,159,307,313,0,0,0,0,0,0,341,342,344,346,348,350,352,354,357,362,365,368,372,374,380,381,0,41,310,319,0,0,0,0,0,0,0,0,0,0,340,293,219,235,0,227,176,218,223,473,0,0,257,475,476,0,0,481,480,496,0,282,151,151,149,280,0,0,151,149,185,14,17,0,0,148,112,0,0,0,313,0,378,0,0,375,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,376,313,0,321,0,0,0,329,0,0,0,0,0,0,312,0,0,225,501,500,0,0,0,497,281,284,151,111,0,186,151,18,40,165,308,309,0,0,0,0,313,315,345,0,347,349,351,353,355,356,360,361,358,359,363,364,366,367,369,370,371,0,314,320,322,323,0,331,330,0,325,0,0,0,150,228,220,0,0,0,0,283,0,187,311,377,0,373,316,0,41,324,332,326,327,0,253,252,477,0,70,0,343,313,317,328,0,379,318,478,0,0,0};
# 1859
static short Cyc_yydefgoto[148]={1074,51,52,466,648,850,53,307,54,291,55,468,56,470,57,58,144,59,60,539,232,452,453,233,63,246,234,65,166,167,66,164,67,268,269,129,130,131,270,235,434,484,485,486,660,801,68,69,665,666,667,70,487,71,457,72,73,161,162,74,119,535,322,704,621,75,622,529,695,521,525,526,428,315,254,97,98,551,476,552,408,409,410,236,308,309,623,440,294,295,296,297,298,299,782,300,301,866,867,868,869,870,871,872,873,874,875,876,877,878,879,880,881,411,419,420,412,413,414,302,197,390,198,544,199,200,201,202,203,204,205,206,207,208,209,210,211,212,808,213,568,569,214,215,77,851,415,444};
# 1877
static short Cyc_yypact[1077]={2880,- -32768,- -32768,- -32768,- -32768,- 28,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3583,243,- 23,- -32768,1040,- -32768,260,6,- -32768,17,36,62,117,127,168,255,258,- -32768,- -32768,222,- -32768,- -32768,- -32768,217,268,264,287,312,312,- -32768,- -32768,336,2741,- -32768,385,617,- -32768,328,821,3583,3583,3583,- -32768,3583,- -32768,- -32768,466,- -32768,260,3497,91,63,574,990,- -32768,- -32768,334,373,410,- -32768,260,405,6600,- -32768,4165,- 45,- -32768,- -32768,- -32768,- -32768,334,6600,- -32768,408,439,4165,436,438,443,- -32768,- 12,- -32768,- -32768,3733,3733,2741,2741,3733,6600,539,- -32768,227,449,200,450,227,4108,6600,- -32768,- -32768,3018,- -32768,2741,3018,2741,3018,66,- -32768,464,467,- -32768,3368,- -32768,- -32768,- -32768,- -32768,4108,- -32768,- -32768,334,139,1739,- -32768,3497,821,479,3733,3658,4934,- -32768,91,- -32768,500,- -32768,- -32768,- -32768,- -32768,502,- -32768,- -32768,78,990,3733,- -32768,- -32768,483,520,537,260,6892,540,6600,6746,575,592,600,633,635,642,660,676,680,6989,6989,- -32768,- -32768,- -32768,2314,688,7037,7037,7037,- -32768,- -32768,- -32768,266,- -32768,- -32768,- 16,638,683,679,686,722,89,749,511,138,- -32768,1141,7037,391,- 39,- -32768,703,110,- -32768,4165,107,2564,273,2842,743,- -32768,- -32768,746,6600,334,2842,731,3883,4108,4183,4108,380,- -32768,- 38,- 38,- -32768,- -32768,43,306,334,- -32768,- -32768,- -32768,- -32768,55,738,- -32768,- -32768,126,324,- -32768,757,771,- -32768,784,- -32768,786,609,803,- -32768,200,5032,3497,479,799,4108,- -32768,433,809,260,811,807,170,813,4290,816,828,825,827,5130,4290,165,817,805,- -32768,- -32768,823,1884,1884,821,1884,- -32768,- -32768,- -32768,832,- -32768,- -32768,- -32768,257,- -32768,479,835,- -32768,- -32768,830,45,854,- -32768,46,837,831,292,838,782,829,6600,3733,- -32768,851,- -32768,- -32768,45,260,- -32768,6600,850,2314,- -32768,4108,- -32768,- -32768,4388,- -32768,881,6600,6600,6600,6600,6600,6600,6600,4108,760,2314,- -32768,- -32768,2029,868,372,6600,- -32768,- -32768,- -32768,6600,- -32768,7037,6600,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,6600,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6600,- -32768,227,- -32768,- -32768,5228,227,6600,- -32768,873,334,- -32768,852,875,- -32768,95,227,6600,4165,241,- -32768,- -32768,- -32768,877,879,872,- -32768,- -32768,- -32768,880,882,4165,- -32768,348,2564,883,3733,- -32768,891,892,- -32768,4183,4183,4183,- -32768,- -32768,3101,5326,350,- -32768,192,- -32768,- -32768,46,- -32768,- -32768,3733,- -32768,- -32768,895,- -32768,890,893,900,- -32768,1398,- -32768,214,632,- -32768,- -32768,- -32768,4108,- -32768,- -32768,- -32768,2602,- -32768,2741,- -32768,2741,- -32768,920,902,- -32768,- -32768,- -32768,- -32768,479,- -32768,- -32768,- -32768,607,6600,909,908,- -32768,- 19,333,334,334,834,6600,6600,906,916,6600,910,1013,1244,923,- -32768,- -32768,- -32768,572,1001,- -32768,5424,6600,2174,2454,- -32768,- -32768,3368,- -32768,- -32768,- -32768,- -32768,3733,- -32768,- -32768,4108,918,3958,- -32768,- -32768,911,971,46,922,4033,831,- -32768,- -32768,- -32768,- -32768,782,928,67,814,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,930,936,958,939,- -32768,- -32768,741,5032,943,949,950,953,954,399,955,956,673,960,959,6844,- -32768,- -32768,961,967,- -32768,638,73,683,679,686,722,89,89,749,749,749,749,511,511,138,138,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,963,- -32768,133,3733,4836,4108,- -32768,- -32768,944,- -32768,- -32768,- -32768,- -32768,2703,- -32768,- -32768,3194,- -32768,- -32768,970,- -32768,102,- -32768,4108,- -32768,- -32768,- -32768,965,831,968,782,948,192,3733,3808,5522,- -32768,- -32768,- 38,334,6698,969,55,3232,974,334,3733,3658,5620,- -32768,214,- -32768,977,- -32768,2741,- -32768,- -32768,972,988,- -32768,787,- -32768,- -32768,433,976,6600,- -32768,831,- -32768,- -32768,- -32768,982,260,597,409,415,6600,853,419,4290,979,5718,5816,641,- -32768,- -32768,986,991,980,457,1884,- -32768,3497,- -32768,830,993,3733,- -32768,994,46,1034,- -32768,992,971,148,- -32768,- -32768,- -32768,- -32768,814,- -32768,996,181,995,- -32768,4500,- -32768,- -32768,6600,6600,1097,6600,6746,- -32768,- -32768,227,227,996,997,4612,- -32768,- -32768,6600,6600,- -32768,- -32768,45,759,1015,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,971,999,- -32768,- -32768,782,45,1000,831,782,989,- -32768,831,1006,1002,- -32768,- -32768,1005,- -32768,45,1007,831,1009,782,1008,239,- -32768,6600,1004,- -32768,6600,- -32768,4108,- -32768,1012,53,834,4290,1019,1017,3049,1003,1021,4290,6600,5914,659,6012,666,6110,853,- -32768,1026,1029,- -32768,- -32768,831,132,- -32768,- -32768,1023,- -32768,1041,1034,831,4108,- -32768,- -32768,613,- -32768,6600,- -32768,- -32768,5032,1030,1032,1027,1042,- -32768,881,1035,1036,- -32768,- -32768,763,- -32768,- -32768,- -32768,- -32768,4836,- -32768,1034,971,- -32768,- -32768,971,1038,- -32768,782,4108,1049,- -32768,- -32768,- -32768,971,1043,- -32768,- -32768,782,- -32768,1046,674,508,- -32768,1045,1052,4108,- -32768,- -32768,1149,853,1056,7134,1051,2454,7037,1048,- -32768,- 3,- -32768,1088,1047,748,808,97,824,612,344,- -32768,- -32768,- -32768,- -32768,1092,7037,2029,- -32768,- -32768,516,4290,541,6208,4290,551,6306,6404,691,1063,- -32768,- -32768,- -32768,- -32768,6600,1064,- -32768,- -32768,993,- -32768,311,293,- -32768,- -32768,- -32768,4108,1164,- -32768,- -32768,- -32768,4724,- -32768,1034,1034,971,- -32768,1068,1070,1034,971,- -32768,- -32768,- -32768,539,1071,- -32768,- -32768,559,4290,1072,853,2314,- -32768,4108,1067,- -32768,1594,7037,6600,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,6600,- -32768,853,1076,- -32768,4290,4290,563,- -32768,4290,4290,576,4290,580,6502,- -32768,1074,46,831,- -32768,- -32768,2454,1075,1077,- -32768,- -32768,- -32768,1034,- -32768,1090,- -32768,1034,- -32768,- -32768,- -32768,- -32768,- -32768,1085,1080,1087,7037,853,- -32768,638,305,683,679,679,722,89,89,749,749,749,749,511,511,138,138,- -32768,- -32768,- -32768,361,- -32768,- -32768,- -32768,- -32768,4290,- -32768,- -32768,4290,- -32768,4290,4290,603,- -32768,- -32768,- -32768,1091,778,1084,4108,- -32768,1093,- -32768,- -32768,996,181,- -32768,- -32768,6600,1594,- -32768,- -32768,- -32768,- -32768,4290,- -32768,- -32768,- -32768,1094,- -32768,648,- -32768,853,- -32768,- -32768,1095,- -32768,- -32768,- -32768,1200,1217,- -32768};
# 1988
static short Cyc_yypgoto[148]={- -32768,286,- -32768,- -32768,- -32768,290,- -32768,- 220,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,22,- -32768,- 92,33,- -32768,- -32768,0,585,- -32768,88,- 184,1109,13,- -32768,- -32768,- 128,- -32768,550,1191,- 724,- -32768,- -32768,- -32768,964,962,565,417,- -32768,- -32768,573,- 68,- 735,- -32768,- -32768,144,- -32768,- -32768,59,- 215,1160,- 389,259,- -32768,1073,- -32768,- -32768,1184,- 396,- -32768,531,- 131,- 159,- 126,- 67,291,548,579,- 416,- 471,- 109,- 419,- 112,- -32768,- 253,- 150,- 542,- 325,- -32768,858,14,- 18,- 135,- 105,- 358,80,- -32768,- -32768,- 15,- 285,- -32768,- 631,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,225,1044,- -32768,667,855,- -32768,143,503,- -32768,- 168,- 356,- 149,- 348,- 341,- 329,912,- 322,- 347,- 338,- 346,- 335,- 176,619,496,931,230,- -32768,- 351,- -32768,245,553,- 64,18,- 111,162};
# 2010
static short Cyc_yytable[7282]={61,247,323,139,333,247,511,512,253,514,553,571,475,64,354,355,356,313,577,578,573,61,314,336,337,583,584,631,325,579,580,581,582,574,64,391,316,456,585,586,266,329,576,645,594,114,139,479,855,432,441,602,692,61,80,146,731,658,663,664,61,61,61,218,61,564,64,398,903,644,61,150,450,64,64,64,- 245,64,359,- 245,626,219,78,64,517,140,237,238,62,442,241,944,143,133,134,135,918,136,399,644,626,443,689,145,115,83,61,61,522,62,228,645,696,659,103,923,229,267,132,64,64,61,360,61,61,61,61,655,243,304,439,441,61,441,64,945,64,64,64,64,700,62,61,104,61,64,740,459,62,62,62,150,62,524,143,64,682,64,62,896,543,40,105,451,292,145,303,446,523,518,442,39,442,249,261,293,- 158,145,367,368,42,824,443,106,988,989,952,953,461,531,993,45,262,40,62,62,702,357,542,587,588,589,351,218,305,727,443,461,321,62,44,62,62,62,62,369,370,147,218,148,62,219,402,954,955,149,196,- 526,744,936,62,739,62,431,39,221,219,403,107,543,638,443,41,42,43,429,433,432,432,432,99,478,242,108,455,1045,520,357,653,1047,437,900,48,49,50,163,61,638,729,312,100,443,273,553,375,19,20,- 223,39,64,- 223,592,376,377,524,595,506,42,303,244,807,61,61,507,61,601,109,245,493,145,710,494,538,771,64,64,1000,64,39,111,217,44,1005,292,292,543,292,42,627,779,628,224,540,305,293,293,629,293,45,112,100,352,152,44,1026,244,48,49,50,122,640,100,641,245,546,113,547,39,642,833,61,39,513,837,62,41,42,43,498,561,42,563,849,64,505,81,604,847,39,524,423,116,292,1052,406,407,39,42,516,62,62,357,62,293,813,42,905,44,127,724,357,639,239,240,461,358,45,357,553,794,445,253,416,447,48,49,50,256,615,257,258,259,260,312,854,117,530,357,461,244,982,686,757,1068,461,984,504,245,632,357,357,431,431,431,118,448,1072,1053,62,757,630,39,922,401,433,433,433,418,462,41,42,43,927,463,418,637,812,643,121,694,163,482,730,100,699,100,61,100,61,612,61,519,357,352,100,392,393,394,436,64,960,64,654,64,437,357,559,541,961,962,352,543,746,1054,438,- 12,357,747,687,61,688,566,748,572,813,759,123,124,436,61,760,458,64,61,437,761,48,49,50,395,165,676,64,357,396,397,64,169,716,762,488,684,145,357,524,143,222,429,777,357,596,39,145,357,778,979,145,223,783,41,42,43,703,76,62,742,62,225,62,826,909,482,226,796,696,483,818,819,227,48,49,50,804,639,39,82,812,248,101,250,102,357,41,42,43,263,793,62,137,685,461,264,813,461,772,809,1006,62,76,965,306,62,543,1012,1013,1008,76,326,820,821,1018,1019,76,732,1014,1015,1016,1017,1009,1010,138,255,141,1020,1021,76,630,1011,319,803,320,429,357,603,168,668,669,931,101,672,357,220,677,327,643,966,611,39,61,101,373,683,318,100,374,41,42,43,1004,76,76,64,328,138,812,357,100,76,332,100,968,795,797,831,76,357,76,76,76,76,972,836,252,462,61,839,61,357,997,942,272,274,1030,679,845,438,357,64,76,64,357,139,271,338,357,1033,292,964,303,1035,48,49,50,252,458,39,703,293,775,145,339,776,168,41,42,43,44,357,340,899,471,361,1059,543,472,482,62,904,125,126,39,488,906,48,49,50,907,934,41,42,43,958,647,685,649,959,650,719,393,394,455,341,789,342,919,357,437,920,477,1067,343,101,62,101,62,101,925,1071,424,981,890,907,101,357,1022,1023,1024,893,344,331,357,334,334,449,751,929,983,430,930,435,84,395,348,349,345,460,720,397,346,334,334,334,977,365,366,357,353,76,786,788,363,524,537,362,460,400,490,1051,364,495,334,16,17,18,271,737,371,372,477,663,664,554,555,556,557,558,76,560,618,619,620,421,100,990,422,100,567,859,994,426,570,533,534,886,708,709,139,454,39,87,185,186,88,89,90,780,781,42,941,168,464,590,828,829,61,1066,916,917,347,948,949,189,827,591,465,39,488,64,567,950,951,1061,357,41,42,43,292,467,834,469,852,956,957,488,1040,482,488,293,473,857,858,843,480,48,49,50,985,887,889,39,892,489,895,491,766,492,500,509,42,809,625,496,128,61,499,508,501,334,502,45,598,510,1001,908,1002,515,64,48,49,50,265,101,520,528,536,292,527,532,967,443,44,971,62,545,101,359,293,101,334,599,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,565,1041,597,606,252,600,607,608,882,610,221,460,609,567,460,614,567,616,998,617,633,634,76,252,76,635,76,636,100,334,252,651,652,656,62,657,970,670,460,974,976,671,661,662,460,673,674,678,680,690,658,1028,1029,693,697,1031,1032,61,1034,477,701,1063,705,706,153,154,155,156,157,707,64,158,159,160,- 526,711,712,713,252,292,714,715,733,84,352,722,745,717,718,252,293,1007,721,725,726,728,738,741,252,768,743,755,765,767,477,334,758,770,773,1025,784,790,792,1055,800,791,1056,312,1057,1058,798,723,802,1037,810,816,830,838,853,822,1042,832,835,840,841,750,85,842,884,844,754,846,1069,856,860,62,848,885,764,861,897,86,87,898,901,88,89,90,912,902,42,924,910,101,911,91,101,914,915,913,921,92,928,93,94,926,567,932,933,95,935,937,940,943,946,96,752,334,963,978,947,980,986,460,991,996,252,992,999,1003,1027,378,1046,1075,76,430,1038,1043,1044,252,460,1048,1049,460,1050,477,1062,1060,814,815,1076,817,774,995,756,1064,1070,1073,251,477,110,474,825,769,481,151,120,806,324,379,380,381,382,383,384,385,386,387,388,805,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,389,605,21,170,171,1039,799,425,735,575,883,562,334,613,1065,22,23,24,84,25,172,0,0,0,173,26,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,477,0,138,0,0,0,0,0,0,36,37,38,0,183,184,0,477,101,0,334,0,271,0,0,0,0,0,0,0,0,334,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,675,0,0,0,0,252,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,334,194,195,0,0,0,0,0,0,0,0,0,0,0,0,271,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,477,252,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,939,0,138,334,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,310,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,312,334,0,0,0,0,0,455,0,46,47,0,0,437,0,48,49,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,0,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,780,781,21,170,171,275,334,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,334,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,- 313,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,289,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,- 41,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,84,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,0,350,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,84,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,0,0,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,646,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,404,0,35,405,87,0,0,88,89,90,0,245,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,406,407,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,734,0,35,405,87,0,0,88,89,90,0,245,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,406,407,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,417,0,35,216,87,0,0,88,89,90,0,0,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,0,0,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,862,863,30,31,- 41,0,0,0,32,33,34,0,0,35,0,84,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,85,0,0,0,45,0,46,47,0,0,29,0,48,49,50,216,87,30,31,88,89,90,0,0,42,34,0,0,35,91,0,0,0,0,0,864,0,93,94,0,36,37,0,95,0,0,865,0,0,96,193,0,0,194,195,0,0,0,0,0,0,0,0,0,310,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,312,0,0,0,0,0,0,436,0,46,47,0,84,437,0,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,85,0,0,30,31,0,0,0,0,0,0,34,736,0,35,216,87,0,0,88,89,90,0,0,42,0,36,37,0,91,0,0,0,0,0,92,0,93,94,0,0,0,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,482,0,46,47,0,0,0,0,48,49,50,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,- 41,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,0,142,0,0,265,0,0,0,0,46,47,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,142,22,23,24,0,0,0,0,46,47,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,310,0,0,311,0,0,0,22,23,24,40,41,0,43,44,0,312,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,230,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,231,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,310,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,312,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,427,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,691,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,698,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,85,0,0,0,0,0,36,37,0,0,0,0,0,0,0,216,87,0,0,88,89,90,0,0,42,0,0,0,0,91,0,0,0,0,0,92,0,93,94,0,40,0,0,95,44,0,0,0,0,96,0,0,0,0,0,46,47,170,171,275,0,276,277,278,279,280,281,282,283,0,0,0,84,0,172,284,0,0,173,0,285,0,0,0,174,175,176,177,178,0,0,0,0,179,0,286,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,87,185,186,88,89,90,0,0,42,0,0,187,288,142,170,171,0,0,0,188,0,548,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,550,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,811,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,823,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,987,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,406,407,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,317,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,503,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,593,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,624,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,681,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,749,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,763,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,785,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,787,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,888,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,891,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,894,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,969,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,973,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,975,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,1036,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,753,87,185,186,88,89,90,180,181,42,182,0,187,0,0,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,0,0,192,193,0,0,194,195,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,39,87,185,186,88,89,90,180,181,42,182,0,187,0,723,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,170,171,0,0,0,0,330,0,0,189,0,0,0,0,190,84,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,39,87,185,186,88,89,90,180,181,42,182,0,187,0,0,0,0,0,0,0,347,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,170,171,0,0,0,0,188,0,0,189,0,0,0,0,190,84,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,0,0,0,0,0,938,0,0,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195};
# 2742
static short Cyc_yycheck[7282]={0,112,161,67,172,116,291,292,117,294,335,359,265,0,190,191,192,148,365,366,361,21,148,173,173,371,372,443,163,367,368,369,370,362,21,211,148,252,373,374,132,169,364,462,395,45,110,267,772,233,88,407,523,53,21,70,598,76,5,6,60,61,62,108,64,350,53,106,803,458,70,71,17,60,61,62,121,64,94,124,438,126,110,70,304,67,104,105,0,127,108,94,70,60,61,62,831,64,137,488,458,139,521,70,45,128,106,107,62,21,122,530,528,132,108,839,128,132,59,106,107,121,138,123,124,125,126,483,110,144,235,88,132,88,121,138,123,124,125,126,536,53,142,126,144,132,617,252,60,61,62,151,64,312,132,142,507,144,70,790,328,115,126,108,142,132,144,124,122,124,127,108,127,114,108,142,123,144,89,90,117,723,139,121,919,920,89,90,252,315,925,128,126,115,106,107,129,124,326,375,376,377,188,108,145,132,139,271,130,121,119,123,124,125,126,126,127,126,108,128,132,126,115,126,127,134,83,132,624,860,142,129,144,233,108,92,126,130,121,407,455,139,116,117,118,231,233,431,432,433,25,266,109,126,128,990,108,124,478,994,134,129,136,137,138,126,266,482,135,121,25,139,133,598,136,20,21,129,108,266,132,392,144,145,443,396,121,117,266,108,109,291,292,128,294,406,128,116,128,266,553,131,320,659,291,292,937,294,108,51,85,119,943,291,292,483,294,117,126,670,128,96,322,264,291,292,134,294,128,107,85,188,73,119,965,108,136,137,138,53,126,96,128,116,330,128,332,108,134,745,350,108,293,749,266,116,117,118,278,345,117,347,123,350,284,122,125,763,108,528,227,107,350,1004,133,134,108,117,121,291,292,124,294,350,709,117,805,119,60,565,124,455,106,107,458,129,128,124,723,684,238,510,129,241,136,137,138,121,426,123,124,125,126,121,770,128,124,124,482,108,109,513,637,1054,488,132,283,116,446,124,124,431,432,433,122,129,1067,132,350,654,440,108,838,218,431,432,433,222,124,116,117,118,848,129,229,455,709,457,122,527,126,128,597,218,531,220,466,222,468,121,470,309,124,330,229,84,85,86,128,466,136,468,482,470,134,124,343,325,144,145,347,659,627,132,235,122,124,628,513,499,518,129,628,360,829,640,121,122,128,509,641,252,499,513,134,641,136,137,138,128,110,499,509,124,133,134,513,122,129,641,271,509,499,124,693,513,128,523,129,124,397,108,509,124,129,901,513,108,129,116,117,118,539,0,466,622,468,121,470,727,813,128,124,689,980,132,716,716,125,136,137,138,698,637,108,22,829,128,25,129,27,124,116,117,118,121,129,499,122,509,654,124,917,657,661,706,944,509,45,884,121,513,770,950,951,946,53,124,719,720,956,957,59,599,952,953,954,955,947,948,67,118,69,958,959,72,626,949,128,697,128,617,124,408,81,492,493,129,85,496,124,91,499,123,644,129,421,108,648,96,139,508,149,408,143,116,117,118,943,106,107,648,125,110,917,124,421,114,128,424,129,686,690,741,121,124,123,124,125,126,129,748,117,124,684,752,686,124,129,865,140,141,129,121,761,436,124,684,145,686,124,765,137,128,124,129,684,883,686,129,136,137,138,148,455,108,703,684,121,686,128,124,169,116,117,118,119,124,128,796,121,93,129,901,125,128,648,804,121,122,108,482,129,136,137,138,133,856,116,117,118,139,466,673,468,143,470,84,85,86,128,128,121,128,832,124,134,835,265,1054,128,218,684,220,686,222,844,129,228,905,121,133,229,124,960,961,962,121,128,170,124,172,173,243,632,121,907,232,124,234,40,128,183,184,128,252,133,134,128,190,191,192,121,91,92,124,128,264,675,676,141,980,319,140,271,122,273,1003,142,276,211,17,18,19,269,610,87,88,335,5,6,338,339,340,341,342,293,344,431,432,433,108,607,921,108,610,353,777,926,128,357,79,80,783,123,124,930,129,108,109,110,111,112,113,114,22,23,117,864,326,123,378,123,124,884,1053,123,124,128,141,142,131,730,390,123,108,637,884,395,91,92,123,124,116,117,118,884,123,746,123,767,87,88,654,981,128,657,884,115,775,776,759,123,136,137,138,912,784,785,108,787,122,789,122,648,128,108,132,117,1050,437,128,121,943,128,128,121,328,121,128,400,128,938,810,940,123,943,136,137,138,125,408,108,132,135,943,129,129,888,139,119,891,884,123,421,94,943,424,359,127,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,129,984,126,123,436,127,124,132,780,124,864,455,129,507,458,129,510,123,935,124,122,128,466,455,468,129,470,124,780,407,462,108,127,121,943,124,890,128,482,893,894,122,489,490,488,132,30,121,44,128,76,968,969,139,129,972,973,1054,975,553,129,1044,129,124,71,72,73,74,75,108,1054,78,79,80,132,129,124,124,510,1054,124,124,135,40,938,123,135,129,129,521,1054,945,129,129,124,129,123,129,530,108,129,129,122,128,598,483,129,128,123,963,128,122,129,1030,77,121,1033,121,1035,1036,123,122,127,977,126,25,108,135,121,129,984,129,129,124,129,629,93,129,132,129,634,129,1059,128,122,1054,135,123,642,129,121,108,109,121,128,112,113,114,128,115,117,109,129,607,129,122,610,129,129,124,129,128,123,130,131,129,670,129,123,136,28,122,128,132,93,142,633,565,93,123,140,124,25,637,123,121,628,124,123,129,121,57,109,0,648,637,129,129,128,641,654,123,129,657,124,709,129,123,712,713,0,715,666,930,636,129,129,129,116,723,36,264,726,657,269,72,49,703,162,95,96,97,98,99,100,101,102,103,104,698,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,125,409,24,25,26,980,693,229,607,363,780,346,659,424,1050,37,38,39,40,41,42,- 1,- 1,- 1,46,47,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,813,- 1,765,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,829,780,- 1,716,- 1,772,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,727,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,805,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,770,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,839,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,917,856,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,862,- 1,930,865,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,883,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,121,901,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,- 1,136,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,944,- 1,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1003,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1053,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,45,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,25,26,58,59,60,- 1,- 1,- 1,64,65,66,- 1,- 1,69,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,37,38,39,123,93,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,51,- 1,136,137,138,108,109,58,59,112,113,114,- 1,- 1,117,66,- 1,- 1,69,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,81,82,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,40,134,- 1,136,137,138,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,93,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,81,82,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,37,38,39,- 1,- 1,- 1,- 1,130,131,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,122,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,123,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,123,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,115,- 1,- 1,136,119,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,- 1,130,131,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,43,- 1,- 1,46,- 1,48,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,63,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,122,25,26,- 1,- 1,- 1,128,- 1,32,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E1[8]="stdcall";static char _tmp4E2[6]="cdecl";static char _tmp4E3[9]="fastcall";static char _tmp4E4[9]="noreturn";static char _tmp4E5[6]="const";static char _tmp4E6[8]="aligned";static char _tmp4E7[7]="packed";static char _tmp4E8[7]="shared";static char _tmp4E9[7]="unused";static char _tmp4EA[5]="weak";static char _tmp4EB[10]="dllimport";static char _tmp4EC[10]="dllexport";static char _tmp4ED[23]="no_instrument_function";static char _tmp4EE[12]="constructor";static char _tmp4EF[11]="destructor";static char _tmp4F0[22]="no_check_memory_usage";static char _tmp4F1[5]="pure";static char _tmp4F2[14]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp426=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp426;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp838;(_tmp838.YYINITIALSVAL).val=0;(_tmp838.YYINITIALSVAL).tag=64;_tmp838;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp836=200;short*_tmp837=_region_calloc(yyregion,sizeof(short),_tmp836);_tag_dyneither(_tmp837,sizeof(short),_tmp836);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp833=(unsigned int)200;union Cyc_YYSTYPE*_tmp834=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp833));{unsigned int _tmp835=_tmp833;unsigned int i;for(i=0;i < _tmp835;i ++){_tmp834[i]=(union Cyc_YYSTYPE)yylval;}}_tmp834;}),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp830=(unsigned int)200;struct Cyc_Yyltype*_tmp831=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp830));{unsigned int _tmp832=_tmp830;unsigned int i;for(i=0;i < _tmp832;i ++){_tmp831[i]=(struct Cyc_Yyltype)Cyc_yynewloc();}}_tmp831;}),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
Cyc_yyerror(({const char*_tmp427="parser stack overflow";_tag_dyneither(_tmp427,sizeof(char),22);}),yystate,yychar);
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp430=(unsigned int)yystacksize;short*_tmp431=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp430));struct _dyneither_ptr _tmp433=_tag_dyneither(_tmp431,sizeof(short),_tmp430);{unsigned int _tmp432=_tmp430;unsigned int i;for(i=0;i < _tmp432;i ++){_tmp431[i]=(short)(
i <= yyssp_offset?*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)): 0);}}_tmp433;});
struct _dyneither_ptr yyvs1=({unsigned int _tmp42C=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp42D=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp42C));struct _dyneither_ptr _tmp42F=_tag_dyneither(_tmp42D,sizeof(union Cyc_YYSTYPE),_tmp42C);{unsigned int _tmp42E=_tmp42C;unsigned int i;for(i=0;i < _tmp42E;i ++){_tmp42D[i]=(union Cyc_YYSTYPE)(
# 219
i <= yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)): yylval);}}_tmp42F;});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp428=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp429=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp428));struct _dyneither_ptr _tmp42B=_tag_dyneither(_tmp429,sizeof(struct Cyc_Yyltype),_tmp428);{unsigned int _tmp42A=_tmp428;unsigned int i;for(i=0;i < _tmp42A;i ++){_tmp429[i]=(struct Cyc_Yyltype)(
i <= yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):
 Cyc_yynewloc());}}_tmp42B;});
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1077,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 375?(int)Cyc_yytranslate[_check_known_subscript_notnull(376,yychar)]: 296;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7281) || Cyc_yycheck[_check_known_subscript_notnull(7282,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(7282,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1076){
int _tmp434=0;_npop_handler(0);return _tmp434;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1077,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(530,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp435=yyn;switch(_tmp435){case 1: _LL255: _LL256: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1191 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1194
goto _LL254;}case 2: _LL257: _LL258: {
# 1196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1197 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp436;_push_handler(& _tmp436);{int _tmp438=0;if(setjmp(_tmp436.handler))_tmp438=1;if(!_tmp438){x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp437=(void*)_exn_thrown;void*_tmp439=_tmp437;void*_tmp43A;if(((struct Cyc_Core_Failure_exn_struct*)_tmp439)->tag == Cyc_Core_Failure){_LL676: _LL677:
 x=0;goto _LL675;}else{_LL678: _tmp43A=_tmp439;_LL679:(int)_rethrow(_tmp43A);}_LL675:;}};}
{struct _handler_cons _tmp43B;_push_handler(& _tmp43B);{int _tmp43D=0;if(setjmp(_tmp43B.handler))_tmp43D=1;if(!_tmp43D){y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp43C=(void*)_exn_thrown;void*_tmp43E=_tmp43C;void*_tmp43F;if(((struct Cyc_Core_Failure_exn_struct*)_tmp43E)->tag == Cyc_Core_Failure){_LL67B: _LL67C:
 y=0;goto _LL67A;}else{_LL67D: _tmp43F=_tmp43E;_LL67E:(int)_rethrow(_tmp43F);}_LL67A:;}};}
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1204
goto _LL254;}case 3: _LL259: _LL25A: {
# 1206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1207 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->hd=({struct Cyc_Absyn_Decl*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->r=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442[0]=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp443;_tmp443.tag=10;_tmp443.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp443.f2=Cyc_yyget_YY16(yyyyvsp[2]);_tmp443;});_tmp442;});_tmp441->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp441;});_tmp440->tl=0;_tmp440;}));
Cyc_Lex_leave_using();
# 1210
goto _LL254;}case 4: _LL25B: _LL25C: {
# 1212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1211 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->hd=({struct Cyc_Absyn_Decl*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->r=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446[0]=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp447;_tmp447.tag=10;_tmp447.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp447.f2=Cyc_yyget_YY16(yyyyvsp[2]);_tmp447;});_tmp446;});_tmp445->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp445;});_tmp444->tl=Cyc_yyget_YY16(yyyyvsp[4]);_tmp444;}));
goto _LL254;}case 5: _LL25D: _LL25E: {
# 1214
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1214 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->hd=({struct Cyc_Absyn_Decl*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->r=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp44B;_tmp44B.tag=9;_tmp44B.f1=({struct _dyneither_ptr*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp44C;});_tmp44B.f2=Cyc_yyget_YY16(yyyyvsp[2]);_tmp44B;});_tmp44A;});_tmp449->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp449;});_tmp448->tl=0;_tmp448;}));
Cyc_Lex_leave_namespace();
# 1217
goto _LL254;}case 6: _LL25F: _LL260: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1219 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44D;_push_handler(& _tmp44D);{int _tmp44F=0;if(setjmp(_tmp44D.handler))_tmp44F=1;if(!_tmp44F){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp44E=(void*)_exn_thrown;void*_tmp450=_tmp44E;void*_tmp452;if(((struct Cyc_Core_Failure_exn_struct*)_tmp450)->tag == Cyc_Core_Failure){_LL680: _LL681:
 nspace=({const char*_tmp451="";_tag_dyneither(_tmp451,sizeof(char),1);});goto _LL67F;}else{_LL682: _tmp452=_tmp450;_LL683:(int)_rethrow(_tmp452);}_LL67F:;}};}
# 1224
{struct _handler_cons _tmp453;_push_handler(& _tmp453);{int _tmp455=0;if(setjmp(_tmp453.handler))_tmp455=1;if(!_tmp455){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp454=(void*)_exn_thrown;void*_tmp456=_tmp454;void*_tmp457;if(((struct Cyc_Core_Failure_exn_struct*)_tmp456)->tag == Cyc_Core_Failure){_LL685: _LL686:
 x=0;goto _LL684;}else{_LL687: _tmp457=_tmp456;_LL688:(int)_rethrow(_tmp457);}_LL684:;}};}
# 1227
{struct _handler_cons _tmp458;_push_handler(& _tmp458);{int _tmp45A=0;if(setjmp(_tmp458.handler))_tmp45A=1;if(!_tmp45A){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp459=(void*)_exn_thrown;void*_tmp45B=_tmp459;void*_tmp45C;if(((struct Cyc_Core_Failure_exn_struct*)_tmp45B)->tag == Cyc_Core_Failure){_LL68A: _LL68B:
 y=0;goto _LL689;}else{_LL68C: _tmp45C=_tmp45B;_LL68D:(int)_rethrow(_tmp45C);}_LL689:;}};}
# 1230
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->hd=({struct Cyc_Absyn_Decl*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->r=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp460;_tmp460.tag=9;_tmp460.f1=({struct _dyneither_ptr*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=nspace;_tmp461;});_tmp460.f2=x;_tmp460;});_tmp45F;});_tmp45E->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp45E;});_tmp45D->tl=y;_tmp45D;}));
# 1232
goto _LL254;}case 7: _LL261: _LL262: {
# 1234
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1236
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1233 "parse.y"
int _tmp462=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp462)
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->hd=({struct Cyc_Absyn_Decl*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->r=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp466;_tmp466.tag=11;_tmp466.f1=Cyc_yyget_YY16(yyyyvsp[2]);_tmp466;});_tmp465;});_tmp464->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp464;});_tmp463->tl=Cyc_yyget_YY16(yyyyvsp[4]);_tmp463;}));else{
# 1237
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->hd=({struct Cyc_Absyn_Decl*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp46A;_tmp46A.tag=12;_tmp46A.f1=Cyc_yyget_YY16(yyyyvsp[2]);_tmp46A.f2=0;_tmp46A;});_tmp469;});_tmp468->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp468;});_tmp467->tl=Cyc_yyget_YY16(yyyyvsp[4]);_tmp467;}));}
# 1239
goto _LL254;}case 8: _LL263: _LL264: {
# 1241
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 1243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 1240 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp46B=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp46C="expecting \"C include\"";_tag_dyneither(_tmp46C,sizeof(char),22);}),_tag_dyneither(_tmp46B,sizeof(void*),0));});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->hd=({struct Cyc_Absyn_Decl*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp470;_tmp470.tag=12;_tmp470.f1=Cyc_yyget_YY16(yyyyvsp[2]);_tmp470.f2=exs;_tmp470;});_tmp46F;});_tmp46E->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp46E;});_tmp46D->tl=Cyc_yyget_YY16(yyyyvsp[5]);_tmp46D;}));
# 1245
goto _LL254;};}case 9: _LL265: _LL266: {
# 1247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1246 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->hd=({struct Cyc_Absyn_Decl*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp472->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp472;});_tmp471->tl=Cyc_yyget_YY16(yyyyvsp[2]);_tmp471;}));
goto _LL254;}case 10: _LL267: _LL268: {
# 1249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1248 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->hd=({struct Cyc_Absyn_Decl*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->r=(void*)& Cyc_Absyn_Portoff_d_val;_tmp474->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp474;});_tmp473->tl=Cyc_yyget_YY16(yyyyvsp[2]);_tmp473;}));
goto _LL254;}case 11: _LL269: _LL26A:
# 1251
 yyval=Cyc_YY16(0);
goto _LL254;case 12: _LL26B: _LL26C: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1254 "parse.y"
struct _dyneither_ptr _tmp475=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(Cyc_strcmp((struct _dyneither_ptr)_tmp475,({const char*_tmp476="C";_tag_dyneither(_tmp476,sizeof(char),2);}))== 0)
yyval=Cyc_YY31(0);else{
if(Cyc_strcmp((struct _dyneither_ptr)_tmp475,({const char*_tmp477="C include";_tag_dyneither(_tmp477,sizeof(char),10);}))== 0)
yyval=Cyc_YY31(1);else{
# 1261
({void*_tmp478=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp479="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp479,sizeof(char),29);}),_tag_dyneither(_tmp478,sizeof(void*),0));});
yyval=Cyc_YY31(1);}}
# 1265
goto _LL254;}case 13: _LL26D: _LL26E: {
# 1267
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1269
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1268 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL254;}case 14: _LL26F: _LL270: {
# 1271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1272 "parse.y"
yyval=yyyyvsp[2];
goto _LL254;}case 15: _LL271: _LL272: {
# 1275
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1273 "parse.y"
yyval=Cyc_YY52(0);
goto _LL254;}case 16: _LL273: _LL274: {
# 1276
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1278
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1277 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->hd=({struct _tuple27*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->f1=(unsigned int)(yyyylsp[0]).first_line;_tmp47B->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47B->f3=0;_tmp47B;});_tmp47A->tl=0;_tmp47A;}));
goto _LL254;}case 17: _LL275: _LL276: {
# 1280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1278 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->hd=({struct _tuple27*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->f1=(unsigned int)(yyyylsp[0]).first_line;_tmp47D->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47D->f3=0;_tmp47D;});_tmp47C->tl=0;_tmp47C;}));
goto _LL254;}case 18: _LL277: _LL278: {
# 1281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1280 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->hd=({struct _tuple27*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->f1=(unsigned int)(yyyylsp[0]).first_line;_tmp47F->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47F->f3=0;_tmp47F;});_tmp47E->tl=Cyc_yyget_YY52(yyyyvsp[2]);_tmp47E;}));
goto _LL254;}case 19: _LL279: _LL27A: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1284 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp482;_tmp482.tag=1;_tmp482.f1=Cyc_yyget_YY15(yyyyvsp[0]);_tmp482;});_tmp481;}),(unsigned int)(yyyylsp[0]).first_line);_tmp480->tl=0;_tmp480;}));
goto _LL254;}case 20: _LL27B: _LL27C: {
# 1287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1285 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 21: _LL27D: _LL27E: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1286 "parse.y"
yyval=Cyc_YY16(0);
goto _LL254;}case 24: _LL27F: _LL280: {
# 1289
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1291
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1295 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 25: _LL281: _LL282: {
# 1298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1297 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp483=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp483,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 26: _LL283: _LL284: {
# 1301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1310 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 27: _LL285: _LL286: {
# 1313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1312 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp484=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp484,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 28: _LL287: _LL288: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1320 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp485=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp485,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 29: _LL289: _LL28A: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1323 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp486=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp486,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 30: _LL28B: _LL28C: {
# 1327
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1328 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL254;}case 31: _LL28D: _LL28E: {
# 1331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1331 "parse.y"
Cyc_Lex_leave_using();
goto _LL254;}case 32: _LL28F: _LL290: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1334 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp487;}));yyval=yyyyvsp[1];
goto _LL254;}case 33: _LL291: _LL292: {
# 1337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1337 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL254;}case 34: _LL293: _LL294: {
# 1340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
int _tmp488=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp488,(unsigned int)_tmp488));
goto _LL254;}case 35: _LL295: _LL296: {
# 1347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1346 "parse.y"
int _tmp489=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp489));
goto _LL254;}case 36: _LL297: _LL298: {
# 1350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1350 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line);_tmp48A->tl=0;_tmp48A;}));
goto _LL254;}case 37: _LL299: _LL29A: {
# 1353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1352 "parse.y"
struct Cyc_List_List*_tmp48B=0;
{struct Cyc_List_List*_tmp48C=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp48C != 0;_tmp48C=_tmp48C->tl){
struct _dyneither_ptr*_tmp48D=(struct _dyneither_ptr*)_tmp48C->hd;
struct _tuple0*qv=({struct _tuple0*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->f1=Cyc_Absyn_Rel_n(0);_tmp490->f2=_tmp48D;_tmp490;});
struct Cyc_Absyn_Vardecl*_tmp48E=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
_tmp48B=({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->hd=_tmp48E;_tmp48F->tl=_tmp48B;_tmp48F;});}}
# 1359
_tmp48B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp48B);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->hd=Cyc_Absyn_letv_decl(_tmp48B,(unsigned int)(yyyylsp[0]).first_line);_tmp491->tl=0;_tmp491;}));
# 1362
goto _LL254;}case 38: _LL29B: _LL29C: {
# 1364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 1366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 1365 "parse.y"
struct _dyneither_ptr _tmp492=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1367
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp492,({const char*_tmp493="`H";_tag_dyneither(_tmp493,sizeof(char),3);}))== 0)
({void*_tmp494=0;Cyc_Warn_err((unsigned int)(yyyylsp[3]).first_line,({const char*_tmp495="bad occurrence of heap region";_tag_dyneither(_tmp495,sizeof(char),30);}),_tag_dyneither(_tmp494,sizeof(void*),0));});
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp492,({const char*_tmp496="`U";_tag_dyneither(_tmp496,sizeof(char),3);}))== 0)
({void*_tmp497=0;Cyc_Warn_err((unsigned int)(yyyylsp[3]).first_line,({const char*_tmp498="bad occurrence of unique region";_tag_dyneither(_tmp498,sizeof(char),32);}),_tag_dyneither(_tmp497,sizeof(void*),0));});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->name=({struct _dyneither_ptr*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=_tmp492;_tmp4A1;});_tmp4A0->identity=- 1;_tmp4A0->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4A0;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp49F;_tmp49F.tag=2;_tmp49F.f1=tv;_tmp49F;});_tmp49E;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->f1=Cyc_Absyn_Loc_n;_tmp49A->f2=({struct _dyneither_ptr*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B[0]=Cyc_yyget_String_tok(yyyyvsp[5]);_tmp49B;});_tmp49A;}),(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp49D;_tmp49D.tag=15;_tmp49D.f1=t;_tmp49D;});_tmp49C;}),0);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line);_tmp499->tl=0;_tmp499;}));
# 1376
goto _LL254;};}case 39: _LL29D: _LL29E: {
# 1378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1378 "parse.y"
int _tmp4A2=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4A3=Cyc_yyget_String_tok(yyyyvsp[2]);
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,({const char*_tmp4A4="H";_tag_dyneither(_tmp4A4,sizeof(char),2);}))== 0)
({void*_tmp4A5=0;Cyc_Warn_err((unsigned int)(yyyylsp[2]).first_line,({const char*_tmp4A6="bad occurrence of heap region `H";_tag_dyneither(_tmp4A6,sizeof(char),33);}),_tag_dyneither(_tmp4A5,sizeof(void*),0));});
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,({const char*_tmp4A7="U";_tag_dyneither(_tmp4A7,sizeof(char),2);}))== 0)
({void*_tmp4A8=0;Cyc_Warn_err((unsigned int)(yyyylsp[2]).first_line,({const char*_tmp4A9="bad occurrence of unique region `U";_tag_dyneither(_tmp4A9,sizeof(char),35);}),_tag_dyneither(_tmp4A8,sizeof(void*),0));});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->name=({struct _dyneither_ptr*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4B5;_tmp4B5.tag=0;_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A3);({void*_tmp4B3[1]={& _tmp4B5};Cyc_aprintf(({const char*_tmp4B4="`%s";_tag_dyneither(_tmp4B4,sizeof(char),4);}),_tag_dyneither(_tmp4B3,sizeof(void*),1));});});_tmp4B2;});_tmp4B1->identity=- 1;_tmp4B1->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4B1;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4B0;_tmp4B0.tag=2;_tmp4B0.f1=tv;_tmp4B0;});_tmp4AF;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->f1=Cyc_Absyn_Loc_n;_tmp4AB->f2=({struct _dyneither_ptr*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=_tmp4A3;_tmp4AC;});_tmp4AB;}),(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4AE;_tmp4AE.tag=15;_tmp4AE.f1=t;_tmp4AE;});_tmp4AD;}),0);
# 1389
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4A2,0,(unsigned int)(yyyylsp[0]).first_line);_tmp4AA->tl=0;_tmp4AA;}));
# 1391
goto _LL254;};}case 40: _LL29F: _LL2A0: {
# 1393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9));
# 1395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9));
# 1393 "parse.y"
struct _dyneither_ptr _tmp4B6=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4B7=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4B8=Cyc_yyget_Exp_tok(yyyyvsp[6]);
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B7,({const char*_tmp4B9="open";_tag_dyneither(_tmp4B9,sizeof(char),5);}))!= 0)({void*_tmp4BA=0;Cyc_Warn_err((unsigned int)(yyyylsp[3]).first_line,({const char*_tmp4BB="expecting `open'";_tag_dyneither(_tmp4BB,sizeof(char),17);}),_tag_dyneither(_tmp4BA,sizeof(void*),0));});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->name=({struct _dyneither_ptr*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4C7;_tmp4C7.tag=0;_tmp4C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4B6);({void*_tmp4C5[1]={& _tmp4C7};Cyc_aprintf(({const char*_tmp4C6="`%s";_tag_dyneither(_tmp4C6,sizeof(char),4);}),_tag_dyneither(_tmp4C5,sizeof(void*),1));});});_tmp4C4;});_tmp4C3->identity=- 1;_tmp4C3->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4C3;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4C2;_tmp4C2.tag=2;_tmp4C2.f1=tv;_tmp4C2;});_tmp4C1;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->f1=Cyc_Absyn_Loc_n;_tmp4BD->f2=({struct _dyneither_ptr*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=_tmp4B6;_tmp4BE;});_tmp4BD;}),(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4C0;_tmp4C0.tag=15;_tmp4C0.f1=t;_tmp4C0;});_tmp4BF;}),0);
# 1402
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4B8,(unsigned int)(yyyylsp[0]).first_line);_tmp4BC->tl=0;_tmp4BC;}));
# 1404
goto _LL254;};}case 41: _LL2A1: _LL2A2:
# 1406
 yyval=Cyc_YY31(0);
goto _LL254;case 42: _LL2A3: _LL2A4: {
# 1409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1408 "parse.y"
yyval=Cyc_YY31(1);
goto _LL254;}case 43: _LL2A5: _LL2A6: {
# 1411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1413 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 44: _LL2A7: _LL2A8: {
# 1416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1415 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
goto _LL254;}case 45: _LL2A9: _LL2AA: {
# 1418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C8;_tmp4C8.sc=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4C8.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4C8.type_specs=
Cyc_Parse_empty_spec(0);_tmp4C8.is_inline=0;_tmp4C8.attributes=0;_tmp4C8;}));
goto _LL254;}case 46: _LL2AB: _LL2AC: {
# 1425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1424 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C9=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp4C9.sc != 0)
({void*_tmp4CA=0;Cyc_Warn_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp4CB="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp4CB,sizeof(char),73);}),_tag_dyneither(_tmp4CA,sizeof(void*),0));});
# 1428
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CC;_tmp4CC.sc=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4CC.tq=_tmp4C9.tq;_tmp4CC.type_specs=_tmp4C9.type_specs;_tmp4CC.is_inline=_tmp4C9.is_inline;_tmp4CC.attributes=_tmp4C9.attributes;_tmp4CC;}));
# 1432
goto _LL254;}case 47: _LL2AD: _LL2AE: {
# 1434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1433 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CD;_tmp4CD.sc=0;_tmp4CD.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4CD.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]);_tmp4CD.is_inline=0;_tmp4CD.attributes=0;_tmp4CD;}));
goto _LL254;}case 48: _LL2AF: _LL2B0: {
# 1437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1436 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CE=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CF;_tmp4CF.sc=_tmp4CE.sc;_tmp4CF.tq=_tmp4CE.tq;_tmp4CF.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4CE.type_specs,Cyc_yyget_YY21(yyyyvsp[0]));_tmp4CF.is_inline=_tmp4CE.is_inline;_tmp4CF.attributes=_tmp4CE.attributes;_tmp4CF;}));
# 1442
goto _LL254;}case 49: _LL2B1: _LL2B2: {
# 1444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1443 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D0;_tmp4D0.sc=0;_tmp4D0.tq=Cyc_yyget_YY23(yyyyvsp[0]);_tmp4D0.type_specs=Cyc_Parse_empty_spec(0);_tmp4D0.is_inline=0;_tmp4D0.attributes=0;_tmp4D0;}));
goto _LL254;}case 50: _LL2B3: _LL2B4: {
# 1446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1445 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D1=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D2;_tmp4D2.sc=_tmp4D1.sc;_tmp4D2.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp4D1.tq);_tmp4D2.type_specs=_tmp4D1.type_specs;_tmp4D2.is_inline=_tmp4D1.is_inline;_tmp4D2.attributes=_tmp4D1.attributes;_tmp4D2;}));
# 1450
goto _LL254;}case 51: _LL2B5: _LL2B6: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1451 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D3;_tmp4D3.sc=0;_tmp4D3.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4D3.type_specs=
Cyc_Parse_empty_spec(0);_tmp4D3.is_inline=1;_tmp4D3.attributes=0;_tmp4D3;}));
goto _LL254;}case 52: _LL2B7: _LL2B8: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1454 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D4=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D5;_tmp4D5.sc=_tmp4D4.sc;_tmp4D5.tq=_tmp4D4.tq;_tmp4D5.type_specs=_tmp4D4.type_specs;_tmp4D5.is_inline=1;_tmp4D5.attributes=_tmp4D4.attributes;_tmp4D5;}));
# 1458
goto _LL254;}case 53: _LL2B9: _LL2BA: {
# 1460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1459 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D6;_tmp4D6.sc=0;_tmp4D6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4D6.type_specs=
Cyc_Parse_empty_spec(0);_tmp4D6.is_inline=0;_tmp4D6.attributes=Cyc_yyget_YY45(yyyyvsp[0]);_tmp4D6;}));
goto _LL254;}case 54: _LL2BB: _LL2BC: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1462 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D7=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D8;_tmp4D8.sc=_tmp4D7.sc;_tmp4D8.tq=_tmp4D7.tq;_tmp4D8.type_specs=_tmp4D7.type_specs;_tmp4D8.is_inline=_tmp4D7.is_inline;_tmp4D8.attributes=
# 1465
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4D7.attributes);_tmp4D8;}));
goto _LL254;}case 55: _LL2BD: _LL2BE: {
# 1468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1469 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL254;}case 56: _LL2BF: _LL2C0: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1471 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL254;}case 57: _LL2C1: _LL2C2: {
# 1475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1473 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL254;}case 58: _LL2C3: _LL2C4: {
# 1477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1475 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL254;}case 59: _LL2C5: _LL2C6: {
# 1479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1478 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),({const char*_tmp4D9="C";_tag_dyneither(_tmp4D9,sizeof(char),2);}))!= 0)
({void*_tmp4DA=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp4DB="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp4DB,sizeof(char),37);}),_tag_dyneither(_tmp4DA,sizeof(void*),0));});
yyval=Cyc_YY20(& externC_sc);
# 1483
goto _LL254;}case 60: _LL2C7: _LL2C8: {
# 1485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1483 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL254;}case 61: _LL2C9: _LL2CA: {
# 1487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1486 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL254;}case 62: _LL2CB: _LL2CC:
# 1490
 yyval=Cyc_YY45(0);
goto _LL254;case 63: _LL2CD: _LL2CE: {
# 1493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1493 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 64: _LL2CF: _LL2D0: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 1498 "parse.y"
yyval=yyyyvsp[3];
goto _LL254;}case 65: _LL2D1: _LL2D2: {
# 1501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1502 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->hd=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4DC->tl=0;_tmp4DC;}));
goto _LL254;}case 66: _LL2D3: _LL2D4: {
# 1505
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1503 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->hd=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4DD->tl=Cyc_yyget_YY45(yyyyvsp[2]);_tmp4DD;}));
goto _LL254;}case 67: _LL2D5: _LL2D6: {
# 1506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1508 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[18]={{{_tmp4E1,_tmp4E1,_tmp4E1 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 8},(void*)& att_aligned},{{_tmp4E7,_tmp4E7,_tmp4E7 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 5},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 14},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1529
struct _dyneither_ptr _tmp4DE=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1531
if((((_get_dyneither_size(_tmp4DE,sizeof(char))> 4  && ((const char*)_tmp4DE.curr)[0]== '_') && ((const char*)_tmp4DE.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4DE,sizeof(char),(int)(_get_dyneither_size(_tmp4DE,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DE,sizeof(char),(int)(_get_dyneither_size(_tmp4DE,sizeof(char))- 3)))== '_')
# 1533
_tmp4DE=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4DE,2,_get_dyneither_size(_tmp4DE,sizeof(char))- 5);{
int i=0;
for(0;i < 18;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4DE,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(18,i)]).f2);
break;}}
# 1540
if(i == 18){
({void*_tmp4DF=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp4E0="unrecognized attribute";_tag_dyneither(_tmp4E0,sizeof(char),23);}),_tag_dyneither(_tmp4DF,sizeof(void*),0));});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1545
goto _LL254;};}case 68: _LL2D7: _LL2D8: {
# 1547
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1549
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1545 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL254;}case 69: _LL2D9: _LL2DA: {
# 1548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1547 "parse.y"
struct _dyneither_ptr _tmp4F4=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp4F5=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp4F6="aligned";_tag_dyneither(_tmp4F6,sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp4F7="__aligned__";_tag_dyneither(_tmp4F7,sizeof(char),12);}))== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp4F9;_tmp4F9.tag=6;_tmp4F9.f1=_tmp4F5;_tmp4F9;});_tmp4F8;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp4FA="section";_tag_dyneither(_tmp4FA,sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp4FB="__section__";_tag_dyneither(_tmp4FB,sizeof(char),12);}))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC[0]=({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmp4FD;_tmp4FD.tag=8;_tmp4FD.f1=str;_tmp4FD;});_tmp4FC;});}else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp4FE="__mode__";_tag_dyneither(_tmp4FE,sizeof(char),9);}))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF[0]=({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmp500;_tmp500.tag=24;_tmp500.f1=str;_tmp500;});_tmp4FF;});}else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp501="alias";_tag_dyneither(_tmp501,sizeof(char),6);}))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502[0]=({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmp503;_tmp503.tag=25;_tmp503.f1=str;_tmp503;});_tmp502;});}else{
# 1563
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp504="regparm";_tag_dyneither(_tmp504,sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp505="__regparm__";_tag_dyneither(_tmp505,sizeof(char),12);}))== 0){
if(n < 0  || n > 3)
({void*_tmp506=0;Cyc_Warn_err((unsigned int)(yyyylsp[2]).first_line,({const char*_tmp507="regparm requires value between 0 and 3";_tag_dyneither(_tmp507,sizeof(char),39);}),_tag_dyneither(_tmp506,sizeof(void*),0));});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp508=_cycalloc_atomic(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmp509;_tmp509.tag=0;_tmp509.f1=n;_tmp509;});_tmp508;});}else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp50A="initializes";_tag_dyneither(_tmp50A,sizeof(char),12);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp50B="__initializes__";_tag_dyneither(_tmp50B,sizeof(char),16);}))== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp50C=_cycalloc_atomic(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmp50D;_tmp50D.tag=20;_tmp50D.f1=n;_tmp50D;});_tmp50C;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp50E="noliveunique";_tag_dyneither(_tmp50E,sizeof(char),13);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp50F="__noliveunique__";_tag_dyneither(_tmp50F,sizeof(char),17);}))== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp510=_cycalloc_atomic(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmp511;_tmp511.tag=21;_tmp511.f1=n;_tmp511;});_tmp510;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp512="noconsume";_tag_dyneither(_tmp512,sizeof(char),10);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,({const char*_tmp513="__noconsume__";_tag_dyneither(_tmp513,sizeof(char),14);}))== 0)
a=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp514=_cycalloc_atomic(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmp515;_tmp515.tag=22;_tmp515.f1=n;_tmp515;});_tmp514;});else{
# 1575
({void*_tmp516=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp517="unrecognized attribute";_tag_dyneither(_tmp517,sizeof(char),23);}),_tag_dyneither(_tmp516,sizeof(void*),0));});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1579
yyval=Cyc_YY46(a);
# 1581
goto _LL254;}case 70: _LL2DB: _LL2DC: {
# 1583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 1585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 1582 "parse.y"
struct _dyneither_ptr _tmp518=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp519=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp51A=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_Int_tok(yyyyvsp[4]));
unsigned int _tmp51B=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[6]).first_line,Cyc_yyget_Int_tok(yyyyvsp[6]));
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp518,({const char*_tmp51C="format";_tag_dyneither(_tmp51C,sizeof(char),7);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp518,({const char*_tmp51D="__format__";_tag_dyneither(_tmp51D,sizeof(char),11);}))== 0){
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp519,({const char*_tmp51E="printf";_tag_dyneither(_tmp51E,sizeof(char),7);}))== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp51F=_cycalloc_atomic(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp520;_tmp520.tag=19;_tmp520.f1=Cyc_Absyn_Printf_ft;_tmp520.f2=(int)_tmp51A;_tmp520.f3=(int)_tmp51B;_tmp520;});_tmp51F;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp519,({const char*_tmp521="scanf";_tag_dyneither(_tmp521,sizeof(char),6);}))== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp522=_cycalloc_atomic(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp523;_tmp523.tag=19;_tmp523.f1=Cyc_Absyn_Scanf_ft;_tmp523.f2=(int)_tmp51A;_tmp523.f3=(int)_tmp51B;_tmp523;});_tmp522;});else{
# 1593
({void*_tmp524=0;Cyc_Warn_err((unsigned int)(yyyylsp[2]).first_line,({const char*_tmp525="unrecognized format type";_tag_dyneither(_tmp525,sizeof(char),25);}),_tag_dyneither(_tmp524,sizeof(void*),0));});}}}else{
# 1595
({void*_tmp526=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp527="unrecognized attribute";_tag_dyneither(_tmp527,sizeof(char),23);}),_tag_dyneither(_tmp526,sizeof(void*),0));});}
yyval=Cyc_YY46(a);
# 1598
goto _LL254;}case 71: _LL2DD: _LL2DE: {
# 1600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1607 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 72: _LL2DF: _LL2E0: {
# 1610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1609 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp529;_tmp529.tag=17;_tmp529.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp529.f2=Cyc_yyget_YY40(yyyyvsp[1]);_tmp529.f3=0;_tmp529.f4=0;_tmp529;});_tmp528;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 73: _LL2E1: _LL2E2: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1613 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 74: _LL2E3: _LL2E4: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1614 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 75: _LL2E5: _LL2E6: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1615 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 76: _LL2E7: _LL2E8: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1616 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 77: _LL2E9: _LL2EA: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1617 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 78: _LL2EB: _LL2EC: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1618 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 79: _LL2ED: _LL2EE: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1619 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 80: _LL2EF: _LL2F0: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1620 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 81: _LL2F1: _LL2F2: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1621 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 82: _LL2F3: _LL2F4: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1622 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 83: _LL2F5: _LL2F6: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1623 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 84: _LL2F7: _LL2F8: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1626 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp52B;_tmp52B.tag=27;_tmp52B.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp52B;});_tmp52A;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 85: _LL2F9: _LL2FA: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1628 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C[0]=({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmp52D;_tmp52D.tag=28;_tmp52D.f1=({const char*_tmp52E="__builtin_va_list";_tag_dyneither(_tmp52E,sizeof(char),18);});_tmp52D.f2=& Cyc_Tcutil_bk;_tmp52D;});_tmp52C;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 86: _LL2FB: _LL2FC: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1630 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 87: _LL2FD: _LL2FE: {
# 1633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1632 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 88: _LL2FF: _LL300: {
# 1635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1634 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 89: _LL301: _LL302: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1636 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 90: _LL303: _LL304: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1638 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp530;_tmp530.tag=10;_tmp530.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));_tmp530;});_tmp52F;}),(unsigned int)(yyyylsp[0]).first_line));
# 1641
goto _LL254;}case 91: _LL305: _LL306: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1642 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp532;_tmp532.tag=15;_tmp532.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp532;});_tmp531;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 92: _LL307: _LL308: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1644 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp534;_tmp534.tag=15;_tmp534.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);_tmp534;});_tmp533;}),(unsigned int)(yyyylsp[0]).first_line));
# 1646
goto _LL254;}case 93: _LL309: _LL30A: {
# 1648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1647 "parse.y"
void*_tmp535=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp537;_tmp537.tag=16;_tmp537.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp537.f2=_tmp535;_tmp537;});_tmp536;}),(unsigned int)(yyyylsp[0]).first_line));
# 1650
goto _LL254;}case 94: _LL30B: _LL30C: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 1651 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp539;_tmp539.tag=16;_tmp539.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp539.f2=Cyc_yyget_YY44(yyyyvsp[4]);_tmp539;});_tmp538;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 95: _LL30D: _LL30E: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1653 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53B;_tmp53B.tag=19;_tmp53B.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp53B;});_tmp53A;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 96: _LL30F: _LL310: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1655 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53D;_tmp53D.tag=19;_tmp53D.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0);_tmp53D;});_tmp53C;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 97: _LL311: _LL312: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1657 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp53F;_tmp53F.tag=18;_tmp53F.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp53F;});_tmp53E;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 98: _LL313: _LL314: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 99: _LL315: _LL316: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1667 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp540;_tmp540.print_const=1;_tmp540.q_volatile=0;_tmp540.q_restrict=0;_tmp540.real_const=1;_tmp540.loc=loc;_tmp540;}));
goto _LL254;}case 100: _LL317: _LL318: {
# 1671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1669 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp541;_tmp541.print_const=0;_tmp541.q_volatile=1;_tmp541.q_restrict=0;_tmp541.real_const=0;_tmp541.loc=0;_tmp541;}));
goto _LL254;}case 101: _LL319: _LL31A: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1670 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp542;_tmp542.print_const=0;_tmp542.q_volatile=0;_tmp542.q_restrict=1;_tmp542.real_const=0;_tmp542.loc=0;_tmp542;}));
goto _LL254;}case 102: _LL31B: _LL31C: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
struct Cyc_Absyn_TypeDecl*_tmp543=({struct Cyc_Absyn_TypeDecl*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->r=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547[0]=({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmp548;_tmp548.tag=1;_tmp548.f1=({struct Cyc_Absyn_Enumdecl*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->sc=Cyc_Absyn_Public;_tmp549->name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp549->fields=({struct Cyc_Core_Opt*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->v=Cyc_yyget_YY48(yyyyvsp[3]);_tmp54A;});_tmp549;});_tmp548;});_tmp547;});_tmp546->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp546;});
# 1678
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp545;_tmp545.tag=26;_tmp545.f1=_tmp543;_tmp545.f2=0;_tmp545;});_tmp544;}),(unsigned int)(yyyylsp[0]).first_line));
# 1680
goto _LL254;}case 103: _LL31D: _LL31E: {
# 1682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1681 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp54C;_tmp54C.tag=13;_tmp54C.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp54C.f2=0;_tmp54C;});_tmp54B;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 104: _LL31F: _LL320: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1683 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp54E;_tmp54E.tag=14;_tmp54E.f1=Cyc_yyget_YY48(yyyyvsp[2]);_tmp54E;});_tmp54D;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 105: _LL321: _LL322: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->name=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp54F->tag=0;_tmp54F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp54F;}));
goto _LL254;}case 106: _LL323: _LL324: {
# 1692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1691 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->name=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp550->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp550->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp550;}));
goto _LL254;}case 107: _LL325: _LL326: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1695 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->hd=Cyc_yyget_YY47(yyyyvsp[0]);_tmp551->tl=0;_tmp551;}));
goto _LL254;}case 108: _LL327: _LL328: {
# 1698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1696 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->hd=Cyc_yyget_YY47(yyyyvsp[0]);_tmp552->tl=0;_tmp552;}));
goto _LL254;}case 109: _LL329: _LL32A: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1697 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->hd=Cyc_yyget_YY47(yyyyvsp[0]);_tmp553->tl=Cyc_yyget_YY48(yyyyvsp[2]);_tmp553;}));
goto _LL254;}case 110: _LL32B: _LL32C: {
# 1700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp555;_tmp555.tag=12;_tmp555.f1=Cyc_yyget_YY22(yyyyvsp[0]);_tmp555.f2=Cyc_yyget_YY24(yyyyvsp[2]);_tmp555;});_tmp554;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 111: _LL32D: _LL32E: {
# 1706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9));
# 1708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9));
# 1709 "parse.y"
struct Cyc_List_List*_tmp556=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp557=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp558=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp556,
Cyc_Absyn_aggrdecl_impl(_tmp557,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1714
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp55A;_tmp55A.tag=26;_tmp55A.f1=_tmp558;_tmp55A.f2=0;_tmp55A;});_tmp559;}),(unsigned int)(yyyylsp[0]).first_line));
# 1716
goto _LL254;}case 112: _LL32F: _LL330: {
# 1718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 1720 "parse.y"
struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp55C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp55D=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp55B,
Cyc_Absyn_aggrdecl_impl(_tmp55C,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1725
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp55F;_tmp55F.tag=26;_tmp55F.f1=_tmp55D;_tmp55F.f2=0;_tmp55F;});_tmp55E;}),(unsigned int)(yyyylsp[0]).first_line));
# 1727
goto _LL254;}case 113: _LL331: _LL332: {
# 1729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1728 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp561;_tmp561.tag=11;_tmp561.f1=({struct Cyc_Absyn_AggrInfo _tmp562;_tmp562.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),({struct Cyc_Core_Opt*_tmp563=_cycalloc_atomic(sizeof(*_tmp563));_tmp563->v=(void*)1;_tmp563;}));_tmp562.targs=Cyc_yyget_YY40(yyyyvsp[3]);_tmp562;});_tmp561;});_tmp560;}),(unsigned int)(yyyylsp[0]).first_line));
# 1731
goto _LL254;}case 114: _LL333: _LL334: {
# 1733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1732 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp565;_tmp565.tag=11;_tmp565.f1=({struct Cyc_Absyn_AggrInfo _tmp566;_tmp566.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0);_tmp566.targs=Cyc_yyget_YY40(yyyyvsp[2]);_tmp566;});_tmp565;});_tmp564;}),(unsigned int)(yyyylsp[0]).first_line));
# 1735
goto _LL254;}case 115: _LL335: _LL336:
# 1737
 yyval=Cyc_YY40(0);
goto _LL254;case 116: _LL337: _LL338: {
# 1740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1741 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL254;}case 117: _LL339: _LL33A: {
# 1744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1745 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL254;}case 118: _LL33B: _LL33C: {
# 1748
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1750
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1746 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL254;}case 119: _LL33D: _LL33E:
# 1749
 yyval=Cyc_YY24(0);
goto _LL254;case 120: _LL33F: _LL340: {
# 1752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1753 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp567=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp567 != 0;_tmp567=_tmp567->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp567->hd,decls);}}{
# 1757
struct Cyc_List_List*_tmp568=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp568 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp568,decls);
yyval=Cyc_YY24(decls);
# 1762
goto _LL254;};}case 121: _LL341: _LL342: {
# 1764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->hd=Cyc_yyget_YY24(yyyyvsp[0]);_tmp569->tl=0;_tmp569;}));
goto _LL254;}case 122: _LL343: _LL344: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1769 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->hd=Cyc_yyget_YY24(yyyyvsp[1]);_tmp56A->tl=Cyc_yyget_YY25(yyyyvsp[0]);_tmp56A;}));
goto _LL254;}case 123: _LL345: _LL346: {
# 1772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1773 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
goto _LL254;}case 124: _LL347: _LL348: {
# 1776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY19(({struct _tuple12*_tmp56B=_region_malloc(yyr,sizeof(*_tmp56B));_tmp56B->tl=0;_tmp56B->hd=Cyc_yyget_YY18(yyyyvsp[0]);_tmp56B;}));
goto _LL254;}case 125: _LL349: _LL34A: {
# 1782
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1781 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp56C=_region_malloc(yyr,sizeof(*_tmp56C));_tmp56C->tl=Cyc_yyget_YY19(yyyyvsp[0]);_tmp56C->hd=Cyc_yyget_YY18(yyyyvsp[2]);_tmp56C;}));
goto _LL254;}case 126: _LL34B: _LL34C: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1786 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp56D;_tmp56D.f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp56D.f2=0;_tmp56D;}));
goto _LL254;}case 127: _LL34D: _LL34E: {
# 1789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1788 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp56E;_tmp56E.f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp56E.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp56E;}));
goto _LL254;}case 128: _LL34F: _LL350: {
# 1791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1794 "parse.y"
struct _RegionHandle _tmp56F=_new_region("temp");struct _RegionHandle*temp=& _tmp56F;_push_region(temp);
{struct _tuple25 _tmp570=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp571=_tmp570;struct Cyc_Absyn_Tqual _tmp57F;struct Cyc_Parse_Type_specifier _tmp57E;struct Cyc_List_List*_tmp57D;_LL68F: _tmp57F=_tmp571.f1;_tmp57E=_tmp571.f2;_tmp57D=_tmp571.f3;_LL690:;
if(_tmp57F.loc == 0)_tmp57F.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp572=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp572 != 0;_tmp572=_tmp572->tl){
struct _tuple24*_tmp573=(struct _tuple24*)_tmp572->hd;struct _tuple24*_tmp574=_tmp573;struct Cyc_Parse_Declarator _tmp57A;struct Cyc_Absyn_Exp*_tmp579;struct Cyc_Absyn_Exp*_tmp578;_LL692: _tmp57A=_tmp574->f1;_tmp579=_tmp574->f2;_tmp578=_tmp574->f3;_LL693:;
decls=({struct _tuple10*_tmp575=_region_malloc(temp,sizeof(*_tmp575));_tmp575->tl=decls;_tmp575->hd=_tmp57A;_tmp575;});
widths_and_reqs=({struct Cyc_List_List*_tmp576=_region_malloc(temp,sizeof(*_tmp576));_tmp576->hd=({struct _tuple16*_tmp577=_region_malloc(temp,sizeof(*_tmp577));_tmp577->f1=_tmp579;_tmp577->f2=_tmp578;_tmp577;});_tmp576->tl=widths_and_reqs;_tmp576;});}}
# 1805
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp57B=Cyc_Parse_speclist2typ(_tmp57E,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp57F,_tmp57B,decls,_tmp57D),widths_and_reqs);
# 1811
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp57C));
# 1813
_npop_handler(0);goto _LL254;};};}
# 1795
;_pop_region(temp);}case 129: _LL351: _LL352: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1821 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp580;_tmp580.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp580.f2=Cyc_yyget_YY21(yyyyvsp[0]);_tmp580.f3=0;_tmp580;}));
goto _LL254;}case 130: _LL353: _LL354: {
# 1824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1823 "parse.y"
struct _tuple25 _tmp581=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp582;_tmp582.f1=_tmp581.f1;_tmp582.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp581.f2);_tmp582.f3=_tmp581.f3;_tmp582;}));
goto _LL254;}case 131: _LL355: _LL356: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1825 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp583;_tmp583.f1=Cyc_yyget_YY23(yyyyvsp[0]);_tmp583.f2=Cyc_Parse_empty_spec(0);_tmp583.f3=0;_tmp583;}));
goto _LL254;}case 132: _LL357: _LL358: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1827 "parse.y"
struct _tuple25 _tmp584=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp585;_tmp585.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp584.f1);_tmp585.f2=_tmp584.f2;_tmp585.f3=_tmp584.f3;_tmp585;}));
goto _LL254;}case 133: _LL359: _LL35A: {
# 1831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1830 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp586;_tmp586.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp586.f2=Cyc_Parse_empty_spec(0);_tmp586.f3=Cyc_yyget_YY45(yyyyvsp[0]);_tmp586;}));
goto _LL254;}case 134: _LL35B: _LL35C: {
# 1833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1832 "parse.y"
struct _tuple25 _tmp587=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp588;_tmp588.f1=_tmp587.f1;_tmp588.f2=_tmp587.f2;_tmp588.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp587.f3);_tmp588;}));
goto _LL254;}case 135: _LL35D: _LL35E: {
# 1835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY35(({struct _tuple25 _tmp589;_tmp589.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp589.f2=Cyc_yyget_YY21(yyyyvsp[0]);_tmp589.f3=0;_tmp589;}));
goto _LL254;}case 136: _LL35F: _LL360: {
# 1841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1840 "parse.y"
struct _tuple25 _tmp58A=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp58B;_tmp58B.f1=_tmp58A.f1;_tmp58B.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp58A.f2);_tmp58B.f3=_tmp58A.f3;_tmp58B;}));
goto _LL254;}case 137: _LL361: _LL362: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1842 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp58C;_tmp58C.f1=Cyc_yyget_YY23(yyyyvsp[0]);_tmp58C.f2=Cyc_Parse_empty_spec(0);_tmp58C.f3=0;_tmp58C;}));
goto _LL254;}case 138: _LL363: _LL364: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1844 "parse.y"
struct _tuple25 _tmp58D=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp58E;_tmp58E.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp58D.f1);_tmp58E.f2=_tmp58D.f2;_tmp58E.f3=_tmp58D.f3;_tmp58E;}));
goto _LL254;}case 139: _LL365: _LL366: {
# 1848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1847 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp58F;_tmp58F.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp58F.f2=Cyc_Parse_empty_spec(0);_tmp58F.f3=Cyc_yyget_YY45(yyyyvsp[0]);_tmp58F;}));
goto _LL254;}case 140: _LL367: _LL368: {
# 1850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1849 "parse.y"
struct _tuple25 _tmp590=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp591;_tmp591.f1=_tmp590.f1;_tmp591.f2=_tmp590.f2;_tmp591.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp590.f3);_tmp591;}));
goto _LL254;}case 141: _LL369: _LL36A: {
# 1852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1853 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
goto _LL254;}case 142: _LL36B: _LL36C: {
# 1856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp592=_region_malloc(yyr,sizeof(*_tmp592));_tmp592->hd=Cyc_yyget_YY28(yyyyvsp[0]);_tmp592->tl=0;_tmp592;}));
goto _LL254;}case 143: _LL36D: _LL36E: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1861 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp593=_region_malloc(yyr,sizeof(*_tmp593));_tmp593->hd=Cyc_yyget_YY28(yyyyvsp[2]);_tmp593->tl=Cyc_yyget_YY29(yyyyvsp[0]);_tmp593;}));
goto _LL254;}case 144: _LL36F: _LL370: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1866 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp594=_region_malloc(yyr,sizeof(*_tmp594));_tmp594->f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp594->f2=0;_tmp594->f3=Cyc_yyget_YY55(yyyyvsp[1]);_tmp594;}));
goto _LL254;}case 145: _LL371: _LL372: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1870 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp595=_region_malloc(yyr,sizeof(*_tmp595));_tmp595->f1=({struct Cyc_Parse_Declarator _tmp596;_tmp596.id=({struct _tuple0*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->f1=Cyc_Absyn_Rel_n(0);_tmp597->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);_tmp597;});_tmp596.tms=0;_tmp596;});_tmp595->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp595->f3=0;_tmp595;}));
# 1872
goto _LL254;}case 146: _LL373: _LL374:
# 1876
 yyval=Cyc_YY28(({struct _tuple24*_tmp599=_region_malloc(yyr,sizeof(*_tmp599));_tmp599->f1=({struct Cyc_Parse_Declarator _tmp59A;_tmp59A.id=({struct _tuple0*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->f1=Cyc_Absyn_Rel_n(0);_tmp59B->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);_tmp59B;});_tmp59A.tms=0;_tmp59A;});_tmp599->f2=0;_tmp599->f3=0;_tmp599;}));
# 1878
goto _LL254;case 147: _LL375: _LL376: {
# 1880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1878 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp59D=_region_malloc(yyr,sizeof(*_tmp59D));_tmp59D->f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp59D->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp59D->f3=0;_tmp59D;}));
goto _LL254;}case 148: _LL377: _LL378: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1882 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL254;}case 149: _LL379: _LL37A:
# 1885
 yyval=Cyc_YY55(0);
goto _LL254;case 150: _LL37B: _LL37C: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1887 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL254;}case 151: _LL37D: _LL37E:
# 1890
 yyval=Cyc_YY55(0);
goto _LL254;case 152: _LL37F: _LL380: {
# 1893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 1895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 1894 "parse.y"
int _tmp59E=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Absyn_TypeDecl*_tmp5A0=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp59F,({struct Cyc_Core_Opt*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->v=Cyc_yyget_YY34(yyyyvsp[4]);_tmp5A3;}),_tmp59E,(unsigned int)(yyyylsp[0]).first_line);
# 1898
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp5A2;_tmp5A2.tag=26;_tmp5A2.f1=_tmp5A0;_tmp5A2.f2=0;_tmp5A2;});_tmp5A1;}),(unsigned int)(yyyylsp[0]).first_line));
# 1900
goto _LL254;}case 153: _LL381: _LL382: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1901 "parse.y"
int _tmp5A4=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp5A6;_tmp5A6.tag=3;_tmp5A6.f1=({struct Cyc_Absyn_DatatypeInfo _tmp5A7;_tmp5A7.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5A8;_tmp5A8.name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5A8.is_extensible=_tmp5A4;_tmp5A8;}));_tmp5A7.targs=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5A7;});_tmp5A6;});_tmp5A5;}),(unsigned int)(yyyylsp[0]).first_line));
# 1904
goto _LL254;}case 154: _LL383: _LL384: {
# 1906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1905 "parse.y"
int _tmp5A9=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp5AB;_tmp5AB.tag=4;_tmp5AB.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp5AC;_tmp5AC.field_info=
Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp5AD;_tmp5AD.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5AD.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp5AD.is_extensible=_tmp5A9;_tmp5AD;}));_tmp5AC.targs=Cyc_yyget_YY40(yyyyvsp[4]);_tmp5AC;});_tmp5AB;});_tmp5AA;}),(unsigned int)(yyyylsp[0]).first_line));
# 1910
goto _LL254;}case 155: _LL385: _LL386: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1913 "parse.y"
yyval=Cyc_YY31(0);
goto _LL254;}case 156: _LL387: _LL388: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1914 "parse.y"
yyval=Cyc_YY31(1);
goto _LL254;}case 157: _LL389: _LL38A: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1918 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5AE->tl=0;_tmp5AE;}));
goto _LL254;}case 158: _LL38B: _LL38C: {
# 1921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1919 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5AF->tl=0;_tmp5AF;}));
goto _LL254;}case 159: _LL38D: _LL38E: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1920 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B0->tl=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5B0;}));
goto _LL254;}case 160: _LL38F: _LL390: {
# 1923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1921 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B1->tl=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5B1;}));
goto _LL254;}case 161: _LL391: _LL392:
# 1924
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL254;case 162: _LL393: _LL394: {
# 1927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1926 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL254;}case 163: _LL395: _LL396: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1927 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL254;}case 164: _LL397: _LL398: {
# 1930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1931 "parse.y"
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B2->typs=0;_tmp5B2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5B2->sc=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5B2;}));
goto _LL254;}case 165: _LL399: _LL39A: {
# 1934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1933 "parse.y"
struct Cyc_List_List*_tmp5B3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B4->typs=_tmp5B3;_tmp5B4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5B4->sc=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5B4;}));
goto _LL254;}case 166: _LL39B: _LL39C: {
# 1937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1939 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 167: _LL39D: _LL39E: {
# 1942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1941 "parse.y"
struct Cyc_Parse_Declarator _tmp5B5=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B6;_tmp5B6.id=_tmp5B5.id;_tmp5B6.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5B5.tms);_tmp5B6;}));
goto _LL254;}case 168: _LL39F: _LL3A0: {
# 1945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=yyyyvsp[0];
goto _LL254;}case 169: _LL3A1: _LL3A2: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1950 "parse.y"
struct Cyc_Parse_Declarator _tmp5B7=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B8;_tmp5B8.id=_tmp5B7.id;_tmp5B8.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5B7.tms);_tmp5B8;}));
goto _LL254;}case 170: _LL3A3: _LL3A4: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 1956 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B9;_tmp5B9.id=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5B9.tms=0;_tmp5B9;}));
goto _LL254;}case 171: _LL3A5: _LL3A6: {
# 1959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 1958 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 172: _LL3A7: _LL3A8: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1962 "parse.y"
struct Cyc_Parse_Declarator _tmp5BA=Cyc_yyget_YY27(yyyyvsp[2]);
_tmp5BA.tms=({struct Cyc_List_List*_tmp5BB=_region_malloc(yyr,sizeof(*_tmp5BB));_tmp5BB->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5BC=_region_malloc(yyr,sizeof(*_tmp5BC));_tmp5BC[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5BD;_tmp5BD.tag=5;_tmp5BD.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp5BD.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5BD;});_tmp5BC;});_tmp5BB->tl=_tmp5BA.tms;_tmp5BB;});
yyval=yyyyvsp[2];
# 1966
goto _LL254;}case 173: _LL3A9: _LL3AA: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1967 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5BE;_tmp5BE.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5BE.tms=({struct Cyc_List_List*_tmp5BF=_region_malloc(yyr,sizeof(*_tmp5BF));_tmp5BF->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C0=_region_malloc(yyr,sizeof(*_tmp5C0));_tmp5C0[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5C1;_tmp5C1.tag=0;_tmp5C1.f1=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C1.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5C1;});_tmp5C0;});_tmp5BF->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5BF;});_tmp5BE;}));
goto _LL254;}case 174: _LL3AB: _LL3AC: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 1969 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C2;_tmp5C2.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C2.tms=({struct Cyc_List_List*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));_tmp5C3->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5C5;_tmp5C5.tag=1;_tmp5C5.f1=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5C5.f2=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5C5.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5C5;});_tmp5C4;});_tmp5C3->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C3;});_tmp5C2;}));
goto _LL254;}case 175: _LL3AD: _LL3AE: {
# 1973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 1975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 1972 "parse.y"
struct _tuple26*_tmp5C6=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5C7=_tmp5C6;struct Cyc_List_List*_tmp5D4;int _tmp5D3;struct Cyc_Absyn_VarargInfo*_tmp5D2;void*_tmp5D1;struct Cyc_List_List*_tmp5D0;_LL695: _tmp5D4=_tmp5C7->f1;_tmp5D3=_tmp5C7->f2;_tmp5D2=_tmp5C7->f3;_tmp5D1=_tmp5C7->f4;_tmp5D0=_tmp5C7->f5;_LL696:;{
struct Cyc_Absyn_Exp*_tmp5C8=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5C9=Cyc_yyget_YY55(yyyyvsp[5]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5CA;_tmp5CA.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5CA.tms=({struct Cyc_List_List*_tmp5CB=_region_malloc(yyr,sizeof(*_tmp5CB));_tmp5CB->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CC=_region_malloc(yyr,sizeof(*_tmp5CC));_tmp5CC[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5CD;_tmp5CD.tag=3;_tmp5CD.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5CE=_region_malloc(yyr,sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5CF;_tmp5CF.tag=1;_tmp5CF.f1=_tmp5D4;_tmp5CF.f2=_tmp5D3;_tmp5CF.f3=_tmp5D2;_tmp5CF.f4=_tmp5D1;_tmp5CF.f5=_tmp5D0;_tmp5CF.f6=_tmp5C8;_tmp5CF.f7=_tmp5C9;_tmp5CF;});_tmp5CE;});_tmp5CD;});_tmp5CC;});_tmp5CB->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5CB;});_tmp5CA;}));
# 1977
goto _LL254;};}case 176: _LL3AF: _LL3B0: {
# 1979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 1978 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5D5;_tmp5D5.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5D5.tms=({struct Cyc_List_List*_tmp5D6=_region_malloc(yyr,sizeof(*_tmp5D6));_tmp5D6->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5D7=_region_malloc(yyr,sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5D8;_tmp5D8.tag=3;_tmp5D8.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5D9=_region_malloc(yyr,sizeof(*_tmp5D9));_tmp5D9[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5DA;_tmp5DA.tag=1;_tmp5DA.f1=0;_tmp5DA.f2=0;_tmp5DA.f3=0;_tmp5DA.f4=
# 1981
Cyc_yyget_YY49(yyyyvsp[2]);_tmp5DA.f5=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5DA.f6=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5DA.f7=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5DA;});_tmp5D9;});_tmp5D8;});_tmp5D7;});_tmp5D6->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5D6;});_tmp5D5;}));
# 1984
goto _LL254;}case 177: _LL3B1: _LL3B2: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1985 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5DB;_tmp5DB.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5DB.tms=({struct Cyc_List_List*_tmp5DC=_region_malloc(yyr,sizeof(*_tmp5DC));_tmp5DC->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5DD=_region_malloc(yyr,sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5DE;_tmp5DE.tag=3;_tmp5DE.f1=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp5DF=_region_malloc(yyr,sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp5E0;_tmp5E0.tag=0;_tmp5E0.f1=Cyc_yyget_YY36(yyyyvsp[2]);_tmp5E0.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5E0;});_tmp5DF;});_tmp5DE;});_tmp5DD;});_tmp5DC->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5DC;});_tmp5DB;}));
goto _LL254;}case 178: _LL3B3: _LL3B4: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 1988 "parse.y"
struct Cyc_List_List*_tmp5E1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E2;_tmp5E2.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5E2.tms=({struct Cyc_List_List*_tmp5E3=_region_malloc(yyr,sizeof(*_tmp5E3));_tmp5E3->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5E4=_region_malloc(yyr,sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp5E5;_tmp5E5.tag=4;_tmp5E5.f1=_tmp5E1;_tmp5E5.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5E5.f3=0;_tmp5E5;});_tmp5E4;});_tmp5E3->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5E3;});_tmp5E2;}));
# 1991
goto _LL254;}case 179: _LL3B5: _LL3B6: {
# 1993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 1995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 1992 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E6;_tmp5E6.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5E6.tms=({struct Cyc_List_List*_tmp5E7=_region_malloc(yyr,sizeof(*_tmp5E7));_tmp5E7->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5E8=_region_malloc(yyr,sizeof(*_tmp5E8));_tmp5E8[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5E9;_tmp5E9.tag=5;_tmp5E9.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp5E9.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5E9;});_tmp5E8;});_tmp5E7->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5E7;});_tmp5E6;}));
# 1995
goto _LL254;}case 180: _LL3B7: _LL3B8: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2006 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5EA;_tmp5EA.id=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5EA.tms=0;_tmp5EA;}));
goto _LL254;}case 181: _LL3B9: _LL3BA: {
# 2009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2008 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5EB;_tmp5EB.id=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5EB.tms=0;_tmp5EB;}));
goto _LL254;}case 182: _LL3BB: _LL3BC: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2010 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 183: _LL3BD: _LL3BE: {
# 2013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2014 "parse.y"
struct Cyc_Parse_Declarator _tmp5EC=Cyc_yyget_YY27(yyyyvsp[2]);
_tmp5EC.tms=({struct Cyc_List_List*_tmp5ED=_region_malloc(yyr,sizeof(*_tmp5ED));_tmp5ED->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5EE=_region_malloc(yyr,sizeof(*_tmp5EE));_tmp5EE[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5EF;_tmp5EF.tag=5;_tmp5EF.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp5EF.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5EF;});_tmp5EE;});_tmp5ED->tl=_tmp5EC.tms;_tmp5ED;});
yyval=yyyyvsp[2];
# 2018
goto _LL254;}case 184: _LL3BF: _LL3C0: {
# 2020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2019 "parse.y"
struct Cyc_Parse_Declarator _tmp5F0=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F1;_tmp5F1.id=_tmp5F0.id;_tmp5F1.tms=({struct Cyc_List_List*_tmp5F2=_region_malloc(yyr,sizeof(*_tmp5F2));_tmp5F2->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5F3=_region_malloc(yyr,sizeof(*_tmp5F3));_tmp5F3[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5F4;_tmp5F4.tag=0;_tmp5F4.f1=
Cyc_yyget_YY51(yyyyvsp[3]);_tmp5F4.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5F4;});_tmp5F3;});_tmp5F2->tl=_tmp5F0.tms;_tmp5F2;});_tmp5F1;}));
goto _LL254;}case 185: _LL3C1: _LL3C2: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2023 "parse.y"
struct Cyc_Parse_Declarator _tmp5F5=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F6;_tmp5F6.id=_tmp5F5.id;_tmp5F6.tms=({struct Cyc_List_List*_tmp5F7=_region_malloc(yyr,sizeof(*_tmp5F7));_tmp5F7->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5F8=_region_malloc(yyr,sizeof(*_tmp5F8));_tmp5F8[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5F9;_tmp5F9.tag=1;_tmp5F9.f1=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5F9.f2=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5F9.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5F9;});_tmp5F8;});_tmp5F7->tl=_tmp5F5.tms;_tmp5F7;});_tmp5F6;}));
# 2027
goto _LL254;}case 186: _LL3C3: _LL3C4: {
# 2029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2028 "parse.y"
struct _tuple26*_tmp5FA=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5FB=_tmp5FA;struct Cyc_List_List*_tmp609;int _tmp608;struct Cyc_Absyn_VarargInfo*_tmp607;void*_tmp606;struct Cyc_List_List*_tmp605;_LL698: _tmp609=_tmp5FB->f1;_tmp608=_tmp5FB->f2;_tmp607=_tmp5FB->f3;_tmp606=_tmp5FB->f4;_tmp605=_tmp5FB->f5;_LL699:;{
struct Cyc_Absyn_Exp*_tmp5FC=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp5FE=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5FF;_tmp5FF.id=_tmp5FE.id;_tmp5FF.tms=({struct Cyc_List_List*_tmp600=_region_malloc(yyr,sizeof(*_tmp600));_tmp600->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp601=_region_malloc(yyr,sizeof(*_tmp601));_tmp601[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp602;_tmp602.tag=3;_tmp602.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp603=_region_malloc(yyr,sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp604;_tmp604.tag=1;_tmp604.f1=_tmp609;_tmp604.f2=_tmp608;_tmp604.f3=_tmp607;_tmp604.f4=_tmp606;_tmp604.f5=_tmp605;_tmp604.f6=_tmp5FC;_tmp604.f7=_tmp5FD;_tmp604;});_tmp603;});_tmp602;});_tmp601;});_tmp600->tl=_tmp5FE.tms;_tmp600;});_tmp5FF;}));
# 2034
goto _LL254;};}case 187: _LL3C5: _LL3C6: {
# 2036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2035 "parse.y"
struct Cyc_Parse_Declarator _tmp60A=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp60B;_tmp60B.id=_tmp60A.id;_tmp60B.tms=({struct Cyc_List_List*_tmp60C=_region_malloc(yyr,sizeof(*_tmp60C));_tmp60C->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp60D=_region_malloc(yyr,sizeof(*_tmp60D));_tmp60D[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp60E;_tmp60E.tag=3;_tmp60E.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp60F=_region_malloc(yyr,sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp610;_tmp610.tag=1;_tmp610.f1=0;_tmp610.f2=0;_tmp610.f3=0;_tmp610.f4=
# 2039
Cyc_yyget_YY49(yyyyvsp[2]);_tmp610.f5=Cyc_yyget_YY50(yyyyvsp[3]);_tmp610.f6=Cyc_yyget_YY55(yyyyvsp[5]);_tmp610.f7=Cyc_yyget_YY55(yyyyvsp[6]);_tmp610;});_tmp60F;});_tmp60E;});_tmp60D;});_tmp60C->tl=_tmp60A.tms;_tmp60C;});_tmp60B;}));
# 2042
goto _LL254;}case 188: _LL3C7: _LL3C8: {
# 2044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2043 "parse.y"
struct Cyc_Parse_Declarator _tmp611=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp612;_tmp612.id=_tmp611.id;_tmp612.tms=({struct Cyc_List_List*_tmp613=_region_malloc(yyr,sizeof(*_tmp613));_tmp613->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp614=_region_malloc(yyr,sizeof(*_tmp614));_tmp614[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp615;_tmp615.tag=3;_tmp615.f1=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp616=_region_malloc(yyr,sizeof(*_tmp616));_tmp616[0]=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp617;_tmp617.tag=0;_tmp617.f1=Cyc_yyget_YY36(yyyyvsp[2]);_tmp617.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp617;});_tmp616;});_tmp615;});_tmp614;});_tmp613->tl=_tmp611.tms;_tmp613;});_tmp612;}));
goto _LL254;}case 189: _LL3C9: _LL3CA: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2047 "parse.y"
struct Cyc_List_List*_tmp618=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp619=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp61A;_tmp61A.id=_tmp619.id;_tmp61A.tms=({struct Cyc_List_List*_tmp61B=_region_malloc(yyr,sizeof(*_tmp61B));_tmp61B->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp61C=_region_malloc(yyr,sizeof(*_tmp61C));_tmp61C[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp61D;_tmp61D.tag=4;_tmp61D.f1=_tmp618;_tmp61D.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp61D.f3=0;_tmp61D;});_tmp61C;});_tmp61B->tl=_tmp619.tms;_tmp61B;});_tmp61A;}));
# 2051
goto _LL254;}case 190: _LL3CB: _LL3CC: {
# 2053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2052 "parse.y"
struct Cyc_Parse_Declarator _tmp61E=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp61F;_tmp61F.id=_tmp61E.id;_tmp61F.tms=({struct Cyc_List_List*_tmp620=_region_malloc(yyr,sizeof(*_tmp620));_tmp620->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp621=_region_malloc(yyr,sizeof(*_tmp621));_tmp621[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp622;_tmp622.tag=5;_tmp622.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp622.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp622;});_tmp621;});_tmp620->tl=_tmp61E.tms;_tmp620;});_tmp61F;}));
# 2055
goto _LL254;}case 191: _LL3CD: _LL3CE: {
# 2057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2065 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 192: _LL3CF: _LL3D0: {
# 2068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2066 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
goto _LL254;}case 193: _LL3D1: _LL3D2: {
# 2069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2070 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
ans=({struct Cyc_List_List*_tmp623=_region_malloc(yyr,sizeof(*_tmp623));_tmp623->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp624=_region_malloc(yyr,sizeof(*_tmp624));_tmp624[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp625;_tmp625.tag=5;_tmp625.f1=(unsigned int)(yyyylsp[3]).first_line;_tmp625.f2=Cyc_yyget_YY45(yyyyvsp[3]);_tmp625;});_tmp624;});_tmp623->tl=ans;_tmp623;});{
# 2074
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp626=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp627=_tmp626;unsigned int _tmp635;union Cyc_Absyn_Constraint*_tmp634;union Cyc_Absyn_Constraint*_tmp633;_LL69B: _tmp635=_tmp627.f1;_tmp634=_tmp627.f2;_tmp633=_tmp627.f3;_LL69C:;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp628=_cycalloc_atomic(sizeof(*_tmp628));_tmp628->ptr_loc=_tmp635;_tmp628->rgn_loc=(unsigned int)(yyyylsp[2]).first_line;_tmp628->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp628;});{
# 2079
struct _tuple14 _tmp629=Cyc_Parse_collapse_pointer_quals(_tmp635,_tmp634,_tmp633,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));struct _tuple14 _tmp62A=_tmp629;union Cyc_Absyn_Constraint*_tmp632;union Cyc_Absyn_Constraint*_tmp631;union Cyc_Absyn_Constraint*_tmp630;void*_tmp62F;_LL69E: _tmp632=_tmp62A.f1;_tmp631=_tmp62A.f2;_tmp630=_tmp62A.f3;_tmp62F=_tmp62A.f4;_LL69F:;
ans=({struct Cyc_List_List*_tmp62B=_region_malloc(yyr,sizeof(*_tmp62B));_tmp62B->hd=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp62C=_region_malloc(yyr,sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp62D;_tmp62D.tag=2;_tmp62D.f1=({struct Cyc_Absyn_PtrAtts _tmp62E;_tmp62E.rgn=_tmp62F;_tmp62E.nullable=_tmp632;_tmp62E.bounds=_tmp631;_tmp62E.zero_term=_tmp630;_tmp62E.ptrloc=ptrloc;_tmp62E;});_tmp62D.f2=Cyc_yyget_YY23(yyyyvsp[4]);_tmp62D;});_tmp62C;});_tmp62B->tl=ans;_tmp62B;});
yyval=Cyc_YY26(ans);
# 2083
goto _LL254;};};}case 194: _LL3D3: _LL3D4:
# 2085
 yyval=Cyc_YY54(0);
goto _LL254;case 195: _LL3D5: _LL3D6: {
# 2088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2086 "parse.y"
yyval=Cyc_YY54(({struct Cyc_List_List*_tmp636=_region_malloc(yyr,sizeof(*_tmp636));_tmp636->hd=Cyc_yyget_YY53(yyyyvsp[0]);_tmp636->tl=Cyc_yyget_YY54(yyyyvsp[1]);_tmp636;}));
goto _LL254;}case 196: _LL3D7: _LL3D8: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2091 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp637=_region_malloc(yyr,sizeof(*_tmp637));_tmp637[0]=({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct _tmp638;_tmp638.tag=0;_tmp638.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp638;});_tmp637;}));
goto _LL254;}case 197: _LL3D9: _LL3DA: {
# 2094
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2093 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp639=_region_malloc(yyr,sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct _tmp63A;_tmp63A.tag=1;_tmp63A.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp63A;});_tmp639;}));
goto _LL254;}case 198: _LL3DB: _LL3DC: {
# 2096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2095 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp63B=_region_malloc(yyr,sizeof(*_tmp63B));_tmp63B[0]=({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct _tmp63C;_tmp63C.tag=2;_tmp63C;});_tmp63B;}));
goto _LL254;}case 199: _LL3DD: _LL3DE: {
# 2098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2097 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp63D=_region_malloc(yyr,sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct _tmp63E;_tmp63E.tag=3;_tmp63E;});_tmp63D;}));
goto _LL254;}case 200: _LL3DF: _LL3E0: {
# 2100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2099 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp63F=_region_malloc(yyr,sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct _tmp640;_tmp640.tag=4;_tmp640;});_tmp63F;}));
goto _LL254;}case 201: _LL3E1: _LL3E2: {
# 2102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2101 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp641=_region_malloc(yyr,sizeof(*_tmp641));_tmp641[0]=({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct _tmp642;_tmp642.tag=5;_tmp642;});_tmp641;}));
goto _LL254;}case 202: _LL3E3: _LL3E4: {
# 2104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2103 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp643=_region_malloc(yyr,sizeof(*_tmp643));_tmp643[0]=({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct _tmp644;_tmp644.tag=6;_tmp644;});_tmp643;}));
goto _LL254;}case 203: _LL3E5: _LL3E6: {
# 2106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2105 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp645=_region_malloc(yyr,sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct _tmp646;_tmp646.tag=7;_tmp646;});_tmp645;}));
goto _LL254;}case 204: _LL3E7: _LL3E8: {
# 2108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2111 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->f1=loc;_tmp647->f2=Cyc_Absyn_true_conref;_tmp647->f3=Cyc_yyget_YY2(yyyyvsp[1]);_tmp647;}));
# 2114
goto _LL254;}case 205: _LL3E9: _LL3EA: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2115 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=loc;_tmp648->f2=Cyc_Absyn_false_conref;_tmp648->f3=Cyc_yyget_YY2(yyyyvsp[1]);_tmp648;}));
# 2118
goto _LL254;}case 206: _LL3EB: _LL3EC: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2119 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=loc;_tmp649->f2=Cyc_Absyn_true_conref;_tmp649->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp649;}));
# 2122
goto _LL254;}case 207: _LL3ED: _LL3EE:
# 2124
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
goto _LL254;case 208: _LL3EF: _LL3F0: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2125 "parse.y"
yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp64B;_tmp64B.tag=1;_tmp64B.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp64B;});_tmp64A;})));
goto _LL254;}case 209: _LL3F1: _LL3F2:
# 2128
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
goto _LL254;case 210: _LL3F3: _LL3F4: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2129 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
goto _LL254;}case 211: _LL3F5: _LL3F6: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2130 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
goto _LL254;}case 212: _LL3F7: _LL3F8:
# 2133
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL254;case 213: _LL3F9: _LL3FA: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2136 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL254;}case 214: _LL3FB: _LL3FC: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2137 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL254;}case 215: _LL3FD: _LL3FE:
# 2140
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
goto _LL254;case 216: _LL3FF: _LL400: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2142 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
goto _LL254;}case 217: _LL401: _LL402: {
# 2145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2147 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp64C->f2=0;_tmp64C->f3=0;_tmp64C->f4=Cyc_yyget_YY49(yyyyvsp[1]);_tmp64C->f5=Cyc_yyget_YY50(yyyyvsp[2]);_tmp64C;}));
goto _LL254;}case 218: _LL403: _LL404: {
# 2150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2149 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp64D->f2=1;_tmp64D->f3=0;_tmp64D->f4=Cyc_yyget_YY49(yyyyvsp[3]);_tmp64D->f5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp64D;}));
goto _LL254;}case 219: _LL405: _LL406: {
# 2152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2152 "parse.y"
struct _tuple8*_tmp64E=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp64F=_tmp64E;struct _dyneither_ptr*_tmp655;struct Cyc_Absyn_Tqual _tmp654;void*_tmp653;_LL6A1: _tmp655=_tmp64F->f1;_tmp654=_tmp64F->f2;_tmp653=_tmp64F->f3;_LL6A2:;{
struct Cyc_Absyn_VarargInfo*_tmp650=({struct Cyc_Absyn_VarargInfo*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->name=_tmp655;_tmp652->tq=_tmp654;_tmp652->type=_tmp653;_tmp652->inject=Cyc_yyget_YY31(yyyyvsp[1]);_tmp652;});
yyval=Cyc_YY39(({struct _tuple26*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->f1=0;_tmp651->f2=0;_tmp651->f3=_tmp650;_tmp651->f4=Cyc_yyget_YY49(yyyyvsp[3]);_tmp651->f5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp651;}));
# 2156
goto _LL254;};}case 220: _LL407: _LL408: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2158 "parse.y"
struct _tuple8*_tmp656=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp657=_tmp656;struct _dyneither_ptr*_tmp65D;struct Cyc_Absyn_Tqual _tmp65C;void*_tmp65B;_LL6A4: _tmp65D=_tmp657->f1;_tmp65C=_tmp657->f2;_tmp65B=_tmp657->f3;_LL6A5:;{
struct Cyc_Absyn_VarargInfo*_tmp658=({struct Cyc_Absyn_VarargInfo*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->name=_tmp65D;_tmp65A->tq=_tmp65C;_tmp65A->type=_tmp65B;_tmp65A->inject=Cyc_yyget_YY31(yyyyvsp[3]);_tmp65A;});
yyval=Cyc_YY39(({struct _tuple26*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp659->f2=0;_tmp659->f3=_tmp658;_tmp659->f4=Cyc_yyget_YY49(yyyyvsp[5]);_tmp659->f5=Cyc_yyget_YY50(yyyyvsp[6]);_tmp659;}));
# 2162
goto _LL254;};}case 221: _LL409: _LL40A: {
# 2164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2166 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp65F;_tmp65F.tag=1;_tmp65F.f1=0;_tmp65F;});_tmp65E;})));
goto _LL254;}case 222: _LL40B: _LL40C: {
# 2169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2167 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
goto _LL254;}case 223: _LL40D: _LL40E:
# 2170
 yyval=Cyc_YY49(0);
goto _LL254;case 224: _LL40F: _LL410: {
# 2173
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2171 "parse.y"
yyval=Cyc_YY49((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp661;_tmp661.tag=24;_tmp661.f1=Cyc_yyget_YY40(yyyyvsp[1]);_tmp661;});_tmp660;}));
goto _LL254;}case 225: _LL411: _LL412:
# 2174
 yyval=Cyc_YY50(0);
goto _LL254;case 226: _LL413: _LL414: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2176 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 227: _LL415: _LL416: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2184 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp662=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp66A;_tmp66A.tag=2;_tmp66A.f1=0;_tmp66A.f2=& Cyc_Tcutil_trk;_tmp66A;});_tmp669;});
struct _dyneither_ptr _tmp663=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp664=Cyc_Parse_id2type(_tmp663,(void*)_tmp662);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->hd=({struct _tuple29*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->f1=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp668;_tmp668.tag=24;_tmp668.f1=Cyc_yyget_YY40(yyyyvsp[0]);_tmp668;});_tmp667;});_tmp666->f2=_tmp664;_tmp666;});_tmp665->tl=0;_tmp665;}));
# 2189
goto _LL254;}case 228: _LL417: _LL418: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2191 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66B=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp673;_tmp673.tag=2;_tmp673.f1=0;_tmp673.f2=& Cyc_Tcutil_trk;_tmp673;});_tmp672;});
struct _dyneither_ptr _tmp66C=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp66D=Cyc_Parse_id2type(_tmp66C,(void*)_tmp66B);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->hd=({struct _tuple29*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->f1=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp671;_tmp671.tag=24;_tmp671.f1=Cyc_yyget_YY40(yyyyvsp[0]);_tmp671;});_tmp670;});_tmp66F->f2=_tmp66D;_tmp66F;});_tmp66E->tl=Cyc_yyget_YY50(yyyyvsp[4]);_tmp66E;}));
# 2196
goto _LL254;}case 229: _LL419: _LL41A:
# 2198
 yyval=Cyc_YY31(0);
goto _LL254;case 230: _LL41B: _LL41C: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2202 "parse.y"
if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),({const char*_tmp674="inject";_tag_dyneither(_tmp674,sizeof(char),7);}))!= 0)
({void*_tmp675=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp676="missing type in function declaration";_tag_dyneither(_tmp676,sizeof(char),37);}),_tag_dyneither(_tmp675,sizeof(void*),0));});
yyval=Cyc_YY31(1);
# 2206
goto _LL254;}case 231: _LL41D: _LL41E: {
# 2208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2209 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 232: _LL41F: _LL420: {
# 2212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2210 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
goto _LL254;}case 233: _LL421: _LL422: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2214 "parse.y"
yyval=Cyc_YY40(0);
goto _LL254;}case 234: _LL423: _LL424: {
# 2217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2215 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 235: _LL425: _LL426: {
# 2218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2217 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp679;_tmp679.tag=25;_tmp679.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp679;});_tmp678;});_tmp677->tl=0;_tmp677;}));
goto _LL254;}case 236: _LL427: _LL428: {
# 2220
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2219 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->hd=Cyc_yyget_YY44(yyyyvsp[0]);_tmp67A->tl=0;_tmp67A;}));
# 2222
goto _LL254;}case 237: _LL429: _LL42A: {
# 2224
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2239 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp67D;_tmp67D.tag=23;_tmp67D.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line);_tmp67D;});_tmp67C;});_tmp67B->tl=0;_tmp67B;}));
goto _LL254;}case 238: _LL42B: _LL42C: {
# 2242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2241 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp680;_tmp680.tag=23;_tmp680.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line);_tmp680;});_tmp67F;});_tmp67E->tl=Cyc_yyget_YY40(yyyyvsp[2]);_tmp67E;}));
goto _LL254;}case 239: _LL42D: _LL42E: {
# 2244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->hd=Cyc_yyget_YY37(yyyyvsp[0]);_tmp681->tl=0;_tmp681;}));
goto _LL254;}case 240: _LL42F: _LL430: {
# 2250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2249 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->hd=Cyc_yyget_YY37(yyyyvsp[2]);_tmp682->tl=Cyc_yyget_YY38(yyyyvsp[0]);_tmp682;}));
goto _LL254;}case 241: _LL431: _LL432: {
# 2252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
struct _tuple25 _tmp683=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp684=_tmp683;struct Cyc_Absyn_Tqual _tmp699;struct Cyc_Parse_Type_specifier _tmp698;struct Cyc_List_List*_tmp697;_LL6A7: _tmp699=_tmp684.f1;_tmp698=_tmp684.f2;_tmp697=_tmp684.f3;_LL6A8:;
if(_tmp699.loc == 0)_tmp699.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp685=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp686=_tmp685;struct _tuple0*_tmp696;struct Cyc_List_List*_tmp695;_LL6AA: _tmp696=_tmp686.id;_tmp695=_tmp686.tms;_LL6AB:;{
void*_tmp687=Cyc_Parse_speclist2typ(_tmp698,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp688=Cyc_Parse_apply_tms(_tmp699,_tmp687,_tmp697,_tmp695);struct _tuple13 _tmp689=_tmp688;struct Cyc_Absyn_Tqual _tmp694;void*_tmp693;struct Cyc_List_List*_tmp692;struct Cyc_List_List*_tmp691;_LL6AD: _tmp694=_tmp689.f1;_tmp693=_tmp689.f2;_tmp692=_tmp689.f3;_tmp691=_tmp689.f4;_LL6AE:;
if(_tmp692 != 0)
({void*_tmp68A=0;Cyc_Warn_err((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp68B="parameter with bad type params";_tag_dyneither(_tmp68B,sizeof(char),31);}),_tag_dyneither(_tmp68A,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp696))
({void*_tmp68C=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp68D="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp68D,sizeof(char),47);}),_tag_dyneither(_tmp68C,sizeof(void*),0));});{
struct _dyneither_ptr*idopt=(*_tmp696).f2;
if(_tmp691 != 0)
({void*_tmp68E=0;Cyc_Warn_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp68F="extra attributes on parameter, ignoring";_tag_dyneither(_tmp68F,sizeof(char),40);}),_tag_dyneither(_tmp68E,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->f1=idopt;_tmp690->f2=_tmp694;_tmp690->f3=_tmp693;_tmp690;}));
# 2269
goto _LL254;};};};}case 242: _LL433: _LL434: {
# 2271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2270 "parse.y"
struct _tuple25 _tmp69A=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp69B=_tmp69A;struct Cyc_Absyn_Tqual _tmp6A2;struct Cyc_Parse_Type_specifier _tmp6A1;struct Cyc_List_List*_tmp6A0;_LL6B0: _tmp6A2=_tmp69B.f1;_tmp6A1=_tmp69B.f2;_tmp6A0=_tmp69B.f3;_LL6B1:;
if(_tmp6A2.loc == 0)_tmp6A2.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp69C=Cyc_Parse_speclist2typ(_tmp6A1,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6A0 != 0)
({void*_tmp69D=0;Cyc_Warn_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp69E="bad attributes on parameter, ignoring";_tag_dyneither(_tmp69E,sizeof(char),38);}),_tag_dyneither(_tmp69D,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->f1=0;_tmp69F->f2=_tmp6A2;_tmp69F->f3=_tmp69C;_tmp69F;}));
# 2277
goto _LL254;};}case 243: _LL435: _LL436: {
# 2279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2278 "parse.y"
struct _tuple25 _tmp6A3=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6A4=_tmp6A3;struct Cyc_Absyn_Tqual _tmp6B4;struct Cyc_Parse_Type_specifier _tmp6B3;struct Cyc_List_List*_tmp6B2;_LL6B3: _tmp6B4=_tmp6A4.f1;_tmp6B3=_tmp6A4.f2;_tmp6B2=_tmp6A4.f3;_LL6B4:;
if(_tmp6B4.loc == 0)_tmp6B4.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6A5=Cyc_Parse_speclist2typ(_tmp6B3,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6A6=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6A7=Cyc_Parse_apply_tms(_tmp6B4,_tmp6A5,_tmp6B2,_tmp6A6);struct _tuple13 _tmp6A8=_tmp6A7;struct Cyc_Absyn_Tqual _tmp6B1;void*_tmp6B0;struct Cyc_List_List*_tmp6AF;struct Cyc_List_List*_tmp6AE;_LL6B6: _tmp6B1=_tmp6A8.f1;_tmp6B0=_tmp6A8.f2;_tmp6AF=_tmp6A8.f3;_tmp6AE=_tmp6A8.f4;_LL6B7:;
if(_tmp6AF != 0)
({void*_tmp6A9=0;Cyc_Warn_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp6AA="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp6AA,sizeof(char),49);}),_tag_dyneither(_tmp6A9,sizeof(void*),0));});
# 2286
if(_tmp6AE != 0)
({void*_tmp6AB=0;Cyc_Warn_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp6AC="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6AC,sizeof(char),38);}),_tag_dyneither(_tmp6AB,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->f1=0;_tmp6AD->f2=_tmp6B1;_tmp6AD->f3=_tmp6B0;_tmp6AD;}));
# 2290
goto _LL254;};}case 244: _LL437: _LL438: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2294 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
goto _LL254;}case 245: _LL439: _LL43A: {
# 2297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2298 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->hd=({struct _dyneither_ptr*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6B6;});_tmp6B5->tl=0;_tmp6B5;}));
goto _LL254;}case 246: _LL43B: _LL43C: {
# 2301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2300 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->hd=({struct _dyneither_ptr*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6B8;});_tmp6B7->tl=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6B7;}));
goto _LL254;}case 247: _LL43D: _LL43E: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2304 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 248: _LL43F: _LL440: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2305 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 249: _LL441: _LL442: {
# 2308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2310 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6BA;_tmp6BA.tag=35;_tmp6BA.f1=0;_tmp6BA.f2=0;_tmp6BA;});_tmp6B9;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 250: _LL443: _LL444: {
# 2313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2312 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6BC;_tmp6BC.tag=35;_tmp6BC.f1=0;_tmp6BC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6BC;});_tmp6BB;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 251: _LL445: _LL446: {
# 2315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2314 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6BE;_tmp6BE.tag=35;_tmp6BE.f1=0;_tmp6BE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6BE;});_tmp6BD;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 252: _LL447: _LL448: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2316 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp6BF=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->f1=Cyc_Absyn_Loc_n;_tmp6C2->f2=({struct _dyneither_ptr*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6C3;});_tmp6C2;}),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2319
(_tmp6BF->tq).real_const=1;
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp6C1;_tmp6C1.tag=26;_tmp6C1.f1=_tmp6BF;_tmp6C1.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6C1.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp6C1.f4=0;_tmp6C1;});_tmp6C0;}),(unsigned int)(yyyylsp[0]).first_line));
# 2322
goto _LL254;}case 253: _LL449: _LL44A: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2324 "parse.y"
void*_tmp6C4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp6C6;_tmp6C6.tag=27;_tmp6C6.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6C6.f2=_tmp6C4;_tmp6C6.f3=0;_tmp6C6;});_tmp6C5;}),(unsigned int)(yyyylsp[0]).first_line));
# 2327
goto _LL254;}case 254: _LL44B: _LL44C: {
# 2329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->hd=({struct _tuple30*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->f1=0;_tmp6C8->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp6C8;});_tmp6C7->tl=0;_tmp6C7;}));
goto _LL254;}case 255: _LL44D: _LL44E: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2334 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=({struct _tuple30*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->f1=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6CA->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6CA;});_tmp6C9->tl=0;_tmp6C9;}));
goto _LL254;}case 256: _LL44F: _LL450: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2336 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=({struct _tuple30*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->f1=0;_tmp6CC->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6CC;});_tmp6CB->tl=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6CB;}));
goto _LL254;}case 257: _LL451: _LL452: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2338 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->hd=({struct _tuple30*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->f1=Cyc_yyget_YY41(yyyyvsp[2]);_tmp6CE->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp6CE;});_tmp6CD->tl=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6CD;}));
goto _LL254;}case 258: _LL453: _LL454: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2342 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
goto _LL254;}case 259: _LL455: _LL456: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2343 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6D1;_tmp6D1.tag=1;_tmp6D1.f1=({struct _dyneither_ptr*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6D2;});_tmp6D1;});_tmp6D0;});_tmp6CF->tl=0;_tmp6CF;}));
goto _LL254;}case 260: _LL457: _LL458: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2348 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=Cyc_yyget_YY42(yyyyvsp[0]);_tmp6D3->tl=0;_tmp6D3;}));
goto _LL254;}case 261: _LL459: _LL45A: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2349 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=Cyc_yyget_YY42(yyyyvsp[1]);_tmp6D4->tl=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6D4;}));
goto _LL254;}case 262: _LL45B: _LL45C: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2353 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp6D6;_tmp6D6.tag=0;_tmp6D6.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6D6;});_tmp6D5;}));
goto _LL254;}case 263: _LL45D: _LL45E: {
# 2356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2354 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6D8;_tmp6D8.tag=1;_tmp6D8.f1=({struct _dyneither_ptr*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp6D9;});_tmp6D8;});_tmp6D7;}));
goto _LL254;}case 264: _LL45F: _LL460: {
# 2357
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2359
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2359 "parse.y"
struct _tuple25 _tmp6DA=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6DB=_tmp6DA;struct Cyc_Absyn_Tqual _tmp6E2;struct Cyc_Parse_Type_specifier _tmp6E1;struct Cyc_List_List*_tmp6E0;_LL6B9: _tmp6E2=_tmp6DB.f1;_tmp6E1=_tmp6DB.f2;_tmp6E0=_tmp6DB.f3;_LL6BA:;{
void*_tmp6DC=Cyc_Parse_speclist2typ(_tmp6E1,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6E0 != 0)
({void*_tmp6DD=0;Cyc_Warn_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp6DE="ignoring attributes in type";_tag_dyneither(_tmp6DE,sizeof(char),28);}),_tag_dyneither(_tmp6DD,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->f1=0;_tmp6DF->f2=_tmp6E2;_tmp6DF->f3=_tmp6DC;_tmp6DF;}));
# 2365
goto _LL254;};}case 265: _LL461: _LL462: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2366 "parse.y"
struct _tuple25 _tmp6E3=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6E4=_tmp6E3;struct Cyc_Absyn_Tqual _tmp6EF;struct Cyc_Parse_Type_specifier _tmp6EE;struct Cyc_List_List*_tmp6ED;_LL6BC: _tmp6EF=_tmp6E4.f1;_tmp6EE=_tmp6E4.f2;_tmp6ED=_tmp6E4.f3;_LL6BD:;{
void*_tmp6E5=Cyc_Parse_speclist2typ(_tmp6EE,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6E6=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6E7=Cyc_Parse_apply_tms(_tmp6EF,_tmp6E5,_tmp6ED,_tmp6E6);
if(_tmp6E7.f3 != 0)
# 2372
({void*_tmp6E8=0;Cyc_Warn_warn((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp6E9="bad type params, ignoring";_tag_dyneither(_tmp6E9,sizeof(char),26);}),_tag_dyneither(_tmp6E8,sizeof(void*),0));});
if(_tmp6E7.f4 != 0)
({void*_tmp6EA=0;Cyc_Warn_warn((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp6EB="bad specifiers, ignoring";_tag_dyneither(_tmp6EB,sizeof(char),25);}),_tag_dyneither(_tmp6EA,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC->f1=0;_tmp6EC->f2=_tmp6E7.f1;_tmp6EC->f3=_tmp6E7.f2;_tmp6EC;}));
# 2377
goto _LL254;};}case 266: _LL463: _LL464: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2380 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 267: _LL465: _LL466: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2381 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F1;_tmp6F1.tag=24;_tmp6F1.f1=0;_tmp6F1;});_tmp6F0;}));
goto _LL254;}case 268: _LL467: _LL468: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2382 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F3;_tmp6F3.tag=24;_tmp6F3.f1=Cyc_yyget_YY40(yyyyvsp[1]);_tmp6F3;});_tmp6F2;}));
goto _LL254;}case 269: _LL469: _LL46A: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2383 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6F5;_tmp6F5.tag=25;_tmp6F5.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6F5;});_tmp6F4;}));
goto _LL254;}case 270: _LL46B: _LL46C: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2384 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F7;_tmp6F7.tag=24;_tmp6F7.f1=({struct Cyc_List_List*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8->hd=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6F8->tl=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6F8;});_tmp6F7;});_tmp6F6;}));
goto _LL254;}case 271: _LL46D: _LL46E: {
# 2387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->hd=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6F9->tl=0;_tmp6F9;}));
goto _LL254;}case 272: _LL46F: _LL470: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2391 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->hd=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6FA->tl=Cyc_yyget_YY40(yyyyvsp[0]);_tmp6FA;}));
goto _LL254;}case 273: _LL471: _LL472: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2396 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FB;_tmp6FB.tms=Cyc_yyget_YY26(yyyyvsp[0]);_tmp6FB;}));
goto _LL254;}case 274: _LL473: _LL474: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2398 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 275: _LL475: _LL476: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2400 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FC;_tmp6FC.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms);_tmp6FC;}));
goto _LL254;}case 276: _LL477: _LL478: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2405 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 277: _LL479: _LL47A: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2407 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FD;_tmp6FD.tms=({struct Cyc_List_List*_tmp6FE=_region_malloc(yyr,sizeof(*_tmp6FE));_tmp6FE->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp6FF=_region_malloc(yyr,sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp700;_tmp700.tag=0;_tmp700.f1=Cyc_yyget_YY51(yyyyvsp[2]);_tmp700.f2=(unsigned int)(yyyylsp[2]).first_line;_tmp700;});_tmp6FF;});_tmp6FE->tl=0;_tmp6FE;});_tmp6FD;}));
goto _LL254;}case 278: _LL47B: _LL47C: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2409 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp701;_tmp701.tms=({struct Cyc_List_List*_tmp702=_region_malloc(yyr,sizeof(*_tmp702));_tmp702->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp703=_region_malloc(yyr,sizeof(*_tmp703));_tmp703[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp704;_tmp704.tag=0;_tmp704.f1=Cyc_yyget_YY51(yyyyvsp[3]);_tmp704.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp704;});_tmp703;});_tmp702->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp702;});_tmp701;}));
goto _LL254;}case 279: _LL47D: _LL47E: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2411 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp705;_tmp705.tms=({struct Cyc_List_List*_tmp706=_region_malloc(yyr,sizeof(*_tmp706));_tmp706->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp707=_region_malloc(yyr,sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp708;_tmp708.tag=1;_tmp708.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp708.f2=Cyc_yyget_YY51(yyyyvsp[3]);_tmp708.f3=(unsigned int)(yyyylsp[3]).first_line;_tmp708;});_tmp707;});_tmp706->tl=0;_tmp706;});_tmp705;}));
goto _LL254;}case 280: _LL47F: _LL480: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2413 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp709;_tmp709.tms=({struct Cyc_List_List*_tmp70A=_region_malloc(yyr,sizeof(*_tmp70A));_tmp70A->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp70B=_region_malloc(yyr,sizeof(*_tmp70B));_tmp70B[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp70C;_tmp70C.tag=1;_tmp70C.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp70C.f2=Cyc_yyget_YY51(yyyyvsp[4]);_tmp70C.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp70C;});_tmp70B;});_tmp70A->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp70A;});_tmp709;}));
# 2416
goto _LL254;}case 281: _LL481: _LL482: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2417 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70D;_tmp70D.tms=({struct Cyc_List_List*_tmp70E=_region_malloc(yyr,sizeof(*_tmp70E));_tmp70E->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp70F=_region_malloc(yyr,sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp710;_tmp710.tag=3;_tmp710.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp711=_region_malloc(yyr,sizeof(*_tmp711));_tmp711[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp712;_tmp712.tag=1;_tmp712.f1=0;_tmp712.f2=0;_tmp712.f3=0;_tmp712.f4=Cyc_yyget_YY49(yyyyvsp[1]);_tmp712.f5=Cyc_yyget_YY50(yyyyvsp[2]);_tmp712.f6=Cyc_yyget_YY55(yyyyvsp[4]);_tmp712.f7=Cyc_yyget_YY55(yyyyvsp[5]);_tmp712;});_tmp711;});_tmp710;});_tmp70F;});_tmp70E->tl=0;_tmp70E;});_tmp70D;}));
# 2419
goto _LL254;}case 282: _LL483: _LL484: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2420 "parse.y"
struct _tuple26*_tmp713=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp714=_tmp713;struct Cyc_List_List*_tmp71F;int _tmp71E;struct Cyc_Absyn_VarargInfo*_tmp71D;void*_tmp71C;struct Cyc_List_List*_tmp71B;_LL6BF: _tmp71F=_tmp714->f1;_tmp71E=_tmp714->f2;_tmp71D=_tmp714->f3;_tmp71C=_tmp714->f4;_tmp71B=_tmp714->f5;_LL6C0:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp715;_tmp715.tms=({struct Cyc_List_List*_tmp716=_region_malloc(yyr,sizeof(*_tmp716));_tmp716->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp717=_region_malloc(yyr,sizeof(*_tmp717));_tmp717[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp718;_tmp718.tag=3;_tmp718.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp719=_region_malloc(yyr,sizeof(*_tmp719));_tmp719[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp71A;_tmp71A.tag=1;_tmp71A.f1=_tmp71F;_tmp71A.f2=_tmp71E;_tmp71A.f3=_tmp71D;_tmp71A.f4=_tmp71C;_tmp71A.f5=_tmp71B;_tmp71A.f6=Cyc_yyget_YY55(yyyyvsp[3]);_tmp71A.f7=Cyc_yyget_YY55(yyyyvsp[4]);_tmp71A;});_tmp719;});_tmp718;});_tmp717;});_tmp716->tl=0;_tmp716;});_tmp715;}));
# 2423
goto _LL254;}case 283: _LL485: _LL486: {
# 2425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2424 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp720;_tmp720.tms=({struct Cyc_List_List*_tmp721=_region_malloc(yyr,sizeof(*_tmp721));_tmp721->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp722=_region_malloc(yyr,sizeof(*_tmp722));_tmp722[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp723;_tmp723.tag=3;_tmp723.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp724=_region_malloc(yyr,sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp725;_tmp725.tag=1;_tmp725.f1=0;_tmp725.f2=0;_tmp725.f3=0;_tmp725.f4=Cyc_yyget_YY49(yyyyvsp[2]);_tmp725.f5=Cyc_yyget_YY50(yyyyvsp[3]);_tmp725.f6=Cyc_yyget_YY55(yyyyvsp[5]);_tmp725.f7=Cyc_yyget_YY55(yyyyvsp[6]);_tmp725;});_tmp724;});_tmp723;});_tmp722;});_tmp721->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp721;});_tmp720;}));
# 2427
goto _LL254;}case 284: _LL487: _LL488: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2428 "parse.y"
struct _tuple26*_tmp726=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp727=_tmp726;struct Cyc_List_List*_tmp732;int _tmp731;struct Cyc_Absyn_VarargInfo*_tmp730;void*_tmp72F;struct Cyc_List_List*_tmp72E;_LL6C2: _tmp732=_tmp727->f1;_tmp731=_tmp727->f2;_tmp730=_tmp727->f3;_tmp72F=_tmp727->f4;_tmp72E=_tmp727->f5;_LL6C3:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp728;_tmp728.tms=({struct Cyc_List_List*_tmp729=_region_malloc(yyr,sizeof(*_tmp729));_tmp729->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp72A=_region_malloc(yyr,sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp72B;_tmp72B.tag=3;_tmp72B.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp72C=_region_malloc(yyr,sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp72D;_tmp72D.tag=1;_tmp72D.f1=_tmp732;_tmp72D.f2=_tmp731;_tmp72D.f3=_tmp730;_tmp72D.f4=_tmp72F;_tmp72D.f5=_tmp72E;_tmp72D.f6=
Cyc_yyget_YY55(yyyyvsp[4]);_tmp72D.f7=Cyc_yyget_YY55(yyyyvsp[5]);_tmp72D;});_tmp72C;});_tmp72B;});_tmp72A;});_tmp729->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp729;});_tmp728;}));
# 2432
goto _LL254;}case 285: _LL489: _LL48A: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2434 "parse.y"
struct Cyc_List_List*_tmp733=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp734;_tmp734.tms=({struct Cyc_List_List*_tmp735=_region_malloc(yyr,sizeof(*_tmp735));_tmp735->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp736=_region_malloc(yyr,sizeof(*_tmp736));_tmp736[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp737;_tmp737.tag=4;_tmp737.f1=_tmp733;_tmp737.f2=(unsigned int)(yyyylsp[1]).first_line;_tmp737.f3=0;_tmp737;});_tmp736;});_tmp735->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp735;});_tmp734;}));
# 2438
goto _LL254;}case 286: _LL48B: _LL48C: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2439 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp738;_tmp738.tms=({struct Cyc_List_List*_tmp739=_region_malloc(yyr,sizeof(*_tmp739));_tmp739->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp73A=_region_malloc(yyr,sizeof(*_tmp73A));_tmp73A[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp73B;_tmp73B.tag=5;_tmp73B.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp73B.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp73B;});_tmp73A;});_tmp739->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp739;});_tmp738;}));
# 2441
goto _LL254;}case 287: _LL48D: _LL48E: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2445 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 288: _LL48F: _LL490: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2446 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 289: _LL491: _LL492: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2447 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 290: _LL493: _LL494: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2448 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 291: _LL495: _LL496: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2449 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 292: _LL497: _LL498: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2450 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 293: _LL499: _LL49A: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2453 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=({struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmp73D;_tmp73D.tag=16;_tmp73D.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp73D;});_tmp73C;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 294: _LL49B: _LL49C: {
# 2456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp73F;_tmp73F.tag=13;_tmp73F.f1=({struct _dyneither_ptr*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp740;});_tmp73F.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp73F;});_tmp73E;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 295: _LL49D: _LL49E: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2463 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 296: _LL49F: _LL4A0: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2464 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 297: _LL4A1: _LL4A2: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2469 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 298: _LL4A3: _LL4A4: {
# 2472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2470 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 299: _LL4A5: _LL4A6: {
# 2473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2475 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 300: _LL4A7: _LL4A8: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2476 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
goto _LL254;}case 301: _LL4A9: _LL4AA: {
# 2479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2477 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp742;_tmp742.tag=13;_tmp742.f1=({struct _dyneither_ptr*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp743;});_tmp742.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0));_tmp742;});_tmp741;}),(unsigned int)(yyyylsp[0]).first_line));
# 2479
goto _LL254;}case 302: _LL4AB: _LL4AC: {
# 2481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2479 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp745;_tmp745.tag=13;_tmp745.f1=({struct _dyneither_ptr*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp746;});_tmp745.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]));_tmp745;});_tmp744;}),(unsigned int)(yyyylsp[0]).first_line));
# 2481
goto _LL254;}case 303: _LL4AD: _LL4AE: {
# 2483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2481 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 304: _LL4AF: _LL4B0: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2482 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 305: _LL4B1: _LL4B2: {
# 2485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2483 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp748;_tmp748.tag=1;_tmp748.f1=Cyc_yyget_YY15(yyyyvsp[0]);_tmp748;});_tmp747;}),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));
goto _LL254;}case 306: _LL4B3: _LL4B4: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2486 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp74A;_tmp74A.tag=1;_tmp74A.f1=Cyc_yyget_YY15(yyyyvsp[0]);_tmp74A;});_tmp749;}),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
goto _LL254;}case 307: _LL4B5: _LL4B6: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2491 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 308: _LL4B7: _LL4B8: {
# 2494
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2496
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2493 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 309: _LL4B9: _LL4BA: {
# 2496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 310: _LL4BB: _LL4BC: {
# 2502
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2502 "parse.y"
struct Cyc_Absyn_Exp*_tmp74B=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp74B,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 311: _LL4BD: _LL4BE: {
# 2506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2506 "parse.y"
struct Cyc_Absyn_Exp*_tmp74C=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp74C,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2509
goto _LL254;}case 312: _LL4BF: _LL4C0: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2512 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 313: _LL4C1: _LL4C2:
# 2515
 yyval=Cyc_YY8(0);
goto _LL254;case 314: _LL4C3: _LL4C4: {
# 2518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2530 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->hd=({struct Cyc_Absyn_Switch_clause*_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp74E->pat_vars=0;_tmp74E->where_clause=0;_tmp74E->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp74E->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp74E;});_tmp74D->tl=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp74D;}));
goto _LL254;}case 315: _LL4C5: _LL4C6: {
# 2535
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2537
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2534 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=({struct Cyc_Absyn_Switch_clause*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp750->pat_vars=0;_tmp750->where_clause=0;_tmp750->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp750->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp750;});_tmp74F->tl=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp74F;}));
goto _LL254;}case 316: _LL4C7: _LL4C8: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2538 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp751=_cycalloc(sizeof(*_tmp751));_tmp751->hd=({struct Cyc_Absyn_Switch_clause*_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp752->pat_vars=0;_tmp752->where_clause=0;_tmp752->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp752->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp752;});_tmp751->tl=Cyc_yyget_YY8(yyyyvsp[4]);_tmp751;}));
goto _LL254;}case 317: _LL4C9: _LL4CA: {
# 2541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2540 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753->hd=({struct Cyc_Absyn_Switch_clause*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp754->pat_vars=0;_tmp754->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp754->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp754->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp754;});_tmp753->tl=
Cyc_yyget_YY8(yyyyvsp[5]);_tmp753;}));
goto _LL254;}case 318: _LL4CB: _LL4CC: {
# 2545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2544 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=({struct Cyc_Absyn_Switch_clause*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp756->pat_vars=0;_tmp756->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp756->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp756->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp756;});_tmp755->tl=Cyc_yyget_YY8(yyyyvsp[6]);_tmp755;}));
goto _LL254;}case 319: _LL4CD: _LL4CE: {
# 2547
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2549
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2551 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 320: _LL4CF: _LL4D0: {
# 2554
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2556
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2555 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 321: _LL4D1: _LL4D2: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2559 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 322: _LL4D3: _LL4D4: {
# 2563
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2562 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 323: _LL4D5: _LL4D6: {
# 2566
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2565 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 324: _LL4D7: _LL4D8: {
# 2569
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2568 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 325: _LL4D9: _LL4DA: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2571 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 326: _LL4DB: _LL4DC: {
# 2575
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2574 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 327: _LL4DD: _LL4DE: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2577 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 328: _LL4DF: _LL4E0: {
# 2581
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9));
# 2583
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9));
# 2580 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 329: _LL4E1: _LL4E2: {
# 2584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2583 "parse.y"
struct Cyc_List_List*_tmp757=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp758=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp757,_tmp758));
# 2588
goto _LL254;}case 330: _LL4E3: _LL4E4: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2589 "parse.y"
struct Cyc_List_List*_tmp759=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp759,_tmp75A));
# 2594
goto _LL254;}case 331: _LL4E5: _LL4E6: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 2595 "parse.y"
struct Cyc_List_List*_tmp75B=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75B,_tmp75C));
# 2600
goto _LL254;}case 332: _LL4E7: _LL4E8: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8));
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8));
# 2601 "parse.y"
struct Cyc_List_List*_tmp75D=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75D,_tmp75E));
# 2606
goto _LL254;}case 333: _LL4E9: _LL4EA: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(({struct _dyneither_ptr*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp75F;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 334: _LL4EB: _LL4EC: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2612 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 335: _LL4ED: _LL4EE: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2613 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 336: _LL4EF: _LL4F0: {
# 2616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2614 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 337: _LL4F1: _LL4F2: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2615 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 338: _LL4F3: _LL4F4: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2617 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 339: _LL4F5: _LL4F6: {
# 2620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2618 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 340: _LL4F7: _LL4F8: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2620 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 341: _LL4F9: _LL4FA: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2629 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 342: _LL4FB: _LL4FC: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2632 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 343: _LL4FD: _LL4FE: {
# 2635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2634 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 344: _LL4FF: _LL500: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2637 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 345: _LL501: _LL502: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2639 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 346: _LL503: _LL504: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2642 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 347: _LL505: _LL506: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2644 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 348: _LL507: _LL508: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2647 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 349: _LL509: _LL50A: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2649 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 350: _LL50B: _LL50C: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2652 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 351: _LL50D: _LL50E: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2654 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 352: _LL50F: _LL510: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2657 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 353: _LL511: _LL512: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2659 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 354: _LL513: _LL514: {
# 2662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2662 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 355: _LL515: _LL516: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2664 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 356: _LL517: _LL518: {
# 2667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2666 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 357: _LL519: _LL51A: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2669 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 358: _LL51B: _LL51C: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2671 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 359: _LL51D: _LL51E: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2673 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 360: _LL51F: _LL520: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2675 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 361: _LL521: _LL522: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2677 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 362: _LL523: _LL524: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2680 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 363: _LL525: _LL526: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2682 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 364: _LL527: _LL528: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2684 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 365: _LL529: _LL52A: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2687 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 366: _LL52B: _LL52C: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2689 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 367: _LL52D: _LL52E: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2691 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 368: _LL52F: _LL530: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2694 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 369: _LL531: _LL532: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2696 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 370: _LL533: _LL534: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2698 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 371: _LL535: _LL536: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2700 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 372: _LL537: _LL538: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2703 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 373: _LL539: _LL53A: {
# 2706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2705 "parse.y"
void*_tmp760=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp760,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2708
goto _LL254;}case 374: _LL53B: _LL53C: {
# 2710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2711 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 375: _LL53D: _LL53E: {
# 2714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2714 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL254;}case 376: _LL53F: _LL540: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2716 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 377: _LL541: _LL542: {
# 2719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2718 "parse.y"
void*_tmp761=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp761,(unsigned int)(yyyylsp[0]).first_line)));
# 2721
goto _LL254;}case 378: _LL543: _LL544: {
# 2723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2722 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 379: _LL545: _LL546: {
# 2725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2724 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL254;}case 380: _LL547: _LL548: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2729 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 381: _LL549: _LL54A: {
# 2732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2737 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 382: _LL54B: _LL54C: {
# 2740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2742 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 383: _LL54D: _LL54E: {
# 2745
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2744 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL254;}case 384: _LL54F: _LL550: {
# 2747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2746 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp762=e->r;void*_tmp763=_tmp762;struct _dyneither_ptr _tmp77B;int _tmp77A;enum Cyc_Absyn_Sign _tmp779;int _tmp778;enum Cyc_Absyn_Sign _tmp777;short _tmp776;enum Cyc_Absyn_Sign _tmp775;char _tmp774;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).LongLong_c).tag){case 2: _LL6C5: _tmp775=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Char_c).val).f1;_tmp774=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Char_c).val).f2;_LL6C6:
# 2750
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp764=_cycalloc_atomic(sizeof(*_tmp764));_tmp764[0]=({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmp765;_tmp765.tag=11;_tmp765.f1=_tmp774;_tmp765;});_tmp764;}),e->loc));goto _LL6C4;case 4: _LL6C7: _tmp777=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Short_c).val).f1;_tmp776=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Short_c).val).f2;_LL6C8:
# 2752
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp766=_cycalloc_atomic(sizeof(*_tmp766));_tmp766[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp767;_tmp767.tag=10;_tmp767.f1=_tmp777;_tmp767.f2=(int)_tmp776;_tmp767;});_tmp766;}),e->loc));goto _LL6C4;case 5: _LL6C9: _tmp779=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Int_c).val).f1;_tmp778=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Int_c).val).f2;_LL6CA:
# 2754
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp768=_cycalloc_atomic(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp769;_tmp769.tag=10;_tmp769.f1=_tmp779;_tmp769.f2=_tmp778;_tmp769;});_tmp768;}),e->loc));goto _LL6C4;case 7: _LL6CB: _tmp77B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Float_c).val).f1;_tmp77A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp763)->f1).Float_c).val).f2;_LL6CC:
# 2756
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmp76B;_tmp76B.tag=12;_tmp76B.f1=_tmp77B;_tmp76B.f2=_tmp77A;_tmp76B;});_tmp76A;}),e->loc));goto _LL6C4;case 1: _LL6CD: _LL6CE:
# 2758
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL6C4;case 8: _LL6CF: _LL6D0:
# 2760
({void*_tmp76C=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp76D="strings cannot occur within patterns";_tag_dyneither(_tmp76D,sizeof(char),37);}),_tag_dyneither(_tmp76C,sizeof(void*),0));});goto _LL6C4;case 9: _LL6D1: _LL6D2:
# 2762
({void*_tmp76E=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp76F="strings cannot occur within patterns";_tag_dyneither(_tmp76F,sizeof(char),37);}),_tag_dyneither(_tmp76E,sizeof(void*),0));});goto _LL6C4;case 6: _LL6D3: _LL6D4:
# 2764
({void*_tmp770=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp771="long long's in patterns not yet implemented";_tag_dyneither(_tmp771,sizeof(char),44);}),_tag_dyneither(_tmp770,sizeof(void*),0));});goto _LL6C4;default: goto _LL6D5;}else{_LL6D5: _LL6D6:
# 2766
({void*_tmp772=0;Cyc_Warn_err((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp773="bad constant in case";_tag_dyneither(_tmp773,sizeof(char),21);}),_tag_dyneither(_tmp772,sizeof(void*),0));});}_LL6C4:;}
# 2769
goto _LL254;}case 385: _LL551: _LL552: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2770 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmp77D;_tmp77D.tag=15;_tmp77D.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp77D;});_tmp77C;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 386: _LL553: _LL554: {
# 2773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2772 "parse.y"
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),({const char*_tmp77E="as";_tag_dyneither(_tmp77E,sizeof(char),3);}))!= 0)
({void*_tmp77F=0;Cyc_Warn_err((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp780="expecting `as'";_tag_dyneither(_tmp780,sizeof(char),15);}),_tag_dyneither(_tmp77F,sizeof(void*),0));});
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp782;_tmp782.tag=1;_tmp782.f1=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->f1=Cyc_Absyn_Loc_n;_tmp783->f2=({struct _dyneither_ptr*_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp784;});_tmp783;}),(void*)& Cyc_Absyn_VoidType_val,0);_tmp782.f2=
Cyc_yyget_YY9(yyyyvsp[2]);_tmp782;});_tmp781;}),(unsigned int)(yyyylsp[0]).first_line));
# 2777
goto _LL254;}case 387: _LL555: _LL556: {
# 2779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 2781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 2778 "parse.y"
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),({const char*_tmp785="alias";_tag_dyneither(_tmp785,sizeof(char),6);}))!= 0)
({void*_tmp786=0;Cyc_Warn_err((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp787="expecting `alias'";_tag_dyneither(_tmp787,sizeof(char),18);}),_tag_dyneither(_tmp786,sizeof(void*),0));});{
int _tmp788=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D->name=({struct _dyneither_ptr*_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp790;});_tmp78D->identity=- 1;_tmp78D->kind=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E[0]=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp78F;_tmp78F.tag=0;_tmp78F.f1=& Cyc_Tcutil_rk;_tmp78F;});_tmp78E;});_tmp78D;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B->f1=Cyc_Absyn_Loc_n;_tmp78B->f2=({struct _dyneither_ptr*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=Cyc_yyget_String_tok(yyyyvsp[5]);_tmp78C;});_tmp78B;}),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp78A;_tmp78A.tag=2;_tmp78A.f1=tv;_tmp78A.f2=vd;_tmp78A;});_tmp789;}),(unsigned int)_tmp788));
# 2786
goto _LL254;};}case 388: _LL557: _LL558: {
# 2788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2787 "parse.y"
struct _tuple22 _tmp791=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp792=_tmp791;struct Cyc_List_List*_tmp796;int _tmp795;_LL6D8: _tmp796=_tmp792.f1;_tmp795=_tmp792.f2;_LL6D9:;
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmp794;_tmp794.tag=5;_tmp794.f1=_tmp796;_tmp794.f2=_tmp795;_tmp794;});_tmp793;}),(unsigned int)(yyyylsp[0]).first_line));
# 2790
goto _LL254;}case 389: _LL559: _LL55A: {
# 2792
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2794
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2791 "parse.y"
struct _tuple22 _tmp797=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp798=_tmp797;struct Cyc_List_List*_tmp79C;int _tmp79B;_LL6DB: _tmp79C=_tmp798.f1;_tmp79B=_tmp798.f2;_LL6DC:;
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmp79A;_tmp79A.tag=16;_tmp79A.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp79A.f2=_tmp79C;_tmp79A.f3=_tmp79B;_tmp79A;});_tmp799;}),(unsigned int)(yyyylsp[0]).first_line));
# 2794
goto _LL254;}case 390: _LL55B: _LL55C: {
# 2796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2795 "parse.y"
struct _tuple22 _tmp79D=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp79E=_tmp79D;struct Cyc_List_List*_tmp7A4;int _tmp7A3;_LL6DE: _tmp7A4=_tmp79E.f1;_tmp7A3=_tmp79E.f2;_LL6DF:;{
struct Cyc_List_List*_tmp79F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp7A1;_tmp7A1.tag=7;_tmp7A1.f1=({struct Cyc_Absyn_AggrInfo*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);_tmp7A2->targs=0;_tmp7A2;});_tmp7A1.f2=_tmp79F;_tmp7A1.f3=_tmp7A4;_tmp7A1.f4=_tmp7A3;_tmp7A1;});_tmp7A0;}),(unsigned int)(yyyylsp[0]).first_line));
# 2800
goto _LL254;};}case 391: _LL55D: _LL55E: {
# 2802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2801 "parse.y"
struct _tuple22 _tmp7A5=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp7A6=_tmp7A5;struct Cyc_List_List*_tmp7AB;int _tmp7AA;_LL6E1: _tmp7AB=_tmp7A6.f1;_tmp7AA=_tmp7A6.f2;_LL6E2:;{
struct Cyc_List_List*_tmp7A7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp7A9;_tmp7A9.tag=7;_tmp7A9.f1=0;_tmp7A9.f2=_tmp7A7;_tmp7A9.f3=_tmp7AB;_tmp7A9.f4=_tmp7AA;_tmp7A9;});_tmp7A8;}),(unsigned int)(yyyylsp[0]).first_line));
# 2805
goto _LL254;};}case 392: _LL55F: _LL560: {
# 2807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2806 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC[0]=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7AD;_tmp7AD.tag=6;_tmp7AD.f1=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7AD;});_tmp7AC;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 393: _LL561: _LL562: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2808 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7AF;_tmp7AF.tag=6;_tmp7AF.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7B1;_tmp7B1.tag=6;_tmp7B1.f1=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7B1;});_tmp7B0;}),(unsigned int)(yyyylsp[0]).first_line);_tmp7AF;});_tmp7AE;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 394: _LL563: _LL564: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2810 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7B3;_tmp7B3.tag=3;_tmp7B3.f1=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4->f1=Cyc_Absyn_Loc_n;_tmp7B4->f2=({struct _dyneither_ptr*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7B5;});_tmp7B4;}),(void*)& Cyc_Absyn_VoidType_val,0);_tmp7B3.f2=
# 2812
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp7B3;});_tmp7B2;}),(unsigned int)(yyyylsp[0]).first_line));
# 2814
goto _LL254;}case 395: _LL565: _LL566: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2815 "parse.y"
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),({const char*_tmp7B6="as";_tag_dyneither(_tmp7B6,sizeof(char),3);}))!= 0)
({void*_tmp7B7=0;Cyc_Warn_err((unsigned int)(yyyylsp[2]).first_line,({const char*_tmp7B8="expecting `as'";_tag_dyneither(_tmp7B8,sizeof(char),15);}),_tag_dyneither(_tmp7B7,sizeof(void*),0));});
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7BA;_tmp7BA.tag=3;_tmp7BA.f1=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB->f1=Cyc_Absyn_Loc_n;_tmp7BB->f2=({struct _dyneither_ptr*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7BC;});_tmp7BB;}),(void*)& Cyc_Absyn_VoidType_val,0);_tmp7BA.f2=
# 2819
Cyc_yyget_YY9(yyyyvsp[3]);_tmp7BA;});_tmp7B9;}),(unsigned int)(yyyylsp[0]).first_line));
# 2821
goto _LL254;}case 396: _LL567: _LL568: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2822 "parse.y"
void*_tmp7BD=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7BF;_tmp7BF.tag=4;_tmp7BF.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7BD);_tmp7BF.f2=
Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0->f1=Cyc_Absyn_Loc_n;_tmp7C0->f2=({struct _dyneither_ptr*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7C1;});_tmp7C0;}),(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7C3;_tmp7C3.tag=19;_tmp7C3.f1=_tmp7BD;_tmp7C3;});_tmp7C2;}),0);_tmp7BF;});_tmp7BE;}),(unsigned int)(yyyylsp[0]).first_line));
# 2827
goto _LL254;}case 397: _LL569: _LL56A: {
# 2829
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 2831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 2828 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7C4=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7C6;_tmp7C6.tag=4;_tmp7C6.f1=_tmp7C4;_tmp7C6.f2=
Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7C7=_cycalloc(sizeof(*_tmp7C7));_tmp7C7->f1=Cyc_Absyn_Loc_n;_tmp7C7->f2=({struct _dyneither_ptr*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7C8;});_tmp7C7;}),(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7CA;_tmp7CA.tag=19;_tmp7CA.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7CB=_cycalloc(sizeof(*_tmp7CB));_tmp7CB[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7CC;_tmp7CC.tag=2;_tmp7CC.f1=_tmp7C4;_tmp7CC;});_tmp7CB;});_tmp7CA;});_tmp7C9;}),0);_tmp7C6;});_tmp7C5;}),(unsigned int)(yyyylsp[0]).first_line));
# 2833
goto _LL254;}case 398: _LL56B: _LL56C: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2836 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7CD->f2=0;_tmp7CD;}));
goto _LL254;}case 399: _LL56D: _LL56E: {
# 2839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2837 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7CE->f2=1;_tmp7CE;}));
goto _LL254;}case 400: _LL56F: _LL570: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2838 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF->f1=0;_tmp7CF->f2=1;_tmp7CF;}));
goto _LL254;}case 401: _LL571: _LL572: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2843 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0->hd=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7D0->tl=0;_tmp7D0;}));
goto _LL254;}case 402: _LL573: _LL574: {
# 2846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2845 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));_tmp7D1->hd=Cyc_yyget_YY9(yyyyvsp[2]);_tmp7D1->tl=Cyc_yyget_YY11(yyyyvsp[0]);_tmp7D1;}));
goto _LL254;}case 403: _LL575: _LL576: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2850 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2->f1=0;_tmp7D2->f2=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7D2;}));
goto _LL254;}case 404: _LL577: _LL578: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2852 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3->f1=Cyc_yyget_YY41(yyyyvsp[0]);_tmp7D3->f2=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7D3;}));
goto _LL254;}case 405: _LL579: _LL57A: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2855 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7D4->f2=0;_tmp7D4;}));
goto _LL254;}case 406: _LL57B: _LL57C: {
# 2858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2856 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7D5->f2=1;_tmp7D5;}));
goto _LL254;}case 407: _LL57D: _LL57E: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2857 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));_tmp7D6->f1=0;_tmp7D6->f2=1;_tmp7D6;}));
goto _LL254;}case 408: _LL57F: _LL580: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2862 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp7D7=_cycalloc(sizeof(*_tmp7D7));_tmp7D7->hd=Cyc_yyget_YY12(yyyyvsp[0]);_tmp7D7->tl=0;_tmp7D7;}));
goto _LL254;}case 409: _LL581: _LL582: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2864 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));_tmp7D8->hd=Cyc_yyget_YY12(yyyyvsp[2]);_tmp7D8->tl=Cyc_yyget_YY13(yyyyvsp[0]);_tmp7D8;}));
goto _LL254;}case 410: _LL583: _LL584: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=yyyyvsp[0];
goto _LL254;}case 411: _LL585: _LL586: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2872 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 412: _LL587: _LL588: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2877 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 413: _LL589: _LL58A: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2879 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 414: _LL58B: _LL58C: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2881 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 415: _LL58D: _LL58E: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2885 "parse.y"
yyval=Cyc_YY7(0);
goto _LL254;}case 416: _LL58F: _LL590: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2886 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7D9=_cycalloc_atomic(sizeof(*_tmp7D9));_tmp7D9->v=(void*)Cyc_Absyn_Times;_tmp7D9;}));
goto _LL254;}case 417: _LL591: _LL592: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2887 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DA=_cycalloc_atomic(sizeof(*_tmp7DA));_tmp7DA->v=(void*)Cyc_Absyn_Div;_tmp7DA;}));
goto _LL254;}case 418: _LL593: _LL594: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2888 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DB=_cycalloc_atomic(sizeof(*_tmp7DB));_tmp7DB->v=(void*)Cyc_Absyn_Mod;_tmp7DB;}));
goto _LL254;}case 419: _LL595: _LL596: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2889 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DC=_cycalloc_atomic(sizeof(*_tmp7DC));_tmp7DC->v=(void*)Cyc_Absyn_Plus;_tmp7DC;}));
goto _LL254;}case 420: _LL597: _LL598: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2890 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DD=_cycalloc_atomic(sizeof(*_tmp7DD));_tmp7DD->v=(void*)Cyc_Absyn_Minus;_tmp7DD;}));
goto _LL254;}case 421: _LL599: _LL59A: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2891 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DE=_cycalloc_atomic(sizeof(*_tmp7DE));_tmp7DE->v=(void*)Cyc_Absyn_Bitlshift;_tmp7DE;}));
goto _LL254;}case 422: _LL59B: _LL59C: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2892 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DF=_cycalloc_atomic(sizeof(*_tmp7DF));_tmp7DF->v=(void*)Cyc_Absyn_Bitlrshift;_tmp7DF;}));
goto _LL254;}case 423: _LL59D: _LL59E: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2893 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E0=_cycalloc_atomic(sizeof(*_tmp7E0));_tmp7E0->v=(void*)Cyc_Absyn_Bitand;_tmp7E0;}));
goto _LL254;}case 424: _LL59F: _LL5A0: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2894 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E1=_cycalloc_atomic(sizeof(*_tmp7E1));_tmp7E1->v=(void*)Cyc_Absyn_Bitxor;_tmp7E1;}));
goto _LL254;}case 425: _LL5A1: _LL5A2: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2895 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E2=_cycalloc_atomic(sizeof(*_tmp7E2));_tmp7E2->v=(void*)Cyc_Absyn_Bitor;_tmp7E2;}));
goto _LL254;}case 426: _LL5A3: _LL5A4: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2900 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 427: _LL5A5: _LL5A6: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2902 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 428: _LL5A7: _LL5A8: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2905 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 429: _LL5A9: _LL5AA: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2908 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 430: _LL5AB: _LL5AC: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 2910 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 431: _LL5AD: _LL5AE: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2912 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 432: _LL5AF: _LL5B0: {
# 2915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 2917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 2914 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 433: _LL5B1: _LL5B2: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2918 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 434: _LL5B3: _LL5B4: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2922 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 435: _LL5B5: _LL5B6: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2924 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 436: _LL5B7: _LL5B8: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2928 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 437: _LL5B9: _LL5BA: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2930 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 438: _LL5BB: _LL5BC: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2934 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 439: _LL5BD: _LL5BE: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2936 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 440: _LL5BF: _LL5C0: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2940 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 441: _LL5C1: _LL5C2: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2942 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 442: _LL5C3: _LL5C4: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2946 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 443: _LL5C5: _LL5C6: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 444: _LL5C7: _LL5C8: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2952 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 445: _LL5C9: _LL5CA: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2954 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 446: _LL5CB: _LL5CC: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2956 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 447: _LL5CD: _LL5CE: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2960 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 448: _LL5CF: _LL5D0: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2962 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 449: _LL5D1: _LL5D2: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2964 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 450: _LL5D3: _LL5D4: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2966 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 451: _LL5D5: _LL5D6: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2968 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 452: _LL5D7: _LL5D8: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2972 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 453: _LL5D9: _LL5DA: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2974 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 454: _LL5DB: _LL5DC: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2976 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 455: _LL5DD: _LL5DE: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2980 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 456: _LL5DF: _LL5E0: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2982 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 457: _LL5E1: _LL5E2: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2984 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 458: _LL5E3: _LL5E4: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2988 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 459: _LL5E5: _LL5E6: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2990 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 460: _LL5E7: _LL5E8: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2992 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 461: _LL5E9: _LL5EA: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 2994 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 462: _LL5EB: _LL5EC: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 2998 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 463: _LL5ED: _LL5EE: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3000 "parse.y"
void*_tmp7E3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp7E3,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3003
goto _LL254;}case 464: _LL5EF: _LL5F0: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3006 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 465: _LL5F1: _LL5F2: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3007 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 466: _LL5F3: _LL5F4: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3008 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 467: _LL5F5: _LL5F6: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3009 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 468: _LL5F7: _LL5F8: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3010 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 469: _LL5F9: _LL5FA: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3011 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 470: _LL5FB: _LL5FC: {
# 3014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3012 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 471: _LL5FD: _LL5FE: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3014 "parse.y"
void*_tmp7E4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp7E4,(unsigned int)(yyyylsp[0]).first_line));
# 3017
goto _LL254;}case 472: _LL5FF: _LL600: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3017 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 473: _LL601: _LL602: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 3019 "parse.y"
void*_tmp7E5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp7E5,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3022
goto _LL254;}case 474: _LL603: _LL604: {
# 3024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3024 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7E7;_tmp7E7.tag=33;_tmp7E7.f1=({struct Cyc_Absyn_MallocInfo _tmp7E8;_tmp7E8.is_calloc=0;_tmp7E8.rgn=0;_tmp7E8.elt_type=0;_tmp7E8.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7E8.fat_result=0;_tmp7E8.inline_call=0;_tmp7E8;});_tmp7E7;});_tmp7E6;}),(unsigned int)(yyyylsp[0]).first_line));
# 3026
goto _LL254;}case 475: _LL605: _LL606: {
# 3028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 3027 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7EA;_tmp7EA.tag=33;_tmp7EA.f1=({struct Cyc_Absyn_MallocInfo _tmp7EB;_tmp7EB.is_calloc=0;_tmp7EB.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7EB.elt_type=0;_tmp7EB.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7EB.fat_result=0;_tmp7EB.inline_call=0;_tmp7EB;});_tmp7EA;});_tmp7E9;}),(unsigned int)(yyyylsp[0]).first_line));
# 3029
goto _LL254;}case 476: _LL607: _LL608: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 3030 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7ED;_tmp7ED.tag=33;_tmp7ED.f1=({struct Cyc_Absyn_MallocInfo _tmp7EE;_tmp7EE.is_calloc=0;_tmp7EE.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7EE.elt_type=0;_tmp7EE.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7EE.fat_result=0;_tmp7EE.inline_call=1;_tmp7EE;});_tmp7ED;});_tmp7EC;}),(unsigned int)(yyyylsp[0]).first_line));
# 3032
goto _LL254;}case 477: _LL609: _LL60A: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9));
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9));
# 3033 "parse.y"
void*_tmp7EF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7F1;_tmp7F1.tag=33;_tmp7F1.f1=({struct Cyc_Absyn_MallocInfo _tmp7F2;_tmp7F2.is_calloc=1;_tmp7F2.rgn=0;_tmp7F2.elt_type=({void**_tmp7F3=_cycalloc(sizeof(*_tmp7F3));_tmp7F3[0]=_tmp7EF;_tmp7F3;});_tmp7F2.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F2.fat_result=0;_tmp7F2.inline_call=0;_tmp7F2;});_tmp7F1;});_tmp7F0;}),(unsigned int)(yyyylsp[0]).first_line));
# 3036
goto _LL254;}case 478: _LL60B: _LL60C: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11));
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11));
# 3038 "parse.y"
void*_tmp7F4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7F6;_tmp7F6.tag=33;_tmp7F6.f1=({struct Cyc_Absyn_MallocInfo _tmp7F7;_tmp7F7.is_calloc=1;_tmp7F7.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F7.elt_type=({void**_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=_tmp7F4;_tmp7F8;});_tmp7F7.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7F7.fat_result=0;_tmp7F7.inline_call=0;_tmp7F7;});_tmp7F6;});_tmp7F5;}),(unsigned int)(yyyylsp[0]).first_line));
# 3041
goto _LL254;}case 479: _LL60D: _LL60E: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3042 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,({struct Cyc_Absyn_Exp*_tmp7F9[1];_tmp7F9[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7F9,sizeof(struct Cyc_Absyn_Exp*),1));}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 480: _LL60F: _LL610: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 3044 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7FB;_tmp7FB.tag=37;_tmp7FB.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7FB.f2=({struct _dyneither_ptr*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp7FC;});_tmp7FB;});_tmp7FA;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 481: _LL611: _LL612: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 3046 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7FE;_tmp7FE.tag=37;_tmp7FE.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);_tmp7FE.f2=({struct _dyneither_ptr*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp7FF;});_tmp7FE;});_tmp7FD;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 482: _LL613: _LL614: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3048 "parse.y"
void*_tmp800=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp800,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 483: _LL615: _LL616: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3051 "parse.y"
struct _tuple20 _tmp801=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp802=_tmp801;int _tmp804;struct _dyneither_ptr _tmp803;_LL6E4: _tmp804=_tmp802.f1;_tmp803=_tmp802.f2;_LL6E5:;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp804,_tmp803,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 484: _LL617: _LL618: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3056 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL254;}case 485: _LL619: _LL61A: {
# 3059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3057 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL254;}case 486: _LL61B: _LL61C: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3058 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL254;}case 487: _LL61D: _LL61E: {
# 3061
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3063 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 488: _LL61F: _LL620: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3065 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 489: _LL621: _LL622: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3067 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 490: _LL623: _LL624: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3069 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 491: _LL625: _LL626: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3071 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),({struct _dyneither_ptr*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp805;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 492: _LL627: _LL628: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3073 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),({struct _dyneither_ptr*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp806;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 493: _LL629: _LL62A: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3075 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 494: _LL62B: _LL62C: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3077 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 495: _LL62D: _LL62E: {
# 3080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 3079 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp808;_tmp808.tag=24;_tmp808.f1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp808.f2=0;_tmp808;});_tmp807;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 496: _LL62F: _LL630: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6));
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6));
# 3081 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80A;_tmp80A.tag=24;_tmp80A.f1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp80A;});_tmp809;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 497: _LL631: _LL632: {
# 3084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7));
# 3086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7));
# 3083 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80C;_tmp80C.tag=24;_tmp80C.f1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80C.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp80C;});_tmp80B;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 498: _LL633: _LL634: {
# 3086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3088 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D->hd=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp80F;_tmp80F.tag=0;_tmp80F.f1=({struct _dyneither_ptr*_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp810;});_tmp80F;});_tmp80E;});_tmp80D->tl=0;_tmp80D;}));
goto _LL254;}case 499: _LL635: _LL636: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3091 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811->hd=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp812=_cycalloc_atomic(sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp813;_tmp813.tag=1;_tmp813.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0]));_tmp813;});_tmp812;});_tmp811->tl=0;_tmp811;}));
goto _LL254;}case 500: _LL637: _LL638: {
# 3094
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3093 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814->hd=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp816;_tmp816.tag=0;_tmp816.f1=({struct _dyneither_ptr*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp817;});_tmp816;});_tmp815;});_tmp814->tl=Cyc_yyget_YY3(yyyyvsp[0]);_tmp814;}));
goto _LL254;}case 501: _LL639: _LL63A: {
# 3096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3096 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818->hd=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp819=_cycalloc_atomic(sizeof(*_tmp819));_tmp819[0]=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp81A;_tmp81A.tag=1;_tmp81A.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2]));_tmp81A;});_tmp819;});_tmp818->tl=Cyc_yyget_YY3(yyyyvsp[0]);_tmp818;}));
goto _LL254;}case 502: _LL63B: _LL63C: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 503: _LL63D: _LL63E: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3104 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 504: _LL63F: _LL640: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3106 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 505: _LL641: _LL642: {
# 3109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3108 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 506: _LL643: _LL644: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3110 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 507: _LL645: _LL646: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2));
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2));
# 3115 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 508: _LL647: _LL648: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 3117 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 509: _LL649: _LL64A: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4));
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4));
# 3120 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 510: _LL64B: _LL64C: {
# 3123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 3125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 3123 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp81C;_tmp81C.tag=28;_tmp81C.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp81C.f2=Cyc_yyget_YY40(yyyyvsp[2]);_tmp81C.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp81C.f4=0;_tmp81C;});_tmp81B;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 511: _LL64D: _LL64E: {
# 3126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5));
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5));
# 3126 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 512: _LL64F: _LL650: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3130 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
goto _LL254;}case 513: _LL651: _LL652: {
# 3133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp81D->tl=0;_tmp81D;}));
goto _LL254;}case 514: _LL653: _LL654: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3));
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3));
# 3138 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp81E->tl=Cyc_yyget_YY4(yyyyvsp[0]);_tmp81E;}));
goto _LL254;}case 515: _LL655: _LL656: {
# 3141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 516: _LL657: _LL658: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3145 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 517: _LL659: _LL65A: {
# 3148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3146 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 518: _LL65B: _LL65C: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3148 "parse.y"
struct _dyneither_ptr _tmp81F=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp81F);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp81F,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3156
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp81F,i,(unsigned int)(yyyylsp[0]).first_line));
# 3158
goto _LL254;}case 519: _LL65D: _LL65E: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3159 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
goto _LL254;}case 520: _LL65F: _LL660: {
# 3162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3163 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->f1=Cyc_Absyn_Rel_n(0);_tmp820->f2=({struct _dyneither_ptr*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp821;});_tmp820;}));
goto _LL254;}case 521: _LL661: _LL662: {
# 3166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3164 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 522: _LL663: _LL664: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3167 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822->f1=Cyc_Absyn_Rel_n(0);_tmp822->f2=({struct _dyneither_ptr*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp823;});_tmp822;}));
goto _LL254;}case 523: _LL665: _LL666: {
# 3170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3168 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 524: _LL667: _LL668: {
# 3171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3173 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 525: _LL669: _LL66A: {
# 3176
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3174 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 526: _LL66B: _LL66C: {
# 3177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3177 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 527: _LL66D: _LL66E: {
# 3180
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3178 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 528: _LL66F: _LL670: {
# 3181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3183 "parse.y"
goto _LL254;}case 529: _LL671: _LL672: {
# 3185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1));
# 3187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1));
# 3183 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL254;}default: _LL673: _LL674:
# 3188
 goto _LL254;}_LL254:;}
# 371 "cyc_bison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 374
yylsp_offset -=yylen;
# 388 "cyc_bison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 391
++ yylsp_offset;
if(yylen == 0){
# 394
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 401
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 411
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(530,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(148,yyn - 148)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 7281) && Cyc_yycheck[_check_known_subscript_notnull(7282,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(7282,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(148,yyn - 148)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1077,yystate)];
# 431
if(yyn > - 32768  && yyn < 7281){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 296 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(7282,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(296,x)])+ 15,count ++);}
msg=({unsigned int _tmp824=(unsigned int)(sze + 15);char*_tmp825=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmp824 + 1));struct _dyneither_ptr _tmp827=_tag_dyneither(_tmp825,sizeof(char),_tmp824 + 1);{unsigned int _tmp826=_tmp824;unsigned int i;for(i=0;i < _tmp826;i ++){_tmp825[i]=(char)'\000';}_tmp825[_tmp826]=(char)0;}_tmp827;});
Cyc_strcpy(msg,({const char*_tmp828="parse error";_tag_dyneither(_tmp828,sizeof(char),12);}));
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 296 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(7282,x + yyn)]== x){
# 453
Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp829=", expecting `";_tag_dyneither(_tmp829,sizeof(char),14);}):(struct _dyneither_ptr)({const char*_tmp82A=" or `";_tag_dyneither(_tmp82A,sizeof(char),6);})));
# 456
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(296,x)]);
Cyc_strcat(msg,({const char*_tmp82B="'";_tag_dyneither(_tmp82B,sizeof(char),2);}));
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
Cyc_yyerror(({const char*_tmp82C="parse error";_tag_dyneither(_tmp82C,sizeof(char),12);}),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp82D=1;_npop_handler(0);return _tmp82D;}
# 485
yychar=-2;}
# 491
yyerrstatus=3;
# 493
goto yyerrhandle;
# 495
yyerrdefault:
# 505 "cyc_bison.simple"
 yyerrpop:
# 507
 if(yyssp_offset == 0){int _tmp82E=1;_npop_handler(0);return _tmp82E;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1077,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 7281) || Cyc_yycheck[_check_known_subscript_notnull(7282,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(7282,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1076){
int _tmp82F=0;_npop_handler(0);return _tmp82F;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3186 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp839=v;struct Cyc_Absyn_Stmt*_tmp853;struct Cyc_Absyn_Exp*_tmp852;struct _tuple0*_tmp851;struct _dyneither_ptr _tmp850;char _tmp84F;union Cyc_Absyn_Cnst _tmp84E;switch((_tmp839.Stmt_tok).tag){case 1: _LL6E7: _tmp84E=(_tmp839.Int_tok).val;_LL6E8:
({struct Cyc_String_pa_PrintArg_struct _tmp83C;_tmp83C.tag=0;_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp84E));({void*_tmp83A[1]={& _tmp83C};Cyc_fprintf(Cyc_stderr,({const char*_tmp83B="%s";_tag_dyneither(_tmp83B,sizeof(char),3);}),_tag_dyneither(_tmp83A,sizeof(void*),1));});});goto _LL6E6;case 2: _LL6E9: _tmp84F=(_tmp839.Char_tok).val;_LL6EA:
({struct Cyc_Int_pa_PrintArg_struct _tmp83F;_tmp83F.tag=1;_tmp83F.f1=(unsigned long)((int)_tmp84F);({void*_tmp83D[1]={& _tmp83F};Cyc_fprintf(Cyc_stderr,({const char*_tmp83E="%c";_tag_dyneither(_tmp83E,sizeof(char),3);}),_tag_dyneither(_tmp83D,sizeof(void*),1));});});goto _LL6E6;case 3: _LL6EB: _tmp850=(_tmp839.String_tok).val;_LL6EC:
({struct Cyc_String_pa_PrintArg_struct _tmp842;_tmp842.tag=0;_tmp842.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp850);({void*_tmp840[1]={& _tmp842};Cyc_fprintf(Cyc_stderr,({const char*_tmp841="\"%s\"";_tag_dyneither(_tmp841,sizeof(char),5);}),_tag_dyneither(_tmp840,sizeof(void*),1));});});goto _LL6E6;case 5: _LL6ED: _tmp851=(_tmp839.QualId_tok).val;_LL6EE:
({struct Cyc_String_pa_PrintArg_struct _tmp845;_tmp845.tag=0;_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp851));({void*_tmp843[1]={& _tmp845};Cyc_fprintf(Cyc_stderr,({const char*_tmp844="%s";_tag_dyneither(_tmp844,sizeof(char),3);}),_tag_dyneither(_tmp843,sizeof(void*),1));});});goto _LL6E6;case 7: _LL6EF: _tmp852=(_tmp839.Exp_tok).val;_LL6F0:
({struct Cyc_String_pa_PrintArg_struct _tmp848;_tmp848.tag=0;_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp852));({void*_tmp846[1]={& _tmp848};Cyc_fprintf(Cyc_stderr,({const char*_tmp847="%s";_tag_dyneither(_tmp847,sizeof(char),3);}),_tag_dyneither(_tmp846,sizeof(void*),1));});});goto _LL6E6;case 8: _LL6F1: _tmp853=(_tmp839.Stmt_tok).val;_LL6F2:
({struct Cyc_String_pa_PrintArg_struct _tmp84B;_tmp84B.tag=0;_tmp84B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp853));({void*_tmp849[1]={& _tmp84B};Cyc_fprintf(Cyc_stderr,({const char*_tmp84A="%s";_tag_dyneither(_tmp84A,sizeof(char),3);}),_tag_dyneither(_tmp849,sizeof(void*),1));});});goto _LL6E6;default: _LL6F3: _LL6F4:
({void*_tmp84C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp84D="?";_tag_dyneither(_tmp84D,sizeof(char),2);}),_tag_dyneither(_tmp84C,sizeof(void*),0));});goto _LL6E6;}_LL6E6:;}
# 3199
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp854=_new_region("yyr");struct _RegionHandle*yyr=& _tmp854;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp855=Cyc_Parse_parse_result;_npop_handler(0);return _tmp855;};
# 3202
;_pop_region(yyr);};}

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
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 38
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 893
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 920
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 924
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 938
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 943
void*Cyc_Absyn_compress_kb(void*);
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 950
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 953
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 955
extern void*Cyc_Absyn_sint_typ;
# 957
void*Cyc_Absyn_float_typ(int);
# 1005
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1056
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
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1098
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1104
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1113
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1115
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1122
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1131
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1136
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1142
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1167
int Cyc_Absyn_fntype_att(void*a);
# 1186
extern int Cyc_Absyn_porting_c_code;struct Cyc_RgnOrder_RgnPO;
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
# 92 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 94
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 97
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 106
return first;}}
# 111
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 126
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 128
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 131
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 140
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned int varloc;struct Cyc_List_List*tms;};
# 147
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 161
typedef void*Cyc_Parse_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Parse_pointer_quals_t;
# 164
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 168
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
({struct _tuple10*_tmp857=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators;_tmp2->hd=_tmp5;_tmp2;});declarators=_tmp857;});
({struct Cyc_List_List*_tmp858=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4;_tmp3->tl=exprs;_tmp3;});exprs=_tmp858;});}
# 175
({struct Cyc_List_List*_tmp859=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp859;});
({struct _tuple10*_tmp85A=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp85A;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 182
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 185
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 187
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 192
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 194
static void*Cyc_Parse_parse_abort(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 196
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc_atomic(sizeof(*_tmp6));({struct Cyc_Parse_Exit_exn_struct _tmp85B=({struct Cyc_Parse_Exit_exn_struct _tmp7;_tmp7.tag=Cyc_Parse_Exit;_tmp7;});_tmp6[0]=_tmp85B;});_tmp6;}));}
# 200
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 202
struct _tuple8*_tmp8=tqt;struct Cyc_Absyn_Tqual _tmpC;void*_tmpB;_LL4: _tmpC=_tmp8->f2;_tmpB=_tmp8->f3;_LL5:;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;
({void*_tmp9=0;({unsigned int _tmp85D=loc;struct _dyneither_ptr _tmp85C=({const char*_tmpA="qualifier on type is ignored";_tag_dyneither(_tmpA,sizeof(char),29);});Cyc_Warn_warn(_tmp85D,_tmp85C,_tag_dyneither(_tmp9,sizeof(void*),0));});});}
# 207
return _tmpB;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 210
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 216
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpD=(void*)pqs->hd;void*_tmpE=_tmpD;void*_tmp12;struct Cyc_Absyn_Exp*_tmp11;switch(*((int*)_tmpE)){case 4: _LL7: _LL8:
 zeroterm=Cyc_Absyn_true_conref;goto _LL6;case 5: _LL9: _LLA:
 zeroterm=Cyc_Absyn_false_conref;goto _LL6;case 7: _LLB: _LLC:
 nullable=Cyc_Absyn_true_conref;goto _LL6;case 6: _LLD: _LLE:
 nullable=Cyc_Absyn_false_conref;goto _LL6;case 3: _LLF: _LL10:
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL6;case 2: _LL11: _LL12:
 bound=Cyc_Absyn_bounds_one_conref;goto _LL6;case 0: _LL13: _tmp11=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpE)->f1;_LL14:
({union Cyc_Absyn_Constraint*_tmp85F=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp85E=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10;_tmp10.tag=1;_tmp10.f1=_tmp11;_tmp10;});_tmpF[0]=_tmp85E;});_tmpF;}));bound=_tmp85F;});goto _LL6;default: _LL15: _tmp12=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpE)->f1;_LL16:
 rgn=_tmp12;goto _LL6;}_LL6:;}
# 228
return({struct _tuple14 _tmp13;_tmp13.f1=nullable;_tmp13.f2=bound;_tmp13.f3=zeroterm;_tmp13.f4=rgn;_tmp13;});}
# 234
struct _tuple0*Cyc_Parse_gensym_enum(){
# 236
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp860=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp860;});({struct _dyneither_ptr*_tmp863=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _dyneither_ptr _tmp862=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp18;_tmp18.tag=1;_tmp18.f1=(unsigned long)enum_counter ++;({void*_tmp16[1]={& _tmp18};({struct _dyneither_ptr _tmp861=({const char*_tmp17="__anonymous_enum_%d__";_tag_dyneither(_tmp17,sizeof(char),22);});Cyc_aprintf(_tmp861,_tag_dyneither(_tmp16,sizeof(void*),1));});});});_tmp15[0]=_tmp862;});_tmp15;});_tmp14->f2=_tmp863;});_tmp14;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 241
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 246
struct _tuple17*_tmp19=field_info;unsigned int _tmp26;struct _tuple0*_tmp25;struct Cyc_Absyn_Tqual _tmp24;void*_tmp23;struct Cyc_List_List*_tmp22;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;_LL18: _tmp26=(_tmp19->f1)->f1;_tmp25=(_tmp19->f1)->f2;_tmp24=(_tmp19->f1)->f3;_tmp23=(_tmp19->f1)->f4;_tmp22=(_tmp19->f1)->f5;_tmp21=(_tmp19->f1)->f6;_tmp20=(_tmp19->f2)->f1;_tmp1F=(_tmp19->f2)->f2;_LL19:;
if(_tmp22 != 0)
({void*_tmp1A=0;({unsigned int _tmp865=loc;struct _dyneither_ptr _tmp864=({const char*_tmp1B="bad type params in struct field";_tag_dyneither(_tmp1B,sizeof(char),32);});Cyc_Warn_err(_tmp865,_tmp864,_tag_dyneither(_tmp1A,sizeof(void*),0));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp25))
({void*_tmp1C=0;({unsigned int _tmp867=loc;struct _dyneither_ptr _tmp866=({const char*_tmp1D="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp1D,sizeof(char),59);});Cyc_Warn_err(_tmp867,_tmp866,_tag_dyneither(_tmp1C,sizeof(void*),0));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->name=(*_tmp25).f2;_tmp1E->tq=_tmp24;_tmp1E->type=_tmp23;_tmp1E->width=_tmp20;_tmp1E->attributes=_tmp21;_tmp1E->requires_clause=_tmp1F;_tmp1E;});}
# 256
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp27;_tmp27.Signed_spec=0;_tmp27.Unsigned_spec=0;_tmp27.Short_spec=0;_tmp27.Long_spec=0;_tmp27.Long_Long_spec=0;_tmp27.Valid_type_spec=0;_tmp27.Type_spec=Cyc_Absyn_sint_typ;_tmp27.loc=loc;_tmp27;});}
# 267
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp28=Cyc_Parse_empty_spec(loc);
_tmp28.Type_spec=t;
_tmp28.Valid_type_spec=1;
return _tmp28;}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp29=Cyc_Parse_empty_spec(loc);
_tmp29.Signed_spec=1;
return _tmp29;}
# 278
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2A=Cyc_Parse_empty_spec(loc);
_tmp2A.Unsigned_spec=1;
return _tmp2A;}
# 283
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2B=Cyc_Parse_empty_spec(loc);
_tmp2B.Short_spec=1;
return _tmp2B;}
# 288
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2C=Cyc_Parse_empty_spec(loc);
_tmp2C.Long_spec=1;
return _tmp2C;}
# 295
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 297
return Cyc_Tcutil_is_array(t)?({
void*_tmp868=t;Cyc_Tcutil_promote_array(_tmp868,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 310 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp2D=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp2E=(struct _tuple8*)x->hd;struct _tuple8*_tmp2F=_tmp2E;struct _dyneither_ptr _tmp49;void**_tmp48;struct _dyneither_ptr*_tmp47;void*_tmp46;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp2F)->f3)->tag == 19){_LL1B: _tmp47=_tmp2F->f1;_tmp46=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2F->f3)->f1;if(_tmp47 != 0){_LL1C:
# 315
{void*_tmp30=_tmp46;void**_tmp3A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30)->tag == 1){_LL22: _tmp3A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30)->f2;_LL23: {
# 319
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _dyneither_ptr _tmp86A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47);({void*_tmp37[1]={& _tmp39};({struct _dyneither_ptr _tmp869=({const char*_tmp38="`%s";_tag_dyneither(_tmp38,sizeof(char),4);});Cyc_aprintf(_tmp869,_tag_dyneither(_tmp37,sizeof(void*),1));});});});_tmp36[0]=_tmp86A;});_tmp36;});
({void*_tmp86F=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp31=_cycalloc(sizeof(*_tmp31));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp86E=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp32;_tmp32.tag=2;({struct Cyc_Absyn_Tvar*_tmp86D=({struct Cyc_Absyn_Tvar*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->name=nm;_tmp33->identity=- 1;({void*_tmp86C=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp86B=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=0;_tmp35.f1=& Cyc_Tcutil_ik;_tmp35;});_tmp34[0]=_tmp86B;});_tmp34;});_tmp33->kind=_tmp86C;});_tmp33;});_tmp32.f1=_tmp86D;});_tmp32;});_tmp31[0]=_tmp86E;});_tmp31;});*_tmp3A=_tmp86F;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 324
({struct Cyc_List_List*_tmp871=({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct _tuple18*_tmp870=({struct _tuple18*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=_tmp47;_tmp3C->f2=_tmp46;_tmp3C;});_tmp3B->hd=_tmp870;});_tmp3B->tl=_tmp2D;_tmp3B;});_tmp2D=_tmp871;});goto _LL1A;}else{if(((struct _tuple8*)_tmp2F)->f1 != 0)goto _LL1F;else{goto _LL1F;}}}else{if(((struct _tuple8*)_tmp2F)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2F)->f3)->tag == 15){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2F)->f3)->f1)->tag == 1){_LL1D: _tmp49=*_tmp2F->f1;_tmp48=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2F->f3)->f1)->f2;_LL1E: {
# 328
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _dyneither_ptr _tmp873=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp49);({void*_tmp43[1]={& _tmp45};({struct _dyneither_ptr _tmp872=({const char*_tmp44="`%s";_tag_dyneither(_tmp44,sizeof(char),4);});Cyc_aprintf(_tmp872,_tag_dyneither(_tmp43,sizeof(void*),1));});});});_tmp42[0]=_tmp873;});_tmp42;});
({void*_tmp878=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp877=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3E;_tmp3E.tag=2;({struct Cyc_Absyn_Tvar*_tmp876=({struct Cyc_Absyn_Tvar*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=nm;_tmp3F->identity=- 1;({void*_tmp875=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp40=_cycalloc(sizeof(*_tmp40));({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp874=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp41;_tmp41.tag=0;_tmp41.f1=& Cyc_Tcutil_rk;_tmp41;});_tmp40[0]=_tmp874;});_tmp40;});_tmp3F->kind=_tmp875;});_tmp3F;});_tmp3E.f1=_tmp876;});_tmp3E;});_tmp3D[0]=_tmp877;});_tmp3D;});*_tmp48=_tmp878;});
goto _LL1A;}}else{goto _LL1F;}}else{goto _LL1F;}}else{_LL1F: _LL20:
 goto _LL1A;}}_LL1A:;}
# 334
return _tmp2D;}
# 338
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp4A=0;
for(0;x != 0;x=x->tl){
void*_tmp4B=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp4C=_tmp4B;void*_tmp4F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->tag == 19){_LL27: _tmp4F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL28:
# 343
({struct Cyc_List_List*_tmp87A=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _tuple18*_tmp879=({struct _tuple18*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name;_tmp4E->f2=_tmp4F;_tmp4E;});_tmp4D->hd=_tmp879;});_tmp4D->tl=_tmp4A;_tmp4D;});_tmp4A=_tmp87A;});goto _LL26;}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 347
return _tmp4A;}
# 351
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp50=e->r;void*_tmp51=_tmp50;struct _dyneither_ptr*_tmp59;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->tag == 1){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->tag == 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->f1)->f1).Rel_n).val == 0){_LL2C: _tmp59=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->f1)->f2;_LL2D:
# 354
{struct Cyc_List_List*_tmp52=tags;for(0;_tmp52 != 0;_tmp52=_tmp52->tl){
struct _tuple18*_tmp53=(struct _tuple18*)_tmp52->hd;struct _tuple18*_tmp54=_tmp53;struct _dyneither_ptr*_tmp58;void*_tmp57;_LL31: _tmp58=_tmp54->f1;_tmp57=_tmp54->f2;_LL32:;
if(({Cyc_strptrcmp(_tmp58,_tmp59)== 0;}))
return({void*_tmp87D=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp55=_cycalloc(sizeof(*_tmp55));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp87C=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp56;_tmp56.tag=38;({void*_tmp87B=Cyc_Tcutil_copy_type(_tmp57);_tmp56.f1=_tmp87B;});_tmp56;});_tmp55[0]=_tmp87C;});_tmp55;});Cyc_Absyn_new_exp(_tmp87D,e->loc);});}}
# 359
goto _LL2B;}else{goto _LL2E;}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 362
return e;}
# 367
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp5A=t;void*_tmp75;struct Cyc_Absyn_Tqual _tmp74;void*_tmp73;union Cyc_Absyn_Constraint*_tmp72;union Cyc_Absyn_Constraint*_tmp71;union Cyc_Absyn_Constraint*_tmp70;struct Cyc_Absyn_PtrLoc*_tmp6F;void*_tmp6E;struct Cyc_Absyn_Tqual _tmp6D;struct Cyc_Absyn_Exp*_tmp6C;union Cyc_Absyn_Constraint*_tmp6B;unsigned int _tmp6A;switch(*((int*)_tmp5A)){case 8: _LL34: _tmp6E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).elt_type;_tmp6D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).tq;_tmp6C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).num_elts;_tmp6B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).zero_term;_tmp6A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).zt_loc;_LL35: {
# 370
struct Cyc_Absyn_Exp*nelts2=_tmp6C;
if(_tmp6C != 0)
({struct Cyc_Absyn_Exp*_tmp87E=Cyc_Parse_substitute_tags_exp(tags,_tmp6C);nelts2=_tmp87E;});{
# 374
void*_tmp5B=Cyc_Parse_substitute_tags(tags,_tmp6E);
if(_tmp6C != nelts2  || _tmp6E != _tmp5B)
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp880=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5D;_tmp5D.tag=8;({struct Cyc_Absyn_ArrayInfo _tmp87F=({struct Cyc_Absyn_ArrayInfo _tmp5E;_tmp5E.elt_type=_tmp5B;_tmp5E.tq=_tmp6D;_tmp5E.num_elts=nelts2;_tmp5E.zero_term=_tmp6B;_tmp5E.zt_loc=_tmp6A;_tmp5E;});_tmp5D.f1=_tmp87F;});_tmp5D;});_tmp5C[0]=_tmp880;});_tmp5C;});
goto _LL33;};}case 5: _LL36: _tmp75=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).elt_typ;_tmp74=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).elt_tq;_tmp73=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).rgn;_tmp72=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).nullable;_tmp71=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).bounds;_tmp70=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).zero_term;_tmp6F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).ptrloc;_LL37: {
# 379
void*_tmp5F=Cyc_Parse_substitute_tags(tags,_tmp75);
union Cyc_Absyn_Constraint*_tmp60=_tmp71;
{union Cyc_Absyn_Constraint _tmp61=*_tmp71;union Cyc_Absyn_Constraint _tmp62=_tmp61;struct Cyc_Absyn_Exp*_tmp66;if((_tmp62.Eq_constr).tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp62.Eq_constr).val)->tag == 1){_LL3B: _tmp66=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp62.Eq_constr).val)->f1;_LL3C:
# 383
({struct Cyc_Absyn_Exp*_tmp881=Cyc_Parse_substitute_tags_exp(tags,_tmp66);_tmp66=_tmp881;});
({union Cyc_Absyn_Constraint*_tmp884=({union Cyc_Absyn_Constraint*_tmp63=_cycalloc(sizeof(*_tmp63));({void*_tmp883=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64=_cycalloc(sizeof(*_tmp64));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp882=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp65;_tmp65.tag=1;_tmp65.f1=_tmp66;_tmp65;});_tmp64[0]=_tmp882;});_tmp64;});(_tmp63->Eq_constr).val=_tmp883;});(_tmp63->Eq_constr).tag=1;_tmp63;});_tmp60=_tmp884;});
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}
# 388
if(_tmp5F != _tmp75  || _tmp60 != _tmp71)
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp887=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp68;_tmp68.tag=5;({struct Cyc_Absyn_PtrInfo _tmp886=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=_tmp5F;_tmp69.elt_tq=_tmp74;({struct Cyc_Absyn_PtrAtts _tmp885=({(_tmp69.ptr_atts).rgn=_tmp73;(_tmp69.ptr_atts).nullable=_tmp72;(_tmp69.ptr_atts).bounds=_tmp60;(_tmp69.ptr_atts).zero_term=_tmp70;(_tmp69.ptr_atts).ptrloc=_tmp6F;_tmp69.ptr_atts;});_tmp69.ptr_atts=_tmp885;});_tmp69;});_tmp68.f1=_tmp886;});_tmp68;});_tmp67[0]=_tmp887;});_tmp67;});
goto _LL33;}default: _LL38: _LL39:
# 393
 goto _LL33;}_LL33:;}
# 395
return t;}
# 400
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp888=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp888;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 407
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 409
return({struct _tuple19*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=(*t).f2;_tmp76->f2=(*t).f3;_tmp76;});}
# 412
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp77=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp77)->tag == 4){_LL40: _LL41:
 return 1;}else{_LL42: _LL43:
 return 0;}_LL3F:;}
# 421
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(({({struct _dyneither_ptr _tmp889=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp889,({const char*_tmp78="`H";_tag_dyneither(_tmp78,sizeof(char),3);}));})== 0;}))
return(void*)& Cyc_Absyn_HeapRgn_val;else{
if(({({struct _dyneither_ptr _tmp88A=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp88A,({const char*_tmp79="`U";_tag_dyneither(_tmp79,sizeof(char),3);}));})== 0;}))
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
if(({({struct _dyneither_ptr _tmp88B=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp88B,({const char*_tmp7A="`RC";_tag_dyneither(_tmp7A,sizeof(char),4);}));})== 0;}))
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 429
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp88E=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7C;_tmp7C.tag=2;({struct Cyc_Absyn_Tvar*_tmp88D=({struct Cyc_Absyn_Tvar*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct _dyneither_ptr*_tmp88C=({struct _dyneither_ptr*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=s;_tmp7E;});_tmp7D->name=_tmp88C;});_tmp7D->identity=- 1;_tmp7D->kind=k;_tmp7D;});_tmp7C.f1=_tmp88D;});_tmp7C;});_tmp7B[0]=_tmp88E;});_tmp7B;});}}}}
# 436
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp7F=t;struct Cyc_Absyn_Tvar*_tmp82;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->tag == 2){_LL45: _tmp82=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->f1;_LL46:
 return _tmp82;}else{_LL47: _LL48:
({void*_tmp80=0;({unsigned int _tmp890=loc;struct _dyneither_ptr _tmp88F=({const char*_tmp81="expecting a list of type variables, not types";_tag_dyneither(_tmp81,sizeof(char),46);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp890,_tmp88F,_tag_dyneither(_tmp80,sizeof(void*),0));});});}_LL44:;}
# 444
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp83=Cyc_Tcutil_compress(t);void*_tmp84=_tmp83;void**_tmp89;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp84)->tag == 2){_LL4A: _tmp89=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp84)->f1)->kind;_LL4B: {
# 447
void*_tmp85=Cyc_Absyn_compress_kb(*_tmp89);void*_tmp86=_tmp85;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp86)->tag == 1){_LL4F: _LL50:
# 449
 if(!leq)({void*_tmp891=Cyc_Tcutil_kind_to_bound(k);*_tmp89=_tmp891;});else{
({void*_tmp893=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp892=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp88;_tmp88.tag=2;_tmp88.f1=0;_tmp88.f2=k;_tmp88;});_tmp87[0]=_tmp892;});_tmp87;});*_tmp89=_tmp893;});}
return;}else{_LL51: _LL52:
 return;}_LL4E:;}}else{_LL4C: _LL4D:
# 454
 return;}_LL49:;}
# 459
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 465
if(tds == 0)return tms;
# 470
if(tms == 0)return 0;{
# 472
void*_tmp8A=(void*)tms->hd;void*_tmp8B=_tmp8A;void*_tmpA8;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp8B)->tag == 3){_LL54: _tmpA8=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp8B)->f1;_LL55:
# 475
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 478
void*_tmp8C=_tmpA8;struct Cyc_List_List*_tmpA6;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp8C)->tag == 1){_LL59: _LL5A:
# 480
({void*_tmp8D=0;({unsigned int _tmp895=loc;struct _dyneither_ptr _tmp894=({const char*_tmp8E="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp8E,sizeof(char),93);});Cyc_Warn_warn(_tmp895,_tmp894,_tag_dyneither(_tmp8D,sizeof(void*),0));});});
# 482
return tms;}else{_LL5B: _tmpA6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp8C)->f1;_LL5C:
# 484
 if(({int _tmp896=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA6);_tmp896 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp8F=0;({unsigned int _tmp898=loc;struct _dyneither_ptr _tmp897=({const char*_tmp90="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp90,sizeof(char),73);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp898,_tmp897,_tag_dyneither(_tmp8F,sizeof(void*),0));});});{
# 488
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpA6 != 0;_tmpA6=_tmpA6->tl){
struct Cyc_List_List*_tmp91=tds;
for(0;_tmp91 != 0;_tmp91=_tmp91->tl){
struct Cyc_Absyn_Decl*_tmp92=(struct Cyc_Absyn_Decl*)_tmp91->hd;
void*_tmp93=_tmp92->r;void*_tmp94=_tmp93;struct Cyc_Absyn_Vardecl*_tmp9D;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp94)->tag == 0){_LL5E: _tmp9D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp94)->f1;_LL5F:
# 495
 if(({Cyc_zstrptrcmp((*_tmp9D->name).f2,(struct _dyneither_ptr*)_tmpA6->hd)!= 0;}))
continue;
if(_tmp9D->initializer != 0)
({void*_tmp95=0;({unsigned int _tmp89A=_tmp92->loc;struct _dyneither_ptr _tmp899=({const char*_tmp96="initializer found in parameter declaration";_tag_dyneither(_tmp96,sizeof(char),43);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp89A,_tmp899,_tag_dyneither(_tmp95,sizeof(void*),0));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp9D->name))
({void*_tmp97=0;({unsigned int _tmp89C=_tmp92->loc;struct _dyneither_ptr _tmp89B=({const char*_tmp98="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp98,sizeof(char),47);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp89C,_tmp89B,_tag_dyneither(_tmp97,sizeof(void*),0));});});
({struct Cyc_List_List*_tmp89E=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));({struct _tuple8*_tmp89D=({struct _tuple8*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->f1=(*_tmp9D->name).f2;_tmp9A->f2=_tmp9D->tq;_tmp9A->f3=_tmp9D->type;_tmp9A;});_tmp99->hd=_tmp89D;});_tmp99->tl=rev_new_params;_tmp99;});rev_new_params=_tmp89E;});
# 504
goto L;}else{_LL60: _LL61:
({void*_tmp9B=0;({unsigned int _tmp8A0=_tmp92->loc;struct _dyneither_ptr _tmp89F=({const char*_tmp9C="nonvariable declaration in parameter type";_tag_dyneither(_tmp9C,sizeof(char),42);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8A0,_tmp89F,_tag_dyneither(_tmp9B,sizeof(void*),0));});});}_LL5D:;}
# 508
L: if(_tmp91 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpA6->hd));({void*_tmp9E[1]={& _tmpA0};({unsigned int _tmp8A2=loc;struct _dyneither_ptr _tmp8A1=({const char*_tmp9F="%s is not given a type";_tag_dyneither(_tmp9F,sizeof(char),23);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8A2,_tmp8A1,_tag_dyneither(_tmp9E,sizeof(void*),1));});});});}
# 511
return({struct Cyc_List_List*_tmpA1=_region_malloc(yy,sizeof(*_tmpA1));({void*_tmp8A7=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA2=_region_malloc(yy,sizeof(*_tmpA2));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8A6=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA3;_tmpA3.tag=3;({void*_tmp8A5=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA4=_region_malloc(yy,sizeof(*_tmpA4));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8A4=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA5;_tmpA5.tag=1;({struct Cyc_List_List*_tmp8A3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmpA5.f1=_tmp8A3;});_tmpA5.f2=0;_tmpA5.f3=0;_tmpA5.f4=0;_tmpA5.f5=0;_tmpA5.f6=0;_tmpA5.f7=0;_tmpA5;});_tmpA4[0]=_tmp8A4;});_tmpA4;});_tmpA3.f1=_tmp8A5;});_tmpA3;});_tmpA2[0]=_tmp8A6;});_tmpA2;});_tmpA1->hd=_tmp8A7;});_tmpA1->tl=0;_tmpA1;});};}_LL58:;}
# 518
goto _LL57;}else{_LL56: _LL57:
 return({struct Cyc_List_List*_tmpA7=_region_malloc(yy,sizeof(*_tmpA7));_tmpA7->hd=(void*)tms->hd;({struct Cyc_List_List*_tmp8A8=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmpA7->tl=_tmp8A8;});_tmpA7;});}_LL53:;};}
# 526
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 530
if(tds != 0)
({struct Cyc_Parse_Declarator _tmp8AA=({struct Cyc_Parse_Declarator _tmpA9;_tmpA9.id=d.id;_tmpA9.varloc=d.varloc;({struct Cyc_List_List*_tmp8A9=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmpA9.tms=_tmp8A9;});_tmpA9;});d=_tmp8AA;});{
# 533
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 539
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 545
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpAA=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmpAB=_tmpAA;switch(_tmpAB){case Cyc_Parse_Extern_sc: _LL63: _LL64:
 sc=Cyc_Absyn_Extern;goto _LL62;case Cyc_Parse_Static_sc: _LL65: _LL66:
 sc=Cyc_Absyn_Static;goto _LL62;default: _LL67: _LL68:
({void*_tmpAC=0;({unsigned int _tmp8AC=loc;struct _dyneither_ptr _tmp8AB=({const char*_tmpAD="bad storage class on function";_tag_dyneither(_tmpAD,sizeof(char),30);});Cyc_Warn_err(_tmp8AC,_tmp8AB,_tag_dyneither(_tmpAC,sizeof(void*),0));});});goto _LL62;}_LL62:;}}{
# 552
void*_tmpAE=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpAF=Cyc_Parse_apply_tms(tq,_tmpAE,atts,d.tms);struct _tuple13 _tmpB0=_tmpAF;struct Cyc_Absyn_Tqual _tmpD5;void*_tmpD4;struct Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD2;_LL6A: _tmpD5=_tmpB0.f1;_tmpD4=_tmpB0.f2;_tmpD3=_tmpB0.f3;_tmpD2=_tmpB0.f4;_LL6B:;
# 557
if(_tmpD3 != 0)
# 560
({void*_tmpB1=0;({unsigned int _tmp8AE=loc;struct _dyneither_ptr _tmp8AD=({const char*_tmpB2="bad type params, ignoring";_tag_dyneither(_tmpB2,sizeof(char),26);});Cyc_Warn_warn(_tmp8AE,_tmp8AD,_tag_dyneither(_tmpB1,sizeof(void*),0));});});{
# 562
void*_tmpB3=_tmpD4;struct Cyc_List_List*_tmpD1;void*_tmpD0;struct Cyc_Absyn_Tqual _tmpCF;void*_tmpCE;struct Cyc_List_List*_tmpCD;int _tmpCC;struct Cyc_Absyn_VarargInfo*_tmpCB;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_List_List*_tmpC5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->tag == 9){_LL6D: _tmpD1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).tvars;_tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).effect;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ret_tqual;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ret_typ;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).args;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).c_varargs;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).cyc_varargs;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).rgn_po;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).attributes;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).requires_clause;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).requires_relns;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ensures_clause;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ensures_relns;_LL6E: {
# 566
struct Cyc_List_List*_tmpB4=0;
{struct Cyc_List_List*_tmpB5=_tmpCD;for(0;_tmpB5 != 0;_tmpB5=_tmpB5->tl){
struct _tuple8*_tmpB6=(struct _tuple8*)_tmpB5->hd;struct _tuple8*_tmpB7=_tmpB6;struct _dyneither_ptr*_tmpC1;struct Cyc_Absyn_Tqual _tmpC0;void*_tmpBF;_LL72: _tmpC1=_tmpB7->f1;_tmpC0=_tmpB7->f2;_tmpBF=_tmpB7->f3;_LL73:;
if(_tmpC1 == 0){
({void*_tmpB8=0;({unsigned int _tmp8B0=loc;struct _dyneither_ptr _tmp8AF=({const char*_tmpB9="missing argument variable in function prototype";_tag_dyneither(_tmpB9,sizeof(char),48);});Cyc_Warn_err(_tmp8B0,_tmp8AF,_tag_dyneither(_tmpB8,sizeof(void*),0));});});
({struct Cyc_List_List*_tmp8B3=({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct _tuple8*_tmp8B2=({struct _tuple8*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _dyneither_ptr*_tmp8B1=({_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2);});_tmpBB->f1=_tmp8B1;});_tmpBB->f2=_tmpC0;_tmpBB->f3=_tmpBF;_tmpBB;});_tmpBA->hd=_tmp8B2;});_tmpBA->tl=_tmpB4;_tmpBA;});_tmpB4=_tmp8B3;});}else{
# 573
({struct Cyc_List_List*_tmp8B5=({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct _tuple8*_tmp8B4=({struct _tuple8*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=_tmpC1;_tmpBE->f2=_tmpC0;_tmpBE->f3=_tmpBF;_tmpBE;});_tmpBD->hd=_tmp8B4;});_tmpBD->tl=_tmpB4;_tmpBD;});_tmpB4=_tmp8B5;});}}}
# 577
return({struct Cyc_Absyn_Fndecl*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->sc=sc;_tmpC2->name=d.id;_tmpC2->tvs=_tmpD1;_tmpC2->is_inline=is_inline;_tmpC2->effect=_tmpD0;_tmpC2->ret_tqual=_tmpCF;_tmpC2->ret_type=_tmpCE;({struct Cyc_List_List*_tmp8B6=
# 580
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB4);_tmpC2->args=_tmp8B6;});_tmpC2->c_varargs=_tmpCC;_tmpC2->cyc_varargs=_tmpCB;_tmpC2->rgn_po=_tmpCA;_tmpC2->body=body;_tmpC2->cached_typ=0;_tmpC2->param_vardecls=0;_tmpC2->fn_vardecl=0;({struct Cyc_List_List*_tmp8B7=
# 587
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpC9,_tmpD2);_tmpC2->attributes=_tmp8B7;});_tmpC2->requires_clause=_tmpC8;_tmpC2->requires_relns=0;_tmpC2->ensures_clause=_tmpC6;_tmpC2->ensures_relns=0;_tmpC2;});}}else{_LL6F: _LL70:
# 592
({void*_tmpC3=0;({unsigned int _tmp8B9=loc;struct _dyneither_ptr _tmp8B8=({const char*_tmpC4="declarator is not a function prototype";_tag_dyneither(_tmpC4,sizeof(char),39);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8B9,_tmp8B8,_tag_dyneither(_tmpC3,sizeof(void*),0));});});}_LL6C:;};};};}static char _tmpD6[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 596
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpD6,_tmpD6,_tmpD6 + 76};static char _tmpD7[87]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 598
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpD7,_tmpD7,_tmpD7 + 87};static char _tmpD8[74]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 600
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpD8,_tmpD8,_tmpD8 + 74};static char _tmpD9[84]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 602
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpD9,_tmpD9,_tmpD9 + 84};
# 609
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 612
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpDA=0;({unsigned int _tmp8BB=loc;struct _dyneither_ptr _tmp8BA=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8BB,_tmp8BA,_tag_dyneither(_tmpDA,sizeof(void*),0));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpDB=0;({unsigned int _tmp8BD=loc;struct _dyneither_ptr _tmp8BC=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8BD,_tmp8BC,_tag_dyneither(_tmpDB,sizeof(void*),0));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpDC=0;({unsigned int _tmp8BF=loc;struct _dyneither_ptr _tmp8BE=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8BF,_tmp8BE,_tag_dyneither(_tmpDC,sizeof(void*),0));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpDD=0;({unsigned int _tmp8C1=loc;struct _dyneither_ptr _tmp8C0=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8C1,_tmp8C0,_tag_dyneither(_tmpDD,sizeof(void*),0));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpDE=0;({unsigned int _tmp8C3=loc;struct _dyneither_ptr _tmp8C2=Cyc_Parse_msg1;Cyc_Warn_err(_tmp8C3,_tmp8C2,_tag_dyneither(_tmpDE,sizeof(void*),0));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 634
return s1;}
# 640
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 643
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 650
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpDF=0;({unsigned int _tmp8C5=loc;struct _dyneither_ptr _tmp8C4=Cyc_Parse_msg4;Cyc_Warn_err(_tmp8C5,_tmp8C4,_tag_dyneither(_tmpDF,sizeof(void*),0));});});
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpE0=0;({unsigned int _tmp8C7=loc;struct _dyneither_ptr _tmp8C6=Cyc_Parse_msg4;Cyc_Warn_err(_tmp8C7,_tmp8C6,_tag_dyneither(_tmpE0,sizeof(void*),0));});});
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 662
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpE1=0;({unsigned int _tmp8C9=loc;struct _dyneither_ptr _tmp8C8=({const char*_tmpE2="missing type within specifier";_tag_dyneither(_tmpE2,sizeof(char),30);});Cyc_Warn_warn(_tmp8C9,_tmp8C8,_tag_dyneither(_tmpE1,sizeof(void*),0));});});
({void*_tmp8CA=Cyc_Absyn_int_typ(sgn,sz);t=_tmp8CA;});}else{
# 667
if(seen_sign){
void*_tmpE3=t;enum Cyc_Absyn_Sign _tmpE7;enum Cyc_Absyn_Size_of _tmpE6;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->tag == 6){_LL75: _tmpE7=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->f1;_tmpE6=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->f2;_LL76:
# 670
 if(_tmpE7 != sgn)
({void*_tmp8CB=Cyc_Absyn_int_typ(sgn,_tmpE6);t=_tmp8CB;});
goto _LL74;}else{_LL77: _LL78:
({void*_tmpE4=0;({unsigned int _tmp8CD=loc;struct _dyneither_ptr _tmp8CC=({const char*_tmpE5="sign specification on non-integral type";_tag_dyneither(_tmpE5,sizeof(char),40);});Cyc_Warn_err(_tmp8CD,_tmp8CC,_tag_dyneither(_tmpE4,sizeof(void*),0));});});goto _LL74;}_LL74:;}
# 675
if(seen_size){
void*_tmpE8=t;enum Cyc_Absyn_Sign _tmpEC;enum Cyc_Absyn_Size_of _tmpEB;switch(*((int*)_tmpE8)){case 6: _LL7A: _tmpEC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE8)->f1;_tmpEB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE8)->f2;_LL7B:
# 678
 if(_tmpEB != sz)
({void*_tmp8CE=Cyc_Absyn_int_typ(_tmpEC,sz);t=_tmp8CE;});
goto _LL79;case 7: _LL7C: _LL7D:
# 682
({void*_tmp8CF=Cyc_Absyn_float_typ(2);t=_tmp8CF;});goto _LL79;default: _LL7E: _LL7F:
({void*_tmpE9=0;({unsigned int _tmp8D1=loc;struct _dyneither_ptr _tmp8D0=({const char*_tmpEA="size qualifier on non-integral type";_tag_dyneither(_tmpEA,sizeof(char),36);});Cyc_Warn_err(_tmp8D1,_tmp8D0,_tag_dyneither(_tmpE9,sizeof(void*),0));});});goto _LL79;}_LL79:;}}
# 686
return t;}
# 689
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 693
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpED=d.id;
unsigned int _tmpEE=d.varloc;
struct _tuple13 _tmpEF=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpF0=_tmpEF;struct Cyc_Absyn_Tqual _tmpF8;void*_tmpF7;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF5;_LL81: _tmpF8=_tmpF0.f1;_tmpF7=_tmpF0.f2;_tmpF6=_tmpF0.f3;_tmpF5=_tmpF0.f4;_LL82:;
# 700
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpF1=_region_malloc(r,sizeof(*_tmpF1));({struct _tuple15*_tmp8D2=({struct _tuple15*_tmpF2=_region_malloc(r,sizeof(*_tmpF2));_tmpF2->f1=_tmpEE;_tmpF2->f2=_tmpED;_tmpF2->f3=_tmpF8;_tmpF2->f4=_tmpF7;_tmpF2->f5=_tmpF6;_tmpF2->f6=_tmpF5;_tmpF2;});_tmpF1->hd=_tmp8D2;});_tmpF1->tl=0;_tmpF1;});else{
# 703
return({struct Cyc_List_List*_tmpF3=_region_malloc(r,sizeof(*_tmpF3));({struct _tuple15*_tmp8D3=({struct _tuple15*_tmpF4=_region_malloc(r,sizeof(*_tmpF4));_tmpF4->f1=_tmpEE;_tmpF4->f2=_tmpED;_tmpF4->f3=_tmpF8;_tmpF4->f4=_tmpF7;_tmpF4->f5=_tmpF6;_tmpF4->f6=_tmpF5;_tmpF4;});_tmpF3->hd=_tmp8D3;});({struct Cyc_List_List*_tmp8D8=({
struct _RegionHandle*_tmp8D7=r;struct Cyc_Absyn_Tqual _tmp8D6=tq;void*_tmp8D5=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp8D4=ds->tl;Cyc_Parse_apply_tmss(_tmp8D7,_tmp8D6,_tmp8D5,_tmp8D4,shared_atts);});_tmpF3->tl=_tmp8D8;});_tmpF3;});}};}
# 707
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 710
if(tms == 0)return({struct _tuple13 _tmpF9;_tmpF9.f1=tq;_tmpF9.f2=t;_tmpF9.f3=0;_tmpF9.f4=atts;_tmpF9;});{
void*_tmpFA=(void*)tms->hd;void*_tmpFB=_tmpFA;unsigned int _tmp124;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_PtrAtts _tmp122;struct Cyc_Absyn_Tqual _tmp121;struct Cyc_List_List*_tmp120;unsigned int _tmp11F;void*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;union Cyc_Absyn_Constraint*_tmp11C;unsigned int _tmp11B;union Cyc_Absyn_Constraint*_tmp11A;unsigned int _tmp119;switch(*((int*)_tmpFB)){case 0: _LL84: _tmp11A=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp119=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL85:
# 713
 return({struct Cyc_Absyn_Tqual _tmp8DB=Cyc_Absyn_empty_tqual(0);void*_tmp8DA=
Cyc_Absyn_array_typ(t,tq,0,_tmp11A,_tmp119);
# 713
struct Cyc_List_List*_tmp8D9=atts;Cyc_Parse_apply_tms(_tmp8DB,_tmp8DA,_tmp8D9,tms->tl);});case 1: _LL86: _tmp11D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp11C=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_tmp11B=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f3;_LL87:
# 716
 return({struct Cyc_Absyn_Tqual _tmp8DE=Cyc_Absyn_empty_tqual(0);void*_tmp8DD=
Cyc_Absyn_array_typ(t,tq,_tmp11D,_tmp11C,_tmp11B);
# 716
struct Cyc_List_List*_tmp8DC=atts;Cyc_Parse_apply_tms(_tmp8DE,_tmp8DD,_tmp8DC,tms->tl);});case 3: _LL88: _tmp11E=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_LL89: {
# 719
void*_tmpFC=_tmp11E;unsigned int _tmp112;struct Cyc_List_List*_tmp111;int _tmp110;struct Cyc_Absyn_VarargInfo*_tmp10F;void*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->tag == 1){_LL91: _tmp111=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f1;_tmp110=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f2;_tmp10F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f3;_tmp10E=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f4;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f5;_tmp10C=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f6;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f7;_LL92: {
# 721
struct Cyc_List_List*typvars=0;
# 723
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
({struct Cyc_List_List*_tmp8DF=({struct Cyc_List_List*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->hd=(void*)as->hd;_tmpFD->tl=fn_atts;_tmpFD;});fn_atts=_tmp8DF;});else{
# 728
({struct Cyc_List_List*_tmp8E0=({struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->hd=(void*)as->hd;_tmpFE->tl=new_atts;_tmpFE;});new_atts=_tmp8E0;});}}}
# 731
if(tms->tl != 0){
void*_tmpFF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp100=_tmpFF;struct Cyc_List_List*_tmp101;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp100)->tag == 4){_LL96: _tmp101=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp100)->f1;_LL97:
# 734
 typvars=_tmp101;
tms=tms->tl;
goto _LL95;}else{_LL98: _LL99:
# 738
 goto _LL95;}_LL95:;}
# 742
if(((((!_tmp110  && _tmp10F == 0) && _tmp111 != 0) && _tmp111->tl == 0) && (*((struct _tuple8*)_tmp111->hd)).f1 == 0) && (*((struct _tuple8*)_tmp111->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 747
_tmp111=0;{
# 750
struct Cyc_List_List*_tmp102=Cyc_Parse_get_arg_tags(_tmp111);
# 752
if(_tmp102 != 0)
({void*_tmp8E1=Cyc_Parse_substitute_tags(_tmp102,t);t=_tmp8E1;});
({void*_tmp8E2=Cyc_Parse_array2ptr(t,0);t=_tmp8E2;});
# 757
{struct Cyc_List_List*_tmp103=_tmp111;for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
struct _tuple8*_tmp104=(struct _tuple8*)_tmp103->hd;struct _tuple8*_tmp105=_tmp104;struct _dyneither_ptr*_tmp108;struct Cyc_Absyn_Tqual _tmp107;void**_tmp106;_LL9B: _tmp108=_tmp105->f1;_tmp107=_tmp105->f2;_tmp106=(void**)& _tmp105->f3;_LL9C:;
if(_tmp102 != 0)
({void*_tmp8E3=Cyc_Parse_substitute_tags(_tmp102,*_tmp106);*_tmp106=_tmp8E3;});
({void*_tmp8E4=Cyc_Parse_array2ptr(*_tmp106,1);*_tmp106=_tmp8E4;});}}
# 769
return({struct Cyc_Absyn_Tqual _tmp8E7=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp8E6=
Cyc_Absyn_function_typ(typvars,_tmp10E,tq,t,_tmp111,_tmp110,_tmp10F,_tmp10D,fn_atts,_tmp10C,_tmp10B);
# 769
struct Cyc_List_List*_tmp8E5=new_atts;Cyc_Parse_apply_tms(_tmp8E7,_tmp8E6,_tmp8E5,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL93: _tmp112=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpFC)->f2;_LL94:
# 776
({void*_tmp109=0;({unsigned int _tmp8E9=_tmp112;struct _dyneither_ptr _tmp8E8=({const char*_tmp10A="function declaration without parameter types";_tag_dyneither(_tmp10A,sizeof(char),45);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8E9,_tmp8E8,_tag_dyneither(_tmp109,sizeof(void*),0));});});}_LL90:;}case 4: _LL8A: _tmp120=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp11F=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL8B:
# 783
 if(tms->tl == 0)
return({struct _tuple13 _tmp113;_tmp113.f1=tq;_tmp113.f2=t;_tmp113.f3=_tmp120;_tmp113.f4=atts;_tmp113;});
# 788
({void*_tmp114=0;({unsigned int _tmp8EB=_tmp11F;struct _dyneither_ptr _tmp8EA=({const char*_tmp115="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmp115,sizeof(char),68);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8EB,_tmp8EA,_tag_dyneither(_tmp114,sizeof(void*),0));});});case 2: _LL8C: _tmp122=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp121=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL8D:
# 791
 return({struct Cyc_Absyn_Tqual _tmp8F0=_tmp121;void*_tmp8EF=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8ED=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp117;_tmp117.tag=5;({struct Cyc_Absyn_PtrInfo _tmp8EC=({struct Cyc_Absyn_PtrInfo _tmp118;_tmp118.elt_typ=t;_tmp118.elt_tq=tq;_tmp118.ptr_atts=_tmp122;_tmp118;});_tmp117.f1=_tmp8EC;});_tmp117;});_tmp116[0]=_tmp8ED;});_tmp116;});struct Cyc_List_List*_tmp8EE=atts;Cyc_Parse_apply_tms(_tmp8F0,_tmp8EF,_tmp8EE,tms->tl);});default: _LL8E: _tmp124=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp123=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL8F:
# 796
 return({struct Cyc_Absyn_Tqual _tmp8F3=tq;void*_tmp8F2=t;struct Cyc_List_List*_tmp8F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp123);Cyc_Parse_apply_tms(_tmp8F3,_tmp8F2,_tmp8F1,tms->tl);});}_LL83:;};}
# 802
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 811
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp125=t;unsigned int _tmp130;struct _tuple0*_tmp12F;struct Cyc_Absyn_Tqual _tmp12E;void*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12B;_LL9E: _tmp130=_tmp125->f1;_tmp12F=_tmp125->f2;_tmp12E=_tmp125->f3;_tmp12D=_tmp125->f4;_tmp12C=_tmp125->f5;_tmp12B=_tmp125->f6;_LL9F:;
# 814
Cyc_Lex_register_typedef(_tmp12F);{
# 816
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp126=_tmp12D;struct Cyc_Core_Opt*_tmp127;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp126)->tag == 1){_LLA1: _tmp127=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp126)->f1;_LLA2:
# 820
 type=0;
if(_tmp127 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp127;}
goto _LLA0;}else{_LLA3: _LLA4:
 kind=0;type=_tmp12D;goto _LLA0;}_LLA0:;}
# 826
return({void*_tmp8F6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp8F5=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp129;_tmp129.tag=8;({struct Cyc_Absyn_Typedefdecl*_tmp8F4=({struct Cyc_Absyn_Typedefdecl*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->name=_tmp12F;_tmp12A->tvs=_tmp12C;_tmp12A->kind=kind;_tmp12A->defn=type;_tmp12A->atts=_tmp12B;_tmp12A->tq=_tmp12E;_tmp12A->extern_c=0;_tmp12A;});_tmp129.f1=_tmp8F4;});_tmp129;});_tmp128[0]=_tmp8F5;});_tmp128;});Cyc_Absyn_new_decl(_tmp8F6,loc);});};}
# 833
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp8F8=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp8F7=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp132;_tmp132.tag=12;_tmp132.f1=d;_tmp132.f2=s;_tmp132;});_tmp131[0]=_tmp8F7;});_tmp131;});Cyc_Absyn_new_stmt(_tmp8F8,d->loc);});}
# 839
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 848
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 852
struct _RegionHandle _tmp133=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp133;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp134=ds;struct Cyc_Absyn_Tqual _tmp19D;struct Cyc_Parse_Type_specifier _tmp19C;struct Cyc_List_List*_tmp19B;_LLA6: _tmp19D=_tmp134.tq;_tmp19C=_tmp134.type_specs;_tmp19B=_tmp134.attributes;_LLA7:;
if(_tmp19D.loc == 0)_tmp19D.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp135=0;({unsigned int _tmp8FA=loc;struct _dyneither_ptr _tmp8F9=({const char*_tmp136="inline qualifier on non-function definition";_tag_dyneither(_tmp136,sizeof(char),44);});Cyc_Warn_warn(_tmp8FA,_tmp8F9,_tag_dyneither(_tmp135,sizeof(void*),0));});});{
# 858
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp137=*ds.sc;enum Cyc_Parse_Storage_class _tmp138=_tmp137;switch(_tmp138){case Cyc_Parse_Typedef_sc: _LLA9: _LLAA:
 istypedef=1;goto _LLA8;case Cyc_Parse_Extern_sc: _LLAB: _LLAC:
 s=Cyc_Absyn_Extern;goto _LLA8;case Cyc_Parse_ExternC_sc: _LLAD: _LLAE:
 s=Cyc_Absyn_ExternC;goto _LLA8;case Cyc_Parse_Static_sc: _LLAF: _LLB0:
 s=Cyc_Absyn_Static;goto _LLA8;case Cyc_Parse_Auto_sc: _LLB1: _LLB2:
 s=Cyc_Absyn_Public;goto _LLA8;case Cyc_Parse_Register_sc: _LLB3: _LLB4:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}goto _LLA8;default: _LLB5: _LLB6:
 s=Cyc_Absyn_Abstract;goto _LLA8;}_LLA8:;}{
# 874
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 878
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 886
void*_tmp139=Cyc_Parse_collapse_type_specifiers(_tmp19C,loc);
if(declarators == 0){
# 890
void*_tmp13A=_tmp139;struct Cyc_List_List*_tmp181;struct _tuple0*_tmp180;struct _tuple0*_tmp17F;int _tmp17E;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Datatypedecl**_tmp17C;enum Cyc_Absyn_AggrKind _tmp17B;struct _tuple0*_tmp17A;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Datatypedecl*_tmp178;struct Cyc_Absyn_Enumdecl*_tmp177;struct Cyc_Absyn_Aggrdecl*_tmp176;switch(*((int*)_tmp13A)){case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)){case 0: _LLB8: _tmp176=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)->f1;_LLB9:
# 892
({struct Cyc_List_List*_tmp8FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp176->attributes,_tmp19B);_tmp176->attributes=_tmp8FB;});
_tmp176->sc=s;{
struct Cyc_List_List*_tmp13E=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));({struct Cyc_Absyn_Decl*_tmp8FE=({void*_tmp8FD=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp8FC=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp13D;_tmp13D.tag=5;_tmp13D.f1=_tmp176;_tmp13D;});_tmp13C[0]=_tmp8FC;});_tmp13C;});Cyc_Absyn_new_decl(_tmp8FD,loc);});_tmp13B->hd=_tmp8FE;});_tmp13B->tl=0;_tmp13B;});_npop_handler(0);return _tmp13E;};case 1: _LLBA: _tmp177=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)->f1;_LLBB:
# 896
 if(_tmp19B != 0)({void*_tmp13F=0;({unsigned int _tmp900=loc;struct _dyneither_ptr _tmp8FF=({const char*_tmp140="attributes on enum not supported";_tag_dyneither(_tmp140,sizeof(char),33);});Cyc_Warn_err(_tmp900,_tmp8FF,_tag_dyneither(_tmp13F,sizeof(void*),0));});});
_tmp177->sc=s;{
struct Cyc_List_List*_tmp144=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_Absyn_Decl*_tmp903=({void*_tmp902=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp142=_cycalloc(sizeof(*_tmp142));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp901=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp143;_tmp143.tag=7;_tmp143.f1=_tmp177;_tmp143;});_tmp142[0]=_tmp901;});_tmp142;});Cyc_Absyn_new_decl(_tmp902,loc);});_tmp141->hd=_tmp903;});_tmp141->tl=0;_tmp141;});_npop_handler(0);return _tmp144;};default: _LLBC: _tmp178=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)->f1;_LLBD:
# 900
 if(_tmp19B != 0)({void*_tmp145=0;({unsigned int _tmp905=loc;struct _dyneither_ptr _tmp904=({const char*_tmp146="attributes on datatypes not supported";_tag_dyneither(_tmp146,sizeof(char),38);});Cyc_Warn_err(_tmp905,_tmp904,_tag_dyneither(_tmp145,sizeof(void*),0));});});
_tmp178->sc=s;{
struct Cyc_List_List*_tmp14A=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));({struct Cyc_Absyn_Decl*_tmp908=({void*_tmp907=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp906=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp149;_tmp149.tag=6;_tmp149.f1=_tmp178;_tmp149;});_tmp148[0]=_tmp906;});_tmp148;});Cyc_Absyn_new_decl(_tmp907,loc);});_tmp147->hd=_tmp908;});_tmp147->tl=0;_tmp147;});_npop_handler(0);return _tmp14A;};}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).aggr_info).UnknownAggr).tag == 1){_LLBE: _tmp17B=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).aggr_info).UnknownAggr).val).f1;_tmp17A=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).aggr_info).UnknownAggr).val).f2;_tmp179=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).targs;_LLBF: {
# 904
struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Aggrdecl*_tmp14C=({struct Cyc_Absyn_Aggrdecl*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->kind=_tmp17B;_tmp153->sc=s;_tmp153->name=_tmp17A;_tmp153->tvs=_tmp14B;_tmp153->impl=0;_tmp153->attributes=0;_tmp153->expected_mem_kind=0;_tmp153;});
if(_tmp19B != 0)({void*_tmp14D=0;({unsigned int _tmp90A=loc;struct _dyneither_ptr _tmp909=({const char*_tmp14E="bad attributes on type declaration";_tag_dyneither(_tmp14E,sizeof(char),35);});Cyc_Warn_err(_tmp90A,_tmp909,_tag_dyneither(_tmp14D,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp152=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));({struct Cyc_Absyn_Decl*_tmp90D=({void*_tmp90C=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp150=_cycalloc(sizeof(*_tmp150));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp90B=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp151;_tmp151.tag=5;_tmp151.f1=_tmp14C;_tmp151;});_tmp150[0]=_tmp90B;});_tmp150;});Cyc_Absyn_new_decl(_tmp90C,loc);});_tmp14F->hd=_tmp90D;});_tmp14F->tl=0;_tmp14F;});_npop_handler(0);return _tmp152;};}}else{goto _LLC8;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).KnownDatatype).tag == 2){_LLC0: _tmp17C=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).KnownDatatype).val;_LLC1:
# 909
 if(_tmp19B != 0)({void*_tmp154=0;({unsigned int _tmp90F=loc;struct _dyneither_ptr _tmp90E=({const char*_tmp155="bad attributes on datatype";_tag_dyneither(_tmp155,sizeof(char),27);});Cyc_Warn_err(_tmp90F,_tmp90E,_tag_dyneither(_tmp154,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp159=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));({struct Cyc_Absyn_Decl*_tmp912=({void*_tmp911=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp910=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp158;_tmp158.tag=6;_tmp158.f1=*_tmp17C;_tmp158;});_tmp157[0]=_tmp910;});_tmp157;});Cyc_Absyn_new_decl(_tmp911,loc);});_tmp156->hd=_tmp912;});_tmp156->tl=0;_tmp156;});_npop_handler(0);return _tmp159;};}else{_LLC2: _tmp17F=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).UnknownDatatype).val).name;_tmp17E=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp17D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).targs;_LLC3: {
# 912
struct Cyc_List_List*_tmp15A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17D);
struct Cyc_Absyn_Decl*_tmp15B=Cyc_Absyn_datatype_decl(s,_tmp17F,_tmp15A,0,_tmp17E,loc);
if(_tmp19B != 0)({void*_tmp15C=0;({unsigned int _tmp914=loc;struct _dyneither_ptr _tmp913=({const char*_tmp15D="bad attributes on datatype";_tag_dyneither(_tmp15D,sizeof(char),27);});Cyc_Warn_err(_tmp914,_tmp913,_tag_dyneither(_tmp15C,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp15F=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->hd=_tmp15B;_tmp15E->tl=0;_tmp15E;});_npop_handler(0);return _tmp15F;};}}case 13: _LLC4: _tmp180=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp13A)->f1;_LLC5: {
# 917
struct Cyc_Absyn_Enumdecl*_tmp160=({struct Cyc_Absyn_Enumdecl*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->sc=s;_tmp168->name=_tmp180;_tmp168->fields=0;_tmp168;});
if(_tmp19B != 0)({void*_tmp161=0;({unsigned int _tmp916=loc;struct _dyneither_ptr _tmp915=({const char*_tmp162="bad attributes on enum";_tag_dyneither(_tmp162,sizeof(char),23);});Cyc_Warn_err(_tmp916,_tmp915,_tag_dyneither(_tmp161,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp167=({struct Cyc_List_List*_tmp163=_cycalloc(sizeof(*_tmp163));({struct Cyc_Absyn_Decl*_tmp919=({struct Cyc_Absyn_Decl*_tmp164=_cycalloc(sizeof(*_tmp164));({void*_tmp918=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp165=_cycalloc(sizeof(*_tmp165));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp917=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp166;_tmp166.tag=7;_tmp166.f1=_tmp160;_tmp166;});_tmp165[0]=_tmp917;});_tmp165;});_tmp164->r=_tmp918;});_tmp164->loc=loc;_tmp164;});_tmp163->hd=_tmp919;});_tmp163->tl=0;_tmp163;});_npop_handler(0);return _tmp167;};}case 14: _LLC6: _tmp181=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp13A)->f1;_LLC7: {
# 923
struct Cyc_Absyn_Enumdecl*_tmp169=({struct Cyc_Absyn_Enumdecl*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->sc=s;({struct _tuple0*_tmp91A=Cyc_Parse_gensym_enum();_tmp171->name=_tmp91A;});({struct Cyc_Core_Opt*_tmp91B=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->v=_tmp181;_tmp172;});_tmp171->fields=_tmp91B;});_tmp171;});
if(_tmp19B != 0)({void*_tmp16A=0;({unsigned int _tmp91D=loc;struct _dyneither_ptr _tmp91C=({const char*_tmp16B="bad attributes on enum";_tag_dyneither(_tmp16B,sizeof(char),23);});Cyc_Warn_err(_tmp91D,_tmp91C,_tag_dyneither(_tmp16A,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp170=({struct Cyc_List_List*_tmp16C=_cycalloc(sizeof(*_tmp16C));({struct Cyc_Absyn_Decl*_tmp920=({struct Cyc_Absyn_Decl*_tmp16D=_cycalloc(sizeof(*_tmp16D));({void*_tmp91F=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp91E=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp16F;_tmp16F.tag=7;_tmp16F.f1=_tmp169;_tmp16F;});_tmp16E[0]=_tmp91E;});_tmp16E;});_tmp16D->r=_tmp91F;});_tmp16D->loc=loc;_tmp16D;});_tmp16C->hd=_tmp920;});_tmp16C->tl=0;_tmp16C;});_npop_handler(0);return _tmp170;};}default: _LLC8: _LLC9:
({void*_tmp173=0;({unsigned int _tmp922=loc;struct _dyneither_ptr _tmp921=({const char*_tmp174="missing declarator";_tag_dyneither(_tmp174,sizeof(char),19);});Cyc_Warn_err(_tmp922,_tmp921,_tag_dyneither(_tmp173,sizeof(void*),0));});});{struct Cyc_List_List*_tmp175=0;_npop_handler(0);return _tmp175;};}_LLB7:;}else{
# 930
struct Cyc_List_List*_tmp182=Cyc_Parse_apply_tmss(mkrgn,_tmp19D,_tmp139,declarators,_tmp19B);
if(istypedef){
# 935
if(!exps_empty)
({void*_tmp183=0;({unsigned int _tmp924=loc;struct _dyneither_ptr _tmp923=({const char*_tmp184="initializer in typedef declaration";_tag_dyneither(_tmp184,sizeof(char),35);});Cyc_Warn_err(_tmp924,_tmp923,_tag_dyneither(_tmp183,sizeof(void*),0));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp182);
struct Cyc_List_List*_tmp185=decls;_npop_handler(0);return _tmp185;};}else{
# 941
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp186=_tmp182;for(0;_tmp186 != 0;(_tmp186=_tmp186->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp187=(struct _tuple15*)_tmp186->hd;struct _tuple15*_tmp188=_tmp187;unsigned int _tmp199;struct _tuple0*_tmp198;struct Cyc_Absyn_Tqual _tmp197;void*_tmp196;struct Cyc_List_List*_tmp195;struct Cyc_List_List*_tmp194;_LLCB: _tmp199=_tmp188->f1;_tmp198=_tmp188->f2;_tmp197=_tmp188->f3;_tmp196=_tmp188->f4;_tmp195=_tmp188->f5;_tmp194=_tmp188->f6;_LLCC:;
if(_tmp195 != 0)
({void*_tmp189=0;({unsigned int _tmp926=loc;struct _dyneither_ptr _tmp925=({const char*_tmp18A="bad type params, ignoring";_tag_dyneither(_tmp18A,sizeof(char),26);});Cyc_Warn_warn(_tmp926,_tmp925,_tag_dyneither(_tmp189,sizeof(void*),0));});});
if(exprs == 0)
({void*_tmp18B=0;({unsigned int _tmp928=loc;struct _dyneither_ptr _tmp927=({const char*_tmp18C="unexpected NULL in parse!";_tag_dyneither(_tmp18C,sizeof(char),26);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp928,_tmp927,_tag_dyneither(_tmp18B,sizeof(void*),0));});});{
struct Cyc_Absyn_Exp*_tmp18D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp18E=Cyc_Absyn_new_vardecl(_tmp199,_tmp198,_tmp196,_tmp18D);
_tmp18E->tq=_tmp197;
_tmp18E->sc=s;
_tmp18E->attributes=_tmp194;{
struct Cyc_Absyn_Decl*_tmp18F=({struct Cyc_Absyn_Decl*_tmp191=_cycalloc(sizeof(*_tmp191));({void*_tmp92A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp192=_cycalloc(sizeof(*_tmp192));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp929=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp193;_tmp193.tag=0;_tmp193.f1=_tmp18E;_tmp193;});_tmp192[0]=_tmp929;});_tmp192;});_tmp191->r=_tmp92A;});_tmp191->loc=loc;_tmp191;});
({struct Cyc_List_List*_tmp92B=({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=_tmp18F;_tmp190->tl=decls;_tmp190;});decls=_tmp92B;});};};}}{
# 956
struct Cyc_List_List*_tmp19A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp19A;};}}};};};};}
# 853
;_pop_region(mkrgn);}
# 962
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(({Cyc_strlen((struct _dyneither_ptr)s)== 1;}) || ({Cyc_strlen((struct _dyneither_ptr)s)== 2;})){
char _tmp19E=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp19F=_tmp19E;switch(_tmp19F){case 65: _LLCE: _LLCF:
 return& Cyc_Tcutil_ak;case 77: _LLD0: _LLD1:
 return& Cyc_Tcutil_mk;case 66: _LLD2: _LLD3:
 return& Cyc_Tcutil_bk;case 82: _LLD4: _LLD5:
 return& Cyc_Tcutil_rk;case 69: _LLD6: _LLD7:
 return& Cyc_Tcutil_ek;case 73: _LLD8: _LLD9:
 return& Cyc_Tcutil_ik;case 85: _LLDA: _LLDB:
# 972
{char _tmp1A0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp1A1=_tmp1A0;switch(_tmp1A1){case 82: _LLE1: _LLE2:
 return& Cyc_Tcutil_urk;case 65: _LLE3: _LLE4:
 return& Cyc_Tcutil_uak;case 77: _LLE5: _LLE6:
 return& Cyc_Tcutil_umk;case 66: _LLE7: _LLE8:
 return& Cyc_Tcutil_ubk;default: _LLE9: _LLEA:
 goto _LLE0;}_LLE0:;}
# 979
goto _LLCD;case 84: _LLDC: _LLDD:
# 981
{char _tmp1A2=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp1A3=_tmp1A2;switch(_tmp1A3){case 82: _LLEC: _LLED:
 return& Cyc_Tcutil_trk;case 65: _LLEE: _LLEF:
 return& Cyc_Tcutil_tak;case 77: _LLF0: _LLF1:
 return& Cyc_Tcutil_tmk;case 66: _LLF2: _LLF3:
 return& Cyc_Tcutil_tbk;default: _LLF4: _LLF5:
 goto _LLEB;}_LLEB:;}
# 988
goto _LLCD;default: _LLDE: _LLDF:
 goto _LLCD;}_LLCD:;}
# 991
({struct Cyc_Int_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=1;({unsigned long _tmp92C=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp1A7.f1=_tmp92C;});({struct Cyc_String_pa_PrintArg_struct _tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1A4[2]={& _tmp1A6,& _tmp1A7};({unsigned int _tmp92E=loc;struct _dyneither_ptr _tmp92D=({const char*_tmp1A5="bad kind: %s; strlen=%d";_tag_dyneither(_tmp1A5,sizeof(char),24);});Cyc_Warn_err(_tmp92E,_tmp92D,_tag_dyneither(_tmp1A4,sizeof(void*),2));});});});});
return& Cyc_Tcutil_bk;}
# 996
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A8=e->r;void*_tmp1A9=_tmp1A8;int _tmp1AC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).Int_c).tag == 5){_LLF7: _tmp1AC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).Int_c).val).f2;_LLF8:
 return _tmp1AC;}else{goto _LLF9;}}else{_LLF9: _LLFA:
# 1000
({void*_tmp1AA=0;({unsigned int _tmp930=loc;struct _dyneither_ptr _tmp92F=({const char*_tmp1AB="expecting integer constant";_tag_dyneither(_tmp1AB,sizeof(char),27);});Cyc_Warn_err(_tmp930,_tmp92F,_tag_dyneither(_tmp1AA,sizeof(void*),0));});});
return 0;}_LLF6:;}
# 1006
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1AD=e->r;void*_tmp1AE=_tmp1AD;struct _dyneither_ptr _tmp1B1;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE)->f1).String_c).tag == 8){_LLFC: _tmp1B1=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE)->f1).String_c).val;_LLFD:
 return _tmp1B1;}else{goto _LLFE;}}else{_LLFE: _LLFF:
# 1010
({void*_tmp1AF=0;({unsigned int _tmp932=loc;struct _dyneither_ptr _tmp931=({const char*_tmp1B0="expecting string constant";_tag_dyneither(_tmp1B0,sizeof(char),26);});Cyc_Warn_err(_tmp932,_tmp931,_tag_dyneither(_tmp1AF,sizeof(void*),0));});});
return _tag_dyneither(0,0,0);}_LLFB:;}
# 1016
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1B2=x;long long _tmp1BA;char _tmp1B9;int _tmp1B8;switch((_tmp1B2.LongLong_c).tag){case 5: _LL101: _tmp1B8=((_tmp1B2.Int_c).val).f2;_LL102:
 return(unsigned int)_tmp1B8;case 2: _LL103: _tmp1B9=((_tmp1B2.Char_c).val).f2;_LL104:
 return(unsigned int)_tmp1B9;case 6: _LL105: _tmp1BA=((_tmp1B2.LongLong_c).val).f2;_LL106: {
# 1021
unsigned long long y=(unsigned long long)_tmp1BA;
if(y > -1)
({void*_tmp1B3=0;({unsigned int _tmp934=loc;struct _dyneither_ptr _tmp933=({const char*_tmp1B4="integer constant too large";_tag_dyneither(_tmp1B4,sizeof(char),27);});Cyc_Warn_err(_tmp934,_tmp933,_tag_dyneither(_tmp1B3,sizeof(void*),0));});});
return(unsigned int)_tmp1BA;}default: _LL107: _LL108:
# 1026
({struct Cyc_String_pa_PrintArg_struct _tmp1B7;_tmp1B7.tag=0;({struct _dyneither_ptr _tmp935=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp1B7.f1=_tmp935;});({void*_tmp1B5[1]={& _tmp1B7};({unsigned int _tmp937=loc;struct _dyneither_ptr _tmp936=({const char*_tmp1B6="expected integer constant but found %s";_tag_dyneither(_tmp1B6,sizeof(char),39);});Cyc_Warn_err(_tmp937,_tmp936,_tag_dyneither(_tmp1B5,sizeof(void*),1));});});});
return 0;}_LL100:;}
# 1032
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1BB=p->r;void*_tmp1BC=_tmp1BB;struct Cyc_Absyn_Exp*_tmp1C9;struct _tuple0*_tmp1C8;struct Cyc_List_List*_tmp1C7;struct _dyneither_ptr _tmp1C6;int _tmp1C5;char _tmp1C4;enum Cyc_Absyn_Sign _tmp1C3;int _tmp1C2;struct Cyc_Absyn_Pat*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C0;struct _tuple0*_tmp1BF;switch(*((int*)_tmp1BC)){case 15: _LL10A: _tmp1BF=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL10B:
 return Cyc_Absyn_unknownid_exp(_tmp1BF,p->loc);case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2)->r)->tag == 0){_LL10C: _tmp1C0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL10D:
# 1036
 return({struct Cyc_Absyn_Exp*_tmp938=Cyc_Absyn_unknownid_exp(_tmp1C0->name,p->loc);Cyc_Absyn_deref_exp(_tmp938,p->loc);});}else{goto _LL11C;}case 6: _LL10E: _tmp1C1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL10F:
 return({struct Cyc_Absyn_Exp*_tmp939=Cyc_Parse_pat2exp(_tmp1C1);Cyc_Absyn_address_exp(_tmp939,p->loc);});case 9: _LL110: _LL111:
 return Cyc_Absyn_null_exp(p->loc);case 10: _LL112: _tmp1C3=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_tmp1C2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2;_LL113:
 return Cyc_Absyn_int_exp(_tmp1C3,_tmp1C2,p->loc);case 11: _LL114: _tmp1C4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL115:
 return Cyc_Absyn_char_exp(_tmp1C4,p->loc);case 12: _LL116: _tmp1C6=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_tmp1C5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2;_LL117:
 return Cyc_Absyn_float_exp(_tmp1C6,_tmp1C5,p->loc);case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BC)->f3 == 0){_LL118: _tmp1C8=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_tmp1C7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2;_LL119: {
# 1043
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1C8,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1C7);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL11C;}case 17: _LL11A: _tmp1C9=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL11B:
 return _tmp1C9;default: _LL11C: _LL11D:
# 1048
({void*_tmp1BD=0;({unsigned int _tmp93B=p->loc;struct _dyneither_ptr _tmp93A=({const char*_tmp1BE="cannot mix patterns and expressions in case";_tag_dyneither(_tmp1BE,sizeof(char),44);});Cyc_Warn_err(_tmp93B,_tmp93A,_tag_dyneither(_tmp1BD,sizeof(void*),0));});});
return Cyc_Absyn_null_exp(p->loc);}_LL109:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1CB[7]="cnst_t";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 7}};
# 1107
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1CC=yy1;union Cyc_Absyn_Cnst _tmp1CD;if((_tmp1CC.Int_tok).tag == 1){_LL11F: _tmp1CD=(_tmp1CC.Int_tok).val;_LL120:
# 1111
 yyzzz=_tmp1CD;
goto _LL11E;}else{_LL121: _LL122:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL11E:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp1CE;(_tmp1CE.Int_tok).val=yy1;(_tmp1CE.Int_tok).tag=1;_tmp1CE;});}static char _tmp1D0[5]="char";
# 1106 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1D0,_tmp1D0,_tmp1D0 + 5}};
# 1108
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1D1=yy1;char _tmp1D2;if((_tmp1D1.Char_tok).tag == 2){_LL124: _tmp1D2=(_tmp1D1.Char_tok).val;_LL125:
# 1112
 yyzzz=_tmp1D2;
goto _LL123;}else{_LL126: _LL127:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL123:;}
# 1116
return yyzzz;}
# 1118
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp1D3;(_tmp1D3.Char_tok).val=yy1;(_tmp1D3.Char_tok).tag=2;_tmp1D3;});}static char _tmp1D5[13]="string_t<`H>";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1D5,_tmp1D5,_tmp1D5 + 13}};
# 1109
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1D6=yy1;struct _dyneither_ptr _tmp1D7;if((_tmp1D6.String_tok).tag == 3){_LL129: _tmp1D7=(_tmp1D6.String_tok).val;_LL12A:
# 1113
 yyzzz=_tmp1D7;
goto _LL128;}else{_LL12B: _LL12C:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL128:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp1D8;(_tmp1D8.String_tok).val=yy1;(_tmp1D8.String_tok).tag=3;_tmp1D8;});}static char _tmp1DA[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 56}};
# 1112
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1DB=yy1;struct _tuple21*_tmp1DC;if((_tmp1DB.YY1).tag == 9){_LL12E: _tmp1DC=(_tmp1DB.YY1).val;_LL12F:
# 1116
 yyzzz=_tmp1DC;
goto _LL12D;}else{_LL130: _LL131:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL12D:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp1DD;(_tmp1DD.YY1).val=yy1;(_tmp1DD.YY1).tag=9;_tmp1DD;});}static char _tmp1DF[19]="conref_t<bounds_t>";
# 1111 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 19}};
# 1113
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1E0=yy1;union Cyc_Absyn_Constraint*_tmp1E1;if((_tmp1E0.YY2).tag == 10){_LL133: _tmp1E1=(_tmp1E0.YY2).val;_LL134:
# 1117
 yyzzz=_tmp1E1;
goto _LL132;}else{_LL135: _LL136:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL132:;}
# 1121
return yyzzz;}
# 1123
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp1E2;(_tmp1E2.YY2).val=yy1;(_tmp1E2.YY2).tag=10;_tmp1E2;});}static char _tmp1E4[28]="list_t<offsetof_field_t,`H>";
# 1112 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1E4,_tmp1E4,_tmp1E4 + 28}};
# 1114
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E5=yy1;struct Cyc_List_List*_tmp1E6;if((_tmp1E5.YY3).tag == 11){_LL138: _tmp1E6=(_tmp1E5.YY3).val;_LL139:
# 1118
 yyzzz=_tmp1E6;
goto _LL137;}else{_LL13A: _LL13B:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL137:;}
# 1122
return yyzzz;}
# 1124
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1E7;(_tmp1E7.YY3).val=yy1;(_tmp1E7.YY3).tag=11;_tmp1E7;});}static char _tmp1E9[6]="exp_t";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1E9,_tmp1E9,_tmp1E9 + 6}};
# 1115
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1EA=yy1;struct Cyc_Absyn_Exp*_tmp1EB;if((_tmp1EA.Exp_tok).tag == 7){_LL13D: _tmp1EB=(_tmp1EA.Exp_tok).val;_LL13E:
# 1119
 yyzzz=_tmp1EB;
goto _LL13C;}else{_LL13F: _LL140:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL13C:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp1EC;(_tmp1EC.Exp_tok).val=yy1;(_tmp1EC.Exp_tok).tag=7;_tmp1EC;});}static char _tmp1EE[17]="list_t<exp_t,`H>";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1EE,_tmp1EE,_tmp1EE + 17}};
# 1123
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EF=yy1;struct Cyc_List_List*_tmp1F0;if((_tmp1EF.YY4).tag == 12){_LL142: _tmp1F0=(_tmp1EF.YY4).val;_LL143:
# 1127
 yyzzz=_tmp1F0;
goto _LL141;}else{_LL144: _LL145:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL141:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1F1;(_tmp1F1.YY4).val=yy1;(_tmp1F1.YY4).tag=12;_tmp1F1;});}static char _tmp1F3[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 47}};
# 1124
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F4=yy1;struct Cyc_List_List*_tmp1F5;if((_tmp1F4.YY5).tag == 13){_LL147: _tmp1F5=(_tmp1F4.YY5).val;_LL148:
# 1128
 yyzzz=_tmp1F5;
goto _LL146;}else{_LL149: _LL14A:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL146:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1F6;(_tmp1F6.YY5).val=yy1;(_tmp1F6.YY5).tag=13;_tmp1F6;});}static char _tmp1F8[9]="primop_t";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 9}};
# 1125
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1F9=yy1;enum Cyc_Absyn_Primop _tmp1FA;if((_tmp1F9.YY6).tag == 14){_LL14C: _tmp1FA=(_tmp1F9.YY6).val;_LL14D:
# 1129
 yyzzz=_tmp1FA;
goto _LL14B;}else{_LL14E: _LL14F:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL14B:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp1FB;(_tmp1FB.YY6).val=yy1;(_tmp1FB.YY6).tag=14;_tmp1FB;});}static char _tmp1FD[19]="opt_t<primop_t,`H>";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1FD,_tmp1FD,_tmp1FD + 19}};
# 1126
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1FE=yy1;struct Cyc_Core_Opt*_tmp1FF;if((_tmp1FE.YY7).tag == 15){_LL151: _tmp1FF=(_tmp1FE.YY7).val;_LL152:
# 1130
 yyzzz=_tmp1FF;
goto _LL150;}else{_LL153: _LL154:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL150:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp200;(_tmp200.YY7).val=yy1;(_tmp200.YY7).tag=15;_tmp200;});}static char _tmp202[7]="qvar_t";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 7}};
# 1127
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp203=yy1;struct _tuple0*_tmp204;if((_tmp203.QualId_tok).tag == 5){_LL156: _tmp204=(_tmp203.QualId_tok).val;_LL157:
# 1131
 yyzzz=_tmp204;
goto _LL155;}else{_LL158: _LL159:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL155:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp205;(_tmp205.QualId_tok).val=yy1;(_tmp205.QualId_tok).tag=5;_tmp205;});}static char _tmp207[7]="stmt_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 7}};
# 1130
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp208=yy1;struct Cyc_Absyn_Stmt*_tmp209;if((_tmp208.Stmt_tok).tag == 8){_LL15B: _tmp209=(_tmp208.Stmt_tok).val;_LL15C:
# 1134
 yyzzz=_tmp209;
goto _LL15A;}else{_LL15D: _LL15E:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL15A:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp20A;(_tmp20A.Stmt_tok).val=yy1;(_tmp20A.Stmt_tok).tag=8;_tmp20A;});}static char _tmp20C[27]="list_t<switch_clause_t,`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp20C,_tmp20C,_tmp20C + 27}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp20D=yy1;struct Cyc_List_List*_tmp20E;if((_tmp20D.YY8).tag == 16){_LL160: _tmp20E=(_tmp20D.YY8).val;_LL161:
# 1138
 yyzzz=_tmp20E;
goto _LL15F;}else{_LL162: _LL163:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL15F:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp20F;(_tmp20F.YY8).val=yy1;(_tmp20F.YY8).tag=16;_tmp20F;});}static char _tmp211[6]="pat_t";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp211,_tmp211,_tmp211 + 6}};
# 1135
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp212=yy1;struct Cyc_Absyn_Pat*_tmp213;if((_tmp212.YY9).tag == 17){_LL165: _tmp213=(_tmp212.YY9).val;_LL166:
# 1139
 yyzzz=_tmp213;
goto _LL164;}else{_LL167: _LL168:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL164:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp214;(_tmp214.YY9).val=yy1;(_tmp214.YY9).tag=17;_tmp214;});}static char _tmp216[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 28}};
# 1140
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp217=yy1;struct _tuple22*_tmp218;if((_tmp217.YY10).tag == 18){_LL16A: _tmp218=(_tmp217.YY10).val;_LL16B:
# 1144
 yyzzz=_tmp218;
goto _LL169;}else{_LL16C: _LL16D:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL169:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp219;(_tmp219.YY10).val=yy1;(_tmp219.YY10).tag=18;_tmp219;});}static char _tmp21B[17]="list_t<pat_t,`H>";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 17}};
# 1141
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21C=yy1;struct Cyc_List_List*_tmp21D;if((_tmp21C.YY11).tag == 19){_LL16F: _tmp21D=(_tmp21C.YY11).val;_LL170:
# 1145
 yyzzz=_tmp21D;
goto _LL16E;}else{_LL171: _LL172:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL16E:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp21E;(_tmp21E.YY11).val=yy1;(_tmp21E.YY11).tag=19;_tmp21E;});}static char _tmp220[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp220,_tmp220,_tmp220 + 36}};
# 1142
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp221=yy1;struct _tuple23*_tmp222;if((_tmp221.YY12).tag == 20){_LL174: _tmp222=(_tmp221.YY12).val;_LL175:
# 1146
 yyzzz=_tmp222;
goto _LL173;}else{_LL176: _LL177:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL173:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp223;(_tmp223.YY12).val=yy1;(_tmp223.YY12).tag=20;_tmp223;});}static char _tmp225[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp225,_tmp225,_tmp225 + 47}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp226=yy1;struct Cyc_List_List*_tmp227;if((_tmp226.YY13).tag == 21){_LL179: _tmp227=(_tmp226.YY13).val;_LL17A:
# 1147
 yyzzz=_tmp227;
goto _LL178;}else{_LL17B: _LL17C:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL178:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp228;(_tmp228.YY13).val=yy1;(_tmp228.YY13).tag=21;_tmp228;});}static char _tmp22A[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp22A,_tmp22A,_tmp22A + 58}};
# 1144
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp22B=yy1;struct _tuple22*_tmp22C;if((_tmp22B.YY14).tag == 22){_LL17E: _tmp22C=(_tmp22B.YY14).val;_LL17F:
# 1148
 yyzzz=_tmp22C;
goto _LL17D;}else{_LL180: _LL181:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL17D:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp22D;(_tmp22D.YY14).val=yy1;(_tmp22D.YY14).tag=22;_tmp22D;});}static char _tmp22F[9]="fndecl_t";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 9}};
# 1145
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp230=yy1;struct Cyc_Absyn_Fndecl*_tmp231;if((_tmp230.YY15).tag == 23){_LL183: _tmp231=(_tmp230.YY15).val;_LL184:
# 1149
 yyzzz=_tmp231;
goto _LL182;}else{_LL185: _LL186:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL182:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp232;(_tmp232.YY15).val=yy1;(_tmp232.YY15).tag=23;_tmp232;});}static char _tmp234[18]="list_t<decl_t,`H>";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 18}};
# 1146
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp235=yy1;struct Cyc_List_List*_tmp236;if((_tmp235.YY16).tag == 24){_LL188: _tmp236=(_tmp235.YY16).val;_LL189:
# 1150
 yyzzz=_tmp236;
goto _LL187;}else{_LL18A: _LL18B:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL187:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp237;(_tmp237.YY16).val=yy1;(_tmp237.YY16).tag=24;_tmp237;});}static char _tmp239[12]="decl_spec_t";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp239,_tmp239,_tmp239 + 12}};
# 1148
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp23A=yy1;struct Cyc_Parse_Declaration_spec _tmp23B;if((_tmp23A.YY17).tag == 25){_LL18D: _tmp23B=(_tmp23A.YY17).val;_LL18E:
# 1152
 yyzzz=_tmp23B;
goto _LL18C;}else{_LL18F: _LL190:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL18C:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp23C;(_tmp23C.YY17).val=yy1;(_tmp23C.YY17).tag=25;_tmp23C;});}static char _tmp23E[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp23E,_tmp23E,_tmp23E + 31}};
# 1149
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp23F=yy1;struct _tuple11 _tmp240;if((_tmp23F.YY18).tag == 26){_LL192: _tmp240=(_tmp23F.YY18).val;_LL193:
# 1153
 yyzzz=_tmp240;
goto _LL191;}else{_LL194: _LL195:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL191:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp241;(_tmp241.YY18).val=yy1;(_tmp241.YY18).tag=26;_tmp241;});}static char _tmp243[23]="declarator_list_t<`yy>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 23}};
# 1150
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp244=yy1;struct _tuple12*_tmp245;if((_tmp244.YY19).tag == 27){_LL197: _tmp245=(_tmp244.YY19).val;_LL198:
# 1154
 yyzzz=_tmp245;
goto _LL196;}else{_LL199: _LL19A:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL196:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp246;(_tmp246.YY19).val=yy1;(_tmp246.YY19).tag=27;_tmp246;});}static char _tmp248[19]="storage_class_t@`H";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 19}};
# 1151
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp249=yy1;enum Cyc_Parse_Storage_class*_tmp24A;if((_tmp249.YY20).tag == 28){_LL19C: _tmp24A=(_tmp249.YY20).val;_LL19D:
# 1155
 yyzzz=_tmp24A;
goto _LL19B;}else{_LL19E: _LL19F:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL19B:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp24B;(_tmp24B.YY20).val=yy1;(_tmp24B.YY20).tag=28;_tmp24B;});}static char _tmp24D[17]="type_specifier_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 17}};
# 1152
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp24E=yy1;struct Cyc_Parse_Type_specifier _tmp24F;if((_tmp24E.YY21).tag == 29){_LL1A1: _tmp24F=(_tmp24E.YY21).val;_LL1A2:
# 1156
 yyzzz=_tmp24F;
goto _LL1A0;}else{_LL1A3: _LL1A4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL1A0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp250;(_tmp250.YY21).val=yy1;(_tmp250.YY21).tag=29;_tmp250;});}static char _tmp252[12]="aggr_kind_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 12}};
# 1154
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp253=yy1;enum Cyc_Absyn_AggrKind _tmp254;if((_tmp253.YY22).tag == 30){_LL1A6: _tmp254=(_tmp253.YY22).val;_LL1A7:
# 1158
 yyzzz=_tmp254;
goto _LL1A5;}else{_LL1A8: _LL1A9:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL1A5:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp255;(_tmp255.YY22).val=yy1;(_tmp255.YY22).tag=30;_tmp255;});}static char _tmp257[8]="tqual_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 8}};
# 1155
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp258=yy1;struct Cyc_Absyn_Tqual _tmp259;if((_tmp258.YY23).tag == 31){_LL1AB: _tmp259=(_tmp258.YY23).val;_LL1AC:
# 1159
 yyzzz=_tmp259;
goto _LL1AA;}else{_LL1AD: _LL1AE:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL1AA:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp25A;(_tmp25A.YY23).val=yy1;(_tmp25A.YY23).tag=31;_tmp25A;});}static char _tmp25C[23]="list_t<aggrfield_t,`H>";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 23}};
# 1156
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25D=yy1;struct Cyc_List_List*_tmp25E;if((_tmp25D.YY24).tag == 32){_LL1B0: _tmp25E=(_tmp25D.YY24).val;_LL1B1:
# 1160
 yyzzz=_tmp25E;
goto _LL1AF;}else{_LL1B2: _LL1B3:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL1AF:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp25F;(_tmp25F.YY24).val=yy1;(_tmp25F.YY24).tag=32;_tmp25F;});}static char _tmp261[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp261,_tmp261,_tmp261 + 34}};
# 1157
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*_tmp263;if((_tmp262.YY25).tag == 33){_LL1B5: _tmp263=(_tmp262.YY25).val;_LL1B6:
# 1161
 yyzzz=_tmp263;
goto _LL1B4;}else{_LL1B7: _LL1B8:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL1B4:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp264;(_tmp264.YY25).val=yy1;(_tmp264.YY25).tag=33;_tmp264;});}static char _tmp266[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp266,_tmp266,_tmp266 + 33}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*_tmp268;if((_tmp267.YY26).tag == 34){_LL1BA: _tmp268=(_tmp267.YY26).val;_LL1BB:
# 1162
 yyzzz=_tmp268;
goto _LL1B9;}else{_LL1BC: _LL1BD:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL1B9:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp269;(_tmp269.YY26).val=yy1;(_tmp269.YY26).tag=34;_tmp269;});}static char _tmp26B[18]="declarator_t<`yy>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp26B,_tmp26B,_tmp26B + 18}};
# 1159
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp26C=yy1;struct Cyc_Parse_Declarator _tmp26D;if((_tmp26C.YY27).tag == 35){_LL1BF: _tmp26D=(_tmp26C.YY27).val;_LL1C0:
# 1163
 yyzzz=_tmp26D;
goto _LL1BE;}else{_LL1C1: _LL1C2:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL1BE:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp26E;(_tmp26E.YY27).val=yy1;(_tmp26E.YY27).tag=35;_tmp26E;});}static char _tmp270[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 45}};
# 1160
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp271=yy1;struct _tuple24*_tmp272;if((_tmp271.YY28).tag == 36){_LL1C4: _tmp272=(_tmp271.YY28).val;_LL1C5:
# 1164
 yyzzz=_tmp272;
goto _LL1C3;}else{_LL1C6: _LL1C7:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL1C3:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp273;(_tmp273.YY28).val=yy1;(_tmp273.YY28).tag=36;_tmp273;});}static char _tmp275[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 57}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp276=yy1;struct Cyc_List_List*_tmp277;if((_tmp276.YY29).tag == 37){_LL1C9: _tmp277=(_tmp276.YY29).val;_LL1CA:
# 1165
 yyzzz=_tmp277;
goto _LL1C8;}else{_LL1CB: _LL1CC:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL1C8:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp278;(_tmp278.YY29).val=yy1;(_tmp278.YY29).tag=37;_tmp278;});}static char _tmp27A[26]="abstractdeclarator_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp27A,_tmp27A,_tmp27A + 26}};
# 1162
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Parse_Abstractdeclarator _tmp27C;if((_tmp27B.YY30).tag == 38){_LL1CE: _tmp27C=(_tmp27B.YY30).val;_LL1CF:
# 1166
 yyzzz=_tmp27C;
goto _LL1CD;}else{_LL1D0: _LL1D1:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL1CD:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp27D;(_tmp27D.YY30).val=yy1;(_tmp27D.YY30).tag=38;_tmp27D;});}static char _tmp27F[5]="bool";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 5}};
# 1163
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp280=yy1;int _tmp281;if((_tmp280.YY31).tag == 39){_LL1D3: _tmp281=(_tmp280.YY31).val;_LL1D4:
# 1167
 yyzzz=_tmp281;
goto _LL1D2;}else{_LL1D5: _LL1D6:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL1D2:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp282;(_tmp282.YY31).val=yy1;(_tmp282.YY31).tag=39;_tmp282;});}static char _tmp284[8]="scope_t";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp284,_tmp284,_tmp284 + 8}};
# 1164
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp285=yy1;enum Cyc_Absyn_Scope _tmp286;if((_tmp285.YY32).tag == 40){_LL1D8: _tmp286=(_tmp285.YY32).val;_LL1D9:
# 1168
 yyzzz=_tmp286;
goto _LL1D7;}else{_LL1DA: _LL1DB:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL1D7:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp287;(_tmp287.YY32).val=yy1;(_tmp287.YY32).tag=40;_tmp287;});}static char _tmp289[16]="datatypefield_t";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp289,_tmp289,_tmp289 + 16}};
# 1165
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp28A=yy1;struct Cyc_Absyn_Datatypefield*_tmp28B;if((_tmp28A.YY33).tag == 41){_LL1DD: _tmp28B=(_tmp28A.YY33).val;_LL1DE:
# 1169
 yyzzz=_tmp28B;
goto _LL1DC;}else{_LL1DF: _LL1E0:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL1DC:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp28C;(_tmp28C.YY33).val=yy1;(_tmp28C.YY33).tag=41;_tmp28C;});}static char _tmp28E[27]="list_t<datatypefield_t,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 27}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp28F=yy1;struct Cyc_List_List*_tmp290;if((_tmp28F.YY34).tag == 42){_LL1E2: _tmp290=(_tmp28F.YY34).val;_LL1E3:
# 1170
 yyzzz=_tmp290;
goto _LL1E1;}else{_LL1E4: _LL1E5:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL1E1:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp291;(_tmp291.YY34).val=yy1;(_tmp291.YY34).tag=42;_tmp291;});}static char _tmp293[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 41}};
# 1167
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple25 _tmp295;if((_tmp294.YY35).tag == 43){_LL1E7: _tmp295=(_tmp294.YY35).val;_LL1E8:
# 1171
 yyzzz=_tmp295;
goto _LL1E6;}else{_LL1E9: _LL1EA:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL1E6:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp296;(_tmp296.YY35).val=yy1;(_tmp296.YY35).tag=43;_tmp296;});}static char _tmp298[17]="list_t<var_t,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp298,_tmp298,_tmp298 + 17}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;if((_tmp299.YY36).tag == 44){_LL1EC: _tmp29A=(_tmp299.YY36).val;_LL1ED:
# 1172
 yyzzz=_tmp29A;
goto _LL1EB;}else{_LL1EE: _LL1EF:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL1EB:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp29B;(_tmp29B.YY36).val=yy1;(_tmp29B.YY36).tag=44;_tmp29B;});}static char _tmp29D[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp29D,_tmp29D,_tmp29D + 31}};
# 1169
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp29E=yy1;struct _tuple8*_tmp29F;if((_tmp29E.YY37).tag == 45){_LL1F1: _tmp29F=(_tmp29E.YY37).val;_LL1F2:
# 1173
 yyzzz=_tmp29F;
goto _LL1F0;}else{_LL1F3: _LL1F4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL1F0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp2A0;(_tmp2A0.YY37).val=yy1;(_tmp2A0.YY37).tag=45;_tmp2A0;});}static char _tmp2A2[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 42}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A3=yy1;struct Cyc_List_List*_tmp2A4;if((_tmp2A3.YY38).tag == 46){_LL1F6: _tmp2A4=(_tmp2A3.YY38).val;_LL1F7:
# 1174
 yyzzz=_tmp2A4;
goto _LL1F5;}else{_LL1F8: _LL1F9:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL1F5:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2A5;(_tmp2A5.YY38).val=yy1;(_tmp2A5.YY38).tag=46;_tmp2A5;});}static char _tmp2A7[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 115}};
# 1171
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2A8=yy1;struct _tuple26*_tmp2A9;if((_tmp2A8.YY39).tag == 47){_LL1FB: _tmp2A9=(_tmp2A8.YY39).val;_LL1FC:
# 1175
 yyzzz=_tmp2A9;
goto _LL1FA;}else{_LL1FD: _LL1FE:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL1FA:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp2AA;(_tmp2AA.YY39).val=yy1;(_tmp2AA.YY39).tag=47;_tmp2AA;});}static char _tmp2AC[18]="list_t<type_t,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,_tmp2AC + 18}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AD=yy1;struct Cyc_List_List*_tmp2AE;if((_tmp2AD.YY40).tag == 48){_LL200: _tmp2AE=(_tmp2AD.YY40).val;_LL201:
# 1176
 yyzzz=_tmp2AE;
goto _LL1FF;}else{_LL202: _LL203:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL1FF:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2AF;(_tmp2AF.YY40).val=yy1;(_tmp2AF.YY40).tag=48;_tmp2AF;});}static char _tmp2B1[24]="list_t<designator_t,`H>";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 24}};
# 1174
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B2=yy1;struct Cyc_List_List*_tmp2B3;if((_tmp2B2.YY41).tag == 49){_LL205: _tmp2B3=(_tmp2B2.YY41).val;_LL206:
# 1178
 yyzzz=_tmp2B3;
goto _LL204;}else{_LL207: _LL208:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL204:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2B4;(_tmp2B4.YY41).val=yy1;(_tmp2B4.YY41).tag=49;_tmp2B4;});}static char _tmp2B6[13]="designator_t";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B6,_tmp2B6,_tmp2B6 + 13}};
# 1175
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2B7=yy1;void*_tmp2B8;if((_tmp2B7.YY42).tag == 50){_LL20A: _tmp2B8=(_tmp2B7.YY42).val;_LL20B:
# 1179
 yyzzz=_tmp2B8;
goto _LL209;}else{_LL20C: _LL20D:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL209:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp2B9;(_tmp2B9.YY42).val=yy1;(_tmp2B9.YY42).tag=50;_tmp2B9;});}static char _tmp2BB[7]="kind_t";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 7}};
# 1176
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2BC=yy1;struct Cyc_Absyn_Kind*_tmp2BD;if((_tmp2BC.YY43).tag == 51){_LL20F: _tmp2BD=(_tmp2BC.YY43).val;_LL210:
# 1180
 yyzzz=_tmp2BD;
goto _LL20E;}else{_LL211: _LL212:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL20E:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp2BE;(_tmp2BE.YY43).val=yy1;(_tmp2BE.YY43).tag=51;_tmp2BE;});}static char _tmp2C0[7]="type_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 7}};
# 1177
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C1=yy1;void*_tmp2C2;if((_tmp2C1.YY44).tag == 52){_LL214: _tmp2C2=(_tmp2C1.YY44).val;_LL215:
# 1181
 yyzzz=_tmp2C2;
goto _LL213;}else{_LL216: _LL217:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL213:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp2C3;(_tmp2C3.YY44).val=yy1;(_tmp2C3.YY44).tag=52;_tmp2C3;});}static char _tmp2C5[23]="list_t<attribute_t,`H>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 23}};
# 1178
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C6=yy1;struct Cyc_List_List*_tmp2C7;if((_tmp2C6.YY45).tag == 53){_LL219: _tmp2C7=(_tmp2C6.YY45).val;_LL21A:
# 1182
 yyzzz=_tmp2C7;
goto _LL218;}else{_LL21B: _LL21C:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL218:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2C8;(_tmp2C8.YY45).val=yy1;(_tmp2C8.YY45).tag=53;_tmp2C8;});}static char _tmp2CA[12]="attribute_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2CA,_tmp2CA,_tmp2CA + 12}};
# 1179
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2CB=yy1;void*_tmp2CC;if((_tmp2CB.YY46).tag == 54){_LL21E: _tmp2CC=(_tmp2CB.YY46).val;_LL21F:
# 1183
 yyzzz=_tmp2CC;
goto _LL21D;}else{_LL220: _LL221:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL21D:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp2CD;(_tmp2CD.YY46).val=yy1;(_tmp2CD.YY46).tag=54;_tmp2CD;});}static char _tmp2CF[12]="enumfield_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 12}};
# 1180
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_Absyn_Enumfield*_tmp2D1;if((_tmp2D0.YY47).tag == 55){_LL223: _tmp2D1=(_tmp2D0.YY47).val;_LL224:
# 1184
 yyzzz=_tmp2D1;
goto _LL222;}else{_LL225: _LL226:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL222:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp2D2;(_tmp2D2.YY47).val=yy1;(_tmp2D2.YY47).tag=55;_tmp2D2;});}static char _tmp2D4[23]="list_t<enumfield_t,`H>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 23}};
# 1181
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;if((_tmp2D5.YY48).tag == 56){_LL228: _tmp2D6=(_tmp2D5.YY48).val;_LL229:
# 1185
 yyzzz=_tmp2D6;
goto _LL227;}else{_LL22A: _LL22B:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL227:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2D7;(_tmp2D7.YY48).val=yy1;(_tmp2D7.YY48).tag=56;_tmp2D7;});}static char _tmp2D9[11]="type_opt_t";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2D9,_tmp2D9,_tmp2D9 + 11}};
# 1182
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2DA=yy1;void*_tmp2DB;if((_tmp2DA.YY49).tag == 57){_LL22D: _tmp2DB=(_tmp2DA.YY49).val;_LL22E:
# 1186
 yyzzz=_tmp2DB;
goto _LL22C;}else{_LL22F: _LL230:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL22C:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp2DC;(_tmp2DC.YY49).val=yy1;(_tmp2DC.YY49).tag=57;_tmp2DC;});}static char _tmp2DE[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 31}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_List_List*_tmp2E0;if((_tmp2DF.YY50).tag == 58){_LL232: _tmp2E0=(_tmp2DF.YY50).val;_LL233:
# 1187
 yyzzz=_tmp2E0;
goto _LL231;}else{_LL234: _LL235:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL231:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2E1;(_tmp2E1.YY50).val=yy1;(_tmp2E1.YY50).tag=58;_tmp2E1;});}static char _tmp2E3[15]="conref_t<bool>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 15}};
# 1184
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2E4=yy1;union Cyc_Absyn_Constraint*_tmp2E5;if((_tmp2E4.YY51).tag == 59){_LL237: _tmp2E5=(_tmp2E4.YY51).val;_LL238:
# 1188
 yyzzz=_tmp2E5;
goto _LL236;}else{_LL239: _LL23A:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL236:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp2E6;(_tmp2E6.YY51).val=yy1;(_tmp2E6.YY51).tag=59;_tmp2E6;});}static char _tmp2E8[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,_tmp2E8 + 45}};
# 1185
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E9=yy1;struct Cyc_List_List*_tmp2EA;if((_tmp2E9.YY52).tag == 60){_LL23C: _tmp2EA=(_tmp2E9.YY52).val;_LL23D:
# 1189
 yyzzz=_tmp2EA;
goto _LL23B;}else{_LL23E: _LL23F:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL23B:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2EB;(_tmp2EB.YY52).val=yy1;(_tmp2EB.YY52).tag=60;_tmp2EB;});}static char _tmp2ED[20]="pointer_qual_t<`yy>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 20}};
# 1186
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2EE=yy1;void*_tmp2EF;if((_tmp2EE.YY53).tag == 61){_LL241: _tmp2EF=(_tmp2EE.YY53).val;_LL242:
# 1190
 yyzzz=_tmp2EF;
goto _LL240;}else{_LL243: _LL244:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL240:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp2F0;(_tmp2F0.YY53).val=yy1;(_tmp2F0.YY53).tag=61;_tmp2F0;});}static char _tmp2F2[21]="pointer_quals_t<`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 21}};
# 1187
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F3=yy1;struct Cyc_List_List*_tmp2F4;if((_tmp2F3.YY54).tag == 62){_LL246: _tmp2F4=(_tmp2F3.YY54).val;_LL247:
# 1191
 yyzzz=_tmp2F4;
goto _LL245;}else{_LL248: _LL249:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL245:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2F5;(_tmp2F5.YY54).val=yy1;(_tmp2F5.YY54).tag=62;_tmp2F5;});}static char _tmp2F7[21]="$(bool,string_t<`H>)";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 21}};
# 1188
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp2F8=yy1;struct _tuple20 _tmp2F9;if((_tmp2F8.Asm_tok).tag == 6){_LL24B: _tmp2F9=(_tmp2F8.Asm_tok).val;_LL24C:
# 1192
 yyzzz=_tmp2F9;
goto _LL24A;}else{_LL24D: _LL24E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL24A:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp2FA;(_tmp2FA.Asm_tok).val=yy1;(_tmp2FA.Asm_tok).tag=6;_tmp2FA;});}static char _tmp2FC[10]="exp_opt_t";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,_tmp2FC + 10}};
# 1189
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp2FD=yy1;struct Cyc_Absyn_Exp*_tmp2FE;if((_tmp2FD.YY55).tag == 63){_LL250: _tmp2FE=(_tmp2FD.YY55).val;_LL251:
# 1193
 yyzzz=_tmp2FE;
goto _LL24F;}else{_LL252: _LL253:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL24F:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp2FF;(_tmp2FF.YY55).val=yy1;(_tmp2FF.YY55).tag=63;_tmp2FF;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1211
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1215
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp300;_tmp300.timestamp=0;_tmp300.first_line=0;_tmp300.first_column=0;_tmp300.last_line=0;_tmp300.last_column=0;_tmp300;});}
# 1218
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1229 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp301[2]="$";static char _tmp302[6]="error";static char _tmp303[12]="$undefined.";static char _tmp304[5]="AUTO";static char _tmp305[9]="REGISTER";static char _tmp306[7]="STATIC";static char _tmp307[7]="EXTERN";static char _tmp308[8]="TYPEDEF";static char _tmp309[5]="VOID";static char _tmp30A[5]="CHAR";static char _tmp30B[6]="SHORT";static char _tmp30C[4]="INT";static char _tmp30D[5]="LONG";static char _tmp30E[6]="FLOAT";static char _tmp30F[7]="DOUBLE";static char _tmp310[7]="SIGNED";static char _tmp311[9]="UNSIGNED";static char _tmp312[6]="CONST";static char _tmp313[9]="VOLATILE";static char _tmp314[9]="RESTRICT";static char _tmp315[7]="STRUCT";static char _tmp316[6]="UNION";static char _tmp317[5]="CASE";static char _tmp318[8]="DEFAULT";static char _tmp319[7]="INLINE";static char _tmp31A[7]="SIZEOF";static char _tmp31B[9]="OFFSETOF";static char _tmp31C[3]="IF";static char _tmp31D[5]="ELSE";static char _tmp31E[7]="SWITCH";static char _tmp31F[6]="WHILE";static char _tmp320[3]="DO";static char _tmp321[4]="FOR";static char _tmp322[5]="GOTO";static char _tmp323[9]="CONTINUE";static char _tmp324[6]="BREAK";static char _tmp325[7]="RETURN";static char _tmp326[5]="ENUM";static char _tmp327[7]="TYPEOF";static char _tmp328[16]="BUILTIN_VA_LIST";static char _tmp329[8]="NULL_kw";static char _tmp32A[4]="LET";static char _tmp32B[6]="THROW";static char _tmp32C[4]="TRY";static char _tmp32D[6]="CATCH";static char _tmp32E[7]="EXPORT";static char _tmp32F[4]="NEW";static char _tmp330[9]="ABSTRACT";static char _tmp331[9]="FALLTHRU";static char _tmp332[6]="USING";static char _tmp333[10]="NAMESPACE";static char _tmp334[9]="DATATYPE";static char _tmp335[7]="MALLOC";static char _tmp336[8]="RMALLOC";static char _tmp337[15]="RMALLOC_INLINE";static char _tmp338[7]="CALLOC";static char _tmp339[8]="RCALLOC";static char _tmp33A[5]="SWAP";static char _tmp33B[9]="REGION_T";static char _tmp33C[6]="TAG_T";static char _tmp33D[7]="REGION";static char _tmp33E[5]="RNEW";static char _tmp33F[8]="REGIONS";static char _tmp340[7]="PORTON";static char _tmp341[8]="PORTOFF";static char _tmp342[12]="DYNREGION_T";static char _tmp343[8]="NUMELTS";static char _tmp344[8]="VALUEOF";static char _tmp345[10]="VALUEOF_T";static char _tmp346[9]="TAGCHECK";static char _tmp347[13]="NUMELTS_QUAL";static char _tmp348[10]="THIN_QUAL";static char _tmp349[9]="FAT_QUAL";static char _tmp34A[13]="NOTNULL_QUAL";static char _tmp34B[14]="NULLABLE_QUAL";static char _tmp34C[14]="REQUIRES_QUAL";static char _tmp34D[13]="ENSURES_QUAL";static char _tmp34E[12]="REGION_QUAL";static char _tmp34F[16]="NOZEROTERM_QUAL";static char _tmp350[14]="ZEROTERM_QUAL";static char _tmp351[12]="TAGGED_QUAL";static char _tmp352[16]="EXTENSIBLE_QUAL";static char _tmp353[7]="PTR_OP";static char _tmp354[7]="INC_OP";static char _tmp355[7]="DEC_OP";static char _tmp356[8]="LEFT_OP";static char _tmp357[9]="RIGHT_OP";static char _tmp358[6]="LE_OP";static char _tmp359[6]="GE_OP";static char _tmp35A[6]="EQ_OP";static char _tmp35B[6]="NE_OP";static char _tmp35C[7]="AND_OP";static char _tmp35D[6]="OR_OP";static char _tmp35E[11]="MUL_ASSIGN";static char _tmp35F[11]="DIV_ASSIGN";static char _tmp360[11]="MOD_ASSIGN";static char _tmp361[11]="ADD_ASSIGN";static char _tmp362[11]="SUB_ASSIGN";static char _tmp363[12]="LEFT_ASSIGN";static char _tmp364[13]="RIGHT_ASSIGN";static char _tmp365[11]="AND_ASSIGN";static char _tmp366[11]="XOR_ASSIGN";static char _tmp367[10]="OR_ASSIGN";static char _tmp368[9]="ELLIPSIS";static char _tmp369[11]="LEFT_RIGHT";static char _tmp36A[12]="COLON_COLON";static char _tmp36B[11]="IDENTIFIER";static char _tmp36C[17]="INTEGER_CONSTANT";static char _tmp36D[7]="STRING";static char _tmp36E[8]="WSTRING";static char _tmp36F[19]="CHARACTER_CONSTANT";static char _tmp370[20]="WCHARACTER_CONSTANT";static char _tmp371[18]="FLOATING_CONSTANT";static char _tmp372[9]="TYPE_VAR";static char _tmp373[13]="TYPEDEF_NAME";static char _tmp374[16]="QUAL_IDENTIFIER";static char _tmp375[18]="QUAL_TYPEDEF_NAME";static char _tmp376[10]="ATTRIBUTE";static char _tmp377[4]="ASM";static char _tmp378[4]="';'";static char _tmp379[4]="'{'";static char _tmp37A[4]="'}'";static char _tmp37B[4]="','";static char _tmp37C[4]="'='";static char _tmp37D[4]="'<'";static char _tmp37E[4]="'>'";static char _tmp37F[4]="'('";static char _tmp380[4]="')'";static char _tmp381[4]="'_'";static char _tmp382[4]="'$'";static char _tmp383[4]="':'";static char _tmp384[4]="'.'";static char _tmp385[4]="'['";static char _tmp386[4]="']'";static char _tmp387[4]="'*'";static char _tmp388[4]="'@'";static char _tmp389[4]="'?'";static char _tmp38A[4]="'+'";static char _tmp38B[4]="'|'";static char _tmp38C[4]="'^'";static char _tmp38D[4]="'&'";static char _tmp38E[4]="'-'";static char _tmp38F[4]="'/'";static char _tmp390[4]="'%'";static char _tmp391[4]="'~'";static char _tmp392[4]="'!'";static char _tmp393[5]="prog";static char _tmp394[17]="translation_unit";static char _tmp395[16]="extern_c_action";static char _tmp396[13]="end_extern_c";static char _tmp397[12]="export_list";static char _tmp398[19]="export_list_values";static char _tmp399[21]="external_declaration";static char _tmp39A[15]="optional_comma";static char _tmp39B[20]="function_definition";static char _tmp39C[21]="function_definition2";static char _tmp39D[13]="using_action";static char _tmp39E[15]="unusing_action";static char _tmp39F[17]="namespace_action";static char _tmp3A0[19]="unnamespace_action";static char _tmp3A1[12]="declaration";static char _tmp3A2[17]="declaration_list";static char _tmp3A3[23]="declaration_specifiers";static char _tmp3A4[24]="storage_class_specifier";static char _tmp3A5[15]="attributes_opt";static char _tmp3A6[11]="attributes";static char _tmp3A7[15]="attribute_list";static char _tmp3A8[10]="attribute";static char _tmp3A9[15]="type_specifier";static char _tmp3AA[25]="type_specifier_notypedef";static char _tmp3AB[5]="kind";static char _tmp3AC[15]="type_qualifier";static char _tmp3AD[15]="enum_specifier";static char _tmp3AE[11]="enum_field";static char _tmp3AF[22]="enum_declaration_list";static char _tmp3B0[26]="struct_or_union_specifier";static char _tmp3B1[16]="type_params_opt";static char _tmp3B2[16]="struct_or_union";static char _tmp3B3[24]="struct_declaration_list";static char _tmp3B4[25]="struct_declaration_list0";static char _tmp3B5[21]="init_declarator_list";static char _tmp3B6[22]="init_declarator_list0";static char _tmp3B7[16]="init_declarator";static char _tmp3B8[19]="struct_declaration";static char _tmp3B9[25]="specifier_qualifier_list";static char _tmp3BA[35]="notypedef_specifier_qualifier_list";static char _tmp3BB[23]="struct_declarator_list";static char _tmp3BC[24]="struct_declarator_list0";static char _tmp3BD[18]="struct_declarator";static char _tmp3BE[20]="requires_clause_opt";static char _tmp3BF[19]="ensures_clause_opt";static char _tmp3C0[19]="datatype_specifier";static char _tmp3C1[14]="qual_datatype";static char _tmp3C2[19]="datatypefield_list";static char _tmp3C3[20]="datatypefield_scope";static char _tmp3C4[14]="datatypefield";static char _tmp3C5[11]="declarator";static char _tmp3C6[23]="declarator_withtypedef";static char _tmp3C7[18]="direct_declarator";static char _tmp3C8[30]="direct_declarator_withtypedef";static char _tmp3C9[8]="pointer";static char _tmp3CA[12]="one_pointer";static char _tmp3CB[14]="pointer_quals";static char _tmp3CC[13]="pointer_qual";static char _tmp3CD[23]="pointer_null_and_bound";static char _tmp3CE[14]="pointer_bound";static char _tmp3CF[18]="zeroterm_qual_opt";static char _tmp3D0[8]="rgn_opt";static char _tmp3D1[11]="tqual_list";static char _tmp3D2[20]="parameter_type_list";static char _tmp3D3[9]="type_var";static char _tmp3D4[16]="optional_effect";static char _tmp3D5[19]="optional_rgn_order";static char _tmp3D6[10]="rgn_order";static char _tmp3D7[16]="optional_inject";static char _tmp3D8[11]="effect_set";static char _tmp3D9[14]="atomic_effect";static char _tmp3DA[11]="region_set";static char _tmp3DB[15]="parameter_list";static char _tmp3DC[22]="parameter_declaration";static char _tmp3DD[16]="identifier_list";static char _tmp3DE[17]="identifier_list0";static char _tmp3DF[12]="initializer";static char _tmp3E0[18]="array_initializer";static char _tmp3E1[17]="initializer_list";static char _tmp3E2[12]="designation";static char _tmp3E3[16]="designator_list";static char _tmp3E4[11]="designator";static char _tmp3E5[10]="type_name";static char _tmp3E6[14]="any_type_name";static char _tmp3E7[15]="type_name_list";static char _tmp3E8[20]="abstract_declarator";static char _tmp3E9[27]="direct_abstract_declarator";static char _tmp3EA[10]="statement";static char _tmp3EB[18]="labeled_statement";static char _tmp3EC[21]="expression_statement";static char _tmp3ED[19]="compound_statement";static char _tmp3EE[16]="block_item_list";static char _tmp3EF[20]="selection_statement";static char _tmp3F0[15]="switch_clauses";static char _tmp3F1[20]="iteration_statement";static char _tmp3F2[15]="jump_statement";static char _tmp3F3[12]="exp_pattern";static char _tmp3F4[20]="conditional_pattern";static char _tmp3F5[19]="logical_or_pattern";static char _tmp3F6[20]="logical_and_pattern";static char _tmp3F7[21]="inclusive_or_pattern";static char _tmp3F8[21]="exclusive_or_pattern";static char _tmp3F9[12]="and_pattern";static char _tmp3FA[17]="equality_pattern";static char _tmp3FB[19]="relational_pattern";static char _tmp3FC[14]="shift_pattern";static char _tmp3FD[17]="additive_pattern";static char _tmp3FE[23]="multiplicative_pattern";static char _tmp3FF[13]="cast_pattern";static char _tmp400[14]="unary_pattern";static char _tmp401[16]="postfix_pattern";static char _tmp402[16]="primary_pattern";static char _tmp403[8]="pattern";static char _tmp404[19]="tuple_pattern_list";static char _tmp405[20]="tuple_pattern_list0";static char _tmp406[14]="field_pattern";static char _tmp407[19]="field_pattern_list";static char _tmp408[20]="field_pattern_list0";static char _tmp409[11]="expression";static char _tmp40A[22]="assignment_expression";static char _tmp40B[20]="assignment_operator";static char _tmp40C[23]="conditional_expression";static char _tmp40D[20]="constant_expression";static char _tmp40E[22]="logical_or_expression";static char _tmp40F[23]="logical_and_expression";static char _tmp410[24]="inclusive_or_expression";static char _tmp411[24]="exclusive_or_expression";static char _tmp412[15]="and_expression";static char _tmp413[20]="equality_expression";static char _tmp414[22]="relational_expression";static char _tmp415[17]="shift_expression";static char _tmp416[20]="additive_expression";static char _tmp417[26]="multiplicative_expression";static char _tmp418[16]="cast_expression";static char _tmp419[17]="unary_expression";static char _tmp41A[15]="unary_operator";static char _tmp41B[19]="postfix_expression";static char _tmp41C[17]="field_expression";static char _tmp41D[19]="primary_expression";static char _tmp41E[25]="argument_expression_list";static char _tmp41F[26]="argument_expression_list0";static char _tmp420[9]="constant";static char _tmp421[20]="qual_opt_identifier";static char _tmp422[17]="qual_opt_typedef";static char _tmp423[18]="struct_union_name";static char _tmp424[11]="field_name";static char _tmp425[12]="right_angle";
# 1582 "parse.y"
static struct _dyneither_ptr Cyc_yytname[293]={{_tmp301,_tmp301,_tmp301 + 2},{_tmp302,_tmp302,_tmp302 + 6},{_tmp303,_tmp303,_tmp303 + 12},{_tmp304,_tmp304,_tmp304 + 5},{_tmp305,_tmp305,_tmp305 + 9},{_tmp306,_tmp306,_tmp306 + 7},{_tmp307,_tmp307,_tmp307 + 7},{_tmp308,_tmp308,_tmp308 + 8},{_tmp309,_tmp309,_tmp309 + 5},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,_tmp30B,_tmp30B + 6},{_tmp30C,_tmp30C,_tmp30C + 4},{_tmp30D,_tmp30D,_tmp30D + 5},{_tmp30E,_tmp30E,_tmp30E + 6},{_tmp30F,_tmp30F,_tmp30F + 7},{_tmp310,_tmp310,_tmp310 + 7},{_tmp311,_tmp311,_tmp311 + 9},{_tmp312,_tmp312,_tmp312 + 6},{_tmp313,_tmp313,_tmp313 + 9},{_tmp314,_tmp314,_tmp314 + 9},{_tmp315,_tmp315,_tmp315 + 7},{_tmp316,_tmp316,_tmp316 + 6},{_tmp317,_tmp317,_tmp317 + 5},{_tmp318,_tmp318,_tmp318 + 8},{_tmp319,_tmp319,_tmp319 + 7},{_tmp31A,_tmp31A,_tmp31A + 7},{_tmp31B,_tmp31B,_tmp31B + 9},{_tmp31C,_tmp31C,_tmp31C + 3},{_tmp31D,_tmp31D,_tmp31D + 5},{_tmp31E,_tmp31E,_tmp31E + 7},{_tmp31F,_tmp31F,_tmp31F + 6},{_tmp320,_tmp320,_tmp320 + 3},{_tmp321,_tmp321,_tmp321 + 4},{_tmp322,_tmp322,_tmp322 + 5},{_tmp323,_tmp323,_tmp323 + 9},{_tmp324,_tmp324,_tmp324 + 6},{_tmp325,_tmp325,_tmp325 + 7},{_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 7},{_tmp328,_tmp328,_tmp328 + 16},{_tmp329,_tmp329,_tmp329 + 8},{_tmp32A,_tmp32A,_tmp32A + 4},{_tmp32B,_tmp32B,_tmp32B + 6},{_tmp32C,_tmp32C,_tmp32C + 4},{_tmp32D,_tmp32D,_tmp32D + 6},{_tmp32E,_tmp32E,_tmp32E + 7},{_tmp32F,_tmp32F,_tmp32F + 4},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 6},{_tmp333,_tmp333,_tmp333 + 10},{_tmp334,_tmp334,_tmp334 + 9},{_tmp335,_tmp335,_tmp335 + 7},{_tmp336,_tmp336,_tmp336 + 8},{_tmp337,_tmp337,_tmp337 + 15},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 5},{_tmp33F,_tmp33F,_tmp33F + 8},{_tmp340,_tmp340,_tmp340 + 7},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 12},{_tmp343,_tmp343,_tmp343 + 8},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 10},{_tmp346,_tmp346,_tmp346 + 9},{_tmp347,_tmp347,_tmp347 + 13},{_tmp348,_tmp348,_tmp348 + 10},{_tmp349,_tmp349,_tmp349 + 9},{_tmp34A,_tmp34A,_tmp34A + 13},{_tmp34B,_tmp34B,_tmp34B + 14},{_tmp34C,_tmp34C,_tmp34C + 14},{_tmp34D,_tmp34D,_tmp34D + 13},{_tmp34E,_tmp34E,_tmp34E + 12},{_tmp34F,_tmp34F,_tmp34F + 16},{_tmp350,_tmp350,_tmp350 + 14},{_tmp351,_tmp351,_tmp351 + 12},{_tmp352,_tmp352,_tmp352 + 16},{_tmp353,_tmp353,_tmp353 + 7},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 7},{_tmp356,_tmp356,_tmp356 + 8},{_tmp357,_tmp357,_tmp357 + 9},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 6},{_tmp35C,_tmp35C,_tmp35C + 7},{_tmp35D,_tmp35D,_tmp35D + 6},{_tmp35E,_tmp35E,_tmp35E + 11},{_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 11},{_tmp361,_tmp361,_tmp361 + 11},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 12},{_tmp364,_tmp364,_tmp364 + 13},{_tmp365,_tmp365,_tmp365 + 11},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 10},{_tmp368,_tmp368,_tmp368 + 9},{_tmp369,_tmp369,_tmp369 + 11},{_tmp36A,_tmp36A,_tmp36A + 12},{_tmp36B,_tmp36B,_tmp36B + 11},{_tmp36C,_tmp36C,_tmp36C + 17},{_tmp36D,_tmp36D,_tmp36D + 7},{_tmp36E,_tmp36E,_tmp36E + 8},{_tmp36F,_tmp36F,_tmp36F + 19},{_tmp370,_tmp370,_tmp370 + 20},{_tmp371,_tmp371,_tmp371 + 18},{_tmp372,_tmp372,_tmp372 + 9},{_tmp373,_tmp373,_tmp373 + 13},{_tmp374,_tmp374,_tmp374 + 16},{_tmp375,_tmp375,_tmp375 + 18},{_tmp376,_tmp376,_tmp376 + 10},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,_tmp392,_tmp392 + 4},{_tmp393,_tmp393,_tmp393 + 5},{_tmp394,_tmp394,_tmp394 + 17},{_tmp395,_tmp395,_tmp395 + 16},{_tmp396,_tmp396,_tmp396 + 13},{_tmp397,_tmp397,_tmp397 + 12},{_tmp398,_tmp398,_tmp398 + 19},{_tmp399,_tmp399,_tmp399 + 21},{_tmp39A,_tmp39A,_tmp39A + 15},{_tmp39B,_tmp39B,_tmp39B + 20},{_tmp39C,_tmp39C,_tmp39C + 21},{_tmp39D,_tmp39D,_tmp39D + 13},{_tmp39E,_tmp39E,_tmp39E + 15},{_tmp39F,_tmp39F,_tmp39F + 17},{_tmp3A0,_tmp3A0,_tmp3A0 + 19},{_tmp3A1,_tmp3A1,_tmp3A1 + 12},{_tmp3A2,_tmp3A2,_tmp3A2 + 17},{_tmp3A3,_tmp3A3,_tmp3A3 + 23},{_tmp3A4,_tmp3A4,_tmp3A4 + 24},{_tmp3A5,_tmp3A5,_tmp3A5 + 15},{_tmp3A6,_tmp3A6,_tmp3A6 + 11},{_tmp3A7,_tmp3A7,_tmp3A7 + 15},{_tmp3A8,_tmp3A8,_tmp3A8 + 10},{_tmp3A9,_tmp3A9,_tmp3A9 + 15},{_tmp3AA,_tmp3AA,_tmp3AA + 25},{_tmp3AB,_tmp3AB,_tmp3AB + 5},{_tmp3AC,_tmp3AC,_tmp3AC + 15},{_tmp3AD,_tmp3AD,_tmp3AD + 15},{_tmp3AE,_tmp3AE,_tmp3AE + 11},{_tmp3AF,_tmp3AF,_tmp3AF + 22},{_tmp3B0,_tmp3B0,_tmp3B0 + 26},{_tmp3B1,_tmp3B1,_tmp3B1 + 16},{_tmp3B2,_tmp3B2,_tmp3B2 + 16},{_tmp3B3,_tmp3B3,_tmp3B3 + 24},{_tmp3B4,_tmp3B4,_tmp3B4 + 25},{_tmp3B5,_tmp3B5,_tmp3B5 + 21},{_tmp3B6,_tmp3B6,_tmp3B6 + 22},{_tmp3B7,_tmp3B7,_tmp3B7 + 16},{_tmp3B8,_tmp3B8,_tmp3B8 + 19},{_tmp3B9,_tmp3B9,_tmp3B9 + 25},{_tmp3BA,_tmp3BA,_tmp3BA + 35},{_tmp3BB,_tmp3BB,_tmp3BB + 23},{_tmp3BC,_tmp3BC,_tmp3BC + 24},{_tmp3BD,_tmp3BD,_tmp3BD + 18},{_tmp3BE,_tmp3BE,_tmp3BE + 20},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,_tmp3C0 + 19},{_tmp3C1,_tmp3C1,_tmp3C1 + 14},{_tmp3C2,_tmp3C2,_tmp3C2 + 19},{_tmp3C3,_tmp3C3,_tmp3C3 + 20},{_tmp3C4,_tmp3C4,_tmp3C4 + 14},{_tmp3C5,_tmp3C5,_tmp3C5 + 11},{_tmp3C6,_tmp3C6,_tmp3C6 + 23},{_tmp3C7,_tmp3C7,_tmp3C7 + 18},{_tmp3C8,_tmp3C8,_tmp3C8 + 30},{_tmp3C9,_tmp3C9,_tmp3C9 + 8},{_tmp3CA,_tmp3CA,_tmp3CA + 12},{_tmp3CB,_tmp3CB,_tmp3CB + 14},{_tmp3CC,_tmp3CC,_tmp3CC + 13},{_tmp3CD,_tmp3CD,_tmp3CD + 23},{_tmp3CE,_tmp3CE,_tmp3CE + 14},{_tmp3CF,_tmp3CF,_tmp3CF + 18},{_tmp3D0,_tmp3D0,_tmp3D0 + 8},{_tmp3D1,_tmp3D1,_tmp3D1 + 11},{_tmp3D2,_tmp3D2,_tmp3D2 + 20},{_tmp3D3,_tmp3D3,_tmp3D3 + 9},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 19},{_tmp3D6,_tmp3D6,_tmp3D6 + 10},{_tmp3D7,_tmp3D7,_tmp3D7 + 16},{_tmp3D8,_tmp3D8,_tmp3D8 + 11},{_tmp3D9,_tmp3D9,_tmp3D9 + 14},{_tmp3DA,_tmp3DA,_tmp3DA + 11},{_tmp3DB,_tmp3DB,_tmp3DB + 15},{_tmp3DC,_tmp3DC,_tmp3DC + 22},{_tmp3DD,_tmp3DD,_tmp3DD + 16},{_tmp3DE,_tmp3DE,_tmp3DE + 17},{_tmp3DF,_tmp3DF,_tmp3DF + 12},{_tmp3E0,_tmp3E0,_tmp3E0 + 18},{_tmp3E1,_tmp3E1,_tmp3E1 + 17},{_tmp3E2,_tmp3E2,_tmp3E2 + 12},{_tmp3E3,_tmp3E3,_tmp3E3 + 16},{_tmp3E4,_tmp3E4,_tmp3E4 + 11},{_tmp3E5,_tmp3E5,_tmp3E5 + 10},{_tmp3E6,_tmp3E6,_tmp3E6 + 14},{_tmp3E7,_tmp3E7,_tmp3E7 + 15},{_tmp3E8,_tmp3E8,_tmp3E8 + 20},{_tmp3E9,_tmp3E9,_tmp3E9 + 27},{_tmp3EA,_tmp3EA,_tmp3EA + 10},{_tmp3EB,_tmp3EB,_tmp3EB + 18},{_tmp3EC,_tmp3EC,_tmp3EC + 21},{_tmp3ED,_tmp3ED,_tmp3ED + 19},{_tmp3EE,_tmp3EE,_tmp3EE + 16},{_tmp3EF,_tmp3EF,_tmp3EF + 20},{_tmp3F0,_tmp3F0,_tmp3F0 + 15},{_tmp3F1,_tmp3F1,_tmp3F1 + 20},{_tmp3F2,_tmp3F2,_tmp3F2 + 15},{_tmp3F3,_tmp3F3,_tmp3F3 + 12},{_tmp3F4,_tmp3F4,_tmp3F4 + 20},{_tmp3F5,_tmp3F5,_tmp3F5 + 19},{_tmp3F6,_tmp3F6,_tmp3F6 + 20},{_tmp3F7,_tmp3F7,_tmp3F7 + 21},{_tmp3F8,_tmp3F8,_tmp3F8 + 21},{_tmp3F9,_tmp3F9,_tmp3F9 + 12},{_tmp3FA,_tmp3FA,_tmp3FA + 17},{_tmp3FB,_tmp3FB,_tmp3FB + 19},{_tmp3FC,_tmp3FC,_tmp3FC + 14},{_tmp3FD,_tmp3FD,_tmp3FD + 17},{_tmp3FE,_tmp3FE,_tmp3FE + 23},{_tmp3FF,_tmp3FF,_tmp3FF + 13},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 16},{_tmp402,_tmp402,_tmp402 + 16},{_tmp403,_tmp403,_tmp403 + 8},{_tmp404,_tmp404,_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 20},{_tmp406,_tmp406,_tmp406 + 14},{_tmp407,_tmp407,_tmp407 + 19},{_tmp408,_tmp408,_tmp408 + 20},{_tmp409,_tmp409,_tmp409 + 11},{_tmp40A,_tmp40A,_tmp40A + 22},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 23},{_tmp40D,_tmp40D,_tmp40D + 20},{_tmp40E,_tmp40E,_tmp40E + 22},{_tmp40F,_tmp40F,_tmp40F + 23},{_tmp410,_tmp410,_tmp410 + 24},{_tmp411,_tmp411,_tmp411 + 24},{_tmp412,_tmp412,_tmp412 + 15},{_tmp413,_tmp413,_tmp413 + 20},{_tmp414,_tmp414,_tmp414 + 22},{_tmp415,_tmp415,_tmp415 + 17},{_tmp416,_tmp416,_tmp416 + 20},{_tmp417,_tmp417,_tmp417 + 26},{_tmp418,_tmp418,_tmp418 + 16},{_tmp419,_tmp419,_tmp419 + 17},{_tmp41A,_tmp41A,_tmp41A + 15},{_tmp41B,_tmp41B,_tmp41B + 19},{_tmp41C,_tmp41C,_tmp41C + 17},{_tmp41D,_tmp41D,_tmp41D + 19},{_tmp41E,_tmp41E,_tmp41E + 25},{_tmp41F,_tmp41F,_tmp41F + 26},{_tmp420,_tmp420,_tmp420 + 9},{_tmp421,_tmp421,_tmp421 + 20},{_tmp422,_tmp422,_tmp422 + 17},{_tmp423,_tmp423,_tmp423 + 18},{_tmp424,_tmp424,_tmp424 + 11},{_tmp425,_tmp425,_tmp425 + 12}};
# 1637
static short Cyc_yyr1[527]={0,146,147,147,147,147,147,147,147,147,147,147,148,149,150,150,151,151,151,152,152,152,153,153,154,154,154,154,155,155,156,157,158,159,160,160,160,160,160,160,160,161,161,162,162,162,162,162,162,162,162,162,162,163,163,163,163,163,163,163,164,164,165,166,166,167,167,167,167,168,168,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,170,171,171,171,172,172,172,173,173,174,174,174,175,175,175,175,175,176,176,177,177,178,178,179,179,180,181,181,182,182,183,184,184,184,184,184,184,185,185,185,185,185,185,186,187,187,188,188,188,188,189,189,190,190,191,191,191,192,192,193,193,193,193,194,194,194,195,195,196,196,197,197,198,198,198,198,198,198,198,198,198,198,199,199,199,199,199,199,199,199,199,199,199,200,200,201,202,202,203,203,203,203,203,203,203,203,204,204,204,205,205,206,206,206,207,207,207,208,208,209,209,209,209,210,210,211,211,212,212,213,213,214,214,215,215,216,216,216,216,217,217,218,218,219,219,219,220,221,221,222,222,223,223,223,223,223,224,224,224,224,225,225,226,226,227,227,228,228,229,229,229,229,229,230,230,231,231,231,232,232,232,232,232,232,232,232,232,232,232,233,233,233,233,233,233,234,235,235,236,236,237,237,237,237,237,237,237,237,238,238,238,238,238,238,239,239,239,239,239,239,240,240,240,240,240,240,240,240,240,240,240,240,240,240,241,241,241,241,241,241,241,241,242,243,243,244,244,245,245,246,246,247,247,248,248,249,249,249,250,250,250,250,250,251,251,251,252,252,252,253,253,253,253,254,254,255,255,255,255,255,255,256,257,258,258,258,258,258,258,258,258,258,258,258,258,258,258,258,258,259,259,259,260,260,261,261,262,262,262,263,263,264,264,265,265,265,266,266,266,266,266,266,266,266,266,266,266,267,267,267,267,267,267,267,268,269,269,270,270,271,271,272,272,273,273,274,274,274,275,275,275,275,275,276,276,276,277,277,277,278,278,278,278,279,279,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,281,281,281,282,282,282,282,282,282,282,282,282,282,282,283,283,283,283,284,284,284,284,284,284,284,284,284,284,285,286,286,287,287,287,287,287,288,288,289,289,290,290,291,291,292,292};
# 1693
static short Cyc_yyr2[527]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1749
static short Cyc_yydefact[1070]={0,21,53,54,55,56,58,71,72,73,74,75,76,77,78,79,97,98,99,115,116,49,0,0,83,0,59,0,0,153,90,94,0,0,0,0,0,0,0,517,219,519,518,520,0,0,86,0,205,205,204,1,0,0,19,0,0,20,0,43,51,45,69,47,80,81,0,84,0,0,164,0,189,192,85,168,113,57,56,50,0,101,0,516,0,517,512,513,514,515,113,0,379,0,0,0,0,242,0,381,382,30,32,0,0,0,0,0,0,0,0,0,154,0,0,0,0,0,0,0,202,203,0,2,0,0,0,0,34,0,121,122,124,44,52,46,48,117,521,522,113,113,0,41,0,0,23,0,221,0,177,165,190,0,196,197,200,201,0,199,198,210,192,0,70,57,105,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,501,502,480,0,0,0,0,0,483,481,482,0,407,409,423,431,433,435,437,439,441,444,449,452,455,459,0,461,484,500,499,517,390,0,0,0,0,0,391,389,37,0,0,113,0,0,0,131,127,129,262,264,0,0,39,0,0,9,10,0,0,113,523,524,220,96,0,0,169,87,240,0,237,0,0,3,0,5,0,35,0,0,0,23,0,118,119,144,112,0,151,0,0,0,0,0,0,0,0,0,0,0,517,292,294,0,302,296,0,300,285,286,287,0,288,289,290,0,42,23,124,22,24,269,0,227,243,0,0,223,221,0,207,0,0,0,212,60,211,193,0,106,102,0,0,0,469,0,425,459,0,426,427,0,0,0,0,0,0,0,0,0,0,462,463,0,0,0,0,465,466,464,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,414,415,416,417,418,419,420,421,422,412,0,467,0,490,491,0,0,0,504,0,113,383,0,0,404,517,0,0,0,0,258,400,405,0,402,0,380,397,398,0,395,0,244,0,0,0,0,265,0,235,132,137,133,135,128,130,221,0,271,263,272,526,525,0,89,93,0,0,0,91,95,111,66,65,0,63,170,221,239,166,271,241,178,179,0,88,206,13,0,31,0,33,0,123,125,246,245,23,25,108,120,0,0,0,139,140,147,0,113,113,159,0,0,0,0,0,517,0,0,0,331,332,333,0,0,335,0,0,0,303,297,124,301,295,293,26,0,176,228,0,0,0,234,222,229,147,0,0,0,223,175,209,208,171,207,0,0,213,61,114,107,430,104,100,0,0,0,517,247,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,503,510,0,509,408,432,0,434,436,438,440,442,443,447,448,445,446,450,451,453,454,456,457,458,411,410,489,486,0,488,0,0,0,393,394,261,0,401,256,259,388,0,257,385,0,392,36,0,386,0,266,0,138,134,136,0,223,0,207,0,273,0,221,0,284,268,0,0,0,113,0,0,0,131,0,113,0,221,0,188,167,238,0,7,0,4,6,27,0,143,126,144,0,0,142,223,152,161,160,0,0,155,0,0,0,310,0,0,0,0,0,0,330,334,0,0,0,298,291,0,28,270,221,0,231,0,0,149,224,0,147,227,215,172,194,195,213,191,468,0,0,248,0,253,471,0,0,0,0,0,476,479,0,0,0,0,0,460,506,0,0,487,485,0,0,0,260,403,406,396,399,387,267,236,147,0,274,275,207,0,0,223,207,0,0,38,92,223,517,0,62,64,0,180,0,0,223,0,207,0,0,8,141,0,145,117,150,162,159,159,0,0,0,0,0,0,0,0,0,0,0,0,0,310,336,0,299,29,223,0,232,230,0,173,0,149,223,0,214,496,0,495,0,249,254,0,0,0,0,0,428,429,489,488,508,492,0,511,424,505,507,0,384,149,147,277,283,147,0,276,207,0,117,0,67,181,187,147,0,186,182,207,15,0,16,0,0,0,158,157,304,310,0,0,0,0,0,0,338,339,341,343,345,347,349,351,354,359,362,365,369,371,377,378,0,0,307,316,0,0,0,0,0,0,0,0,0,0,337,217,233,0,225,174,216,221,470,0,0,255,472,473,0,0,478,477,493,0,280,149,149,147,278,40,0,0,149,147,183,14,17,0,146,110,0,0,0,310,0,375,0,0,372,310,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,373,310,0,318,0,0,0,326,0,0,0,0,0,0,309,0,0,223,498,497,0,0,0,494,279,282,149,109,0,184,149,18,163,305,306,0,0,0,0,310,312,342,0,344,346,348,350,352,353,357,358,355,356,360,361,363,364,366,367,368,0,311,317,319,320,0,328,327,0,322,0,0,0,148,226,218,0,0,0,0,281,0,185,308,374,0,370,313,0,310,321,329,323,324,0,251,250,474,0,68,0,340,310,314,325,0,376,315,475,0,0,0};
# 1859
static short Cyc_yydefgoto[147]={1067,51,52,468,648,847,53,307,54,291,55,470,56,472,57,144,58,59,537,232,454,455,233,62,249,234,64,166,167,65,164,66,269,270,129,130,131,271,235,434,483,484,485,658,797,67,68,663,664,665,69,486,70,459,71,72,161,162,73,120,533,322,701,619,74,620,527,692,519,523,524,428,315,257,96,97,549,475,550,408,409,410,236,308,309,621,440,294,295,296,297,298,299,779,300,301,861,862,863,864,865,866,867,868,869,870,871,872,873,874,875,876,411,419,420,412,413,414,302,197,390,198,542,199,200,201,202,203,204,205,206,207,208,209,210,211,212,804,213,566,567,214,215,76,848,415,444};
# 1877
static short Cyc_yypact[1070]={2919,- -32768,- -32768,- -32768,- -32768,- 60,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3610,205,- 61,- -32768,1243,- -32768,65,- 39,- -32768,- 29,- 27,66,18,27,64,76,505,165,- -32768,135,- -32768,- -32768,- -32768,174,516,156,204,143,143,- -32768,- -32768,237,2782,- -32768,665,708,- -32768,450,3610,3610,3610,- -32768,3610,- -32768,- -32768,614,- -32768,65,3526,511,154,542,769,- -32768,- -32768,236,248,274,- -32768,65,276,6589,- -32768,4484,291,- -32768,- -32768,- -32768,- -32768,236,6589,- -32768,268,301,4484,322,326,337,- -32768,225,- -32768,- -32768,1223,1223,- 5,354,2782,2782,1223,6589,552,- -32768,241,353,662,362,241,4178,6589,- -32768,- -32768,3055,- -32768,2782,3055,2782,3055,- -32768,378,377,- -32768,3399,- -32768,- -32768,- -32768,- -32768,4178,- -32768,- -32768,236,175,1794,- -32768,3526,450,400,1223,3691,4957,- -32768,511,- -32768,409,- -32768,- -32768,- -32768,- -32768,411,- -32768,- -32768,106,769,1223,- -32768,- -32768,440,449,493,65,6732,499,6589,3729,534,538,584,591,593,627,634,636,684,6827,6827,- -32768,- -32768,- -32768,2361,688,6874,6874,6874,- -32768,- -32768,- -32768,191,- -32768,- -32768,- 13,600,602,592,672,661,302,686,100,197,- -32768,1081,6874,254,34,- -32768,698,105,- -32768,4484,110,945,361,781,716,- -32768,- -32768,756,6589,236,781,725,3935,4178,4259,4178,243,- -32768,80,80,- -32768,770,758,- -32768,- -32768,67,427,236,- -32768,- -32768,- -32768,- -32768,33,751,- -32768,- -32768,631,446,- -32768,782,785,- -32768,793,- -32768,795,- -32768,662,5053,3526,400,798,4178,- -32768,928,797,65,802,799,255,800,4325,801,817,805,809,5149,4325,309,806,- -32768,- -32768,803,1937,1937,450,1937,- -32768,- -32768,- -32768,818,- -32768,- -32768,- -32768,157,- -32768,400,807,- -32768,- -32768,808,84,832,- -32768,55,813,811,535,816,778,819,6589,1223,- -32768,825,- -32768,- -32768,84,65,- -32768,6589,827,2361,- -32768,4178,- -32768,- -32768,4421,- -32768,854,6589,6589,6589,6589,6589,6589,6589,4178,142,2361,- -32768,- -32768,1937,822,452,6589,- -32768,- -32768,- -32768,6589,- -32768,6874,6589,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6589,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6589,- -32768,241,- -32768,- -32768,5245,241,6589,- -32768,826,236,- -32768,828,829,- -32768,102,241,6589,4484,269,- -32768,- -32768,- -32768,830,834,833,- -32768,- -32768,- -32768,831,840,4484,- -32768,589,945,838,1223,- -32768,845,848,- -32768,4259,4259,4259,- -32768,- -32768,1378,5341,292,- -32768,529,- -32768,- -32768,55,- -32768,- -32768,841,866,1223,- -32768,- -32768,853,- -32768,849,850,857,- -32768,3136,- -32768,571,370,- -32768,- -32768,- -32768,4178,- -32768,- -32768,- -32768,2645,- -32768,2782,- -32768,2782,- -32768,- -32768,- -32768,- -32768,400,- -32768,- -32768,- -32768,996,6589,861,860,- -32768,- 11,336,236,236,859,6589,6589,855,856,6589,858,954,2223,867,- -32768,- -32768,- -32768,599,943,- -32768,5437,2080,2499,- -32768,- -32768,3399,- -32768,- -32768,- -32768,- -32768,1223,- -32768,- -32768,4178,864,4016,- -32768,- -32768,874,916,55,887,4097,811,- -32768,- -32768,- -32768,- -32768,778,888,- 46,775,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,889,871,911,890,- -32768,- -32768,745,5053,891,897,900,901,903,507,899,902,280,904,906,3851,- -32768,- -32768,905,908,- -32768,600,244,602,592,672,661,302,302,686,686,686,686,100,100,197,197,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,910,- -32768,137,1223,4861,4178,- -32768,- -32768,895,- -32768,- -32768,- -32768,- -32768,3229,- -32768,- -32768,7005,- -32768,- -32768,912,- -32768,88,- -32768,4178,- -32768,- -32768,- -32768,914,811,918,778,913,529,1223,3813,5533,- -32768,- -32768,6589,919,80,236,6685,923,33,3265,926,236,1223,3691,5629,- -32768,571,- -32768,915,- -32768,2782,- -32768,- -32768,- -32768,1141,- -32768,- -32768,928,935,6589,- -32768,811,- -32768,- -32768,- -32768,947,65,685,514,562,6589,846,563,4325,940,5725,5821,689,- -32768,- -32768,949,920,948,1937,- -32768,3526,- -32768,808,953,1223,- -32768,957,55,1004,- -32768,958,916,94,- -32768,- -32768,- -32768,- -32768,775,- -32768,962,158,960,- -32768,4531,- -32768,- -32768,6589,6589,1061,6589,3729,- -32768,- -32768,241,241,962,963,4641,- -32768,- -32768,6589,6589,- -32768,- -32768,84,749,981,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,916,964,- -32768,- -32768,778,84,966,811,778,956,565,- -32768,- -32768,811,972,969,- -32768,- -32768,970,- -32768,84,971,811,976,778,973,359,- -32768,- -32768,6589,- -32768,4178,- -32768,982,31,859,4325,989,987,1505,986,997,4325,6589,5917,707,6013,713,6109,846,- -32768,998,- -32768,- -32768,811,161,- -32768,- -32768,993,- -32768,1013,1004,811,4178,- -32768,- -32768,379,- -32768,6589,- -32768,- -32768,5053,1000,1001,1003,1011,- -32768,854,1009,1014,- -32768,- -32768,753,- -32768,- -32768,- -32768,- -32768,4861,- -32768,1004,916,- -32768,- -32768,916,1016,- -32768,778,1021,4178,1037,- -32768,- -32768,- -32768,916,1019,- -32768,- -32768,778,- -32768,1026,714,1022,1027,4178,- -32768,- -32768,1124,846,1040,6969,1036,2499,6874,1033,- -32768,4,- -32768,1078,1032,742,796,321,804,462,404,- -32768,- -32768,- -32768,- -32768,1080,6874,1937,- -32768,- -32768,605,4325,616,6205,4325,648,6301,6397,736,1052,- -32768,- -32768,- -32768,6589,1062,- -32768,- -32768,953,- -32768,533,307,- -32768,- -32768,- -32768,4178,1160,- -32768,- -32768,- -32768,4751,- -32768,1004,1004,916,- -32768,- -32768,1065,1066,1004,916,- -32768,- -32768,- -32768,552,- -32768,- -32768,652,4325,1068,846,2361,- -32768,4178,1060,- -32768,1651,6874,6589,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6874,6589,- -32768,846,1072,- -32768,4325,4325,656,- -32768,4325,4325,659,4325,660,6493,- -32768,1067,55,811,- -32768,- -32768,2499,1070,1069,- -32768,- -32768,- -32768,1004,- -32768,1085,- -32768,1004,- -32768,- -32768,- -32768,- -32768,1079,1074,1077,6874,846,- -32768,600,341,602,592,592,661,302,302,686,686,686,686,100,100,197,197,- -32768,- -32768,- -32768,347,- -32768,- -32768,- -32768,- -32768,4325,- -32768,- -32768,4325,- -32768,4325,4325,673,- -32768,- -32768,- -32768,1082,776,1075,4178,- -32768,1084,- -32768,- -32768,962,158,- -32768,- -32768,6589,1651,- -32768,- -32768,- -32768,- -32768,4325,- -32768,- -32768,- -32768,1086,- -32768,567,- -32768,846,- -32768,- -32768,1087,- -32768,- -32768,- -32768,1206,1207,- -32768};
# 1987
static short Cyc_yypgoto[147]={- -32768,202,- -32768,- -32768,- -32768,283,- -32768,- 238,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 64,- 124,41,- -32768,- -32768,0,576,- -32768,52,- 186,1098,23,- -32768,- -32768,- 131,- -32768,16,1179,- 695,- -32768,- -32768,- -32768,952,951,701,428,- -32768,- -32768,564,- 166,- 636,- -32768,- -32768,127,- -32768,- -32768,238,- 199,1147,- 402,- 37,- -32768,1063,- -32768,- -32768,1173,- 441,- -32768,523,- 126,- 154,- 108,- 458,252,550,540,- 410,- 487,- 103,- 425,- 128,- -32768,- 248,- 170,- 577,- 284,- -32768,837,- 133,- 77,- 117,- 178,- 274,138,- -32768,- -32768,- 45,- 278,- -32768,- 258,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,136,1020,- -32768,643,835,- -32768,339,815,- -32768,- 141,- 353,- 162,- 347,- 344,- 361,907,- 339,- 356,- 234,- 343,- 332,- 102,633,473,917,210,- -32768,- 352,- -32768,272,487,- 62,9,- 111,- 2};
# 2009
static short Cyc_yytable[7146]={60,572,250,336,139,143,250,323,267,575,576,337,569,509,510,256,512,571,474,728,316,60,313,63,146,574,237,238,581,582,478,333,244,629,689,152,661,662,329,645,314,583,584,592,63,115,325,432,77,139,452,551,61,60,600,351,458,439,644,60,60,60,79,60,656,82,515,102,143,60,150,696,562,61,850,140,63,461,292,359,303,699,63,63,63,644,63,268,354,355,356,443,63,697,686,103,938,104,429,304,133,134,135,645,136,61,220,60,60,391,145,61,61,61,239,61,693,520,240,657,246,61,60,360,60,60,60,60,737,653,63,63,60,577,578,579,580,107,398,453,939,918,60,820,60,63,108,63,63,63,63,150,- 156,441,680,63,273,275,522,61,61,98,739,897,624,63,441,63,40,399,441,39,105,145,61,521,61,61,61,61,42,741,83,293,61,145,624,541,109,448,106,912,442,463,61,540,61,544,438,545,518,769,110,303,443,442,516,529,218,442,463,218,559,312,561,736,112,443,460,40,217,- 221,477,402,- 221,443,219,292,292,219,292,224,- 523,431,321,487,445,373,403,651,113,374,449,536,424,432,432,432,39,86,185,186,87,88,89,123,433,42,638,357,39,117,451,119,247,803,541,60,347,42,726,189,248,585,586,587,514,982,983,357,45,590,638,116,987,593,292,832,894,522,63,60,60,836,60,599,132,99,443,163,114,829,842,707,768,833,274,517,145,242,243,39,551,357,63,63,776,63,358,61,42,844,538,539,259,80,260,261,262,263,118,375,293,293,893,293,392,393,394,376,377,541,898,61,61,228,61,247,1038,613,60,229,1040,252,401,248,99,122,418,691,163,39,716,393,394,418,357,99,- 12,436,42,632,522,63,724,437,899,48,49,50,395,492,165,305,493,396,397,683,429,367,368,293,602,916,222,639,169,218,463,438,406,407,61,790,922,256,395,223,946,947,- 243,717,397,- 243,849,219,596,497,436,463,460,196,809,504,437,463,369,370,505,357,221,431,431,431,674,506,551,978,756,685,628,225,39,681,487,948,949,143,226,245,41,42,43,756,433,433,433,637,808,643,227,721,481,357,729,39,684,241,60,357,60,1046,60,41,42,43,39,1047,727,251,846,652,429,357,41,42,43,75,416,253,99,63,99,63,99,63,457,264,60,265,744,99,437,305,659,660,900,60,743,81,901,60,100,761,101,759,541,1033,745,306,61,63,61,758,61,19,20,352,799,891,63,511,75,760,63,319,522,320,954,145,75,809,973,814,601,75,955,956,145,357,61,815,145,138,450,141,39,609,75,61,700,903,326,61,693,42,423,168,464,128,327,100,827,465,357,639,45,808,792,564,1002,1003,100,822,48,49,50,800,1005,1006,463,999,805,463,75,75,1001,930,138,952,487,959,75,953,1004,816,817,1011,1012,75,793,75,75,75,75,487,328,292,487,303,1013,1014,39,503,628,332,541,809,44,357,750,42,75,44,713,147,357,148,791,247,976,774,45,149,643,682,44,248,60,751,48,49,50,625,312,626,168,528,39,997,338,627,913,808,339,914,41,42,43,352,647,63,649,993,650,920,48,49,50,998,99,60,557,60,357,357,352,357,44,775,780,361,835,99,1064,640,99,641,901,570,61,1019,139,642,63,100,63,100,610,100,340,357,1007,1008,1009,1010,100,341,677,342,39,357,293,700,145,823,935,357,41,42,43,363,960,61,137,61,594,39,357,1045,362,830,462,962,734,41,42,43,928,984,365,366,75,343,541,988,840,457,936,462,344,489,345,437,494,48,49,50,39,1060,357,371,372,979,464,966,958,42,357,990,75,357,357,1023,124,125,1026,1028,45,1061,977,975,16,17,18,357,48,49,50,994,1052,995,1065,331,772,334,334,773,786,682,346,357,364,168,353,292,348,349,400,255,522,83,421,334,334,334,885,126,127,357,666,667,888,924,670,357,925,675,272,153,154,155,156,157,334,1034,158,159,160,255,765,426,1015,1016,1017,971,531,532,357,616,617,618,422,139,661,662,705,706,777,778,824,825,84,292,910,911,446,99,456,60,99,942,943,447,417,944,945,216,86,950,951,87,88,89,1044,100,42,1054,357,852,853,90,63,466,1056,1059,467,91,100,92,93,100,854,877,469,94,471,488,881,479,293,95,490,499,500,491,495,498,501,508,266,61,805,430,258,435,507,60,518,513,525,526,44,530,462,443,359,462,543,563,595,604,534,597,598,75,605,75,607,75,63,334,608,606,318,612,614,630,462,748,615,272,631,633,462,634,669,635,293,636,654,668,655,292,672,83,676,678,671,61,687,656,334,703,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,690,783,785,694,698,702,704,708,709,- 523,961,710,711,965,712,714,719,730,715,723,718,722,735,39,764,84,725,749,788,334,738,41,42,43,740,742,60,404,99,754,405,86,757,481,87,88,89,482,248,42,767,48,49,50,90,781,991,770,787,63,91,312,92,93,789,406,407,794,94,796,476,720,798,806,95,812,826,293,834,818,828,100,831,837,100,838,839,841,61,1021,1022,39,843,1024,1025,845,1027,851,855,41,42,43,44,856,334,879,892,880,895,882,884,481,887,462,890,896,904,905,906,48,49,50,907,535,75,908,255,378,462,917,909,462,915,919,902,921,923,927,926,476,771,929,552,553,554,555,556,255,558,931,1048,934,937,1049,255,1050,1051,565,940,941,957,568,972,379,380,381,382,383,384,385,386,387,388,974,980,985,996,986,992,1062,1020,1039,588,1031,1037,334,1036,221,1043,1041,1042,1055,1053,389,589,1068,1069,989,255,565,1057,755,1063,1066,254,111,473,151,766,255,480,121,802,964,324,1032,968,970,255,795,7,8,9,10,11,12,13,14,15,16,17,18,19,20,801,603,39,732,425,878,138,623,1058,0,41,42,43,0,611,22,23,24,560,100,0,0,481,0,0,573,352,0,0,29,48,49,50,1000,0,0,30,31,83,0,230,0,0,35,0,334,36,0,0,0,0,1018,0,0,0,0,0,0,37,38,0,0,0,0,0,1030,0,0,0,0,0,0,1035,0,0,0,565,0,565,0,0,0,255,0,0,0,0,0,0,84,0,40,41,430,43,44,0,255,231,0,0,334,0,0,85,86,46,47,87,88,89,0,334,42,0,0,0,0,90,0,0,476,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,0,95,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,334,0,0,0,0,0,0,0,0,0,0,476,138,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,747,35,0,0,36,0,0,753,0,0,0,0,0,0,0,763,37,38,0,0,0,0,0,0,0,0,0,0,272,0,0,0,0,0,0,0,0,0,0,310,0,565,0,0,0,0,0,0,933,40,41,334,43,44,0,312,0,0,0,0,255,0,436,0,46,47,0,0,437,334,48,49,50,0,0,0,0,0,0,476,0,0,810,811,0,813,334,0,857,858,0,0,0,476,0,272,821,0,0,0,0,0,0,83,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,0,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,0,0,0,0,0,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,216,86,0,0,87,88,89,0,0,42,0,0,0,476,90,0,0,0,334,0,859,0,92,93,0,0,0,0,94,476,0,860,0,0,95,193,0,0,194,195,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,777,778,21,170,171,276,334,277,278,279,280,281,282,283,284,22,23,24,83,25,172,285,0,0,173,26,286,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,35,180,181,36,182,0,0,0,0,0,476,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,86,185,186,87,88,89,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,276,0,277,278,279,280,281,282,283,284,22,23,24,83,25,172,285,0,0,173,26,286,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,35,180,181,36,182,0,0,0,0,0,0,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,86,185,186,87,88,89,40,41,42,43,44,187,288,142,289,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,276,0,277,278,279,280,281,282,283,284,22,23,24,83,25,172,285,0,0,173,26,286,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,35,180,181,36,182,0,0,0,0,0,0,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,86,185,186,87,88,89,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,276,0,277,278,279,280,281,282,283,284,22,23,24,83,25,172,285,0,0,173,26,286,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,35,180,181,36,182,0,0,0,0,0,0,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,496,86,185,186,87,88,89,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,25,172,0,0,0,173,26,0,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,35,180,181,36,182,0,0,0,0,0,0,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,40,41,42,43,44,187,673,0,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,35,180,181,36,182,0,0,0,0,0,0,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,40,41,42,43,44,187,0,350,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,35,180,181,36,182,0,0,0,0,0,0,0,0,0,0,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,40,41,42,43,44,187,0,0,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,646,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,0,0,0,0,45,0,46,47,0,0,29,0,48,49,50,0,0,30,31,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,310,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,312,0,0,0,0,0,0,457,0,46,47,0,0,437,83,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,84,0,0,30,31,0,0,0,0,0,35,0,731,36,0,405,86,0,0,87,88,89,0,248,42,37,38,0,0,90,0,0,0,0,0,91,0,92,93,0,406,407,0,94,0,0,0,0,0,95,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,481,0,46,47,0,0,0,0,48,49,50,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,32,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,0,142,0,0,266,0,0,0,0,46,47,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,32,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,142,22,23,24,0,0,0,0,46,47,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,170,171,35,0,0,36,0,0,0,0,0,0,0,0,0,83,0,37,38,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,310,180,181,311,182,0,0,0,0,0,40,41,0,43,44,0,312,0,183,184,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,39,86,185,186,87,88,89,0,0,42,0,0,187,0,335,22,23,24,0,0,188,0,0,189,0,0,0,0,190,29,0,191,0,0,192,193,30,31,194,195,0,170,171,35,0,0,36,0,0,0,0,0,0,0,0,0,83,0,37,38,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,310,180,181,0,182,0,0,0,0,0,40,41,0,43,44,0,312,0,183,184,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,39,86,185,186,87,88,89,0,0,42,0,0,187,0,720,22,23,24,0,0,188,0,0,189,0,0,0,0,190,29,0,191,0,0,192,193,30,31,194,195,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,427,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,688,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,695,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,170,171,276,0,277,278,279,280,281,282,283,284,0,0,0,83,0,172,285,0,0,173,40,286,0,0,44,174,175,176,177,178,0,0,0,0,179,46,47,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,496,86,185,186,87,88,89,0,0,42,0,0,187,288,142,170,171,0,0,0,188,0,546,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,83,0,0,547,86,185,186,87,88,89,0,248,42,0,0,187,0,335,548,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,83,0,172,0,84,0,173,0,0,0,0,0,174,175,176,177,178,0,0,216,86,179,0,87,88,89,180,181,42,182,0,0,0,90,0,0,0,0,0,91,0,92,93,183,184,0,0,94,0,0,0,0,0,95,0,0,0,0,0,0,0,0,0,0,0,0,547,86,185,186,87,88,89,0,248,42,0,0,187,0,335,807,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,547,86,185,186,87,88,89,0,248,42,0,0,187,0,335,819,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,547,86,185,186,87,88,89,0,248,42,0,0,187,0,335,981,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,547,86,185,186,87,88,89,0,248,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,406,407,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,317,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,502,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,591,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,622,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,679,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,746,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,762,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,782,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,784,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,883,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,886,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,889,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,963,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,967,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,969,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,1029,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,83,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,183,184,0,0,83,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,752,86,185,186,87,88,89,180,181,42,182,0,187,0,0,0,0,0,0,0,188,0,0,189,183,184,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,170,171,0,0,0,0,330,0,0,189,0,0,0,0,190,83,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,183,184,0,0,83,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,39,86,185,186,87,88,89,180,181,42,182,0,187,0,0,0,0,0,0,0,347,0,0,189,183,184,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,170,171,0,0,0,0,188,0,0,189,0,0,0,0,190,83,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,83,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,185,186,87,88,89,0,0,42,0,0,187,0,0,0,0,0,0,0,932,84,0,189,0,0,0,0,190,0,0,191,0,733,192,193,216,86,194,195,87,88,89,0,0,42,0,0,0,0,90,0,0,0,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,0,95};
# 2727
static short Cyc_yycheck[7146]={0,362,113,173,66,69,117,161,132,365,366,173,359,291,292,118,294,361,266,596,148,21,148,0,69,364,103,104,371,372,268,172,109,443,521,72,5,6,169,464,148,373,374,395,21,45,163,233,108,111,17,335,0,53,407,188,255,235,460,59,60,61,21,63,75,126,304,106,132,69,70,529,350,21,769,66,53,255,142,92,144,127,59,60,61,487,63,132,190,191,192,137,69,534,519,124,92,124,231,144,59,60,61,528,63,53,90,107,108,211,69,59,60,61,119,63,526,62,123,130,111,69,122,136,124,125,126,127,615,482,107,108,132,367,368,369,370,119,104,106,136,836,142,720,144,122,119,124,125,126,127,151,121,86,506,132,140,141,312,107,108,25,620,799,438,142,86,144,113,135,86,106,106,132,122,120,124,125,126,127,115,622,40,142,132,144,460,328,124,122,124,827,125,255,142,326,144,330,235,332,106,659,126,267,137,125,122,315,106,125,272,106,345,119,347,127,51,137,255,113,84,127,267,113,130,137,124,291,292,124,294,95,130,233,128,272,238,137,128,477,105,141,244,320,228,431,432,433,106,107,108,109,110,111,112,53,233,115,457,122,106,105,246,120,106,107,407,267,126,115,133,129,114,375,376,377,119,913,914,122,126,392,481,45,920,396,350,745,127,443,267,291,292,751,294,406,58,25,137,124,126,742,760,551,657,746,131,309,267,107,108,106,596,122,291,292,668,294,127,267,115,762,322,325,122,120,124,125,126,127,126,134,291,292,792,294,82,83,84,142,143,482,800,291,292,120,294,106,984,426,350,126,988,115,218,114,84,120,222,525,124,106,82,83,84,229,122,95,120,126,115,448,526,350,130,132,801,134,135,136,126,126,108,145,129,131,132,511,521,87,88,350,123,834,126,457,120,106,460,436,131,132,350,681,845,508,126,106,87,88,119,131,132,122,767,124,400,279,126,481,457,82,706,285,132,487,124,125,119,122,91,431,432,433,498,126,720,130,637,516,440,119,106,507,481,124,125,511,122,110,114,115,116,652,431,432,433,457,706,459,123,563,126,122,597,106,511,113,468,122,470,130,472,114,115,116,106,130,595,126,121,481,615,122,114,115,116,0,127,127,218,468,220,470,222,472,126,119,498,122,626,229,132,265,488,489,127,507,625,22,131,511,25,641,27,641,657,975,626,119,468,498,470,640,472,20,21,188,694,787,507,293,45,641,511,126,690,126,134,498,53,825,895,713,408,58,142,143,507,122,498,713,511,66,127,68,106,421,71,507,537,809,122,511,974,115,227,80,122,119,121,84,738,127,122,637,126,825,686,127,941,942,95,724,134,135,136,695,944,945,652,938,703,655,107,108,940,855,111,137,637,879,115,141,943,716,717,950,951,122,687,124,125,126,127,652,123,681,655,683,952,953,106,284,624,126,767,911,117,122,632,115,145,117,127,124,122,126,683,106,107,127,126,132,644,507,117,114,648,633,134,135,136,124,119,126,169,122,106,937,126,132,828,911,126,831,114,115,116,330,468,648,470,931,472,841,134,135,136,937,408,681,343,683,122,122,347,122,117,127,127,91,127,421,127,124,424,126,131,360,648,959,764,132,681,218,683,220,119,222,126,122,946,947,948,949,229,126,119,126,106,122,681,700,683,727,859,122,114,115,116,139,127,681,120,683,397,106,122,997,138,743,255,127,608,114,115,116,851,915,89,90,265,126,895,921,758,126,860,272,126,274,126,132,277,134,135,136,106,1047,122,85,86,906,122,127,878,115,122,127,293,122,122,127,119,120,127,127,126,1047,901,899,17,18,19,122,134,135,136,932,127,934,1060,170,119,172,173,122,119,671,126,122,140,326,126,879,183,184,120,118,974,40,106,190,191,192,119,119,120,122,491,492,119,119,495,122,122,498,137,70,71,72,73,74,211,978,77,78,79,148,648,126,954,955,956,119,78,79,122,431,432,433,106,925,5,6,121,122,22,23,121,122,91,937,121,122,106,605,127,879,608,139,140,125,103,89,90,106,107,85,86,110,111,112,996,408,115,121,122,772,773,120,879,121,1037,1046,121,126,421,128,129,424,774,777,121,134,121,120,780,121,879,140,120,106,119,126,126,126,119,126,123,879,1043,232,119,234,130,937,106,121,127,130,117,127,457,137,92,460,121,127,124,121,133,125,125,468,122,470,127,472,937,328,122,130,149,127,121,126,481,630,122,270,106,120,487,126,120,127,937,122,119,126,122,1047,30,40,119,44,130,937,126,75,359,122,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,137,673,674,127,127,127,106,127,122,130,883,122,122,886,122,127,121,133,127,122,127,127,121,106,120,91,127,119,119,407,127,114,115,116,127,133,1047,103,777,127,106,107,127,126,110,111,112,130,114,115,126,134,135,136,120,126,929,121,120,1047,126,119,128,129,127,131,132,121,134,76,266,120,125,124,140,25,106,1047,133,127,127,605,127,122,608,127,127,127,1047,962,963,106,127,966,967,133,969,126,120,114,115,116,117,127,482,130,119,121,126,781,782,126,784,637,786,113,127,127,126,134,135,136,122,319,648,127,436,57,652,119,127,655,127,107,806,127,121,121,127,335,664,28,338,339,340,341,342,457,344,120,1023,126,130,1026,464,1028,1029,353,91,138,91,357,121,93,94,95,96,97,98,99,100,101,102,122,25,121,127,122,121,1052,119,107,378,127,126,563,127,859,122,121,127,127,121,123,390,0,0,925,508,395,127,636,127,127,117,37,265,71,655,519,270,49,700,885,162,974,888,889,528,690,8,9,10,11,12,13,14,15,16,17,18,19,20,21,695,409,106,605,229,777,764,437,1043,- 1,114,115,116,- 1,424,37,38,39,346,777,- 1,- 1,126,- 1,- 1,363,932,- 1,- 1,51,134,135,136,939,- 1,- 1,58,59,40,- 1,62,- 1,- 1,65,- 1,657,68,- 1,- 1,- 1,- 1,957,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,971,- 1,- 1,- 1,- 1,- 1,- 1,978,- 1,- 1,- 1,506,- 1,508,- 1,- 1,- 1,626,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,113,114,637,116,117,- 1,641,120,- 1,- 1,713,- 1,- 1,106,107,128,129,110,111,112,- 1,724,115,- 1,- 1,- 1,- 1,120,- 1,- 1,551,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,767,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,596,925,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,627,65,- 1,- 1,68,- 1,- 1,634,- 1,- 1,- 1,- 1,- 1,- 1,- 1,642,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,769,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,668,- 1,- 1,- 1,- 1,- 1,- 1,857,113,114,860,116,117,- 1,119,- 1,- 1,- 1,- 1,801,- 1,126,- 1,128,129,- 1,- 1,132,878,134,135,136,- 1,- 1,- 1,- 1,- 1,- 1,706,- 1,- 1,709,710,- 1,712,895,- 1,25,26,- 1,- 1,- 1,720,- 1,836,723,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,851,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,938,- 1,940,941,942,943,944,945,946,947,948,949,950,951,952,953,954,955,956,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,809,120,- 1,- 1,- 1,996,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,825,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1046,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,911,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,46,47,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,45,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,37,38,39,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,51,- 1,134,135,136,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,132,40,134,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,91,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,103,68,- 1,106,107,- 1,- 1,110,111,112,- 1,114,115,80,81,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,129,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,37,38,39,- 1,- 1,- 1,- 1,128,129,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,25,26,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,66,67,106,69,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,83,84,- 1,- 1,- 1,- 1,- 1,128,129,8,9,10,11,12,13,14,15,16,17,18,19,20,21,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,37,38,39,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,51,- 1,137,- 1,- 1,140,141,58,59,144,145,- 1,25,26,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,83,84,- 1,- 1,- 1,- 1,- 1,128,129,8,9,10,11,12,13,14,15,16,17,18,19,20,21,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,37,38,39,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,51,- 1,137,- 1,- 1,140,141,58,59,144,145,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,39,121,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,39,121,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,43,- 1,- 1,46,113,48,- 1,- 1,117,52,53,54,55,56,- 1,- 1,- 1,- 1,61,128,129,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,25,26,- 1,- 1,- 1,126,- 1,32,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,25,26,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,40,- 1,42,- 1,91,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,106,107,61,- 1,110,111,112,66,67,115,69,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,83,84,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,25,26,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,25,26,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,25,26,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,40,137,42,- 1,140,141,46,- 1,144,145,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,106,107,108,109,110,111,112,66,67,115,69,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,83,84,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,40,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,106,107,108,109,110,111,112,66,67,115,69,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,83,84,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,40,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,91,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E1[8]="stdcall";static char _tmp4E2[6]="cdecl";static char _tmp4E3[9]="fastcall";static char _tmp4E4[9]="noreturn";static char _tmp4E5[6]="const";static char _tmp4E6[8]="aligned";static char _tmp4E7[7]="packed";static char _tmp4E8[7]="shared";static char _tmp4E9[7]="unused";static char _tmp4EA[5]="weak";static char _tmp4EB[10]="dllimport";static char _tmp4EC[10]="dllexport";static char _tmp4ED[23]="no_instrument_function";static char _tmp4EE[12]="constructor";static char _tmp4EF[11]="destructor";static char _tmp4F0[22]="no_check_memory_usage";static char _tmp4F1[5]="pure";static char _tmp4F2[14]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp427=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp427;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp839;(_tmp839.YYINITIALSVAL).val=0;(_tmp839.YYINITIALSVAL).tag=64;_tmp839;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp837=200;short*_tmp838=_region_calloc(yyregion,sizeof(short),_tmp837);_tag_dyneither(_tmp838,sizeof(short),_tmp837);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=({_tag_dyneither(({unsigned int _tmp834=(unsigned int)200;union Cyc_YYSTYPE*_tmp835=(union Cyc_YYSTYPE*)({struct _RegionHandle*_tmp93C=yyregion;_region_malloc(_tmp93C,_check_times(sizeof(union Cyc_YYSTYPE),_tmp834));});{unsigned int _tmp836=_tmp834;unsigned int i;for(i=0;i < _tmp836;i ++){_tmp835[i]=(union Cyc_YYSTYPE)yylval;}}_tmp835;}),sizeof(union Cyc_YYSTYPE),(unsigned int)200);});
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=({_tag_dyneither(({unsigned int _tmp831=(unsigned int)200;struct Cyc_Yyltype*_tmp832=(struct Cyc_Yyltype*)({struct _RegionHandle*_tmp93D=yyregion;_region_malloc(_tmp93D,_check_times(sizeof(struct Cyc_Yyltype),_tmp831));});{unsigned int _tmp833=_tmp831;unsigned int i;for(i=0;i < _tmp833;i ++){({typeof((struct Cyc_Yyltype)Cyc_yynewloc())_tmp93E=(struct Cyc_Yyltype)Cyc_yynewloc();_tmp832[i]=_tmp93E;});}}_tmp832;}),sizeof(struct Cyc_Yyltype),(unsigned int)200);});
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
({struct _dyneither_ptr _tmp940=({const char*_tmp428="parser stack overflow";_tag_dyneither(_tmp428,sizeof(char),22);});int _tmp93F=yystate;Cyc_yyerror(_tmp940,_tmp93F,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp431=(unsigned int)yystacksize;short*_tmp432=(short*)({struct _RegionHandle*_tmp941=yyregion;_region_malloc(_tmp941,_check_times(sizeof(short),_tmp431));});struct _dyneither_ptr _tmp434=_tag_dyneither(_tmp432,sizeof(short),_tmp431);{unsigned int _tmp433=_tmp431;unsigned int i;for(i=0;i < _tmp433;i ++){_tmp432[i]=(short)(
i <= yyssp_offset?*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)): 0);}}_tmp434;});
struct _dyneither_ptr yyvs1=({unsigned int _tmp42D=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp42E=(union Cyc_YYSTYPE*)({struct _RegionHandle*_tmp942=yyregion;_region_malloc(_tmp942,_check_times(sizeof(union Cyc_YYSTYPE),_tmp42D));});struct _dyneither_ptr _tmp430=_tag_dyneither(_tmp42E,sizeof(union Cyc_YYSTYPE),_tmp42D);{unsigned int _tmp42F=_tmp42D;unsigned int i;for(i=0;i < _tmp42F;i ++){_tmp42E[i]=(union Cyc_YYSTYPE)(
# 219
i <= yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)): yylval);}}_tmp430;});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp429=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp42A=(struct Cyc_Yyltype*)({struct _RegionHandle*_tmp943=yyregion;_region_malloc(_tmp943,_check_times(sizeof(struct Cyc_Yyltype),_tmp429));});struct _dyneither_ptr _tmp42C=_tag_dyneither(_tmp42A,sizeof(struct Cyc_Yyltype),_tmp429);{unsigned int _tmp42B=_tmp429;unsigned int i;for(i=0;i < _tmp42B;i ++){({typeof((struct Cyc_Yyltype)(
i <= yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):
 Cyc_yynewloc()))_tmp944=(struct Cyc_Yyltype)(
# 222
i <= yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):
 Cyc_yynewloc());_tmp42A[i]=_tmp944;});}}_tmp42C;});
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1070,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
({int _tmp945=Cyc_yylex(yylex_buf,& yylval,& yylloc);yychar=_tmp945;});
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 293;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7145) || Cyc_yycheck[_check_known_subscript_notnull(7146,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[yyn];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1069){
int _tmp435=0;_npop_handler(0);return _tmp435;}
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
 yyn=(int)Cyc_yydefact[yystate];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(527,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp436=yyn;switch(_tmp436){case 1: _LL255: _LL256: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1194 "parse.y"
yyval=yyyyvsp[0];
({struct Cyc_List_List*_tmp946=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_parse_result=_tmp946;});
# 1197
goto _LL254;}case 2: _LL257: _LL258: {
# 1199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1200 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp437;_push_handler(& _tmp437);{int _tmp439=0;if(setjmp(_tmp437.handler))_tmp439=1;if(!_tmp439){({struct Cyc_List_List*_tmp947=Cyc_yyget_YY16(yyyyvsp[0]);x=_tmp947;});;_pop_handler();}else{void*_tmp438=(void*)_exn_thrown;void*_tmp43A=_tmp438;void*_tmp43B;if(((struct Cyc_Core_Failure_exn_struct*)_tmp43A)->tag == Cyc_Core_Failure){_LL670: _LL671:
 x=0;goto _LL66F;}else{_LL672: _tmp43B=_tmp43A;_LL673:(int)_rethrow(_tmp43B);}_LL66F:;}};}
{struct _handler_cons _tmp43C;_push_handler(& _tmp43C);{int _tmp43E=0;if(setjmp(_tmp43C.handler))_tmp43E=1;if(!_tmp43E){({struct Cyc_List_List*_tmp948=Cyc_yyget_YY16(yyyyvsp[1]);y=_tmp948;});;_pop_handler();}else{void*_tmp43D=(void*)_exn_thrown;void*_tmp43F=_tmp43D;void*_tmp440;if(((struct Cyc_Core_Failure_exn_struct*)_tmp43F)->tag == Cyc_Core_Failure){_LL675: _LL676:
 y=0;goto _LL674;}else{_LL677: _tmp440=_tmp43F;_LL678:(int)_rethrow(_tmp440);}_LL674:;}};}
({union Cyc_YYSTYPE _tmp949=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));yyval=_tmp949;});
# 1207
goto _LL254;}case 3: _LL259: _LL25A: {
# 1209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1210 "parse.y"
({union Cyc_YYSTYPE _tmp94F=Cyc_YY16(({struct Cyc_List_List*_tmp441=_cycalloc(sizeof(*_tmp441));({struct Cyc_Absyn_Decl*_tmp94E=({struct Cyc_Absyn_Decl*_tmp442=_cycalloc(sizeof(*_tmp442));({void*_tmp94D=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp443=_cycalloc(sizeof(*_tmp443));({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp94C=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp444;_tmp444.tag=10;({struct _tuple0*_tmp94A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp444.f1=_tmp94A;});({struct Cyc_List_List*_tmp94B=Cyc_yyget_YY16(yyyyvsp[2]);_tmp444.f2=_tmp94B;});_tmp444;});_tmp443[0]=_tmp94C;});_tmp443;});_tmp442->r=_tmp94D;});_tmp442->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp442;});_tmp441->hd=_tmp94E;});_tmp441->tl=0;_tmp441;}));yyval=_tmp94F;});
Cyc_Lex_leave_using();
# 1213
goto _LL254;}case 4: _LL25B: _LL25C: {
# 1215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1214 "parse.y"
({union Cyc_YYSTYPE _tmp956=Cyc_YY16(({struct Cyc_List_List*_tmp445=_cycalloc(sizeof(*_tmp445));({struct Cyc_Absyn_Decl*_tmp954=({struct Cyc_Absyn_Decl*_tmp446=_cycalloc(sizeof(*_tmp446));({void*_tmp953=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp447=_cycalloc(sizeof(*_tmp447));({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp952=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp448;_tmp448.tag=10;({struct _tuple0*_tmp950=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp448.f1=_tmp950;});({struct Cyc_List_List*_tmp951=Cyc_yyget_YY16(yyyyvsp[2]);_tmp448.f2=_tmp951;});_tmp448;});_tmp447[0]=_tmp952;});_tmp447;});_tmp446->r=_tmp953;});_tmp446->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp446;});_tmp445->hd=_tmp954;});({struct Cyc_List_List*_tmp955=Cyc_yyget_YY16(yyyyvsp[4]);_tmp445->tl=_tmp955;});_tmp445;}));yyval=_tmp956;});
goto _LL254;}case 5: _LL25D: _LL25E: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1217 "parse.y"
({union Cyc_YYSTYPE _tmp95D=Cyc_YY16(({struct Cyc_List_List*_tmp449=_cycalloc(sizeof(*_tmp449));({struct Cyc_Absyn_Decl*_tmp95C=({struct Cyc_Absyn_Decl*_tmp44A=_cycalloc(sizeof(*_tmp44A));({void*_tmp95B=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp95A=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp44C;_tmp44C.tag=9;({struct _dyneither_ptr*_tmp958=({struct _dyneither_ptr*_tmp44D=_cycalloc(sizeof(*_tmp44D));({struct _dyneither_ptr _tmp957=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp44D[0]=_tmp957;});_tmp44D;});_tmp44C.f1=_tmp958;});({struct Cyc_List_List*_tmp959=Cyc_yyget_YY16(yyyyvsp[2]);_tmp44C.f2=_tmp959;});_tmp44C;});_tmp44B[0]=_tmp95A;});_tmp44B;});_tmp44A->r=_tmp95B;});_tmp44A->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp44A;});_tmp449->hd=_tmp95C;});_tmp449->tl=0;_tmp449;}));yyval=_tmp95D;});
Cyc_Lex_leave_namespace();
# 1220
goto _LL254;}case 6: _LL25F: _LL260: {
# 1222
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1224
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1222 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44E;_push_handler(& _tmp44E);{int _tmp450=0;if(setjmp(_tmp44E.handler))_tmp450=1;if(!_tmp450){({struct _dyneither_ptr _tmp95E=Cyc_yyget_String_tok(yyyyvsp[0]);nspace=_tmp95E;});;_pop_handler();}else{void*_tmp44F=(void*)_exn_thrown;void*_tmp451=_tmp44F;void*_tmp453;if(((struct Cyc_Core_Failure_exn_struct*)_tmp451)->tag == Cyc_Core_Failure){_LL67A: _LL67B:
({struct _dyneither_ptr _tmp95F=({const char*_tmp452="";_tag_dyneither(_tmp452,sizeof(char),1);});nspace=_tmp95F;});goto _LL679;}else{_LL67C: _tmp453=_tmp451;_LL67D:(int)_rethrow(_tmp453);}_LL679:;}};}
# 1227
{struct _handler_cons _tmp454;_push_handler(& _tmp454);{int _tmp456=0;if(setjmp(_tmp454.handler))_tmp456=1;if(!_tmp456){({struct Cyc_List_List*_tmp960=Cyc_yyget_YY16(yyyyvsp[2]);x=_tmp960;});;_pop_handler();}else{void*_tmp455=(void*)_exn_thrown;void*_tmp457=_tmp455;void*_tmp458;if(((struct Cyc_Core_Failure_exn_struct*)_tmp457)->tag == Cyc_Core_Failure){_LL67F: _LL680:
 x=0;goto _LL67E;}else{_LL681: _tmp458=_tmp457;_LL682:(int)_rethrow(_tmp458);}_LL67E:;}};}
# 1230
{struct _handler_cons _tmp459;_push_handler(& _tmp459);{int _tmp45B=0;if(setjmp(_tmp459.handler))_tmp45B=1;if(!_tmp45B){({struct Cyc_List_List*_tmp961=Cyc_yyget_YY16(yyyyvsp[4]);y=_tmp961;});;_pop_handler();}else{void*_tmp45A=(void*)_exn_thrown;void*_tmp45C=_tmp45A;void*_tmp45D;if(((struct Cyc_Core_Failure_exn_struct*)_tmp45C)->tag == Cyc_Core_Failure){_LL684: _LL685:
 y=0;goto _LL683;}else{_LL686: _tmp45D=_tmp45C;_LL687:(int)_rethrow(_tmp45D);}_LL683:;}};}
# 1233
({union Cyc_YYSTYPE _tmp966=Cyc_YY16(({struct Cyc_List_List*_tmp45E=_cycalloc(sizeof(*_tmp45E));({struct Cyc_Absyn_Decl*_tmp965=({struct Cyc_Absyn_Decl*_tmp45F=_cycalloc(sizeof(*_tmp45F));({void*_tmp964=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp460=_cycalloc(sizeof(*_tmp460));({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp963=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp461;_tmp461.tag=9;({struct _dyneither_ptr*_tmp962=({struct _dyneither_ptr*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462[0]=nspace;_tmp462;});_tmp461.f1=_tmp962;});_tmp461.f2=x;_tmp461;});_tmp460[0]=_tmp963;});_tmp460;});_tmp45F->r=_tmp964;});_tmp45F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp45F;});_tmp45E->hd=_tmp965;});_tmp45E->tl=y;_tmp45E;}));yyval=_tmp966;});
# 1235
goto _LL254;}case 7: _LL261: _LL262: {
# 1237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1236 "parse.y"
int _tmp463=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp463)
({union Cyc_YYSTYPE _tmp96C=Cyc_YY16(({struct Cyc_List_List*_tmp464=_cycalloc(sizeof(*_tmp464));({struct Cyc_Absyn_Decl*_tmp96A=({struct Cyc_Absyn_Decl*_tmp465=_cycalloc(sizeof(*_tmp465));({void*_tmp969=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp466=_cycalloc(sizeof(*_tmp466));({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp968=({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp467;_tmp467.tag=11;({struct Cyc_List_List*_tmp967=Cyc_yyget_YY16(yyyyvsp[2]);_tmp467.f1=_tmp967;});_tmp467;});_tmp466[0]=_tmp968;});_tmp466;});_tmp465->r=_tmp969;});_tmp465->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp465;});_tmp464->hd=_tmp96A;});({struct Cyc_List_List*_tmp96B=Cyc_yyget_YY16(yyyyvsp[4]);_tmp464->tl=_tmp96B;});_tmp464;}));yyval=_tmp96C;});else{
# 1240
({union Cyc_YYSTYPE _tmp972=Cyc_YY16(({struct Cyc_List_List*_tmp468=_cycalloc(sizeof(*_tmp468));({struct Cyc_Absyn_Decl*_tmp970=({struct Cyc_Absyn_Decl*_tmp469=_cycalloc(sizeof(*_tmp469));({void*_tmp96F=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp96E=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp46B;_tmp46B.tag=12;({struct Cyc_List_List*_tmp96D=Cyc_yyget_YY16(yyyyvsp[2]);_tmp46B.f1=_tmp96D;});_tmp46B.f2=0;_tmp46B;});_tmp46A[0]=_tmp96E;});_tmp46A;});_tmp469->r=_tmp96F;});_tmp469->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp469;});_tmp468->hd=_tmp970;});({struct Cyc_List_List*_tmp971=Cyc_yyget_YY16(yyyyvsp[4]);_tmp468->tl=_tmp971;});_tmp468;}));yyval=_tmp972;});}
# 1242
goto _LL254;}case 8: _LL263: _LL264: {
# 1244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 1246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 1243 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp46C=0;({unsigned int _tmp974=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp973=({const char*_tmp46D="expecting \"C include\"";_tag_dyneither(_tmp46D,sizeof(char),22);});Cyc_Warn_err(_tmp974,_tmp973,_tag_dyneither(_tmp46C,sizeof(void*),0));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
({union Cyc_YYSTYPE _tmp97A=Cyc_YY16(({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct Cyc_Absyn_Decl*_tmp978=({struct Cyc_Absyn_Decl*_tmp46F=_cycalloc(sizeof(*_tmp46F));({void*_tmp977=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp470=_cycalloc(sizeof(*_tmp470));({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp976=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp471;_tmp471.tag=12;({struct Cyc_List_List*_tmp975=Cyc_yyget_YY16(yyyyvsp[2]);_tmp471.f1=_tmp975;});_tmp471.f2=exs;_tmp471;});_tmp470[0]=_tmp976;});_tmp470;});_tmp46F->r=_tmp977;});_tmp46F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp46F;});_tmp46E->hd=_tmp978;});({struct Cyc_List_List*_tmp979=Cyc_yyget_YY16(yyyyvsp[5]);_tmp46E->tl=_tmp979;});_tmp46E;}));yyval=_tmp97A;});
# 1248
goto _LL254;};}case 9: _LL265: _LL266: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1249 "parse.y"
({union Cyc_YYSTYPE _tmp97D=Cyc_YY16(({struct Cyc_List_List*_tmp472=_cycalloc(sizeof(*_tmp472));({struct Cyc_Absyn_Decl*_tmp97B=({struct Cyc_Absyn_Decl*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp473->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp473;});_tmp472->hd=_tmp97B;});({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY16(yyyyvsp[2]);_tmp472->tl=_tmp97C;});_tmp472;}));yyval=_tmp97D;});
goto _LL254;}case 10: _LL267: _LL268: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1251 "parse.y"
({union Cyc_YYSTYPE _tmp980=Cyc_YY16(({struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*_tmp474));({struct Cyc_Absyn_Decl*_tmp97E=({struct Cyc_Absyn_Decl*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->r=(void*)& Cyc_Absyn_Portoff_d_val;_tmp475->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp475;});_tmp474->hd=_tmp97E;});({struct Cyc_List_List*_tmp97F=Cyc_yyget_YY16(yyyyvsp[2]);_tmp474->tl=_tmp97F;});_tmp474;}));yyval=_tmp980;});
goto _LL254;}case 11: _LL269: _LL26A:
# 1254
({union Cyc_YYSTYPE _tmp981=Cyc_YY16(0);yyval=_tmp981;});
goto _LL254;case 12: _LL26B: _LL26C: {
# 1257
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1259
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1257 "parse.y"
struct _dyneither_ptr _tmp476=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({({struct _dyneither_ptr _tmp982=(struct _dyneither_ptr)_tmp476;Cyc_strcmp(_tmp982,({const char*_tmp477="C";_tag_dyneither(_tmp477,sizeof(char),2);}));})== 0;}))
({union Cyc_YYSTYPE _tmp983=Cyc_YY31(0);yyval=_tmp983;});else{
if(({({struct _dyneither_ptr _tmp984=(struct _dyneither_ptr)_tmp476;Cyc_strcmp(_tmp984,({const char*_tmp478="C include";_tag_dyneither(_tmp478,sizeof(char),10);}));})== 0;}))
({union Cyc_YYSTYPE _tmp985=Cyc_YY31(1);yyval=_tmp985;});else{
# 1264
({void*_tmp479=0;({unsigned int _tmp987=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp986=({const char*_tmp47A="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp47A,sizeof(char),29);});Cyc_Warn_err(_tmp987,_tmp986,_tag_dyneither(_tmp479,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmp988=Cyc_YY31(1);yyval=_tmp988;});}}
# 1268
goto _LL254;}case 13: _LL26D: _LL26E: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1271 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL254;}case 14: _LL26F: _LL270: {
# 1274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1275 "parse.y"
yyval=yyyyvsp[2];
goto _LL254;}case 15: _LL271: _LL272: {
# 1278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1276 "parse.y"
({union Cyc_YYSTYPE _tmp989=Cyc_YY52(0);yyval=_tmp989;});
goto _LL254;}case 16: _LL273: _LL274: {
# 1279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1280 "parse.y"
({union Cyc_YYSTYPE _tmp98C=Cyc_YY52(({struct Cyc_List_List*_tmp47B=_cycalloc(sizeof(*_tmp47B));({struct _tuple27*_tmp98B=({struct _tuple27*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->f1=(unsigned int)(yyyylsp[0]).first_line;({struct _tuple0*_tmp98A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47C->f2=_tmp98A;});_tmp47C->f3=0;_tmp47C;});_tmp47B->hd=_tmp98B;});_tmp47B->tl=0;_tmp47B;}));yyval=_tmp98C;});
goto _LL254;}case 17: _LL275: _LL276: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1281 "parse.y"
({union Cyc_YYSTYPE _tmp98F=Cyc_YY52(({struct Cyc_List_List*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct _tuple27*_tmp98E=({struct _tuple27*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->f1=(unsigned int)(yyyylsp[0]).first_line;({struct _tuple0*_tmp98D=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47E->f2=_tmp98D;});_tmp47E->f3=0;_tmp47E;});_tmp47D->hd=_tmp98E;});_tmp47D->tl=0;_tmp47D;}));yyval=_tmp98F;});
goto _LL254;}case 18: _LL277: _LL278: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1283 "parse.y"
({union Cyc_YYSTYPE _tmp993=Cyc_YY52(({struct Cyc_List_List*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct _tuple27*_tmp991=({struct _tuple27*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->f1=(unsigned int)(yyyylsp[0]).first_line;({struct _tuple0*_tmp990=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp480->f2=_tmp990;});_tmp480->f3=0;_tmp480;});_tmp47F->hd=_tmp991;});({struct Cyc_List_List*_tmp992=Cyc_yyget_YY52(yyyyvsp[2]);_tmp47F->tl=_tmp992;});_tmp47F;}));yyval=_tmp993;});
goto _LL254;}case 19: _LL279: _LL27A: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1287 "parse.y"
({union Cyc_YYSTYPE _tmp998=Cyc_YY16(({struct Cyc_List_List*_tmp481=_cycalloc(sizeof(*_tmp481));({struct Cyc_Absyn_Decl*_tmp997=({void*_tmp996=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp482=_cycalloc(sizeof(*_tmp482));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp995=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp483;_tmp483.tag=1;({struct Cyc_Absyn_Fndecl*_tmp994=Cyc_yyget_YY15(yyyyvsp[0]);_tmp483.f1=_tmp994;});_tmp483;});_tmp482[0]=_tmp995;});_tmp482;});Cyc_Absyn_new_decl(_tmp996,(unsigned int)(yyyylsp[0]).first_line);});_tmp481->hd=_tmp997;});_tmp481->tl=0;_tmp481;}));yyval=_tmp998;});
goto _LL254;}case 20: _LL27B: _LL27C: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1288 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 21: _LL27D: _LL27E: {
# 1291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1289 "parse.y"
({union Cyc_YYSTYPE _tmp999=Cyc_YY16(0);yyval=_tmp999;});
goto _LL254;}case 24: _LL27F: _LL280: {
# 1292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1298 "parse.y"
({union Cyc_YYSTYPE _tmp99D=Cyc_YY15(({struct _RegionHandle*_tmp99C=yyr;struct Cyc_Parse_Declarator _tmp99B=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp99A=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp99C,0,_tmp99B,0,_tmp99A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp99D;});
goto _LL254;}case 25: _LL281: _LL282: {
# 1301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1300 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp484=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9A1=Cyc_YY15(({struct _RegionHandle*_tmp9A0=yyr;struct Cyc_Parse_Declarator _tmp99F=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp99E=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp9A0,& _tmp484,_tmp99F,0,_tmp99E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A1;});
goto _LL254;}case 26: _LL283: _LL284: {
# 1304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1313 "parse.y"
({union Cyc_YYSTYPE _tmp9A6=Cyc_YY15(({struct _RegionHandle*_tmp9A5=yyr;struct Cyc_Parse_Declarator _tmp9A4=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp9A3=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp9A2=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp9A5,0,_tmp9A4,_tmp9A3,_tmp9A2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A6;});
goto _LL254;}case 27: _LL285: _LL286: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1315 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp485=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9AB=Cyc_YY15(({struct _RegionHandle*_tmp9AA=yyr;struct Cyc_Parse_Declarator _tmp9A9=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp9A8=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp9A7=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp9AA,& _tmp485,_tmp9A9,_tmp9A8,_tmp9A7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9AB;});
goto _LL254;}case 28: _LL287: _LL288: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1323 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp486=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9AF=Cyc_YY15(({struct _RegionHandle*_tmp9AE=yyr;struct Cyc_Parse_Declarator _tmp9AD=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp9AC=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp9AE,& _tmp486,_tmp9AD,0,_tmp9AC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9AF;});
goto _LL254;}case 29: _LL289: _LL28A: {
# 1327
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1326 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp487=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9B4=Cyc_YY15(({struct _RegionHandle*_tmp9B3=yyr;struct Cyc_Parse_Declarator _tmp9B2=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp9B1=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp9B0=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp9B3,& _tmp487,_tmp9B2,_tmp9B1,_tmp9B0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9B4;});
goto _LL254;}case 30: _LL28B: _LL28C: {
# 1330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1331 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL254;}case 31: _LL28D: _LL28E: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1334 "parse.y"
Cyc_Lex_leave_using();
goto _LL254;}case 32: _LL28F: _LL290: {
# 1337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1337 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp488=_cycalloc(sizeof(*_tmp488));({struct _dyneither_ptr _tmp9B5=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp488[0]=_tmp9B5;});_tmp488;}));yyval=yyyyvsp[1];
goto _LL254;}case 33: _LL291: _LL292: {
# 1340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1340 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL254;}case 34: _LL293: _LL294: {
# 1343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
int _tmp489=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp9B8=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp9B7=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp9B6=(unsigned int)_tmp489;Cyc_Parse_make_declarations(_tmp9B7,0,_tmp9B6,(unsigned int)_tmp489);}));yyval=_tmp9B8;});
goto _LL254;}case 35: _LL295: _LL296: {
# 1350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1349 "parse.y"
int _tmp48A=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp9BC=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp9BB=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp9BA=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp9B9=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp9BB,_tmp9BA,_tmp9B9,(unsigned int)_tmp48A);}));yyval=_tmp9BC;});
goto _LL254;}case 36: _LL297: _LL298: {
# 1353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1353 "parse.y"
({union Cyc_YYSTYPE _tmp9C0=Cyc_YY16(({struct Cyc_List_List*_tmp48B=_cycalloc(sizeof(*_tmp48B));({struct Cyc_Absyn_Decl*_tmp9BF=({struct Cyc_Absyn_Pat*_tmp9BE=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp9BD=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp9BE,_tmp9BD,(unsigned int)(yyyylsp[0]).first_line);});_tmp48B->hd=_tmp9BF;});_tmp48B->tl=0;_tmp48B;}));yyval=_tmp9C0;});
goto _LL254;}case 37: _LL299: _LL29A: {
# 1356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1355 "parse.y"
struct Cyc_List_List*_tmp48C=0;
{struct Cyc_List_List*_tmp48D=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp48D != 0;_tmp48D=_tmp48D->tl){
struct _dyneither_ptr*_tmp48E=(struct _dyneither_ptr*)_tmp48D->hd;
struct _tuple0*qv=({struct _tuple0*_tmp491=_cycalloc(sizeof(*_tmp491));({union Cyc_Absyn_Nmspace _tmp9C1=Cyc_Absyn_Rel_n(0);_tmp491->f1=_tmp9C1;});_tmp491->f2=_tmp48E;_tmp491;});
struct Cyc_Absyn_Vardecl*_tmp48F=({struct _tuple0*_tmp9C2=qv;Cyc_Absyn_new_vardecl(0,_tmp9C2,Cyc_Absyn_wildtyp(0),0);});
({struct Cyc_List_List*_tmp9C3=({struct Cyc_List_List*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->hd=_tmp48F;_tmp490->tl=_tmp48C;_tmp490;});_tmp48C=_tmp9C3;});}}
# 1362
({struct Cyc_List_List*_tmp9C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp48C);_tmp48C=_tmp9C4;});
({union Cyc_YYSTYPE _tmp9C6=Cyc_YY16(({struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));({struct Cyc_Absyn_Decl*_tmp9C5=Cyc_Absyn_letv_decl(_tmp48C,(unsigned int)(yyyylsp[0]).first_line);_tmp492->hd=_tmp9C5;});_tmp492->tl=0;_tmp492;}));yyval=_tmp9C6;});
# 1365
goto _LL254;}case 38: _LL29B: _LL29C: {
# 1367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 1369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 1368 "parse.y"
struct _dyneither_ptr _tmp493=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1370
if(({({struct _dyneither_ptr _tmp9C7=(struct _dyneither_ptr)_tmp493;Cyc_zstrcmp(_tmp9C7,({const char*_tmp494="`H";_tag_dyneither(_tmp494,sizeof(char),3);}));})== 0;}))
({void*_tmp495=0;({unsigned int _tmp9C9=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp9C8=({const char*_tmp496="bad occurrence of heap region";_tag_dyneither(_tmp496,sizeof(char),30);});Cyc_Warn_err(_tmp9C9,_tmp9C8,_tag_dyneither(_tmp495,sizeof(void*),0));});});
if(({({struct _dyneither_ptr _tmp9CA=(struct _dyneither_ptr)_tmp493;Cyc_zstrcmp(_tmp9CA,({const char*_tmp497="`U";_tag_dyneither(_tmp497,sizeof(char),3);}));})== 0;}))
({void*_tmp498=0;({unsigned int _tmp9CC=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp9CB=({const char*_tmp499="bad occurrence of unique region";_tag_dyneither(_tmp499,sizeof(char),32);});Cyc_Warn_err(_tmp9CC,_tmp9CB,_tag_dyneither(_tmp498,sizeof(void*),0));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));({struct _dyneither_ptr*_tmp9CD=({struct _dyneither_ptr*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=_tmp493;_tmp4A2;});_tmp4A1->name=_tmp9CD;});_tmp4A1->identity=- 1;({void*_tmp9CE=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4A1->kind=_tmp9CE;});_tmp4A1;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49F=_cycalloc(sizeof(*_tmp49F));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp9CF=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A0;_tmp4A0.tag=2;_tmp4A0.f1=tv;_tmp4A0;});_tmp49F[0]=_tmp9CF;});_tmp49F;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp9D4=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp9D3=({struct _tuple0*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp9D1=({struct _dyneither_ptr*_tmp49C=_cycalloc(sizeof(*_tmp49C));({struct _dyneither_ptr _tmp9D0=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp49C[0]=_tmp9D0;});_tmp49C;});_tmp49B->f2=_tmp9D1;});_tmp49B;});Cyc_Absyn_new_vardecl(_tmp9D4,_tmp9D3,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9D2=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp49E;_tmp49E.tag=15;_tmp49E.f1=t;_tmp49E;});_tmp49D[0]=_tmp9D2;});_tmp49D;}),0);});
({union Cyc_YYSTYPE _tmp9D6=Cyc_YY16(({struct Cyc_List_List*_tmp49A=_cycalloc(sizeof(*_tmp49A));({struct Cyc_Absyn_Decl*_tmp9D5=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp49A->hd=_tmp9D5;});_tmp49A->tl=0;_tmp49A;}));yyval=_tmp9D6;});
# 1379
goto _LL254;};}case 39: _LL29D: _LL29E: {
# 1381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1381 "parse.y"
struct _dyneither_ptr _tmp4A3=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({({struct _dyneither_ptr _tmp9D7=(struct _dyneither_ptr)_tmp4A3;Cyc_zstrcmp(_tmp9D7,({const char*_tmp4A4="H";_tag_dyneither(_tmp4A4,sizeof(char),2);}));})== 0;}))
({void*_tmp4A5=0;({unsigned int _tmp9D9=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9D8=({const char*_tmp4A6="bad occurrence of heap region `H";_tag_dyneither(_tmp4A6,sizeof(char),33);});Cyc_Warn_err(_tmp9D9,_tmp9D8,_tag_dyneither(_tmp4A5,sizeof(void*),0));});});
if(({({struct _dyneither_ptr _tmp9DA=(struct _dyneither_ptr)_tmp4A3;Cyc_zstrcmp(_tmp9DA,({const char*_tmp4A7="U";_tag_dyneither(_tmp4A7,sizeof(char),2);}));})== 0;}))
({void*_tmp4A8=0;({unsigned int _tmp9DC=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9DB=({const char*_tmp4A9="bad occurrence of unique region `U";_tag_dyneither(_tmp4A9,sizeof(char),35);});Cyc_Warn_err(_tmp9DC,_tmp9DB,_tag_dyneither(_tmp4A8,sizeof(void*),0));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct _dyneither_ptr*_tmp9DF=({struct _dyneither_ptr*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({struct _dyneither_ptr _tmp9DE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4B5;_tmp4B5.tag=0;_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A3);({void*_tmp4B3[1]={& _tmp4B5};({struct _dyneither_ptr _tmp9DD=({const char*_tmp4B4="`%s";_tag_dyneither(_tmp4B4,sizeof(char),4);});Cyc_aprintf(_tmp9DD,_tag_dyneither(_tmp4B3,sizeof(void*),1));});});});_tmp4B2[0]=_tmp9DE;});_tmp4B2;});_tmp4B1->name=_tmp9DF;});_tmp4B1->identity=- 1;({void*_tmp9E0=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4B1->kind=_tmp9E0;});_tmp4B1;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp9E1=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4B0;_tmp4B0.tag=2;_tmp4B0.f1=tv;_tmp4B0;});_tmp4AF[0]=_tmp9E1;});_tmp4AF;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp9E5=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp9E4=({struct _tuple0*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp9E2=({struct _dyneither_ptr*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=_tmp4A3;_tmp4AC;});_tmp4AB->f2=_tmp9E2;});_tmp4AB;});Cyc_Absyn_new_vardecl(_tmp9E5,_tmp9E4,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9E3=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4AE;_tmp4AE.tag=15;_tmp4AE.f1=t;_tmp4AE;});_tmp4AD[0]=_tmp9E3;});_tmp4AD;}),0);});
({union Cyc_YYSTYPE _tmp9E7=Cyc_YY16(({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({struct Cyc_Absyn_Decl*_tmp9E6=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp4AA->hd=_tmp9E6;});_tmp4AA->tl=0;_tmp4AA;}));yyval=_tmp9E7;});
# 1392
goto _LL254;};}case 40: _LL29F: _LL2A0: {
# 1394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 1396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 1394 "parse.y"
struct _dyneither_ptr _tmp4B6=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp4B7=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp4B8=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({({struct _dyneither_ptr _tmp9E8=(struct _dyneither_ptr)_tmp4B7;Cyc_strcmp(_tmp9E8,({const char*_tmp4B9="open";_tag_dyneither(_tmp4B9,sizeof(char),5);}));})!= 0;}))({void*_tmp4BA=0;({unsigned int _tmp9EA=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp9E9=({const char*_tmp4BB="expecting `open'";_tag_dyneither(_tmp4BB,sizeof(char),17);});Cyc_Warn_err(_tmp9EA,_tmp9E9,_tag_dyneither(_tmp4BA,sizeof(void*),0));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));({struct _dyneither_ptr*_tmp9ED=({struct _dyneither_ptr*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));({struct _dyneither_ptr _tmp9EC=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4C7;_tmp4C7.tag=0;_tmp4C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4B6);({void*_tmp4C5[1]={& _tmp4C7};({struct _dyneither_ptr _tmp9EB=({const char*_tmp4C6="`%s";_tag_dyneither(_tmp4C6,sizeof(char),4);});Cyc_aprintf(_tmp9EB,_tag_dyneither(_tmp4C5,sizeof(void*),1));});});});_tmp4C4[0]=_tmp9EC;});_tmp4C4;});_tmp4C3->name=_tmp9ED;});_tmp4C3->identity=- 1;({void*_tmp9EE=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4C3->kind=_tmp9EE;});_tmp4C3;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp9EF=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4C2;_tmp4C2.tag=2;_tmp4C2.f1=tv;_tmp4C2;});_tmp4C1[0]=_tmp9EF;});_tmp4C1;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp9F3=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp9F2=({struct _tuple0*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp9F0=({struct _dyneither_ptr*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=_tmp4B6;_tmp4BE;});_tmp4BD->f2=_tmp9F0;});_tmp4BD;});Cyc_Absyn_new_vardecl(_tmp9F3,_tmp9F2,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9F1=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4C0;_tmp4C0.tag=15;_tmp4C0.f1=t;_tmp4C0;});_tmp4BF[0]=_tmp9F1;});_tmp4BF;}),0);});
({union Cyc_YYSTYPE _tmp9F5=Cyc_YY16(({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({struct Cyc_Absyn_Decl*_tmp9F4=Cyc_Absyn_region_decl(tv,vd,_tmp4B8,(unsigned int)(yyyylsp[0]).first_line);_tmp4BC->hd=_tmp9F4;});_tmp4BC->tl=0;_tmp4BC;}));yyval=_tmp9F5;});
# 1404
goto _LL254;};}case 41: _LL2A1: _LL2A2: {
# 1406
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1408
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1408 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 42: _LL2A3: _LL2A4: {
# 1411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1410 "parse.y"
({union Cyc_YYSTYPE _tmp9F7=Cyc_YY16(({struct Cyc_List_List*_tmp9F6=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F6,Cyc_yyget_YY16(yyyyvsp[1]));}));yyval=_tmp9F7;});
goto _LL254;}case 43: _LL2A5: _LL2A6: {
# 1413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmp9FB=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C8;({enum Cyc_Parse_Storage_class*_tmp9F8=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4C8.sc=_tmp9F8;});({struct Cyc_Absyn_Tqual _tmp9F9=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4C8.tq=_tmp9F9;});({struct Cyc_Parse_Type_specifier _tmp9FA=
Cyc_Parse_empty_spec(0);_tmp4C8.type_specs=_tmp9FA;});_tmp4C8.is_inline=0;_tmp4C8.attributes=0;_tmp4C8;}));
# 1416
yyval=_tmp9FB;});
# 1418
goto _LL254;}case 44: _LL2A7: _LL2A8: {
# 1420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1419 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C9=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp4C9.sc != 0)
({void*_tmp4CA=0;({unsigned int _tmp9FD=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9FC=({const char*_tmp4CB="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp4CB,sizeof(char),73);});Cyc_Warn_warn(_tmp9FD,_tmp9FC,_tag_dyneither(_tmp4CA,sizeof(void*),0));});});
# 1423
({union Cyc_YYSTYPE _tmp9FF=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CC;({enum Cyc_Parse_Storage_class*_tmp9FE=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4CC.sc=_tmp9FE;});_tmp4CC.tq=_tmp4C9.tq;_tmp4CC.type_specs=_tmp4C9.type_specs;_tmp4CC.is_inline=_tmp4C9.is_inline;_tmp4CC.attributes=_tmp4C9.attributes;_tmp4CC;}));yyval=_tmp9FF;});
# 1427
goto _LL254;}case 45: _LL2A9: _LL2AA: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1428 "parse.y"
({union Cyc_YYSTYPE _tmpA02=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CD;_tmp4CD.sc=0;({struct Cyc_Absyn_Tqual _tmpA00=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4CD.tq=_tmpA00;});({struct Cyc_Parse_Type_specifier _tmpA01=
Cyc_yyget_YY21(yyyyvsp[0]);_tmp4CD.type_specs=_tmpA01;});_tmp4CD.is_inline=0;_tmp4CD.attributes=0;_tmp4CD;}));
# 1428
yyval=_tmpA02;});
# 1430
goto _LL254;}case 46: _LL2AB: _LL2AC: {
# 1432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1431 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CE=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA06=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CF;_tmp4CF.sc=_tmp4CE.sc;_tmp4CF.tq=_tmp4CE.tq;({struct Cyc_Parse_Type_specifier _tmpA05=({
unsigned int _tmpA04=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmpA03=_tmp4CE.type_specs;Cyc_Parse_combine_specifiers(_tmpA04,_tmpA03,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp4CF.type_specs=_tmpA05;});_tmp4CF.is_inline=_tmp4CE.is_inline;_tmp4CF.attributes=_tmp4CE.attributes;_tmp4CF;}));
# 1432
yyval=_tmpA06;});
# 1437
goto _LL254;}case 47: _LL2AD: _LL2AE: {
# 1439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1438 "parse.y"
({union Cyc_YYSTYPE _tmpA09=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D0;_tmp4D0.sc=0;({struct Cyc_Absyn_Tqual _tmpA07=Cyc_yyget_YY23(yyyyvsp[0]);_tmp4D0.tq=_tmpA07;});({struct Cyc_Parse_Type_specifier _tmpA08=Cyc_Parse_empty_spec(0);_tmp4D0.type_specs=_tmpA08;});_tmp4D0.is_inline=0;_tmp4D0.attributes=0;_tmp4D0;}));yyval=_tmpA09;});
goto _LL254;}case 48: _LL2AF: _LL2B0: {
# 1441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1440 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D1=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA0C=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D2;_tmp4D2.sc=_tmp4D1.sc;({struct Cyc_Absyn_Tqual _tmpA0B=({struct Cyc_Absyn_Tqual _tmpA0A=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpA0A,_tmp4D1.tq);});_tmp4D2.tq=_tmpA0B;});_tmp4D2.type_specs=_tmp4D1.type_specs;_tmp4D2.is_inline=_tmp4D1.is_inline;_tmp4D2.attributes=_tmp4D1.attributes;_tmp4D2;}));yyval=_tmpA0C;});
# 1445
goto _LL254;}case 49: _LL2B1: _LL2B2: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1446 "parse.y"
({union Cyc_YYSTYPE _tmpA0F=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D3;_tmp4D3.sc=0;({struct Cyc_Absyn_Tqual _tmpA0D=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4D3.tq=_tmpA0D;});({struct Cyc_Parse_Type_specifier _tmpA0E=
Cyc_Parse_empty_spec(0);_tmp4D3.type_specs=_tmpA0E;});_tmp4D3.is_inline=1;_tmp4D3.attributes=0;_tmp4D3;}));
# 1446
yyval=_tmpA0F;});
# 1448
goto _LL254;}case 50: _LL2B3: _LL2B4: {
# 1450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1449 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D4=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA10=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D5;_tmp4D5.sc=_tmp4D4.sc;_tmp4D5.tq=_tmp4D4.tq;_tmp4D5.type_specs=_tmp4D4.type_specs;_tmp4D5.is_inline=1;_tmp4D5.attributes=_tmp4D4.attributes;_tmp4D5;}));yyval=_tmpA10;});
# 1453
goto _LL254;}case 51: _LL2B5: _LL2B6: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1454 "parse.y"
({union Cyc_YYSTYPE _tmpA14=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D6;_tmp4D6.sc=0;({struct Cyc_Absyn_Tqual _tmpA11=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4D6.tq=_tmpA11;});({struct Cyc_Parse_Type_specifier _tmpA12=
Cyc_Parse_empty_spec(0);_tmp4D6.type_specs=_tmpA12;});_tmp4D6.is_inline=0;({struct Cyc_List_List*_tmpA13=Cyc_yyget_YY45(yyyyvsp[0]);_tmp4D6.attributes=_tmpA13;});_tmp4D6;}));
# 1454
yyval=_tmpA14;});
# 1456
goto _LL254;}case 52: _LL2B7: _LL2B8: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1457 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D7=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA17=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D8;_tmp4D8.sc=_tmp4D7.sc;_tmp4D8.tq=_tmp4D7.tq;_tmp4D8.type_specs=_tmp4D7.type_specs;_tmp4D8.is_inline=_tmp4D7.is_inline;({struct Cyc_List_List*_tmpA16=({
# 1460
struct Cyc_List_List*_tmpA15=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA15,_tmp4D7.attributes);});_tmp4D8.attributes=_tmpA16;});_tmp4D8;}));
# 1458
yyval=_tmpA17;});
# 1461
goto _LL254;}case 53: _LL2B9: _LL2BA: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1464 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
({union Cyc_YYSTYPE _tmpA18=Cyc_YY20(& auto_sc);yyval=_tmpA18;});
goto _LL254;}case 54: _LL2BB: _LL2BC: {
# 1468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1466 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
({union Cyc_YYSTYPE _tmpA19=Cyc_YY20(& register_sc);yyval=_tmpA19;});
goto _LL254;}case 55: _LL2BD: _LL2BE: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1468 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
({union Cyc_YYSTYPE _tmpA1A=Cyc_YY20(& static_sc);yyval=_tmpA1A;});
goto _LL254;}case 56: _LL2BF: _LL2C0: {
# 1472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1470 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
({union Cyc_YYSTYPE _tmpA1B=Cyc_YY20(& extern_sc);yyval=_tmpA1B;});
goto _LL254;}case 57: _LL2C1: _LL2C2: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1473 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({({struct _dyneither_ptr _tmpA1C=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpA1C,({const char*_tmp4D9="C";_tag_dyneither(_tmp4D9,sizeof(char),2);}));})!= 0;}))
({void*_tmp4DA=0;({unsigned int _tmpA1E=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA1D=({const char*_tmp4DB="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp4DB,sizeof(char),37);});Cyc_Warn_err(_tmpA1E,_tmpA1D,_tag_dyneither(_tmp4DA,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpA1F=Cyc_YY20(& externC_sc);yyval=_tmpA1F;});
# 1478
goto _LL254;}case 58: _LL2C3: _LL2C4: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1478 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
({union Cyc_YYSTYPE _tmpA20=Cyc_YY20(& typedef_sc);yyval=_tmpA20;});
goto _LL254;}case 59: _LL2C5: _LL2C6: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1481 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
({union Cyc_YYSTYPE _tmpA21=Cyc_YY20(& abstract_sc);yyval=_tmpA21;});
goto _LL254;}case 60: _LL2C7: _LL2C8:
# 1485
({union Cyc_YYSTYPE _tmpA22=Cyc_YY45(0);yyval=_tmpA22;});
goto _LL254;case 61: _LL2C9: _LL2CA: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1488 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 62: _LL2CB: _LL2CC: {
# 1491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 1493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 1493 "parse.y"
yyval=yyyyvsp[3];
goto _LL254;}case 63: _LL2CD: _LL2CE: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1497 "parse.y"
({union Cyc_YYSTYPE _tmpA24=Cyc_YY45(({struct Cyc_List_List*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));({void*_tmpA23=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4DC->hd=_tmpA23;});_tmp4DC->tl=0;_tmp4DC;}));yyval=_tmpA24;});
goto _LL254;}case 64: _LL2CF: _LL2D0: {
# 1500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1498 "parse.y"
({union Cyc_YYSTYPE _tmpA27=Cyc_YY45(({struct Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));({void*_tmpA25=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4DD->hd=_tmpA25;});({struct Cyc_List_List*_tmpA26=Cyc_yyget_YY45(yyyyvsp[2]);_tmp4DD->tl=_tmpA26;});_tmp4DD;}));yyval=_tmpA27;});
goto _LL254;}case 65: _LL2D1: _LL2D2: {
# 1501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1503 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[18]={{{_tmp4E1,_tmp4E1,_tmp4E1 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 8},(void*)& att_aligned},{{_tmp4E7,_tmp4E7,_tmp4E7 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 5},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 14},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1524
struct _dyneither_ptr _tmp4DE=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1526
if((((_get_dyneither_size(_tmp4DE,sizeof(char))> 4  && ((const char*)_tmp4DE.curr)[0]== '_') && ((const char*)_tmp4DE.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4DE,sizeof(char),(int)(_get_dyneither_size(_tmp4DE,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DE,sizeof(char),(int)(_get_dyneither_size(_tmp4DE,sizeof(char))- 3)))== '_')
# 1528
({struct _dyneither_ptr _tmpA28=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4DE,2,_get_dyneither_size(_tmp4DE,sizeof(char))- 5);_tmp4DE=_tmpA28;});{
int i=0;
for(0;i < 18;++ i){
if(({Cyc_strcmp((struct _dyneither_ptr)_tmp4DE,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18,i)]).f1)== 0;})){
({union Cyc_YYSTYPE _tmpA29=Cyc_YY46((att_map[i]).f2);yyval=_tmpA29;});
break;}}
# 1535
if(i == 18){
({void*_tmp4DF=0;({unsigned int _tmpA2B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA2A=({const char*_tmp4E0="unrecognized attribute";_tag_dyneither(_tmp4E0,sizeof(char),23);});Cyc_Warn_err(_tmpA2B,_tmpA2A,_tag_dyneither(_tmp4DF,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpA2C=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);yyval=_tmpA2C;});}
# 1540
goto _LL254;};}case 66: _LL2D3: _LL2D4: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1540 "parse.y"
({union Cyc_YYSTYPE _tmpA2D=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);yyval=_tmpA2D;});
goto _LL254;}case 67: _LL2D5: _LL2D6: {
# 1543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1542 "parse.y"
struct _dyneither_ptr _tmp4F4=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp4F5=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({({struct _dyneither_ptr _tmpA2F=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA2F,({const char*_tmp4F6="aligned";_tag_dyneither(_tmp4F6,sizeof(char),8);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA2E=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA2E,({const char*_tmp4F7="__aligned__";_tag_dyneither(_tmp4F7,sizeof(char),12);}));})== 0;}))
({void*_tmpA31=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA30=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp4F9;_tmp4F9.tag=6;_tmp4F9.f1=_tmp4F5;_tmp4F9;});_tmp4F8[0]=_tmpA30;});_tmp4F8;});a=_tmpA31;});else{
if(({({struct _dyneither_ptr _tmpA33=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA33,({const char*_tmp4FA="section";_tag_dyneither(_tmp4FA,sizeof(char),8);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA32=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA32,({const char*_tmp4FB="__section__";_tag_dyneither(_tmp4FB,sizeof(char),12);}));})== 0;})){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
({void*_tmpA35=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpA34=({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmp4FD;_tmp4FD.tag=8;_tmp4FD.f1=str;_tmp4FD;});_tmp4FC[0]=_tmpA34;});_tmp4FC;});a=_tmpA35;});}else{
if(({({struct _dyneither_ptr _tmpA36=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA36,({const char*_tmp4FE="__mode__";_tag_dyneither(_tmp4FE,sizeof(char),9);}));})== 0;})){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
({void*_tmpA38=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpA37=({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmp500;_tmp500.tag=24;_tmp500.f1=str;_tmp500;});_tmp4FF[0]=_tmpA37;});_tmp4FF;});a=_tmpA38;});}else{
if(({({struct _dyneither_ptr _tmpA39=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA39,({const char*_tmp501="alias";_tag_dyneither(_tmp501,sizeof(char),6);}));})== 0;})){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
({void*_tmpA3B=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp502=_cycalloc(sizeof(*_tmp502));({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmpA3A=({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmp503;_tmp503.tag=25;_tmp503.f1=str;_tmp503;});_tmp502[0]=_tmpA3A;});_tmp502;});a=_tmpA3B;});}else{
# 1558
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp4F5);
if(({({struct _dyneither_ptr _tmpA3D=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA3D,({const char*_tmp504="regparm";_tag_dyneither(_tmp504,sizeof(char),8);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA3C=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA3C,({const char*_tmp505="__regparm__";_tag_dyneither(_tmp505,sizeof(char),12);}));})== 0;})){
if(n < 0  || n > 3)
({void*_tmp506=0;({unsigned int _tmpA3F=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpA3E=({const char*_tmp507="regparm requires value between 0 and 3";_tag_dyneither(_tmp507,sizeof(char),39);});Cyc_Warn_err(_tmpA3F,_tmpA3E,_tag_dyneither(_tmp506,sizeof(void*),0));});});
({void*_tmpA41=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp508=_cycalloc_atomic(sizeof(*_tmp508));({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA40=({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmp509;_tmp509.tag=0;_tmp509.f1=n;_tmp509;});_tmp508[0]=_tmpA40;});_tmp508;});a=_tmpA41;});}else{
if(({({struct _dyneither_ptr _tmpA43=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA43,({const char*_tmp50A="initializes";_tag_dyneither(_tmp50A,sizeof(char),12);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA42=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA42,({const char*_tmp50B="__initializes__";_tag_dyneither(_tmp50B,sizeof(char),16);}));})== 0;}))
({void*_tmpA45=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp50C=_cycalloc_atomic(sizeof(*_tmp50C));({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA44=({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmp50D;_tmp50D.tag=20;_tmp50D.f1=n;_tmp50D;});_tmp50C[0]=_tmpA44;});_tmp50C;});a=_tmpA45;});else{
if(({({struct _dyneither_ptr _tmpA47=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA47,({const char*_tmp50E="noliveunique";_tag_dyneither(_tmp50E,sizeof(char),13);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA46=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA46,({const char*_tmp50F="__noliveunique__";_tag_dyneither(_tmp50F,sizeof(char),17);}));})== 0;}))
({void*_tmpA49=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp510=_cycalloc_atomic(sizeof(*_tmp510));({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA48=({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmp511;_tmp511.tag=21;_tmp511.f1=n;_tmp511;});_tmp510[0]=_tmpA48;});_tmp510;});a=_tmpA49;});else{
if(({({struct _dyneither_ptr _tmpA4B=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA4B,({const char*_tmp512="noconsume";_tag_dyneither(_tmp512,sizeof(char),10);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA4A=(struct _dyneither_ptr)_tmp4F4;Cyc_zstrcmp(_tmpA4A,({const char*_tmp513="__noconsume__";_tag_dyneither(_tmp513,sizeof(char),14);}));})== 0;}))
({void*_tmpA4D=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp514=_cycalloc_atomic(sizeof(*_tmp514));({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA4C=({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmp515;_tmp515.tag=22;_tmp515.f1=n;_tmp515;});_tmp514[0]=_tmpA4C;});_tmp514;});a=_tmpA4D;});else{
# 1570
({void*_tmp516=0;({unsigned int _tmpA4F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA4E=({const char*_tmp517="unrecognized attribute";_tag_dyneither(_tmp517,sizeof(char),23);});Cyc_Warn_err(_tmpA4F,_tmpA4E,_tag_dyneither(_tmp516,sizeof(void*),0));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1574
({union Cyc_YYSTYPE _tmpA50=Cyc_YY46(a);yyval=_tmpA50;});
# 1576
goto _LL254;}case 68: _LL2D7: _LL2D8: {
# 1578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 1580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 1577 "parse.y"
struct _dyneither_ptr _tmp518=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp519=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp51A=({unsigned int _tmpA51=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmpA51,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp51B=({unsigned int _tmpA52=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmpA52,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({({struct _dyneither_ptr _tmpA54=(struct _dyneither_ptr)_tmp518;Cyc_zstrcmp(_tmpA54,({const char*_tmp51C="format";_tag_dyneither(_tmp51C,sizeof(char),7);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA53=(struct _dyneither_ptr)_tmp518;Cyc_zstrcmp(_tmpA53,({const char*_tmp51D="__format__";_tag_dyneither(_tmp51D,sizeof(char),11);}));})== 0;})){
if(({({struct _dyneither_ptr _tmpA56=(struct _dyneither_ptr)_tmp519;Cyc_zstrcmp(_tmpA56,({const char*_tmp51E="printf";_tag_dyneither(_tmp51E,sizeof(char),7);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA55=(struct _dyneither_ptr)_tmp519;Cyc_zstrcmp(_tmpA55,({const char*_tmp51F="__printf__";_tag_dyneither(_tmp51F,sizeof(char),11);}));})== 0;}))
({void*_tmpA58=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp520=_cycalloc_atomic(sizeof(*_tmp520));({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA57=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp521;_tmp521.tag=19;_tmp521.f1=Cyc_Absyn_Printf_ft;_tmp521.f2=(int)_tmp51A;_tmp521.f3=(int)_tmp51B;_tmp521;});_tmp520[0]=_tmpA57;});_tmp520;});a=_tmpA58;});else{
if(({({struct _dyneither_ptr _tmpA5A=(struct _dyneither_ptr)_tmp519;Cyc_zstrcmp(_tmpA5A,({const char*_tmp522="scanf";_tag_dyneither(_tmp522,sizeof(char),6);}));})== 0;}) || ({({struct _dyneither_ptr _tmpA59=(struct _dyneither_ptr)_tmp519;Cyc_zstrcmp(_tmpA59,({const char*_tmp523="__scanf__";_tag_dyneither(_tmp523,sizeof(char),10);}));})== 0;}))
({void*_tmpA5C=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp524=_cycalloc_atomic(sizeof(*_tmp524));({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA5B=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp525;_tmp525.tag=19;_tmp525.f1=Cyc_Absyn_Scanf_ft;_tmp525.f2=(int)_tmp51A;_tmp525.f3=(int)_tmp51B;_tmp525;});_tmp524[0]=_tmpA5B;});_tmp524;});a=_tmpA5C;});else{
# 1588
({void*_tmp526=0;({unsigned int _tmpA5E=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpA5D=({const char*_tmp527="unrecognized format type";_tag_dyneither(_tmp527,sizeof(char),25);});Cyc_Warn_err(_tmpA5E,_tmpA5D,_tag_dyneither(_tmp526,sizeof(void*),0));});});}}}else{
# 1590
({void*_tmp528=0;({unsigned int _tmpA60=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA5F=({const char*_tmp529="unrecognized attribute";_tag_dyneither(_tmp529,sizeof(char),23);});Cyc_Warn_err(_tmpA60,_tmpA5F,_tag_dyneither(_tmp528,sizeof(void*),0));});});}
({union Cyc_YYSTYPE _tmpA61=Cyc_YY46(a);yyval=_tmpA61;});
# 1593
goto _LL254;}case 69: _LL2D9: _LL2DA: {
# 1595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1602 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 70: _LL2DB: _LL2DC: {
# 1605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1604 "parse.y"
({union Cyc_YYSTYPE _tmpA66=Cyc_YY21(({void*_tmpA65=(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA64=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp52B;_tmp52B.tag=17;({struct _tuple0*_tmpA62=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp52B.f1=_tmpA62;});({struct Cyc_List_List*_tmpA63=Cyc_yyget_YY40(yyyyvsp[1]);_tmp52B.f2=_tmpA63;});_tmp52B.f3=0;_tmp52B.f4=0;_tmp52B;});_tmp52A[0]=_tmpA64;});_tmp52A;});Cyc_Parse_type_spec(_tmpA65,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA66;});
goto _LL254;}case 71: _LL2DD: _LL2DE: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1608 "parse.y"
({union Cyc_YYSTYPE _tmpA67=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA67;});
goto _LL254;}case 72: _LL2DF: _LL2E0: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1609 "parse.y"
({union Cyc_YYSTYPE _tmpA68=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA68;});
goto _LL254;}case 73: _LL2E1: _LL2E2: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1610 "parse.y"
({union Cyc_YYSTYPE _tmpA69=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA69;});
goto _LL254;}case 74: _LL2E3: _LL2E4: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1611 "parse.y"
({union Cyc_YYSTYPE _tmpA6A=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA6A;});
goto _LL254;}case 75: _LL2E5: _LL2E6: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1612 "parse.y"
({union Cyc_YYSTYPE _tmpA6B=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA6B;});
goto _LL254;}case 76: _LL2E7: _LL2E8: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1613 "parse.y"
({union Cyc_YYSTYPE _tmpA6D=Cyc_YY21(({void*_tmpA6C=Cyc_Absyn_float_typ(0);Cyc_Parse_type_spec(_tmpA6C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA6D;});
goto _LL254;}case 77: _LL2E9: _LL2EA: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1614 "parse.y"
({union Cyc_YYSTYPE _tmpA6F=Cyc_YY21(({void*_tmpA6E=Cyc_Absyn_float_typ(1);Cyc_Parse_type_spec(_tmpA6E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA6F;});
goto _LL254;}case 78: _LL2EB: _LL2EC: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1615 "parse.y"
({union Cyc_YYSTYPE _tmpA70=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA70;});
goto _LL254;}case 79: _LL2ED: _LL2EE: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1616 "parse.y"
({union Cyc_YYSTYPE _tmpA71=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA71;});
goto _LL254;}case 80: _LL2EF: _LL2F0: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1617 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 81: _LL2F1: _LL2F2: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1618 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 82: _LL2F3: _LL2F4: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1621 "parse.y"
({union Cyc_YYSTYPE _tmpA75=Cyc_YY21(({void*_tmpA74=(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpA73=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp52D;_tmp52D.tag=27;({struct Cyc_Absyn_Exp*_tmpA72=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp52D.f1=_tmpA72;});_tmp52D;});_tmp52C[0]=_tmpA73;});_tmp52C;});Cyc_Parse_type_spec(_tmpA74,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA75;});
goto _LL254;}case 83: _LL2F5: _LL2F6: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1623 "parse.y"
({union Cyc_YYSTYPE _tmpA79=Cyc_YY21(({void*_tmpA78=(void*)({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmpA77=({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmp52F;_tmp52F.tag=28;({struct _dyneither_ptr _tmpA76=({const char*_tmp530="__builtin_va_list";_tag_dyneither(_tmp530,sizeof(char),18);});_tmp52F.f1=_tmpA76;});_tmp52F.f2=& Cyc_Tcutil_bk;_tmp52F;});_tmp52E[0]=_tmpA77;});_tmp52E;});Cyc_Parse_type_spec(_tmpA78,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA79;});
goto _LL254;}case 84: _LL2F7: _LL2F8: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1625 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 85: _LL2F9: _LL2FA: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1627 "parse.y"
({union Cyc_YYSTYPE _tmpA7B=Cyc_YY21(({void*_tmpA7A=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmpA7A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA7B;});
goto _LL254;}case 86: _LL2FB: _LL2FC: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1629 "parse.y"
({union Cyc_YYSTYPE _tmpA7D=Cyc_YY21(({void*_tmpA7C=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmpA7C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA7D;});
goto _LL254;}case 87: _LL2FD: _LL2FE: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1631 "parse.y"
({union Cyc_YYSTYPE _tmpA7F=Cyc_YY21(({void*_tmpA7E=({Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);});Cyc_Parse_type_spec(_tmpA7E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA7F;});
goto _LL254;}case 88: _LL2FF: _LL300: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1633 "parse.y"
({union Cyc_YYSTYPE _tmpA84=Cyc_YY21(({void*_tmpA83=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp531=_cycalloc(sizeof(*_tmp531));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpA82=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp532;_tmp532.tag=10;({struct Cyc_List_List*_tmpA81=({unsigned int _tmpA80=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmpA80,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});_tmp532.f1=_tmpA81;});_tmp532;});_tmp531[0]=_tmpA82;});_tmp531;});
# 1633
Cyc_Parse_type_spec(_tmpA83,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA84;});
# 1636
goto _LL254;}case 89: _LL301: _LL302: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1637 "parse.y"
({union Cyc_YYSTYPE _tmpA88=Cyc_YY21(({void*_tmpA87=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp533=_cycalloc(sizeof(*_tmp533));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA86=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp534;_tmp534.tag=15;({void*_tmpA85=Cyc_yyget_YY44(yyyyvsp[2]);_tmp534.f1=_tmpA85;});_tmp534;});_tmp533[0]=_tmpA86;});_tmp533;});Cyc_Parse_type_spec(_tmpA87,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA88;});
goto _LL254;}case 90: _LL303: _LL304: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1639 "parse.y"
({union Cyc_YYSTYPE _tmpA8C=Cyc_YY21(({void*_tmpA8B=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp535=_cycalloc(sizeof(*_tmp535));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA8A=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp536;_tmp536.tag=15;({void*_tmpA89=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);_tmp536.f1=_tmpA89;});_tmp536;});_tmp535[0]=_tmpA8A;});_tmp535;});Cyc_Parse_type_spec(_tmpA8B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA8C;});
# 1641
goto _LL254;}case 91: _LL305: _LL306: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1642 "parse.y"
void*_tmp537=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
({union Cyc_YYSTYPE _tmpA90=Cyc_YY21(({void*_tmpA8F=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp538=_cycalloc(sizeof(*_tmp538));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpA8E=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp539;_tmp539.tag=16;({void*_tmpA8D=Cyc_yyget_YY44(yyyyvsp[2]);_tmp539.f1=_tmpA8D;});_tmp539.f2=_tmp537;_tmp539;});_tmp538[0]=_tmpA8E;});_tmp538;});Cyc_Parse_type_spec(_tmpA8F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA90;});
# 1645
goto _LL254;}case 92: _LL307: _LL308: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 1646 "parse.y"
({union Cyc_YYSTYPE _tmpA95=Cyc_YY21(({void*_tmpA94=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpA93=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp53B;_tmp53B.tag=16;({void*_tmpA91=Cyc_yyget_YY44(yyyyvsp[2]);_tmp53B.f1=_tmpA91;});({void*_tmpA92=Cyc_yyget_YY44(yyyyvsp[4]);_tmp53B.f2=_tmpA92;});_tmp53B;});_tmp53A[0]=_tmpA93;});_tmp53A;});Cyc_Parse_type_spec(_tmpA94,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA95;});
goto _LL254;}case 93: _LL309: _LL30A: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1648 "parse.y"
({union Cyc_YYSTYPE _tmpA99=Cyc_YY21(({void*_tmpA98=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpA97=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53D;_tmp53D.tag=19;({void*_tmpA96=Cyc_yyget_YY44(yyyyvsp[2]);_tmp53D.f1=_tmpA96;});_tmp53D;});_tmp53C[0]=_tmpA97;});_tmp53C;});Cyc_Parse_type_spec(_tmpA98,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA99;});
goto _LL254;}case 94: _LL30B: _LL30C: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1650 "parse.y"
({union Cyc_YYSTYPE _tmpA9D=Cyc_YY21(({void*_tmpA9C=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpA9B=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53F;_tmp53F.tag=19;({void*_tmpA9A=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0);_tmp53F.f1=_tmpA9A;});_tmp53F;});_tmp53E[0]=_tmpA9B;});_tmp53E;});Cyc_Parse_type_spec(_tmpA9C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA9D;});
goto _LL254;}case 95: _LL30D: _LL30E: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1652 "parse.y"
({union Cyc_YYSTYPE _tmpAA1=Cyc_YY21(({void*_tmpAA0=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp540=_cycalloc(sizeof(*_tmp540));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpA9F=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp541;_tmp541.tag=18;({struct Cyc_Absyn_Exp*_tmpA9E=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp541.f1=_tmpA9E;});_tmp541;});_tmp540[0]=_tmpA9F;});_tmp540;});Cyc_Parse_type_spec(_tmpAA0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAA1;});
goto _LL254;}case 96: _LL30F: _LL310: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpAA3=Cyc_YY43(({struct _dyneither_ptr _tmpAA2=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmpAA2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAA3;});
goto _LL254;}case 97: _LL311: _LL312: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1662 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
({union Cyc_YYSTYPE _tmpAA4=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp542;_tmp542.print_const=1;_tmp542.q_volatile=0;_tmp542.q_restrict=0;_tmp542.real_const=1;_tmp542.loc=loc;_tmp542;}));yyval=_tmpAA4;});
goto _LL254;}case 98: _LL313: _LL314: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1664 "parse.y"
({union Cyc_YYSTYPE _tmpAA5=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp543;_tmp543.print_const=0;_tmp543.q_volatile=1;_tmp543.q_restrict=0;_tmp543.real_const=0;_tmp543.loc=0;_tmp543;}));yyval=_tmpAA5;});
goto _LL254;}case 99: _LL315: _LL316: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1665 "parse.y"
({union Cyc_YYSTYPE _tmpAA6=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp544;_tmp544.print_const=0;_tmp544.q_volatile=0;_tmp544.q_restrict=1;_tmp544.real_const=0;_tmp544.loc=0;_tmp544;}));yyval=_tmpAA6;});
goto _LL254;}case 100: _LL317: _LL318: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
struct Cyc_Absyn_TypeDecl*_tmp545=({struct Cyc_Absyn_TypeDecl*_tmp548=_cycalloc(sizeof(*_tmp548));({void*_tmpAAC=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp549=_cycalloc(sizeof(*_tmp549));({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAAB=({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmp54A;_tmp54A.tag=1;({struct Cyc_Absyn_Enumdecl*_tmpAAA=({struct Cyc_Absyn_Enumdecl*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->sc=Cyc_Absyn_Public;({struct _tuple0*_tmpAA7=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp54B->name=_tmpAA7;});({struct Cyc_Core_Opt*_tmpAA9=({struct Cyc_Core_Opt*_tmp54C=_cycalloc(sizeof(*_tmp54C));({struct Cyc_List_List*_tmpAA8=Cyc_yyget_YY48(yyyyvsp[3]);_tmp54C->v=_tmpAA8;});_tmp54C;});_tmp54B->fields=_tmpAA9;});_tmp54B;});_tmp54A.f1=_tmpAAA;});_tmp54A;});_tmp549[0]=_tmpAAB;});_tmp549;});_tmp548->r=_tmpAAC;});_tmp548->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp548;});
# 1673
({union Cyc_YYSTYPE _tmpAAF=Cyc_YY21(({void*_tmpAAE=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp546=_cycalloc(sizeof(*_tmp546));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAAD=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp547;_tmp547.tag=26;_tmp547.f1=_tmp545;_tmp547.f2=0;_tmp547;});_tmp546[0]=_tmpAAD;});_tmp546;});Cyc_Parse_type_spec(_tmpAAE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAAF;});
# 1675
goto _LL254;}case 101: _LL319: _LL31A: {
# 1677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1676 "parse.y"
({union Cyc_YYSTYPE _tmpAB3=Cyc_YY21(({void*_tmpAB2=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAB1=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp54E;_tmp54E.tag=13;({struct _tuple0*_tmpAB0=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp54E.f1=_tmpAB0;});_tmp54E.f2=0;_tmp54E;});_tmp54D[0]=_tmpAB1;});_tmp54D;});Cyc_Parse_type_spec(_tmpAB2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAB3;});
goto _LL254;}case 102: _LL31B: _LL31C: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1678 "parse.y"
({union Cyc_YYSTYPE _tmpAB7=Cyc_YY21(({void*_tmpAB6=(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAB5=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp550;_tmp550.tag=14;({struct Cyc_List_List*_tmpAB4=Cyc_yyget_YY48(yyyyvsp[2]);_tmp550.f1=_tmpAB4;});_tmp550;});_tmp54F[0]=_tmpAB5;});_tmp54F;});Cyc_Parse_type_spec(_tmpAB6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAB7;});
goto _LL254;}case 103: _LL31D: _LL31E: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpAB9=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp551=_cycalloc(sizeof(*_tmp551));({struct _tuple0*_tmpAB8=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp551->name=_tmpAB8;});_tmp551->tag=0;_tmp551->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp551;}));yyval=_tmpAB9;});
goto _LL254;}case 104: _LL31F: _LL320: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1686 "parse.y"
({union Cyc_YYSTYPE _tmpABC=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp552=_cycalloc(sizeof(*_tmp552));({struct _tuple0*_tmpABA=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp552->name=_tmpABA;});({struct Cyc_Absyn_Exp*_tmpABB=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp552->tag=_tmpABB;});_tmp552->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp552;}));yyval=_tmpABC;});
goto _LL254;}case 105: _LL321: _LL322: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1690 "parse.y"
({union Cyc_YYSTYPE _tmpABE=Cyc_YY48(({struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));({struct Cyc_Absyn_Enumfield*_tmpABD=Cyc_yyget_YY47(yyyyvsp[0]);_tmp553->hd=_tmpABD;});_tmp553->tl=0;_tmp553;}));yyval=_tmpABE;});
goto _LL254;}case 106: _LL323: _LL324: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1691 "parse.y"
({union Cyc_YYSTYPE _tmpAC0=Cyc_YY48(({struct Cyc_List_List*_tmp554=_cycalloc(sizeof(*_tmp554));({struct Cyc_Absyn_Enumfield*_tmpABF=Cyc_yyget_YY47(yyyyvsp[0]);_tmp554->hd=_tmpABF;});_tmp554->tl=0;_tmp554;}));yyval=_tmpAC0;});
goto _LL254;}case 107: _LL325: _LL326: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1692 "parse.y"
({union Cyc_YYSTYPE _tmpAC3=Cyc_YY48(({struct Cyc_List_List*_tmp555=_cycalloc(sizeof(*_tmp555));({struct Cyc_Absyn_Enumfield*_tmpAC1=Cyc_yyget_YY47(yyyyvsp[0]);_tmp555->hd=_tmpAC1;});({struct Cyc_List_List*_tmpAC2=Cyc_yyget_YY48(yyyyvsp[2]);_tmp555->tl=_tmpAC2;});_tmp555;}));yyval=_tmpAC3;});
goto _LL254;}case 108: _LL327: _LL328: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
({union Cyc_YYSTYPE _tmpAC8=Cyc_YY21(({void*_tmpAC7=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp556=_cycalloc(sizeof(*_tmp556));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAC6=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp557;_tmp557.tag=12;({enum Cyc_Absyn_AggrKind _tmpAC4=Cyc_yyget_YY22(yyyyvsp[0]);_tmp557.f1=_tmpAC4;});({struct Cyc_List_List*_tmpAC5=Cyc_yyget_YY24(yyyyvsp[2]);_tmp557.f2=_tmpAC5;});_tmp557;});_tmp556[0]=_tmpAC6;});_tmp556;});Cyc_Parse_type_spec(_tmpAC7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAC8;});
goto _LL254;}case 109: _LL329: _LL32A: {
# 1701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);}));
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);}));
# 1704 "parse.y"
struct Cyc_List_List*_tmp558=({unsigned int _tmpAC9=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAC9,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp559=({unsigned int _tmpACA=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpACA,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp55A=({enum Cyc_Absyn_AggrKind _tmpAD0=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmpACF=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpACE=_tmp558;struct Cyc_Absyn_AggrdeclImpl*_tmpACD=({
struct Cyc_List_List*_tmpACC=_tmp559;struct Cyc_List_List*_tmpACB=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmpACC,_tmpACB,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1706
Cyc_Absyn_aggr_tdecl(_tmpAD0,Cyc_Absyn_Public,_tmpACF,_tmpACE,_tmpACD,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1709
({union Cyc_YYSTYPE _tmpAD3=Cyc_YY21(({void*_tmpAD2=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAD1=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp55C;_tmp55C.tag=26;_tmp55C.f1=_tmp55A;_tmp55C.f2=0;_tmp55C;});_tmp55B[0]=_tmpAD1;});_tmp55B;});Cyc_Parse_type_spec(_tmpAD2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAD3;});
# 1711
goto _LL254;}case 110: _LL32B: _LL32C: {
# 1713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 1715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 1715 "parse.y"
struct Cyc_List_List*_tmp55D=({unsigned int _tmpAD4=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAD4,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp55E=({unsigned int _tmpAD5=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAD5,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp55F=({enum Cyc_Absyn_AggrKind _tmpADB=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmpADA=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpAD9=_tmp55D;struct Cyc_Absyn_AggrdeclImpl*_tmpAD8=({
struct Cyc_List_List*_tmpAD7=_tmp55E;struct Cyc_List_List*_tmpAD6=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmpAD7,_tmpAD6,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1717
Cyc_Absyn_aggr_tdecl(_tmpADB,Cyc_Absyn_Public,_tmpADA,_tmpAD9,_tmpAD8,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1720
({union Cyc_YYSTYPE _tmpADE=Cyc_YY21(({void*_tmpADD=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp560=_cycalloc(sizeof(*_tmp560));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpADC=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp561;_tmp561.tag=26;_tmp561.f1=_tmp55F;_tmp561.f2=0;_tmp561;});_tmp560[0]=_tmpADC;});_tmp560;});Cyc_Parse_type_spec(_tmpADD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpADE;});
# 1722
goto _LL254;}case 111: _LL32D: _LL32E: {
# 1724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1723 "parse.y"
({union Cyc_YYSTYPE _tmpAE6=Cyc_YY21(({void*_tmpAE5=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp562=_cycalloc(sizeof(*_tmp562));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAE4=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp563;_tmp563.tag=11;({struct Cyc_Absyn_AggrInfo _tmpAE3=({struct Cyc_Absyn_AggrInfo _tmp564;({union Cyc_Absyn_AggrInfoU _tmpAE1=({enum Cyc_Absyn_AggrKind _tmpAE0=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmpADF=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmpAE0,_tmpADF,({struct Cyc_Core_Opt*_tmp565=_cycalloc_atomic(sizeof(*_tmp565));_tmp565->v=(void*)1;_tmp565;}));});_tmp564.aggr_info=_tmpAE1;});({struct Cyc_List_List*_tmpAE2=Cyc_yyget_YY40(yyyyvsp[3]);_tmp564.targs=_tmpAE2;});_tmp564;});_tmp563.f1=_tmpAE3;});_tmp563;});_tmp562[0]=_tmpAE4;});_tmp562;});Cyc_Parse_type_spec(_tmpAE5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAE6;});
# 1726
goto _LL254;}case 112: _LL32F: _LL330: {
# 1728
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1730
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1727 "parse.y"
({union Cyc_YYSTYPE _tmpAED=Cyc_YY21(({void*_tmpAEC=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp566=_cycalloc(sizeof(*_tmp566));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAEB=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp567;_tmp567.tag=11;({struct Cyc_Absyn_AggrInfo _tmpAEA=({struct Cyc_Absyn_AggrInfo _tmp568;({union Cyc_Absyn_AggrInfoU _tmpAE8=({enum Cyc_Absyn_AggrKind _tmpAE7=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmpAE7,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});_tmp568.aggr_info=_tmpAE8;});({struct Cyc_List_List*_tmpAE9=Cyc_yyget_YY40(yyyyvsp[2]);_tmp568.targs=_tmpAE9;});_tmp568;});_tmp567.f1=_tmpAEA;});_tmp567;});_tmp566[0]=_tmpAEB;});_tmp566;});Cyc_Parse_type_spec(_tmpAEC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAED;});
# 1730
goto _LL254;}case 113: _LL331: _LL332:
# 1732
({union Cyc_YYSTYPE _tmpAEE=Cyc_YY40(0);yyval=_tmpAEE;});
goto _LL254;case 114: _LL333: _LL334: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1736 "parse.y"
({union Cyc_YYSTYPE _tmpAEF=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmpAEF;});
goto _LL254;}case 115: _LL335: _LL336: {
# 1739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1740 "parse.y"
({union Cyc_YYSTYPE _tmpAF0=Cyc_YY22(Cyc_Absyn_StructA);yyval=_tmpAF0;});
goto _LL254;}case 116: _LL337: _LL338: {
# 1743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1741 "parse.y"
({union Cyc_YYSTYPE _tmpAF1=Cyc_YY22(Cyc_Absyn_UnionA);yyval=_tmpAF1;});
goto _LL254;}case 117: _LL339: _LL33A:
# 1744
({union Cyc_YYSTYPE _tmpAF2=Cyc_YY24(0);yyval=_tmpAF2;});
goto _LL254;case 118: _LL33B: _LL33C: {
# 1747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1748 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp569=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp569 != 0;_tmp569=_tmp569->tl){
({struct Cyc_List_List*_tmpAF3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp569->hd,decls);decls=_tmpAF3;});}}{
# 1752
struct Cyc_List_List*_tmp56A=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp56A != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp56A,decls);
({union Cyc_YYSTYPE _tmpAF4=Cyc_YY24(decls);yyval=_tmpAF4;});
# 1757
goto _LL254;};}case 119: _LL33D: _LL33E: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpAF6=Cyc_YY25(({struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));({struct Cyc_List_List*_tmpAF5=Cyc_yyget_YY24(yyyyvsp[0]);_tmp56B->hd=_tmpAF5;});_tmp56B->tl=0;_tmp56B;}));yyval=_tmpAF6;});
goto _LL254;}case 120: _LL33F: _LL340: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1764 "parse.y"
({union Cyc_YYSTYPE _tmpAF9=Cyc_YY25(({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));({struct Cyc_List_List*_tmpAF7=Cyc_yyget_YY24(yyyyvsp[1]);_tmp56C->hd=_tmpAF7;});({struct Cyc_List_List*_tmpAF8=Cyc_yyget_YY25(yyyyvsp[0]);_tmp56C->tl=_tmpAF8;});_tmp56C;}));yyval=_tmpAF9;});
goto _LL254;}case 121: _LL341: _LL342: {
# 1767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1768 "parse.y"
({union Cyc_YYSTYPE _tmpAFA=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));yyval=_tmpAFA;});
goto _LL254;}case 122: _LL343: _LL344: {
# 1771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpAFC=Cyc_YY19(({struct _tuple12*_tmp56D=_region_malloc(yyr,sizeof(*_tmp56D));_tmp56D->tl=0;({struct _tuple11 _tmpAFB=Cyc_yyget_YY18(yyyyvsp[0]);_tmp56D->hd=_tmpAFB;});_tmp56D;}));yyval=_tmpAFC;});
goto _LL254;}case 123: _LL345: _LL346: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1776 "parse.y"
({union Cyc_YYSTYPE _tmpAFF=Cyc_YY19(({struct _tuple12*_tmp56E=_region_malloc(yyr,sizeof(*_tmp56E));({struct _tuple12*_tmpAFD=Cyc_yyget_YY19(yyyyvsp[0]);_tmp56E->tl=_tmpAFD;});({struct _tuple11 _tmpAFE=Cyc_yyget_YY18(yyyyvsp[2]);_tmp56E->hd=_tmpAFE;});_tmp56E;}));yyval=_tmpAFF;});
goto _LL254;}case 124: _LL347: _LL348: {
# 1779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1781 "parse.y"
({union Cyc_YYSTYPE _tmpB01=Cyc_YY18(({struct _tuple11 _tmp56F;({struct Cyc_Parse_Declarator _tmpB00=Cyc_yyget_YY27(yyyyvsp[0]);_tmp56F.f1=_tmpB00;});_tmp56F.f2=0;_tmp56F;}));yyval=_tmpB01;});
goto _LL254;}case 125: _LL349: _LL34A: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1783 "parse.y"
({union Cyc_YYSTYPE _tmpB04=Cyc_YY18(({struct _tuple11 _tmp570;({struct Cyc_Parse_Declarator _tmpB02=Cyc_yyget_YY27(yyyyvsp[0]);_tmp570.f1=_tmpB02;});({struct Cyc_Absyn_Exp*_tmpB03=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp570.f2=_tmpB03;});_tmp570;}));yyval=_tmpB04;});
goto _LL254;}case 126: _LL34B: _LL34C: {
# 1786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1789 "parse.y"
struct _RegionHandle _tmp571=_new_region("temp");struct _RegionHandle*temp=& _tmp571;_push_region(temp);
{struct _tuple25 _tmp572=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp573=_tmp572;struct Cyc_Absyn_Tqual _tmp581;struct Cyc_Parse_Type_specifier _tmp580;struct Cyc_List_List*_tmp57F;_LL689: _tmp581=_tmp573.f1;_tmp580=_tmp573.f2;_tmp57F=_tmp573.f3;_LL68A:;
if(_tmp581.loc == 0)_tmp581.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp574=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp574 != 0;_tmp574=_tmp574->tl){
struct _tuple24*_tmp575=(struct _tuple24*)_tmp574->hd;struct _tuple24*_tmp576=_tmp575;struct Cyc_Parse_Declarator _tmp57C;struct Cyc_Absyn_Exp*_tmp57B;struct Cyc_Absyn_Exp*_tmp57A;_LL68C: _tmp57C=_tmp576->f1;_tmp57B=_tmp576->f2;_tmp57A=_tmp576->f3;_LL68D:;
({struct _tuple10*_tmpB05=({struct _tuple10*_tmp577=_region_malloc(temp,sizeof(*_tmp577));_tmp577->tl=decls;_tmp577->hd=_tmp57C;_tmp577;});decls=_tmpB05;});
({struct Cyc_List_List*_tmpB07=({struct Cyc_List_List*_tmp578=_region_malloc(temp,sizeof(*_tmp578));({struct _tuple16*_tmpB06=({struct _tuple16*_tmp579=_region_malloc(temp,sizeof(*_tmp579));_tmp579->f1=_tmp57B;_tmp579->f2=_tmp57A;_tmp579;});_tmp578->hd=_tmpB06;});_tmp578->tl=widths_and_reqs;_tmp578;});widths_and_reqs=_tmpB07;});}}
# 1800
({struct _tuple10*_tmpB08=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);decls=_tmpB08;});
({struct Cyc_List_List*_tmpB09=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);widths_and_reqs=_tmpB09;});{
void*_tmp57D=Cyc_Parse_speclist2typ(_tmp580,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57E=({struct _RegionHandle*_tmpB0C=temp;struct _RegionHandle*_tmpB0B=temp;struct Cyc_List_List*_tmpB0A=
Cyc_Parse_apply_tmss(temp,_tmp581,_tmp57D,decls,_tmp57F);
# 1803
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmpB0C,_tmpB0B,_tmpB0A,widths_and_reqs);});
# 1806
({union Cyc_YYSTYPE _tmpB0D=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp57E));yyval=_tmpB0D;});
# 1808
_npop_handler(0);goto _LL254;};};}
# 1790
;_pop_region(temp);}case 127: _LL34D: _LL34E: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1816 "parse.y"
({union Cyc_YYSTYPE _tmpB10=Cyc_YY35(({struct _tuple25 _tmp582;({struct Cyc_Absyn_Tqual _tmpB0E=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp582.f1=_tmpB0E;});({struct Cyc_Parse_Type_specifier _tmpB0F=Cyc_yyget_YY21(yyyyvsp[0]);_tmp582.f2=_tmpB0F;});_tmp582.f3=0;_tmp582;}));yyval=_tmpB10;});
goto _LL254;}case 128: _LL34F: _LL350: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1818 "parse.y"
struct _tuple25 _tmp583=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB14=Cyc_YY35(({struct _tuple25 _tmp584;_tmp584.f1=_tmp583.f1;({struct Cyc_Parse_Type_specifier _tmpB13=({unsigned int _tmpB12=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmpB11=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmpB12,_tmpB11,_tmp583.f2);});_tmp584.f2=_tmpB13;});_tmp584.f3=_tmp583.f3;_tmp584;}));yyval=_tmpB14;});
goto _LL254;}case 129: _LL351: _LL352: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1820 "parse.y"
({union Cyc_YYSTYPE _tmpB17=Cyc_YY35(({struct _tuple25 _tmp585;({struct Cyc_Absyn_Tqual _tmpB15=Cyc_yyget_YY23(yyyyvsp[0]);_tmp585.f1=_tmpB15;});({struct Cyc_Parse_Type_specifier _tmpB16=Cyc_Parse_empty_spec(0);_tmp585.f2=_tmpB16;});_tmp585.f3=0;_tmp585;}));yyval=_tmpB17;});
goto _LL254;}case 130: _LL353: _LL354: {
# 1823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1822 "parse.y"
struct _tuple25 _tmp586=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB1A=Cyc_YY35(({struct _tuple25 _tmp587;({struct Cyc_Absyn_Tqual _tmpB19=({struct Cyc_Absyn_Tqual _tmpB18=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpB18,_tmp586.f1);});_tmp587.f1=_tmpB19;});_tmp587.f2=_tmp586.f2;_tmp587.f3=_tmp586.f3;_tmp587;}));yyval=_tmpB1A;});
goto _LL254;}case 131: _LL355: _LL356: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1825 "parse.y"
({union Cyc_YYSTYPE _tmpB1E=Cyc_YY35(({struct _tuple25 _tmp588;({struct Cyc_Absyn_Tqual _tmpB1B=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp588.f1=_tmpB1B;});({struct Cyc_Parse_Type_specifier _tmpB1C=Cyc_Parse_empty_spec(0);_tmp588.f2=_tmpB1C;});({struct Cyc_List_List*_tmpB1D=Cyc_yyget_YY45(yyyyvsp[0]);_tmp588.f3=_tmpB1D;});_tmp588;}));yyval=_tmpB1E;});
goto _LL254;}case 132: _LL357: _LL358: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1827 "parse.y"
struct _tuple25 _tmp589=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB21=Cyc_YY35(({struct _tuple25 _tmp58A;_tmp58A.f1=_tmp589.f1;_tmp58A.f2=_tmp589.f2;({struct Cyc_List_List*_tmpB20=({struct Cyc_List_List*_tmpB1F=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB1F,_tmp589.f3);});_tmp58A.f3=_tmpB20;});_tmp58A;}));yyval=_tmpB21;});
goto _LL254;}case 133: _LL359: _LL35A: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpB24=Cyc_YY35(({struct _tuple25 _tmp58B;({struct Cyc_Absyn_Tqual _tmpB22=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp58B.f1=_tmpB22;});({struct Cyc_Parse_Type_specifier _tmpB23=Cyc_yyget_YY21(yyyyvsp[0]);_tmp58B.f2=_tmpB23;});_tmp58B.f3=0;_tmp58B;}));yyval=_tmpB24;});
goto _LL254;}case 134: _LL35B: _LL35C: {
# 1836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1835 "parse.y"
struct _tuple25 _tmp58C=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB28=Cyc_YY35(({struct _tuple25 _tmp58D;_tmp58D.f1=_tmp58C.f1;({struct Cyc_Parse_Type_specifier _tmpB27=({unsigned int _tmpB26=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmpB25=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmpB26,_tmpB25,_tmp58C.f2);});_tmp58D.f2=_tmpB27;});_tmp58D.f3=_tmp58C.f3;_tmp58D;}));yyval=_tmpB28;});
goto _LL254;}case 135: _LL35D: _LL35E: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1837 "parse.y"
({union Cyc_YYSTYPE _tmpB2B=Cyc_YY35(({struct _tuple25 _tmp58E;({struct Cyc_Absyn_Tqual _tmpB29=Cyc_yyget_YY23(yyyyvsp[0]);_tmp58E.f1=_tmpB29;});({struct Cyc_Parse_Type_specifier _tmpB2A=Cyc_Parse_empty_spec(0);_tmp58E.f2=_tmpB2A;});_tmp58E.f3=0;_tmp58E;}));yyval=_tmpB2B;});
goto _LL254;}case 136: _LL35F: _LL360: {
# 1840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1839 "parse.y"
struct _tuple25 _tmp58F=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB2E=Cyc_YY35(({struct _tuple25 _tmp590;({struct Cyc_Absyn_Tqual _tmpB2D=({struct Cyc_Absyn_Tqual _tmpB2C=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpB2C,_tmp58F.f1);});_tmp590.f1=_tmpB2D;});_tmp590.f2=_tmp58F.f2;_tmp590.f3=_tmp58F.f3;_tmp590;}));yyval=_tmpB2E;});
goto _LL254;}case 137: _LL361: _LL362: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1842 "parse.y"
({union Cyc_YYSTYPE _tmpB32=Cyc_YY35(({struct _tuple25 _tmp591;({struct Cyc_Absyn_Tqual _tmpB2F=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp591.f1=_tmpB2F;});({struct Cyc_Parse_Type_specifier _tmpB30=Cyc_Parse_empty_spec(0);_tmp591.f2=_tmpB30;});({struct Cyc_List_List*_tmpB31=Cyc_yyget_YY45(yyyyvsp[0]);_tmp591.f3=_tmpB31;});_tmp591;}));yyval=_tmpB32;});
goto _LL254;}case 138: _LL363: _LL364: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1844 "parse.y"
struct _tuple25 _tmp592=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB35=Cyc_YY35(({struct _tuple25 _tmp593;_tmp593.f1=_tmp592.f1;_tmp593.f2=_tmp592.f2;({struct Cyc_List_List*_tmpB34=({struct Cyc_List_List*_tmpB33=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB33,_tmp592.f3);});_tmp593.f3=_tmpB34;});_tmp593;}));yyval=_tmpB35;});
goto _LL254;}case 139: _LL365: _LL366: {
# 1847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1848 "parse.y"
({union Cyc_YYSTYPE _tmpB36=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));yyval=_tmpB36;});
goto _LL254;}case 140: _LL367: _LL368: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpB38=Cyc_YY29(({struct Cyc_List_List*_tmp594=_region_malloc(yyr,sizeof(*_tmp594));({struct _tuple24*_tmpB37=Cyc_yyget_YY28(yyyyvsp[0]);_tmp594->hd=_tmpB37;});_tmp594->tl=0;_tmp594;}));yyval=_tmpB38;});
goto _LL254;}case 141: _LL369: _LL36A: {
# 1857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1856 "parse.y"
({union Cyc_YYSTYPE _tmpB3B=Cyc_YY29(({struct Cyc_List_List*_tmp595=_region_malloc(yyr,sizeof(*_tmp595));({struct _tuple24*_tmpB39=Cyc_yyget_YY28(yyyyvsp[2]);_tmp595->hd=_tmpB39;});({struct Cyc_List_List*_tmpB3A=Cyc_yyget_YY29(yyyyvsp[0]);_tmp595->tl=_tmpB3A;});_tmp595;}));yyval=_tmpB3B;});
goto _LL254;}case 142: _LL36B: _LL36C: {
# 1859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1861 "parse.y"
({union Cyc_YYSTYPE _tmpB3E=Cyc_YY28(({struct _tuple24*_tmp596=_region_malloc(yyr,sizeof(*_tmp596));({struct Cyc_Parse_Declarator _tmpB3C=Cyc_yyget_YY27(yyyyvsp[0]);_tmp596->f1=_tmpB3C;});_tmp596->f2=0;({struct Cyc_Absyn_Exp*_tmpB3D=Cyc_yyget_YY55(yyyyvsp[1]);_tmp596->f3=_tmpB3D;});_tmp596;}));yyval=_tmpB3E;});
goto _LL254;}case 143: _LL36D: _LL36E: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1865 "parse.y"
({union Cyc_YYSTYPE _tmpB44=Cyc_YY28(({struct _tuple24*_tmp597=_region_malloc(yyr,sizeof(*_tmp597));({struct Cyc_Parse_Declarator _tmpB42=({struct Cyc_Parse_Declarator _tmp598;({struct _tuple0*_tmpB41=({struct _tuple0*_tmp599=_cycalloc(sizeof(*_tmp599));({union Cyc_Absyn_Nmspace _tmpB3F=Cyc_Absyn_Rel_n(0);_tmp599->f1=_tmpB3F;});({struct _dyneither_ptr*_tmpB40=({_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);});_tmp599->f2=_tmpB40;});_tmp599;});_tmp598.id=_tmpB41;});_tmp598.varloc=0;_tmp598.tms=0;_tmp598;});_tmp597->f1=_tmpB42;});({struct Cyc_Absyn_Exp*_tmpB43=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp597->f2=_tmpB43;});_tmp597->f3=0;_tmp597;}));yyval=_tmpB44;});
# 1867
goto _LL254;}case 144: _LL36F: _LL370:
# 1871
({union Cyc_YYSTYPE _tmpB49=Cyc_YY28(({struct _tuple24*_tmp59B=_region_malloc(yyr,sizeof(*_tmp59B));({struct Cyc_Parse_Declarator _tmpB48=({struct Cyc_Parse_Declarator _tmp59C;({struct _tuple0*_tmpB47=({struct _tuple0*_tmp59D=_cycalloc(sizeof(*_tmp59D));({union Cyc_Absyn_Nmspace _tmpB45=Cyc_Absyn_Rel_n(0);_tmp59D->f1=_tmpB45;});({struct _dyneither_ptr*_tmpB46=({_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);});_tmp59D->f2=_tmpB46;});_tmp59D;});_tmp59C.id=_tmpB47;});_tmp59C.varloc=0;_tmp59C.tms=0;_tmp59C;});_tmp59B->f1=_tmpB48;});_tmp59B->f2=0;_tmp59B->f3=0;_tmp59B;}));yyval=_tmpB49;});
# 1873
goto _LL254;case 145: _LL371: _LL372: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1873 "parse.y"
({union Cyc_YYSTYPE _tmpB4C=Cyc_YY28(({struct _tuple24*_tmp59F=_region_malloc(yyr,sizeof(*_tmp59F));({struct Cyc_Parse_Declarator _tmpB4A=Cyc_yyget_YY27(yyyyvsp[0]);_tmp59F->f1=_tmpB4A;});({struct Cyc_Absyn_Exp*_tmpB4B=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp59F->f2=_tmpB4B;});_tmp59F->f3=0;_tmp59F;}));yyval=_tmpB4C;});
goto _LL254;}case 146: _LL373: _LL374: {
# 1876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1877 "parse.y"
({union Cyc_YYSTYPE _tmpB4D=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpB4D;});
goto _LL254;}case 147: _LL375: _LL376:
# 1880
({union Cyc_YYSTYPE _tmpB4E=Cyc_YY55(0);yyval=_tmpB4E;});
goto _LL254;case 148: _LL377: _LL378: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1882 "parse.y"
({union Cyc_YYSTYPE _tmpB4F=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpB4F;});
goto _LL254;}case 149: _LL379: _LL37A:
# 1885
({union Cyc_YYSTYPE _tmpB50=Cyc_YY55(0);yyval=_tmpB50;});
goto _LL254;case 150: _LL37B: _LL37C: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 1889 "parse.y"
int _tmp5A0=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp5A1=({unsigned int _tmpB51=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB51,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp5A2=({struct _tuple0*_tmpB56=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpB55=_tmp5A1;struct Cyc_Core_Opt*_tmpB54=({struct Cyc_Core_Opt*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));({struct Cyc_List_List*_tmpB52=Cyc_yyget_YY34(yyyyvsp[4]);_tmp5A5->v=_tmpB52;});_tmp5A5;});int _tmpB53=_tmp5A0;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmpB56,_tmpB55,_tmpB54,_tmpB53,(unsigned int)(yyyylsp[0]).first_line);});
# 1893
({union Cyc_YYSTYPE _tmpB59=Cyc_YY21(({void*_tmpB58=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB57=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp5A4;_tmp5A4.tag=26;_tmp5A4.f1=_tmp5A2;_tmp5A4.f2=0;_tmp5A4;});_tmp5A3[0]=_tmpB57;});_tmp5A3;});Cyc_Parse_type_spec(_tmpB58,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB59;});
# 1895
goto _LL254;}case 151: _LL37D: _LL37E: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1896 "parse.y"
int _tmp5A6=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpB60=Cyc_YY21(({void*_tmpB5F=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB5E=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp5A8;_tmp5A8.tag=3;({struct Cyc_Absyn_DatatypeInfo _tmpB5D=({struct Cyc_Absyn_DatatypeInfo _tmp5A9;({union Cyc_Absyn_DatatypeInfoU _tmpB5B=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5AA;({struct _tuple0*_tmpB5A=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5AA.name=_tmpB5A;});_tmp5AA.is_extensible=_tmp5A6;_tmp5AA;}));_tmp5A9.datatype_info=_tmpB5B;});({struct Cyc_List_List*_tmpB5C=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5A9.targs=_tmpB5C;});_tmp5A9;});_tmp5A8.f1=_tmpB5D;});_tmp5A8;});_tmp5A7[0]=_tmpB5E;});_tmp5A7;});Cyc_Parse_type_spec(_tmpB5F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB60;});
# 1899
goto _LL254;}case 152: _LL37F: _LL380: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1900 "parse.y"
int _tmp5AB=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpB68=Cyc_YY21(({void*_tmpB67=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB66=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp5AD;_tmp5AD.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmpB65=({struct Cyc_Absyn_DatatypeFieldInfo _tmp5AE;({union Cyc_Absyn_DatatypeFieldInfoU _tmpB63=
Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp5AF;({struct _tuple0*_tmpB61=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5AF.datatype_name=_tmpB61;});({struct _tuple0*_tmpB62=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp5AF.field_name=_tmpB62;});_tmp5AF.is_extensible=_tmp5AB;_tmp5AF;}));_tmp5AE.field_info=_tmpB63;});({struct Cyc_List_List*_tmpB64=Cyc_yyget_YY40(yyyyvsp[4]);_tmp5AE.targs=_tmpB64;});_tmp5AE;});_tmp5AD.f1=_tmpB65;});_tmp5AD;});_tmp5AC[0]=_tmpB66;});_tmp5AC;});
# 1901
Cyc_Parse_type_spec(_tmpB67,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB68;});
# 1905
goto _LL254;}case 153: _LL381: _LL382: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1908 "parse.y"
({union Cyc_YYSTYPE _tmpB69=Cyc_YY31(0);yyval=_tmpB69;});
goto _LL254;}case 154: _LL383: _LL384: {
# 1911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1909 "parse.y"
({union Cyc_YYSTYPE _tmpB6A=Cyc_YY31(1);yyval=_tmpB6A;});
goto _LL254;}case 155: _LL385: _LL386: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1913 "parse.y"
({union Cyc_YYSTYPE _tmpB6C=Cyc_YY34(({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));({struct Cyc_Absyn_Datatypefield*_tmpB6B=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B0->hd=_tmpB6B;});_tmp5B0->tl=0;_tmp5B0;}));yyval=_tmpB6C;});
goto _LL254;}case 156: _LL387: _LL388: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1914 "parse.y"
({union Cyc_YYSTYPE _tmpB6E=Cyc_YY34(({struct Cyc_List_List*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));({struct Cyc_Absyn_Datatypefield*_tmpB6D=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B1->hd=_tmpB6D;});_tmp5B1->tl=0;_tmp5B1;}));yyval=_tmpB6E;});
goto _LL254;}case 157: _LL389: _LL38A: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1915 "parse.y"
({union Cyc_YYSTYPE _tmpB71=Cyc_YY34(({struct Cyc_List_List*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));({struct Cyc_Absyn_Datatypefield*_tmpB6F=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B2->hd=_tmpB6F;});({struct Cyc_List_List*_tmpB70=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5B2->tl=_tmpB70;});_tmp5B2;}));yyval=_tmpB71;});
goto _LL254;}case 158: _LL38B: _LL38C: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1916 "parse.y"
({union Cyc_YYSTYPE _tmpB74=Cyc_YY34(({struct Cyc_List_List*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));({struct Cyc_Absyn_Datatypefield*_tmpB72=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B3->hd=_tmpB72;});({struct Cyc_List_List*_tmpB73=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5B3->tl=_tmpB73;});_tmp5B3;}));yyval=_tmpB74;});
goto _LL254;}case 159: _LL38D: _LL38E:
# 1919
({union Cyc_YYSTYPE _tmpB75=Cyc_YY32(Cyc_Absyn_Public);yyval=_tmpB75;});
goto _LL254;case 160: _LL38F: _LL390: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1921 "parse.y"
({union Cyc_YYSTYPE _tmpB76=Cyc_YY32(Cyc_Absyn_Extern);yyval=_tmpB76;});
goto _LL254;}case 161: _LL391: _LL392: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1922 "parse.y"
({union Cyc_YYSTYPE _tmpB77=Cyc_YY32(Cyc_Absyn_Static);yyval=_tmpB77;});
goto _LL254;}case 162: _LL393: _LL394: {
# 1925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1926 "parse.y"
({union Cyc_YYSTYPE _tmpB7A=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));({struct _tuple0*_tmpB78=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B4->name=_tmpB78;});_tmp5B4->typs=0;_tmp5B4->loc=(unsigned int)(yyyylsp[0]).first_line;({enum Cyc_Absyn_Scope _tmpB79=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5B4->sc=_tmpB79;});_tmp5B4;}));yyval=_tmpB7A;});
goto _LL254;}case 163: _LL395: _LL396: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1928 "parse.y"
struct Cyc_List_List*_tmp5B5=({unsigned int _tmpB7B=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmpB7B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
({union Cyc_YYSTYPE _tmpB7E=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));({struct _tuple0*_tmpB7C=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B6->name=_tmpB7C;});_tmp5B6->typs=_tmp5B5;_tmp5B6->loc=(unsigned int)(yyyylsp[0]).first_line;({enum Cyc_Absyn_Scope _tmpB7D=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5B6->sc=_tmpB7D;});_tmp5B6;}));yyval=_tmpB7E;});
goto _LL254;}case 164: _LL397: _LL398: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1934 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 165: _LL399: _LL39A: {
# 1937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1936 "parse.y"
struct Cyc_Parse_Declarator _tmp5B7=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB81=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B8;_tmp5B8.id=_tmp5B7.id;_tmp5B8.varloc=_tmp5B7.varloc;({struct Cyc_List_List*_tmpB80=({struct Cyc_List_List*_tmpB7F=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB7F,_tmp5B7.tms);});_tmp5B8.tms=_tmpB80;});_tmp5B8;}));yyval=_tmpB81;});
goto _LL254;}case 166: _LL39B: _LL39C: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
yyval=yyyyvsp[0];
goto _LL254;}case 167: _LL39D: _LL39E: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1945 "parse.y"
struct Cyc_Parse_Declarator _tmp5B9=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB84=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5BA;_tmp5BA.id=_tmp5B9.id;_tmp5BA.varloc=_tmp5B9.varloc;({struct Cyc_List_List*_tmpB83=({struct Cyc_List_List*_tmpB82=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB82,_tmp5B9.tms);});_tmp5BA.tms=_tmpB83;});_tmp5BA;}));yyval=_tmpB84;});
goto _LL254;}case 168: _LL39F: _LL3A0: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 1951 "parse.y"
({union Cyc_YYSTYPE _tmpB86=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5BB;({struct _tuple0*_tmpB85=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5BB.id=_tmpB85;});_tmp5BB.varloc=(unsigned int)(yyyylsp[0]).first_line;_tmp5BB.tms=0;_tmp5BB;}));yyval=_tmpB86;});
goto _LL254;}case 169: _LL3A1: _LL3A2: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 1953 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 170: _LL3A3: _LL3A4: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1957 "parse.y"
struct Cyc_Parse_Declarator _tmp5BC=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpB8A=({struct Cyc_List_List*_tmp5BD=_region_malloc(yyr,sizeof(*_tmp5BD));({void*_tmpB89=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5BE=_region_malloc(yyr,sizeof(*_tmp5BE));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB88=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5BF;_tmp5BF.tag=5;_tmp5BF.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpB87=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5BF.f2=_tmpB87;});_tmp5BF;});_tmp5BE[0]=_tmpB88;});_tmp5BE;});_tmp5BD->hd=_tmpB89;});_tmp5BD->tl=_tmp5BC.tms;_tmp5BD;});_tmp5BC.tms=_tmpB8A;});
yyval=yyyyvsp[2];
# 1961
goto _LL254;}case 171: _LL3A5: _LL3A6: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1962 "parse.y"
({union Cyc_YYSTYPE _tmpB92=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C0;({struct _tuple0*_tmpB8B=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C0.id=_tmpB8B;});({unsigned int _tmpB8C=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5C0.varloc=_tmpB8C;});({struct Cyc_List_List*_tmpB91=({struct Cyc_List_List*_tmp5C1=_region_malloc(yyr,sizeof(*_tmp5C1));({void*_tmpB8F=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C2=_region_malloc(yyr,sizeof(*_tmp5C2));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB8E=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5C3;_tmp5C3.tag=0;({union Cyc_Absyn_Constraint*_tmpB8D=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C3.f1=_tmpB8D;});_tmp5C3.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5C3;});_tmp5C2[0]=_tmpB8E;});_tmp5C2;});_tmp5C1->hd=_tmpB8F;});({struct Cyc_List_List*_tmpB90=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C1->tl=_tmpB90;});_tmp5C1;});_tmp5C0.tms=_tmpB91;});_tmp5C0;}));yyval=_tmpB92;});
goto _LL254;}case 172: _LL3A7: _LL3A8: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 1964 "parse.y"
({union Cyc_YYSTYPE _tmpB9B=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C4;({struct _tuple0*_tmpB93=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C4.id=_tmpB93;});({unsigned int _tmpB94=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5C4.varloc=_tmpB94;});({struct Cyc_List_List*_tmpB9A=({struct Cyc_List_List*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));({void*_tmpB98=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB97=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5C7;_tmp5C7.tag=1;({struct Cyc_Absyn_Exp*_tmpB95=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5C7.f1=_tmpB95;});({union Cyc_Absyn_Constraint*_tmpB96=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5C7.f2=_tmpB96;});_tmp5C7.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5C7;});_tmp5C6[0]=_tmpB97;});_tmp5C6;});_tmp5C5->hd=_tmpB98;});({struct Cyc_List_List*_tmpB99=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C5->tl=_tmpB99;});_tmp5C5;});_tmp5C4.tms=_tmpB9A;});_tmp5C4;}));
# 1964
yyval=_tmpB9B;});
# 1966
goto _LL254;}case 173: _LL3A9: _LL3AA: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 1967 "parse.y"
struct _tuple26*_tmp5C8=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5C9=_tmp5C8;struct Cyc_List_List*_tmp5D6;int _tmp5D5;struct Cyc_Absyn_VarargInfo*_tmp5D4;void*_tmp5D3;struct Cyc_List_List*_tmp5D2;_LL68F: _tmp5D6=_tmp5C9->f1;_tmp5D5=_tmp5C9->f2;_tmp5D4=_tmp5C9->f3;_tmp5D3=_tmp5C9->f4;_tmp5D2=_tmp5C9->f5;_LL690:;{
struct Cyc_Absyn_Exp*_tmp5CA=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5CB=Cyc_yyget_YY55(yyyyvsp[5]);
({union Cyc_YYSTYPE _tmpBA4=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5CC;({struct _tuple0*_tmpB9C=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5CC.id=_tmpB9C;});({unsigned int _tmpB9D=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5CC.varloc=_tmpB9D;});({struct Cyc_List_List*_tmpBA3=({struct Cyc_List_List*_tmp5CD=_region_malloc(yyr,sizeof(*_tmp5CD));({void*_tmpBA1=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CE=_region_malloc(yyr,sizeof(*_tmp5CE));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBA0=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5CF;_tmp5CF.tag=3;({void*_tmpB9F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5D0=_region_malloc(yyr,sizeof(*_tmp5D0));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB9E=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5D1;_tmp5D1.tag=1;_tmp5D1.f1=_tmp5D6;_tmp5D1.f2=_tmp5D5;_tmp5D1.f3=_tmp5D4;_tmp5D1.f4=_tmp5D3;_tmp5D1.f5=_tmp5D2;_tmp5D1.f6=_tmp5CA;_tmp5D1.f7=_tmp5CB;_tmp5D1;});_tmp5D0[0]=_tmpB9E;});_tmp5D0;});_tmp5CF.f1=_tmpB9F;});_tmp5CF;});_tmp5CE[0]=_tmpBA0;});_tmp5CE;});_tmp5CD->hd=_tmpBA1;});({struct Cyc_List_List*_tmpBA2=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5CD->tl=_tmpBA2;});_tmp5CD;});_tmp5CC.tms=_tmpBA3;});_tmp5CC;}));yyval=_tmpBA4;});
# 1972
goto _LL254;};}case 174: _LL3AB: _LL3AC: {
# 1974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 1976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 1973 "parse.y"
({union Cyc_YYSTYPE _tmpBB1=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5D7;({struct _tuple0*_tmpBA5=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5D7.id=_tmpBA5;});({unsigned int _tmpBA6=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5D7.varloc=_tmpBA6;});({struct Cyc_List_List*_tmpBB0=({struct Cyc_List_List*_tmp5D8=_region_malloc(yyr,sizeof(*_tmp5D8));({void*_tmpBAE=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5D9=_region_malloc(yyr,sizeof(*_tmp5D9));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBAD=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5DA;_tmp5DA.tag=3;({void*_tmpBAC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5DB=_region_malloc(yyr,sizeof(*_tmp5DB));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBAB=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5DC;_tmp5DC.tag=1;_tmp5DC.f1=0;_tmp5DC.f2=0;_tmp5DC.f3=0;({void*_tmpBA7=
# 1976
Cyc_yyget_YY49(yyyyvsp[2]);_tmp5DC.f4=_tmpBA7;});({struct Cyc_List_List*_tmpBA8=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5DC.f5=_tmpBA8;});({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5DC.f6=_tmpBA9;});({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5DC.f7=_tmpBAA;});_tmp5DC;});_tmp5DB[0]=_tmpBAB;});_tmp5DB;});_tmp5DA.f1=_tmpBAC;});_tmp5DA;});_tmp5D9[0]=_tmpBAD;});_tmp5D9;});_tmp5D8->hd=_tmpBAE;});({struct Cyc_List_List*_tmpBAF=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5D8->tl=_tmpBAF;});_tmp5D8;});_tmp5D7.tms=_tmpBB0;});_tmp5D7;}));
# 1973
yyval=_tmpBB1;});
# 1979
goto _LL254;}case 175: _LL3AD: _LL3AE: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1980 "parse.y"
({union Cyc_YYSTYPE _tmpBBB=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5DD;({struct _tuple0*_tmpBB2=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5DD.id=_tmpBB2;});({unsigned int _tmpBB3=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5DD.varloc=_tmpBB3;});({struct Cyc_List_List*_tmpBBA=({struct Cyc_List_List*_tmp5DE=_region_malloc(yyr,sizeof(*_tmp5DE));({void*_tmpBB8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5DF=_region_malloc(yyr,sizeof(*_tmp5DF));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBB7=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5E0;_tmp5E0.tag=3;({void*_tmpBB6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp5E1=_region_malloc(yyr,sizeof(*_tmp5E1));({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBB5=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp5E2;_tmp5E2.tag=0;({struct Cyc_List_List*_tmpBB4=Cyc_yyget_YY36(yyyyvsp[2]);_tmp5E2.f1=_tmpBB4;});_tmp5E2.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5E2;});_tmp5E1[0]=_tmpBB5;});_tmp5E1;});_tmp5E0.f1=_tmpBB6;});_tmp5E0;});_tmp5DF[0]=_tmpBB7;});_tmp5DF;});_tmp5DE->hd=_tmpBB8;});({struct Cyc_List_List*_tmpBB9=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5DE->tl=_tmpBB9;});_tmp5DE;});_tmp5DD.tms=_tmpBBA;});_tmp5DD;}));yyval=_tmpBBB;});
goto _LL254;}case 176: _LL3AF: _LL3B0: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 1983 "parse.y"
struct Cyc_List_List*_tmp5E3=({unsigned int _tmpBBC=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpBBC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpBC3=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E4;({struct _tuple0*_tmpBBD=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5E4.id=_tmpBBD;});({unsigned int _tmpBBE=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5E4.varloc=_tmpBBE;});({struct Cyc_List_List*_tmpBC2=({struct Cyc_List_List*_tmp5E5=_region_malloc(yyr,sizeof(*_tmp5E5));({void*_tmpBC0=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5E6=_region_malloc(yyr,sizeof(*_tmp5E6));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBBF=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp5E7;_tmp5E7.tag=4;_tmp5E7.f1=_tmp5E3;_tmp5E7.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5E7.f3=0;_tmp5E7;});_tmp5E6[0]=_tmpBBF;});_tmp5E6;});_tmp5E5->hd=_tmpBC0;});({struct Cyc_List_List*_tmpBC1=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5E5->tl=_tmpBC1;});_tmp5E5;});_tmp5E4.tms=_tmpBC2;});_tmp5E4;}));yyval=_tmpBC3;});
# 1986
goto _LL254;}case 177: _LL3B1: _LL3B2: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 1987 "parse.y"
({union Cyc_YYSTYPE _tmpBCB=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E8;({struct _tuple0*_tmpBC4=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5E8.id=_tmpBC4;});({unsigned int _tmpBC5=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5E8.varloc=_tmpBC5;});({struct Cyc_List_List*_tmpBCA=({struct Cyc_List_List*_tmp5E9=_region_malloc(yyr,sizeof(*_tmp5E9));({void*_tmpBC8=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5EA=_region_malloc(yyr,sizeof(*_tmp5EA));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC7=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5EB;_tmp5EB.tag=5;_tmp5EB.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpBC6=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5EB.f2=_tmpBC6;});_tmp5EB;});_tmp5EA[0]=_tmpBC7;});_tmp5EA;});_tmp5E9->hd=_tmpBC8;});({struct Cyc_List_List*_tmpBC9=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5E9->tl=_tmpBC9;});_tmp5E9;});_tmp5E8.tms=_tmpBCA;});_tmp5E8;}));
# 1987
yyval=_tmpBCB;});
# 1990
goto _LL254;}case 178: _LL3B3: _LL3B4: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2001 "parse.y"
({union Cyc_YYSTYPE _tmpBCD=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5EC;({struct _tuple0*_tmpBCC=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5EC.id=_tmpBCC;});_tmp5EC.varloc=(unsigned int)(yyyylsp[0]).first_line;_tmp5EC.tms=0;_tmp5EC;}));yyval=_tmpBCD;});
goto _LL254;}case 179: _LL3B5: _LL3B6: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2003 "parse.y"
({union Cyc_YYSTYPE _tmpBCF=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5ED;({struct _tuple0*_tmpBCE=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5ED.id=_tmpBCE;});_tmp5ED.varloc=(unsigned int)(yyyylsp[0]).first_line;_tmp5ED.tms=0;_tmp5ED;}));yyval=_tmpBCF;});
goto _LL254;}case 180: _LL3B7: _LL3B8: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2005 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 181: _LL3B9: _LL3BA: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2009 "parse.y"
struct Cyc_Parse_Declarator _tmp5EE=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpBD3=({struct Cyc_List_List*_tmp5EF=_region_malloc(yyr,sizeof(*_tmp5EF));({void*_tmpBD2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5F0=_region_malloc(yyr,sizeof(*_tmp5F0));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBD1=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5F1;_tmp5F1.tag=5;_tmp5F1.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpBD0=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5F1.f2=_tmpBD0;});_tmp5F1;});_tmp5F0[0]=_tmpBD1;});_tmp5F0;});_tmp5EF->hd=_tmpBD2;});_tmp5EF->tl=_tmp5EE.tms;_tmp5EF;});_tmp5EE.tms=_tmpBD3;});
yyval=yyyyvsp[2];
# 2013
goto _LL254;}case 182: _LL3BB: _LL3BC: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2014 "parse.y"
struct Cyc_Parse_Declarator _tmp5F2=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBD8=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F3;_tmp5F3.id=_tmp5F2.id;_tmp5F3.varloc=_tmp5F2.varloc;({struct Cyc_List_List*_tmpBD7=({struct Cyc_List_List*_tmp5F4=_region_malloc(yyr,sizeof(*_tmp5F4));({void*_tmpBD6=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5F5=_region_malloc(yyr,sizeof(*_tmp5F5));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBD5=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5F6;_tmp5F6.tag=0;({union Cyc_Absyn_Constraint*_tmpBD4=
Cyc_yyget_YY51(yyyyvsp[3]);_tmp5F6.f1=_tmpBD4;});_tmp5F6.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5F6;});_tmp5F5[0]=_tmpBD5;});_tmp5F5;});_tmp5F4->hd=_tmpBD6;});_tmp5F4->tl=_tmp5F2.tms;_tmp5F4;});_tmp5F3.tms=_tmpBD7;});_tmp5F3;}));
# 2015
yyval=_tmpBD8;});
# 2017
goto _LL254;}case 183: _LL3BD: _LL3BE: {
# 2019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2018 "parse.y"
struct Cyc_Parse_Declarator _tmp5F7=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBDE=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F8;_tmp5F8.id=_tmp5F7.id;_tmp5F8.varloc=_tmp5F7.varloc;({struct Cyc_List_List*_tmpBDD=({struct Cyc_List_List*_tmp5F9=_region_malloc(yyr,sizeof(*_tmp5F9));({void*_tmpBDC=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5FA=_region_malloc(yyr,sizeof(*_tmp5FA));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBDB=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5FB;_tmp5FB.tag=1;({struct Cyc_Absyn_Exp*_tmpBD9=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5FB.f1=_tmpBD9;});({union Cyc_Absyn_Constraint*_tmpBDA=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5FB.f2=_tmpBDA;});_tmp5FB.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5FB;});_tmp5FA[0]=_tmpBDB;});_tmp5FA;});_tmp5F9->hd=_tmpBDC;});_tmp5F9->tl=_tmp5F7.tms;_tmp5F9;});_tmp5F8.tms=_tmpBDD;});_tmp5F8;}));
# 2019
yyval=_tmpBDE;});
# 2022
goto _LL254;}case 184: _LL3BF: _LL3C0: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2023 "parse.y"
struct _tuple26*_tmp5FC=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5FD=_tmp5FC;struct Cyc_List_List*_tmp60B;int _tmp60A;struct Cyc_Absyn_VarargInfo*_tmp609;void*_tmp608;struct Cyc_List_List*_tmp607;_LL692: _tmp60B=_tmp5FD->f1;_tmp60A=_tmp5FD->f2;_tmp609=_tmp5FD->f3;_tmp608=_tmp5FD->f4;_tmp607=_tmp5FD->f5;_LL693:;{
struct Cyc_Absyn_Exp*_tmp5FE=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5FF=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp600=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBE4=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp601;_tmp601.id=_tmp600.id;_tmp601.varloc=_tmp600.varloc;({struct Cyc_List_List*_tmpBE3=({struct Cyc_List_List*_tmp602=_region_malloc(yyr,sizeof(*_tmp602));({void*_tmpBE2=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp603=_region_malloc(yyr,sizeof(*_tmp603));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE1=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp604;_tmp604.tag=3;({void*_tmpBE0=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp605=_region_malloc(yyr,sizeof(*_tmp605));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBDF=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp606;_tmp606.tag=1;_tmp606.f1=_tmp60B;_tmp606.f2=_tmp60A;_tmp606.f3=_tmp609;_tmp606.f4=_tmp608;_tmp606.f5=_tmp607;_tmp606.f6=_tmp5FE;_tmp606.f7=_tmp5FF;_tmp606;});_tmp605[0]=_tmpBDF;});_tmp605;});_tmp604.f1=_tmpBE0;});_tmp604;});_tmp603[0]=_tmpBE1;});_tmp603;});_tmp602->hd=_tmpBE2;});_tmp602->tl=_tmp600.tms;_tmp602;});_tmp601.tms=_tmpBE3;});_tmp601;}));yyval=_tmpBE4;});
# 2029
goto _LL254;};}case 185: _LL3C1: _LL3C2: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2030 "parse.y"
struct Cyc_Parse_Declarator _tmp60C=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBEE=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp60D;_tmp60D.id=_tmp60C.id;_tmp60D.varloc=_tmp60C.varloc;({struct Cyc_List_List*_tmpBED=({struct Cyc_List_List*_tmp60E=_region_malloc(yyr,sizeof(*_tmp60E));({void*_tmpBEC=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp60F=_region_malloc(yyr,sizeof(*_tmp60F));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBEB=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp610;_tmp610.tag=3;({void*_tmpBEA=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp611=_region_malloc(yyr,sizeof(*_tmp611));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE9=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp612;_tmp612.tag=1;_tmp612.f1=0;_tmp612.f2=0;_tmp612.f3=0;({void*_tmpBE5=
# 2034
Cyc_yyget_YY49(yyyyvsp[2]);_tmp612.f4=_tmpBE5;});({struct Cyc_List_List*_tmpBE6=Cyc_yyget_YY50(yyyyvsp[3]);_tmp612.f5=_tmpBE6;});({struct Cyc_Absyn_Exp*_tmpBE7=Cyc_yyget_YY55(yyyyvsp[5]);_tmp612.f6=_tmpBE7;});({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_yyget_YY55(yyyyvsp[6]);_tmp612.f7=_tmpBE8;});_tmp612;});_tmp611[0]=_tmpBE9;});_tmp611;});_tmp610.f1=_tmpBEA;});_tmp610;});_tmp60F[0]=_tmpBEB;});_tmp60F;});_tmp60E->hd=_tmpBEC;});_tmp60E->tl=_tmp60C.tms;_tmp60E;});_tmp60D.tms=_tmpBED;});_tmp60D;}));
# 2031
yyval=_tmpBEE;});
# 2037
goto _LL254;}case 186: _LL3C3: _LL3C4: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2038 "parse.y"
struct Cyc_Parse_Declarator _tmp613=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBF5=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp614;_tmp614.id=_tmp613.id;_tmp614.varloc=_tmp613.varloc;({struct Cyc_List_List*_tmpBF4=({struct Cyc_List_List*_tmp615=_region_malloc(yyr,sizeof(*_tmp615));({void*_tmpBF3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp616=_region_malloc(yyr,sizeof(*_tmp616));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF2=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp617;_tmp617.tag=3;({void*_tmpBF1=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp618=_region_malloc(yyr,sizeof(*_tmp618));({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBF0=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp619;_tmp619.tag=0;({struct Cyc_List_List*_tmpBEF=Cyc_yyget_YY36(yyyyvsp[2]);_tmp619.f1=_tmpBEF;});_tmp619.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp619;});_tmp618[0]=_tmpBF0;});_tmp618;});_tmp617.f1=_tmpBF1;});_tmp617;});_tmp616[0]=_tmpBF2;});_tmp616;});_tmp615->hd=_tmpBF3;});_tmp615->tl=_tmp613.tms;_tmp615;});_tmp614.tms=_tmpBF4;});_tmp614;}));yyval=_tmpBF5;});
goto _LL254;}case 187: _LL3C5: _LL3C6: {
# 2042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2042 "parse.y"
struct Cyc_List_List*_tmp61A=({unsigned int _tmpBF6=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpBF6,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp61B=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBFA=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp61C;_tmp61C.id=_tmp61B.id;_tmp61C.varloc=_tmp61B.varloc;({struct Cyc_List_List*_tmpBF9=({struct Cyc_List_List*_tmp61D=_region_malloc(yyr,sizeof(*_tmp61D));({void*_tmpBF8=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp61E=_region_malloc(yyr,sizeof(*_tmp61E));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBF7=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp61F;_tmp61F.tag=4;_tmp61F.f1=_tmp61A;_tmp61F.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp61F.f3=0;_tmp61F;});_tmp61E[0]=_tmpBF7;});_tmp61E;});_tmp61D->hd=_tmpBF8;});_tmp61D->tl=_tmp61B.tms;_tmp61D;});_tmp61C.tms=_tmpBF9;});_tmp61C;}));yyval=_tmpBFA;});
# 2046
goto _LL254;}case 188: _LL3C7: _LL3C8: {
# 2048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2047 "parse.y"
struct Cyc_Parse_Declarator _tmp620=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBFF=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp621;_tmp621.id=_tmp620.id;_tmp621.varloc=_tmp620.varloc;({struct Cyc_List_List*_tmpBFE=({struct Cyc_List_List*_tmp622=_region_malloc(yyr,sizeof(*_tmp622));({void*_tmpBFD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp623=_region_malloc(yyr,sizeof(*_tmp623));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBFC=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp624;_tmp624.tag=5;_tmp624.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpBFB=Cyc_yyget_YY45(yyyyvsp[1]);_tmp624.f2=_tmpBFB;});_tmp624;});_tmp623[0]=_tmpBFC;});_tmp623;});_tmp622->hd=_tmpBFD;});_tmp622->tl=_tmp620.tms;_tmp622;});_tmp621.tms=_tmpBFE;});_tmp621;}));yyval=_tmpBFF;});
# 2050
goto _LL254;}case 189: _LL3C9: _LL3CA: {
# 2052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2060 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 190: _LL3CB: _LL3CC: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2061 "parse.y"
({union Cyc_YYSTYPE _tmpC01=Cyc_YY26(({struct Cyc_List_List*_tmpC00=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC00,Cyc_yyget_YY26(yyyyvsp[1]));}));yyval=_tmpC01;});
goto _LL254;}case 191: _LL3CD: _LL3CE: {
# 2064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2065 "parse.y"
struct Cyc_List_List*ans=0;
if(({Cyc_yyget_YY45(yyyyvsp[3])!= 0;}))
({struct Cyc_List_List*_tmpC05=({struct Cyc_List_List*_tmp625=_region_malloc(yyr,sizeof(*_tmp625));({void*_tmpC04=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp626=_region_malloc(yyr,sizeof(*_tmp626));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC03=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp627;_tmp627.tag=5;_tmp627.f1=(unsigned int)(yyyylsp[3]).first_line;({struct Cyc_List_List*_tmpC02=Cyc_yyget_YY45(yyyyvsp[3]);_tmp627.f2=_tmpC02;});_tmp627;});_tmp626[0]=_tmpC03;});_tmp626;});_tmp625->hd=_tmpC04;});_tmp625->tl=ans;_tmp625;});ans=_tmpC05;});{
# 2069
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp628=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp629=_tmp628;unsigned int _tmp637;union Cyc_Absyn_Constraint*_tmp636;union Cyc_Absyn_Constraint*_tmp635;_LL695: _tmp637=_tmp629.f1;_tmp636=_tmp629.f2;_tmp635=_tmp629.f3;_LL696:;
if(Cyc_Absyn_porting_c_code)
({struct Cyc_Absyn_PtrLoc*_tmpC06=({struct Cyc_Absyn_PtrLoc*_tmp62A=_cycalloc_atomic(sizeof(*_tmp62A));_tmp62A->ptr_loc=_tmp637;_tmp62A->rgn_loc=(unsigned int)(yyyylsp[2]).first_line;_tmp62A->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp62A;});ptrloc=_tmpC06;});{
# 2074
struct _tuple14 _tmp62B=({unsigned int _tmpC0A=_tmp637;union Cyc_Absyn_Constraint*_tmpC09=_tmp636;union Cyc_Absyn_Constraint*_tmpC08=_tmp635;void*_tmpC07=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmpC0A,_tmpC09,_tmpC08,_tmpC07,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp62C=_tmp62B;union Cyc_Absyn_Constraint*_tmp634;union Cyc_Absyn_Constraint*_tmp633;union Cyc_Absyn_Constraint*_tmp632;void*_tmp631;_LL698: _tmp634=_tmp62C.f1;_tmp633=_tmp62C.f2;_tmp632=_tmp62C.f3;_tmp631=_tmp62C.f4;_LL699:;
({struct Cyc_List_List*_tmpC0F=({struct Cyc_List_List*_tmp62D=_region_malloc(yyr,sizeof(*_tmp62D));({void*_tmpC0E=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp62E=_region_malloc(yyr,sizeof(*_tmp62E));({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC0D=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp62F;_tmp62F.tag=2;({struct Cyc_Absyn_PtrAtts _tmpC0B=({struct Cyc_Absyn_PtrAtts _tmp630;_tmp630.rgn=_tmp631;_tmp630.nullable=_tmp634;_tmp630.bounds=_tmp633;_tmp630.zero_term=_tmp632;_tmp630.ptrloc=ptrloc;_tmp630;});_tmp62F.f1=_tmpC0B;});({struct Cyc_Absyn_Tqual _tmpC0C=Cyc_yyget_YY23(yyyyvsp[4]);_tmp62F.f2=_tmpC0C;});_tmp62F;});_tmp62E[0]=_tmpC0D;});_tmp62E;});_tmp62D->hd=_tmpC0E;});_tmp62D->tl=ans;_tmp62D;});ans=_tmpC0F;});
({union Cyc_YYSTYPE _tmpC10=Cyc_YY26(ans);yyval=_tmpC10;});
# 2078
goto _LL254;};};}case 192: _LL3CF: _LL3D0:
# 2080
({union Cyc_YYSTYPE _tmpC11=Cyc_YY54(0);yyval=_tmpC11;});
goto _LL254;case 193: _LL3D1: _LL3D2: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2081 "parse.y"
({union Cyc_YYSTYPE _tmpC14=Cyc_YY54(({struct Cyc_List_List*_tmp638=_region_malloc(yyr,sizeof(*_tmp638));({void*_tmpC12=Cyc_yyget_YY53(yyyyvsp[0]);_tmp638->hd=_tmpC12;});({struct Cyc_List_List*_tmpC13=Cyc_yyget_YY54(yyyyvsp[1]);_tmp638->tl=_tmpC13;});_tmp638;}));yyval=_tmpC14;});
goto _LL254;}case 194: _LL3D3: _LL3D4: {
# 2084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2086 "parse.y"
({union Cyc_YYSTYPE _tmpC17=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp639=_region_malloc(yyr,sizeof(*_tmp639));({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct _tmpC16=({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct _tmp63A;_tmp63A.tag=0;({struct Cyc_Absyn_Exp*_tmpC15=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp63A.f1=_tmpC15;});_tmp63A;});_tmp639[0]=_tmpC16;});_tmp639;}));yyval=_tmpC17;});
goto _LL254;}case 195: _LL3D5: _LL3D6: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2088 "parse.y"
({union Cyc_YYSTYPE _tmpC1A=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp63B=_region_malloc(yyr,sizeof(*_tmp63B));({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct _tmpC19=({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct _tmp63C;_tmp63C.tag=1;({void*_tmpC18=Cyc_yyget_YY44(yyyyvsp[2]);_tmp63C.f1=_tmpC18;});_tmp63C;});_tmp63B[0]=_tmpC19;});_tmp63B;}));yyval=_tmpC1A;});
goto _LL254;}case 196: _LL3D7: _LL3D8: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2090 "parse.y"
({union Cyc_YYSTYPE _tmpC1C=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp63D=_region_malloc(yyr,sizeof(*_tmp63D));({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct _tmpC1B=({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct _tmp63E;_tmp63E.tag=2;_tmp63E;});_tmp63D[0]=_tmpC1B;});_tmp63D;}));yyval=_tmpC1C;});
goto _LL254;}case 197: _LL3D9: _LL3DA: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2092 "parse.y"
({union Cyc_YYSTYPE _tmpC1E=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp63F=_region_malloc(yyr,sizeof(*_tmp63F));({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct _tmpC1D=({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct _tmp640;_tmp640.tag=3;_tmp640;});_tmp63F[0]=_tmpC1D;});_tmp63F;}));yyval=_tmpC1E;});
goto _LL254;}case 198: _LL3DB: _LL3DC: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2094 "parse.y"
({union Cyc_YYSTYPE _tmpC20=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp641=_region_malloc(yyr,sizeof(*_tmp641));({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct _tmpC1F=({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct _tmp642;_tmp642.tag=4;_tmp642;});_tmp641[0]=_tmpC1F;});_tmp641;}));yyval=_tmpC20;});
goto _LL254;}case 199: _LL3DD: _LL3DE: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2096 "parse.y"
({union Cyc_YYSTYPE _tmpC22=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp643=_region_malloc(yyr,sizeof(*_tmp643));({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct _tmpC21=({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct _tmp644;_tmp644.tag=5;_tmp644;});_tmp643[0]=_tmpC21;});_tmp643;}));yyval=_tmpC22;});
goto _LL254;}case 200: _LL3DF: _LL3E0: {
# 2099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2098 "parse.y"
({union Cyc_YYSTYPE _tmpC24=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp645=_region_malloc(yyr,sizeof(*_tmp645));({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct _tmpC23=({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct _tmp646;_tmp646.tag=6;_tmp646;});_tmp645[0]=_tmpC23;});_tmp645;}));yyval=_tmpC24;});
goto _LL254;}case 201: _LL3E1: _LL3E2: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2100 "parse.y"
({union Cyc_YYSTYPE _tmpC26=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp647=_region_malloc(yyr,sizeof(*_tmp647));({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct _tmpC25=({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct _tmp648;_tmp648.tag=7;_tmp648;});_tmp647[0]=_tmpC25;});_tmp647;}));yyval=_tmpC26;});
goto _LL254;}case 202: _LL3E3: _LL3E4: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2106 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpC28=Cyc_YY1(({struct _tuple21*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=loc;_tmp649->f2=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmpC27=Cyc_yyget_YY2(yyyyvsp[1]);_tmp649->f3=_tmpC27;});_tmp649;}));yyval=_tmpC28;});
# 2109
goto _LL254;}case 203: _LL3E5: _LL3E6: {
# 2111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2110 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpC2A=Cyc_YY1(({struct _tuple21*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->f1=loc;_tmp64A->f2=Cyc_Absyn_false_conref;({union Cyc_Absyn_Constraint*_tmpC29=Cyc_yyget_YY2(yyyyvsp[1]);_tmp64A->f3=_tmpC29;});_tmp64A;}));yyval=_tmpC2A;});
# 2113
goto _LL254;}case 204: _LL3E7: _LL3E8: {
# 2115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2114 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpC2B=Cyc_YY1(({struct _tuple21*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->f1=loc;_tmp64B->f2=Cyc_Absyn_true_conref;_tmp64B->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp64B;}));yyval=_tmpC2B;});
# 2117
goto _LL254;}case 205: _LL3E9: _LL3EA:
# 2119
({union Cyc_YYSTYPE _tmpC2C=Cyc_YY2(Cyc_Absyn_bounds_one_conref);yyval=_tmpC2C;});
goto _LL254;case 206: _LL3EB: _LL3EC: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2120 "parse.y"
({union Cyc_YYSTYPE _tmpC2F=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64C=_cycalloc(sizeof(*_tmp64C));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC2E=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp64D;_tmp64D.tag=1;({struct Cyc_Absyn_Exp*_tmpC2D=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp64D.f1=_tmpC2D;});_tmp64D;});_tmp64C[0]=_tmpC2E;});_tmp64C;})));yyval=_tmpC2F;});
goto _LL254;}case 207: _LL3ED: _LL3EE:
# 2123
({union Cyc_YYSTYPE _tmpC30=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());yyval=_tmpC30;});
goto _LL254;case 208: _LL3EF: _LL3F0: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2124 "parse.y"
({union Cyc_YYSTYPE _tmpC31=Cyc_YY51(Cyc_Absyn_true_conref);yyval=_tmpC31;});
goto _LL254;}case 209: _LL3F1: _LL3F2: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2125 "parse.y"
({union Cyc_YYSTYPE _tmpC32=Cyc_YY51(Cyc_Absyn_false_conref);yyval=_tmpC32;});
goto _LL254;}case 210: _LL3F3: _LL3F4:
# 2128
({union Cyc_YYSTYPE _tmpC33=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpC33;});
goto _LL254;case 211: _LL3F5: _LL3F6: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2131 "parse.y"
({Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);});yyval=yyyyvsp[0];
goto _LL254;}case 212: _LL3F7: _LL3F8: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2132 "parse.y"
({union Cyc_YYSTYPE _tmpC34=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpC34;});
goto _LL254;}case 213: _LL3F9: _LL3FA:
# 2135
({union Cyc_YYSTYPE _tmpC35=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));yyval=_tmpC35;});
goto _LL254;case 214: _LL3FB: _LL3FC: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2137 "parse.y"
({union Cyc_YYSTYPE _tmpC37=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmpC36=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpC36,Cyc_yyget_YY23(yyyyvsp[1]));}));yyval=_tmpC37;});
goto _LL254;}case 215: _LL3FD: _LL3FE: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2142 "parse.y"
({union Cyc_YYSTYPE _tmpC3B=Cyc_YY39(({struct _tuple26*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct Cyc_List_List*_tmpC38=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp64E->f1=_tmpC38;});_tmp64E->f2=0;_tmp64E->f3=0;({void*_tmpC39=Cyc_yyget_YY49(yyyyvsp[1]);_tmp64E->f4=_tmpC39;});({struct Cyc_List_List*_tmpC3A=Cyc_yyget_YY50(yyyyvsp[2]);_tmp64E->f5=_tmpC3A;});_tmp64E;}));yyval=_tmpC3B;});
goto _LL254;}case 216: _LL3FF: _LL400: {
# 2145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2144 "parse.y"
({union Cyc_YYSTYPE _tmpC3F=Cyc_YY39(({struct _tuple26*_tmp64F=_cycalloc(sizeof(*_tmp64F));({struct Cyc_List_List*_tmpC3C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp64F->f1=_tmpC3C;});_tmp64F->f2=1;_tmp64F->f3=0;({void*_tmpC3D=Cyc_yyget_YY49(yyyyvsp[3]);_tmp64F->f4=_tmpC3D;});({struct Cyc_List_List*_tmpC3E=Cyc_yyget_YY50(yyyyvsp[4]);_tmp64F->f5=_tmpC3E;});_tmp64F;}));yyval=_tmpC3F;});
goto _LL254;}case 217: _LL401: _LL402: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2147 "parse.y"
struct _tuple8*_tmp650=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp651=_tmp650;struct _dyneither_ptr*_tmp657;struct Cyc_Absyn_Tqual _tmp656;void*_tmp655;_LL69B: _tmp657=_tmp651->f1;_tmp656=_tmp651->f2;_tmp655=_tmp651->f3;_LL69C:;{
struct Cyc_Absyn_VarargInfo*_tmp652=({struct Cyc_Absyn_VarargInfo*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->name=_tmp657;_tmp654->tq=_tmp656;_tmp654->type=_tmp655;({int _tmpC40=Cyc_yyget_YY31(yyyyvsp[1]);_tmp654->inject=_tmpC40;});_tmp654;});
({union Cyc_YYSTYPE _tmpC43=Cyc_YY39(({struct _tuple26*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->f1=0;_tmp653->f2=0;_tmp653->f3=_tmp652;({void*_tmpC41=Cyc_yyget_YY49(yyyyvsp[3]);_tmp653->f4=_tmpC41;});({struct Cyc_List_List*_tmpC42=Cyc_yyget_YY50(yyyyvsp[4]);_tmp653->f5=_tmpC42;});_tmp653;}));yyval=_tmpC43;});
# 2151
goto _LL254;};}case 218: _LL403: _LL404: {
# 2153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2153 "parse.y"
struct _tuple8*_tmp658=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp659=_tmp658;struct _dyneither_ptr*_tmp65F;struct Cyc_Absyn_Tqual _tmp65E;void*_tmp65D;_LL69E: _tmp65F=_tmp659->f1;_tmp65E=_tmp659->f2;_tmp65D=_tmp659->f3;_LL69F:;{
struct Cyc_Absyn_VarargInfo*_tmp65A=({struct Cyc_Absyn_VarargInfo*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->name=_tmp65F;_tmp65C->tq=_tmp65E;_tmp65C->type=_tmp65D;({int _tmpC44=Cyc_yyget_YY31(yyyyvsp[3]);_tmp65C->inject=_tmpC44;});_tmp65C;});
({union Cyc_YYSTYPE _tmpC48=Cyc_YY39(({struct _tuple26*_tmp65B=_cycalloc(sizeof(*_tmp65B));({struct Cyc_List_List*_tmpC45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp65B->f1=_tmpC45;});_tmp65B->f2=0;_tmp65B->f3=_tmp65A;({void*_tmpC46=Cyc_yyget_YY49(yyyyvsp[5]);_tmp65B->f4=_tmpC46;});({struct Cyc_List_List*_tmpC47=Cyc_yyget_YY50(yyyyvsp[6]);_tmp65B->f5=_tmpC47;});_tmp65B;}));yyval=_tmpC48;});
# 2157
goto _LL254;};}case 219: _LL405: _LL406: {
# 2159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2161 "parse.y"
({union Cyc_YYSTYPE _tmpC4B=Cyc_YY44(({struct _dyneither_ptr _tmpC4A=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpC4A,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp660=_cycalloc(sizeof(*_tmp660));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC49=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp661;_tmp661.tag=1;_tmp661.f1=0;_tmp661;});_tmp660[0]=_tmpC49;});_tmp660;}));}));yyval=_tmpC4B;});
goto _LL254;}case 220: _LL407: _LL408: {
# 2164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2162 "parse.y"
({union Cyc_YYSTYPE _tmpC4D=Cyc_YY44(({struct _dyneither_ptr _tmpC4C=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpC4C,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));yyval=_tmpC4D;});
goto _LL254;}case 221: _LL409: _LL40A:
# 2165
({union Cyc_YYSTYPE _tmpC4E=Cyc_YY49(0);yyval=_tmpC4E;});
goto _LL254;case 222: _LL40B: _LL40C: {
# 2168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2166 "parse.y"
({union Cyc_YYSTYPE _tmpC51=Cyc_YY49((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp662=_cycalloc(sizeof(*_tmp662));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC50=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp663;_tmp663.tag=24;({struct Cyc_List_List*_tmpC4F=Cyc_yyget_YY40(yyyyvsp[1]);_tmp663.f1=_tmpC4F;});_tmp663;});_tmp662[0]=_tmpC50;});_tmp662;}));yyval=_tmpC51;});
goto _LL254;}case 223: _LL40D: _LL40E:
# 2169
({union Cyc_YYSTYPE _tmpC52=Cyc_YY50(0);yyval=_tmpC52;});
goto _LL254;case 224: _LL40F: _LL410: {
# 2172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2171 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 225: _LL411: _LL412: {
# 2174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2179 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp664=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC53=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp66C;_tmp66C.tag=2;_tmp66C.f1=0;_tmp66C.f2=& Cyc_Tcutil_trk;_tmp66C;});_tmp66B[0]=_tmpC53;});_tmp66B;});
struct _dyneither_ptr _tmp665=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp666=Cyc_Parse_id2type(_tmp665,(void*)_tmp664);
({union Cyc_YYSTYPE _tmpC58=Cyc_YY50(({struct Cyc_List_List*_tmp667=_cycalloc(sizeof(*_tmp667));({struct _tuple29*_tmpC57=({struct _tuple29*_tmp668=_cycalloc(sizeof(*_tmp668));({void*_tmpC56=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp669=_cycalloc(sizeof(*_tmp669));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC55=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp66A;_tmp66A.tag=24;({struct Cyc_List_List*_tmpC54=Cyc_yyget_YY40(yyyyvsp[0]);_tmp66A.f1=_tmpC54;});_tmp66A;});_tmp669[0]=_tmpC55;});_tmp669;});_tmp668->f1=_tmpC56;});_tmp668->f2=_tmp666;_tmp668;});_tmp667->hd=_tmpC57;});_tmp667->tl=0;_tmp667;}));yyval=_tmpC58;});
# 2184
goto _LL254;}case 226: _LL413: _LL414: {
# 2186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2186 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp674=_cycalloc(sizeof(*_tmp674));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC59=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp675;_tmp675.tag=2;_tmp675.f1=0;_tmp675.f2=& Cyc_Tcutil_trk;_tmp675;});_tmp674[0]=_tmpC59;});_tmp674;});
struct _dyneither_ptr _tmp66E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp66F=Cyc_Parse_id2type(_tmp66E,(void*)_tmp66D);
({union Cyc_YYSTYPE _tmpC5F=Cyc_YY50(({struct Cyc_List_List*_tmp670=_cycalloc(sizeof(*_tmp670));({struct _tuple29*_tmpC5D=({struct _tuple29*_tmp671=_cycalloc(sizeof(*_tmp671));({void*_tmpC5C=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp672=_cycalloc(sizeof(*_tmp672));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5B=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp673;_tmp673.tag=24;({struct Cyc_List_List*_tmpC5A=Cyc_yyget_YY40(yyyyvsp[0]);_tmp673.f1=_tmpC5A;});_tmp673;});_tmp672[0]=_tmpC5B;});_tmp672;});_tmp671->f1=_tmpC5C;});_tmp671->f2=_tmp66F;_tmp671;});_tmp670->hd=_tmpC5D;});({struct Cyc_List_List*_tmpC5E=Cyc_yyget_YY50(yyyyvsp[4]);_tmp670->tl=_tmpC5E;});_tmp670;}));yyval=_tmpC5F;});
# 2191
goto _LL254;}case 227: _LL415: _LL416:
# 2193
({union Cyc_YYSTYPE _tmpC60=Cyc_YY31(0);yyval=_tmpC60;});
goto _LL254;case 228: _LL417: _LL418: {
# 2196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2197 "parse.y"
if(({({struct _dyneither_ptr _tmpC61=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmpC61,({const char*_tmp676="inject";_tag_dyneither(_tmp676,sizeof(char),7);}));})!= 0;}))
({void*_tmp677=0;({unsigned int _tmpC63=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC62=({const char*_tmp678="missing type in function declaration";_tag_dyneither(_tmp678,sizeof(char),37);});Cyc_Warn_err(_tmpC63,_tmpC62,_tag_dyneither(_tmp677,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpC64=Cyc_YY31(1);yyval=_tmpC64;});
# 2201
goto _LL254;}case 229: _LL419: _LL41A: {
# 2203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2204 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 230: _LL41B: _LL41C: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2205 "parse.y"
({union Cyc_YYSTYPE _tmpC66=Cyc_YY40(({struct Cyc_List_List*_tmpC65=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC65,Cyc_yyget_YY40(yyyyvsp[2]));}));yyval=_tmpC66;});
goto _LL254;}case 231: _LL41D: _LL41E: {
# 2208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2209 "parse.y"
({union Cyc_YYSTYPE _tmpC67=Cyc_YY40(0);yyval=_tmpC67;});
goto _LL254;}case 232: _LL41F: _LL420: {
# 2212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2210 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 233: _LL421: _LL422: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2212 "parse.y"
({union Cyc_YYSTYPE _tmpC6B=Cyc_YY40(({struct Cyc_List_List*_tmp679=_cycalloc(sizeof(*_tmp679));({void*_tmpC6A=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp67A=_cycalloc(sizeof(*_tmp67A));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC69=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp67B;_tmp67B.tag=25;({void*_tmpC68=Cyc_yyget_YY44(yyyyvsp[2]);_tmp67B.f1=_tmpC68;});_tmp67B;});_tmp67A[0]=_tmpC69;});_tmp67A;});_tmp679->hd=_tmpC6A;});_tmp679->tl=0;_tmp679;}));yyval=_tmpC6B;});
goto _LL254;}case 234: _LL423: _LL424: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2214 "parse.y"
({Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);});
({union Cyc_YYSTYPE _tmpC6D=Cyc_YY40(({struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*_tmp67C));({void*_tmpC6C=Cyc_yyget_YY44(yyyyvsp[0]);_tmp67C->hd=_tmpC6C;});_tmp67C->tl=0;_tmp67C;}));yyval=_tmpC6D;});
# 2217
goto _LL254;}case 235: _LL425: _LL426: {
# 2219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2234 "parse.y"
({union Cyc_YYSTYPE _tmpC72=Cyc_YY40(({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));({void*_tmpC71=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC70=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp67F;_tmp67F.tag=23;({void*_tmpC6F=({struct _tuple8*_tmpC6E=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpC6E,(unsigned int)(yyyylsp[0]).first_line);});_tmp67F.f1=_tmpC6F;});_tmp67F;});_tmp67E[0]=_tmpC70;});_tmp67E;});_tmp67D->hd=_tmpC71;});_tmp67D->tl=0;_tmp67D;}));yyval=_tmpC72;});
goto _LL254;}case 236: _LL427: _LL428: {
# 2237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2236 "parse.y"
({union Cyc_YYSTYPE _tmpC78=Cyc_YY40(({struct Cyc_List_List*_tmp680=_cycalloc(sizeof(*_tmp680));({void*_tmpC76=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp681=_cycalloc(sizeof(*_tmp681));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC75=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp682;_tmp682.tag=23;({void*_tmpC74=({struct _tuple8*_tmpC73=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpC73,(unsigned int)(yyyylsp[0]).first_line);});_tmp682.f1=_tmpC74;});_tmp682;});_tmp681[0]=_tmpC75;});_tmp681;});_tmp680->hd=_tmpC76;});({struct Cyc_List_List*_tmpC77=Cyc_yyget_YY40(yyyyvsp[2]);_tmp680->tl=_tmpC77;});_tmp680;}));yyval=_tmpC78;});
goto _LL254;}case 237: _LL429: _LL42A: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpC7A=Cyc_YY38(({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));({struct _tuple8*_tmpC79=Cyc_yyget_YY37(yyyyvsp[0]);_tmp683->hd=_tmpC79;});_tmp683->tl=0;_tmp683;}));yyval=_tmpC7A;});
goto _LL254;}case 238: _LL42B: _LL42C: {
# 2245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2244 "parse.y"
({union Cyc_YYSTYPE _tmpC7D=Cyc_YY38(({struct Cyc_List_List*_tmp684=_cycalloc(sizeof(*_tmp684));({struct _tuple8*_tmpC7B=Cyc_yyget_YY37(yyyyvsp[2]);_tmp684->hd=_tmpC7B;});({struct Cyc_List_List*_tmpC7C=Cyc_yyget_YY38(yyyyvsp[0]);_tmp684->tl=_tmpC7C;});_tmp684;}));yyval=_tmpC7D;});
goto _LL254;}case 239: _LL42D: _LL42E: {
# 2247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
struct _tuple25 _tmp685=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp686=_tmp685;struct Cyc_Absyn_Tqual _tmp69C;struct Cyc_Parse_Type_specifier _tmp69B;struct Cyc_List_List*_tmp69A;_LL6A1: _tmp69C=_tmp686.f1;_tmp69B=_tmp686.f2;_tmp69A=_tmp686.f3;_LL6A2:;
if(_tmp69C.loc == 0)_tmp69C.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp687=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp688=_tmp687;struct _tuple0*_tmp699;unsigned int _tmp698;struct Cyc_List_List*_tmp697;_LL6A4: _tmp699=_tmp688.id;_tmp698=_tmp688.varloc;_tmp697=_tmp688.tms;_LL6A5:;{
void*_tmp689=Cyc_Parse_speclist2typ(_tmp69B,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp68A=Cyc_Parse_apply_tms(_tmp69C,_tmp689,_tmp69A,_tmp697);struct _tuple13 _tmp68B=_tmp68A;struct Cyc_Absyn_Tqual _tmp696;void*_tmp695;struct Cyc_List_List*_tmp694;struct Cyc_List_List*_tmp693;_LL6A7: _tmp696=_tmp68B.f1;_tmp695=_tmp68B.f2;_tmp694=_tmp68B.f3;_tmp693=_tmp68B.f4;_LL6A8:;
if(_tmp694 != 0)
({void*_tmp68C=0;({unsigned int _tmpC7F=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpC7E=({const char*_tmp68D="parameter with bad type params";_tag_dyneither(_tmp68D,sizeof(char),31);});Cyc_Warn_err(_tmpC7F,_tmpC7E,_tag_dyneither(_tmp68C,sizeof(void*),0));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp699))
({void*_tmp68E=0;({unsigned int _tmpC81=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC80=({const char*_tmp68F="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp68F,sizeof(char),47);});Cyc_Warn_err(_tmpC81,_tmpC80,_tag_dyneither(_tmp68E,sizeof(void*),0));});});{
struct _dyneither_ptr*idopt=(*_tmp699).f2;
if(_tmp693 != 0)
({void*_tmp690=0;({unsigned int _tmpC83=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC82=({const char*_tmp691="extra attributes on parameter, ignoring";_tag_dyneither(_tmp691,sizeof(char),40);});Cyc_Warn_warn(_tmpC83,_tmpC82,_tag_dyneither(_tmp690,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpC84=Cyc_YY37(({struct _tuple8*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->f1=idopt;_tmp692->f2=_tmp696;_tmp692->f3=_tmp695;_tmp692;}));yyval=_tmpC84;});
# 2264
goto _LL254;};};};}case 240: _LL42F: _LL430: {
# 2266
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2268
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2265 "parse.y"
struct _tuple25 _tmp69D=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp69E=_tmp69D;struct Cyc_Absyn_Tqual _tmp6A5;struct Cyc_Parse_Type_specifier _tmp6A4;struct Cyc_List_List*_tmp6A3;_LL6AA: _tmp6A5=_tmp69E.f1;_tmp6A4=_tmp69E.f2;_tmp6A3=_tmp69E.f3;_LL6AB:;
if(_tmp6A5.loc == 0)_tmp6A5.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp69F=Cyc_Parse_speclist2typ(_tmp6A4,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6A3 != 0)
({void*_tmp6A0=0;({unsigned int _tmpC86=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC85=({const char*_tmp6A1="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6A1,sizeof(char),38);});Cyc_Warn_warn(_tmpC86,_tmpC85,_tag_dyneither(_tmp6A0,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpC87=Cyc_YY37(({struct _tuple8*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->f1=0;_tmp6A2->f2=_tmp6A5;_tmp6A2->f3=_tmp69F;_tmp6A2;}));yyval=_tmpC87;});
# 2272
goto _LL254;};}case 241: _LL431: _LL432: {
# 2274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2273 "parse.y"
struct _tuple25 _tmp6A6=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6A7=_tmp6A6;struct Cyc_Absyn_Tqual _tmp6B7;struct Cyc_Parse_Type_specifier _tmp6B6;struct Cyc_List_List*_tmp6B5;_LL6AD: _tmp6B7=_tmp6A7.f1;_tmp6B6=_tmp6A7.f2;_tmp6B5=_tmp6A7.f3;_LL6AE:;
if(_tmp6B7.loc == 0)_tmp6B7.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6A8=Cyc_Parse_speclist2typ(_tmp6B6,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6A9=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6AA=Cyc_Parse_apply_tms(_tmp6B7,_tmp6A8,_tmp6B5,_tmp6A9);struct _tuple13 _tmp6AB=_tmp6AA;struct Cyc_Absyn_Tqual _tmp6B4;void*_tmp6B3;struct Cyc_List_List*_tmp6B2;struct Cyc_List_List*_tmp6B1;_LL6B0: _tmp6B4=_tmp6AB.f1;_tmp6B3=_tmp6AB.f2;_tmp6B2=_tmp6AB.f3;_tmp6B1=_tmp6AB.f4;_LL6B1:;
if(_tmp6B2 != 0)
({void*_tmp6AC=0;({unsigned int _tmpC89=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC88=({const char*_tmp6AD="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp6AD,sizeof(char),49);});Cyc_Warn_warn(_tmpC89,_tmpC88,_tag_dyneither(_tmp6AC,sizeof(void*),0));});});
# 2281
if(_tmp6B1 != 0)
({void*_tmp6AE=0;({unsigned int _tmpC8B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC8A=({const char*_tmp6AF="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6AF,sizeof(char),38);});Cyc_Warn_warn(_tmpC8B,_tmpC8A,_tag_dyneither(_tmp6AE,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpC8C=Cyc_YY37(({struct _tuple8*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->f1=0;_tmp6B0->f2=_tmp6B4;_tmp6B0->f3=_tmp6B3;_tmp6B0;}));yyval=_tmpC8C;});
# 2285
goto _LL254;};}case 242: _LL433: _LL434: {
# 2287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2289 "parse.y"
({union Cyc_YYSTYPE _tmpC8D=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));yyval=_tmpC8D;});
goto _LL254;}case 243: _LL435: _LL436: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2293 "parse.y"
({union Cyc_YYSTYPE _tmpC90=Cyc_YY36(({struct Cyc_List_List*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));({struct _dyneither_ptr*_tmpC8F=({struct _dyneither_ptr*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));({struct _dyneither_ptr _tmpC8E=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6B9[0]=_tmpC8E;});_tmp6B9;});_tmp6B8->hd=_tmpC8F;});_tmp6B8->tl=0;_tmp6B8;}));yyval=_tmpC90;});
goto _LL254;}case 244: _LL437: _LL438: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2295 "parse.y"
({union Cyc_YYSTYPE _tmpC94=Cyc_YY36(({struct Cyc_List_List*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct _dyneither_ptr*_tmpC92=({struct _dyneither_ptr*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));({struct _dyneither_ptr _tmpC91=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6BB[0]=_tmpC91;});_tmp6BB;});_tmp6BA->hd=_tmpC92;});({struct Cyc_List_List*_tmpC93=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6BA->tl=_tmpC93;});_tmp6BA;}));yyval=_tmpC94;});
goto _LL254;}case 245: _LL439: _LL43A: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2299 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 246: _LL43B: _LL43C: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2300 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 247: _LL43D: _LL43E: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2305 "parse.y"
({union Cyc_YYSTYPE _tmpC97=Cyc_Exp_tok(({void*_tmpC96=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC95=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6BD;_tmp6BD.tag=35;_tmp6BD.f1=0;_tmp6BD.f2=0;_tmp6BD;});_tmp6BC[0]=_tmpC95;});_tmp6BC;});Cyc_Absyn_new_exp(_tmpC96,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC97;});
goto _LL254;}case 248: _LL43F: _LL440: {
# 2308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2307 "parse.y"
({union Cyc_YYSTYPE _tmpC9B=Cyc_Exp_tok(({void*_tmpC9A=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC99=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6BF;_tmp6BF.tag=35;_tmp6BF.f1=0;({struct Cyc_List_List*_tmpC98=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6BF.f2=_tmpC98;});_tmp6BF;});_tmp6BE[0]=_tmpC99;});_tmp6BE;});Cyc_Absyn_new_exp(_tmpC9A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC9B;});
goto _LL254;}case 249: _LL441: _LL442: {
# 2310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2309 "parse.y"
({union Cyc_YYSTYPE _tmpC9F=Cyc_Exp_tok(({void*_tmpC9E=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9D=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6C1;_tmp6C1.tag=35;_tmp6C1.f1=0;({struct Cyc_List_List*_tmpC9C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6C1.f2=_tmpC9C;});_tmp6C1;});_tmp6C0[0]=_tmpC9D;});_tmp6C0;});Cyc_Absyn_new_exp(_tmpC9E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC9F;});
goto _LL254;}case 250: _LL443: _LL444: {
# 2312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2311 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp6C2=({unsigned int _tmpCA4=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmpCA3=({struct _tuple0*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpCA1=({struct _dyneither_ptr*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));({struct _dyneither_ptr _tmpCA0=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6C6[0]=_tmpCA0;});_tmp6C6;});_tmp6C5->f2=_tmpCA1;});_tmp6C5;});void*_tmpCA2=Cyc_Absyn_uint_typ;Cyc_Absyn_new_vardecl(_tmpCA4,_tmpCA3,_tmpCA2,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));});
# 2314
(_tmp6C2->tq).real_const=1;
({union Cyc_YYSTYPE _tmpCA9=Cyc_Exp_tok(({void*_tmpCA8=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCA7=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp6C4;_tmp6C4.tag=26;_tmp6C4.f1=_tmp6C2;({struct Cyc_Absyn_Exp*_tmpCA5=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6C4.f2=_tmpCA5;});({struct Cyc_Absyn_Exp*_tmpCA6=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp6C4.f3=_tmpCA6;});_tmp6C4.f4=0;_tmp6C4;});_tmp6C3[0]=_tmpCA7;});_tmp6C3;});Cyc_Absyn_new_exp(_tmpCA8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCA9;});
# 2317
goto _LL254;}case 251: _LL445: _LL446: {
# 2319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2319 "parse.y"
void*_tmp6C7=({struct _tuple8*_tmpCAA=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpCAA,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpCAE=Cyc_Exp_tok(({void*_tmpCAD=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCAC=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp6C9;_tmp6C9.tag=27;({struct Cyc_Absyn_Exp*_tmpCAB=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6C9.f1=_tmpCAB;});_tmp6C9.f2=_tmp6C7;_tmp6C9.f3=0;_tmp6C9;});_tmp6C8[0]=_tmpCAC;});_tmp6C8;});Cyc_Absyn_new_exp(_tmpCAD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCAE;});
# 2322
goto _LL254;}case 252: _LL447: _LL448: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpCB1=Cyc_YY5(({struct Cyc_List_List*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));({struct _tuple30*_tmpCB0=({struct _tuple30*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->f1=0;({struct Cyc_Absyn_Exp*_tmpCAF=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp6CB->f2=_tmpCAF;});_tmp6CB;});_tmp6CA->hd=_tmpCB0;});_tmp6CA->tl=0;_tmp6CA;}));yyval=_tmpCB1;});
goto _LL254;}case 253: _LL449: _LL44A: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2329 "parse.y"
({union Cyc_YYSTYPE _tmpCB5=Cyc_YY5(({struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));({struct _tuple30*_tmpCB4=({struct _tuple30*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));({struct Cyc_List_List*_tmpCB2=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6CD->f1=_tmpCB2;});({struct Cyc_Absyn_Exp*_tmpCB3=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6CD->f2=_tmpCB3;});_tmp6CD;});_tmp6CC->hd=_tmpCB4;});_tmp6CC->tl=0;_tmp6CC;}));yyval=_tmpCB5;});
goto _LL254;}case 254: _LL44B: _LL44C: {
# 2332
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2331 "parse.y"
({union Cyc_YYSTYPE _tmpCB9=Cyc_YY5(({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));({struct _tuple30*_tmpCB7=({struct _tuple30*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->f1=0;({struct Cyc_Absyn_Exp*_tmpCB6=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6CF->f2=_tmpCB6;});_tmp6CF;});_tmp6CE->hd=_tmpCB7;});({struct Cyc_List_List*_tmpCB8=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6CE->tl=_tmpCB8;});_tmp6CE;}));yyval=_tmpCB9;});
goto _LL254;}case 255: _LL44D: _LL44E: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2333 "parse.y"
({union Cyc_YYSTYPE _tmpCBE=Cyc_YY5(({struct Cyc_List_List*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));({struct _tuple30*_tmpCBC=({struct _tuple30*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({struct Cyc_List_List*_tmpCBA=Cyc_yyget_YY41(yyyyvsp[2]);_tmp6D1->f1=_tmpCBA;});({struct Cyc_Absyn_Exp*_tmpCBB=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp6D1->f2=_tmpCBB;});_tmp6D1;});_tmp6D0->hd=_tmpCBC;});({struct Cyc_List_List*_tmpCBD=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6D0->tl=_tmpCBD;});_tmp6D0;}));yyval=_tmpCBE;});
goto _LL254;}case 256: _LL44F: _LL450: {
# 2336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2337 "parse.y"
({union Cyc_YYSTYPE _tmpCBF=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));yyval=_tmpCBF;});
goto _LL254;}case 257: _LL451: _LL452: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2338 "parse.y"
({union Cyc_YYSTYPE _tmpCC4=Cyc_YY41(({struct Cyc_List_List*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));({void*_tmpCC3=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCC2=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6D4;_tmp6D4.tag=1;({struct _dyneither_ptr*_tmpCC1=({struct _dyneither_ptr*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));({struct _dyneither_ptr _tmpCC0=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6D5[0]=_tmpCC0;});_tmp6D5;});_tmp6D4.f1=_tmpCC1;});_tmp6D4;});_tmp6D3[0]=_tmpCC2;});_tmp6D3;});_tmp6D2->hd=_tmpCC3;});_tmp6D2->tl=0;_tmp6D2;}));yyval=_tmpCC4;});
goto _LL254;}case 258: _LL453: _LL454: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2343 "parse.y"
({union Cyc_YYSTYPE _tmpCC6=Cyc_YY41(({struct Cyc_List_List*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));({void*_tmpCC5=Cyc_yyget_YY42(yyyyvsp[0]);_tmp6D6->hd=_tmpCC5;});_tmp6D6->tl=0;_tmp6D6;}));yyval=_tmpCC6;});
goto _LL254;}case 259: _LL455: _LL456: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2344 "parse.y"
({union Cyc_YYSTYPE _tmpCC9=Cyc_YY41(({struct Cyc_List_List*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));({void*_tmpCC7=Cyc_yyget_YY42(yyyyvsp[1]);_tmp6D7->hd=_tmpCC7;});({struct Cyc_List_List*_tmpCC8=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6D7->tl=_tmpCC8;});_tmp6D7;}));yyval=_tmpCC9;});
goto _LL254;}case 260: _LL457: _LL458: {
# 2347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2348 "parse.y"
({union Cyc_YYSTYPE _tmpCCC=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCCB=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp6D9;_tmp6D9.tag=0;({struct Cyc_Absyn_Exp*_tmpCCA=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6D9.f1=_tmpCCA;});_tmp6D9;});_tmp6D8[0]=_tmpCCB;});_tmp6D8;}));yyval=_tmpCCC;});
goto _LL254;}case 261: _LL459: _LL45A: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2349 "parse.y"
({union Cyc_YYSTYPE _tmpCD0=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCCF=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6DB;_tmp6DB.tag=1;({struct _dyneither_ptr*_tmpCCE=({struct _dyneither_ptr*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));({struct _dyneither_ptr _tmpCCD=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp6DC[0]=_tmpCCD;});_tmp6DC;});_tmp6DB.f1=_tmpCCE;});_tmp6DB;});_tmp6DA[0]=_tmpCCF;});_tmp6DA;}));yyval=_tmpCD0;});
goto _LL254;}case 262: _LL45B: _LL45C: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2354 "parse.y"
struct _tuple25 _tmp6DD=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6DE=_tmp6DD;struct Cyc_Absyn_Tqual _tmp6E5;struct Cyc_Parse_Type_specifier _tmp6E4;struct Cyc_List_List*_tmp6E3;_LL6B3: _tmp6E5=_tmp6DE.f1;_tmp6E4=_tmp6DE.f2;_tmp6E3=_tmp6DE.f3;_LL6B4:;{
void*_tmp6DF=Cyc_Parse_speclist2typ(_tmp6E4,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6E3 != 0)
({void*_tmp6E0=0;({unsigned int _tmpCD2=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCD1=({const char*_tmp6E1="ignoring attributes in type";_tag_dyneither(_tmp6E1,sizeof(char),28);});Cyc_Warn_warn(_tmpCD2,_tmpCD1,_tag_dyneither(_tmp6E0,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpCD3=Cyc_YY37(({struct _tuple8*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->f1=0;_tmp6E2->f2=_tmp6E5;_tmp6E2->f3=_tmp6DF;_tmp6E2;}));yyval=_tmpCD3;});
# 2360
goto _LL254;};}case 263: _LL45D: _LL45E: {
# 2362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2361 "parse.y"
struct _tuple25 _tmp6E6=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6E7=_tmp6E6;struct Cyc_Absyn_Tqual _tmp6F2;struct Cyc_Parse_Type_specifier _tmp6F1;struct Cyc_List_List*_tmp6F0;_LL6B6: _tmp6F2=_tmp6E7.f1;_tmp6F1=_tmp6E7.f2;_tmp6F0=_tmp6E7.f3;_LL6B7:;{
void*_tmp6E8=Cyc_Parse_speclist2typ(_tmp6F1,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6E9=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6EA=Cyc_Parse_apply_tms(_tmp6F2,_tmp6E8,_tmp6F0,_tmp6E9);
if(_tmp6EA.f3 != 0)
# 2367
({void*_tmp6EB=0;({unsigned int _tmpCD5=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCD4=({const char*_tmp6EC="bad type params, ignoring";_tag_dyneither(_tmp6EC,sizeof(char),26);});Cyc_Warn_warn(_tmpCD5,_tmpCD4,_tag_dyneither(_tmp6EB,sizeof(void*),0));});});
if(_tmp6EA.f4 != 0)
({void*_tmp6ED=0;({unsigned int _tmpCD7=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCD6=({const char*_tmp6EE="bad specifiers, ignoring";_tag_dyneither(_tmp6EE,sizeof(char),25);});Cyc_Warn_warn(_tmpCD7,_tmpCD6,_tag_dyneither(_tmp6ED,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpCD8=Cyc_YY37(({struct _tuple8*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->f1=0;_tmp6EF->f2=_tmp6EA.f1;_tmp6EF->f3=_tmp6EA.f2;_tmp6EF;}));yyval=_tmpCD8;});
# 2372
goto _LL254;};}case 264: _LL45F: _LL460: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2375 "parse.y"
({union Cyc_YYSTYPE _tmpCDA=Cyc_YY44(({struct _tuple8*_tmpCD9=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpCD9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCDA;});
goto _LL254;}case 265: _LL461: _LL462: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2376 "parse.y"
({union Cyc_YYSTYPE _tmpCDC=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCDB=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F4;_tmp6F4.tag=24;_tmp6F4.f1=0;_tmp6F4;});_tmp6F3[0]=_tmpCDB;});_tmp6F3;}));yyval=_tmpCDC;});
goto _LL254;}case 266: _LL463: _LL464: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2377 "parse.y"
({union Cyc_YYSTYPE _tmpCDF=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCDE=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F6;_tmp6F6.tag=24;({struct Cyc_List_List*_tmpCDD=Cyc_yyget_YY40(yyyyvsp[1]);_tmp6F6.f1=_tmpCDD;});_tmp6F6;});_tmp6F5[0]=_tmpCDE;});_tmp6F5;}));yyval=_tmpCDF;});
goto _LL254;}case 267: _LL465: _LL466: {
# 2380
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2378 "parse.y"
({union Cyc_YYSTYPE _tmpCE2=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCE1=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6F8;_tmp6F8.tag=25;({void*_tmpCE0=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6F8.f1=_tmpCE0;});_tmp6F8;});_tmp6F7[0]=_tmpCE1;});_tmp6F7;}));yyval=_tmpCE2;});
goto _LL254;}case 268: _LL467: _LL468: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2379 "parse.y"
({union Cyc_YYSTYPE _tmpCE7=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE6=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6FA;_tmp6FA.tag=24;({struct Cyc_List_List*_tmpCE5=({struct Cyc_List_List*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));({void*_tmpCE3=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6FB->hd=_tmpCE3;});({struct Cyc_List_List*_tmpCE4=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6FB->tl=_tmpCE4;});_tmp6FB;});_tmp6FA.f1=_tmpCE5;});_tmp6FA;});_tmp6F9[0]=_tmpCE6;});_tmp6F9;}));yyval=_tmpCE7;});
goto _LL254;}case 269: _LL469: _LL46A: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpCE9=Cyc_YY40(({struct Cyc_List_List*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));({void*_tmpCE8=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6FC->hd=_tmpCE8;});_tmp6FC->tl=0;_tmp6FC;}));yyval=_tmpCE9;});
goto _LL254;}case 270: _LL46B: _LL46C: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2386 "parse.y"
({union Cyc_YYSTYPE _tmpCEC=Cyc_YY40(({struct Cyc_List_List*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));({void*_tmpCEA=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6FD->hd=_tmpCEA;});({struct Cyc_List_List*_tmpCEB=Cyc_yyget_YY40(yyyyvsp[0]);_tmp6FD->tl=_tmpCEB;});_tmp6FD;}));yyval=_tmpCEC;});
goto _LL254;}case 271: _LL46D: _LL46E: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2391 "parse.y"
({union Cyc_YYSTYPE _tmpCEE=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FE;({struct Cyc_List_List*_tmpCED=Cyc_yyget_YY26(yyyyvsp[0]);_tmp6FE.tms=_tmpCED;});_tmp6FE;}));yyval=_tmpCEE;});
goto _LL254;}case 272: _LL46F: _LL470: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2393 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 273: _LL471: _LL472: {
# 2396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2395 "parse.y"
({union Cyc_YYSTYPE _tmpCF1=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FF;({struct Cyc_List_List*_tmpCF0=({struct Cyc_List_List*_tmpCEF=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCEF,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp6FF.tms=_tmpCF0;});_tmp6FF;}));yyval=_tmpCF1;});
goto _LL254;}case 274: _LL473: _LL474: {
# 2398
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2400 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 275: _LL475: _LL476: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2402 "parse.y"
({union Cyc_YYSTYPE _tmpCF6=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp700;({struct Cyc_List_List*_tmpCF5=({struct Cyc_List_List*_tmp701=_region_malloc(yyr,sizeof(*_tmp701));({void*_tmpCF4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp702=_region_malloc(yyr,sizeof(*_tmp702));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCF3=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp703;_tmp703.tag=0;({union Cyc_Absyn_Constraint*_tmpCF2=Cyc_yyget_YY51(yyyyvsp[2]);_tmp703.f1=_tmpCF2;});_tmp703.f2=(unsigned int)(yyyylsp[2]).first_line;_tmp703;});_tmp702[0]=_tmpCF3;});_tmp702;});_tmp701->hd=_tmpCF4;});_tmp701->tl=0;_tmp701;});_tmp700.tms=_tmpCF5;});_tmp700;}));yyval=_tmpCF6;});
goto _LL254;}case 276: _LL477: _LL478: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2404 "parse.y"
({union Cyc_YYSTYPE _tmpCFC=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp704;({struct Cyc_List_List*_tmpCFB=({struct Cyc_List_List*_tmp705=_region_malloc(yyr,sizeof(*_tmp705));({void*_tmpCF9=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp706=_region_malloc(yyr,sizeof(*_tmp706));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCF8=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp707;_tmp707.tag=0;({union Cyc_Absyn_Constraint*_tmpCF7=Cyc_yyget_YY51(yyyyvsp[3]);_tmp707.f1=_tmpCF7;});_tmp707.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp707;});_tmp706[0]=_tmpCF8;});_tmp706;});_tmp705->hd=_tmpCF9;});({struct Cyc_List_List*_tmpCFA=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp705->tl=_tmpCFA;});_tmp705;});_tmp704.tms=_tmpCFB;});_tmp704;}));yyval=_tmpCFC;});
goto _LL254;}case 277: _LL479: _LL47A: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2406 "parse.y"
({union Cyc_YYSTYPE _tmpD02=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp708;({struct Cyc_List_List*_tmpD01=({struct Cyc_List_List*_tmp709=_region_malloc(yyr,sizeof(*_tmp709));({void*_tmpD00=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp70A=_region_malloc(yyr,sizeof(*_tmp70A));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpCFF=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp70B;_tmp70B.tag=1;({struct Cyc_Absyn_Exp*_tmpCFD=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp70B.f1=_tmpCFD;});({union Cyc_Absyn_Constraint*_tmpCFE=Cyc_yyget_YY51(yyyyvsp[3]);_tmp70B.f2=_tmpCFE;});_tmp70B.f3=(unsigned int)(yyyylsp[3]).first_line;_tmp70B;});_tmp70A[0]=_tmpCFF;});_tmp70A;});_tmp709->hd=_tmpD00;});_tmp709->tl=0;_tmp709;});_tmp708.tms=_tmpD01;});_tmp708;}));yyval=_tmpD02;});
goto _LL254;}case 278: _LL47B: _LL47C: {
# 2409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2408 "parse.y"
({union Cyc_YYSTYPE _tmpD09=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70C;({struct Cyc_List_List*_tmpD08=({struct Cyc_List_List*_tmp70D=_region_malloc(yyr,sizeof(*_tmp70D));({void*_tmpD06=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp70E=_region_malloc(yyr,sizeof(*_tmp70E));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD05=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp70F;_tmp70F.tag=1;({struct Cyc_Absyn_Exp*_tmpD03=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp70F.f1=_tmpD03;});({union Cyc_Absyn_Constraint*_tmpD04=Cyc_yyget_YY51(yyyyvsp[4]);_tmp70F.f2=_tmpD04;});_tmp70F.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp70F;});_tmp70E[0]=_tmpD05;});_tmp70E;});_tmp70D->hd=_tmpD06;});({struct Cyc_List_List*_tmpD07=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp70D->tl=_tmpD07;});_tmp70D;});_tmp70C.tms=_tmpD08;});_tmp70C;}));
# 2408
yyval=_tmpD09;});
# 2411
goto _LL254;}case 279: _LL47D: _LL47E: {
# 2413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2412 "parse.y"
({union Cyc_YYSTYPE _tmpD13=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp710;({struct Cyc_List_List*_tmpD12=({struct Cyc_List_List*_tmp711=_region_malloc(yyr,sizeof(*_tmp711));({void*_tmpD11=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp712=_region_malloc(yyr,sizeof(*_tmp712));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD10=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp713;_tmp713.tag=3;({void*_tmpD0F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp714=_region_malloc(yyr,sizeof(*_tmp714));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD0E=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp715;_tmp715.tag=1;_tmp715.f1=0;_tmp715.f2=0;_tmp715.f3=0;({void*_tmpD0A=Cyc_yyget_YY49(yyyyvsp[1]);_tmp715.f4=_tmpD0A;});({struct Cyc_List_List*_tmpD0B=Cyc_yyget_YY50(yyyyvsp[2]);_tmp715.f5=_tmpD0B;});({struct Cyc_Absyn_Exp*_tmpD0C=Cyc_yyget_YY55(yyyyvsp[4]);_tmp715.f6=_tmpD0C;});({struct Cyc_Absyn_Exp*_tmpD0D=Cyc_yyget_YY55(yyyyvsp[5]);_tmp715.f7=_tmpD0D;});_tmp715;});_tmp714[0]=_tmpD0E;});_tmp714;});_tmp713.f1=_tmpD0F;});_tmp713;});_tmp712[0]=_tmpD10;});_tmp712;});_tmp711->hd=_tmpD11;});_tmp711->tl=0;_tmp711;});_tmp710.tms=_tmpD12;});_tmp710;}));yyval=_tmpD13;});
# 2414
goto _LL254;}case 280: _LL47F: _LL480: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2415 "parse.y"
struct _tuple26*_tmp716=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp717=_tmp716;struct Cyc_List_List*_tmp722;int _tmp721;struct Cyc_Absyn_VarargInfo*_tmp720;void*_tmp71F;struct Cyc_List_List*_tmp71E;_LL6B9: _tmp722=_tmp717->f1;_tmp721=_tmp717->f2;_tmp720=_tmp717->f3;_tmp71F=_tmp717->f4;_tmp71E=_tmp717->f5;_LL6BA:;
({union Cyc_YYSTYPE _tmpD1B=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp718;({struct Cyc_List_List*_tmpD1A=({struct Cyc_List_List*_tmp719=_region_malloc(yyr,sizeof(*_tmp719));({void*_tmpD19=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp71A=_region_malloc(yyr,sizeof(*_tmp71A));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD18=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp71B;_tmp71B.tag=3;({void*_tmpD17=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp71C=_region_malloc(yyr,sizeof(*_tmp71C));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD16=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp71D;_tmp71D.tag=1;_tmp71D.f1=_tmp722;_tmp71D.f2=_tmp721;_tmp71D.f3=_tmp720;_tmp71D.f4=_tmp71F;_tmp71D.f5=_tmp71E;({struct Cyc_Absyn_Exp*_tmpD14=Cyc_yyget_YY55(yyyyvsp[3]);_tmp71D.f6=_tmpD14;});({struct Cyc_Absyn_Exp*_tmpD15=Cyc_yyget_YY55(yyyyvsp[4]);_tmp71D.f7=_tmpD15;});_tmp71D;});_tmp71C[0]=_tmpD16;});_tmp71C;});_tmp71B.f1=_tmpD17;});_tmp71B;});_tmp71A[0]=_tmpD18;});_tmp71A;});_tmp719->hd=_tmpD19;});_tmp719->tl=0;_tmp719;});_tmp718.tms=_tmpD1A;});_tmp718;}));yyval=_tmpD1B;});
# 2418
goto _LL254;}case 281: _LL481: _LL482: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2419 "parse.y"
({union Cyc_YYSTYPE _tmpD26=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp723;({struct Cyc_List_List*_tmpD25=({struct Cyc_List_List*_tmp724=_region_malloc(yyr,sizeof(*_tmp724));({void*_tmpD23=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp725=_region_malloc(yyr,sizeof(*_tmp725));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD22=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp726;_tmp726.tag=3;({void*_tmpD21=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp727=_region_malloc(yyr,sizeof(*_tmp727));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD20=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp728;_tmp728.tag=1;_tmp728.f1=0;_tmp728.f2=0;_tmp728.f3=0;({void*_tmpD1C=Cyc_yyget_YY49(yyyyvsp[2]);_tmp728.f4=_tmpD1C;});({struct Cyc_List_List*_tmpD1D=Cyc_yyget_YY50(yyyyvsp[3]);_tmp728.f5=_tmpD1D;});({struct Cyc_Absyn_Exp*_tmpD1E=Cyc_yyget_YY55(yyyyvsp[5]);_tmp728.f6=_tmpD1E;});({struct Cyc_Absyn_Exp*_tmpD1F=Cyc_yyget_YY55(yyyyvsp[6]);_tmp728.f7=_tmpD1F;});_tmp728;});_tmp727[0]=_tmpD20;});_tmp727;});_tmp726.f1=_tmpD21;});_tmp726;});_tmp725[0]=_tmpD22;});_tmp725;});_tmp724->hd=_tmpD23;});({struct Cyc_List_List*_tmpD24=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp724->tl=_tmpD24;});_tmp724;});_tmp723.tms=_tmpD25;});_tmp723;}));
# 2419
yyval=_tmpD26;});
# 2422
goto _LL254;}case 282: _LL483: _LL484: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2423 "parse.y"
struct _tuple26*_tmp729=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp72A=_tmp729;struct Cyc_List_List*_tmp735;int _tmp734;struct Cyc_Absyn_VarargInfo*_tmp733;void*_tmp732;struct Cyc_List_List*_tmp731;_LL6BC: _tmp735=_tmp72A->f1;_tmp734=_tmp72A->f2;_tmp733=_tmp72A->f3;_tmp732=_tmp72A->f4;_tmp731=_tmp72A->f5;_LL6BD:;
({union Cyc_YYSTYPE _tmpD2F=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72B;({struct Cyc_List_List*_tmpD2E=({struct Cyc_List_List*_tmp72C=_region_malloc(yyr,sizeof(*_tmp72C));({void*_tmpD2C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp72D=_region_malloc(yyr,sizeof(*_tmp72D));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD2B=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp72E;_tmp72E.tag=3;({void*_tmpD2A=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp72F=_region_malloc(yyr,sizeof(*_tmp72F));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD29=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp730;_tmp730.tag=1;_tmp730.f1=_tmp735;_tmp730.f2=_tmp734;_tmp730.f3=_tmp733;_tmp730.f4=_tmp732;_tmp730.f5=_tmp731;({struct Cyc_Absyn_Exp*_tmpD27=
Cyc_yyget_YY55(yyyyvsp[4]);_tmp730.f6=_tmpD27;});({struct Cyc_Absyn_Exp*_tmpD28=Cyc_yyget_YY55(yyyyvsp[5]);_tmp730.f7=_tmpD28;});_tmp730;});_tmp72F[0]=_tmpD29;});_tmp72F;});_tmp72E.f1=_tmpD2A;});_tmp72E;});_tmp72D[0]=_tmpD2B;});_tmp72D;});_tmp72C->hd=_tmpD2C;});({struct Cyc_List_List*_tmpD2D=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp72C->tl=_tmpD2D;});_tmp72C;});_tmp72B.tms=_tmpD2E;});_tmp72B;}));
# 2424
yyval=_tmpD2F;});
# 2427
goto _LL254;}case 283: _LL485: _LL486: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2429 "parse.y"
struct Cyc_List_List*_tmp736=({unsigned int _tmpD30=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpD30,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpD35=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp737;({struct Cyc_List_List*_tmpD34=({struct Cyc_List_List*_tmp738=_region_malloc(yyr,sizeof(*_tmp738));({void*_tmpD32=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp739=_region_malloc(yyr,sizeof(*_tmp739));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD31=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp73A;_tmp73A.tag=4;_tmp73A.f1=_tmp736;_tmp73A.f2=(unsigned int)(yyyylsp[1]).first_line;_tmp73A.f3=0;_tmp73A;});_tmp739[0]=_tmpD31;});_tmp739;});_tmp738->hd=_tmpD32;});({struct Cyc_List_List*_tmpD33=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp738->tl=_tmpD33;});_tmp738;});_tmp737.tms=_tmpD34;});_tmp737;}));
# 2430
yyval=_tmpD35;});
# 2433
goto _LL254;}case 284: _LL487: _LL488: {
# 2435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2434 "parse.y"
({union Cyc_YYSTYPE _tmpD3B=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73B;({struct Cyc_List_List*_tmpD3A=({struct Cyc_List_List*_tmp73C=_region_malloc(yyr,sizeof(*_tmp73C));({void*_tmpD38=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp73D=_region_malloc(yyr,sizeof(*_tmp73D));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD37=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp73E;_tmp73E.tag=5;_tmp73E.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpD36=Cyc_yyget_YY45(yyyyvsp[1]);_tmp73E.f2=_tmpD36;});_tmp73E;});_tmp73D[0]=_tmpD37;});_tmp73D;});_tmp73C->hd=_tmpD38;});({struct Cyc_List_List*_tmpD39=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp73C->tl=_tmpD39;});_tmp73C;});_tmp73B.tms=_tmpD3A;});_tmp73B;}));yyval=_tmpD3B;});
# 2436
goto _LL254;}case 285: _LL489: _LL48A: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2440 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 286: _LL48B: _LL48C: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2441 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 287: _LL48D: _LL48E: {
# 2444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2442 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 288: _LL48F: _LL490: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2443 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 289: _LL491: _LL492: {
# 2446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2444 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 290: _LL493: _LL494: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2445 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 291: _LL495: _LL496: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
({union Cyc_YYSTYPE _tmpD41=Cyc_Stmt_tok(({void*_tmpD40=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD3F=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp740;_tmp740.tag=13;({struct _dyneither_ptr*_tmpD3D=({struct _dyneither_ptr*_tmp741=_cycalloc(sizeof(*_tmp741));({struct _dyneither_ptr _tmpD3C=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp741[0]=_tmpD3C;});_tmp741;});_tmp740.f1=_tmpD3D;});({struct Cyc_Absyn_Stmt*_tmpD3E=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp740.f2=_tmpD3E;});_tmp740;});_tmp73F[0]=_tmpD3F;});_tmp73F;});Cyc_Absyn_new_stmt(_tmpD40,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD41;});
goto _LL254;}case 292: _LL497: _LL498: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2455 "parse.y"
({union Cyc_YYSTYPE _tmpD42=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD42;});
goto _LL254;}case 293: _LL499: _LL49A: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2456 "parse.y"
({union Cyc_YYSTYPE _tmpD44=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD43=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpD43,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD44;});
goto _LL254;}case 294: _LL49B: _LL49C: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2461 "parse.y"
({union Cyc_YYSTYPE _tmpD45=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD45;});
goto _LL254;}case 295: _LL49D: _LL49E: {
# 2464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2462 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 296: _LL49F: _LL4A0: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2467 "parse.y"
({union Cyc_YYSTYPE _tmpD47=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpD46=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpD46,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));yyval=_tmpD47;});
goto _LL254;}case 297: _LL4A1: _LL4A2: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2468 "parse.y"
({union Cyc_YYSTYPE _tmpD49=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpD48=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpD48,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpD49;});
goto _LL254;}case 298: _LL4A3: _LL4A4: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2469 "parse.y"
({union Cyc_YYSTYPE _tmpD50=Cyc_Stmt_tok(({void*_tmpD4F=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp742=_cycalloc(sizeof(*_tmp742));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD4E=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp743;_tmp743.tag=13;({struct _dyneither_ptr*_tmpD4B=({struct _dyneither_ptr*_tmp744=_cycalloc(sizeof(*_tmp744));({struct _dyneither_ptr _tmpD4A=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp744[0]=_tmpD4A;});_tmp744;});_tmp743.f1=_tmpD4B;});({struct Cyc_Absyn_Stmt*_tmpD4D=({struct Cyc_List_List*_tmpD4C=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpD4C,Cyc_Absyn_skip_stmt(0));});_tmp743.f2=_tmpD4D;});_tmp743;});_tmp742[0]=_tmpD4E;});_tmp742;});Cyc_Absyn_new_stmt(_tmpD4F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD50;});
# 2471
goto _LL254;}case 299: _LL4A5: _LL4A6: {
# 2473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2471 "parse.y"
({union Cyc_YYSTYPE _tmpD57=Cyc_Stmt_tok(({void*_tmpD56=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp745=_cycalloc(sizeof(*_tmp745));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD55=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp746;_tmp746.tag=13;({struct _dyneither_ptr*_tmpD52=({struct _dyneither_ptr*_tmp747=_cycalloc(sizeof(*_tmp747));({struct _dyneither_ptr _tmpD51=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp747[0]=_tmpD51;});_tmp747;});_tmp746.f1=_tmpD52;});({struct Cyc_Absyn_Stmt*_tmpD54=({struct Cyc_List_List*_tmpD53=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpD53,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp746.f2=_tmpD54;});_tmp746;});_tmp745[0]=_tmpD55;});_tmp745;});Cyc_Absyn_new_stmt(_tmpD56,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD57;});
# 2473
goto _LL254;}case 300: _LL4A7: _LL4A8: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2473 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 301: _LL4A9: _LL4AA: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2474 "parse.y"
({union Cyc_YYSTYPE _tmpD5A=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpD59=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpD58=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpD59,_tmpD58,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5A;});
goto _LL254;}case 302: _LL4AB: _LL4AC: {
# 2477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2475 "parse.y"
({union Cyc_YYSTYPE _tmpD5F=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpD5E=({void*_tmpD5D=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp748=_cycalloc(sizeof(*_tmp748));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD5C=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp749;_tmp749.tag=1;({struct Cyc_Absyn_Fndecl*_tmpD5B=Cyc_yyget_YY15(yyyyvsp[0]);_tmp749.f1=_tmpD5B;});_tmp749;});_tmp748[0]=_tmpD5C;});_tmp748;});Cyc_Absyn_new_decl(_tmpD5D,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpD5E,
Cyc_Absyn_skip_stmt(0));}));
# 2475
yyval=_tmpD5F;});
# 2477
goto _LL254;}case 303: _LL4AD: _LL4AE: {
# 2479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2478 "parse.y"
({union Cyc_YYSTYPE _tmpD64=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpD63=({void*_tmpD62=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD61=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp74B;_tmp74B.tag=1;({struct Cyc_Absyn_Fndecl*_tmpD60=Cyc_yyget_YY15(yyyyvsp[0]);_tmp74B.f1=_tmpD60;});_tmp74B;});_tmp74A[0]=_tmpD61;});_tmp74A;});Cyc_Absyn_new_decl(_tmpD62,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpD63,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpD64;});
goto _LL254;}case 304: _LL4AF: _LL4B0: {
# 2481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2483 "parse.y"
({union Cyc_YYSTYPE _tmpD68=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD67=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpD66=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpD65=Cyc_Absyn_skip_stmt(0);Cyc_Absyn_ifthenelse_stmt(_tmpD67,_tmpD66,_tmpD65,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD68;});
goto _LL254;}case 305: _LL4B1: _LL4B2: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2485 "parse.y"
({union Cyc_YYSTYPE _tmpD6C=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD6B=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpD6A=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpD69=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpD6B,_tmpD6A,_tmpD69,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6C;});
goto _LL254;}case 306: _LL4B3: _LL4B4: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
({union Cyc_YYSTYPE _tmpD6F=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD6E=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpD6D=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpD6E,_tmpD6D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6F;});
goto _LL254;}case 307: _LL4B5: _LL4B6: {
# 2494
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2496
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2494 "parse.y"
struct Cyc_Absyn_Exp*_tmp74C=({struct _tuple0*_tmpD70=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpD70,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD73=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD72=_tmp74C;struct Cyc_List_List*_tmpD71=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpD72,_tmpD71,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD73;});
goto _LL254;}case 308: _LL4B7: _LL4B8: {
# 2498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2498 "parse.y"
struct Cyc_Absyn_Exp*_tmp74D=({struct Cyc_List_List*_tmpD74=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpD74,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD77=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD76=_tmp74D;struct Cyc_List_List*_tmpD75=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpD76,_tmpD75,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD77;});
# 2501
goto _LL254;}case 309: _LL4B9: _LL4BA: {
# 2503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2504 "parse.y"
({union Cyc_YYSTYPE _tmpD7A=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpD79=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpD78=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpD79,_tmpD78,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7A;});
goto _LL254;}case 310: _LL4BB: _LL4BC:
# 2507
({union Cyc_YYSTYPE _tmpD7B=Cyc_YY8(0);yyval=_tmpD7B;});
goto _LL254;case 311: _LL4BD: _LL4BE: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2522 "parse.y"
({union Cyc_YYSTYPE _tmpD80=Cyc_YY8(({struct Cyc_List_List*_tmp74E=_cycalloc(sizeof(*_tmp74E));({struct Cyc_Absyn_Switch_clause*_tmpD7E=({struct Cyc_Absyn_Switch_clause*_tmp74F=_cycalloc(sizeof(*_tmp74F));({struct Cyc_Absyn_Pat*_tmpD7C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp74F->pattern=_tmpD7C;});_tmp74F->pat_vars=0;_tmp74F->where_clause=0;({struct Cyc_Absyn_Stmt*_tmpD7D=
Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp74F->body=_tmpD7D;});_tmp74F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp74F;});_tmp74E->hd=_tmpD7E;});({struct Cyc_List_List*_tmpD7F=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp74E->tl=_tmpD7F;});_tmp74E;}));
# 2522
yyval=_tmpD80;});
# 2525
goto _LL254;}case 312: _LL4BF: _LL4C0: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2526 "parse.y"
({union Cyc_YYSTYPE _tmpD85=Cyc_YY8(({struct Cyc_List_List*_tmp750=_cycalloc(sizeof(*_tmp750));({struct Cyc_Absyn_Switch_clause*_tmpD83=({struct Cyc_Absyn_Switch_clause*_tmp751=_cycalloc(sizeof(*_tmp751));({struct Cyc_Absyn_Pat*_tmpD81=Cyc_yyget_YY9(yyyyvsp[1]);_tmp751->pattern=_tmpD81;});_tmp751->pat_vars=0;_tmp751->where_clause=0;({struct Cyc_Absyn_Stmt*_tmpD82=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp751->body=_tmpD82;});_tmp751->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp751;});_tmp750->hd=_tmpD83;});({struct Cyc_List_List*_tmpD84=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp750->tl=_tmpD84;});_tmp750;}));
# 2526
yyval=_tmpD85;});
# 2529
goto _LL254;}case 313: _LL4C1: _LL4C2: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2530 "parse.y"
({union Cyc_YYSTYPE _tmpD8A=Cyc_YY8(({struct Cyc_List_List*_tmp752=_cycalloc(sizeof(*_tmp752));({struct Cyc_Absyn_Switch_clause*_tmpD88=({struct Cyc_Absyn_Switch_clause*_tmp753=_cycalloc(sizeof(*_tmp753));({struct Cyc_Absyn_Pat*_tmpD86=Cyc_yyget_YY9(yyyyvsp[1]);_tmp753->pattern=_tmpD86;});_tmp753->pat_vars=0;_tmp753->where_clause=0;({struct Cyc_Absyn_Stmt*_tmpD87=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp753->body=_tmpD87;});_tmp753->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp753;});_tmp752->hd=_tmpD88;});({struct Cyc_List_List*_tmpD89=Cyc_yyget_YY8(yyyyvsp[4]);_tmp752->tl=_tmpD89;});_tmp752;}));yyval=_tmpD8A;});
goto _LL254;}case 314: _LL4C3: _LL4C4: {
# 2533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2532 "parse.y"
({union Cyc_YYSTYPE _tmpD90=Cyc_YY8(({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));({struct Cyc_Absyn_Switch_clause*_tmpD8E=({struct Cyc_Absyn_Switch_clause*_tmp755=_cycalloc(sizeof(*_tmp755));({struct Cyc_Absyn_Pat*_tmpD8B=Cyc_yyget_YY9(yyyyvsp[1]);_tmp755->pattern=_tmpD8B;});_tmp755->pat_vars=0;({struct Cyc_Absyn_Exp*_tmpD8C=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp755->where_clause=_tmpD8C;});({struct Cyc_Absyn_Stmt*_tmpD8D=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp755->body=_tmpD8D;});_tmp755->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp755;});_tmp754->hd=_tmpD8E;});({struct Cyc_List_List*_tmpD8F=
Cyc_yyget_YY8(yyyyvsp[5]);_tmp754->tl=_tmpD8F;});_tmp754;}));
# 2532
yyval=_tmpD90;});
# 2535
goto _LL254;}case 315: _LL4C5: _LL4C6: {
# 2537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2536 "parse.y"
({union Cyc_YYSTYPE _tmpD96=Cyc_YY8(({struct Cyc_List_List*_tmp756=_cycalloc(sizeof(*_tmp756));({struct Cyc_Absyn_Switch_clause*_tmpD94=({struct Cyc_Absyn_Switch_clause*_tmp757=_cycalloc(sizeof(*_tmp757));({struct Cyc_Absyn_Pat*_tmpD91=Cyc_yyget_YY9(yyyyvsp[1]);_tmp757->pattern=_tmpD91;});_tmp757->pat_vars=0;({struct Cyc_Absyn_Exp*_tmpD92=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp757->where_clause=_tmpD92;});({struct Cyc_Absyn_Stmt*_tmpD93=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp757->body=_tmpD93;});_tmp757->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp757;});_tmp756->hd=_tmpD94;});({struct Cyc_List_List*_tmpD95=Cyc_yyget_YY8(yyyyvsp[6]);_tmp756->tl=_tmpD95;});_tmp756;}));yyval=_tmpD96;});
goto _LL254;}case 316: _LL4C7: _LL4C8: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2543 "parse.y"
({union Cyc_YYSTYPE _tmpD99=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD98=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpD97=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpD98,_tmpD97,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD99;});
goto _LL254;}case 317: _LL4C9: _LL4CA: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2547 "parse.y"
({union Cyc_YYSTYPE _tmpD9C=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpD9B=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpD9A=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpD9B,_tmpD9A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9C;});
goto _LL254;}case 318: _LL4CB: _LL4CC: {
# 2550
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2552
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2551 "parse.y"
({union Cyc_YYSTYPE _tmpDA1=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDA0=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpD9F=Cyc_Absyn_true_exp(0);struct Cyc_Absyn_Exp*_tmpD9E=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Stmt*_tmpD9D=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2551
Cyc_Absyn_for_stmt(_tmpDA0,_tmpD9F,_tmpD9E,_tmpD9D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA1;});
# 2553
goto _LL254;}case 319: _LL4CD: _LL4CE: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2554 "parse.y"
({union Cyc_YYSTYPE _tmpDA6=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDA5=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDA4=Cyc_Absyn_true_exp(0);struct Cyc_Absyn_Exp*_tmpDA3=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpDA2=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2554
Cyc_Absyn_for_stmt(_tmpDA5,_tmpDA4,_tmpDA3,_tmpDA2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA6;});
# 2556
goto _LL254;}case 320: _LL4CF: _LL4D0: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2557 "parse.y"
({union Cyc_YYSTYPE _tmpDAB=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDAA=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDA9=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDA8=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Stmt*_tmpDA7=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2557
Cyc_Absyn_for_stmt(_tmpDAA,_tmpDA9,_tmpDA8,_tmpDA7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDAB;});
# 2559
goto _LL254;}case 321: _LL4D1: _LL4D2: {
# 2561
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2560 "parse.y"
({union Cyc_YYSTYPE _tmpDB0=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDAF=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDAE=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDAD=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpDAC=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2560
Cyc_Absyn_for_stmt(_tmpDAF,_tmpDAE,_tmpDAD,_tmpDAC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB0;});
# 2562
goto _LL254;}case 322: _LL4D3: _LL4D4: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2563 "parse.y"
({union Cyc_YYSTYPE _tmpDB5=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDB4=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDB3=Cyc_Absyn_true_exp(0);struct Cyc_Absyn_Exp*_tmpDB2=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Stmt*_tmpDB1=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2563
Cyc_Absyn_for_stmt(_tmpDB4,_tmpDB3,_tmpDB2,_tmpDB1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB5;});
# 2565
goto _LL254;}case 323: _LL4D5: _LL4D6: {
# 2567
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2569
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2566 "parse.y"
({union Cyc_YYSTYPE _tmpDBA=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDB9=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDB8=Cyc_Absyn_true_exp(0);struct Cyc_Absyn_Exp*_tmpDB7=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpDB6=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2566
Cyc_Absyn_for_stmt(_tmpDB9,_tmpDB8,_tmpDB7,_tmpDB6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBA;});
# 2568
goto _LL254;}case 324: _LL4D7: _LL4D8: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2569 "parse.y"
({union Cyc_YYSTYPE _tmpDBF=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDBE=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDBD=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpDBC=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Stmt*_tmpDBB=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2569
Cyc_Absyn_for_stmt(_tmpDBE,_tmpDBD,_tmpDBC,_tmpDBB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBF;});
# 2571
goto _LL254;}case 325: _LL4D9: _LL4DA: {
# 2573
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);}));
# 2575
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);}));
# 2572 "parse.y"
({union Cyc_YYSTYPE _tmpDC4=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDC3=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDC2=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpDC1=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpDC0=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2572
Cyc_Absyn_for_stmt(_tmpDC3,_tmpDC2,_tmpDC1,_tmpDC0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC4;});
# 2574
goto _LL254;}case 326: _LL4DB: _LL4DC: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2575 "parse.y"
struct Cyc_List_List*_tmp758=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp759=({struct Cyc_Absyn_Exp*_tmpDC8=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDC7=Cyc_Absyn_true_exp(0);struct Cyc_Absyn_Exp*_tmpDC6=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Stmt*_tmpDC5=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2576
Cyc_Absyn_for_stmt(_tmpDC8,_tmpDC7,_tmpDC6,_tmpDC5,(unsigned int)(yyyylsp[0]).first_line);});
# 2578
({union Cyc_YYSTYPE _tmpDC9=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp758,_tmp759));yyval=_tmpDC9;});
# 2580
goto _LL254;}case 327: _LL4DD: _LL4DE: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2581 "parse.y"
struct Cyc_List_List*_tmp75A=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75B=({struct Cyc_Absyn_Exp*_tmpDCD=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDCC=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDCB=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Stmt*_tmpDCA=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2582
Cyc_Absyn_for_stmt(_tmpDCD,_tmpDCC,_tmpDCB,_tmpDCA,(unsigned int)(yyyylsp[0]).first_line);});
# 2584
({union Cyc_YYSTYPE _tmpDCE=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75A,_tmp75B));yyval=_tmpDCE;});
# 2586
goto _LL254;}case 328: _LL4DF: _LL4E0: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 2587 "parse.y"
struct Cyc_List_List*_tmp75C=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75D=({struct Cyc_Absyn_Exp*_tmpDD2=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDD1=Cyc_Absyn_true_exp(0);struct Cyc_Absyn_Exp*_tmpDD0=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpDCF=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2588
Cyc_Absyn_for_stmt(_tmpDD2,_tmpDD1,_tmpDD0,_tmpDCF,(unsigned int)(yyyylsp[0]).first_line);});
# 2590
({union Cyc_YYSTYPE _tmpDD3=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75C,_tmp75D));yyval=_tmpDD3;});
# 2592
goto _LL254;}case 329: _LL4E1: _LL4E2: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);}));
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);}));
# 2593 "parse.y"
struct Cyc_List_List*_tmp75E=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75F=({struct Cyc_Absyn_Exp*_tmpDD7=Cyc_Absyn_false_exp(0);struct Cyc_Absyn_Exp*_tmpDD6=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDD5=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpDD4=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2594
Cyc_Absyn_for_stmt(_tmpDD7,_tmpDD6,_tmpDD5,_tmpDD4,(unsigned int)(yyyylsp[0]).first_line);});
# 2596
({union Cyc_YYSTYPE _tmpDD8=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75E,_tmp75F));yyval=_tmpDD8;});
# 2598
goto _LL254;}case 330: _LL4E3: _LL4E4: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
({union Cyc_YYSTYPE _tmpDDB=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpDDA=({struct _dyneither_ptr*_tmp760=_cycalloc(sizeof(*_tmp760));({struct _dyneither_ptr _tmpDD9=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp760[0]=_tmpDD9;});_tmp760;});Cyc_Absyn_goto_stmt(_tmpDDA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDDB;});
goto _LL254;}case 331: _LL4E5: _LL4E6: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2604 "parse.y"
({union Cyc_YYSTYPE _tmpDDC=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDC;});
goto _LL254;}case 332: _LL4E7: _LL4E8: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2605 "parse.y"
({union Cyc_YYSTYPE _tmpDDD=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDD;});
goto _LL254;}case 333: _LL4E9: _LL4EA: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2606 "parse.y"
({union Cyc_YYSTYPE _tmpDDE=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDE;});
goto _LL254;}case 334: _LL4EB: _LL4EC: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2607 "parse.y"
({union Cyc_YYSTYPE _tmpDE0=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDDF=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpDDF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE0;});
goto _LL254;}case 335: _LL4ED: _LL4EE: {
# 2610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2609 "parse.y"
({union Cyc_YYSTYPE _tmpDE1=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDE1;});
goto _LL254;}case 336: _LL4EF: _LL4F0: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2610 "parse.y"
({union Cyc_YYSTYPE _tmpDE2=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDE2;});
goto _LL254;}case 337: _LL4F1: _LL4F2: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2612 "parse.y"
({union Cyc_YYSTYPE _tmpDE4=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpDE3=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpDE3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE4;});
goto _LL254;}case 338: _LL4F3: _LL4F4: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2621 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 339: _LL4F5: _LL4F6: {
# 2624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2624 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 340: _LL4F7: _LL4F8: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2626 "parse.y"
({union Cyc_YYSTYPE _tmpDE8=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDE7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDE6=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDE5=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpDE7,_tmpDE6,_tmpDE5,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDE8;});
goto _LL254;}case 341: _LL4F9: _LL4FA: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2629 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 342: _LL4FB: _LL4FC: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2631 "parse.y"
({union Cyc_YYSTYPE _tmpDEB=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDEA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDE9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpDEA,_tmpDE9,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDEB;});
goto _LL254;}case 343: _LL4FD: _LL4FE: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2634 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 344: _LL4FF: _LL500: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2636 "parse.y"
({union Cyc_YYSTYPE _tmpDEE=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDED=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDEC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpDED,_tmpDEC,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDEE;});
goto _LL254;}case 345: _LL501: _LL502: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2639 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 346: _LL503: _LL504: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2641 "parse.y"
({union Cyc_YYSTYPE _tmpDF1=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDEF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpDF0,_tmpDEF,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDF1;});
goto _LL254;}case 347: _LL505: _LL506: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2644 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 348: _LL507: _LL508: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2646 "parse.y"
({union Cyc_YYSTYPE _tmpDF4=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF2=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpDF3,_tmpDF2,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDF4;});
goto _LL254;}case 349: _LL509: _LL50A: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2649 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 350: _LL50B: _LL50C: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2651 "parse.y"
({union Cyc_YYSTYPE _tmpDF7=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpDF6,_tmpDF5,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDF7;});
goto _LL254;}case 351: _LL50D: _LL50E: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2654 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 352: _LL50F: _LL510: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2656 "parse.y"
({union Cyc_YYSTYPE _tmpDFA=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF8=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpDF9,_tmpDF8,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDFA;});
goto _LL254;}case 353: _LL511: _LL512: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2658 "parse.y"
({union Cyc_YYSTYPE _tmpDFD=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDFC=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDFB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpDFC,_tmpDFB,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDFD;});
goto _LL254;}case 354: _LL513: _LL514: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2661 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 355: _LL515: _LL516: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2663 "parse.y"
({union Cyc_YYSTYPE _tmpE00=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDFF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDFE=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpDFF,_tmpDFE,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE00;});
goto _LL254;}case 356: _LL517: _LL518: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2665 "parse.y"
({union Cyc_YYSTYPE _tmpE03=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE02=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE01=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpE02,_tmpE01,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE03;});
goto _LL254;}case 357: _LL519: _LL51A: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2667 "parse.y"
({union Cyc_YYSTYPE _tmpE06=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE05=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE04=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpE05,_tmpE04,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE06;});
goto _LL254;}case 358: _LL51B: _LL51C: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2669 "parse.y"
({union Cyc_YYSTYPE _tmpE09=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE08=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE07=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpE08,_tmpE07,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE09;});
goto _LL254;}case 359: _LL51D: _LL51E: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2672 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 360: _LL51F: _LL520: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2674 "parse.y"
({union Cyc_YYSTYPE _tmpE0C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE0B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE0A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpE0B,_tmpE0A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE0C;});
goto _LL254;}case 361: _LL521: _LL522: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2676 "parse.y"
({union Cyc_YYSTYPE _tmpE0F=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE0E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE0D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpE0E,_tmpE0D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE0F;});
goto _LL254;}case 362: _LL523: _LL524: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2679 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 363: _LL525: _LL526: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2681 "parse.y"
({union Cyc_YYSTYPE _tmpE12=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE10=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpE11,_tmpE10,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE12;});
goto _LL254;}case 364: _LL527: _LL528: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2683 "parse.y"
({union Cyc_YYSTYPE _tmpE15=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE14=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE13=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpE14,_tmpE13,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE15;});
goto _LL254;}case 365: _LL529: _LL52A: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2686 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 366: _LL52B: _LL52C: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2688 "parse.y"
({union Cyc_YYSTYPE _tmpE18=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE16=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpE17,_tmpE16,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE18;});
goto _LL254;}case 367: _LL52D: _LL52E: {
# 2691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2690 "parse.y"
({union Cyc_YYSTYPE _tmpE1B=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE1A=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE19=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpE1A,_tmpE19,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE1B;});
goto _LL254;}case 368: _LL52F: _LL530: {
# 2693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2692 "parse.y"
({union Cyc_YYSTYPE _tmpE1E=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE1D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE1C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpE1D,_tmpE1C,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE1E;});
goto _LL254;}case 369: _LL531: _LL532: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2695 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 370: _LL533: _LL534: {
# 2698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2697 "parse.y"
void*_tmp761=({struct _tuple8*_tmpE1F=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpE1F,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpE22=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpE21=_tmp761;struct Cyc_Absyn_Exp*_tmpE20=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpE21,_tmpE20,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE22;});
# 2700
goto _LL254;}case 371: _LL535: _LL536: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2703 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 372: _LL537: _LL538: {
# 2706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2706 "parse.y"
({union Cyc_YYSTYPE _tmpE23=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpE23;});
goto _LL254;}case 373: _LL539: _LL53A: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2708 "parse.y"
({union Cyc_YYSTYPE _tmpE26=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpE25=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpE24=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpE25,_tmpE24,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE26;});
goto _LL254;}case 374: _LL53B: _LL53C: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2710 "parse.y"
void*_tmp762=({struct _tuple8*_tmpE27=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpE27,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpE28=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp762,(unsigned int)(yyyylsp[0]).first_line)));yyval=_tmpE28;});
# 2713
goto _LL254;}case 375: _LL53D: _LL53E: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2714 "parse.y"
({union Cyc_YYSTYPE _tmpE2A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE29=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpE29,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE2A;});
goto _LL254;}case 376: _LL53F: _LL540: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2716 "parse.y"
({union Cyc_YYSTYPE _tmpE2D=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpE2C=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpE2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpE2C,_tmpE2B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE2D;});
goto _LL254;}case 377: _LL541: _LL542: {
# 2719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2721 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 378: _LL543: _LL544: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2729 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 379: _LL545: _LL546: {
# 2732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2734 "parse.y"
({union Cyc_YYSTYPE _tmpE2E=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpE2E;});
goto _LL254;}case 380: _LL547: _LL548: {
# 2737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2736 "parse.y"
({union Cyc_YYSTYPE _tmpE2F=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpE2F;});
goto _LL254;}case 381: _LL549: _LL54A: {
# 2739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2738 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp763=e->r;void*_tmp764=_tmp763;struct _dyneither_ptr _tmp77C;int _tmp77B;enum Cyc_Absyn_Sign _tmp77A;int _tmp779;enum Cyc_Absyn_Sign _tmp778;short _tmp777;enum Cyc_Absyn_Sign _tmp776;char _tmp775;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).LongLong_c).tag){case 2: _LL6BF: _tmp776=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Char_c).val).f1;_tmp775=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Char_c).val).f2;_LL6C0:
# 2742
({union Cyc_YYSTYPE _tmpE32=Cyc_YY9(({void*_tmpE31=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp765=_cycalloc_atomic(sizeof(*_tmp765));({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE30=({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmp766;_tmp766.tag=11;_tmp766.f1=_tmp775;_tmp766;});_tmp765[0]=_tmpE30;});_tmp765;});Cyc_Absyn_new_pat(_tmpE31,e->loc);}));yyval=_tmpE32;});goto _LL6BE;case 4: _LL6C1: _tmp778=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Short_c).val).f1;_tmp777=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Short_c).val).f2;_LL6C2:
# 2744
({union Cyc_YYSTYPE _tmpE35=Cyc_YY9(({void*_tmpE34=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp767=_cycalloc_atomic(sizeof(*_tmp767));({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE33=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp768;_tmp768.tag=10;_tmp768.f1=_tmp778;_tmp768.f2=(int)_tmp777;_tmp768;});_tmp767[0]=_tmpE33;});_tmp767;});Cyc_Absyn_new_pat(_tmpE34,e->loc);}));yyval=_tmpE35;});goto _LL6BE;case 5: _LL6C3: _tmp77A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Int_c).val).f1;_tmp779=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Int_c).val).f2;_LL6C4:
# 2746
({union Cyc_YYSTYPE _tmpE38=Cyc_YY9(({void*_tmpE37=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp769=_cycalloc_atomic(sizeof(*_tmp769));({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE36=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp76A;_tmp76A.tag=10;_tmp76A.f1=_tmp77A;_tmp76A.f2=_tmp779;_tmp76A;});_tmp769[0]=_tmpE36;});_tmp769;});Cyc_Absyn_new_pat(_tmpE37,e->loc);}));yyval=_tmpE38;});goto _LL6BE;case 7: _LL6C5: _tmp77C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Float_c).val).f1;_tmp77B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp764)->f1).Float_c).val).f2;_LL6C6:
# 2748
({union Cyc_YYSTYPE _tmpE3B=Cyc_YY9(({void*_tmpE3A=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp76B=_cycalloc(sizeof(*_tmp76B));({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE39=({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmp76C;_tmp76C.tag=12;_tmp76C.f1=_tmp77C;_tmp76C.f2=_tmp77B;_tmp76C;});_tmp76B[0]=_tmpE39;});_tmp76B;});Cyc_Absyn_new_pat(_tmpE3A,e->loc);}));yyval=_tmpE3B;});goto _LL6BE;case 1: _LL6C7: _LL6C8:
# 2750
({union Cyc_YYSTYPE _tmpE3C=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));yyval=_tmpE3C;});goto _LL6BE;case 8: _LL6C9: _LL6CA:
# 2752
({void*_tmp76D=0;({unsigned int _tmpE3E=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE3D=({const char*_tmp76E="strings cannot occur within patterns";_tag_dyneither(_tmp76E,sizeof(char),37);});Cyc_Warn_err(_tmpE3E,_tmpE3D,_tag_dyneither(_tmp76D,sizeof(void*),0));});});goto _LL6BE;case 9: _LL6CB: _LL6CC:
# 2754
({void*_tmp76F=0;({unsigned int _tmpE40=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE3F=({const char*_tmp770="strings cannot occur within patterns";_tag_dyneither(_tmp770,sizeof(char),37);});Cyc_Warn_err(_tmpE40,_tmpE3F,_tag_dyneither(_tmp76F,sizeof(void*),0));});});goto _LL6BE;case 6: _LL6CD: _LL6CE:
# 2756
({void*_tmp771=0;({unsigned int _tmpE42=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE41=({const char*_tmp772="long long's in patterns not yet implemented";_tag_dyneither(_tmp772,sizeof(char),44);});Cyc_Warn_err(_tmpE42,_tmpE41,_tag_dyneither(_tmp771,sizeof(void*),0));});});goto _LL6BE;default: goto _LL6CF;}else{_LL6CF: _LL6D0:
# 2758
({void*_tmp773=0;({unsigned int _tmpE44=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE43=({const char*_tmp774="bad constant in case";_tag_dyneither(_tmp774,sizeof(char),21);});Cyc_Warn_err(_tmpE44,_tmpE43,_tag_dyneither(_tmp773,sizeof(void*),0));});});}_LL6BE:;}
# 2761
goto _LL254;}case 382: _LL54B: _LL54C: {
# 2763
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2765
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2762 "parse.y"
({union Cyc_YYSTYPE _tmpE48=Cyc_YY9(({void*_tmpE47=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE46=({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmp77E;_tmp77E.tag=15;({struct _tuple0*_tmpE45=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp77E.f1=_tmpE45;});_tmp77E;});_tmp77D[0]=_tmpE46;});_tmp77D;});Cyc_Absyn_new_pat(_tmpE47,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE48;});
goto _LL254;}case 383: _LL54D: _LL54E: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2764 "parse.y"
if(({({struct _dyneither_ptr _tmpE49=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpE49,({const char*_tmp77F="as";_tag_dyneither(_tmp77F,sizeof(char),3);}));})!= 0;}))
({void*_tmp780=0;({unsigned int _tmpE4B=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpE4A=({const char*_tmp781="expecting `as'";_tag_dyneither(_tmp781,sizeof(char),15);});Cyc_Warn_err(_tmpE4B,_tmpE4A,_tag_dyneither(_tmp780,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpE53=Cyc_YY9(({void*_tmpE52=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp782=_cycalloc(sizeof(*_tmp782));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE51=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp783;_tmp783.tag=1;({struct Cyc_Absyn_Vardecl*_tmpE4F=({unsigned int _tmpE4E=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpE4E,({struct _tuple0*_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE4D=({struct _dyneither_ptr*_tmp785=_cycalloc(sizeof(*_tmp785));({struct _dyneither_ptr _tmpE4C=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp785[0]=_tmpE4C;});_tmp785;});_tmp784->f2=_tmpE4D;});_tmp784;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp783.f1=_tmpE4F;});({struct Cyc_Absyn_Pat*_tmpE50=
Cyc_yyget_YY9(yyyyvsp[2]);_tmp783.f2=_tmpE50;});_tmp783;});_tmp782[0]=_tmpE51;});_tmp782;});
# 2766
Cyc_Absyn_new_pat(_tmpE52,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE53;});
# 2769
goto _LL254;}case 384: _LL54F: _LL550: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 2770 "parse.y"
if(({({struct _dyneither_ptr _tmpE54=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpE54,({const char*_tmp786="alias";_tag_dyneither(_tmp786,sizeof(char),6);}));})!= 0;}))
({void*_tmp787=0;({unsigned int _tmpE56=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpE55=({const char*_tmp788="expecting `alias'";_tag_dyneither(_tmp788,sizeof(char),18);});Cyc_Warn_err(_tmpE56,_tmpE55,_tag_dyneither(_tmp787,sizeof(void*),0));});});{
int _tmp789=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp78E=_cycalloc(sizeof(*_tmp78E));({struct _dyneither_ptr*_tmpE58=({struct _dyneither_ptr*_tmp791=_cycalloc(sizeof(*_tmp791));({struct _dyneither_ptr _tmpE57=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp791[0]=_tmpE57;});_tmp791;});_tmp78E->name=_tmpE58;});_tmp78E->identity=- 1;({void*_tmpE5A=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE59=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp790;_tmp790.tag=0;_tmp790.f1=& Cyc_Tcutil_rk;_tmp790;});_tmp78F[0]=_tmpE59;});_tmp78F;});_tmp78E->kind=_tmpE5A;});_tmp78E;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpE5F=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpE5E=({struct _tuple0*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE5C=({struct _dyneither_ptr*_tmp78D=_cycalloc(sizeof(*_tmp78D));({struct _dyneither_ptr _tmpE5B=Cyc_yyget_String_tok(yyyyvsp[5]);_tmp78D[0]=_tmpE5B;});_tmp78D;});_tmp78C->f2=_tmpE5C;});_tmp78C;});Cyc_Absyn_new_vardecl(_tmpE5F,_tmpE5E,({
struct _tuple8*_tmpE5D=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpE5D,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpE62=Cyc_YY9(({void*_tmpE61=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp78A=_cycalloc(sizeof(*_tmp78A));({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE60=({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp78B;_tmp78B.tag=2;_tmp78B.f1=tv;_tmp78B.f2=vd;_tmp78B;});_tmp78A[0]=_tmpE60;});_tmp78A;});Cyc_Absyn_new_pat(_tmpE61,(unsigned int)_tmp789);}));yyval=_tmpE62;});
# 2778
goto _LL254;};}case 385: _LL551: _LL552: {
# 2780
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2779 "parse.y"
struct _tuple22 _tmp792=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp793=_tmp792;struct Cyc_List_List*_tmp797;int _tmp796;_LL6D2: _tmp797=_tmp793.f1;_tmp796=_tmp793.f2;_LL6D3:;
({union Cyc_YYSTYPE _tmpE65=Cyc_YY9(({void*_tmpE64=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp794=_cycalloc(sizeof(*_tmp794));({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE63=({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmp795;_tmp795.tag=5;_tmp795.f1=_tmp797;_tmp795.f2=_tmp796;_tmp795;});_tmp794[0]=_tmpE63;});_tmp794;});Cyc_Absyn_new_pat(_tmpE64,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE65;});
# 2782
goto _LL254;}case 386: _LL553: _LL554: {
# 2784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2783 "parse.y"
struct _tuple22 _tmp798=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp799=_tmp798;struct Cyc_List_List*_tmp79D;int _tmp79C;_LL6D5: _tmp79D=_tmp799.f1;_tmp79C=_tmp799.f2;_LL6D6:;
({union Cyc_YYSTYPE _tmpE69=Cyc_YY9(({void*_tmpE68=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp79A=_cycalloc(sizeof(*_tmp79A));({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE67=({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmp79B;_tmp79B.tag=16;({struct _tuple0*_tmpE66=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp79B.f1=_tmpE66;});_tmp79B.f2=_tmp79D;_tmp79B.f3=_tmp79C;_tmp79B;});_tmp79A[0]=_tmpE67;});_tmp79A;});Cyc_Absyn_new_pat(_tmpE68,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE69;});
# 2786
goto _LL254;}case 387: _LL555: _LL556: {
# 2788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2787 "parse.y"
struct _tuple22 _tmp79E=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp79F=_tmp79E;struct Cyc_List_List*_tmp7A5;int _tmp7A4;_LL6D8: _tmp7A5=_tmp79F.f1;_tmp7A4=_tmp79F.f2;_LL6D9:;{
struct Cyc_List_List*_tmp7A0=({unsigned int _tmpE6A=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpE6A,Cyc_yyget_YY40(yyyyvsp[2]));});
({union Cyc_YYSTYPE _tmpE6F=Cyc_YY9(({void*_tmpE6E=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE6D=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp7A2;_tmp7A2.tag=7;({struct Cyc_Absyn_AggrInfo*_tmpE6C=({struct Cyc_Absyn_AggrInfo*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));({union Cyc_Absyn_AggrInfoU _tmpE6B=({Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);});_tmp7A3->aggr_info=_tmpE6B;});_tmp7A3->targs=0;_tmp7A3;});_tmp7A2.f1=_tmpE6C;});_tmp7A2.f2=_tmp7A0;_tmp7A2.f3=_tmp7A5;_tmp7A2.f4=_tmp7A4;_tmp7A2;});_tmp7A1[0]=_tmpE6D;});_tmp7A1;});Cyc_Absyn_new_pat(_tmpE6E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE6F;});
# 2792
goto _LL254;};}case 388: _LL557: _LL558: {
# 2794
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2793 "parse.y"
struct _tuple22 _tmp7A6=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp7A7=_tmp7A6;struct Cyc_List_List*_tmp7AC;int _tmp7AB;_LL6DB: _tmp7AC=_tmp7A7.f1;_tmp7AB=_tmp7A7.f2;_LL6DC:;{
struct Cyc_List_List*_tmp7A8=({unsigned int _tmpE70=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpE70,Cyc_yyget_YY40(yyyyvsp[1]));});
({union Cyc_YYSTYPE _tmpE73=Cyc_YY9(({void*_tmpE72=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE71=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp7AA;_tmp7AA.tag=7;_tmp7AA.f1=0;_tmp7AA.f2=_tmp7A8;_tmp7AA.f3=_tmp7AC;_tmp7AA.f4=_tmp7AB;_tmp7AA;});_tmp7A9[0]=_tmpE71;});_tmp7A9;});Cyc_Absyn_new_pat(_tmpE72,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE73;});
# 2797
goto _LL254;};}case 389: _LL559: _LL55A: {
# 2799
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2798 "parse.y"
({union Cyc_YYSTYPE _tmpE77=Cyc_YY9(({void*_tmpE76=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE75=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7AE;_tmp7AE.tag=6;({struct Cyc_Absyn_Pat*_tmpE74=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7AE.f1=_tmpE74;});_tmp7AE;});_tmp7AD[0]=_tmpE75;});_tmp7AD;});Cyc_Absyn_new_pat(_tmpE76,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE77;});
goto _LL254;}case 390: _LL55B: _LL55C: {
# 2801
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2800 "parse.y"
({union Cyc_YYSTYPE _tmpE7E=Cyc_YY9(({void*_tmpE7D=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7AF=_cycalloc(sizeof(*_tmp7AF));({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE7C=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7B0;_tmp7B0.tag=6;({struct Cyc_Absyn_Pat*_tmpE7B=({void*_tmpE7A=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE79=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7B2;_tmp7B2.tag=6;({struct Cyc_Absyn_Pat*_tmpE78=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7B2.f1=_tmpE78;});_tmp7B2;});_tmp7B1[0]=_tmpE79;});_tmp7B1;});Cyc_Absyn_new_pat(_tmpE7A,(unsigned int)(yyyylsp[0]).first_line);});_tmp7B0.f1=_tmpE7B;});_tmp7B0;});_tmp7AF[0]=_tmpE7C;});_tmp7AF;});Cyc_Absyn_new_pat(_tmpE7D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE7E;});
goto _LL254;}case 391: _LL55D: _LL55E: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2802 "parse.y"
({union Cyc_YYSTYPE _tmpE86=Cyc_YY9(({void*_tmpE85=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE84=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7B4;_tmp7B4.tag=3;({struct Cyc_Absyn_Vardecl*_tmpE82=({unsigned int _tmpE81=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpE81,({struct _tuple0*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE80=({struct _dyneither_ptr*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));({struct _dyneither_ptr _tmpE7F=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7B6[0]=_tmpE7F;});_tmp7B6;});_tmp7B5->f2=_tmpE80;});_tmp7B5;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp7B4.f1=_tmpE82;});({struct Cyc_Absyn_Pat*_tmpE83=
# 2804
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp7B4.f2=_tmpE83;});_tmp7B4;});_tmp7B3[0]=_tmpE84;});_tmp7B3;});
# 2802
Cyc_Absyn_new_pat(_tmpE85,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE86;});
# 2806
goto _LL254;}case 392: _LL55F: _LL560: {
# 2808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2807 "parse.y"
if(({({struct _dyneither_ptr _tmpE87=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpE87,({const char*_tmp7B7="as";_tag_dyneither(_tmp7B7,sizeof(char),3);}));})!= 0;}))
({void*_tmp7B8=0;({unsigned int _tmpE89=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpE88=({const char*_tmp7B9="expecting `as'";_tag_dyneither(_tmp7B9,sizeof(char),15);});Cyc_Warn_err(_tmpE89,_tmpE88,_tag_dyneither(_tmp7B8,sizeof(void*),0));});});
({union Cyc_YYSTYPE _tmpE91=Cyc_YY9(({void*_tmpE90=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE8F=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7BB;_tmp7BB.tag=3;({struct Cyc_Absyn_Vardecl*_tmpE8D=({unsigned int _tmpE8C=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpE8C,({struct _tuple0*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE8B=({struct _dyneither_ptr*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));({struct _dyneither_ptr _tmpE8A=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7BD[0]=_tmpE8A;});_tmp7BD;});_tmp7BC->f2=_tmpE8B;});_tmp7BC;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp7BB.f1=_tmpE8D;});({struct Cyc_Absyn_Pat*_tmpE8E=
# 2811
Cyc_yyget_YY9(yyyyvsp[3]);_tmp7BB.f2=_tmpE8E;});_tmp7BB;});_tmp7BA[0]=_tmpE8F;});_tmp7BA;});
# 2809
Cyc_Absyn_new_pat(_tmpE90,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE91;});
# 2813
goto _LL254;}case 393: _LL561: _LL562: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2814 "parse.y"
void*_tmp7BE=({struct _dyneither_ptr _tmpE92=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpE92,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
({union Cyc_YYSTYPE _tmpE9C=Cyc_YY9(({void*_tmpE9B=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE9A=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7C0;_tmp7C0.tag=4;({struct Cyc_Absyn_Tvar*_tmpE93=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7BE);_tmp7C0.f1=_tmpE93;});({struct Cyc_Absyn_Vardecl*_tmpE99=({
unsigned int _tmpE98=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpE97=({struct _tuple0*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE95=({struct _dyneither_ptr*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));({struct _dyneither_ptr _tmpE94=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7C2[0]=_tmpE94;});_tmp7C2;});_tmp7C1->f2=_tmpE95;});_tmp7C1;});Cyc_Absyn_new_vardecl(_tmpE98,_tmpE97,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE96=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7C4;_tmp7C4.tag=19;_tmp7C4.f1=_tmp7BE;_tmp7C4;});_tmp7C3[0]=_tmpE96;});_tmp7C3;}),0);});_tmp7C0.f2=_tmpE99;});_tmp7C0;});_tmp7BF[0]=_tmpE9A;});_tmp7BF;});
# 2815
Cyc_Absyn_new_pat(_tmpE9B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE9C;});
# 2819
goto _LL254;}case 394: _LL563: _LL564: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2820 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7C5=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
({union Cyc_YYSTYPE _tmpEA7=Cyc_YY9(({void*_tmpEA6=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEA5=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7C7;_tmp7C7.tag=4;_tmp7C7.f1=_tmp7C5;({struct Cyc_Absyn_Vardecl*_tmpEA4=({
unsigned int _tmpEA3=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpEA2=({struct _tuple0*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE9E=({struct _dyneither_ptr*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));({struct _dyneither_ptr _tmpE9D=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7C9[0]=_tmpE9D;});_tmp7C9;});_tmp7C8->f2=_tmpE9E;});_tmp7C8;});Cyc_Absyn_new_vardecl(_tmpEA3,_tmpEA2,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEA1=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7CB;_tmp7CB.tag=19;({void*_tmpEA0=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7CC=_cycalloc(sizeof(*_tmp7CC));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE9F=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7CD;_tmp7CD.tag=2;_tmp7CD.f1=_tmp7C5;_tmp7CD;});_tmp7CC[0]=_tmpE9F;});_tmp7CC;});_tmp7CB.f1=_tmpEA0;});_tmp7CB;});_tmp7CA[0]=_tmpEA1;});_tmp7CA;}),0);});_tmp7C7.f2=_tmpEA4;});_tmp7C7;});_tmp7C6[0]=_tmpEA5;});_tmp7C6;});
# 2821
Cyc_Absyn_new_pat(_tmpEA6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEA7;});
# 2825
goto _LL254;}case 395: _LL565: _LL566: {
# 2827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2828 "parse.y"
({union Cyc_YYSTYPE _tmpEA9=Cyc_YY10(({struct _tuple22*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));({struct Cyc_List_List*_tmpEA8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7CE->f1=_tmpEA8;});_tmp7CE->f2=0;_tmp7CE;}));yyval=_tmpEA9;});
goto _LL254;}case 396: _LL567: _LL568: {
# 2831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2829 "parse.y"
({union Cyc_YYSTYPE _tmpEAB=Cyc_YY10(({struct _tuple22*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));({struct Cyc_List_List*_tmpEAA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7CF->f1=_tmpEAA;});_tmp7CF->f2=1;_tmp7CF;}));yyval=_tmpEAB;});
goto _LL254;}case 397: _LL569: _LL56A: {
# 2832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2830 "parse.y"
({union Cyc_YYSTYPE _tmpEAC=Cyc_YY10(({struct _tuple22*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0->f1=0;_tmp7D0->f2=1;_tmp7D0;}));yyval=_tmpEAC;});
goto _LL254;}case 398: _LL56B: _LL56C: {
# 2833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2835 "parse.y"
({union Cyc_YYSTYPE _tmpEAE=Cyc_YY11(({struct Cyc_List_List*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));({struct Cyc_Absyn_Pat*_tmpEAD=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7D1->hd=_tmpEAD;});_tmp7D1->tl=0;_tmp7D1;}));yyval=_tmpEAE;});
goto _LL254;}case 399: _LL56D: _LL56E: {
# 2838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2837 "parse.y"
({union Cyc_YYSTYPE _tmpEB1=Cyc_YY11(({struct Cyc_List_List*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));({struct Cyc_Absyn_Pat*_tmpEAF=Cyc_yyget_YY9(yyyyvsp[2]);_tmp7D2->hd=_tmpEAF;});({struct Cyc_List_List*_tmpEB0=Cyc_yyget_YY11(yyyyvsp[0]);_tmp7D2->tl=_tmpEB0;});_tmp7D2;}));yyval=_tmpEB1;});
goto _LL254;}case 400: _LL56F: _LL570: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2842 "parse.y"
({union Cyc_YYSTYPE _tmpEB3=Cyc_YY12(({struct _tuple23*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3->f1=0;({struct Cyc_Absyn_Pat*_tmpEB2=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7D3->f2=_tmpEB2;});_tmp7D3;}));yyval=_tmpEB3;});
goto _LL254;}case 401: _LL571: _LL572: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2844 "parse.y"
({union Cyc_YYSTYPE _tmpEB6=Cyc_YY12(({struct _tuple23*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));({struct Cyc_List_List*_tmpEB4=Cyc_yyget_YY41(yyyyvsp[0]);_tmp7D4->f1=_tmpEB4;});({struct Cyc_Absyn_Pat*_tmpEB5=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7D4->f2=_tmpEB5;});_tmp7D4;}));yyval=_tmpEB6;});
goto _LL254;}case 402: _LL573: _LL574: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2847 "parse.y"
({union Cyc_YYSTYPE _tmpEB8=Cyc_YY14(({struct _tuple22*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));({struct Cyc_List_List*_tmpEB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7D5->f1=_tmpEB7;});_tmp7D5->f2=0;_tmp7D5;}));yyval=_tmpEB8;});
goto _LL254;}case 403: _LL575: _LL576: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2848 "parse.y"
({union Cyc_YYSTYPE _tmpEBA=Cyc_YY14(({struct _tuple22*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));({struct Cyc_List_List*_tmpEB9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7D6->f1=_tmpEB9;});_tmp7D6->f2=1;_tmp7D6;}));yyval=_tmpEBA;});
goto _LL254;}case 404: _LL577: _LL578: {
# 2851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2849 "parse.y"
({union Cyc_YYSTYPE _tmpEBB=Cyc_YY14(({struct _tuple22*_tmp7D7=_cycalloc(sizeof(*_tmp7D7));_tmp7D7->f1=0;_tmp7D7->f2=1;_tmp7D7;}));yyval=_tmpEBB;});
goto _LL254;}case 405: _LL579: _LL57A: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2854 "parse.y"
({union Cyc_YYSTYPE _tmpEBD=Cyc_YY13(({struct Cyc_List_List*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));({struct _tuple23*_tmpEBC=Cyc_yyget_YY12(yyyyvsp[0]);_tmp7D8->hd=_tmpEBC;});_tmp7D8->tl=0;_tmp7D8;}));yyval=_tmpEBD;});
goto _LL254;}case 406: _LL57B: _LL57C: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2856 "parse.y"
({union Cyc_YYSTYPE _tmpEC0=Cyc_YY13(({struct Cyc_List_List*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));({struct _tuple23*_tmpEBE=Cyc_yyget_YY12(yyyyvsp[2]);_tmp7D9->hd=_tmpEBE;});({struct Cyc_List_List*_tmpEBF=Cyc_yyget_YY13(yyyyvsp[0]);_tmp7D9->tl=_tmpEBF;});_tmp7D9;}));yyval=_tmpEC0;});
goto _LL254;}case 407: _LL57D: _LL57E: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
yyval=yyyyvsp[0];
goto _LL254;}case 408: _LL57F: _LL580: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2864 "parse.y"
({union Cyc_YYSTYPE _tmpEC3=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEC2=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEC1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpEC2,_tmpEC1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEC3;});
goto _LL254;}case 409: _LL581: _LL582: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2869 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 410: _LL583: _LL584: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2871 "parse.y"
({union Cyc_YYSTYPE _tmpEC7=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEC6=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpEC5=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpEC4=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpEC6,_tmpEC5,_tmpEC4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEC7;});
goto _LL254;}case 411: _LL585: _LL586: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2873 "parse.y"
({union Cyc_YYSTYPE _tmpECA=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEC9=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEC8=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpEC9,_tmpEC8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpECA;});
goto _LL254;}case 412: _LL587: _LL588: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2877 "parse.y"
({union Cyc_YYSTYPE _tmpECB=Cyc_YY7(0);yyval=_tmpECB;});
goto _LL254;}case 413: _LL589: _LL58A: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2878 "parse.y"
({union Cyc_YYSTYPE _tmpECC=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DA=_cycalloc_atomic(sizeof(*_tmp7DA));_tmp7DA->v=(void*)Cyc_Absyn_Times;_tmp7DA;}));yyval=_tmpECC;});
goto _LL254;}case 414: _LL58B: _LL58C: {
# 2881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2879 "parse.y"
({union Cyc_YYSTYPE _tmpECD=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DB=_cycalloc_atomic(sizeof(*_tmp7DB));_tmp7DB->v=(void*)Cyc_Absyn_Div;_tmp7DB;}));yyval=_tmpECD;});
goto _LL254;}case 415: _LL58D: _LL58E: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2880 "parse.y"
({union Cyc_YYSTYPE _tmpECE=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DC=_cycalloc_atomic(sizeof(*_tmp7DC));_tmp7DC->v=(void*)Cyc_Absyn_Mod;_tmp7DC;}));yyval=_tmpECE;});
goto _LL254;}case 416: _LL58F: _LL590: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2881 "parse.y"
({union Cyc_YYSTYPE _tmpECF=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DD=_cycalloc_atomic(sizeof(*_tmp7DD));_tmp7DD->v=(void*)Cyc_Absyn_Plus;_tmp7DD;}));yyval=_tmpECF;});
goto _LL254;}case 417: _LL591: _LL592: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2882 "parse.y"
({union Cyc_YYSTYPE _tmpED0=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DE=_cycalloc_atomic(sizeof(*_tmp7DE));_tmp7DE->v=(void*)Cyc_Absyn_Minus;_tmp7DE;}));yyval=_tmpED0;});
goto _LL254;}case 418: _LL593: _LL594: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2883 "parse.y"
({union Cyc_YYSTYPE _tmpED1=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DF=_cycalloc_atomic(sizeof(*_tmp7DF));_tmp7DF->v=(void*)Cyc_Absyn_Bitlshift;_tmp7DF;}));yyval=_tmpED1;});
goto _LL254;}case 419: _LL595: _LL596: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2884 "parse.y"
({union Cyc_YYSTYPE _tmpED2=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E0=_cycalloc_atomic(sizeof(*_tmp7E0));_tmp7E0->v=(void*)Cyc_Absyn_Bitlrshift;_tmp7E0;}));yyval=_tmpED2;});
goto _LL254;}case 420: _LL597: _LL598: {
# 2887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2885 "parse.y"
({union Cyc_YYSTYPE _tmpED3=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E1=_cycalloc_atomic(sizeof(*_tmp7E1));_tmp7E1->v=(void*)Cyc_Absyn_Bitand;_tmp7E1;}));yyval=_tmpED3;});
goto _LL254;}case 421: _LL599: _LL59A: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2886 "parse.y"
({union Cyc_YYSTYPE _tmpED4=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E2=_cycalloc_atomic(sizeof(*_tmp7E2));_tmp7E2->v=(void*)Cyc_Absyn_Bitxor;_tmp7E2;}));yyval=_tmpED4;});
goto _LL254;}case 422: _LL59B: _LL59C: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2887 "parse.y"
({union Cyc_YYSTYPE _tmpED5=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E3=_cycalloc_atomic(sizeof(*_tmp7E3));_tmp7E3->v=(void*)Cyc_Absyn_Bitor;_tmp7E3;}));yyval=_tmpED5;});
goto _LL254;}case 423: _LL59D: _LL59E: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2892 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 424: _LL59F: _LL5A0: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2894 "parse.y"
({union Cyc_YYSTYPE _tmpED9=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpED8=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpED7=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpED6=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpED8,_tmpED7,_tmpED6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpED9;});
goto _LL254;}case 425: _LL5A1: _LL5A2: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2897 "parse.y"
({union Cyc_YYSTYPE _tmpEDB=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEDA=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpEDA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEDB;});
goto _LL254;}case 426: _LL5A3: _LL5A4: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2900 "parse.y"
({union Cyc_YYSTYPE _tmpEDD=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEDC=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpEDC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEDD;});
goto _LL254;}case 427: _LL5A5: _LL5A6: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2902 "parse.y"
({union Cyc_YYSTYPE _tmpEDF=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEDE=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpEDE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEDF;});
goto _LL254;}case 428: _LL5A7: _LL5A8: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2904 "parse.y"
({union Cyc_YYSTYPE _tmpEE2=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEE1=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpEE0=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpEE1,_tmpEE0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE2;});
goto _LL254;}case 429: _LL5A9: _LL5AA: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 2906 "parse.y"
({union Cyc_YYSTYPE _tmpEE5=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEE4=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpEE3=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpEE4,_tmpEE3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE5;});
goto _LL254;}case 430: _LL5AB: _LL5AC: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2910 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 431: _LL5AD: _LL5AE: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2914 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 432: _LL5AF: _LL5B0: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2916 "parse.y"
({union Cyc_YYSTYPE _tmpEE8=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEE7=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEE6=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpEE7,_tmpEE6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE8;});
goto _LL254;}case 433: _LL5B1: _LL5B2: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2920 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 434: _LL5B3: _LL5B4: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2922 "parse.y"
({union Cyc_YYSTYPE _tmpEEB=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEEA=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEE9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpEEA,_tmpEE9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEEB;});
goto _LL254;}case 435: _LL5B5: _LL5B6: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2926 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 436: _LL5B7: _LL5B8: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2928 "parse.y"
({union Cyc_YYSTYPE _tmpEEE=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEED=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEEC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpEED,_tmpEEC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEEE;});
goto _LL254;}case 437: _LL5B9: _LL5BA: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2932 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 438: _LL5BB: _LL5BC: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2934 "parse.y"
({union Cyc_YYSTYPE _tmpEF1=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF0=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEEF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpEF0,_tmpEEF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEF1;});
goto _LL254;}case 439: _LL5BD: _LL5BE: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2938 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 440: _LL5BF: _LL5C0: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2940 "parse.y"
({union Cyc_YYSTYPE _tmpEF4=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF3=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF2=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpEF3,_tmpEF2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEF4;});
goto _LL254;}case 441: _LL5C1: _LL5C2: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2944 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 442: _LL5C3: _LL5C4: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2946 "parse.y"
({union Cyc_YYSTYPE _tmpEF7=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF6=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpEF6,_tmpEF5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEF7;});
goto _LL254;}case 443: _LL5C5: _LL5C6: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2948 "parse.y"
({union Cyc_YYSTYPE _tmpEFA=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF9=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF8=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpEF9,_tmpEF8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEFA;});
goto _LL254;}case 444: _LL5C7: _LL5C8: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2952 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 445: _LL5C9: _LL5CA: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2954 "parse.y"
({union Cyc_YYSTYPE _tmpEFD=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEFC=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEFB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpEFC,_tmpEFB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEFD;});
goto _LL254;}case 446: _LL5CB: _LL5CC: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2956 "parse.y"
({union Cyc_YYSTYPE _tmpF00=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEFF=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEFE=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpEFF,_tmpEFE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF00;});
goto _LL254;}case 447: _LL5CD: _LL5CE: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2958 "parse.y"
({union Cyc_YYSTYPE _tmpF03=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF02=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF01=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpF02,_tmpF01,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF03;});
goto _LL254;}case 448: _LL5CF: _LL5D0: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2960 "parse.y"
({union Cyc_YYSTYPE _tmpF06=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF05=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF04=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpF05,_tmpF04,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF06;});
goto _LL254;}case 449: _LL5D1: _LL5D2: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2964 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 450: _LL5D3: _LL5D4: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2966 "parse.y"
({union Cyc_YYSTYPE _tmpF09=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF08=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF07=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpF08,_tmpF07,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF09;});
goto _LL254;}case 451: _LL5D5: _LL5D6: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2968 "parse.y"
({union Cyc_YYSTYPE _tmpF0C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF0B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF0A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpF0B,_tmpF0A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF0C;});
goto _LL254;}case 452: _LL5D7: _LL5D8: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2972 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 453: _LL5D9: _LL5DA: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2974 "parse.y"
({union Cyc_YYSTYPE _tmpF0F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF0E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF0D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpF0E,_tmpF0D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF0F;});
goto _LL254;}case 454: _LL5DB: _LL5DC: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2976 "parse.y"
({union Cyc_YYSTYPE _tmpF12=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF11=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF10=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpF11,_tmpF10,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF12;});
goto _LL254;}case 455: _LL5DD: _LL5DE: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2980 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 456: _LL5DF: _LL5E0: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2982 "parse.y"
({union Cyc_YYSTYPE _tmpF15=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF14=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF13=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpF14,_tmpF13,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF15;});
goto _LL254;}case 457: _LL5E1: _LL5E2: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2984 "parse.y"
({union Cyc_YYSTYPE _tmpF18=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF17=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF16=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpF17,_tmpF16,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF18;});
goto _LL254;}case 458: _LL5E3: _LL5E4: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 2986 "parse.y"
({union Cyc_YYSTYPE _tmpF1B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF1A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF19=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpF1A,_tmpF19,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF1B;});
goto _LL254;}case 459: _LL5E5: _LL5E6: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2990 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 460: _LL5E7: _LL5E8: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 2992 "parse.y"
void*_tmp7E4=({struct _tuple8*_tmpF1C=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpF1C,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpF1F=Cyc_Exp_tok(({void*_tmpF1E=_tmp7E4;struct Cyc_Absyn_Exp*_tmpF1D=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpF1E,_tmpF1D,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF1F;});
# 2995
goto _LL254;}case 461: _LL5E9: _LL5EA: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 2998 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 462: _LL5EB: _LL5EC: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 2999 "parse.y"
({union Cyc_YYSTYPE _tmpF21=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF20=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpF20,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF21;});
goto _LL254;}case 463: _LL5ED: _LL5EE: {
# 3002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3000 "parse.y"
({union Cyc_YYSTYPE _tmpF23=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF22=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpF22,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF23;});
goto _LL254;}case 464: _LL5EF: _LL5F0: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3001 "parse.y"
({union Cyc_YYSTYPE _tmpF25=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF24=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpF24,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF25;});
goto _LL254;}case 465: _LL5F1: _LL5F2: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3002 "parse.y"
({union Cyc_YYSTYPE _tmpF27=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF26=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpF26,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF27;});
goto _LL254;}case 466: _LL5F3: _LL5F4: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3003 "parse.y"
({union Cyc_YYSTYPE _tmpF29=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF28=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpF28,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF29;});
goto _LL254;}case 467: _LL5F5: _LL5F6: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3004 "parse.y"
({union Cyc_YYSTYPE _tmpF2C=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpF2B=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF2A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpF2B,_tmpF2A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF2C;});
goto _LL254;}case 468: _LL5F7: _LL5F8: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3006 "parse.y"
void*_tmp7E5=({struct _tuple8*_tmpF2D=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpF2D,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpF2E=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp7E5,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpF2E;});
# 3009
goto _LL254;}case 469: _LL5F9: _LL5FA: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3009 "parse.y"
({union Cyc_YYSTYPE _tmpF30=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF2F=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpF2F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF30;});
goto _LL254;}case 470: _LL5FB: _LL5FC: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 3011 "parse.y"
void*_tmp7E6=({struct _tuple8*_tmpF31=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpF31,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpF34=Cyc_Exp_tok(({void*_tmpF33=_tmp7E6;struct Cyc_List_List*_tmpF32=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpF33,_tmpF32,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF34;});
# 3014
goto _LL254;}case 471: _LL5FD: _LL5FE: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3016 "parse.y"
({union Cyc_YYSTYPE _tmpF39=Cyc_Exp_tok(({void*_tmpF38=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF37=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7E8;_tmp7E8.tag=33;({struct Cyc_Absyn_MallocInfo _tmpF36=({struct Cyc_Absyn_MallocInfo _tmp7E9;_tmp7E9.is_calloc=0;_tmp7E9.rgn=0;_tmp7E9.elt_type=0;({struct Cyc_Absyn_Exp*_tmpF35=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7E9.num_elts=_tmpF35;});_tmp7E9.fat_result=0;_tmp7E9.inline_call=0;_tmp7E9;});_tmp7E8.f1=_tmpF36;});_tmp7E8;});_tmp7E7[0]=_tmpF37;});_tmp7E7;});Cyc_Absyn_new_exp(_tmpF38,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF39;});
# 3018
goto _LL254;}case 472: _LL5FF: _LL600: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 3019 "parse.y"
({union Cyc_YYSTYPE _tmpF3F=Cyc_Exp_tok(({void*_tmpF3E=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF3D=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7EB;_tmp7EB.tag=33;({struct Cyc_Absyn_MallocInfo _tmpF3C=({struct Cyc_Absyn_MallocInfo _tmp7EC;_tmp7EC.is_calloc=0;({struct Cyc_Absyn_Exp*_tmpF3A=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7EC.rgn=_tmpF3A;});_tmp7EC.elt_type=0;({struct Cyc_Absyn_Exp*_tmpF3B=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7EC.num_elts=_tmpF3B;});_tmp7EC.fat_result=0;_tmp7EC.inline_call=0;_tmp7EC;});_tmp7EB.f1=_tmpF3C;});_tmp7EB;});_tmp7EA[0]=_tmpF3D;});_tmp7EA;});Cyc_Absyn_new_exp(_tmpF3E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF3F;});
# 3021
goto _LL254;}case 473: _LL601: _LL602: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 3022 "parse.y"
({union Cyc_YYSTYPE _tmpF45=Cyc_Exp_tok(({void*_tmpF44=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF43=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7EE;_tmp7EE.tag=33;({struct Cyc_Absyn_MallocInfo _tmpF42=({struct Cyc_Absyn_MallocInfo _tmp7EF;_tmp7EF.is_calloc=0;({struct Cyc_Absyn_Exp*_tmpF40=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7EF.rgn=_tmpF40;});_tmp7EF.elt_type=0;({struct Cyc_Absyn_Exp*_tmpF41=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7EF.num_elts=_tmpF41;});_tmp7EF.fat_result=0;_tmp7EF.inline_call=1;_tmp7EF;});_tmp7EE.f1=_tmpF42;});_tmp7EE;});_tmp7ED[0]=_tmpF43;});_tmp7ED;});Cyc_Absyn_new_exp(_tmpF44,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF45;});
# 3024
goto _LL254;}case 474: _LL603: _LL604: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);}));
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);}));
# 3025 "parse.y"
void*_tmp7F0=({struct _tuple8*_tmpF46=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpF46,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpF4C=Cyc_Exp_tok(({void*_tmpF4B=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF4A=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7F2;_tmp7F2.tag=33;({struct Cyc_Absyn_MallocInfo _tmpF49=({struct Cyc_Absyn_MallocInfo _tmp7F3;_tmp7F3.is_calloc=1;_tmp7F3.rgn=0;({void**_tmpF47=({void**_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=_tmp7F0;_tmp7F4;});_tmp7F3.elt_type=_tmpF47;});({struct Cyc_Absyn_Exp*_tmpF48=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F3.num_elts=_tmpF48;});_tmp7F3.fat_result=0;_tmp7F3.inline_call=0;_tmp7F3;});_tmp7F2.f1=_tmpF49;});_tmp7F2;});_tmp7F1[0]=_tmpF4A;});_tmp7F1;});Cyc_Absyn_new_exp(_tmpF4B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF4C;});
# 3028
goto _LL254;}case 475: _LL605: _LL606: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);}));
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);}));
# 3030 "parse.y"
void*_tmp7F5=({struct _tuple8*_tmpF4D=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpF4D,(unsigned int)(yyyylsp[8]).first_line);});
({union Cyc_YYSTYPE _tmpF54=Cyc_Exp_tok(({void*_tmpF53=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF52=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7F7;_tmp7F7.tag=33;({struct Cyc_Absyn_MallocInfo _tmpF51=({struct Cyc_Absyn_MallocInfo _tmp7F8;_tmp7F8.is_calloc=1;({struct Cyc_Absyn_Exp*_tmpF4E=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F8.rgn=_tmpF4E;});({void**_tmpF4F=({void**_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=_tmp7F5;_tmp7F9;});_tmp7F8.elt_type=_tmpF4F;});({struct Cyc_Absyn_Exp*_tmpF50=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7F8.num_elts=_tmpF50;});_tmp7F8.fat_result=0;_tmp7F8.inline_call=0;_tmp7F8;});_tmp7F7.f1=_tmpF51;});_tmp7F7;});_tmp7F6[0]=_tmpF52;});_tmp7F6;});Cyc_Absyn_new_exp(_tmpF53,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF54;});
# 3033
goto _LL254;}case 476: _LL607: _LL608: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3034 "parse.y"
({union Cyc_YYSTYPE _tmpF57=Cyc_Exp_tok(({struct Cyc_List_List*_tmpF56=({struct Cyc_Absyn_Exp*_tmp7FA[1];({struct Cyc_Absyn_Exp*_tmpF55=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7FA[0]=_tmpF55;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7FA,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpF56,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF57;});
goto _LL254;}case 477: _LL609: _LL60A: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 3036 "parse.y"
({union Cyc_YYSTYPE _tmpF5D=Cyc_Exp_tok(({void*_tmpF5C=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF5B=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7FC;_tmp7FC.tag=37;({struct Cyc_Absyn_Exp*_tmpF58=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7FC.f1=_tmpF58;});({struct _dyneither_ptr*_tmpF5A=({struct _dyneither_ptr*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));({struct _dyneither_ptr _tmpF59=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp7FD[0]=_tmpF59;});_tmp7FD;});_tmp7FC.f2=_tmpF5A;});_tmp7FC;});_tmp7FB[0]=_tmpF5B;});_tmp7FB;});Cyc_Absyn_new_exp(_tmpF5C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF5D;});
goto _LL254;}case 478: _LL60B: _LL60C: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 3038 "parse.y"
({union Cyc_YYSTYPE _tmpF64=Cyc_Exp_tok(({void*_tmpF63=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF62=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7FF;_tmp7FF.tag=37;({struct Cyc_Absyn_Exp*_tmpF5F=({struct Cyc_Absyn_Exp*_tmpF5E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpF5E,(unsigned int)(yyyylsp[2]).first_line);});_tmp7FF.f1=_tmpF5F;});({struct _dyneither_ptr*_tmpF61=({struct _dyneither_ptr*_tmp800=_cycalloc(sizeof(*_tmp800));({struct _dyneither_ptr _tmpF60=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp800[0]=_tmpF60;});_tmp800;});_tmp7FF.f2=_tmpF61;});_tmp7FF;});_tmp7FE[0]=_tmpF62;});_tmp7FE;});Cyc_Absyn_new_exp(_tmpF63,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF64;});
goto _LL254;}case 479: _LL60D: _LL60E: {
# 3041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3040 "parse.y"
void*_tmp801=({struct _tuple8*_tmpF65=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpF65,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpF66=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp801,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpF66;});
goto _LL254;}case 480: _LL60F: _LL610: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3043 "parse.y"
struct _tuple20 _tmp802=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp803=_tmp802;int _tmp805;struct _dyneither_ptr _tmp804;_LL6DE: _tmp805=_tmp803.f1;_tmp804=_tmp803.f2;_LL6DF:;
({union Cyc_YYSTYPE _tmpF67=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp805,_tmp804,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpF67;});
goto _LL254;}case 481: _LL611: _LL612: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3048 "parse.y"
({union Cyc_YYSTYPE _tmpF68=Cyc_YY6(Cyc_Absyn_Bitnot);yyval=_tmpF68;});
goto _LL254;}case 482: _LL613: _LL614: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3049 "parse.y"
({union Cyc_YYSTYPE _tmpF69=Cyc_YY6(Cyc_Absyn_Not);yyval=_tmpF69;});
goto _LL254;}case 483: _LL615: _LL616: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3050 "parse.y"
({union Cyc_YYSTYPE _tmpF6A=Cyc_YY6(Cyc_Absyn_Minus);yyval=_tmpF6A;});
goto _LL254;}case 484: _LL617: _LL618: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3055 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 485: _LL619: _LL61A: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3057 "parse.y"
({union Cyc_YYSTYPE _tmpF6D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF6C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF6B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpF6C,_tmpF6B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF6D;});
goto _LL254;}case 486: _LL61B: _LL61C: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3059 "parse.y"
({union Cyc_YYSTYPE _tmpF6F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF6E=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpF6E,0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF6F;});
goto _LL254;}case 487: _LL61D: _LL61E: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3061 "parse.y"
({union Cyc_YYSTYPE _tmpF72=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF71=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpF70=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpF71,_tmpF70,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF72;});
goto _LL254;}case 488: _LL61F: _LL620: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3063 "parse.y"
({union Cyc_YYSTYPE _tmpF76=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF75=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpF74=({struct _dyneither_ptr*_tmp806=_cycalloc(sizeof(*_tmp806));({struct _dyneither_ptr _tmpF73=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp806[0]=_tmpF73;});_tmp806;});Cyc_Absyn_aggrmember_exp(_tmpF75,_tmpF74,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF76;});
goto _LL254;}case 489: _LL621: _LL622: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3065 "parse.y"
({union Cyc_YYSTYPE _tmpF7A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF79=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpF78=({struct _dyneither_ptr*_tmp807=_cycalloc(sizeof(*_tmp807));({struct _dyneither_ptr _tmpF77=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp807[0]=_tmpF77;});_tmp807;});Cyc_Absyn_aggrarrow_exp(_tmpF79,_tmpF78,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF7A;});
goto _LL254;}case 490: _LL623: _LL624: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3067 "parse.y"
({union Cyc_YYSTYPE _tmpF7C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF7B=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpF7B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF7C;});
goto _LL254;}case 491: _LL625: _LL626: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3069 "parse.y"
({union Cyc_YYSTYPE _tmpF7E=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF7D=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpF7D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF7E;});
goto _LL254;}case 492: _LL627: _LL628: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 3071 "parse.y"
({union Cyc_YYSTYPE _tmpF82=Cyc_Exp_tok(({void*_tmpF81=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp808=_cycalloc(sizeof(*_tmp808));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF80=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp809;_tmp809.tag=24;({struct _tuple8*_tmpF7F=Cyc_yyget_YY37(yyyyvsp[1]);_tmp809.f1=_tmpF7F;});_tmp809.f2=0;_tmp809;});_tmp808[0]=_tmpF80;});_tmp808;});Cyc_Absyn_new_exp(_tmpF81,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF82;});
goto _LL254;}case 493: _LL629: _LL62A: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);}));
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);}));
# 3073 "parse.y"
({union Cyc_YYSTYPE _tmpF87=Cyc_Exp_tok(({void*_tmpF86=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp80A=_cycalloc(sizeof(*_tmp80A));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF85=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80B;_tmp80B.tag=24;({struct _tuple8*_tmpF83=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80B.f1=_tmpF83;});({struct Cyc_List_List*_tmpF84=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp80B.f2=_tmpF84;});_tmp80B;});_tmp80A[0]=_tmpF85;});_tmp80A;});Cyc_Absyn_new_exp(_tmpF86,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF87;});
goto _LL254;}case 494: _LL62B: _LL62C: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);}));
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);}));
# 3075 "parse.y"
({union Cyc_YYSTYPE _tmpF8C=Cyc_Exp_tok(({void*_tmpF8B=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF8A=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80D;_tmp80D.tag=24;({struct _tuple8*_tmpF88=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80D.f1=_tmpF88;});({struct Cyc_List_List*_tmpF89=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp80D.f2=_tmpF89;});_tmp80D;});_tmp80C[0]=_tmpF8A;});_tmp80C;});Cyc_Absyn_new_exp(_tmpF8B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF8C;});
goto _LL254;}case 495: _LL62D: _LL62E: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3080 "parse.y"
({union Cyc_YYSTYPE _tmpF91=Cyc_YY3(({struct Cyc_List_List*_tmp80E=_cycalloc(sizeof(*_tmp80E));({void*_tmpF90=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF8F=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp810;_tmp810.tag=0;({struct _dyneither_ptr*_tmpF8E=({struct _dyneither_ptr*_tmp811=_cycalloc(sizeof(*_tmp811));({struct _dyneither_ptr _tmpF8D=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp811[0]=_tmpF8D;});_tmp811;});_tmp810.f1=_tmpF8E;});_tmp810;});_tmp80F[0]=_tmpF8F;});_tmp80F;});_tmp80E->hd=_tmpF90;});_tmp80E->tl=0;_tmp80E;}));yyval=_tmpF91;});
goto _LL254;}case 496: _LL62F: _LL630: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3083 "parse.y"
({union Cyc_YYSTYPE _tmpF96=Cyc_YY3(({struct Cyc_List_List*_tmp812=_cycalloc(sizeof(*_tmp812));({void*_tmpF95=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp813=_cycalloc_atomic(sizeof(*_tmp813));({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF94=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp814;_tmp814.tag=1;({unsigned int _tmpF93=({unsigned int _tmpF92=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpF92,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp814.f1=_tmpF93;});_tmp814;});_tmp813[0]=_tmpF94;});_tmp813;});_tmp812->hd=_tmpF95;});_tmp812->tl=0;_tmp812;}));yyval=_tmpF96;});
goto _LL254;}case 497: _LL631: _LL632: {
# 3086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3085 "parse.y"
({union Cyc_YYSTYPE _tmpF9C=Cyc_YY3(({struct Cyc_List_List*_tmp815=_cycalloc(sizeof(*_tmp815));({void*_tmpF9A=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp816=_cycalloc(sizeof(*_tmp816));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF99=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp817;_tmp817.tag=0;({struct _dyneither_ptr*_tmpF98=({struct _dyneither_ptr*_tmp818=_cycalloc(sizeof(*_tmp818));({struct _dyneither_ptr _tmpF97=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp818[0]=_tmpF97;});_tmp818;});_tmp817.f1=_tmpF98;});_tmp817;});_tmp816[0]=_tmpF99;});_tmp816;});_tmp815->hd=_tmpF9A;});({struct Cyc_List_List*_tmpF9B=Cyc_yyget_YY3(yyyyvsp[0]);_tmp815->tl=_tmpF9B;});_tmp815;}));yyval=_tmpF9C;});
goto _LL254;}case 498: _LL633: _LL634: {
# 3088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3088 "parse.y"
({union Cyc_YYSTYPE _tmpFA2=Cyc_YY3(({struct Cyc_List_List*_tmp819=_cycalloc(sizeof(*_tmp819));({void*_tmpFA0=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp81A=_cycalloc_atomic(sizeof(*_tmp81A));({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF9F=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp81B;_tmp81B.tag=1;({unsigned int _tmpF9E=({unsigned int _tmpF9D=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpF9D,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp81B.f1=_tmpF9E;});_tmp81B;});_tmp81A[0]=_tmpF9F;});_tmp81A;});_tmp819->hd=_tmpFA0;});({struct Cyc_List_List*_tmpFA1=Cyc_yyget_YY3(yyyyvsp[0]);_tmp819->tl=_tmpFA1;});_tmp819;}));yyval=_tmpFA2;});
goto _LL254;}case 499: _LL635: _LL636: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpFA4=Cyc_Exp_tok(({struct _tuple0*_tmpFA3=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpFA3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFA4;});
goto _LL254;}case 500: _LL637: _LL638: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3096 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 501: _LL639: _LL63A: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3098 "parse.y"
({union Cyc_YYSTYPE _tmpFA6=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFA5=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpFA5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFA6;});
goto _LL254;}case 502: _LL63B: _LL63C: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3100 "parse.y"
({union Cyc_YYSTYPE _tmpFA8=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFA7=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpFA7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFA8;});
goto _LL254;}case 503: _LL63D: _LL63E: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3102 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 504: _LL63F: _LL640: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);}));
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);}));
# 3107 "parse.y"
({union Cyc_YYSTYPE _tmpFAA=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpFA9=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpFA9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFAA;});
goto _LL254;}case 505: _LL641: _LL642: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 3109 "parse.y"
({union Cyc_YYSTYPE _tmpFAD=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpFAC=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpFAB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpFAC,_tmpFAB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFAD;});
goto _LL254;}case 506: _LL643: _LL644: {
# 3112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);}));
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);}));
# 3112 "parse.y"
({union Cyc_YYSTYPE _tmpFAF=Cyc_Exp_tok(({struct Cyc_List_List*_tmpFAE=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpFAE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFAF;});
goto _LL254;}case 507: _LL645: _LL646: {
# 3115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 3117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 3115 "parse.y"
({union Cyc_YYSTYPE _tmpFB5=Cyc_Exp_tok(({void*_tmpFB4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp81C=_cycalloc(sizeof(*_tmp81C));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFB3=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp81D;_tmp81D.tag=28;({struct _tuple0*_tmpFB0=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp81D.f1=_tmpFB0;});({struct Cyc_List_List*_tmpFB1=Cyc_yyget_YY40(yyyyvsp[2]);_tmp81D.f2=_tmpFB1;});({struct Cyc_List_List*_tmpFB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp81D.f3=_tmpFB2;});_tmp81D.f4=0;_tmp81D;});_tmp81C[0]=_tmpFB3;});_tmp81C;});Cyc_Absyn_new_exp(_tmpFB4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFB5;});
goto _LL254;}case 508: _LL647: _LL648: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);}));
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);}));
# 3118 "parse.y"
({union Cyc_YYSTYPE _tmpFB7=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpFB6=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpFB6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFB7;});
goto _LL254;}case 509: _LL649: _LL64A: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3122 "parse.y"
({union Cyc_YYSTYPE _tmpFB8=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));yyval=_tmpFB8;});
goto _LL254;}case 510: _LL64B: _LL64C: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpFBA=Cyc_YY4(({struct Cyc_List_List*_tmp81E=_cycalloc(sizeof(*_tmp81E));({struct Cyc_Absyn_Exp*_tmpFB9=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp81E->hd=_tmpFB9;});_tmp81E->tl=0;_tmp81E;}));yyval=_tmpFBA;});
goto _LL254;}case 511: _LL64D: _LL64E: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);}));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);}));
# 3130 "parse.y"
({union Cyc_YYSTYPE _tmpFBD=Cyc_YY4(({struct Cyc_List_List*_tmp81F=_cycalloc(sizeof(*_tmp81F));({struct Cyc_Absyn_Exp*_tmpFBB=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp81F->hd=_tmpFBB;});({struct Cyc_List_List*_tmpFBC=Cyc_yyget_YY4(yyyyvsp[0]);_tmp81F->tl=_tmpFBC;});_tmp81F;}));yyval=_tmpFBD;});
goto _LL254;}case 512: _LL64F: _LL650: {
# 3133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
({union Cyc_YYSTYPE _tmpFBF=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpFBE=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpFBE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFBF;});
goto _LL254;}case 513: _LL651: _LL652: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3137 "parse.y"
({union Cyc_YYSTYPE _tmpFC1=Cyc_Exp_tok(({char _tmpFC0=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpFC0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFC1;});
goto _LL254;}case 514: _LL653: _LL654: {
# 3140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3138 "parse.y"
({union Cyc_YYSTYPE _tmpFC3=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFC2=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpFC2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFC3;});
goto _LL254;}case 515: _LL655: _LL656: {
# 3141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3140 "parse.y"
struct _dyneither_ptr _tmp820=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp820);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp820,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3148
({union Cyc_YYSTYPE _tmpFC4=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp820,i,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpFC4;});
# 3150
goto _LL254;}case 516: _LL657: _LL658: {
# 3152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3151 "parse.y"
({union Cyc_YYSTYPE _tmpFC5=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpFC5;});
goto _LL254;}case 517: _LL659: _LL65A: {
# 3154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3155 "parse.y"
({union Cyc_YYSTYPE _tmpFC9=Cyc_QualId_tok(({struct _tuple0*_tmp821=_cycalloc(sizeof(*_tmp821));({union Cyc_Absyn_Nmspace _tmpFC6=Cyc_Absyn_Rel_n(0);_tmp821->f1=_tmpFC6;});({struct _dyneither_ptr*_tmpFC8=({struct _dyneither_ptr*_tmp822=_cycalloc(sizeof(*_tmp822));({struct _dyneither_ptr _tmpFC7=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp822[0]=_tmpFC7;});_tmp822;});_tmp821->f2=_tmpFC8;});_tmp821;}));yyval=_tmpFC9;});
goto _LL254;}case 518: _LL65B: _LL65C: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3156 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 519: _LL65D: _LL65E: {
# 3159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3159 "parse.y"
({union Cyc_YYSTYPE _tmpFCD=Cyc_QualId_tok(({struct _tuple0*_tmp823=_cycalloc(sizeof(*_tmp823));({union Cyc_Absyn_Nmspace _tmpFCA=Cyc_Absyn_Rel_n(0);_tmp823->f1=_tmpFCA;});({struct _dyneither_ptr*_tmpFCC=({struct _dyneither_ptr*_tmp824=_cycalloc(sizeof(*_tmp824));({struct _dyneither_ptr _tmpFCB=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp824[0]=_tmpFCB;});_tmp824;});_tmp823->f2=_tmpFCC;});_tmp823;}));yyval=_tmpFCD;});
goto _LL254;}case 520: _LL65F: _LL660: {
# 3162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3160 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 521: _LL661: _LL662: {
# 3163
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3165 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 522: _LL663: _LL664: {
# 3168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3166 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 523: _LL665: _LL666: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3169 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 524: _LL667: _LL668: {
# 3172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3170 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 525: _LL669: _LL66A: {
# 3173
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3175 "parse.y"
goto _LL254;}case 526: _LL66B: _LL66C: {
# 3177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);}));
# 3179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(({_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);}));
# 3175 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL254;}default: _LL66D: _LL66E:
# 3180
 goto _LL254;}_LL254:;}
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 378
yylsp_offset -=yylen;
# 392 "cycbison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 395
++ yylsp_offset;
if(yylen == 0){
# 398
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).first_column=yylloc.first_column;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 405
({int _tmpFCE=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpFCE;});
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
({int _tmpFD0=({short _tmpFCF=Cyc_yypgoto[_check_known_subscript_notnull(147,yyn - 146)];_tmpFCF + *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});yystate=_tmpFD0;});
if((yystate >= 0  && yystate <= 7145) && Cyc_yycheck[_check_known_subscript_notnull(7146,yystate)]== ((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 421
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(147,yyn - 146)];}
# 423
goto yynewstate;
# 425
yyerrlab:
# 427
 if(yyerrstatus == 0){
# 430
++ yynerrs;
# 433
yyn=(int)Cyc_yypact[yystate];
# 435
if(yyn > - 32768  && yyn < 7145){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;x < 293 / sizeof(char*);++ x){
# 445
if(Cyc_yycheck[_check_known_subscript_notnull(7146,x + yyn)]== x)
(({unsigned long _tmpFD1=({Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(293,x)])+ 15;});sze +=_tmpFD1;}),count ++);}
({struct _dyneither_ptr _tmpFD3=({unsigned int _tmp825=(unsigned int)(sze + 15);char*_tmp826=(char*)({struct _RegionHandle*_tmpFD2=yyregion;_region_malloc(_tmpFD2,_check_times(sizeof(char),_tmp825 + 1));});struct _dyneither_ptr _tmp828=_tag_dyneither(_tmp826,sizeof(char),_tmp825 + 1);{unsigned int _tmp827=_tmp825;unsigned int i;for(i=0;i < _tmp827;i ++){_tmp826[i]=(char)'\000';}_tmp826[_tmp827]=(char)0;}_tmp828;});msg=_tmpFD3;});
({struct _dyneither_ptr _tmpFD4=msg;Cyc_strcpy(_tmpFD4,({const char*_tmp829="parse error";_tag_dyneither(_tmp829,sizeof(char),12);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;x < 293 / sizeof(char*);++ x){
# 455
if(Cyc_yycheck[_check_known_subscript_notnull(7146,x + yyn)]== x){
# 457
({struct _dyneither_ptr _tmpFD5=msg;Cyc_strcat(_tmpFD5,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp82A=", expecting `";_tag_dyneither(_tmp82A,sizeof(char),14);}):(struct _dyneither_ptr)({const char*_tmp82B=" or `";_tag_dyneither(_tmp82B,sizeof(char),6);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(293,x)]);
({struct _dyneither_ptr _tmpFD6=msg;Cyc_strcat(_tmpFD6,({const char*_tmp82C="'";_tag_dyneither(_tmp82C,sizeof(char),2);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpFD8=({const char*_tmp82D="parse error";_tag_dyneither(_tmp82D,sizeof(char),12);});int _tmpFD7=yystate;Cyc_yyerror(_tmpFD8,_tmpFD7,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp82E=1;_npop_handler(0);return _tmp82E;}
# 489
yychar=-2;}
# 495
yyerrstatus=3;
# 497
goto yyerrhandle;
# 499
yyerrdefault:
# 509 "cycbison.simple"
 yyerrpop:
# 511
 if(yyssp_offset == 0){int _tmp82F=1;_npop_handler(0);return _tmp82F;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1070,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7145) || Cyc_yycheck[_check_known_subscript_notnull(7146,yyn)]!= 1)goto yyerrdefault;
# 537
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 540
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 544
if(yyn == 0)goto yyerrpop;}
# 546
if(yyn == 1069){
int _tmp830=0;_npop_handler(0);return _tmp830;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3178 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp83A=v;struct Cyc_Absyn_Stmt*_tmp854;struct Cyc_Absyn_Exp*_tmp853;struct _tuple0*_tmp852;struct _dyneither_ptr _tmp851;char _tmp850;union Cyc_Absyn_Cnst _tmp84F;switch((_tmp83A.Stmt_tok).tag){case 1: _LL6E1: _tmp84F=(_tmp83A.Int_tok).val;_LL6E2:
({struct Cyc_String_pa_PrintArg_struct _tmp83D;_tmp83D.tag=0;({struct _dyneither_ptr _tmpFD9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp84F));_tmp83D.f1=_tmpFD9;});({void*_tmp83B[1]={& _tmp83D};({struct Cyc___cycFILE*_tmpFDB=Cyc_stderr;struct _dyneither_ptr _tmpFDA=({const char*_tmp83C="%s";_tag_dyneither(_tmp83C,sizeof(char),3);});Cyc_fprintf(_tmpFDB,_tmpFDA,_tag_dyneither(_tmp83B,sizeof(void*),1));});});});goto _LL6E0;case 2: _LL6E3: _tmp850=(_tmp83A.Char_tok).val;_LL6E4:
({struct Cyc_Int_pa_PrintArg_struct _tmp840;_tmp840.tag=1;_tmp840.f1=(unsigned long)((int)_tmp850);({void*_tmp83E[1]={& _tmp840};({struct Cyc___cycFILE*_tmpFDD=Cyc_stderr;struct _dyneither_ptr _tmpFDC=({const char*_tmp83F="%c";_tag_dyneither(_tmp83F,sizeof(char),3);});Cyc_fprintf(_tmpFDD,_tmpFDC,_tag_dyneither(_tmp83E,sizeof(void*),1));});});});goto _LL6E0;case 3: _LL6E5: _tmp851=(_tmp83A.String_tok).val;_LL6E6:
({struct Cyc_String_pa_PrintArg_struct _tmp843;_tmp843.tag=0;_tmp843.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp851);({void*_tmp841[1]={& _tmp843};({struct Cyc___cycFILE*_tmpFDF=Cyc_stderr;struct _dyneither_ptr _tmpFDE=({const char*_tmp842="\"%s\"";_tag_dyneither(_tmp842,sizeof(char),5);});Cyc_fprintf(_tmpFDF,_tmpFDE,_tag_dyneither(_tmp841,sizeof(void*),1));});});});goto _LL6E0;case 5: _LL6E7: _tmp852=(_tmp83A.QualId_tok).val;_LL6E8:
({struct Cyc_String_pa_PrintArg_struct _tmp846;_tmp846.tag=0;({struct _dyneither_ptr _tmpFE0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp852));_tmp846.f1=_tmpFE0;});({void*_tmp844[1]={& _tmp846};({struct Cyc___cycFILE*_tmpFE2=Cyc_stderr;struct _dyneither_ptr _tmpFE1=({const char*_tmp845="%s";_tag_dyneither(_tmp845,sizeof(char),3);});Cyc_fprintf(_tmpFE2,_tmpFE1,_tag_dyneither(_tmp844,sizeof(void*),1));});});});goto _LL6E0;case 7: _LL6E9: _tmp853=(_tmp83A.Exp_tok).val;_LL6EA:
({struct Cyc_String_pa_PrintArg_struct _tmp849;_tmp849.tag=0;({struct _dyneither_ptr _tmpFE3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp853));_tmp849.f1=_tmpFE3;});({void*_tmp847[1]={& _tmp849};({struct Cyc___cycFILE*_tmpFE5=Cyc_stderr;struct _dyneither_ptr _tmpFE4=({const char*_tmp848="%s";_tag_dyneither(_tmp848,sizeof(char),3);});Cyc_fprintf(_tmpFE5,_tmpFE4,_tag_dyneither(_tmp847,sizeof(void*),1));});});});goto _LL6E0;case 8: _LL6EB: _tmp854=(_tmp83A.Stmt_tok).val;_LL6EC:
({struct Cyc_String_pa_PrintArg_struct _tmp84C;_tmp84C.tag=0;({struct _dyneither_ptr _tmpFE6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp854));_tmp84C.f1=_tmpFE6;});({void*_tmp84A[1]={& _tmp84C};({struct Cyc___cycFILE*_tmpFE8=Cyc_stderr;struct _dyneither_ptr _tmpFE7=({const char*_tmp84B="%s";_tag_dyneither(_tmp84B,sizeof(char),3);});Cyc_fprintf(_tmpFE8,_tmpFE7,_tag_dyneither(_tmp84A,sizeof(void*),1));});});});goto _LL6E0;default: _LL6ED: _LL6EE:
({void*_tmp84D=0;({struct Cyc___cycFILE*_tmpFEA=Cyc_stderr;struct _dyneither_ptr _tmpFE9=({const char*_tmp84E="?";_tag_dyneither(_tmp84E,sizeof(char),2);});Cyc_fprintf(_tmpFEA,_tmpFE9,_tag_dyneither(_tmp84D,sizeof(void*),0));});});goto _LL6E0;}_LL6E0:;}
# 3191
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp855=_new_region("yyr");struct _RegionHandle*yyr=& _tmp855;_push_region(yyr);
({struct _RegionHandle*_tmpFEB=yyr;Cyc_yyparse(_tmpFEB,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp856=Cyc_Parse_parse_result;_npop_handler(0);return _tmp856;};
# 3194
;_pop_region(yyr);};}

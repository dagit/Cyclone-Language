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
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 38
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 46
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Relations_Reln;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 917 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 934
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 952
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 957
extern void*Cyc_Absyn_sint_typ;
# 974
void*Cyc_Absyn_string_typ(void*rgn);
# 996
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1185
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 36 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 47
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 63
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 67
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 78
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 112 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 35 "tcexp.h"
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 34 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;struct Cyc_Port_Edit{unsigned int loc;struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};
# 78 "port.cyc"
typedef struct Cyc_Port_Edit*Cyc_Port_edit_t;
# 83
typedef struct Cyc_List_List*Cyc_Port_edits_t;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 85
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
return(int)e1 - (int)e2;}
# 93
typedef void*Cyc_Port_ctype_t;struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};
# 95
typedef struct Cyc_Port_Cvar*Cyc_Port_cvar_t;struct Cyc_Port_Cfield{void*qual;struct _dyneither_ptr*f;void*type;};
# 102
typedef struct Cyc_Port_Cfield*Cyc_Port_cfield_t;
# 107
typedef struct Cyc_List_List*Cyc_Port_cfields_t;struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Cvar;struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
# 137
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9};
# 150
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmpA=t;struct Cyc_Port_Cvar*_tmp4E;void*_tmp4D;struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4B;struct Cyc_List_List*_tmp4A;void*_tmp49;struct Cyc_Absyn_Aggrdecl*_tmp48;struct Cyc_List_List*_tmp47;void*_tmp46;void*_tmp45;void*_tmp44;void*_tmp43;void*_tmp42;void*_tmp41;void*_tmp40;void*_tmp3F;struct _dyneither_ptr*_tmp3E;switch(*((int*)_tmpA)){case 0: _LL1: _LL2:
 return({const char*_tmpB="const";_tag_dyneither(_tmpB,sizeof(char),6);});case 1: _LL3: _LL4:
 return({const char*_tmpC="notconst";_tag_dyneither(_tmpC,sizeof(char),9);});case 2: _LL5: _LL6:
 return({const char*_tmpD="thin";_tag_dyneither(_tmpD,sizeof(char),5);});case 3: _LL7: _LL8:
 return({const char*_tmpE="fat";_tag_dyneither(_tmpE,sizeof(char),4);});case 4: _LL9: _LLA:
 return({const char*_tmpF="void";_tag_dyneither(_tmpF,sizeof(char),5);});case 5: _LLB: _LLC:
 return({const char*_tmp10="zero";_tag_dyneither(_tmp10,sizeof(char),5);});case 6: _LLD: _LLE:
 return({const char*_tmp11="arith";_tag_dyneither(_tmp11,sizeof(char),6);});case 7: _LLF: _LL10:
 return({const char*_tmp12="heap";_tag_dyneither(_tmp12,sizeof(char),5);});case 8: _LL11: _LL12:
 return({const char*_tmp13="ZT";_tag_dyneither(_tmp13,sizeof(char),3);});case 9: _LL13: _LL14:
 return({const char*_tmp14="NZT";_tag_dyneither(_tmp14,sizeof(char),4);});case 10: _LL15: _tmp3E=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpA)->f1;_LL16:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E);({void*_tmp15[1]={& _tmp17};Cyc_aprintf(({const char*_tmp16="%s";_tag_dyneither(_tmp16,sizeof(char),3);}),_tag_dyneither(_tmp15,sizeof(void*),1));});});case 11: _LL17: _tmp43=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp42=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f2;_tmp41=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f3;_tmp40=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f4;_tmp3F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f5;_LL18:
# 166
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 168
Cyc_Port_ctype2string(deep,_tmp3F));({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp40));({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 167
Cyc_Port_ctype2string(deep,_tmp41));({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp42));({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 166
Cyc_Port_ctype2string(deep,_tmp43));({void*_tmp18[5]={& _tmp1A,& _tmp1B,& _tmp1C,& _tmp1D,& _tmp1E};Cyc_aprintf(({const char*_tmp19="ptr(%s,%s,%s,%s,%s)";_tag_dyneither(_tmp19,sizeof(char),20);}),_tag_dyneither(_tmp18,sizeof(void*),5));});});});});});});case 12: _LL19: _tmp46=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp45=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f2;_tmp44=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f3;_LL1A:
# 170
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23;_tmp23.tag=0;_tmp23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,_tmp44));({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp45));({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 170
Cyc_Port_ctype2string(deep,_tmp46));({void*_tmp1F[3]={& _tmp21,& _tmp22,& _tmp23};Cyc_aprintf(({const char*_tmp20="array(%s,%s,%s)";_tag_dyneither(_tmp20,sizeof(char),16);}),_tag_dyneither(_tmp1F,sizeof(void*),3));});});});});case 13: _LL1B: _tmp48=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA)->f1)->f1;_tmp47=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA)->f1)->f2;_LL1C: {
# 173
struct _dyneither_ptr s=_tmp48->kind == Cyc_Absyn_StructA?({const char*_tmp2D="struct";_tag_dyneither(_tmp2D,sizeof(char),7);}):({const char*_tmp2E="union";_tag_dyneither(_tmp2E,sizeof(char),6);});
if(!deep)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp48->name));({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp24[2]={& _tmp26,& _tmp27};Cyc_aprintf(({const char*_tmp25="%s %s";_tag_dyneither(_tmp25,sizeof(char),6);}),_tag_dyneither(_tmp24,sizeof(void*),2));});});});else{
# 177
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_cfields2string(0,_tmp47));({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 177
Cyc_Absynpp_qvar2string(_tmp48->name));({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp28[3]={& _tmp2A,& _tmp2B,& _tmp2C};Cyc_aprintf(({const char*_tmp29="%s %s {%s}";_tag_dyneither(_tmp29,sizeof(char),11);}),_tag_dyneither(_tmp28,sizeof(void*),3));});});});});}}case 14: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f2 != 0){_LL1D: _tmp4A=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp49=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f2;_LL1E:
# 179
 return Cyc_Port_ctype2string(deep,_tmp49);}else{_LL1F: _tmp4B=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f1;_LL20:
# 181
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp4B));({void*_tmp2F[1]={& _tmp31};Cyc_aprintf(({const char*_tmp30="aggr {%s}";_tag_dyneither(_tmp30,sizeof(char),10);}),_tag_dyneither(_tmp2F,sizeof(void*),1));});});}case 15: _LL21: _tmp4D=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp4C=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA)->f2;_LL22:
# 183
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp4D));({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp4C));({void*_tmp32[2]={& _tmp34,& _tmp35};Cyc_aprintf(({const char*_tmp33="fn(%s)->%s";_tag_dyneither(_tmp33,sizeof(char),11);}),_tag_dyneither(_tmp32,sizeof(void*),2));});});});default: _LL23: _tmp4E=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpA)->f1;_LL24:
# 185
 if((unsigned int)_tmp4E->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp4E->eq)));else{
if(!deep  || _tmp4E->uppers == 0  && _tmp4E->lowers == 0)
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp38;_tmp38.tag=1;_tmp38.f1=(unsigned long)_tmp4E->id;({void*_tmp36[1]={& _tmp38};Cyc_aprintf(({const char*_tmp37="var(%d)";_tag_dyneither(_tmp37,sizeof(char),8);}),_tag_dyneither(_tmp36,sizeof(void*),1));});});else{
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 192
Cyc_Port_ctypes2string(0,_tmp4E->uppers));({struct Cyc_Int_pa_PrintArg_struct _tmp3C;_tmp3C.tag=1;_tmp3C.f1=(unsigned long)_tmp4E->id;({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 190
Cyc_Port_ctypes2string(0,_tmp4E->lowers));({void*_tmp39[3]={& _tmp3B,& _tmp3C,& _tmp3D};Cyc_aprintf(({const char*_tmp3A="var([%s]<=%d<=[%s])";_tag_dyneither(_tmp3A,sizeof(char),20);}),_tag_dyneither(_tmp39,sizeof(void*),3));});});});});}}}_LL0:;}
# 195
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _dyneither_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=Cyc_Port_ctype2string(deep,t);_tmp4F;});}
struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,struct Cyc_List_List*xs){
struct _dyneither_ptr*_tmp50=({struct _dyneither_ptr*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=s;_tmp54;});
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp51=xs;
struct Cyc_List_List*_tmp52=xs->tl;
for(0;_tmp52 != 0;(_tmp51=_tmp52,_tmp52=_tmp52->tl)){
_tmp51->tl=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=_tmp50;_tmp53->tl=_tmp52;_tmp53;});}
# 204
return xs;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 206
static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,f->type));({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->qual));({void*_tmp56[3]={& _tmp58,& _tmp59,& _tmp5A};Cyc_aprintf(({const char*_tmp57="%s %s: %s";_tag_dyneither(_tmp57,sizeof(char),10);}),_tag_dyneither(_tmp56,sizeof(void*),3));});});});});
return({struct _dyneither_ptr*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=s;_tmp55;});}
# 211
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(({const char*_tmp5B=",";_tag_dyneither(_tmp5B,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts)));}struct Cyc_Port_Cfield;
# 214
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(({const char*_tmp5C=";";_tag_dyneither(_tmp5C,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs)));}
# 220
static void*Cyc_Port_notconst_ct(){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct(){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct(){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct(){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct(){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct(){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct _tmp5E;_tmp5E.tag=10;_tmp5E.f1=n;_tmp5E;});_tmp5D;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp60;_tmp60.tag=14;_tmp60.f1=fs;_tmp60.f2=0;_tmp60;});_tmp5F;});}
# 234
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp62;_tmp62.tag=13;_tmp62.f1=p;_tmp62;});_tmp61;});}
# 237
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 239
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp64;_tmp64.tag=11;_tmp64.f1=elt;_tmp64.f2=qual;_tmp64.f3=ptr_kind;_tmp64.f4=r;_tmp64.f5=zt;_tmp64;});_tmp63;});}
# 241
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp66;_tmp66.tag=12;_tmp66.f1=elt;_tmp66.f2=qual;_tmp66.f3=zt;_tmp66;});_tmp65;});}
# 244
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp68;_tmp68.tag=15;_tmp68.f1=return_type;_tmp68.f2=args;_tmp68;});_tmp67;});}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 247
static void*Cyc_Port_var(){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp6A;_tmp6A.tag=16;_tmp6A.f1=({struct Cyc_Port_Cvar*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->id=counter ++;_tmp6B->eq=0;_tmp6B->uppers=0;_tmp6B->lowers=0;_tmp6B;});_tmp6A;});_tmp69;});}
# 251
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 254
static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6F;_tmp6F.tag=1;_tmp6F.f1=(unsigned long)counter ++;({void*_tmp6D[1]={& _tmp6F};Cyc_aprintf(({const char*_tmp6E="`R%d";_tag_dyneither(_tmp6E,sizeof(char),5);}),_tag_dyneither(_tmp6D,sizeof(void*),1));});});
return({struct _dyneither_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=s;_tmp6C;});}
# 262
static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
# 264
static void*Cyc_Port_compress_ct(void*t){
void*_tmp70=t;void**_tmp7C;void***_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp79;switch(*((int*)_tmp70)){case 16: _LL26: _tmp7B=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->eq;_tmp7A=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->uppers;_tmp79=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->lowers;_LL27: {
# 267
void**_tmp71=*_tmp7B;
if((unsigned int)_tmp71){
# 271
void*r=Cyc_Port_compress_ct(*_tmp71);
if(*_tmp71 != r)*_tmp7B=({void**_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=r;_tmp72;});
return r;}
# 278
for(0;_tmp79 != 0;_tmp79=_tmp79->tl){
void*_tmp73=(void*)_tmp79->hd;void*_tmp74=_tmp73;switch(*((int*)_tmp74)){case 0: _LL2D: _LL2E:
 goto _LL30;case 9: _LL2F: _LL30:
 goto _LL32;case 4: _LL31: _LL32:
 goto _LL34;case 13: _LL33: _LL34:
 goto _LL36;case 15: _LL35: _LL36:
# 285
*_tmp7B=({void**_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=(void*)_tmp79->hd;_tmp75;});
return(void*)_tmp79->hd;default: _LL37: _LL38:
# 288
 goto _LL2C;}_LL2C:;}
# 291
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
void*_tmp76=(void*)_tmp7A->hd;void*_tmp77=_tmp76;switch(*((int*)_tmp77)){case 1: _LL3A: _LL3B:
 goto _LL3D;case 8: _LL3C: _LL3D:
 goto _LL3F;case 5: _LL3E: _LL3F:
 goto _LL41;case 13: _LL40: _LL41:
 goto _LL43;case 15: _LL42: _LL43:
# 298
*_tmp7B=({void**_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=(void*)_tmp7A->hd;_tmp78;});
return(void*)_tmp7A->hd;default: _LL44: _LL45:
# 301
 goto _LL39;}_LL39:;}
# 304
return t;}case 14: _LL28: _tmp7C=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp70)->f2;_LL29:
# 307
 if((unsigned int)_tmp7C)return Cyc_Port_compress_ct(*_tmp7C);else{
return t;}default: _LL2A: _LL2B:
# 311
 return t;}_LL25:;}
# 317
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp7D=Cyc_Port_compress_ct(t);void*_tmp7E=_tmp7D;void*_tmp95;struct Cyc_List_List*_tmp94;void*_tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;struct _dyneither_ptr*_tmp8B;switch(*((int*)_tmp7E)){case 0: _LL47: _LL48:
 goto _LL4A;case 1: _LL49: _LL4A:
 goto _LL4C;case 2: _LL4B: _LL4C:
 goto _LL4E;case 3: _LL4D: _LL4E:
 goto _LL50;case 4: _LL4F: _LL50:
 goto _LL52;case 5: _LL51: _LL52:
 goto _LL54;case 6: _LL53: _LL54:
 goto _LL56;case 8: _LL55: _LL56:
 goto _LL58;case 9: _LL57: _LL58:
 goto _LL5A;case 14: _LL59: _LL5A:
 goto _LL5C;case 13: _LL5B: _LL5C:
 goto _LL5E;case 16: _LL5D: _LL5E:
 goto _LL60;case 7: _LL5F: _LL60:
 return t;case 10: _LL61: _tmp8B=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp7E)->f1;_LL62:
# 333
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmp8B))
*instenv=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*instenv,_tmp8B,Cyc_Port_var());
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmp8B);case 11: _LL63: _tmp90=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp8F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f2;_tmp8E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f3;_tmp8D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f4;_tmp8C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f5;_LL64: {
# 337
struct _tuple10 _tmp7F=({struct _tuple10 _tmp85;_tmp85.f1=Cyc_Port_inst(instenv,_tmp90);_tmp85.f2=Cyc_Port_inst(instenv,_tmp8D);_tmp85;});struct _tuple10 _tmp80=_tmp7F;void*_tmp84;void*_tmp83;_LL6A: _tmp84=_tmp80.f1;_tmp83=_tmp80.f2;_LL6B:;
if(_tmp84 == _tmp90  && _tmp83 == _tmp8D)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp82;_tmp82.tag=11;_tmp82.f1=_tmp84;_tmp82.f2=_tmp8F;_tmp82.f3=_tmp8E;_tmp82.f4=_tmp83;_tmp82.f5=_tmp8C;_tmp82;});_tmp81;});}case 12: _LL65: _tmp93=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp92=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f2;_tmp91=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f3;_LL66: {
# 341
void*_tmp86=Cyc_Port_inst(instenv,_tmp93);
if(_tmp86 == _tmp93)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp88;_tmp88.tag=12;_tmp88.f1=_tmp86;_tmp88.f2=_tmp92;_tmp88.f3=_tmp91;_tmp88;});_tmp87;});}default: _LL67: _tmp95=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp94=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7E)->f2;_LL68:
# 345
 return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp8A;_tmp8A.tag=15;_tmp8A.f1=Cyc_Port_inst(instenv,_tmp95);_tmp8A.f2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp94);_tmp8A;});_tmp89;});}_LL46:;}
# 349
void*Cyc_Port_instantiate(void*t){
return Cyc_Port_inst(({struct Cyc_Dict_Dict*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp96;}),t);}
# 356
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmp97=ts;for(0;(unsigned int)_tmp97;_tmp97=_tmp97->tl){
void*_tmp98=Cyc_Port_compress_ct((void*)_tmp97->hd);
if(t == _tmp98)found=1;}}
# 362
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(0;ts != 0;ts=ts->tl){
if(t == Cyc_Port_compress_ct((void*)ts->hd))continue;
res=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->hd=(void*)ts->hd;_tmp99->tl=res;_tmp99;});}
# 368
return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 373
void Cyc_Port_generalize(int is_rgn,void*t){
t=Cyc_Port_compress_ct(t);{
void*_tmp9A=t;void*_tmpA5;struct Cyc_List_List*_tmpA4;void*_tmpA3;void*_tmpA2;void*_tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;void*_tmp9D;void*_tmp9C;struct Cyc_Port_Cvar*_tmp9B;switch(*((int*)_tmp9A)){case 16: _LL6D: _tmp9B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9A)->f1;_LL6E:
# 378
 _tmp9B->uppers=Cyc_Port_filter_self(t,_tmp9B->uppers);
_tmp9B->lowers=Cyc_Port_filter_self(t,_tmp9B->lowers);
if(is_rgn){
# 383
if(_tmp9B->uppers == 0  && _tmp9B->lowers == 0){
Cyc_Port_unifies(t,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));
return;}
# 388
if((unsigned int)_tmp9B->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp9B->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 392
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp9B->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 396
return;case 0: _LL6F: _LL70:
 goto _LL72;case 1: _LL71: _LL72:
 goto _LL74;case 2: _LL73: _LL74:
 goto _LL76;case 3: _LL75: _LL76:
 goto _LL78;case 4: _LL77: _LL78:
 goto _LL7A;case 5: _LL79: _LL7A:
 goto _LL7C;case 6: _LL7B: _LL7C:
 goto _LL7E;case 14: _LL7D: _LL7E:
 goto _LL80;case 13: _LL7F: _LL80:
 goto _LL82;case 10: _LL81: _LL82:
 goto _LL84;case 9: _LL83: _LL84:
 goto _LL86;case 8: _LL85: _LL86:
 goto _LL88;case 7: _LL87: _LL88:
 return;case 11: _LL89: _tmpA0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmp9F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f2;_tmp9E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f3;_tmp9D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f4;_tmp9C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f5;_LL8A:
# 413
 Cyc_Port_generalize(0,_tmpA0);Cyc_Port_generalize(1,_tmp9D);goto _LL6C;case 12: _LL8B: _tmpA3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmpA2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f2;_tmpA1=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f3;_LL8C:
# 415
 Cyc_Port_generalize(0,_tmpA3);Cyc_Port_generalize(0,_tmpA2);goto _LL6C;default: _LL8D: _tmpA5=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmpA4=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9A)->f2;_LL8E:
# 417
 Cyc_Port_generalize(0,_tmpA5);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmpA4);goto _LL6C;}_LL6C:;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 423
static int Cyc_Port_occurs(void*v,void*t){
t=Cyc_Port_compress_ct(t);
if(t == v)return 1;{
void*_tmpA6=t;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;void*_tmpB0;struct Cyc_List_List*_tmpAF;void*_tmpAE;void*_tmpAD;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;void*_tmpA8;void*_tmpA7;switch(*((int*)_tmpA6)){case 11: _LL90: _tmpAB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f2;_tmpA9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f3;_tmpA8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f4;_tmpA7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f5;_LL91:
# 428
 return(((Cyc_Port_occurs(v,_tmpAB) || Cyc_Port_occurs(v,_tmpAA)) || Cyc_Port_occurs(v,_tmpA9)) || Cyc_Port_occurs(v,_tmpA8)) || 
Cyc_Port_occurs(v,_tmpA7);case 12: _LL92: _tmpAE=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAD=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f2;_tmpAC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f3;_LL93:
# 431
 return(Cyc_Port_occurs(v,_tmpAE) || Cyc_Port_occurs(v,_tmpAD)) || Cyc_Port_occurs(v,_tmpAC);case 15: _LL94: _tmpB0=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAF=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA6)->f2;_LL95:
# 433
 if(Cyc_Port_occurs(v,_tmpB0))return 1;
for(0;(unsigned int)_tmpAF;_tmpAF=_tmpAF->tl){
if(Cyc_Port_occurs(v,(void*)_tmpAF->hd))return 1;}
return 0;case 13: _LL96: _tmpB1=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA6)->f1)->f2;_LL97:
 return 0;case 14: _LL98: _tmpB2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA6)->f1;_LL99:
# 439
 for(0;(unsigned int)_tmpB2;_tmpB2=_tmpB2->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpB2->hd)->qual) || Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpB2->hd)->type))return 1;}
return 0;default: _LL9A: _LL9B:
 return 0;}_LL8F:;};}char Cyc_Port_Unify_ct[9]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 451
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 453
static int Cyc_Port_leq(void*t1,void*t2);
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 457
static void Cyc_Port_unify_ct(void*t1,void*t2){
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);
if(t1 == t2)return;{
struct _tuple10 _tmpB4=({struct _tuple10 _tmpE3;_tmpE3.f1=t1;_tmpE3.f2=t2;_tmpE3;});struct _tuple10 _tmpB5=_tmpB4;struct Cyc_List_List*_tmpE2;void***_tmpE1;struct Cyc_Absyn_Aggrdecl*_tmpE0;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*_tmpDE;void***_tmpDD;struct Cyc_List_List*_tmpDC;void***_tmpDB;struct Cyc_Absyn_Aggrdecl*_tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpD8;void***_tmpD7;struct _tuple11*_tmpD6;struct _tuple11*_tmpD5;void*_tmpD4;struct Cyc_List_List*_tmpD3;void*_tmpD2;struct Cyc_List_List*_tmpD1;void*_tmpD0;void*_tmpCF;void*_tmpCE;void*_tmpCD;void*_tmpCC;void*_tmpCB;struct _dyneither_ptr _tmpCA;struct _dyneither_ptr _tmpC9;void*_tmpC8;void*_tmpC7;void*_tmpC6;void*_tmpC5;void*_tmpC4;void*_tmpC3;void*_tmpC2;void*_tmpC1;void*_tmpC0;void*_tmpBF;struct Cyc_Port_Cvar*_tmpBE;struct Cyc_Port_Cvar*_tmpBD;if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f1)->tag == 16){_LL9D: _tmpBD=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_LL9E:
# 463
 if(!Cyc_Port_occurs(t1,t2)){
# 466
{struct Cyc_List_List*_tmpB6=Cyc_Port_filter_self(t1,_tmpBD->uppers);for(0;(unsigned int)_tmpB6;_tmpB6=_tmpB6->tl){
if(!Cyc_Port_leq(t2,(void*)_tmpB6->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmpB7=Cyc_Port_filter_self(t1,_tmpBD->lowers);for(0;(unsigned int)_tmpB7;_tmpB7=_tmpB7->tl){
if(!Cyc_Port_leq((void*)_tmpB7->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
_tmpBD->eq=({void**_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=t2;_tmpB8;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}else{if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 16){_LL9F: _tmpBE=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLA0:
 Cyc_Port_unify_ct(t2,t1);return;}else{switch(*((int*)_tmpB5.f1)){case 11: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 11){_LLA1: _tmpC8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpC7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpC6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f3;_tmpC5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f4;_tmpC4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f5;_tmpC3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpC2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_tmpC1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f3;_tmpC0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f4;_tmpBF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f5;_LLA2:
# 475
 Cyc_Port_unify_ct(_tmpC8,_tmpC3);Cyc_Port_unify_ct(_tmpC7,_tmpC2);Cyc_Port_unify_ct(_tmpC6,_tmpC1);Cyc_Port_unify_ct(_tmpC5,_tmpC0);
Cyc_Port_unify_ct(_tmpC4,_tmpBF);
return;}else{goto _LLB1;}case 10: if(((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 10){_LLA3: _tmpCA=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpC9=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLA4:
# 479
 if(Cyc_strcmp((struct _dyneither_ptr)_tmpCA,(struct _dyneither_ptr)_tmpC9)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}else{goto _LLB1;}case 12: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 12){_LLA5: _tmpD0=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpCF=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpCE=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f3;_tmpCD=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpCC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_tmpCB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f3;_LLA6:
# 482
 Cyc_Port_unify_ct(_tmpD0,_tmpCD);Cyc_Port_unify_ct(_tmpCF,_tmpCC);Cyc_Port_unify_ct(_tmpCE,_tmpCB);return;}else{goto _LLB1;}case 15: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 15){_LLA7: _tmpD4=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpD3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpD2=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpD1=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLA8:
# 484
 Cyc_Port_unify_ct(_tmpD4,_tmpD2);Cyc_Port_unify_cts(_tmpD3,_tmpD1);return;}else{goto _LLB1;}case 13: switch(*((int*)_tmpB5.f2)){case 13: _LLA9: _tmpD6=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpD5=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLAA:
# 486
 if(_tmpD6 == _tmpD5)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}case 14: _LLAF: _tmpDA=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1)->f1;_tmpD9=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1)->f2;_tmpD8=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpD7=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLB0:
# 496
 Cyc_Port_merge_fields(_tmpD9,_tmpD8,0);
*_tmpD7=({void**_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=t1;_tmpBC;});
return;default: goto _LLB1;}case 14: switch(*((int*)_tmpB5.f2)){case 14: _LLAB: _tmpDE=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpDD=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpDC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpDB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLAC: {
# 488
void*_tmpB9=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmpDE,_tmpDC,1));
*_tmpDD=(*_tmpDB=({void**_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=_tmpB9;_tmpBA;}));
return;}case 13: _LLAD: _tmpE2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpE1=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpE0=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1)->f1;_tmpDF=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1)->f2;_LLAE:
# 492
 Cyc_Port_merge_fields(_tmpDF,_tmpE2,0);
*_tmpE1=({void**_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=t2;_tmpBB;});
return;default: goto _LLB1;}default: _LLB1: _LLB2:
# 499
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}_LL9C:;};}
# 503
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 507
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 513
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 515
struct Cyc_List_List*common=0;
{struct Cyc_List_List*_tmpE4=fs2;for(0;(unsigned int)_tmpE4;_tmpE4=_tmpE4->tl){
struct Cyc_Port_Cfield*_tmpE5=(struct Cyc_Port_Cfield*)_tmpE4->hd;
int found=0;
{struct Cyc_List_List*_tmpE6=fs1;for(0;(unsigned int)_tmpE6;_tmpE6=_tmpE6->tl){
struct Cyc_Port_Cfield*_tmpE7=(struct Cyc_Port_Cfield*)_tmpE6->hd;
if(Cyc_strptrcmp(_tmpE7->f,_tmpE5->f)== 0){
common=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=_tmpE7;_tmpE8->tl=common;_tmpE8;});
Cyc_Port_unify_ct(_tmpE7->qual,_tmpE5->qual);
Cyc_Port_unify_ct(_tmpE7->type,_tmpE5->type);
found=1;
break;}}}
# 529
if(!found){
if(allow_f1_subset_f2)
common=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=_tmpE5;_tmpE9->tl=common;_tmpE9;});else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 535
{struct Cyc_List_List*_tmpEA=fs1;for(0;(unsigned int)_tmpEA;_tmpEA=_tmpEA->tl){
struct Cyc_Port_Cfield*_tmpEB=(struct Cyc_Port_Cfield*)_tmpEA->hd;
int found=0;
{struct Cyc_List_List*_tmpEC=fs2;for(0;(unsigned int)_tmpEC;_tmpEC=_tmpEC->tl){
struct Cyc_Port_Cfield*_tmpED=(struct Cyc_Port_Cfield*)_tmpEC->hd;
if(Cyc_strptrcmp(_tmpEB->f,_tmpED->f))found=1;}}
# 542
if(!found)
common=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=_tmpEB;_tmpEE->tl=common;_tmpEE;});}}
# 545
return common;}
# 548
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmpEF;_push_handler(& _tmpEF);{int _tmpF1=0;if(setjmp(_tmpEF.handler))_tmpF1=1;if(!_tmpF1){
# 555
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmpF0=(void*)_exn_thrown;void*_tmpF2=_tmpF0;void*_tmpF3;if(((struct Cyc_Port_Unify_ct_exn_struct*)_tmpF2)->tag == Cyc_Port_Unify_ct){_LLB4: _LLB5:
# 562
 return 0;}else{_LLB6: _tmpF3=_tmpF2;_LLB7:(int)_rethrow(_tmpF3);}_LLB3:;}};}
# 564
return 1;}struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 570
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 572
t=Cyc_Port_compress_ct(t);
{void*_tmpF4=t;switch(*((int*)_tmpF4)){case 1: _LLB9: _LLBA:
# 576
 goto _LLBC;case 8: _LLBB: _LLBC:
 goto _LLBE;case 5: _LLBD: _LLBE:
 goto _LLC0;case 2: _LLBF: _LLC0:
 goto _LLC2;case 3: _LLC1: _LLC2:
 goto _LLC4;case 12: _LLC3: _LLC4:
 goto _LLC6;case 13: _LLC5: _LLC6:
 goto _LLC8;case 15: _LLC7: _LLC8:
 goto _LLCA;case 7: _LLC9: _LLCA:
# 587
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4: _LLCB: _LLCC:
# 592
 goto _LLCE;case 0: _LLCD: _LLCE:
 goto _LLD0;case 9: _LLCF: _LLD0:
# 595
 return*uppers;default: _LLD1: _LLD2:
 goto _LLB8;}_LLB8:;}
# 599
{struct Cyc_List_List*_tmpF5=*uppers;for(0;(unsigned int)_tmpF5;_tmpF5=_tmpF5->tl){
void*_tmpF6=Cyc_Port_compress_ct((void*)_tmpF5->hd);
# 602
if(t == _tmpF6)return*uppers;{
struct _tuple10 _tmpF7=({struct _tuple10 _tmp10E;_tmp10E.f1=t;_tmp10E.f2=_tmpF6;_tmp10E;});struct _tuple10 _tmpF8=_tmpF7;void*_tmp10D;void*_tmp10C;void*_tmp10B;void*_tmp10A;void*_tmp109;void*_tmp108;void*_tmp107;void*_tmp106;void*_tmp105;void*_tmp104;switch(*((int*)_tmpF8.f1)){case 6: switch(*((int*)_tmpF8.f2)){case 11: _LLD4: _LLD5:
# 607
 goto _LLD7;case 5: _LLD6: _LLD7:
 goto _LLD9;case 12: _LLD8: _LLD9:
# 610
 return*uppers;default: goto _LLDC;}case 11: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->tag == 11){_LLDA: _tmp10D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f1;_tmp10C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f2;_tmp10B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f3;_tmp10A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f4;_tmp109=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f5;_tmp108=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f1;_tmp107=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f2;_tmp106=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f3;_tmp105=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f4;_tmp104=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f5;_LLDB: {
# 615
struct _tuple12 _tmpF9=({struct _tuple12 _tmp103;_tmp103.f1=Cyc_Port_var();_tmp103.f2=Cyc_Port_var();_tmp103.f3=Cyc_Port_var();_tmp103.f4=Cyc_Port_var();_tmp103.f5=Cyc_Port_var();_tmp103;});struct _tuple12 _tmpFA=_tmpF9;void*_tmp102;void*_tmp101;void*_tmp100;void*_tmpFF;void*_tmpFE;_LLDF: _tmp102=_tmpFA.f1;_tmp101=_tmpFA.f2;_tmp100=_tmpFA.f3;_tmpFF=_tmpFA.f4;_tmpFE=_tmpFA.f5;_LLE0:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFB=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmpFD;_tmpFD.tag=11;_tmpFD.f1=_tmp102;_tmpFD.f2=_tmp101;_tmpFD.f3=_tmp100;_tmpFD.f4=_tmpFF;_tmpFD.f5=_tmpFE;_tmpFD;});_tmpFC;});
Cyc_Port_leq(_tmp102,_tmp10D);Cyc_Port_leq(_tmp102,_tmp108);
Cyc_Port_leq(_tmp101,_tmp10C);Cyc_Port_leq(_tmp101,_tmp107);
Cyc_Port_leq(_tmp100,_tmp10B);Cyc_Port_leq(_tmp100,_tmp107);
Cyc_Port_leq(_tmpFF,_tmp10A);Cyc_Port_leq(_tmpFF,_tmp105);
Cyc_Port_leq(_tmpFE,_tmp109);Cyc_Port_leq(_tmpFE,_tmp104);
_tmpF5->hd=(void*)((void*)_tmpFB);
return*uppers;};}}else{goto _LLDC;}default: _LLDC: _LLDD:
 goto _LLD3;}_LLD3:;};}}
# 627
return({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=t;_tmp10F->tl=*uppers;_tmp10F;});}
# 632
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 634
t=Cyc_Port_compress_ct(t);
{void*_tmp110=t;switch(*((int*)_tmp110)){case 0: _LLE2: _LLE3:
 goto _LLE5;case 8: _LLE4: _LLE5:
 goto _LLE7;case 2: _LLE6: _LLE7:
 goto _LLE9;case 3: _LLE8: _LLE9:
 goto _LLEB;case 4: _LLEA: _LLEB:
 goto _LLED;case 13: _LLEC: _LLED:
 goto _LLEF;case 15: _LLEE: _LLEF:
 goto _LLF1;case 10: _LLF0: _LLF1:
# 644
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7: _LLF2: _LLF3:
 goto _LLF5;case 1: _LLF4: _LLF5:
 goto _LLF7;case 9: _LLF6: _LLF7:
# 650
 return*lowers;default: _LLF8: _LLF9:
# 652
 goto _LLE1;}_LLE1:;}
# 654
{struct Cyc_List_List*_tmp111=*lowers;for(0;(unsigned int)_tmp111;_tmp111=_tmp111->tl){
void*_tmp112=Cyc_Port_compress_ct((void*)_tmp111->hd);
if(t == _tmp112)return*lowers;{
struct _tuple10 _tmp113=({struct _tuple10 _tmp12A;_tmp12A.f1=t;_tmp12A.f2=_tmp112;_tmp12A;});struct _tuple10 _tmp114=_tmp113;void*_tmp129;void*_tmp128;void*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;if(((struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp114.f2)->tag == 4){_LLFB: _LLFC:
 goto _LLFE;}else{switch(*((int*)_tmp114.f1)){case 5: switch(*((int*)_tmp114.f2)){case 6: _LLFD: _LLFE:
 goto _LL100;case 11: _LLFF: _LL100:
 goto _LL102;default: goto _LL107;}case 11: switch(*((int*)_tmp114.f2)){case 6: _LL101: _LL102:
 goto _LL104;case 11: _LL105: _tmp129=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f1;_tmp128=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f2;_tmp127=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f3;_tmp126=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f4;_tmp125=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f5;_tmp124=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f1;_tmp123=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f2;_tmp122=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f3;_tmp121=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f4;_tmp120=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f5;_LL106: {
# 668
struct _tuple12 _tmp115=({struct _tuple12 _tmp11F;_tmp11F.f1=Cyc_Port_var();_tmp11F.f2=Cyc_Port_var();_tmp11F.f3=Cyc_Port_var();_tmp11F.f4=Cyc_Port_var();_tmp11F.f5=Cyc_Port_var();_tmp11F;});struct _tuple12 _tmp116=_tmp115;void*_tmp11E;void*_tmp11D;void*_tmp11C;void*_tmp11B;void*_tmp11A;_LL10A: _tmp11E=_tmp116.f1;_tmp11D=_tmp116.f2;_tmp11C=_tmp116.f3;_tmp11B=_tmp116.f4;_tmp11A=_tmp116.f5;_LL10B:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp117=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp119;_tmp119.tag=11;_tmp119.f1=_tmp11E;_tmp119.f2=_tmp11D;_tmp119.f3=_tmp11C;_tmp119.f4=_tmp11B;_tmp119.f5=_tmp11A;_tmp119;});_tmp118;});
Cyc_Port_leq(_tmp129,_tmp11E);Cyc_Port_leq(_tmp124,_tmp11E);
Cyc_Port_leq(_tmp128,_tmp11D);Cyc_Port_leq(_tmp123,_tmp11D);
Cyc_Port_leq(_tmp127,_tmp11C);Cyc_Port_leq(_tmp123,_tmp11C);
Cyc_Port_leq(_tmp126,_tmp11B);Cyc_Port_leq(_tmp121,_tmp11B);
Cyc_Port_leq(_tmp125,_tmp11A);Cyc_Port_leq(_tmp120,_tmp11A);
_tmp111->hd=(void*)((void*)_tmp117);
return*lowers;};}default: goto _LL107;}case 12: if(((struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp114.f2)->tag == 6){_LL103: _LL104:
# 663
 return*lowers;}else{goto _LL107;}default: _LL107: _LL108:
# 677
 goto _LLFA;}}_LLFA:;};}}
# 680
return({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->hd=t;_tmp12B->tl=*lowers;_tmp12B;});}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 687
static int Cyc_Port_leq(void*t1,void*t2){
# 693
if(t1 == t2)return 1;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{
struct _tuple10 _tmp12C=({struct _tuple10 _tmp14C;_tmp14C.f1=t1;_tmp14C.f2=t2;_tmp14C;});struct _tuple10 _tmp12D=_tmp12C;struct Cyc_Port_Cvar*_tmp14B;void*_tmp14A;void*_tmp149;void*_tmp148;void*_tmp147;void*_tmp146;void*_tmp145;void*_tmp144;void*_tmp143;void*_tmp142;void*_tmp141;void*_tmp140;void*_tmp13F;void*_tmp13E;void*_tmp13D;void*_tmp13C;void*_tmp13B;void*_tmp13A;void*_tmp139;void*_tmp138;void*_tmp137;void*_tmp136;void*_tmp135;void*_tmp134;struct Cyc_Port_Cvar*_tmp133;struct Cyc_Port_Cvar*_tmp132;struct Cyc_Port_Cvar*_tmp131;struct _dyneither_ptr _tmp130;struct _dyneither_ptr _tmp12F;struct _dyneither_ptr _tmp12E;switch(*((int*)_tmp12D.f1)){case 7: _LL10D: _LL10E:
 return 1;case 10: switch(*((int*)_tmp12D.f2)){case 10: _LL10F: _tmp12F=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp12E=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL110:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp12F,(struct _dyneither_ptr)_tmp12E)== 0;case 7: _LL111: _tmp130=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_LL112:
 return 0;case 16: goto _LL139;default: goto _LL13B;}case 1: switch(*((int*)_tmp12D.f2)){case 0: _LL113: _LL114:
 return 1;case 16: goto _LL139;default: goto _LL13B;}case 0: switch(*((int*)_tmp12D.f2)){case 1: _LL115: _LL116:
 return 0;case 16: goto _LL139;default: goto _LL13B;}case 9: switch(*((int*)_tmp12D.f2)){case 8: _LL117: _LL118:
 return 0;case 16: goto _LL139;default: goto _LL13B;}case 8: switch(*((int*)_tmp12D.f2)){case 9: _LL119: _LL11A:
 return 1;case 16: goto _LL139;default: goto _LL13B;}case 16: switch(*((int*)_tmp12D.f2)){case 0: _LL11B: _LL11C:
 return 1;case 4: _LL11D: _LL11E:
 return 1;case 16: _LL135: _tmp132=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp131=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL136:
# 723
 _tmp132->uppers=Cyc_Port_filter_self(t1,_tmp132->uppers);
_tmp131->lowers=Cyc_Port_filter_self(t2,_tmp131->lowers);
_tmp132->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp132->uppers);
_tmp131->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp131->lowers);
return 1;default: _LL137: _tmp133=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_LL138:
# 729
 _tmp133->uppers=Cyc_Port_filter_self(t1,_tmp133->uppers);
_tmp133->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp133->uppers);
return 1;}case 4: _LL11F: _LL120:
# 706
 return 0;case 5: switch(*((int*)_tmp12D.f2)){case 6: _LL121: _LL122:
 return 1;case 11: _LL123: _LL124:
 return 1;case 4: _LL125: _LL126:
 return 1;case 16: goto _LL139;default: goto _LL13B;}case 11: switch(*((int*)_tmp12D.f2)){case 6: _LL127: _LL128:
 return 1;case 4: _LL129: _LL12A:
 return 1;case 11: _LL12F: _tmp13D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp13C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp13B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp13A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f4;_tmp139=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f5;_tmp138=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp137=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp136=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_tmp135=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f4;_tmp134=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f5;_LL130:
# 715
 return(((Cyc_Port_leq(_tmp13D,_tmp138) && Cyc_Port_leq(_tmp13C,_tmp137)) && Cyc_Port_unifies(_tmp13B,_tmp136)) && Cyc_Port_leq(_tmp13A,_tmp135)) && 
Cyc_Port_leq(_tmp139,_tmp134);case 16: goto _LL139;default: goto _LL13B;}case 12: switch(*((int*)_tmp12D.f2)){case 6: _LL12B: _LL12C:
# 712
 return 1;case 4: _LL12D: _LL12E:
 return 1;case 12: _LL131: _tmp143=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp142=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp141=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp140=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp13F=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp13E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_LL132:
# 718
 return(Cyc_Port_leq(_tmp143,_tmp140) && Cyc_Port_leq(_tmp142,_tmp13F)) && Cyc_Port_leq(_tmp141,_tmp13E);case 11: _LL133: _tmp14A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp149=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp148=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp147=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp146=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp145=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_tmp144=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f5;_LL134:
# 720
 return((Cyc_Port_leq(_tmp14A,_tmp147) && Cyc_Port_leq(_tmp149,_tmp146)) && Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp145)) && 
Cyc_Port_leq(_tmp148,_tmp144);case 16: goto _LL139;default: goto _LL13B;}default: if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->tag == 16){_LL139: _tmp14B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL13A:
# 733
 _tmp14B->lowers=Cyc_Port_filter_self(t2,_tmp14B->lowers);
_tmp14B->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp14B->lowers);
return 1;}else{_LL13B: _LL13C:
 return Cyc_Port_unifies(t1,t2);}}_LL10C:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*edits;int porting;};
# 742
typedef struct Cyc_Port_GlobalCenv*Cyc_Port_gcenv_t;
# 765
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0,Cyc_Port_FnArg_pos  = 1,Cyc_Port_FnBody_pos  = 2,Cyc_Port_Toplevel_pos  = 3};
typedef enum Cyc_Port_CPos Cyc_Port_cpos_t;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 768
typedef struct Cyc_Port_Cenv*Cyc_Port_cenv_t;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;
# 778
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->typedef_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->struct_dict=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->union_dict=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->qualifier_edits=0;_tmp14E->pointer_edits=0;_tmp14E->zeroterm_edits=0;_tmp14E->edits=0;_tmp14E->porting=0;_tmp14E->return_type=
# 787
Cyc_Port_void_ct();_tmp14E;});
return({struct Cyc_Port_Cenv*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->gcenv=g;_tmp14D->cpos=Cyc_Port_Toplevel_pos;_tmp14D->var_dict=
# 790
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14D;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 796
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnArg_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 799
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnRes_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 802
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_Toplevel_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 805
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return(env->gcenv)->return_type;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 808
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp14F;_push_handler(& _tmp14F);{int _tmp151=0;if(setjmp(_tmp14F.handler))_tmp151=1;if(!_tmp151){
{struct _tuple10 _tmp152=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple10 _tmp153=_tmp152;void*_tmp155;_LL13E: _tmp155=_tmp153.f1;_LL13F:;{
void*_tmp154=_tmp155;_npop_handler(0);return _tmp154;};}
# 810
;_pop_handler();}else{void*_tmp150=(void*)_exn_thrown;void*_tmp156=_tmp150;void*_tmp157;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp156)->tag == Cyc_Dict_Absent){_LL141: _LL142:
# 817
 return Cyc_Absyn_sint_typ;}else{_LL143: _tmp157=_tmp156;_LL144:(int)_rethrow(_tmp157);}_LL140:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 822
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){
{struct _tuple10 _tmp15B=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple10 _tmp15C=_tmp15B;void*_tmp15E;_LL146: _tmp15E=_tmp15C.f2;_LL147:;{
void*_tmp15D=_tmp15E;_npop_handler(0);return _tmp15D;};}
# 824
;_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15F=_tmp159;void*_tmp160;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15F)->tag == Cyc_Dict_Absent){_LL149: _LL14A:
# 831
 return Cyc_Port_var();}else{_LL14B: _tmp160=_tmp15F;_LL14C:(int)_rethrow(_tmp160);}_LL148:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 837
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 839
struct _tuple11**_tmp161=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);
if((unsigned int)_tmp161)
return*_tmp161;else{
# 843
struct Cyc_Absyn_Aggrdecl*_tmp162=({struct Cyc_Absyn_Aggrdecl*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->kind=Cyc_Absyn_StructA;_tmp164->sc=Cyc_Absyn_Public;_tmp164->name=n;_tmp164->tvs=0;_tmp164->impl=0;_tmp164->attributes=0;_tmp164->expected_mem_kind=0;_tmp164;});
# 846
struct _tuple11*p=({struct _tuple11*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->f1=_tmp162;_tmp163->f2=0;_tmp163;});
(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 854
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 856
struct _tuple11**_tmp165=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);
if((unsigned int)_tmp165)
return*_tmp165;else{
# 860
struct Cyc_Absyn_Aggrdecl*_tmp166=({struct Cyc_Absyn_Aggrdecl*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->kind=Cyc_Absyn_UnionA;_tmp168->sc=Cyc_Absyn_Public;_tmp168->name=n;_tmp168->tvs=0;_tmp168->impl=0;_tmp168->attributes=0;_tmp168->expected_mem_kind=0;_tmp168;});
# 863
struct _tuple11*p=({struct _tuple11*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->f1=_tmp166;_tmp167->f2=0;_tmp167;});
(env->gcenv)->union_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple13{void*f1;struct _tuple10*f2;};
# 870
static struct _tuple10 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){
{struct _tuple13 _tmp16C=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple13 _tmp16D=_tmp16C;struct _tuple10*_tmp16F;_LL14E: _tmp16F=_tmp16D.f2;_LL14F:;{
struct _tuple10 _tmp16E=*_tmp16F;_npop_handler(0);return _tmp16E;};}
# 872
;_pop_handler();}else{void*_tmp16A=(void*)_exn_thrown;void*_tmp170=_tmp16A;void*_tmp172;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp170)->tag == Cyc_Dict_Absent){_LL151: _LL152:
# 879
 return({struct _tuple10 _tmp171;_tmp171.f1=Cyc_Port_var();_tmp171.f2=Cyc_Port_var();_tmp171;});}else{_LL153: _tmp172=_tmp170;_LL154:(int)_rethrow(_tmp172);}_LL150:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 882
static struct _tuple13*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 886
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 890
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
{struct _tuple13 _tmp176=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple13 _tmp177=_tmp176;void*_tmp17C;_LL156: _tmp17C=_tmp177.f1;_LL157:;
LOOP: {void*_tmp178=_tmp17C;struct _tuple0*_tmp17B;switch(*((int*)_tmp178)){case 17: _LL159: _tmp17B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp178)->f1;_LL15A:
 _tmp17C=Cyc_Port_lookup_typedef(env,_tmp17B);goto LOOP;case 9: _LL15B: _LL15C: {
int _tmp179=1;_npop_handler(0);return _tmp179;}default: _LL15D: _LL15E: {
int _tmp17A=0;_npop_handler(0);return _tmp17A;}}_LL158:;}}
# 892
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp17D=_tmp174;void*_tmp17E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp17D)->tag == Cyc_Dict_Absent){_LL160: _LL161:
# 903
 return 0;}else{_LL162: _tmp17E=_tmp17D;_LL163:(int)_rethrow(_tmp17E);}_LL15F:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 907
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp17F;_push_handler(& _tmp17F);{int _tmp181=0;if(setjmp(_tmp17F.handler))_tmp181=1;if(!_tmp181){
{struct _tuple13 _tmp182=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple13 _tmp183=_tmp182;void*_tmp188;_LL165: _tmp188=_tmp183.f1;_LL166:;
LOOP: {void*_tmp184=_tmp188;struct _tuple0*_tmp187;switch(*((int*)_tmp184)){case 17: _LL168: _tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp184)->f1;_LL169:
 _tmp188=Cyc_Port_lookup_typedef(env,_tmp187);goto LOOP;case 8: _LL16A: _LL16B: {
int _tmp185=1;_npop_handler(0);return _tmp185;}default: _LL16C: _LL16D: {
int _tmp186=0;_npop_handler(0);return _tmp186;}}_LL167:;}}
# 909
;_pop_handler();}else{void*_tmp180=(void*)_exn_thrown;void*_tmp189=_tmp180;void*_tmp18A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp189)->tag == Cyc_Dict_Absent){_LL16F: _LL170:
# 920
 return 0;}else{_LL171: _tmp18A=_tmp189;_LL172:(int)_rethrow(_tmp18A);}_LL16E:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 926
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
return({struct Cyc_Port_Cenv*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->gcenv=env->gcenv;_tmp18B->var_dict=env->var_dict;_tmp18B->cpos=cpos;_tmp18B;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 931
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
(env->gcenv)->return_type=t;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 936
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype){
# 938
return({struct Cyc_Port_Cenv*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->gcenv=env->gcenv;_tmp18C->var_dict=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(env->var_dict,x,({struct _tuple13*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=t;_tmp18D->f2=({struct _tuple10*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->f1=qual;_tmp18E->f2=ctype;_tmp18E;});_tmp18D;}));_tmp18C->cpos=env->cpos;_tmp18C;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 944
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
(env->gcenv)->typedef_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->typedef_dict,n,({struct _tuple10*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->f1=t;_tmp18F->f2=ct;_tmp18F;}));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple14{void*f1;void*f2;unsigned int f3;};
# 951
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned int loc){
# 953
(env->gcenv)->qualifier_edits=({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=({struct _tuple14*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->f1=new_qual;_tmp191->f2=orig_qual;_tmp191->f3=loc;_tmp191;});_tmp190->tl=(env->gcenv)->qualifier_edits;_tmp190;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 959
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned int loc){
# 961
(env->gcenv)->pointer_edits=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=({struct _tuple14*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->f1=new_ptr;_tmp193->f2=orig_ptr;_tmp193->f3=loc;_tmp193;});_tmp192->tl=(env->gcenv)->pointer_edits;_tmp192;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 965
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned int loc){
# 967
(env->gcenv)->zeroterm_edits=({struct Cyc_List_List*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->hd=({struct _tuple14*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->f1=new_zt;_tmp195->f2=orig_zt;_tmp195->f3=loc;_tmp195;});_tmp194->tl=(env->gcenv)->zeroterm_edits;_tmp194;});}
# 973
static void*Cyc_Port_main_type(){
struct _tuple8*arg1=({struct _tuple8*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->f1=0;_tmp19B->f2=
Cyc_Absyn_empty_tqual(0);_tmp19B->f3=Cyc_Absyn_sint_typ;_tmp19B;});
struct _tuple8*arg2=({struct _tuple8*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->f1=0;_tmp19A->f2=
Cyc_Absyn_empty_tqual(0);_tmp19A->f3=
Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0)),Cyc_Absyn_wildtyp(0),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());_tmp19A;});
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp197;_tmp197.tag=9;_tmp197.f1=({struct Cyc_Absyn_FnInfo _tmp198;_tmp198.tvars=0;_tmp198.effect=0;_tmp198.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp198.ret_typ=Cyc_Absyn_sint_typ;_tmp198.args=({struct _tuple8*_tmp199[2];_tmp199[1]=arg2;_tmp199[0]=arg1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp199,sizeof(struct _tuple8*),2));});_tmp198.c_varargs=0;_tmp198.cyc_varargs=0;_tmp198.rgn_po=0;_tmp198.attributes=0;_tmp198.requires_clause=0;_tmp198.requires_relns=0;_tmp198.ensures_clause=0;_tmp198.ensures_relns=0;_tmp198;});_tmp197;});_tmp196;});}struct Cyc_Port_Cenv;
# 992
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 995
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp19C=Cyc_Port_empty_cenv();
# 998
_tmp19C=Cyc_Port_add_var(_tmp19C,({struct _tuple0*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->f1=Cyc_Absyn_Loc_n;_tmp19D->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",sizeof(char),5);_tmp19D;}),Cyc_Port_main_type(),Cyc_Port_const_ct(),
Cyc_Port_type_to_ctype(_tmp19C,Cyc_Port_main_type()));
return _tmp19C;}
# 1006
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp19F=Cyc_Port_compress_ct(t);void*_tmp1A0=_tmp19F;void*_tmp1AD;struct Cyc_List_List*_tmp1AC;void*_tmp1AB;void*_tmp1AA;void*_tmp1A9;void*_tmp1A8;void*_tmp1A7;void*_tmp1A6;void*_tmp1A5;void*_tmp1A4;struct _dyneither_ptr*_tmp1A3;switch(*((int*)_tmp1A0)){case 0: _LL174: _LL175:
 goto _LL177;case 1: _LL176: _LL177:
 goto _LL179;case 2: _LL178: _LL179:
 goto _LL17B;case 3: _LL17A: _LL17B:
 goto _LL17D;case 4: _LL17C: _LL17D:
 goto _LL17F;case 5: _LL17E: _LL17F:
 goto _LL181;case 6: _LL180: _LL181:
 goto _LL183;case 14: _LL182: _LL183:
 goto _LL185;case 13: _LL184: _LL185:
 goto _LL187;case 16: _LL186: _LL187:
 goto _LL189;case 8: _LL188: _LL189:
 goto _LL18B;case 9: _LL18A: _LL18B:
 goto _LL18D;case 7: _LL18C: _LL18D:
 return;case 10: _LL18E: _tmp1A3=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1A0)->f1;_LL18F:
# 1022
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,_tmp1A3))
*cts=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,int*v))Cyc_Dict_insert)(*cts,_tmp1A3,({int*_tmp1A1=_cycalloc_atomic(sizeof(*_tmp1A1));_tmp1A1[0]=0;_tmp1A1;}));{
int*_tmp1A2=((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,_tmp1A3);
*_tmp1A2=*_tmp1A2 + 1;
return;};case 11: _LL190: _tmp1A8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A0)->f1;_tmp1A7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A0)->f2;_tmp1A6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A0)->f3;_tmp1A5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A0)->f4;_tmp1A4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A0)->f5;_LL191:
# 1028
 Cyc_Port_region_counts(cts,_tmp1A8);
Cyc_Port_region_counts(cts,_tmp1A5);
return;case 12: _LL192: _tmp1AB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A0)->f1;_tmp1AA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A0)->f2;_tmp1A9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A0)->f3;_LL193:
# 1032
 Cyc_Port_region_counts(cts,_tmp1AB);
return;default: _LL194: _tmp1AD=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1A0)->f1;_tmp1AC=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1A0)->f2;_LL195:
# 1035
 Cyc_Port_region_counts(cts,_tmp1AD);
for(0;(unsigned int)_tmp1AC;_tmp1AC=_tmp1AC->tl){Cyc_Port_region_counts(cts,(void*)_tmp1AC->hd);}
return;}_LL173:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1046
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1048
c=Cyc_Port_compress_ct(c);{
struct _tuple10 _tmp1AE=({struct _tuple10 _tmp1CF;_tmp1CF.f1=t;_tmp1CF.f2=c;_tmp1CF;});struct _tuple10 _tmp1AF=_tmp1AE;void*_tmp1CE;struct Cyc_List_List*_tmp1CD;void*_tmp1CC;struct Cyc_List_List*_tmp1CB;void*_tmp1CA;void*_tmp1C9;void*_tmp1C8;void*_tmp1C7;struct Cyc_Absyn_PtrLoc*_tmp1C6;void*_tmp1C5;void*_tmp1C4;switch(*((int*)_tmp1AF.f1)){case 5: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1AF.f2)->tag == 11){_LL197: _tmp1C8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AF.f1)->f1).elt_typ;_tmp1C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AF.f1)->f1).ptr_atts).rgn;_tmp1C6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AF.f1)->f1).ptr_atts).ptrloc;_tmp1C5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1AF.f2)->f1;_tmp1C4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1AF.f2)->f4;_LL198:
# 1051
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1C8,_tmp1C5);{
unsigned int _tmp1B0=(unsigned int)_tmp1C6?_tmp1C6->rgn_loc:(unsigned int)0;
_tmp1C4=Cyc_Port_compress_ct(_tmp1C4);
{struct _tuple10 _tmp1B1=({struct _tuple10 _tmp1BF;_tmp1BF.f1=_tmp1C7;_tmp1BF.f2=_tmp1C4;_tmp1BF;});struct _tuple10 _tmp1B2=_tmp1B1;struct _dyneither_ptr*_tmp1BE;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B2.f1)->tag == 1)switch(*((int*)_tmp1B2.f2)){case 7: _LL1A0: if(!fn_res){_LL1A1:
# 1056
(env->gcenv)->edits=({struct Cyc_List_List*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=({struct Cyc_Port_Edit*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->loc=_tmp1B0;_tmp1B4->old_string=({const char*_tmp1B6="`H ";_tag_dyneither(_tmp1B6,sizeof(char),4);});_tmp1B4->new_string=({const char*_tmp1B5="";_tag_dyneither(_tmp1B5,sizeof(char),1);});_tmp1B4;});_tmp1B3->tl=(env->gcenv)->edits;_tmp1B3;});
# 1058
goto _LL19F;}else{goto _LL1A4;}case 10: _LL1A2: _tmp1BE=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1B2.f2)->f1;_LL1A3: {
# 1060
int _tmp1B7=*((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(counts,_tmp1BE);
if(_tmp1B7 > 1)
(env->gcenv)->edits=({struct Cyc_List_List*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->hd=({struct Cyc_Port_Edit*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->loc=_tmp1B0;_tmp1B9->old_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp1BE);({void*_tmp1BB[1]={& _tmp1BD};Cyc_aprintf(({const char*_tmp1BC="%s ";_tag_dyneither(_tmp1BC,sizeof(char),4);}),_tag_dyneither(_tmp1BB,sizeof(void*),1));});});_tmp1B9->new_string=({const char*_tmp1BA="";_tag_dyneither(_tmp1BA,sizeof(char),1);});_tmp1B9;});_tmp1B8->tl=(env->gcenv)->edits;_tmp1B8;});
# 1064
goto _LL19F;}default: goto _LL1A4;}else{_LL1A4: _LL1A5:
 goto _LL19F;}_LL19F:;}
# 1067
goto _LL196;};}else{goto _LL19D;}case 8: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1AF.f2)->tag == 12){_LL199: _tmp1CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AF.f1)->f1).elt_type;_tmp1C9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1AF.f2)->f1;_LL19A:
# 1069
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1CA,_tmp1C9);
goto _LL196;}else{goto _LL19D;}case 9: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1AF.f2)->tag == 15){_LL19B: _tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AF.f1)->f1).ret_typ;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AF.f1)->f1).args;_tmp1CC=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1AF.f2)->f1;_tmp1CB=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1AF.f2)->f2;_LL19C:
# 1072
 Cyc_Port_register_rgns(env,counts,1,_tmp1CE,_tmp1CC);
for(0;_tmp1CD != 0  && _tmp1CB != 0;(_tmp1CD=_tmp1CD->tl,_tmp1CB=_tmp1CB->tl)){
struct _tuple8 _tmp1C0=*((struct _tuple8*)_tmp1CD->hd);struct _tuple8 _tmp1C1=_tmp1C0;void*_tmp1C3;_LL1A7: _tmp1C3=_tmp1C1.f3;_LL1A8:;{
void*_tmp1C2=(void*)_tmp1CB->hd;
Cyc_Port_register_rgns(env,counts,0,_tmp1C3,_tmp1C2);};}
# 1078
goto _LL196;}else{goto _LL19D;}default: _LL19D: _LL19E:
 goto _LL196;}_LL196:;};}struct Cyc_Port_Cenv;
# 1085
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);struct Cyc_Port_Cenv;
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1090
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1D0=t;enum Cyc_Absyn_Size_of _tmp1D2;struct _tuple0*_tmp1D1;switch(*((int*)_tmp1D0)){case 17: _LL1AA: _tmp1D1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1D0)->f1;_LL1AB:
# 1093
(*_tmp1D1).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(env,_tmp1D1));case 6: _LL1AC: _tmp1D2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1D0)->f2;_LL1AD:
 return 1;default: _LL1AE: _LL1AF:
 return 0;}_LL1A9:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1101
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1D3=t;struct Cyc_List_List*_tmp20A;union Cyc_Absyn_AggrInfoU _tmp209;void*_tmp208;struct Cyc_List_List*_tmp207;void*_tmp206;struct Cyc_Absyn_Tqual _tmp205;union Cyc_Absyn_Constraint*_tmp204;unsigned int _tmp203;void*_tmp202;struct Cyc_Absyn_Tqual _tmp201;void*_tmp200;union Cyc_Absyn_Constraint*_tmp1FF;union Cyc_Absyn_Constraint*_tmp1FE;union Cyc_Absyn_Constraint*_tmp1FD;struct Cyc_Absyn_PtrLoc*_tmp1FC;struct _tuple0*_tmp1FB;switch(*((int*)_tmp1D3)){case 17: _LL1B1: _tmp1FB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1D3)->f1;_LL1B2:
# 1104
(*_tmp1FB).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,_tmp1FB);case 0: _LL1B3: _LL1B4:
 return Cyc_Port_void_ct();case 5: _LL1B5: _tmp202=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).elt_typ;_tmp201=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).elt_tq;_tmp200=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).rgn;_tmp1FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).nullable;_tmp1FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).bounds;_tmp1FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).zero_term;_tmp1FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).ptrloc;_LL1B6: {
# 1108
unsigned int _tmp1D4=(unsigned int)_tmp1FC?_tmp1FC->ptr_loc:(unsigned int)0;
unsigned int _tmp1D5=(unsigned int)_tmp1FC?_tmp1FC->rgn_loc:(unsigned int)0;
unsigned int _tmp1D6=(unsigned int)_tmp1FC?_tmp1FC->zt_loc:(unsigned int)0;
# 1114
void*_tmp1D7=Cyc_Port_type_to_ctype(env,_tmp202);
void*new_rgn;
# 1117
{void*_tmp1D8=_tmp200;struct _dyneither_ptr*_tmp1D9;switch(*((int*)_tmp1D8)){case 20: _LL1C8: _LL1C9:
# 1119
 new_rgn=Cyc_Port_heap_ct();goto _LL1C7;case 2: _LL1CA: _tmp1D9=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D8)->f1)->name;_LL1CB:
# 1121
 new_rgn=Cyc_Port_rgnvar_ct(_tmp1D9);goto _LL1C7;case 1: _LL1CC: _LL1CD:
# 1125
 if(Cyc_Port_in_fn_arg(env))
new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{
# 1128
if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))
new_rgn=Cyc_Port_heap_ct();else{
new_rgn=Cyc_Port_var();}}
goto _LL1C7;default: _LL1CE: _LL1CF:
# 1133
 new_rgn=Cyc_Port_heap_ct();goto _LL1C7;}_LL1C7:;}{
# 1135
int _tmp1DA=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FE)== (void*)& Cyc_Absyn_DynEither_b_val;
int orig_zt;
{union Cyc_Absyn_Constraint _tmp1DB=*_tmp1FD;union Cyc_Absyn_Constraint _tmp1DC=_tmp1DB;if((_tmp1DC.No_constr).tag == 3){_LL1D1: _LL1D2:
 orig_zt=Cyc_Port_is_char(env,_tmp202);goto _LL1D0;}else{_LL1D3: _LL1D4:
 orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FD);goto _LL1D0;}_LL1D0:;}
# 1141
if((env->gcenv)->porting){
void*_tmp1DD=Cyc_Port_var();
# 1146
Cyc_Port_register_const_cvar(env,_tmp1DD,_tmp201.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp201.loc);
# 1149
if(_tmp201.print_const)Cyc_Port_unify_ct(_tmp1DD,Cyc_Port_const_ct());{
# 1155
void*_tmp1DE=Cyc_Port_var();
# 1158
Cyc_Port_register_ptr_cvars(env,_tmp1DE,_tmp1DA?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),_tmp1D4);
# 1160
if(_tmp1DA)Cyc_Port_unify_ct(_tmp1DE,Cyc_Port_fat_ct());{
void*_tmp1DF=Cyc_Port_var();
# 1164
Cyc_Port_register_zeroterm_cvars(env,_tmp1DF,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp1D6);
# 1166
{union Cyc_Absyn_Constraint _tmp1E0=*_tmp1FD;union Cyc_Absyn_Constraint _tmp1E1=_tmp1E0;if((_tmp1E1.No_constr).tag == 3){_LL1D6: _LL1D7:
# 1168
 goto _LL1D5;}else{_LL1D8: _LL1D9:
# 1170
 Cyc_Port_unify_ct(_tmp1DF,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FD)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1D5;}_LL1D5:;}
# 1176
return Cyc_Port_ptr_ct(_tmp1D7,_tmp1DD,_tmp1DE,new_rgn,_tmp1DF);};};}else{
# 1180
return Cyc_Port_ptr_ct(_tmp1D7,_tmp201.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
_tmp1DA?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),new_rgn,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}};}case 6: _LL1B7: _LL1B8:
# 1184
 goto _LL1BA;case 7: _LL1B9: _LL1BA:
 return Cyc_Port_arith_ct();case 8: _LL1BB: _tmp206=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).elt_type;_tmp205=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).tq;_tmp204=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).zero_term;_tmp203=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).zt_loc;_LL1BC: {
# 1188
void*_tmp1E2=Cyc_Port_type_to_ctype(env,_tmp206);
int orig_zt;
{union Cyc_Absyn_Constraint _tmp1E3=*_tmp204;union Cyc_Absyn_Constraint _tmp1E4=_tmp1E3;if((_tmp1E4.No_constr).tag == 3){_LL1DB: _LL1DC:
 orig_zt=0;goto _LL1DA;}else{_LL1DD: _LL1DE:
 orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp204);goto _LL1DA;}_LL1DA:;}
# 1194
if((env->gcenv)->porting){
void*_tmp1E5=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,_tmp1E5,_tmp205.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp205.loc);{
# 1203
void*_tmp1E6=Cyc_Port_var();
Cyc_Port_register_zeroterm_cvars(env,_tmp1E6,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp203);
# 1206
{union Cyc_Absyn_Constraint _tmp1E7=*_tmp204;union Cyc_Absyn_Constraint _tmp1E8=_tmp1E7;if((_tmp1E8.No_constr).tag == 3){_LL1E0: _LL1E1:
# 1208
 goto _LL1DF;}else{_LL1E2: _LL1E3:
# 1210
 Cyc_Port_unify_ct(_tmp1E6,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp204)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1DF;}_LL1DF:;}
# 1213
return Cyc_Port_array_ct(_tmp1E2,_tmp1E5,_tmp1E6);};}else{
# 1215
return Cyc_Port_array_ct(_tmp1E2,_tmp205.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}case 9: _LL1BD: _tmp208=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D3)->f1).ret_typ;_tmp207=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D3)->f1).args;_LL1BE: {
# 1219
void*_tmp1E9=Cyc_Port_type_to_ctype(Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos),_tmp208);
struct Cyc_Port_Cenv*_tmp1EA=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
struct _tuple8 _tmp1EB=*((struct _tuple8*)_tmp207->hd);struct _tuple8 _tmp1EC=_tmp1EB;struct Cyc_Absyn_Tqual _tmp1EF;void*_tmp1EE;_LL1E5: _tmp1EF=_tmp1EC.f2;_tmp1EE=_tmp1EC.f3;_LL1E6:;
cargs=({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=Cyc_Port_type_to_ctype(_tmp1EA,_tmp1EE);_tmp1ED->tl=cargs;_tmp1ED;});}
# 1226
return Cyc_Port_fn_ct(_tmp1E9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));}case 11: _LL1BF: _tmp209=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D3)->f1).aggr_info;_LL1C0: {
# 1228
union Cyc_Absyn_AggrInfoU _tmp1F0=_tmp209;struct Cyc_Absyn_Aggrdecl**_tmp1FA;struct _tuple0*_tmp1F9;struct _tuple0*_tmp1F8;if((_tmp1F0.UnknownAggr).tag == 1){if(((_tmp1F0.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL1E8: _tmp1F8=((_tmp1F0.UnknownAggr).val).f2;_LL1E9:
# 1230
(*_tmp1F8).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp1F1=Cyc_Port_lookup_struct_decl(env,_tmp1F8);
return Cyc_Port_known_aggr_ct(_tmp1F1);};}else{_LL1EA: _tmp1F9=((_tmp1F0.UnknownAggr).val).f2;_LL1EB:
# 1234
(*_tmp1F9).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp1F2=Cyc_Port_lookup_union_decl(env,_tmp1F9);
return Cyc_Port_known_aggr_ct(_tmp1F2);};}}else{_LL1EC: _tmp1FA=(_tmp1F0.KnownAggr).val;_LL1ED: {
# 1238
struct Cyc_Absyn_Aggrdecl*_tmp1F3=*_tmp1FA;
enum Cyc_Absyn_AggrKind _tmp1F4=_tmp1F3->kind;enum Cyc_Absyn_AggrKind _tmp1F5=_tmp1F4;if(_tmp1F5 == Cyc_Absyn_StructA){_LL1EF: _LL1F0: {
# 1241
struct _tuple11*_tmp1F6=Cyc_Port_lookup_struct_decl(env,_tmp1F3->name);
return Cyc_Port_known_aggr_ct(_tmp1F6);}}else{_LL1F1: _LL1F2: {
# 1244
struct _tuple11*_tmp1F7=Cyc_Port_lookup_union_decl(env,_tmp1F3->name);
return Cyc_Port_known_aggr_ct(_tmp1F7);}}_LL1EE:;}}_LL1E7:;}case 13: _LL1C1: _LL1C2:
# 1248
 return Cyc_Port_arith_ct();case 14: _LL1C3: _tmp20A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1D3)->f1;_LL1C4:
# 1251
 for(0;(unsigned int)_tmp20A;_tmp20A=_tmp20A->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp20A->hd)->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp20A->hd)->name,Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}
# 1255
return Cyc_Port_arith_ct();default: _LL1C5: _LL1C6:
 return Cyc_Port_arith_ct();}_LL1B0:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1261
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp20B=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);
struct Cyc_List_List*_tmp20C=args->tl;
enum Cyc_Absyn_Primop _tmp20D=p;enum Cyc_Absyn_Primop _tmp212;switch(_tmp20D){case Cyc_Absyn_Plus: _LL1F4: _LL1F5: {
# 1269
void*_tmp20E=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp20C))->hd);
if(Cyc_Port_leq(_tmp20B,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){
Cyc_Port_leq(_tmp20E,Cyc_Port_arith_ct());
return _tmp20B;}else{
if(Cyc_Port_leq(_tmp20E,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){
Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
return _tmp20E;}else{
# 1277
Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
Cyc_Port_leq(_tmp20E,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus: _LL1F6: _LL1F7:
# 1286
 if(_tmp20C == 0){
# 1288
Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}else{
# 1291
void*_tmp20F=Cyc_Port_compress_ct((void*)_tmp20C->hd);
if(Cyc_Port_leq(_tmp20B,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){
if(Cyc_Port_leq(_tmp20F,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var())))
return Cyc_Port_arith_ct();else{
# 1296
Cyc_Port_leq(_tmp20F,Cyc_Port_arith_ct());
return _tmp20B;}}else{
# 1300
Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times: _LL1F8: _LL1F9:
# 1305
 goto _LL1FB;case Cyc_Absyn_Div: _LL1FA: _LL1FB: goto _LL1FD;case Cyc_Absyn_Mod: _LL1FC: _LL1FD: goto _LL1FF;case Cyc_Absyn_Eq: _LL1FE: _LL1FF: goto _LL201;case Cyc_Absyn_Neq: _LL200: _LL201: goto _LL203;case Cyc_Absyn_Lt: _LL202: _LL203: goto _LL205;case Cyc_Absyn_Gt: _LL204: _LL205: goto _LL207;case Cyc_Absyn_Lte: _LL206: _LL207:
 goto _LL209;case Cyc_Absyn_Gte: _LL208: _LL209: goto _LL20B;case Cyc_Absyn_Bitand: _LL20A: _LL20B: goto _LL20D;case Cyc_Absyn_Bitor: _LL20C: _LL20D: goto _LL20F;case Cyc_Absyn_Bitxor: _LL20E: _LL20F: goto _LL211;case Cyc_Absyn_Bitlshift: _LL210: _LL211: goto _LL213;case Cyc_Absyn_Bitlrshift: _LL212: _LL213:
 goto _LL215;case Cyc_Absyn_Bitarshift: _LL214: _LL215:
 Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
Cyc_Port_leq((void*)((struct Cyc_List_List*)_check_null(_tmp20C))->hd,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();case Cyc_Absyn_Not: _LL216: _LL217:
 goto _LL219;case Cyc_Absyn_Bitnot: _LL218: _LL219:
# 1313
 Cyc_Port_leq(_tmp20B,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();default: _LL21A: _tmp212=_tmp20D;_LL21B:
({void*_tmp210=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp211=".size primop";_tag_dyneither(_tmp211,sizeof(char),13);}),_tag_dyneither(_tmp210,sizeof(void*),0));});}_LL1F3:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1320
static struct _tuple10 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp213=e->r;void*_tmp214=_tmp213;struct Cyc_Absyn_Exp*_tmp236;struct _dyneither_ptr*_tmp235;struct Cyc_Absyn_Exp*_tmp234;struct _dyneither_ptr*_tmp233;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_Absyn_Exp*_tmp230;void*_tmp22F;switch(*((int*)_tmp214)){case 1: _LL21D: _tmp22F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp214)->f1;_LL21E: {
# 1323
struct _tuple0*_tmp215=Cyc_Absyn_binding2qvar(_tmp22F);
(*_tmp215).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_var(env,_tmp215);}case 22: _LL21F: _tmp231=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp214)->f1;_tmp230=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp214)->f2;_LL220: {
# 1327
void*_tmp216=Cyc_Port_var();
void*_tmp217=Cyc_Port_var();
void*_tmp218=Cyc_Port_gen_exp(env,_tmp231);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp230),Cyc_Port_arith_ct());{
void*_tmp219=Cyc_Port_ptr_ct(_tmp216,_tmp217,Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var());
Cyc_Port_leq(_tmp218,_tmp219);
return({struct _tuple10 _tmp21A;_tmp21A.f1=_tmp217;_tmp21A.f2=_tmp216;_tmp21A;});};}case 19: _LL221: _tmp232=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp214)->f1;_LL222: {
# 1335
void*_tmp21B=Cyc_Port_var();
void*_tmp21C=Cyc_Port_var();
void*_tmp21D=Cyc_Port_ptr_ct(_tmp21B,_tmp21C,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp232),_tmp21D);
return({struct _tuple10 _tmp21E;_tmp21E.f1=_tmp21C;_tmp21E.f2=_tmp21B;_tmp21E;});}case 20: _LL223: _tmp234=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp214)->f1;_tmp233=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp214)->f2;_LL224: {
# 1341
void*_tmp21F=Cyc_Port_var();
void*_tmp220=Cyc_Port_var();
void*_tmp221=Cyc_Port_gen_exp(env,_tmp234);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp234),Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp222[1];_tmp222[0]=({struct Cyc_Port_Cfield*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->qual=_tmp220;_tmp223->f=_tmp233;_tmp223->type=_tmp21F;_tmp223;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp222,sizeof(struct Cyc_Port_Cfield*),1));})));
return({struct _tuple10 _tmp224;_tmp224.f1=_tmp220;_tmp224.f2=_tmp21F;_tmp224;});}case 21: _LL225: _tmp236=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp214)->f1;_tmp235=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp214)->f2;_LL226: {
# 1347
void*_tmp225=Cyc_Port_var();
void*_tmp226=Cyc_Port_var();
void*_tmp227=Cyc_Port_gen_exp(env,_tmp236);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp236),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp228[1];_tmp228[0]=({struct Cyc_Port_Cfield*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->qual=_tmp226;_tmp229->f=_tmp235;_tmp229->type=_tmp225;_tmp229;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp228,sizeof(struct Cyc_Port_Cfield*),1));})),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));
return({struct _tuple10 _tmp22A;_tmp22A.f1=_tmp226;_tmp22A.f2=_tmp225;_tmp22A;});}default: _LL227: _LL228:
({void*_tmp22B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp22E;_tmp22E.tag=0;_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp22C[1]={& _tmp22E};Cyc_aprintf(({const char*_tmp22D="gen_lhs: %s";_tag_dyneither(_tmp22D,sizeof(char),12);}),_tag_dyneither(_tmp22C,sizeof(void*),1));});}),_tag_dyneither(_tmp22B,sizeof(void*),0));});}_LL21C:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1358
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp237=e->r;void*_tmp238=_tmp237;struct Cyc_Absyn_Stmt*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;void**_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct _dyneither_ptr*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct _dyneither_ptr*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;void*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_List_List*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;enum Cyc_Absyn_Incrementor _tmp28D;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Core_Opt*_tmp28B;struct Cyc_Absyn_Exp*_tmp28A;enum Cyc_Absyn_Primop _tmp289;struct Cyc_List_List*_tmp288;void*_tmp287;switch(*((int*)_tmp238)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp238)->f1).Wstring_c).tag){case 2: _LL22A: _LL22B:
 goto _LL22D;case 3: _LL22C: _LL22D:
 goto _LL22F;case 4: _LL22E: _LL22F:
 goto _LL231;case 6: _LL230: _LL231:
 goto _LL233;case 7: _LL23C: _LL23D:
# 1369
 return Cyc_Port_arith_ct();case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp238)->f1).Int_c).val).f2 == 0){_LL23E: _LL23F:
 return Cyc_Port_zero_ct();}else{_LL240: _LL241:
 return Cyc_Port_arith_ct();}case 8: _LL242: _LL243:
# 1373
 return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());case 9: _LL244: _LL245:
# 1375
 return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());default: _LL246: _LL247:
 return Cyc_Port_zero_ct();}case 16: _LL232: _LL233:
# 1364
 goto _LL235;case 17: _LL234: _LL235:
 goto _LL237;case 18: _LL236: _LL237:
 goto _LL239;case 31: _LL238: _LL239:
 goto _LL23B;case 32: _LL23A: _LL23B:
 goto _LL23D;case 1: _LL248: _tmp287=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_LL249: {
# 1378
struct _tuple0*_tmp239=Cyc_Absyn_binding2qvar(_tmp287);
(*_tmp239).f1=Cyc_Absyn_Loc_n;{
struct _tuple10 _tmp23A=Cyc_Port_lookup_var(env,_tmp239);struct _tuple10 _tmp23B=_tmp23A;void*_tmp241;_LL28D: _tmp241=_tmp23B.f2;_LL28E:;
if(Cyc_Port_isfn(env,_tmp239)){
_tmp241=Cyc_Port_instantiate(_tmp241);
return Cyc_Port_ptr_ct(_tmp241,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),Cyc_Port_nozterm_ct());}else{
# 1385
if(Cyc_Port_isarray(env,_tmp239)){
void*_tmp23C=Cyc_Port_var();
void*_tmp23D=Cyc_Port_var();
void*_tmp23E=Cyc_Port_var();
void*_tmp23F=Cyc_Port_array_ct(_tmp23C,_tmp23D,_tmp23E);
Cyc_Port_unifies(_tmp241,_tmp23F);{
void*_tmp240=Cyc_Port_ptr_ct(_tmp23C,_tmp23D,Cyc_Port_fat_ct(),Cyc_Port_var(),_tmp23E);
return _tmp240;};}else{
return _tmp241;}}};}case 2: _LL24A: _tmp289=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp288=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL24B:
 return Cyc_Port_gen_primop(env,_tmp289,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp288));case 3: _LL24C: _tmp28C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp28B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_tmp28A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp238)->f3;_LL24D: {
# 1396
struct _tuple10 _tmp242=Cyc_Port_gen_lhs(env,_tmp28C);struct _tuple10 _tmp243=_tmp242;void*_tmp249;void*_tmp248;_LL290: _tmp249=_tmp243.f1;_tmp248=_tmp243.f2;_LL291:;
Cyc_Port_unifies(_tmp249,Cyc_Port_notconst_ct());{
void*_tmp244=Cyc_Port_gen_exp(env,_tmp28A);
if((unsigned int)_tmp28B){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp247;_tmp247.hd=_tmp244;_tmp247.tl=0;_tmp247;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp246;_tmp246.hd=_tmp248;_tmp246.tl=& x2;_tmp246;});
void*_tmp245=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp28B->v,& x1);
Cyc_Port_leq(_tmp245,_tmp248);
return _tmp248;}else{
# 1406
Cyc_Port_leq(_tmp244,_tmp248);
return _tmp248;}};}case 4: _LL24E: _tmp28E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp28D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL24F: {
# 1410
struct _tuple10 _tmp24A=Cyc_Port_gen_lhs(env,_tmp28E);struct _tuple10 _tmp24B=_tmp24A;void*_tmp24D;void*_tmp24C;_LL293: _tmp24D=_tmp24B.f1;_tmp24C=_tmp24B.f2;_LL294:;
Cyc_Port_unifies(_tmp24D,Cyc_Port_notconst_ct());
# 1413
Cyc_Port_leq(_tmp24C,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));
Cyc_Port_leq(_tmp24C,Cyc_Port_arith_ct());
return _tmp24C;}case 5: _LL250: _tmp291=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp290=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_tmp28F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238)->f3;_LL251: {
# 1417
void*_tmp24E=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp291),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp290),_tmp24E);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp28F),_tmp24E);
return _tmp24E;}case 6: _LL252: _tmp293=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp292=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL253:
 _tmp295=_tmp293;_tmp294=_tmp292;goto _LL255;case 7: _LL254: _tmp295=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp294=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL255:
# 1424
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp295),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp294),Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();case 8: _LL256: _tmp297=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp296=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL257:
# 1428
 Cyc_Port_gen_exp(env,_tmp297);
return Cyc_Port_gen_exp(env,_tmp296);case 9: _LL258: _tmp299=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp298=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL259: {
# 1431
void*_tmp24F=Cyc_Port_var();
void*_tmp250=Cyc_Port_gen_exp(env,_tmp299);
struct Cyc_List_List*_tmp251=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp298);
struct Cyc_List_List*_tmp252=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp251);
Cyc_Port_unifies(_tmp250,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp24F,_tmp252),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_nozterm_ct()));
for(0;_tmp251 != 0;(_tmp251=_tmp251->tl,_tmp252=_tmp252->tl)){
Cyc_Port_leq((void*)_tmp251->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp252))->hd);}
# 1439
return _tmp24F;}case 10: _LL25A: _LL25B:
({void*_tmp253=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp254="throw";_tag_dyneither(_tmp254,sizeof(char),6);}),_tag_dyneither(_tmp253,sizeof(void*),0));});case 11: _LL25C: _tmp29A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_LL25D:
 return Cyc_Port_gen_exp(env,_tmp29A);case 12: _LL25E: _LL25F:
({void*_tmp255=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp256="instantiate";_tag_dyneither(_tmp256,sizeof(char),12);}),_tag_dyneither(_tmp255,sizeof(void*),0));});case 13: _LL260: _tmp29C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp29B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL261: {
# 1444
void*_tmp257=Cyc_Port_type_to_ctype(env,_tmp29C);
void*_tmp258=Cyc_Port_gen_exp(env,_tmp29B);
Cyc_Port_leq(_tmp257,_tmp258);
return _tmp258;}case 14: _LL262: _tmp29D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_LL263: {
# 1449
struct _tuple10 _tmp259=Cyc_Port_gen_lhs(env,_tmp29D);struct _tuple10 _tmp25A=_tmp259;void*_tmp25C;void*_tmp25B;_LL296: _tmp25C=_tmp25A.f1;_tmp25B=_tmp25A.f2;_LL297:;
return Cyc_Port_ptr_ct(_tmp25B,_tmp25C,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());}case 22: _LL264: _tmp29F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp29E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL265: {
# 1452
void*_tmp25D=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp29E),Cyc_Port_arith_ct());{
void*_tmp25E=Cyc_Port_gen_exp(env,_tmp29F);
Cyc_Port_leq(_tmp25E,Cyc_Port_ptr_ct(_tmp25D,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));
return _tmp25D;};}case 19: _LL266: _tmp2A0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_LL267: {
# 1458
void*_tmp25F=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A0),Cyc_Port_ptr_ct(_tmp25F,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));
return _tmp25F;}case 20: _LL268: _tmp2A2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp2A1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL269: {
# 1462
void*_tmp260=Cyc_Port_var();
void*_tmp261=Cyc_Port_gen_exp(env,_tmp2A2);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A2),Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp262[1];_tmp262[0]=({struct Cyc_Port_Cfield*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->qual=Cyc_Port_var();_tmp263->f=_tmp2A1;_tmp263->type=_tmp260;_tmp263;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp262,sizeof(struct Cyc_Port_Cfield*),1));})));
return _tmp260;}case 21: _LL26A: _tmp2A4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp2A3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL26B: {
# 1467
void*_tmp264=Cyc_Port_var();
void*_tmp265=Cyc_Port_gen_exp(env,_tmp2A4);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A4),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp266[1];_tmp266[0]=({struct Cyc_Port_Cfield*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->qual=Cyc_Port_var();_tmp267->f=_tmp2A3;_tmp267->type=_tmp264;_tmp267;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp266,sizeof(struct Cyc_Port_Cfield*),1));})),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));
return _tmp264;}case 33: _LL26C: _tmp2A6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp238)->f1).elt_type;_tmp2A5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp238)->f1).num_elts;_LL26D:
# 1473
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A5),Cyc_Port_arith_ct());{
void*_tmp268=(unsigned int)_tmp2A6?Cyc_Port_type_to_ctype(env,*_tmp2A6): Cyc_Port_var();
return Cyc_Port_ptr_ct(_tmp268,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_var());};case 34: _LL26E: _tmp2A8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp2A7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL26F:
({void*_tmp269=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26A="Swap_e";_tag_dyneither(_tmp26A,sizeof(char),7);}),_tag_dyneither(_tmp269,sizeof(void*),0));});case 15: _LL270: _LL271:
({void*_tmp26B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26C="New_e";_tag_dyneither(_tmp26C,sizeof(char),6);}),_tag_dyneither(_tmp26B,sizeof(void*),0));});case 23: _LL272: _LL273:
({void*_tmp26D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26E="Tuple_e";_tag_dyneither(_tmp26E,sizeof(char),8);}),_tag_dyneither(_tmp26D,sizeof(void*),0));});case 24: _LL274: _LL275:
({void*_tmp26F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp270="CompoundLit_e";_tag_dyneither(_tmp270,sizeof(char),14);}),_tag_dyneither(_tmp26F,sizeof(void*),0));});case 25: _LL276: _LL277:
({void*_tmp271=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp272="Array_e";_tag_dyneither(_tmp272,sizeof(char),8);}),_tag_dyneither(_tmp271,sizeof(void*),0));});case 26: _LL278: _LL279:
({void*_tmp273=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp274="Comprehension_e";_tag_dyneither(_tmp274,sizeof(char),16);}),_tag_dyneither(_tmp273,sizeof(void*),0));});case 27: _LL27A: _LL27B:
({void*_tmp275=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp276="ComprehensionNoinit_e";_tag_dyneither(_tmp276,sizeof(char),22);}),_tag_dyneither(_tmp275,sizeof(void*),0));});case 28: _LL27C: _LL27D:
({void*_tmp277=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp278="Aggregate_e";_tag_dyneither(_tmp278,sizeof(char),12);}),_tag_dyneither(_tmp277,sizeof(void*),0));});case 29: _LL27E: _LL27F:
({void*_tmp279=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp27A="AnonStruct_e";_tag_dyneither(_tmp27A,sizeof(char),13);}),_tag_dyneither(_tmp279,sizeof(void*),0));});case 30: _LL280: _LL281:
({void*_tmp27B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp27C="Datatype_e";_tag_dyneither(_tmp27C,sizeof(char),11);}),_tag_dyneither(_tmp27B,sizeof(void*),0));});case 35: _LL282: _LL283:
({void*_tmp27D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp27E="UnresolvedMem_e";_tag_dyneither(_tmp27E,sizeof(char),16);}),_tag_dyneither(_tmp27D,sizeof(void*),0));});case 36: _LL284: _tmp2A9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_LL285:
({void*_tmp27F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp280="StmtExp_e";_tag_dyneither(_tmp280,sizeof(char),10);}),_tag_dyneither(_tmp27F,sizeof(void*),0));});case 38: _LL286: _LL287:
({void*_tmp281=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp282="Valueof_e";_tag_dyneither(_tmp282,sizeof(char),10);}),_tag_dyneither(_tmp281,sizeof(void*),0));});case 39: _LL288: _LL289:
({void*_tmp283=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp284="Asm_e";_tag_dyneither(_tmp284,sizeof(char),6);}),_tag_dyneither(_tmp283,sizeof(void*),0));});default: _LL28A: _LL28B:
({void*_tmp285=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp286="Tagcheck_e";_tag_dyneither(_tmp286,sizeof(char),11);}),_tag_dyneither(_tmp285,sizeof(void*),0));});}_LL229:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1495
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp2AA=s->r;void*_tmp2AB=_tmp2AA;struct Cyc_Absyn_Stmt*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Stmt*_tmp2C5;struct Cyc_Absyn_Decl*_tmp2C4;struct Cyc_Absyn_Stmt*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_List_List*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Stmt*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Stmt*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Stmt*_tmp2B9;struct Cyc_Absyn_Stmt*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Absyn_Stmt*_tmp2B6;struct Cyc_Absyn_Stmt*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;switch(*((int*)_tmp2AB)){case 0: _LL299: _LL29A:
 return;case 1: _LL29B: _tmp2B4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_LL29C:
 Cyc_Port_gen_exp(env,_tmp2B4);return;case 2: _LL29D: _tmp2B6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_tmp2B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2;_LL29E:
 Cyc_Port_gen_stmt(env,_tmp2B6);Cyc_Port_gen_stmt(env,_tmp2B5);return;case 3: _LL29F: _tmp2B7=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_LL2A0: {
# 1501
void*_tmp2AC=Cyc_Port_lookup_return_type(env);
void*_tmp2AD=(unsigned int)_tmp2B7?Cyc_Port_gen_exp(env,_tmp2B7): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp2AD,_tmp2AC);
return;}case 4: _LL2A1: _tmp2BA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_tmp2B9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2;_tmp2B8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f3;_LL2A2:
# 1506
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2BA),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp2B9);
Cyc_Port_gen_stmt(env,_tmp2B8);
return;case 5: _LL2A3: _tmp2BC=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1).f1;_tmp2BB=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2;_LL2A4:
# 1511
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2BC),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp2BB);
return;case 6: _LL2A5: _LL2A6:
 goto _LL2A8;case 7: _LL2A7: _LL2A8:
 goto _LL2AA;case 8: _LL2A9: _LL2AA:
 return;case 9: _LL2AB: _tmp2C0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_tmp2BF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2).f1;_tmp2BE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f3).f1;_tmp2BD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f4;_LL2AC:
# 1518
 Cyc_Port_gen_exp(env,_tmp2C0);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2BF),Cyc_Port_arith_ct());
Cyc_Port_gen_exp(env,_tmp2BE);
Cyc_Port_gen_stmt(env,_tmp2BD);
return;case 10: _LL2AD: _tmp2C2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_tmp2C1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2;_LL2AE:
# 1524
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2C2),Cyc_Port_arith_ct());
for(0;(unsigned int)_tmp2C1;_tmp2C1=_tmp2C1->tl){
# 1527
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp2C1->hd)->body);}
# 1529
return;case 11: _LL2AF: _LL2B0:
({void*_tmp2AE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2AF="fallthru";_tag_dyneither(_tmp2AF,sizeof(char),9);}),_tag_dyneither(_tmp2AE,sizeof(void*),0));});case 12: _LL2B1: _tmp2C4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_tmp2C3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2;_LL2B2:
 env=Cyc_Port_gen_localdecl(env,_tmp2C4);Cyc_Port_gen_stmt(env,_tmp2C3);return;case 13: _LL2B3: _tmp2C5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2;_LL2B4:
 Cyc_Port_gen_stmt(env,_tmp2C5);return;case 14: _LL2B5: _tmp2C7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f1;_tmp2C6=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2AB)->f2).f1;_LL2B6:
# 1534
 Cyc_Port_gen_stmt(env,_tmp2C7);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2C6),Cyc_Port_arith_ct());
return;case 15: _LL2B7: _LL2B8:
({void*_tmp2B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B1="try/catch";_tag_dyneither(_tmp2B1,sizeof(char),10);}),_tag_dyneither(_tmp2B0,sizeof(void*),0));});default: _LL2B9: _LL2BA:
({void*_tmp2B2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B3="reset region";_tag_dyneither(_tmp2B3,sizeof(char),13);}),_tag_dyneither(_tmp2B2,sizeof(void*),0));});}_LL298:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1543
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp2C8=e->r;void*_tmp2C9=_tmp2C8;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2EE;switch(*((int*)_tmp2C9)){case 35: _LL2BC: _tmp2EE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2C9)->f2;_LL2BD:
 _tmp2EF=_tmp2EE;goto _LL2BF;case 25: _LL2BE: _tmp2EF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2C9)->f1;_LL2BF:
 _tmp2F0=_tmp2EF;goto _LL2C1;case 24: _LL2C0: _tmp2F0=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2C9)->f2;_LL2C1:
# 1548
 LOOP: {
void*_tmp2CA=t;struct Cyc_Absyn_Aggrdecl*_tmp2EB;struct _tuple0*_tmp2EA;void*_tmp2E9;union Cyc_Absyn_Constraint*_tmp2E8;unsigned int _tmp2E7;struct _tuple0*_tmp2E6;switch(*((int*)_tmp2CA)){case 17: _LL2C9: _tmp2E6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2CA)->f1;_LL2CA:
# 1551
(*_tmp2E6).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,_tmp2E6);goto LOOP;case 8: _LL2CB: _tmp2E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CA)->f1).elt_type;_tmp2E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CA)->f1).zero_term;_tmp2E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CA)->f1).zt_loc;_LL2CC: {
# 1554
void*_tmp2CB=Cyc_Port_var();
void*_tmp2CC=Cyc_Port_var();
void*_tmp2CD=Cyc_Port_var();
void*_tmp2CE=Cyc_Port_type_to_ctype(env,_tmp2E9);
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
struct _tuple15 _tmp2CF=*((struct _tuple15*)_tmp2F0->hd);struct _tuple15 _tmp2D0=_tmp2CF;struct Cyc_List_List*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D4;_LL2D4: _tmp2D5=_tmp2D0.f1;_tmp2D4=_tmp2D0.f2;_LL2D5:;
if((unsigned int)_tmp2D5)({void*_tmp2D1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2D2="designators in initializers";_tag_dyneither(_tmp2D2,sizeof(char),28);}),_tag_dyneither(_tmp2D1,sizeof(void*),0));});{
void*_tmp2D3=Cyc_Port_gen_initializer(env,_tmp2E9,_tmp2D4);
Cyc_Port_leq(_tmp2D3,_tmp2CB);};}
# 1564
return Cyc_Port_array_ct(_tmp2CB,_tmp2CC,_tmp2CD);}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CA)->f1).aggr_info).UnknownAggr).tag == 1){if(((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CA)->f1).aggr_info).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL2CD: _tmp2EA=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CA)->f1).aggr_info).UnknownAggr).val).f2;_LL2CE:
# 1566
(*_tmp2EA).f1=Cyc_Absyn_Loc_n;{
struct _tuple11 _tmp2D6=*Cyc_Port_lookup_struct_decl(env,_tmp2EA);struct _tuple11 _tmp2D7=_tmp2D6;struct Cyc_Absyn_Aggrdecl*_tmp2DA;_LL2D7: _tmp2DA=_tmp2D7.f1;_LL2D8:;
if(_tmp2DA == 0)({void*_tmp2D8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2D9="struct is not yet defined";_tag_dyneither(_tmp2D9,sizeof(char),26);}),_tag_dyneither(_tmp2D8,sizeof(void*),0));});
_tmp2EB=_tmp2DA;goto _LL2D0;};}else{goto _LL2D1;}}else{_LL2CF: _tmp2EB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CA)->f1).aggr_info).KnownAggr).val;_LL2D0: {
# 1571
struct Cyc_List_List*_tmp2DB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2EB->impl))->fields;
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
struct _tuple15 _tmp2DC=*((struct _tuple15*)_tmp2F0->hd);struct _tuple15 _tmp2DD=_tmp2DC;struct Cyc_List_List*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E2;_LL2DA: _tmp2E3=_tmp2DD.f1;_tmp2E2=_tmp2DD.f2;_LL2DB:;
if((unsigned int)_tmp2E3)({void*_tmp2DE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2DF="designators in initializers";_tag_dyneither(_tmp2DF,sizeof(char),28);}),_tag_dyneither(_tmp2DE,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrfield*_tmp2E0=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2DB))->hd;
_tmp2DB=_tmp2DB->tl;{
void*_tmp2E1=Cyc_Port_gen_initializer(env,_tmp2E0->type,_tmp2E2);;};};}
# 1579
return Cyc_Port_type_to_ctype(env,t);}}default: _LL2D1: _LL2D2:
({void*_tmp2E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2E5="bad type for aggregate initializer";_tag_dyneither(_tmp2E5,sizeof(char),35);}),_tag_dyneither(_tmp2E4,sizeof(void*),0));});}_LL2C8:;}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C9)->f1).Wstring_c).tag){case 8: _LL2C2: _LL2C3:
# 1582
 goto _LL2C5;case 9: _LL2C4: _LL2C5:
# 1584
 LOOP2: {
void*_tmp2EC=t;struct _tuple0*_tmp2ED;switch(*((int*)_tmp2EC)){case 17: _LL2DD: _tmp2ED=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2EC)->f1;_LL2DE:
# 1587
(*_tmp2ED).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,_tmp2ED);goto LOOP2;case 8: _LL2DF: _LL2E0:
 return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_zterm_ct());default: _LL2E1: _LL2E2:
 return Cyc_Port_gen_exp(env,e);}_LL2DC:;}default: goto _LL2C6;}default: _LL2C6: _LL2C7:
# 1592
 return Cyc_Port_gen_exp(env,e);}_LL2BB:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1597
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp2F1=d->r;void*_tmp2F2=_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2F6;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F2)->tag == 0){_LL2E4: _tmp2F6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F2)->f1;_LL2E5: {
# 1600
void*_tmp2F3=Cyc_Port_var();
void*q;
if((env->gcenv)->porting){
q=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,q,
(_tmp2F6->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp2F6->tq).loc);}else{
# 1613
q=(_tmp2F6->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1615
(*_tmp2F6->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,_tmp2F6->name,_tmp2F6->type,q,_tmp2F3);
Cyc_Port_unifies(_tmp2F3,Cyc_Port_type_to_ctype(env,_tmp2F6->type));
if((unsigned int)_tmp2F6->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp2F6->initializer);
void*t2=Cyc_Port_gen_initializer(env,_tmp2F6->type,e);
Cyc_Port_leq(t2,_tmp2F3);}
# 1623
return env;}}else{_LL2E6: _LL2E7:
({void*_tmp2F4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2F5="non-local decl that isn't a variable";_tag_dyneither(_tmp2F5,sizeof(char),37);}),_tag_dyneither(_tmp2F4,sizeof(void*),0));});}_LL2E3:;}
# 1628
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp2F7=*arg;struct _tuple8 _tmp2F8=_tmp2F7;struct _dyneither_ptr*_tmp2FC;struct Cyc_Absyn_Tqual _tmp2FB;void*_tmp2FA;_LL2E9: _tmp2FC=_tmp2F8.f1;_tmp2FB=_tmp2F8.f2;_tmp2FA=_tmp2F8.f3;_LL2EA:;
return({struct _tuple8*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->f1=0;_tmp2F9->f2=_tmp2FB;_tmp2F9->f3=_tmp2FA;_tmp2F9;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1633
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple16{struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1635
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp2FD=d->r;void*_tmp2FE=_tmp2FD;struct Cyc_Absyn_Enumdecl*_tmp33B;struct Cyc_Absyn_Aggrdecl*_tmp33A;struct Cyc_Absyn_Typedefdecl*_tmp339;struct Cyc_Absyn_Fndecl*_tmp338;struct Cyc_Absyn_Vardecl*_tmp337;switch(*((int*)_tmp2FE)){case 13: _LL2EC: _LL2ED:
# 1638
(env->gcenv)->porting=1;
return env;case 14: _LL2EE: _LL2EF:
# 1641
(env->gcenv)->porting=0;
return env;case 0: _LL2F0: _tmp337=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2FE)->f1;_LL2F1:
# 1644
(*_tmp337->name).f1=Cyc_Absyn_Loc_n;
# 1648
if(Cyc_Port_declared_var(env,_tmp337->name)){
struct _tuple10 _tmp2FF=Cyc_Port_lookup_var(env,_tmp337->name);struct _tuple10 _tmp300=_tmp2FF;void*_tmp302;void*_tmp301;_LL2FD: _tmp302=_tmp300.f1;_tmp301=_tmp300.f2;_LL2FE:;{
void*q2;
if((env->gcenv)->porting  && !Cyc_Port_isfn(env,_tmp337->name)){
q2=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,q2,
(_tmp337->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp337->tq).loc);}else{
# 1662
q2=(_tmp337->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1664
Cyc_Port_unifies(_tmp302,q2);
Cyc_Port_unifies(_tmp301,Cyc_Port_type_to_ctype(env,_tmp337->type));
if((unsigned int)_tmp337->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp337->initializer);
Cyc_Port_leq(Cyc_Port_gen_initializer(env,_tmp337->type,e),_tmp301);}
# 1670
return env;};}else{
# 1672
return Cyc_Port_gen_localdecl(env,d);}case 1: _LL2F2: _tmp338=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2FE)->f1;_LL2F3:
# 1678
(*_tmp338->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple13*predeclared=0;
# 1681
if(Cyc_Port_declared_var(env,_tmp338->name))
predeclared=Cyc_Port_lookup_full_var(env,_tmp338->name);{
# 1684
void*_tmp303=_tmp338->ret_type;
struct Cyc_List_List*_tmp304=_tmp338->args;
struct Cyc_List_List*_tmp305=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp304);
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp306=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp322;_tmp322.tag=9;_tmp322.f1=({struct Cyc_Absyn_FnInfo _tmp323;_tmp323.tvars=0;_tmp323.effect=0;_tmp323.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp323.ret_typ=_tmp303;_tmp323.args=_tmp305;_tmp323.c_varargs=0;_tmp323.cyc_varargs=0;_tmp323.rgn_po=0;_tmp323.attributes=0;_tmp323.requires_clause=0;_tmp323.requires_relns=0;_tmp323.ensures_clause=0;_tmp323.ensures_relns=0;_tmp323;});_tmp322;});_tmp321;});
# 1691
struct Cyc_Port_Cenv*_tmp307=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp308=Cyc_Port_type_to_ctype(_tmp307,_tmp303);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp309=_tmp304;for(0;(unsigned int)_tmp309;_tmp309=_tmp309->tl){
struct _tuple8 _tmp30A=*((struct _tuple8*)_tmp309->hd);struct _tuple8 _tmp30B=_tmp30A;struct _dyneither_ptr*_tmp312;struct Cyc_Absyn_Tqual _tmp311;void*_tmp310;_LL300: _tmp312=_tmp30B.f1;_tmp311=_tmp30B.f2;_tmp310=_tmp30B.f3;_LL301:;{
# 1699
void*_tmp30C=Cyc_Port_type_to_ctype(_tmp307,_tmp310);
void*tqv;
if((env->gcenv)->porting){
tqv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,tqv,_tmp311.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp311.loc);}else{
# 1711
tqv=_tmp311.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1713
c_args=({struct Cyc_List_List*_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->hd=({struct _tuple16*_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->f1=_tmp312;_tmp30E->f2=_tmp310;_tmp30E->f3=tqv;_tmp30E->f4=_tmp30C;_tmp30E;});_tmp30D->tl=c_args;_tmp30D;});
c_arg_types=({struct Cyc_List_List*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->hd=_tmp30C;_tmp30F->tl=c_arg_types;_tmp30F;});};}}
# 1716
c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);
c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);{
void*_tmp313=Cyc_Port_fn_ct(_tmp308,c_arg_types);
# 1722
(*_tmp338->name).f1=Cyc_Absyn_Loc_n;
_tmp307=Cyc_Port_add_var(_tmp307,_tmp338->name,(void*)_tmp306,Cyc_Port_const_ct(),_tmp313);
Cyc_Port_add_return_type(_tmp307,_tmp308);
{struct Cyc_List_List*_tmp314=c_args;for(0;(unsigned int)_tmp314;_tmp314=_tmp314->tl){
struct _tuple16 _tmp315=*((struct _tuple16*)_tmp314->hd);struct _tuple16 _tmp316=_tmp315;struct _dyneither_ptr*_tmp31B;void*_tmp31A;void*_tmp319;void*_tmp318;_LL303: _tmp31B=_tmp316.f1;_tmp31A=_tmp316.f2;_tmp319=_tmp316.f3;_tmp318=_tmp316.f4;_LL304:;
_tmp307=Cyc_Port_add_var(_tmp307,({struct _tuple0*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->f1=Cyc_Absyn_Loc_n;_tmp317->f2=_tmp31B;_tmp317;}),_tmp31A,_tmp319,_tmp318);}}
# 1729
Cyc_Port_gen_stmt(_tmp307,_tmp338->body);
# 1734
Cyc_Port_generalize(0,_tmp313);{
# 1741
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp313);
# 1745
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp306,_tmp313);
env=Cyc_Port_add_var(_tmp307,_tmp338->name,(void*)_tmp306,Cyc_Port_const_ct(),_tmp313);
if((unsigned int)predeclared){
# 1754
struct _tuple13 _tmp31C=*predeclared;struct _tuple13 _tmp31D=_tmp31C;void*_tmp320;void*_tmp31F;void*_tmp31E;_LL306: _tmp320=_tmp31D.f1;_tmp31F=(_tmp31D.f2)->f1;_tmp31E=(_tmp31D.f2)->f2;_LL307:;
Cyc_Port_unifies(_tmp31F,Cyc_Port_const_ct());
Cyc_Port_unifies(_tmp31E,Cyc_Port_instantiate(_tmp313));
# 1758
Cyc_Port_register_rgns(env,counts,1,_tmp320,_tmp313);}
# 1760
return env;};};};};case 8: _LL2F4: _tmp339=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2FE)->f1;_LL2F5: {
# 1766
void*_tmp324=(void*)_check_null(_tmp339->defn);
void*_tmp325=Cyc_Port_type_to_ctype(env,_tmp324);
(*_tmp339->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,_tmp339->name,_tmp324,_tmp325);
return env;}case 5: _LL2F6: _tmp33A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2FE)->f1;_LL2F7: {
# 1776
struct _tuple0*_tmp326=_tmp33A->name;
(*_tmp33A->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp327=_tmp33A->kind;enum Cyc_Absyn_AggrKind _tmp328=_tmp327;if(_tmp328 == Cyc_Absyn_StructA){_LL309: _LL30A:
# 1782
 previous=Cyc_Port_lookup_struct_decl(env,_tmp326);
goto _LL308;}else{_LL30B: _LL30C:
# 1785
 previous=Cyc_Port_lookup_union_decl(env,_tmp326);
goto _LL308;}_LL308:;}
# 1788
if((unsigned int)_tmp33A->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp329=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33A->impl))->fields;for(0;(unsigned int)_tmp329;_tmp329=_tmp329->tl){
struct Cyc_Absyn_Aggrfield*_tmp32A=(struct Cyc_Absyn_Aggrfield*)_tmp329->hd;
void*qv;
if((env->gcenv)->porting){
qv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,qv,
(_tmp32A->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp32A->tq).loc);}else{
# 1804
qv=(_tmp32A->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1806
void*_tmp32B=Cyc_Port_type_to_ctype(env,_tmp32A->type);
if(cf != 0){
struct Cyc_Port_Cfield _tmp32C=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _tmp32D=_tmp32C;void*_tmp32F;void*_tmp32E;_LL30E: _tmp32F=_tmp32D.qual;_tmp32E=_tmp32D.type;_LL30F:;
cf=cf->tl;
Cyc_Port_unifies(qv,_tmp32F);
Cyc_Port_unifies(_tmp32B,_tmp32E);}
# 1813
curr=({struct Cyc_List_List*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->hd=({struct Cyc_Port_Cfield*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->qual=qv;_tmp331->f=_tmp32A->name;_tmp331->type=_tmp32B;_tmp331;});_tmp330->tl=curr;_tmp330;});};}}
# 1815
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1820
return env;};}case 7: _LL2F8: _tmp33B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2FE)->f1;_LL2F9:
# 1825
(*_tmp33B->name).f1=Cyc_Absyn_Loc_n;
# 1827
if((unsigned int)_tmp33B->fields){
struct Cyc_List_List*_tmp332=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33B->fields))->v;for(0;(unsigned int)_tmp332;_tmp332=_tmp332->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp332->hd)->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp332->hd)->name,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp334;_tmp334.tag=13;_tmp334.f1=_tmp33B->name;_tmp334.f2=_tmp33B;_tmp334;});_tmp333;}),
Cyc_Port_const_ct(),Cyc_Port_arith_ct());}}
# 1833
return env;default: _LL2FA: _LL2FB:
# 1835
 if((env->gcenv)->porting)
({void*_tmp335=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp336="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_dyneither(_tmp336,sizeof(char),69);}),_tag_dyneither(_tmp335,sizeof(void*),0));});
return env;}_LL2EB:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1842
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned int)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
return env;}struct Cyc_Port_Cenv;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1849
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1851
struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),ds);
# 1853
struct Cyc_List_List*_tmp33C=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp33D=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp33E=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp33F=(env->gcenv)->edits;
# 1858
{struct Cyc_List_List*_tmp340=_tmp33C;for(0;(unsigned int)_tmp340;_tmp340=_tmp340->tl){
struct _tuple14 _tmp341=*((struct _tuple14*)_tmp340->hd);struct _tuple14 _tmp342=_tmp341;void*_tmp345;void*_tmp344;unsigned int _tmp343;_LL311: _tmp345=_tmp342.f1;_tmp344=_tmp342.f2;_tmp343=_tmp342.f3;_LL312:;
Cyc_Port_unifies(_tmp345,_tmp344);}}
# 1862
{struct Cyc_List_List*_tmp346=_tmp33D;for(0;(unsigned int)_tmp346;_tmp346=_tmp346->tl){
struct _tuple14 _tmp347=*((struct _tuple14*)_tmp346->hd);struct _tuple14 _tmp348=_tmp347;void*_tmp34B;void*_tmp34A;unsigned int _tmp349;_LL314: _tmp34B=_tmp348.f1;_tmp34A=_tmp348.f2;_tmp349=_tmp348.f3;_LL315:;
Cyc_Port_unifies(_tmp34B,_tmp34A);}}
# 1866
{struct Cyc_List_List*_tmp34C=_tmp33E;for(0;(unsigned int)_tmp34C;_tmp34C=_tmp34C->tl){
struct _tuple14 _tmp34D=*((struct _tuple14*)_tmp34C->hd);struct _tuple14 _tmp34E=_tmp34D;void*_tmp351;void*_tmp350;unsigned int _tmp34F;_LL317: _tmp351=_tmp34E.f1;_tmp350=_tmp34E.f2;_tmp34F=_tmp34E.f3;_LL318:;
Cyc_Port_unifies(_tmp351,_tmp350);}}
# 1872
for(0;(unsigned int)_tmp33C;_tmp33C=_tmp33C->tl){
struct _tuple14 _tmp352=*((struct _tuple14*)_tmp33C->hd);struct _tuple14 _tmp353=_tmp352;void*_tmp360;void*_tmp35F;unsigned int _tmp35E;_LL31A: _tmp360=_tmp353.f1;_tmp35F=_tmp353.f2;_tmp35E=_tmp353.f3;_LL31B:;
if(!Cyc_Port_unifies(_tmp360,_tmp35F) && (int)_tmp35E){
void*_tmp354=Cyc_Port_compress_ct(_tmp35F);void*_tmp355=_tmp354;switch(*((int*)_tmp355)){case 2: _LL31D: _LL31E:
# 1877
 _tmp33F=({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->hd=({struct Cyc_Port_Edit*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->loc=_tmp35E;_tmp357->old_string=({const char*_tmp359="?";_tag_dyneither(_tmp359,sizeof(char),2);});_tmp357->new_string=({const char*_tmp358="*";_tag_dyneither(_tmp358,sizeof(char),2);});_tmp357;});_tmp356->tl=_tmp33F;_tmp356;});
goto _LL31C;case 3: _LL31F: _LL320:
# 1880
 _tmp33F=({struct Cyc_List_List*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->hd=({struct Cyc_Port_Edit*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->loc=_tmp35E;_tmp35B->old_string=({const char*_tmp35D="*";_tag_dyneither(_tmp35D,sizeof(char),2);});_tmp35B->new_string=({const char*_tmp35C="?";_tag_dyneither(_tmp35C,sizeof(char),2);});_tmp35B;});_tmp35A->tl=_tmp33F;_tmp35A;});
goto _LL31C;default: _LL321: _LL322:
 goto _LL31C;}_LL31C:;}}
# 1887
for(0;(unsigned int)_tmp33D;_tmp33D=_tmp33D->tl){
struct _tuple14 _tmp361=*((struct _tuple14*)_tmp33D->hd);struct _tuple14 _tmp362=_tmp361;void*_tmp36F;void*_tmp36E;unsigned int _tmp36D;_LL324: _tmp36F=_tmp362.f1;_tmp36E=_tmp362.f2;_tmp36D=_tmp362.f3;_LL325:;
if(!Cyc_Port_unifies(_tmp36F,_tmp36E) && (int)_tmp36D){
void*_tmp363=Cyc_Port_compress_ct(_tmp36E);void*_tmp364=_tmp363;switch(*((int*)_tmp364)){case 1: _LL327: _LL328:
# 1892
 _tmp33F=({struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365->hd=({struct Cyc_Port_Edit*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->loc=_tmp36D;_tmp366->old_string=({const char*_tmp368="const ";_tag_dyneither(_tmp368,sizeof(char),7);});_tmp366->new_string=({const char*_tmp367="";_tag_dyneither(_tmp367,sizeof(char),1);});_tmp366;});_tmp365->tl=_tmp33F;_tmp365;});goto _LL326;case 0: _LL329: _LL32A:
# 1894
 _tmp33F=({struct Cyc_List_List*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->hd=({struct Cyc_Port_Edit*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->loc=_tmp36D;_tmp36A->old_string=({const char*_tmp36C="";_tag_dyneither(_tmp36C,sizeof(char),1);});_tmp36A->new_string=({const char*_tmp36B="const ";_tag_dyneither(_tmp36B,sizeof(char),7);});_tmp36A;});_tmp369->tl=_tmp33F;_tmp369;});goto _LL326;default: _LL32B: _LL32C:
 goto _LL326;}_LL326:;}}
# 1900
for(0;(unsigned int)_tmp33E;_tmp33E=_tmp33E->tl){
struct _tuple14 _tmp370=*((struct _tuple14*)_tmp33E->hd);struct _tuple14 _tmp371=_tmp370;void*_tmp37E;void*_tmp37D;unsigned int _tmp37C;_LL32E: _tmp37E=_tmp371.f1;_tmp37D=_tmp371.f2;_tmp37C=_tmp371.f3;_LL32F:;
if(!Cyc_Port_unifies(_tmp37E,_tmp37D) && (int)_tmp37C){
void*_tmp372=Cyc_Port_compress_ct(_tmp37D);void*_tmp373=_tmp372;switch(*((int*)_tmp373)){case 8: _LL331: _LL332:
# 1905
 _tmp33F=({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->hd=({struct Cyc_Port_Edit*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->loc=_tmp37C;_tmp375->old_string=({const char*_tmp377="@nozeroterm ";_tag_dyneither(_tmp377,sizeof(char),13);});_tmp375->new_string=({const char*_tmp376="";_tag_dyneither(_tmp376,sizeof(char),1);});_tmp375;});_tmp374->tl=_tmp33F;_tmp374;});goto _LL330;case 9: _LL333: _LL334:
# 1907
 _tmp33F=({struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->hd=({struct Cyc_Port_Edit*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->loc=_tmp37C;_tmp379->old_string=({const char*_tmp37B="@zeroterm ";_tag_dyneither(_tmp37B,sizeof(char),11);});_tmp379->new_string=({const char*_tmp37A="";_tag_dyneither(_tmp37A,sizeof(char),1);});_tmp379;});_tmp378->tl=_tmp33F;_tmp378;});goto _LL330;default: _LL335: _LL336:
 goto _LL330;}_LL330:;}}
# 1914
_tmp33F=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp33F);
# 1916
while((unsigned int)_tmp33F  && ((struct Cyc_Port_Edit*)_tmp33F->hd)->loc == 0){
# 1920
_tmp33F=_tmp33F->tl;}
# 1922
return _tmp33F;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1925
static unsigned int Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1931
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp37F=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*_tmp380=((struct Cyc_List_List*(*)(unsigned int(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp37F);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp381=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp380));
for(0;(unsigned int)_tmp37F;(_tmp37F=_tmp37F->tl,_tmp381=_tmp381->tl)){
struct Cyc_Port_Edit _tmp382=*((struct Cyc_Port_Edit*)_tmp37F->hd);struct Cyc_Port_Edit _tmp383=_tmp382;unsigned int _tmp38B;struct _dyneither_ptr _tmp38A;struct _dyneither_ptr _tmp389;_LL338: _tmp38B=_tmp383.loc;_tmp38A=_tmp383.old_string;_tmp389=_tmp383.new_string;_LL339:;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp381))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0;_tmp388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp389);({struct Cyc_String_pa_PrintArg_struct _tmp387;_tmp387.tag=0;_tmp387.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38A);({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0;_tmp386.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sloc);({void*_tmp384[3]={& _tmp386,& _tmp387,& _tmp388};Cyc_printf(({const char*_tmp385="%s: insert `%s' for `%s'\n";_tag_dyneither(_tmp385,sizeof(char),26);}),_tag_dyneither(_tmp384,sizeof(void*),3));});});});});};}};}

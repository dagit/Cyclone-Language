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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100 "cycboot.h"
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 119
void*Cyc_Core_identity(void*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 54 "string.h"
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 48 "fn.h"
struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);
# 51
struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));
# 54
void*Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _dyneither_ptr f1;struct Cyc_List_List*f2;};
# 56
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 74
struct Cyc_PP_Doc*Cyc_PP_oline_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 88
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 97
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
# 100
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs);
# 103
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);
# 105
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 126
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 51 "pp.cyc"
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0U,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0U,0};struct _tuple1{void*f1;void*f2;};
# 54
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple1 _tmp2=({struct _tuple1 _tmp6;_tmp6.f1=a1;_tmp6.f2=a2;_tmp6;});struct _tuple1 _tmp3=_tmp2;if(((struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f1)->tag == 0U){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f2)->tag == 0U){_LL3: _LL4:
 return a1;}else{_LL5: _LL6:
 return(void*)({struct Cyc_PP_Append_PP_Alist_struct*_tmp4=_cycalloc(sizeof(*_tmp4));({struct Cyc_PP_Append_PP_Alist_struct _tmpA2=({struct Cyc_PP_Append_PP_Alist_struct _tmp5;_tmp5.tag=2U;_tmp5.f1=a1;_tmp5.f2=a2;_tmp5;});_tmp4[0]=_tmpA2;});_tmp4;});}}_LL0:;}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_tmp7=y;void*_tmpB;void*_tmpA;void*_tmp9;switch(*((int*)_tmp7)){case 0U: _LL1: _LL2:
 return l;case 1U: _LL3: _tmp9=(void*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmp7)->f1;_LL4:
 return({struct Cyc_List_List*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=_tmp9;_tmp8->tl=l;_tmp8;});default: _LL5: _tmpB=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp7)->f1;_tmpA=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp7)->f2;_LL6:
 return({void*_tmpA3=_tmpB;Cyc_PP_list_of_alist_f(_tmpA3,Cyc_PP_list_of_alist_f(_tmpA,l));});}_LL0:;}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*_tmpC=((struct Cyc_Xarray_Xarray*(*)(int,void*))Cyc_Xarray_create)(16,al);
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmpC,al);{
int last=0;
while(last >= 0){
void*_tmpD=((void*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmpC,last);void*_tmpE=_tmpD;void*_tmp14;void*_tmp13;struct _dyneither_ptr*_tmp12;switch(*((int*)_tmpE)){case 0U: _LL1: _LL2:
 -- last;goto _LL0;case 1U: _LL3: _tmp12=(struct _dyneither_ptr*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmpE)->f1;_LL4:
 -- last;({struct Cyc_String_pa_PrintArg_struct _tmp11;_tmp11.tag=0U;_tmp11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp12);({void*_tmpF[1U]={& _tmp11};({struct Cyc___cycFILE*_tmpA5=f;struct _dyneither_ptr _tmpA4=({const char*_tmp10="%s";_tag_dyneither(_tmp10,sizeof(char),3U);});Cyc_fprintf(_tmpA5,_tmpA4,_tag_dyneither(_tmpF,sizeof(void*),1U));});});});goto _LL0;default: _LL5: _tmp14=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmpE)->f1;_tmp13=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmpE)->f2;_LL6:
# 107
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmpC,last,_tmp13);
if(({int _tmpA6=last;_tmpA6 == ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmpC)- 1;}))
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmpC,_tmp14);else{
# 111
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmpC,last + 1,_tmp14);}
++ last;
goto _LL0;}_LL0:;}
# 116
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpC);};}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_tmpA7=d->f;((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA7,({struct Cyc_PP_Ppstate*_tmp15=_cycalloc_atomic(sizeof(*_tmp15));_tmp15->ci=0;_tmp15->cc=0;_tmp15->cl=1;_tmp15->pw=w;_tmp15->epw=w;_tmp15;}));});
Cyc_PP_dump_out(f,o->ppout);}
# 126
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_tmpA8=d->f;((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA8,({struct Cyc_PP_Ppstate*_tmp17=_cycalloc_atomic(sizeof(*_tmp17));_tmp17->ci=0;_tmp17->cc=0;_tmp17->cl=1;_tmp17->pw=w;_tmp17->epw=w;_tmp17;}));});
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout);Cyc_str_sepstr(_tmpA9,({const char*_tmp16="";_tag_dyneither(_tmp16,sizeof(char),1U);}));});}
# 133
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_tmpAA=d->f;((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpAA,({struct Cyc_PP_Ppstate*_tmp1A=_cycalloc_atomic(sizeof(*_tmp1A));_tmp1A->ci=0;_tmp1A->cc=0;_tmp1A->cl=1;_tmp1A->pw=w;_tmp1A->epw=w;_tmp1A;}));});
return({struct _tuple0*_tmp18=_cycalloc(sizeof(*_tmp18));({struct _dyneither_ptr _tmpAC=(struct _dyneither_ptr)({
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout);Cyc_str_sepstr(_tmpAB,({const char*_tmp19="";_tag_dyneither(_tmp19,sizeof(char),1U);}));});_tmp18->f1=_tmpAC;});({struct Cyc_List_List*_tmpAD=
((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->links);_tmp18->f2=_tmpAD;});_tmp18;});}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _dyneither_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl == 0)
({struct Cyc_Core_Opt*_tmpAF=({struct Cyc_Core_Opt*_tmp1B=_cycalloc(sizeof(*_tmp1B));({struct Cyc_Hashtable_Table*_tmpAE=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(int))Cyc_Core_identity);_tmp1B->v=_tmpAE;});_tmp1B;});Cyc_PP_bhashtbl=_tmpAF;});
if(i < 0)(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));({struct Cyc_Core_Failure_exn_struct _tmpB1=({struct Cyc_Core_Failure_exn_struct _tmp1D;_tmp1D.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmpB0=({const char*_tmp1E="nlblanks";_tag_dyneither(_tmp1E,sizeof(char),9U);});_tmp1D.f1=_tmpB0;});_tmp1D;});_tmp1C[0]=_tmpB1;});_tmp1C;}));{
struct _handler_cons _tmp1F;_push_handler(& _tmp1F);{int _tmp21=0;if(setjmp(_tmp1F.handler))_tmp21=1;if(!_tmp21){
{struct _dyneither_ptr _tmp22=*((struct _dyneither_ptr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0U);return _tmp22;};_pop_handler();}else{void*_tmp20=(void*)_exn_thrown;void*_tmp23=_tmp20;void*_tmp3F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp23)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 152
 if(!Cyc_PP_tex_output){
int _tmp24=i / 8;
int _tmp25=i % 8;
int _tmp26=(2 + _tmp24)+ _tmp25;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)_tmp26);
({struct _dyneither_ptr _tmp27=_dyneither_ptr_plus(nlb,sizeof(char),0);char _tmp28=*((char*)_check_dyneither_subscript(_tmp27,sizeof(char),0U));char _tmp29='\n';if(_get_dyneither_size(_tmp27,sizeof(char))== 1U  && (_tmp28 == '\000'  && _tmp29 != '\000'))_throw_arraybounds();*((char*)_tmp27.curr)=_tmp29;});
{int j=0;for(0;j < _tmp24;++ j){
({struct _dyneither_ptr _tmp2A=_dyneither_ptr_plus(nlb,sizeof(char),j + 1);char _tmp2B=*((char*)_check_dyneither_subscript(_tmp2A,sizeof(char),0U));char _tmp2C='\t';if(_get_dyneither_size(_tmp2A,sizeof(char))== 1U  && (_tmp2B == '\000'  && _tmp2C != '\000'))_throw_arraybounds();*((char*)_tmp2A.curr)=_tmp2C;});}}
{int j=0;for(0;j < _tmp25;++ j){
({struct _dyneither_ptr _tmp2D=_dyneither_ptr_plus(nlb,sizeof(char),(j + 1)+ _tmp24);char _tmp2E=*((char*)_check_dyneither_subscript(_tmp2D,sizeof(char),0U));char _tmp2F=' ';if(_get_dyneither_size(_tmp2D,sizeof(char))== 1U  && (_tmp2E == '\000'  && _tmp2F != '\000'))_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});}}
({struct Cyc_Hashtable_Table*_tmpB3=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v;int _tmpB2=i;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)(_tmpB3,_tmpB2,({struct _dyneither_ptr*_tmp30=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp30[0]=(struct _dyneither_ptr)nlb;_tmp30;}));});
return(struct _dyneither_ptr)nlb;}else{
# 168
int _tmp31=3 + i;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)(_tmp31 + 1));
({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(nlb,sizeof(char),0);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34='\\';if(_get_dyneither_size(_tmp32,sizeof(char))== 1U  && (_tmp33 == '\000'  && _tmp34 != '\000'))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});
({struct _dyneither_ptr _tmp35=_dyneither_ptr_plus(nlb,sizeof(char),1);char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0U));char _tmp37='\\';if(_get_dyneither_size(_tmp35,sizeof(char))== 1U  && (_tmp36 == '\000'  && _tmp37 != '\000'))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});
({struct _dyneither_ptr _tmp38=_dyneither_ptr_plus(nlb,sizeof(char),2);char _tmp39=*((char*)_check_dyneither_subscript(_tmp38,sizeof(char),0U));char _tmp3A='\n';if(_get_dyneither_size(_tmp38,sizeof(char))== 1U  && (_tmp39 == '\000'  && _tmp3A != '\000'))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});
{int j=3;for(0;j < _tmp31;++ j){
({struct _dyneither_ptr _tmp3B=_dyneither_ptr_plus(nlb,sizeof(char),j);char _tmp3C=*((char*)_check_dyneither_subscript(_tmp3B,sizeof(char),0U));char _tmp3D='~';if(_get_dyneither_size(_tmp3B,sizeof(char))== 1U  && (_tmp3C == '\000'  && _tmp3D != '\000'))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});}}
({struct Cyc_Hashtable_Table*_tmpB5=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v;int _tmpB4=i;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)(_tmpB5,_tmpB4,({struct _dyneither_ptr*_tmp3E=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp3E[0]=(struct _dyneither_ptr)nlb;_tmp3E;}));});
return(struct _dyneither_ptr)nlb;}}else{_LL3: _tmp3F=_tmp23;_LL4:(int)_rethrow(_tmp3F);}_LL0:;}};};}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f1;struct _dyneither_ptr f2;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
struct _tuple2*_tmp40=clo;int _tmp46;struct _dyneither_ptr _tmp45;_LL1: _tmp46=_tmp40->f1;_tmp45=_tmp40->f2;_LL2:;
return({struct Cyc_PP_Out*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->newcc=st->cc + _tmp46;_tmp41->newcl=st->cl;({void*_tmpB8=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp42=_cycalloc(sizeof(*_tmp42));({struct Cyc_PP_Single_PP_Alist_struct _tmpB7=({struct Cyc_PP_Single_PP_Alist_struct _tmp43;_tmp43.tag=1U;({struct _dyneither_ptr*_tmpB6=({struct _dyneither_ptr*_tmp44=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp44[0]=(struct _dyneither_ptr)_tmp45;_tmp44;});_tmp43.f1=_tmpB6;});_tmp43;});_tmp42[0]=_tmpB7;});_tmp42;});_tmp41->ppout=_tmpB8;});_tmp41->links=(void*)& Cyc_PP_Empty_link;_tmp41;});}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)s);
return({struct Cyc_PP_Doc*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->mwo=slen;_tmp47->mw=Cyc_PP_infinity;({struct Cyc_Fn_Function*_tmpB9=
# 198
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({struct _tuple2*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->f1=slen;_tmp48->f2=s;_tmp48;}));_tmp47->f=_tmpB9;});_tmp47;});}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl == 0){
({struct Cyc_Hashtable_Table*_tmpBA=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);t=_tmpBA;});
({struct Cyc_Core_Opt*_tmpBB=({struct Cyc_Core_Opt*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->v=t;_tmp49;});Cyc_PP_str_hashtbl=_tmpBB;});}else{
# 208
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{
# 210
struct _handler_cons _tmp4A;_push_handler(& _tmp4A);{int _tmp4C=0;if(setjmp(_tmp4A.handler))_tmp4C=1;if(!_tmp4C){
{struct Cyc_PP_Doc*_tmp4E=({struct Cyc_Hashtable_Table*_tmpBC=t;((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(_tmpBC,({struct _dyneither_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=s;_tmp4D;}));});_npop_handler(0U);return _tmp4E;};_pop_handler();}else{void*_tmp4B=(void*)_exn_thrown;void*_tmp4F=_tmp4B;void*_tmp51;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp4F)->tag == Cyc_Core_Not_found){_LL1: _LL2: {
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
({struct Cyc_Hashtable_Table*_tmpBE=t;struct _dyneither_ptr*_tmpBD=({struct _dyneither_ptr*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=s;_tmp50;});((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(_tmpBE,_tmpBD,d);});
return d;}}else{_LL3: _tmp51=_tmp4F;_LL4:(int)_rethrow(_tmp51);}_LL0:;}};};}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*s){return Cyc_PP_text(*s);}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _dyneither_ptr s,int slen){
return({struct Cyc_PP_Doc*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->mwo=slen;_tmp52->mw=Cyc_PP_infinity;({struct Cyc_Fn_Function*_tmpBF=
# 228
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({struct _tuple2*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->f1=slen;_tmp53->f2=s;_tmp53;}));_tmp52->f=_tmpBF;});_tmp52;});}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2 == 0){
({struct Cyc_Hashtable_Table*_tmpC0=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);t=_tmpC0;});
({struct Cyc_Core_Opt*_tmpC1=({struct Cyc_Core_Opt*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->v=t;_tmp54;});Cyc_PP_str_hashtbl2=_tmpC1;});}else{
# 239
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{
# 241
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){
{struct Cyc_PP_Doc*_tmp59=({struct Cyc_Hashtable_Table*_tmpC2=t;((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(_tmpC2,({struct _dyneither_ptr*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=s;_tmp58;}));});_npop_handler(0U);return _tmp59;};_pop_handler();}else{void*_tmp56=(void*)_exn_thrown;void*_tmp5A=_tmp56;void*_tmp5C;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp5A)->tag == Cyc_Core_Not_found){_LL1: _LL2: {
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
({struct Cyc_Hashtable_Table*_tmpC4=t;struct _dyneither_ptr*_tmpC3=({struct _dyneither_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=s;_tmp5B;});((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(_tmpC4,_tmpC3,d);});
return d;}}else{_LL3: _tmp5C=_tmp5A;_LL4:(int)_rethrow(_tmp5C);}_LL0:;}};};}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc(){
if(Cyc_PP_nil_doc_opt == 0)
({struct Cyc_Core_Opt*_tmpC6=({struct Cyc_Core_Opt*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct Cyc_PP_Doc*_tmpC5=Cyc_PP_text(({const char*_tmp5E="";_tag_dyneither(_tmp5E,sizeof(char),1U);}));_tmp5D->v=_tmpC5;});_tmp5D;});Cyc_PP_nil_doc_opt=_tmpC6;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0)
({struct Cyc_Core_Opt*_tmpC8=({struct Cyc_Core_Opt*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct Cyc_PP_Doc*_tmpC7=Cyc_PP_text(({const char*_tmp60="";_tag_dyneither(_tmp60,sizeof(char),1U);}));_tmp5F->v=_tmpC7;});_tmp5F;});Cyc_PP_blank_doc_opt=_tmpC8;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple3{int f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple4{int f1;int f2;int f3;struct _dyneither_ptr f4;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
struct _tuple3*_tmp61=clo;int _tmp6B;struct _dyneither_ptr _tmp6A;struct _dyneither_ptr _tmp69;_LL1: _tmp6B=_tmp61->f1;_tmp6A=_tmp61->f2;_tmp69=_tmp61->f3;_LL2:;
return({struct Cyc_PP_Out*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->newcc=st->cc + _tmp6B;_tmp62->newcl=st->cl;({void*_tmpCB=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp66=_cycalloc(sizeof(*_tmp66));({struct Cyc_PP_Single_PP_Alist_struct _tmpCA=({struct Cyc_PP_Single_PP_Alist_struct _tmp67;_tmp67.tag=1U;({struct _dyneither_ptr*_tmpC9=({struct _dyneither_ptr*_tmp68=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp68[0]=(struct _dyneither_ptr)_tmp6A;_tmp68;});_tmp67.f1=_tmpC9;});_tmp67;});_tmp66[0]=_tmpCA;});_tmp66;});_tmp62->ppout=_tmpCB;});({void*_tmpCE=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp63=_cycalloc(sizeof(*_tmp63));({struct Cyc_PP_Single_PP_Alist_struct _tmpCD=({struct Cyc_PP_Single_PP_Alist_struct _tmp64;_tmp64.tag=1U;({struct _tuple4*_tmpCC=({struct _tuple4*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=st->cl;_tmp65->f2=st->cc;_tmp65->f3=_tmp6B;_tmp65->f4=_tmp69;_tmp65;});_tmp64.f1=_tmpCC;});_tmp64;});_tmp63[0]=_tmpCD;});_tmp63;});_tmp62->links=_tmpCE;});_tmp62;});}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)shrt);
return({struct Cyc_PP_Doc*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->mwo=slen;_tmp6C->mw=Cyc_PP_infinity;({struct Cyc_Fn_Function*_tmpCF=
# 281
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,({struct _tuple3*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->f1=slen;_tmp6D->f2=shrt;_tmp6D->f3=full;_tmp6D;}));_tmp6C->f=_tmpCF;});_tmp6C;});}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
return({struct Cyc_PP_Out*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->newcc=st->ci;_tmp6E->newcl=st->cl + 1;({void*_tmpD3=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_PP_Single_PP_Alist_struct _tmpD2=({struct Cyc_PP_Single_PP_Alist_struct _tmp70;_tmp70.tag=1U;({struct _dyneither_ptr*_tmpD1=({struct _dyneither_ptr*_tmp71=_cycalloc(sizeof(struct _dyneither_ptr)* 1);({typeof((struct _dyneither_ptr)
# 288
Cyc_PP_nlblanks(st->ci))_tmpD0=(struct _dyneither_ptr)Cyc_PP_nlblanks(st->ci);_tmp71[0]=_tmpD0;});_tmp71;});_tmp70.f1=_tmpD1;});_tmp70;});_tmp6F[0]=_tmpD2;});_tmp6F;});_tmp6E->ppout=_tmpD3;});_tmp6E->links=(void*)& Cyc_PP_Empty_link;_tmp6E;});}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc(){
if(Cyc_PP_line_doc_opt == 0)
({struct Cyc_Core_Opt*_tmpD6=({struct Cyc_Core_Opt*_tmp72=_cycalloc(sizeof(*_tmp72));({struct Cyc_PP_Doc*_tmpD5=({struct Cyc_PP_Doc*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->mwo=0;_tmp73->mw=0;({struct Cyc_Fn_Function*_tmpD4=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(Cyc_PP_line_f);_tmp73->f=_tmpD4;});_tmp73;});_tmp72->v=_tmpD5;});_tmp72;});Cyc_PP_line_doc_opt=_tmpD6;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple5{int f1;struct Cyc_PP_Doc*f2;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
struct _tuple5*_tmp74=clo;int _tmp77;struct Cyc_PP_Doc*_tmp76;_LL1: _tmp77=_tmp74->f1;_tmp76=_tmp74->f2;_LL2:;{
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp75=_cycalloc_atomic(sizeof(*_tmp75));_tmp75->ci=st->ci + _tmp77;_tmp75->cc=st->cc;_tmp75->cl=st->cl;_tmp75->pw=st->pw;_tmp75->epw=st->epw;_tmp75;});
# 305
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp76->f,st2);};}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
return({struct Cyc_PP_Doc*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->mwo=d->mwo;_tmp78->mw=d->mw;({struct Cyc_Fn_Function*_tmpD7=
# 310
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,({struct _tuple5*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=k;_tmp79->f2=d;_tmp79;}));_tmp78->f=_tmpD7;});_tmp78;});}
# 315
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 320
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f1;struct Cyc_PP_Doc*f2;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp7A=clo;struct Cyc_PP_Doc*_tmp7F;struct Cyc_PP_Doc*_tmp7E;_LL1: _tmp7F=_tmp7A->f1;_tmp7E=_tmp7A->f2;_LL2:;{
int epw2=Cyc_PP_max(st->pw - _tmp7E->mw,st->epw - _tmp7F->mwo);
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp7D=_cycalloc_atomic(sizeof(*_tmp7D));_tmp7D->ci=st->ci;_tmp7D->cc=st->cc;_tmp7D->cl=st->cl;_tmp7D->pw=st->pw;_tmp7D->epw=epw2;_tmp7D;});
struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7F->f,st2);
struct Cyc_PP_Ppstate*st3=({struct Cyc_PP_Ppstate*_tmp7C=_cycalloc_atomic(sizeof(*_tmp7C));_tmp7C->ci=st->ci;_tmp7C->cc=o1->newcc;_tmp7C->cl=o1->newcl;_tmp7C->pw=st->pw;_tmp7C->epw=epw2;_tmp7C;});
struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7E->f,st3);
struct Cyc_PP_Out*o3=({struct Cyc_PP_Out*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->newcc=o2->newcc;_tmp7B->newcl=o2->newcl;({void*_tmpD8=
# 334
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->ppout,o2->ppout);_tmp7B->ppout=_tmpD8;});({void*_tmpD9=
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->links,o2->links);_tmp7B->links=_tmpD9;});_tmp7B;});
return o3;};}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp80=_cycalloc(sizeof(*_tmp80));({int _tmpDA=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo);_tmp80->mwo=_tmpDA;});({int _tmpDB=
Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_tmp80->mw=_tmpDB;});({struct Cyc_Fn_Function*_tmpDC=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,({struct _tuple6*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=d1;_tmp81->f2=d2;_tmp81;}));_tmp80->f=_tmpDC;});_tmp80;});}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr l){
struct Cyc_PP_Doc*_tmp82=Cyc_PP_nil_doc();
{int i=(int)(_get_dyneither_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;-- i){
({struct Cyc_PP_Doc*_tmpDD=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_dyneither_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),_tmp82);_tmp82=_tmpDD;});}}
# 348
return _tmp82;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*_tmp83=ds0;for(0;_tmp83 != 0;_tmp83=_tmp83->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_tmp83->hd;
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
({struct Cyc_PP_Ppstate*_tmpDE=({struct Cyc_PP_Ppstate*_tmp84=_cycalloc_atomic(sizeof(*_tmp84));_tmp84->ci=st->ci;_tmp84->cc=o->newcc;_tmp84->cl=o->newcl;_tmp84->pw=st->pw;_tmp84->epw=st->epw - d->mwo;_tmp84;});st=_tmpDE;});
({struct Cyc_List_List*_tmpDF=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=o;_tmp85->tl=os;_tmp85;});os=_tmpDF;});}}{
# 361
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
({void*_tmpE0=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);s=_tmpE0;});
({void*_tmpE1=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);links=_tmpE1;});}
# 369
return({struct Cyc_PP_Out*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->newcc=newcc;_tmp86->newcl=newcl;_tmp86->ppout=s;_tmp86->links=links;_tmp86;});};}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 375
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 381
{struct Cyc_List_List*_tmp87=doclist;for(0;_tmp87 != 0;_tmp87=_tmp87->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp87->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp87->hd)->mw);
mw=mw2;
mwo=mwo2;}}
# 387
return({struct Cyc_PP_Doc*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->mwo=mw;_tmp88->mw=mwo;({struct Cyc_Fn_Function*_tmpE2=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(Cyc_PP_long_cats_f,orig);_tmp88->f=_tmpE2;});_tmp88;});};}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist == 0)return Cyc_PP_nil_doc();else{
if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 394
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return({struct Cyc_PP_Doc*_tmpE3=(struct Cyc_PP_Doc*)doclist->hd;Cyc_PP_concat(_tmpE3,Cyc_PP_cats(doclist->tl));});}}}}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _dyneither_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _dyneither_ptr docs=*docs_ptr;
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
({struct Cyc_PP_Ppstate*_tmpE4=({struct Cyc_PP_Ppstate*_tmp89=_cycalloc_atomic(sizeof(*_tmp89));_tmp89->ci=st->ci;_tmp89->cc=o->newcc;_tmp89->cl=o->newcl;_tmp89->pw=st->pw;_tmp89->epw=st->epw - d->mwo;_tmp89;});st=_tmpE4;});
({struct Cyc_List_List*_tmpE5=({struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->hd=o;_tmp8A->tl=os;_tmp8A;});os=_tmpE5;});}}{
# 408
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
({void*_tmpE6=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);s=_tmpE6;});
({void*_tmpE7=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);links=_tmpE7;});}
# 416
return({struct Cyc_PP_Out*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->newcc=newcc;_tmp8B->newcl=newcl;_tmp8B->ppout=s;_tmp8B->links=links;_tmp8B;});};}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs){
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));({struct Cyc_Core_Failure_exn_struct _tmpE9=({struct Cyc_Core_Failure_exn_struct _tmp8D;_tmp8D.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmpE8=({const char*_tmp8E="cats_arr -- size zero array";_tag_dyneither(_tmp8E,sizeof(char),28U);});_tmp8D.f1=_tmpE8;});_tmp8D;});_tmp8C[0]=_tmpE9;});_tmp8C;}));{
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (((struct Cyc_PP_Doc**)docs.curr)[i])->mw);
mw=mw2;
mwo=mwo2;}}
# 432
return({struct Cyc_PP_Doc*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->mwo=mw;_tmp8F->mw=mwo;({struct Cyc_Fn_Function*_tmpEA=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _dyneither_ptr*,struct Cyc_PP_Ppstate*),struct _dyneither_ptr*x))Cyc_Fn_make_fn)(Cyc_PP_cats_arr_f,({struct _dyneither_ptr*_tmp90=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp90[0]=(struct _dyneither_ptr)docs;_tmp90;}));_tmp8F->f=_tmpEA;});_tmp8F;});};}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp91=clo;struct Cyc_PP_Doc*_tmp93;struct Cyc_PP_Doc*_tmp92;_LL1: _tmp93=_tmp91->f1;_tmp92=_tmp91->f2;_LL2:;{
int dfits=st->cc + _tmp93->mwo <= st->epw  || st->cc + _tmp93->mw <= st->pw;
# 439
if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp93->f,st);else{
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp92->f,st);}};}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp94=_cycalloc(sizeof(*_tmp94));({int _tmpEB=Cyc_PP_min(d->mwo,d2->mwo);_tmp94->mwo=_tmpEB;});({int _tmpEC=
Cyc_PP_min(d->mw,d2->mw);_tmp94->mw=_tmpEC;});({struct Cyc_Fn_Function*_tmpED=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,({struct _tuple6*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->f1=d;_tmp95->f2=d2;_tmp95;}));_tmp94->f=_tmpED;});_tmp94;});}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc(){
return({struct Cyc_PP_Doc*_tmpEE=Cyc_PP_nil_doc();Cyc_PP_doc_union(_tmpEE,Cyc_PP_line_doc());});}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp96=_cycalloc_atomic(sizeof(*_tmp96));_tmp96->ci=st->cc;_tmp96->cc=st->cc;_tmp96->cl=st->cl;_tmp96->pw=st->pw;_tmp96->epw=st->epw;_tmp96;});
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st2);}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*d2=({struct Cyc_PP_Doc*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->mwo=d->mwo;_tmp97->mw=d->mw;({struct Cyc_Fn_Function*_tmpEF=
# 461
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d);_tmp97->f=_tmpEF;});_tmp97;});
return d2;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_tmp98[4U];({struct Cyc_PP_Doc*_tmpF0=Cyc_PP_ppseq_f(pp,sep,l->tl);_tmp98[3U]=_tmpF0;});({struct Cyc_PP_Doc*_tmpF1=Cyc_PP_oline_doc();_tmp98[2U]=_tmpF1;});({struct Cyc_PP_Doc*_tmpF2=Cyc_PP_text(sep);_tmp98[1U]=_tmpF2;});({struct Cyc_PP_Doc*_tmpF3=pp(l->hd);_tmp98[0U]=_tmpF3;});Cyc_PP_cat(_tag_dyneither(_tmp98,sizeof(struct Cyc_PP_Doc*),4U));});}}}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
# 481
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp99=l;
while(((struct Cyc_List_List*)_check_null(_tmp99))->tl != 0){
struct Cyc_List_List*_tmp9A=_tmp99->tl;
({struct Cyc_List_List*_tmpF5=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->hd=sep2;({struct Cyc_List_List*_tmpF4=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=oline;_tmp9C->tl=_tmp9A;_tmp9C;});_tmp9B->tl=_tmpF4;});_tmp9B;});_tmp99->tl=_tmpF5;});
_tmp99=_tmp9A;}
# 489
return Cyc_PP_cats(l);}}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_tmp9D[4U];({struct Cyc_PP_Doc*_tmpF6=Cyc_PP_ppseql_f(pp,sep,l->tl);_tmp9D[3U]=_tmpF6;});({struct Cyc_PP_Doc*_tmpF7=Cyc_PP_line_doc();_tmp9D[2U]=_tmpF7;});({struct Cyc_PP_Doc*_tmpF8=Cyc_PP_text(sep);_tmp9D[1U]=_tmpF8;});({struct Cyc_PP_Doc*_tmpF9=pp(l->hd);_tmp9D[0U]=_tmpF9;});Cyc_PP_cat(_tag_dyneither(_tmp9D,sizeof(struct Cyc_PP_Doc*),4U));});}}}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
return({struct Cyc_PP_Doc*_tmp9E[4U];({struct Cyc_PP_Doc*_tmpFA=Cyc_PP_seql_f(sep,l->tl);_tmp9E[3U]=_tmpFA;});({struct Cyc_PP_Doc*_tmpFB=Cyc_PP_line_doc();_tmp9E[2U]=_tmpFB;});({struct Cyc_PP_Doc*_tmpFC=Cyc_PP_text(sep);_tmp9E[1U]=_tmpFC;});_tmp9E[0U]=(struct Cyc_PP_Doc*)l->hd;Cyc_PP_cat(_tag_dyneither(_tmp9E,sizeof(struct Cyc_PP_Doc*),4U));});}}}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp9F[3U];({struct Cyc_PP_Doc*_tmpFD=
# 524
Cyc_PP_text(stop);_tmp9F[2U]=_tmpFD;});({struct Cyc_PP_Doc*_tmpFE=
# 523
Cyc_PP_seq(sep,ss);_tmp9F[1U]=_tmpFE;});({struct Cyc_PP_Doc*_tmpFF=
# 522
Cyc_PP_text(start);_tmp9F[0U]=_tmpFF;});Cyc_PP_cat(_tag_dyneither(_tmp9F,sizeof(struct Cyc_PP_Doc*),3U));});}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
if(ss == 0)return Cyc_PP_nil_doc();else{
return({struct Cyc_PP_Doc*_tmpA0[3U];({struct Cyc_PP_Doc*_tmp100=
# 532
Cyc_PP_text(stop);_tmpA0[2U]=_tmp100;});({struct Cyc_PP_Doc*_tmp101=
# 531
Cyc_PP_seq(sep,ss);_tmpA0[1U]=_tmp101;});({struct Cyc_PP_Doc*_tmp102=
# 530
Cyc_PP_text(start);_tmpA0[0U]=_tmp102;});Cyc_PP_cat(_tag_dyneither(_tmpA0,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpA1[3U];({struct Cyc_PP_Doc*_tmp103=
# 538
Cyc_PP_text(stop);_tmpA1[2U]=_tmp103;});({struct Cyc_PP_Doc*_tmp104=
# 537
Cyc_PP_seql(sep,ss);_tmpA1[1U]=_tmp104;});({struct Cyc_PP_Doc*_tmp105=
# 536
Cyc_PP_text(start);_tmpA1[0U]=_tmp105;});Cyc_PP_cat(_tag_dyneither(_tmpA1,sizeof(struct Cyc_PP_Doc*),3U));});}

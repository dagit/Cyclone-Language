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
# 52
extern struct Cyc___cycFILE*Cyc_stdin;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 142 "cycboot.h"
int Cyc_getc(struct Cyc___cycFILE*);
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
# 222 "cycboot.h"
int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 247
int Cyc_vsscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 302
int isupper(int);
# 314
double atof(const char*);
long strtol(char*,char**,int);
# 317
unsigned long strtoul(char*,char**,int);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 80 "scanf.cyc"
typedef long long Cyc__IO_ssize_t;
# 126 "scanf.cyc"
static struct _dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt);
# 139
static short*Cyc_va_arg_short_ptr(void*a){
void*_tmp0=a;unsigned short*_tmp5;short*_tmp4;switch(*((int*)_tmp0)){case 0U: _LL1: _tmp4=((struct Cyc_ShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;_LL2:
 return _tmp4;case 1U: _LL3: _tmp5=((struct Cyc_UShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;_LL4:
 return(short*)_tmp5;default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Core_Invalid_argument_exn_struct _tmp3F=({struct Cyc_Core_Invalid_argument_exn_struct _tmp2;_tmp2.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp3E=({const char*_tmp3="scan expects short pointer";_tag_dyneither(_tmp3,sizeof(char),27U);});_tmp2.f1=_tmp3E;});_tmp2;});_tmp1[0]=_tmp3F;});_tmp1;}));}_LL0:;}
# 147
static int*Cyc_va_arg_int_ptr(void*a){
void*_tmp6=a;unsigned int*_tmpB;int*_tmpA;switch(*((int*)_tmp6)){case 2U: _LL8: _tmpA=((struct Cyc_IntPtr_sa_ScanfArg_struct*)_tmp6)->f1;_LL9:
 return _tmpA;case 3U: _LLA: _tmpB=((struct Cyc_UIntPtr_sa_ScanfArg_struct*)_tmp6)->f1;_LLB:
 return(int*)_tmpB;default: _LLC: _LLD:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));({struct Cyc_Core_Invalid_argument_exn_struct _tmp41=({struct Cyc_Core_Invalid_argument_exn_struct _tmp8;_tmp8.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp40=({const char*_tmp9="scan expects int pointer";_tag_dyneither(_tmp9,sizeof(char),25U);});_tmp8.f1=_tmp40;});_tmp8;});_tmp7[0]=_tmp41;});_tmp7;}));}_LL7:;}
# 155
static struct _dyneither_ptr Cyc_va_arg_string_ptr(void*a){
void*_tmpC=a;struct _dyneither_ptr _tmp11;struct _dyneither_ptr _tmp10;switch(*((int*)_tmpC)){case 4U: _LLF: _tmp10=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmpC)->f1;_LL10:
 return _dyneither_ptr_decrease_size(_tmp10,sizeof(char),1U);case 7U: _LL11: _tmp11=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmpC)->f1;_LL12:
 return _tmp11;default: _LL13: _LL14:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpD=_cycalloc(sizeof(*_tmpD));({struct Cyc_Core_Invalid_argument_exn_struct _tmp43=({struct Cyc_Core_Invalid_argument_exn_struct _tmpE;_tmpE.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp42=({const char*_tmpF="scan expects char pointer";_tag_dyneither(_tmpF,sizeof(char),26U);});_tmpE.f1=_tmp42;});_tmpE;});_tmpD[0]=_tmp43;});_tmpD;}));}_LLE:;}
# 163
static double*Cyc_va_arg_double_ptr(void*a){
void*_tmp12=a;double*_tmp16;if(((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmp12)->tag == 5U){_LL16: _tmp16=((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmp12)->f1;_LL17:
 return _tmp16;}else{_LL18: _LL19:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp13=_cycalloc(sizeof(*_tmp13));({struct Cyc_Core_Invalid_argument_exn_struct _tmp45=({struct Cyc_Core_Invalid_argument_exn_struct _tmp14;_tmp14.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp44=({const char*_tmp15="scan expects double pointer";_tag_dyneither(_tmp15,sizeof(char),28U);});_tmp14.f1=_tmp44;});_tmp14;});_tmp13[0]=_tmp45;});_tmp13;}));}_LL15:;}
# 170
static float*Cyc_va_arg_float_ptr(void*a){
void*_tmp17=a;float*_tmp1B;if(((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp17)->tag == 6U){_LL1B: _tmp1B=((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp17)->f1;_LL1C:
 return _tmp1B;}else{_LL1D: _LL1E:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Core_Invalid_argument_exn_struct _tmp47=({struct Cyc_Core_Invalid_argument_exn_struct _tmp19;_tmp19.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp46=({const char*_tmp1A="scan expects float pointer";_tag_dyneither(_tmp1A,sizeof(char),27U);});_tmp19.f1=_tmp46;});_tmp19;});_tmp18[0]=_tmp47;});_tmp18;}));}_LL1A:;}
# 177
static struct _dyneither_ptr Cyc_va_arg_char_ptr(void*a){
void*_tmp1C=a;struct _dyneither_ptr _tmp21;struct _dyneither_ptr _tmp20;switch(*((int*)_tmp1C)){case 7U: _LL20: _tmp20=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmp1C)->f1;_LL21:
 return _tmp20;case 4U: _LL22: _tmp21=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmp1C)->f1;_LL23:
 return _dyneither_ptr_decrease_size(_tmp21,sizeof(char),1U);default: _LL24: _LL25:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct Cyc_Core_Invalid_argument_exn_struct _tmp49=({struct Cyc_Core_Invalid_argument_exn_struct _tmp1E;_tmp1E.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp48=({const char*_tmp1F="scan expects char pointer";_tag_dyneither(_tmp1F,sizeof(char),26U);});_tmp1E.f1=_tmp48;});_tmp1E;});_tmp1D[0]=_tmp49;});_tmp1D;}));}_LL1F:;}
# 188
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp){
# 198
struct _dyneither_ptr fmt=fmt0;
int c;
long long width;
struct _dyneither_ptr p=_tag_dyneither(0U,0U,0U);
int n;
int flags=0;
# 205
struct _dyneither_ptr p0=_tag_dyneither(0U,0U,0U);
int nassigned;
int nread;
# 209
int base=0;
int use_strtoul=0;
# 213
char ccltab[256U];
# 215
char buf[351U];
# 217
{unsigned int _tmp3C=350U;unsigned int i;for(i=0;i < _tmp3C;i ++){buf[i]=(char)'0';}buf[_tmp3C]=(char)0U;}{int seen_eof=0;
# 220
static short basefix[17U]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
# 223
nassigned=0;
nread=0;
for(0;1;0){
c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == 0)
goto done;
if( isspace(c)){
for(0;1;0){
({int _tmp4A=_IO_getc(fp);c=_tmp4A;});
if(c == - 1){
++ seen_eof;
break;}
# 236
if(! isspace(c)){
_IO_ungetc(c,fp);
break;}
# 240
++ nread;}
# 242
continue;}
# 244
if(c != '%')
goto literal;
width=(long long)0;
flags=0;
# 252
again: c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
{int _tmp22=c;switch(_tmp22){case 37U: _LL27: _LL28:
# 255
 literal:
({int _tmp4B=_IO_getc(fp);n=_tmp4B;});
if(n == - 1)
goto eof_failure;
if(n != c){
_IO_ungetc(n,fp);
goto match_failure;}
# 263
++ nread;
continue;case 42U: _LL29: _LL2A:
# 267
 if(flags)goto control_failure;
flags=8;
goto again;case 108U: _LL2B: _LL2C:
# 271
 if(flags & ~(8 | 64))goto control_failure;
flags |=1;
goto again;case 76U: _LL2D: _LL2E:
# 275
 if(flags & ~(8 | 64))goto control_failure;
flags |=2;
goto again;case 104U: _LL2F: _LL30:
# 279
 if(flags & ~(8 | 64))goto control_failure;
flags |=4;
goto again;case 48U: _LL31: _LL32:
# 283
 goto _LL34;case 49U: _LL33: _LL34: goto _LL36;case 50U: _LL35: _LL36: goto _LL38;case 51U: _LL37: _LL38: goto _LL3A;case 52U: _LL39: _LL3A: goto _LL3C;case 53U: _LL3B: _LL3C:
 goto _LL3E;case 54U: _LL3D: _LL3E: goto _LL40;case 55U: _LL3F: _LL40: goto _LL42;case 56U: _LL41: _LL42: goto _LL44;case 57U: _LL43: _LL44:
 if(flags & ~(8 | 64))goto control_failure;
flags |=64;
width=(width * 10 + c)- '0';
goto again;case 68U: _LL45: _LL46:
# 298 "scanf.cyc"
 flags |=1;
goto _LL48;case 100U: _LL47: _LL48:
# 301
 c=3;
use_strtoul=0;
base=10;
goto _LL26;case 105U: _LL49: _LL4A:
# 307
 c=3;
use_strtoul=0;
base=0;
goto _LL26;case 79U: _LL4B: _LL4C:
# 313
 flags |=1;
goto _LL4E;case 111U: _LL4D: _LL4E:
# 316
 c=3;
use_strtoul=1;
base=8;
goto _LL26;case 117U: _LL4F: _LL50:
# 322
 c=3;
use_strtoul=1;
base=10;
goto _LL26;case 88U: _LL51: _LL52:
# 327
 goto _LL54;case 120U: _LL53: _LL54:
 flags |=256;
c=3;
use_strtoul=1;
base=16;
goto _LL26;case 69U: _LL55: _LL56:
# 334
 goto _LL58;case 70U: _LL57: _LL58: goto _LL5A;case 101U: _LL59: _LL5A: goto _LL5C;case 102U: _LL5B: _LL5C: goto _LL5E;case 103U: _LL5D: _LL5E:
 c=4;
goto _LL26;case 115U: _LL5F: _LL60:
# 339
 c=2;
goto _LL26;case 91U: _LL61: _LL62:
# 343
({struct _dyneither_ptr _tmp4C=Cyc___sccl(ccltab,fmt);fmt=_tmp4C;});
flags |=32;
c=1;
goto _LL26;case 99U: _LL63: _LL64:
# 349
 flags |=32;
c=0;
goto _LL26;case 112U: _LL65: _LL66:
# 354
 flags |=16 | 256;
c=3;
use_strtoul=1;
base=16;
goto _LL26;case 110U: _LL67: _LL68:
# 361
 if(flags & 8)
continue;
if(flags & 4)
({short _tmp4D=(short)nread;*Cyc_va_arg_short_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp4D;});else{
if(flags & 1)
({long _tmp4E=nread;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp4E;});else{
# 368
({int _tmp4F=nread;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp4F;});}}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 0U: _LL69: _LL6A:
# 376
 nassigned=-1;
goto done;default: _LL6B: _LL6C:
# 380
 if( isupper(c))
flags |=1;
c=3;
use_strtoul=0;
base=10;
goto _LL26;}_LL26:;}
# 391
if(_IO_peekc(fp)== - 1)
goto eof_failure;
# 398
if((flags & 32)== 0){
({int _tmp50=_IO_peekc(fp);n=_tmp50;});
while( isspace(n)){
({int _tmp51=_IO_getc(fp);n=_tmp51;});
++ nread;
({int _tmp52=_IO_peekc(fp);n=_tmp52;});
if(n == - 1)
goto eof_failure;}}{
# 415
int _tmp23=c;switch(_tmp23){case 0U: _LL6E: _LL6F:
# 421
 if(width == 0)
width=(long long)1;
if(flags & 8){
long long sum=(long long)0;
for(0;width > 0;0){
({int _tmp53=_IO_getc(fp);n=_tmp53;});
if(n == - 1  && width != 0)
goto eof_failure;else{
if(n == - 1){
++ seen_eof;
break;}}
# 433
++ sum;
-- width;}
# 436
nread +=sum;}else{
# 438
long long sum=(long long)0;
struct _dyneither_ptr _tmp24=Cyc_va_arg_char_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
for(0;width > 0;0){
({int _tmp54=_IO_getc(fp);n=_tmp54;});
if(n == - 1  && width != 0)
goto eof_failure;else{
if(n == - 1){
++ seen_eof;
break;}}
# 448
*((char*)_check_dyneither_subscript(_tmp24,sizeof(char),0U))=(char)n;
_dyneither_ptr_inplace_plus(& _tmp24,sizeof(char),1);
++ sum;
-- width;}
# 453
nread +=sum;
++ nassigned;}
# 456
goto _LL6D;case 1U: _LL70: _LL71:
# 460
 if(width == 0)
width=(long long)4294967295U;
# 463
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while((int)ccltab[_check_known_subscript_notnull(256U,(int)((char)c))]){
++ n;
_IO_getc(fp);
if(-- width == 0)
break;
if(({int _tmp55=_IO_peekc(fp);c=_tmp55;})== - 1){
if(n == 0)
goto eof_failure;
++ seen_eof;
break;}}
# 478
if(n == 0)
goto match_failure;};}else{
# 481
struct _dyneither_ptr p4=(struct _dyneither_ptr)Cyc_va_arg_string_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _dyneither_ptr p5=p4;
int c=_IO_peekc(fp);
while((int)ccltab[_check_known_subscript_notnull(256U,(int)((char)c))]){
if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(p5,sizeof(char),0U))=(char)c;
_dyneither_ptr_inplace_plus(& p5,sizeof(char),1);
_IO_getc(fp);
if(-- width == 0)
break;
if(({int _tmp56=_IO_peekc(fp);c=_tmp56;})== - 1){
if((char*)p5.curr == (char*)p0.curr)
goto eof_failure;
++ seen_eof;
break;}}
# 498
n=(p5.curr - p4.curr)/ sizeof(char);
if(n == 0)
goto match_failure;
if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(p5,sizeof(char),0U))='\000';
++ nassigned;};}
# 505
nread +=n;
goto _LL6D;case 2U: _LL72: _LL73:
# 510
 if(width == 0)
width=(long long)4294967295U;
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while(! isspace((int)((unsigned char)c))){
++ n;
_IO_getc(fp);
if(-- width == 0)
break;
if(({int _tmp57=_IO_peekc(fp);c=_tmp57;})== - 1){
++ seen_eof;
break;}}
# 525
nread +=n;};}else{
# 527
struct _dyneither_ptr _tmp25=Cyc_va_arg_string_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _dyneither_ptr _tmp26=_tmp25;
int c=_IO_peekc(fp);
while(! isspace((int)((unsigned char)c))){
({int _tmp58=_IO_getc(fp);c=_tmp58;});
if(_get_dyneither_size(_tmp26,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0U))=(char)c;
_dyneither_ptr_inplace_plus(& _tmp26,sizeof(char),1);
if(-- width == 0)
break;
if(({int _tmp59=_IO_peekc(fp);c=_tmp59;})== - 1){
++ seen_eof;
break;}}
# 542
if(_get_dyneither_size(_tmp26,sizeof(char))== 0)goto eof_failure;
*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0U))='\000';
nread +=(_tmp26.curr - _tmp25.curr)/ sizeof(char);
++ nassigned;};}
# 547
continue;case 3U: _LL74: _LL75:
# 551
 if(width == 0  || width > sizeof(buf)- 1)
width=(long long)(sizeof(buf)- 1);
flags |=(64 | 128)| 512;
for(({struct _dyneither_ptr _tmp5A=({char*_tmp27=buf;_tag_dyneither(_tmp27,sizeof(char),_get_zero_arr_size_char((void*)_tmp27,351U));});p=_tmp5A;});width != 0;-- width){
({int _tmp5B=(int)((unsigned char)_IO_peekc(fp));c=_tmp5B;});
# 560
{int _tmp28=c;switch(_tmp28){case 48U: _LL7B: _LL7C:
# 575 "scanf.cyc"
 if(base == 0){
base=8;
flags |=256;}
# 579
if(flags & 512)
flags &=~((64 | 512)| 128);else{
# 582
flags &=~((64 | 256)| 128);}
goto ok;case 49U: _LL7D: _LL7E:
# 586
 goto _LL80;case 50U: _LL7F: _LL80: goto _LL82;case 51U: _LL81: _LL82: goto _LL84;case 52U: _LL83: _LL84: goto _LL86;case 53U: _LL85: _LL86:
 goto _LL88;case 54U: _LL87: _LL88: goto _LL8A;case 55U: _LL89: _LL8A:
 base=(int)basefix[_check_known_subscript_notnull(17U,base)];
flags &=~((64 | 256)| 128);
goto ok;case 56U: _LL8B: _LL8C:
# 593
 goto _LL8E;case 57U: _LL8D: _LL8E:
 base=(int)basefix[_check_known_subscript_notnull(17U,base)];
if(base <= 8)
goto _LL7A;
flags &=~((64 | 256)| 128);
goto ok;case 65U: _LL8F: _LL90:
# 601
 goto _LL92;case 66U: _LL91: _LL92: goto _LL94;case 67U: _LL93: _LL94: goto _LL96;case 68U: _LL95: _LL96: goto _LL98;case 69U: _LL97: _LL98:
 goto _LL9A;case 70U: _LL99: _LL9A: goto _LL9C;case 97U: _LL9B: _LL9C: goto _LL9E;case 98U: _LL9D: _LL9E: goto _LLA0;case 99U: _LL9F: _LLA0:
 goto _LLA2;case 100U: _LLA1: _LLA2: goto _LLA4;case 101U: _LLA3: _LLA4: goto _LLA6;case 102U: _LLA5: _LLA6:
# 605
 if(base <= 10)
goto _LL7A;
flags &=~((64 | 256)| 128);
goto ok;case 43U: _LLA7: _LLA8:
# 611
 goto _LLAA;case 45U: _LLA9: _LLAA:
 if(flags & 64){
flags &=~ 64;
goto ok;}
# 616
goto _LL7A;case 120U: _LLAB: _LLAC:
# 619
 goto _LLAE;case 88U: _LLAD: _LLAE:
 if(flags & 256  && ({char*_tmp5C=(char*)(_dyneither_ptr_plus(p,sizeof(char),- 1)).curr;_tmp5C == buf;})){
base=16;
flags &=~ 256;
goto ok;}
# 625
goto _LL7A;default: _LLAF: _LLB0:
# 627
 goto _LL7A;}_LL7A:;}
# 635
break;
ok:
# 640
({struct _dyneither_ptr _tmp29=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp2A=*((char*)_check_dyneither_subscript(_tmp29,sizeof(char),0U));char _tmp2B=(char)c;if(_get_dyneither_size(_tmp29,sizeof(char))== 1U  && (_tmp2A == '\000'  && _tmp2B != '\000'))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});
_IO_getc(fp);
if(_IO_peekc(fp)== - 1){
++ seen_eof;
break;}}
# 653
if(flags & 128){
if((char*)p.curr > buf){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U)),fp);}
# 658
goto match_failure;}
# 660
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),-1));
if(c == 'x'  || c == 'X'){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc(c,fp);}
# 665
if((flags & 8)== 0){
unsigned long res;
# 668
({struct _dyneither_ptr _tmp2C=p;char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0U));char _tmp2E='\000';if(_get_dyneither_size(_tmp2C,sizeof(char))== 1U  && (_tmp2D == '\000'  && _tmp2E != '\000'))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});
if(use_strtoul)
({unsigned long _tmp5D= strtoul(buf,0,base);res=_tmp5D;});else{
# 672
({unsigned long _tmp5E=(unsigned long) strtol(buf,0,base);res=_tmp5E;});}
if(flags & 16)
({int _tmp5F=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp5F;});else{
if(flags & 4)
({short _tmp60=(short)res;*Cyc_va_arg_short_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp60;});else{
if(flags & 1)
({int _tmp61=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp61;});else{
# 680
({int _tmp62=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp62;});}}}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 684
({int _tmp64=({unsigned char*_tmp63=p.curr;_tmp63 - ({char*_tmp2F=buf;_tag_dyneither(_tmp2F,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F,351U));}).curr;})/ sizeof(char);nread +=_tmp64;});
goto _LL6D;case 4U: _LL76: _LL77:
# 689
 if(width == 0  || width > sizeof(buf)- 1)
width=(long long)(sizeof(buf)- 1);
flags |=((64 | 128)| 256)| 512;
for(({struct _dyneither_ptr _tmp65=({char*_tmp30=buf;_tag_dyneither(_tmp30,sizeof(char),_get_zero_arr_size_char((void*)_tmp30,351U));});p=_tmp65;});width != 0;-- width){
({int _tmp66=_IO_peekc(fp);c=_tmp66;});
# 698
{int _tmp31=c;switch(_tmp31){case 48U: _LLB2: _LLB3:
# 700
 goto _LLB5;case 49U: _LLB4: _LLB5: goto _LLB7;case 50U: _LLB6: _LLB7: goto _LLB9;case 51U: _LLB8: _LLB9: goto _LLBB;case 52U: _LLBA: _LLBB:
 goto _LLBD;case 53U: _LLBC: _LLBD: goto _LLBF;case 54U: _LLBE: _LLBF: goto _LLC1;case 55U: _LLC0: _LLC1: goto _LLC3;case 56U: _LLC2: _LLC3:
 goto _LLC5;case 57U: _LLC4: _LLC5:
 flags &=~(64 | 128);
goto fok;case 43U: _LLC6: _LLC7:
# 706
 goto _LLC9;case 45U: _LLC8: _LLC9:
 if(flags & 64){
flags &=~ 64;
goto fok;}
# 711
goto _LLB1;case 46U: _LLCA: _LLCB:
# 713
 if(flags & 256){
flags &=~(64 | 256);
goto fok;}
# 717
goto _LLB1;case 101U: _LLCC: _LLCD:
 goto _LLCF;case 69U: _LLCE: _LLCF:
# 720
 if((flags & (128 | 512))== 512){
flags=(flags & ~(512 | 256)| 64)| 128;
# 724
goto fok;}
# 726
goto _LLB1;default: _LLD0: _LLD1:
# 728
 goto _LLB1;}_LLB1:;}
# 730
break;
fok:
({struct _dyneither_ptr _tmp32=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34=(char)c;if(_get_dyneither_size(_tmp32,sizeof(char))== 1U  && (_tmp33 == '\000'  && _tmp34 != '\000'))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});
_IO_getc(fp);
if(_IO_peekc(fp)== - 1){
++ seen_eof;
break;}}
# 744
if(flags & 128){
if(flags & 512){
# 747
while((char*)p.curr > buf){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U)),fp);}
# 751
goto match_failure;}
# 754
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U));
if(c != 'e'  && c != 'E'){
_IO_ungetc(c,fp);
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),0U));}
# 761
_IO_ungetc(c,fp);}
# 763
if((flags & 8)== 0){
double res;
({struct _dyneither_ptr _tmp35=p;char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0U));char _tmp37='\000';if(_get_dyneither_size(_tmp35,sizeof(char))== 1U  && (_tmp36 == '\000'  && _tmp37 != '\000'))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});
({double _tmp67= atof((const char*)buf);res=_tmp67;});
if(flags & 1)
({double _tmp68=res;*Cyc_va_arg_double_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp68;});else{
# 770
({float _tmp69=(float)res;*Cyc_va_arg_float_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U)))=_tmp69;});}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 774
({int _tmp6B=({unsigned char*_tmp6A=p.curr;_tmp6A - ({char*_tmp38=buf;_tag_dyneither(_tmp38,sizeof(char),_get_zero_arr_size_char((void*)_tmp38,351U));}).curr;})/ sizeof(char);nread +=_tmp6B;});
goto _LL6D;default: _LL78: _LL79:
# 777
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp39=_cycalloc(sizeof(*_tmp39));({struct Cyc_Core_Impossible_exn_struct _tmp6D=({struct Cyc_Core_Impossible_exn_struct _tmp3A;_tmp3A.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6C=({const char*_tmp3B="scanf3";_tag_dyneither(_tmp3B,sizeof(char),7U);});_tmp3A.f1=_tmp6C;});_tmp3A;});_tmp39[0]=_tmp6D;});_tmp39;}));}_LL6D:;};}
# 781
eof_failure:
 ++ seen_eof;
input_failure:
 if(nassigned == 0)
nassigned=-1;
control_failure:
 match_failure:
 if((unsigned int)errp)
*errp |=2;
done:
 if((unsigned int)errp  && seen_eof)
*errp |=1;
return nassigned;};}
# 802
static struct _dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt){
# 807
int c;int n;int v;
# 810
c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == '^'){
v=1;
c=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}else{
# 815
v=0;}
# 817
for(n=0;n < 256;++ n){
tab[n]=(char)v;}
if(c == 0)
return _dyneither_ptr_plus(fmt,sizeof(char),- 1);
# 829 "scanf.cyc"
v=1 - v;
for(0;1;0){
tab[c]=(char)v;
doswitch:
 n=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));{
int _tmp3D=n;switch(_tmp3D){case 0U: _LLD3: _LLD4:
# 837
 return _dyneither_ptr_plus(fmt,sizeof(char),- 1);case 45U: _LLD5: _LLD6:
# 858 "scanf.cyc"
 n=(int)*((const char*)_check_dyneither_subscript(fmt,sizeof(char),0U));
if(n == ']'  || n < c){
c=(int)'-';
goto _LLD2;}
# 863
_dyneither_ptr_inplace_plus(& fmt,sizeof(char),1);
do{
tab[_check_known_subscript_notnull(256U,++ c)]=(char)v;}while(c < n);
# 873
goto doswitch;
# 881
goto _LLD2;case 93U: _LLD7: _LLD8:
# 884
 return fmt;default: _LLD9: _LLDA:
# 887
 c=n;
goto _LLD2;}_LLD2:;};}}
# 896
static int Cyc_string_getc(struct _dyneither_ptr*sptr){
char c;
struct _dyneither_ptr s=*sptr;
if((({char*_tmp6E=(char*)s.curr;_tmp6E == (char*)(_tag_dyneither(0U,0U,0U)).curr;}) || _get_dyneither_size(s,sizeof(char))== 0) || (c=*((const char*)_check_dyneither_subscript(s,sizeof(char),0)))== '\000')return - 1;
({struct _dyneither_ptr _tmp6F=_dyneither_ptr_plus(s,sizeof(char),1);*sptr=_tmp6F;});
return(int)c;}
# 904
static int Cyc_string_ungetc(int ignore,struct _dyneither_ptr*sptr){
({struct _dyneither_ptr _tmp70=_dyneither_ptr_plus(*sptr,sizeof(char),- 1);*sptr=_tmp70;});
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _dyneither_ptr*sptr){
char c;
struct _dyneither_ptr s=*sptr;
if((({char*_tmp71=(char*)s.curr;_tmp71 == (char*)(_tag_dyneither(0U,0U,0U)).curr;}) || _get_dyneither_size(s,sizeof(char))== 0) || (c=*((const char*)_check_dyneither_subscript(s,sizeof(char),0)))== '\000')return - 1;
return(int)c;}
# 917
int Cyc_vsscanf(struct _dyneither_ptr src1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 920
struct _dyneither_ptr src=(struct _dyneither_ptr)src1;
int err=0;
return((int(*)(int(*_IO_getc)(struct _dyneither_ptr*),int(*_IO_ungetc)(int,struct _dyneither_ptr*),int(*_IO_peekc)(struct _dyneither_ptr*),struct _dyneither_ptr*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,& err);}
# 926
int Cyc_sscanf(struct _dyneither_ptr src,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 929
return Cyc_vsscanf(src,fmt,ap);}
# 933
int Cyc_peekc(struct Cyc___cycFILE*stream){
int c=Cyc_fgetc(stream);
Cyc_ungetc(c,stream);
return c;}
# 939
int Cyc_vfscanf(struct Cyc___cycFILE*stream,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 942
int err=0;
return((int(*)(int(*_IO_getc)(struct Cyc___cycFILE*),int(*_IO_ungetc)(int,struct Cyc___cycFILE*),int(*_IO_peekc)(struct Cyc___cycFILE*),struct Cyc___cycFILE*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_getc,Cyc_ungetc,Cyc_peekc,stream,fmt,ap,& err);}
# 946
int Cyc_fscanf(struct Cyc___cycFILE*stream,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 949
return Cyc_vfscanf(stream,fmt,ap);}
# 952
int Cyc_scanf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 955
return Cyc_vfscanf(Cyc_stdin,fmt,ap);}
